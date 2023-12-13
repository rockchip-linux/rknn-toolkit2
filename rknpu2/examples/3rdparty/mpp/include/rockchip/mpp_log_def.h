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

#ifndef __MPP_LOG_DEF_H__
#define __MPP_LOG_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MPP_LOG_UNKNOWN     0   /* internal use only                    */
#define MPP_LOG_FATAL       1   /* fatal error on aborting              */
#define MPP_LOG_ERROR       2   /* error log on unrecoverable failures  */
#define MPP_LOG_WARN        3   /* warning log on recoverable failures  */
#define MPP_LOG_INFO        4   /* Informational log                    */
#define MPP_LOG_DEBUG       5   /* Debug log                            */
#define MPP_LOG_VERBOSE     6   /* Verbose log                          */
#define MPP_LOG_SILENT      7   /* internal use only                    */

#ifdef __cplusplus
}
#endif

#endif /*__MPP_LOG_DEF_H__*/
