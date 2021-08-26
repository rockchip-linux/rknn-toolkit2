import cv2
from PIL import Image
import time
import numpy as np
import torch
import onnxruntime

from rknn.api import RKNN

NUM_CLS = 80
LISTSIZE = NUM_CLS+5
SPAN = 3
OBJ_THRESH = 0.2
NMS_THRESH = 0.5
CLASSES = ("person", "bicycle", "car","motorbike ","aeroplane ","bus ","train","truck ","boat","traffic light",
           "fire hydrant","stop sign ","parking meter","bench","bird","cat","dog ","horse ","sheep","cow","elephant",
           "bear","zebra ","giraffe","backpack","umbrella","handbag","tie","suitcase","frisbee","skis","snowboard","sports ball","kite",
           "baseball bat","baseball glove","skateboard","surfboard","tennis racket","bottle","wine glass","cup","fork","knife ",
           "spoon","bowl","banana","apple","sandwich","orange","broccoli","carrot","hot dog","pizza ","donut","cake","chair","sofa",
           "pottedplant","bed","diningtable","toilet ","tvmonitor","laptop	","mouse	","remote ","keyboard ","cell phone","microwave ",
           "oven ","toaster","sink","refrigerator ","book","clock","vase","scissors ","teddy bear ","hair drier", "toothbrush ")

masks = [[0,1,2], [3,4,5], [6,7,8]] #yolov5s
anchors = [[10,13],[16,30],[33,23],[30,61],[62,45],[59,119],[116,90],[156,198],[373,326]]

