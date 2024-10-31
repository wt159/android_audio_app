package com.chinatsp.controller;


import android.car.Car;
import android.car.media.CarAudioManager;
import android.content.Context;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.media.audiofx.AudioEffect;
import android.media.audiofx.NoiseSuppressor;

import com.chinatsp.audiolp.*;

import java.util.Objects;

import androidx.core.content.ContextCompat;

public class AudioController {
    private CarAudioManager mCarAudioManager;
    private Context mContext;
    private AudioManager mAudioManager;
    static private final String TAG = LogUtils.TAG + AudioController.class.getSimpleName() + " : ";
    private AudioEffect mAE;

    public AudioController(Context context) {
        mContext = context;
        mCarAudioManager = connectCarAudioService();
        if (mCarAudioManager == null) {
            log("CarAudioManager connect failed");
        }

        mAudioManager = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
    }

    public void setGroupVolume(int group, int volume) {
        if (!isPermissionGranted(Car.PERMISSION_CAR_CONTROL_AUDIO_VOLUME)) {
            return;
        }
        mCarAudioManager.setGroupVolume(group, volume, 0);
    }

    public int getGroupVolume(int group) {
        if (!isPermissionGranted(Car.PERMISSION_CAR_CONTROL_AUDIO_VOLUME)) {
            return 10;
        }
        return mCarAudioManager.getGroupVolume(group);
    }

    public void setParameters(String param) {
        if (!isPermissionGranted(Car.PERMISSION_CAR_CONTROL_AUDIO_VOLUME)) {
            return ;
        }
        mAudioManager.setParameters(param);
    }

    public void setMicMute(boolean on) {
        mAudioManager.setMicrophoneMute(on);
    }

    private boolean isPermissionGranted(String permission) {
        boolean denied = ContextCompat.checkSelfPermission(mContext, permission) == PackageManager.PERMISSION_DENIED;
        if (denied)
            log(permission + " permission denied");
        else
            log(permission + " permission granted");
        return !denied;
    }

    private void log(String s) {
        System.out.println(TAG + s);
    }

    private CarAudioManager connectCarAudioService() {
        Car car = Objects.requireNonNull(Car.createCar(mContext));
        return (CarAudioManager) car.getCarManager(Car.AUDIO_SERVICE);
    }
}
