/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package gui;


import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author hrushi
 */
public class GnsmManager {

    private File file;
    private FileReader fileReader;
    private FileWriter fileWriter;
    private BufferedReader reader;
    private BufferedWriter writer;
    private static int READ = 11;
    private static int WRITE = 12;
    private static int SEL_NAME = 1;
    private static int SEL_IP = 2;
    private static int SEL_MAC = 3;
    String gnstPath;
    String hostPath;

    public GnsmManager(String gnstFilePath, String hostFilePath) {
        gnstPath = gnstFilePath;
        hostPath = hostFilePath;
    }

    public int getHostCount() {

        int count = 0;
        if (openFile(READ, hostPath)) {
            String[] strArr;
            strArr = readIntoArray();
            if (strArr != null) {
                count = strArr.length;
            }
        }
        closeLastOpenedFile();
        return count;

    }

    public String getHostName(int index) {

        String retName = null;
        if (index != -1) {
            if (openFile(READ, hostPath)) {
                String[] strArr, strArr_1;
                strArr = readIntoArray();
                if (strArr.length > index) {
                    strArr_1 = strArr[index].split(" ");
                    retName = strArr_1[3];
                }
            }
        }
        closeLastOpenedFile();
        return retName;

    }

    public int getHostIndex(String name, String ipAdd, String macAdd, int flag) {
        int index = -1;
        if (openFile(READ, hostPath)) {

            String[] strArr, strArr_1;
            strArr = readIntoArray();

            if (strArr != null) {
                for (int i = 0; i < strArr.length; i++) {
                    if (strArr.length > i) {
                        strArr_1 = strArr[i].split(" ");
                        if (strArr_1.length > 7) {
                            if (strArr_1[3] != null && strArr_1[5] != null && strArr_1[7] != null) {
                                if (flag == SEL_NAME) {
                                    if (strArr_1[3].equals(name)) {
                                        index = i;
                                        break;
                                    }
                                } else if (flag == SEL_IP) {
                                    if (strArr_1[5].equals(ipAdd)) {
                                        index = i;
                                        break;
                                    }
                                } else if (flag == SEL_MAC) {
                                    if (strArr_1[7].equals(macAdd)) {
                                        index = i;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        closeLastOpenedFile();
        return index;
    }

    public String getMyName() {
        String myName = null;

        if (openFile(READ, hostPath)) {
            String[] strArr, strArr_1;
            strArr = readIntoArray();

            for (int i = 0; i < strArr.length; i++) {
                strArr_1 = strArr[i].split(" ");
                if (strArr_1.length > 10) {
                    if (strArr_1[7].equals(strArr_1[10])) {
                        myName = strArr_1[3];
                        break;
                    }
                }

            }

        }
        closeLastOpenedFile();

        return myName;
    }

    public String getMyMac() {
        String myMac = null;

        if (openFile(READ, hostPath)) {
            String[] strArr, strArr_1;
            strArr = readIntoArray();


            for (int i = 0; i < strArr.length; i++) {
                strArr_1 = strArr[i].split(" ");

                if (strArr_1.length > 10) {
                    if (strArr_1[7].equals(strArr_1[10])) {
                        myMac = strArr_1[10];
                        break;
                    }
                }

            }

        }
        closeLastOpenedFile();

        return myMac;
    }

    public String getMyIP() {
        String myIP = null;

        if (openFile(READ, hostPath)) {
            String[] strArr, strArr_1;
            strArr = readIntoArray();

            for (int i = 0; i < strArr.length; i++) {
                strArr_1 = strArr[i].split(" ");
                if (strArr_1.length > 10) {
                    if (strArr_1[7].equals(strArr_1[10])) {
                        myIP = strArr_1[5];
                        break;
                    }
                }

            }

        }
        closeLastOpenedFile();

        return myIP;

    }

    public String getHostIP(int index) {
        String retIP = null;
        if (index != -1) {
            if (openFile(READ, hostPath)) {
                String[] strArr, strArr_1;
                strArr = readIntoArray();
                if (strArr.length > index) {

                    strArr_1 = strArr[index].split(" ");
                    if (strArr_1.length > 5) {
                        if (strArr_1[5] != null) {
                            retIP = strArr_1[5];
                        }
                    }
                }
            }
            closeLastOpenedFile();
        }

        String[] temp = retIP.split("\\.");

        if (temp[0].equals("192")) {
            return retIP;
        } else {
            String tempStr = temp[3] + "." + temp[2] + "." + temp[1] + "." + temp[0];
            return tempStr;
        }
    }

    public String getHostIP(String name) {
        if (getHostIndex(name, null, null, SEL_NAME) != -1) {
            return getHostIP(getHostIndex(name, null, null, SEL_NAME));
        } else {
            return null;

        }
    }

    public String getHostMac(int index) {
        String retMac = null;
        if (index != -1) {
            if (openFile(READ, hostPath)) {
                String[] strArr, strArr_1;
                strArr = readIntoArray();
                if (strArr.length > index) {
                    strArr_1 = strArr[index].split(" ");
                    if (strArr_1.length > 7) {
                        if (strArr_1[7] != null) {
                            retMac = strArr_1[7];
                        }
                    }
                }
            }
        }
        closeLastOpenedFile();
        return retMac;
    }

    public String getHostMac(String name) {
        if (getHostIndex(name, null, null, SEL_NAME) != -1) {
            return getHostMac(getHostIndex(name, null, null, SEL_NAME));
        } else {
            return null;

        }
    }

    public int getTxSession(int index1, int index2) {
        int txSession = -1;
        if (index1 != -1 && index2 != -1) {
            int line_number = 0;
            if (getHostCount() != 0) {
                if (index1 != -1 && index2 != -1) {
                    if (index2 > index1) {
                        line_number = index1 * (getHostCount() - 1) + (index2 - 1);
                    } else {
                        line_number = index1 * (getHostCount() - 1) + index2;
                    }

                    if (openFile(READ, gnstPath)) {
                        String[] strArr, strArr_1 = null;
                        strArr = readIntoArray();
                        if (strArr.length > line_number) {
                            strArr_1 = strArr[line_number].split(" ");
                            if (strArr_1.length > 5) {
                                if (strArr_1[5] != null) {
                                    txSession = Integer.parseInt(strArr_1[5]);
                                }
                            }
                        }
                    }
                }
                closeLastOpenedFile();
            }
        }
        return txSession;
    }

    public int getTxSession(String name1, String name2) {
        if (getHostIndex(name1, null, null, SEL_NAME) != -1 && getHostIndex(name2, null, null, SEL_NAME) != -1) {
            return getTxSession(getHostIndex(name1, null, null, SEL_NAME), getHostIndex(name2, null, null, SEL_NAME));
        } else {
            return -1;
        }
    }

    public int getRxSession(int index1, int index2) {
        int rxSession = -1;

        int line_number = 0;
        if (getHostCount() != 0) {
            if (index1 != -1 && index2 != -1) {
                if (index2 > index1) {
                    line_number = index1 * (getHostCount() - 1) + (index2 - 1);
                } else {
                    line_number = index1 * (getHostCount() - 1) + index2;
                }
                if (openFile(READ, gnstPath)) {
                    String[] strArr, strArr_1;
                    strArr = readIntoArray();
                    if (strArr.length > line_number) {
                        strArr_1 = strArr[line_number].split(" ");
                        if (strArr_1.length > 9) {
                            if (strArr_1[9] != null) {
                                rxSession = Integer.parseInt(strArr_1[9]);
                            }
                        }
                    }
                }
            }
            closeLastOpenedFile();
        }
        return rxSession;
    }

    public int getRxSession(String name1, String name2) {
        if (getHostIndex(name1, null, null, SEL_NAME) != -1 && getHostIndex(name2, null, null, SEL_NAME) != -1) {
            return getRxSession(getHostIndex(name1, null, null, SEL_NAME), getHostIndex(name2, null, null, SEL_NAME));
        } else {
            return -1;
        }
    }

    public int getNumRx(int index1, int index2) {
        int numRx = -1;
        if (index1 != -1 && index2 != -1) {
            int line_number = 0;
            if (getHostCount() != 0) {
                if (index1 != -1 && index2 != -1) {
                    if (index2 > index1) {
                        line_number = index1 * (getHostCount() - 1) + (index2 - 1);
                    } else {
                        line_number = index1 * (getHostCount() - 1) + index2;
                    }
                    if (openFile(READ, gnstPath)) {
                        String[] strArr, strArr_1;
                        strArr = readIntoArray();
                        if (strArr.length > line_number) {
                            strArr_1 = strArr[line_number].split(" ");
                            if (strArr_1.length > 7) {
                                if (strArr_1[7] != null) {
                                    numRx = Integer.parseInt(strArr_1[7]);
                                }
                            }
                        }
                    }
                }
                closeLastOpenedFile();
            }
        }
        return numRx;
    }

    public int getNumRx(String name1, String name2) {
        if (getHostIndex(name1, null, null, SEL_NAME) != -1 && getHostIndex(name2, null, null, SEL_NAME) != -1) {
            return getNumRx(getHostIndex(name1, null, null, SEL_NAME), getHostIndex(name2, null, null, SEL_NAME));
        } else {
            return -1;
        }
    }

    public int getNumTx(int index1, int index2) {
        int numTx = -1;
        if (index1 != -1 && index2 != -1) {
            int line_number = 0;
            if (getHostCount() != 0) {
                if (index1 != -1 && index2 != -1) {
                    if (index2 > index1) {
                        line_number = index1 * (getHostCount() - 1) + (index2 - 1);
                    } else {
                        line_number = index1 * (getHostCount() - 1) + index2;
                    }
                    if (openFile(READ, gnstPath)) {
                        String[] strArr, strArr_1;
                        strArr = readIntoArray();
                        if (strArr.length > line_number) {
                            strArr_1 = strArr[line_number].split(" ");
                            if (strArr_1.length > 3) {
                                if (strArr_1[3] != null) {
                                    numTx = Integer.parseInt(strArr_1[3]);
                                }
                            }
                        }
                    }
                }
                closeLastOpenedFile();

            }
        }
        return numTx;
    }

    public int getNumTx(String name1, String name2) {
        if (getHostIndex(name1, null, null, SEL_NAME) != -1 && getHostIndex(name2, null, null, SEL_NAME) != -1) {
            return getNumTx(getHostIndex(name1, null, null, SEL_NAME), getHostIndex(name2, null, null, SEL_NAME));
        } else {
            return -1;
        }
    }

    public int getPktLoss(int index1, int index2) {
        if (getNumTx(index1, index2) != -1 && getNumRx(index1, index2) != -1) {
            return (getNumTx(index1, index2) - getNumRx(index1, index2));
        } else {
            return -1;
        }
    }

    public int getPktLoss(String name1, String name2) {
        if (getHostIndex(name1, null, null, SEL_NAME) != -1 && getHostIndex(name2, null, null, SEL_NAME) != -1) {
            return getPktLoss(getHostIndex(name1, null, null, SEL_NAME), getHostIndex(name2, null, null, SEL_NAME));
        } else {
            return -1;
        }
    }

    public int getPercPktLoss(int index1, int index2) {
        if (getPktLoss(index1, index2) != -1 && getNumTx(index1, index1) != -1) {
            return (getPktLoss(index1, index2) * 100) / getNumTx(index1, index1);
        } else {
            return -1;
        }
    }

    public int getPercPktLoss(String name1, String name2) {
        if (getHostIndex(name1, null, null, SEL_NAME) != -1 && getHostIndex(name2, null, null, SEL_NAME) != -1) {
            return getPercPktLoss(getHostIndex(name1, null, null, SEL_NAME), getHostIndex(name2, null, null, SEL_NAME));
        } else {
            return -1;
        }
    }

    public int getVD(int index1, int index2) {
        int vd = -1;
        if (index1 != -1 && index2 != -1) {
            if (getNumTx(index1, index2) != -1 && getNumRx(index1, index2) != -1 && getTxSession(index1, index2) != -1 && getRxSession(index1, index2) != -1) {

                int numTx = getNumTx(index1, index2);
                int numRx = getNumRx(index1, index2);
                int txSession = getTxSession(index1, index2);
                int rxSession = getRxSession(index1, index2);

                if (txSession != rxSession) {
                    vd = -1;
                } else {
                    if (numRx >= numTx) {
                        if (numRx == 0 && numTx == 0) {
                            vd = 255;
                        } else {
                            vd = ((numTx * 100) / numRx);
                        }
                    } else {
                        if ((((255 - 100) * numRx) / numTx) >= 255) {
                            vd = 1;
                        } else {
                            vd = (255 - (((255 - 100) * numRx) / numTx));
                        }
                    }
                }
            }
        }
        return vd;
    }

    public int getVD(String name1, String name2) {
        if (getHostIndex(name1, null, null, SEL_NAME) != -1 && getHostIndex(name2, null, null, SEL_NAME) != -1) {
            return getVD(getHostIndex(name1, null, null, SEL_NAME), getHostIndex(name2, null, null, SEL_NAME));
        } else {
            return -1;
        }
    }

    private boolean openFile(int fileMode, String filename) {

        try {
            if (fileMode == READ) {

                file = new File(filename);
                fileReader = new FileReader(file);
                reader = new BufferedReader(fileReader);

            } else {

                file = new File(gnstPath);
                fileWriter = new FileWriter(file);
                writer = new BufferedWriter(fileWriter);

            }

            return true;



        } catch (FileNotFoundException ex) {

            System.out.println("File not found exception in openfile");
            ex.printStackTrace();
            return false;

        } catch (IOException ex) {
            System.out.println("IO Exception in openfile");
            Logger.getLogger(
                    FileHandler.class.getName()).log(Level.SEVERE, null, ex);
            ex.printStackTrace();
            return false;
        }
    }

    private String[] readIntoArray() {

        String[] strArr = null;
        String text = null;
        String str = null;
        StringBuffer contents = new StringBuffer();

        try {
            if (reader != null) {
                while ((text = reader.readLine()) != null) {
                    contents.append(text).append(System.getProperty("line.separator"));




                }
            }
        } catch (IOException ex) {
            Logger.getLogger(FileHandler.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println(
                    "File not found in readintoarray");
            ex.printStackTrace();
        } catch (NullPointerException ex) {

            System.out.println("Null ptr exception caught in readintoarray");
            ex.printStackTrace();

        }

        str = contents.toString();
        strArr = str.split("\\n");


        return strArr;


    }

    private void closeLastOpenedFile() {
        try {

            reader.close();
            fileReader.close();
            reader = null;
            file = null;

            if (writer != null) {
                writer.close();
            }
            if (fileWriter != null) {
                fileWriter.close();
                fileWriter = null;
            }
            file = null;



        } catch (IOException ex) {

            System.out.println("inside closefile");
            Logger.getLogger(
                    FileHandler.class.getName()).log(Level.SEVERE, null, ex);
            ex.printStackTrace();

        } catch (NullPointerException exc) {

            System.out.println("Null ptr. exception in closefile");
            exc.printStackTrace();
        }
    }
}
