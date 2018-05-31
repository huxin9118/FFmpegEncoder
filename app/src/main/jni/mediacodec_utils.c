#include "mediacodec/mediacodec.h"

void NV12toYUV420Planar(uint8_t* input, int offset, uint8_t* output, int width, int height) {
	int frameSize = width * height;
	int qFrameSize = frameSize / 4;

	memmove(output, input + offset, frameSize); // Y
	int i = 0;
	for (i = 0; i < qFrameSize; i++) {
		output[frameSize + i] = input[offset + frameSize + i * 2]; // U
		output[frameSize + qFrameSize + i] = input[offset + frameSize + i * 2 + 1]; // V
	}
}

void NV21toYUV420Planar(uint8_t* input, int offset, uint8_t* output, int width, int height) {
	int frameSize = width * height;
	int qFrameSize = frameSize / 4;

	memmove(output, input + offset, frameSize); // Y
	int i = 0;
	for (i = 0; i < qFrameSize; i++) {
		output[frameSize + i] = input[offset + frameSize + i * 2 + 1]; // U
		output[frameSize + qFrameSize + i] = input[offset + frameSize + i * 2]; // V
	}
}

void I420toYUV420SemiPlanar(uint8_t* input, int offset, uint8_t* output, int width, int height) {
	int frameSize = width * height;
	int qFrameSize = frameSize / 4;

	memmove(output, input + offset, frameSize); // Y
	int i = 0;
	for (i = 0; i < qFrameSize; i++) {
		output[frameSize + i * 2] = input[offset + frameSize + i]; // Cb (U)
		output[frameSize + i * 2 + 1] = input[offset + frameSize + i + qFrameSize]; // Cr (V)
	}
}

void I420toNV21(uint8_t* input, int offset, uint8_t* output, int width, int height) {
	int frameSize = width * height;
	int qFrameSize = frameSize / 4;

	memmove(output, input + offset, frameSize); // Y
	int i = 0;
	for (i = 0; i < qFrameSize; i++) {
		output[frameSize + i * 2 + 1] = input[offset + frameSize + i]; // Cb (U)
		output[frameSize + i * 2] = input[offset + frameSize + i + qFrameSize]; // Cr (V)
	}
}

void swapNV12toNV21(uint8_t* input, int offset, uint8_t* output, int width, int height){
	int frameSize = width * height;
	int qFrameSize = frameSize / 2;
	
	memmove(output, input + offset, frameSize); // Y
	int i = 0;
	for (i = 0; i + 1 < qFrameSize; i += 2) {
		output[frameSize + i] = input[offset + frameSize + i + 1]; // U
		output[frameSize + i + 1] = input[offset + frameSize + i]; // V
	}
}

void CropYUV420SemiPlanar(uint8_t* input, int width, int height, uint8_t* output,
					int crop_left, int crop_right, int crop_top, int crop_bottom) {
	int i = 0;
	for(i = crop_top; i <= crop_bottom; i++) {
		memmove(output + (i * (crop_right - crop_left + 1)), input + (i * width + crop_left), crop_right - crop_left + 1); // Y
	}

	for(i = crop_top; i <= crop_bottom / 2; i++) {
		memmove(output + ((crop_right - crop_left + 1) * (crop_bottom - crop_top + 1) + i * (crop_right - crop_left + 1)), 
				input + (width * height + i * width + crop_left), crop_right - crop_left + 1); // UV
	}
}

void CropYUV420Planar(uint8_t* input, int width, int height, uint8_t* output,
					int crop_left, int crop_right, int crop_top, int crop_bottom) {
	int i = 0;
	for(i = crop_top; i <= crop_bottom; i++) {
		memmove(output + (i * (crop_right - crop_left + 1)), input + (i * width + crop_left), crop_right - crop_left + 1); // Y
	}

	for(i = crop_top; i <= crop_bottom / 2; i++) {
		memmove(output + ((crop_right - crop_left + 1) * (crop_bottom - crop_top + 1) + i * (crop_right - crop_left + 1) / 2), 
				input + (width * height + i * width / 2 + crop_left), (crop_right - crop_left + 1) / 2); // U
	}
	
	for(i = crop_top; i <= crop_bottom / 2; i++) {
		memmove(output + ((crop_right - crop_left + 1) * (crop_bottom - crop_top + 1) / 4 * 5 + i * (crop_right - crop_left + 1) / 2), 
				input + (width * height / 4 * 5 + i * width / 2 + crop_left), (crop_right - crop_left + 1) / 2); // U
	}
}