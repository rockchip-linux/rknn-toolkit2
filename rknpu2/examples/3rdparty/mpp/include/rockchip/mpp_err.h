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

#ifndef __MPP_ERR_H__
#define __MPP_ERR_H__

#define RK_OK                   0
#define RK_SUCCESS              0

typedef enum {
    MPP_SUCCESS                 = RK_SUCCESS,
    MPP_OK                      = RK_OK,

    MPP_NOK                     = -1,
    MPP_ERR_UNKNOW              = -2,
    MPP_ERR_NULL_PTR            = -3,
    MPP_ERR_MALLOC              = -4,
    MPP_ERR_OPEN_FILE           = -5,
    MPP_ERR_VALUE               = -6,
    MPP_ERR_READ_BIT            = -7,
    MPP_ERR_TIMEOUT             = -8,
    MPP_ERR_PERM                = -9,

    MPP_ERR_BASE                = -1000,

    /* The error in stream processing */
    MPP_ERR_LIST_STREAM         = MPP_ERR_BASE - 1,
    MPP_ERR_INIT                = MPP_ERR_BASE - 2,
    MPP_ERR_VPU_CODEC_INIT      = MPP_ERR_BASE - 3,
    MPP_ERR_STREAM              = MPP_ERR_BASE - 4,
    MPP_ERR_FATAL_THREAD        = MPP_ERR_BASE - 5,
    MPP_ERR_NOMEM               = MPP_ERR_BASE - 6,
    MPP_ERR_PROTOL              = MPP_ERR_BASE - 7,
    MPP_FAIL_SPLIT_FRAME        = MPP_ERR_BASE - 8,
    MPP_ERR_VPUHW               = MPP_ERR_BASE - 9,
    MPP_EOS_STREAM_REACHED      = MPP_ERR_BASE - 11,
    MPP_ERR_BUFFER_FULL         = MPP_ERR_BASE - 12,
    MPP_ERR_DISPLAY_FULL        = MPP_ERR_BASE - 13,
} MPP_RET;

#endif /*__MPP_ERR_H__*/
