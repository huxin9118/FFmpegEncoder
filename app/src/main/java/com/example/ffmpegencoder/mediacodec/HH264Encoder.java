package com.example.ffmpegencoder.mediacodec;

import android.annotation.SuppressLint;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.os.Bundle;
import android.util.Log;

import java.nio.ByteBuffer;

/**
 * Android 4.1.2以上系统提供MediaCodec接口，可以对H264进行编解码
 * 
 * @author chenyang
 * 
 */
@SuppressLint("NewApi")
public class HH264Encoder extends H264Encoder {

	/**
	 * 日志标签
	 */
	public static final String TAG = HH264Encoder.class.getSimpleName();

	/**
	 * 调试日志开关
	 */
	public static final boolean DEBUG = true;

	/**
	 * 缓存获取超时时间
	 */
	public static final long TIME_OUT = 1000;

	/**
	 * MediaCodec接口
	 */
	private MediaCodec codec;

	/**
	 * 帧索引
	 */
	private int frameIndex = 0;

	/**
	 * 第一个I帧是否编码出
	 */
	private boolean firstI = false;

	private byte[] inData;
	private byte[] outData;

	private final MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
	private final Bundle params = new Bundle();
	private int h264_parser;
	private int bitrate_delay;
	private int current_bitrate;

	/**
	 * SPS缓存
	 */
	byte[] sps = null;
	/**
	 * PPS缓存
	 */
	byte[] pps = null;
	/**
	 * 起始码 { 0x00, 0x00, 0x00, 0x01 }
	 */
	private byte[] startCode = { 0x00, 0x00, 0x00, 0x01 };

