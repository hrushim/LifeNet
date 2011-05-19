/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package gui;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import sun.misc.BASE64Decoder;

/**
 *
 * @author hrushi
 */
public class UDPSocketRx {

    UnifiedForm form;
    int srcPort;
    DatagramSocket socketRx = null;
    int readSize;

    public UDPSocketRx(UnifiedForm f, int srcP) {
        form = f;
        srcPort = srcP;
    }

    /* Unit tested with UDPSocketTx */
    public NetMessage RxMessage(int timeOut) {
        NetMessage message = null;
        ByteArrayInputStream byteIStream;
        ObjectInputStream iStream;
        byte[] buf = new byte[4096];
        DatagramPacket packet = new DatagramPacket(buf, buf.length);
        DatagramSocket socketRcv = null;

        try {
            socketRcv = new DatagramSocket(srcPort);
            System.out.flush();
        } catch (SocketException exc) {
            if (form.DEV_DEBUG) {
                exc.printStackTrace();
            }
        }


        if (socketRcv != null) {
            try {
                socketRcv.setSoTimeout(timeOut);
            } catch (SocketException exc) {
                if (form.DEV_DEBUG) {
                    exc.printStackTrace();
                }
                socketRcv.close();

            }
            try {
                socketRcv.receive(packet);
                buf = new BASE64Decoder().decodeBuffer(new String(buf));
            } catch (IOException exc) {
                if (form.DEV_DEBUG) {
                    exc.printStackTrace();
                }
                socketRcv.close();
            }

            if (!socketRcv.isClosed()) {

                byteIStream = new ByteArrayInputStream(buf);
                if (byteIStream != null) {
                    try {
                        iStream = new ObjectInputStream(byteIStream);
                        try {
                            message = (NetMessage) iStream.readObject();

                        } catch (ClassNotFoundException exc) {
                            if (form.DEV_DEBUG) {
                                exc.printStackTrace();
                            }
                        }
                    } catch (IOException exc) {
                        if (form.DEV_DEBUG) {
                            exc.printStackTrace();
                        }
                    }

                }
            }
        }
        if (!socketRcv.isClosed()) {
            socketRcv.close();
        }
        return message;
    }
}
