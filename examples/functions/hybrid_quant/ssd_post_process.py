import numpy as np
import cv2
from rknn.api import RKNN
import math
import PIL.Image as Image
import PIL.ImageDraw as ImageDraw
import PIL.ImageFont as ImageFont
import re

np.set_printoptions(threshold=np.inf)

CLASSES = ('__background__', 'person', 'bicycle', 'car', 'motorcycle', 'airplane', 'bus', 'train', 'truck', 'boat',
           'traffic light', 'fire hydrant', '???', 'stop sign', 'parking meter', 'bench', 'bird', 'cat', 'dog', 'horse',
           'sheep', 'cow', 'elephant', 'bear', 'zebra', 'giraffe', '???', 'backpack', 'umbrella', '???', '???',
           'handbag', 'tie', 'suitcase', 'frisbee', 'skis', 'snowboard', 'sports ball', 'kite', 'baseball bat',
           'baseball glove', 'skateboard', 'surfboard', 'tennis racket', 'bottle', '???', 'wine glass', 'cup', 'fork',
           'knife', 'spoon', 'bowl', 'banana', 'apple', 'sandwich', 'orange', 'broccoli', 'carrot', 'hot dog', 'pizza',
           'donut', 'cake', 'chair', 'couch', 'potted plant', 'bed', '???', 'dining table', '???', '???', 'toilet',
           '???', 'tv', 'laptop', 'mouse', 'remote', 'keyboard', 'cell phone', 'microwave', 'oven', 'toaster', 'sink',
           'refrigerator', '???', 'book', 'clock', 'vase', 'scissors', 'teddy bear', 'hair drier', 'toothbrush')

NUM_CLS = 91

CONF_THRESH = 0.5
NMS_THRESH = 0.45
TOP_BOXES = 100
max_boxes_to_draw = 100

Y_SCALE = 10.0
X_SCALE = 10.0
H_SCALE = 5.0
W_SCALE = 5.0

prior_file = './box_priors.txt'

box_priors_ = []
fp = open(prior_file, 'r')
ls = fp.readlines()
for s in ls:
    aList = re.findall('([-+]?\d+(\.\d*)?|\.\d+)([eE][-+]?\d+)?', s)
    for ss in aList:
        aNum = float((ss[0] + ss[2]))
        box_priors_.append(aNum)
fp.close()


def softmax(x):
    return np.exp(x) / np.sum(np.exp(x), axis=0)


def IntersectBBox(box1, box2):
    if box1[0] > box2[2] or box1[2] < box2[0] or box1[1] > box2[3] or box1[3] < box2[1]:
        return 0
    else:
        area1 = (box1[2] - box1[0]) * (box1[3] - box1[1])
        area2 = (box2[2] - box2[0]) * (box2[3] - box2[1])

        xx1 = max(box1[0], box2[0])
        yy1 = max(box1[1], box2[1])
        xx2 = min(box1[2], box2[2])
        yy2 = min(box1[3], box2[3])

        w = max(0, xx2 - xx1)
        h = max(0, yy2 - yy1)

        ovr = w * h / (area1 + area2 - w * h + 0.000001)
        return ovr


