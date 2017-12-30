LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	tty.c \
	test.c

LOCAL_MODULE := tty_forward
LOCAL_MODULE_TAGS := optional
LOCAL_STATIC_LIBRARIES := libcutils
include $(BUILD_EXECUTABLE)
