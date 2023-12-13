# Caffe MobileNet V2

## Model Source
The model used in this example come from the following open source projects:  
https://github.com/shicai/MobileNet-Caffe

## Script Usage
*Usage:*
```
python test.py
```
*rknn_convert usage:*
```
python3 -m rknn.api.rknn_convert -t rk3566 -i  ./model_config.yml -o ./
```
*Description:*
- The default target platform in script is 'rk3566', please modify the 'target_platform' parameter of 'rknn.config' according to the actual platform.
- If connecting board is required, please add the 'target' parameter in 'rknn.init_runtime'.

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results, as follows:
```
-----TOP 5-----
[155] score:0.994629 class:"Shih-Tzu"
[154] score:0.001950 class:"Pekinese, Pekingese, Peke"
[204] score:0.001950 class:"Lhasa, Lhasa apso"
[283] score:0.000674 class:"Persian cat"
[196] score:0.000109 class:"miniature schnauzer"
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.