# RKNNToolkit2 OPs Support

## Explanation of terms:

**Remarks**:

    Operators' specifications must meet the remarks' requirements.

**Broadcast rule**: 

- per-layer： 

        shape(A) = (2, 3, 4, 5), shape(B) = (,), i.e. B is a scalar ==> shape(result) = (2, 3, 4, 5)

- per-channel: 
      
        shape(A) = (2, 3, 4, 5), shape(B) = (3,), ==> shape(result) = (2, 3, 4, 5)
        
        shape(A) = (2, 3, 4, 5), shape(B) = (1,3,1,1), ==> shape(result) = (2, 3, 4, 5)
    
- per-element: 

        shape(A) = (2, 3, 4, 5), shape(B) = (2,3,4,5) ==> shape(result) = (2, 3, 4, 5)

- other:

        shape(A) = (2, 3, 4, 5), shape(B) = (5,) ==> shape(result) = (2, 3, 4, 5)

**Input Size Restrictions Description**


Assuming that input size is [N,H,W,C] (layout is NHWC)

- Case 1: the first layer is **Convolution**, whose kernel size is [kernel_height, kernel_width]

  **W * kernel_height < 7168**

  **kernel_height * kernel_width < 128**


- Case 2: first layer is not Convolution, and C == 1 or C == 3 or C == 4

  **W < 7168**

- others:
  
  **No Restrictions**

  
  
  


## ONNX OPs supported by RKNN Toolkit2

According to [ONNX official instructions](https://github.com/microsoft/onnxruntime/blob/master/docs/Versioning.md 'ONNX Version Description'), the corresponding ONNX opset version is 12.  
The list of ONNX OPs supported by RKNN Toolkit2 is as follows:

| **Operators**         | **Remarks**                                                                                                                                                                                                         |
| --------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Add                   | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element                                                                                              |
| AveragePool           | channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br />auto_pad: NOTSET<br />count_include_pad: 1 <br />ceil_mode: 0                      |
| ArgMin                |                                                                                                                                                                                                                     |
| ArgMax                |                                                                                                                                                                                                                     |
| BatchNormalization    | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| cast                  | only support bool/int8/float                                                                                                                                                                                        |
| Clip                  | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| Concat                |                                                                                                                                                                                                                     |
| Conv                  | channel: [0, 150000] <br />kernel height/width: [1, 31]<br />stride height/width: [1, 7]<br />pad left/right/top/bottom: [0, 15]                                                                                    |
| ConvTranspose         | channel: [0, 150000]<br />kernel height/width: [1, 31]<br />stride height/width: 2, 4, 8<br />kernels: [1, 8192]<br />pad left/right/top/bottom: [0, 15]                                                            |
| DepthToSpace          |                                                                                                                                                                                                                     |
| Div                   | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8192]<br />support broadcast rule: per-element/other                                                                                                      |
| Elu                   | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />                                                                                                                                               |
| Exp                   |                                                                                                                                                                                                                     |
| Flatten               |                                                                                                                                                                                                                     |
| Gather                |                                                                                                                                                                                                                     |
| Gemm                  | channel: [0, 150000]<br />One input should be Const                                                                                                                                                                 |
| GlobalAveragePool     | channel: [1, 8192]<br />kernel height/width: [1, 343]<br />                                                                                                                                                         |
| GlobalMaxPool         | channel: [1, 8192]<br />kernel height/width: [1, 343]<br />                                                                                                                                                         |
| Greater               | support broadcast rule: per-element/other                                                                                                                                                                           |
| HardSigmoid           | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| HardSwish             | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| InstanceNormalization |                                                                                                                                                                                                                     |
| LeakyRelu             | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| Less                  | support broadcast rule: per-element/other                                                                                                                                                                           |
| LpNormalization       |                                                                                                                                                                                                                     |
| LRN                   |                                                                                                                                                                                                                     |
| LSTM                  | batchsize: 1<br />input_forget: 0                                                                                                                                                                                   |
| GRU                   | batchsize: 1                                                                                                                                                                                                        |
| MatMul                |                                                                                                                                                                                                                     |
| Max                   | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| MaxPool               | channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br />auto_pad: NOTSET<br />ceil_mode: 0<br />dilations: unsupport<br />storage_order: 0 |
| MaxRoiPool            |                                                                                                                                                                                                                     |
| MaxUnpool             | unsupport pad                                                                                                                                                                                                       |
| Min                   | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| Mul                   | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element                                                                                              |
| Pad                   | width: [1, 8176]<br />mode: constant<br />pads n_begin/n_end/c_begin/c_end: 1                                                                                                                                       |
| Pow                   |                                                                                                                                                                                                                     |
| PRelu                 | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />slope support broadcast rule: per-layer/channel                                                                                                |
| ReduceMax             |                                                                                                                                                                                                                     |
| ReduceMean            | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8192]                                                                                                                                                     |
| ReduceSum             | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8192]                                                                                                                                                     |
| ReduceMin             |                                                                                                                                                                                                                     |
| Relu                  | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| Reshape               | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| Resize                | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />mode: nearest<br />scales: [1, 8]                                                                                                              |
| ReverseSequence       | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />batch_axis: 1 <br />time_axis: 0                                                                                                               |
| RoiAlign              | pool type: average                                                                                                                                                                                                  |
| Sigmoid               | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| Slice                 | steps: 1                                                                                                                                                                                                            |
| Softmax               | channel: [1, 8192]<br />axis: 1                                                                                                                                                                                     |
| SpaceToDetph          |                                                                                                                                                                                                                     |
| Split                 | axis: 1(channel)                                                                                                                                                                                                    |
| Sub                   | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element                                                                                              |
| Squeeze               |                                                                                                                                                                                                                     |
| Tanh                  | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| Tile                  |                                                                                                                                                                                                                     |
| Transpose             | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                                                                     |
| Upsample (resize)     | coordinate_transformation_mode unsupport tf_crop_and_resize                                                                                                                                                         |


