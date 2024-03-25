# Note
  This repository is no longer maintained and has been moved to https://github.com/airockchip/rknn-toolkit2/ .
  本仓库不再维护，已经移到https://github.com/airockchip/rknn-toolkit2 。

# Description
  RKNN software stack can help users to quickly deploy AI models to Rockchip chips. The overall framework is as follows:
    <center class="half">
        <div style="background-color:#ffffff;">
        <img src="res/framework.png" title="RKNN"/>
    </center>

  In order to use RKNPU, users need to first run the RKNN-Toolkit2 tool on the computer, convert the trained model into an RKNN format model, and then inference on the development board using the RKNN C API or Python API.

- RKNN-Toolkit2 is a software development kit for users to perform model conversion, inference and performance evaluation on PC and Rockchip NPU platforms.

- RKNN-Toolkit-Lite2 provides Python programming interfaces for Rockchip NPU platform to help users deploy RKNN models and accelerate the implementation of AI applications.

- RKNN Runtime provides C/C++ programming interfaces for Rockchip NPU platform to help users deploy RKNN models and accelerate the implementation of AI applications.

- RKNPU kernel driver is responsible for interacting with NPU hardware. It has been open source and can be found in the Rockchip kernel code.

# Support Platform
  - RK3566/RK3568 Series
  - RK3588 Series
  - RK3562 Series
  - RV1103/RV1106


Note:

​      **For RK1808/RV1109/RV1126/RK3399Pro, please refer to :**

​          https://github.com/airockchip/rknn-toolkit

​          https://github.com/airockchip/rknpu

​          https://github.com/airockchip/RK3399Pro_npu


# Download
- You can also download all packages, docker image, examples, docs and platform-tools from [RKNPU2_SDK](https://console.zbox.filez.com/l/I00fc3), fetch code: rknn
- You can get more examples from [rknn mode zoo](https://github.com/airockchip/rknn_model_zoo)

# Notes
- RKNN-Toolkit2 is not compatible with [RKNN-Toolkit](https://github.com/airockchip/rknn-toolkit)
- Currently only support on:
  - Ubuntu 18.04 python 3.6/3.7
  - Ubuntu 20.04 python 3.8/3.9
  - Ubuntu 22.04 python 3.10/3.11
- Latest version:1.6.0(Release version)

# CHANGELOG

## 1.6.0
 - Support ONNX model of OPSET 12~19
 - Support custom operators (including CPU and GPU)
 - Optimization operators support such as dynamic weighted convolution, Layernorm, RoiAlign, Softmax, ReduceL2, Gelu, GLU, etc.
 - Added support for python3.7/3.9/3.11
 - Add rknn_convert function
 - Optimize transformer support
 - Optimize the MatMul API, such as increasing the K limit length, RK3588 adding int4 * int4 -> int16 support, etc.
 - Optimize RV1106 rknn_init initialization time, memory consumption, etc.
 - RV1106 adds int16 support for some operators
 - Fixed the problem that the convolution operator of RV1106 platform may make random errors in some cases.
 - Optimize user manual
 - Reconstruct the rknn model zoo and add support for multiple models such as detection, segmentation, OCR, and license plate recognition.

 for older version, please refer [CHANGELOG](CHANGELOG.md)

# Feedback and Community Support
- [Redmine](https://redmine.rock-chips.com) (**Feedback recommended, Please consult our sales or FAE for the redmine account**)
- QQ Group Chat: 1025468710 (full, please join group 3)
- QQ Group Chat2: 547021958 (full, please join group 3)
- QQ Group Chat3: 469385426
<center class="half">
  <img width="200" height="200"  src="res/QQGroupQRCode.png" title="QQ Group Chat"/>
  <img width="200" height="200"  src="res/QQGroup2QRCode.png" title="QQ Group Chat2"/>
  <img width="200" height="200"  src="res/QQGroup3QRCode.png" title="QQ Group Chat3"/>
</center>


