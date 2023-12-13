# TensorFlow Inception V3 QAT

## Model Source
The model used in this example come from:
https://storage.googleapis.com/download.tensorflow.org/models/tflite_11_05_08/inception_v3_quant.tgz

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
[   2] score:0.996572 class:"goldfish, Carassius auratus"
[ 408] score:0.000464 class:"ambulance"
[ 795] score:0.000201 class:"shower curtain"
[ 352] score:0.000070 class:"hartebeest"
[ 974] score:0.000051 class:"coral reef"
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.