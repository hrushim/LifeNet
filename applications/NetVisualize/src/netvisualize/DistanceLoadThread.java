/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package netvisualize;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author hrushi
 */
public class DistanceLoadThread extends Thread {

    DisplayForm form;
    private boolean RUN_FLAG = false;
    int sleepInterval;

    public DistanceLoadThread(DisplayForm f, int sleepTime) {
        form = f;
        sleepInterval = sleepTime;
    }

    /* Unit testing done */
    public void run() {
        while (RUN_FLAG) {

     //       System.out.println("Inside Distance load thread..");
            GnsmManager gnsm = new GnsmManager(form.PATH_GNST, form.PATH_HOSTS);

            for (int i = 0; i < gnsm.getHostCount(); i++) {
                if (i == form.refIndex1) {
                    form.coOrdS[i][0] = 0;
                    form.coOrdS[i][1] = gnsm.getVD(form.refIndex2, i);
                } else if (i == form.refIndex2) {
                    form.coOrdS[i][0] = gnsm.getVD(form.refIndex1, i);
                    form.coOrdS[i][1] = 0;
                } else {
                    form.coOrdS[i][0] = gnsm.getVD(form.refIndex1, i);
                    form.coOrdS[i][1] = gnsm.getVD(form.refIndex2, i);
                }

            }

            try {
                Thread.sleep(sleepInterval);
            } catch (InterruptedException ex) {
                Logger.getLogger(ReferenceThread.class.getName()).log(Level.SEVERE, null, ex);
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
