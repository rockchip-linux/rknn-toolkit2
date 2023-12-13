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

#ifndef __RK_VENC_CMD_H__
#define __RK_VENC_CMD_H__

#include "mpp_frame.h"
#include "rk_venc_rc.h"

/*
 * Configure of encoder is very complicated. So we divide configures into
 * four parts:
 *
 * 1. Rate control parameter
 *    This is quality and bitrate request from user.
 *
 * 2. Data source MppFrame parameter
 *    This is data source buffer information.
 *    Now it is PreP config
 *    PreP  : Encoder Preprocess configuration
 *
 * 3. Video codec infomation
 *    This is user custormized stream information.
 *    including:
 *    H.264 / H.265 / vp8 / mjpeg
 *
 * 4. Misc parameter
 *    including:
 *    Split : Slice split configuration
 *    GopRef: Reference gop configuration
 *    ROI   : Region Of Interest
 *    OSD   : On Screen Display
 *    MD    : Motion Detection
 *
 * The module transcation flow is as follows:
 *
 *                 +                      +
 *     User        |      Mpi/Mpp         |          EncImpl
 *                 |                      |            Hal
 *                 |                      |
 * +----------+    |    +---------+       |       +-----------+
 * |          |    |    |         +-----RcCfg----->           |
 * |  RcCfg   +--------->         |       |       |  EncImpl  |
 * |          |    |    |         |   +-Frame----->           |
 * +----------+    |    |         |   |   |       +--+-----^--+
 *                 |    |         |   |   |          |     |
 *                 |    |         |   |   |          |     |
 * +----------+    |    |         |   |   |       syntax   |
 * |          |    |    |         |   |   |          |     |
 * | MppFrame +--------->  MppEnc +---+   |          |   result
 * |          |    |    |         |   |   |          |     |
 * +----------+    |    |         |   |   |          |     |
 *                 |    |         |   |   |       +--v-----+--+
 *                 |    |         |   +-Frame----->           |
 * +----------+    |    |         |       |       |           |
 * |          |    |    |         +---CodecCfg---->    Hal    |
 * | CodecCfg +--------->         |       |       |           |
 * |          |    |    |         <-----Extra----->           |
 * +----------+    |    +---------+       |       +-----------+
 *                 |                      |
 *                 |                      |
 *                 +                      +
 *
 * The function call flow is shown below:
 *
 *  mpi                      mpp_enc         controller                  hal
 *   +                          +                 +                       +
 *   |                          |                 |                       |
 *   |                          |                 |                       |
 *   +----------init------------>                 |                       |
 *   |                          |                 |                       |
 *   |                          |                 |                       |
 *   |         PrepCfg          |                 |                       |
 *   +---------control---------->     PrepCfg     |                       |
 *   |                          +-----control----->                       |
 *   |                          |                 |        PrepCfg        |
 *   |                          +--------------------------control-------->
 *   |                          |                 |                    allocate
 *   |                          |                 |                     buffer
 *   |                          |                 |                       |
 *   |          RcCfg           |                 |                       |
 *   +---------control---------->      RcCfg      |                       |
 *   |                          +-----control----->                       |
 *   |                          |              rc_init                    |
 *   |                          |                 |                       |
 *   |                          |                 |                       |
 *   |         CodecCfg         |                 |                       |
 *   +---------control---------->                 |        CodecCfg       |
 *   |                          +--------------------------control-------->
 *   |                          |                 |                    generate
 *   |                          |                 |                    sps/pps
 *   |                          |                 |     Get extra info    |
 *   |                          +--------------------------control-------->
 *   |      Get extra info      |                 |                       |
 *   +---------control---------->                 |                       |
 *   |                          |                 |                       |
 *   |                          |                 |                       |
 *   |         ROICfg           |                 |                       |
 *   +---------control---------->                 |        ROICfg         |
 *   |                          +--------------------------control-------->
 *   |                          |                 |                       |
 *   |         OSDCfg           |                 |                       |
 *   +---------control---------->                 |        OSDCfg         |
 *   |                          +--------------------------control-------->
 *   |                          |                 |                       |
 *   |          MDCfg           |                 |                       |
 *   +---------control---------->                 |         MDCfg         |
 *   |                          +--------------------------control-------->
 *   |                          |                 |                       |
 *   |      Set extra info      |                 |                       |
 *   +---------control---------->                 |     Set extra info    |
 *   |                          +--------------------------control-------->
 *   |                          |                 |                       |
 *   |           task           |                 |                       |
 *   +----------encode---------->      task       |                       |
 *   |                          +-----encode------>                       |
 *   |                          |              encode                     |
 *   |                          |                 |        syntax         |
 *   |                          +--------------------------gen_reg-------->
 *   |                          |                 |                       |
 *   |                          |                 |                       |
 *   |                          +---------------------------start--------->
 *   |                          |                 |                       |
 *   |                          |                 |                       |
 *   |                          +---------------------------wait---------->
 *   |                          |                 |                       |
 *   |                          |    callback     |                       |
 *   |                          +----------------->                       |
 *   +--OSD-MD--encode---------->                 |                       |
 *   |             .            |                 |                       |
 *   |             .            |                 |                       |
 *   |             .            |                 |                       |
 *   +--OSD-MD--encode---------->                 |                       |
 *   |                          |                 |                       |
 *   +----------deinit---------->                 |                       |
 *   +                          +                 +                       +
 */

/*
 * encoder query interface is only for debug usage
 */
#define MPP_ENC_QUERY_STATUS        (0x00000001)
#define MPP_ENC_QUERY_WAIT          (0x00000002)
#define MPP_ENC_QUERY_FPS           (0x00000004)
#define MPP_ENC_QUERY_BPS           (0x00000008)
#define MPP_ENC_QUERY_ENC_IN_FRM    (0x00000010)
#define MPP_ENC_QUERY_ENC_WORK      (0x00000020)
#define MPP_ENC_QUERY_ENC_OUT_PKT   (0x00000040)

#define MPP_ENC_QUERY_ALL           (MPP_ENC_QUERY_STATUS       | \
                                     MPP_ENC_QUERY_WAIT         | \
                                     MPP_ENC_QUERY_FPS          | \
                                     MPP_ENC_QUERY_BPS          | \
                                     MPP_ENC_QUERY_ENC_IN_FRM   | \
                                     MPP_ENC_QUERY_ENC_WORK     | \
                                     MPP_ENC_QUERY_ENC_OUT_PKT)

typedef struct MppEncQueryCfg_t {
    /*
     * 32 bit query flag for query data check
     * Each bit represent a query data switch.
     * bit 0 - for querying encoder runtime status
     * bit 1 - for querying encoder runtime waiting status
     * bit 2 - for querying encoder realtime encode fps
     * bit 3 - for querying encoder realtime output bps
     * bit 4 - for querying encoder input frame count
     * bit 5 - for querying encoder start hardware times
     * bit 6 - for querying encoder output packet count
     */
    RK_U32      query_flag;

    /* 64 bit query data output */
    RK_U32      rt_status;
    RK_U32      rt_wait;
    RK_U32      rt_fps;
    RK_U32      rt_bps;
    RK_U32      enc_in_frm_cnt;
    RK_U32      enc_hw_run_cnt;
    RK_U32      enc_out_pkt_cnt;
} MppEncQueryCfg;

