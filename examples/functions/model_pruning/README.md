# How to use model_pruning function

## Model Source
The model used in this example come from the following open source projects:  
https://github.com/shicai/MobileNet-Caffe

## Script Usage
*Usage:*
```
python test.py
```
*Description:*
- The default target platform in script is 'rk3566', please modify the 'target_platform' parameter of 'rknn.config' according to the actual platform.
- If connecting board is required, please add the 'target' parameter in 'rknn.init_runtime'.
- The 'model_pruning' parameter of 'rknn.config' is set to True.
- When verbose is set to True, the following similar prompts will appear during the build process, 
indicating that model pruning has been effective for this model. (This means that approximately 6.9% 
of the weights have been removed, resulting in a saving of about 13.4% of the computational workload.)
Please note that not all models can be pruned, only models with sparse weights are likely to benefit from pruning.
    ```
    I model_pruning ...
    I model_pruning results:
    I     -1.12144 MB (-6.9%)
    I     -0.00016 T (-13.4%)
    I model_pruning done.
    ```

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results, as follows:
```
-----TOP 5-----
[155]: 0.724609375
[154]: 0.1920166015625
[204]: 0.0509033203125
[284]: 0.004177093505859375
[252 283]: 0.0038623809814453125
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.