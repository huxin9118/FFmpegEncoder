package com.example.ffmpegencoder.mediacodec;

import android.media.MediaFormat;

public abstract class H264Encoder implements Encoder {

	/**
	 * 日志标签
	 */
	public static final String TAG = HH264Encoder.class.getSimpleName();

	/**
	 * 默认配置参数:宽度
	 */
	public static final int DEFAULT_CONFIG_WIDTH = 640;

	/**
	 * 默认配置参数:高度
	 */
	public static final int DEFAULT_CONFIG_HEIGHT = 480;
	/**
	 * 视频宽度
	 */
	protected int width = DEFAULT_CONFIG_WIDTH;
	/**
	 * 视频高度
	 */
	protected int height = DEFAULT_CONFIG_HEIGHT;
	/**
	 * 帧率
	 */
	protected int framerate;
	/**
	 * 码流
	 */
	protected int bitrate;
	/**
	 * 颜色格式
	 */
	protected int colorFormat;

	protected int profile;
	protected int level;

	/**
	 * 错误码
	 */
	protected int errorCode = ERROR_CODE_NO_ERROR;
	
	/**
	 * 构造方法
	 */
	public H264Encoder() {
		super();
	}

	public H264Encoder(int width, int height, int framerate, int bitrate) {
		super();
		this.width = width;
		this.height = height;
		this.framerate = framerate;
		this.bitrate = bitrate;
	}


	@Override
	public void config(String key, Object value) {
		if (MediaFormat.KEY_WIDTH.equals(key)) {
			setWidth((Integer) value);
		} else if (MediaFormat.KEY_HEIGHT.equals(key)) {
			setHeight((Integer) value);
		} else if (MediaFormat.KEY_BIT_RATE.equals(key)) {
			setBitrate((Integer) value);
		} else if (MediaFormat.KEY_FRAME_RATE.equals(key)) {
			setFramerate((Integer) value);
		} 
	}

	@Override
	public Object getConfig(String key) {
		if (MediaFormat.KEY_WIDTH.equals(key)) {
			return getWidth();
		} else if (MediaFormat.KEY_HEIGHT.equals(key)) {
			return getHeight();
		} else if (MediaFormat.KEY_BIT_RATE.equals(key)) {
			return getBitrate();
		} else if (MediaFormat.KEY_FRAME_RATE.equals(key)) {
			return getFramerate();
		}

		return null;
	}

	/**
	 * @see #width
	 * @return the width
	 */
	public int getWidth() {
		return width;
	}

	/**
	 * @see #width
	 * @param width
	 *            the width to set
	 */
	public void setWidth(int width) {
		this.width = width;
	}

	/**
	 * @see #height
	 * @return the height
	 */
	public int getHeight() {
		return height;
	}

	/**
	 * @see #height
	 * @param height
	 *            the height to set
	 */
	public void setHeight(int height) {
		this.height = height;
	}

	/**
	 * @see #framerate
	 * @return the framerate
	 */
	public int getFramerate() {
		return framerate;
	}

	/**
	 * @see #framerate
	 * @param framerate
	 *            the framerate to set
	 */
	public void setFramerate(int framerate) {
		this.framerate = framerate;
	}

	/**
	 * @see #bitrate
	 * @return the bitrate
	 */
	public int getBitrate() {
		return bitrate;
	}

	/**
	 * @see #bitrate
	 * @param bitrate
	 *            the bitrate to set
	 */
	public void setBitrate(int bitrate) {
		this.bitrate = bitrate;
	}


	@Override
	public int getErrorCode() {
		return errorCode;
	}
}
