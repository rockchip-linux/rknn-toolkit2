rknn_matmul_api_demo是一个使用matmul C API在NPU上执行矩阵乘法的示例。


用法:
```
Usage:
./rknn_matmul_api_demo <matmul_type> <M> <K> <N> <B_layout> <AC_layout> <loop_count>
        matmul_type = 1: RKNN_FLOAT16_MM_FLOAT16_TO_FLOAT32
        matmul_type = 2: RKNN_INT8_MM_INT8_TO_INT32
        matmul_type = 10: RKNN_INT4_MM_INT4_TO_INT16
Example: A = [4,64], B = [64,32], int8 matmul test command as followed:
./rknn_matmul_api_demo 2 4 64 32

```
以下 <TARGET_PLATFORM> 表示RK3566_RK3568、RK3562或RK3588。

# Aarch64 Linux 示例
## 编译

将`build-linux_<TARGET_PLATFORM>.sh`中的`GCC_COMPILER`修改成交叉编译器路径, 然后执行


```
./build-linux_<TARGET_PLATFORM>.sh
```
## 安装

将 install/rknn_matmul_api_demo_Linux 拷贝到设备上。

- 如果使用Rockchip的EVB板，可以使用以下命令：

连接设备并将程序传输到`/userdata`

```
adb push install/rknn_matmul_api_demo_Linux /userdata/
```

- 如果你的板子有sshd服务，可以使用scp命令或者其他方式将程序和模型传输到板子上。

## 运行


```
adb shell
cd /userdata/rknn_matmul_api_demo_Linux/
```

```
export LD_LIBRARY_PATH=./lib
./rknn_matmul_api_demo 2 4 64 32
```

# Android 示例
## 编译

将`build-android_<TARGET_PLATFORM>.sh`中的`ANDROID_NDK_PATH`修改成平台对应的NDK，然后执行


```
./build-android_<TARGET_PLATFORM>.sh
```

## 安装

连接设备并将程序传输到`/data`

```
adb push install/rknn_matmul_api_demo_Android /data/
```

## 运行

```
adb shell
cd /data/rknn_matmul_api_demo_Android/
```

```
export LD_LIBRARY_PATH=./lib
./rknn_matmul_api_demo 2 4 64 32
```
