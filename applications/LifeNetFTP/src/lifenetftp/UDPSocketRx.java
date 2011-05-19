/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package lifenetftp;

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

    MainForm form;
    int srcPort;
    DatagramSocket socketRx = null;
    int readSize;

    public UDPSocketRx(MainForm f, int srcP) {
        form = f;
        srcPort = srcP;
    }

    public boolean createUDPRxListener() {
        try {
            socketRx = new DatagramSocket(srcPort);
            if (Main.DEBUG) {
                FTPDebug.printDebug("Socketfunctions listener created at port in UDPSocketRx" + srcPort, Main.DEBUG);
                System.out.flush();
            }
        } catch (SocketException exc) {
            if (Main.DEBUG) {
                exc.printStackTrace();
            }
            return false;
        }
        return true;
    }

    public void destroyUDPRxListener() {
        if (socketRx != null) {
            if (!socketRx.isClosed()) {
                socketRx.close();
            }
        }
    }

    public NetMessage rxMessageOnCreatedSocket(int timeOut) {
        NetMessage message = null;
        ByteArrayInputStream byteIStream;
        ObjectInputStream iStream;
        byte[] buf = new byte[Main.MAXBUF];
        DatagramPacket packet = new DatagramPacket(buf, buf.length);

        if (socketRx != null) {
            try {
                socketRx.setSoTimeout(timeOut);
            } catch (SocketException exc) {
                if (Main.DEBUG) {
                    exc.printStackTrace();
                }
                socketRx.close();
            }
            try {
                socketRx.receive(packet);
                buf = new BASE64Decoder().decodeBuffer(new String(buf));
            } catch (IOException exc) {
                if (Main.DEBUG) {
                    exc.printStackTrace();
                }
                socketRx.close();
            }
            if (!socketRx.isClosed()) {
                byteIStream = new ByteArrayInputStream(buf);
                if (byteIStream != null) {
                    try {
                        iStream = new ObjectInputStream(byteIStream);
                        try {
                            message = (NetMessage) iStream.readObject();
                        } catch (ClassNotFoundException exc) {
                            if (Main.DEBUG) {
                                exc.printStackTrace();
                            }
                        }
                    } catch (IOException exc) {
                        if (Main.DEBUG) {
                            exc.printStackTrace();
                        }
                    }
                    if (message != null) {
                        if (Main.DEBUG) {
                            FTPDebug.printDebug(">> Incoming " + String.valueOf(message.getType()), Main.DEBUG);
                        }
                    }
                }
            }
        }
        /*
        if (socketRx != null) {
            if (!socketRx.isClosed()) {
                socketRx.close();
            }
        }
         */
        return message;
    }

    public NetMessage RxMessage(int timeOut) {
        NetMessage message = null;
        ByteArrayInputStream byteIStream;
        ObjectInputStream iStream;
        byte[] buf = new byte[Main.MAXBUF];
        DatagramPacket packet = new DatagramPacket(buf, buf.length);
        DatagramSocket socketRcv = null;

        try {
            socketRcv = new DatagramSocket(srcPort);
            if (Main.DEBUG) {
                FTPDebug.printDebug("Socketfunctions listener created at port in UDPSocketRx" + srcPort, Main.DEBUG);
                System.out.flush();
            }
        } catch (SocketException exc) {
            if (Main.DEBUG) {
                exc.printStackTrace();
            }
        }


        if (socketRcv != null) {
            try {
                socketRcv.setSoTimeout(timeOut);
            } catch (SocketException exc) {
                if (Main.DEBUG) {
                    exc.printStackTrace();
                }
                socketRcv.close();
            }
            try {
                socketRcv.receive(packet);
                buf = new BASE64Decoder().decodeBuffer(new String(buf));
            } catch (IOException exc) {
                if (Main.DEBUG) {
         //           exc.printStackTrace();
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
                            if (Main.DEBUG) {
                                exc.printStackTrace();
                            }
                        }
                    } catch (IOException exc) {
                        if (Main.DEBUG) {
                            exc.printStackTrace();
                        }
                    }
                    if (message != null) {
                        if (Main.DEBUG) {
                            FTPDebug.printDebug(">> Incoming " + String.valueOf(message.getType()), Main.DEBUG);
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
