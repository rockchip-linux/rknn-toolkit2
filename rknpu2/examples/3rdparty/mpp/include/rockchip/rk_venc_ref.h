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

#ifndef __RK_VENC_REF_H__
#define __RK_VENC_REF_H__

#include "rk_type.h"
#include "mpp_err.h"

/*
 * MPP reference management system follows the model of H.264/H.265 reference
 * frame mangement.
 *
 * The reference frame is defined into two type: long-term reference frame and
 * short-refernce frame (lt_ref and st_ref).
 *
 * The lt_ref can be only indexed by long-term reference frame index (lt_idx).
 * The st_ref can be indexed by its temporal id (tid) and previous count.
 *
 * MppEncRefMode defined the way for user to reference the required frame.
 *
 * Normal reference mode without argument
 *  REF_TO_PREV_REF_FRM   - refer to previous reference frame in encode order (No matter Lt or St)
 *  REF_TO_PREV_ST_REF    - refer to previous short-term reference frame
 *  REF_TO_PREV_LT_REF    - refer to previous long-term reference frame
 *  REF_TO_PREV_INTRA     - refer to previous Intra / IDR frame
 *  REF_TO_ST_REF_SETUP   - refer to refernce frame defined in StRefSetup
 *
 * Normal reference mode with argument
 *  REF_TO_TEMPORAL_LAYER - refer to previous reference frame with temporal id argument
 *  REF_TO_LT_REF_IDX     - refer to long-term reference frame with lt_ref_idx argument
 *  REF_TO_ST_PREV_N_REF    - refer to short-term reference frame with diff frame_num argument
 *
 * Long-term reference only mode
 *  REF_TO_ST_REF_SETUP   - use corresponding mode of original short-term reference frame
 *
 * Short-term reference only mode
 *  REF_TO_LT_REF_SETUP   - indicate that this frame will be overwrited by long-term config
 *
 * By combining frames with these modes user can define many kinds of reference hierarchy
 * structure. But normally user should use simplified preset hierarchy pattern.
 *
 * The rules for virtual cpb management is similiar to H.264/H.265
 * 1. When one frame is marked as long-term reference frame it will be kept in cpb until
 *    it is replaced by other frame with the same lt_idx or IDR frame.
 * 2. When one frame is marked as short-term reference frame it will be inert into cpb when
 *    there is enough storage space. When the number of total sum of long-term and short-term
 *    reference frame excess the cpb size limit the oldest short-term frame will be removed.
 *    This is call sliding window in H.264.
 */

/* max 4 temporal layer */
#define MPP_ENC_MAX_TEMPORAL_LAYER_NUM      4
/* max 4 long-term reference frame */
#define MPP_ENC_MAX_LT_REF_NUM              16

/*
 * Group Of Picture (GOP) config is separated into three parts:
 *
 * 1. Intra / IDR frame config
 *    igop  - the interval of two intra / IDR frames
 *
 * 2. Long-term reference config (MppEncRefLtFrmCfg)
 *
 *    Setup long-term reference index max lt_idx, loop interval and reference
 *    mode for auto long-term reference frame generation. The encoder will
 *    mark frame to be long-term reference frame with given interval.
 *
 *    2.1 lt_idx
 *    The long-term reference frame index is unique identifier for a long-term
 *    reference frame.
 *    The max long-term reference frame index should NOT larger than
 *    max_num_ref_frames in sps.
 *
 *    2.2 lt_gap
 *    When lt_gap is zero the long-term reference frame generation is disabled.
 *    When lt_gap is non-zero (usually 2~3 second interval) then the long-term
 *    reference frame will be generated for error recovery or smart hierarchy.
 *
 *    2.2 lt_delay
 *    The lt_delay is the delay time for generation of long-term reference frame.
 *    The start point of lt_delay is the IDR/intra frame genertaed by igop.
 *
 *    2.4 ref_mode: Long-term refernce frame reference mode
 *    NOTE: temporal id of longterm reference frame is always zero.
 *
 *    Examples:
 *    Sequence has only one lt_ref 0 and setup one long-term reference frame
 *    every 300 frame.
 *    {
 *    .lt_idx       = 0,
 *    .lt_gap       = 300,
 *    .lt_delay     = 0,
 *    }
 *    result:
 *    frame   0 ...... 299 300 301 ...... 599 600 601
 *    lt_idx  0 xxxxxx  x   0   x  xxxxxx  x   0   x
 *
 *    Sequence has lt_ref from 0 to 2 and setup a long-term reference frame
 *    every 100 frame.
 *    {
 *    .lt_idx       = 0,
 *    .lt_gap       = 300,
 *    .lt_delay     = 0,
 *    }
 *    {
 *    .lt_idx       = 1,
 *    .lt_gap       = 300,
 *    .lt_delay     = 100,
 *    }
 *    {
 *    .lt_idx       = 2,
 *    .lt_gap       = 300,
 *    .lt_delay     = 200,
 *    }
 *    result:
 *    frame   0 ... 99 100 101 ... 199 200 201 ... 299 300 301
 *    lt_idx  0 xxx  x  1   x  xxx  x   2   x  xxx  x   0   x
 *
 * 3. Short-term reference config (MppEncStRefSetup)
 *
 *    3.1 is_non_ref
 *    The is_non_ref indicated the current frame is reference frame or not.
 *
 *    3.2 temporal_id
 *    The temporal id of the current frame configure.
 *
 *    3.3 ref_mode: short-term refernce frame reference mode
 *
 *    3.4 repeat
 *    The repeat time of the short-term reference frame configure.
 *    The overall frame count with the same config is repeat + 1.
 *
 *    Examples:
 *
 */