/*
 * base working mode parameter
 */
typedef enum MppEncBaseCfgChange_e {
    MPP_ENC_BASE_CFG_CHANGE_LOW_DELAY   = (1 << 0),
    MPP_ENC_BASE_CFG_CHANGE_ALL         = (0xFFFFFFFF),
} MppEncBaseCfgChange;

typedef struct MppEncBaseCfg_t {
    RK_U32  change;

    RK_S32  low_delay;
} MppEncBaseCfg;

/*
 * Rate control parameter
 */
typedef enum MppEncRcCfgChange_e {
    MPP_ENC_RC_CFG_CHANGE_RC_MODE       = (1 << 0),
    MPP_ENC_RC_CFG_CHANGE_QUALITY       = (1 << 1),
    MPP_ENC_RC_CFG_CHANGE_BPS           = (1 << 2),     /* change on bps target / max / min */
    MPP_ENC_RC_CFG_CHANGE_FPS_IN        = (1 << 5),     /* change on fps in  flex / numerator / denorminator */
    MPP_ENC_RC_CFG_CHANGE_FPS_OUT       = (1 << 6),     /* change on fps out flex / numerator / denorminator */
    MPP_ENC_RC_CFG_CHANGE_GOP           = (1 << 7),
    MPP_ENC_RC_CFG_CHANGE_SKIP_CNT      = (1 << 8),
    MPP_ENC_RC_CFG_CHANGE_MAX_REENC     = (1 << 9),
    MPP_ENC_RC_CFG_CHANGE_DROP_FRM      = (1 << 10),
    MPP_ENC_RC_CFG_CHANGE_MAX_I_PROP    = (1 << 11),
    MPP_ENC_RC_CFG_CHANGE_MIN_I_PROP    = (1 << 12),
    MPP_ENC_RC_CFG_CHANGE_INIT_IP_RATIO = (1 << 13),
    MPP_ENC_RC_CFG_CHANGE_PRIORITY      = (1 << 14),
    MPP_ENC_RC_CFG_CHANGE_SUPER_FRM     = (1 << 15),
    /* qp related change flag */
    MPP_ENC_RC_CFG_CHANGE_QP_INIT       = (1 << 16),
    MPP_ENC_RC_CFG_CHANGE_QP_RANGE      = (1 << 17),
    MPP_ENC_RC_CFG_CHANGE_QP_RANGE_I    = (1 << 18),
    MPP_ENC_RC_CFG_CHANGE_QP_MAX_STEP   = (1 << 19),
    MPP_ENC_RC_CFG_CHANGE_QP_IP         = (1 << 20),
    MPP_ENC_RC_CFG_CHANGE_QP_VI         = (1 << 21),
    MPP_ENC_RC_CFG_CHANGE_QP_ROW        = (1 << 22),
    MPP_ENC_RC_CFG_CHANGE_QP_ROW_I      = (1 << 23),
    MPP_ENC_RC_CFG_CHANGE_DEBREATH      = (1 << 24),
    MPP_ENC_RC_CFG_CHANGE_HIER_QP       = (1 << 25),
    MPP_ENC_RC_CFG_CHANGE_ST_TIME       = (1 << 26),
    MPP_ENC_RC_CFG_CHANGE_REFRESH       = (1 << 27),
    MPP_ENC_RC_CFG_CHANGE_GOP_REF_CFG   = (1 << 28),
    MPP_ENC_RC_CFG_CHANGE_ALL           = (0xFFFFFFFF),
} MppEncRcCfgChange;

typedef enum MppEncRcQuality_e {
    MPP_ENC_RC_QUALITY_WORST,
    MPP_ENC_RC_QUALITY_WORSE,
    MPP_ENC_RC_QUALITY_MEDIUM,
    MPP_ENC_RC_QUALITY_BETTER,
    MPP_ENC_RC_QUALITY_BEST,
    MPP_ENC_RC_QUALITY_CQP,
    MPP_ENC_RC_QUALITY_AQ_ONLY,
    MPP_ENC_RC_QUALITY_BUTT
} MppEncRcQuality;

typedef struct MppEncRcCfg_t {
    RK_U32  change;

    /*
     * rc_mode - rate control mode
     *
     * mpp provide two rate control mode:
     *
     * Constant Bit Rate (CBR) mode
     * - paramter 'bps*' define target bps
     * - paramter quality and qp will not take effect
     *
     * Variable Bit Rate (VBR) mode
     * - paramter 'quality' define 5 quality levels
     * - paramter 'bps*' is used as reference but not strict condition
     * - special Constant QP (CQP) mode is under VBR mode
     *   CQP mode will work with qp in CodecCfg. But only use for test
     *
     * default: CBR
     */
    MppEncRcMode rc_mode;

    /*
     * quality - quality parameter, only takes effect in VBR mode
     *
     * Mpp does not give the direct parameter in different protocol.
     *
     * Mpp provide total 5 quality level:
     * Worst - worse - Medium - better - best
     *
     * extra CQP level means special constant-qp (CQP) mode
     *
     * default value: Medium
     */
    MppEncRcQuality quality;

    /*
     * bit rate parameters
     * mpp gives three bit rate control parameter for control
     * bps_target   - target  bit rate, unit: bit per second
     * bps_max      - maximun bit rate, unit: bit per second
     * bps_min      - minimun bit rate, unit: bit per second
     * if user need constant bit rate set parameters to the similar value
     * if user need variable bit rate set parameters as they need
     */
    RK_S32  bps_target;
    RK_S32  bps_max;
    RK_S32  bps_min;

    /*
     * frame rate parameters have great effect on rate control
     *
     * fps_in_flex
     * 0 - fix input frame rate
     * 1 - variable input frame rate
     *
     * fps_in_num
     * input frame rate numerator, if 0 then default 30
     *
     * fps_in_denorm
     * input frame rate denorminator, if 0 then default 1
     *
     * fps_out_flex
     * 0 - fix output frame rate
     * 1 - variable output frame rate
     *
     * fps_out_num
     * output frame rate numerator, if 0 then default 30
     *
     * fps_out_denorm
     * output frame rate denorminator, if 0 then default 1
     */
    RK_S32  fps_in_flex;
    RK_S32  fps_in_num;
    RK_S32  fps_in_denorm;
    RK_S32  fps_out_flex;
    RK_S32  fps_out_num;
    RK_S32  fps_out_denorm;

    /*
     * gop - group of picture, gap between Intra frame
     * 0 for only 1 I frame the rest are all P frames
     * 1 for all I frame
     * 2 for I P I P I P
     * 3 for I P P I P P
     * etc...
     */
    RK_S32  gop;
    void    *ref_cfg;

    /*
     * skip_cnt - max continuous frame skip count
     * 0 - frame skip is not allow
     */
    RK_S32  skip_cnt;

    /*
     * max_reenc_times - max reencode time for one frame
     * 0 - reencode is not allowed
     * 1~3 max reencode time is limited to 3
     */
    RK_U32  max_reenc_times;

    /*
     * stats_time   - the time of bitrate statistics
     */
    RK_S32  stats_time;

    /*
     * drop frame parameters
     * used on bitrate is far over the max bitrate
     *
     * drop_mode
     *
     * MPP_ENC_RC_DROP_FRM_DISABLED
     * - do not drop frame when bitrate overflow.
     * MPP_ENC_RC_DROP_FRM_NORMAL
     * - do not encode the dropped frame when bitrate overflow.
     * MPP_ENC_RC_DROP_FRM_PSKIP
     * - encode a all skip frame when bitrate overflow.
     *
     * drop_threshold
     *
     * The percentage threshold over max_bitrate for trigger frame drop.
     *
     * drop_gap
     * The max continuous frame drop number
     */
    MppEncRcDropFrmMode     drop_mode;
    RK_U32                  drop_threshold;
    RK_U32                  drop_gap;

    MppEncRcSuperFrameMode  super_mode;
    RK_U32                  super_i_thd;
    RK_U32                  super_p_thd;

    MppEncRcPriority        rc_priority;

    RK_U32                  debreath_en;
    RK_U32                  debre_strength;
    RK_S32                  max_i_prop;
    RK_S32                  min_i_prop;
    RK_S32                  init_ip_ratio;

    /* general qp control */
    RK_S32                  qp_init;
    RK_S32                  qp_max;
    RK_S32                  qp_max_i;
    RK_S32                  qp_min;
    RK_S32                  qp_min_i;
    RK_S32                  qp_max_step;                /* delta qp between each two P frame */
    RK_S32                  qp_delta_ip;                /* delta qp between I and P */
    RK_S32                  qp_delta_vi;                /* delta qp between vi and P */

    RK_S32                  hier_qp_en;
    RK_S32                  hier_qp_delta[4];
    RK_S32                  hier_frame_num[4];

    RK_U32                  refresh_en;
    MppEncRcRefreshMode     refresh_mode;
    RK_U32                  refresh_num;
    RK_S32                  refresh_length;
} MppEncRcCfg;


