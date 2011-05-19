/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package lifenetftp;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author hrushi
 */
public class BuddiesUpdateThread extends Thread {

    MainForm form;
    private boolean RUN_FLAG = false;
    int timeout;

    public BuddiesUpdateThread(MainForm f, int sleepTime) {
        timeout = sleepTime;
        form = f;
        if (Main.DEBUG) {
            FTPDebug.printDebug("Buddies update thr created", Main.DEBUG);
            System.out.flush();
        }
    }

    public void run() {

        while (RUN_FLAG) {

            try {
                Thread.sleep(timeout);
            } catch (InterruptedException ex) {
                Logger.getLogger(BuddiesUpdateThread.class.getName()).log(Level.SEVERE, null, ex);
            }

            String[] strArr1;
            String[] strSplit1;
            String selName;
            FileHandler buddiesR = new FileHandler(Main.CONFIGFILEPATH);

            selName = form.getBuddyName();

            form.clearBuddiesPanel();

            if (buddiesR.openFile(0)) {
                strArr1 = buddiesR.readIntoArray(); // The file is read into an array of strings
                buddiesR.closeFile();

                /* Add each host from the 'hosts' file into the buddies list of the form */
                for (int i = 0; i < strArr1.length; i++) {

                    strSplit1 = strArr1[i].split(" ");
                    form.addBuddy(strSplit1[0]);
                    FTPDebug.printDebug("Added Buddy " + strSplit1[0], Main.DEBUG);
                    if (strSplit1.length > 2) {
                        if (strSplit1[2].trim().equals(strSplit1[3].trim())) {
                            form.addMyDetails(strSplit1[0], strSplit1[1], strSplit1[2]);
                        }
                    }
                }

            } else {

                /* If the file is not opened succesfully, it is assumed that the failure to open the 'hosts' file is a
                 * result of the network not being active. Hence, it is concluded that the network is inactive and the
                 * relevant error message is shown in the relevant text boxes. */

                form.clearBuddiesPanel();
                form.addBuddy("Network inactive");
                form.addMyDetails("Network inactive", "Network inactive", "Network inactive");
            }

            form.selectBuddy(selName);
            form.repaint();

        }
        if (Main.DEBUG) {
            System.out.println("\nComing out of BuddiesUpdateThread");
            System.out.flush();
        }
    }

    public void setRunning(boolean choice) {
        RUN_FLAG = choice;
    }

    public boolean isRunning() {
        return RUN_FLAG;
    }
}
