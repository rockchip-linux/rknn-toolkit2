# How to use accuracy-analysis function

## Model Source
The model used in this example come from:  
https://s3.amazonaws.com/onnx-model-zoo/resnet/resnet50v2/resnet50v2.onnx

## Script Usage
*Usage:*
```
python test.py
```
*Description:*
- The default target platform in script is 'rk3566', please modify the 'target_platform' parameter of 'rknn.config' according to the actual platform.
- If connecting board is required, please add the 'target' parameter in 'rknn.accuracy_analysis'.

## Expected Results
This example will outputs the results of the accuracy analysis and store all the results in the snapshot directory, as follows:
```
# simulator_error: calculate the simulator errors.
#              entire: errors between 'golden' and 'simulator'.
#              single: single layer errors. (compare to 'entire', the input of each layer is come from 'golden')!
# ('nan' means that tensor are 'all zeros', or 'all equal', or 'large values', etc)

layer_name                                                  simulator_error        
                                                           entire    single        
-----------------------------------------------------------------------------------
[Input] data                                              1.000000  1.000000       
[exDataConvert] data_int8                                 0.999973  0.999973       
[BatchNormalization] resnetv24_batchnorm0_fwd             0.999946  0.999946 

...

[Relu] resnetv24_relu1_fwd                                0.983521  0.999891       
[Conv] resnetv24_pool1_fwd                                0.995452  0.999986       
[Conv] resnetv24_dense0_fwd_conv                          0.994497  0.999933       
[Reshape] resnetv24_dense0_fwd_int8                       0.994497  0.999945       
[exDataConvert] resnetv24_dense0_fwd                      0.994497  0.999945 
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.