typedef enum MppEncHwCfgChange_e {
    /* qp related hardware config flag */
    MPP_ENC_HW_CFG_CHANGE_QP_ROW        = (1 << 0),
    MPP_ENC_HW_CFG_CHANGE_QP_ROW_I      = (1 << 1),
    MPP_ENC_HW_CFG_CHANGE_AQ_THRD_I     = (1 << 2),
    MPP_ENC_HW_CFG_CHANGE_AQ_THRD_P     = (1 << 3),
    MPP_ENC_HW_CFG_CHANGE_AQ_STEP_I     = (1 << 4),
    MPP_ENC_HW_CFG_CHANGE_AQ_STEP_P     = (1 << 5),
    MPP_ENC_HW_CFG_CHANGE_MB_RC         = (1 << 6),
    MPP_ENC_HW_CFG_CHANGE_CU_MODE_BIAS  = (1 << 8),
    MPP_ENC_HW_CFG_CHANGE_CU_SKIP_BIAS  = (1 << 9),
    MPP_ENC_HW_CFG_CHANGE_ALL           = (0xFFFFFFFF),
} MppEncHwCfgChange;

/*
 * Hardware related rate control config
 *
 * This config will open some detail feature to external user to control
 * hardware behavior directly.
 */
typedef struct MppEncHwCfg_t {
    RK_U32                  change;

    /* vepu541/vepu540 */
    RK_S32                  qp_delta_row;               /* delta qp between two row in P frame */
    RK_S32                  qp_delta_row_i;             /* delta qp between two row in I frame */
    RK_U32                  aq_thrd_i[16];
    RK_U32                  aq_thrd_p[16];
    RK_S32                  aq_step_i[16];
    RK_S32                  aq_step_p[16];

    /* vepu1/2 */
    RK_S32                  mb_rc_disable;

    /* vepu580 */
    RK_S32                  extra_buf;

    /*
     * block mode decision bias config
     * 0 - intra32x32
     * 1 - intra16x16
     * 2 - intra8x8
     * 3 - intra4x4
     * 4 - inter64x64
     * 5 - inter32x32
     * 6 - inter16x16
     * 7 - inter8x8
     * value range 0 ~ 15, default : 8
     * If the value is smaller then encoder will be more likely to encode corresponding block mode.
     */
    RK_S32                  mode_bias[8];

    /*
     * skip mode bias config
     * skip_bias_en - enable flag for skip bias config
     * skip_sad     - sad threshold for skip / non-skip
     * skip_bias    - tendency for skip, value range 0 ~ 15, default : 8
     *                If the value is smaller then encoder will be more likely to encode skip block.
     */
    RK_S32                  skip_bias_en;
    RK_S32                  skip_sad;
    RK_S32                  skip_bias;
} MppEncHwCfg;

/*
 * Mpp preprocess parameter
 */
typedef enum MppEncPrepCfgChange_e {
    MPP_ENC_PREP_CFG_CHANGE_INPUT       = (1 << 0),     /* change on input config */
    MPP_ENC_PREP_CFG_CHANGE_FORMAT      = (1 << 2),     /* change on format */
    /* transform parameter */
    MPP_ENC_PREP_CFG_CHANGE_ROTATION    = (1 << 4),     /* change on rotation */
    MPP_ENC_PREP_CFG_CHANGE_MIRRORING   = (1 << 5),     /* change on mirroring */
    MPP_ENC_PREP_CFG_CHANGE_FLIP        = (1 << 6),     /* change on flip */
    /* enhancement parameter */
    MPP_ENC_PREP_CFG_CHANGE_DENOISE     = (1 << 8),     /* change on denoise */
    MPP_ENC_PREP_CFG_CHANGE_SHARPEN     = (1 << 9),     /* change on denoise */
    /* color related parameter */
    MPP_ENC_PREP_CFG_CHANGE_COLOR_RANGE = (1 << 16),    /* change on color range */
    MPP_ENC_PREP_CFG_CHANGE_COLOR_SPACE = (1 << 17),    /* change on color range */
    MPP_ENC_PREP_CFG_CHANGE_COLOR_PRIME = (1 << 18),    /* change on color primaries */
    MPP_ENC_PREP_CFG_CHANGE_COLOR_TRC   = (1 << 19),    /* change on color transfer  */

    MPP_ENC_PREP_CFG_CHANGE_ALL         = (0xFFFFFFFF),
} MppEncPrepCfgChange;

/*
 * Preprocess sharpen parameter
 *
 * 5x5 sharpen core
 *
 * enable_y  - enable luma sharpen
 * enable_uv - enable chroma sharpen
 */
