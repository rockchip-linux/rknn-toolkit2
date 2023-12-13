# How to use dynamic shape function

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
- The 'dynamic_input' parameter of 'rknn.config' is set to:
    ```
    dynamic_input = [
        [[1,3,256,256]],    # set 1: [input0_256]
        [[1,3,160,160]],    # set 2: [input0_160]
        [[1,3,224,224]],    # set 3: [input0_224]
    ]
    ```
    to simulate models with dynamic input shapes.

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results for each different input shape, as follows:
```
--> Running model with input shape [1,3,224,224]
GraphPreparing : 100%|██████████████████████████████████████████| 104/104 [00:00<00:00, 4764.33it/s]
SessionPreparing : 100%|█████████████████████████████████████████| 104/104 [00:00<00:00, 366.73it/s]
-----TOP 5-----
[155] score:0.993652 class:"Shih-Tzu"
[154] score:0.002277 class:"Pekinese, Pekingese, Peke"
[204] score:0.002277 class:"Lhasa, Lhasa apso"
[283] score:0.000673 class:"Persian cat"
[196] score:0.000109 class:"miniature schnauzer"
--> Running model with input shape [1,3,160,160]
GraphPreparing : 100%|██████████████████████████████████████████| 104/104 [00:00<00:00, 9800.88it/s]
SessionPreparing : 100%|████████████████████████████████████████| 104/104 [00:00<00:00, 1296.61it/s]
-----TOP 5-----
[155] score:0.963867 class:"Shih-Tzu"
[154] score:0.029099 class:"Pekinese, Pekingese, Peke"
[204] score:0.006393 class:"Lhasa, Lhasa apso"
[194] score:0.000415 class:"Dandie Dinmont, Dandie Dinmont terrier"
[219] score:0.000090 class:"cocker spaniel, English cocker spaniel, cocker"
--> Running model with input shape [1,3,256,256]
GraphPreparing : 100%|██████████████████████████████████████████| 104/104 [00:00<00:00, 8788.48it/s]
SessionPreparing : 100%|████████████████████████████████████████| 104/104 [00:00<00:00, 1404.91it/s]
-----TOP 5-----
[155] score:0.981445 class:"Shih-Tzu"
[154] score:0.008896 class:"Pekinese, Pekingese, Peke"
[204] score:0.004169 class:"Lhasa, Lhasa apso"
[193] score:0.000783 class:"Australian terrier"
[283] score:0.000783 class:"Persian cat"
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.