# Pytorch ResNet18 QAT

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
- This is a QAT model, and the do_quantization of rknn.build needs to be set to False.

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results, as follows:
```
-----TOP 5-----
[812]: 0.9997414350509644
[404]: 0.0001939184294315055
[657]: 1.4925829418643843e-05
[466 744 895]: 8.44217083795229e-06
[466 744 895]: 8.44217083795229e-06
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.