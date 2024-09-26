package com.chinatsp.audiofocus;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioFocusRequest;
import android.media.AudioManager;
import android.media.MediaPlayer;

import com.chinatsp.audiolp.R;

public class NavigationFocus extends FocusHandler{
    static private final String TAG = "Mr.Long NavigationFocus ";
    private final MediaPlayer mPlayer;
    private final AudioAttributes attr = new AudioAttributes.Builder()
            .setUsage(AudioAttributes.USAGE_ASSISTANCE_NAVIGATION_GUIDANCE)
            .setContentType(AudioAttributes.CONTENT_TYPE_UNKNOWN)
            .build();
    private final AudioFocusRequest afr = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK)
            .setAudioAttributes(attr)
            .setOnAudioFocusChangeListener(super::handlerAudioFocusChange)
            .build();

    public NavigationFocus(Context context, AudioManager audioManager) {
        super(audioManager);
        mPlayer = MediaPlayer.create(context, R.raw.well_worth_the_wait, attr, audioManager.generateAudioSessionId());
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
