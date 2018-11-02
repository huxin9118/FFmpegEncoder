LOCAL_PATH := $(call my-dir)

###########################
#
# FFmpeg library
#
###########################
include $(CLEAR_VARS)
LOCAL_MODULE := ffmpeg
LOCAL_SRC_FILES := libffmpeg.so
include $(PREBUILT_SHARED_LIBRARY)

###########################
#
# mediacodec ndk shared library
#
###########################

include $(CLEAR_VARS)
LOCAL_MODULE:= libnative_codec16
LOCAL_SRC_FILES:= libnative_codec16.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libnative_codec17
LOCAL_SRC_FILES:= libnative_codec17.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libnative_codec18
LOCAL_SRC_FILES:= libnative_codec18.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libnative_codec19
LOCAL_SRC_FILES:= libnative_codec19.so
include $(PREBUILT_SHARED_LIBRARY)

###########################
#
# h264_parser library
#
###########################
include $(CLEAR_VARS)
LOCAL_MODULE := h264_parser
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES := h264_parser.c \
					h264_parser_wrapper.c
LOCAL_LDLIBS := -llog -lz
include $(BUILD_SHARED_LIBRARY)

# Program
include $(CLEAR_VARS)
LOCAL_MODULE := ffmpegencoder
LOCAL_SRC_FILES := simplest_ffmpeg_encoder.c \
                     NativeCodec.cpp \
                     mediacodec_utils.c \
                     mediacodec_encoder.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_LDLIBS := -llog -lz
LOCAL_SHARED_LIBRARIES := ffmpeg
include $(BUILD_SHARED_LIBRARY)

