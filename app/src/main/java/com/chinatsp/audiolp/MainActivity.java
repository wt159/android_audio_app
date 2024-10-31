package com.chinatsp.audiolp;

import android.Manifest;
import android.audio.policy.configuration.V7_0.AudioDevice;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.media.AudioAttributes;
import android.media.AudioDeviceInfo;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.media.HwAudioSource;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.SeekBar;
import android.widget.Spinner;

import com.chinatsp.audiofocus.AudioFocusActivity;
import com.chinatsp.parse.CarAudioPlatformConfig;
import com.chinatsp.audcal.*;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.io.InputStream;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Enumeration;
import java.util.Locale;
import java.nio.ByteBuffer;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import static android.media.AudioFormat.ENCODING_PCM_16BIT;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "Mr.Long MainActivity ";
    private int mVolumeGroup = 0;
    private AudioController mAudioController;
    private Recorder mRecorder = null;
    private Recorder mExtRecorder = null;
    private Recorder mECNSRecorder = null;
    private Recorder mBECallRecorder = null;
    private AudioManager mAudioManager;
    private boolean mRunning = true;

    private AudioFile mAudioFile = null;

    // 录音权限
    private boolean mPermissionRecord = false;
    private static String mPermissionKeyRecord = Manifest.permission.RECORD_AUDIO;
    // car volume 权限
    private static String mPermissionKeyCarVolume = "android.car.permission.CAR_CONTROL_AUDIO_VOLUME";

    // 文件读写权限
    private boolean mPermissionFile = false;
    private static String mPermissionKeyFile = Manifest.permission.WRITE_EXTERNAL_STORAGE;
    boolean mPermissionCarFile = false;
    private static String mPermissionKeyCarSetting = "android.car.permission.CAR_CONTROL_AUDIO_SETTINGS";
    boolean mPermissionCarSettings = false;
    private Context mContext;

    private boolean mFMEnable = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mAudioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        mContext = getApplicationContext();
        // 权限申请
        requestPermissionAll();

        mAudioController = new AudioController(this);


        // 音频焦点界面跳转按钮
        findViewById(R.id.focus_activity).setOnClickListener(startFocusActivity());

        // 自定义控件1
        findViewById(R.id.custom1).setOnClickListener(onClickCustom1());
        findViewById(R.id.custom2).setOnClickListener(onClickCustom2());
        findViewById(R.id.custom3).setOnClickListener(onClickCustom3());
        findViewById(R.id.custom4).setOnClickListener(onClickCustom4());

        // 录音 车内
        findViewById(R.id.start_record_button).setOnClickListener(onClickStartRecord());
        findViewById(R.id.stop_record_button).setOnClickListener(onClickStopRecord());

        // 录音 车外
        findViewById(R.id.start_record_ext_button).setOnClickListener(this::onClickStartExternalRecord);
        findViewById(R.id.stop_record_ext_button).setOnClickListener(this::onClickStopExternalRecord);

        // 录音 ECNS
        findViewById(R.id.start_record_ecns_button).setOnClickListener(this::onClickStartECNSRecord);
        findViewById(R.id.stop_record_ecns_button).setOnClickListener(this::onClickStopECNSRecord);

        // 录音 BECALL
        findViewById(R.id.start_record_becall_button).setOnClickListener(this::onClickStartBECALLRecord);
        findViewById(R.id.stop_record_becall_button).setOnClickListener(this::onClickStopBECALLRecord);

        // 录音 FM
        findViewById(R.id.start_record_fm_button).setOnClickListener(this::onClickStartFMRecord);
        findViewById(R.id.stop_record_fm_button).setOnClickListener(this::onClickStopFMRecord);


        // 音量滑动选择组件
        SeekBar volumeSeek = findViewById(R.id.volume_seek);
        volumeSeek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                log("onStopTrackingTouch progress=" + seekBar.getProgress() + "current vol:" + mAudioController.getGroupVolume(mVolumeGroup));
                mAudioController.setGroupVolume(mVolumeGroup, seekBar.getProgress());
                int min = mAudioController.getGroupMinVolume(mVolumeGroup);
                int max = mAudioController.getGroupMaxVolume(mVolumeGroup);
                log("onStopTrackingTouch groupId: " + mVolumeGroup + " minVolume: " + min + " MaxVolume: " + max);
            }
        });

        // 音量类型选择控件
        Spinner volumeGroupSpinner = findViewById(R.id.volume_spinner);
        volumeGroupSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
                log("onItemSelected pos:" + pos + " id:" + id);
                mVolumeGroup = pos;
