# Instructions

There are two ways to use the RKNN API on the Android platform:

1. Directly link to librknnrt.so.
2. Link to librknn_api_android.so, which is implemented based on Android platform HIDL.

For Android devices that need to pass CTS/VTS testing, it is recommended to use the RKNN API implemented based on Android platform HIDL. If devices don't need to pass CTS/VTS testing, it is suggested to directly link and use librknnrt.so, which provides better performance due to shorter interface calling process.

The code for using the RKNN API implemented with Android HIDL can be found in the vendor/rockchip/hardware/interfaces/neuralnetworks directory of the RK3566_RK3568/RK3588 Android system SDK. After completing the Android system compilation, some NPU-related libraries will be generated (for applications, only librknn_api_android.so needs to be linked).

**This example is applicable to librknn_api_android.so.**

## Compilation

- Compile librknn_api_android.so

  First, download the RK3566_RK3568/RK3588 Android SDK, and in the root directory of the Android SDK, then execute following commands:

  ```
  source build/envsetup.sh
  lunch your target ## Choose according to your actual situation
  mmm vendor/rockchip/hardware/interfaces/neuralnetworks/ -j16
  ```

  The following files will be generated:

  ```
  /vendor/lib/librknn_api_android.so
  /vendor/lib/librknnhal_bridge.rockchip.so
  /vendor/lib64/librknn_api_android.so
  /vendor/lib64/librknnhal_bridge.rockchip.so
  /vendor/lib64/rockchip.hardware.neuralnetworks@1.0.so
  /vendor/lib64/rockchip.hardware.neuralnetworks@1.0-adapter-helper.so
  /vendor/lib64/hw/rockchip.hardware.neuralnetworks@1.0-impl.so
  /vendor/bin/hw/rockchip.hardware.neuralnetworks@1.0-service
  ```

- Compile this demo

  Copy $RKNPU2_SDK to the root directory of the Android SDK, and execute:

  ```
  mmm rknpu2/examples/librknn_api_android_demo
  ```

  The following file will be generated:

  vendor/bin/rknn_create_mem_demo

## Execution

- Push rknn_create_mem_demo to the /vendor/bin/ directory of the target device.
- Push the model to the /data/ directory of the target device.
- Make sure that the [rockchip.hardware.neuralnetworks@1.0-service](mailto:rockchip.hardware.neuralnetworks@1.0-service) is running on the device.

```
 rknn_create_mem_demo /data/model/RK3566_RK3568/mobilenet_v1.rknn /data/model/dog_224x224.jpg
```

## FAQ

- What should I do if the [rockchip.hardware.neuralnetworks@1.0-service](mailto:rockchip.hardware.neuralnetworks@1.0-service) is not running?

  If the service is not running, make sure that the vendor/rockchip/hardware/interfaces/neuralnetworks/ directory exists in the Android SDK, recompile the system firmware, and burn it onto the device. Please refer to the instructions for compiling the SDK firmware for specific steps.

- Encounter the error "sizeof(rknn_tensor_attr) != sizeof(::rockchip::hardware::neuralnetworks::V1_0::RKNNTensorAttr)"

  You need to update the vendor/rockchip/hardware/interfaces/neuralnetworks to the latest version.
