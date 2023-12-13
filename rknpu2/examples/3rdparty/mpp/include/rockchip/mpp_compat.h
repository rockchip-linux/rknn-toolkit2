/*
 * Copyright 2021 Rockchip Electronics Co. LTD
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

#ifndef __MPP_COMPAT_H__
#define __MPP_COMPAT_H__

#include "rk_type.h"
#include "mpp_err.h"

typedef enum MppCompatId_e {
    MPP_COMPAT_INC_FBC_BUF_SIZE,
    MPP_COMPAT_ENC_ASYNC_INPUT,
    MPP_COMPAT_DEC_FBC_HDR_256_ODD,
    MPP_COMPAT_BUTT,
} MppCompatId;

typedef enum MppCompatType_e {
    MPP_COMPAT_BOOL,
    MPP_COMPAT_S32,
    MPP_COMPAT_TYPE_BUTT,
} MppCompatType;

typedef struct MppCompat_t MppCompat;

/* external user can only update value_ext to notify mpp to change its behavior */
struct MppCompat_t {
    const MppCompatId   feature_id;
    const MppCompatType feature_type;
    const RK_S32        value_mpp;
    RK_S32              value_usr;
    const char          *name;
    MppCompat * const   next;
};

#ifdef __cplusplus
extern "C" {
#endif

MppCompat *mpp_compat_query(void);
MppCompat *mpp_compat_query_by_id(MppCompatId id);
MPP_RET mpp_compat_update(MppCompat *compat, RK_S32 value);

void mpp_compat_show(void);

#ifdef __cplusplus
}
#endif

#endif /*__MPP_COMPAT_H__*/
