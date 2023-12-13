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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include "rknn_api.h"
#include <sys/time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>

#define NPY_SUPPORT 1
#if NPY_SUPPORT
#include "cnpy/cnpy.h"
using namespace cnpy;
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
    std::string shape_str = attr->n_dims < 1 ? "" : std::to_string(attr->dims[0]);
    for (int i = 1; i < attr->n_dims; ++i)
    {
        shape_str += ", " + std::to_string(attr->dims[i]);
    }

    printf("  index=%d, name=%s, n_dims=%d, dims=[%s], n_elems=%d, size=%d, w_stride = %d, size_with_stride=%d, fmt=%s, "
           "type=%s, qnt_type=%s, "
           "zp=%d, scale=%f\n",
           attr->index, attr->name, attr->n_dims, shape_str.c_str(), attr->n_elems, attr->size, attr->w_stride,
           attr->size_with_stride, get_format_string(attr->fmt), get_type_string(attr->type),
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

#if NPY_SUPPORT
static unsigned char *load_npy(const char *input_path, rknn_tensor_attr *input_attr, int *input_size)
{
    if (input_attr->fmt != RKNN_TENSOR_NHWC)
    {
        fprintf(stderr, "Only support input fmt = RKNN_TENSOR_NHWC\n");
        return NULL;
    }
    int req_height = input_attr->dims[1];
    int req_width = input_attr->dims[2];
    int req_channel = input_attr->dims[3];

    printf("Loading %s\n", input_path);

    NpyArray npy_data = npy_load(input_path);

    int type_bytes = npy_data.word_size;
    std::string typeName = npy_data.typeName;

    printf("npy data type:%s\n", typeName.c_str());

    if (typeName != "uint8")
    {
        fprintf(stderr, "Model required npy data type = uint8, but get %s\n", typeName.c_str());
        return NULL;
    }

    // npy shape = NHWC
    int npy_shape[4] = {1, 1, 1, 1};

    int start = npy_data.shape.size() == 4 ? 0 : 1;
    for (size_t i = 0; i < npy_data.shape.size() && i < 4; ++i)
    {
        npy_shape[start + i] = npy_data.shape[i];
    }

    int height = npy_shape[1];
    int width = npy_shape[2];
    int channel = npy_shape[3];

    if ((input_attr->fmt != RKNN_TENSOR_UNDEFINED) &&
        (width != req_width || height != req_height || channel != req_channel))
    {
        printf("npy shape match failed!, npy shape =(%d, %d, %d) , expect input shape = (%d, %d, %d)\n",
               height, width, channel, req_height, req_width, req_channel);
        return NULL;
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

template <typename T>
static void save_npy(const char *output_path, T *output_data, rknn_tensor_attr *output_attr)
{
    std::vector<size_t> output_shape;

    for (uint32_t i = 0; i < output_attr->n_dims; ++i)
    {
        output_shape.push_back(output_attr->dims[i]);
    }

    npy_save<T>(output_path, output_data, output_shape);
}
#endif

static unsigned char *load_image(const char *image_path, rknn_tensor_attr *input_attr)
{
    int req_channel = input_attr->dims[3];
    int height = 0;
    int width = 0;
    int channel = 0;

    printf("Loading %s\n", image_path);

    unsigned char *image_data = stbi_load(image_path, &width, &height, &channel, req_channel);
    if (image_data == NULL)
    {
        printf("load image failed!\n");
        return NULL;
    }

    switch (input_attr->fmt)
    {
    case RKNN_TENSOR_NHWC:
        input_attr->dims[0] = 1;
        input_attr->dims[1] = height;
        input_attr->dims[2] = width;
        input_attr->dims[3] = channel;
        break;

    default:
        fprintf(stderr, "load_image error, unsupport model input layout: %s\n", get_format_string(input_attr->fmt));
        break;
    }

    return image_data;
}

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

static int write_data_to_file(char *path, char *data, unsigned int size)
{
    FILE *fp;

    fp = fopen(path, "w");
    if (fp == NULL)
    {
        printf("open error: %s", path);
        return -1;
    }

    fwrite(data, 1, size, fp);
    fflush(fp);

    fclose(fp);
    return 0;
}

static void *load_file(const char *file_path, size_t *file_size)
{
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        printf("failed to open file: %s\n", file_path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size_t size = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);

    void *file_data = malloc(size);
    if (file_data == NULL)
    {
        fclose(fp);
        printf("failed allocate file size: %zu\n", size);
        return NULL;
    }

    if (fread(file_data, 1, size, fp) != size)
    {
        fclose(fp);
        free(file_data);
        printf("failed to read file data!\n");
        return NULL;
    }

    fclose(fp);

    *file_size = size;

    return file_data;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage:%s model_path [input_path] [loop_count] [core_mask] [output_dir]\n", argv[0]);
        return -1;
    }
    // 程序参数
    char *model_path = argv[1];
    std::vector<std::string> input_paths_split;
    if (argc > 2)
    {
        char *input_paths = argv[2];
        input_paths_split = split(input_paths, "#");
    }

    int loop_count = 1;
    if (argc > 3)
    {
        loop_count = atoi(argv[3]);
    }

    uint32_t core_mask = 1;
    if (argc > 4)
    {
        core_mask = strtoul(argv[4], NULL, 10);
    }

    char *output_dir = NULL;
    if (argc > 5)
    {
        output_dir = argv[4];
    }

    // 加载模型
    rknn_context ctx;
    int ret = rknn_init(&ctx, model_path, 0, 0, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "rknn_init error! ret=%d\n", ret);
        return -1;
    }
    printf("Load model sucess\n");

    // 获取模型SDK版本
    rknn_sdk_version sdk_ver;
    ret = rknn_query(ctx, RKNN_QUERY_SDK_VERSION, &sdk_ver, sizeof(sdk_ver));
    if (ret != RKNN_SUCC)
    {
        fprintf(stderr, "rknn_query error! ret=%d\n", ret);
        return -1;
    }
    printf("rknn_api/rknnrt version: %s, driver version: %s\n", sdk_ver.api_version, sdk_ver.drv_version);

    // 获取模型输入输出个数
    rknn_input_output_num io_num;
    ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
    if (ret != RKNN_SUCC)
    {
        fprintf(stderr, "rknn_query error! ret=%d\n", ret);
        return -1;
    }
    if (io_num.n_input != input_paths_split.size())
    {
        return -1;
    }

    printf("model input num: %d, output num: %d\n", io_num.n_input, io_num.n_output);

    // 获取模型输入输出信息
    printf("input tensors:\n");
    rknn_tensor_attr input_attrs[io_num.n_input];
    memset(input_attrs, 0, io_num.n_input * sizeof(rknn_tensor_attr));
    for (uint32_t i = 0; i < io_num.n_input; i++)
    {
        input_attrs[i].index = i;
        // query info
        ret = rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr));
        if (ret < 0)
        {
            fprintf(stderr, "rknn_query error! ret=%d\n", ret);
            return -1;
        }
        dump_tensor_attr(&input_attrs[i]);
    }

    printf("output tensors:\n");
    rknn_tensor_attr output_attrs[io_num.n_output];
    memset(output_attrs, 0, io_num.n_output * sizeof(rknn_tensor_attr));
    for (uint32_t i = 0; i < io_num.n_output; i++)
    {
        output_attrs[i].index = i;
        // query info
        ret = rknn_query(ctx, RKNN_QUERY_OUTPUT_ATTR, &(output_attrs[i]), sizeof(rknn_tensor_attr));
        if (ret != RKNN_SUCC)
        {
            fprintf(stderr, "rknn_query error! ret=%d\n", ret);
            return -1;
        }
        dump_tensor_attr(&output_attrs[i]);
    }

    // 查询模型支持的输入形状
    printf("dynamic inputs shape range:\n");
    rknn_input_range shape_range[io_num.n_input];
    memset(shape_range, 0, io_num.n_input * sizeof(rknn_input_range));
    for (uint32_t i = 0; i < io_num.n_input; i++)
    {
        shape_range[i].index = i;
        ret = rknn_query(ctx, RKNN_QUERY_INPUT_DYNAMIC_RANGE, &shape_range[i], sizeof(rknn_input_range));
        if (ret != RKNN_SUCC)
        {
            fprintf(stderr, "rknn_query error! ret=%d\n", ret);
            return -1;
        }
        dump_input_dynamic_range(&shape_range[i]);
    }

    // 加载测试图片或npy文件
    std::vector<cv::Mat> imgs;
    unsigned char *input_data[io_num.n_input];
    int input_size[io_num.n_input];
    for (int i = 0; i < io_num.n_input; i++)
    {
        input_data[i] = NULL;
        input_size[i] = input_attrs[i].n_elems * sizeof(uint8_t);
    }
    int shape_num = shape_range[0].shape_number;
    if (input_paths_split.size() > 0)
    {
        // Load input
        if (io_num.n_input != input_paths_split.size())
        {
            printf("input missing!, need input number: %d\n", io_num.n_input);
            return -1;
        }
        for (int i = 0; i < io_num.n_input; i++)
        {
            if (strstr(input_paths_split[i].c_str(), ".npy"))
            {
#if NPY_SUPPORT
                // 读取NHWC uint8格式npy
                input_data[i] = load_npy(input_paths_split[i].c_str(), &input_attrs[i], &input_size[i]);
                if (input_size[i] != input_attrs[i].n_elems * sizeof(uint8_t))
                {
                    fprintf(stderr, "Read input[%d] npy size error, read: %d, but requried: %lu\n", i, input_size[i], input_attrs[i].n_elems * sizeof(uint8_t));
                    return -1;
                }
                if (input_data[i] == NULL)
                {
                    return -1;
                }
                int height = input_attrs[i].fmt == RKNN_TENSOR_NHWC ? input_attrs[i].dims[1] : input_attrs[i].dims[2];
                int width = input_attrs[i].fmt == RKNN_TENSOR_NHWC ? input_attrs[i].dims[2] : input_attrs[i].dims[3];
                cv::Mat image(height, width, CV_8UC3, input_data[i]);
                imgs.emplace_back(image);
#else
                return -1;
#endif
            }
            else
            {
                cv::Mat img = cv::imread(input_paths_split[i]);
                imgs.emplace_back(img);
            }
        }
    }
    else
    {
        for (int i = 0; i < io_num.n_input; i++)
        {
            input_data[i] = (unsigned char *)malloc(input_size[i]);
        }
    }

    // 创建最大的输入tensor内存
    rknn_tensor_mem *input_mems[io_num.n_input];
    for (int i = 0; i < io_num.n_input; i++)
    {
        // default input type is int8 (normalize and quantize need compute in outside)
        // if set uint8, will fuse normalize and quantize to npu
        input_attrs[i].type = RKNN_TENSOR_UINT8;
        // default fmt is NHWC, npu only support NHWC in zero copy mode
        input_attrs[i].fmt = RKNN_TENSOR_NHWC;

        input_mems[i] = rknn_create_mem(ctx, input_attrs[i].size_with_stride);
    }

    // 创建最大的输出tensor内存
    rknn_tensor_mem *output_mems[io_num.n_output];
    for (uint32_t i = 0; i < io_num.n_output; ++i)
    {
        // default output type is depend on model, this require float32 to compute top5
        // allocate float32 output tensor
        int output_size = output_attrs[i].size * sizeof(float);
        output_mems[i] = rknn_create_mem(ctx, output_size);
    }

    // 加载输入并设置模型输入形状，每次切换输入形状要调用一次
    for (int s = 0; s < shape_num; ++s)
    {
        for (int i = 0; i < io_num.n_input; i++)
        {
            for (int j = 0; j < input_attrs[i].n_dims; ++j)
            {
                input_attrs[i].dims[j] = shape_range[i].dyn_range[s][j];
            }
        }
        ret = rknn_set_input_shapes(ctx, io_num.n_input, input_attrs);
        if (ret < 0)
        {
            fprintf(stderr, "rknn_set_input_shape error! ret=%d\n", ret);
            return -1;
        }

        // 获取当前次推理的输入和输出形状
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

        // 指定NPU核心数量，仅3588支持
        rknn_set_core_mask(ctx, (rknn_core_mask)core_mask);

        // 设置输入信息
        rknn_input inputs[io_num.n_input];
        memset(inputs, 0, io_num.n_input * sizeof(rknn_input));
        std::vector<cv::Mat> resize_imgs;
        resize_imgs.resize(io_num.n_input);
        for (int i = 0; i < io_num.n_input; i++)
        {
            int height = cur_input_attrs[i].fmt == RKNN_TENSOR_NHWC ? cur_input_attrs[i].dims[1] : cur_input_attrs[i].dims[2];
            int width = cur_input_attrs[i].fmt == RKNN_TENSOR_NHWC ? cur_input_attrs[i].dims[2] : cur_input_attrs[i].dims[3];
            int stride = cur_input_attrs[i].w_stride;
            cv::resize(imgs[i], resize_imgs[i], cv::Size(width, height));
            int input_size = resize_imgs[i].total() * resize_imgs[i].channels();
            // 拷贝外部数据到零拷贝输入缓冲区
            if (width == stride)
            {
                memcpy(input_mems[i]->virt_addr, resize_imgs[i].data, input_size);
            }
            else
            {
                int height = cur_input_attrs[i].dims[1];
                int channel = cur_input_attrs[i].dims[3];
                // copy from src to dst with stride
                uint8_t *src_ptr = resize_imgs[i].data;
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

        // 更新输入零拷贝缓冲区内存
        for (int i = 0; i < io_num.n_input; i++)
        {
            cur_input_attrs[i].type = RKNN_TENSOR_UINT8;
            ret = rknn_set_io_mem(ctx, input_mems[i], &cur_input_attrs[i]);
            if (ret < 0)
            {
                printf("rknn_set_io_mem fail! ret=%d\n", ret);
                return -1;
            }
        }

        // 更新输出零拷贝缓冲区内存
        for (uint32_t i = 0; i < io_num.n_output; ++i)
        {
            // default output type is depend on model, this require float32 to compute top5
            cur_output_attrs[i].type = RKNN_TENSOR_FLOAT32;
            cur_output_attrs[i].fmt = RKNN_TENSOR_NCHW;
            // set output memory and attribute
            ret = rknn_set_io_mem(ctx, output_mems[i], &cur_output_attrs[i]);
            if (ret < 0)
            {
                printf("rknn_set_io_mem fail! ret=%d\n", ret);
                return -1;
            }
        }

        // 进行推理
        printf("Begin perf ...\n");
        double total_time = 0;
        for (int i = 0; i < loop_count; ++i)
        {
            int64_t start_us = getCurrentTimeUs();
            ret = rknn_run(ctx, NULL);
            int64_t elapse_us = getCurrentTimeUs() - start_us;
            if (ret < 0)
            {
                printf("rknn run error %d\n", ret);
                return -1;
            }
            total_time += elapse_us / 1000.f;
            printf("%4d: Elapse Time = %.2fms, FPS = %.2f\n", i, elapse_us / 1000.f, 1000.f * 1000.f / elapse_us);
        }
        printf("Avg FPS = %.3f\n", loop_count * 1000.f / total_time);

#if NPY_SUPPORT
        // save output
        for (uint32_t i = 0; i < io_num.n_output; i++)
        {
            char output_path[PATH_MAX];
            sprintf(output_path, "%s/rt_output%d.npy", output_dir ? output_dir : ".", i);
            save_npy<float>(output_path, (float *)output_mems[i]->virt_addr, &cur_output_attrs[i]);
        }
#endif

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
    }
    // 释放资源
    for (uint32_t i = 0; i < io_num.n_input; ++i)
    {
        rknn_destroy_mem(ctx, input_mems[i]);
    }
    for (uint32_t i = 0; i < io_num.n_output; ++i)
    {
        rknn_destroy_mem(ctx, output_mems[i]);
    }

    rknn_destroy(ctx);
    for (int i = 0; i < io_num.n_input; i++)
    {
        free(input_data[i]);
    }
    return 0;
}