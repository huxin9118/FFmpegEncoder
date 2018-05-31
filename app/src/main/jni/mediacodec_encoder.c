#include "mediacodec/mediacodec.h"

MediaCodecEncoder* mediacodec_encoder_alloc(int isDebug, int width, int height, int frame_rate, int bit_rate, int timeout, YUV_PIXEL_FORMAT yuv_pixel_format) {
	if (isDebug) {
		MediaCodec_LOGI("[alloc]");
	}
	MediaCodecEncoder* encoder = (MediaCodecEncoder*)malloc(sizeof(MediaCodecEncoder));	
	encoder->codec = NULL;
    
	encoder->mime = NULL;
	encoder->colorFormat = 0;
    
	encoder->DEBUG = isDebug;
	encoder->TIME_OUT = timeout;
	encoder->MAX_TIME_OUT = 0;
	encoder->MIMETYPE_VIDEO_AVC = "video/avc";//只有高通cpu支持hevc-encoder
	encoder->profile = 0;
	encoder->level = 0;
	encoder->yuv_pixel_format = yuv_pixel_format;
	encoder->sps = NULL;
	encoder->sps_length = 0;
	encoder->pps = NULL;
	encoder->pps_length = 0;
	encoder->startCode = "\0\0\0\1";
	encoder->startCode_length = 4;
	encoder->firstI = 0;
    
	encoder->width = width;
	encoder->height = height;
	encoder->frame_rate = frame_rate;
	encoder->bit_rate = bit_rate;
	encoder->frameIndex = 0;
	
	encoder->SDK_INT = 0;
	encoder->phone_type = NULL;
	encoder->hardware = NULL;
	return encoder;
}

int mediacodec_encoder_free(MediaCodecEncoder* encoder) {
	if(encoder){
		if (encoder->DEBUG) {
			MediaCodec_LOGI("[free]");
		}
		free(encoder);
		return 0;
	}
	else{
		if (encoder->DEBUG) {
			MediaCodec_LOGE("[free]ERROR_SDK_ENCODER_IS_NULL");
		}
		return -1;
	}
}

