package com.chinatsp.audiofocus;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioDeviceInfo;
import android.media.AudioFocusRequest;
import android.media.AudioManager;
import android.media.MediaPlayer;

import com.chinatsp.audiolp.R;

public class ExternalVoiceFocus extends FocusHandler{
    static private final String TAG = "Mr.Long ExternalVoiceFocus ";
    private final MediaPlayer mPlayer;
    private final AudioAttributes attr = new AudioAttributes.Builder()
            .setUsage(AudioAttributes.USAGE_ASSISTANT)
            .setContentType(AudioAttributes.CONTENT_TYPE_UNKNOWN)
            .build();
    private final AudioFocusRequest afr = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN_TRANSIENT)
            .setAudioAttributes(attr)
            .setOnAudioFocusChangeListener(super::handlerAudioFocusChange)
            .build();

    public ExternalVoiceFocus(Context context, AudioManager audioManager) {
        super(audioManager);
        mPlayer = MediaPlayer.create(context, R.raw.dtmf, attr, audioManager.generateAudioSessionId());
        AudioDeviceInfo[] ads = audioManager.getDevices(AudioManager.GET_DEVICES_OUTPUTS);
        for (AudioDeviceInfo ad : ads) {
            if (ad.getAddress().equals("BUS01_SYS_NOTIFICATION"))
                mPlayer.setPreferredDevice(ad);
        }
        mPlayer.setLooping(true);
    }


    @Override
    protected AudioFocusRequest getAudioFocusRequest() {
        return afr;
    }

    @Override
    protected MediaPlayer getPlayer() {
        return mPlayer;
    }

    @Override
    public void log(String s) {
        System.out.println(TAG + s);
    }

}
