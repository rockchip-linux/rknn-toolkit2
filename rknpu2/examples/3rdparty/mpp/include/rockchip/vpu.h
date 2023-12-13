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

#ifndef __VPU_H__
#define __VPU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "rk_type.h"

#define VPU_SUCCESS                     (0)
#define VPU_FAILURE                     (-1)

#define VPU_HW_WAIT_OK                  VPU_SUCCESS
#define VPU_HW_WAIT_ERROR               VPU_FAILURE
#define VPU_HW_WAIT_TIMEOUT             1

// vpu decoder 60 registers, size 240B
#define VPU_REG_NUM_DEC                 (60)
// vpu post processor 41 registers, size 164B
#define VPU_REG_NUM_PP                  (41)
// vpu decoder + post processor 101 registers, size 404B
#define VPU_REG_NUM_DEC_PP              (VPU_REG_NUM_DEC+VPU_REG_NUM_PP)
// vpu encoder 96 registers, size 384B
#define VPU_REG_NUM_ENC                 (96)

typedef enum {
    VPU_ENC                 = 0x0,
    VPU_DEC                 = 0x1,
    VPU_PP                  = 0x2,
    VPU_DEC_PP              = 0x3,
    VPU_DEC_HEVC            = 0x4,
    VPU_DEC_RKV             = 0x5,
    VPU_ENC_RKV             = 0x6,
    VPU_DEC_AVSPLUS         = 0x7,
    VPU_ENC_VEPU22          = 0x8,
    VPU_TYPE_BUTT           ,
} VPU_CLIENT_TYPE;

/* Hardware decoder configuration description */

typedef struct VPUHwDecConfig {
    RK_U32 maxDecPicWidth;         /* Maximum video decoding width supported  */
    RK_U32 maxPpOutPicWidth;       /* Maximum output width of Post-Processor */
    RK_U32 h264Support;            /* HW supports h.264 */
    RK_U32 jpegSupport;            /* HW supports JPEG */
    RK_U32 mpeg4Support;           /* HW supports MPEG-4 */
    RK_U32 customMpeg4Support;     /* HW supports custom MPEG-4 features */
    RK_U32 vc1Support;             /* HW supports VC-1 Simple */
    RK_U32 mpeg2Support;           /* HW supports MPEG-2 */
    RK_U32 ppSupport;              /* HW supports post-processor */
    RK_U32 ppConfig;               /* HW post-processor functions bitmask */
    RK_U32 sorensonSparkSupport;   /* HW supports Sorenson Spark */
    RK_U32 refBufSupport;          /* HW supports reference picture buffering */
    RK_U32 vp6Support;             /* HW supports VP6 */
    RK_U32 vp7Support;             /* HW supports VP7 */
    RK_U32 vp8Support;             /* HW supports VP8 */
    RK_U32 avsSupport;             /* HW supports AVS */
    RK_U32 jpegESupport;           /* HW supports JPEG extensions */
    RK_U32 rvSupport;              /* HW supports REAL */
    RK_U32 mvcSupport;             /* HW supports H264 MVC extension */
} VPUHwDecConfig_t;

/* Hardware encoder configuration description */

typedef struct VPUHwEndConfig {
    RK_U32 maxEncodedWidth;        /* Maximum supported width for video encoding (not JPEG) */
    RK_U32 h264Enabled;            /* HW supports H.264 */
    RK_U32 jpegEnabled;            /* HW supports JPEG */
    RK_U32 mpeg4Enabled;           /* HW supports MPEG-4 */
    RK_U32 vsEnabled;              /* HW supports video stabilization */
    RK_U32 rgbEnabled;             /* HW supports RGB input */
    RK_U32 reg_size;                /* HW bus type in use */
    RK_U32 reserv[2];
} VPUHwEncConfig_t;

typedef enum {
    // common command
    VPU_CMD_REGISTER            ,
    VPU_CMD_REGISTER_ACK_OK     ,
    VPU_CMD_REGISTER_ACK_FAIL   ,
    VPU_CMD_UNREGISTER          ,

    VPU_SEND_CONFIG             ,
    VPU_SEND_CONFIG_ACK_OK      ,
    VPU_SEND_CONFIG_ACK_FAIL    ,

    VPU_GET_HW_INFO             ,
    VPU_GET_HW_INFO_ACK_OK      ,
    VPU_GET_HW_INFO_ACK_FAIL    ,

    VPU_CMD_BUTT                ,
} VPU_CMD_TYPE;

int VPUClientInit(VPU_CLIENT_TYPE type);
RK_S32 VPUClientRelease(int socket);
RK_S32 VPUClientSendReg(int socket, RK_U32 *regs, RK_U32 nregs);
RK_S32 VPUClientSendReg2(RK_S32 socket, RK_S32 offset, RK_S32 size, void *param);
RK_S32 VPUClientWaitResult(int socket, RK_U32 *regs, RK_U32 nregs, VPU_CMD_TYPE *cmd, RK_S32 *len);
RK_S32 VPUClientGetHwCfg(int socket, RK_U32 *cfg, RK_U32 cfg_size);
RK_S32 VPUClientGetIOMMUStatus();
RK_U32 VPUCheckSupportWidth();

#ifdef __cplusplus
}
#endif

#endif /* __VPU_H__ */
