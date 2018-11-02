package com.example.ffmpegencoder.mediacodec;

import android.annotation.SuppressLint;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.util.Log;

import java.nio.ByteBuffer;

/**
 * Android 4.1.2以上系统提供MediaCodec接口，可以对H264进行编解码
 * 
 * @author chenyang
 * 
 */
@SuppressLint("NewApi")
public class HH264Decoder extends H264Decoder {

	/**
	 * Android标签
	 */
	public static final String TAG = HH264Decoder.class.getSimpleName();

	/**
	 * 调试日志开关
	 */
	public static final boolean DEBUG = true;

	/**
	 * 解码缓存获得超时时间
	 */
	public static final long TIME_OUT = 1000;

	/**
	 * MediaCodec接口
	 */
	private MediaCodec codec;

	/**
	 * YUV宽度步长
	 */
	private int stride;
	/**
	 * YUV高度步长
	 */
	private int sliceHeight;

	/**
	 * 解码器输出格式类型
	 */
	private String mime;
	/**
	 * 解码器输出媒体颜色格式
	 */
	private int colorFormat;

	/**
	 * 输出YUV缓存数据
	 */
	private byte[] outData;


	private MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();

	// private FileOutputStream outfile;

	/**
	 * {@link H264Decoder#H264Decoder()}
	 */
	public HH264Decoder() {
		super();
	}

	/**
	 * 构造方法
	 * 
	 * @param width
	 * @param height
	 */
	public HH264Decoder(int width, int height) {
		super(width, height);
	}

	/**
	 * {@link Decoder#flush()}
	 */
	@Override
	public void flush() {
		if(codec != null) {
			if (DEBUG) {
				Log.i(TAG, "flush");
			}
			codec.flush();
		}
	}

