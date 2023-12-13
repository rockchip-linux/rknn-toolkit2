## 连板调试简介
RKNN Toolkit2的连板功能一般需要更新板端的 rknn_server 和 librknnrt.so/librknnmrt.so，并且手动启动 rknn_server 才能正常工作。
rknn_server: 是一个运行在板子上的后台代理服务，用于接收PC通过USB传输过来的协议，然后执行板端runtime对应的接口，并返回结果给PC。

- librknnrt.so: 是一个板端的runtime库。
- librknnmrt.so: 是专用于1103/1106平台的runtime库。

有些固件默认已经集成了rknn_server，如果已经集成，可以忽略下面的启动步骤。

## 一、版本要求
- 若使用动态形状输入RKNN模型，要求rknn_server和RKNPU Runtime库版本>=1.5.0。


## 二、rknn_server存放目录
RV1106、RK356X和RK3588上的rknn_server分别对应存放在runtime/目录下对应的平台里(RV1103与RV1106使用相同的库和rknn_server)，以RK356X为例，rknn_server的存放目录层次如下：
### Android平台
```
Android
└── rknn_server
    ├── arm64
    │   └── rknn_server
    └── arm
        └── rknn_server
```

### Linux平台
```
Linux
└── rknn_server
    ├── aarch64
    │   └── usr
    │       └── bin
    │           ├── restart_rknn.sh
    │           ├── rknn_server
    │           └── start_rknn.sh
    └── armhf
        └── usr
            └── bin
                ├── restart_rknn.sh
                ├── rknn_server
                └── start_rknn.sh
```

## 三、 启动步骤
本章节适用于RK3566/RK3568/RK3588平台，RV1103/RV1106平台请参考**RV1106启动步骤**章节。

以RK356X为例，进入到runtime/目录，根据不同平台，选择以下不同的启动步骤：

### 1. Android平台
BOARD_ARCH在64位安卓系统中，对应arm64目录，在32位系统，对应arm目录
1. adb root && adb remount
2. adb push Android/rknn_server/${BOARD_ARCH}/rknn_server到板子/vendor/bin/目录
3. adb push Android/librknn_api/${BOARD_ARCH}/librknnrt.so到/vendor/lib64（64位系统特有）和/vendor/lib目录
4. 进入板子的**串口**终端，执行：
```
su
chmod +x /vendor/bin/rknn_server
sync
reboot
```
5. 重新进入板子的**串口**终端，执行 `ps |grep rknn_server`, 查看是否有 rknn_server的进程id（较新的固件开机会自动启动rknn_server），如果不存在，则手动执行:
```
su
setenforce 0
/vendor/bin/rknn_server &
```

### 2. Linux平台
BOARD_ARCH在64位Linux系统中，对应aarch64目录，在32位系统，对应armhf目录
1. adb push Linux/rknn_server/${BOARD_ARCH}/usr/bin/下的所有文件到/usr/bin目录
2. adb push Linux/librknn_api/${BOARD_ARCH}/librknnrt.so到/usr/lib目录
3. 进入板子的**串口**终端，执行：
```
chmod +x /usr/bin/rknn_server
chmod +x /usr/bin/start_rknn.sh
chmod +x /usr/bin/restart_rknn.sh
restart_rknn.sh
```


### **串口**查看rknn_server详细日志
#### 1. Android平台
1. 进入**串口**终端,设置日志等级
```
su
setenforce 0
setprop persist.vendor.rknn.server.log.level 5
```
2. 杀死rknn_server进程
```
kill -9 `pgrep rknn_server`
```
3. 重启rknn_server进程(若固件没有自启动rknn_server)
```
/vendor/bin/rknn_server &
logcat
```
4. 再次使用python接口连板推理
#### 2. Linux平台
1. 进入**串口**终端,设置日志等级
```
export RKNN_SERVER_LOGLEVEL=5
```
2. 重启rknn_server进程(若固件没有自启动rknn_server)
```
restart_rknn.sh
```
3. 再次使用python接口连板推理

## 四、 RV1106启动步骤
RV1103/RV1106上使用的RKNPU Runtime库是librknnmrt.so，使用32-bit的rknn_server,启动步骤如下：
1. adb push Linux/rknn_server/armhf-uclibc/usr/bin下的所有文件到/oem/usr/bin目录
2. adb push Linux/librknn_api/armhf-uclibc/librknnmrt.so到/oem/usr/lib目录
3. 进入板子的**串口**终端，执行：
```
chmod +x /oem/usr/bin/rknn_server
chmod +x /oem/usr/bin/start_rknn.sh
chmod +x /oem/usr/bin/restart_rknn.sh
restart_rknn.sh
```

### RV1106使用**串口**查看rknn_server详细日志
1. 进入**串口**终端,设置日志等级
```
export RKNN_SERVER_LOGLEVEL=5
```
2. 重启rknn_server进程
```
restart_rknn.sh
```
3. 再次使用python接口连板推理

**注意：**

- 遇到"E RKNN: failed to allocate fd, ret: -1, errno: 12"报错，可以在RV1103/RV1106上运行RkLunch-stop.sh，关闭其他占用内存的应用后再连板推理。
- RV1103/RV1106使用init_runtime python接口时，**不支持**perf_debug=True参数。
- accuracy_analysis python接口使用时，可能会因为模型太大，板子上存储容量不够导致运行失败，可以在板子上使用df -h命令来确认。