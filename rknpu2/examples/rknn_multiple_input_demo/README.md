The following <TARGET_PLATFORM> represents RK3566_RK3568,RK3562 or RK3588.
# Aarch64 Linux Demo
## Build

modify `GCC_COMPILER` on `build-linux.sh` for target platform, then execute

```
./build-linux.sh
```

## Install

Copy install/rknn_multiple_input_demo_Linux to the devices under `/userdata`.

- If you use rockchip's evb board, you can use the following way:

Connect device and push the program and rknn model to `/userdata`

```
adb push install/rknn_multiple_input_demo_Linux /userdata/
```

- If your board has sshd service, you can use scp or other methods to copy the program and rknn model to the board.

## Run

```
adb shell
cd /userdata/rknn_multiple_input_demo_Linux/
```



```
export LD_LIBRARY_PATH=./lib
./rknn_multiple_input_demo model/<TARGET_PLATFORM>/multiple_input_demo.rknn model/input1.bin#model/input2.bin
```

# Android Demo
## Build

modify `ANDROID_NDK_PATH` on `build-android.sh` for target platform, then execute

```
./build-android.sh
```

## Install

connect device and push build output into `/data`

```
adb push install/rknn_multiple_input_demo_Android /data/
```

## Run

```
adb shell
cd /data/rknn_multiple_input_demo_Android/
```



```
export LD_LIBRARY_PATH=./lib
./rknn_multiple_input_demo model/<TARGET_PLATFORM>/multiple_input_demo.rknn model/input1.bin#model/input2.bin
```