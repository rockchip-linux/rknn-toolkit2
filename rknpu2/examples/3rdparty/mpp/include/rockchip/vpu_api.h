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

#ifndef __VPU_API_H__
#define __VPU_API_H__

#include "rk_type.h"
#include "mpp_err.h"

/**
 * @brief rockchip media process interface
 */

#define VPU_API_NOPTS_VALUE          (0x8000000000000000LL)

/*
 * bit definition of ColorType in structure VPU_FRAME
 */
#define VPU_OUTPUT_FORMAT_TYPE_MASK                 (0x0000ffff)
#define VPU_OUTPUT_FORMAT_ARGB8888                  (0x00000000)
#define VPU_OUTPUT_FORMAT_ABGR8888                  (0x00000001)
#define VPU_OUTPUT_FORMAT_RGB888                    (0x00000002)
#define VPU_OUTPUT_FORMAT_RGB565                    (0x00000003)
#define VPU_OUTPUT_FORMAT_RGB555                    (0x00000004)
#define VPU_OUTPUT_FORMAT_YUV420_SEMIPLANAR         (0x00000005)
#define VPU_OUTPUT_FORMAT_YUV420_PLANAR             (0x00000006)
#define VPU_OUTPUT_FORMAT_YUV422                    (0x00000007)
#define VPU_OUTPUT_FORMAT_YUV444                    (0x00000008)
#define VPU_OUTPUT_FORMAT_YCH420                    (0x00000009)
#define VPU_OUTPUT_FORMAT_BIT_MASK                  (0x000f0000)
#define VPU_OUTPUT_FORMAT_BIT_8                     (0x00000000)
#define VPU_OUTPUT_FORMAT_BIT_10                    (0x00010000)
#define VPU_OUTPUT_FORMAT_BIT_12                    (0x00020000)
#define VPU_OUTPUT_FORMAT_BIT_14                    (0x00030000)
#define VPU_OUTPUT_FORMAT_BIT_16                    (0x00040000)
#define VPU_OUTPUT_FORMAT_FBC_MASK                  (0x00f00000)
#define VPU_OUTPUT_FORMAT_FBC_AFBC_V1               (0x00100000)
#define VPU_OUTPUT_FORMAT_FBC_AFBC_V2               (0x00200000)
#define VPU_OUTPUT_FORMAT_DYNCRANGE_MASK            (0x0f000000)
#define VPU_OUTPUT_FORMAT_DYNCRANGE_SDR             (0x00000000)
#define VPU_OUTPUT_FORMAT_DYNCRANGE_HDR10           (0x01000000)
#define VPU_OUTPUT_FORMAT_DYNCRANGE_HDR_HLG         (0x02000000)
#define VPU_OUTPUT_FORMAT_DYNCRANGE_HDR_DOLBY       (0x03000000)

/**
 * @brief input picture type
 */
typedef enum {
    ENC_INPUT_YUV420_PLANAR = 0,              /**< YYYY... UUUU... VVVV */
    ENC_INPUT_YUV420_SEMIPLANAR = 1,          /**< YYYY... UVUVUV...    */
    ENC_INPUT_YUV422_INTERLEAVED_YUYV = 2,    /**< YUYVYUYV...          */
    ENC_INPUT_YUV422_INTERLEAVED_UYVY = 3,    /**< UYVYUYVY...          */
    ENC_INPUT_RGB565 = 4,                     /**< 16-bit RGB           */
    ENC_INPUT_BGR565 = 5,                     /**< 16-bit RGB           */
    ENC_INPUT_RGB555 = 6,                     /**< 15-bit RGB           */
    ENC_INPUT_BGR555 = 7,                     /**< 15-bit RGB           */
    ENC_INPUT_RGB444 = 8,                     /**< 12-bit RGB           */
    ENC_INPUT_BGR444 = 9,                     /**< 12-bit RGB           */
    ENC_INPUT_RGB888 = 10,                    /**< 24-bit RGB           */
    ENC_INPUT_BGR888 = 11,                    /**< 24-bit RGB           */
    ENC_INPUT_RGB101010 = 12,                 /**< 30-bit RGB           */
    ENC_INPUT_BGR101010 = 13                  /**< 30-bit RGB           */
} EncInputPictureType;

