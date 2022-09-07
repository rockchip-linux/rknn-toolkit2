import numpy as np
import cv2
from rknn.api import RKNN

if __name__ == '__main__':

    # Create RKNN object
    rknn = RKNN(verbose=True)
    
    # Pre-process config
    print('--> Config model')
    rknn.config(mean_values=[127.5, 127.5, 127.5], std_values=[127.5, 127.5, 127.5])
    print('done')

    # Load model
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
    print('Please modify ssd_mobilenet_v2.quantization.cfg!')
    print('==================================================================================================')
    print('Modify Method: Fill the customized_quantize_layers with the output name & dtype of the custom layer.')
    print('')
    print('For example:')
    print('    custom_quantize_layers:')
    print('        FeatureExtractor/MobilenetV2/expanded_conv/depthwise/BatchNorm/batchnorm/add_1:0: float16')
    print('        FeatureExtractor/MobilenetV2/expanded_conv/depthwise/Relu6:0: float16')
    print('Or:')
    print('    custom_quantize_layers: {')
    print('        FeatureExtractor/MobilenetV2/expanded_conv/depthwise/BatchNorm/batchnorm/add_1:0: float16,')
    print('        FeatureExtractor/MobilenetV2/expanded_conv/depthwise/Relu6:0: float16,')
    print('    }')
    print('==================================================================================================')

    rknn.release()

