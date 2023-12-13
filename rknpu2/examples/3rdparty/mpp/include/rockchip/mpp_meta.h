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

#ifndef __MPP_META_H__
#define __MPP_META_H__

#include <stdint.h>
#include "rk_type.h"

#define FOURCC_META(a, b, c, d) ((RK_U32)(a) << 24  | \
                                ((RK_U32)(b) << 16) | \
                                ((RK_U32)(c) << 8)  | \
                                ((RK_U32)(d) << 0))

/*
 * Mpp Metadata definition
 *
 * Metadata is for information transmision in mpp.
 * Mpp task will contain two meta data:
 *
 * 1. Data flow metadata
 *    This metadata contains information of input / output data flow. For example
 *    A. decoder input side task the input packet must be defined and output frame
 *    may not be defined. Then decoder will try malloc or use committed buffer to
 *    complete decoding.
 *    B. decoder output side task
 *
 *
 * 2. Flow control metadata
 *
 */
typedef enum MppMetaDataType_e {
    /*
     * mpp meta data of data flow
     * reference counter will be used for these meta data type
     */
    TYPE_FRAME                  = FOURCC_META('m', 'f', 'r', 'm'),
    TYPE_PACKET                 = FOURCC_META('m', 'p', 'k', 't'),
    TYPE_BUFFER                 = FOURCC_META('m', 'b', 'u', 'f'),

    /* mpp meta data of normal data type */
    TYPE_S32                    = FOURCC_META('s', '3', '2', ' '),
    TYPE_S64                    = FOURCC_META('s', '6', '4', ' '),
    TYPE_PTR                    = FOURCC_META('p', 't', 'r', ' '),
} MppMetaType;

typedef enum MppMetaKey_e {
    /* data flow key */
    KEY_INPUT_FRAME             = FOURCC_META('i', 'f', 'r', 'm'),
    KEY_INPUT_PACKET            = FOURCC_META('i', 'p', 'k', 't'),
    KEY_OUTPUT_FRAME            = FOURCC_META('o', 'f', 'r', 'm'),
    KEY_OUTPUT_PACKET           = FOURCC_META('o', 'p', 'k', 't'),
    /* output motion information for motion detection */
    KEY_MOTION_INFO             = FOURCC_META('m', 'v', 'i', 'f'),
    KEY_HDR_INFO                = FOURCC_META('h', 'd', 'r', ' '),
    KEY_HDR_META_OFFSET         = FOURCC_META('h', 'd', 'r', 'o'),
    KEY_HDR_META_SIZE           = FOURCC_META('h', 'd', 'r', 'l'),

    /* flow control key */
    KEY_INPUT_BLOCK             = FOURCC_META('i', 'b', 'l', 'k'),
    KEY_OUTPUT_BLOCK            = FOURCC_META('o', 'b', 'l', 'k'),
    KEY_INPUT_IDR_REQ           = FOURCC_META('i', 'i', 'd', 'r'),   /* input idr frame request flag */
    KEY_OUTPUT_INTRA            = FOURCC_META('o', 'i', 'd', 'r'),   /* output intra frame indicator */

    /* mpp_frame / mpp_packet meta data info key */
    KEY_TEMPORAL_ID             = FOURCC_META('t', 'l', 'i', 'd'),
    KEY_LONG_REF_IDX            = FOURCC_META('l', 't', 'i', 'd'),
    KEY_ENC_AVERAGE_QP          = FOURCC_META('a', 'v', 'g', 'q'),
    KEY_ROI_DATA                = FOURCC_META('r', 'o', 'i', ' '),
    KEY_OSD_DATA                = FOURCC_META('o', 's', 'd', ' '),
    KEY_OSD_DATA2               = FOURCC_META('o', 's', 'd', '2'),
    KEY_USER_DATA               = FOURCC_META('u', 's', 'r', 'd'),
    KEY_USER_DATAS              = FOURCC_META('u', 'r', 'd', 's'),

    /*
     * For vepu580 roi buffer config mode
     * The encoder roi structure is so complex that we should provide a buffer
     * tunnel for externl user to config encoder hardware by direct sending
     * roi data buffer.
     * This way can reduce the config parsing and roi buffer data generating
     * overhead in mpp.
     */
    KEY_ROI_DATA2               = FOURCC_META('r', 'o', 'i', '2'),

    /*
     * qpmap for rv1109/1126 encoder qpmap config
     * Input data is a MppBuffer which contains an array of 16bit Vepu541RoiCfg.
     * And each 16bit represents a 16x16 block qp info.
     *
     * H.264 - 16x16 block qp is arranged in raster order:
     * each value is a 16bit data
     * 00 01 02 03 04 05 06 07 -> 00 01 02 03 04 05 06 07
     * 10 11 12 13 14 15 16 17    10 11 12 13 14 15 16 17
     * 20 21 22 23 24 25 26 27    20 21 22 23 24 25 26 27
     * 30 31 32 33 34 35 36 37    30 31 32 33 34 35 36 37
     *
     * H.265 - 16x16 block qp is reorder to 64x64/32x32 ctu order then 64x64 / 32x32 ctu raster order
     * 64x64 ctu
     * 00 01 02 03 04 05 06 07 -> 00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33 04 05 06 07 14 15 16 17 24 25 26 27 34 35 36 37
     * 10 11 12 13 14 15 16 17
     * 20 21 22 23 24 25 26 27
     * 30 31 32 33 34 35 36 37
     * 32x32 ctu
     * 00 01 02 03 04 05 06 07 -> 00 01 10 11 02 03 12 13 04 05 14 15 06 07 16 17
     * 10 11 12 13 14 15 16 17    20 21 30 31 22 23 32 33 24 25 34 35 26 27 36 37
     * 20 21 22 23 24 25 26 27
     * 30 31 32 33 34 35 36 37
     */
    KEY_QPMAP0                  = FOURCC_META('e', 'q', 'm', '0'),

    /* input motion list for smart p rate control */
    KEY_MV_LIST                 = FOURCC_META('m', 'v', 'l', 't'),

    /* frame long-term reference frame operation */
    KEY_ENC_MARK_LTR            = FOURCC_META('m', 'l', 't', 'r'),
    KEY_ENC_USE_LTR             = FOURCC_META('u', 'l', 't', 'r'),

    /* MLVEC specified encoder feature  */
    KEY_ENC_FRAME_QP            = FOURCC_META('f', 'r', 'm', 'q'),
    KEY_ENC_BASE_LAYER_PID      = FOURCC_META('b', 'p', 'i', 'd'),

    /* Thumbnail info for decoder output frame */
    KEY_DEC_TBN_EN              = FOURCC_META('t', 'b', 'e', 'n'),
    KEY_DEC_TBN_Y_OFFSET        = FOURCC_META('t', 'b', 'y', 'o'),
    KEY_DEC_TBN_UV_OFFSET       = FOURCC_META('t', 'b', 'c', 'o'),
} MppMetaKey;

