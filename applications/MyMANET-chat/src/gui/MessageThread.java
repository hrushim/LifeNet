/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package gui;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author hrushi
 */
public class MessageThread extends Thread {

    UnifiedForm form;
    private boolean RUN_FLAG = false;
    int sleepInterval;

    public MessageThread(UnifiedForm f, int sleepTime) {

        sleepInterval = sleepTime;
        form = f;
        PrintDebug.printDebug("Message thr created", UnifiedForm.DEV_DEBUG);

    }

    /* Unit testing done */
    public void run() {
        while (RUN_FLAG) {

            NetMessage message = null;
            message = (new UDPSocketRx(form, form.SRC_PORT)).RxMessage(form.TIMEOUT_CNTRL);
            if (message != null) {
                String srcIP = message.getSrcIP();
                GnsmManager gnsM = new GnsmManager(form.PATH_GNST, form.PATH_HOSTS);
                int index = gnsM.getHostIndex(null, srcIP, null, 2);
                if (form.getSeqNum(index) < message.getSeq()) {
                    form.modifyMainTextArea(gnsM.getHostName(index) + "-> " + message.getPayloadInString());
                    form.setSeqNum(index, message.getSeq());
                } else {
                }
                if (gnsM.getMyIP() != null) {
                    new UDPSocketTx(form, message.getDestIP(), message.getSrcIP(), form.MESS_ACK, message.getSeq(), "ACK", form.DST_PORT);
                }
            } else {
            }
            try {
                Thread.sleep(sleepInterval);
            } catch (InterruptedException ex) {
                Logger.getLogger(BuddiesUpdateThread.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }

    public void setRunning(boolean flag) {
        RUN_FLAG = flag;
    }

    public boolean isRunning() {
        return RUN_FLAG;
    }
}
