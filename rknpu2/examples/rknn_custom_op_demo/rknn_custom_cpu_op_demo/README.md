# RKNN Custom CPU OP Demo
This demo is an example of showing how to implement custom cpu OP on C demo for RKNN model, taking 'Sigmoid' as an example. Replace ONNX OP Sigmoid with custom OP cstSigmoid. Users can refer to [RKNN-Toolkit2 example](https://github.com/airockchip/rknn-toolkit2/tree/master/rknn-toolkit2/examples/functions/custom_op/non-onnx_standard) for how to replace ONNX OP and convert ONNX model to RKNN model with registered custom op.

## Model Source
The model and YOLOX version used in this example come from the following open source project:
https://github.com/airockchip/YOLOX

Download link:

[yolox_s.onnx](https://ftrg.zbox.filez.com/v2/delivery/data/ec1c6f44f8c24155875ac5bce7aa6b3c/examples/yolox/yolox_s.onnx)

The converted RKNN models of platforms RK3562, RK356X and RK3588 are located in model folder.

# Aarch64 Linux Demo

## Build

modify `GCC_COMPILER` on `build-linux_<TARGET_PLATFORM>.sh` for target platform,

then execute

```
./build-linux_<TARGET_PLATFORM>.sh
```

## *Install*

Copy install/rknn_custom_cpu_op_demo_Linux  to the devices under /userdata/.

- If you use rockchip's evb board, you can use the following way:

Connect device and push the program and RKNN model to `/userdata`

```
adb push install/rknn_custom_cpu_op_demo_Linux/ /data/
```

- If board has sshd service, users can use scp or other methods to copy the program and RKNN model to the board.

## Run

```
adb shell
cd /data/rknn_custom_cpu_op_demo_Linux
```

```shell
export LD_LIBRARY_PATH=./lib
./rknn_custom_cpu_op_demo model/<TARGET_PLATFORM>/yolox_s_custom_sigmoid.rknn model/test_image.jpg 1
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
 adb push install/rknn_custom_cpu_op_demo_Android/ /data/
```

## Run

```
adb shell
cd /data/rknn_custom_cpu_op_demo_Android/
```

then exporting LD_LIBRARY_PATH to the local path of lib.

```
export LD_LIBRARY_PATH=./lib
./rknn_custom_cpu_op_demo model/<TARGET_PLATFORM>/yolox_s_custom_sigmoid.rknn model/test_image.jpg 1
```

Note: Different platforms, different versions of tools and drivers may have slightly different results.

## Results

Take RKN3588 platform as an example, its results are as shown:

bus @ (92 132 558 438) 0.933  
person @ (104 237 221 534) 0.901  
person @ (211 240 285 504) 0.901  
person @ (472 248 559 525) 0.774  
person @ (79 329 118 512) 0.347  
bicycle @ (80 420 96 515) 0.253