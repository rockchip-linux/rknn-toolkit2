# Pytorch ResNet18 QAT

## Model Source
The model used in this example come from the 'torchvision', more details in the 'export_pytorch_model' function of the script.

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
- This is a QAT model, and the do_quantization of rknn.build needs to be set to False.

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results, as follows:
```
-----TOP 5-----
[812] score:0.999741 class:"space shuttle"
[404] score:0.000194 class:"airliner"
[657] score:0.000015 class:"missile"
[466] score:0.000008 class:"bullet train, bullet"
[744] score:0.000008 class:"projectile, missile"
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.