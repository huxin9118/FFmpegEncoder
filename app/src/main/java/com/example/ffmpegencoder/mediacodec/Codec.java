package com.example.ffmpegencoder.mediacodec;

/**
 * 编解码接口
 * @author chenyang
 *
 */
public interface Codec {
	
	/**
	 * 错误码:没有错误
	 */
	public static final int ERROR_CODE_NO_ERROR = 0x00;
	
	/**
	 * 错误码:放入输入缓存失败
	 */
	public static final int ERROR_CODE_INPUT_BUFFER_FAILURE = 0x01;
	
	/**
	 * 错误码:输出参数为NULL
	 */
	public static final int ERROR_CODE_OUT_BUF_NULL = 0x02;

	/**
	 * 错误码:输出参数溢出
	 */
	public static final int ERROR_CODE_OUT_BUF_FLOW = 0x03;
	
	/**
	 * 错误码:解码器没有打开
	 */
	public static final int ERROR_CODE_CODEC_NOT_OPEN = 0x04;
	
	/**
	 * 打开解码器
	 */
	public void open();

	/**
	 * 关闭解码器
	 */
	public void close();

	/**
	 * 刷新解码器
	 */
	public void flush();

	/**
	 * 配置解码器
	 * @param key 关键字
	 * @param value 值
	 */
	public void config(String key, Object value);
	
	/**
	 * 获取配置
	 * @param key 关键字
	 * @return 值
	 */
	public Object getConfig(String key);
	
	/**
	 * 返回错误
	 * @return 错误码
	 */
	public int getErrorCode();
}