def letterbox_image(image, size):
    iw, ih = image.size
    w, h = size
    scale = min(w / iw, h / ih)
    nw = int(iw * scale)
    nh = int(ih * scale)

    image = np.array(image)
    image = cv2.resize(image, (nw, nh), interpolation=cv2.INTER_LINEAR)
    image = Image.fromarray(image)
    new_image = Image.new('RGB', size, (128, 128, 128))
    new_image.paste(image, ((w - nw) // 2, (h - nh) // 2))
    return new_image

def w_bbox_iou(box1, box2, x1y1x2y2=True):
    """
    计算IOU
    """
    if not x1y1x2y2:
        b1_x1, b1_x2 = box1[:, 0] - box1[:, 2] / 2, box1[:, 0] + box1[:, 2] / 2
        b1_y1, b1_y2 = box1[:, 1] - box1[:, 3] / 2, box1[:, 1] + box1[:, 3] / 2
        b2_x1, b2_x2 = box2[:, 0] - box2[:, 2] / 2, box2[:, 0] + box2[:, 2] / 2
        b2_y1, b2_y2 = box2[:, 1] - box2[:, 3] / 2, box2[:, 1] + box2[:, 3] / 2
    else:
        b1_x1, b1_y1, b1_x2, b1_y2 = box1[:, 0], box1[:, 1], box1[:, 2], box1[:, 3]
        b2_x1, b2_y1, b2_x2, b2_y2 = box2[:, 0], box2[:, 1], box2[:, 2], box2[:, 3]

    inter_rect_x1 = torch.max(b1_x1, b2_x1)
    inter_rect_y1 = torch.max(b1_y1, b2_y1)
    inter_rect_x2 = torch.min(b1_x2, b2_x2)
    inter_rect_y2 = torch.min(b1_y2, b2_y2)

    inter_area = torch.clamp(inter_rect_x2 - inter_rect_x1 + 1, min=0) * \
                 torch.clamp(inter_rect_y2 - inter_rect_y1 + 1, min=0)

    b1_area = (b1_x2 - b1_x1 + 1) * (b1_y2 - b1_y1 + 1)
    b2_area = (b2_x2 - b2_x1 + 1) * (b2_y2 - b2_y1 + 1)

    iou = inter_area / (b1_area + b2_area - inter_area + 1e-16)

    return iou


def w_non_max_suppression(prediction, num_classes, conf_thres=0.1, nms_thres=0.4):
    # 求左上角和右下角
    # box_corner = prediction.new(prediction.shape)
    box_corner = torch.FloatTensor(prediction.shape)
    box_corner[:, :, 0] = prediction[:, :, 0] - prediction[:, :, 2] / 2
    box_corner[:, :, 1] = prediction[:, :, 1] - prediction[:, :, 3] / 2
    box_corner[:, :, 2] = prediction[:, :, 0] + prediction[:, :, 2] / 2
    box_corner[:, :, 3] = prediction[:, :, 1] + prediction[:, :, 3] / 2
    prediction[:, :, :4] = box_corner[:, :, :4]

    output = [None for _ in range(len(prediction))]
    for image_i, image_pred in enumerate(prediction):
        # 利用置信度进行第一轮筛选
        conf_mask = (image_pred[:, 4] >= conf_thres).squeeze()
        image_pred = image_pred[conf_mask]

        if not image_pred.size(0):
            continue

        # 获得种类及其置信度
        class_conf, class_pred = torch.max(image_pred[:, 5:5 + num_classes], 1, keepdim=True)

        # 获得的内容为(x1, y1, x2, y2, obj_conf, class_conf, class_pred)
        detections = torch.cat((image_pred[:, :5], class_conf.float(), class_pred.float()), 1)

        # 获得种类
        unique_labels = detections[:, -1].cpu().unique()

        if prediction.is_cuda:
            unique_labels = unique_labels.cuda()

        for c in unique_labels:
            # 获得某一类初步筛选后全部的预测结果
            detections_class = detections[detections[:, -1] == c]
            # 按照存在物体的置信度排序
            _, conf_sort_index = torch.sort(detections_class[:, 4], descending=True)
            detections_class = detections_class[conf_sort_index]
            # 进行非极大抑制
            max_detections = []
            while detections_class.size(0):
                # 取出这一类置信度最高的，一步一步往下判断，判断重合程度是否大于nms_thres，如果是则去除掉
                max_detections.append(detections_class[0].unsqueeze(0))
                if len(detections_class) == 1:
                    break
                ious = w_bbox_iou(max_detections[-1], detections_class[1:])
                detections_class = detections_class[1:][ious < nms_thres]
            # 堆叠
            max_detections = torch.cat(max_detections).data
            # Add max detections to outputs
            output[image_i] = max_detections if output[image_i] is None else torch.cat(
                (output[image_i], max_detections))

    return output


def onnx_postprocess(outputs, img_size_w, img_size_h):
    boxs = []
    a = torch.tensor(anchors).float().view(3, -1, 2)
    anchor_grid = a.clone().view(3, 1, -1, 1, 1, 2)
    for index, out in enumerate(outputs):
        out = torch.from_numpy(out)
        batch = out.shape[1]
        feature_h = out.shape[2]
        feature_w = out.shape[3]

        # Feature map corresponds to the original image zoom factor
        stride_w = int(img_size_w / feature_w)
        stride_h = int(img_size_h / feature_h)

        grid_x, grid_y = np.meshgrid(np.arange(feature_w), np.arange(feature_h))
        grid_x, grid_y = torch.from_numpy(np.array(grid_x)).float(), torch.from_numpy(np.array(grid_y)).float()

        # cx, cy, w, h
        pred_boxes = torch.FloatTensor(out[..., :4].shape)
        pred_boxes[..., 0] = (torch.sigmoid(out[..., 0]) * 2.0 - 0.5 + grid_x) * stride_w  # cx
        pred_boxes[..., 1] = (torch.sigmoid(out[..., 1]) * 2.0 - 0.5 + grid_y) * stride_h  # cy
        pred_boxes[..., 2:4] = (torch.sigmoid(out[..., 2:4]) * 2) ** 2 * anchor_grid[index]  # wh
        pred_boxes_np = pred_boxes.numpy()

        conf = torch.sigmoid(out[..., 4])
        pred_cls = torch.sigmoid(out[..., 5:])

        output = torch.cat((pred_boxes.view(1, -1, 4),
                            conf.view(1, -1, 1),
                            pred_cls.view(1, -1, NUM_CLS)),
                           -1)
        boxs.append(output)

    outputx = torch.cat(boxs, 1)
    # NMS
    batch_detections = w_non_max_suppression(outputx, NUM_CLS, conf_thres=OBJ_THRESH, nms_thres=NMS_THRESH)

    return batch_detections


def clip_coords(boxes, img_shape):
    # Clip bounding xyxy bounding boxes to image shape (height, width)
    boxes[:, 0].clamp_(0, img_shape[1])  # x1
    boxes[:, 1].clamp_(0, img_shape[0])  # y1
    boxes[:, 2].clamp_(0, img_shape[1])  # x2
    boxes[:, 3].clamp_(0, img_shape[0])  # y2


def scale_coords(img1_shape, coords, img0_shape, ratio_pad=None):
    # Rescale coords (xyxy) from img1_shape to img0_shape
    if ratio_pad is None:  # calculate from img0_shape
        gain = min(img1_shape[0]/img0_shape[0], img1_shape[1]/img0_shape[1])  # gain  = old / new
        pad = (img1_shape[1] - img0_shape[1] * gain) / 2, (img1_shape[0] - img0_shape[0] * gain) / 2  # wh padding
    else:
        gain = ratio_pad[0][0]
        pad = ratio_pad[1]

    coords[:, [0, 2]] -= pad[0]  # x padding
    coords[:, [1, 3]] -= pad[1]  # y padding
    coords[:, :4] /= gain
    clip_coords(coords, img0_shape)
    return coords


def display(detections=None, image_src=None, input_size=(640, 640), line_thickness=None, text_bg_alpha=0.0):
    labels = detections[..., -1]
    boxs = detections[..., :4]
    confs = detections[..., 4]

    h, w, c = image_src.shape

    boxs[:, :] = scale_coords(input_size, boxs[:, :], (h, w)).round()

    tl = line_thickness or round(0.002 * (w + h) / 2) + 1
    for i, box in enumerate(boxs):
        x1, y1, x2, y2 = box

        ratio = (y2-y1)/(x2-x1)

        x1, y1, x2, y2 = int(x1.numpy()), int(y1.numpy()), int(x2.numpy()), int(y2.numpy())
        np.random.seed(int(labels[i].numpy()) + 2020)
        color = (np.random.randint(0, 255), 0, np.random.randint(0, 255))
        cv2.rectangle(image_src, (x1, y1), (x2, y2), color, max(int((w + h) / 600), 1), cv2.LINE_AA)
        label = '{0:.3f}'.format(confs[i])
        t_size = cv2.getTextSize(label, 0, fontScale=tl / 3, thickness=1)[0]
        c2 = x1 + t_size[0] + 3, y1 - t_size[1] - 5
        if text_bg_alpha == 0.0:
            cv2.rectangle(image_src, (x1 - 1, y1), c2, color, cv2.FILLED, cv2.LINE_AA)
        else:
            # 透明文本背景
            alphaReserve = text_bg_alpha  # 0：不透明 1：透明
            BChannel, GChannel, RChannel = color
            xMin, yMin = int(x1 - 1), int(y1 - t_size[1] - 3)
            xMax, yMax = int(x1 + t_size[0]), int(y1)
            image_src[yMin:yMax, xMin:xMax, 0] = image_src[yMin:yMax, xMin:xMax, 0] * alphaReserve + BChannel * (1 - alphaReserve)
            image_src[yMin:yMax, xMin:xMax, 1] = image_src[yMin:yMax, xMin:xMax, 1] * alphaReserve + GChannel * (1 - alphaReserve)
            image_src[yMin:yMax, xMin:xMax, 2] = image_src[yMin:yMax, xMin:xMax, 2] * alphaReserve + RChannel * (1 - alphaReserve)
        cv2.putText(image_src, label, (x1 + 3, y1 - 4), 0, tl / 3, [255, 255, 255],
                    thickness=1, lineType=cv2.LINE_AA)



if __name__ == '__main__':

    exp = 'yolov5s'
    Width = 640
    Height = 640
    MODEL_PATH = './yolov5s.onnx'
    im_file = './dog_bike_car_640x640.jpg'
    RKNN_MODEL_PATH = './{}.rknn'.format(exp + '-' + str(Width) + '-' + str(Height))
    DATASET = './dataset.txt'

    # Create RKNN object
    rknn = RKNN(verbose=True)


    rknn.config(mean_values=[[0, 0, 0]], std_values=[[255, 255, 255]])
    # Load model
    print('--> Loading model')
    ret = rknn.load_onnx(MODEL_PATH)
    if ret != 0:
        print('load model failed!')
        exit(ret)
    print('done')

    # Build model
    print('--> Building model')
    ret = rknn.build(do_quantization=True, dataset=DATASET)
    if ret != 0:
        print('build model failed.')
        exit(ret)
    print('done')

    # Export rknn model
    print('--> Export RKNN model')
    ret = rknn.export_rknn(RKNN_MODEL_PATH)
    if ret != 0:
        print('Export rknn model failed.')
        exit(ret)
    print('done')

    # Set inputs
    image_src = Image.open('./dog_bike_car_640x640.jpg')
    img = letterbox_image(image_src, (Width, Height))
    img = np.array(img)

    # init runtime environment
    print('--> Init runtime environment')
    ret = rknn.init_runtime()
    if ret != 0:
        print('Init runtime environment failed')
        exit(ret)
    print('done')

    # inference
    print('--> inference')
    start = time.time()
    outputs = rknn.inference(inputs=[img])
    end = time.time()
    print('inference time: ', end - start)
    print('done')

    np.save('./onnx_yolov5_0.npy', outputs[0])
    np.save('./onnx_yolov5_1.npy', outputs[1])
    np.save('./onnx_yolov5_2.npy', outputs[2])

    # inference process 
    image_src = np.array(image_src)
    detections = onnx_postprocess(outputs, Width, Height)
    if detections[0] is not None:
       display(detections[0], image_src)

    image_src = cv2.cvtColor(image_src,cv2.COLOR_BGR2RGB)

    cv2.imwrite("result.jpg", image_src)

    rknn.release()

