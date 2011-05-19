/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package lifenetftp;

import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Arrays;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;

/**
 *
 * @author hrushi
 */
public class TxFTPThread extends Thread {

    private boolean RUN_FLAG = true;
    MainForm form;
    int destPortNo;
    int srcPortNo;
    int timeOut;
    int timeInterval;
    int maxTicks = Main.MAX_TICKS_TXFTP;
    int seqNum, prevSeqNum = 1;
    String fileName;
    //UDPSocketRx udpSockRx;

    public TxFTPThread(MainForm f, int destPort, int srcPort, int tOut, int interval, String fName) {
        form = f;
        destPortNo = destPort;
        srcPortNo = srcPort;
        timeOut = tOut;
        timeInterval = interval;
        fileName = fName;
        FTPDebug.printDebug("TxFTPThread created, srcPort " + srcPort + " destPort " + destPort + " for " + fileName, Main.DEBUG);
    }

    public void run() {
        int lengthRead = 0;
        int length = 0;
        int numTicks = 0;

        FileInputStream ifStream = null;
        DataInputStream idStream = null;


        try {
            ifStream = new FileInputStream(form.getUploadDirName() + "/" + fileName);
        } catch (FileNotFoundException ex) {
            Logger.getLogger(TxFTPThread.class.getName()).log(Level.SEVERE, null, ex);
        }
        idStream = new DataInputStream(ifStream);

        byte[] payload2 = null;

        while (RUN_FLAG && idStream != null) {
            FTPDebug.printDebug("TxFTPThread running...", Main.DEBUG);

            //NetMessage message = udpSockRx.rxMessageOnCreatedSocket(timeOut);
            NetMessage message = (new UDPSocketRx(form, srcPortNo)).RxMessage(timeOut);
            if (message != null) {
                if (message.getType() == Main.MSGCODE_DATA_REQUEST) {
                    seqNum = message.getSeq();
                    FTPDebug.printDebug("Received message by txFTPThread seq=" + seqNum, Main.DEBUG);
                    FTPDebug.printDebug("Sending message " + Main.MSGCODE_DATA_ACK + " from txFTPThread to " + destPortNo, Main.DEBUG);
                    String[] strArr = message.getPayloadInString().split(";");
                    byte[] payload1 = new byte[Integer.parseInt(strArr[1])];

                    try {
                        if (seqNum == (prevSeqNum + 1)) {
                            length = idStream.read(payload1, 0, Integer.parseInt(strArr[1]));
                            FTPDebug.printDebug("Sending message of length = " + length, Main.DEBUG);
                            if (length != -1) {
                                payload2 = Arrays.copyOf(payload1, length);
                            }
                        } else {
                            length = 0;
                        }
                    } catch (IOException ex) {
                        Logger.getLogger(TxFTPThread.class.getName()).log(Level.SEVERE, null, ex);
                    }
                    if (seqNum >= prevSeqNum) {
                        if (length >= 0) {
                            lengthRead += length;
                            FTPDebug.printDebug("Lengthread = " + lengthRead, Main.DEBUG);
                            if (payload2 != null) {
                                new UDPSocketTx(form, message.getDestIP(), message.getSrcIP(), Main.MSGCODE_DATA_ACK, seqNum, payload2, destPortNo);
                            }
                        } else {
                            new UDPSocketTx(form, message.getDestIP(), message.getSrcIP(), Main.MSGCODE_DATA_ACK, message.getSeq(), Main.MSG_EOF, destPortNo);
                        }
                        prevSeqNum = seqNum;
                    }
                    numTicks = 0;
                }

            } else if (length <= 0) {
                numTicks++;
                FTPDebug.printDebug("Null message received and incrementing ticks " + numTicks, Main.DEBUG);
                try {
                    if (ifStream.available() == 0) {
                        if (numTicks > Main.MAX_TICKS_FTP_COMPLETE) {
                            JOptionPane.showMessageDialog(form, Main.NOTIFICATION_TXFTP_END);
                            try {
                                if (idStream != null) {
                                    idStream.close();
                                }
                                if (ifStream != null) {
                                    ifStream.close();
                                }
                            } catch (IOException exc) {
                                FTPDebug.printDebug("IO exception while closing streams", Main.DEBUG);
                            }
                            form.lockTxFTP(false);
                            break;
                        }
                    } else {
                        if (numTicks > maxTicks) {
                            JOptionPane.showMessageDialog(form, Main.NOTIFICATION_TXFTP_END);
                            try {
                                if (idStream != null) {
                                    idStream.close();
                                }
                                if (ifStream != null) {
                                    ifStream.close();
                                }
                            } catch (IOException exc) {
                                FTPDebug.printDebug("IO exception while closing streams", Main.DEBUG);
                            }
                            form.lockTxFTP(false);
                            break;
                        }
                    }
                } catch (IOException exc) {
                    FTPDebug.printDebug("IO exception finding available bytes from stream", Main.DEBUG);
                }

                try {
                    Thread.sleep(timeInterval);
                } catch (InterruptedException ex) {
                    Logger.getLogger(TxFTPThread.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            FTPDebug.printDebug("Coming out of TxFTPThread", Main.DEBUG);
        }

    }

    void setRunning(boolean choice) {
        RUN_FLAG = choice;
    }

    public boolean isRunning() {
        return RUN_FLAG;
    }
}
