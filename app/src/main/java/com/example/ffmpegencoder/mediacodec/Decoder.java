package com.example.ffmpegencoder.mediacodec;

/**
 * 解码接口
 * @author chenyang
 *
 */
public interface Decoder extends Codec {

	/**
	 * 解码方法
	 * @param in 待解码数组
	 * @param offset 待解码数组偏移量
	 * @param out 解码后数组
	 * @param length 待解码长度
	 * @return 解码后长度
	 */
	public int decode(byte[] in, int offset, byte[] out, int length);

	/**
	 * 解码后直接回放，无需返回数据
	 * @param in 待解码数组
	 * @param offset 待解码数组偏移量
	 * @param length 待解码长度
	 */
	public void decodeAndRender(byte[] in, int offset, int length);
}
