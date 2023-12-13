1、目前RV1106/RV1103仅支持rknn_mobilenet_demo 和 rknn_yolov5_demo两个demo，在examples/RV1106_RV1103目录下，demo中的**build脚本用RV1106指代RV1106/RV1103**

2、RK356X和RK3588支持example目录下，除examples/RV1106_RV1103文件夹外的所有demo

3、**RV1106/RV1103设置LD_LIBRARY_PATH必须为全路径**，例如：export LD_LIBRARY_PATH=/userdata/lib

4、RK356X和RK3588设置LD_LIBRARY_PATH为全路径和相对路径均可
