package com.chinatsp.audcal;

import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;

public class AuttAckPacket {
    private static final Map<Integer, PacketInfo> piMap = new HashMap<>();

    static {
        piMap.put(0, new PacketInfo(8, 7, new byte[] {(byte)0xFF}));
        piMap.put(1, new PacketInfo(6, 4, new byte[] {(byte)0x02, 0x0}));
        piMap.put(2, new PacketInfo(17, 16, new byte[] {(byte)0xFF}));
        piMap.put(3, new PacketInfo(16, 16, new byte[0]));
        piMap.put(4, new PacketInfo(17, 16, new byte[] {(byte)0xFF}));
        piMap.put(5, new PacketInfo(16, 16, new byte[0]));
        piMap.put(6, new PacketInfo(9, 8, new byte[] {(byte)0xFF}));
        piMap.put(8, new PacketInfo(13, 12, new byte[] {(byte)0xFF}));
        piMap.put(9, new PacketInfo(9, 8, new byte[] {(byte)0xFF}));
        piMap.put(11, new PacketInfo(12, 8, new byte[] {(byte)0x0, 0x0, 0x0, 0x0}));
        piMap.put(12, new PacketInfo(12, 8, new byte[] {(byte)0x0, 0x0, 0x0, 0x0}));
        piMap.put(15, new PacketInfo(5, 5, new byte[0] ));
    }

    public static byte[] errorMsg(byte[] auttPkt) {
        if (auttPkt.length < 2)
            return new byte[0];
        PacketInfo pi = piMap.get(Byte.toUnsignedInt(auttPkt[1]));
        if (pi == null)
            return new byte[0];

        ByteBuffer bf = ByteBuffer.allocate(pi.mPktLen);
        bf.put(auttPkt, 0, pi.mAuttDataLen);
        if (pi.mLastDataLen > 0)
            bf.put(pi.mLastData, 0, pi.mLastDataLen);
        return bf.array();
    }


    private static class PacketInfo {
        int mPktLen;
        int mAuttDataLen;
        int mLastDataLen;
        byte[] mLastData;
        public PacketInfo(int pktLen, int auttDataLen, byte[] lastData) {
            mPktLen = pktLen;
            mAuttDataLen = auttDataLen;
            mLastDataLen = mPktLen - mAuttDataLen;
            mLastData = lastData;
        }
    }
}
