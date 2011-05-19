/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package lifenetftp;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author hrushi
 */
public class ControlThread extends Thread {

    private boolean RUN_FLAG = false;
    MainForm form;
    int srcPortNo;

    public ControlThread(MainForm f, int srcP) {
        form = f;
        srcPortNo = srcP;
        FTPDebug.printDebug("Main Debug " + Main.DEBUG, Main.DEBUG);
    }

    private String getFileList() {
        if (!form.getDownloadDirName().equals("") && !form.getUploadDirName().equals("")) {
            String payload = "";

            String[] command = {"/bin/sh",
                "-c",
                "stat " + form.getUploadDirName() + "/* | grep -A 1 File | awk ' { print $2 } ' "
            };

            Process pr = null;
            try {
                pr = Runtime.getRuntime().exec(command);
            } catch (IOException exc) {
                if (Main.DEBUG) {
                    exc.printStackTrace();
                }
            }
            try {
                if (pr != null) {
                    pr.waitFor();
                }
            } catch (InterruptedException exc) {
                if (Main.DEBUG) {
                    exc.printStackTrace();
                }
            }
            BufferedReader bReader = new BufferedReader(new InputStreamReader(pr.getInputStream()));
            String line = "";

            try {
                while ((line = bReader.readLine()) != null) {
                    if (line.length() > 0) {
                        if (line.contains(form.getUploadDirName() + "/")) {
                            if (Main.DEBUG) {
                                FTPDebug.printDebug(line.split(form.getUploadDirName() + "/")[1].split("'")[0], Main.DEBUG);
                            }
                            payload = payload + line.split(form.getUploadDirName() + "/")[1].split("'")[0] + ":";
                        } else {
                            payload = payload + line.trim() + ";";
                        }
                    }

                }
                bReader.close();
            } catch (IOException exc) {
                if (Main.DEBUG) {
                    exc.printStackTrace();
                }
            }

            return payload;
        } else {
            return null;
        }
    }

    public void run() {

        while (RUN_FLAG) {

            NetMessage message = (new UDPSocketRx(form, srcPortNo)).RxMessage(Main.CTRL_RX_TIMEOUT);
            if (message != null) {
                if (Main.DEBUG) {
                    FTPDebug.printDebug("Received message by control thread", Main.DEBUG);
                    message.printData();
                }
                if (message.getType() == Main.MSGCODE_SEND_FILE_LIST) {
                    String fileList = form.getFileListFromUploadDir();
                    if(Main.DEBUG) {
                        FTPDebug.printDebug(fileList, Main.DEBUG);
                    }
                    if (form.getMyIP() != null && fileList != null && !form.isTxLocked()) {
                        new UDPSocketTx(form, message.getDestIP(), message.getSrcIP(), Main.MSGCODE_SEND_FILE_LIST_ACK, message.getSeq(), getFileList(), Main.CONTROL_ACK_SRC_PORT);
                    }
                } else if (message.getType() == Main.MSGCODE_LOCK_FTP) {
                    if (form.getMyIP() != null) {
                        if (!form.isTxLocked()) {
                            new UDPSocketTx(form, message.getDestIP(), message.getSrcIP(), Main.MSGCODE_LOCK_FTP_ACK_APPROVE, message.getSeq(), Main.MSG_LOCK_FTP_APPROVE, Main.CONTROL_ACK_SRC_PORT);
                            String strArr[] = message.getPayloadInString().split(":");
                            form.lockTxFTP(true);
                            if(Main.DEBUG) {
                                System.out.println("Tx locked! on " + strArr[1]);
                                System.out.flush();
                            }
                            TxFTPThread txFTPThread = new TxFTPThread(form, Main.DATA_ACK_SRC_PORT, Main.DATA_SRC_PORT, Main.DATA_TIMEOUT, Main.DATA_THREAD_INTERVAL, strArr[1]);
                            txFTPThread.start();
                            txFTPThread.setRunning(true);
                        } else {
                            new UDPSocketTx(form, message.getDestIP(), message.getSrcIP(), Main.MSGCODE_LOCK_FTP_ACK_DENY, message.getSeq(), Main.MSG_LOCK_FTP_APPROVE, Main.CONTROL_ACK_SRC_PORT);
                        }
                    }
                }

                try {
                    Thread.sleep(Main.CNTRL_THREAD_INTERVAL);
                } catch (InterruptedException ex) {
                    Logger.getLogger(ControlThread.class.getName()).log(Level.SEVERE, null, ex);
                }
            }

        }
    }

    void setRunning(boolean choice) {
        RUN_FLAG = choice;
    }

    public boolean isRunning() {
        return RUN_FLAG;
    }
}
