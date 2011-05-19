/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package gui;

/**
 *
 * @author hrushi
 * Used for printing debug statements
 */
public class PrintDebug {

    /* Unit test done */
    public static void printDebug(String string, boolean flag) {
        if (flag) {
            System.out.println(string);
            System.out.flush();
        }
    }
}
