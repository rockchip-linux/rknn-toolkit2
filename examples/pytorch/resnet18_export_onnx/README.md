# How to export onnx model from pytorch

## Script Usage
*Usage:*
```
python test.py
```
The script will build a ResNet18 model through pytorch, and export the onnx model of ResNet18 through the torch.onnx.export interface. it is recommended to set the opset_version to 12.