typedef enum VPU_API_CMD {
    VPU_API_ENC_SETCFG,
    VPU_API_ENC_GETCFG,
    VPU_API_ENC_SETFORMAT,
    VPU_API_ENC_SETIDRFRAME,

    VPU_API_ENABLE_DEINTERLACE,
    VPU_API_SET_VPUMEM_CONTEXT,
    VPU_API_USE_PRESENT_TIME_ORDER,
    VPU_API_SET_DEFAULT_WIDTH_HEIGH,
    VPU_API_SET_INFO_CHANGE,
    VPU_API_USE_FAST_MODE,
    VPU_API_DEC_GET_STREAM_COUNT,
    VPU_API_GET_VPUMEM_USED_COUNT,
    VPU_API_GET_FRAME_INFO,
    VPU_API_SET_OUTPUT_BLOCK,
    VPU_API_GET_EOS_STATUS,
    VPU_API_SET_OUTPUT_MODE,

    /* get sps/pps header */
    VPU_API_GET_EXTRA_INFO = 0x200,

    VPU_API_SET_IMMEDIATE_OUT = 0x1000,
    VPU_API_SET_PARSER_SPLIT_MODE,          /* NOTE: should control before init */
    VPU_API_DEC_OUT_FRM_STRUCT_TYPE,
    VPU_API_DEC_EN_THUMBNAIL,
    VPU_API_DEC_EN_HDR_META,
    VPU_API_DEC_EN_MVC,
    VPU_API_DEC_EN_FBC_HDR_256_ODD,

    VPU_API_ENC_VEPU22_START = 0x2000,
    VPU_API_ENC_SET_VEPU22_CFG,
    VPU_API_ENC_GET_VEPU22_CFG,
    VPU_API_ENC_SET_VEPU22_CTU_QP,
    VPU_API_ENC_SET_VEPU22_ROI,

    VPU_API_ENC_MPP        = 0x3000,
    VPU_API_ENC_MPP_SETCFG,
    VPU_API_ENC_MPP_GETCFG,

    /* mlvec dynamic configure */
    VPU_API_ENC_MLVEC_CFG = 0x4000,
    VPU_API_ENC_SET_MAX_TID,
    VPU_API_ENC_SET_MARK_LTR,
    VPU_API_ENC_SET_USE_LTR,
    VPU_API_ENC_SET_FRAME_QP,
    VPU_API_ENC_SET_BASE_LAYER_PID,
} VPU_API_CMD;

typedef struct {
    RK_U32   TimeLow;
    RK_U32   TimeHigh;
} TIME_STAMP;

typedef struct {
    RK_U32   CodecType;
    RK_U32   ImgWidth;
    RK_U32   ImgHeight;
    RK_U32   ImgHorStride;
    RK_U32   ImgVerStride;
    RK_U32   BufSize;
} VPU_GENERIC;

typedef struct VPUMem {
    RK_U32  phy_addr;
    RK_U32 *vir_addr;
    RK_U32  size;
    RK_U32 *offset;
} VPUMemLinear_t;

typedef struct tVPU_FRAME {
    RK_U32              FrameBusAddr[2];    // 0: Y address; 1: UV address;
    RK_U32              FrameWidth;         // buffer horizontal stride
    RK_U32              FrameHeight;        // buffer vertical   stride
    RK_U32              OutputWidth;        // deprecated
    RK_U32              OutputHeight;       // deprecated
    RK_U32              DisplayWidth;       // valid width  for display
    RK_U32              DisplayHeight;      // valid height for display
    RK_U32              CodingType;
    RK_U32              FrameType;          // frame; top_field_first; bot_field_first
    RK_U32              ColorType;
    RK_U32              DecodeFrmNum;
    TIME_STAMP          ShowTime;
    RK_U32              ErrorInfo;          // error information
    RK_U32              employ_cnt;
    VPUMemLinear_t      vpumem;
    struct tVPU_FRAME  *next_frame;
    union {
        struct {
            RK_U32      Res0[2];
            struct {
                RK_U32      ColorPrimaries : 8;
                RK_U32      ColorTransfer  : 8;
                RK_U32      ColorCoeffs    : 8;
                RK_U32      ColorRange     : 1;
                RK_U32      Res1           : 7;
            };

            RK_U32      Res2;
        };

        RK_U32          Res[4];
    };
} VPU_FRAME;

typedef struct FrameThumbInfo {
    RK_U32      enable;
    RK_U32      yOffset;
    RK_U32      uvOffset;
} FrameThumbInfo_t;

