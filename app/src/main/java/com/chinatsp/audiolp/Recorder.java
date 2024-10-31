package com.chinatsp.audiolp;

import android.media.AudioDeviceInfo;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.media.audiofx.AcousticEchoCanceler;
import android.media.audiofx.AudioEffect;
import android.media.audiofx.NoiseSuppressor;
import java.io.IOException;
import android.os.Environment;
import android.content.Context;
import com.chinatsp.audiolp.*;
import com.chinatsp.audiofocus.ICallFocus;

public class Recorder {
    static private final String TAG = LogUtils.TAG + Recorder.class.getSimpleName() + " : ";
    private AudioRecord mAudioRecord;
    private final int mSampleRate = 16000;
    private final int mFormat = AudioFormat.ENCODING_PCM_16BIT;
    private int mChannelsMask = 0x60000C;
    private int mChannels = 0;
    private int mSource = MediaRecorder.AudioSource.MIC;
    private NoiseSuppressor mNoiseSuppressor;
    private AcousticEchoCanceler mAEC;
    private String mAddress = "";
    private AudioFile mAudioFile;
    private String mFilePath = "/data/local/tmp";

    private Context mContext;

    private boolean mIsRecording = false;

    private  String mParentName = "record";

    private void createAudioRecord() {
        if (mChannels == 1) {
            mChannelsMask = AudioFormat.CHANNEL_IN_MONO;
            int bufSize = AudioRecord.getMinBufferSize(mSampleRate, mChannelsMask, mFormat);
            mAudioRecord = new AudioRecord(mSource, mSampleRate, mChannelsMask, mFormat, bufSize);
        } else if (mChannels == 2) {
            mChannelsMask = AudioFormat.CHANNEL_IN_STEREO;
            int bufSize = AudioRecord.getMinBufferSize(mSampleRate, mChannelsMask, mFormat);
            mAudioRecord = new AudioRecord(mSource, mSampleRate, mChannelsMask, mFormat, bufSize);
        } else if (mChannels == 4) {
            mAudioRecord = new AudioRecord.Builder()
                    .setAudioSource(mSource)
                    .setAudioFormat(new AudioFormat.Builder()
                            .setEncoding(mFormat)
                            .setSampleRate(mSampleRate)
                            .setChannelIndexMask(0xf /* 0xf: 4channels, 0...4 */)
                            .build())
                    .build();
        } else if (mChannels == 8) {
            mAudioRecord = new AudioRecord.Builder()
                    .setAudioSource(mSource)
                    .setAudioFormat(new AudioFormat.Builder()
                            .setEncoding(mFormat)
                            .setSampleRate(mSampleRate)
                            .setChannelIndexMask(0xff)
                            .build())
                    .build();
        }
    }

    private void createAudioEffect() {
        mNoiseSuppressor = NoiseSuppressor.create(mAudioRecord.getAudioSessionId());
        mAEC = AcousticEchoCanceler.create(mAudioRecord.getAudioSessionId());
    }


    public Recorder(Context context, String name, int source, AudioManager audioManager, String address, int channels) {
        mSource = source;
        mContext = context;
        mChannels = channels;
        createAudioRecord();
        mParentName = name;
        if (mContext != null) {
            mFilePath = mContext.getExternalCacheDir().getPath();
        }
        mFilePath = mFilePath + "/" + mParentName + ".pcm";

        log("file path " + mFilePath);
        if (audioManager == null || address == null || address.equals("")) {
            log("audioManager or address is null");
            return ;
        }


        AudioDeviceInfo[] ads = audioManager.getDevices(AudioManager.GET_DEVICES_INPUTS);
        for (AudioDeviceInfo adi : ads) {
            if (adi.getAddress().equals(address)) {
                log("setPreferredDevice " + adi.getAddress());
                mAudioRecord.setPreferredDevice(adi);
            }
        }
        mAddress = address;
    }

    public Recorder(Context context, String name, AudioManager audioManager, String address) {
        this(context, name, MediaRecorder.AudioSource.MIC, audioManager, address, 4);
    }

    public Recorder(Context context, String name, int source) {
        this(context, name, source, null, null, 4);
    }

    public void stop() {
        if (mAudioRecord.getRecordingState() != AudioRecord.RECORDSTATE_STOPPED) {
            log("stop recording");
            mIsRecording = false;
            mAudioRecord.stop();
            mAudioRecord.release();
        }
    }

    public void setNS(boolean enable) {
        if (mNoiseSuppressor == null) {
            log("NoiseSuppressor == null, NoiseSuppressor.create()");
            mNoiseSuppressor = NoiseSuppressor.create(mAudioRecord.getAudioSessionId());

        }
        if (mNoiseSuppressor.getEnabled() == enable) {
            log("NoiseSuppressor already is " + enable);
            return;
        }

        if (mNoiseSuppressor.setEnabled(enable) != AudioEffect.SUCCESS) {
            log("NoiseSuppressor.setEnabled(" + enable + ") failed");
            return;
        }
        log("NoiseSuppressor.setEnabled(" + enable + ") success");
    }

    public void setAEC(boolean enable) {
        if (mAEC == null) {
            log("AcousticEchoCanceler == null, AcousticEchoCanceler.create()");
            mAEC = AcousticEchoCanceler.create(mAudioRecord.getAudioSessionId());
        }
        if (mAEC.getEnabled() == enable) {
            log("AEC already is " + enable);
            return;
        }

        if (mAEC.setEnabled(enable) != AudioEffect.SUCCESS) {
            log("AEC.setEnabled(" + enable + ") failed");
            return;
        }
        log("AEC.setEnabled(" + enable + ") success");
    }

    public void noiseSuppressorSetEnable(boolean enable) {
        log("noiseSuppressorSetEnable + " + enable);
        mNoiseSuppressor.setEnabled(enable);
    }

    public static void log(String s) {
        System.out.println(TAG + s);
    }

    public void start() {
        if (mAudioRecord.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
            return;
        }
        try {
            mAudioFile = new AudioFile(mFilePath, true);
        } catch (IOException e) {
            e.printStackTrace();
            log("open audio file failed");
            return;
        }
        //int bufSize = mAudioRecord.getMinBufferSize(mSampleRate, mChannels, mFormat);
        int byteSize = 7680;
        log("start recording, bufSize:" + byteSize);
        mIsRecording = true;
        mAudioRecord.startRecording();
        new Thread((Runnable) () -> {
            if (mAudioRecord.getPreferredDevice() != null)
                log("preferredDevice:" + mAudioRecord.getPreferredDevice().toString());
            else if (!mAddress.equals(""))
                log("preferredDevice = null, mAddress = " + mAddress);
            while (mAudioRecord.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING && mIsRecording)  {
                byte[] buf = new byte[byteSize];
                int readSize = mAudioRecord.read(buf, 0, byteSize);
                if (readSize != byteSize) {
                    log("readSize != byteSize: " + readSize + " | " + byteSize);
                }
                try {
                    mAudioFile.writeData(buf, byteSize);
                } catch (IOException e) {
                    e.printStackTrace();
                    log("write audio file failed");
                    return;
                }
            }
            try {
                mAudioFile.close();
            } catch (IOException e) {
                e.printStackTrace();
                log("open audio file failed");
                return;
            }
            log("exit record thread");
        }).start();
    }

    public int read(byte[] buffer, int size) {
        return mAudioRecord.read(buffer, 0, size);
    }
}