int mediacodec_encoder_open(MediaCodecEncoder* encoder) {
	if(encoder){
		if (encoder->DEBUG) {
			MediaCodec_LOGI("[open]");
		}
		char sdk[10] = {0};
		__system_property_get("ro.build.version.sdk",sdk);
		encoder->SDK_INT = atoi(sdk);
		
		if(encoder-> SDK_INT >= 16){
			if (encoder->DEBUG) {
				MediaCodec_LOGI("[open]SDK_INT : %d", encoder->SDK_INT);
			}
			if(encoder-> SDK_INT >= 21){
				encoder->MAX_TIME_OUT = 100;
			}
			else{
				encoder->MAX_TIME_OUT = 200;
			}
		}
		else{
			if (encoder->DEBUG) {
				MediaCodec_LOGE("[open]ERROR_SDK_VERSION_TOO_LOW  SDK_INT : %d", encoder->SDK_INT);
			}
			return -1;//ERROR_SDK_VERSION_TOO_LOW
		}
		
		char phone[20] = {0};
		__system_property_get("ro.product.model",phone);
		encoder->phone_type = (char*)malloc(20);
		memmove(encoder->phone_type, phone, 20);
		if (encoder->DEBUG) {
			MediaCodec_LOGI("[open]phone_type : %s", encoder->phone_type);
		}
		
		char cpu[20] = {0};
		__system_property_get("ro.hardware",cpu);
		encoder->hardware = (char*)malloc(20);
		memmove(encoder->hardware, cpu, 20);
		if (encoder->DEBUG) {
			MediaCodec_LOGI("[open]hardware : %s", encoder->hardware);
		}
		
		int status;
		
		// /** Constant quality mode */
		// public static final int BITRATE_MODE_CQ = 0;
		// /** Variable bitrate mode */
		// public static final int BITRATE_MODE_VBR = 1;
		// /** Constant bitrate mode */
		// public static final int BITRATE_MODE_CBR = 2;
		
		
		// // from OMX_VIDEO_AVCPROFILETYPE
		// public static final int AVCProfileBaseline = 0x01;
		// public static final int AVCProfileMain     = 0x02;
		// public static final int AVCProfileExtended = 0x04;
		// public static final int AVCProfileHigh     = 0x08;
		// public static final int AVCProfileHigh10   = 0x10;
		// public static final int AVCProfileHigh422  = 0x20;
		// public static final int AVCProfileHigh444  = 0x40;

		// // from OMX_VIDEO_AVCLEVELTYPE
		// public static final int AVCLevel1       = 0x01;
		// public static final int AVCLevel1b      = 0x02;
		// public static final int AVCLevel11      = 0x04;
		// public static final int AVCLevel12      = 0x08;
		// public static final int AVCLevel13      = 0x10;
		// public static final int AVCLevel2       = 0x20;
		// public static final int AVCLevel21      = 0x40;
		// public static final int AVCLevel22      = 0x80;
		// public static final int AVCLevel3       = 0x100;
		// public static final int AVCLevel31      = 0x200;
		// public static final int AVCLevel32      = 0x400;
		// public static final int AVCLevel4       = 0x800;
		// public static final int AVCLevel41      = 0x1000;
		// public static final int AVCLevel42      = 0x2000;
		// public static final int AVCLevel5       = 0x4000;
		// public static final int AVCLevel51      = 0x8000;
		// public static final int AVCLevel52      = 0x10000;
		
		// // from OMX_VIDEO_HEVCPROFILETYPE
		// public static final int HEVCProfileMain        = 0x01;
		// public static final int HEVCProfileMain10      = 0x02;
		// public static final int HEVCProfileMain10HDR10 = 0x1000;

		// // from OMX_VIDEO_HEVCLEVELTYPE
		// public static final int HEVCMainTierLevel1  = 0x1;
		// public static final int HEVCHighTierLevel1  = 0x2;
		// public static final int HEVCMainTierLevel2  = 0x4;
		// public static final int HEVCHighTierLevel2  = 0x8;
		// public static final int HEVCMainTierLevel21 = 0x10;
		// public static final int HEVCHighTierLevel21 = 0x20;
		// public static final int HEVCMainTierLevel3  = 0x40;
		// public static final int HEVCHighTierLevel3  = 0x80;
		// public static final int HEVCMainTierLevel31 = 0x100;
		// public static final int HEVCHighTierLevel31 = 0x200;
		// public static final int HEVCMainTierLevel4  = 0x400;
		// public static final int HEVCHighTierLevel4  = 0x800;
		// public static final int HEVCMainTierLevel41 = 0x1000;
		// public static final int HEVCHighTierLevel41 = 0x2000;
		// public static final int HEVCMainTierLevel5  = 0x4000;
		// public static final int HEVCHighTierLevel5  = 0x8000;
		// public static final int HEVCMainTierLevel51 = 0x10000;
		// public static final int HEVCHighTierLevel51 = 0x20000;
		// public static final int HEVCMainTierLevel52 = 0x40000;
		// public static final int HEVCHighTierLevel52 = 0x80000;
		// public static final int HEVCMainTierLevel6  = 0x100000;
		// public static final int HEVCHighTierLevel6  = 0x200000;
		// public static final int HEVCMainTierLevel61 = 0x400000;
		// public static final int HEVCHighTierLevel61 = 0x800000;
		// public static final int HEVCMainTierLevel62 = 0x1000000;
		// public static final int HEVCHighTierLevel62 = 0x2000000;

		encoder->codec = AMediaCodec_createEncoderByType(encoder->MIMETYPE_VIDEO_AVC);
		AMediaFormat* mediaFormat = AMediaFormat_new();
		AMediaFormat_setString(mediaFormat, AMEDIAFORMAT_KEY_MIME, encoder->MIMETYPE_VIDEO_AVC);
		AMediaFormat_setInt32(mediaFormat, AMEDIAFORMAT_KEY_WIDTH, encoder->width);
		AMediaFormat_setInt32(mediaFormat, AMEDIAFORMAT_KEY_HEIGHT, encoder->height);
		AMediaFormat_setInt32(mediaFormat, AMEDIAFORMAT_KEY_BIT_RATE, encoder->bit_rate);
		// AMediaFormat_setInt32(mediaFormat, "max-bitrate", encoder->bit_rate * 2);
		// AMediaFormat_setInt32(mediaFormat, "bitrate-mode", 1);//BITRATE_MODE_VBR
		AMediaFormat_setInt32(mediaFormat, AMEDIAFORMAT_KEY_FRAME_RATE, encoder->frame_rate);
		
		if(encoder->hardware[0] == 'm' && encoder->hardware[1] == 't'){
			AMediaFormat_setInt32(mediaFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT, 19);//COLOR_FormatYUV420Planar
		}
		else{
			AMediaFormat_setInt32(mediaFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT, 21);//COLOR_FormatYUV420SemiPlanar
		}
		
		AMediaFormat_setInt32(mediaFormat, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, 1);//1s
		if(!strcmp("video/avc",encoder->MIMETYPE_VIDEO_AVC)){
			encoder->profile = 0x08;//AVCProfileHigh
			
			encoder->level = 0x100;//AVCLevel30
			if(encoder->width * encoder->height >= 1280 * 720){
				encoder->level = 0x200;//AVCLevel31
			}
			if(encoder->width * encoder->height >= 1920 * 1080){
				encoder->level = 0x800;//AVCLevel40
			}
		}
		else if(!strcmp("video/hevc",encoder->MIMETYPE_VIDEO_AVC)){
			encoder->profile =  0x01;//HEVCProfileMain
			
			encoder->level = 0x80;//HEVCHighTierLevel30
			if(encoder->width * encoder->height >= 1280 * 720){
				encoder->level = 0x200;//HEVCHighTierLevel31
			}
			if(encoder->width * encoder->height >= 1920 * 1080){
				encoder->level = 0x800;//HEVCHighTierLevel40
			}
		}
		if (encoder->DEBUG) {
			MediaCodec_LOGI("[open]AMEDIAFORMAT_KEY_MIME : %s  profile : %x  level : %x", encoder->MIMETYPE_VIDEO_AVC, encoder->profile, encoder->level);
		}
		
		AMediaFormat_setInt32(mediaFormat, "profile", encoder->profile);
		AMediaFormat_setInt32(mediaFormat, "level", encoder->level);
		
		
		status = AMediaCodec_configure(encoder->codec, mediaFormat, NULL, NULL, 1);
		if(status){
			if (encoder->DEBUG) {
				MediaCodec_LOGE("[open]AMediaCodec_configure error");
			}		
			return status;
		}
		status = AMediaCodec_start(encoder->codec);
		if(status){
			if (encoder->DEBUG) {
				MediaCodec_LOGE("[open]AMediaCodec_start error");
			}		
			return status;
		}
		status = AMediaCodec_flush(encoder->codec);
		if(status){
			if (encoder->DEBUG) {
				MediaCodec_LOGE("[open]AMediaCodec_flush error");
			}
			return status;
		}
		return status;
	}
	else{
		if (encoder->DEBUG) {
			MediaCodec_LOGE("[open]ERROR_SDK_ENCODER_IS_NULL");
		}
		return -1;
	}
}	

