package com.chinatsp.parse;

import android.util.Xml;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;

import androidx.annotation.NonNull;

public class CarAudioPlatformConfig {
    static private String TAG = "CarAudioPlatformConfig ";
    private String NAMESPACE = null;
    private boolean mA2bEnable = false;
    private InputStream mInputStream;
    private VolumeCurveConfig mVolumeCurve;
    private EQConfig mEQConfig;
    private FaderBalanceConfig mFaderBalanceConfig;
    private int mMask = 0;
    private int mOffset = 0;

    private class VolumeCurveConfig {
        private final int mVolumeMaxLevel;
        private final int mVolumeMinLevel;
        private final int mVolumeDefaultLevel;

        private final int[] mVolumeCurve;

        public VolumeCurveConfig(int volumeMax, int volumeMin, int volumeDefault, int[] volumeCurve) {
            mVolumeMaxLevel = volumeMax;
            mVolumeMinLevel = volumeMin;
            mVolumeDefaultLevel = volumeDefault;
            mVolumeCurve = Arrays.copyOf(volumeCurve, volumeCurve.length);
        }

        public int getVolumeMaxLevel() {
            return mVolumeMaxLevel;
        }

        public int getVolumeMinLevel() {
            return mVolumeMinLevel;
        }

        public int getVolumeDefaultLevel() {
            return mVolumeDefaultLevel;
        }

        public int[] getVolumeCurve() {
            return mVolumeCurve;
        }
    }

    private class EQConfig {
        private int mEQBands;
        private int mEQMaxGain;
        private int mEQMixGain;
        private int mEQNumOfPresets;
        private int mEQDefaultMode;
        private int[] mEQCenterFreq;
        private String[] mEQNamesOfPresets;

        public EQConfig(int EQBands, int EQMaxGain, int EQMixGain, int EQNumOfPresets, int EQDefaultMode, int[] EQCenterFreq, String[] EQNamesOfPresets) {
            mEQBands = EQBands;
            mEQMaxGain = EQMaxGain;
            mEQMixGain = EQMixGain;
            mEQNumOfPresets = EQNumOfPresets;
            mEQDefaultMode = EQDefaultMode;
            mEQCenterFreq = EQCenterFreq;
            mEQNamesOfPresets = EQNamesOfPresets;
        }
    }

    private class FaderBalanceConfig {
        private int mFaderLevelRange;
        private int mDefaultFaderValue;
        private int mBalanceLevelRange;
        private int mDefaultBalanceValue;
        private int[] mBalance_Gain;
        private int[] mFader_Gain;

        public FaderBalanceConfig(int faderLevelRange, int defaultFaderValue, int balanceLevelRange, int defaultBalanceValue, int[] balance_Gain, int[] fader_Gain) {
            mFaderLevelRange = faderLevelRange;
            mDefaultFaderValue = defaultFaderValue;
            mBalanceLevelRange = balanceLevelRange;
            mDefaultBalanceValue = defaultBalanceValue;
            mBalance_Gain = balance_Gain;
            mFader_Gain = fader_Gain;
        }
    }

    public CarAudioPlatformConfig(@NonNull String configFile, boolean a2bEnable) {
        mA2bEnable = a2bEnable;
        File file = new File(configFile);
        if (!file.exists()) {
            log( configFile + " not exist!");
        }
        try {
            mInputStream = new FileInputStream(file);
        } catch (Exception e) {
            log(e.toString());
            e.printStackTrace();
        }
    }

    public CarAudioPlatformConfig(@NonNull InputStream inputStream, boolean a2bEnable) {
        mInputStream = inputStream;
        mA2bEnable = a2bEnable;
    }

    private void resetMaskAndOffset() {
        mMask = 0;
        mOffset = 0;
    }