	/**
	 * {@link Decoder#open()}
	 */
	@SuppressWarnings("deprecation")
	@Override
	public void open() {
		if(codec != null){
			close();
		}
		if (DEBUG) {
			Log.i(TAG, "open");
		}

		try {
			codec = MediaCodec.createDecoderByType(MediaFormat.MIMETYPE_VIDEO_AVC);
			MediaFormat mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, width, height);

			codec.configure(mediaFormat, null, null, 0);
			codec.start();
		} catch (Exception e) {
			throw new RuntimeException(e);
		}
	}

	/**
	 * {@link Decoder#close()}
	 */
	@Override
	public void close() {
		if(codec != null) {
			if (DEBUG) {
				Log.i(TAG, "close");
			}
			try {
				codec.stop();
			}catch (Exception e) {
				e.printStackTrace();
			}finally {
				codec.release();
				codec = null;
			}

			// if(outfile != null){
			// try {
			// outfile.close();
			// } catch (IOException e) {
			// e.printStackTrace();
			// }
			// outfile = null;
			// }
		}
	}

	/**
	 * {@link Decoder#decode(byte[], int, byte[], int)}
	 */
	@SuppressWarnings("deprecation")
	@Override
	public int decode(byte[] in, int offset, byte[] out, int length) {
		if(codec != null) {
			if (DEBUG) {
				Log.i(TAG, "decode");
			}

			int size = 0;
			errorCode = ERROR_CODE_NO_ERROR;

			if (out == null) {
				errorCode = ERROR_CODE_OUT_BUF_NULL;
				return 0;
			}

			int inputBufferIndex = codec.dequeueInputBuffer(-1);
			if (DEBUG) {
				Log.i(TAG, "inputBufferIndex : " + inputBufferIndex);
			}

			if (inputBufferIndex >= 0) {
				ByteBuffer inputBuffer = codec.getInputBuffer(inputBufferIndex);
				inputBuffer.clear();
				inputBuffer.put(in, offset, length);
				codec.queueInputBuffer(inputBufferIndex, 0, length, 0, 0);
			} else {
				errorCode = ERROR_CODE_INPUT_BUFFER_FAILURE;
			}

			int outputBufferIndex = codec.dequeueOutputBuffer(bufferInfo, TIME_OUT);
			int expectBufferSize = 0;
			if (DEBUG) {
				Log.i(TAG, "outputBufferIndex : " + outputBufferIndex);
			}

			if (outputBufferIndex != MediaCodec.INFO_TRY_AGAIN_LATER) {
				if (outputBufferIndex == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {

				} else if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
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
					 *        }
				 *	</code>
				 */
				MediaFormat mediaFormat = codec.getOutputFormat();

				try {
					width = mediaFormat.getInteger(MediaFormat.KEY_WIDTH);
				} catch (Exception e) {
					Log.e(TAG, e.getMessage(), e);
					width = DEFAULT_CONFIG_WIDTH;
				}

				try {
					height = mediaFormat.getInteger(MediaFormat.KEY_HEIGHT);
				} catch (Exception e) {
					Log.e(TAG, e.getMessage(), e);
					height = DEFAULT_CONFIG_WIDTH;
				}

				try {
					crop_left = mediaFormat.getInteger("crop-left");
					crop_right = mediaFormat.getInteger("crop-right");
					crop_top = mediaFormat.getInteger("crop-top");
					crop_bottom = mediaFormat.getInteger("crop-bottom");
				}catch (Exception e) {
					Log.
							e(TAG, e.getMessage(), e);
					crop_left = 0;
					crop_right = width - 1;
					crop_top = 0;
					crop_bottom = height - 1;
				}

				try {
					if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
						stride = mediaFormat.getInteger(MediaFormat.KEY_STRIDE);
					} else {
						stride = width;
					}
				} catch (Exception e) {
					Log.e(TAG, e.getMessage(), e);
					stride = 0;
				}

					try {
						if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
							sliceHeight = mediaFormat.getInteger(MediaFormat.KEY_SLICE_HEIGHT);
						} else {
							sliceHeight = height;
						}
					} catch (Exception e) {
						Log.e(TAG, e.getMessage(), e);
						sliceHeight = 0;
					}

					try {
						mime = mediaFormat.getString(MediaFormat.KEY_MIME);
					} catch (Exception e) {
						Log.e(TAG, e.getMessage(), e);
						mime = null;
					}

					try {
						colorFormat = mediaFormat.getInteger(MediaFormat.KEY_COLOR_FORMAT);
						Log.i(TAG, "output colorFormat:" + colorFormat);
					} catch (Exception e) {
						Log.e(TAG, e.getMessage(), e);
						colorFormat = -1;
					}

					if (DEBUG) {
						Log.i(TAG, "mediaFormat : " + mediaFormat.toString());
					}

					if (stride == 0 || stride < width) {
						stride = width;
					}

					if (sliceHeight == 0 || sliceHeight < height) {
						sliceHeight = height;
					}

					expectBufferSize = stride * sliceHeight * 3 / 2;
					if (outData == null || outData.length < expectBufferSize) {
						outData = new byte[expectBufferSize];
					}
				} else if (outputBufferIndex >= 0) {
					expectBufferSize = stride * sliceHeight * 3 / 2;
					if (outData == null || outData.length < expectBufferSize) {
						outData = new byte[expectBufferSize];
					}
					if (DEBUG) {
						Log.i(TAG, "expectBufferSize=" + expectBufferSize + " bufferInfo.size=" + bufferInfo.size + " bufferInfo.offset=" + bufferInfo.size);
					}

					ByteBuffer outputBuffer = codec.getOutputBuffer(outputBufferIndex);
					if (bufferInfo.size >= expectBufferSize) {
						outputBuffer.position(bufferInfo.offset);
						outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
						outputBuffer.get(outData, 0, bufferInfo.size);

						if (width == stride && height == sliceHeight) {
							System.arraycopy(outData, 0, out, 0, outData.length);
						} else {
							int offset0 = 0;
							int offset1 = 0;
							for (int i = 0; i < sliceHeight; i++) {
								System.arraycopy(outData, offset0, out, offset1, width);
								offset0 += stride;
								offset1 += width;
							}
							for (int j = 0; j < sliceHeight / 2; j++) {
								System.arraycopy(outData, offset0, out, offset1, width / 2);
								offset0 += stride / 2;
								offset1 += (width / 2);
							}
						}

						if (crop_right - crop_left + 1 < width || crop_bottom - crop_top + 1 < height) {
							size = (crop_right - crop_left + 1) * (crop_bottom - crop_top + 1) * 3 / 2;
							if (colorFormat == MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar) {
								H264Utils.CropYUV420Planar(out, width, height, outData, crop_left, crop_right, crop_top, crop_bottom);
								System.arraycopy(outData, 0, out, 0, size);
							} else {
								H264Utils.CropYUV420SemiPlanar(out, width, height, outData, crop_left, crop_right, crop_top, crop_bottom);
								H264Utils.NV12toYUV420Planar(outData, 0, out, crop_right - crop_left + 1, crop_bottom - crop_top + 1);
							}
						} else {
							size = width * height * 3 / 2;
							if (colorFormat == MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar) {
//								System.arraycopy(out, 0, outData, 0, size);
//								System.arraycopy(outData, 0, out, 0, size);
							} else {
								System.arraycopy(out, 0, outData, 0, size);
								H264Utils.NV12toYUV420Planar(outData, 0, out, crop_right - crop_left + 1, crop_bottom - crop_top + 1);
							}
						}

//						if(outfile == null){
//							try {
//								outfile = new FileOutputStream("sdcard/out_"+(crop_right - crop_left + 1)+"x"+(crop_bottom - crop_top + 1)+".yuv");
//							} catch (FileNotFoundException e) {
//								e.printStackTrace();
//							}
//						} else{
//							try {
//								outfile.write(out,0,size);
//							} catch (IOException e) {
//								e.printStackTrace();
//							}
//						}
					}
					codec.releaseOutputBuffer(outputBufferIndex, false);
				}
			}
			return size;
		}
		return 0;
	}

	/**
	 * {@link Decoder#decodeAndRender(byte[], int, int)}
	 */
	@SuppressWarnings("deprecation")
	@Override
	public void decodeAndRender(byte[] in, int offset, int length) {
		if(codec != null) {
			if (DEBUG) {
				Log.i(TAG, "decodeAndRender");
			}

			int inputBufferIndex = codec.dequeueInputBuffer(-1);
			if (inputBufferIndex >= 0) {
				ByteBuffer inputBuffer = codec.getInputBuffer(inputBufferIndex);
				inputBuffer.clear();
				inputBuffer.put(in, offset, length);
				codec.queueInputBuffer(inputBufferIndex, 0, length, 0, 0);
			}

			MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
			int outputBufferIndex = codec.dequeueOutputBuffer(bufferInfo, 0);
			if (outputBufferIndex >= 0) {
				codec.releaseOutputBuffer(outputBufferIndex, true);
			}
		}
	}


	@Override
	public Object getConfig(String key) {
		if (MediaFormat.KEY_COLOR_FORMAT.equals(key)) {
			return colorFormat;
		} else if (MediaFormat.KEY_MIME.equals(key)) {
			return mime;
		}
		return super.getConfig(key);
	}
	
}
