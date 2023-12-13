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

#ifndef __MPP_PACKET_H__
#define __MPP_PACKET_H__

#include "mpp_meta.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MppPacket interface
 *
 * mpp_packet_init = mpp_packet_new + mpp_packet_set_data + mpp_packet_set_size
 * mpp_packet_copy_init = mpp_packet_init + memcpy
 */
MPP_RET mpp_packet_new(MppPacket *packet);
MPP_RET mpp_packet_init(MppPacket *packet, void *data, size_t size);
MPP_RET mpp_packet_init_with_buffer(MppPacket *packet, MppBuffer buffer);
MPP_RET mpp_packet_copy_init(MppPacket *packet, const MppPacket src);
MPP_RET mpp_packet_deinit(MppPacket *packet);

/*
 * data   : ( R/W ) start address of the whole packet memory
 * size   : ( R/W ) total size of the whole packet memory
 * pos    : ( R/W ) current access position of the whole packet memory, used for buffer read/write
 * length : ( R/W ) the rest length from current position to end of buffer
 *                  NOTE: normally length is updated only by set_pos,
 *                        so set length must be used carefully for special usage
 */
void    mpp_packet_set_data(MppPacket packet, void *data);
void    mpp_packet_set_size(MppPacket packet, size_t size);
void    mpp_packet_set_pos(MppPacket packet, void *pos);
void    mpp_packet_set_length(MppPacket packet, size_t size);

void*   mpp_packet_get_data(const MppPacket packet);
void*   mpp_packet_get_pos(const MppPacket packet);
size_t  mpp_packet_get_size(const MppPacket packet);
size_t  mpp_packet_get_length(const MppPacket packet);


void    mpp_packet_set_pts(MppPacket packet, RK_S64 pts);
RK_S64  mpp_packet_get_pts(const MppPacket packet);
void    mpp_packet_set_dts(MppPacket packet, RK_S64 dts);
RK_S64  mpp_packet_get_dts(const MppPacket packet);

void    mpp_packet_set_flag(MppPacket packet, RK_U32 flag);
RK_U32  mpp_packet_get_flag(const MppPacket packet);

MPP_RET mpp_packet_set_eos(MppPacket packet);
MPP_RET mpp_packet_clr_eos(MppPacket packet);
RK_U32  mpp_packet_get_eos(MppPacket packet);
MPP_RET mpp_packet_set_extra_data(MppPacket packet);

void        mpp_packet_set_buffer(MppPacket packet, MppBuffer buffer);
MppBuffer   mpp_packet_get_buffer(const MppPacket packet);

/*
 * data access interface
 */
MPP_RET mpp_packet_read(MppPacket packet, size_t offset, void *data, size_t size);
MPP_RET mpp_packet_write(MppPacket packet, size_t offset, void *data, size_t size);

/*
 * meta data access interface
 */
RK_S32  mpp_packet_has_meta(const MppPacket packet);
MppMeta mpp_packet_get_meta(const MppPacket packet);

/*
 * multi packet sequence interface for slice/split encoding/decoding
 * partition - the packet is a part of a while image
 * soi - Start Of Image
 * eoi - End Of Image
 */
RK_U32  mpp_packet_is_partition(const MppPacket packet);
RK_U32  mpp_packet_is_soi(const MppPacket packet);
RK_U32  mpp_packet_is_eoi(const MppPacket packet);

/*
 * packet segement pack info for
 * segment number - number of segment
 * segment info   - base address of segment info
 */
typedef struct MppPktSeg_t MppPktSeg;

struct MppPktSeg_t {
    RK_S32          index;
    RK_S32          type;
    RK_U32          offset;
    RK_U32          len;
    const MppPktSeg *next;
};

RK_U32 mpp_packet_get_segment_nb(const MppPacket packet);
const MppPktSeg *mpp_packet_get_segment_info(const MppPacket packet);

#ifdef __cplusplus
}
#endif

#endif /*__MPP_PACKET_H__*/
