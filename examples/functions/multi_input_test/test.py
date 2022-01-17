import numpy as np
import cv2
from rknn.api import RKNN

if __name__ == '__main__':

    # Create RKNN object
    rknn = RKNN(verbose=True)

    # Pre-process config
    print('--> Config model')
    rknn.config(mean_values=[[127.5, 127.5, 127.5], [0, 0, 0], [0, 0, 0], [127.5]],
                std_values=[[128, 128, 128], [1, 1, 1], [1, 1, 1], [128]])
    print('done')

    # Load model
    print('--> Loading model')
    ret = rknn.load_tensorflow(tf_pb='./conv_128.pb',
                               inputs=['input1', 'input2', 'input3', 'input4'],
                               outputs=['output'],
                               input_size_list=[[1, 128, 128, 3], [1, 128, 128, 3], [1, 128, 128, 3], [1, 128, 128, 1]])
    if ret != 0:
        print('Load model failed!')
        exit(ret)
    print('done')

    # Build model
    print('--> Building model')
    ret = rknn.build(do_quantization=True, dataset='./dataset.txt')
    if ret != 0:
        print('Build model failed!')
        exit(ret)
    print('done')

    # Export rknn model
    print('--> Export rknn model')
    ret = rknn.export_rknn('./conv_128.rknn')
    if ret != 0:
        print('Export rknn model failed!')
        exit(ret)
    print('done')

    # Init runtime environment
    print('--> Init runtime environment')
    ret = rknn.init_runtime()
    if ret != 0:
        print('Init runtime environment failed!')
        exit(ret)
    print('done')

    # Set inputs
    img = cv2.imread('./dog_128x128.jpg')
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

    img_gray = cv2.imread('./dog_128x128_gray.png', cv2.IMREAD_GRAYSCALE)
    img_gray = np.expand_dims(img_gray, -1)

    input2 = np.load('input2.npy').astype('float32')

    input3 = np.load('input3.npy').astype('float32')

    # Inference
    print('--> Running model')
    outputs = rknn.inference(inputs=[img, input2, input3, img_gray])
    np.save('./functions_multi_input_test_0.npy', outputs[0])
    print('done')
    outputs[0] = outputs[0].reshape((1, -1))
    print('inference result: ', outputs)

    rknn.release()