int mediacodec_encoder_close(MediaCodecEncoder* encoder) {
	if(encoder){
		if (encoder->DEBUG) {
			MediaCodec_LOGI("[close]");
		}
		
		int status;
		
		if(encoder->codec){
			status = AMediaCodec_flush(encoder->codec);
			if(status){
				if (encoder->DEBUG) {
					MediaCodec_LOGE("[close]AMediaCodec_flush error");
				}
				return status;
			}		
			status = AMediaCodec_stop(encoder->codec);
			if(status){
				if (encoder->DEBUG) {
					MediaCodec_LOGE("[close]AMediaCodec_stop error");
				}
				return status;
			}
			status = AMediaCodec_delete(encoder->codec);
			if(status){
				if (encoder->DEBUG) {
					MediaCodec_LOGE("[close]AMediaCodec_delete error");
				}
				return status;
			}
			encoder->codec = NULL;
		}
		if(encoder->sps){
			free(encoder->sps);
			encoder->sps = NULL;
		}
		if(encoder->pps){
			free(encoder->pps);
			encoder->pps = NULL;
		}
		if(encoder->phone_type){
			free(encoder->phone_type);
			encoder->phone_type = NULL;
		}
		if(encoder->hardware){
			free(encoder->hardware);
			encoder->hardware = NULL;
		}
		return status;
	}
	else{
		if (encoder->DEBUG) {
			MediaCodec_LOGE("[close]ERROR_SDK_ENCODER_IS_NULL");
		}
		return -1;
	}
}

