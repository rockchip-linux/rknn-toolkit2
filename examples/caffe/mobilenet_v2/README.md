# Caffe MobileNet V2

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
- If connecting board is required, please add the 'target' parameter in 'rknn.init_runtime'.

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results, as follows:
```
-----TOP 5-----
[155]: 0.9931640625
[154]: 0.00266265869140625
[204]: 0.0019779205322265625
[283]: 0.0009202957153320312
[194]: 0.0001285076141357422
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.