typedef struct {
    RK_U32              enable_y;
    RK_U32              enable_uv;
    RK_S32              coef[5];
    RK_S32              div;
    RK_S32              threshold;
} MppEncPrepSharpenCfg;

/*
 * input frame rotation parameter
 * 0 - disable rotation
 * 1 - 90 degree
 * 2 - 180 degree
 * 3 - 270 degree
 */
typedef enum MppEncRotationCfg_e {
    MPP_ENC_ROT_0,
    MPP_ENC_ROT_90,
    MPP_ENC_ROT_180,
    MPP_ENC_ROT_270,
    MPP_ENC_ROT_BUTT
} MppEncRotationCfg;

typedef struct MppEncPrepCfg_t {
    RK_U32              change;

    /*
     * Mpp encoder input data dimension config
     *
     * width / height / hor_stride / ver_stride / format
     * These information will be used for buffer allocation and rc config init
     * The output format is always YUV420. So if input is RGB then color
     * conversion will be done internally
     */
    RK_S32              width;
    RK_S32              height;
    RK_S32              hor_stride;
    RK_S32              ver_stride;

    /*
     * Mpp encoder input data format config
     */
    MppFrameFormat      format;
    MppFrameColorSpace  color;
    MppFrameColorPrimaries colorprim;
    MppFrameColorTransferCharacteristic colortrc;
    MppFrameColorRange  range;

    /* suffix ext means the user set config externally */
    MppEncRotationCfg   rotation;
    MppEncRotationCfg   rotation_ext;

    /*
     * input frame mirroring parameter
     * 0 - disable mirroring
     * 1 - horizontal mirroring
     */
    RK_S32              mirroring;
    RK_S32              mirroring_ext;

    /*
     * input frame flip parameter
     * 0 - disable flip
     * 1 - flip, vertical mirror transformation
     */
    RK_S32              flip;

    /*
     * TODO:
     */
    RK_S32              denoise;

    MppEncPrepSharpenCfg sharpen;
} MppEncPrepCfg;

/*
 * Mpp Motion Detection parameter
 *
 * Mpp can output Motion Detection infomation for each frame.
 * If user euqueue a encode task with KEY_MOTION_INFO by following function
 * then encoder will output Motion Detection information to the buffer.
 *
 * mpp_task_meta_set_buffer(task, KEY_MOTION_INFO, buffer);
 *
 * Motion Detection information will be organized in this way:
 * 1. Each 16x16 block will have a 32 bit block information which contains
 *    15 bit SAD(Sum of Abstract Difference value
 *    9 bit signed horizontal motion vector
 *    8 bit signed vertical motion vector
 * 2. The sequence of MD information in the buffer is corresponding to the
 *    block position in the frame, left-to right, top-to-bottom.
 * 3. If the width of the frame is not a multiple of 256 pixels (16 macro
 *    blocks), DMA would extend the frame to a multiple of 256 pixels and
 *    the extended blocks' MD information are 32'h0000_0000.
 * 4. Buffer must be ion buffer and 1024 byte aligned.
 */
typedef struct MppEncMDBlkInfo_t {
    RK_U32              sad     : 15;   /* bit  0~14 - SAD */
    RK_S32              mvx     : 9;    /* bit 15~23 - signed horizontal mv */
    RK_S32              mvy     : 8;    /* bit 24~31 - signed vertical mv */
} MppEncMDBlkInfo;

typedef enum MppEncHeaderMode_e {
    /* default mode: attach vps/sps/pps only on first frame */
    MPP_ENC_HEADER_MODE_DEFAULT,
    /* IDR mode: attach vps/sps/pps on each IDR frame */
    MPP_ENC_HEADER_MODE_EACH_IDR,
    MPP_ENC_HEADER_MODE_BUTT,
} MppEncHeaderMode;

typedef enum MppEncSeiMode_e {
    MPP_ENC_SEI_MODE_DISABLE,                /* default mode, SEI writing is disabled */
    MPP_ENC_SEI_MODE_ONE_SEQ,                /* one sequence has only one SEI */
    MPP_ENC_SEI_MODE_ONE_FRAME               /* one frame may have one SEI, if SEI info has changed */
} MppEncSeiMode;

/*
 * Mpp codec parameter
 * parameter is defined from here
 */

/*
 * H.264 configurable parameter
 */
typedef enum MppEncH264CfgChange_e {
    /* change on stream type */
    MPP_ENC_H264_CFG_STREAM_TYPE            = (1 << 0),
    /* change on svc / profile / level */
    MPP_ENC_H264_CFG_CHANGE_PROFILE         = (1 << 1),
    /* change on entropy_coding_mode / cabac_init_idc */
    MPP_ENC_H264_CFG_CHANGE_ENTROPY         = (1 << 2),

    /* change on transform8x8_mode */
    MPP_ENC_H264_CFG_CHANGE_TRANS_8x8       = (1 << 4),
    /* change on constrained_intra_pred_mode */
    MPP_ENC_H264_CFG_CHANGE_CONST_INTRA     = (1 << 5),
    /* change on chroma_cb_qp_offset/ chroma_cr_qp_offset */
    MPP_ENC_H264_CFG_CHANGE_CHROMA_QP       = (1 << 6),
    /* change on deblock_disable / deblock_offset_alpha / deblock_offset_beta */
    MPP_ENC_H264_CFG_CHANGE_DEBLOCKING      = (1 << 7),
    /* change on use_longterm */
    MPP_ENC_H264_CFG_CHANGE_LONG_TERM       = (1 << 8),
    /* change on scaling_list_mode */
    MPP_ENC_H264_CFG_CHANGE_SCALING_LIST    = (1 << 9),
    /* change on poc type */
    MPP_ENC_H264_CFG_CHANGE_POC_TYPE        = (1 << 10),
    /* change on log2 max poc lsb minus 4 */
    MPP_ENC_H264_CFG_CHANGE_MAX_POC_LSB     = (1 << 11),
    /* change on log2 max frame number minus 4 */
    MPP_ENC_H264_CFG_CHANGE_MAX_FRM_NUM     = (1 << 12),
    /* change on gaps_in_frame_num_value_allowed_flag */
    MPP_ENC_H264_CFG_CHANGE_GAPS_IN_FRM_NUM = (1 << 13),

    /* change on max_qp / min_qp */
    MPP_ENC_H264_CFG_CHANGE_QP_LIMIT        = (1 << 16),
    /* change on max_qp_i / min_qp_i */
    MPP_ENC_H264_CFG_CHANGE_QP_LIMIT_I      = (1 << 17),
    /* change on max_qp_step */
    MPP_ENC_H264_CFG_CHANGE_MAX_QP_STEP     = (1 << 18),
    /* change on qp_delta_ip */
    MPP_ENC_H264_CFG_CHANGE_QP_DELTA        = (1 << 19),
    /* change on intra_refresh_mode / intra_refresh_arg */
    MPP_ENC_H264_CFG_CHANGE_INTRA_REFRESH   = (1 << 20),
    /* change on max long-term reference frame count */
    MPP_ENC_H264_CFG_CHANGE_MAX_LTR         = (1 << 21),
    /* change on max temporal id */
    MPP_ENC_H264_CFG_CHANGE_MAX_TID         = (1 << 22),
    /* change on adding prefix nal */
    MPP_ENC_H264_CFG_CHANGE_ADD_PREFIX      = (1 << 23),
    /* change on base layer priority id */
    MPP_ENC_H264_CFG_CHANGE_BASE_LAYER_PID  = (1 << 24),

    /* change on vui */
    MPP_ENC_H264_CFG_CHANGE_VUI             = (1 << 28),

    /* change on constraint */
    MPP_ENC_H264_CFG_CHANGE_CONSTRAINT_SET  = (1 << 29),

    MPP_ENC_H264_CFG_CHANGE_ALL             = (0xFFFFFFFF),
} MppEncH264CfgChange;

