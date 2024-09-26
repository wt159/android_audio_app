package com.chinatsp.audiofocus;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioFocusRequest;
import android.media.AudioManager;
import android.media.MediaPlayer;

import com.chinatsp.audiolp.R;

public class AssistantFocus extends FocusHandler{
    static private final String TAG = "Mr.Long AssistantFocus ";
    private final MediaPlayer mPlayer;
    private final AudioAttributes attr = new AudioAttributes.Builder()
            .setUsage(AudioAttributes.USAGE_VOICE_COMMUNICATION)
            .setContentType(AudioAttributes.CONTENT_TYPE_UNKNOWN)
            .build();
    private final AudioFocusRequest afr = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN_TRANSIENT)
            .setAudioAttributes(attr)
            .setOnAudioFocusChangeListener(super::handlerAudioFocusChange)
            .build();

    public AssistantFocus(Context context, AudioManager audioManager) {
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
