package com.chinatsp.audiofocus;

import android.app.Activity;
import android.content.Context;
import android.media.AudioManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.chinatsp.audiolp.R;



public class AudioFocusActivity extends Activity {
    private final static String TAG = "Mr.Long AudioFocusActivity: ";
    OnClickHandler mOnClickHandler;
    private AudioManager mAudioManager;

    @Override
    public void onCreate(Bundle saveInstanceState) {
        super.onCreate(saveInstanceState);
        setContentView(R.layout.activity_audiofocus);
        mOnClickHandler = OnClickHandler.getInstance(this);
        mAudioManager = (AudioManager)this.getSystemService(Context.AUDIO_SERVICE);

        FocusHandler media = new MediaFocus(this, mAudioManager);
        FocusHandler navi = new NavigationFocus(this, mAudioManager);
        FocusHandler assi = new AssistantFocus(this, mAudioManager);
        FocusHandler notification = new NotificationFocus(this, mAudioManager);
        FocusHandler externalVoice = new ExternalVoiceFocus(this, mAudioManager);

        // media focus
        findViewById(R.id.play_media_button).setOnClickListener(media::requestClick);
        findViewById(R.id.stop_media_button).setOnClickListener(media::abandonClick);
        // navigation focus
        findViewById(R.id.play_navi_button).setOnClickListener(navi::requestClick);
        findViewById(R.id.stop_navi_button).setOnClickListener(navi::abandonClick);
        // assistant focus
        findViewById(R.id.play_assistant_button).setOnClickListener(assi::requestClick);
        findViewById(R.id.stop_assistant_button).setOnClickListener(assi::abandonClick);
        // notification focus
        findViewById(R.id.play_notification_button).setOnClickListener(notification::requestClick);
        findViewById(R.id.stop_notification_button).setOnClickListener(notification::abandonClick);
        // ext voice focus
        findViewById(R.id.play_ext_voice_button).setOnClickListener(externalVoice::requestClick);
        findViewById(R.id.stop_ext_voice_button).setOnClickListener(externalVoice::abandonClick);
        // ktv focus
        findViewById(R.id.play_ktv_button).setOnClickListener(mOnClickHandler.playOnClick());
        findViewById(R.id.stop_ktv_button).setOnClickListener(mOnClickHandler.stopOnClick());
        findViewById(R.id.abandon_all_focus).setOnClickListener(mOnClickHandler.stopAllFocusOnClick());

//        findViewById(R.id.play_media_button).setOnClickListener(mOnClickHandler.playOnClick());
//        findViewById(R.id.stop_media_button).setOnClickListener(mOnClickHandler.stopOnClick());
//        findViewById(R.id.play_navi_button).setOnClickListener(mOnClickHandler.playOnClick());
//        findViewById(R.id.stop_navi_button).setOnClickListener(mOnClickHandler.stopOnClick());
//        findViewById(R.id.play_assistant_button).setOnClickListener(mOnClickHandler.playOnClick());
//        findViewById(R.id.stop_assistant_button).setOnClickListener(mOnClickHandler.stopOnClick());
//        findViewById(R.id.play_ktv_button).setOnClickListener(mOnClickHandler.playOnClick());
//        findViewById(R.id.stop_ktv_button).setOnClickListener(mOnClickHandler.stopOnClick());
//        findViewById(R.id.abandon_all_focus).setOnClickListener(mOnClickHandler.stopAllFocusOnClick());
    }

    public void backOnClick(View v) {
        mAudioManager.setParameters("app_set_param_key=value;");
        this.finish();
    }
}