    public void load() throws Exception {
        XmlPullParser parser = Xml.newPullParser();
        parser.setFeature(XmlPullParser.FEATURE_PROCESS_NAMESPACES, NAMESPACE != null);
        parser.setInput(mInputStream, "utf-8");

        parser.nextTag();
        parser.require(XmlPullParser.START_TAG, NAMESPACE, "platform-configs");

        String version = parser.getAttributeValue(NAMESPACE, "Version");
        log("Version = " + version);

        while (parser.next() != XmlPullParser.END_TAG) {
            if (parser.getEventType() != XmlPullParser.START_TAG)
                continue;
            if ("VolumeCurveConfig".equals(parser.getName())) {
                parseVolumeCurveConfig(parser);
            } else if ("EQConfig".equals(parser.getName())) {
                parseEQConfig(parser);
            } else if ("FaderBalanceConfig".equals(parser.getName())) {
                parseFaderBalanceConfig(parser);
            } else {
                skip(parser);
            }
        }
    }

    private int parseIntValue(XmlPullParser parser, String attr) {
        String v = parser.getAttributeValue(NAMESPACE, attr);
        log("parseIntValue " + parser.getName() + " " + attr + " = " + v);
        return Integer.parseInt(v);
    }

    private int parseIntValue(XmlPullParser parser, String attr, int defVal) {
        if (attr.equals(parser.getAttributeName(0))) {
            mMask |= 0x1 << mOffset;
            mOffset ++;
            return parseIntValue(parser, attr);
        }
        return defVal;
    }

    private String skipSpaces(String s) {
        if (s.charAt(0) == ' ') {
            char[] a = s.toCharArray();
            for (int i = 0; i < a.length; i++) {
                if (a[i] == ' ')
                    continue;
                return String.copyValueOf(a, i, a.length - i);
            }
        }
        return s;
    }

    private int[] parseIntArray(XmlPullParser parser, String attr) {
        ArrayList<Integer> value = new ArrayList<>();
        String v = parser.getAttributeValue(NAMESPACE, attr);
        for (String s : v.split(",")) {
            s = skipSpaces(s);
            int val = Integer.parseInt(s);
            value.add(val);
        }
        int[] ret = new int[value.size()];
        for (int i = 0; i < value.size(); i++) {
            ret[i] = value.get(i);
        }
        log("parseIntValue " + parser.getName() + " " + attr + " = " + Arrays.toString(ret));
        return ret;
    }

    private int[] parseIntArray(XmlPullParser parser, String attr, int[] defVal) {
        if (attr.equals(parser.getAttributeName(0))) {
            mMask |= 0x1 << mOffset;
            mOffset ++;
            return parseIntArray(parser, attr);
        }
        return defVal;
    }

    private String[] parseStringArray(XmlPullParser parser, String attr, String[] defVal) {
        if (attr.equals(parser.getAttributeName(0))) {
            mMask |= 0x1 << mOffset;
            mOffset ++;

            String[] v = parser.getAttributeValue(NAMESPACE, attr).split(",");
            log("parseIntValue " + parser.getName() + " " + attr + " = " + Arrays.toString(v));
            return v;
        }
        return defVal;
    }

    private void parseVolumeCurveConfig(XmlPullParser parser) throws Exception {
        int volumeMax = -1;
        int volumeMin = -1;
        int volumeDefault = -1;
        int[] volumeArray = null;
        resetMaskAndOffset();
        while (parser.next() != XmlPullParser.END_TAG) {
            if (parser.getEventType() != XmlPullParser.START_TAG)
                continue;
            if ("item".equals(parser.getName())) {
                volumeMax = parseIntValue(parser, "VolumeMaxLevel", volumeMax);
                volumeMin = parseIntValue(parser, "VolumeMinLevel", volumeMin);
                volumeDefault = parseIntValue(parser, "VolumeDefaultLevel", volumeDefault);
                volumeArray = parseIntArray(parser, "VolumeCurve", volumeArray);
                skip(parser);
            }
        }
        if (mMask != 0xf) {
            throw new Exception("VolumeCurveConfig node: missing attribute, mMask = " + mMask);
        }
//        assert volumeArray != null;
//        if (volumeMax != volumeArray.length - 1) {
//            throw new Exception("volume curve count != volumeMaxLevel + 1");
//        }
        mVolumeCurve = new VolumeCurveConfig(volumeMax, volumeMin, volumeDefault, volumeArray);
    }

