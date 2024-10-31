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

import com.chinatsp.audiolp.*;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

import androidx.annotation.NonNull;

public class OnClickHandler {
    static private final String TAG = LogUtils.TAG + OnClickHandler.class.getSimpleName() + " : ";
    private final Context mContext;
    private ArrayList<FocusHandler> mFocusLists;

    public static OnClickHandler mOnClickHandler = null;
    public static OnClickHandler getInstance(Context context, ArrayList<FocusHandler> focusLists) {
        if (mOnClickHandler == null)
            return mOnClickHandler = new OnClickHandler(context, focusLists);
        return mOnClickHandler;
    }
    public OnClickHandler(Context context, ArrayList<FocusHandler> focusLists) {
        mContext = context;
        mFocusLists = focusLists;
    }

    protected View.OnClickListener stopAllFocusOnClick() {
        return v -> {
            for (FocusHandler focus : mFocusLists) {
                focus.abandonFocus();
            }
        };
    }
}