#define mpp_meta_get(meta) mpp_meta_get_with_tag(meta, MODULE_TAG, __FUNCTION__)

#include "mpp_frame.h"
#include "mpp_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

MPP_RET mpp_meta_get_with_tag(MppMeta *meta, const char *tag, const char *caller);
MPP_RET mpp_meta_put(MppMeta meta);
RK_S32  mpp_meta_size(MppMeta meta);

MPP_RET mpp_meta_set_s32(MppMeta meta, MppMetaKey key, RK_S32 val);
MPP_RET mpp_meta_set_s64(MppMeta meta, MppMetaKey key, RK_S64 val);
MPP_RET mpp_meta_set_ptr(MppMeta meta, MppMetaKey key, void  *val);
MPP_RET mpp_meta_get_s32(MppMeta meta, MppMetaKey key, RK_S32 *val);
MPP_RET mpp_meta_get_s64(MppMeta meta, MppMetaKey key, RK_S64 *val);
MPP_RET mpp_meta_get_ptr(MppMeta meta, MppMetaKey key, void  **val);

MPP_RET mpp_meta_set_frame (MppMeta meta, MppMetaKey key, MppFrame  frame);
MPP_RET mpp_meta_set_packet(MppMeta meta, MppMetaKey key, MppPacket packet);
MPP_RET mpp_meta_set_buffer(MppMeta meta, MppMetaKey key, MppBuffer buffer);
MPP_RET mpp_meta_get_frame (MppMeta meta, MppMetaKey key, MppFrame  *frame);
MPP_RET mpp_meta_get_packet(MppMeta meta, MppMetaKey key, MppPacket *packet);
MPP_RET mpp_meta_get_buffer(MppMeta meta, MppMetaKey key, MppBuffer *buffer);

MPP_RET mpp_meta_get_s32_d(MppMeta meta, MppMetaKey key, RK_S32 *val, RK_S32 def);
MPP_RET mpp_meta_get_s64_d(MppMeta meta, MppMetaKey key, RK_S64 *val, RK_S64 def);
MPP_RET mpp_meta_get_ptr_d(MppMeta meta, MppMetaKey key, void  **val, void *def);
MPP_RET mpp_meta_get_frame_d(MppMeta meta, MppMetaKey key, MppFrame *frame, MppFrame def);
MPP_RET mpp_meta_get_packet_d(MppMeta meta, MppMetaKey key, MppPacket *packet, MppPacket def);
MPP_RET mpp_meta_get_buffer_d(MppMeta meta, MppMetaKey key, MppBuffer *buffer, MppBuffer def);

#ifdef __cplusplus
}
#endif

#endif /*__MPP_META_H__*/
