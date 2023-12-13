/*
 * Copyright 2016 Rockchip Electronics Co. LTD
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

#ifndef __MPP_RC_DEFS_H__
#define __MPP_RC_DEFS_H__

#include "rk_venc_ref.h"

#define MAX_CPB_REFS                    (8)

typedef enum EncFrmType_e {
    INTER_P_FRAME   = 0,
    INTER_B_FRAME   = 1,
    INTRA_FRAME     = 2,
    INTER_VI_FRAME  = 3,
    INTRA_RFH_FRAME = 4,
} EncFrmType;

/*
 * EncFrmStatus controls record the encoding frame status and also control
 * work flow of encoder. It is the communicat channel between encoder implement
 * module, rate control module and hardware module.
 *
 * bit  0 ~ 31 frame status
 *      0 ~ 15 current frame status
 *     16 ~ 31 reference frame status
 * bit 32 ~ 63 encoding flow control
 */
typedef union EncFrmStatus_u {
    struct {
        /*
         * bit  0 ~ 31  frame status
         */
        /* status flag */
        RK_U32          valid           : 1;
        /*
         * 0 - write the reconstructed frame pixel to memory
         * 1 - do not write the reconstructed frame pixel to memory
         */
        RK_U32          non_recn        : 1;

        /*
         * 0 - normal frame and normal dpb management
         * 1 - save recon frame as first pass extra frame. Used in two pass mode
         */
        RK_U32          save_pass1      : 1;

        /*
         * 0 - use normal input source frame as input
         * 1 - use the previously stored first pass recon frame as input frame
         */
        RK_U32          use_pass1       : 1;

        /* reference status flag */
        /*
         * 0 - inter frame
         * 1 - intra frame
         */
        RK_U32          is_intra        : 1;

        /*
         * Valid when is_intra is true
         * 0 - normal intra frame
         * 1 - IDR frame
         */
        RK_U32          is_idr          : 1;

        /*
         * 0 - mark as reference frame
         * 1 - mark as non-refernce frame
         */
        RK_U32          is_non_ref      : 1;

        /*
         * Valid when is_non_ref is false
         * 0 - mark as short-term reference frame
         * 1 - mark as long-term refernce frame
         */
        RK_U32          is_lt_ref       : 1;

        /* bit 8 - 15 */
        RK_U32          lt_idx          : 4;
        RK_U32          temporal_id     : 4;

        /* distance between current frame and reference frame */
        MppEncRefMode   ref_mode        : 6;
        RK_S32          ref_arg         : 8;
        RK_S32          ref_dist        : 2;

        /*
         * bit 32 ~ 63  encoder flow control flags
         */
        /*
         * 0 - normal frame encoding
         * 1 - current frame will be dropped
         */
        RK_U32          drop            : 1;

        /*
         * 0 - rate control module does not change frame type parameter
         * 1 - rate control module changes frame type parameter reencode is needed
         *     to reprocess the dpb process. Also this means dpb module will follow
         *     the frame status parameter provided by rate control module.
         */
        RK_U32          re_dpb_proc     : 1;

        /*
         * 0 - current frame encoding is in normal flow
         * 1 - current frame encoding is in reencode flow
         */
        RK_U32          reencode        : 1;

        /*
         * When true current frame size is super large then the frame should be reencoded.
         */
        RK_U32          super_frame     : 1;

        /*
         * When true currnet frame is force to encoded as software skip frame
         */
        RK_U32          force_pskip     : 1;

        /*
         * Current frame is intra refresh frame
         */
        RK_U32          is_i_refresh    : 1;
        /*
         * Current frame needs add recovery point prefix
         */
        RK_U32          is_i_recovery   : 1;
        RK_U32          reserved1       : 1;

        /* reencode times */
        RK_U32          reencode_times  : 8;

        /* sequential index for each frame */
        RK_U32          seq_idx         : 16;
    };
    RK_U64 val;
} EncFrmStatus;

typedef struct EncCpbStatus_t {
    RK_S32              seq_idx;

    EncFrmStatus        curr;
    EncFrmStatus        refr;

    /* initial cpb status for current frame encoding */
    EncFrmStatus        init[MAX_CPB_REFS];
    /* final cpb status after current frame encoding */
    EncFrmStatus        final[MAX_CPB_REFS];
} EncCpbStatus;

#define ENC_RC_FORCE_QP                 (0x00000001)

typedef struct EncRcForceCfg_t {
    RK_U32              force_flag;
    RK_S32              force_qp;
    RK_U32              reserve[6];
} EncRcForceCfg;

/*
 * communication channel between rc / hal / hardware
 *
 * rc   -> hal      bit_target / bit_max / bit_min
 * hal  -> hw       quality_target / quality_max / quality_min
 * hw   -> rc / hal bit_real / quality_real / madi / madp
 */
typedef struct EncRcCommonInfo_t {
    EncFrmType      frame_type;

    /* rc to hal */
    RK_S32          bit_target;
    RK_S32          bit_max;
    RK_S32          bit_min;

    RK_S32          quality_target;
    RK_S32          quality_max;
    RK_S32          quality_min;

    /* rc from hardware */
    RK_S32          bit_real;
    RK_S32          quality_real;
    RK_S32          madi;
    RK_S32          madp;

    RK_U32          iblk4_prop; // scale 256
    RK_S32          reserve[15];
} EncRcTaskInfo;

typedef struct EncRcTask_s {
    EncCpbStatus    cpb;
    EncFrmStatus    frm;
    EncRcTaskInfo   info;
    EncRcForceCfg   force;
    MppFrame        frame;
} EncRcTask;

#endif /* __MPP_RC_DEFS_H__ */