typedef struct FrameHdrInfo {
    RK_U32      isHdr;
    RK_U32      offset;
    RK_U32      size;
} FrameHdrInfo_t;

typedef struct VideoFrame {
    VPU_FRAME        vpuFrame;
    FrameThumbInfo_t thumbInfo;
    FrameHdrInfo_t   hdrInfo;
    RK_U32           viewId;
    RK_U32           reserved[16];
} VideoFrame_t;

typedef struct VideoPacket {
    RK_S64 pts;                /* with unit of us*/
    RK_S64 dts;                /* with unit of us*/
    RK_U8 *data;
    RK_S32 size;
    RK_U32 capability;
    RK_U32 nFlags;
} VideoPacket_t;

typedef struct DecoderOut {
    RK_U8 *data;
    RK_U32 size;
    RK_S64 timeUs;
    RK_S32 nFlags;
} DecoderOut_t;

typedef struct ParserOut {
    RK_U8 *data;
    RK_U32 size;
    RK_S64 timeUs;
    RK_U32 nFlags;
    RK_U32 width;
    RK_U32 height;
} ParserOut_t;

typedef struct EncInputStream {
    RK_U8 *buf;
    RK_S32 size;
    RK_U32 bufPhyAddr;
    RK_S64 timeUs;
    RK_U32 nFlags;
} EncInputStream_t;

typedef struct EncoderOut {
    RK_U8 *data;
    RK_S32 size;
    RK_S64 timeUs;
    RK_S32 keyFrame;

} EncoderOut_t;

/*
 * @brief Enumeration used to define the possible video compression codings.
 * @note  This essentially refers to file extensions. If the coding is
 *        being used to specify the ENCODE type, then additional work
 *        must be done to configure the exact flavor of the compression
 *        to be used.  For decode cases where the user application can
 *        not differentiate between MPEG-4 and H.264 bit streams, it is
 *        up to the codec to handle this.
 *
 *        sync with the omx_video.h
 */
typedef enum OMX_RK_VIDEO_CODINGTYPE {
    OMX_RK_VIDEO_CodingUnused,                          /**< Value when coding is N/A */
    OMX_RK_VIDEO_CodingAutoDetect,                      /**< Autodetection of coding type */
    OMX_RK_VIDEO_CodingMPEG2,                           /**< AKA: H.262 */
    OMX_RK_VIDEO_CodingH263,                            /**< H.263 */
    OMX_RK_VIDEO_CodingMPEG4,                           /**< MPEG-4 */
    OMX_RK_VIDEO_CodingWMV,                             /**< Windows Media Video (WMV1,WMV2,WMV3)*/
    OMX_RK_VIDEO_CodingRV,                              /**< all versions of Real Video */
    OMX_RK_VIDEO_CodingAVC,                             /**< H.264/AVC */
    OMX_RK_VIDEO_CodingMJPEG,                           /**< Motion JPEG */
    OMX_RK_VIDEO_CodingVP8,                             /**< VP8 */
    OMX_RK_VIDEO_CodingVP9,                             /**< VP9 */
    OMX_RK_VIDEO_CodingVC1 = 0x01000000,                /**< Windows Media Video (WMV1,WMV2,WMV3)*/
    OMX_RK_VIDEO_CodingFLV1,                            /**< Sorenson H.263 */
    OMX_RK_VIDEO_CodingDIVX3,                           /**< DIVX3 */
    OMX_RK_VIDEO_CodingVP6,
    OMX_RK_VIDEO_CodingHEVC,                            /**< H.265/HEVC */
    OMX_RK_VIDEO_CodingAVSPLUS,                         /**< AVS+ profile 0x48 */
    OMX_RK_VIDEO_CodingAVS,                             /**< AVS  profile 0x20 */
    OMX_RK_VIDEO_CodingAVS2,                            /**< AVS2 */
    OMX_RK_VIDEO_CodingAV1,                             /**< av1 */
    OMX_RK_VIDEO_CodingKhronosExtensions = 0x6F000000,  /**< Reserved region for introducing Khronos Standard Extensions */
    OMX_RK_VIDEO_CodingVendorStartUnused = 0x7F000000,  /**< Reserved region for introducing Vendor Extensions */
    OMX_RK_VIDEO_CodingMax = 0x7FFFFFFF
} OMX_RK_VIDEO_CODINGTYPE;

