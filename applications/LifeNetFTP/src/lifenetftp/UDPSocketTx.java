/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package lifenetftp;

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

    public UDPSocketTx(MainForm form, String srcIP, String destIP, int mType, int seqNum, String payload, int destPortNo) {
        sendOverSocket(form, srcIP, destIP, mType, seqNum, payload.getBytes(), destPortNo);
    }

    public UDPSocketTx(MainForm form, String srcIP, String destIP, int mType, int seqNum, byte[] payload, int destPortNo) {
        sendOverSocket(form, srcIP, destIP, mType, seqNum, payload, destPortNo);
    }

    private void sendOverSocket(MainForm form, String srcIP, String destIP, int mType, int seqNum, byte[] payload, int destPortNo) {
        NetMessage message = null;
        InetAddress ipAdd = null;
        DatagramPacket packet = null;
        ByteArrayOutputStream byteStream;
        ObjectOutputStream oStream;
        byte[] buf;
        DatagramSocket socketSend = null;

        message = new NetMessage(srcIP.toCharArray(), destIP.toCharArray(), seqNum, payload, mType);
        if (message != null) {
            if (Main.DEBUG) {
                message.printData();
            }
            try {
                socketSend = new DatagramSocket();
            } catch (SocketException exc) {
                if (Main.DEBUG) {
                    exc.printStackTrace();
                }
            }
            try {
                ipAdd = InetAddress.getByName(String.valueOf(destIP));
            } catch (UnknownHostException exc) {
                if (Main.DEBUG) {
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
                if (Main.DEBUG) {
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
                if (Main.DEBUG) {
                    exc.printStackTrace();
                }
            }

        }
    }
}
