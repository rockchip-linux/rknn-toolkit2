package com.rockchip.gpadc.demo.rga;

public class HALDefine {
    // for RGA
    public static final int RK_FORMAT_RGBA_8888    = (0x0 << 8);
    public static final int RK_FORMAT_RGB_888      = (0x2 << 8);
    public static final int RK_FORMAT_YCrCb_420_SP = (0xe << 8);

    // for RGA
    public static final int IM_HAL_TRANSFORM_FLIP_H = (1 << 3); // 0x08

    // for camera comes with RK3588
    public static int CAMERA_PREVIEW_WIDTH = 1280;
    public static int CAMERA_PREVIEW_HEIGHT = 720;
}
