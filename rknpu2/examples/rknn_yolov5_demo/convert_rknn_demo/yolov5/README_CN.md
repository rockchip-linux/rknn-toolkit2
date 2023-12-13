# onnx模型

* onnx_models/yolov5s_relu.onnx


模型来源：https://github.com/airockchip/rknn_model_zoo

# 转换rknn模型

1. 将onnx2rknn.py以下的参数修改成对应的平台，例如RK3566_RK3568模型，修改为：

```python
platform="rk3566"
```

2. 执行python onnx2rknn.py

3. rknn模型生成在rknn_models目录
