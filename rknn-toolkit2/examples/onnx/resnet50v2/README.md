# ONNX ResNet50 V2

## Model Source
The model used in this example come from:  
https://s3.amazonaws.com/onnx-model-zoo/resnet/resnet50v2/resnet50v2.onnx

## Script Usage
*Usage:*
```
python test.py
```
*rknn_convert usage:*
```
python3 -m rknn.api.rknn_convert -t rk3568 -i ./model_config.yml -o ./
```
*Description:*
- The default target platform in script is 'rk3566', please modify the 'target_platform' parameter of 'rknn.config' according to the actual platform.
- If connecting board is required, please add the 'target' parameter in 'rknn.init_runtime'.

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results, as follows:
```
-----TOP 5-----
[155] score:0.742885 class:"Shih-Tzu"
[154] score:0.225878 class:"Pekinese, Pekingese, Peke"
[262] score:0.015507 class:"Brabancon griffon"
[152] score:0.003501 class:"Japanese spaniel"
[254] score:0.002801 class:"pug, pug-dog"
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.