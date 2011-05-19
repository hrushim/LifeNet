/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package lifenetftp;

import java.util.Arrays;

/**
 *
 * @author hrushi
 */
public class UDPTxAck {

    MainForm form;
    String srcIP;
    String destIP;
    int mType;
    int seqNum;
    byte[] payload;
    int destPortNo;
    int srcPortNo;
    int numTicks;
    int timeOut;
    UDPSocketRx udpSockRx;

    public UDPTxAck(MainForm f, String srcIPv4, String destIPv4, int type, int seq, String pay_load, int destPort, int srcPort, int ticks, int tOut) {
        loadObjectData(f, srcIPv4, destIPv4, type, seq, pay_load.getBytes(), destPort, srcPort, ticks, tOut);
    }

    public UDPTxAck(MainForm f, String srcIPv4, String destIPv4, int type, int seq, byte[] pay_load, int destPort, int srcPort, int ticks, int tOut) {
        loadObjectData(f, srcIPv4, destIPv4, type, seq, pay_load, destPort, srcPort, ticks, tOut);
    }

    private void loadObjectData(MainForm f, String srcIPv4, String destIPv4, int type, int seq, byte[] pay_load, int destPort, int srcPort, int ticks, int tOut) {
        form = f;
        srcIP = srcIPv4;
        destIP = destIPv4;
        mType = type;
        seqNum = seq;
        payload = Arrays.copyOf(pay_load, pay_load.length);
        destPortNo = destPort;
        srcPortNo = srcPort;
        numTicks = ticks;
        timeOut = tOut;
    }

    public boolean createTxAckListener() {
        udpSockRx = new UDPSocketRx(form, srcPortNo);
        return udpSockRx.createUDPRxListener();
    }

    public void destroyTxAckListener() {
        udpSockRx.destroyUDPRxListener();
    }

    public NetMessage SendUDPTxAckOverCreatedListener() {
        if (Main.DEBUG) {
            printObjectData();
        }
        for (int currTicks = 0; currTicks < numTicks; currTicks++) {
            new UDPSocketTx(form, srcIP, destIP, mType, seqNum, payload, destPortNo);
            NetMessage message = udpSockRx.rxMessageOnCreatedSocket(timeOut);
            if (message != null) {
                if (message.getSeq() == seqNum) {
                    FTPDebug.printDebug("Valid ack obtained in TxAck: " + message.getSeq(), Main.DEBUG);
                    return message;
                } else {
                    FTPDebug.printDebug("Continuing in TxAck as no ack obtained", Main.DEBUG);
                    continue;
                }
            } else {
                FTPDebug.printDebug("Continuing in TxAck as no ack obtained", Main.DEBUG);
                continue;
            }
        }
        return null;
    }

    public void printObjectData() {
        System.out.println("==== UDPTxAck Object ==== ");
        System.out.println("SrcIP: " + srcIP);
        System.out.println("DestIP: " + destIP);
        System.out.println("SrcPortNo: " + srcPortNo);
        System.out.println("DestPortNo: " + destPortNo);
        System.out.println("Type: " + mType);
        System.out.println("SeqNum: " + seqNum);
//        System.out.println("Payload: " + new String(payload));
        System.out.println("NumTicks: " + numTicks);
        System.out.println("TimeOut: " + timeOut);
        System.out.println("==== End UDPTxAck Object ==== ");
    }

    public NetMessage SendUDPTxAck() {

        if (Main.DEBUG) {
            printObjectData();
        }
        for (int currTicks = 0; currTicks < numTicks; currTicks++) {
            new UDPSocketTx(form, srcIP, destIP, mType, seqNum, payload, destPortNo);
            NetMessage message = (new UDPSocketRx(form, srcPortNo)).RxMessage(timeOut);
            if (message != null) {
                if (message.getSeq() == seqNum) {
                    FTPDebug.printDebug("Valid ack obtained in TxAck: " + message.getSeq(), Main.DEBUG);
                    return message;
                } else {
                    FTPDebug.printDebug("Continuing in TxAck as no ack obtained", Main.DEBUG);
                    continue;
                }
            } else {
                FTPDebug.printDebug("Continuing in TxAck as no ack obtained", Main.DEBUG);
                continue;
            }
        }
        return null;
    }
}
