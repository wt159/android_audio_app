package com.chinatsp.audiofocus;


import android.annotation.SuppressLint;
import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioDeviceInfo;
import android.media.AudioFocusRequest;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Build;
import android.view.View;

import com.chinatsp.audiolp.R;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

import androidx.annotation.NonNull;

public class OnClickHandler {
    private final static String TAG = "Mr.Long OnClickHandler ";
    private final int AUDIO_FOCUS_MEDIA = 0;
    private final int AUDIO_FOCUS_NAVI = 1;
    private final int AUDIO_FOCUS_ASSIS = 2;
    private final int AUDIO_FOCUS_KTV = 3;
    private final int AUDIO_FOCUS_MAX = AUDIO_FOCUS_KTV;
    private AudioManager mAudioManager;
    private final Map<Integer, MediaPlayer> mPlayers = new HashMap<>();
    private final Map<Integer, AudioFocusRequest> mAudioFocusMap = new HashMap<>();
    private final String[] mFocusName;
    private final Context mContext;

    public static OnClickHandler mOnClickHandler = null;
    //  AudioAttributes
    private final AudioAttributes mediaAttr = new AudioAttributes.Builder()
            .setUsage(AudioAttributes.USAGE_MEDIA)
            .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
            .build();
    private final AudioAttributes naviAttr = new AudioAttributes.Builder()
            .setUsage(AudioAttributes.USAGE_ASSISTANCE_NAVIGATION_GUIDANCE)
            .setContentType(AudioAttributes.CONTENT_TYPE_UNKNOWN)
            .setFlags(AudioAttributes.FLAG_AUDIBILITY_ENFORCED)
            .build();
    private final AudioAttributes assiAttr = new AudioAttributes.Builder()
            .setUsage(AudioAttributes.USAGE_ASSISTANT)
            .setContentType(AudioAttributes.CONTENT_TYPE_UNKNOWN)
            .build();
    private final AudioAttributes ktvAttr = new AudioAttributes.Builder()
            //.setUsage(AudioAttributes.USAGE_ANNOUNCEMENT)
            .build();

