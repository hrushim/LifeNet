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
public class ReferenceThread extends Thread {

    DisplayForm form;
    private boolean RUN_FLAG = false;
    int sleepInterval;

    public ReferenceThread(DisplayForm f, int sleepTime) {
        form = f;
        sleepInterval = sleepTime;
    }

    /* Unit testing done */
    public void run() {
        while (RUN_FLAG) {

            GnsmManager gnsm = new GnsmManager(form.PATH_GNST, form.PATH_HOSTS);
            form.refIndex1 = gnsm.getHostIndex(gnsm.getMyName(), null, null, 1);
            form.refIndex2 = getFarthestFromNode(form.refIndex1);

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

    /* Unit testing done */
    public int getFarthestFromNode(int index) {
        int retIndex = -1;

        GnsmManager gnsm = new GnsmManager(form.PATH_GNST, form.PATH_HOSTS);

        int max = 0;
        for (int i = 0; i < gnsm.getHostCount(); i++) {
            if (i != index) {
                if (max < gnsm.getVD(index, i)) {
                    max = gnsm.getVD(index, i);
                    retIndex = i;
                }
            }
        }

        return retIndex;
    }
}
