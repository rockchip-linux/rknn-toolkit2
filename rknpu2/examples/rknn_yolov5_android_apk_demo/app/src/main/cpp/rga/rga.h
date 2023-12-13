/*
 * Copyright (C) 2016 Rockchip Electronics Co., Ltd.
 * Authors:
 *    Zhiqin Wei <wzq@rock-chips.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _RGA_DRIVER_H_
#define _RGA_DRIVER_H_


#ifndef ENABLE
#define ENABLE 1
#endif

#ifndef DISABLE
#define DISABLE 0
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* In order to be compatible with RK_FORMAT_XX and HAL_PIXEL_FORMAT_XX,
 * RK_FORMAT_XX is shifted to the left by 8 bits to distinguish.  */
typedef enum _Rga_SURF_FORMAT {
    RK_FORMAT_RGBA_8888    = 0x0 << 8,
    RK_FORMAT_RGBX_8888    = 0x1 << 8,
    RK_FORMAT_RGB_888      = 0x2 << 8,
    RK_FORMAT_BGRA_8888    = 0x3 << 8,
    RK_FORMAT_RGB_565      = 0x4 << 8,
    RK_FORMAT_RGBA_5551    = 0x5 << 8,
    RK_FORMAT_RGBA_4444    = 0x6 << 8,
    RK_FORMAT_BGR_888      = 0x7 << 8,

    RK_FORMAT_YCbCr_422_SP = 0x8 << 8,
    RK_FORMAT_YCbCr_422_P  = 0x9 << 8,
    RK_FORMAT_YCbCr_420_SP = 0xa << 8,
    RK_FORMAT_YCbCr_420_P  = 0xb << 8,

    RK_FORMAT_YCrCb_422_SP = 0xc << 8,
    RK_FORMAT_YCrCb_422_P  = 0xd << 8,
    RK_FORMAT_YCrCb_420_SP = 0xe << 8,
    RK_FORMAT_YCrCb_420_P  = 0xf << 8,

    RK_FORMAT_BPP1         = 0x10 << 8,
    RK_FORMAT_BPP2         = 0x11 << 8,
    RK_FORMAT_BPP4         = 0x12 << 8,
    RK_FORMAT_BPP8         = 0x13 << 8,

    RK_FORMAT_Y4           = 0x14 << 8,
    RK_FORMAT_YCbCr_400    = 0x15 << 8,

    RK_FORMAT_BGRX_8888    = 0x16 << 8,

    RK_FORMAT_YVYU_422     = 0x18 << 8,
    RK_FORMAT_YVYU_420     = 0x19 << 8,
    RK_FORMAT_VYUY_422     = 0x1a << 8,
    RK_FORMAT_VYUY_420     = 0x1b << 8,
    RK_FORMAT_YUYV_422     = 0x1c << 8,
    RK_FORMAT_YUYV_420     = 0x1d << 8,
    RK_FORMAT_UYVY_422     = 0x1e << 8,
    RK_FORMAT_UYVY_420     = 0x1f << 8,

    RK_FORMAT_YCbCr_420_SP_10B = 0x20 << 8,
    RK_FORMAT_YCrCb_420_SP_10B = 0x21 << 8,
    RK_FORMAT_YCbCr_422_SP_10B = 0x22 << 8,
    RK_FORMAT_YCrCb_422_SP_10B = 0x23 << 8,
    /* For compatibility with misspellings */
    RK_FORMAT_YCbCr_422_10b_SP = RK_FORMAT_YCbCr_422_SP_10B,
    RK_FORMAT_YCrCb_422_10b_SP = RK_FORMAT_YCrCb_422_SP_10B,

    RK_FORMAT_BGR_565      = 0x24 << 8,
    RK_FORMAT_BGRA_5551    = 0x25 << 8,
    RK_FORMAT_BGRA_4444    = 0x26 << 8,

    RK_FORMAT_ARGB_8888    = 0x28 << 8,
    RK_FORMAT_XRGB_8888    = 0x29 << 8,
    RK_FORMAT_ARGB_5551    = 0x2a << 8,
    RK_FORMAT_ARGB_4444    = 0x2b << 8,
    RK_FORMAT_ABGR_8888    = 0x2c << 8,
    RK_FORMAT_XBGR_8888    = 0x2d << 8,
    RK_FORMAT_ABGR_5551    = 0x2e << 8,
    RK_FORMAT_ABGR_4444    = 0x2f << 8,

    RK_FORMAT_RGBA2BPP     = 0x30 << 8,

    RK_FORMAT_UNKNOWN      = 0x100 << 8,
} RgaSURF_FORMAT;

enum {
    yuv2rgb_mode0            = 0x0,     /* BT.601 MPEG */
    yuv2rgb_mode1            = 0x1,     /* BT.601 JPEG */
    yuv2rgb_mode2            = 0x2,     /* BT.709      */

    rgb2yuv_601_full                = 0x1 << 8,
    rgb2yuv_709_full                = 0x2 << 8,
    yuv2yuv_601_limit_2_709_limit   = 0x3 << 8,
    yuv2yuv_601_limit_2_709_full    = 0x4 << 8,
    yuv2yuv_709_limit_2_601_limit   = 0x5 << 8,
    yuv2yuv_709_limit_2_601_full    = 0x6 << 8,     //not support
    yuv2yuv_601_full_2_709_limit    = 0x7 << 8,
    yuv2yuv_601_full_2_709_full     = 0x8 << 8,     //not support
    yuv2yuv_709_full_2_601_limit    = 0x9 << 8,     //not support
    yuv2yuv_709_full_2_601_full     = 0xa << 8,     //not support
    full_csc_mask = 0xf00,
};

enum {
    RGA3_SCHEDULER_CORE0    = 1 << 0,
    RGA3_SCHEDULER_CORE1    = 1 << 1,
    RGA2_SCHEDULER_CORE0    = 1 << 2,
};

/* RGA3 rd_mode */
enum
{
    raster_mode             = 0x1 << 0,
    fbc_mode                = 0x1 << 1,
    tile_mode               = 0x1 << 2,
};

#ifdef __cplusplus
}
#endif

#endif /*_RK29_IPP_DRIVER_H_*/
