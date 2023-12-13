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
#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "rknn_api.h"

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

using namespace std;
using namespace cv;

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
  printf("  index=%d, name=%s, n_dims=%d, dims=[%d, %d, %d, %d], n_elems=%d, size=%d, fmt=%s, type=%s, qnt_type=%s, "
         "zp=%d, scale=%f\n",
         attr->index, attr->name, attr->n_dims, attr->dims[0], attr->dims[1], attr->dims[2], attr->dims[3],
         attr->n_elems, attr->size, get_format_string(attr->fmt), get_type_string(attr->type),
         get_qnt_type_string(attr->qnt_type), attr->zp, attr->scale);
}

/*-------------------------------------------
                  Main Functions
-------------------------------------------*/
int main(int argc, char* argv[])
{
  if (argc < 3) {
    printf("Usage:%s model_path input_path [loop_count]\n", argv[0]);
    return -1;
  }

  char* model_path = argv[1];
  char* input_path = argv[2];

  int loop_count = 1;
  if (argc > 3) {
    loop_count = atoi(argv[3]);
  }

  rknn_context ctx = 0;

  // Load RKNN Model
  int ret = rknn_init(&ctx, model_path, 0, 0, NULL);
  if (ret < 0) {
    printf("rknn_init fail! ret=%d\n", ret);
    return -1;
  }

  // Get sdk and driver version
  rknn_sdk_version sdk_ver;
  ret = rknn_query(ctx, RKNN_QUERY_SDK_VERSION, &sdk_ver, sizeof(sdk_ver));
  if (ret != RKNN_SUCC) {
    printf("rknn_query fail! ret=%d\n", ret);
    return -1;
  }
  printf("rknn_api/rknnrt version: %s, driver version: %s\n", sdk_ver.api_version, sdk_ver.drv_version);

  // Get Model Input Output Info
  rknn_input_output_num io_num;
  ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
  if (ret != RKNN_SUCC) {
    printf("rknn_query fail! ret=%d\n", ret);
    return -1;
  }
  printf("model input num: %d, output num: %d\n", io_num.n_input, io_num.n_output);

  printf("input tensors:\n");
  rknn_tensor_attr input_attrs[io_num.n_input];
  memset(input_attrs, 0, io_num.n_input * sizeof(rknn_tensor_attr));
  for (uint32_t i = 0; i < io_num.n_input; i++) {
    input_attrs[i].index = i;
    // query info
    ret = rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr));
    if (ret < 0) {
      printf("rknn_init error! ret=%d\n", ret);
      return -1;
    }
    dump_tensor_attr(&input_attrs[i]);
  }

  printf("output tensors:\n");
  rknn_tensor_attr output_attrs[io_num.n_output];
  memset(output_attrs, 0, io_num.n_output * sizeof(rknn_tensor_attr));
  for (uint32_t i = 0; i < io_num.n_output; i++) {
    output_attrs[i].index = i;
    // query info
    ret = rknn_query(ctx, RKNN_QUERY_OUTPUT_ATTR, &(output_attrs[i]), sizeof(rknn_tensor_attr));
    if (ret != RKNN_SUCC) {
      printf("rknn_query fail! ret=%d\n", ret);
      return -1;
    }
    dump_tensor_attr(&output_attrs[i]);
  }

  // Get custom string
  rknn_custom_string custom_string;
  ret = rknn_query(ctx, RKNN_QUERY_CUSTOM_STRING, &custom_string, sizeof(custom_string));
  if (ret != RKNN_SUCC) {
    printf("rknn_query fail! ret=%d\n", ret);
    return -1;
  }
  printf("custom string: %s\n", custom_string.string);

  unsigned char*     input_data   = NULL;
  rknn_tensor_type   input_type   = RKNN_TENSOR_UINT8;
  rknn_tensor_format input_layout = RKNN_TENSOR_NHWC;

  // Load image
  int req_height  = 0;
  int req_width   = 0;
  int req_channel = 0;

  switch (input_attrs[0].fmt) {
  case RKNN_TENSOR_NHWC:
    req_height  = input_attrs[0].dims[1];
    req_width   = input_attrs[0].dims[2];
    req_channel = input_attrs[0].dims[3];
    break;
  case RKNN_TENSOR_NCHW:
    req_height  = input_attrs[0].dims[2];
    req_width   = input_attrs[0].dims[3];
    req_channel = input_attrs[0].dims[1];
    break;
  default:
    printf("meet unsupported layout\n");
    return -1;
  }

  int height  = 0;
  int width   = 0;
  int channel = 0;

  cv::Mat orig_img = imread(input_path, cv::IMREAD_COLOR);
  if (!orig_img.data) {
    printf("cv::imread %s fail!\n", input_path);
    return -1;
  }

  // if origin model is from Caffe, you maybe not need do BGR2RGB.
  cv::Mat orig_img_rgb;
  cv::cvtColor(orig_img, orig_img_rgb, cv::COLOR_BGR2RGB);

  cv::Mat img = orig_img_rgb.clone();
  if (orig_img.cols != req_width || orig_img.rows != req_height) {
    printf("resize %d %d to %d %d\n", orig_img.cols, orig_img.rows, req_width, req_height);
    cv::resize(orig_img_rgb, img, cv::Size(req_width, req_height), 0, 0, cv::INTER_LINEAR);
  }
  input_data = img.data;
  if (!input_data) {
    return -1;
  }

  // Create input tensor memory
  rknn_tensor_mem* input_mems[1];
  // default input type is int8 (normalize and quantize need compute in outside)
  // if set uint8, will fuse normalize and quantize to npu
  input_attrs[0].type = input_type;
  // default fmt is NHWC, npu only support NHWC in zero copy mode
  input_attrs[0].fmt = input_layout;

  input_mems[0] = rknn_create_mem(ctx, input_attrs[0].size_with_stride);

  // Copy input data to input tensor memory
  width      = input_attrs[0].dims[2];
  int stride = input_attrs[0].w_stride;

  if (width == stride) {
    memcpy(input_mems[0]->virt_addr, input_data, width * input_attrs[0].dims[1] * input_attrs[0].dims[3]);
  } else {
    int height  = input_attrs[0].dims[1];
    int channel = input_attrs[0].dims[3];
    // copy from src to dst with stride
    uint8_t* src_ptr = input_data;
    uint8_t* dst_ptr = (uint8_t*)input_mems[0]->virt_addr;
    // width-channel elements
    int src_wc_elems = width * channel;
    int dst_wc_elems = stride * channel;
    for (int h = 0; h < height; ++h) {
      memcpy(dst_ptr, src_ptr, src_wc_elems);
      src_ptr += src_wc_elems;
      dst_ptr += dst_wc_elems;
    }
  }

  // Create output tensor memory
  rknn_tensor_mem* output_mems[io_num.n_output];
  for (uint32_t i = 0; i < io_num.n_output; ++i) {
    // default output type is depend on model, this require float32 to compute top5
    // allocate float32 output tensor
    int output_size = output_attrs[i].n_elems * sizeof(float);
    output_mems[i]  = rknn_create_mem(ctx, output_size);
  }

  // Set input tensor memory
  ret = rknn_set_io_mem(ctx, input_mems[0], &input_attrs[0]);
  if (ret < 0) {
    printf("rknn_set_io_mem fail! ret=%d\n", ret);
    return -1;
  }

  // Set output tensor memory
  for (uint32_t i = 0; i < io_num.n_output; ++i) {
    // default output type is depend on model, this require float32 to compute top5
    output_attrs[i].type = RKNN_TENSOR_FLOAT32;
    // set output memory and attribute
    ret = rknn_set_io_mem(ctx, output_mems[i], &output_attrs[i]);
    if (ret < 0) {
      printf("rknn_set_io_mem fail! ret=%d\n", ret);
      return -1;
    }
  }

  // Run
  printf("Begin perf ...\n");
  for (int i = 0; i < loop_count; ++i) {
    int64_t start_us  = getCurrentTimeUs();
    ret               = rknn_run(ctx, NULL);
    int64_t elapse_us = getCurrentTimeUs() - start_us;
    if (ret < 0) {
      printf("rknn run error %d\n", ret);
      return -1;
    }
    printf("%4d: Elapse Time = %.2fms, FPS = %.2f\n", i, elapse_us / 1000.f, 1000.f * 1000.f / elapse_us);
  }

  // Get top 5
  uint32_t topNum = 5;
  for (uint32_t i = 0; i < io_num.n_output; i++) {
    uint32_t MaxClass[topNum];
    float    fMaxProb[topNum];
    float*   buffer    = (float*)output_mems[i]->virt_addr;
    uint32_t sz        = output_attrs[i].n_elems;
    int      top_count = sz > topNum ? topNum : sz;

    rknn_GetTopN(buffer, fMaxProb, MaxClass, sz, topNum);

    printf("---- Top%d ----\n", top_count);
    for (int j = 0; j < top_count; j++) {
      printf("%8.6f - %d\n", fMaxProb[j], MaxClass[j]);
    }
  }

  // Destroy rknn memory
  rknn_destroy_mem(ctx, input_mems[0]);
  for (uint32_t i = 0; i < io_num.n_output; ++i) {
    rknn_destroy_mem(ctx, output_mems[i]);
  }

  // destroy
  rknn_destroy(ctx);
  return 0;
}
