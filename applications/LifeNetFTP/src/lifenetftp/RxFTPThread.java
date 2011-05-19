/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package lifenetftp;

import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;

/**
 *
 * @author hrushi
 */
public class RxFTPThread extends Thread {

    int seqNum;
    boolean RUN_FLAG = true;
    MainForm form;
    int srcPortNo;
    int destPortNo;
    int timeOut;
    int timeInterval;
    String fileName;
    int fileSize;
    int blockSize;
    int recvSize = 0;
    long startTimeInMs;
    long endTimeInMs;
    int maxTicks = Main.MAX_TICKS_RXFTP;
    UDPTxAck udpTxAck;

    public RxFTPThread(MainForm f, int seq, int destPort, int srcPort, int tOut, int interval, String fName, int fSize, int bSize) {
        form = f;
        seqNum = seq;
        destPortNo = destPort;
        srcPortNo = srcPort;
        timeOut = tOut;
        timeInterval = interval;
        fileName = fName;
        fileSize = fSize;
        blockSize = bSize;
        FTPDebug.printDebug("RxFTPThread created, srcPort " + srcPort + " destPort " + destPort + " for " + fileName + " (" + fSize + ")", Main.DEBUG);
    }

    public void run() {

        int numTicks = 0;
        int initFwdVd, initRevVd;


        FileOutputStream ofStream = null;
        try {
            ofStream = new FileOutputStream(form.getDownloadDirName() + "/" + fileName);
        } catch (FileNotFoundException ex) {
            Logger.getLogger(RxFTPThread.class.getName()).log(Level.SEVERE, null, ex);
        }
        DataOutputStream odStream = new DataOutputStream(ofStream);


        while (RUN_FLAG && ofStream != null && odStream != null) {

            FTPDebug.printDebug("RxFTPThread running...", Main.DEBUG);

            if (seqNum == Main.SEQ_DATA) {
                startTimeInMs = System.currentTimeMillis();
            }

            NetMessage message;
            FTPDebug.printDebug("Sending message " + Main.MSGCODE_DATA_REQUEST + " from rxFTPThread to " + destPortNo, Main.DEBUG);

            String myIp = form.getMyIP();
            String buddyIP = form.getBuddyIP();
            if (myIp != null && buddyIP != null) {
                message = new UDPTxAck(form, form.getMyIP(), form.getBuddyIP(), Main.MSGCODE_DATA_REQUEST, seqNum, Main.MSG_DATA_REQUEST + String.valueOf(blockSize) + ";", destPortNo, srcPortNo, Main.NUM_TICKS_DATA, Main.DATA_TIMEOUT).SendUDPTxAck();
                if (message == null) {
                    numTicks++;
                    FTPDebug.printDebug("Null message received", Main.DEBUG);
                    if (numTicks > maxTicks) {
                        JOptionPane.showMessageDialog(form, Main.ERROR_DATA_REQUEST);
                        break;
                    }

                } else {

                    FTPDebug.printDebug("Received message by rxFTPThread", Main.DEBUG);
                    if (message.getPayloadInString().equals(Main.MSG_EOF)) {
                        endTimeInMs = System.currentTimeMillis();
                        double timeDiffInMs = endTimeInMs - startTimeInMs;
                        double bandWidth = fileSize / (timeDiffInMs / 1000);
                        FTPDebug.printDebug("Transfer time = " + timeDiffInMs / 1000, Main.DEBUG);
                        FTPDebug.printDebug("Bandwidth = " + bandWidth, Main.DEBUG);
                        form.setBandwidth(bandWidth);
                        form.lockRxFTP(false);
                        numTicks = 0;
                        new UDPSocketTx(form, message.getDestIP(), message.getSrcIP(), Main.MSGCODE_DATA_ACK, message.getSeq(), Main.MSG_EOF, destPortNo);
                        break;
                    } else {
                        seqNum++;
                        recvSize += message.getPayloadLength();

                        FTPDebug.printDebug("Bytes received " + message.getPayloadLength() + " total bytes " + recvSize, Main.DEBUG);
                        FTPDebug.printDebug("Setting progress bar value " + ((recvSize * 100) / (fileSize - 1)) + " Recvsize " + message.getPayloadLength(), Main.DEBUG);
                        form.setProgressBarValue((recvSize * 100) / (fileSize - 1));
                        try {
                            odStream.write(message.getPayload(), 0, message.getPayload().length);
                        } catch (IOException exc) {
                            if (Main.DEBUG) {
                                exc.printStackTrace();
                            }
                        }
                        numTicks = 0;

                        endTimeInMs = System.currentTimeMillis();
                        double timeDiffInMs = endTimeInMs - startTimeInMs;
                        double bandWidth = recvSize / (timeDiffInMs / 1000);
                        form.setBandwidth(bandWidth);
                    }
                }

                try {

                    GnsmManager gnsManage = new GnsmManager(Main.GNSTFILEPATH, Main.HOSTSFILEPATH);
                    if (form.getMyName() != null && form.getBuddyName() != null) {
                        int fwdVd = gnsManage.getVD(form.getMyName(), form.getBuddyName());
                        int revVd = gnsManage.getVD(form.getBuddyName(), form.getMyName());

                        if (fwdVd > 0 && revVd > 0) {
                            if (fwdVd <= Main.RATE_CNTRL_VD_THRESHOLD && revVd <= Main.RATE_CNTRL_VD_THRESHOLD) {
                                if (timeInterval > Main.RATE_CNTRL_MIN_DELAY) {
                                    timeInterval -= Main.RATE_CNTRL_DEC_INTERVAL;
                                }
                            } else {
                                if (timeInterval < Main.RATE_CNTRL_MAX_DELAY) {
                                    timeInterval += Main.RATE_CNTRL_INC_INTERVAL;
                                }
                            }
                            FTPDebug.printDebug("\n\n\nFwdVD = " + fwdVd + "\nrevVD" + revVd + "Data thread interval = " + timeInterval + "\n\n\n", Main.DEBUG);
                        }
                    }

                    Thread.sleep(timeInterval);
                } catch (InterruptedException ex) {
                    Logger.getLogger(RxFTPThread.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        }
        if (odStream != null) {
            try {
                odStream.close();
            } catch (IOException ex) {
                Logger.getLogger(RxFTPThread.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        if (ofStream != null) {
            try {
                ofStream.close();
            } catch (IOException ex) {
                Logger.getLogger(RxFTPThread.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        FTPDebug.printDebug("Coming out of RxFTPThread", Main.DEBUG);
        //udpTxAck.destroyTxAckListener();
        //}
    }

    public void setRunning(boolean choice) {
        RUN_FLAG = choice;
    }

    public boolean isRunning() {
        return RUN_FLAG;
    }
}