typedef enum CODEC_TYPE {
    CODEC_NONE,
    CODEC_DECODER,
    CODEC_ENCODER,
    CODEC_BUTT,
} CODEC_TYPE;

typedef enum VPU_API_ERR {
    VPU_API_OK                      = 0,
    VPU_API_ERR_UNKNOW              = -1,
    VPU_API_ERR_BASE                = -1000,
    VPU_API_ERR_LIST_STREAM         = VPU_API_ERR_BASE - 1,
    VPU_API_ERR_INIT                = VPU_API_ERR_BASE - 2,
    VPU_API_ERR_VPU_CODEC_INIT      = VPU_API_ERR_BASE - 3,
    VPU_API_ERR_STREAM              = VPU_API_ERR_BASE - 4,
    VPU_API_ERR_FATAL_THREAD        = VPU_API_ERR_BASE - 5,
    VPU_API_EOS_STREAM_REACHED      = VPU_API_ERR_BASE - 11,

    VPU_API_ERR_BUTT,
} VPU_API_ERR;

typedef enum VPU_FRAME_ERR {
    VPU_FRAME_ERR_UNKNOW           = 0x0001,
    VPU_FRAME_ERR_UNSUPPORT        = 0x0002,

} VPU_FRAME_ERR;

typedef struct EncParameter {
    RK_S32 width;
    RK_S32 height;
    RK_S32 rc_mode;                 /* 0 - CQP mode; 1 - CBR mode; 2 - FIXQP mode*/
    RK_S32 bitRate;                 /* target bitrate */
    RK_S32 framerate;
    RK_S32 qp;
    RK_S32 enableCabac;
    RK_S32 cabacInitIdc;
    RK_S32 format;
    RK_S32 intraPicRate;
    RK_S32 framerateout;
    RK_S32 profileIdc;
    RK_S32 levelIdc;
    RK_S32 reserved[3];
} EncParameter_t;

typedef struct EXtraCfg {
    RK_S32 vc1extra_size;
    RK_S32 vp6codeid;
    RK_S32 tsformat;
    RK_U32 ori_vpu; /* use origin vpu framework */
    /* below used in decode */
    RK_U32 mpp_mode;     /* use mpp framework */
    RK_U32 bit_depth;    /* 8 or 10 bit */
    RK_U32 yuv_format;   /* 0:420 1:422 2:444 */
    RK_U32 reserved[16];
} EXtraCfg_t;

/**
 * @brief vpu function interface
 */
typedef struct VpuCodecContext {
    void* vpuApiObj;

    CODEC_TYPE codecType;
    OMX_RK_VIDEO_CODINGTYPE videoCoding;

    RK_U32 width;
    RK_U32 height;
    void  *extradata;
    RK_S32 extradata_size;

    RK_U8  enableparsing;

    RK_S32 no_thread;
    EXtraCfg_t extra_cfg;

    void* private_data;

    /*
     ** 1: error state(not working)  0: working
    */
    RK_S32 decoder_err;


    /**
     * Allocate and initialize an VpuCodecContext.
     *
     * @param ctx The context of vpu api, allocated in this function.
     * @param extraData The extra data of codec, some codecs need / can
     *        use extradata like Huffman tables, also live VC1 codec can
     *        use extradata to initialize itself.
     * @param extra_size The size of extra data.
     *
     * @return 0 for init success, others for failure.
     * @note check whether ctx has been allocated success after you do init.
     */
    RK_S32 (*init)(struct VpuCodecContext *ctx, RK_U8 *extraData, RK_U32 extra_size);
    /**
     * @brief both send video stream packet to decoder and get video frame from
     *        decoder at the same time
     * @param ctx The context of vpu codec
     * @param pkt[in] Stream to be decoded
     * @param aDecOut[out] Decoding frame
     * @return 0 for decode success, others for failure.
     */
    RK_S32 (*decode)(struct VpuCodecContext *ctx, VideoPacket_t *pkt, DecoderOut_t *aDecOut);
    /**
     * @brief both send video frame to encoder and get encoded video stream from
     *        encoder at the same time.
     * @param ctx The context of vpu codec
     * @param aEncInStrm[in] Frame to be encoded
     * @param aEncOut[out] Encoding stream
     * @return 0 for encode success, others for failure.
     */
    RK_S32 (*encode)(struct VpuCodecContext *ctx, EncInputStream_t *aEncInStrm, EncoderOut_t *aEncOut);
    /**
     * @brief flush codec while do fast forward playing.
     * @param ctx The context of vpu codec
     * @return 0 for flush success, others for failure.
     */
    RK_S32 (*flush)(struct VpuCodecContext *ctx);
    RK_S32 (*control)(struct VpuCodecContext *ctx, VPU_API_CMD cmdType, void* param);
    /**
     * @brief send video stream packet to decoder only, async interface
     * @param ctx The context of vpu codec
     * @param pkt Stream to be decoded
     * @return 0 for success, others for failure.
     */
    RK_S32 (*decode_sendstream)(struct VpuCodecContext *ctx, VideoPacket_t *pkt);
    /**
     * @brief get video frame from decoder only, async interface
     * @param ctx The context of vpu codec
     * @param aDecOut Decoding frame
     * @return 0 for success, others for failure.
     */
    RK_S32 (*decode_getframe)(struct VpuCodecContext *ctx, DecoderOut_t *aDecOut);
    /**
     * @brief send video frame to encoder only, async interface
     * @param ctx The context of vpu codec
     * @param aEncInStrm Frame to be encoded
     * @return 0 for success, others for failure.
     */
    RK_S32 (*encoder_sendframe)(struct VpuCodecContext *ctx, EncInputStream_t *aEncInStrm);
    /**
     * @brief get encoded video packet from encoder only, async interface
     * @param ctx The context of vpu codec
     * @param aEncOut Encoding stream
     * @return 0 for success, others for failure.
     */
    RK_S32 (*encoder_getstream)(struct VpuCodecContext *ctx, EncoderOut_t *aEncOut);
} VpuCodecContext_t;

