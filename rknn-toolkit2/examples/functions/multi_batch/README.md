# How to expand batch for use multi-batch function

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
- If connecting board is required, pl'ease add the 'target' parameter in 'rknn.init_runtime'.
- You can modify the 'rknn_batch_size' parameter of 'rknn.build' to achieve the effect of multi-batch.

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results for every batch, as follows:
```
----- Batch 0: TOP 5 -----
[155] score:0.993652 class:"Shih-Tzu"
[154] score:0.002834 class:"Pekinese, Pekingese, Peke"
[204] score:0.002172 class:"Lhasa, Lhasa apso"
[283] score:0.000571 class:"Persian cat"
[284] score:0.000133 class:"Siamese cat, Siamese"
----- Batch 1: TOP 5 -----
[  1] score:0.598145 class:"goldfish, Carassius auratus"
[794] score:0.062073 class:"shower curtain"
[996] score:0.062073 class:"hen-of-the-woods, hen of the woods, Polyporus frondosus, Grifola frondosa"
[927] score:0.041687 class:"trifle"
[115] score:0.027863 class:"sea slug, nudibranch"
----- Batch 2: TOP 5 -----
[812] score:0.999023 class:"space shuttle"
[148] score:0.000293 class:"killer whale, killer, orca, grampus, sea wolf, Orcinus orca"
[517] score:0.000132 class:"crane"
[833] score:0.000132 class:"submarine, pigboat, sub, U-boat"
[525] score:0.000089 class:"dam, dike, dyke"
----- Batch 3: TOP 5 -----
[155] score:0.993652 class:"Shih-Tzu"
[154] score:0.002834 class:"Pekinese, Pekingese, Peke"
[204] score:0.002172 class:"Lhasa, Lhasa apso"
[283] score:0.000571 class:"Persian cat"
[284] score:0.000133 class:"Siamese cat, Siamese"
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.