# RKNNToolkit2 OPs Support

Base on RKNN Toolkit2 Version 0.6.0

## Explanation of terms:

Remarks:

    Operators' specifications must meet the remarks' requirements.

Broadcast rule: 

- per-layer： 

        shape(A) = (2, 3, 4, 5), shape(B) = (,), i.e. B is a scalar ==> shape(result) = (2, 3, 4, 5)

- per-channel: 
      
        shape(A) = (2, 3, 4, 5), shape(B) = (3,), ==> shape(result) = (2, 3, 4, 5)
        
        shape(A) = (2, 3, 4, 5), shape(B) = (1,3,1,1), ==> shape(result) = (2, 3, 4, 5)
    
- per-element: 

        shape(A) = (2, 3, 4, 5), shape(B) = (2,3,4,5) ==> shape(result) = (2, 3, 4, 5)

- other:

        shape(A) = (2, 3, 4, 5), shape(B) = (5,) ==> shape(result) = (2, 3, 4, 5)



## Caffe OPs supported by RKNN Toolkit2

Caffe protocols RKNN Toolkit2 uses only based on the officially modified protocol of berkeley.
The protocol based on the official revision of berkeley comes from [berkeley caffe](https://github.com/BVLC/caffe/tree/master/src/caffe/proto 'Berkeley Caffe'), commit hash is 21d0608. On this basis RKNN Toolkit2 have added some OPs.  
Based on this protocol, the list of Caffe OPs supported by RKNN Toolkit2 Version 0.6.0 is as follows:

| **Operators**        | **Remarks** |
| -------------------- | ----------- |
| BatchNorm           |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| bn (BatchNorm + Scale) |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br /> according to https://github.com/TimoSaemann/caffe-segnet-cudnn5|
| BNLL                ||
| Concat              |axis: 1,2,3|
| Convolution         |channel: [1, 8192]<br />kernel height/width: [1, 31]<br />stride height/width: [1, 7]<br />kernels: [1, 8184]<br />pad left/right/top/bottom: [0, 15]<br />group: 1, channel / N <br /><br />|
| ConvolutionDepthwise|channel:[1, 8192]<br />kernel height/width: [1, 8]<br />stride height/width: [1, 7]<br />kernels: 1<br />pad left/right/top/bottom: [0, 15]|
| Crop                ||
| Deconvolution       |channel: [1, 8192]<br />kernel height/width: [1, 31]<br />stride height/width: 2, 4, 8<br />kernels: [1, 8192]<br />pad left/right/top/bottom: [0, 15]|
| Dropout             ||
| Eltwise             |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element|
| Flatten             ||
| InnerProduct        |channel: [1, 8192]|
| LRN                 ||
| Normalize           ||
| Permute             ||
| Power               ||
| Pooling             | **AveragePool**:<br />channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br /><br />**GlobalAveragePool**:<br />channel: [1, 8192]<br />kernel height/width: [1, 128]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7] <br /><br />**MaxPool/GlobalMaxPool**:<br />channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br /><br />**MaxPool**: <br />auto_pad only support NOTSET，ceil_mode only support 0，unsupport dilations |
| PRelu               |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />slope: per-layer/channel|
| Proposal            |batch: 1|
| Reduction           |output dims <= 4|
| Relu                |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Relu6               |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Reorg               ||
| Reshape             ||
| Resize              |bilinear; nearest|
| Reverse             ||
| ROIPooling          |according to https://github.com/twmht/caffe-pva-faster-rcnn|
| Scale               |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Sigmoid             |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Slice               ||
| Softmax             ||
| Split               ||
| TanH                |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Tile                ||
| Transpose           ||
| Upsample            |according to https://github.com/SeanQ88/caffe_upsample and https://github.com/TimoSaemann/caffe-segnet-cudnn5|

## ONNX OPs supported by RKNN Toolkit2

