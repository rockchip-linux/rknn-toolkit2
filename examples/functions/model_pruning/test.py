import numpy as np
import cv2
from rknn.api import RKNN


def show_outputs(outputs):
    np.save('./functions_model_pruning_0.npy', outputs[0])
    output = outputs[0].reshape(-1)
    output_sorted = sorted(output, reverse=True)
    top5_str = 'mobilenet\n-----TOP 5-----\n'
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
    rknn.config(mean_values=[103.94, 116.78, 123.68], std_values=[58.82, 58.82, 58.82], quant_img_RGB2BGR=True, target_platform='rk3566', model_pruning=True)
    print('done')

    # Load model (from https://github.com/shicai/MobileNet-Caffe)
    print('--> Loading model')
    ret = rknn.load_caffe(model='./mobilenet_deploy.prototxt',
                          blobs='./mobilenet.caffemodel')
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

    # Tips
    print('')
    print('======================================== Tips ==========================================================')
    print('When verbose is set to True, the following similar prompts will appear during the build process, ')
    print('indicating that model pruning has been effective for this model. (This means that approximately 6.9% ')
    print('of the weights have been removed, resulting in a saving of about 13.4% of the computational workload.)')
    print('Please note that not all models can be pruned, only models with sparse weights are likely to benefit from pruning.')
    print('')
    print('  I model_pruning ...')
    print('  I model_pruning results:')
    print('  I     -1.12144 MB (-6.9%)')
    print('  I     -0.00016 T (-13.4%)')
    print('  I model_pruning done.')
    print('')
    print('=========================================+++++++========================================================')
    print('')

    # Export rknn model
    print('--> Export rknn model')
    ret = rknn.export_rknn('./mobilenet.rknn')
    if ret != 0:
        print('Export rknn model failed!')
        exit(ret)
    print('done')

    # Set inputs
    img = cv2.imread('./dog_224x224.jpg')

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
    show_outputs(outputs)
    print('done')

    rknn.release()
