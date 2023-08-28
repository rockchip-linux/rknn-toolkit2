# How to expand batch for use multi-batch function

## Model Source
The model used in this example come from the following open source projects:  
https://github.com/shicai/MobileNet-Caffe

## Script Usage
*Usage:*
```
python test.py
```
*Description:*
- The default target platform in script is 'rk3566', please modify the 'target_platform' parameter of 'rknn.config' according to the actual platform.
- If connecting board is required, pl'ease add the 'target' parameter in 'rknn.init_runtime'.
- You can modify the 'rknn_batch_size' parameter of 'rknn.build' to achieve the effect of multi-batch.

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results for every batch, as follows:
```
mobilenet_v1
-----TOP 5-----
[155]: 0.9931640625
[154 204]: 0.00254058837890625
[154 204]: 0.00254058837890625
[283]: 0.0005893707275390625
[284]: 0.0002849102020263672

mobilenet_v1
-----TOP 5-----
[155]: 0.9931640625
[154 204]: 0.00254058837890625
[154 204]: 0.00254058837890625
[283]: 0.0005893707275390625
[284]: 0.0002849102020263672

mobilenet_v1
-----TOP 5-----
[155]: 0.9931640625
[154 204]: 0.00254058837890625
[154 204]: 0.00254058837890625
[283]: 0.0005893707275390625
[284]: 0.0002849102020263672

mobilenet_v1
-----TOP 5-----
[155]: 0.9931640625
[154 204]: 0.00254058837890625
[154 204]: 0.00254058837890625
[283]: 0.0005893707275390625
[284]: 0.0002849102020263672
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.