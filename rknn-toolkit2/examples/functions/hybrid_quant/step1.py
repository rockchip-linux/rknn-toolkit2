import numpy as np
import cv2
from rknn.api import RKNN

if __name__ == '__main__':

    # Create RKNN object
    rknn = RKNN(verbose=True)
    
    # Pre-process config
    print('--> Config model')
    rknn.config(mean_values=[127.5, 127.5, 127.5], std_values=[127.5, 127.5, 127.5], target_platform='rk3566')
    print('done')

    # Load model (from https://github.com/tensorflow/models/blob/master/research/object_detection/g3doc/tf1_detection_zoo.md  ssd_mobilenet_v2_coco)
    print('--> Loading model')
    ret = rknn.load_tensorflow(tf_pb='./ssd_mobilenet_v2.pb',
                               inputs=['FeatureExtractor/MobilenetV2/MobilenetV2/input'],
                               outputs=['concat_1', 'concat'],
                               input_size_list=[[1,300,300,3]])
    if ret != 0:
        print('Load model failed!')
        exit(ret)
    print('done')

    # Build model
    print('--> hybrid_quantization_step1')
    ret = rknn.hybrid_quantization_step1(dataset='./dataset.txt', proposal=False)
    if ret != 0:
        print('hybrid_quantization_step1 failed!')
        exit(ret)
    print('done')

    # Tips
    print("======================================== Tips ==========================================================")
    print("After the execution of 'rknn.hybrid_quantization_step1' is completed, a configuration file")
    print("'ssd_mobilenet_v2.quantization.cfg' will be generated. This configuration file is used to set the layer")
    print("that needs to be hybrid quantized. ")
    print("")
    print("Users can set the 'output name' and 'data type' of the layer that needs to be configured into")
    print("'custom_quantize_layers', as follows:")
    print("")
    print("     custom_quantize_layers:")
    print("         FeatureExtractor/MobilenetV2/expanded_conv/depthwise/BatchNorm/batchnorm/add_1:0: float16")
    print("         FeatureExtractor/MobilenetV2/expanded_conv/depthwise/Relu6:0: float16")
    print("")
    print("Currently supported data types are: 'int8', 'int16', 'float16'.")
    print("")
    print("In addition, the configuration file will automatically fill in some layers that may cause a decrease in")
    print("accuracy (may be inaccurate). Users can consider these layers together with the results of accuracy")
    print("analysis to obtain the layers that actually cause a decrease in accuracy and set them into")
    print("'custom_quantize_layers'.")
    print("")
    print("After modifying 'ssd_mobilenet_v2.quantization.cfg', you can continue to execute 'step2.py' to complete")
    print("the model conversion.")
    print("========================================================================================================")
    print("")

    rknn.release()

