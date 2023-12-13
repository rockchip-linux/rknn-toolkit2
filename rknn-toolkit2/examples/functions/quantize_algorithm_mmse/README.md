# How to use MMSE quantize algorithm

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
layer_name                                                                                 simulator_error                    
                                                                                       entire              single             
                                                                                    cos      euc        cos      euc          
--------------------------------------------------------------------------------------------------------------------------
[Input] input:0                                                                   1.00000 | 0.0       1.00000 | 0.0           
[exDataConvert] input:0_int8                                                      0.99999 | 0.8565    0.99999 | 0.8565        
[Conv] MobilenetV1/MobilenetV1/Conv2d_0/BatchNorm/FusedBatchNorm:0                
[Clip] MobilenetV1/MobilenetV1/Conv2d_0/Relu6:0                                   0.99999 | 4.5562    0.99999 | 4.5562 

...

[Clip] MobilenetV1/MobilenetV1/Conv2d_13_pointwise/Relu6:0                        0.99681 | 29.000    0.99979 | 7.3940        
[Conv] MobilenetV1/Logits/AvgPool_1a/AvgPool:0                                    0.99903 | 1.4259    0.99997 | 0.2520        
[Conv] MobilenetV1/Logits/Conv2d_1c_1x1/BiasAdd:0                                 0.99914 | 4.6217    0.99992 | 1.4081        
[Reshape] MobilenetV1/Logits/SpatialSqueeze:0_int8                                0.99914 | 4.6217    0.99995 | 1.0506        
[exDataConvert] MobilenetV1/Logits/SpatialSqueeze:0                               0.99914 | 4.6217    0.99995 | 1.0506   
```
```
-----TOP 5-----
[ 156] score:0.945152 class:"Shih-Tzu"
[ 155] score:0.050125 class:"Pekinese, Pekingese, Peke"
[ 205] score:0.003332 class:"Lhasa, Lhasa apso"
[ 284] score:0.000685 class:"Persian cat"
[ 260] score:0.000090 class:"Pomeranian"
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.