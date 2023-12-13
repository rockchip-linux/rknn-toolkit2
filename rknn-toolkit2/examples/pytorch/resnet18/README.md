# Pytorch ResNet18

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

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results, as follows:
```
-----TOP 5-----
[812] score:0.999739 class:"space shuttle"
[404] score:0.000197 class:"airliner"
[657] score:0.000013 class:"missile"
[833] score:0.000009 class:"submarine, pigboat, sub, U-boat"
[744] score:0.000007 class:"projectile, missile"
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.