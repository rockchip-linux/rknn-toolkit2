# Copyright 2021 The Rockchip Electronics Co., Ltd
# @File    : save_capture_img.py
# @Author  : raul.rao
# @Date    : 06/07/22
# @Desc    : read rgb raw data and save it to jpg file.
# =====================================================

import cv2
from cv2 import cvtColor
import numpy as np

if __name__ == '__main__':
    data = np.loadtxt('resized_img_5.rgb')
    data = np.reshape(data, (640, 640, 3)).astype(np.uint8)
    img = cv2.cvtColor(data, cv2.COLOR_RGB2BGR)
    cv2.imshow("resized img", img)
    cv2.waitKeyEx(0)

    cv2.imwrite('test.jpg', img)