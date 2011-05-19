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
public class DisplayPaintThread extends Thread {

    DisplayForm form;
    private boolean RUN_FLAG = false;
    int sleepInterval;

    public DisplayPaintThread(DisplayForm f, int sleepTime) {
        form = f;
        sleepInterval = sleepTime;
    }

    public void run() {
        while (RUN_FLAG) {

            form.repaint();

            try {
                Thread.sleep(sleepInterval);
            } catch (InterruptedException ex) {
                Logger.getLogger(DisplayPaintThread.class.getName()).log(Level.SEVERE, null, ex);
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
