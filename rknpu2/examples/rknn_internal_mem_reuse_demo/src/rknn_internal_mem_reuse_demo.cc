// Copyright (c) 2021 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


/*-------------------------------------------
                Includes
-------------------------------------------*/
#include "rknn_api.h"

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>

#define NPY_SUPPORT 0

#if NPY_SUPPORT
#  include "cnpy/cnpy.h"
#endif

/*-------------------------------------------
                  Functions
-------------------------------------------*/
static inline int64_t getCurrentTimeUs()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

static int rknn_GetTopN(float* pfProb, float* pfMaxProb, uint32_t* pMaxClass, uint32_t outputCount, uint32_t topNum)
{
  uint32_t i, j;
  uint32_t top_count = outputCount > topNum ? topNum : outputCount;

  for (i = 0; i < topNum; ++i) {
    pfMaxProb[i] = -FLT_MAX;
    pMaxClass[i] = -1;
  }

  for (j = 0; j < top_count; j++) {
    for (i = 0; i < outputCount; i++) {
      if ((i == *(pMaxClass + 0)) || (i == *(pMaxClass + 1)) || (i == *(pMaxClass + 2)) || (i == *(pMaxClass + 3)) ||
          (i == *(pMaxClass + 4))) {
        continue;
      }

      if (pfProb[i] > *(pfMaxProb + j)) {
        *(pfMaxProb + j) = pfProb[i];
        *(pMaxClass + j) = i;
      }
    }
  }

  return 1;
}


static void dump_tensor_attr(rknn_tensor_attr* attr)
{
  char dims[128] = {0};
  for (int i = 0; i < attr->n_dims; ++i) {
    int idx = strlen(dims);
    sprintf(&dims[idx], "%d%s", attr->dims[i], (i == attr->n_dims - 1) ? "" : ", ");
  }
  printf("  index=%d, name=%s, n_dims=%d, dims=[%s], n_elems=%d, size=%d, fmt=%s, type=%s, qnt_type=%s, "
         "zp=%d, scale=%f\n",
         attr->index, attr->name, attr->n_dims, dims, attr->n_elems, attr->size, get_format_string(attr->fmt),
         get_type_string(attr->type), get_qnt_type_string(attr->qnt_type), attr->zp, attr->scale);
}

