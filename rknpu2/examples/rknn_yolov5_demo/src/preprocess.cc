// Copyright (c) 2023 by Rockchip Electronics Co., Ltd. All Rights Reserved.
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

#include "preprocess.h"

void letterbox(const cv::Mat &image, cv::Mat &padded_image, BOX_RECT &pads, const float scale, const cv::Size &target_size, const cv::Scalar &pad_color)
{
    // 调整图像大小
    cv::Mat resized_image;
    cv::resize(image, resized_image, cv::Size(), scale, scale);

    // 计算填充大小
    int pad_width = target_size.width - resized_image.cols;
    int pad_height = target_size.height - resized_image.rows;

    pads.left = pad_width / 2;
    pads.right = pad_width - pads.left;
    pads.top = pad_height / 2;
    pads.bottom = pad_height - pads.top;

    // 在图像周围添加填充
    cv::copyMakeBorder(resized_image, padded_image, pads.top, pads.bottom, pads.left, pads.right, cv::BORDER_CONSTANT, pad_color);
}

int resize_rga(rga_buffer_t &src, rga_buffer_t &dst, const cv::Mat &image, cv::Mat &resized_image, const cv::Size &target_size)
{
    im_rect src_rect;
    im_rect dst_rect;
    memset(&src_rect, 0, sizeof(src_rect));
    memset(&dst_rect, 0, sizeof(dst_rect));
    size_t img_width = image.cols;
    size_t img_height = image.rows;
    if (image.type() != CV_8UC3)
    {
        printf("source image type is %d!\n", image.type());
        return -1;
    }
    size_t target_width = target_size.width;
    size_t target_height = target_size.height;
    src = wrapbuffer_virtualaddr((void *)image.data, img_width, img_height, RK_FORMAT_RGB_888);
    dst = wrapbuffer_virtualaddr((void *)resized_image.data, target_width, target_height, RK_FORMAT_RGB_888);
    int ret = imcheck(src, dst, src_rect, dst_rect);
    if (IM_STATUS_NOERROR != ret)
    {
        fprintf(stderr, "rga check error! %s", imStrError((IM_STATUS)ret));
        return -1;
    }
    IM_STATUS STATUS = imresize(src, dst);
    return 0;
}
