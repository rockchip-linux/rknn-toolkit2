# How to use MMSE function

## Model Source
The model used in this example come from the following open source projects:  
https://github.com/tensorflow/models/blob/master/research/slim/nets/mobilenet_v1.md

## Script Usage
*Usage:*
```
python test.py
```
*Description:*
- The default target platform in script is 'rk3566', please modify the 'target_platform' parameter of 'rknn.config' according to the actual platform.
- If connecting board is required, please add the 'target' parameter in 'rknn.init_runtime'.
- The 'quantized_algorithm' parameter of 'rknn.config' is set to 'mmse'. and a 'MmseQuant2' progress bar can be seen during the conversion process, indicating the execution progress of MMSE.


## Expected Results
This example will outputs the results of the accuracy analysis and print the TOP5 labels and corresponding scores of the test image classification results, as follows:
```
layer_name                                                                          simulator_error        
                                                                                   entire    single        
-----------------------------------------------------------------------------------------------------------
[Input] input:0                                                                   1.000000  1.000000       
[exDataConvert] input:0_int8                                                      0.999986  0.999986       
[Conv] MobilenetV1/MobilenetV1/Conv2d_0/BatchNorm/FusedBatchNorm:0                
[Clip] MobilenetV1/MobilenetV1/Conv2d_0/Relu6:0                                   0.999986  0.999986 

...

[Clip] MobilenetV1/MobilenetV1/Conv2d_13_pointwise/Relu6:0                        0.858769  0.999334       
[Conv] MobilenetV1/Logits/AvgPool_1a/AvgPool:0                                    0.948201  0.999804       
[Conv] MobilenetV1/Logits/Conv2d_1c_1x1/BiasAdd:0                                 0.963938  0.999562       
[Reshape] MobilenetV1/Logits/SpatialSqueeze:0_int8                                0.963938  0.999906       
[exDataConvert] MobilenetV1/Logits/SpatialSqueeze:0                               0.963938  0.999906  
```
```
-----TOP 5-----
[155]: 0.9931640625
[154]: 0.00266265869140625
[204]: 0.0019779205322265625
[283]: 0.0009202957153320312
[194]: 0.0001285076141357422
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.