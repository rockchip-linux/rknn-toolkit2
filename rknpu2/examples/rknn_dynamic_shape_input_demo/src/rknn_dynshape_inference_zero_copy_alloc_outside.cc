/****************************************************************************
 *
 *    Copyright (c) 2017 - 2023 by Rockchip Corp.  All rights reserved.
 *
 *    The material in this file is confidential and contains trade secrets
 *    of Rockchip Corporation. This is proprietary information owned by
 *    Rockchip Corporation. No part of this work may be disclosed,
 *    reproduced, copied, transmitted, or used in any way for any purpose,
 *    without the express written permission of Rockchip Corporation.
 *
 *****************************************************************************/

/*-------------------------------------------
                Includes
-------------------------------------------*/
#include "rk_mpi_mmz.h"
#include "rknn_api.h"

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>

#define NPY_SUPPORT 1
#if NPY_SUPPORT
#include "cnpy/cnpy.h"
using namespace cnpy;
#endif

#define TIME_BEGIN(name)             \
  struct timeval tv##name;           \
  gettimeofday(&tv##name, NULL);     \
  long val##name = tv##name.tv_usec; \
  long min##name = tv##name.tv_sec;

#define TIME_END(name)                                  \
  gettimeofday(&tv##name, NULL);                        \
  val##name = tv##name.tv_usec - val##name;             \
  val##name += 1000000 * (tv##name.tv_sec - min##name); \
  printf("[%s]exectime is %ld us, %ld ms\n", #name, val##name, val##name / 1000);

static int mb_flags = RK_MMZ_ALLOC_TYPE_IOMMU | RK_MMZ_ALLOC_CACHEABLE;

#define SIZE_ALIGN(size, align) (((size) + ((align)-1)) & (~((align)-1)))
#define SIZE_ALIGN_128(size) SIZE_ALIGN(size, 128)

/*-------------------------------------------
                  Functions
-------------------------------------------*/
static inline int64_t getCurrentTimeUs()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

static int rknn_GetTopN(float *pfProb, float *pfMaxProb, uint32_t *pMaxClass, uint32_t outputCount, uint32_t topNum)
{
  uint32_t i, j;
  uint32_t top_count = outputCount > topNum ? topNum : outputCount;

  for (i = 0; i < topNum; ++i)
  {
    pfMaxProb[i] = -FLT_MAX;
    pMaxClass[i] = -1;
  }

  for (j = 0; j < top_count; j++)
  {
    for (i = 0; i < outputCount; i++)
    {
      if ((i == *(pMaxClass + 0)) || (i == *(pMaxClass + 1)) || (i == *(pMaxClass + 2)) || (i == *(pMaxClass + 3)) ||
          (i == *(pMaxClass + 4)))
      {
        continue;
      }

      if (pfProb[i] > *(pfMaxProb + j))
      {
        *(pfMaxProb + j) = pfProb[i];
        *(pMaxClass + j) = i;
      }
    }
  }

  return 1;
}

static void dump_tensor_attr(rknn_tensor_attr *attr)
{
  printf("  index=%d, name=%s, n_dims=%d, dims=[%d, %d, %d, %d], n_elems=%d, size=%d, fmt=%s, type=%s, qnt_type=%s, "
         "zp=%d, scale=%f\n",
         attr->index, attr->name, attr->n_dims, attr->dims[0], attr->dims[1], attr->dims[2], attr->dims[3],
         attr->n_elems, attr->size, get_format_string(attr->fmt), get_type_string(attr->type),
         get_qnt_type_string(attr->qnt_type), attr->zp, attr->scale);
}

static void dump_input_dynamic_range(rknn_input_range *dyn_range)
{
  std::string range_str = "";
  for (int n = 0; n < dyn_range->shape_number; ++n)
  {
    range_str += n == 0 ? "[" : ",[";
    range_str += dyn_range->n_dims < 1 ? "" : std::to_string(dyn_range->dyn_range[n][0]);
    for (int i = 1; i < dyn_range->n_dims; ++i)
    {
      range_str += ", " + std::to_string(dyn_range->dyn_range[n][i]);
    }
    range_str += "]";
  }

  printf("  index=%d, name=%s, shape_number=%d, range=[%s], fmt = %s\n", dyn_range->index, dyn_range->name,
         dyn_range->shape_number, range_str.c_str(), get_format_string(dyn_range->fmt));
}

static unsigned char *load_image(const char *image_path, rknn_tensor_attr *input_attr)
{
  int req_height = 0;
  int req_width = 0;
  int req_channel = 0;

  switch (input_attr->fmt)
  {
  case RKNN_TENSOR_NHWC:
    req_height = input_attr->dims[1];
    req_width = input_attr->dims[2];
    req_channel = input_attr->dims[3];
    break;
  case RKNN_TENSOR_NCHW:
    req_height = input_attr->dims[2];
    req_width = input_attr->dims[3];
    req_channel = input_attr->dims[1];
    break;
  default:
    printf("meet unsupported layout\n");
    return NULL;
  }

  int height = 0;
  int width = 0;
  int channel = 0;

  unsigned char *image_data = stbi_load(image_path, &width, &height, &channel, req_channel);
  if (image_data == NULL)
  {
    printf("load image failed!\n");
    return NULL;
  }

  if (width != req_width || height != req_height)
  {
    unsigned char *image_resized = (unsigned char *)STBI_MALLOC(req_width * req_height * req_channel);
    if (!image_resized)
    {
      printf("malloc image failed!\n");
      STBI_FREE(image_data);
      return NULL;
    }
    if (stbir_resize_uint8(image_data, width, height, 0, image_resized, req_width, req_height, 0, channel) != 1)
    {
      printf("resize image failed!\n");
      STBI_FREE(image_data);
      return NULL;
    }
    STBI_FREE(image_data);
    image_data = image_resized;
  }

  return image_data;
}

#if NPY_SUPPORT
static unsigned char *load_npy(const char *input_path, rknn_tensor_attr *input_attr, int *input_type, int *input_size,
                               int *type_bytes)
{
  printf("Loading %s\n", input_path);

  NpyArray npy_data = npy_load(input_path);

  *type_bytes = npy_data.word_size;
  std::string typeName = npy_data.typeName;

  printf("npy data type:%s\n", typeName.c_str());

  if (typeName == "int8")
  {
    *input_type = RKNN_TENSOR_INT8;
  }
  else if (typeName == "uint8")
  {
    *input_type = RKNN_TENSOR_UINT8;
  }
  else if (typeName == "float16")
  {
    *input_type = RKNN_TENSOR_FLOAT16;
  }
  else if (typeName == "float32")
  {
    *input_type = RKNN_TENSOR_FLOAT32;
  }
  else if (typeName == "8")
  {
    *input_type = RKNN_TENSOR_BOOL;
  }
  else if (typeName == "int64")
  {
    *input_type = RKNN_TENSOR_INT64;
  }

  // npy shape = NHWC
  std::vector<int> npy_shape;
  for (size_t i = 0; i < npy_data.shape.size(); ++i)
  {
    npy_shape.emplace_back(npy_data.shape[i]);
  }

  int height = npy_shape.size() > 1 ? npy_shape[1] : 1;
  int width = npy_shape.size() > 2 ? npy_shape[2] : 1;
  int channel = npy_shape.size() > 3 ? npy_shape[3] : 1;

  switch (input_attr->fmt)
  {
  case RKNN_TENSOR_NHWC:
    input_attr->dims[0] = npy_shape[0];
    input_attr->dims[1] = height;
    input_attr->dims[2] = width;
    input_attr->dims[3] = channel;
    break;
  case RKNN_TENSOR_UNDEFINED:
    for (int idx = 0; idx < input_attr->n_dims; ++idx)
    {
      input_attr->dims[idx] = npy_shape[idx];
    }
    break;
  default:
    fprintf(stderr, "load_npy error, unsupport model input layout: %s\n", get_format_string(input_attr->fmt));
    break;
  }

  unsigned char *data = (unsigned char *)malloc(npy_data.num_bytes());
  if (!data)
  {
    return NULL;
  }

  // TODO: copy
  memcpy(data, npy_data.data<unsigned char>(), npy_data.num_bytes());

  *input_size = npy_data.num_bytes();

  return data;
}

static void save_npy(const char *output_path, float *output_data, rknn_tensor_attr *output_attr)
{
  std::vector<size_t> output_shape;

  for (uint32_t i = 0; i < output_attr->n_dims; ++i)
  {
    output_shape.push_back(output_attr->dims[i]);
  }

  npy_save<float>(output_path, output_data, output_shape);
}
#endif

static std::vector<std::string> split(const std::string &str, const std::string &pattern)
{
  std::vector<std::string> res;
  if (str == "")
    return res;
  std::string strs = str + pattern;
  size_t pos = strs.find(pattern);
  while (pos != strs.npos)
  {
    std::string temp = strs.substr(0, pos);
    res.push_back(temp);
    strs = strs.substr(pos + 1, strs.size());
    pos = strs.find(pattern);
  }
  return res;
}

uint32_t get_file_size(char *file_name)
{
  FILE *fid = fopen(file_name, "rb");
  if (fid == NULL)
  {
    printf("open file error\n");
    fclose(fid);
    return -1;
  }

  fseek(fid, 0, SEEK_END);
  uint32_t size = ftell(fid);

  fclose(fid);
  return size;
}

int read_bin_file(char *file_name, void *out_ptr, unsigned int size)
{
  FILE *fid = fopen(file_name, "rb");
  if (fid == NULL)
  {
    printf("open file error\n");
    fclose(fid);
    return -1;
  }
  fread(out_ptr, 1, size, fid);

  fclose(fid);
  return 0;
}

/*-------------------------------------------
                  Main Functions
-------------------------------------------*/
int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("Usage:%s model_path input_path [loop_count] [core_mask] [output_dir]\n", argv[0]);
    return -1;
  }

  char *model_path = argv[1];
  char *input_paths = argv[2];
  std::vector<std::string> input_paths_split = split(input_paths, "#");

  int loop_count = 1;
  if (argc > 3)
  {
    loop_count = atoi(argv[3]);
  }

  uint32_t core_mask = 1;
  if (argc > 4)
  {
    // core_mask = atoi(argv[4]);
    core_mask = strtoul(argv[4], NULL, 10);
  }

  char *output_dir = NULL;
  if (argc > 5)
  {
    output_dir = argv[5];
  }

  rknn_context ctx = 0;

  // Allocate model memory in outside
  MB_BLK model_mb;
  uint32_t model_size = get_file_size(model_path);
  int ret = RK_MPI_MMZ_Alloc(&model_mb, model_size, mb_flags);
  if (ret < 0)
  {
    printf("RK_MPI_MMZ_Alloc failed, ret: %d\n", ret);
    return ret;
  }
  void *model_virt = RK_MPI_MMZ_Handle2VirAddr(model_mb);
  if (model_virt == NULL)
  {
    printf("RK_MPI_MMZ_Handle2VirAddr failed!\n");
    return -1;
  }
  ret = read_bin_file(model_path, model_virt, model_size);
  if (ret < 0)
  {
    printf("read_bin_file failed, ret: %d\n", ret);
    return ret;
  }

  TIME_BEGIN(dummpy_rknn_init);
  ret = rknn_init(&ctx, model_virt, model_size, RKNN_FLAG_COLLECT_MODEL_INFO_ONLY | RKNN_FLAG_MEM_ALLOC_OUTSIDE, NULL);
  TIME_END(dummpy_rknn_init);
  if (ret < 0)
  {
    printf("rknn_init with RKNN_FLAG_COLLECT_MODEL_INFO_ONLY fail! ret=%d\n", ret);
    return -1;
  }

  // [dummpy_rknn_init] Get weight and internal mem size
  rknn_mem_size mem_size;
  ret = rknn_query(ctx, RKNN_QUERY_MEM_SIZE, &mem_size, sizeof(mem_size));
  if (ret != RKNN_SUCC)
  {
    printf("rknn_query fail! ret=%d\n", ret);
    return -1;
  }
  printf("[dummpy init] total weight size: %d, total internal size: %d\n", mem_size.total_weight_size,
         mem_size.total_internal_size);

  // Load RKNN Model
  TIME_BEGIN(rknn_init);
  ret = rknn_init(&ctx, model_virt, model_size, RKNN_FLAG_MEM_ALLOC_OUTSIDE, NULL);
  TIME_END(rknn_init);
  if (ret < 0)
  {
    printf("rknn_init fail! ret=%d\n", ret);
    return -1;
  }

  // Get sdk and driver version
  rknn_sdk_version sdk_ver;
  ret = rknn_query(ctx, RKNN_QUERY_SDK_VERSION, &sdk_ver, sizeof(sdk_ver));
  if (ret != RKNN_SUCC)
  {
    printf("rknn_query fail! ret=%d\n", ret);
    return -1;
  }
  printf("rknn_api/rknnrt version: %s, driver version: %s\n", sdk_ver.api_version, sdk_ver.drv_version);

  // Get weight and internal mem size
  ret = rknn_query(ctx, RKNN_QUERY_MEM_SIZE, &mem_size, sizeof(mem_size));
  if (ret != RKNN_SUCC)
  {
    printf("rknn_query fail! ret=%d\n", ret);
    return -1;
  }
  printf("total weight size: %u, total internal size: %u\n", mem_size.total_weight_size, mem_size.total_internal_size);

  // Get Model Input Output Info
  rknn_input_output_num io_num;
  ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
  if (ret != RKNN_SUCC)
  {
    printf("rknn_query fail! ret=%d\n", ret);
    return -1;
  }
  printf("model input num: %d, output num: %d\n", io_num.n_input, io_num.n_output);

  printf("default input tensors:\n");
  rknn_tensor_attr input_attrs[io_num.n_input];
  memset(input_attrs, 0, io_num.n_input * sizeof(rknn_tensor_attr));
  for (uint32_t i = 0; i < io_num.n_input; i++)
  {
    input_attrs[i].index = i;
    // query info
    ret = rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr));
    if (ret < 0)
    {
      printf("rknn_init error! ret=%d\n", ret);
      return -1;
    }
    dump_tensor_attr(&input_attrs[i]);
  }

  printf("default output tensors:\n");
  rknn_tensor_attr output_attrs[io_num.n_output];
  memset(output_attrs, 0, io_num.n_output * sizeof(rknn_tensor_attr));
  for (uint32_t i = 0; i < io_num.n_output; i++)
  {
    output_attrs[i].index = i;
    // query info
    ret = rknn_query(ctx, RKNN_QUERY_OUTPUT_ATTR, &(output_attrs[i]), sizeof(rknn_tensor_attr));
    if (ret != RKNN_SUCC)
    {
      printf("rknn_query fail! ret=%d\n", ret);
      return -1;
    }
    dump_tensor_attr(&output_attrs[i]);
  }

  // Get custom string
  rknn_custom_string custom_string;
  ret = rknn_query(ctx, RKNN_QUERY_CUSTOM_STRING, &custom_string, sizeof(custom_string));
  if (ret != RKNN_SUCC)
  {
    printf("rknn_query fail! ret=%d\n", ret);
    return -1;
  }
  printf("custom string: %s\n", custom_string.string);

  printf("dynamic inputs shape range:\n");
  rknn_input_range dyn_range[io_num.n_input];
  memset(dyn_range, 0, io_num.n_input * sizeof(rknn_input_range));
  for (uint32_t i = 0; i < io_num.n_input; i++)
  {
    dyn_range[i].index = i;
    ret = rknn_query(ctx, RKNN_QUERY_INPUT_DYNAMIC_RANGE, &dyn_range[i], sizeof(rknn_input_range));
    if (ret != RKNN_SUCC)
    {
      printf("rknn_query fail! ret=%d\n", ret);
      return -1;
    }
    dump_input_dynamic_range(&dyn_range[i]);
  }

  unsigned char *input_data[io_num.n_input];
  int input_type[io_num.n_input];
  int input_layout[io_num.n_input];
  int input_size[io_num.n_input];
  int type_bytes[io_num.n_input];
  for (int i = 0; i < io_num.n_input; i++)
  {
    input_data[i] = NULL;
    input_type[i] = RKNN_TENSOR_UINT8;
    input_layout[i] = RKNN_TENSOR_NHWC;
    input_size[i] = input_attrs[i].size;
    type_bytes[i] = 1;
  }

  // Load input
  if (io_num.n_input != input_paths_split.size())
  {
    return -1;
  }
  for (int i = 0; i < io_num.n_input; i++)
  {
    if (strstr(input_paths_split[i].c_str(), ".npy"))
    {
// Load npy
#if NPY_SUPPORT
      input_data[i] =
          load_npy(input_paths_split[i].c_str(), &input_attrs[i], &input_type[i], &input_size[i], &type_bytes[i]);
#else
      return -1;
#endif
    }
    else
    {
      // Load image
      for (int i = 0; i < io_num.n_input; i++)
      {
        input_data[i] = load_image(input_paths_split[i].c_str(), &input_attrs[i]);
      }
    }

    if (!input_data[i])
    {
      return -1;
    }
  }

  ret = rknn_set_input_shapes(ctx, io_num.n_input, input_attrs);
  if (ret < 0)
  {
    fprintf(stderr, "rknn_set_input_shapes error! ret=%d\n", ret);
    return -1;
  }

  // Allocate weight memory in outside
  MB_BLK weight_mb;
  rknn_tensor_mem *weight_mem;
  ret = RK_MPI_MMZ_Alloc(&weight_mb, SIZE_ALIGN_128(mem_size.total_weight_size), mb_flags);
  if (ret < 0)
  {
    printf("RK_MPI_MMZ_Alloc failed, ret: %d\n", ret);
    return ret;
  }
  void *weight_virt = RK_MPI_MMZ_Handle2VirAddr(weight_mb);
  if (weight_virt == NULL)
  {
    printf("RK_MPI_MMZ_Handle2VirAddr failed!\n");
    return -1;
  }
  int weight_fd = RK_MPI_MMZ_Handle2Fd(weight_mb);
  if (weight_fd < 0)
  {
    printf("RK_MPI_MMZ_Handle2Fd failed!\n");
    return -1;
  }
  weight_mem = rknn_create_mem_from_fd(ctx, weight_fd, weight_virt, mem_size.total_weight_size, 0);
  ret = rknn_set_weight_mem(ctx, weight_mem);
  if (ret < 0)
  {
    printf("rknn_set_weight_mem fail! ret=%d\n", ret);
    return -1;
  }
  printf("weight mb info: virt = %p, fd = %d, size: %d\n", weight_virt, weight_fd, mem_size.total_weight_size);

  // Allocate internal memory in outside
  MB_BLK internal_mb;
  rknn_tensor_mem *internal_mem;
  ret = RK_MPI_MMZ_Alloc(&internal_mb, SIZE_ALIGN_128(mem_size.total_internal_size), mb_flags);
  if (ret < 0)
  {
    printf("RK_MPI_MMZ_Alloc failed, ret: %d\n", ret);
    return ret;
  }
  void *internal_virt = RK_MPI_MMZ_Handle2VirAddr(internal_mb);
  if (internal_virt == NULL)
  {
    printf("RK_MPI_MMZ_Handle2VirAddr failed!\n");
    return -1;
  }
  int internal_fd = RK_MPI_MMZ_Handle2Fd(internal_mb);
  if (internal_fd < 0)
  {
    printf("RK_MPI_MMZ_Handle2Fd failed!\n");
    return -1;
  }
  internal_mem = rknn_create_mem_from_fd(ctx, internal_fd, internal_virt, mem_size.total_internal_size, 0);
  ret = rknn_set_internal_mem(ctx, internal_mem);
  if (ret < 0)
  {
    printf("rknn_set_internal_mem fail! ret=%d\n", ret);
    return -1;
  }
  printf("internal mb info: virt = %p, fd = %d, size: %d\n", internal_virt, internal_fd, mem_size.total_internal_size);

  printf("current input tensors:\n");
  rknn_tensor_attr cur_input_attrs[io_num.n_input];
  memset(cur_input_attrs, 0, io_num.n_input * sizeof(rknn_tensor_attr));
  for (uint32_t i = 0; i < io_num.n_input; i++)
  {
    cur_input_attrs[i].index = i;
    // query info
    ret = rknn_query(ctx, RKNN_QUERY_CURRENT_INPUT_ATTR, &(cur_input_attrs[i]), sizeof(rknn_tensor_attr));
    if (ret < 0)
    {
      printf("rknn_init error! ret=%d\n", ret);
      return -1;
    }
    dump_tensor_attr(&cur_input_attrs[i]);
  }

  printf("current output tensors:\n");
  rknn_tensor_attr cur_output_attrs[io_num.n_output];
  memset(cur_output_attrs, 0, io_num.n_output * sizeof(rknn_tensor_attr));
  for (uint32_t i = 0; i < io_num.n_output; i++)
  {
    cur_output_attrs[i].index = i;
    // query info
    ret = rknn_query(ctx, RKNN_QUERY_CURRENT_OUTPUT_ATTR, &(cur_output_attrs[i]), sizeof(rknn_tensor_attr));
    if (ret != RKNN_SUCC)
    {
      printf("rknn_query fail! ret=%d\n", ret);
      return -1;
    }
    dump_tensor_attr(&cur_output_attrs[i]);
  }

  // Allocate inputs memory in outside
  MB_BLK input_mbs[io_num.n_input];
  void *input_virts[io_num.n_input];
  int input_fds[io_num.n_input];
  for (uint32_t i = 0; i < io_num.n_input; ++i)
  {
    int input_size = cur_input_attrs[i].size_with_stride;

    ret = RK_MPI_MMZ_Alloc(&input_mbs[i], SIZE_ALIGN_128(input_size), mb_flags);
    if (ret < 0)
    {
      printf("RK_MPI_MMZ_Alloc failed, ret: %d\n", ret);
      return ret;
    }
    input_virts[i] = RK_MPI_MMZ_Handle2VirAddr(input_mbs[i]);
    if (input_virts[i] == NULL)
    {
      printf("RK_MPI_MMZ_Handle2VirAddr failed!\n");
      return -1;
    }
    input_fds[i] = RK_MPI_MMZ_Handle2Fd(input_mbs[i]);
    if (input_fds[i] < 0)
    {
      printf("RK_MPI_MMZ_Handle2Fd failed!\n");
      return -1;
    }
    printf("input%d mb info: virt = %p, fd = %d, size = %d\n", i, input_virts[i], input_fds[i], input_size);
  }

  // Allocate outputs memory in outside
  MB_BLK output_mbs[io_num.n_output];
  void *output_virts[io_num.n_output];
  int output_fds[io_num.n_output];
  for (uint32_t i = 0; i < io_num.n_output; ++i)
  {
    // default output type is depend on model, this require float32 to compute top5
    cur_output_attrs[i].type = RKNN_TENSOR_FLOAT32;
    int output_size = cur_output_attrs[i].n_elems * sizeof(float);
    cur_output_attrs[i].size = output_size;
    ret = RK_MPI_MMZ_Alloc(&output_mbs[i], SIZE_ALIGN_128(output_size), mb_flags);
    if (ret < 0)
    {
      printf("RK_MPI_MMZ_Alloc failed, ret: %d\n", ret);
      return ret;
    }
    output_virts[i] = RK_MPI_MMZ_Handle2VirAddr(output_mbs[i]);
    if (output_virts[i] == NULL)
    {
      printf("RK_MPI_MMZ_Handle2VirAddr failed!\n");
      return -1;
    }
    output_fds[i] = RK_MPI_MMZ_Handle2Fd(output_mbs[i]);
    if (output_fds[i] < 0)
    {
      printf("RK_MPI_MMZ_Handle2Fd failed!\n");
      return -1;
    }
    printf("output%d mb info: virt = %p, fd = %d, size = %d\n", i, output_virts[i], output_fds[i], output_size);
  }

  // Create input tensor memory
  rknn_tensor_mem *input_mems[io_num.n_input];
  for (int i = 0; i < io_num.n_input; i++)
  {
    // default input type is int8 (normalize and quantize need compute in outside)
    // if set uint8, will fuse normalize and quantize to npu
    cur_input_attrs[i].type = RKNN_TENSOR_UINT8;
    // default fmt is NHWC, npu only support NHWC in zero copy mode
    cur_input_attrs[i].fmt = RKNN_TENSOR_NHWC;

    input_mems[i] = rknn_create_mem_from_fd(ctx, input_fds[i], input_virts[i], cur_input_attrs[i].size_with_stride, 0);

    // Copy input data to input tensor memory
    int width = cur_input_attrs[i].dims[2];
    int stride = cur_input_attrs[i].w_stride;

    if (width == stride)
    {
      memcpy(input_mems[i]->virt_addr, input_data[i], input_size[i]);
    }
    else
    {
      int height = cur_input_attrs[i].dims[1];
      int channel = cur_input_attrs[i].dims[3];
      // copy from src to dst with stride
      uint8_t *src_ptr = input_data[i];
      uint8_t *dst_ptr = (uint8_t *)input_mems[i]->virt_addr;
      // width-channel elements
      int src_wc_elems = width * channel;
      int dst_wc_elems = stride * channel;
      for (int b = 0; b < cur_input_attrs[i].dims[0]; b++)
      {
        for (int h = 0; h < height; ++h)
        {
          memcpy(dst_ptr, src_ptr, src_wc_elems);
          src_ptr += src_wc_elems;
          dst_ptr += dst_wc_elems;
        }
      }
    }
  }

  // Create output tensor memory
  rknn_tensor_mem *output_mems[io_num.n_output];
  for (uint32_t i = 0; i < io_num.n_output; ++i)
  {
    output_mems[i] = rknn_create_mem_from_fd(ctx, output_fds[i], output_virts[i], cur_output_attrs[i].size, 0);
  }

  // Set input tensor memory
  for (uint32_t i = 0; i < io_num.n_input; ++i)
  {
    ret = rknn_set_io_mem(ctx, input_mems[i], &cur_input_attrs[i]);
    if (ret < 0)
    {
      printf("rknn_set_io_mem fail! ret=%d\n", ret);
      return -1;
    }
  }

  // Set output tensor memory
  for (uint32_t i = 0; i < io_num.n_output; ++i)
  {
    // set output memory and attribute
    ret = rknn_set_io_mem(ctx, output_mems[i], &cur_output_attrs[i]);
    if (ret < 0)
    {
      printf("rknn_set_io_mem fail! ret=%d\n", ret);
      return -1;
    }
  }

  // weight flush cache
  ret = RK_MPI_MMZ_FlushCacheEnd(weight_mb, 0, SIZE_ALIGN_128(mem_size.total_weight_size), RK_MMZ_SYNC_RW);
  if (ret < 0)
  {
    printf("weight_mb FlushCacheEnd fail! ret=%d\n", ret);
    return -1;
  }

  // Run
  printf("Begin perf ...\n");
  for (int i = 0; i < loop_count; ++i)
  {
    ret = RK_MPI_MMZ_FlushCacheEnd(internal_mb, 0, SIZE_ALIGN_128(mem_size.total_internal_size), RK_MMZ_SYNC_RW);
    if (ret < 0)
    {
      printf("internal_mb FlushCacheEnd fail! ret=%d\n", ret);
      return -1;
    }

    for (uint32_t i = 0; i < io_num.n_input; ++i)
    {
      ret =
          RK_MPI_MMZ_FlushCacheEnd(input_mbs[i], 0, SIZE_ALIGN_128(cur_input_attrs[i].size_with_stride), RK_MMZ_SYNC_RW);
      if (ret < 0)
      {
        printf("input_mbs FlushCacheEnd fail! ret=%d\n", ret);
        return -1;
      }
    }

    for (uint32_t i = 0; i < io_num.n_output; ++i)
    {
      ret = RK_MPI_MMZ_FlushCacheEnd(output_mbs[i], 0, SIZE_ALIGN_128(cur_output_attrs[i].n_elems * sizeof(float)),
                                     RK_MMZ_SYNC_RW);
      if (ret < 0)
      {
        printf("output_mbs FlushCacheEnd fail! ret=%d\n", ret);
        return -1;
      }
    }

    int64_t start_us = getCurrentTimeUs();
    ret = rknn_run(ctx, NULL);
    int64_t elapse_us = getCurrentTimeUs() - start_us;
    if (ret < 0)
    {
      printf("rknn run error %d\n", ret);
      return -1;
    }
    printf("%4d: Elapse Time = %.2fms, FPS = %.2f\n", i, elapse_us / 1000.f, 1000.f * 1000.f / elapse_us);

    for (uint32_t i = 0; i < io_num.n_output; ++i)
    {
      ret = RK_MPI_MMZ_FlushCacheStart(output_mbs[i], 0, SIZE_ALIGN_128(cur_output_attrs[i].n_elems * sizeof(float)),
                                       RK_MMZ_SYNC_RW);
      if (ret < 0)
      {
        printf("output_mbs FlushCacheStart fail! ret=%d\n", ret);
        return -1;
      }
    }
  }

  // 处理输出结果
  for (uint32_t i = 0; i < io_num.n_output; i++)
  {
    float *output = (float *)output_mems[i]->virt_addr;
    int out_elems = cur_output_attrs[i].n_elems;
    std::vector<std::pair<float, int>> results;
    for (int i = 0; i < out_elems; i++)
    {
      results.emplace_back(output[i], i);
    }
    std::partial_sort(results.begin(), results.begin() + 5, results.end(), std::greater<std::pair<float, int>>());
    printf(" --- Top5 ---\n");
    for (int i = 0; i < 5; i++)
    {
      printf("%-3d: %.2f%%\n", results[i].second, results[i].first * 100);
    }
  }

  // save output
  for (uint32_t i = 0; i < io_num.n_output; i++)
  {
    char output_path[PATH_MAX];
    sprintf(output_path, "%s/rt_output%d.npy", output_dir ? output_dir : ".", i);
    rknn_tensor_attr cur_output_attrs_npy = cur_output_attrs[i];
    if (cur_output_attrs[i].fmt == RKNN_TENSOR_NHWC && cur_output_attrs[i].n_dims == 4)
    {
      std::vector<int> axis = {0, 2, 3, 1};
      for (int j = 0; j < axis.size(); j++)
      {
        cur_output_attrs_npy.dims[j] = cur_output_attrs[i].dims[axis[j]];
      }
    }
    save_npy(output_path, (float *)output_mems[i]->virt_addr, &cur_output_attrs_npy);
  }

  // free mb blk memory
  RK_MPI_MMZ_Free(model_mb);
  RK_MPI_MMZ_Free(weight_mb);
  RK_MPI_MMZ_Free(internal_mb);
  for (uint32_t i = 0; i < io_num.n_input; ++i)
  {
    RK_MPI_MMZ_Free(input_mbs[i]);
  }
  for (uint32_t i = 0; i < io_num.n_output; ++i)
  {
    RK_MPI_MMZ_Free(output_mbs[i]);
  }

  // Destroy rknn memory
  for (uint32_t i = 0; i < io_num.n_input; ++i)
  {
    rknn_destroy_mem(ctx, input_mems[i]);
    free(input_data[i]);
  }
  for (uint32_t i = 0; i < io_num.n_output; ++i)
  {
    rknn_destroy_mem(ctx, output_mems[i]);
  }
  rknn_destroy_mem(ctx, weight_mem);
  rknn_destroy_mem(ctx, internal_mem);

  // destroy
  rknn_destroy(ctx);

  return 0;
}