## Caffe OPs supported by RKNN Toolkit2

Caffe protocols RKNN Toolkit2 uses only based on the officially modified protocol of berkeley.
The protocol based on the official revision of berkeley comes from [berkeley caffe](https://github.com/BVLC/caffe/tree/master/src/caffe/proto 'Berkeley Caffe'), commit hash is 21d0608. On this basis RKNN Toolkit2 have added some OPs.  
Based on this protocol, the list of Caffe OPs supported by RKNN Toolkit2 is as follows:

| **Operators**          | **Remarks**                                                                                                            |
| ---------------------- | ---------------------------------------------------------------------------------------------------------------------- |
| BatchNorm              | same as onnx BatchNormalization                                                                                        |
| bn (BatchNorm + Scale) | same as onnx BatchNormalization according to https://github.com/TimoSaemann/caffe-segnet-cudnn5                        |
| BNLL                   |                                                                                                                        |
| Concat                 | same as onnx Concat                                                                                                    |
| Convolution            | same as onnx Conv                                                                                                      |
| ConvolutionDepthwise   | kernel height/width: [1, 8]<br />kernels: 1<br />others same as onnx Conv                                              |
| Crop                   |                                                                                                                        |
| Deconvolution          | same as ConvTranspose                                                                                                  |
| Dropout                |                                                                                                                        |
| Eltwise                | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element |
| Flatten                |                                                                                                                        |
| HardSigmoid            |                                                                                                                        |
| InnerProduct           | same as onnx Gemm                                                                                                      |
| LRN                    | same as onnx LRN                                                                                                       |
| Lstm                   | same as onnx LSTM according to https://github.com/xmfbit/warpctc-caffe                                                 |
| Normalize              |                                                                                                                        |
| Permute                | same as onnx Transpose                                                                                                 |
| Power                  |                                                                                                                        |
| Pooling                | same as onnx pooling                                                                                                   |
| PRelu                  | same as onnx PRelu                                                                                                     |
| Proposal               | batch: 1                                                                                                               |
| Reduction              | output dims <= 4                                                                                                       |
| Relu                   | same as onnx Relu                                                                                                      |
| Relu6                  | same as onnx Clip                                                                                                      |
| Reorg                  |                                                                                                                        |
| Reshape                | same as onnx Reshape                                                                                                   |
| Resize                 | bilinear; nearest                                                                                                      |
| Reverse                |                                                                                                                        |
| ROIPooling             | same as MaxRoiPool according to https://github.com/twmht/caffe-pva-faster-rcnn                                         |
| Scale                  | same as onnx Mul                                                                                                       |
| Sigmoid                | same as onnx Sigmoid                                                                                                   |
| Slice                  | same as onnx Split                                                                                                     |
| Softmax                | same as onnx Softmax                                                                                                   |
| Split                  | same as onnx Slice                                                                                                     |
| TanH                   | same as onnx TanH                                                                                                      |
| Tile                   | same as onnx Tile                                                                                                      |
| Transpose              | same as onnx Transpose                                                                                                 |
| Upsample               | according to https://github.com/SeanQ88/caffe_upsample and https://github.com/TimoSaemann/caffe-segnet-cudnn5          |


## Pytorch OPs supported by RKNN Toolkit2

The Pytorch version supported by RKNN Toolkit2 is >1.6.0, models generated by other versions may not support.  
The list of Pytorch OPs supported by RKNN Toolkit2 is as follows:

| **Operators**             | **Remarks**                                                     |
| ------------------------- | --------------------------------------------------------------- |
| aten::_convolution        | same as onnx Conv                                               |
| aten::add                 | same as onnx Add                                                |
| aten::avg_pool2d          | same as onnx AveragePool                                        |
| aten::batch_norm          | same as onnx BatchNormalization                                 |
| aten::cat                 | same as onnx Concat                                             |
| aten::chunk               |                                                                 |
| aten::dropout             |                                                                 |
| aten::elu                 | same as onnx Elu                                                |
| aten::flatten             |                                                                 |
| aten::hardswish           | same as onnx HardSwish                                          |
| aten::instance_norm       | same as onnx InstanceNormalization                              |
| aten::layer_norm          | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8192] |
| aten::leaky_relu          | same as onnx LeakyRelu                                          |
| aten::linear              | same as onnx Gemm                                               |
| aten::matmul              | same as onnx MatMul                                             |
| aten::max_pool2d          | same as onnx MaxPool                                            |
| aten::mean                | output dims <= 4                                                |
| aten::mul                 | same as onnx Mul                                                |
| aten::pad                 | same as onnx Pad                                                |
| aten::permute             | same as onnx Transpose                                          |
| aten::prelu               | same as onnx PRelu                                              |
| aten::relu                | same as onnx Relu                                               |
| aten::reshape             |                                                                 |
| aten::sigmoid             | same as onnx Sigmoid                                            |
| aten::slice               | same as onnx Slice                                              |
| aten::split               | same as onnx Split                                              |
| aten::softmax             | same as onnx Softmax                                            |
| aten::squeeze             | same as onnx Squeeze                                            |
| aten::sum                 | output dims <= 4                                                |
| aten::tanh                | same as onnx TanH                                               |
| aten::upsample_bilinear2d | same as onnx Resize                                             |
| aten::upsample_nearest2d  | same as onnx Resize                                             |
| aten::view                | same as onnx Reshape                                            |

