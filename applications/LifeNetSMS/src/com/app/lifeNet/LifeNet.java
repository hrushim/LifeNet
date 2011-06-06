
/*
 * UnifiedForm.java
 *
 * Created on Nov 6, 2009, 2:07:51 PM
 */
package com.app.lifeNet;

import javax.swing.*;
import java.awt.event.KeyEvent;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.logging.Handler;

/**
 *
 * @author hrushi
 */
public class LifeNet extends javax.swing.JFrame implements Runnable {

    public static int MSG_TYPE_DATA = 1;
    public static int MSG_TYPE_ACK = 2;
    public static int MSG_SRC_DATA_PORT = 30000;
    public static int MSG_SRC_ACK_PORT = 30001;
    public static int MSG_RCV_TIMEOUT = 100;
    public static int THREAD_CNTRL_INTERVAL = 50;
    public static long SEQ_CNT = 0; // Used for sequencing data packets
    public static int MAX_NUM_OF_RX_MSGS = 255;
    public static int MSG_START_REP_CNT = 50;
    public static int MSG_REP_CNT = 5;
    public static String selectedUserName; // Not sure
    public static String HOSTS_FILE_NAME = System.getProperty("user.home") + "/.LifeNetData/config/hosts.txt";
    public static String GNST_FILE_NAME = System.getProperty("user.home") + "/.LifeNetData/config/gnst.txt";
    static String MANIFOLD_FILE_NAME = "/proc/txstats";
    boolean RUN_FLAG = false;
    Thread updateThread;
    MessageThread messTh;
    private DefaultListModel namesLM;
    long[] arrSeqNum;
    String messageData = null;

    public LifeNet() {
        initComponents();

        LifeNetApi.init(HOSTS_FILE_NAME, GNST_FILE_NAME);

        namesLM = new DefaultListModel();
        NamesList.setModel(namesLM);

        updateThread = new Thread(this);
        setRunning(true);
        updateThread.start();

        messTh = new MessageThread(this, THREAD_CNTRL_INTERVAL);
        messTh.setRunning(true);
        messTh.start();
    }

    public void updateAliveContacts() {
        namesLM.clear();
        StringBuilder sb = new StringBuilder();
        try {
            BufferedReader in = new BufferedReader(new FileReader(HOSTS_FILE_NAME));
            String str;

            int i = 0;
            while ((str = in.readLine()) != null) {
                String[] strArr = str.split(" ");
                sb.append(strArr[3]);
                // strVector.add(strArr[3]);
                if (LifeNetApi.getMyName().trim().equals(strArr[3])) {
                    namesLM.addElement(strArr[3]);
                } else {
                    namesLM.addElement(strArr[3] + "    [(ED = " + LifeNetApi.getED(LifeNetApi.getMyName(), strArr[3]) + "), (" + LifeNetApi.getNumTxNumRx(LifeNetApi.getMyName(), strArr[3]) + ")]");
                }
                sb.append(" ");
            }
            in.close();

        } catch (Exception e) {
            sb.append("<Disconnected>");
        }
        if (namesLM.isEmpty()) {
            namesLM.addElement("<Nobody online>");
        }
    }

    public void run() {
        // TODO Auto-generated method stub
        while (true) {
            try {

                updateAliveContacts();

                ChatMessage chatMsg = null;

                if (MessageQueue.newMsgReadFlag == true) {
                    for (int i = 0; i < MessageQueue.unreadCount; i++) {

                        chatMsg = (ChatMessage) MessageQueue.unreadMessageVector.elementAt(i);
                        ChatMessage chatMsgNew = new ChatMessage(chatMsg.srcName, chatMsg.seq, chatMsg.payload.length(), chatMsg.payload, chatMsg.type);
                        chatMsgNew.rxTime = chatMsg.rxTime;
                        MessageQueue.readMessageVector.add(chatMsgNew);
                        MessageQueue.readCount++;

                    }
                    MessageQueue.unreadCount = 0;
                    MessageQueue.newMsgReadFlag = false;
                    MessageQueue.unreadMessageVector.clear();
                }

                if (MessageQueue.unreadCount > 0) {

                    if (MessageQueue.unreadCount == 1) {
                        jLabel2.setText("1 New Message");
                    } else {
                        jLabel2.setText(MessageQueue.unreadCount + " New Messages");
                    }
                } else {
                    jLabel2.setText("No new messages");
                }

                jLabel3.setText("Inbox(" + MessageQueue.readCount + ")");

                Thread.sleep(5000);

            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }

    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        buttonGroup1 = new javax.swing.ButtonGroup();
        jPanel4 = new javax.swing.JPanel();
        jScrollPane3 = new javax.swing.JScrollPane();
        NamesList = new javax.swing.JList();
        jLabel1 = new javax.swing.JLabel();
        jPanel1 = new javax.swing.JPanel();
        jLabel2 = new javax.swing.JLabel();
        jLabel3 = new javax.swing.JLabel();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setName("MyMANET_gui"); // NOI18N
        setResizable(false);
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosed(java.awt.event.WindowEvent evt) {
                formWindowClosed(evt);
            }
        });

        jPanel4.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        jPanel4.setMinimumSize(new java.awt.Dimension(117, 284));

