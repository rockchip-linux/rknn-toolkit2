#ifndef __MPP_ENCODER_H__
#define __MPP_ENCODER_H__

#include "rockchip/mpp_frame.h"
#include "rockchip/rk_mpi.h"
#include <pthread.h>
#include <string.h>

typedef void (*MppEncoderFrameCallback)(void* userdata, const char* data, int size);

typedef struct
{
    RK_U32 width;
    RK_U32 height;
    RK_U32 hor_stride;
    RK_U32 ver_stride;
    MppFrameFormat fmt;
    MppCodingType type;

    RK_U32 osd_enable;
    RK_U32 osd_mode;
    RK_U32 split_mode;
    RK_U32 split_arg;
    RK_U32 split_out;

    RK_U32 user_data_enable;
    RK_U32 roi_enable;

    // rate control runtime parameter
    RK_S32 fps_in_flex;
    RK_S32 fps_in_den;
    RK_S32 fps_in_num;
    RK_S32 fps_out_flex;
    RK_S32 fps_out_den;
    RK_S32 fps_out_num;
    RK_S32 bps;
    RK_S32 bps_max;
    RK_S32 bps_min;
    RK_S32 rc_mode;
    RK_S32 gop_mode;
    RK_S32 gop_len;
    RK_S32 vi_len;

    /* general qp control */
    RK_S32 qp_init;
    RK_S32 qp_max;
    RK_S32 qp_max_i;
    RK_S32 qp_min;
    RK_S32 qp_min_i;
    RK_S32 qp_max_step; /* delta qp between each two P frame */
    RK_S32 qp_delta_ip; /* delta qp between I and P */
    RK_S32 qp_delta_vi; /* delta qp between vi and P */

    RK_U32 constraint_set;
    RK_U32 rotation;
    RK_U32 mirroring;
    RK_U32 flip;

    MppEncHeaderMode header_mode;
    MppEncSeiMode sei_mode;
} MppEncoderParams;

class MppEncoder {
  public:
    MppEncoder();
    ~MppEncoder();
    int Init(MppEncoderParams& params, void* userdata);
    int SetCallback(MppEncoderFrameCallback callback);
    int Encode(void* mpp_buf, char* enc_buf, int max_size);
    int GetHeader(char* enc_buf, int max_size);
    int Reset();
    void* ImportBuffer(int index, size_t size, int fd, int type);
    size_t GetFrameSize();
    void* GetInputFrameBuffer();
    int GetInputFrameBufferFd(void* mpp_buffer);
    void* GetInputFrameBufferAddr(void* mpp_buffer);
  private:
    int InitParams(MppEncoderParams& params);
    int SetupEncCfg();

    MppCtx mpp_ctx = NULL;
    MppApi* mpp_mpi = NULL;
    RK_S32 chn = 0;

    MppEncoderFrameCallback callback = NULL;

    // global flow control flag
    // RK_U32 frm_eos = 0;
    // RK_U32 pkt_eos = 0;
    // RK_U32 frm_pkt_cnt = 0;
    // RK_S32 frame_num = 0;
    // RK_S32 frame_count = 0;
    // RK_U64 stream_size = 0;

    /* encoder config set */
    MppEncCfg cfg = NULL;
    MppEncPrepCfg prep_cfg;
    MppEncRcCfg rc_cfg;
    MppEncCodecCfg codec_cfg;
    MppEncSliceSplit split_cfg;
    MppEncOSDPltCfg osd_plt_cfg;
    MppEncOSDPlt osd_plt;
    MppEncOSDData osd_data;
    // RoiRegionCfg    roi_region;
    MppEncROICfg roi_cfg;

    // input / output
    MppBufferGroup buf_grp = NULL;
    MppBuffer frm_buf = NULL;
    MppBuffer pkt_buf = NULL;
    MppBuffer md_info = NULL;

    // MppEncRoiCtx roi_ctx;

    // resources
    size_t header_size;
    size_t frame_size;
    size_t mdinfo_size;
    /* NOTE: packet buffer may overflow */
    size_t packet_size;

    MppEncoderParams enc_params;

    void* userdata = NULL;
};

#endif  //__MPP_ENCODER_H__
