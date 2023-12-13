# 说明
Android平台有两种方式来调用RKNN API
1）应用直接链接librknnrt.so
2）应用链接Android平台HIDL实现的librknn_api_android.so
对于需要通过CTS/VTS测试的Android设备可以使用基于Android平台HIDL实现的RKNN API。如果不需要通过CTS/VTS测试的设备建议直接链接使用librknnrt.so，对各个接口调用流程的链路更短，可以提供更好的性能。

对于使用Android HIDL实现的RKNN API的代码位于RK3566_RK3568/RK3588 Android系统SDK的vendor/rockchip/hardware/interfaces/neuralnetworks目录下。当完成Android系统编译后，将会生成一些NPU相关的库（对于应用只需要链接使用librknn_api_android.so即可）

**本示例适用于librknn_api_android.so。**



# 编译

- 编译librknn_api_android.so

    需要先下载RK3566_RK3568/RK3588 Android SDK，在Android SDK根目录执行

    ```
    source build/envsetup.sh
    lunch your target ##需要根据自己的实际情况进行选择
    mmm vendor/rockchip/hardware/interfaces/neuralnetworks/ -j16

    ```

    将生成

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


-  编译本demo

    将$RKNPU2_SDK拷贝到Android SDK根目录，并执行：

    ```
    mmm rknpu2/examples/librknn_api_android_demo
    ```

    将生成的vendor/bin/rknn_create_mem_demo

# 运行


- 将rknn_create_mem_demo推到板子/vendor/bin/目录

- 将model推到板子/data/目录

- 确保板子的rockchip.hardware.neuralnetworks@1.0-service已经运行

```
    rknn_create_mem_demo /data/model/RK3566_RK3568/mobilenet_v1.rknn /data/model/dog_224x224.jpg
```



# FAQ

- rockchip.hardware.neuralnetworks@1.0-service服务没有运行怎么办

    如果该服务没有运行，从Android SDK确保vendor/rockchip/hardware/interfaces/neuralnetworks/目录存在，并且重新编译系统固件，并重新烧写到板子上，具体步骤请参考SDK编译固件的说明。

- 遇到sizeof(rknn_tensor_attr) != sizeof(::rockchip::hardware::neuralnetworks::V1_0::RKNNTensorAttr)的错误

    需要更新vendor/rockchip/hardware/interfaces/neuralnetworks到最新代码
