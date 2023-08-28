# Caffe VGG-SSD

## Model Source
The model used in this example come from the following open source projects:  
https://github.com/weiliu89/caffe/tree/ssd#models  
But the model download link has expired, please download from https://eyun.baidu.com/s/3jJhPRzo, password is 'rknn'.

## Script Usage
*Usage:*
```
python test.py
```
*Description:*
- The default target platform in script is 'rk3566', please modify the 'target_platform' parameter of 'rknn.config' according to the actual platform.
- If connecting board is required, please add the 'target' parameter in 'rknn.init_runtime'.

## Expected Results
This example will save the result of object detection to the 'result.jpg', as follows:  
![result](result_truth.jpg)
- Note: Different platforms, different versions of tools and drivers may have slightly different results.