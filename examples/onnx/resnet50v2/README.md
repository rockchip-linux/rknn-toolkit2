# ONNX ResNet50 V2

## Model Source
The model used in this example come from:  
https://s3.amazonaws.com/onnx-model-zoo/resnet/resnet50v2/resnet50v2.onnx

## Script Usage
*Usage:*
```
python test.py
```
*Description:*
- The default target platform in script is 'rk3566', please modify the 'target_platform' parameter of 'rknn.config' according to the actual platform.
- If connecting board is required, please add the 'target' parameter in 'rknn.init_runtime'.

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results, as follows:
```
-----TOP 5-----
[155]: 0.742885410785675
[154]: 0.22587773203849792
[262]: 0.015506524592638016
[152]: 0.0035010927822440863
[254]: 0.00280063902027905
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.