# TFLite MobileNet V2

## Model Source
The model used in this example come from the following open source projects:  
https://www.tensorflow.org/lite/guide/hosted_models?hl=zh-cn

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
[156]: 0.92822265625
[155]: 0.06317138671875
[205]: 0.004299163818359375
[284]: 0.0030956268310546875
[285]: 0.00017058849334716797
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.