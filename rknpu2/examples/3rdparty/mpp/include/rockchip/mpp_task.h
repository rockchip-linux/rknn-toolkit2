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

#ifndef __MPP_TASK_H__
#define __MPP_TASK_H__

#include "mpp_meta.h"

/*
 * Advanced task flow
 * Advanced task flow introduces three concepts: port, task and item
 *
 * Port is from OpenMAX
 * Port has two type: input port and output port which are all for data transaction.
 * Port work like a queue. task will be dequeue from or enqueue to one port.
 * On input side user will dequeue task from input port, setup task and enqueue task
 * back to input port.
 * On output side user will dequeue task from output port, get the information from
 * and then enqueue task back to output port.
 *
 * Task indicates one transaction on the port.
 * Task has two working mode: async mode and sync mode
 * If mpp is work in sync mode on task enqueue function return the task has been done
 * If mpp is work in async mode on task enqueue function return the task is just put
 * on the task queue for process.
 * Task can carry different items. Task just like a container of items
 *
 * Item indicates MppPacket or MppFrame which is contained in one task
 */

/*
 * One mpp task queue has two ports: input and output
 *
 * The whole picture is:
 * Top layer mpp has two ports: mpp_input_port and mpp_output_port
 * But internally these two ports belongs to two task queue.
 * The mpp_input_port is the mpp_input_task_queue's input port.
 * The mpp_output_port is the mpp_output_task_queue's output port.
 *
 * Each port uses its task queue to communication
 */
typedef enum {
    MPP_PORT_INPUT,
    MPP_PORT_OUTPUT,
    MPP_PORT_BUTT,
} MppPortType;

/*
 * Advance task work flow mode:
 ******************************************************************************
 * 1. async mode (default_val)
 *
 * mpp_init(type, coding, MPP_WORK_ASYNC)
 *
 * input thread
 * a - poll(input)
 * b - dequeue(input, *task)
 * c - task_set_item(packet/frame)
 * d - enqueue(input, task)     // when enqueue return the task is not done yet
 *
 * output thread
 * a - poll(output)
 * b - dequeue(output, *task)
 * c - task_get_item(frame/packet)
 * d - enqueue(output, task)
 ******************************************************************************
 * 2. sync mode
 *
 * mpp_init(type, coding, MPP_WORK_SYNC)
 *
 * a - poll(input)
 * b - dequeue(input, *task)
 * c - task_set_item(packet/frame)
 * d - enqueue(task)            // when enqueue return the task is finished
 ******************************************************************************
 */
typedef enum {
    MPP_TASK_ASYNC,
    MPP_TASK_SYNC,
    MPP_TASK_WORK_MODE_BUTT,
} MppTaskWorkMode;

/*
 * Mpp port poll type
 *
 * MPP_POLL_BLOCK           - for block poll
 * MPP_POLL_NON_BLOCK       - for non-block poll
 * small than MPP_POLL_MAX  - for poll with timeout in ms
 * small than MPP_POLL_BUTT or larger than MPP_POLL_MAX is invalid value
 */
typedef enum {
    MPP_POLL_BUTT       = -2,
    MPP_POLL_BLOCK      = -1,
    MPP_POLL_NON_BLOCK  = 0,
    MPP_POLL_MAX        = 8000,
} MppPollType;

/*
 * Mpp timeout define
 * MPP_TIMEOUT_BLOCK            - for block poll
 * MPP_TIMEOUT_NON_BLOCK        - for non-block poll
 * small than MPP_TIMEOUT_MAX   - for poll with timeout in ms
 * small than MPP_TIMEOUT_BUTT or larger than MPP_TIMEOUT_MAX is invalid value
 */
#define MPP_TIMEOUT_BUTT        (-2L)
#define MPP_TIMEOUT_BLOCK       (-1L)
#define MPP_TIMEOUT_NON_BLOCK   (0L)
#define MPP_TIMEOUT_MAX         (8000L)

