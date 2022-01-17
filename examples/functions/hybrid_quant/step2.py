import numpy as np
import cv2
from rknn.api import RKNN
from ssd_post_process import ssd_post_process

if __name__ == '__main__':

    # Create RKNN object
    rknn = RKNN(verbose=True)

    # Build model
    print('--> hybrid_quantization_step2')
    ret = rknn.hybrid_quantization_step2(model_input='./ssd_mobilenet_v2.model',
                                         data_input='./ssd_mobilenet_v2.data',
                                         model_quantization_cfg='./ssd_mobilenet_v2.quantization.cfg')
    if ret != 0:
        print('hybrid_quantization_step2 failed!')
        exit(ret)
    print('done')

    # Export rknn model
    print('--> Export rknn model')
    ret = rknn.export_rknn('./ssd_mobilenet_v2.rknn')
    if ret != 0:
        print('Export rknn model failed!')
        exit(ret)
    print('done')

    # Accuracy analysis
    print('--> Accuracy analysis')
    ret = rknn.accuracy_analysis(inputs=['./dog_bike_car_300x300.jpg'], output_dir=None)
    if ret != 0:
        print('Accuracy analysis failed!')
        exit(ret)
    print('done')

    # Set inputs
    img = cv2.imread('./dog_bike_car_300x300.jpg')
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

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
    np.save('./functions_hybrid_quant_0.npy', outputs[0])
    np.save('./functions_hybrid_quant_1.npy', outputs[1])
    ssd_post_process(np.reshape(outputs[0], (-1)), np.reshape(outputs[1], (-1)), './dog_bike_car_300x300.jpg', './')
    print('done')

    rknn.release()
