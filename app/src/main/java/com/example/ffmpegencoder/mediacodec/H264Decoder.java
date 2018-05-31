package com.example.ffmpegencoder.mediacodec;


/**
 * H264解码器父类，实现H264 RTP解包和组帧功能
 * 
 * @author chenyang
 * 
 */
public abstract class H264Decoder implements Decoder {
	/**
	 * 关键字:宽度
	 */
	public static final String KEY_CONFIG_WIDTH = "width";
	/**
	 * 关键字:高度
	 */
	public static final String KEY_CONFIG_HEIGHT = "height";

	/**
	 * 默认配置参数:宽度
	 */
	public static final int DEFAULT_CONFIG_WIDTH = 640;

	/**
	 * 默认配置参数:高度
	 */
	public static final int DEFAULT_CONFIG_HEIGHT = 480;

	/**
	 * 宽度
	 */
	protected int width = DEFAULT_CONFIG_WIDTH;

	/**
	 * 高度
	 */
	protected int height = DEFAULT_CONFIG_HEIGHT;

	protected int crop_left;
	protected int crop_right;
	protected int crop_top;
	protected int crop_bottom;
	
	/**
	 * 错误码
	 */
	protected int errorCode = ERROR_CODE_NO_ERROR;

	/**
	 * 默认构造方法
	 */
	public H264Decoder() {
		super();
	}

	/**
	 * 如果视频SPS和PPS为初始参数，则不支持分辨率变更
	 * 
	 * @param width
	 *            视频宽度
	 * @param height
	 *            视频高度
	 */
	public H264Decoder(int width, int height) {
		this.width = width;
		this.height = height;
	}

	/**
	 * (non-Javadoc)
	 * 
	 * @see com.example.ffmpegdecoder.mediacodec.Codec#config(String,
	 *      Object)
	 */
	@Override
	public void config(String key, Object value) {
		if (KEY_CONFIG_WIDTH.equals(key)) {
			width = (Integer) value;
		} else if (KEY_CONFIG_HEIGHT.equals(key)) {
			height = (Integer) value;
		}
	}

	/**
	 * (non-Javadoc)
	 *
	 * @see com.example.ffmpegdecoder.mediacodec.Codec#getConfig(String)
	 */
	@Override
	public Object getConfig(String key) {
		if (KEY_CONFIG_WIDTH.equals(key)) {
			return crop_right - crop_left + 1 < width ? crop_right - crop_left + 1 : width;
		} else if (KEY_CONFIG_HEIGHT.equals(key)) {
			return crop_bottom - crop_top + 1 < height ? crop_bottom - crop_top + 1 : height;
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


	/** (non-Javadoc)
	 * @see com.example.ffmpegdecoder.mediacodec.Codec#getErrorCode()
	 */
	@Override
	public int getErrorCode() {
		return errorCode;
	}
}