#define REF_MODE_MODE_MASK              (0x1F)
#define REF_MODE_ARG_MASK               (0xFFFF0000)

typedef enum MppEncRefMode_e {
    /* max 32 mode in 32-bit */
    /* for default ref global config */
    REF_MODE_GLOBAL,
    REF_TO_PREV_REF_FRM                 = REF_MODE_GLOBAL,
    REF_TO_PREV_ST_REF,
    REF_TO_PREV_LT_REF,
    REF_TO_PREV_INTRA,

    /* for global config with args */
    REF_MODE_GLOBAL_WITH_ARG            = 0x4,
    /* with ref arg as temporal layer id */
    REF_TO_TEMPORAL_LAYER               = REF_MODE_GLOBAL_WITH_ARG,
    /* with ref arg as long-term reference picture index */
    REF_TO_LT_REF_IDX,
    /* with ref arg as short-term reference picture difference frame_num */
    REF_TO_ST_PREV_N_REF,
    REF_MODE_GLOBAL_BUTT,

    /* for lt-ref */
    REF_MODE_LT                         = 0x18,
    REF_TO_ST_REF_SETUP,
    REF_MODE_LT_BUTT,

    /* for st-ref */
    REF_MODE_ST                         = 0x1C,
    REF_TO_LT_REF_SETUP,
    REF_MODE_ST_BUTT,
} MppEncRefMode;

typedef struct MppEncRefLtFrmCfg_t {
    RK_S32              lt_idx;         /* lt_idx of the reference frame */
    RK_S32              temporal_id;    /* temporal_id of the reference frame */
    MppEncRefMode       ref_mode;
    RK_S32              ref_arg;
    RK_S32              lt_gap;         /* gap between two lt-ref with same lt_idx */
    RK_S32              lt_delay;       /* delay offset to igop start frame */
} MppEncRefLtFrmCfg;

typedef struct MppEncRefStFrmCfg_t {
    RK_S32              is_non_ref;
    RK_S32              temporal_id;
    MppEncRefMode       ref_mode;
    RK_S32              ref_arg;
    RK_S32              repeat;         /* repeat times */
} MppEncRefStFrmCfg;

typedef struct MppEncRefPreset_t {
    /* input parameter for query */
    const char          *name;
    RK_S32              max_lt_cnt;
    RK_S32              max_st_cnt;
    MppEncRefLtFrmCfg   *lt_cfg;
    MppEncRefStFrmCfg   *st_cfg;

    /* output parameter */
    RK_S32              lt_cnt;
    RK_S32              st_cnt;
} MppEncRefPreset;

typedef void* MppEncRefCfg;

#ifdef __cplusplus
extern "C" {
#endif

MPP_RET mpp_enc_ref_cfg_init(MppEncRefCfg *ref);
MPP_RET mpp_enc_ref_cfg_deinit(MppEncRefCfg *ref);

MPP_RET mpp_enc_ref_cfg_reset(MppEncRefCfg ref);
MPP_RET mpp_enc_ref_cfg_set_cfg_cnt(MppEncRefCfg ref, RK_S32 lt_cnt, RK_S32 st_cnt);
MPP_RET mpp_enc_ref_cfg_add_lt_cfg(MppEncRefCfg ref, RK_S32 cnt, MppEncRefLtFrmCfg *frm);
MPP_RET mpp_enc_ref_cfg_add_st_cfg(MppEncRefCfg ref, RK_S32 cnt, MppEncRefStFrmCfg *frm);
MPP_RET mpp_enc_ref_cfg_check(MppEncRefCfg ref);

/*
 * A new reference configure will restart a new gop and clear cpb by default.
 * The keep cpb function will let encoder keeps the current cpb status and do NOT
 * reset all the reference frame in cpb.
 */
MPP_RET mpp_enc_ref_cfg_set_keep_cpb(MppEncRefCfg ref, RK_S32 keep);
MPP_RET mpp_enc_ref_cfg_get_preset(MppEncRefPreset *preset);
MPP_RET mpp_enc_ref_cfg_show(MppEncRefCfg ref);

#ifdef __cplusplus
}
#endif

#endif /*__RK_VENC_REF_H__*/
