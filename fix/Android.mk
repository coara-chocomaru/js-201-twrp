LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := mmc64_writeprotect_native
LOCAL_SRC_FILES := mmc64_writeprotect_native.c

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := RECOVERY_EXECUTABLES
LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/sbin

LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_CFLAGS := \
    -O2 \
    -Wall \
    -Wextra \
    -Wno-unused-parameter

include $(BUILD_EXECUTABLE)