    private void parseEQConfig(XmlPullParser parser) throws Exception {
        int eqBands = 0;
        int eqMaxGain = 0;
        int eqMinGain = 0;
        int eqNumOfPresets = 0;
        int eqDefaultMode = 0;
        int[] eqCenterFreq = null;
        String[] eqNamesOfPresets = null;

        resetMaskAndOffset();
        while (parser.next() != XmlPullParser.END_TAG) {
            if (parser.getEventType() != XmlPullParser.START_TAG)
                continue;
            if ("item".equals(parser.getName())) {
                eqBands = parseIntValue(parser, "EQBands", eqBands);
                eqMaxGain = parseIntValue(parser, "EQMaxGain", eqMaxGain);
                eqMinGain = parseIntValue(parser, "EQMixGain", eqMinGain);
                eqNumOfPresets = parseIntValue(parser, "EQNumOfPresets", eqNumOfPresets);
                eqDefaultMode = parseIntValue(parser, "EQDefaultMode", eqDefaultMode);
                eqCenterFreq = parseIntArray(parser, "EQCenterFreq", eqCenterFreq);
                eqNamesOfPresets = parseStringArray(parser, "EQNamesOfPresets", eqNamesOfPresets);
                skip(parser);
            }
        }
        if (mMask != 0x7f) {
            throw new Exception("EQConfig node: missing attribute, mMask = " + mMask);
        }

        assert eqCenterFreq != null;
        if (eqBands != eqCenterFreq.length) {
            throw new Exception("EQConfig node: EQCenterFreq.length(" + eqCenterFreq.length + ") != eqBands(" + eqBands + ")");
        }

        assert eqNamesOfPresets != null;
        if (eqNumOfPresets != eqNamesOfPresets.length) {
            throw new Exception("EQConfig node: eqNamesOfPresets.length(" + eqNamesOfPresets.length + ") != eqNumOfPresets(" + eqNumOfPresets + ")");
        }

        if (eqDefaultMode < 0 || eqDefaultMode >= eqNumOfPresets) {
            throw new Exception("EQConfig node: eqDefaultMode out of range, range:[0, " + eqDefaultMode + ")");
        }

        if (eqMaxGain < eqMinGain) {
            throw new Exception("EQConfig node: eqMaxGain < eqMinGain");
        }
        mEQConfig = new EQConfig(eqBands, eqMaxGain, eqMinGain, eqNumOfPresets, eqDefaultMode, eqCenterFreq, eqNamesOfPresets);
    }

    private void parseFaderBalanceConfig(XmlPullParser parser) throws Exception {
        int faderRange = 0;
        int balanceRange = 0;
        int faderDefault = 0;
        int balanceDefault = 0;
        int[] faderGains = null;
        int[] balanceGains = null;

        resetMaskAndOffset();
        while (parser.next() != XmlPullParser.END_TAG) {
            if (parser.getEventType() != XmlPullParser.START_TAG)
                continue;
            if ("item".equals(parser.getName())) {
                faderRange = parseIntValue(parser, "FaderLevelRange", faderRange);
                faderDefault = parseIntValue(parser, "DefaultFaderValue", faderDefault);
                balanceRange = parseIntValue(parser, "BalanceLevelRange", balanceRange);
                balanceDefault = parseIntValue(parser, "DefaultBalanceValue", balanceDefault);
                faderGains = parseIntArray(parser, "Fader_Gain", faderGains);
                balanceGains = parseIntArray(parser, "Balance_Gain", balanceGains);
                skip(parser);
            }
        }
        if (mMask != 0x3f) {
            throw new Exception("FaderBalanceConfig node: missing attribute, mMask = " + mMask);
        }
        mFaderBalanceConfig = new FaderBalanceConfig(
                faderRange, faderDefault, balanceRange, balanceDefault, balanceGains, faderGains
        );
    }

    private void skip(XmlPullParser parser) throws XmlPullParserException, IOException {
        if (parser.getEventType() != XmlPullParser.START_TAG) {
            throw new IllegalStateException();
        }
        int depth = 1;
        while (depth != 0) {
            switch (parser.next()) {
                case XmlPullParser.END_TAG:
                    depth--;
                    break;
                case XmlPullParser.START_TAG:
                    depth++;
                    break;
            }
        }
    }

    public static void log(String s) {
        System.out.println(TAG + s);
    }

}
