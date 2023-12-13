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
[155] score:0.734863 class:"Shih-Tzu"
[154] score:0.180176 class:"Pekinese, Pekingese, Peke"
[204] score:0.051605 class:"Lhasa, Lhasa apso"
[284] score:0.004234 class:"Siamese cat, Siamese"
[252] score:0.003918 class:"affenpinscher, monkey pinscher, monkey dog"
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.