//                volumeSeek.setProgress(mAudioController.getGroupVolume(mVolumeGroup));

            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });



    }

    private void getOutDevicesString() {
        AudioDeviceInfo[] ads = mAudioManager.getDevices(AudioManager.GET_DEVICES_OUTPUTS);
    }

    private void playerViewInit() {
        String[] s = {"1", "2", "3"};
        Spinner spinner = findViewById(R.id.usage_spinner);
        ArrayAdapter<String> adapter = new ArrayAdapter<String> (this,android.R.layout.simple_list_item_multiple_choice, s);
        spinner.setAdapter(adapter);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        log( "permission nums: " + permissions.length + " result nums: " + grantResults.length);
        for (int i = 0; i < permissions.length; i++) {
            boolean permissionResult = false;
            if (grantResults.length > 0 && grantResults[i] == PackageManager.PERMISSION_GRANTED)
                permissionResult = true;
            log("permission :" + permissions[i] + " result:" + (permissionResult ? "granted" : "denied"));
        }
    }

    void requestPermissionAll() {
        String[] permissions = {
            mPermissionKeyRecord,
            mPermissionKeyCarVolume,
            mPermissionKeyFile,
            mPermissionKeyCarSetting
        };
        ActivityCompat.requestPermissions(this, permissions, 0);
    }

    private void requestPermissions(String permissions, int requestCode) {
        if (ContextCompat.checkSelfPermission(this, permissions) == PackageManager.PERMISSION_DENIED) {
            ActivityCompat.requestPermissions(this, new String[]{permissions}, requestCode);
        }
    }

    View.OnClickListener startFocusActivity() {
        return v -> {
            System.out.println(TAG + "startFocusActivity");
            startActivity(new Intent(this, AudioFocusActivity.class));
        };
    }

    View.OnClickListener onClickCustom1() {
        return v -> {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    int value = mAudioController.getBalance();
                    log("car audio manager balance val: " + value);
                    mAudioController.setBalance(value+1);
                    value = mAudioController.getBalance();
                    log("car audio manager balance val: " + value);

                    value = mAudioController.getFade();
                    log("car audio manager fade val: " + value);
                    mAudioController.setFade(value+1);
                    value = mAudioController.getFade();
                    log("car audio manager fade val: " + value);

                    value = mAudioController.getEffect();
                    log("car audio manager getEffect val: " + value);
                    mAudioController.setEffect(value+1);
                    value = mAudioController.getEffect();
                    log("car audio manager getEffect val: " + value);

                    value = mAudioController.get3DType();
                    log("car audio manager get3DType val: " + value);
                    mAudioController.set3DType(value+1);
                    value = mAudioController.get3DType();
                    log("car audio manager get3DType val: " + value);

                    value = mAudioController.getBassLevel();
                    log("car audio manager getBassLevel val: " + value);
                    mAudioController.setBassLevel(value+1);
                    value = mAudioController.getBassLevel();
                    log("car audio manager getBassLevel val: " + value);

                    value = mAudioController.getMidLevel();
                    log("car audio manager getMidLevel val: " + value);
                    mAudioController.setMidLevel(value+1);
                    value = mAudioController.getMidLevel();
                    log("car audio manager getMidLevel val: " + value);

                    value = mAudioController.getTrebleLevel();
                    log("car audio manager getTrebleLevel val: " + value);
                    mAudioController.setTrebleLevel(value+1);
                    value = mAudioController.getTrebleLevel();
                    log("car audio manager getTrebleLevel val: " + value);
                }
            }).start();
        };
    }

    View.OnClickListener onClickCustom2() {
        return v -> {
            log("onClickCustom2");
            mRunning = false;
            InputStream is = this.getResources().openRawResource(R.xml.car_audio_platform_configuration);

            CarAudioPlatformConfig pc = new CarAudioPlatformConfig("/vendor/etc/car_audio_platform_configuration.xml", false);
            try {
                pc.load();
            } catch (XmlPullParserException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            }
        };
    }

    public static String getIpAddressString() {
        try {
            for (Enumeration<NetworkInterface> enNetI = NetworkInterface
                    .getNetworkInterfaces(); enNetI.hasMoreElements(); ) {
                NetworkInterface netI = enNetI.nextElement();
                for (Enumeration<InetAddress> enumIpAddr = netI
                        .getInetAddresses(); enumIpAddr.hasMoreElements(); ) {
                    InetAddress inetAddress = enumIpAddr.nextElement();
                    if (inetAddress instanceof Inet4Address && !inetAddress.isLoopbackAddress()) {
                        log(inetAddress.getHostAddress());
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "0.0.0.0";
    }


    View.OnClickListener onClickCustom3() {
        return v -> {
            log("onClickCustom3 ");
            if (false) {
                AudioDeviceInfo inDevs[] = mAudioManager.getDevices(AudioManager.GET_DEVICES_INPUTS);
                int fmInfoIndex = -1;
                for (int i = 0; i < inDevs.length; i++) {
                    if (inDevs[i].getType() == AudioDeviceInfo.TYPE_FM_TUNER) {
                        fmInfoIndex = i;
                    }
                    log("device|type:" + inDevs[i].getType() + " addr:" + inDevs[i].getAddress());
                }
                if (fmInfoIndex == -1) {
                    log("no fm tuner devices");
                    return;
                }
                log("device info:" + inDevs[fmInfoIndex].toString());
                HwAudioSource mHwAudioSource = new HwAudioSource.Builder()
                        .setAudioDeviceInfo(inDevs[fmInfoIndex])
                        .setAudioAttributes(new AudioAttributes.Builder()
                                .setUsage(AudioAttributes.USAGE_MEDIA)
                                .setFlags(0x2)
                                .build())
                        .build();
                mHwAudioSource.start();
                mHwAudioSource.stop();
            } else {
                if (mFMEnable) {
                    mAudioManager.setParameters("fm_enable=false");
                    mFMEnable = false;
                } else {
                    mAudioManager.setParameters("fm_enable=true");
                    mFMEnable = true;
                }
            }
        };
    }

    private byte [] int2byte(int[] intArray) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(intArray.length * 4);
        for (int value : intArray) {
            byteBuffer.putInt(value);
        }
        byte[] byteArray = byteBuffer.array();
        return byteArray;
    }

    private byte [] short2byte(short[] shortArray) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(shortArray.length * 2);
        for (short value : shortArray) {
            byteBuffer.putShort(value);
        }
        byte[] byteArray = byteBuffer.array();
        return byteArray;
    }

    /**
     * byte转short
     * @param data
     * @return
     */
    public short[] byteToShort(byte[] data) {
        short[] shortValue = new short[data.length / 2];
        for (int i = 0; i < shortValue.length; i++) {
            shortValue[i] = (short) ((data[i * 2] & 0xff) | ((data[i * 2 + 1] & 0xff) << 8));
        }
        return shortValue;
    }

    View.OnClickListener onClickCustom4() {
        return v -> {
            log("onClickCustom4");
            mAudioManager.setMode(AudioManager.MODE_NORMAL);
            AudioCalControl calControl = new AudioCalControl(this,
                    1, 0x1000FF01, 0x11130,
                    0x1e002f20, 0x1e002f21, 102);

            byte[] getData = calControl.getCalData();
            short[] shortData = byteToShort(getData);
            log("getData length:" + shortData.length);
            for (short b : shortData) {
                log("getData: " + (b&0x0FFFF));
            }



            calControl.setModuleID(0x1e002f20);
			calControl.setParamID(0x1e002f22);
			getData = calControl.getCalData();
            shortData = byteToShort(getData);
            log("getData length:" + shortData.length);
            for (short b : shortData) {
                log("getData: " + (b&0x0FFFF));
            }
			
			calControl.setModuleID(0x1e002f20);
			calControl.setParamID(0x1e002f23);
			getData = calControl.getCalData();
            shortData = byteToShort(getData);
            log("getData length:" + shortData.length);
            for (short b : shortData) {
                log("getData: " + (b&0x0FFFF));
            }
			
			calControl.setModuleID(0x1e002f20);
			calControl.setParamID(0x1e002f24);
			getData = calControl.getCalData();
            shortData = byteToShort(getData);
            log("getData length:" + shortData.length);
            for (short b : shortData) {
                log("getData: " + (b&0x0FFFF));
            }
			
			calControl.setModuleID(0x1e002f00);
			calControl.setParamID(0x1e002f02);
			getData = calControl.getCalData();
            shortData = byteToShort(getData);
            log("getData length:" + shortData.length);
            for (short b : shortData) {
                log("getData: " + (b&0x0FFFF));
            }
			
			calControl.setModuleID(0x1e002f00);
			calControl.setParamID(0x1e002f03);
			getData = calControl.getCalData();
            shortData = byteToShort(getData);
            log("getData length:" + shortData.length);
            for (short b : shortData) {
                log("getData: " + (b&0x0FFFF));
            }
			


            log("read finish");
        };
    }

    View.OnClickListener onClickStartRecord() {
        return v -> {
            log("onClickStartRecord");
            if (mRecorder == null)
                mRecorder = new Recorder(mContext, "car_in", MediaRecorder.AudioSource.MIC, null, null, 4);
            mRecorder.start();
        };
    }

    View.OnClickListener onClickStopRecord() {
        return v -> {
            log("onClickStopRecord");
            if (mRecorder != null) {
                mRecorder.stop();
                mRecorder = null;
            }
        };
    }

    public void onClickStartExternalRecord(View v) {
        log("onClickStartExternalRecord");
//        if (mExtRecorder == null)
//            mExtRecorder = new Recorder(mAudioManager, "BUS09_INPUT_FRONT_PASSENGER");
        if (mExtRecorder == null)
            mExtRecorder = new Recorder(mContext, "car_ext", MediaRecorder.AudioSource.VOICE_RECOGNITION, null, null, 1);
        mExtRecorder.start();

    }

    public void onClickStopExternalRecord(View v) {
        log("onClickStopExternalRecord");
        if (mExtRecorder != null) {
            mExtRecorder.stop();
            mExtRecorder = null;
        }
    }

    public void onClickStartECNSRecord(View v) {
        log("onClickStartECNSRecord");
        if (mECNSRecorder == null)
            mECNSRecorder = new Recorder(mContext, "ecns", MediaRecorder.AudioSource.VOICE_COMMUNICATION, mAudioManager, "BUS11_INPUT_ECNS", 4);
        mECNSRecorder.start();

    }

    public void onClickStopECNSRecord(View v) {
        log("onClickStopECNSRecord");
        if (mECNSRecorder != null) {
            mECNSRecorder.setAEC(false);
            mECNSRecorder.setNS(false);
            mECNSRecorder.stop();
            mECNSRecorder = null;
        }
    }

    public void onClickStartBECALLRecord(View v) {
        log("onClickStartBECALLRecord");
        if (mBECallRecorder == null)
            mBECallRecorder = new Recorder(mContext, "becall", mAudioManager, "BUS17_INPUT_REAR_SEAT");
        mBECallRecorder.start();
    }

    public void onClickStopBECALLRecord(View v) {
        log("onClickStopBECALLRecord");
        if (mBECallRecorder != null) {
            mBECallRecorder.stop();
            mBECallRecorder = null;
        }
    }

    public void onClickStartFMRecord(View v) {
        log("onClickStartFMRecord");
        if (mBECallRecorder == null)
            mBECallRecorder = new Recorder(mContext, "fm", mAudioManager, "BUS17_INPUT_REAR_SEAT");
        mBECallRecorder.start();
    }

    public void onClickStopFMRecord(View v) {
        log("onClickStopFMRecord");
        if (mBECallRecorder != null) {
            mBECallRecorder.stop();
            mBECallRecorder = null;
        }
    }

    public static void log(String s) {
        System.out.println(TAG + s);
    }
}