typedef struct MppEncH264Cfg_t {
    RK_U32              change;

    /*
     * H.264 stream format
     * 0 - H.264 Annex B: NAL unit starts with '00 00 00 01'
     * 1 - Plain NAL units without startcode
     */
    RK_S32              stream_type;

    /*
     * H.264 codec syntax config
     *
     * do NOT setup the three option below unless you are familiar with encoder detail
     * poc_type             - picture order count type 0 ~ 2
     * log2_max_poc_lsb     - used in sps with poc_type 0,
     * log2_max_frame_num   - used in sps
     */
    RK_U32              poc_type;
    RK_U32              hw_poc_type;
    RK_U32              log2_max_poc_lsb;
    RK_U32              log2_max_frame_num;
    RK_U32              gaps_not_allowed;

    /*
     * H.264 profile_idc parameter
     * 66  - Baseline profile
     * 77  - Main profile
     * 100 - High profile
     */
    RK_S32              profile;

    /*
     * H.264 level_idc parameter
     * 10 / 11 / 12 / 13    - qcif@15fps / cif@7.5fps / cif@15fps / cif@30fps
     * 20 / 21 / 22         - cif@30fps / half-D1@@25fps / D1@12.5fps
     * 30 / 31 / 32         - D1@25fps / 720p@30fps / 720p@60fps
     * 40 / 41 / 42         - 1080p@30fps / 1080p@30fps / 1080p@60fps
     * 50 / 51 / 52         - 4K@30fps
     */
    RK_S32              level;

    /*
     * H.264 entropy coding method
     * 0 - CAVLC
     * 1 - CABAC
     * When CABAC is select cabac_init_idc can be range 0~2
     */
    RK_S32              entropy_coding_mode;
    RK_S32              entropy_coding_mode_ex;
    RK_S32              cabac_init_idc;
    RK_S32              cabac_init_idc_ex;

    /*
     * 8x8 intra prediction and 8x8 transform enable flag
     * This flag can only be enable under High profile
     * 0 : disable (BP/MP)
     * 1 : enable  (HP)
     */
    RK_S32              transform8x8_mode;
    RK_S32              transform8x8_mode_ex;

    /*
     * 0 : disable
     * 1 : enable
     */
    RK_S32              constrained_intra_pred_mode;

    /*
     * 0 : flat scaling list
     * 1 : default scaling list for all cases
     * 2 : customized scaling list (not supported)
     */
    RK_S32              scaling_list_mode;

    /*
     * chroma qp offset (-12 - 12)
     */
    RK_S32              chroma_cb_qp_offset;
    RK_S32              chroma_cr_qp_offset;

    /*
     * H.264 deblock filter mode flag
     * 0 : enable
     * 1 : disable
     * 2 : disable deblocking filter at slice boundaries
     *
     * deblock filter offset alpha (-6 - 6)
     * deblock filter offset beta  (-6 - 6)
     */
    RK_S32              deblock_disable;
    RK_S32              deblock_offset_alpha;
    RK_S32              deblock_offset_beta;

    /*
     * H.264 long term reference picture enable flag
     * 0 - disable
     * 1 - enable
     */
    RK_S32              use_longterm;

    /*
     * quality config
     * qp_max       - 8  ~ 51
     * qp_max_i     - 10 ~ 40
     * qp_min       - 8  ~ 48
     * qp_min_i     - 10 ~ 40
     * qp_max_step  - max delta qp step between two frames
     */
    RK_S32              qp_init;
    RK_S16              qp_max;
    RK_S16              qp_max_i;
    RK_S16              qp_min;
    RK_S16              qp_min_i;
    RK_S16              qp_max_step;
    RK_S16              qp_delta_ip;

    /*
     * intra fresh config
     *
     * intra_refresh_mode
     * 0 - no intra refresh
     * 1 - intra refresh by MB row
     * 2 - intra refresh by MB column
     * 3 - intra refresh by MB gap
     *
     * intra_refresh_arg
     * mode 0 - no effect
     * mode 1 - refresh MB row number
     * mode 2 - refresh MB colmn number
     * mode 3 - refresh MB gap count
     */
    RK_S32              intra_refresh_mode;
    RK_S32              intra_refresh_arg;

    /* extra mode config */
    RK_S32              max_ltr_frames;
    RK_S32              max_tid;
    RK_S32              prefix_mode;
    RK_S32              base_layer_pid;
    /*
     * Mpp encoder constraint_set parameter
     * Mpp encoder constraint_set controls constraint_setx_flag in AVC.
     * Mpp encoder constraint_set uses type RK_U32 to store force_flag and constraint_force as followed.
     * | 00 | force_flag | 00 | constraint_force |
     * As for force_flag and constraint_force, only low 6 bits are valid,
     * corresponding to constraint_setx_flag from 5 to 0.
     * If force_flag bit is enabled, constraint_setx_flag will be set correspondingly.
     * Otherwise, constraint_setx_flag will use default value.
     */
    RK_U32              constraint_set;
} MppEncH264Cfg;

#define H265E_MAX_ROI_NUMBER  64

typedef struct H265eRect_t {
    RK_S32              left;
    RK_S32              right;
    RK_S32              top;
    RK_S32              bottom;
} H265eRect;

typedef struct H265eRoi_Region_t {
    RK_U8               level;
    H265eRect           rect;
} H265eRoiRegion;

/*
 * roi region only can be setting when rc_enable = 1
 */
typedef struct MppEncH265RoiCfg_t {
    /*
     * the value is defined by H265eCtuMethod
     */

    RK_U8               method;
    /*
     * the number of roi,the value must less than H265E_MAX_ROI_NUMBER
     */
    RK_S32              num;

    /* delat qp using in roi region*/
    RK_U32              delta_qp;

    /* roi region */
    H265eRoiRegion      region[H265E_MAX_ROI_NUMBER];
} MppEncH265RoiCfg;

