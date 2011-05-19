/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package lifenetftp;

/**
 *
 * @author hrushi
 * Used for printing debug statements
 */
public class FTPDebug {

    public static void printDebug(String string, boolean flag) {
        if(flag) {
            System.out.println(string);
            System.out.flush();
        }
    }

}
