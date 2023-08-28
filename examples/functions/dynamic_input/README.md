# How to use dynamic_input function

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
        [[1,3,192,192]],    # set 0: [input0_192]
        [[1,3,256,256]],    # set 1: [input0_256]
        [[1,3,160,160]],    # set 2: [input0_160]
        [[1,3,224,224]],    # set 3: [input0_224]
    ]
    ```
    to simulate models with dynamic input shapes.

## Expected Results
This example will print the TOP5 labels and corresponding scores of the test image classification results for each different input shape, as follows:
```
-----TOP 5-----
Analysing : 100%|███████████████████████████████████████████████| 104/104 [00:00<00:00, 7669.31it/s]
Preparing : 100%|███████████████████████████████████████████████| 104/104 [00:00<00:00, 1079.43it/s]
mobilenet_v1
-----TOP 5-----
[155]: 0.994140625
[154]: 0.0022792816162109375
[204]: 0.001964569091796875
[283]: 0.0009207725524902344
[194 196 284]: 9.40561294555664e-05

Analysing : 100%|███████████████████████████████████████████████| 104/104 [00:00<00:00, 6171.67it/s]
Preparing : 100%|███████████████████████████████████████████████| 104/104 [00:00<00:00, 1598.60it/s]
mobilenet_v1
-----TOP 5-----
[155]: 0.9580078125
[154]: 0.0338134765625
[204]: 0.0074310302734375
[194]: 0.0003018379211425781
[219]: 0.00014257431030273438

Analysing : 100%|███████████████████████████████████████████████| 104/104 [00:00<00:00, 9171.15it/s]
Preparing : 100%|███████████████████████████████████████████████| 104/104 [00:00<00:00, 1389.63it/s]
mobilenet_v1
-----TOP 5-----
[155]: 0.9833984375
[154]: 0.007686614990234375
[204]: 0.0036029815673828125
[283]: 0.0009179115295410156
[193]: 0.0007853507995605469
```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.