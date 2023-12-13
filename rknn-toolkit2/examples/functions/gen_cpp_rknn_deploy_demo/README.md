## RKNN Deploy demo

### 1.Introduction

RKNN-Toolkit2 support generate cpp deploy demo.



Support - platform: RK3562/ RK3566/ RK3568/ RK3588



### 2.How to use

1.To enable this function, export rknn model with setting "**cpp_gen_cfg=True**" as followed:

```
rknn.export_rknn('./mobilenet_v2.rknn', cpp_gen_cfg=True)
```



2.After setting "**cpp_gen_cfg=True**", a folder named "**./rknn_deploy_demo**" should be created. Then follow the introduction of "**./rknn_deploy_demo/README.md**" to execute the demo.



