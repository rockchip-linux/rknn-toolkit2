# onnx model

* onnx_models/yolov5s_relu.onnx

The model comes from https://github.com/airockchip/rknn_model_zoo

# convert rknn model

1. Modify the following parameters of onnx2rknn.py to the corresponding platform, such as the RK3566_RK3568 model, modify as follows:

```python
platform="rk3566"
```

2. Execute python onnx2rknn.py

3. The rknn model is generated in the rknn_models directory