typedef struct H265eCtuQp_t {
    /* the qp value using in ctu region */
    RK_U32              qp;

    /*
     * define the ctu region
     * method = H265E_METHOD_CUT_SIZE, the value of rect is in ctu size
     * method = H264E_METHOD_COORDINATE,the value of rect is in coordinates
     */
    H265eRect           rect;
} H265eCtu;

typedef struct H265eCtuRegion_t {
    /*
     * the value is defined by H265eCtuMethod
     */
    RK_U8               method;

    /*
     * the number of ctu,the value must less than H265E_MAX_ROI_NUMBER
     */
    RK_S32              num;

    /* ctu region */
    H265eCtu            ctu[H265E_MAX_ROI_NUMBER];
} MppEncH265CtuCfg;

/*
 * define the method when set CTU/ROI parameters
 * this value is using by method in H265eCtuRegion or H265eRoi struct
 */
typedef enum {
    H265E_METHOD_CTU_SIZE,
    H264E_METHOD_COORDINATE,
} H265eCtuMethod;

/*
 * H.265 configurable parameter
 */
typedef struct MppEncH265VuiCfg_t {
    RK_U32              change;
    RK_S32              vui_present;
    RK_S32              vui_aspect_ratio;
    RK_S32              vui_sar_size;
    RK_S32              full_range;
    RK_S32              time_scale;
} MppEncH265VuiCfg;

typedef enum MppEncH265CfgChange_e {
    /* change on stream type */
    MPP_ENC_H265_CFG_PROFILE_LEVEL_TILER_CHANGE = (1 << 0),
    MPP_ENC_H265_CFG_INTRA_QP_CHANGE            = (1 << 1),
    MPP_ENC_H265_CFG_FRAME_RATE_CHANGE          = (1 << 2),
    MPP_ENC_H265_CFG_BITRATE_CHANGE             = (1 << 3),
    MPP_ENC_H265_CFG_GOP_SIZE                   = (1 << 4),
    MPP_ENC_H265_CFG_RC_QP_CHANGE               = (1 << 5),
    MPP_ENC_H265_CFG_INTRA_REFRESH_CHANGE       = (1 << 6),
    MPP_ENC_H265_CFG_INDEPEND_SLICE_CHANGE      = (1 << 7),
    MPP_ENC_H265_CFG_DEPEND_SLICE_CHANGE        = (1 << 8),
    MPP_ENC_H265_CFG_CTU_CHANGE                 = (1 << 9),
    MPP_ENC_H265_CFG_ROI_CHANGE                 = (1 << 10),
    MPP_ENC_H265_CFG_CU_CHANGE                  = (1 << 11),
    MPP_ENC_H265_CFG_DBLK_CHANGE                = (1 << 12),
    MPP_ENC_H265_CFG_SAO_CHANGE                 = (1 << 13),
    MPP_ENC_H265_CFG_TRANS_CHANGE               = (1 << 14),
    MPP_ENC_H265_CFG_SLICE_CHANGE               = (1 << 15),
    MPP_ENC_H265_CFG_ENTROPY_CHANGE             = (1 << 16),
    MPP_ENC_H265_CFG_MERGE_CHANGE               = (1 << 17),
    MPP_ENC_H265_CFG_CHANGE_VUI                 = (1 << 18),
    MPP_ENC_H265_CFG_RC_I_QP_CHANGE             = (1 << 19),
    MPP_ENC_H265_CFG_RC_MAX_QP_STEP_CHANGE      = (1 << 21),
    MPP_ENC_H265_CFG_RC_IP_DELTA_QP_CHANGE      = (1 << 20),
    MPP_ENC_H265_CFG_TILE_CHANGE                = (1 << 22),
    MPP_ENC_H265_CFG_SLICE_LPFACS_CHANGE        = (1 << 23),
    MPP_ENC_H265_CFG_TILE_LPFACS_CHANGE         = (1 << 24),
    MPP_ENC_H265_CFG_CHANGE_ALL                 = (0xFFFFFFFF),
} MppEncH265CfgChange;

typedef struct MppEncH265SliceCfg_t {
    /* default value: 0, means no slice split*/
    RK_U32  split_enable;

    /* 0: by bits number; 1: by lcu line number*/
    RK_U32  split_mode;

    /*
     * when splitmode is 0, this value presents bits number,
     * when splitmode is 1, this value presents lcu line number
     */
    RK_U32  slice_size;
    RK_U32  slice_out;
} MppEncH265SliceCfg;

typedef struct MppEncH265CuCfg_t {
    RK_U32  cu32x32_en;                             /*default: 1 */
    RK_U32  cu16x16_en;                             /*default: 1 */
    RK_U32  cu8x8_en;                               /*default: 1 */
    RK_U32  cu4x4_en;                               /*default: 1 */

    // intra pred
    RK_U32  constrained_intra_pred_flag;            /*default: 0 */
    RK_U32  strong_intra_smoothing_enabled_flag;    /*INTRA_SMOOTH*/
    RK_U32  pcm_enabled_flag;                       /*default: 0, enable ipcm*/
    RK_U32  pcm_loop_filter_disabled_flag;

} MppEncH265CuCfg;

typedef struct MppEncH265RefCfg_t {
    RK_U32  num_lt_ref_pic;                         /*default: 0*/
} MppEncH265RefCfg;


typedef struct MppEncH265DblkCfg_t {
    RK_U32  slice_deblocking_filter_disabled_flag;  /* default value: 0. {0,1} */
    RK_S32  slice_beta_offset_div2;                 /* default value: 0. [-6,+6] */
    RK_S32  slice_tc_offset_div2;                   /* default value: 0. [-6,+6] */
} MppEncH265DblkCfg_t;

typedef struct MppEncH265SaoCfg_t {
    RK_U32  slice_sao_luma_disable;
    RK_U32  slice_sao_chroma_disable;
} MppEncH265SaoCfg;

typedef struct MppEncH265TransCfg_t {
    RK_U32  transquant_bypass_enabled_flag;
    RK_U32  transform_skip_enabled_flag;
    RK_U32  defalut_ScalingList_enable;             /* default: 0 */
    RK_S32  cb_qp_offset;
    RK_S32  cr_qp_offset;
} MppEncH265TransCfg;

typedef struct MppEncH265MergeCfg_t {
    RK_U32  max_mrg_cnd;
    RK_U32  merge_up_flag;
    RK_U32  merge_left_flag;
} MppEncH265MergesCfg;

typedef struct MppEncH265EntropyCfg_t {
    RK_U32  cabac_init_flag;                        /* default: 0 */
} MppEncH265EntropyCfg;