The ONNX version supported by RKNN Toolkit2 is 0.6.0. According to [ONNX official instructions](https://github.com/microsoft/onnxruntime/blob/master/docs/Versioning.md 'ONNX Version Description'), the corresponding ONNX opset version is 12.  
The list of ONNX OPs supported by RKNN Toolkit2 Version 0.6.0 is as follows:

| **Operators**                 | **Remarks** |
| ----------------------------- | ----------- |
| Add                           |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element|
| AveragePool           |channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]|
| BatchNormalization            |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Clip                          |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Concat                        |axis: 1,2,3|
| Conv                          |channel: [1, 8192]<br />kernel height/width: [1, 31]<br />stride height/width: [1, 7]<br />kernels: [1, 8184]<br />pad left/right/top/bottom: [0, 15]<br />dilation: [1, 31]<br />group: 1, channel / N|
| ConvTranspose                 |channel: [1, 8192]<br />kernel height/width: [1, 31]<br />stride height/width: 2, 4, 8<br />kernels: [1, 8192]<br />pad left/right/top/bottom: [0, 15]<br />dilation: [1, 31]<br />group: 1, channel / N|
| DepthToSpace                  ||
| Div                           |support broadcast rule: per-element/other|
| Flatten                       ||
| Gemm                          |channel: [1, 8192]<br /> One input should be Const|
| GlobalAveragePool     |channel: [1, 8192]<br />kernel height/width: [1, 128]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]|
| GlobalMaxPool        |channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]|
| Greater                       |support broadcast rule: per-element/other|
| HardSigmoid                   ||
| LeakyRelu                     |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Less                          |support broadcast rule: per-element/other|
| LpNormalization               ||
| LRN                           ||
| MatMul                        |channel: [1, 8192]<br />dims: 2|
| Max                           |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br /> dims=4|
| MaxPool                       |channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br />auto_pad only support NOTSET，ceil_mode only support 0，unsupport dilations|
| MaxRoiPool                    ||
| MaxUnpool                     |unsupport pad|
| Mul                           |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element|
| Pad                           |pad value should >= 0; pad dims must be 2 when mode is reflect or edge|
| Pow                           ||
| PRelu                         |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />slope support broadcast rule:: per-layer/channel|
| ReduceMean                    |output dims <= 4|
| ReduceSum                     |output dims <= 4|
| Relu                          |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Reshape                       ||
| Resize                        |bilinear; nearest2d|
| ReverseSequence               |            
| RoiAlign                      |pool type: average|
| Slice                         ||
| Softmax                       ||
| SpaceToDetph                  ||
| Split                         ||
| Squeeze                       ||
| Tanh                          |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Tile                          ||
| Transpose                     ||
| Upsample (resize)             ||                                            |

## Pytorch OPs supported by RKNN Toolkit2

The Pytorch version supported by RKNN Toolkit2 is 1.6.0, models generated by other versions may not support.  
The list of Pytorch OPs supported by RKNN Toolkit2 Version 0.6.0 is as follows:

| **Operators**             | **Remarks** |
| ------------------------- | ----------- |
| aten::_convolution        |channel: [1, 8192]<br />kernel height/width: [1, 31]<br />stride height/width: [1, 7]<br />kernels: [1, 8184]<br />pad left/right/top/bottom: [0, 15]<br />dilation: [1, 31]<br />group: 1, channel / N|
| aten::add                 |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element |
| aten::avg_pool2d          |channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]|
| aten::batch_norm          |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| aten::cat                 |axis: 1,2,3|
| aten::dropout             ||
| aten::flatten             ||
| aten::leaky_relu          |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| aten::matmul              |channel: [1, 8192]<br />dims: 2|
| aten::max_pool2d          |channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br />auto_pad only support NOTSET，ceil_mode only support 0，unsupport dilations|
| aten::mean                |output dims <= 4|
| aten::mul                 |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element |
| aten::pad                 |pad value should >= 0; pad dims must be 2 when mode is reflect or edge|
| aten::permute             ||
| aten::prelu               |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />slope support broadcast rule:: per-layer/channel|
| aten::relu                |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| aten::reshape             ||
| aten::sigmoid             ||
| aten::slice               ||
| aten::softmax             ||
| aten::squeeze             ||
| aten::sum                 |output dims <= 4|
| aten::upsample_bilinear2d ||
| aten::upsample_nearest2d  ||
| aten::view                ||

## TensorFlow OPs supported by RKNN Toolkit2

