import numpy as np
import cv2
from rknn.api import RKNN


def show_outputs(outputs):
    output_ = outputs[0].reshape((-1, 1000))
    fp = open('./labels.txt', 'r')
    labels = fp.readlines()
    for batch, output in enumerate(output_):
        index = sorted(range(len(output)), key=lambda k : output[k], reverse=True)
        top5_str = '----- Batch {}: TOP 5 -----\n'.format(batch)
        for i in range(5):
            value = output[index[i]]
            if value > 0:
                topi = '[{:>3d}] score:{:.6f} class:"{}"\n'.format(index[i], value, labels[index[i]].strip().split(':')[-1])
            else:
                topi = '[ -1]: 0.0\n'
            top5_str += topi
        print(top5_str.strip())


def show_perfs(perfs):
    perfs = 'perfs: {}\n'.format(outputs)
    print(perfs)


if __name__ == '__main__':

    # Create RKNN object
    rknn = RKNN(verbose=True)

    # Pre-process config
    print('--> Config model')
    rknn.config(mean_values=[103.94, 116.78, 123.68], std_values=[58.82, 58.82, 58.82], quant_img_RGB2BGR=True, target_platform='rk3566')
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
    ret = rknn.build(do_quantization=True, dataset='./dataset.txt', rknn_batch_size=4)
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

    # Set inputs
    img_0 = cv2.imread('./dog_224x224.jpg')
    img_0 = np.expand_dims(img_0, 0)
    img_1 = cv2.imread('./goldfish_224x224.jpg')
    img_1 = np.expand_dims(img_1, 0)
    img_2 = cv2.imread('./space_shuttle_224.jpg')
    img_2 = np.expand_dims(img_2, 0)
    
    img = np.concatenate((img_0, img_1, img_2, img_0), axis=0)  # the inputs data need to be merged together using np.concatenate. 

    # Init runtime environment
    print('--> Init runtime environment')
    ret = rknn.init_runtime()
    if ret != 0:
        print('Init runtime environment failed!')
        exit(ret)
    print('done')

    # Inference
    print('--> Running model')
    outputs = rknn.inference(inputs=[img], data_format=['nhwc'])
    np.save('./functions_multi_batch_0.npy', outputs[0])
    show_outputs(outputs)
    print('done')

    rknn.release()