static void* load_file(const char* file_path, size_t* file_size)
{
  FILE* fp = fopen(file_path, "r");
  if (fp == NULL) {
    printf("failed to open file: %s\n", file_path);
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  size_t size = (size_t)ftell(fp);
  fseek(fp, 0, SEEK_SET);

  void* file_data = malloc(size);
  if (file_data == NULL) {
    fclose(fp);
    printf("failed allocate file size: %zu\n", size);
    return NULL;
  }

  if (fread(file_data, 1, size, fp) != size) {
    fclose(fp);
    free(file_data);
    printf("failed to read file data!\n");
    return NULL;
  }

  fclose(fp);

  *file_size = size;

  return file_data;
}

static int load_bin(const char *filename,  void *data,  int max_size)
{

    FILE *fp;
    int ret = 0;

    fp = fopen(filename, "rb");
    if (NULL == fp)
    {
        printf("Open file %s failed.\n", filename);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);

    if (size != max_size) {
      printf("file size not match: %d vs %d!\n", size, max_size);
      fclose(fp);
      return -1;
    }

    ret = fseek(fp, 0, SEEK_SET);
    ret = fread(data, 1, size, fp);

    fclose(fp);

    return ret;
}


static unsigned char* load_image(const char* image_path, rknn_tensor_attr* input_attr)
{
  int req_height  = 0;
  int req_width   = 0;
  int req_channel = 0;

  switch (input_attr->fmt) {
  case RKNN_TENSOR_NHWC:
    req_height  = input_attr->dims[1];
    req_width   = input_attr->dims[2];
    req_channel = input_attr->dims[3];
    break;
  case RKNN_TENSOR_NCHW:
    req_height  = input_attr->dims[2];
    req_width   = input_attr->dims[3];
    req_channel = input_attr->dims[1];
    break;
  default:
    printf("meet unsupported layout\n");
    return NULL;
  }

  int height  = 0;
  int width   = 0;
  int channel = 0;

  unsigned char* image_data = stbi_load(image_path, &width, &height, &channel, req_channel);
  if (image_data == NULL) {
    printf("load image failed!\n");
    return NULL;
  }

  if (width != req_width || height != req_height) {
    unsigned char* image_resized = (unsigned char*)STBI_MALLOC(req_width * req_height * req_channel);
    if (!image_resized) {
      printf("malloc image failed!\n");
      STBI_FREE(image_data);
      return NULL;
    }
    if (stbir_resize_uint8(image_data, width, height, 0, image_resized, req_width, req_height, 0, channel) != 1) {
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
static unsigned char* load_npy(const char* input_path, rknn_tensor_attr* input_attr, int* input_type, int* input_size)
{
  int req_height  = 0;
  int req_width   = 0;
  int req_channel = 0;

  printf("Loading %s\n", input_path);

  switch (input_attr->fmt) {
  case RKNN_TENSOR_NHWC:
    req_height  = input_attr->dims[1];
    req_width   = input_attr->dims[2];
    req_channel = input_attr->dims[3];
    break;
  case RKNN_TENSOR_NCHW:
    req_height  = input_attr->dims[2];
    req_width   = input_attr->dims[3];
    req_channel = input_attr->dims[1];
    break;
  case RKNN_TENSOR_UNDEFINED:
    break;
  default:
    printf("meet unsupported layout\n");
    return NULL;
  }

  cnpy_array npy_data;

  bool writable = false;
  if (cnpy_open(input_path, writable, &npy_data) != CNPY_SUCCESS) {
    printf("Unable to load file %s\n", input_path);
    return NULL;
  }

  int        data_bytes = npy_data.raw_data_size - npy_data.data_begin;
  cnpy_dtype dtype      = npy_data.dtype;

  if (dtype == CNPY_I8) {
    *input_type = RKNN_TENSOR_INT8;
  } else if (dtype == CNPY_U8) {
    *input_type = RKNN_TENSOR_UINT8;
  } else if (dtype == CNPY_F4) {
    *input_type = RKNN_TENSOR_FLOAT32;
  }

  // npy shape = NHWC
  int npy_shape[4] = {1, 1, 1, 1};

  int start = npy_data.n_dim == 4 ? 0 : 1;
  for (size_t i = 0; i < npy_data.n_dim && i < 4; ++i) {
    npy_shape[start + i] = npy_data.dims[i];
  }

  int height  = npy_shape[1];
  int width   = npy_shape[2];
  int channel = npy_shape[3];

  if ((input_attr->fmt != RKNN_TENSOR_UNDEFINED) &&
      (width != req_width || height != req_height || channel != req_channel)) {
    printf("npy shape match failed!, (%d, %d, %d) != (%d, %d, %d)\n", height, width, channel, req_height, req_width,
           req_channel);
    return NULL;
  }

  unsigned char* data = (unsigned char*)malloc(data_bytes);
  if (!data) {
    return NULL;
  }

  // TODO: copy
  memcpy(data, npy_data.raw_data + npy_data.data_begin, data_bytes);

  *input_size = data_bytes;

  return data;
}

static int save_npy(const char* output_path, float* output_data, rknn_tensor_attr* output_attr)
{
  int size = 1;

  for (uint32_t i = 0; i < output_attr->n_dims; ++i) {
    size *= output_attr->dims[i];
  }

  cnpy_array      npy_data;
  cnpy_byte_order byte_order = CNPY_LE;      /* little endian */
  cnpy_dtype      dtype      = CNPY_F4;      /* float */
  cnpy_flat_order order      = CNPY_C_ORDER; /* Fortran (row major) order */

  if (cnpy_create(output_path, byte_order, dtype, order, output_attr->n_dims, (const size_t*)output_attr->dims,
                  &npy_data) != CNPY_SUCCESS) {
    cnpy_perror("Unable to create file: ");
    return -1;
  }

  memcpy(npy_data.raw_data + npy_data.data_begin, (uint8_t*)output_data, sizeof(float) * size);

  /* optional: */
  if (cnpy_close(&npy_data) != CNPY_SUCCESS) {
    cnpy_perror("Unable to close file: ");
    return -1;
  }
  return 0;
}
#endif

#define MAX_OUTPUT_NUM 4
#define TOTAL_RKNN_MODEL_NUM 2

/*-------------------------------------------
                  Main Functions
-------------------------------------------*/
int main(int argc, char* argv[])
{
  if (argc < 5) {
    printf("Usage:%s model_path_a input_path_a model_path_b input_path_b [loop_count] \n", argv[0]);
    return -1;
  }

  char* model_path_a = argv[1];
  char* input_path_a = argv[2];
  char* model_path_b = argv[3];
  char* input_path_b = argv[4];

  int loop_count = 1;
  if (argc > 5) {
    loop_count = atoi(argv[5]);
  }

  char *model_path[TOTAL_RKNN_MODEL_NUM];
  char *input_path[TOTAL_RKNN_MODEL_NUM];
  rknn_context ctx[TOTAL_RKNN_MODEL_NUM];
  rknn_mem_size mem_size[TOTAL_RKNN_MODEL_NUM];
  rknn_input_output_num io_num[TOTAL_RKNN_MODEL_NUM];
  rknn_tensor_mem* internal_mem[TOTAL_RKNN_MODEL_NUM];
  rknn_tensor_mem *weight_mems[TOTAL_RKNN_MODEL_NUM];
  rknn_tensor_attr input_attrs[TOTAL_RKNN_MODEL_NUM][1]; // this demo only support one input
  rknn_tensor_attr output_attrs[TOTAL_RKNN_MODEL_NUM][MAX_OUTPUT_NUM];
  rknn_tensor_mem* input_mems[TOTAL_RKNN_MODEL_NUM][1];  // this demo only support one input
  rknn_tensor_mem* output_mems[TOTAL_RKNN_MODEL_NUM][MAX_OUTPUT_NUM];
  rknn_tensor_mem* internal_mem_max = NULL;
  uint32_t max_internal_size = 0;
  unsigned char*     input_data   = NULL;
  rknn_tensor_type   input_type   = RKNN_TENSOR_UINT8;
  rknn_tensor_format input_layout = RKNN_TENSOR_NHWC;
  int ret = 0;

  memset(ctx, 0x00, sizeof(ctx));
  memset(internal_mem, 0x00, sizeof(internal_mem));
  memset(input_mems, 0x00, sizeof(input_mems));
  memset(output_mems, 0x00, sizeof(output_mems));

  model_path[0] = model_path_a;
  model_path[1] = model_path_b;
  input_path[0] = input_path_a;
  input_path[1] = input_path_b;

  for (int n=0; n<TOTAL_RKNN_MODEL_NUM; n++) {

    printf("\033[0;32mLoading %s ... \033[0;0m\n", model_path[n]);

    // Load RKNN Model
    // Init rknn from model path
    ret = rknn_init(&ctx[n], model_path[n], 0, RKNN_FLAG_MEM_ALLOC_OUTSIDE, NULL);
    // ret = rknn_init(&ctx[n], model_path[n], 0, 0, NULL);

    if (ret < 0) {
      printf("rknn_init fail! ret=%d\n", ret);
      return -1;
    }

    // Get sdk and driver version
    rknn_sdk_version sdk_ver;
    ret = rknn_query(ctx[n], RKNN_QUERY_SDK_VERSION, &sdk_ver, sizeof(sdk_ver));
    if (ret != RKNN_SUCC) {
      printf("rknn_query fail! ret=%d\n", ret);
      rknn_destroy(ctx[n]);
      goto out;
    }
    printf("rknn_api/rknnrt version: %s, driver version: %s\n", sdk_ver.api_version, sdk_ver.drv_version);

    // Get weight and internal mem size, dma used size
    ret = rknn_query(ctx[n], RKNN_QUERY_MEM_SIZE, &mem_size[n], sizeof(mem_size[n]));
    if (ret != RKNN_SUCC) {
      printf("rknn_query fail! ret=%d\n", ret);
      return -1;
    }
    printf("total weight size: %d, total internal size: %d\n", mem_size[n].total_weight_size, mem_size[n].total_internal_size);
    printf("total dma used size: %zu\n", (size_t)mem_size[n].total_dma_allocated_size);


    // Get Model Input Output Info
    ret = rknn_query(ctx[n], RKNN_QUERY_IN_OUT_NUM, &io_num[n], sizeof(io_num[n]));
    if (ret != RKNN_SUCC) {
      printf("rknn_query fail! ret=%d\n", ret);
      goto out;
    }
    printf("model input num: %d, output num: %d\n", io_num[n].n_input, io_num[n].n_output);

    if (io_num[n].n_output > MAX_OUTPUT_NUM) {
      printf("Please adjust the value of MAX_OUTPUT_NUM, it is too small for this model\n");
      return -1;
    };

    printf("input tensors:\n");
    memset(input_attrs[n], 0, io_num[n].n_input * sizeof(rknn_tensor_attr));
    for (uint32_t i = 0; i < io_num[n].n_input; i++) {
      input_attrs[n][i].index = i;
      // query info
      ret = rknn_query(ctx[n], RKNN_QUERY_INPUT_ATTR, &(input_attrs[n][i]), sizeof(rknn_tensor_attr));
      if (ret < 0) {
        printf("rknn_init error! ret=%d\n", ret);
        goto out;
      }
      dump_tensor_attr(&input_attrs[n][i]);
    }

    printf("output tensors:\n");
    memset(output_attrs[n], 0, io_num[n].n_output * sizeof(rknn_tensor_attr));
    for (uint32_t i = 0; i < io_num[n].n_output; i++) {
      output_attrs[n][i].index = i;
      // query info
      ret = rknn_query(ctx[n], RKNN_QUERY_OUTPUT_ATTR, &(output_attrs[n][i]), sizeof(rknn_tensor_attr));
      if (ret != RKNN_SUCC) {
        printf("rknn_query fail! ret=%d\n", ret);
        goto out;
      }
      dump_tensor_attr(&output_attrs[n][i]);
    }

    // Get custom string
    rknn_custom_string custom_string;
    ret = rknn_query(ctx[n], RKNN_QUERY_CUSTOM_STRING, &custom_string, sizeof(custom_string));
    if (ret != RKNN_SUCC) {
      printf("rknn_query fail! ret=%d\n", ret);
      goto out;
    }
    printf("custom string: %s\n", custom_string.string);

    // check max max_internal_size

    if (max_internal_size < mem_size[n].total_internal_size) {
      max_internal_size = mem_size[n].total_internal_size;
    }
  }

  printf("\033[0;32mMax internal size %d \033[0;0m\n", max_internal_size);

  // Allocate internal memory in outside
  internal_mem_max = rknn_create_mem(ctx[0], max_internal_size);

  for (int n=0; n<TOTAL_RKNN_MODEL_NUM; n++) {
    internal_mem[n] = rknn_create_mem_from_fd(ctx[n], internal_mem_max->fd,
                                              internal_mem_max->virt_addr, mem_size[n].total_internal_size, 0);
    ret = rknn_set_internal_mem(ctx[n], internal_mem[n]);
    if (ret < 0)
    {
      printf("rknn_set_internal_mem fail! ret=%d\n", ret);
      goto out;
    }

    printf("internal cma info: virt = %p, phy=0x%lx, fd =%d, size=%d\n", internal_mem[n]->virt_addr, internal_mem[n]->phys_addr, internal_mem[n]->fd, internal_mem[n]->size);

    // 使用rknn_create_mem作为分配器， 分配和设置每个模型的外部weight内存
    weight_mems[n] = rknn_create_mem(ctx[n], mem_size[n].total_weight_size);
    rknn_set_weight_mem(ctx[n], weight_mems[n]);
  }

  for (int n = 0; n < TOTAL_RKNN_MODEL_NUM; n++)
  {
    // Create input tensor memory
    // default input type is int8 (normalize and quantize need compute in outside)
    // if set uint8, will fuse normalize and quantize to npu
    input_attrs[n][0].type = input_type;
    // default fmt is NHWC, npu only support NHWC in zero copy mode
    input_attrs[n][0].fmt = input_layout;

    input_mems[n][0] = rknn_create_mem(ctx[n], input_attrs[n][0].size_with_stride);

    // Set input tensor memory
    ret = rknn_set_io_mem(ctx[n], input_mems[n][0], &input_attrs[n][0]);
    if (ret < 0)
    {
      printf("rknn_set_io_mem fail! ret=%d\n", ret);
      goto out;
    }

    // Create output tensor memory
    for (uint32_t i = 0; i < io_num[n].n_output; ++i) {
      output_mems[n][i] = rknn_create_mem(ctx[n], output_attrs[n][i].n_elems * sizeof(float));
    }

    // Set output tensor memory
    for (uint32_t i = 0; i < io_num[n].n_output; ++i) {
      // set output memory and attribute
      output_attrs[n][i].type = RKNN_TENSOR_FLOAT32;
      output_attrs[n][i].fmt = RKNN_TENSOR_NCHW;
      ret                  = rknn_set_io_mem(ctx[n], output_mems[n][i], &output_attrs[n][i]);
      if (ret < 0) {
        printf("rknn_set_io_mem fail! ret=%d\n", ret);
        goto out;
      }
    }
  }

  // Copy input data to input tensor memory
  for (int n=0; n<TOTAL_RKNN_MODEL_NUM; n++) {
    // Load image
    if (strstr(input_path[n], ".npy")) {
  #if NPY_SUPPORT
      int input_size = 0;
      input_data     = load_npy(input_path[n], &input_attrs[n][0], (int*)&input_type, &input_size);
  #else
      return -1;
  #endif
    } else {
      input_data = load_image(input_path[n], &input_attrs[n][0]);
    }
    if (!input_data) {
      printf("Load %s fail!\n", input_path[n]);
      goto out;
    }

    int height  = input_attrs[n][0].dims[1];
    int width  = input_attrs[n][0].dims[2];
    int channel = input_attrs[n][0].dims[3];
    int stride = input_attrs[n][0].w_stride;

    // TODO, you must resize the image if the size of input image  don't match the input shape
    if (width == stride)
    {
      memcpy((char *)(input_mems[n][0]->virt_addr) + input_mems[n][0]->offset, input_data, input_attrs[n][0].dims[2] * input_attrs[n][0].dims[1] * input_attrs[n][0].dims[3]);
    }
    else
    {
      // copy from src to dst with stride
      uint8_t* src_ptr = input_data;
      uint8_t* dst_ptr = (uint8_t*)input_mems[n][0]->virt_addr+input_mems[n][0]->offset;
      // width-channel elements
      int src_wc_elems = width * channel;
      int dst_wc_elems = stride * channel;
      for (int h = 0; h < height; ++h) {
        memcpy(dst_ptr, src_ptr, src_wc_elems);
        src_ptr += src_wc_elems;
        dst_ptr += dst_wc_elems;
      }
    }

    STBI_FREE(input_data);
  }

  // Run
  printf("Begin perf ...\n");
  for (int n=0; n<TOTAL_RKNN_MODEL_NUM; n++) {
    printf("==== %s ====\n", model_path[n]);
    for (int i = 0; i < loop_count; ++i) {
      int64_t start_us  = getCurrentTimeUs();
      ret               = rknn_run(ctx[n], NULL);
      int64_t elapse_us = getCurrentTimeUs() - start_us;
      if (ret < 0) {
        printf("rknn run error %d\n", ret);
        goto out;
      }

      printf("%4d: Elapse Time = %.2fms, FPS = %.2f\n", i, elapse_us / 1000.f, 1000.f * 1000.f / elapse_us);
    }

    // Get top 5
    uint32_t topNum = 5;
    for (uint32_t i = 0; i < io_num[n].n_output; i++) {
      uint32_t MaxClass[topNum];
      float    fMaxProb[topNum];

      float*   buffer    = (float*)output_mems[n][i]->virt_addr;
      uint32_t sz        = output_attrs[n][i].n_elems;
      int      top_count = sz > topNum ? topNum : sz;

      rknn_GetTopN(buffer, fMaxProb, MaxClass, sz, topNum);

      printf("---- Top%d ----\n", top_count);
      for (int j = 0; j < top_count; j++) {
        printf("%8.6f - %d\n", fMaxProb[j], MaxClass[j]);
      }
    }
  }


out:

  // free all objects
  if (internal_mem_max) {
    rknn_destroy_mem(ctx[0], internal_mem_max);
  }

  for (int n=0; n<TOTAL_RKNN_MODEL_NUM; n++) {
    // Destroy rknn memory

    if (ctx[n]) {
      if (input_mems[n][0]) {
        rknn_destroy_mem(ctx[n], input_mems[n][0]);
      }

      for (uint32_t i = 0; i < io_num[n].n_output; ++i) {
        if (output_mems[n][i]) {
          rknn_destroy_mem(ctx[n], output_mems[n][i]);
        }
      }

      if (internal_mem[n]) {
        rknn_destroy_mem(ctx[n], internal_mem[n]);
      }

      if (weight_mems[n])
      {
        rknn_destroy_mem(ctx[n], weight_mems[n]);
      }

      // destroy
      rknn_destroy(ctx[n]);
    }
  }


  return 0;

}
