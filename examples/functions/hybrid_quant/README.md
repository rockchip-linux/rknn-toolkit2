# How to use hybrid-quantization function

## Model Source
The model used in this example come from:  
https://github.com/tensorflow/models/blob/master/research/object_detection/g3doc/tf1_detection_zoo.md  ssd_mobilenet_v2_coco

## Script Usage
*Usage:*
```
1. python step1.py
2. modify ssd_mobilenet_v2.quantization.cfg according to the prompt of step1.py
3. python step2.py
```
*Description:*
- The default target platform in script is 'rk3566', please modify the 'target_platform' parameter of 'rknn.config' according to the actual platform.
- If connecting board is required, please add the 'target' parameter in 'rknn.init_runtime'.

## Expected Results
This example will outputs the results of the accuracy analysis and save the result of object detection to the 'result.jpg', as follows:  
```
layer_name                                                                                                     simulator_error        
                                                                                                              entire    single        
--------------------------------------------------------------------------------------------------------------------------------------
[Input] FeatureExtractor/MobilenetV2/MobilenetV2/input:0                                                     1.000000  1.000000       
[exDataConvert] FeatureExtractor/MobilenetV2/MobilenetV2/input:0_int8                                        0.999964  0.999964       
[Conv] Conv__343:0                                                                                           
[Clip] FeatureExtractor/MobilenetV2/Conv/Relu6:0                                                             0.999976  0.999976   

...

[Conv] BoxPredictor_0/BoxEncodingPredictor/BiasAdd:0                                                         0.997577  0.999687       
[Transpose] BoxPredictor_0/BoxEncodingPredictor/BiasAdd__341:0                                               0.997577  0.999960       
[Reshape] concat_swap_concat_reshape_i0_out                                                                  0.997577  0.999960       
[Concat] concat_swap_concat_reshape_o0_out                                                                   0.997513  0.999939       
[Reshape] concat:0_int8                                                                                      0.997513  0.999943       
[exDataConvert] concat:0                                                                                     0.997513  0.999943 
```
![result](result_truth.jpg)
- Note: Different platforms, different versions of tools and drivers may have slightly different results.