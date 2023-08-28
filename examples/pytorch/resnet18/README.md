# Pytorch ResNet18

## Model Source
The model used in this example come from the 'torchvision', more details in the 'export_pytorch_model' function of the script.

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
[812]: 0.9997393488883972
[404]: 0.00019667540618684143
[657]: 1.2877902918262407e-05
[833]: 9.024500286614057e-06
[744 895]: 7.119915153452894e-06
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.