#include <stdio.h>
#include <stdlib.h>
#include "mediacodec/NativeCodec.h"

#include <jni.h>
#include <android/log.h>
#include <sys/system_properties.h>
#define MediaCodec_LOGI(...) __android_log_print(ANDROID_LOG_INFO , "MediaCodecDecoder", __VA_ARGS__)
#define MediaCodec_LOGE(...) __android_log_print(ANDROID_LOG_ERROR , "MediaCodecDecoder", __VA_ARGS__)

typedef enum YUV_PIXEL_FORMAT{
	I420,
	NV12,
	NV21
}YUV_PIXEL_FORMAT;

//-------------------encode-------------------------
typedef struct MediaCodecEncoder{
	AMediaCodec* codec;//MediaCodec接口
	
	int32_t width;
	int32_t height;
	int32_t bit_rate;
	int32_t frame_rate;
	int64_t frameIndex;
	uint8_t* sps;
	uint8_t* pps;
	char* startCode;
	int32_t sps_length;
	int32_t pps_length;
	int32_t startCode_length;
	int32_t firstI;

	const char* mime;//解码器输出格式类型
	int32_t colorFormat;//解码器输出媒体颜色格式

	int SDK_INT;//SDK版本
	char* phone_type;//手机型号
	char* hardware;//cpu型号
	int DEBUG;//调试日志开关
	int64_t TIME_OUT;//解码缓存获得超时时间
	int64_t MAX_TIME_OUT;
	char* MIMETYPE_VIDEO_AVC;
	int profile;
	int level;
	YUV_PIXEL_FORMAT yuv_pixel_format;
}MediaCodecEncoder;

MediaCodecEncoder* mediacodec_encoder_alloc(int isDebug, int width, int height, int frame_rate, int bit_rate, int timeout, YUV_PIXEL_FORMAT yuv_pixel_format);
int mediacodec_encoder_free(MediaCodecEncoder* encoder);

int mediacodec_encoder_open(MediaCodecEncoder* encoder);
int mediacodec_encoder_close(MediaCodecEncoder* encoder);
	
int mediacodec_encoder_encode(MediaCodecEncoder* encoder, uint8_t* in, int offset, uint8_t* out, int length, int* error_code);

int mediacodec_encoder_getConfig_int(MediaCodecEncoder* encoder, char* key);
int mediacodec_encoder_setConfig_int(MediaCodecEncoder* encoder, char* key, int value);
uint64_t mediacodec_encoder_computePresentationTime(MediaCodecEncoder* encoder);
int mediacodec_encoder_ffAvcFindStartcodeInternal(uint8_t* data, int offset, int end);
int mediacodec_encoder_ffAvcFindStartcode(uint8_t* data, int offset, int end);

//-------------------encode-------------------------

//-------------------decode-------------------------
typedef struct MediaCodecDecoder{
	AMediaCodec* codec;//MediaCodec接口
	
	int32_t width;
	int32_t height;
	int32_t stride;//YUV宽度步长
	int32_t sliceHeight;//YUV高度步长
	int32_t crop_left;
	int32_t crop_right;
	int32_t crop_top;
	int32_t crop_bottom;
	const char* mime;//解码器输出格式类型
	int32_t colorFormat;//解码器输出媒体颜色格式

	int SDK_INT;//SDK版本
	char* phone_type;//手机型号
	char* hardware;//cpu型号
	int DEBUG;//调试日志开关
	int64_t TIME_OUT;//解码缓存获得超时时间
	int64_t MAX_TIME_OUT;
	char* MIMETYPE_VIDEO_AVC;
	YUV_PIXEL_FORMAT yuv_pixel_format;
}MediaCodecDecoder;

MediaCodecDecoder* mediacodec_decoder_alloc1(int isDebug, int timeout, YUV_PIXEL_FORMAT yuv_pixel_format);
MediaCodecDecoder* mediacodec_decoder_alloc2(int isDebug);
MediaCodecDecoder* mediacodec_decoder_alloc3();
int mediacodec_decoder_free(MediaCodecDecoder* decoder);

int mediacodec_decoder_open(MediaCodecDecoder* decoder);
int mediacodec_decoder_close(MediaCodecDecoder* decoder);
	
int mediacodec_decoder_decode(MediaCodecDecoder* decoder, uint8_t* in, int offset, uint8_t* out, int length, int* error_code);
int mediacodec_decoder_getConfig_int(MediaCodecDecoder* decoder, char* key);
int mediacodec_decoder_setConfig_int(MediaCodecDecoder* decoder, char* key, int value);
//-------------------decode-------------------------

//-------------------utils-------------------------
void NV12toYUV420Planar(uint8_t* input, int offset, uint8_t* output, int width, int height);
void NV21toYUV420Planar(uint8_t* input, int offset, uint8_t* output, int width, int height);
void I420toYUV420SemiPlanar(uint8_t* input, int offset, uint8_t* output, int width, int height);
void I420toNV21(uint8_t* input, int offset, uint8_t* output, int width, int height);
void swapNV12toNV21(uint8_t* input, int offset, uint8_t* output, int width, int height);
void CropYUV420SemiPlanar(uint8_t* input, int width, int height, uint8_t* output,
					int crop_left, int crop_right, int crop_top, int crop_bottom);
void CropYUV420Planar(uint8_t* input, int width, int height, uint8_t* output,
					int crop_left, int crop_right, int crop_top, int crop_bottom);
//-------------------utils-------------------------