package com.chinatsp.audiolp;

import android.content.Context;
import android.media.AudioManager;
import android.util.Base64;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import androidx.annotation.NonNull;

class AudioCalControl {
    static private final String TAG = LogUtils.TAG + AudioCalControl.class.getSimpleName() + " : ";
    private final AudioManager mAudioManager;

    static private  int mPersist = 0;
    static private int mTopoId = 0;
    static private int mAppType = 0;
    static private int mModuleId = 0;
    static private int mParamId = 0;
    static private int mInstanceId = 0;
    static private int mSndDevId = 0;
    static private int mDevId = 0;
    static private int mCalType = 0;
    static private int mSampleRate = 48000;
    static private MsgPrint mMsg = null;

    public AudioCalControl(Context context, int calType, int topoId,int appType,
                           int moduleId, int paramId, int instanceId, int sndDevId, int devId, int sampleRate) {
        mTopoId = topoId;
        mAppType = appType;
        mModuleId = moduleId;
        mParamId = paramId;
        mInstanceId = instanceId;
        mSndDevId = sndDevId;
        mDevId = devId;
        mCalType = calType;
        mSampleRate = sampleRate;
        mAudioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
    }

    @NonNull
    @Override
    public String toString() {
        return  calParamKeyValueString();
    }


    public AudioCalControl(Context context, int calType, int topoId,int appType, int moduleId, int paramId, int instanceId, int sndDevId) {
        this(context, calType, topoId, appType, moduleId, paramId, instanceId, sndDevId, mDevId, mSampleRate);
    }

    public AudioCalControl(Context context, int calType, int topoId,int appType, int moduleId, int paramId, int sndDevId) {
        this(context, calType, topoId, appType, moduleId, paramId, mInstanceId, sndDevId, mDevId, mSampleRate);
    }

    public void setPersist(int persist) { mPersist = persist; }

    public void setModuleID(int moduleId) { mModuleId = moduleId; }

    public void setParamID(int paramId) { mParamId = paramId; }

    private String keyAndValueToString(String key, int value) {
        return key + "=" + value + ";";
    }

    private String keyAndValueToHexString(String key, int value) {
        return key + "=" + "0x" + Integer.toHexString(value) + ";";
    }

    private String calParamKeyValueString() {
        return  keyAndValueToString("cal_persist", mPersist) +
                keyAndValueToString("cal_caltype", mCalType) +
                keyAndValueToString("cal_devid", mDevId) +
                keyAndValueToString("cal_apptype", mAppType) +
                keyAndValueToString("cal_instanceid", mInstanceId) +
                keyAndValueToString("cal_samplerate", mSampleRate) +
                keyAndValueToString("cal_snddevid", mSndDevId) +
                keyAndValueToHexString("cal_topoid", mTopoId) +
                keyAndValueToHexString("cal_moduleid", mModuleId) +
                keyAndValueToHexString("cal_paramid", mParamId);
    }

    public void setCalData(byte[] data) {
        String key = calParamKeyValueString();
        String calData = "cal_data=" + b64encode(data) + ";";
        key += calData;
        log("setCalData " + key, "");
        if (mAudioManager != null)
            mAudioManager.setParameters(key);
        else
            log("error: mAudioManager == null " + key);
    }

    public byte[] getCalData() {
        String key = calParamKeyValueString();
        key += "cal_data=0";
        String value;
        if (mAudioManager == null) {
            log("error: mAudioManager == null " + key);
            return new byte[0];
        }
        log(key);
        value = mAudioManager.getParameters(key);
        log(value);

        // 查询执行结果
        String calResultStr = getValueByKey("cal_result", value);
        if (calResultStr.equals("")) {
            log("error: cal_result = null");
            return new byte[0];
        }

        int calResult = Integer.parseInt(calResultStr);
        if (calResult != 0) {
            if (calResult == -100)
                log("error: active device/stream not found (result=-100) for topology " + Integer.toHexString(mTopoId) +
                        " and appType " + mAppType + " and sndDevice " + mSndDevId);
            else
                log("error: get adsp param result = " + calResult + ". " + this);
            return new byte[0];
        }

        String calDataStr = getValueByKey("cal_data", value);
        byte[] rawCalData = b64decode(calDataStr);
        if (rawCalData.length == 0) {
            log("error. cal_data length = 0");
            return rawCalData;
        }
        printByteArray(rawCalData);

        int calDataLen = getCalDataLength(rawCalData);
        log("data length:" + calDataLen, "");

        byte[] paramVal = getParamValueByRawData(rawCalData, calDataLen);
        printByteArray(paramVal);
        return paramVal;
    }

    private int getCalDataLength(byte[] bytes) {
        ByteBuffer byteBuf = ByteBuffer.allocate(Integer.BYTES);
        byteBuf.order(ByteOrder.LITTLE_ENDIAN);
        byteBuf.put(bytes, 0, 4);
        return byteBuf.getInt(0);
    }

    private byte[] getParamValueByRawData(byte[] rawData, int dataLength) {
        if (dataLength + 4 > rawData.length) {
            return rawData;
        }
        ByteBuffer bf = ByteBuffer.allocate(dataLength);
        bf.put(rawData, 4, dataLength);
        return bf.array();
    }

    public String getValueByKey(String key, String keyValuePairs) {
        String[] res;
        String[] sparams = keyValuePairs.split(";");
        for (String sparam : sparams) {
            res = sparam.split("=");
            if (res[0].equalsIgnoreCase(key)) {
                if (res.length <= 1)
                    return "";
                return res[1];
            }
        }
        return "";
    }

    private void printByteArray(byte[] bytes) {
        StringBuilder sb = new StringBuilder();
        sb.append("param value:");
        for (byte aByte : bytes) {
            sb.append(Integer.toHexString((int) aByte));
            sb.append(" ");
        }
        log(sb.toString(), "");
    }

    private void printIntArray(int[] ints) {
        StringBuilder sb = new StringBuilder();
        for (int i : ints) {
            sb.append(Integer.toHexString(i));
            sb.append(" ");
        }
        log(sb.toString(), "");
    }

    public String b64encode(byte[] params) {
        if(params.length == 0) {
            return "";
        }
        return Base64.encodeToString(params, 0, params.length, Base64.NO_WRAP);
    }

    public byte[] b64decode(String value) {
        if(value.equals("")) {
            return new byte[0];
        }
        return Base64.decode(value, Base64.NO_WRAP);
    }



    public static void registerMsgPrint(@NonNull MsgPrint msg) {
        mMsg = msg;
    }

    public interface MsgPrint {
        void log(String s);
    }

    private static void log(String s) {
        if (mMsg != null)
            mMsg.log(s);
        System.out.println(TAG + s);
    }

    private static void log(String s, String __unused) {
        System.out.println(TAG + s);
    }
}
