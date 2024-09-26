package com.chinatsp.audiofocus;

import android.media.AudioFocusRequest;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.view.View;

public abstract class FocusHandler {
    final AudioManager mAudioManager;

    public FocusHandler(AudioManager audioManager) {
        mAudioManager = audioManager;
    }

    public void handlerAudioFocusChange(int focus) {
        log(" focus change " + focus);
        MediaPlayer player = getPlayer();
        switch (focus) {
            case AudioManager.AUDIOFOCUS_GAIN:
            case AudioManager.AUDIOFOCUS_GAIN_TRANSIENT:
            case AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK:
            case AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_EXCLUSIVE:
                if (player != null && !player.isPlaying()) {
                    player.start();
                }
                log("play");
                break;
            case AudioManager.AUDIOFOCUS_LOSS: {
                if (player != null && player.isPlaying()) {
                    player.stop();
                    player.prepareAsync();
                }
                log("stop");
                break;
            }
            case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT: {
                if (player != null && player.isPlaying()) {
                    player.pause();
                }
                log("pause");
                break;
            }
        }
    }

    protected void requestFocus() {
        getPlayer().start();

        if (mAudioManager.requestAudioFocus(getAudioFocusRequest()) == AudioManager.AUDIOFOCUS_GAIN) {
            log("focus gain");
            MediaPlayer player = getPlayer();
            if (player != null && !player.isPlaying()) {
                player.start();
                return;
            }
            if (player == null) {
                log("player is null");
            } else
                log("is playing");

        } else
            log("focus abandon");
    }

    protected void abandonFocus() {
        MediaPlayer player = getPlayer();
        if (player != null) {
            player.stop();
            player.prepareAsync();
            log("stop play");
        } else {
            log("player is null");
        }
        mAudioManager.abandonAudioFocusRequest(getAudioFocusRequest());
        log("abandonFocus()");
    }

    protected void requestClick(View v) {
        requestFocus();
    }

    protected void abandonClick(View v) {
        abandonFocus();
    }

    abstract protected AudioFocusRequest getAudioFocusRequest();
    abstract protected MediaPlayer getPlayer();
    abstract void log(String s);
}