def ssd_post_process(conf_data, loc_data, imgpath, output_dir='.'):
    prior_num = int(len(loc_data) / 4)  # num prior boxes

    prior_bboxes = np.array(box_priors_)
    prior_bboxes = prior_bboxes.reshape(4, prior_num)

    conf_data = conf_data.reshape(-1, NUM_CLS)

    for i in range(prior_num):
        conf_data[i] = softmax(conf_data[i])

    idx_class_conf = []
    bboxes = []

    # conf
    for prior_idx in range(0, prior_num):
        conf_data[prior_idx][0] = 0
        max_val = np.max(conf_data[prior_idx])
        max_idx = np.argmax(conf_data[prior_idx])
        if max_val > CONF_THRESH:
            idx_class_conf.append([prior_idx, max_idx, max_val])

    idx_class_conf_sorted = sorted(idx_class_conf, key=lambda x: x[2], reverse=True)

    idx_class_conf = idx_class_conf_sorted[:min(TOP_BOXES, len(idx_class_conf_sorted))]

    # boxes
    for i in range(0, prior_num):
        bbox_center_x = loc_data[4 * i + 1] / X_SCALE * prior_bboxes[3][i] + prior_bboxes[1][i]
        bbox_center_y = loc_data[4 * i + 0] / Y_SCALE * prior_bboxes[2][i] + prior_bboxes[0][i]
        bbox_w = math.exp(loc_data[4 * i + 3] / W_SCALE) * prior_bboxes[3][i]
        bbox_h = math.exp(loc_data[4 * i + 2] / H_SCALE) * prior_bboxes[2][i]

        tmp = []
        tmp.append(max(min(bbox_center_x - bbox_w / 2., 1), 0))
        tmp.append(max(min(bbox_center_y - bbox_h / 2., 1), 0))
        tmp.append(max(min(bbox_center_x + bbox_w / 2., 1), 0))
        tmp.append(max(min(bbox_center_y + bbox_h / 2., 1), 0))
        bboxes.append(tmp)

    # nms
    cur_class_num = 0
    idx_class_conf_ = []
    for i in range(0, len(idx_class_conf)):
        keep = True
        k = 0
        while k < cur_class_num:
            if keep:
                ovr = IntersectBBox(bboxes[idx_class_conf[i][0]], bboxes[idx_class_conf_[k][0]])
                if idx_class_conf_[k][1] == idx_class_conf[i][1] and ovr > NMS_THRESH:
                    keep = False
                    break
                k += 1
            else:
                break
        if keep:
            idx_class_conf_.append(idx_class_conf[i])
            cur_class_num += 1
    idx_class_conf_ = idx_class_conf_[:min(len(idx_class_conf_), max_boxes_to_draw)]

    box_class_score = []

    for i in range(0, len(idx_class_conf_)):
        bboxes[idx_class_conf_[i][0]].append(idx_class_conf_[i][1])
        bboxes[idx_class_conf_[i][0]].append(idx_class_conf_[i][2])
        box_class_score.append(bboxes[idx_class_conf_[i][0]])

    img = cv2.imread(imgpath)
    img_pil = Image.fromarray(img)
    draw = ImageDraw.Draw(img_pil)

    font = ImageFont.load_default()

    name = imgpath.split("/")[-1][:-4]

    for i in range(0, len(box_class_score)):
        x1 = box_class_score[i][0] * img.shape[1]
        y1 = box_class_score[i][1] * img.shape[0]
        x2 = box_class_score[i][2] * img.shape[1]
        y2 = box_class_score[i][3] * img.shape[0]

        # draw rect
        color = (0, int(box_class_score[i][4] / 20.0 * 255), 255)
        draw.line([(x1, y1), (x1, y2), (x2, y2),
                   (x2, y1), (x1, y1)], width=2, fill=color)

        display_str = CLASSES[box_class_score[i][4]] + ":" + str('%.2f' % box_class_score[i][5])
        display_str_height = np.ceil((1 + 2 * 0.05) * font.getsize(display_str)[1]) + 1

        if y1 > display_str_height:
            text_bottom = y1
        else:
            text_bottom = y1 + display_str_height

        text_width, text_height = font.getsize(display_str)
        margin = np.ceil(0.05 * text_height)
        draw.rectangle([(x1, text_bottom - text_height - 2 * margin), (x1 + text_width, text_bottom)], fill=color)
        draw.text((x1 + margin, text_bottom - text_height - margin), display_str, fill='black', font=font)

    print('write output image: {}{}_quant.jpg'.format(output_dir, name))
    np.copyto(img, np.array(img_pil))
    cv2.imwrite("{}{}_quant.jpg".format(output_dir, name), img)
    print('write output image finished.')
