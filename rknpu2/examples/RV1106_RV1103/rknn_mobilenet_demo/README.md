The following <TARGET_PLATFORM> is RV1106 or RV1103


# Arm Linux Demo

## build

modify `GCC_COMPILER` on `build-linux_<TARGET_PLATFORM>.sh` for target platform,

then execute

```
./build-linux_<TARGET_PLATFORM>.sh
```

## install

connect device and push build output into `/userdata`

```
adb push install/rknn_mobilenet_demo_Linux /userdata/
```

## run

```
adb shell
cd /userdata/rknn_mobilenet_demo_Linux/
```

```
export LD_LIBRARY_PATH=./lib
./rknn_mobilenet_demo model/<TARGET_PLATFORM>/mobilenet_v1.rknn model/dog_224x224.jpg
```
