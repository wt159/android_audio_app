package com.chinatsp.audiolp;


import android.car.Car;
import android.car.media.CarAudioManager;
import android.content.Context;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.media.audiofx.AudioEffect;

import java.util.Objects;

import com.chinatsp.audiofocus.ICallFocus;
import com.chinatsp.audiolp.*;
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
//        if (!isPermissionGranted(Car.PERMISSION_CAR_CONTROL_AUDIO_VOLUME)) {
//            return;
//        }
        mCarAudioManager.setGroupVolume(group, volume, 0);
    }

    public int getGroupVolume(int group) {
//        if (!isPermissionGranted(Car.PERMISSION_CAR_CONTROL_AUDIO_VOLUME)) {
//            return 10;
//        }
        return mCarAudioManager.getGroupVolume(group);
    }
    public int getGroupMinVolume(int group) {
        return mCarAudioManager.getGroupMinVolume(group);
    }
    public int getGroupMaxVolume(int group) {
        return mCarAudioManager.getGroupMaxVolume(group);
    }

    public void setBalance(int value) {
        if (value < -7) {
            value = -7;
        } else if (value > 7) {
            value = 7;
        }
        mCarAudioManager.setBalance(value);
    }

    public  int getBalance() {
        return mCarAudioManager.getBalance();
    }

    public void setFade(int value) {
        mCarAudioManager.setFade(value);
    }
    public int getFade() {
        return mCarAudioManager.getFade();
    }
    public void setEffect(int type) {
        mCarAudioManager.setEffect(type);
    }
    public int getEffect() {
        return mCarAudioManager.getEffect();
    }
    public void set3DType(int type) {
        mCarAudioManager.set3DType(type);
    }
    public int get3DType() {
        return mCarAudioManager.get3DType();
    }
    public void setBassLevel(int level) {
        mCarAudioManager.setBMT(0, level);
    }
    public int getBassLevel() {
        return mCarAudioManager.getBMT(0);
    }
    public void setMidLevel(int level) {
        mCarAudioManager.setBMT(1, level);
    }
    public int getMidLevel() {
        return mCarAudioManager.getBMT(1);
    }
    public void setTrebleLevel(int level) {
        mCarAudioManager.setBMT(2, level);
    }
    public int getTrebleLevel() {
        return mCarAudioManager.getBMT(2);
    }

    public void setParameters(String param) {
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
