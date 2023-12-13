The following <TARGET_PLATFORM> represents RK3566_RK3568, RK3562 or RK3588.
# Aarch64 Linux Demo
## Build

modify `GCC_COMPILER` on `build-linux_<TARGET_PLATFORM>.sh` for target platform, then execute

```
./build-linux_<TARGET_PLATFORM>.sh
```

## Install

Copy install/rknn_api_demo_Linux and ../rknn_mobilenet_demo/model/ to the devices.

- If you use rockchip's evb board, you can use the following way:

Connect device and push the program and rknn model to `/userdata`

```
adb push install/rknn_api_demo_Linux /userdata/
adb push ../rknn_mobilenet_demo/model/ /userdata/rknn_api_demo_Linux
```

- If your board has sshd service, you can use scp or other methods to copy the program and rknn model to the board.

## Run

```
adb shell
cd /userdata/rknn_api_demo_Linux/
```

```
export LD_LIBRARY_PATH=./lib
./rknn_create_mem_demo model/<TARGET_PLATFORM>/mobilenet_v1.rknn model/dog_224x224.jpg
./rknn_create_mem_with_rga_demo model/<TARGET_PLATFORM>/mobilenet_v1.rknn model/dog_224x224.jpg
```

# Android Demo
## Build

modify `ANDROID_NDK_PATH` on `build-android_<TARGET_PLATFORM>.sh` for target platform, then execute

```
./build-android_<TARGET_PLATFORM>.sh
```

## Install

connect device and push build output into `/data`

```
adb push install/rknn_api_demo_Android /data/
adb push ../rknn_mobilenet_demo/model/ /userdata/rknn_api_demo_Android
```

## Run

```
adb shell
cd /data/rknn_api_demo_Android/
```

```
export LD_LIBRARY_PATH=./lib
./rknn_create_mem_demo model/<TARGET_PLATFORM>/mobilenet_v1.rknn model/dog_224x224.jpg
./rknn_create_mem_with_rga_demo model/<TARGET_PLATFORM>/mobilenet_v1.rknn model/dog_224x224.jpg
./rknn_with_mmz_demo model/<TARGET_PLATFORM>/mobilenet_v1.rknn model/dog_224x224.jpg
./rknn_set_internal_mem_from_fd_demo model/<TARGET_PLATFORM>/mobilenet_v1.rknn model/dog_224x224.jpg
./rknn_set_internal_mem_from_phy_demo model/<TARGET_PLATFORM>/mobilenet_v1.rknn model/dog_224x224.jpg
```

# Note
 - You may need to update libmpimmz.so and its header file of this project according to the implementation of MMZ in the system.
 - You may need to update librga.so and its header file of this project according to the implementation of RGA in the system. https://github.com/airockchip/librga.
    For rk3562, the librga version need to be 1.9.1 or higher.
 - You may need to use r19c or older version of ndk for compiling with MMZ related demo.