	/**
	 * 构造方法
	 * 
	 * @param width
	 *            视频宽度
	 * @param height
	 *            视频高度
	 * @param framerate
	 *            帧率
	 * @param bitrate
	 *            码流
	 */
	public HH264Encoder(int width, int height, int framerate, int bitrate) {
		super(width, height, framerate, bitrate);
		colorFormat = getFinalSupportColorFormat();
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
	 * {@link Encoder#open()}
	 */
	@Override
	public void open() {
		if(codec != null) {
			close();
		}
		try {
			codec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC);
			MediaFormat mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, width, height);
			mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, bitrate);
			mediaFormat.setInteger("max-bitrate", bitrate * 2);
			mediaFormat.setInteger(MediaFormat.KEY_BITRATE_MODE, 1);
			mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, framerate);
			mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, colorFormat);
			if (DEBUG) {
				Log.i(TAG, "encoder open colorFormat " + colorFormat);
			}
			mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1); // 关键帧间隔时间,单位s

			profile = 0x08;//AVCProfileHigh
			level = 0x100;//AVCLevel30
			if(width * height >= 1280 * 720){
				level = 0x200;//AVCLevel31
			}
			if(width * height >= 1920 * 1080){
				level = 0x800;//AVCLevel40
			}
			mediaFormat.setInteger(MediaFormat.KEY_PROFILE, profile);
			mediaFormat.setInteger(MediaFormat.KEY_LEVEL, level);

			codec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
			codec.start();

			h264_parser = allocH264Parser();
			current_bitrate = bitrate;
		} catch (Exception e) {
			throw new RuntimeException(e);
		}
		firstI = false;
		frameIndex = 0;
	}

	/**
	 * {@link Encoder#close()}
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
				freeH264Parser(h264_parser);
				codec = null;
			}
		}
	}

	/**
	 * {@link Encoder#encode(byte[], int, byte[], int)}
	 * 
	 */
	@SuppressWarnings("deprecation")
	@Override
	public int encode(byte[] in, int offset, byte[] out, int length) {
		if (DEBUG) {
			Log.i(TAG, "encode");
		}

		int pos = 0;
		errorCode = ERROR_CODE_NO_ERROR;

		if (out == null) {
			errorCode = ERROR_CODE_OUT_BUF_NULL;
			return 0;
		}

		int inputBufferIndex = codec.dequeueInputBuffer(-1);
		if (DEBUG) {
			Log.i(TAG, "inputBufferIndex : " + inputBufferIndex);
		}
		if (inData == null) {
			inData = new byte[length];
		}

		if (inputBufferIndex >= 0) {
			ByteBuffer inputBuffer = codec.getInputBuffer(inputBufferIndex);
			if(colorFormat == MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar) {
				System.arraycopy(in, offset, inData, 0, length);
			}else {
				H264Utils.I420toYUV420SemiPlanar(in, offset, inData, width, height);
			}
			inputBuffer.clear();
			inputBuffer.put(inData, offset, length);
			codec.queueInputBuffer(inputBufferIndex, 0, length, computePresentationTime(frameIndex++), 0);
		} else {
			errorCode = ERROR_CODE_INPUT_BUFFER_FAILURE;
		}

		int outputBufferIndex = codec.dequeueOutputBuffer(bufferInfo, TIME_OUT);
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
				 *	}
				 *	</code>
				 */
				MediaFormat mediaFormat = codec.getOutputFormat();

				if (DEBUG) {
					Log.i(TAG, "mediaFormat : " + mediaFormat.toString());
				}
			} else if (outputBufferIndex >= 0) {
				if (outData == null || outData.length != bufferInfo.size) {
					outData = new byte[bufferInfo.size];
				}
				if (DEBUG) {
					Log.i(TAG, "bufferInfo.size=" + bufferInfo.size + " bufferInfo.offset=" + bufferInfo.offset);
				}
				ByteBuffer outputBuffer = codec.getOutputBuffer(outputBufferIndex);
				outputBuffer.position(bufferInfo.offset);
				outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
				outputBuffer.get(outData, 0, bufferInfo.size);

				if (sps == null || pps == null) {
					// 保存pps sps 只有开始时 第一个帧里有， 保存起来后面用
					offset = 0;

					while (offset < bufferInfo.size && (sps == null || pps == null)) {
						while (outData[offset] == 0) {
							offset++;
						}
						int count = H264Utils.ffAvcFindStartcode(outData, offset, bufferInfo.size);
						offset++;

						int naluLength = count - offset;
						int type = outData[offset] & 0x1F;

						if (type == 7) {
							sps = new byte[naluLength + startCode.length];
							System.arraycopy(startCode, 0, sps, 0, startCode.length);
							System.arraycopy(outData, offset, sps, startCode.length, naluLength);
							setSPS(h264_parser, sps, startCode.length, sps.length);
						} else if (type == 8) {
							pps = new byte[naluLength + startCode.length];
							System.arraycopy(startCode, 0, pps, 0, startCode.length);
							System.arraycopy(outData, offset, pps, startCode.length, naluLength);
							setPPS(h264_parser, pps, startCode.length, pps.length);
						} else if (type == 5) {
							firstI = true;
						}
						offset += naluLength;
					}
				} else {
					// 保存pps sps 只有开始时 第一个帧里有， 保存起来后面用
					if (!firstI) {
						offset = 0;
						while (offset < bufferInfo.size && !firstI) {
							while (outData[offset] == 0) {
								offset++;

								if (offset >= bufferInfo.size) {
									break;
								}
							}

							if (offset >= bufferInfo.size) {
								break;
							}

							offset++;
							int count = H264Utils.ffAvcFindStartcode(outData, offset, bufferInfo.size);

							if (offset >= bufferInfo.size) {
								break;
							}

							int naluLength = count - offset;
							int type = outData[offset] & 0x1F;

							if (type == 5) {
								firstI = true;
							}
							offset += naluLength;
						}
					}
				}


				if(false) {
					if (((outData[startCode.length] & 0x1f) == 1) && frameIndex%1==0) {
						int sliceQPY = getSliceQPY(h264_parser, outData, startCode.length, bufferInfo.size);
						if(false) {
							if (sliceQPY < 40) {
								current_bitrate = bitrate;
							}
							if (40 <= sliceQPY && sliceQPY < 42) {
								current_bitrate = bitrate + 200000;
							}
							if (42 <= sliceQPY && sliceQPY < 44) {
								current_bitrate = bitrate + 500000;
							}
							if (44 <= sliceQPY && sliceQPY < 46) {
								current_bitrate = bitrate + 900000;
							}
							if (46 <= sliceQPY && sliceQPY < 48) {
								current_bitrate = bitrate + 1400000;
							}
							if (48 <= sliceQPY && sliceQPY < 50) {
								current_bitrate = bitrate + 2000000;
							}
							if (50 <= sliceQPY && sliceQPY < 51) {
								current_bitrate = bitrate + 2700000;
							}
							if (51 <= sliceQPY && sliceQPY < 52) {
								current_bitrate = bitrate + 3500000;
							}

							params.clear();
							params.putInt(MediaCodec.PARAMETER_KEY_VIDEO_BITRATE, current_bitrate);
							codec.setParameters(params);
						}else{
							if(bitrate_delay == 0) {
								if (39 <= sliceQPY) {
									current_bitrate += 100000;
									bitrate_delay = 5;
								}
								if (current_bitrate > bitrate * 2) {
									current_bitrate = bitrate * 2;
								}
								if (sliceQPY <= 35) {
									current_bitrate -= 200000;
									bitrate_delay = 8;
								}
								if (current_bitrate < bitrate) {
									current_bitrate = bitrate;
								}
							}else{
								bitrate_delay--;
							}
							params.clear();
							params.putInt(MediaCodec.PARAMETER_KEY_VIDEO_BITRATE, current_bitrate);
							codec.setParameters(params);
						}

						if (DEBUG) {
							Log.i(TAG, "sliceQPY=" + sliceQPY + " frameIndex=" + frameIndex + " current_bitrate=" + current_bitrate);
						}
					}
				}else{
					if (((outData[startCode.length] & 0x1f) == 1) && frameIndex%1==0) {
						int sliceQPY = getSliceQPY(h264_parser, outData, startCode.length, bufferInfo.size);
						if (DEBUG) {
							Log.i(TAG, "sliceQPY=" + sliceQPY);
						}
					}
				}

				// System.out.println("out[startCode.length] : " + (out[startCode.length] & 0x1f));
				// key frame 编码器生成关键帧时只有 00 00 00 01 65
				// 没有pps sps,要加上
				if ((outData[startCode.length] & 0x1f) == 5) {
					if(sps != null){
						System.arraycopy(sps, 0, out, pos, sps.length);
						pos += sps.length;
					}
					if(pps != null){
						System.arraycopy(pps, 0, out, pos, pps.length);
						pos += pps.length;
					}
					System.arraycopy(outData, 0, out, pos, bufferInfo.size);
					pos += bufferInfo.size;
				}else{
					System.arraycopy(outData, 0, out, pos, bufferInfo.size);
					pos += bufferInfo.size;
				}

				if (!firstI) {
					pos = 0;
				}

				codec.releaseOutputBuffer(outputBufferIndex, false);
			}
		}
		return pos;
	}

	/**
	 * 获取最终的颜色模式
	 * 
	 * @return
	 */
	@SuppressWarnings("deprecation")
	public int getFinalSupportColorFormat() {
		int[] colorFormats = getSupportColorFormat();

		if (colorFormats == null || colorFormats.length <= 0) {
			return MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar;
		} else {
			for (int c : colorFormats) {
				Log.i(TAG, "colorFormats:" + c);
				if (c == MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar) {
					return c;
				}
			}

			for (int c : colorFormats) {
				if (c == MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar) {
					return c;
				}
			}

			return colorFormats[0];
		}
	}

	/**
	 * 获取支持的颜色模式列表
	 * 
	 * @return
	 */
	@SuppressWarnings("deprecation")
	public int[] getSupportColorFormat() {
		int count = MediaCodecList.getCodecCount();

		for (int i = 0; i < count; i++) {
			MediaCodecInfo info = MediaCodecList.getCodecInfoAt(i);

			if (info.isEncoder()) {
				try {
					return info.getCapabilitiesForType(MediaFormat.MIMETYPE_VIDEO_AVC).colorFormats;
				} catch (Exception e) {
					continue;
				}
			}
		}

		return null;
	}
	
	/**
	 * Generates the presentation time for frame N, in microseconds.
	 */
	private long computePresentationTime(long frameIndex) {
		return 132l + frameIndex * 1000000l / (long) framerate;
	}

	static {
		System.loadLibrary("h264_parser");
	}

	native int allocH264Parser();
	native void freeH264Parser(int point);
	native void setSPS(int point, byte[] sps, int pos, int length);
	native void setPPS(int point, byte[] pps, int pos, int length);
	native int getSliceQPY(int point, byte[] slice, int pos, int length);
}
