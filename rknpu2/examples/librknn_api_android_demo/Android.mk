LOCAL_PATH:= $(call my-dir)
#======================================================================
#
# rknn_create_mem_demo
#
#======================================================================
include $(CLEAR_VARS)
LOCAL_VENDOR_MODULE := true

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code -Wno-implicit-fallthrough

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/include \

LOCAL_SHARED_LIBRARIES := \
    liblog \
    librknn_api_android 

LOCAL_HEADER_LIBRARIES += \
    librknn_api_android_headers \

LOCAL_SRC_FILES:= \
    rknn_create_mem_demo.cpp \

LOCAL_MODULE:= rknn_create_mem_demo

include $(BUILD_EXECUTABLE)

