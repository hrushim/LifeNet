/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package lifenetftp;

/**
 *
 * @author hrushi
 */
public class Main {

    public static String CONFIGFILEPATH = System.getProperty("user.home") + "/.LifeNetData/config/hosts";
    public static String GNSTFILEPATH = System.getProperty("user.home") + "/.LifeNetData/config/gnst.txt";
    public static String HOSTSFILEPATH = System.getProperty("user.home") + "/.LifeNetData/config/hosts.txt";
    public static String ERROR_FILE_REQUEST = "Destination not replying to SEND-FILE-LIST request";
    public static String ERROR_LOCK_FTP = "Destination not replying to LOCK-FTP message";
    public static String ERROR_DATA_REQUEST = "Destination not replying to DATA-REQUEST message";
    public static String ERROR_DATA_ACK = "Destination not replying to DATA-ACK message";
    public static String MSG_SEND_FILE_LIST = "SEND-FILE-LIST";
    public static String MSG_LOCK_FTP = "LOCK-FTP?";
    public static String MSG_LOCK_FTP_APPROVE = "YES-LOCK-FTP";
    public static String MSG_LOCK_FTP_DENY = "LOCK-FTP-DENIED";
    public static String MSG_DATA_REQUEST = "SEND-DATA;";
    public static String NOTIFICATION_TXFTP_END = "File transfer terminated at source";
    public static String NOTIFICATION_RXFTP_END = "File transfer terminated at destination";
    public static String MSG_DATA = "DATA;";
    public static String MSG_EOF = "$$EOF$$";
    public static boolean DEBUG = false;
    public static int NUM_TICKS_CONTROL = 50;
    public static int NUM_TICKS_DATA = 20;
    public static int MAX_TICKS_RXFTP = 500;
    public static int MAX_TICKS_TXFTP = 500;
    public static int MAX_TICKS_FTP_COMPLETE = 20;
    public static int SEQ_FILE_REQUEST = 0;
    public static int SEQ_LOCK_FTP = 1;
    public static int SEQ_DATA = 2;
    public static int MAXBUF = 6000;
    
    public static int DATA_TIMEOUT = 200;
    public static int DATA_THREAD_INTERVAL = 1000;
    
    public static int CTRL_RX_TIMEOUT = 200;
    public static long CNTRL_THREAD_INTERVAL = 100;

    public static int RATE_CNTRL_VD_THRESHOLD = 80;
    public static int RATE_CNTRL_MIN_DELAY = 9;
    public static int RATE_CNTRL_MAX_DELAY = 1000;
    public static int RATE_CNTRL_DEC_INTERVAL = 5;
    public static int RATE_CNTRL_INC_INTERVAL = 20;

    public static int SERIAL_NETMESSAGE = 3000;

//    public static long RX_THREAD_INTERVAL = 1000;
//    public static long TX_THREAD_INTERVAL = 1000;
    public static int BUDDIES_UPDATE_INTERVAL = 10000;
    public static int CONTROL_SRC_PORT = 20000;
    public static int CONTROL_ACK_SRC_PORT = 20001;
    public static int DATA_SRC_PORT = 30000;
    public static int DATA_ACK_SRC_PORT = 30001;
    public static int FTP_SRC_PORT = 20000;
    public static int FTP_ACK_SRC_PORT = 20001;
    public static int MSGCODE_SEND_FILE_LIST = 1000;
    public static int MSGCODE_SEND_FILE_LIST_ACK = 1001;
    public static int MSGCODE_FILE_LIST = 1002;
    public static int MSGCODE_FILE_LIST_ACK = 1003;
    public static int MSGCODE_LOCK_FTP = 1004;
    public static int MSGCODE_LOCK_FTP_ACK_APPROVE = 1005;
    public static int MSGCODE_LOCK_FTP_ACK_DENY = 1006;
    public static int MSGCODE_DATA_REQUEST = 1007;
    public static int MSGCODE_DATA_ACK = 1008;
    public static int FTP_BLOCK_SIZE = 512;

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        (new MainForm()).setVisible(true);
    }
}
