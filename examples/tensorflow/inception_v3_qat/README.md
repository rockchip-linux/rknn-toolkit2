# TensorFlow Inception V3 QAT

## Model Source
The model used in this example come from:
https://storage.googleapis.com/download.tensorflow.org/models/tflite_11_05_08/inception_v3_quant.tgz

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
[2]: 0.9965722560882568
[408]: 0.00046414724783971906
[795]: 0.00020119294640608132
[352]: 6.953041884116828e-05
[974]: 5.136874460731633e-05
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.