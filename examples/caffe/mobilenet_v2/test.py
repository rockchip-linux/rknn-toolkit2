import numpy as np
import cv2
from rknn.api import RKNN

def show_outputs(outputs):
    np.save('./caffe_mobilenet_v2_0.npy', outputs[0])
    output = outputs[0].reshape(-1)
    output_sorted = sorted(output, reverse=True)
    top5_str = 'mobilenet_v2\n-----TOP 5-----\n'
    for i in range(5):
        value = output_sorted[i]
        index = np.where(output == value)
        for j in range(len(index)):
            if (i + j) >= 5:
                break
            if value > 0:
                topi = '{}: {}\n'.format(index[j], value)
            else:
                topi = '-1: 0.0\n'
            top5_str += topi
    print(top5_str)

if __name__ == '__main__':

    # Create RKNN object
    rknn = RKNN()
    
    # pre-process config
    print('--> config model')
    rknn.config(mean_values=[103.94, 116.78, 123.68], std_values=[58.82, 58.82, 58.82], quant_img_RGB2BGR=True)
    print('done')

    # Load tensorflow model
    print('--> Loading model')
    ret = rknn.load_caffe(model='./mobilenet_v2.prototxt',
                          proto='caffe',
                          blobs='./mobilenet_v2.caffemodel')
    if ret != 0:
        print('Load mobilenet_v2 failed! Ret = {}'.format(ret))
        exit(ret)
    print('done')

    # Build model
    print('--> Building model')
    ret = rknn.build(do_quantization=True, dataset='./dataset.txt')
    if ret != 0:
        print('Build mobilenet_v2 failed!')
        exit(ret)
    print('done')

    # Export rknn model
    print('--> Export RKNN model')
    ret = rknn.export_rknn('./mobilenet_v2.rknn')
    if ret != 0:
        print('Export mobilenet_v2.rknn failed!')
        exit(ret)
    print('done')

    # Set inputs
    img = cv2.imread('./goldfish_224x224.jpg')

    print('--> Init runtime environment')
    ret = rknn.init_runtime()
    if ret != 0:
        print('Init runtime environment failed')
        exit(ret)
    print('done')

    # Inference
    print('--> Running model')
    outputs = rknn.inference(inputs=[img])
    show_outputs(outputs)
    print('done')

    rknn.release()

