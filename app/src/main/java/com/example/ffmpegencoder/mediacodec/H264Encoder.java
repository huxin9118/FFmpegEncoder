package com.example.ffmpegencoder.mediacodec;

/**
 * @author c22188
 *
 */
public abstract class H264Encoder implements Encoder {

	/**
	 * 日志标签
	 */
	public static final String TAG = HH264Encoder.class.getSimpleName();
	/**
	 * 关键字:宽度
	 */
	public static final String KEY_CONFIG_WIDTH = "width";
	/**
	 * 关键字:高度
	 */
	public static final String KEY_CONFIG_HEIGHT = "height";
	/**
	 * 关键字:帧率
	 */
	public static final String KEY_CONFIG_FRAMERATE = "framerate";
	/**
	 * 关键字:码流
	 */
	public static final String KEY_CONFIG_BITRATE = "bitrate";
	/**
	 * 关键字:颜色格式
	 */
	public static final String KEY_CONFIG_COLORFORMAT = "colorFormat";

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
	protected int mWidth = DEFAULT_CONFIG_WIDTH;
	/**
	 * 视频高度
	 */
	protected int mHeight = DEFAULT_CONFIG_HEIGHT;
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

	/**
	 * 构造方法
	 * 
	 * @param width
	 * @param height
	 * @param framerate
	 * @param bitrate
	 * @param colorFormat
	 */
	public H264Encoder(int width, int height, int framerate, int bitrate) {
		super();
		this.mWidth = width;
		this.mHeight = height;
		this.framerate = framerate;
		this.bitrate = bitrate;
	}

	/**
	 * (non-Javadoc)
	 * 
	 * @see com.hytera.media.codec.Codec#config(String,
	 *      Object)
	 */
	@Override
	public void config(String key, Object value) {
		if (KEY_CONFIG_WIDTH.equals(key)) {
			setWidth((Integer) value);
		} else if (KEY_CONFIG_HEIGHT.equals(key)) {
			setHeight((Integer) value);
		} else if (KEY_CONFIG_BITRATE.equals(key)) {
			setBitrate((Integer) value);
		} else if (KEY_CONFIG_FRAMERATE.equals(key)) {
			setFramerate((Integer) value);
		} 
	}

	/**
	 * (non-Javadoc)
	 * 
	 * @see com.hytera.media.codec.Codec#getConfig(String)
	 */
	@Override
	public Object getConfig(String key) {
		if (KEY_CONFIG_WIDTH.equals(key)) {
			return getWidth();
		} else if (KEY_CONFIG_HEIGHT.equals(key)) {
			return getHeight();
		} else if (KEY_CONFIG_BITRATE.equals(key)) {
			return getBitrate();
		} else if (KEY_CONFIG_FRAMERATE.equals(key)) {
			return getFramerate();
		}

		return null;
	}

	/**
	 * @see #mWidth
	 * @return the mWidth
	 */
	public int getWidth() {
		return mWidth;
	}

	/**
	 * @see #mWidth
	 * @param width
	 *            the mWidth to set
	 */
	public void setWidth(int width) {
		this.mWidth = width;
	}

	/**
	 * @see #mHeight
	 * @return the mHeight
	 */
	public int getHeight() {
		return mHeight;
	}

	/**
	 * @see #mHeight
	 * @param height
	 *            the mHeight to set
	 */
	public void setHeight(int height) {
		this.mHeight = height;
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


	/** (non-Javadoc)
	 * @see com.hytera.media.codec.Codec#getErrorCode()
	 */
	@Override
	public int getErrorCode() {
		return errorCode;
	}
}
