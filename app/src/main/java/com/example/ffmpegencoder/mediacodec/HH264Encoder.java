package com.example.ffmpegencoder.mediacodec;

import android.annotation.SuppressLint;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaFormat;
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
	public static final long TIME_OUT = 100;

	/**
	 * MediaCodec接口
	 */
	private MediaCodec mediaCodec;

	/**
	 * 编码器是否开启
	 */
	private boolean open = false;
	/**
	 * 
	 */
	private byte[] yuv420 = null;
	/**
	 * 帧索引
	 */
	private int frameIndex = 0;
	/**
	 * 第一个I帧是否编码出
	 */
	private boolean firstI = false;
	/**
	 * 输出数据缓存
	 */
	private byte[] outDataCache;
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
	 * {@link Encoder#open()}
	 */
	@Override
	public void open() {
		yuv420 = new byte[mWidth * mHeight * 3 / 2];

		try {
			mediaCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC);
		} catch (Exception e) {
			throw new RuntimeException(e);
		}
		MediaFormat mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, mWidth, mHeight);
		mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, bitrate);
		mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, framerate);
		mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, colorFormat);
		mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1); // 关键帧间隔时间
																		// 单位s

		mediaCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
		mediaCodec.start();
		firstI = false;
		open = true;
		frameIndex = 0;

		if (DEBUG) {
			Log.d(TAG, "encoder open colorFormat " + colorFormat);
		}
	}

	/**
	 * {@link Encoder#close()}
	 */
	@Override
	public void close() {
		try {
			mediaCodec.stop();
		} catch (Exception e) {
		}

		try {
			mediaCodec.release();
		} catch (Exception e) {
		}
		open = false;
		firstI = false;

		if (DEBUG) {
			Log.d(TAG, "encoder close");
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
			Log.d(TAG, "encode");
		}
		
		errorCode = ERROR_CODE_NO_ERROR;
		
		if (!open) {
			errorCode = ERROR_CODE_CODEC_NOT_OPEN;
			return 0;
		}

		int pos = 0;
		byte[] inBuf = in;
		int l = length;

		Log.d(TAG, "input colorFormat:"+colorFormat);
		if (colorFormat == MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar) {
			inBuf = yuv420;
			l = yuv420.length;
			H264Utils.I420toYUV420SemiPlanar(in, offset, yuv420, mWidth, mHeight);
		} else if (colorFormat == MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar) {
			inBuf = yuv420;
			l = yuv420.length;
			System.arraycopy(in, offset, yuv420, 0, yuv420.length);
		}

		try {
			ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
			ByteBuffer[] outputBuffers = mediaCodec.getOutputBuffers();
			int inputBufferIndex = mediaCodec.dequeueInputBuffer(TIME_OUT);
			if (inputBufferIndex >= 0) {
				ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
				inputBuffer.clear();
				inputBuffer.put(inBuf, offset, l);
				mediaCodec.queueInputBuffer(inputBufferIndex, 0, l, computePresentationTime(frameIndex++), 0);
			} else {
				errorCode = ERROR_CODE_INPUT_BUFFER_FAILURE;
			}

	
			MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
			int outputBufferIndex = 0;

			while (outputBufferIndex != MediaCodec.INFO_TRY_AGAIN_LATER) {
				outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);

				if (DEBUG) {
					Log.d(TAG, outputBufferIndex + " outputBufferIndex");
				}

				if (outputBufferIndex == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
					outputBuffers = mediaCodec.getOutputBuffers();
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
					MediaFormat mediaFormat = mediaCodec.getOutputFormat();

					if (DEBUG) {
						Log.d(TAG, "mediaFormat : " + mediaFormat.toString());
					}
				} else if (outputBufferIndex >= 0) {
					ByteBuffer outputBuffer = outputBuffers[outputBufferIndex];

					if (DEBUG) {
						Log.d(TAG, outputBufferIndex + " outputBufferIndex");
						Log.d(TAG, bufferInfo.size + " bufferInfo.size");
					}
					
					if (DEBUG) {
						Log.d(TAG, outputBuffer.remaining() + " outputBuffer.remaining");
					}

					if (outDataCache == null || outDataCache.length != bufferInfo.size) {
						outDataCache = new byte[bufferInfo.size];
					}

					if (DEBUG) {
						Log.d(TAG, outDataCache.length + " outDataCache.length");
					}

					outputBuffer.get(outDataCache);

					if (sps == null || pps == null) {
						// 保存pps sps 只有开始时 第一个帧里有，
						// 保存起来后面用
						offset = 0;

						while (offset < bufferInfo.size && (sps == null || pps == null)) {
							int count = 0;

							while (outDataCache[offset] == 0) {
								offset++;
							}

							count = H264Utils.ffAvcFindStartcode(outDataCache, offset, bufferInfo.size);
							offset++;

							int naluLength = count - offset;
							int type = outDataCache[offset] & 0x1F;
							Log.i(TAG, "count: "+count+"  offset: "+offset);
							if (type == 7) {
								sps = new byte[naluLength + 4];
								System.arraycopy(startCode, 0, sps, 0, startCode.length);
								System.arraycopy(outDataCache, offset, sps, 4, naluLength);
							} else if (type == 8) {
								pps = new byte[naluLength + 4];
								System.arraycopy(startCode, 0, pps, 0, startCode.length);
								System.arraycopy(outDataCache, offset, pps, 4, naluLength);
							} else if (type == 5) {
								firstI = true;
							}
							offset += naluLength;
						}
					} else {
						// 保存pps sps 只有开始时 第一个帧里有， 保存起来后面用
						if (firstI) {
							System.arraycopy(outDataCache, 0, out, pos, bufferInfo.size);
							pos += bufferInfo.size;
						} else {
							offset = 0;

							Loop: while (offset < bufferInfo.size && !firstI) {
								int count = 0;

								while (outDataCache[offset] == 0) {
									offset++;

									if (offset >= bufferInfo.size) {
										break Loop;
									}
								}

								count = H264Utils.ffAvcFindStartcode(outDataCache, offset, bufferInfo.size);
								offset++;

								if (offset >= bufferInfo.size) {
									break Loop;
								}

								int naluLength = count - offset;
								int type = outDataCache[offset] & 0x1F;

								if (type == 5) {
									firstI = true;
								}
								offset += naluLength;
							}

							if (firstI) {
								System.arraycopy(outDataCache, 0, out, pos, bufferInfo.size);
								pos += bufferInfo.size;
							}
						}

						mediaCodec.releaseOutputBuffer(outputBufferIndex, false);
					}

					// System.out.println("out[4] : " + (out[4] & 0x1f));
					// key frame 编码器生成关键帧时只有 00 00 00 01 65
					// 没有pps sps,要加上
					if ((out[4] & 0x1f) == 5) {
						int spsPpslength = sps.length + pps.length;
						System.arraycopy(out, 0, yuv420, 0, pos);
						System.arraycopy(sps, 0, out, 0, sps.length);
						System.arraycopy(pps, 0, out, sps.length, pps.length);
						System.arraycopy(yuv420, 0, out, spsPpslength, pos);
						pos += spsPpslength;
					}
				}
			}
		} catch (Throwable t) {
			t.printStackTrace();
		}

		if (!firstI) {
			pos = 0;
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
				Log.d(TAG, "colorFormats:" + c);
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

}