int mediacodec_encoder_encode(MediaCodecEncoder* encoder, uint8_t* in, int offset, uint8_t* out, int length, int* error_code) {
	if(encoder){
		if (encoder->DEBUG) {
			MediaCodec_LOGI("[encode]");
		}
		
		int pos = 0;
		*error_code = 0;

		if (out == NULL) {
			if (encoder->DEBUG) {
				if (encoder->DEBUG) {
					MediaCodec_LOGE("ERROR_CODE_OUT_BUF_NULL");
				}
			}
			return -2;//ERROR_CODE_OUT_BUF_NULL
		}

		ssize_t inputBufferIndex = AMediaCodec_dequeueInputBuffer(encoder->codec, encoder->TIME_OUT);
		size_t inputBufferSize = 0;

		if (encoder->DEBUG) {
			MediaCodec_LOGI("inputBufferIndex : %d", inputBufferIndex);
		}
		
		if (inputBufferIndex >= 0) {
			uint8_t* inputBuffer = AMediaCodec_getInputBuffer(encoder->codec, inputBufferIndex, &inputBufferSize);
			if(inputBuffer != NULL && inputBufferSize >= length){
				if(encoder->hardware[0] == 'm' && encoder->hardware[1] == 't'){
					if(encoder->yuv_pixel_format == I420){
						memmove(inputBuffer, in, length);
					}
					else if(encoder->yuv_pixel_format == NV12){
						NV12toYUV420Planar(in, offset, inputBuffer, encoder->width, encoder->height);
					}
					else if(encoder->yuv_pixel_format == NV21){
						NV21toYUV420Planar(in, offset, inputBuffer, encoder->width, encoder->height);
					}
				}
				else{
					if(encoder->yuv_pixel_format == I420){
						I420toYUV420SemiPlanar(in, offset, inputBuffer, encoder->width, encoder->height);
					}
					else if(encoder->yuv_pixel_format == NV12){
						memmove(inputBuffer, in, length);
					}
					else if(encoder->yuv_pixel_format == NV21){
						swapNV12toNV21(in, offset, inputBuffer, encoder->width, encoder->height);
					}
				}
				AMediaCodec_queueInputBuffer(encoder->codec, inputBufferIndex, 0, length, mediacodec_encoder_computePresentationTime(encoder), 0);
			}
			else{
				if (encoder->DEBUG) {
					MediaCodec_LOGE("ERROR_CODE_INPUT_BUFFER_FAILURE inputBufferSize/in.length : %d/%d", inputBufferIndex, length);
				}
				*error_code = -3;//ERROR_CODE_INPUT_BUFFER_FAILURE
			}
		} else {
			if (encoder->DEBUG) {
				MediaCodec_LOGE("ERROR_CODE_INPUT_BUFFER_FAILURE inputBufferIndex : %d", inputBufferIndex);
			}
			*error_code = -3;//ERROR_CODE_INPUT_BUFFER_FAILURE
		}

		AMediaCodecBufferInfo bufferInfo;
		ssize_t outputBufferIndex = 0;
		size_t outputBufferSize = 0;
		
		while (outputBufferIndex != AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
			outputBufferIndex = AMediaCodec_dequeueOutputBuffer(encoder->codec, &bufferInfo, encoder->TIME_OUT);

			if (encoder->DEBUG) {
				MediaCodec_LOGI("outputBufferIndex : %d",outputBufferIndex);
			}
			
			
			if(outputBufferIndex <= -20000){
				if (encoder->DEBUG) {
					MediaCodec_LOGE("AMEDIA_DRM_ERROR_BASE");
				}
				*error_code = outputBufferIndex;
				return pos;
			}
			if(outputBufferIndex <= -10000){
				if (encoder->DEBUG) {
					MediaCodec_LOGE("AMEDIA_ERROR_BASE");
				}
				*error_code = outputBufferIndex;
				return pos;
			}

			if (outputBufferIndex == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
				// outputBuffers = codec.getOutputBuffers();
				continue;
			} 
			else if (outputBufferIndex == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
				/**
				 * <code>
				 *	mediaFormat : {
				 *		image-data=java.nio.HeapByteBuffer[pos=0 lim=104 cap=104],
				 *		mime=video/raw,
				 *		crop-top=0,
				 *		crop-right=703,
				 *		slice-height=576,
				 *		color-format=21,
				 *		height=576, width=704,
				 *		crop-bottom=575, crop-left=0,
				 *		hdr-static-info=java.nio.HeapByteBuffer[pos=0 lim=25 cap=25],
				 *		stride=704
				 *    }
				 *	</code>
				 */
				
				AMediaFormat* mediaFormat = AMediaCodec_getOutputFormat(encoder->codec);

				AMediaFormat_getString(mediaFormat, AMEDIAFORMAT_KEY_MIME, &(encoder->mime));
				AMediaFormat_getInt32(mediaFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT, &(encoder->colorFormat));

				if (encoder->DEBUG) {
					MediaCodec_LOGI("mediaFormat : %s",AMediaFormat_toString(mediaFormat));
				}
			} 
			else if (outputBufferIndex >= 0) {
				uint8_t* outputBuffer = AMediaCodec_getOutputBuffer(encoder->codec, outputBufferIndex, &outputBufferSize);
				if(outputBuffer){
					if (encoder->DEBUG) {
						MediaCodec_LOGI("bufferInfo : %d",bufferInfo.size);
					}
					
					if(!strcmp("video/avc",encoder->MIMETYPE_VIDEO_AVC)){
						if (encoder->sps == NULL || encoder->pps == NULL) {//pps sps只有开始时第一个帧里有，保存起来后面用
							offset = 0;

							while (offset < bufferInfo.size && (encoder->sps == NULL || encoder->pps == NULL)) {
								int count = 0;

								while (outputBuffer[offset] == 0) {
									offset++;
								}

								offset++;
								count = mediacodec_encoder_ffAvcFindStartcode(outputBuffer, offset, bufferInfo.size);

								int naluLength = count - offset;
								int type = outputBuffer[offset] & 0x1F;
								if (encoder->DEBUG) {
									MediaCodec_LOGI("count : %d offset : %d type : %d",count,offset,type);
								}
								if (type == 7) {
									encoder->sps = (uint8_t*)malloc(naluLength + encoder->startCode_length);
									encoder->sps_length = naluLength + encoder->startCode_length;
									memmove(encoder->sps, encoder->startCode, encoder->startCode_length);
									memmove(encoder->sps+encoder->startCode_length, outputBuffer+offset, naluLength);
								} else if (type == 8) {
									encoder->pps = (uint8_t*)malloc(naluLength + encoder->startCode_length);
									encoder->pps_length = naluLength + encoder->startCode_length;
									memmove(encoder->pps, encoder->startCode, encoder->startCode_length);
									memmove(encoder->pps+encoder->startCode_length, outputBuffer+offset, naluLength);
								} else if (type == 5) {
									encoder->firstI = 1;
								}
								offset += naluLength;
							}
						}
						else{
							if (!encoder->firstI) {
								offset = 0;
								while (offset < bufferInfo.size && !encoder->firstI) {
									int count = 0;

									while (outputBuffer[offset] == 0) {
										offset++;

										if (offset >= bufferInfo.size) {
											break;
										}
									}
									
									if (offset >= bufferInfo.size) {
										break;
									}

									offset++;
									count = mediacodec_encoder_ffAvcFindStartcode(outputBuffer, offset, bufferInfo.size);

									if (offset >= bufferInfo.size) {
										break;
									}

									int naluLength = count - offset;
									int type = outputBuffer[offset] & 0x1F;
									if (encoder->DEBUG) {
										MediaCodec_LOGI("count : %d offset : %d type : %d",count,offset,type);
									}
									if (type == 5) {
										encoder->firstI = 1;
									}
									offset += naluLength;
								}
							}
						}
						
						// key frame 编码器生成关键帧时只有 00 00 00 01 65
						// 没有pps encoder->sps,要加上
						if ((outputBuffer[encoder->startCode_length] & 0x1f) == 5) {
							if(encoder->sps){
								memmove(out+pos, encoder->sps, encoder->sps_length);
								pos += encoder->sps_length;
							}
							if(encoder->pps){
								memmove(out+pos, encoder->pps, encoder->pps_length);
								pos += encoder->pps_length;
							}
							memmove(out+pos, outputBuffer, bufferInfo.size);
							pos += bufferInfo.size;
						}
						else
						{
							memmove(out+pos, outputBuffer, bufferInfo.size);
							pos += bufferInfo.size;
						}
						
						if (!encoder->firstI) {
							pos = 0;
						}
					}
					else if(!strcmp("video/hevc",encoder->MIMETYPE_VIDEO_AVC)){
						memmove(out+pos, outputBuffer, bufferInfo.size);
						pos += bufferInfo.size;
					}
				}
				AMediaCodec_releaseOutputBuffer(encoder->codec, outputBufferIndex, 0);
			}
		}

		return pos;
	}
	else{
		if (encoder->DEBUG) {
			MediaCodec_LOGE("[encode]ERROR_SDK_ENCODER_IS_NULL");
		}
		*error_code = -4;
		return 0;
	}
}

