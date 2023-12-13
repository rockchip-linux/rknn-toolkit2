rknn_benchmark is used to test the performance of the rknn model. Please make sure that the cpu/ddr/npu has been clocked to the highest frequency before testing.

Usage:

./rknn_benchmark xxx.rknn [input_data]  [loop_count] [core_mask]

core_mask: 0: auto, 1: npu core1, 2: npu core2, 4:npu core3,

​                     3: npu core1&2,

​                     7: npu core1&2&3

Only RK3588 support core mask.

Such as:

```
./rknn_benchmark mobilenet_v1.rknn
./rknn_benchmark mobilenet_v1.rknn dog.jpg 10 3
./rknn_benchmark mobilenet_v1.rknn dog.npy 10 7
./rknn_benchmark xxx.rknn input1.npy#input2.npy
```


The following <TARGET_PLATFORM> represents RK3566_RK3568, RK3562 or RK3588

# Aarch64 Linux Demo
## Build

modify `GCC_COMPILER` on `build-linux_<TARGET_PLATFORM>.sh` for target platform,

then execute

```
./build-linux_<TARGET_PLATFORM>.sh
```

## Install

Copy install/rknn_benchmark_Linux to the devices under /userdata/.

- If you use rockchip's evb board, you can use the following way:

Connect device and push the program and rknn model to `/userdata`

```
adb push install/rknn_benchmark_Linux /userdata/
```

- If your board has sshd service, you can use scp or other methods to copy the program and rknn model to the board.

## Run

```
adb shell
cd /userdata/rknn_benchmark_Linux/
```

```
export LD_LIBRARY_PATH=./lib
./rknn_benchmark xxx.rknn
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
adb push install/rknn_benchmark_Android /data/
```

## Run

```
adb shell
cd /data/rknn_benchmark_Android/
```

```
export LD_LIBRARY_PATH=./lib
./rknn_benchmark xxx.rknn
```