/* allocated vpu codec context */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief open context of vpu
 * @param ctx pointer of vpu codec context
 */
RK_S32 vpu_open_context(struct VpuCodecContext **ctx);
/**
 * @brief close context of vpu
 * @param ctx pointer of vpu codec context
 */
RK_S32 vpu_close_context(struct VpuCodecContext **ctx);

#ifdef __cplusplus
}
#endif

/*
 * vpu_mem api
 */
#define vpu_display_mem_pool_FIELDS \
    RK_S32 (*commit_hdl)(vpu_display_mem_pool *p, RK_S32 hdl, RK_S32 size); \
    void* (*get_free)(vpu_display_mem_pool *p); \
    RK_S32 (*inc_used)(vpu_display_mem_pool *p, void *hdl); \
    RK_S32 (*put_used)(vpu_display_mem_pool *p, void *hdl); \
    RK_S32 (*reset)(vpu_display_mem_pool *p); \
    RK_S32 (*get_unused_num)(vpu_display_mem_pool *p); \
    RK_S32 buff_size;\
    float version; \
    RK_S32 res[18];

typedef struct vpu_display_mem_pool vpu_display_mem_pool;

struct vpu_display_mem_pool {
    vpu_display_mem_pool_FIELDS
};

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * vpu memory handle interface
 */
RK_S32 VPUMemJudgeIommu(void);
RK_S32 VPUMallocLinear(VPUMemLinear_t *p, RK_U32 size);
RK_S32 VPUFreeLinear(VPUMemLinear_t *p);
RK_S32 VPUMemDuplicate(VPUMemLinear_t *dst, VPUMemLinear_t *src);
RK_S32 VPUMemLink(VPUMemLinear_t *p);
RK_S32 VPUMemFlush(VPUMemLinear_t *p);
RK_S32 VPUMemClean(VPUMemLinear_t *p);
RK_S32 VPUMemInvalidate(VPUMemLinear_t *p);
RK_S32 VPUMemGetFD(VPUMemLinear_t *p);
RK_S32 VPUMallocLinearFromRender(VPUMemLinear_t *p, RK_U32 size, void *ctx);

/*
 * vpu memory allocator and manager interface
 */
vpu_display_mem_pool* open_vpu_memory_pool(void);
void close_vpu_memory_pool(vpu_display_mem_pool *p);
int create_vpu_memory_pool_allocator(vpu_display_mem_pool **ipool, int num, int size);
void release_vpu_memory_pool_allocator(vpu_display_mem_pool *ipool);

#ifdef __cplusplus
}
#endif

#endif /*__VPU_API_H__*/
