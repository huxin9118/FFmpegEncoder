/*
 * NativeCodec.cpp
 *
 *  Created on: 2016年12月11日
 *      Author: zsp-asus
 */

#include "mediacodec/NativeCodec.h"
#include <dlfcn.h>
#include <pthread.h>
#include <sys/system_properties.h>
#include <stdlib.h>
#include <stdio.h>

#include <jni.h>
#include <android/log.h>
#define NativeCodec_LOGI(...) __android_log_print(ANDROID_LOG_INFO , "NativeCodec", __VA_ARGS__)
#define NativeCodec_LOGE(...) __android_log_print(ANDROID_LOG_ERROR , "NativeCodec", __VA_ARGS__)

#ifdef __cplusplus
extern "C"{
#endif

typedef AMediaCodec* (*pf_AMediaCodec_createDecoderByType)(const char *mime_type);

typedef AMediaCodec* (*pf_AMediaCodec_createEncoderByType)(const char *mime_type);

typedef AMediaCodec* (*pf_AMediaCodec_createCodecByName)(const char *name);

typedef media_status_t(*pf_AMediaCodec_configure)(AMediaCodec*,
	const AMediaFormat* format,
	ANativeWindow* surface,
	AMediaCrypto *crypto,
	uint32_t flags);

typedef media_status_t(*pf_AMediaCodec_start)(AMediaCodec*);

typedef media_status_t(*pf_AMediaCodec_stop)(AMediaCodec*);

typedef media_status_t(*pf_AMediaCodec_flush)(AMediaCodec*);

typedef media_status_t(*pf_AMediaCodec_delete)(AMediaCodec*);

typedef AMediaFormat* (*pf_AMediaCodec_getOutputFormat)(AMediaCodec*);

typedef ssize_t(*pf_AMediaCodec_dequeueInputBuffer)(AMediaCodec*,
	int64_t timeoutUs);

typedef uint8_t* (*pf_AMediaCodec_getInputBuffer)(AMediaCodec*,
	size_t idx, size_t *out_size);

typedef media_status_t(*pf_AMediaCodec_queueInputBuffer)(AMediaCodec*,
	size_t idx, off_t offset, size_t size, uint64_t time, uint32_t flags);

typedef ssize_t(*pf_AMediaCodec_dequeueOutputBuffer)(AMediaCodec*,
	AMediaCodecBufferInfo *info, int64_t timeoutUs);

typedef uint8_t* (*pf_AMediaCodec_getOutputBuffer)(AMediaCodec*,
	size_t idx, size_t *out_size);

typedef media_status_t(*pf_AMediaCodec_releaseOutputBuffer)(AMediaCodec*,
	size_t idx, int render);

typedef AMediaFormat *(*pf_AMediaFormat_new)();
typedef media_status_t(*pf_AMediaFormat_delete)(AMediaFormat*);

typedef char* (*pf_AMediaFormat_toString)(AMediaFormat*);

typedef int(*pf_AMediaFormat_getBuffer)(AMediaFormat*, const char *name, void** data, size_t *size);

typedef void(*pf_AMediaFormat_setString)(AMediaFormat*,
	const char* name, const char* value);

typedef void(*pf_AMediaFormat_setBuffer)(AMediaFormat*, const char* name, void* data, size_t size);

typedef void(*pf_AMediaFormat_setInt32)(AMediaFormat*,
	const char* name, int32_t value);

typedef int(*pf_AMediaFormat_getInt32)(AMediaFormat*,
	const char *name, int32_t *out);
	
typedef int(*pf_AMediaFormat_getString)(AMediaFormat*, 
	const char *name, const char **out);

const char* AMEDIAFORMAT_KEY_AAC_PROFILE = "aac-profile";
const char* AMEDIAFORMAT_KEY_BIT_RATE = "bitrate";
const char* AMEDIAFORMAT_KEY_CHANNEL_COUNT = "channel-count";
const char* AMEDIAFORMAT_KEY_CHANNEL_MASK = "channel-mask";
const char* AMEDIAFORMAT_KEY_COLOR_FORMAT = "color-format";
const char* AMEDIAFORMAT_KEY_DURATION = "durationUs";
const char* AMEDIAFORMAT_KEY_FLAC_COMPRESSION_LEVEL = "flac-compression-level";
const char* AMEDIAFORMAT_KEY_FRAME_RATE = "frame-rate";
const char* AMEDIAFORMAT_KEY_HEIGHT = "height";
const char* AMEDIAFORMAT_KEY_IS_ADTS = "is-adts";
const char* AMEDIAFORMAT_KEY_IS_AUTOSELECT = "is-autoselect";
const char* AMEDIAFORMAT_KEY_IS_DEFAULT = "is-default";
const char* AMEDIAFORMAT_KEY_IS_FORCED_SUBTITLE = "is-forced-subtitle";
const char* AMEDIAFORMAT_KEY_I_FRAME_INTERVAL = "i-frame-interval";
const char* AMEDIAFORMAT_KEY_LANGUAGE = "language";
const char* AMEDIAFORMAT_KEY_MAX_HEIGHT = "max-height";
const char* AMEDIAFORMAT_KEY_MAX_INPUT_SIZE = "max-input-size";
const char* AMEDIAFORMAT_KEY_MAX_WIDTH = "max-width";
const char* AMEDIAFORMAT_KEY_MIME = "mime";
const char* AMEDIAFORMAT_KEY_PUSH_BLANK_BUFFERS_ON_STOP = "push-blank-buffers-on-shutdown";
const char* AMEDIAFORMAT_KEY_REPEAT_PREVIOUS_FRAME_AFTER = "repeat-previous-frame-after";
const char* AMEDIAFORMAT_KEY_SAMPLE_RATE = "sample-rate";
const char* AMEDIAFORMAT_KEY_WIDTH = "width";
const char* AMEDIAFORMAT_KEY_STRIDE = "stride";

struct dlsys
{
	struct {
		pf_AMediaCodec_createDecoderByType _createDecoderByType;
		pf_AMediaCodec_createEncoderByType _createEncoderByType;
		pf_AMediaCodec_createCodecByName _createCodecByName;
		pf_AMediaCodec_configure _configure;
		pf_AMediaCodec_start _start;
		pf_AMediaCodec_stop _stop;
		pf_AMediaCodec_flush _flush;
		pf_AMediaCodec_delete _delete;
		pf_AMediaCodec_getOutputFormat _getOutputFormat;
		pf_AMediaCodec_dequeueInputBuffer _dequeueInputBuffer;
		pf_AMediaCodec_getInputBuffer _getInputBuffer;
		pf_AMediaCodec_queueInputBuffer _queueInputBuffer;
		pf_AMediaCodec_dequeueOutputBuffer _dequeueOutputBuffer;
		pf_AMediaCodec_getOutputBuffer _getOutputBuffer;
		pf_AMediaCodec_releaseOutputBuffer _releaseOutputBuffer;
	} AMediaCodec;
	struct {
		pf_AMediaFormat_new _new;
		pf_AMediaFormat_delete _delete;
		pf_AMediaFormat_toString _toString;
		pf_AMediaFormat_getBuffer _getBuffer;
		pf_AMediaFormat_setString _setString;
		pf_AMediaFormat_setBuffer _setBuffer;
		pf_AMediaFormat_setInt32 _setInt32;
		pf_AMediaFormat_getInt32 _getInt32;
		pf_AMediaFormat_getString _getString;
	} AMediaFormat;
};
static struct dlsys gdlsys;

struct members
{
	const char *name;
	int offset;
	bool critical;
};
static struct members members[] =
{
#define OFF(x) offsetof(struct dlsys, AMediaCodec.x)
{ "AMediaCodec_createDecoderByType", OFF(_createDecoderByType), true },
{ "AMediaCodec_createEncoderByType", OFF(_createEncoderByType), true },
{ "AMediaCodec_createCodecByName", OFF(_createCodecByName), true },
{ "AMediaCodec_configure", OFF(_configure), true },
{ "AMediaCodec_start", OFF(_start), true },
{ "AMediaCodec_stop", OFF(_stop), true },
{ "AMediaCodec_flush", OFF(_flush), true },
{ "AMediaCodec_delete", OFF(_delete), true },
{ "AMediaCodec_getOutputFormat", OFF(_getOutputFormat), true },
{ "AMediaCodec_dequeueInputBuffer", OFF(_dequeueInputBuffer), true },
{ "AMediaCodec_getInputBuffer", OFF(_getInputBuffer), true },
{ "AMediaCodec_queueInputBuffer", OFF(_queueInputBuffer), true },
{ "AMediaCodec_dequeueOutputBuffer", OFF(_dequeueOutputBuffer), true },
{ "AMediaCodec_getOutputBuffer", OFF(_getOutputBuffer), true },
{ "AMediaCodec_releaseOutputBuffer", OFF(_releaseOutputBuffer), true },
#undef OFF
#define OFF(x) offsetof(struct dlsys, AMediaFormat.x)
{ "AMediaFormat_new", OFF(_new), true },
{ "AMediaFormat_delete", OFF(_delete), true },
{ "AMediaFormat_toString", OFF(_toString), true },
{ "AMediaFormat_getBuffer", OFF(_getBuffer), true },
{ "AMediaFormat_setString", OFF(_setString), true },
{ "AMediaFormat_setBuffer", OFF(_setBuffer), true },
{ "AMediaFormat_setInt32", OFF(_setInt32), true },
{ "AMediaFormat_getInt32", OFF(_getInt32), true },
{ "AMediaFormat_getString", OFF(_getString), true },
#undef OFF
{ NULL, 0, false }
};
#undef OFF

/* Initialize all symbols.
* Done only one time during the first initialisation */

static bool init_symbols()
{
	int g_sdkVersion = 0;
	char sdk[10] = {0};
	__system_property_get("ro.build.version.sdk",sdk);
	g_sdkVersion = atoi(sdk);
	 
	void *ndk_handle;
	static pthread_mutex_t mutex = {__PTHREAD_MUTEX_INIT_VALUE};
	static int i_init_state = -1;
	bool ret;

	pthread_mutex_lock(&mutex);

	if (i_init_state != -1)
		goto end;

	i_init_state = 0;

	if (g_sdkVersion >= 20){
		ndk_handle = dlopen("libmediandk.so", RTLD_NOW);
		NativeCodec_LOGI("[init_symbols]SDK_INT : %d  ndk_handle : %s", g_sdkVersion,"libmediandk.so");
	}
	else {
		char libname[64] = {0};
		sprintf(libname, "libnative_codec%d.so", g_sdkVersion);
		//char *libname = "libnative_codec.so";
		ndk_handle = dlopen(libname, RTLD_LAZY);
		NativeCodec_LOGI("[init_symbols]SDK_INT : %d  ndk_handle : %s", g_sdkVersion,libname);
	}
	if (!ndk_handle)
		goto end;

	for (int i = 0; members[i].name; i++)
	{
		void *sym = dlsym(ndk_handle, members[i].name);
		if (!sym && members[i].critical)
		{
			NativeCodec_LOGE("con not find simbol %s", members[i].name);
			dlclose(ndk_handle);
			goto end;
		}
		*(void **)((uint8_t*)&gdlsys + members[i].offset) = sym;
	}

	i_init_state = 1;
end:
	ret = i_init_state == 1;
	if (!ret)
		NativeCodec_LOGE("MediaCodec NDK init failed");

	pthread_mutex_unlock(&mutex);
	return ret;
}

AMediaCodec* AMediaCodec_createCodecByName(const char *name)
{
	init_symbols();
	return gdlsys.AMediaCodec._createCodecByName(name);
}

AMediaCodec* AMediaCodec_createDecoderByType(const char *mime_type)
{
	init_symbols();
	return gdlsys.AMediaCodec._createDecoderByType(mime_type);
}

AMediaCodec* AMediaCodec_createEncoderByType(const char *mime_type)
{
	init_symbols();
	return gdlsys.AMediaCodec._createEncoderByType(mime_type);
}

media_status_t AMediaCodec_delete(AMediaCodec* codec)
{
	return gdlsys.AMediaCodec._delete(codec);
}

media_status_t AMediaCodec_configure(
	AMediaCodec* codec,
	const AMediaFormat* format,
	ANativeWindow* surface,
	AMediaCrypto *crypto,
	uint32_t flags)
{
	return gdlsys.AMediaCodec._configure(codec, format, surface, crypto, flags);
}

////////AmediaCodec///////////////////
media_status_t AMediaCodec_start(AMediaCodec* codec)
{
	return gdlsys.AMediaCodec._start(codec);
}

media_status_t AMediaCodec_stop(AMediaCodec* codec)
{
	return gdlsys.AMediaCodec._stop(codec);
}

media_status_t AMediaCodec_flush(AMediaCodec* codec)
{
	return gdlsys.AMediaCodec._flush(codec);
}

uint8_t* AMediaCodec_getInputBuffer(AMediaCodec* codec, size_t idx, size_t *out_size)
{
	return gdlsys.AMediaCodec._getInputBuffer(codec, idx, out_size);
}
uint8_t* AMediaCodec_getOutputBuffer(AMediaCodec* codec, size_t idx, size_t *out_size)
{
	return gdlsys.AMediaCodec._getOutputBuffer(codec, idx, out_size);
}
ssize_t AMediaCodec_dequeueInputBuffer(AMediaCodec* codec, int64_t timeoutUs)
{
	return gdlsys.AMediaCodec._dequeueInputBuffer(codec, timeoutUs);
}
media_status_t AMediaCodec_queueInputBuffer(AMediaCodec* codec,
	size_t idx, off_t offset, size_t size, uint64_t time, uint32_t flags)
{
	return gdlsys.AMediaCodec._queueInputBuffer(codec, idx, offset, size, time, flags);
}

ssize_t AMediaCodec_dequeueOutputBuffer(AMediaCodec* codec, AMediaCodecBufferInfo *info, int64_t timeoutUs)
{
	return gdlsys.AMediaCodec._dequeueOutputBuffer(codec, info, timeoutUs);
}
AMediaFormat* AMediaCodec_getOutputFormat(AMediaCodec* codec)
{
	return gdlsys.AMediaCodec._getOutputFormat(codec);
}
media_status_t AMediaCodec_releaseOutputBuffer(AMediaCodec* codec, size_t idx, int render)
{
	return gdlsys.AMediaCodec._releaseOutputBuffer(codec, idx, render);
}

/////////////////AmediaFormat///////////////////////
AMediaFormat *AMediaFormat_new()
{
	init_symbols();
	return gdlsys.AMediaFormat._new();
}
media_status_t AMediaFormat_delete(AMediaFormat* format)
{
	return gdlsys.AMediaFormat._delete(format);
}
const char* AMediaFormat_toString(AMediaFormat* format)
{
	return gdlsys.AMediaFormat._toString(format);
}

int AMediaFormat_getInt32(AMediaFormat* format, const char *name, int32_t *out)
{
	return gdlsys.AMediaFormat._getInt32(format, name, out);
}

int AMediaFormat_getString(AMediaFormat* format, const char *name, const char **out)
{
	return gdlsys.AMediaFormat._getString(format, name, out);
}

int AMediaFormat_getBuffer(AMediaFormat* format, const char *name, void** data, size_t *size)
{
	return gdlsys.AMediaFormat._getBuffer(format, name, data, size);
}

void AMediaFormat_setInt32(AMediaFormat* format, const char* name, int32_t value)
{
	return gdlsys.AMediaFormat._setInt32(format, name, value);
}

void AMediaFormat_setString(AMediaFormat* format, const char* name, const char* value)
{
	return gdlsys.AMediaFormat._setString(format, name, value);
}
void AMediaFormat_setBuffer(AMediaFormat* format, const char* name, void* data, size_t size)
{
	return gdlsys.AMediaFormat._setBuffer(format, name, data, size);
}


#ifdef __cplusplus
}
#endif
