package com.example.ffmpegencoder.activity;

import android.annotation.TargetApi;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.app.Activity;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import com.example.ffmpegencoder.R;

import java.io.File;
import java.lang.ref.WeakReference;

public class MainActivity extends Activity {

    private final String PERMISSION_WRITE_EXTERNAL_STORAGE= "android.permission.WRITE_EXTERNAL_STORAGE";
    private final int PERMISSION_REQUESTCODE = 0;
    private final int GET_CONTENT_REQUESTCODE = 0;
    private final int OUTPUT_URL_H264_SUBSTRING_BEGIN_INDEX = 7;
    private final String TAG = this.getClass().getSimpleName();
    private Toast mToast;
    private boolean isShow;

    private int yuv_pixel_type = -1;
    private int yuv_pixel_w = -1;
    private int yuv_pixel_h = -1;
    private int yuv_fps = -1;
    private int yuv_bitrate = -1;
    private int yuv_codec_type = 0;
    private int frameSum;
    static boolean isSetPixel = false;
    static boolean isSetPixelWandH = false;

    private LinearLayout layout_h264;
    private ImageView img_encode;
    private RelativeLayout btn_encode;
    private ProgressBar progressBar;

    private RadioGroup encode_method;
    private RadioButton ffmpeg;
    private RadioButton mediacodec;

    private RadioGroup pixel_format;
    private RadioButton i420;
    private RadioButton nv12;
    private RadioButton nv21;

    private TextView bitrate;
    private SeekBar bitrateBar;
    private final int max_bitrate = 10000;
    private final int min_bitrate = 100;
    private TextView framerate;
    private SeekBar framerateBar;
    private final int max_framerate = 60;
    private final int min_framerate = 10;
    private int max_kbps = 0;
    private int min_kbps = 0;

    private String input_url;
    private TextView text_input;
    private EditText text_output;
    private EditText text_width;
    private EditText text_height;
    private TextView text_frameCount;

    private boolean isSelect = false;
    private Handler progressRateHandler = new progressRateHandler(this);
    private Thread encodeThread;

    long time_start;
    long time_end;
    int frame_count;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        layout_h264 = (LinearLayout) this.findViewById(R.id.ly_h264);
        layout_h264.setAlpha(0.5f);
        btn_encode = (RelativeLayout) this.findViewById(R.id.ly_decode);
        img_encode = (ImageView) this.findViewById(R.id.btn_decode);
        btn_encode.setClickable(false);
        toggleBtnDecode(false);

        progressBar = (ProgressBar) this.findViewById(R.id.progressBar);
        progressBar.setVisibility(View.INVISIBLE);
        text_input = (TextView) this.findViewById(R.id.input_url);
        text_output = (EditText) this.findViewById(R.id.output_url);
        text_output.setEnabled(false);
        text_width = (EditText) this.findViewById(R.id.width);
        text_height = (EditText) this.findViewById(R.id.height);
        text_width.setEnabled(false);
        text_height.setEnabled(false);
        text_frameCount = (TextView) this.findViewById(R.id.frameCount);

        text_input.setOnClickListener(new OnClickListener() {
            @TargetApi(Build.VERSION_CODES.M)
            public void onClick(View arg0) {
                if(checkSelfPermission(PERMISSION_WRITE_EXTERNAL_STORAGE)!= PackageManager.PERMISSION_GRANTED){
                    requestPermissions(new String[]{PERMISSION_WRITE_EXTERNAL_STORAGE},PERMISSION_REQUESTCODE);
                }
                else{
                    Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                    intent.addCategory(Intent.CATEGORY_OPENABLE);
                    intent.setType("*/*");
                    startActivityForResult(Intent.createChooser(intent,"请选择文件管理器"),GET_CONTENT_REQUESTCODE);
                }
            }
        });

