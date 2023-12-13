下述<TARGET_PLATFORM>是RV1106或RV1103

# Arm Linux Demo

## 编译

修改目标平台的`build-linux_<TARGET_PLATFORM>.sh`上的`GCC_COMPILER`，

然后执行

```
./build-linux_<TARGET_PLATFORM>.sh
```

## 安装

连接设备并将构建输出推送到“/userdata”

```
adb push install/rknn_mobilenet_demo_Linux /userdata/
```

## 运行

```
adb shell
cd /userdata/rknn_mobilenet_demo_Linux/
```

```
export LD_LIBRARY_PATH=./lib
./rknn_mobilenet_demo model/<TARGET_PLATFORM>/mobilenet_v1.rknn model/dog_224x224.jpg
```