int mediacodec_encoder_getConfig_int(MediaCodecEncoder* encoder, char* key) {
	if (!strcmp(AMEDIAFORMAT_KEY_WIDTH, key)) {
		return encoder->width;
	} 
	else if (!strcmp(AMEDIAFORMAT_KEY_HEIGHT, key)) {
		return encoder->height;
	} 
	else if (!strcmp(AMEDIAFORMAT_KEY_COLOR_FORMAT, key)) {
		return encoder->colorFormat;
	}
	else if (!strcmp("timeout", key)) {
		return encoder->TIME_OUT;
	}
	else if (!strcmp("max-timeout", key)) {
		return encoder->MAX_TIME_OUT;
	}
	return -1;
}

int mediacodec_encoder_setConfig_int(MediaCodecEncoder* encoder, char* key, int value) {
	if (!strcmp("timeout", key)) {
		encoder->TIME_OUT = value;
		return 0;
	}
	else{
		return -1;
	}
}

uint64_t mediacodec_encoder_computePresentationTime(MediaCodecEncoder* encoder) {
	if(encoder->DEBUG){
		MediaCodec_LOGI("encoder->frameIndex : %lld",encoder->frameIndex);
	}
	encoder->frameIndex++;
	return encoder->frameIndex * 1000000L / encoder->frame_rate + 132L;
}