typedef struct MppEncH265Cfg_t {
    RK_U32              change;

    /* H.265 codec syntax config */
    RK_S32              profile;
    RK_S32              level;
    RK_S32              tier;

    /* constraint intra prediction flag */
    RK_S32              const_intra_pred;
    RK_S32              ctu_size;
    RK_S32              max_cu_size;
    RK_S32              tmvp_enable;
    RK_S32              amp_enable;
    RK_S32              wpp_enable;
    RK_S32              merge_range;
    RK_S32              sao_enable;
    RK_U32              num_ref;

    /* quality config */
    RK_S32              max_qp;
    RK_S32              min_qp;
    RK_S32              max_i_qp;
    RK_S32              min_i_qp;
    RK_S32              ip_qp_delta;
    RK_S32              max_delta_qp;
    RK_S32              intra_qp;
    RK_S32              gop_delta_qp;
    RK_S32              qp_init;
    RK_S32              qp_max_step;
    RK_S32              raw_dealt_qp;
    RK_U8               qpmax_map[8];
    RK_U8               qpmin_map[8];
    RK_S32              qpmap_mode;

    /* intra fresh config */
    RK_S32              intra_refresh_mode;
    RK_S32              intra_refresh_arg;

    /* slice mode config */
    RK_S32              independ_slice_mode;
    RK_S32              independ_slice_arg;
    RK_S32              depend_slice_mode;
    RK_S32              depend_slice_arg;

    MppEncH265CuCfg      cu_cfg;
    MppEncH265SliceCfg   slice_cfg;
    MppEncH265EntropyCfg entropy_cfg;
    MppEncH265TransCfg   trans_cfg;
    MppEncH265SaoCfg     sao_cfg;
    MppEncH265DblkCfg_t  dblk_cfg;
    MppEncH265RefCfg     ref_cfg;
    MppEncH265MergesCfg  merge_cfg;
    RK_S32               auto_tile;
    RK_U32               lpf_acs_sli_en;
    RK_U32               lpf_acs_tile_disable;

    /* extra info */
    MppEncH265VuiCfg    vui;

    MppEncH265CtuCfg    ctu;
    MppEncH265RoiCfg    roi;
} MppEncH265Cfg;

/*
 * motion jpeg configurable parameter
 */
typedef enum MppEncJpegCfgChange_e {
    /* change on quant parameter */
    MPP_ENC_JPEG_CFG_CHANGE_QP              = (1 << 0),
    MPP_ENC_JPEG_CFG_CHANGE_QTABLE          = (1 << 1),
    MPP_ENC_JPEG_CFG_CHANGE_QFACTOR         = (1 << 2),
    MPP_ENC_JPEG_CFG_CHANGE_ALL             = (0xFFFFFFFF),
} MppEncJpegCfgChange;

typedef struct MppEncJpegCfg_t {
    RK_U32              change;
    RK_S32              quant;
    /*
     * quality factor config
     *
     * q_factor     - 1  ~ 99
     * qf_max       - 1  ~ 99
     * qf_min       - 1  ~ 99
     * qtable_y: qtable for luma
     * qtable_u: qtable for chroma
     * qtable_v: default equal qtable_u
     */
    RK_S32              q_factor;
    RK_S32              qf_max;
    RK_S32              qf_min;
    RK_U8               *qtable_y;
    RK_U8               *qtable_u;
    RK_U8               *qtable_v;
} MppEncJpegCfg;

/*
 * vp8 configurable parameter
 */
typedef enum MppEncVP8CfgChange_e {
    MPP_ENC_VP8_CFG_CHANGE_QP              = (1 << 0),
    MPP_ENC_VP8_CFG_CHANGE_DIS_IVF         = (1 << 1),
    MPP_ENC_VP8_CFG_CHANGE_ALL             = (0xFFFFFFFF),
} MppEncVP8CfgChange;

typedef struct MppEncVp8Cfg_t {
    RK_U32              change;
    RK_S32              quant;

    RK_S32              qp_init;
    RK_S32              qp_max;
    RK_S32              qp_max_i;
    RK_S32              qp_min;
    RK_S32              qp_min_i;
    RK_S32              qp_max_step;
    RK_S32              disable_ivf;
} MppEncVp8Cfg;

/**
 * @ingroup rk_mpi
 * @brief MPP encoder codec configuration parameters
 * @details The encoder codec configuration parameters are different for each
 *          compression codings. For example, H.264 encoder can configure
 *          profile, level, qp, etc. while jpeg encoder can configure qp
 *          only. The detailed parameters can refer the corresponding data
 *          structure such as MppEncH264Cfg and MppEncJpegCfg. This data
 *          structure is associated with MPP_ENC_SET_CODEC_CFG command.
 */
typedef struct MppEncCodecCfg_t {
    MppCodingType       coding;

    union {
        RK_U32          change;
        MppEncH264Cfg   h264;
        MppEncH265Cfg   h265;
        MppEncJpegCfg   jpeg;
        MppEncVp8Cfg    vp8;
    };
} MppEncCodecCfg;

typedef enum MppEncSliceSplit_e {
    /* change on quant parameter */
    MPP_ENC_SPLIT_CFG_CHANGE_MODE           = (1 << 0),
    MPP_ENC_SPLIT_CFG_CHANGE_ARG            = (1 << 1),
    MPP_ENC_SPLIT_CFG_CHANGE_OUTPUT         = (1 << 2),
    MPP_ENC_SPLIT_CFG_CHANGE_ALL            = (0xFFFFFFFF),
} MppEncSliceSplitChange;

typedef enum MppEncSplitMode_e {
    MPP_ENC_SPLIT_NONE,
    MPP_ENC_SPLIT_BY_BYTE,
    MPP_ENC_SPLIT_BY_CTU,
} MppEncSplitMode;

typedef enum MppEncSplitOutMode_e {
    MPP_ENC_SPLIT_OUT_LOWDELAY              = (1 << 0),
    MPP_ENC_SPLIT_OUT_SEGMENT               = (1 << 1),
} MppEncSplitOutMode;

typedef struct MppEncSliceSplit_t {
    RK_U32  change;

    /*
     * slice split mode
     *
     * MPP_ENC_SPLIT_NONE    - No slice is split
     * MPP_ENC_SPLIT_BY_BYTE - Slice is split by byte number
     * MPP_ENC_SPLIT_BY_CTU  - Slice is split by macroblock / ctu number
     */
    RK_U32  split_mode;

    /*
     * slice split size parameter
     *
     * When split by byte number this value is the max byte number for each
     * slice.
     * When split by macroblock / ctu number this value is the MB/CTU number
     * for each slice.
     */
    RK_U32  split_arg;

    /*
     * slice split output mode
     *
     * MPP_ENC_SPLIT_OUT_LOWDELAY
     * - When enabled encoder will lowdelay output each slice in a single packet
     * MPP_ENC_SPLIT_OUT_SEGMENT
     * - When enabled encoder will packet with segment info for each slice
     */
    RK_U32  split_out;
} MppEncSliceSplit;

/**
 * @brief Mpp ROI parameter
 *        Region configure define a rectangle as ROI
 * @note  x, y, w, h are calculated in pixels, which had better be 16-pixel aligned.
 *        These parameters MUST retain in memory when encoder is running.
 *        Both absolute qp and relative qp are supported in vepu541.
 *        Only absolute qp is supported in rv1108
 */
