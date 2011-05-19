/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package gui;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import sun.misc.BASE64Encoder;

/**
 *
 * @author hrushi
 */
public class UDPSocketTx {

    /* Unit testing done */
    public UDPSocketTx(UnifiedForm form, String srcIP, String destIP, int mType, long seqNum, String payload, int destPortNo) {
        sendOverSocket(form, srcIP, destIP, mType, seqNum, payload.getBytes(), destPortNo);
    }

    /* Unit testing done */
    public UDPSocketTx(UnifiedForm form, String srcIP, String destIP, int mType, long seqNum, byte[] payload, int destPortNo) {
        sendOverSocket(form, srcIP, destIP, mType, seqNum, payload, destPortNo);
    }

    /* Unit testong done */
    private void sendOverSocket(UnifiedForm form, String srcIP, String destIP, int mType, long seqNum, byte[] payload, int destPortNo) {
        NetMessage message = null;
        InetAddress ipAdd = null;
        DatagramPacket packet = null;
        ByteArrayOutputStream byteStream;
        ObjectOutputStream oStream;
        byte[] buf;
        DatagramSocket socketSend = null;

        message = new NetMessage(srcIP.toCharArray(), destIP.toCharArray(), seqNum, payload, mType);
        if (message != null) {
            try {
                socketSend = new DatagramSocket();
            } catch (SocketException exc) {
                if (form.DEV_DEBUG) {
                    exc.printStackTrace();
                }
            }
            try {
                ipAdd = InetAddress.getByName(String.valueOf(destIP));
            } catch (UnknownHostException exc) {
                if (form.DEV_DEBUG) {
                    exc.printStackTrace();
                }
            }

            byteStream = new ByteArrayOutputStream();
            try {
                if (byteStream != null) {
                    oStream = new ObjectOutputStream(byteStream);
                    oStream.writeObject(message);
                    oStream.flush();
                }
            } catch (IOException exc) {
                if (form.DEV_DEBUG) {
                    exc.printStackTrace();
                }
            }

            if (byteStream != null) {
                buf = byteStream.toByteArray();
                buf = new BASE64Encoder().encode(buf).getBytes();
                packet = new DatagramPacket(buf, buf.length, ipAdd, destPortNo);
            }

            try {
                if (packet != null) {
                    socketSend.send(packet);
                    byteStream.flush();
                }
            } catch (IOException exc) {
                if (form.DEV_DEBUG) {
                    exc.printStackTrace();
                }
            }
        }
    }
}
