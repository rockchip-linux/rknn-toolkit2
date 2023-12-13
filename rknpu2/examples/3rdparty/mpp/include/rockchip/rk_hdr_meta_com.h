/*
 * Copyright 2022 Rockchip Electronics Co. LTD
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

#ifndef __RK_HDR_META_COM_H__
#define __RK_HDR_META_COM_H__

#include "rk_type.h"

typedef enum HdrCodecType_e {
    HDR_AVS2 = 0,
    HDR_HEVC = 1,
    HDR_H264 = 2,
    HDR_AV1  = 3,
    HDR_CODEC_BUT,
} HdrCodecType;

typedef enum HdrFormat_e {
    HDR_NONE    = 0,
    HDR10       = 1,
    HLG         = 2,
//  RESERVED3 = 3, //reserved for more future static hdr format
//  RESERVED4 = 4, //reserved for more future static hdr format
    HDRVIVID    = 5,
//  RESERVED6 = 6, //reserved for hdr vivid
//  RESERVED7 = 7, //reserved for hdr vivid
    HDR10PLUS   = 8,
//  RESERVED9 = 9, //reserved for hdr10+
//  RESERVED10 = 10,//reserved for hdr10+
    DOLBY       = 11,
//  RESERVED12 = 12, //reserved for other dynamic hdr format
//  RESERVED13 = 13, //reserved for  other dynamic hdr format
    HDR_FORMAT_MAX,
} HdrFormat;

typedef enum HdrPayloadFormat_e {
    STATIC = 0,
    DYNAMIC = 1,
    HDR_PAYLOAD_FORMAT_MAX,
} HdrPayloadFormat;

typedef struct HdrStaticMeta_t {
    RK_U32  color_space;
    RK_U32  color_primaries;
    RK_U32  color_trc;
    RK_U32  red_x;
    RK_U32  red_y;
    RK_U32  green_x;
    RK_U32  green_y;
    RK_U32  blue_x;
    RK_U32  blue_y;
    RK_U32  white_point_x;
    RK_U32  white_point_y;
    RK_U32  min_luminance;
    RK_U32  max_luminance;
    RK_U32  max_cll;
    RK_U32  max_fall;
    RK_U32  reserved[4];
} HdrStaticMeta;

/*
 * HDR metadata format from codec
 *
 *  +----------+
 *  |  header1 |
 *  +----------+
 *  |          |
 *  |  payload |
 *  |          |
 *  +----------+
 *  |  header2 |
 *  +----------+
 *  |          |
 *  |  payload |
 *  |          |
 *  +----------+
 *  |  header3 |
 *  +----------+
 *  |          |
 *  |  payload |
 *  |          |
 *  +----------+
 */
typedef struct RkMetaHdrHeader_t {
    /* For transmission */
    RK_U16  magic;              /* magic word for checking overwrite error      */
    RK_U16  size;               /* total header+payload length including header */
    RK_U16  message_total;      /* total message count in current transmission  */
    RK_U16  message_index;      /* current message index in the transmission    */

    /* For payload identification */
    RK_U16  version;            /* payload structure version                    */
    RK_U16  hdr_format;         /* HDR protocol: HDR10, HLG, Dolby, HDRVivid ...    */
    RK_U16  hdr_payload_type;   /* HDR data type: static data, dynamic data ... */
    RK_U16  video_format;       /* video format: H.264, H.265, AVS2 ...         */

    /* For extenstion usage */
    RK_U32  reserve[4];

    /* payload data aligned to 32bits */
    RK_U32  payload[];
} RkMetaHdrHeader;

void fill_hdr_meta_to_frame(MppFrame frame, HdrCodecType codec_type);

#endif