        btn_encode.setOnClickListener(new OnClickListener() {
            public void onClick(View arg0){
                if(isSelect) {
                    final String urltext_input = text_input.getText().toString();
                    final String urltext_output = text_output.getText().toString().substring(OUTPUT_URL_H264_SUBSTRING_BEGIN_INDEX);

                    Log.i("inputurl", urltext_input);
                    Log.i("outputurl", urltext_output);

                    progressBar.setVisibility(View.VISIBLE);
                    encodeThread = new Thread() {
                        @Override
                        public void run() {
                            Log.i(TAG, "yuv_pixel_w x yuv_pixel_h : "+ yuv_pixel_w +" x "+ yuv_pixel_h);
                            time_start = System.currentTimeMillis();
//                            if(yuv_codec_type == 1){
//                                MediaEncode(urltext_input, urltext_output, yuv_pixel_w, yuv_pixel_h);
//                                return;
//                            }
                            if(yuv_pixel_w != -1 && yuv_pixel_h != -1) {
                                encode(urltext_input, urltext_output, yuv_pixel_w, yuv_pixel_h,yuv_fps,yuv_bitrate,yuv_pixel_type,yuv_codec_type);
                            }
                            else{
                                encode(urltext_input, urltext_output, 640, 480,yuv_fps,yuv_bitrate,yuv_pixel_type,yuv_codec_type);//缺省配置
                            }
                        }
                    };
                    encodeThread.start();
                    btn_encode.setClickable(false);
                    toggleBtnDecode(false);
                }
                else{
                    showToast("请先选择YUV路径",Toast.LENGTH_SHORT);
                }
            }
        });

