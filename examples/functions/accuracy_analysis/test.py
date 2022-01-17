import os
import urllib
import traceback
import time
import sys
import numpy as np
import cv2
from rknn.api import RKNN
import urllib.request

ONNX_MODEL = 'resnet50v2.onnx'
RKNN_MODEL = 'resnet50v2.rknn'


def show_outputs(outputs):
    output = outputs
    output_sorted = sorted(output, reverse=True)
    top5_str = 'resnet50v2\n-----TOP 5-----\n'
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


def readable_speed(speed):
    speed_bytes = float(speed)
    speed_kbytes = speed_bytes / 1024
    if speed_kbytes > 1024:
        speed_mbytes = speed_kbytes / 1024
        if speed_mbytes > 1024:
            speed_gbytes = speed_mbytes / 1024
            return "{:.2f} GB/s".format(speed_gbytes)
        else:
            return "{:.2f} MB/s".format(speed_mbytes)
    else:
        return "{:.2f} KB/s".format(speed_kbytes)


def show_progress(blocknum, blocksize, totalsize):
    speed = (blocknum * blocksize) / (time.time() - start_time)
    speed_str = " Speed: {}".format(readable_speed(speed))
    recv_size = blocknum * blocksize

    f = sys.stdout
    progress = (recv_size / totalsize)
    progress_str = "{:.2f}%".format(progress * 100)
    n = round(progress * 50)
    s = ('#' * n).ljust(50, '-')
    f.write(progress_str.ljust(8, ' ') + '[' + s + ']' + speed_str)
    f.flush()
    f.write('\r\n')


if __name__ == '__main__':

    # Create RKNN object
    rknn = RKNN(verbose=True)

    # If resnet50v2 does not exist, download it.
    # Download address:
    # https://s3.amazonaws.com/onnx-model-zoo/resnet/resnet50v2/resnet50v2.onnx
    if not os.path.exists(ONNX_MODEL):
        print('--> Download {}'.format(ONNX_MODEL))
        url = 'https://s3.amazonaws.com/onnx-model-zoo/resnet/resnet50v2/resnet50v2.onnx'
        download_file = ONNX_MODEL
        try:
            start_time = time.time()
            urllib.request.urlretrieve(url, download_file, show_progress)
        except:
            print('Download {} failed.'.format(download_file))
            print(traceback.format_exc())
            exit(-1)
        print('done')

    # Pre-process config
    print('--> Config model')
    rknn.config(mean_values=[123.68, 116.28, 103.53], std_values=[57.38, 57.38, 57.38])
    print('done')

    # Load model
    print('--> Loading model')
    ret = rknn.load_onnx(model=ONNX_MODEL)
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
    ret = rknn.accuracy_analysis(inputs=['./dog_224x224.jpg'], output_dir='./snapshot')
    if ret != 0:
        print('Accuracy analysis failed!')
        exit(ret)
    print('done')

    print('float32:')
    output = np.genfromtxt('./snapshot/golden/resnetv24_dense0_fwd.txt')
    show_outputs(output)

    print('quantized:')
    output = np.genfromtxt('./snapshot/simulator/resnetv24_dense0_fwd.txt')
    show_outputs(output)

    rknn.release()
