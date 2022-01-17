import numpy as np
import cv2
from rknn.api import RKNN


def show_outputs(outputs):
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
    rknn = RKNN(verbose=True)

    # Pre-process config
    print('--> Config model')
    rknn.config(mean_values=[103.94, 116.78, 123.68], std_values=[58.82, 58.82, 58.82],
                quant_img_RGB2BGR=True, target_platform='rk3566')
    print('done')

    # Load model
    print('--> Loading model')
    ret = rknn.load_caffe(model='./../../caffe/mobilenet_v2/mobilenet_v2.prototxt',
                          blobs='./../../caffe/mobilenet_v2/mobilenet_v2.caffemodel')
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
    ret = rknn.export_rknn('./mobilenet_v2.rknn')
    if ret != 0:
        print('Export rknn model failed!')
        exit(ret)
    print('done')

    # Export encrypted RKNN model
    print('--> Export encrypted rknn model')
    ret = rknn.export_encrypted_rknn_model('./mobilenet_v2.rknn', None, 3)

    # load rknn model
    print('--> Load rknn model')
    ret = rknn.load_rknn('./mobilenet_v2.rknn')
    if ret != 0:
        print('Load rknn model failed!')
        exit(ret)
    print('done')

    # Set inputs
    img = cv2.imread('./dog_224x224.jpg')

    # Init runtime environment
    print('--> Init runtime environment')
    ret = rknn.init_runtime(target='rk3566', perf_debug=True, eval_mem=True)
    if ret != 0:
        print('Init runtime environment failed!')
        exit(ret)
    print('done')

    # eval perf
    print('--> Eval perf')
    rknn.eval_perf(inputs=[img])

    # eval perf
    print('--> Eval memory')
    rknn.eval_memory()

    # Inference
    print('--> Running model')
    outputs = rknn.inference(inputs=[img])
    np.save('./functions_board_test_0.npy', outputs[0])
    show_outputs(outputs)
    print('done')

    # Accuracy analysis
    print('--> Accuracy analysis')
    ret = rknn.accuracy_analysis(inputs=['./dog_224x224.jpg'], output_dir='./snapshot', target='rk3566')
    if ret != 0:
        print('Accuracy analysis failed!')
        exit(ret)
    print('done')

    rknn.release()