        encode_method = (RadioGroup) this.findViewById(R.id.encode_method);
        ffmpeg = (RadioButton) this.findViewById(R.id.ffmpeg);
        mediacodec = (RadioButton) this.findViewById(R.id.mediacodec);
        encode_method.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                Log.i(TAG, "onCheckedChanged: "+checkedId);
                if(checkedId == ffmpeg.getId()){
                    yuv_codec_type = 0;
                }
                else if(checkedId == mediacodec.getId()){
                    yuv_codec_type = 1;
                }
            }
        });

        pixel_format = (RadioGroup) this.findViewById(R.id.yuv_pixel_format);
        i420 = (RadioButton) this.findViewById(R.id.i420);
        nv12 = (RadioButton) this.findViewById(R.id.nv12);
        nv21 = (RadioButton) this.findViewById(R.id.nv21);
        pixel_format.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                if(checkedId == i420.getId()){
                    yuv_pixel_type = 0;
                }
                else if(checkedId == nv12.getId()){
                    yuv_pixel_type = 5;
                }
                else if(checkedId == nv21.getId()){
                    yuv_pixel_type = 6;
                }
            }
        });

        bitrate = (TextView)this.findViewById(R.id.bitrate);
        bitrateBar = (SeekBar)this.findViewById(R.id.bitrateBar);
        bitrateBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                yuv_bitrate = progress + min_kbps;
                bitrate.setText(yuv_bitrate+"");
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        bitrateBar.setEnabled(false);

        framerate = (TextView)this.findViewById(R.id.framerate);
        framerateBar = (SeekBar)this.findViewById(R.id.framerateBar);
        framerateBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                yuv_fps = progress + min_framerate;
                framerate.setText(yuv_fps+"");

                updateCodecParameter();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        framerateBar.setEnabled(false);

        text_width.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if(hasFocus){
                    if(yuv_pixel_w != -1) {
                        text_width.setText(yuv_pixel_w +"");
                    }
                    else{
                        text_width.setText("");
                    }
                    text_width.setSelection(text_width.getText().length());
                }
                else{
                    try {
                        yuv_pixel_w = Integer.parseInt(text_width.getText().toString());
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    if(yuv_pixel_w != -1) {
                        text_width.setText(text_width.getHint().toString() + " " + yuv_pixel_w + " px");
                    }
                    else{
                        text_width.setText(text_width.getHint().toString() + " " + " 640（缺省，请手动配置）");
                    }
                    text_width.setSelection(text_width.getText().length());

                    updateCodecParameter();
                }
            }
        });

        text_height.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if(hasFocus){
                    if(yuv_pixel_h != -1) {
                        text_height.setText(yuv_pixel_h +"");
                    }
                    else{
                        text_height.setText("");
                    }
                    text_height.setSelection(text_height.getText().length());
                }
                else{
                    try {
                        yuv_pixel_h = Integer.parseInt(text_height.getText().toString());
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    if(yuv_pixel_h != -1) {
                        text_height.setText(text_height.getHint().toString() + " " + yuv_pixel_h + " px");
                    }
                    else{
                        text_height.setText(text_height.getHint().toString() + " " + " 480（缺省，请手动配置）");
                    }
                    text_height.setSelection(text_height.getText().length());

                    updateCodecParameter();
                }
            }
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        switch (requestCode) {
            case PERMISSION_REQUESTCODE:
                if ("android.permission.WRITE_EXTERNAL_STORAGE".equals(permissions[0])
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                    intent.addCategory(Intent.CATEGORY_OPENABLE);
                    intent.setType("*/*");
                    startActivityForResult(Intent.createChooser(intent,"请选择文件管理器"),GET_CONTENT_REQUESTCODE);
                }
        }
    }

    boolean updatePixelData(String input_url){
        isSetPixelWandH = false;
        yuv_pixel_w = -1;
        yuv_pixel_h = -1;
        yuv_fps = -1;
        yuv_pixel_type = -1;
        int typeIndex = input_url.lastIndexOf("&");
        int fpsIndex = input_url.lastIndexOf("@");
        int _Index = input_url.lastIndexOf("_");
        int xIndex = input_url.lastIndexOf("x");
        int pointIndex = input_url.lastIndexOf(".");
        if (_Index < xIndex && xIndex < pointIndex) {
            try {
                yuv_pixel_w = Integer.parseInt(input_url.substring(_Index + 1, xIndex));
                yuv_pixel_h = Integer.parseInt(input_url.substring(xIndex + 1, pointIndex));
                isSetPixelWandH = true;
                if(typeIndex == -1 && fpsIndex == -1){
                    Log.i(TAG, "输入YUV格式信息为 W:"+yuv_pixel_w+" H:"+yuv_pixel_h);
                    return false;
                }
                if(typeIndex == -1) {
                    yuv_fps = Integer.parseInt(input_url.substring(fpsIndex + 1, _Index));
                    Log.i(TAG, "输入YUV格式信息为 W:"+yuv_pixel_w+" H:"+yuv_pixel_h+" fps:"+yuv_fps);
                    return false;
                }
                if(fpsIndex == -1){
                    yuv_pixel_type = Integer.parseInt(input_url.substring(typeIndex + 1, _Index));
                    Log.i(TAG, "输入YUV格式信息为 W:"+yuv_pixel_w+" H:"+yuv_pixel_h+" type:"+yuv_pixel_type);
                    return false;
                }
                if(typeIndex < fpsIndex && fpsIndex < _Index){
                    yuv_pixel_type = Integer.parseInt(input_url.substring(typeIndex + 1, fpsIndex));
                    yuv_fps = Integer.parseInt(input_url.substring(fpsIndex + 1, _Index));
                    Log.i(TAG, "输入YUV格式信息为 W:"+yuv_pixel_w+" H:"+yuv_pixel_h+" type:"+yuv_pixel_type+" fps:"+yuv_fps);
                    return true;
                }
            } catch (Exception e) {
                Log.i(TAG, "输入YUV格式信息解析错误");
                return false;
            }
        }
        Log.i(TAG, "输入YUV格式信息解析错误");
        return false;
    }

    void updateCodecParameter(){
        int width = yuv_pixel_w == -1 ?  640 : yuv_pixel_w;
        int height = yuv_pixel_h == -1 ?  480 : yuv_pixel_h;
        int fps = yuv_fps == -1 ?  25 : yuv_fps;
        int kbps = width * height * 3 / 2  * fps * 8 / 100 / 1000;
//        int fps = 25;
//        int kbps = 1024*3/2;

        File file = new File(input_url);
        frameSum = (int)( (file.length() * 2) / (width * height * 3));
        Log.i(TAG, "frameSum: "+ frameSum);
        text_frameCount.setText(text_frameCount.getHint().toString() + " " + frameSum);

        framerate.setText(fps+"");
        framerateBar.setMax(max_framerate - min_framerate);
        framerateBar.setProgress(fps - min_framerate);
        Log.i(TAG, "framerate: "+ fps);

        Log.i(TAG, "bitrate: "+ kbps);
        bitrate.setText(kbps+"");
        max_kbps = kbps * 5 < max_bitrate ? kbps * 5 : max_bitrate;
        min_kbps = kbps / 5 > min_bitrate ? kbps / 5 : min_bitrate;
        bitrateBar.setMax(max_kbps - min_kbps);
        bitrateBar.setProgress(kbps - min_kbps);
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(resultCode == Activity.RESULT_OK){
            switch (requestCode){
                case GET_CONTENT_REQUESTCODE:
                    Uri uri = data.getData();
                    if(".yuv".equals(data.getData().toString().substring(data.getData().toString().length()-4).toLowerCase())) {
                        input_url = uri.getPath();
                        if("/root".equals(input_url.substring(0,5))){
                            input_url = input_url.substring(5);
                        }
                        isSelect = true;
                        text_output.setEnabled(true);
                        text_width.setEnabled(true);
                        text_height.setEnabled(true);
                        bitrateBar.setEnabled(true);
                        framerateBar.setEnabled(true);
                        layout_h264.setAlpha(1f);
                        text_input.setText(input_url);

                        isSetPixel = updatePixelData(input_url);
                        if(isSetPixelWandH){
                            text_width.setText(text_width.getHint().toString() + " " + yuv_pixel_w + " px");
                            text_height.setText(text_height.getHint().toString() + " " + yuv_pixel_h + " px");

                            if(isSetPixel) {
                                text_output.setText("h264输出：" + input_url.substring(0, input_url.lastIndexOf("&")) + ".h264");
                                text_output.setSelection(text_output.getText().length());
                            }
                            else{
                                if(yuv_fps == -1 && yuv_pixel_type == -1) {
                                    text_output.setText("h264输出：" + input_url.substring(0, input_url.lastIndexOf("_")) + ".h264");
                                    text_output.setSelection(text_output.getText().length());
                                }
                                else {
                                    if (yuv_fps == -1) {
                                        text_output.setText("h264输出：" + input_url.substring(0, input_url.lastIndexOf("&")) + ".h264");
                                        text_output.setSelection(text_output.getText().length());
                                    }
                                    if (yuv_pixel_type == -1) {
                                        text_output.setText("h264输出：" + input_url.substring(0, input_url.lastIndexOf("@")) + ".h264");
                                        text_output.setSelection(text_output.getText().length());
                                    }
                                }
                            }
                        }
                        else{
                            text_width.setText(text_width.getHint().toString() + " " + " 640（缺省，请手动配置）");
                            text_height.setText(text_height.getHint().toString() + " " + " 480（缺省，请手动配置）");

                            text_output.setText("h264输出：" + input_url.substring(0, input_url.lastIndexOf(".")) + ".h264");
                            text_output.setSelection(text_output.getText().length());
                        }
                        updateCodecParameter();

                        progressBar.setMax(frameSum);
                        progressBar.setProgress(0);

                        text_width.setSelection(text_width.getText().length());
                        text_height.setSelection(text_height.getText().length());
                        if(yuv_pixel_type == -1 || yuv_pixel_type == 0) {
                            pixel_format.check(i420.getId());
                        }else if(yuv_pixel_type == 5){
                            pixel_format.check(nv12.getId());
                        }else if(yuv_pixel_type == 6){
                            pixel_format.check(nv21.getId());
                        }

                        btn_encode.setClickable(true);
                        toggleBtnDecode(true);
                    }
                    else{
                        showToast( "文件打开失败,该文件可能不是.yuv文件！", Toast.LENGTH_SHORT);
                    }
                    break;
            }
        }
    }

    static class progressRateHandler extends Handler {
        WeakReference<MainActivity> mActivityReference;
        progressRateHandler(MainActivity activity) {
            mActivityReference= new WeakReference(activity);
        }
        @Override
        public void handleMessage(Message msg) {
            final MainActivity activity = mActivityReference.get();
            if (activity != null) {
                if(msg.what == -1) {
                    activity.progressBar.setProgress(activity.progressBar.getMax());
                    activity.cancelToast();
                    activity.showToast("视频编码完成~~", Toast.LENGTH_SHORT);
                    activity.btn_encode.setClickable(true);
                    activity.toggleBtnDecode(true);
                }
                else if(msg.what == -2) {
                    activity.progressBar.setProgress(0);
                    activity.cancelToast();
                    activity.showToast("视频编码取消！！", Toast.LENGTH_SHORT);
                    activity.btn_encode.setClickable(true);
                    activity.toggleBtnDecode(true);
                }
                else {
                    if(activity.isShow) {
                        activity.showToast("正在编码第 " + msg.what + " 帧", Toast.LENGTH_SHORT);
                    }
                    activity.progressBar.setProgress(msg.what);
                }
            }
        }
    }


    //JNI
    public void setProgressRate(int progress){
        frame_count = progress;
        progressRateHandler.sendEmptyMessage(progress);
//		Log.i("++++++", progress+"");
    }

    public void setProgressRateFull(){
        time_end = System.currentTimeMillis();
        frame_count++;
        Log.i("frame_count", ""+frame_count);
        Log.i("time_duration", ""+((double)(time_end - time_start))/1000);
        Log.i("avg_frame_rate", ""+((double)(frame_count*1000))/((double)(time_end - time_start)));
        progressRateHandler.sendEmptyMessage(-1);
//		Log.i("+++---", -1+"");
    }

    public void setProgressRateEmpty(){
        progressRateHandler.sendEmptyMessage(-2);
//		Log.i("+++---", -2+"");
    }

    public native int encode(String inputurl, String outputurl,int wdith,int height,int fps,int bitrate,int pixel_type,int codec_type);
    public native int MediaEncode(String inputurl, String outputurl,int wdith,int height);
    public native void encodeCancel();

    static{
//        System.loadLibrary("avutil-54");
//        System.loadLibrary("swresample-1");
//        System.loadLibrary("avcodec-56");
//        System.loadLibrary("avformat-56");
//        System.loadLibrary("swscale-3");
//        System.loadLibrary("postproc-53");
//        System.loadLibrary("avfilter-5");
//        System.loadLibrary("avdevice-56");
        System.loadLibrary("ffmpegencoder");
        System.loadLibrary("ffmpeg");
    }

    void toggleBtnDecode(boolean isClickable){
        if(isClickable)
            img_encode.setImageResource(R.drawable.ic_export_send);
        else
            img_encode.setImageResource(R.drawable.ic_export_send_gray);
    }

    @Override
    protected void onResume() {
        super.onResume();
        isShow = true;
    }

    @Override
    protected void onPause() {
        super.onPause();
        isShow = false;
        cancelToast();
    }

    /**
     * 显示Toast，解决重复弹出问题
     */
    public void showToast(String text , int time) {
        if(mToast == null) {
            mToast = Toast.makeText(this, text, time);
        } else {
            mToast.setText(text);
            mToast.setDuration(Toast.LENGTH_SHORT);
        }
        mToast.show();
    }

    /**
     * 隐藏Toast
     */
    public void cancelToast() {
        if (mToast != null) {
            mToast.cancel();
            mToast = null;
        }
    }


    public void onBackPressed() {
        if(encodeThread != null && encodeThread.isAlive()){
            encodeCancel();
            cancelToast();
        }
        else{
            cancelToast();
            super.onBackPressed();
        }
    }
}
