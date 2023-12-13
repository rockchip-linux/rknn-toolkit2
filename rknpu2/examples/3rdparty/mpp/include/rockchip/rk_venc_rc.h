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

#ifndef __RK_VENC_RC_H__
#define __RK_VENC_RC_H__

#include "rk_type.h"

#define MPP_ENC_MIN_BPS     (SZ_1K)
#define MPP_ENC_MAX_BPS     (SZ_1M * 200)

/* Rate control parameter */
typedef enum MppEncRcMode_e {
    MPP_ENC_RC_MODE_VBR,
    MPP_ENC_RC_MODE_CBR,
    MPP_ENC_RC_MODE_FIXQP,
    MPP_ENC_RC_MODE_AVBR,
    MPP_ENC_RC_MODE_BUTT
} MppEncRcMode;

typedef enum MppEncRcPriority_e {
    MPP_ENC_RC_BY_BITRATE_FIRST,
    MPP_ENC_RC_BY_FRM_SIZE_FIRST,
    MPP_ENC_RC_PRIORITY_BUTT
} MppEncRcPriority;

typedef enum MppEncRcDropFrmMode_e {
    MPP_ENC_RC_DROP_FRM_DISABLED,
    MPP_ENC_RC_DROP_FRM_NORMAL,
    MPP_ENC_RC_DROP_FRM_PSKIP,
    MPP_ENC_RC_DROP_FRM_BUTT
} MppEncRcDropFrmMode;

typedef enum MppEncRcSuperFrameMode_t {
    MPP_ENC_RC_SUPER_FRM_NONE,
    MPP_ENC_RC_SUPER_FRM_DROP,
    MPP_ENC_RC_SUPER_FRM_REENC,
    MPP_ENC_RC_SUPER_FRM_BUTT
} MppEncRcSuperFrameMode;

typedef enum MppEncRcGopMode_e {
    MPP_ENC_RC_NORMAL_P,
    MPP_ENC_RC_SMART_P,
    MPP_ENC_RC_GOP_MODE_BUTT,
} MppEncRcGopMode;

typedef enum MppEncRcIntraRefreshMode_e {
    MPP_ENC_RC_INTRA_REFRESH_ROW = 0,
    MPP_ENC_RC_INTRA_REFRESH_COL,
    MPP_ENC_RC_INTRA_REFRESH_BUTT
} MppEncRcRefreshMode;

#endif /*__RK_VENC_RC_H__*/
