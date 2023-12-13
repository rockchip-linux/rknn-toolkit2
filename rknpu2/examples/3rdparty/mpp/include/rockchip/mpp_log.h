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

#ifndef __MPP_LOG_H__
#define __MPP_LOG_H__

#include "rk_type.h"
#include "mpp_log_def.h"

/*
 * _c function will add condition check
 * _f function will add function name to the log
 * _cf function will add both function name and condition check
 */

/*
 * mpp runtime log system usage:
 * mpp_logf is for fatal logging. For use when aborting
 * mpp_loge is for error logging. For use with unrecoverable failures.
 * mpp_logw is for warning logging. For use with recoverable failures.
 * mpp_logi is for informational logging.
 * mpp_logd is for debug logging.
 * mpp_logv is for verbose logging
 */

#define mpp_logf(fmt, ...)  _mpp_log_l(MPP_LOG_FATAL,   MODULE_TAG, fmt, NULL, ## __VA_ARGS__)
#define mpp_loge(fmt, ...)  _mpp_log_l(MPP_LOG_ERROR,   MODULE_TAG, fmt, NULL, ## __VA_ARGS__)
#define mpp_logw(fmt, ...)  _mpp_log_l(MPP_LOG_WARN,    MODULE_TAG, fmt, NULL, ## __VA_ARGS__)
#define mpp_logi(fmt, ...)  _mpp_log_l(MPP_LOG_INFO,    MODULE_TAG, fmt, NULL, ## __VA_ARGS__)
#define mpp_logd(fmt, ...)  _mpp_log_l(MPP_LOG_DEBUG,   MODULE_TAG, fmt, NULL, ## __VA_ARGS__)
#define mpp_logv(fmt, ...)  _mpp_log_l(MPP_LOG_VERBOSE, MODULE_TAG, fmt, NULL, ## __VA_ARGS__)

#define mpp_logf_f(fmt, ...)  _mpp_log_l(MPP_LOG_FATAL,   MODULE_TAG, fmt, __FUNCTION__, ## __VA_ARGS__)
#define mpp_loge_f(fmt, ...)  _mpp_log_l(MPP_LOG_ERROR,   MODULE_TAG, fmt, __FUNCTION__, ## __VA_ARGS__)
#define mpp_logw_f(fmt, ...)  _mpp_log_l(MPP_LOG_WARN,    MODULE_TAG, fmt, __FUNCTION__, ## __VA_ARGS__)
#define mpp_logi_f(fmt, ...)  _mpp_log_l(MPP_LOG_INFO,    MODULE_TAG, fmt, __FUNCTION__, ## __VA_ARGS__)
#define mpp_logd_f(fmt, ...)  _mpp_log_l(MPP_LOG_DEBUG,   MODULE_TAG, fmt, __FUNCTION__, ## __VA_ARGS__)
#define mpp_logv_f(fmt, ...)  _mpp_log_l(MPP_LOG_VERBOSE, MODULE_TAG, fmt, __FUNCTION__, ## __VA_ARGS__)

#define mpp_logf_c(cond, fmt, ...)  do { if (cond) mpp_logf(fmt, ## __VA_ARGS__); } while (0)
#define mpp_loge_c(cond, fmt, ...)  do { if (cond) mpp_loge(fmt, ## __VA_ARGS__); } while (0)
#define mpp_logw_c(cond, fmt, ...)  do { if (cond) mpp_logw(fmt, ## __VA_ARGS__); } while (0)
#define mpp_logi_c(cond, fmt, ...)  do { if (cond) mpp_logi(fmt, ## __VA_ARGS__); } while (0)
#define mpp_logd_c(cond, fmt, ...)  do { if (cond) mpp_logd(fmt, ## __VA_ARGS__); } while (0)
#define mpp_logv_c(cond, fmt, ...)  do { if (cond) mpp_logv(fmt, ## __VA_ARGS__); } while (0)

#define mpp_logf_cf(cond, fmt, ...) do { if (cond) mpp_logf_f(fmt, ## __VA_ARGS__); } while (0)
#define mpp_loge_cf(cond, fmt, ...) do { if (cond) mpp_loge_f(fmt, ## __VA_ARGS__); } while (0)
#define mpp_logw_cf(cond, fmt, ...) do { if (cond) mpp_logw_f(fmt, ## __VA_ARGS__); } while (0)
#define mpp_logi_cf(cond, fmt, ...) do { if (cond) mpp_logi_f(fmt, ## __VA_ARGS__); } while (0)
#define mpp_logd_cf(cond, fmt, ...) do { if (cond) mpp_logd_f(fmt, ## __VA_ARGS__); } while (0)
#define mpp_logv_cf(cond, fmt, ...) do { if (cond) mpp_logv_f(fmt, ## __VA_ARGS__); } while (0)

/*
 * mpp runtime log system usage:
 * mpp_err is for error status message, it will print for sure.
 * mpp_log is for important message like open/close/reset/flush, it will print too.
 */

#define mpp_log(fmt, ...)   mpp_logi(fmt, ## __VA_ARGS__)
#define mpp_err(fmt, ...)   mpp_loge(fmt, ## __VA_ARGS__)

#define mpp_log_f(fmt, ...)  mpp_logi_f(fmt, ## __VA_ARGS__)
#define mpp_err_f(fmt, ...)  mpp_loge_f(fmt, ## __VA_ARGS__)

#define mpp_log_c(cond, fmt, ...)   do { if (cond) mpp_log(fmt, ## __VA_ARGS__); } while (0)
#define mpp_log_cf(cond, fmt, ...)  do { if (cond) mpp_log_f(fmt, ## __VA_ARGS__); } while (0)

#ifdef __cplusplus
extern "C" {
#endif

void _mpp_log_l(int level, const char *tag, const char *fmt, const char *func, ...);

void mpp_set_log_level(int level);
int mpp_get_log_level(void);

/* deprecated function */
void _mpp_log(const char *tag, const char *fmt, const char *func, ...);
void _mpp_err(const char *tag, const char *fmt, const char *func, ...);

#ifdef __cplusplus
}
#endif

#endif /*__MPP_LOG_H__*/
