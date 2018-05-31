/*
 * NativeCodec.h
 *
 *  Created on: 2016年12月11日
 *      Author: zsp-asus
 */

#ifndef NATIVECODEC_H_
#define NATIVECODEC_H_

#define AMEDIACODEC_FLAG_CODEC_CONFIG 2

#ifdef __cplusplus
extern "C"{
#endif

#include <sys/types.h>
#include <android/native_window.h>

/**
* Enumeration defining possible uncompressed image/video formats.
*
* ENUMS:
*  Unused                 : Placeholder value when format is N/A
*  Monochrome             : black and white
*  8bitRGB332             : Red 7:5, Green 4:2, Blue 1:0
*  12bitRGB444            : Red 11:8, Green 7:4, Blue 3:0
*  16bitARGB4444          : Alpha 15:12, Red 11:8, Green 7:4, Blue 3:0
*  16bitARGB1555          : Alpha 15, Red 14:10, Green 9:5, Blue 4:0
*  16bitRGB565            : Red 15:11, Green 10:5, Blue 4:0
*  16bitBGR565            : Blue 15:11, Green 10:5, Red 4:0
*  18bitRGB666            : Red 17:12, Green 11:6, Blue 5:0
*  18bitARGB1665          : Alpha 17, Red 16:11, Green 10:5, Blue 4:0
*  19bitARGB1666          : Alpha 18, Red 17:12, Green 11:6, Blue 5:0
*  24bitRGB888            : Red 24:16, Green 15:8, Blue 7:0
*  24bitBGR888            : Blue 24:16, Green 15:8, Red 7:0
*  24bitARGB1887          : Alpha 23, Red 22:15, Green 14:7, Blue 6:0
*  25bitARGB1888          : Alpha 24, Red 23:16, Green 15:8, Blue 7:0
*  32bitBGRA8888          : Blue 31:24, Green 23:16, Red 15:8, Alpha 7:0
*  32bitARGB8888          : Alpha 31:24, Red 23:16, Green 15:8, Blue 7:0
*  YUV411Planar           : U,Y are subsampled by a factor of 4 horizontally
*  YUV411PackedPlanar     : packed per payload in planar slices
*  YUV420Planar           : Three arrays Y,U,V.
*  YUV420PackedPlanar     : packed per payload in planar slices
*  YUV420SemiPlanar       : Two arrays, one is all Y, the other is U and V
*  YUV422Planar           : Three arrays Y,U,V.
*  YUV422PackedPlanar     : packed per payload in planar slices
*  YUV422SemiPlanar       : Two arrays, one is all Y, the other is U and V
*  YCbYCr                 : Organized as 16bit YUYV (i.e. YCbYCr)
*  YCrYCb                 : Organized as 16bit YVYU (i.e. YCrYCb)
*  CbYCrY                 : Organized as 16bit UYVY (i.e. CbYCrY)
*  CrYCbY                 : Organized as 16bit VYUY (i.e. CrYCbY)
*  YUV444Interleaved      : Each pixel contains equal parts YUV
*  RawBayer8bit           : SMIA camera output format
*  RawBayer10bit          : SMIA camera output format
*  RawBayer8bitcompressed : SMIA camera output format
*/
typedef enum OMX_COLOR_FORMATTYPE {
	OMX_COLOR_FormatUnused,
	OMX_COLOR_FormatMonochrome,
	OMX_COLOR_Format8bitRGB332,
	OMX_COLOR_Format12bitRGB444,
	OMX_COLOR_Format16bitARGB4444,
	OMX_COLOR_Format16bitARGB1555,
	OMX_COLOR_Format16bitRGB565,
	OMX_COLOR_Format16bitBGR565,
	OMX_COLOR_Format18bitRGB666,
	OMX_COLOR_Format18bitARGB1665,
	OMX_COLOR_Format19bitARGB1666,
	OMX_COLOR_Format24bitRGB888,
	OMX_COLOR_Format24bitBGR888,
	OMX_COLOR_Format24bitARGB1887,
	OMX_COLOR_Format25bitARGB1888,
	OMX_COLOR_Format32bitBGRA8888,
	OMX_COLOR_Format32bitARGB8888,
	OMX_COLOR_FormatYUV411Planar,
	OMX_COLOR_FormatYUV411PackedPlanar,
	OMX_COLOR_FormatYUV420Planar,
	OMX_COLOR_FormatYUV420PackedPlanar,
	OMX_COLOR_FormatYUV420SemiPlanar,
	OMX_COLOR_FormatYUV422Planar,
	OMX_COLOR_FormatYUV422PackedPlanar,
	OMX_COLOR_FormatYUV422SemiPlanar,
	OMX_COLOR_FormatYCbYCr,
	OMX_COLOR_FormatYCrYCb,
	OMX_COLOR_FormatCbYCrY,
	OMX_COLOR_FormatCrYCbY,
	OMX_COLOR_FormatYUV444Interleaved,
	OMX_COLOR_FormatRawBayer8bit,
	OMX_COLOR_FormatRawBayer10bit,
	OMX_COLOR_FormatRawBayer8bitcompressed,
	OMX_COLOR_FormatL2,
	OMX_COLOR_FormatL4,
	OMX_COLOR_FormatL8,
	OMX_COLOR_FormatL16,
	OMX_COLOR_FormatL24,
	OMX_COLOR_FormatL32,
	OMX_COLOR_FormatYUV420PackedSemiPlanar,
	OMX_COLOR_FormatYUV422PackedSemiPlanar,
	OMX_COLOR_Format18BitBGR666,
	OMX_COLOR_Format24BitARGB6666,
	OMX_COLOR_Format24BitABGR6666,
	OMX_COLOR_FormatKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
	OMX_COLOR_FormatVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
	OMX_COLOR_TI_FormatYUV420PackedSemiPlanar = 0x7f000100,
	OMX_COLOR_QCOM_FormatYUV420SemiPlanar = 0x7fa30c00,
	OMX_COLOR_FormatMax = 0x7FFFFFFF
} OMX_COLOR_FORMATTYPE;

struct AMediaCodec;
typedef struct AMediaCodec AMediaCodec;

struct AMediaCodecBufferInfo {
	int32_t offset;
	int32_t size;
	int64_t presentationTimeUs;
	uint32_t flags;
};
typedef struct AMediaCodecBufferInfo AMediaCodecBufferInfo;
typedef struct AMediaCodecCryptoInfo AMediaCodecCryptoInfo;

struct AMediaFormat;
typedef struct AMediaFormat AMediaFormat;

enum {
	AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM = 4,
	AMEDIACODEC_CONFIGURE_FLAG_ENCODE = 1,
	AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED = -3,
	AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED = -2,
	AMEDIACODEC_INFO_TRY_AGAIN_LATER = -1
};

struct AMediaCrypto;
typedef struct AMediaCrypto AMediaCrypto;

typedef enum {
	AMEDIA_OK = 0,

	AMEDIA_ERROR_BASE = -10000,
	AMEDIA_ERROR_UNKNOWN = AMEDIA_ERROR_BASE,
	AMEDIA_ERROR_MALFORMED = AMEDIA_ERROR_BASE - 1,
	AMEDIA_ERROR_UNSUPPORTED = AMEDIA_ERROR_BASE - 2,
	AMEDIA_ERROR_INVALID_OBJECT = AMEDIA_ERROR_BASE - 3,
	AMEDIA_ERROR_INVALID_PARAMETER = AMEDIA_ERROR_BASE - 4,

	AMEDIA_DRM_ERROR_BASE = -20000,
	AMEDIA_DRM_NOT_PROVISIONED = AMEDIA_DRM_ERROR_BASE - 1,
	AMEDIA_DRM_RESOURCE_BUSY = AMEDIA_DRM_ERROR_BASE - 2,
	AMEDIA_DRM_DEVICE_REVOKED = AMEDIA_DRM_ERROR_BASE - 3,
	AMEDIA_DRM_SHORT_BUFFER = AMEDIA_DRM_ERROR_BASE - 4,
	AMEDIA_DRM_SESSION_NOT_OPENED = AMEDIA_DRM_ERROR_BASE - 5,
	AMEDIA_DRM_TAMPER_DETECTED = AMEDIA_DRM_ERROR_BASE - 6,
	AMEDIA_DRM_VERIFY_FAILED = AMEDIA_DRM_ERROR_BASE - 7,
	AMEDIA_DRM_NEED_KEY = AMEDIA_DRM_ERROR_BASE - 8,
	AMEDIA_DRM_LICENSE_EXPIRED = AMEDIA_DRM_ERROR_BASE - 9
} media_status_t;

extern const char* AMEDIAFORMAT_KEY_AAC_PROFILE;
extern const char* AMEDIAFORMAT_KEY_BIT_RATE;
extern const char* AMEDIAFORMAT_KEY_CHANNEL_COUNT;
extern const char* AMEDIAFORMAT_KEY_CHANNEL_MASK;
extern const char* AMEDIAFORMAT_KEY_COLOR_FORMAT;
extern const char* AMEDIAFORMAT_KEY_DURATION;
extern const char* AMEDIAFORMAT_KEY_FLAC_COMPRESSION_LEVEL;
extern const char* AMEDIAFORMAT_KEY_FRAME_RATE;
extern const char* AMEDIAFORMAT_KEY_HEIGHT;
extern const char* AMEDIAFORMAT_KEY_IS_ADTS;
extern const char* AMEDIAFORMAT_KEY_IS_AUTOSELECT;
extern const char* AMEDIAFORMAT_KEY_IS_DEFAULT;
extern const char* AMEDIAFORMAT_KEY_IS_FORCED_SUBTITLE;
extern const char* AMEDIAFORMAT_KEY_I_FRAME_INTERVAL;
extern const char* AMEDIAFORMAT_KEY_LANGUAGE;
extern const char* AMEDIAFORMAT_KEY_MAX_HEIGHT;
extern const char* AMEDIAFORMAT_KEY_MAX_INPUT_SIZE;
extern const char* AMEDIAFORMAT_KEY_MAX_WIDTH;
extern const char* AMEDIAFORMAT_KEY_MIME;
extern const char* AMEDIAFORMAT_KEY_PUSH_BLANK_BUFFERS_ON_STOP;
extern const char* AMEDIAFORMAT_KEY_REPEAT_PREVIOUS_FRAME_AFTER;
extern const char* AMEDIAFORMAT_KEY_SAMPLE_RATE;
extern const char* AMEDIAFORMAT_KEY_WIDTH;
extern const char* AMEDIAFORMAT_KEY_STRIDE;

AMediaCodec* AMediaCodec_createCodecByName(const char *name);

AMediaCodec* AMediaCodec_createDecoderByType(const char *mime_type);

AMediaCodec* AMediaCodec_createEncoderByType(const char *mime_type);

media_status_t AMediaCodec_delete(AMediaCodec* codec);

media_status_t AMediaCodec_configure(
	AMediaCodec* codec,
	const AMediaFormat* format,
	ANativeWindow* surface,
	AMediaCrypto *crypto,
	uint32_t flags);

////////AmediaCodec///////////////////
media_status_t AMediaCodec_start(AMediaCodec* codec);

media_status_t AMediaCodec_stop(AMediaCodec* codec);

media_status_t AMediaCodec_flush(AMediaCodec* codec);

uint8_t* AMediaCodec_getInputBuffer(AMediaCodec* codec, size_t idx, size_t *out_size);
uint8_t* AMediaCodec_getOutputBuffer(AMediaCodec* codec, size_t idx, size_t *out_size);
ssize_t AMediaCodec_dequeueInputBuffer(AMediaCodec* codec, int64_t timeoutUs);
media_status_t AMediaCodec_queueInputBuffer(AMediaCodec* codec,
	size_t idx, off_t offset, size_t size, uint64_t time, uint32_t flags);

ssize_t AMediaCodec_dequeueOutputBuffer(AMediaCodec* codec, AMediaCodecBufferInfo *info, int64_t timeoutUs);
AMediaFormat* AMediaCodec_getOutputFormat(AMediaCodec* codec);
media_status_t AMediaCodec_releaseOutputBuffer(AMediaCodec* codec, size_t idx, int render);

/////////////////AmediaFormat///////////////////////
AMediaFormat *AMediaFormat_new();
media_status_t AMediaFormat_delete(AMediaFormat* format);
const char* AMediaFormat_toString(AMediaFormat* format);

int AMediaFormat_getInt32(AMediaFormat* format, const char *name, int32_t *out);
int AMediaFormat_getString(AMediaFormat* format, const char *name, const char **out);
int AMediaFormat_getBuffer(AMediaFormat* format, const char *name, void** data, size_t *size);

void AMediaFormat_setInt32(AMediaFormat* format, const char* name, int32_t value);

void AMediaFormat_setString(AMediaFormat* format, const char* name, const char* value);
void AMediaFormat_setBuffer(AMediaFormat* format, const char* name, void* data, size_t size);


#ifdef __cplusplus
}
#endif

#endif /* NATIVECODEC_H_ */
