# How to connect the board for debugging

## Model Source
The model used in this example come from the following open source projects:  
https://github.com/shicai/MobileNet-Caffe

## Script Usage
*Usage:*
```
python test.py
```
*Description:*
- The default target platform in script is 'rk3588', please modify the 'target_platform' parameter of 'rknn.config' according to the actual platform.
- The 'target' parameter in 'rknn.init_runtime'/'rknn.accuracy_analysis' is set to 'rk3588'.
- The 'perf_debug' and 'eval_mem' parameter of 'rknn.config' is set to True.

## Expected Results
This example would output the following results:
- Output encrypted rknn model: mobilenet_v2.crypt.rknn
- List devices that connect to host
    ```
    *************************
    all device(s) with adb mode:
    rk3588
    *************************
    ```
- Print version
    ```
    ==============================================
    RKNN VERSION:
    API: librknn_api version: 1.5.1 (7c1b999 build@2023-07-18T12:45:49)
    DRV: rknn_server: 1.5.0 (a4d9293 build: 2023-06-01 18:02:59)
        rknnrt: 1.5.1b19 (32afb0e92@2023-07-14T12:45:31)
    ==============================================
    ```
- Print perf information
    ```
    ===================================================================================================================
                                Performance                              
            #### The performance result is just for debugging, ####
            #### may worse than actual performance!            ####
    ===================================================================================================================
    ID   OpType           DataType Target InputShape                                   OutputShape            DDR Cycles     NPU Cycles     Total Cycles   Time(us)       MacUsage(%)    WorkLoad(0/1/2)-ImproveTherical        Task Number    Task Size      Regcmd Size    RW(KB)         FullName        
    1    InputOperator    UINT8    CPU    \                                            (1,3,224,224)          0              0              0              21             \              0.0%/0.0%/0.0% - Up:0.0%               0              0              0              147.00         InputOperator:data
    2    ConvRelu         UINT8    NPU    (1,3,224,224),(32,3,3,3),(32)                (1,32,112,112)         94150          10584          94150          375            2.82           0.0%/0.0%/0.0% - Up:0.0%               0              0              0              543.75         Conv:conv1      

    ...

    Total Operator Elapsed Time(us): 20274
    Total Memory RW Amount(MB): 0
    Operator Time-Consuming Ranking:
    OpType           Call Number     CPU Time(us)    NPU Time(us)    Total Time(us)    Time Ratio(%)
    ConvRelu         36              0               13166           13166             64.94           
    ConvAdd          10              0               3677            3677              18.13           
    Conv             9               0               2878            2878              14.19           
    exSoftmax13      1               387             0               387               1.9             
    exDataConvert    1               0               120             120               0.59            
    OutputOperator   1               25              0               25                0.12            
    InputOperator    1               21              0               21                0.1     
    ===================================================================================================================
    ```
- Print memory information
    ```
    ======================================================
                Memory Profile Info Dump                  
    ======================================================
    NPU model memory detail(bytes):
        Total Weight Memory: 3.53 MiB
        Total Internal Tensor Memory: 1.67 MiB
        Total Memory: 5.58 MiB

    INFO: When evaluating memory usage, we need consider  
    the size of model, current model size is: 4.00 MiB       
    ======================================================
    ```
- Print the TOP5 labels and corresponding scores of the test image classification results
    ```
    -----TOP 5-----
    [155]: 0.99462890625
    [204]: 0.0019969940185546875
    [154]: 0.0017757415771484375
    [283]: 0.0006690025329589844
    [284]: 0.0001976490020751953
    ```
- Output the results of the accuracy analysis:
    ```
    # simulator_error: calculate the simulator errors.
    #              entire: errors between 'golden' and 'simulator'.
    #              single: single layer errors. (compare to 'entire', the input of each layer is come from 'golden')!
    # runtime_error: calculate the runtime errors.
    #              entire: errors between 'golden' and 'runtime'.
    #              single_sim: single layer errors between 'simulator' and 'runtime'.
    # ('nan' means that tensor are 'all zeros', or 'all equal', or 'large values', etc)

    layer_name                         simulator_error          runtime_error     
                                    entire    single         entire   single_sim     
    ------------------------------------------------------------------------------
    [Input] data                     1.000000  1.000000       
    [exDataConvert] data_int8        0.999986  0.999986       
    [Conv] conv1/scale               
    [Relu] relu1                     0.999967  0.999967       0.999960  0.999942       
    [Conv] conv2_1/expand/scale      
    [Relu] relu2_1/expand            0.999929  0.999940       0.999924  0.999998       
    [Conv] conv2_1/dwise/scale       
    [Relu] relu2_1/dwise             0.999491  0.999585       0.999489  0.999998       
    [Conv] conv2_1/linear/scale      0.996095  0.999104       0.996004  0.999993  

    ...

    [Relu] relu6_4                   0.979375  0.999889       0.982138  0.999999       
    [Conv] pool6                     0.985147  0.999967       0.987996  1.000000       
    [Conv] fc7                       0.982232  0.999905       0.984954  1.000000       
    [exDataConvert] fc7__float16     0.982232  0.999957       0.984955  1.000000       
    [exSoftmax13] prob               1.000000  1.000000       0.999999  1.000000 
    ```
- Note: Different platforms, different versions of tools and drivers may have slightly different results.