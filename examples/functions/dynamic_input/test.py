import numpy as np
import cv2
from rknn.api import RKNN


def show_outputs(outputs):
    output_ = outputs[0].reshape((-1, 1000))
    for output in output_:
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


def show_perfs(perfs):
    perfs = 'perfs: {}\n'.format(outputs)
    print(perfs)


if __name__ == '__main__':

    # Create RKNN object
    rknn = RKNN(verbose=True)

    # The multiple sets of input shapes specified by the user, to simulate the function of dynamic input.
    # Please make sure the model can be dynamic when enable 'config.dynamic_input', and shape in dynamic_input are correctly!
    dynamic_input = [
        [[1,3,192,192]],    # set 0: [input0_192]
        [[1,3,256,256]],    # set 1: [input0_256]
        [[1,3,160,160]],    # set 2: [input0_160]
        [[1,3,224,224]],    # set 3: [input0_224]
    ]

    # Pre-process config
    print('--> Config model')
    rknn.config(mean_values=[103.94, 116.78, 123.68], std_values=[58.82, 58.82, 58.82], quant_img_RGB2BGR=True, target_platform='rk3566', dynamic_input=dynamic_input)
    print('done')

    # Load model (from https://github.com/shicai/MobileNet-Caffe)
    print('--> Loading model')
    ret = rknn.load_caffe(model='../../caffe/mobilenet_v2/mobilenet_v2_deploy.prototxt',
                          blobs='../../caffe/mobilenet_v2/mobilenet_v2.caffemodel')
    if ret != 0:
        print('Load model failed!')
        exit(ret)
    print('done')

    # Build model
    print('--> Building model')
    ret = rknn.build(do_quantization=True, dataset='../../caffe/mobilenet_v2/dataset.txt')
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

    # Init runtime environment
    print('--> Init runtime environment')
    ret = rknn.init_runtime()
    if ret != 0:
        print('Init runtime environment failed!')
        exit(ret)
    print('done')

    # Set inputs
    img = cv2.imread('./dog_224x224.jpg')

    # Inference
    print('--> Running model')
    img2 = cv2.resize(img, (224,224))
    img2 = np.expand_dims(img2, 0)
    img2 = np.transpose(img2, (0,3,1,2))    # [1,3,224,224]
    outputs = rknn.inference(inputs=[img2], data_format=['nchw'])
    np.save('./functions_dynamic_input_0.npy', outputs[0])
    show_outputs(outputs)

    img3 = cv2.resize(img, (160,160))
    img3 = np.expand_dims(img3, 0)
    img3 = np.transpose(img3, (0,3,1,2))    # [1,3,160,160]
    outputs = rknn.inference(inputs=[img3], data_format=['nchw'])
    np.save('./functions_dynamic_input_1.npy', outputs[0])
    show_outputs(outputs)

    img4 = cv2.resize(img, (256,256))
    img4 = np.expand_dims(img4, 0)
    img4 = np.transpose(img4, (0,3,1,2))    # [1,3,256,256]
    outputs = rknn.inference(inputs=[img4], data_format=['nchw'])
    np.save('./functions_dynamic_input_2.npy', outputs[0])
    show_outputs(outputs)
    print('done')

    rknn.release()
