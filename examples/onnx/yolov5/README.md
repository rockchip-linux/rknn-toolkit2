# YOLOV5 DEMO

### 1.About onnx file

- This model remove the post-process layer(which is not available on NPU now).
- To export .onnx format model file, refer to https://github.com/airockchip/rknn_model_zoo/tree/main/models/vision/object_detection/yolov5-pytorch



### 2.Run demo

```
python test.py
```