/*
 * MppTask is descriptor of a task which send to mpp for process
 * mpp can support different type of work mode, for example:
 *
 * decoder:
 *
 * 1. typical decoder mode:
 * input    - MppPacket     (normal cpu buffer, need cpu copy)
 * output   - MppFrame      (ion/drm buffer in external/internal mode)
 * 2. secure decoder mode:
 * input    - MppPacket     (externel ion/drm buffer, cpu can not access)
 * output   - MppFrame      (ion/drm buffer in external/internal mode, cpu can not access)
 *
 * interface usage:
 *
 * typical flow
 * input side:
 * task_dequeue(ctx, PORT_INPUT, &task);
 * task_put_item(task, MODE_INPUT, packet)
 * task_enqueue(ctx, PORT_INPUT, task);
 * output side:
 * task_dequeue(ctx, PORT_OUTPUT, &task);
 * task_get_item(task, MODE_OUTPUT, &frame)
 * task_enqueue(ctx, PORT_OUTPUT, task);
 *
 * secure flow
 * input side:
 * task_dequeue(ctx, PORT_INPUT, &task);
 * task_put_item(task, MODE_INPUT, packet)
 * task_put_item(task, MODE_OUTPUT, frame)  // buffer will be specified here
 * task_enqueue(ctx, PORT_INPUT, task);
 * output side:
 * task_dequeue(ctx, PORT_OUTPUT, &task);
 * task_get_item(task, MODE_OUTPUT, &frame)
 * task_enqueue(ctx, PORT_OUTPUT, task);
 *
 * encoder:
 *
 * 1. typical encoder mode:
 * input    - MppFrame      (ion/drm buffer in external mode)
 * output   - MppPacket     (normal cpu buffer, need cpu copy)
 * 2. user input encoder mode:
 * input    - MppFrame      (normal cpu buffer, need to build hardware table for this buffer)
 * output   - MppPacket     (normal cpu buffer, need cpu copy)
 * 3. secure encoder mode:
 * input    - MppFrame      (ion/drm buffer in external mode, cpu can not access)
 * output   - MppPacket     (externel ion/drm buffer, cpu can not access)
 *
 * typical / user input flow
 * input side:
 * task_dequeue(ctx, PORT_INPUT, &task);
 * task_put_item(task, MODE_INPUT, frame)
 * task_enqueue(ctx, PORT_INPUT, task);
 * output side:
 * task_dequeue(ctx, PORT_OUTPUT, &task);
 * task_get_item(task, MODE_OUTPUT, &packet)
 * task_enqueue(ctx, PORT_OUTPUT, task);
 *
 * secure flow
 * input side:
 * task_dequeue(ctx, PORT_INPUT, &task);
 * task_put_item(task, MODE_OUTPUT, packet)  // buffer will be specified here
 * task_put_item(task, MODE_INPUT, frame)
 * task_enqueue(ctx, PORT_INPUT, task);
 * output side:
 * task_dequeue(ctx, PORT_OUTPUT, &task);
 * task_get_item(task, MODE_OUTPUT, &packet)
 * task_get_item(task, MODE_OUTPUT, &frame)
 * task_enqueue(ctx, PORT_OUTPUT, task);
 *
 * NOTE: this flow can specify the output frame. User will setup both intput frame and output packet
 * buffer at the input side. Then at output side when user gets a finished task user can get the output
 * packet and corresponding released input frame.
 *
 * image processing
 *
 * 1. typical image process mode:
 * input    - MppFrame      (ion/drm buffer in external mode)
 * output   - MppFrame      (ion/drm buffer in external mode)
 *
 * typical / user input flow
 * input side:
 * task_dequeue(ctx, PORT_INPUT, &task);
 * task_put_item(task, MODE_INPUT, frame)
 * task_enqueue(ctx, PORT_INPUT, task);
 * output side:
 * task_dequeue(ctx, PORT_OUTPUT, &task);
 * task_get_item(task, MODE_OUTPUT, &frame)
 * task_enqueue(ctx, PORT_OUTPUT, task);
 */
/* NOTE: use index rather then handle to descripbe task */

#ifdef __cplusplus
extern "C" {
#endif

MPP_RET mpp_task_meta_set_s32(MppTask task, MppMetaKey key, RK_S32 val);
MPP_RET mpp_task_meta_set_s64(MppTask task, MppMetaKey key, RK_S64 val);
MPP_RET mpp_task_meta_set_ptr(MppTask task, MppMetaKey key, void  *val);
MPP_RET mpp_task_meta_set_frame (MppTask task, MppMetaKey key, MppFrame  frame);
MPP_RET mpp_task_meta_set_packet(MppTask task, MppMetaKey key, MppPacket packet);
MPP_RET mpp_task_meta_set_buffer(MppTask task, MppMetaKey key, MppBuffer buffer);

MPP_RET mpp_task_meta_get_s32(MppTask task, MppMetaKey key, RK_S32 *val, RK_S32 default_val);
MPP_RET mpp_task_meta_get_s64(MppTask task, MppMetaKey key, RK_S64 *val, RK_S64 default_val);
MPP_RET mpp_task_meta_get_ptr(MppTask task, MppMetaKey key, void  **val, void  *default_val);
MPP_RET mpp_task_meta_get_frame (MppTask task, MppMetaKey key, MppFrame  *frame);
MPP_RET mpp_task_meta_get_packet(MppTask task, MppMetaKey key, MppPacket *packet);
MPP_RET mpp_task_meta_get_buffer(MppTask task, MppMetaKey key, MppBuffer *buffer);

#ifdef __cplusplus
}
#endif

#endif /*__MPP_QUEUE_H__*/
