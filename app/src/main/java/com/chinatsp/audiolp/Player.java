package com.chinatsp.audiolp;

import android.media.AudioFormat;
import android.media.MediaPlayer;

public class Player {
    static private final String TAG = LogUtils.TAG + Player.class.getSimpleName() + " : ";
    private MediaPlayer mMediaPlayer;
    private final int mSampleRate = 48000;
    private final int mFormat = AudioFormat.ENCODING_PCM_16BIT;
    private int mChannels = AudioFormat.CHANNEL_IN_STEREO;
}
