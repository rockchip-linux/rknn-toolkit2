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
# simulator_error: calculate the output error of each layer of the simulator (compared to the 'golden' value).
#              entire: output error of each layer between 'golden' and 'simulator', these errors will accumulate layer by layer.
#              single: single-layer output error between 'golden' and 'simulator', can better reflect the single-layer accuracy of the simulator.

layer_name                                                         simulator_error                    
                                                               entire              single             
                                                            cos      euc        cos      euc          
--------------------------------------------------------------------------------------------------
[Input] data                                              1.00000 | 0.0       1.00000 | 0.0           
[exDataConvert] data_int8                                 0.99997 | 2.3275    0.99997 | 2.3275        
[BatchNormalization] resnetv24_batchnorm0_fwd             0.99995 | 2.4514    0.99995 | 2.4514        

...

[Relu] resnetv24_relu1_fwd                                0.98352 | 41.498    0.99989 | 3.3598        
[Conv] resnetv24_pool1_fwd                                0.99545 | 2.2519    0.99999 | 0.1243        
[Conv] resnetv24_dense0_fwd_conv                          0.99450 | 6.8046    0.99993 | 0.7382        
[Reshape] resnetv24_dense0_fwd_int8                       0.99450 | 6.8046    0.99994 | 0.6719        
[exDataConvert] resnetv24_dense0_fwd                      0.99450 | 6.8046    0.99994 | 0.6719
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.