package com.chinatsp.audiofocus;

import android.app.Activity;
import android.content.Context;
import android.media.AudioManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import com.chinatsp.audiolp.*;
import java.util.ArrayList;


public class AudioFocusActivity extends Activity {
    private final static String TAG = LogUtils.TAG + AudioFocusActivity.class.getSimpleName() + " : ";
    OnClickHandler mOnClickHandler;
    private AudioManager mAudioManager;
    private ArrayList<FocusHandler> mFocusLists;

    @Override
    public void onCreate(Bundle saveInstanceState) {
        super.onCreate(saveInstanceState);
        setContentView(R.layout.activity_audiofocus);
        mAudioManager = (AudioManager)this.getSystemService(Context.AUDIO_SERVICE);

        FocusHandler media = new MediaFocus(this, mAudioManager);
        FocusHandler navi = new NavigationFocus(this, mAudioManager);
        FocusHandler assi = new AssistantFocus(this, mAudioManager);
        FocusHandler notification = new NotificationFocus(this, mAudioManager);
        FocusHandler externalVoice = new ExternalVoiceFocus(this, mAudioManager);
        FocusHandler call = new CallFocus(this, mAudioManager);
        FocusHandler ring = new RingFocus(this, mAudioManager);
        FocusHandler safeVoice = new SafeVoiceFocus(this, mAudioManager);
        FocusHandler bcall = new BCallFocus(this, mAudioManager);
        FocusHandler icall = new ICallFocus(this, mAudioManager);

        findViewById(R.id.play_media_button).setOnClickListener(media::requestClick);
        findViewById(R.id.stop_media_button).setOnClickListener(media::abandonClick);
        findViewById(R.id.play_navi_button).setOnClickListener(navi::requestClick);
        findViewById(R.id.stop_navi_button).setOnClickListener(navi::abandonClick);
        findViewById(R.id.play_assistant_button).setOnClickListener(assi::requestClick);
        findViewById(R.id.stop_assistant_button).setOnClickListener(assi::abandonClick);
        findViewById(R.id.play_notification_button).setOnClickListener(notification::requestClick);
        findViewById(R.id.stop_notification_button).setOnClickListener(notification::abandonClick);
        findViewById(R.id.play_ext_voice_button).setOnClickListener(externalVoice::requestClick);
        findViewById(R.id.stop_ext_voice_button).setOnClickListener(externalVoice::abandonClick);
        findViewById(R.id.play_call_button).setOnClickListener(call::requestClick);
        findViewById(R.id.stop_call_button).setOnClickListener(call::abandonClick);
        findViewById(R.id.play_ring_button).setOnClickListener(ring::requestClick);
        findViewById(R.id.stop_ring_button).setOnClickListener(ring::abandonClick);
        findViewById(R.id.play_safe_voice_button).setOnClickListener(safeVoice::requestClick);
        findViewById(R.id.stop_safe_voice_button).setOnClickListener(safeVoice::abandonClick);
        findViewById(R.id.play_bcall_button).setOnClickListener(bcall::requestClick);
        findViewById(R.id.stop_bcall_button).setOnClickListener(bcall::abandonClick);
        findViewById(R.id.play_icall_button).setOnClickListener(icall::requestClick);
        findViewById(R.id.stop_icall_button).setOnClickListener(icall::abandonClick);

        mFocusLists = new ArrayList<FocusHandler>();
        mFocusLists.add(media);
        mFocusLists.add(navi);
        mFocusLists.add(assi);
        mFocusLists.add(notification);
        mFocusLists.add(externalVoice);
        mFocusLists.add(call);
        mFocusLists.add(ring);
        mFocusLists.add(safeVoice);
        mFocusLists.add(bcall);
        mFocusLists.add(icall);
        mOnClickHandler = OnClickHandler.getInstance(this, mFocusLists);
        findViewById(R.id.abandon_all_focus).setOnClickListener(mOnClickHandler.stopAllFocusOnClick());


        // ktv focus
//        findViewById(R.id.play_ktv_button).setOnClickListener(mOnClickHandler.playOnClick());
//        findViewById(R.id.stop_ktv_button).setOnClickListener(mOnClickHandler.stopOnClick());
    }

    public void backOnClick(View v) {
        mAudioManager.setParameters("app_set_param_key=value;");
        this.finish();
    }
}
