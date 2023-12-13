#ifndef __MPP_DECODER_H__
#define __MPP_DECODER_H__

#include <string.h>
#include "rockchip/rk_mpi.h"
#include "rockchip/mpp_frame.h"
#include <string.h>
#include <pthread.h>

#define MPI_DEC_STREAM_SIZE         (SZ_4K)
#define MPI_DEC_LOOP_COUNT          4
#define MAX_FILE_NAME_LENGTH        256

typedef void (*MppDecoderFrameCallback)(void* userdata, int width_stride, int height_stride, int width, int height, int format, int fd, void* data);

typedef struct
{
    MppCtx          ctx;
    MppApi          *mpi;
    RK_U32          eos;
    char            *buf;

    MppBufferGroup  frm_grp;
    MppBufferGroup  pkt_grp;
    MppPacket       packet;
    size_t          packet_size;
    MppFrame        frame;

    RK_S32          frame_count;
    RK_S32          frame_num;
    size_t          max_usage;
} MpiDecLoopData;

class MppDecoder
{
public:
    MppCtx mpp_ctx          = NULL;
    MppApi *mpp_mpi         = NULL;
    MppDecoder();
    ~MppDecoder();
    int Init(int video_type, int fps, void* userdata);
    int SetCallback(MppDecoderFrameCallback callback);
    int Decode(uint8_t* pkt_data, int pkt_size, int pkt_eos);
    int Reset();
private:
    // base flow context
    MpiCmd mpi_cmd      = MPP_CMD_BASE;
    MppParam mpp_param1      = NULL;
    RK_U32 need_split   = 1;
    RK_U32 width_mpp       ;
    RK_U32 height_mpp      ;
    MppCodingType mpp_type;
    size_t packet_size  = 2400*1300*3/2;
    MpiDecLoopData loop_data;
    // bool vedio_type;//判断vedio是h264/h265
    MppPacket packet = NULL;
    MppFrame  frame  = NULL;
    pthread_t th;
    MppDecoderFrameCallback callback;
    int fps = -1;
    unsigned long last_frame_time_ms = 0;

    void* userdata = NULL;
};

size_t mpp_frame_get_buf_size(const MppFrame s);
size_t mpp_buffer_group_usage(MppBufferGroup group);

#endif //__MPP_DECODER_H__