    //  AudioFocusRequest
    private final AudioFocusRequest mediaAFR = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN)
            .setAudioAttributes(mediaAttr)
            .setAcceptsDelayedFocusGain(true)
            .setOnAudioFocusChangeListener(f -> handlerAudioFocusChange(f, AUDIO_FOCUS_MEDIA))
            .build();
    private final AudioFocusRequest naviAFR = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN_TRANSIENT)
            .setAudioAttributes(naviAttr)
            .setOnAudioFocusChangeListener(f -> handlerAudioFocusChange(f, AUDIO_FOCUS_NAVI))
            .build();
    private final AudioFocusRequest assiAFR = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_EXCLUSIVE)
            .setAudioAttributes(assiAttr)
            .setOnAudioFocusChangeListener(f -> handlerAudioFocusChange(f, AUDIO_FOCUS_ASSIS))
            .build();
    private final AudioFocusRequest ktvAFR = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN)
            .setAudioAttributes(ktvAttr)
            .setOnAudioFocusChangeListener(f -> handlerAudioFocusChange(f, AUDIO_FOCUS_KTV))
            .build();


    public static OnClickHandler getInstance(Context context) {
        if (mOnClickHandler == null)
            return mOnClickHandler = new OnClickHandler(context);
        return mOnClickHandler;
    }

    private void initPlayer(int playerType, AudioAttributes attr, int resId) {
        MediaPlayer player = MediaPlayer.create(mContext, resId, attr, mAudioManager.generateAudioSessionId());
        player.setLooping(true);
        mPlayers.put(playerType, player);
    }


    public OnClickHandler(Context context) {
        mContext = context;
        mAudioFocusMap.put(AUDIO_FOCUS_MEDIA, mediaAFR);
        mAudioFocusMap.put(AUDIO_FOCUS_NAVI, naviAFR);
        mAudioFocusMap.put(AUDIO_FOCUS_ASSIS, assiAFR);
        mAudioFocusMap.put(AUDIO_FOCUS_KTV, ktvAFR);
        mFocusName = new String[] {"MEDIA ", "NAVI ", "ASSI ", "KTV ",};

        mAudioManager = (AudioManager)context.getSystemService(Context.AUDIO_SERVICE);
        initPlayer(AUDIO_FOCUS_MEDIA, mediaAttr, R.raw.guyongzhe_48000_16_2);
        initPlayer(AUDIO_FOCUS_NAVI, naviAttr, R.raw.well_worth_the_wait);
        initPlayer(AUDIO_FOCUS_ASSIS, assiAttr, R.raw.media);
        initPlayer(AUDIO_FOCUS_KTV, ktvAttr, R.raw.signal_1k_0db_90s);
    }

    private AudioDeviceInfo getDeviceByAddress(@NonNull String address, int deviceFlag) {
        AudioDeviceInfo[] ads = mAudioManager.getDevices(deviceFlag);
        for (AudioDeviceInfo ad : ads) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                if (Objects.equals(ad.getAddress(), address))
                    return ad;
            }
        }
        System.out.println("Mr.Long do not found input device, address:" + address);
        return null;
    }

    private MediaPlayer getPlayerByFocusType(int focusType) {
        if (focusType < AUDIO_FOCUS_MEDIA || focusType > AUDIO_FOCUS_MAX) {
            System.out.println(TAG + "focusType " + focusType + " is invalid");
            return new MediaPlayer();
        }
        return mPlayers.get(focusType);
    }

    private AudioFocusRequest getFocusRequestByFocusType(int focusType) {
        return mAudioFocusMap.get(focusType);
    }

    private String getFocusTypeName(int focusType) {
        return mFocusName[focusType];
    }

    private void handlerAudioFocusChange(int focusChange, int focusType) {
        MediaPlayer player = getPlayerByFocusType(focusType);
        String focusTypeName = getFocusTypeName(focusType);

        System.out.println(TAG + focusTypeName + " focus change " + focusChange);
        switch (focusChange) {
            case AudioManager.AUDIOFOCUS_GAIN:
            case AudioManager.AUDIOFOCUS_GAIN_TRANSIENT:
            case AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK:
            case AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_EXCLUSIVE:
                if (!player.isPlaying()) {
                    player.start();
                    System.out.println(TAG + focusTypeName + " start play");
                }
                break;
            case AudioManager.AUDIOFOCUS_LOSS: {
                if (player.isPlaying()) {
                    player.stop();
                    player.prepareAsync();
                }
            }
            case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT: {
                if (player.isPlaying())
                    player.pause();
                break;
            }
        }
    }

    @SuppressLint("NonConstantResourceId")
    private int getFocusTypeByViewId(int viewId) {
        int FocusType = -1;
        switch (viewId) {
            case R.id.play_media_button:
            case R.id.stop_media_button:
                FocusType = AUDIO_FOCUS_MEDIA;
                break;
            case R.id.play_navi_button:
            case R.id.stop_navi_button:
                FocusType = AUDIO_FOCUS_NAVI;
                break;
            case R.id.play_assistant_button:
            case R.id.stop_assistant_button:
                FocusType = AUDIO_FOCUS_ASSIS;
                break;
            case R.id.play_ktv_button:
            case R.id.stop_ktv_button:
                FocusType = AUDIO_FOCUS_KTV;
                break;
        }
        return FocusType;
    }

    @SuppressLint("NonConstantResourceId")
    protected View.OnClickListener playOnClick() {
        return v -> {
            int focusType = getFocusTypeByViewId(v.getId());
            if (focusType < AUDIO_FOCUS_MEDIA || focusType > AUDIO_FOCUS_MAX) {
                System.out.println(TAG + "unknow focus");
                return;
            }
            MediaPlayer player = getPlayerByFocusType(focusType);
            if (mAudioManager.requestAudioFocus(getFocusRequestByFocusType(focusType)) == AudioManager.AUDIOFOCUS_GAIN) {
                if (player.isPlaying()) {
                    System.out.println(TAG + getFocusTypeName(focusType) + "audio focus playing");
                    return;
                }
                player.start();
                System.out.println(TAG + getFocusTypeName(focusType) + "focus gain");
            } else
                System.out.println(TAG + getFocusTypeName(focusType) + "focus abandon");
        };
    }

    protected View.OnClickListener stopOnClick() {
        return v -> {
            int focusType = getFocusTypeByViewId(v.getId());
            if (focusType < AUDIO_FOCUS_MEDIA || focusType > AUDIO_FOCUS_MAX) {
                System.out.println(TAG + "unknow focus");
                return;
            }
            MediaPlayer player = getPlayerByFocusType(focusType);
            if (player.isPlaying()) {
                player.stop();
                player.prepareAsync();
            }
            mAudioManager.abandonAudioFocusRequest(getFocusRequestByFocusType(focusType));
            System.out.println(TAG  + getFocusTypeName(focusType) + "stop");
        };
    }

    protected View.OnClickListener stopAllFocusOnClick() {
        return v -> {
            for (Map.Entry<Integer, MediaPlayer> entry : mPlayers.entrySet()) {
                int focusType = entry.getKey();
                MediaPlayer player = entry.getValue();
                if (player.isPlaying()) {
                    player.stop();
                    player.prepareAsync();
                    mAudioManager.abandonAudioFocusRequest(getFocusRequestByFocusType(focusType));
                    System.out.println(TAG + getFocusTypeName(focusType) + "stop");
                }
            }
        };
    }
}
