import numpy as np
import cv2
from rknn.api import RKNN


def show_outputs(outputs):
    output = outputs[0][0]
    output_sorted = sorted(output, reverse=True)
    top5_str = 'mobilenet_v1\n-----TOP 5-----\n'
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
    rknn.config(mean_values=[128, 128, 128], std_values=[128, 128, 128],
                quantized_method='layer', quantized_algorithm='mmse')
    print('done')

    # Load model
    print('--> Loading model')
    ret = rknn.load_tensorflow(tf_pb='mobilenet_v1.pb',
                               inputs=['input'],
                               input_size_list=[[1, 224, 224, 3]],
                               outputs=['MobilenetV1/Logits/SpatialSqueeze'])
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

    # Accuracy analysis
    print('--> Accuracy analysis')
    ret = rknn.accuracy_analysis(inputs=['dog_224x224.jpg'], output_dir=None)
    if ret != 0:
        print('Accuracy analysis failed!')
        exit(ret)
    print('done')

    f = open('./snapshot/error_analysis.txt')
    lines = f.readlines()
    cos = lines[-1].split()[1]
    if float(cos) >= 0.965:
        print('cos = {}, mmse work!'.format(cos))
    else:
        print('cos = {} < 0.965, mmse abnormal!'.format(cos))
    f.close()

    # Set inputs
    img = cv2.imread('./dog_224x224.jpg')
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    img = np.expand_dims(img, 0)

    # Init runtime environment
    print('--> Init runtime environment')
    ret = rknn.init_runtime()
    if ret != 0:
        print('Init runtime environment failed!')
        exit(ret)
    print('done')

    # Inference
    print('--> Running model')
    outputs = rknn.inference(inputs=[img])
    np.save('./functions_mmse_0.npy', outputs[0])
    show_outputs(outputs)
    print('done')

    rknn.release()
