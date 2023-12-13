# Yolo-v5 demo

## Guide for exporting rknn model

Please refer to this link:  https://github.com/airockchip/rknn_model_zoo/tree/main/models/CV/object_detection/yolo

## Precautions

1. Use rknn-toolkit2 version greater than or equal to **1.4.0**.
2. When using the model trained by yourself, please pay attention to aligning post-processing parameters such as anchor, otherwise it will cause post-processing analysis errors.
3. The official website and rk pre-training models both detect 80 types of targets. If you train your own model, you need to change the OBJ_CLASS_NUM and NMS_THRESH, BOX_THRESH post-processing parameters in include/postprocess.h.
4. The demo needs the support of librga.so, please refer to https://github.com/airockchip/librga for compiling and using
5. Due to hardware limitations, the demo model moves the post-processing part of the yolov5 model to the cpu implementation by default. The models attached to this demo all use relu as the activation function. Compared with the silu activation function, the accuracy is slightly lower, and the performance is greatly improved.


## Android Demo

### Compiling && Building

According to the target platform, modifying the path for Android NDK on 'build-android_<TARGET_PLATFORM>.sh'

for example,

```sh
ANDROID_NDK_PATH=~/opt/tool_chain/android-ndk-r17
```

then, running this script：

```sh
./build-android_<TARGET_PLATFORM>.sh
```

### Push all build output file to the board

Connecting the usb port to PC,  then pushing all demo files to the board,

```sh
adb root
adb remount
adb push install/rknn_yolov5_demo /data/
```

### Running

```sh
adb shell
cd /data/rknn_yolov5_demo/

export LD_LIBRARY_PATH=./lib
./rknn_yolov5_demo model/<TARGET_PLATFORM>/yolov5s-640-640.rknn model/bus.jpg
```

## Aarch64 Linux Demo

### Compiling && Building

According to the target platform, modifying the path for 'TOOL_CHAIN' on 'build-android_<TARGET_PLATFORM>.sh'

```sh
export TOOL_CHAIN=~/opt/tool_chain/gcc-9.3.0-x86_64_aarch64-linux-gnu/host
```

then run the script：

```sh
./build-linux_<TARGET_PLATFORM>.sh
```

### Push all build output file to the board


Push install/rknn_yolov5_demo_Linux to the board,

- If using adb via the EVB board：

```
adb push install/rknn_yolov5_demo_Linux /userdata/
```

- For other boards,  using the scp or other different approaches to push all files under install/rknn_yolov5_demo_Linux to '/userdata'

### Running

```sh
adb shell
cd /userdata/rknn_yolov5_demo_Linux/

export LD_LIBRARY_PATH=./lib
./rknn_yolov5_demo model/<TARGET_PLATFORM>/yolov5s-640-640.rknn model/bus.jpg
```

Note: Try searching the location of librga.so and add it to LD_LIBRARY_PATH if the librga.so is not found on the lib folder.
Using the following commands to add to LD_LIBRARY_PATH.

```sh
export LD_LIBRARY_PATH=./lib:<LOCATION_LIBRGA.SO>
```

## Guide for Video Demo：
- H264
```
./rknn_yolov5_video_demo model/<TARGET_PLATFORM>/yolov5s-640-640.rknn xxx.h264 264
```
For converting to h264 via the ffmpeg ：
```
ffmpeg -i xxx.mp4 -vcodec h264 xxx.h264
```

- H265
```
./rknn_yolov5_video_demo model/<TARGET_PLATFORM>/yolov5s-640-640.rknn xxx.hevc 265
```
For converting to h265 via the ffmpeg ：
```
ffmpeg -i xxx.mp4 -vcodec hevc xxx.hevc
```
- RTSP
```
./rknn_yolov5_video_demo model/<TARGET_PLATFORM>/yolov5s-640-640.rknn <RTSP_URL> 265
```

### Remark

- **RK3562 only supports h264 video stream **
- **rtsp video stream only available on the Linux system **
- **The h264 name of the video stream input cannot be "out.h264", it will be overwritten.**