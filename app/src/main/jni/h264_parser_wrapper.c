#include "h264_parser.h"

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "h264_parser", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "h264_parser", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf("h264_parser " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("h264_parser " format "\n", ##__VA_ARGS__)
#endif

#define CONCAT1(prefix, class, function)    CONCAT2(prefix, class, function)
#define CONCAT2(prefix, class, function)    Java_ ## prefix ## _ ## class ## _ ## function
#define JAVA_PACKET							com_example_ffmpegencoder_mediacodec
#define JAVA_HH264Encoder(function)         CONCAT1(JAVA_PACKET, HH264Encoder, function)
#define JAVA_CLASS_HH264Encoder   			"com/example/ffmpegsdlplayer/mediacodec/HH264Encoder"

typedef struct h264_parser_context{
	sps_rbsp sps_rbsp;
	pps_rbsp pps_rbsp;
	slice_header slice_header;
}h264_parser_context;

JNIEXPORT jint JNICALL JAVA_HH264Encoder(allocH264Parser)(JNIEnv* env, jobject obj){
	h264_parser_context* ctx = (h264_parser_context*)malloc(sizeof(h264_parser_context));
	LOGI("allocH264Parser ctx=%p",ctx);
	return (jint)ctx;
}

JNIEXPORT void JNICALL JAVA_HH264Encoder(freeH264Parser)(JNIEnv* env, jobject obj, jint point){
	h264_parser_context* ctx = (h264_parser_context*)point;
	LOGI("freeH264Parser ctx=%p",ctx);
	free(ctx);
}

JNIEXPORT void JNICALL JAVA_HH264Encoder(setSPS)(JNIEnv* env, jobject obj, jint point, jbyteArray sps_array, jint pos, jint length){
	h264_parser_context* ctx = (h264_parser_context*)point;
	uint8_t sps[length-pos];
	(*env)->GetByteArrayRegion(env, sps_array, pos, length-pos, (jbyte*)sps);
	memset(&ctx->sps_rbsp,0,sizeof(sps_rbsp));
	int width,height;
	h264_decode_seq_parameter_set(sps, length-pos, &ctx->sps_rbsp, &width, &height);
}

JNIEXPORT void JNICALL JAVA_HH264Encoder(setPPS)(JNIEnv* env, jobject obj, jint point, jbyteArray pps_array, jint pos, jint length){
	h264_parser_context* ctx = (h264_parser_context*)point;
	LOGI("setPPS ctx=%p",ctx);
	uint8_t pps[length-pos];
	(*env)->GetByteArrayRegion(env, pps_array, pos, length-pos, (jbyte*)pps);
	memset(&ctx->pps_rbsp,0,sizeof(pps_rbsp));
	h264_decode_pic_parameter_set(pps, length-pos, &ctx->pps_rbsp);
}

JNIEXPORT jint JNICALL JAVA_HH264Encoder(getSliceQPY)(JNIEnv* env, jobject obj, jint point, jbyteArray slice_array, jint pos, jint length){
	h264_parser_context* ctx = (h264_parser_context*)point;
	LOGI("getSliceQPY ctx=%p",ctx);
	uint8_t slice[length-pos];
	(*env)->GetByteArrayRegion(env, slice_array, pos, length-pos, (jbyte*)slice);
	memset(&ctx->slice_header,0,sizeof(slice_header));
	h264_decode_slice_header(slice, length-pos, &ctx->sps_rbsp, &ctx->pps_rbsp, &ctx->slice_header);
	return (jint)(26 + ctx->pps_rbsp.pic_init_qp_minus26 + ctx->slice_header.slice_qp_delta);
}