int mediacodec_encoder_ffAvcFindStartcodeInternal(uint8_t* data, int offset, int end) {
	int a = offset + 4 - (offset & 3);

	for (end -= 3; offset < a && offset < end; offset++) {
		if (data[offset] == 0 && data[offset + 1] == 0 && data[offset + 2] == 1){
			return offset;
		}
	}

	for (end -= 3; offset < end; offset += 4) {
		int x = ((data[offset] << 8 | data[offset + 1]) << 8 | data[offset + 2]) << 8 | data[offset + 3];
//			System.out.println(Integer.toHexString(x));
		// if ((x - 0x01000100) & (~x) & 0x80008000) // little endian
		// if ((x - 0x00010001) & (~x) & 0x00800080) // big endian
		if (((x - 0x01010101) & (~x) & 0x80808080) != 0) { // generic
			if (data[offset + 1] == 0) {
				if (data[offset] == 0 && data[offset + 2] == 1)
					return offset;
				if (data[offset + 2] == 0 && data[offset + 3] == 1)
					return offset + 1;
			}
			if (data[offset + 3] == 0) {
				if (data[offset + 2] == 0 && data[offset + 4] == 1)
					return offset + 2;
				if (data[offset + 4] == 0 && data[offset + 5] == 1)
					return offset + 3;
			}
		}
	}

	for (end += 3; offset < end; offset++) {
		if (data[offset] == 0 && data[offset + 1] == 0 && data[offset + 2] == 1){
			return offset;
		}
	}

	return end + 3;
}

int mediacodec_encoder_ffAvcFindStartcode(uint8_t* data, int offset, int end) {
	int out = mediacodec_encoder_ffAvcFindStartcodeInternal(data, offset, end);
	if (offset < out && out < end && data[out - 1] == 0){
		out--;
	}
	return out;
}
