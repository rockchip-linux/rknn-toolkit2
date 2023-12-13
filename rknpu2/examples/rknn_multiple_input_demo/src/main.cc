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

#include <string>
#include <vector>

/*-------------------------------------------
                  Functions
-------------------------------------------*/

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

static std::vector<std::string> split(const std::string& str, const std::string& pattern)
{
  std::vector<std::string> res;
  if (str == "")
    return res;
  std::string strs = str + pattern;
  size_t      pos  = strs.find(pattern);
  while (pos != strs.npos) {
    std::string temp = strs.substr(0, pos);
    res.push_back(temp);
    strs = strs.substr(pos + 1, strs.size());
    pos  = strs.find(pattern);
  }
  return res;
}

/*-------------------------------------------
                  Main Functions
-------------------------------------------*/
int main(int argc, char* argv[])
{
  char*                    model_path        = argv[1];
  char*                    input_paths       = argv[2];
  std::vector<std::string> input_paths_split = split(input_paths, "#");

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

  unsigned char* input_data[io_num.n_input];
  int            input_type[io_num.n_input];
  int            input_layout[io_num.n_input];
  int            input_size[io_num.n_input];
  for (int i = 0; i < io_num.n_input; i++) {
    input_data[i]   = NULL;
    input_type[i]   = RKNN_TENSOR_UINT8;
    input_layout[i] = RKNN_TENSOR_NHWC;
    input_size[i]   = input_attrs[i].n_elems * sizeof(uint8_t);
  }

  // Load input
  if (io_num.n_input != input_paths_split.size()) {
    return -1;
  }
  for (int i = 0; i < io_num.n_input; i++) {
    input_data[i] = new unsigned char[input_attrs[i].size];

    printf("%s\n", input_paths_split[i].c_str());

    FILE* fp = fopen(input_paths_split[i].c_str(), "rb");
    if (fp == NULL) {
      perror("open failed!");
      return -1;
    }

    fread(input_data[i], input_attrs[i].size, 1, fp);
    fclose(fp);
    if (!input_data[i]) {
      return -1;
    }
  }

  rknn_input inputs[io_num.n_input];
  memset(inputs, 0, io_num.n_input * sizeof(rknn_input));
  for (int i = 0; i < io_num.n_input; i++) {
    inputs[i].index        = i;
    inputs[i].pass_through = 0;
    inputs[i].type         = (rknn_tensor_type)input_type[i];
    inputs[i].fmt          = (rknn_tensor_format)input_layout[i];
    inputs[i].buf          = input_data[i];
    inputs[i].size         = input_size[i];
  }

  // Set input
  ret = rknn_inputs_set(ctx, io_num.n_input, inputs);
  if (ret < 0) {
    printf("rknn_input_set fail! ret=%d\n", ret);
    return -1;
  }

  ret = rknn_run(ctx, NULL);
  if (ret < 0) {
    printf("rknn_run fail! ret=%d\n", ret);
    return -1;
  }

  // Get output
  rknn_output outputs[io_num.n_output];
  memset(outputs, 0, io_num.n_output * sizeof(rknn_output));
  for (uint32_t i = 0; i < io_num.n_output; ++i) {
    outputs[i].want_float  = 1;
    outputs[i].index       = i;
    outputs[i].is_prealloc = 0;
  }

  ret = rknn_outputs_get(ctx, io_num.n_output, outputs, NULL);
  if (ret < 0) {
    printf("rknn_outputs_get fail! ret=%d\n", ret);
    return ret;
  }

  // Get top 5
  uint32_t topNum = 5;
  for (uint32_t i = 0; i < io_num.n_output; i++) {
    uint32_t MaxClass[topNum];
    float    fMaxProb[topNum];
    float*   buffer    = (float*)outputs[i].buf;
    uint32_t sz        = outputs[i].size / sizeof(float);
    int      top_count = sz > topNum ? topNum : sz;

    rknn_GetTopN(buffer, fMaxProb, MaxClass, sz, topNum);

    printf("---- Top%d ----\n", top_count);
    for (int j = 0; j < top_count; j++) {
      printf("%8.6f - %d\n", fMaxProb[j], MaxClass[j]);
    }
  }

  // release outputs
  ret = rknn_outputs_release(ctx, io_num.n_output, outputs);

  // destroy
  rknn_destroy(ctx);

  for (int i = 0; i < io_num.n_input; i++) {
    free(input_data[i]);
  }

  return 0;
}
