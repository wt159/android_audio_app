package com.chinatsp.audiofocus;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioFocusRequest;
import android.media.AudioManager;
import android.media.MediaPlayer;

import com.chinatsp.audiolp.*;

public class ICallFocus extends FocusHandler {
    static private final String TAG = LogUtils.TAG + ICallFocus.class.getSimpleName() + " : ";
    private final MediaPlayer mMediaPlayer;
    private final AudioAttributes attr = new AudioAttributes.Builder()
            .setUsage(AudioAttributes.USAGE_ASSISTANCE_ACCESSIBILITY)
            .setContentType(AudioAttributes.CONTENT_TYPE_UNKNOWN)
            .build();
    private final AudioFocusRequest afr = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN)
            .setAudioAttributes(attr)
            .setAcceptsDelayedFocusGain(true)
            .setOnAudioFocusChangeListener(super::handlerAudioFocusChange)
            .build();

    public ICallFocus(Context context, AudioManager audioManager) {
        super(audioManager);
        mMediaPlayer = MediaPlayer.create(context, R.raw.well_worth_the_wait, attr, audioManager.generateAudioSessionId());
//        AudioDeviceInfo[] ads = audioManager.getDevices(AudioManager.GET_DEVICES_OUTPUTS);
//        for (AudioDeviceInfo adi : ads) {
//            if (adi.getAddress().equals("BUS07_SYS_SOUND")) {
//                log("setPreferredDevice " + adi.getAddress());
//                mMediaPlayer.setPreferredDevice(adi);
//            }
//        }

        mMediaPlayer.setLooping(true);
    }


    @Override
    protected AudioFocusRequest getAudioFocusRequest() {
        return afr;
    }

    @Override
    protected MediaPlayer getPlayer() {
        return mMediaPlayer;
    }

    @Override
    public void log(String s) {
        System.out.println(TAG + s);
    }
}