## TensorFlow OPs supported by RKNN Toolkit2

The pb files (contain OPs belows) generated by TensorFlow version 1.12 - 1.15 for 1.x and 2.3 - 2.5 for 2.x are supported by RKNN Toolkit2. For more information on TensorFlow version compatibility, please refer to [tensorflow official instructions on OP version](https://www.tensorflow.org/guide/versions 'Tensorflow official instructions on OP version') . 
The list of TensorFlow OPs supported by RKNN Toolkit2 is as follows:

| **Operators**         | **Remarks**                                                               |
| --------------------- | ------------------------------------------------------------------------- |
| Add                   | same as onnx Add                                                          |
| AvgPool               | same as onnx AveragePool                                                  |
| Concat                | same as onnx Concat                                                       |
| Conv2D                | same as onnx Conv                                                         |
| DepthToSpace          |                                                                           |
| DepthwiseConv2d       | kernel height/width: [1, 8]<br />kernels: 1<br />others same as onnx Conv |
| Div                   | same as onnx Div                                                          |
| Dropout               |                                                                           |
| Flatten               |                                                                           |
| LeakyRelu             | same as onnx LeakyRelu                                                    |
| Less                  | same as onnx Less                                                         |
| LRN                   |                                                                           |
| MatMul                |                                                                           |
| MaxPool               | same as onnx MaxPool                                                      |
| Mean                  | output dims <= 4                                                          |
| Pad                   | same as onnx Pad                                                          |
| Relu                  | same as onnx Relu                                                         |
| Reshape               |                                                                           |
| ResizeBilinear        |                                                                           |
| ResizeNearestNeighbor |                                                                           |
| Sigmoid               |                                                                           |
| Slice                 |                                                                           |
| Softmax               |                                                                           |
| Softplus              | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]           |
| SpaceToDepth          |                                                                           |
| Split                 |                                                                           |
| Squeeze               |                                                                           |
| StridedSlice          |                                                                           |
| Tanh                  | same as onnx TanH                                                         |
| Transpose             |                                                                           |

## Darknet OPs supported by RKNN Toolkit2
The list of Darknet OPs supported by RKNN Toolkit2 is as follows:

| **Operators**           | **Remarks**                                                                                                                                                                   |
| ----------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| add                     | same as onnx Add                                                                                                                                                              |
| batchnormalize          | same as onnx BatchNormalization                                                                                                                                               |
| concat                  | same as onnx Concat                                                                                                                                                           |
| convolutional           | same as onnx Conv                                                                                                                                                             |
| depthwise_convolutional | kernel height/width: [1, 8]<br />kernels: 1<br />others same as onnx Conv                                                                                                     |
| fullconnect             |                                                                                                                                                                               |
| leakyrelu               | same as onnx LeakyRelu                                                                                                                                                        |
| mish                    | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                               |
| pooling                 | **AveragePool**: same as onnx AveragePool   <br /> **GlobalAveragePool**: same as onnx GlobalAveragePool <br /> **MaxPool/GlobalMaxPool**: same as onnx MaxPool/GlobalMaxPool |
| route                   |                                                                                                                                                                               |
| shortcut                |                                                                                                                                                                               |
| softmax                 |                                                                                                                                                                               |
| upsampling              |                                                                                                                                                                               |