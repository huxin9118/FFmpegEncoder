package com.example.ffmpegencoder.mediacodec;

public class H264Utils {

	public static int ffAvcFindStartcode(byte[] data, int offset, int end) {
		int out = ffAvcFindStartcodeInternal(data, offset, end);
		if (offset < out && out < end && data[out - 1] == 0)
			out--;
		return out;
	}
	private static int ffAvcFindStartcodeInternal(byte[] data, int offset, int end) {
		int a = offset + 4 - (offset & 3);

		for (end -= 3; offset < a && offset < end; offset++) {
			if (data[offset] == 0 && data[offset + 1] == 0 && data[offset + 2] == 1)
				return offset;
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
			if (data[offset] == 0 && data[offset + 1] == 0 && data[offset + 2] == 1)
				return offset;
		}

		return end + 3;
	}

	 private void swapYV12toI420(byte[] yv12bytes, byte[] i420bytes, int width,int height) {
		 System.arraycopy(yv12bytes, 0, i420bytes, 0, width * height);
		 System.arraycopy(yv12bytes, width * height + width * height / 4,
		 i420bytes, width * height, width * height / 4);
		 System.arraycopy(yv12bytes, width * height, i420bytes, width * height + width * height / 4, width * height / 4);
	 }

	public static byte[] swapNV12toNV21(final byte[] input, final int offset, final byte[] output, final int width, final int height) {
		final int frameSize = width * height;
		final int qFrameSize = frameSize / 2;

		System.arraycopy(input, offset, output, 0, frameSize); // Y

		for (int i = 0; i + 1 < qFrameSize; i += 2) {
			output[frameSize + i] = input[offset + frameSize + i + 1]; // U
			output[frameSize + i + 1] = input[offset + frameSize + i]; // V
		}
		return output;
	}

	public static byte[] NV21toYUV420Planar(final byte[] input, final int offset, final byte[] output, final int width, final int height) {
		final int frameSize = width * height;
		final int qFrameSize = frameSize / 4;

		System.arraycopy(input, offset, output, 0, frameSize); // Y

		for (int i = 0; i < qFrameSize; i++) {
			output[frameSize + i] = input[offset + frameSize + i * 2 + 1]; // U
			output[frameSize + qFrameSize + i] = input[offset + frameSize + i * 2]; // V
		}

		return output;
	}

	public static byte[] NV12toYUV420Planar(final byte[] input, final int offset, final byte[] output, final int width, final int height) {
		final int frameSize = width * height;
		final int qFrameSize = frameSize / 4;

		System.arraycopy(input, offset, output, 0, frameSize); // Y

		for (int i = 0; i < qFrameSize; i++) {
			output[frameSize + i] = input[offset + frameSize + i * 2]; // U
			output[frameSize + qFrameSize + i] = input[offset + frameSize + i * 2 + 1]; // V
		}
		return output;
	}

	public static byte[] YV12toYUV420SemiPlanar(final byte[] input, final int offset, final byte[] output, final int width, final int height) {
		/*
		 * COLOR_TI_FormatYUV420PackedSemiPlanar is NV12 We convert by putting
		 * the corresponding U and V bytes together (interleaved).
		 */
		final int frameSize = width * height;
		final int qFrameSize = frameSize / 4;

		System.arraycopy(input, offset, output, 0, frameSize); // Y

		for (int i = 0; i < qFrameSize; i++) {
			output[frameSize + i * 2] = input[offset + frameSize + i + qFrameSize]; // Cb (U)
			output[frameSize + i * 2 + 1] = input[offset + frameSize + i]; // Cr (V)
		}
		return output;
	}

	public static byte[] I420toYUV420SemiPlanar(final byte[] input, final int offset, final byte[] output, final int width, final int height) {
		/*
		 * COLOR_TI_FormatYUV420PackedSemiPlanar is NV12 We convert by putting
		 * the corresponding U and V bytes together (interleaved).
		 */
		final int frameSize = width * height;
		final int qFrameSize = frameSize / 4;

		System.arraycopy(input, offset, output, 0, frameSize); // Y

		for (int i = 0; i < qFrameSize; i++) {
			output[frameSize + i * 2] = input[offset + frameSize + i]; // Cb (U)
			output[frameSize + i * 2 + 1] = input[offset + frameSize + i + qFrameSize]; // Cr (V)
		}
		return output;
	}

	public static byte[] I420toNV21(final byte[] input, final int offset, final byte[] output, final int width, final int height) {
		/*
		 * COLOR_TI_FormatYUV420PackedSemiPlanar is NV12 We convert by putting
		 * the corresponding U and V bytes together (interleaved).
		 */
		final int frameSize = width * height;
		final int qFrameSize = frameSize / 4;

		System.arraycopy(input, offset, output, 0, frameSize); // Y

		for (int i = 0; i < qFrameSize; i++) {
			output[frameSize + i * 2 + 1] = input[offset + frameSize + i]; // Cb (U)
			output[frameSize + i * 2 + 1] = input[offset + frameSize + i + qFrameSize]; // Cr (V)
		}
		return output;
	}

	public static void CropYUV420SemiPlanar(final byte[] input, final int width, final int height, final byte[] output,
										  final int crop_left, final int crop_right, final int crop_top, final int crop_bottom) {

		for(int i = crop_top; i <= crop_bottom; i++) {
			System.arraycopy(input, i * width + crop_left, output, i * (crop_right - crop_left + 1), crop_right - crop_left + 1); // Y
		}

		for(int i = crop_top; i <= crop_bottom / 2; i++) {
			System.arraycopy(input, width * height + i * width + crop_left, output,
					(crop_right - crop_left + 1) * (crop_bottom - crop_top + 1) + i * (crop_right - crop_left + 1),
					crop_right - crop_left + 1); // Y
		}
	}

	public static void CropYUV420Planar(final byte[] input, final int width, final int height, final byte[] output,
											final int crop_left, final int crop_right, final int crop_top, final int crop_bottom) {

		for(int i = crop_top; i <= crop_bottom; i++) {
			System.arraycopy(input, i * width + crop_left, output, i * (crop_right - crop_left + 1), crop_right - crop_left + 1); // Y
		}

		for(int i = crop_top; i <= crop_bottom / 2; i++) {
			System.arraycopy(input, width * height + i * width / 2 + crop_left,
							output,((crop_right - crop_left + 1) * (crop_bottom - crop_top + 1) + i * (crop_right - crop_left + 1) / 2),
					(crop_right - crop_left + 1) / 2); // U
		}

		for(int i = crop_top; i <= crop_bottom / 2; i++) {
			System.arraycopy(input, width * height / 4 * 5 + i * width / 2 + crop_left,
					output,((crop_right - crop_left + 1) * (crop_bottom - crop_top + 1) / 4 * 5 + i * (crop_right - crop_left + 1) / 2),
					(crop_right - crop_left + 1) / 2); // V
		}
	}
}
