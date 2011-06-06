/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.app.lifeNet;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author hrushi
 */
public class BuddiesUpdateThread extends Thread {

    LifeNet form;
    private boolean RUN_FLAG = false;
    int timeout;
    String fileName;

    public BuddiesUpdateThread(LifeNet f, int sleepTime, String filePath) {

        fileName = filePath;
        timeout = sleepTime;
        form = f;

    }

    /* Unit testing done */
    /*
     * Read elements from the hosts file in a loop and update the names list accordingly
     */
    public void run() {

        while (RUN_FLAG) {
/*
            GnsmManager gnsM = new GnsmManager(LifeNet.PATH_GNST, fileName);

            for (int i = 0; i < gnsM.getHostCount(); i++) {
                if (gnsM.getMyName().trim().equals(gnsM.getHostName(i)) == false) {
                    if (!gnsM.getHostName(i).startsWith("router")) {
                        form.checkAndAddInNamesList(gnsM.getHostName(i));
                    }
                }
            }

            //TODO: remove elements if not present

            form.setMyIP(gnsM.getMyIP());
            form.setMyName(gnsM.getMyName());
            form.setMyMac(gnsM.getMyMac());

 *
 */
            try {
                Thread.sleep(timeout);
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
