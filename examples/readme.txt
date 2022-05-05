The directory structure of examples is as follows:
.
├── caffe
│   ├── mobilenet_v2                    # mobilenet_v2 float model
│   └── vgg-ssd                         # vgg-ssd float model
├── onnx
│   ├── resnet50v2                      # resnet50v2 float model
│   └── yolov5                          # yolov5 float model
├── pytorch
│   ├── resnet18                        # resnet18 float model
│   ├── resnet18_qat                    # resnet18 QAT model
│   └── resnet18_export_onnx            # how to export onnx model from pytorch
├── tensorflow
│   ├── ssd_mobilenet_v1                # ssd_mobilenet_v1 float model
│   └── inception_v3_qat                # inception_v3 QAT model
├── tflite
│   ├── mobilenet_v1                    # mobilenet_v1 float model
│   └── mobilenet_v1_qat                # mobilenet_v1 QAT model
├── darknet
│   └── yolov3_416x416                  # yolov3 float model
└── functions
    ├── accuracy_analysis               # how to use accuracy-analysis function
    ├── batch_size                      # how to expand batch for use multi-batch function
    ├── multi_input_test                # multi-input float model
    ├── hybrid_quant                    # how to use hybrid-quantization function
    ├── mmse                            # how to use mmse function
    └── board_test                      # how to connect the board for debugging