typedef struct MppEncROIRegion_t {
    RK_U16              x;              /**< horizontal position of top left corner */
    RK_U16              y;              /**< vertical position of top left corner */
    RK_U16              w;              /**< width of ROI rectangle */
    RK_U16              h;              /**< height of ROI rectangle */
    RK_U16              intra;          /**< flag of forced intra macroblock */
    RK_S16              quality;        /**< absolute / relative qp of macroblock */
    RK_U16              qp_area_idx;    /**< qp min max area select*/
    RK_U8               area_map_en;    /**< enable area map */
    RK_U8               abs_qp_en;      /**< absolute qp enable flag*/
} MppEncROIRegion;

/**
 * @brief MPP encoder's ROI configuration
 */
typedef struct MppEncROICfg_t {
    RK_U32              number;        /**< ROI rectangle number */
    MppEncROIRegion     *regions;      /**< ROI parameters */
} MppEncROICfg;

/**
 * @brief Mpp ROI parameter for vepu54x / vepu58x
 * @note  These encoders have more complex roi configure structure.
 *        User need to generate roi structure data for different soc.
 *        And send buffers to encoder through metadata.
 */
typedef struct MppEncROICfg2_t {
    MppBuffer          base_cfg_buf;
    MppBuffer          qp_cfg_buf;
    MppBuffer          amv_cfg_buf;
    MppBuffer          mv_cfg_buf;

    RK_U32             roi_qp_en    : 1;
    RK_U32             roi_amv_en   : 1;
    RK_U32             roi_mv_en    : 1;
    RK_U32             reserve_bits : 29;
    RK_U32             reserve[3];
} MppEncROICfg2;

/*
 * Mpp OSD parameter
 *
 * Mpp OSD support total 8 regions
 * Mpp OSD support 256-color palette two mode palette:
 * 1. Configurable OSD palette
 *    When palette is set.
 * 2. fixed OSD palette
 *    When palette is NULL.
 *
 * if MppEncOSDPlt.buf != NULL , palette includes maximun 256 levels,
 * every level composed of 32 bits defined below:
 * Y     : 8 bits
 * U     : 8 bits
 * V     : 8 bits
 * alpha : 8 bits
 */
#define MPP_ENC_OSD_PLT_WHITE           ((255<<24)|(128<<16)|(128<<8)|235)
#define MPP_ENC_OSD_PLT_YELLOW          ((255<<24)|(146<<16)|( 16<<8)|210)
#define MPP_ENC_OSD_PLT_CYAN            ((255<<24)|( 16<<16)|(166<<8)|170)
#define MPP_ENC_OSD_PLT_GREEN           ((255<<24)|( 34<<16)|( 54<<8)|145)
#define MPP_ENC_OSD_PLT_TRANS           ((  0<<24)|(222<<16)|(202<<8)|106)
#define MPP_ENC_OSD_PLT_RED             ((255<<24)|(240<<16)|( 90<<8)| 81)
#define MPP_ENC_OSD_PLT_BLUE            ((255<<24)|(110<<16)|(240<<8)| 41)
#define MPP_ENC_OSD_PLT_BLACK           ((255<<24)|(128<<16)|(128<<8)| 16)

typedef enum MppEncOSDPltType_e {
    MPP_ENC_OSD_PLT_TYPE_DEFAULT,
    MPP_ENC_OSD_PLT_TYPE_USERDEF,
    MPP_ENC_OSD_PLT_TYPE_BUTT,
} MppEncOSDPltType;

/* OSD palette value define */
typedef union MppEncOSDPltVal_u {
    struct {
        RK_U32          v       : 8;
        RK_U32          u       : 8;
        RK_U32          y       : 8;
        RK_U32          alpha   : 8;
    };
    RK_U32              val;
} MppEncOSDPltVal;

typedef struct MppEncOSDPlt_t {
    MppEncOSDPltVal     data[256];
} MppEncOSDPlt;

typedef enum MppEncOSDPltCfgChange_e {
    MPP_ENC_OSD_PLT_CFG_CHANGE_MODE     = (1 << 0),     /* change osd plt type */
    MPP_ENC_OSD_PLT_CFG_CHANGE_PLT_VAL  = (1 << 1),     /* change osd plt table value */
    MPP_ENC_OSD_PLT_CFG_CHANGE_ALL      = (0xFFFFFFFF),
} MppEncOSDPltCfgChange;

typedef struct MppEncOSDPltCfg_t {
    RK_U32              change;
    MppEncOSDPltType    type;
    MppEncOSDPlt        *plt;
} MppEncOSDPltCfg;

/* position info is unit in 16 pixels(one MB), and
 * x-directon range in pixels = (rd_pos_x - lt_pos_x + 1) * 16;
 * y-directon range in pixels = (rd_pos_y - lt_pos_y + 1) * 16;
 */
typedef struct MppEncOSDRegion_t {
    RK_U32              enable;
    RK_U32              inverse;
    RK_U32              start_mb_x;
    RK_U32              start_mb_y;
    RK_U32              num_mb_x;
    RK_U32              num_mb_y;
    RK_U32              buf_offset;
} MppEncOSDRegion;

/* if num_region > 0 && region==NULL
 * use old osd data
 */
typedef struct MppEncOSDData_t {
    MppBuffer           buf;
    RK_U32              num_region;
    MppEncOSDRegion     region[8];
} MppEncOSDData;

typedef struct MppEncOSDRegion2_t {
    RK_U32              enable;
    RK_U32              inverse;
    RK_U32              start_mb_x;
    RK_U32              start_mb_y;
    RK_U32              num_mb_x;
    RK_U32              num_mb_y;
    RK_U32              buf_offset;
    MppBuffer           buf;
} MppEncOSDRegion2;

typedef struct MppEncOSDData2_t {
    RK_U32              num_region;
    MppEncOSDRegion2    region[8];
} MppEncOSDData2;

typedef struct MppEncUserData_t {
    RK_U32              len;
    void                *pdata;
} MppEncUserData;

typedef struct MppEncUserDataFull_t {
    RK_U32              len;
    RK_U8               *uuid;
    void                *pdata;
} MppEncUserDataFull;

typedef struct MppEncUserDataSet_t {
    RK_U32              count;
    MppEncUserDataFull  *datas;
} MppEncUserDataSet;

typedef enum MppEncSceneMode_e {
    MPP_ENC_SCENE_MODE_DEFAULT,
    MPP_ENC_SCENE_MODE_IPC,
    MPP_ENC_SCENE_MODE_BUTT,
} MppEncSceneMode;

typedef enum MppEncFineTuneCfgChange_e {
    /* change on scene mode */
    MPP_ENC_TUNE_CFG_CHANGE_SCENE_MODE      = (1 << 0),
} MppEncFineTuneCfgChange;

typedef struct MppEncFineTuneCfg_t {
    RK_U32              change;

    MppEncSceneMode     scene_mode;
} MppEncFineTuneCfg;

#endif /*__RK_VENC_CMD_H__*/