The pb files (contain OPs belows) generated by TensorFlow version 1.12 - 1.15 are supported by RKNN Toolkit2. For more information on TensorFlow version compatibility, please refer to [tensorflow official instructions on OP version](https://www.tensorflow.org/guide/versions 'Tensorflow official instructions on OP version') . 
The list of TensorFlow OPs supported by RKNN Toolkit2 is as follows:

| **Operators**                      | **Remarks** |
| ---------------------------------- | ----------- |
| AvgPool                |channel: [1, 8192]<br>kernel height/width: [1, 7]<br>stride height/width: [1, 8]<br>pad left/right/top/bottom: [0, 7]|
| Concat                 |axis: 1,2,3|
| Conv2D                 |channel: [1, 8192]<br>kernel height/width: [1, 31]<br />stride height/width: [1, 7]<br />kernels: [1, 8184]<br />pad left/right/top/bottom: [0, 15]<br />dilation: [1, 31]<br />group: 1, channel / N|
| DepthToSpace           ||
| DepthwiseConv2d        |channel:[1, 8192]<br />kernel height/width: [1, 8]<br />stride height/width: [1, 7]<br />kernels: 1<br />pad left/right/top/bottom: [0, 15]|
| Div                    |support broadcast rule: per-element/other|
| Dropout                ||
| Flatten                ||
| LeakyRelu              |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Less                   |support broadcast rule: per-element/other|
| LRN                    ||
| MatMul                 |channel: [1, 8192]<br />dims: 2|
| MaxPool                |channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br />auto_pad only support NOTSET，ceil_mode only support 0，unsupport dilations|
| Mean                   |output dims <= 4|
| Pad                    |pad value should >= 0; pad dims must be 2 when mode is reflect or edge|
| Relu                   |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Reshape                ||
| ResizeBilinear         ||
| ResizeNearestNeighbor  ||
| Sigmoid                ||
| Slice                  ||
| Softmax                ||
| Softplus               ||
| SpaceToDepth           ||
| Split                  ||
| Squeeze                ||
| StridedSlice           ||
| Tanh                   |channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
| Transpose              ||

## TensorFlow Lite OPs supported by RKNN Toolkit2
RKNN Toolkit2 uses the TF Lite schema commits in link: 
https://github.com/tensorflow/tensorflow/commits/master/tensorflow/lite/schema/schema.fbs  
Commit hash: 0c4f5dfea4ceb3d7c0b46fc04828420a344f7598.  
Because TF Lite schema may not compatible with each other, TF Lite models with older or newer schema may not be loaded successfully.  
The list of TensorFlow Lite OPs supported by RKNN Toolkit2 is as follows:

| **Operators** | **Remarks** |
|---| ----------- |
|ADD|channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element |
|AVERAGE_POOL_2D|channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]|
|CONCATENATION|axis: 1,2,3|
|CONV_2D_TRANSPOSE|channel: [1, 8192]<br />kernel height/width: [1, 31]<br />stride height/width: 2, 4, 8<br />kernels: [1, 8192]<br />pad left/right/top/bottom: [0, 15]<br />dilation: [1, 31]<br />group: 1, channel / N|
|CONV_2D|channel: [1, 8192]<br />kernel height/width: [1, 31]<br />stride height/width: [1, 7]<br />kernels: [1, 8184]<br />pad left/right/top/bottom: [0, 15]<br />dilation: [1, 31]<br />group: 1, channel / N|
|DEPTH_TO_SPACE||
|DEPTHWISE_CONV_2D|channel:[1, 8192]<br />kernel height/width: [1, 8]<br />stride height/width: [1, 7]<br />kernels: 1<br />pad left/right/top/bottom: [0, 15]|
|DIV|support broadcast rule: per-element/other |
|FULLY_CONNECTED|channel: [1, 8192]<br />|
|GREATER|support broadcast rule: per-element/other|
|L2_NORMALIZATION|dims: 4|
|LEAKY_RELU|channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
|LESS|support broadcast rule: per-element/other|
|LOCAL_RESPONSE_NORMALIZATION||
|MAX_POOL_2D|channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br />auto_pad only support NOTSET，ceil_mode only support 0，unsupport dilations|
|MUL|channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element |
|PAD|pad value should >= 0; pad dims must be 2 when mode is reflect or edge|
|PRELU|channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />slope support broadcast rule:: per-layer/channel|
|RELU|channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
|RESHAPE||
|RESIZE_BILINEAR||
|RESIZE_NEAREST_NEIGHBOR||
|SOFTMAX||
|SPACE_TO_DEPTH||
|SPLIT/SPLIT_V||
|SQUEEZE||
|STRIDED_SLICE||
|TANH|channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
|TRANSPOSE||

## Darknet OPs supported by RKNN Toolkit2
The list of Darknet OPs supported by RKNN Toolkit2 Version 0.6.0 is as follows:

| **Operators** | **Remarks** |
|---| ----------- |
|add|channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />support broadcast rule: per-layer/channel/element |
|batchnormalize|channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
|concat|axis: 1,2,3|
|convolutional|hannel: [1, 8192]<br />kernel height/width: [1, 31]<br />stride height/width: [1, 7]<br />kernels: [1, 8184]<br />pad left/right/top/bottom: [0, 15]<br />dilation: [1, 31]<br />group: 1, channel / N|
|depthwise_convolutional|channel:[1, 8192]<br />kernel height/width: [1, 8]<br />stride height/width: [1, 7]<br />kernels: 1<br />pad left/right/top/bottom: [0, 15]|
|fullconnect|channel: [1, 8192]|
|leakyrelu|channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]|
|mish||
|pooling|**AveragePool**:<br />channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br /><br /> **GlobalAveragePool**:<br /> channel: [1, 8192]<br />kernel height/width: [1, 128]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7] <br /><br /> **MaxPool/GlobalMaxPool**:<br /> channel: [1, 8192]<br />kernel height/width: [1, 7]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br />MaxPool: <br />auto_pad only support NOTSET，ceil_mode only support 0，unsupport dilations|
|route||
|shortcut||
|softmax||
|upsampling||