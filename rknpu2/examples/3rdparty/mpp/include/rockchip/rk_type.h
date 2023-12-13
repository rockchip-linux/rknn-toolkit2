/*
 * Copyright 2015 Rockchip Electronics Co. LTD
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

#ifndef __RK_TYPE_H__
#define __RK_TYPE_H__

#include <stddef.h>

#if defined(_WIN32) && !defined(__MINGW32CE__)

typedef unsigned char           RK_U8;
typedef unsigned short          RK_U16;
typedef unsigned int            RK_U32;
typedef unsigned long           RK_ULONG;
typedef unsigned __int64        RK_U64;

typedef signed char             RK_S8;
typedef signed short            RK_S16;
typedef signed int              RK_S32;
typedef signed long             RK_LONG;
typedef signed __int64          RK_S64;

#else

typedef unsigned char           RK_U8;
typedef unsigned short          RK_U16;
typedef unsigned int            RK_U32;
typedef unsigned long           RK_ULONG;
typedef unsigned long long int  RK_U64;


typedef signed char             RK_S8;
typedef signed short            RK_S16;
typedef signed int              RK_S32;
typedef signed long             RK_LONG;
typedef signed long long int    RK_S64;

#endif

#ifndef MODULE_TAG
#define MODULE_TAG              NULL
#endif

/**
 * @ingroup rk_mpi
 * @brief The type of mpp context
 * @details This type is used when calling mpp_init(), which including decoder,
 *          encoder and Image Signal Process(ISP). So far decoder and encoder
 *          are supported perfectly, and ISP will be supported in the future.
 */
typedef enum {
    MPP_CTX_DEC,  /**< decoder */
    MPP_CTX_ENC,  /**< encoder */
    MPP_CTX_ISP,  /**< isp */
    MPP_CTX_BUTT, /**< undefined */
} MppCtxType;

/**
 * @ingroup rk_mpi
 * @brief Enumeration used to define the possible video compression codings.
 *        sync with the omx_video.h
 *
 * @note  This essentially refers to file extensions. If the coding is
 *        being used to specify the ENCODE type, then additional work
 *        must be done to configure the exact flavor of the compression
 *        to be used.  For decode cases where the user application can
 *        not differentiate between MPEG-4 and H.264 bit streams, it is
 *        up to the codec to handle this.
 */
typedef enum {
    MPP_VIDEO_CodingUnused,             /**< Value when coding is N/A */
    MPP_VIDEO_CodingAutoDetect,         /**< Autodetection of coding type */
    MPP_VIDEO_CodingMPEG2,              /**< AKA: H.262 */
    MPP_VIDEO_CodingH263,               /**< H.263 */
    MPP_VIDEO_CodingMPEG4,              /**< MPEG-4 */
    MPP_VIDEO_CodingWMV,                /**< Windows Media Video (WMV1,WMV2,WMV3)*/
    MPP_VIDEO_CodingRV,                 /**< all versions of Real Video */
    MPP_VIDEO_CodingAVC,                /**< H.264/AVC */
    MPP_VIDEO_CodingMJPEG,              /**< Motion JPEG */
    MPP_VIDEO_CodingVP8,                /**< VP8 */
    MPP_VIDEO_CodingVP9,                /**< VP9 */
    MPP_VIDEO_CodingVC1 = 0x01000000,   /**< Windows Media Video (WMV1,WMV2,WMV3)*/
    MPP_VIDEO_CodingFLV1,               /**< Sorenson H.263 */
    MPP_VIDEO_CodingDIVX3,              /**< DIVX3 */
    MPP_VIDEO_CodingVP6,
    MPP_VIDEO_CodingHEVC,               /**< H.265/HEVC */
    MPP_VIDEO_CodingAVSPLUS,            /**< AVS+ */
    MPP_VIDEO_CodingAVS,                /**< AVS profile=0x20 */
    MPP_VIDEO_CodingAVS2,               /**< AVS2 */
    MPP_VIDEO_CodingAV1,                /**< av1 */
    MPP_VIDEO_CodingKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    MPP_VIDEO_CodingVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    MPP_VIDEO_CodingMax = 0x7FFFFFFF
} MppCodingType;

/*
 * All external interface object list here.
 * The interface object is defined as void * for expandability
 * The cross include between these objects will introduce extra
 * compiling difficulty. So we move them together in this header.
 *
 * Object interface header list:
 *
 * MppCtx           - rk_mpi.h
 * MppParam         - rk_mpi.h
 *
 * MppFrame         - mpp_frame.h
 * MppPacket        - mpp_packet.h
 *
 * MppBuffer        - mpp_buffer.h
 * MppBufferGroup   - mpp_buffer.h
 *
 * MppTask          - mpp_task.h
 * MppMeta          - mpp_meta.h
 */

typedef void* MppCtx;
typedef void* MppParam;

typedef void* MppFrame;
typedef void* MppPacket;

typedef void* MppBuffer;
typedef void* MppBufferGroup;

typedef void* MppTask;
typedef void* MppMeta;

#endif /*__RK_TYPE_H__*/