        NamesList.setModel(new javax.swing.AbstractListModel() {
            String[] strings = { "Demo" };
            public int getSize() { return strings.length; }
            public Object getElementAt(int i) { return strings[i]; }
        });
        NamesList.setDragEnabled(true);
        NamesList.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                NamesListMouseClicked(evt);
            }
        });
        jScrollPane3.setViewportView(NamesList);

        jLabel1.setText("Buddies");

        javax.swing.GroupLayout jPanel4Layout = new javax.swing.GroupLayout(jPanel4);
        jPanel4.setLayout(jPanel4Layout);
        jPanel4Layout.setHorizontalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel4Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jScrollPane3, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 241, Short.MAX_VALUE)
                    .addComponent(jLabel1))
                .addContainerGap())
        );
        jPanel4Layout.setVerticalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel4Layout.createSequentialGroup()
                .addGap(14, 14, 14)
                .addComponent(jLabel1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane3, javax.swing.GroupLayout.PREFERRED_SIZE, 211, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );

        jPanel1.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        jLabel2.setHorizontalAlignment(javax.swing.SwingConstants.LEFT);
        jLabel2.setText("No New Messages");
        jLabel2.setVerticalAlignment(javax.swing.SwingConstants.TOP);
        jLabel2.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel2MouseClicked(evt);
            }
        });

        jLabel3.setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
        jLabel3.setText("Inbox(0)");
        jLabel3.setVerticalAlignment(javax.swing.SwingConstants.TOP);
        jLabel3.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jLabel3MouseClicked(evt);
            }
        });

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabel2)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 55, Short.MAX_VALUE)
                .addComponent(jLabel3)
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel2)
                    .addComponent(jLabel3))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jPanel4, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel4, javax.swing.GroupLayout.PREFERRED_SIZE, 262, javax.swing.GroupLayout.PREFERRED_SIZE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    /* When the group chat button is selected select the first item of the groups list as default */
    /* This function is called when the enter key is pressed onto jTextArea2 */
    private void processSend() {
        if (LifeNet.SEQ_CNT == 0) {
            ChatMessage msg = new ChatMessage(LifeNetApi.getMyName(), 0, 4, "TEST", LifeNet.MSG_TYPE_DATA);
            UdpTx.send(LifeNetApi.getIpFromName((String) NamesList.getSelectedValue()), msg, LifeNet.MSG_START_REP_CNT);
            LifeNet.SEQ_CNT++;

        }

        ChatMessage msg = new ChatMessage(LifeNetApi.getMyName(), LifeNet.SEQ_CNT, messageData.length(), messageData, LifeNet.MSG_TYPE_DATA);
        UdpTx.send(LifeNetApi.getIpFromName((String) NamesList.getSelectedValue()), msg, LifeNet.MSG_REP_CNT);
        LifeNet.SEQ_CNT++;
    }

private void formWindowClosed(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosed
    // TODO add your handling code here:
    setRunning(true);
    messTh.setRunning(true);
}//GEN-LAST:event_formWindowClosed

private void NamesListMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_NamesListMouseClicked
    // TODO add your handling code here:
    selectedUserName = (String) NamesList.getSelectedValue();
    new MessageWindow((String) NamesList.getSelectedValue()).setVisible(true);
}//GEN-LAST:event_NamesListMouseClicked

private void jLabel3MouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jLabel3MouseClicked
    // TODO add your handling code here:
    new InboxWindow().setVisible(true);
}//GEN-LAST:event_jLabel3MouseClicked

private void jLabel2MouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jLabel2MouseClicked
    // TODO add your handling code here:
    new InboxView().setVisible(true);
}//GEN-LAST:event_jLabel2MouseClicked
    /* Unit testing completed
     * This function is used to modify the main textArea where the user can see the chat messages */

    public void setRunning(boolean flag) {
        RUN_FLAG = flag;
    }

    public boolean isRunning() {
        return RUN_FLAG;
    }

    public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {

            public void run() {
                new LifeNet().setVisible(true);

            }
        });

    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JList NamesList;
    private javax.swing.ButtonGroup buttonGroup1;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JScrollPane jScrollPane3;
    // End of variables declaration//GEN-END:variables
}

/*

public void checkAndAddInNamesList(String strValue) {

boolean foundFlag = false;

for (int i = 0; i < namesLM.getSize(); i++) {
if (strValue.equals(String.valueOf(namesLM.elementAt(i)))) {
foundFlag = true;
break;
}
}

if (foundFlag == false) {
namesLM.addElement(strValue);
}

}
public void modifyMainTextArea(String str1) {
}

public long getSeqNum(int index) {
return arrSeqNum[index];
}

public void setSeqNum(int index, long value) {
arrSeqNum[index] = value;
}

public void setMyName(String strValue) {
}

public void setMyIP(String strValue) {
}

public void setMyMac(String strValue) {
}
public String getMyName() {

//GnsmManager gnsM = new GnsmManager(PATH_GNST, PATH_HOSTS);
//return gnsM.getMyName();

return "";
}
public LifeNet() {
initComponents();

LifeNetApi.init(PATH_HOSTS, PATH_GNST);

//        arrSeqNum = new long[MAX_HOSTS];
namesLM = new DefaultListModel();
NamesList.setModel(namesLM);

if (TEST == false) {
budThread = new BuddiesUpdateThread(this, THREAD_BUDDIES_INTERVAL, PATH_HOSTS);
budThread.setRunning(true);
budThread.start();

messTh = new MessageThread(this, THREAD_CNTRL_INTERVAL);
messTh.setRunning(true);
messTh.start();

}

}
 *
 *
 */
