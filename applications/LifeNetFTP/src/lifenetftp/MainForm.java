/*
 * MainForm.java
 *
 * Created on May 11, 2010, 3:43:02 PM
 */
package lifenetftp;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import javax.swing.DefaultListModel;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;

/**
 *
 * @author hrushi
 */
public class MainForm extends javax.swing.JFrame {

    /** Creates new form MainForm */
    public MainForm() {
        initComponents();
        disableAllGUIComponents();
        namesListModel = new DefaultListModel();
        NamesList.setModel(namesListModel);
        namesListModel.clear();

        filesListModel = new DefaultListModel();
        FilesList.setModel(filesListModel);
        filesListModel.clear();
    }

    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPanel1 = new javax.swing.JPanel();
        dloadDirLabel = new javax.swing.JLabel();
        dloadDirButton = new javax.swing.JButton();
        dloadDirText = new javax.swing.JTextField();
        dloadDirLabel1 = new javax.swing.JLabel();
        uploadDirButton = new javax.swing.JButton();
        uploadDirText = new javax.swing.JTextField();
        jPanel2 = new javax.swing.JPanel();
        jLabel3 = new javax.swing.JLabel();
        jLabel9 = new javax.swing.JLabel();
        jPanel3 = new javax.swing.JPanel();
        jPanel4 = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        NamesList = new javax.swing.JList();
        jLabel1 = new javax.swing.JLabel();
        jLabel4 = new javax.swing.JLabel();
        buddyMACText = new javax.swing.JTextField();
        jLabel5 = new javax.swing.JLabel();
        buddyIPText = new javax.swing.JTextField();
        jPanel5 = new javax.swing.JPanel();
        jLabel2 = new javax.swing.JLabel();
        jScrollPane2 = new javax.swing.JScrollPane();
        FilesList = new javax.swing.JList();
        dloadButton = new javax.swing.JButton();
        jProgressBar1 = new javax.swing.JProgressBar();
        jPanel6 = new javax.swing.JPanel();
        jLabel6 = new javax.swing.JLabel();
        bwLabel = new javax.swing.JLabel();
        jPanel7 = new javax.swing.JPanel();
        jLabel7 = new javax.swing.JLabel();
        myNameText = new javax.swing.JTextField();
        jLabel8 = new javax.swing.JLabel();
        myIPText = new javax.swing.JTextField();
        jLabel10 = new javax.swing.JLabel();
        myMacText = new javax.swing.JTextField();
        jPanel8 = new javax.swing.JPanel();
        startButton = new javax.swing.JButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

        jPanel1.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        dloadDirLabel.setText("Download Directory");

        dloadDirButton.setText("Select");
        dloadDirButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                dloadDirButtonActionPerformed(evt);
            }
        });

        dloadDirText.setEditable(false);

        dloadDirLabel1.setText("Upload Directory");

        uploadDirButton.setText("Select");
        uploadDirButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                uploadDirButtonActionPerformed(evt);
            }
        });

        uploadDirText.setEditable(false);

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(dloadDirLabel)
                    .addComponent(dloadDirLabel1))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addComponent(dloadDirButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(dloadDirText, javax.swing.GroupLayout.PREFERRED_SIZE, 164, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addComponent(uploadDirButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(uploadDirText, javax.swing.GroupLayout.PREFERRED_SIZE, 164, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap(27, Short.MAX_VALUE))
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(dloadDirLabel)
                    .addComponent(dloadDirButton)
                    .addComponent(dloadDirText, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(dloadDirLabel1)
                    .addComponent(uploadDirButton)
                    .addComponent(uploadDirText, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jPanel2.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        jLabel3.setFont(new java.awt.Font("Corbel", 1, 18));
        jLabel3.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jLabel3.setText("LifeNet ");
        jLabel3.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);

        jLabel9.setFont(new java.awt.Font("Century Schoolbook L", 2, 16));
        jLabel9.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jLabel9.setText("File transfer");
        jLabel9.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addGap(154, 154, 154)
                .addComponent(jLabel3)
                .addContainerGap(212, Short.MAX_VALUE))
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel2Layout.createSequentialGroup()
                .addContainerGap(212, Short.MAX_VALUE)
                .addComponent(jLabel9)
                .addGap(142, 142, 142))
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabel3)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jLabel9)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jPanel3.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        jPanel4.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        NamesList.setModel(new javax.swing.AbstractListModel() {
            String[] strings = { "Item 1", "Item 2", "Item 3", "Item 4", "Item 5" };
            public int getSize() { return strings.length; }
            public Object getElementAt(int i) { return strings[i]; }
        });
        NamesList.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                NamesListMouseClicked(evt);
            }
        });
        NamesList.addListSelectionListener(new javax.swing.event.ListSelectionListener() {
            public void valueChanged(javax.swing.event.ListSelectionEvent evt) {
                NamesListValueChanged(evt);
            }
        });
        jScrollPane1.setViewportView(NamesList);

        jLabel1.setText("Buddies");

        jLabel4.setText("Buddy IPv4");

        buddyMACText.setEditable(false);

        jLabel5.setText("Buddy MAC");

        buddyIPText.setEditable(false);

        javax.swing.GroupLayout jPanel4Layout = new javax.swing.GroupLayout(jPanel4);
        jPanel4.setLayout(jPanel4Layout);
        jPanel4Layout.setHorizontalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel4Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                        .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 129, Short.MAX_VALUE)
                        .addComponent(jLabel1, javax.swing.GroupLayout.Alignment.LEADING))
                    .addComponent(buddyMACText, javax.swing.GroupLayout.DEFAULT_SIZE, 129, Short.MAX_VALUE)
                    .addComponent(jLabel5, javax.swing.GroupLayout.DEFAULT_SIZE, 129, Short.MAX_VALUE)
                    .addComponent(jLabel4, javax.swing.GroupLayout.DEFAULT_SIZE, 129, Short.MAX_VALUE))
                .addContainerGap())
            .addGroup(jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addGroup(jPanel4Layout.createSequentialGroup()
                    .addGap(12, 12, 12)
                    .addComponent(buddyIPText, javax.swing.GroupLayout.DEFAULT_SIZE, 129, Short.MAX_VALUE)
                    .addGap(12, 12, 12)))
        );
        jPanel4Layout.setVerticalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel4Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabel1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addComponent(jLabel4)
                .addGap(50, 50, 50)
                .addComponent(jLabel5)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(buddyMACText, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(24, Short.MAX_VALUE))
            .addGroup(jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addGroup(jPanel4Layout.createSequentialGroup()
                    .addGap(205, 205, 205)
                    .addComponent(buddyIPText, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addContainerGap(84, Short.MAX_VALUE)))
        );

        jPanel5.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        jLabel2.setText("Files Available");

        FilesList.setModel(new javax.swing.AbstractListModel() {
            String[] strings = { "Item 1", "Item 2", "Item 3", "Item 4", "Item 5" };
            public int getSize() { return strings.length; }
            public Object getElementAt(int i) { return strings[i]; }
        });
        jScrollPane2.setViewportView(FilesList);

        dloadButton.setText("Download File");
        dloadButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                dloadButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel5Layout = new javax.swing.GroupLayout(jPanel5);
        jPanel5.setLayout(jPanel5Layout);
        jPanel5Layout.setHorizontalGroup(
            jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel5Layout.createSequentialGroup()
                .addGroup(jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel5Layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jLabel2, javax.swing.GroupLayout.PREFERRED_SIZE, 155, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(jPanel5Layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 254, Short.MAX_VALUE))
                    .addGroup(jPanel5Layout.createSequentialGroup()
                        .addGap(54, 54, 54)
                        .addComponent(dloadButton, javax.swing.GroupLayout.PREFERRED_SIZE, 171, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap())
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel5Layout.createSequentialGroup()
                .addContainerGap(66, Short.MAX_VALUE)
                .addComponent(jProgressBar1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(64, 64, 64))
        );
        jPanel5Layout.setVerticalGroup(
            jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel5Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabel2, javax.swing.GroupLayout.PREFERRED_SIZE, 14, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 199, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(dloadButton)
                .addGap(18, 18, 18)
                .addComponent(jProgressBar1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(16, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addComponent(jPanel4, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel5, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel4, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jPanel5, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );

        jPanel6.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        jLabel6.setText("Bandwidth ->");

        bwLabel.setText("Yet to start");

        javax.swing.GroupLayout jPanel6Layout = new javax.swing.GroupLayout(jPanel6);
        jPanel6.setLayout(jPanel6Layout);
        jPanel6Layout.setHorizontalGroup(
            jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel6Layout.createSequentialGroup()
                .addGap(120, 120, 120)
                .addComponent(jLabel6, javax.swing.GroupLayout.PREFERRED_SIZE, 115, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(bwLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 115, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(89, Short.MAX_VALUE))
        );
        jPanel6Layout.setVerticalGroup(
            jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel6Layout.createSequentialGroup()
                .addGap(20, 20, 20)
                .addGroup(jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel6)
                    .addComponent(bwLabel))
                .addContainerGap(21, Short.MAX_VALUE))
        );

        jPanel7.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        jLabel7.setText("My Name");

        myNameText.setEditable(false);

        jLabel8.setText("My IPv4");

        myIPText.setEditable(false);

        jLabel10.setText("My MAC");

        myMacText.setEditable(false);

        javax.swing.GroupLayout jPanel7Layout = new javax.swing.GroupLayout(jPanel7);
        jPanel7.setLayout(jPanel7Layout);
        jPanel7Layout.setHorizontalGroup(
            jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel7Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jLabel7)
                    .addComponent(myNameText, javax.swing.GroupLayout.PREFERRED_SIZE, 129, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(myIPText, javax.swing.GroupLayout.PREFERRED_SIZE, 129, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel8, javax.swing.GroupLayout.PREFERRED_SIZE, 80, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(12, 12, 12)
                .addGroup(jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(myMacText, javax.swing.GroupLayout.PREFERRED_SIZE, 129, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel10, javax.swing.GroupLayout.PREFERRED_SIZE, 92, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );
        jPanel7Layout.setVerticalGroup(
            jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel7Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel7)
                    .addComponent(jLabel8)
                    .addComponent(jLabel10))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(myNameText, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(myIPText, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(myMacText, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jPanel8.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        startButton.setText("START");
        startButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                startButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel8Layout = new javax.swing.GroupLayout(jPanel8);
        jPanel8.setLayout(jPanel8Layout);
        jPanel8Layout.setHorizontalGroup(
            jPanel8Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel8Layout.createSequentialGroup()
                .addGap(192, 192, 192)
                .addComponent(startButton)
                .addContainerGap(176, Short.MAX_VALUE))
        );
        jPanel8Layout.setVerticalGroup(
            jPanel8Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel8Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(startButton)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel3, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jPanel2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jPanel6, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jPanel7, javax.swing.GroupLayout.DEFAULT_SIZE, 449, Short.MAX_VALUE)
            .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jPanel8, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jPanel2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel8, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel3, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel6, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel7, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void loadComponents() {

        cThread = new ControlThread(this, Main.CONTROL_SRC_PORT);
        cThread.setRunning(true);
        cThread.start();

        bThread = new BuddiesUpdateThread(this, Main.BUDDIES_UPDATE_INTERVAL);
        bThread.setRunning(true);
        bThread.start();

        enableOnlyStartAndBuddylist();
    }

    private void unloadComponents() {
        cThread.setRunning(false);
        bThread.setRunning(false);
        namesListModel.clear();
        filesListModel.clear();
        enableOnlyStartAndBuddylist();
    }

    private void setGUIComponents(boolean choice0, boolean choice1, boolean choice2, boolean choice3, boolean choice4) {

        startButton.setEnabled(choice0);

        myIPText.setEnabled(choice1);
        myMacText.setEnabled(choice1);
        myNameText.setEnabled(choice1);

        NamesList.setEnabled(choice2);
        buddyIPText.setEnabled(choice2);
        buddyMACText.setEnabled(choice2);

        FilesList.setEnabled(choice3);

        dloadButton.setEnabled(choice4);
        jProgressBar1.setEnabled(choice4);
    }

    private void dloadDirButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_dloadDirButtonActionPerformed
        // TODO add your handling code here:
        dloadDirChooser = new JFileChooser();
        dloadDirChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        if (dloadDirChooser.showDialog(this, "Select Dir") == JFileChooser.APPROVE_OPTION) {
            DOWNLOAD_DIR_NAME = dloadDirChooser.getSelectedFile().getAbsolutePath();
            dloadDirText.setText(DOWNLOAD_DIR_NAME);
        }
        checkValidDirSelection();
        FTPDebug.printDebug("Selected download Dir", Main.DEBUG);
    }//GEN-LAST:event_dloadDirButtonActionPerformed

    private void uploadDirButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_uploadDirButtonActionPerformed
        // TODO add your handling code here:
        uploadDirChooser = new JFileChooser();
        uploadDirChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        if (uploadDirChooser.showDialog(this, "Select Dir") == JFileChooser.APPROVE_OPTION) {
            UPLOAD_DIR_NAME = uploadDirChooser.getSelectedFile().getAbsolutePath();
            uploadDirText.setText(UPLOAD_DIR_NAME);
        }
        checkValidDirSelection();
    }//GEN-LAST:event_uploadDirButtonActionPerformed

    private void startButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_startButtonActionPerformed
        // TODO add your handling code here:
        if (startButton.getText().equals("START")) {
            startButton.setText("STOP");
            loadComponents();
        } else {
            startButton.setText("START");
            unloadComponents();
        }
    }//GEN-LAST:event_startButtonActionPerformed

    private void NamesListValueChanged(javax.swing.event.ListSelectionEvent evt) {//GEN-FIRST:event_NamesListValueChanged
        // TODO add your handling code here:
    }//GEN-LAST:event_NamesListValueChanged

    private void NamesListMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_NamesListMouseClicked
        // TODO add your handling code here:
        updateBuddyDetailsOnSelection();

        NetMessage message;
        if ((message = (new UDPTxAck(this, getMyIP(), getBuddyIP(), Main.MSGCODE_SEND_FILE_LIST, Main.SEQ_FILE_REQUEST, Main.MSG_SEND_FILE_LIST, Main.CONTROL_SRC_PORT, Main.CONTROL_ACK_SRC_PORT, Main.NUM_TICKS_CONTROL, Main.CTRL_RX_TIMEOUT)).SendUDPTxAck()) == null) {
            JOptionPane.showMessageDialog(this, Main.ERROR_FILE_REQUEST);
        } else {
            if (Main.DEBUG) {
                System.out.println("Obtained file list:");
                System.out.println(message.getPayloadInString());
            }
            filesListModel.clear();
            String[] strArr = message.getPayloadInString().split(";");
            for (int i = 0; i < strArr.length; i++) {
                filesListModel.addElement(strArr[i]);
            }
            enableOnlyStartBuddylistFileslistDownload();
            System.out.flush();
        }

    }//GEN-LAST:event_NamesListMouseClicked

    private void dloadButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_dloadButtonActionPerformed
        // TODO add your handling code here:
        if (!NamesList.getSelectedValue().toString().equals("") && !FilesList.getSelectedValue().toString().equals("")) {

            NetMessage message;
            if ((message = (new UDPTxAck(this, getMyIP(), getBuddyIP(), Main.MSGCODE_LOCK_FTP, Main.SEQ_LOCK_FTP, Main.MSG_LOCK_FTP + ":" + FilesList.getSelectedValue().toString(), Main.CONTROL_SRC_PORT, Main.CONTROL_ACK_SRC_PORT, Main.MAX_TICKS_RXFTP, Main.DATA_TIMEOUT)).SendUDPTxAck()) == null) {
                JOptionPane.showMessageDialog(this, Main.ERROR_LOCK_FTP);
            } else {
                lockRxFTP(true);
                if (Main.DEBUG) {
                    System.out.println("Obtained message after dload press: " + message.getPayloadInString());
                    System.out.println("Rx locked!");
                    System.out.flush();
                }
                String strArr[] = FilesList.getSelectedValue().toString().split(":");
                RxFTPThread rxFTPThread = new RxFTPThread(this, Main.SEQ_DATA, Main.DATA_SRC_PORT, Main.DATA_ACK_SRC_PORT, Main.DATA_TIMEOUT, Main.RATE_CNTRL_MAX_DELAY, strArr[0], Integer.parseInt(strArr[1]), Main.FTP_BLOCK_SIZE);
                rxFTPThread.start();
                rxFTPThread.setRunning(true);
            }
        }
    }//GEN-LAST:event_dloadButtonActionPerformed

    private void updateBuddyDetailsOnSelection() {
        String[] strArr1;
        String[] strSplit1;
        FileHandler buddiesR = new FileHandler(Main.CONFIGFILEPATH);
        String selValue;

        /* Refer documentation from the initializeform function */
        if (buddiesR.openFile(0)) {
            strArr1 = buddiesR.readIntoArray();
            buddiesR.closeFile();

            try {

                if (NamesList.getSelectedValue() != null) {

                    selValue = NamesList.getSelectedValue().toString().trim();  // get the selected value from the list

                    /* Search for the selected value from the list into the hosts file to obtain the relevant IP and MAC
                     * addressed to be displayed in their relevant text boxes on the form */
                    for (int i = 0; i < strArr1.length; i++) {

                        strSplit1 = strArr1[i].split(" ");  // split each line from the hosts file into multiple strings

                        /* Update the text boxes when the match is found */
                        if (strSplit1[0].trim().equals(selValue.trim())) {
                            if (Main.DEBUG) {
                                System.out.println("Updating buddyIP " + strSplit1[1] + "and name " + strSplit1[0] + " upon selection in nameslist");
                                System.out.flush();
                            }
                            buddyIPText.setText(strSplit1[1]);
                            buddyMACText.setText(strSplit1[2]);
                        }
                    }
                }
                this.repaint();

            } catch (ArrayIndexOutOfBoundsException exc) {
                if (Main.DEBUG) {
                    System.out.println("Arrayoutofbounds in list selected");
                    exc.printStackTrace();
                }
            }
        }
    }

    private void disableAllGUIComponents() {
        setGUIComponents(false, false, false, false, false);
    }

    private void enableOnlyStartAndBuddylist() {
        setGUIComponents(true, false, true, false, false);
    }

    private void enableOnlyStartBuddylistFileslistDownload() {
        setGUIComponents(true, false, true, true, true);
    }

    private void checkValidDirSelection() {
        if (DOWNLOAD_DIR_NAME != null && UPLOAD_DIR_NAME != null) {
            if (!DOWNLOAD_DIR_NAME.equals("") && !UPLOAD_DIR_NAME.equals("")) {
                File fileDown = new File(DOWNLOAD_DIR_NAME);
                File fileUP = new File(UPLOAD_DIR_NAME);
                if (fileDown.exists() && fileUP.exists()) {
                    setGUIComponents(true, false, false, false, false);
                } else {
                    ENABLE_FTP = false;
                }
            }
        }
    }

    public void selectBuddy(String selName) {
        for (int i = 0; i < namesListModel.size(); i++) {
            if (namesListModel.getElementAt(i).toString().equals(selName)) {
                NamesList.setSelectedIndex(i);
                break;
            }
        }
        updateBuddyDetailsOnSelection();
    }

    public String getFileListFromUploadDir() {
        Process pr = null;
        String payload = null;

        String[] command = {"/bin/sh",
            "-c",
            "stat " + UPLOAD_DIR_NAME + "/* | grep -A 1 File | awk ' { print $2 } ' "
        };

        try {
            pr = Runtime.getRuntime().exec(command);
        } catch (IOException exc) {
            if (Main.DEBUG) {
                exc.printStackTrace();
            }
        }
        try {
            pr.waitFor();
        } catch (InterruptedException exc) {
            if (Main.DEBUG) {
                exc.printStackTrace();
            }
        }
        BufferedReader bReader = new BufferedReader(new InputStreamReader(pr.getInputStream()));
        String line = "";

        try {
            while ((line = bReader.readLine()) != null) {
                if (line.length() > 0) {
                    if (line.contains(UPLOAD_DIR_NAME + "/")) {
                        if (Main.DEBUG) {
                            System.out.println(line.split(UPLOAD_DIR_NAME + "/")[1].split("'")[0]);
                        }
                        payload = payload + line.split(UPLOAD_DIR_NAME + "/")[1].split("'")[0] + ":";
                    } else {
                        payload = payload + line.trim() + ";";
                    }
                }

            }
            bReader.close();
        } catch (IOException exc) {
            if (Main.DEBUG) {
                exc.printStackTrace();
            }
        }
        return payload;
    }

    public String getDownloadDirName() {
        if (DOWNLOAD_DIR_NAME != null) {
            return DOWNLOAD_DIR_NAME;
        } else {
            return null;
        }
    }

    public String getUploadDirName() {
        if (UPLOAD_DIR_NAME != null) {
            return UPLOAD_DIR_NAME;
        } else {
            return null;
        }
    }

    public void clearBuddiesPanel() {
        namesListModel.clear();
        buddyIPText.setText("");
        buddyMACText.setText("");
    }

    public void addBuddy(String name) {
        namesListModel.addElement(name);
        this.repaint();
    }

    public void addMyDetails(String name, String ip, String mac) {
        myIPText.setText(ip);
        myMacText.setText(mac);
        myNameText.setText(name);
        this.repaint();
    }

    public String getBuddyName() {
        if (namesListModel.getSize() > 0) {
            if (!NamesList.isSelectionEmpty()) {
                return NamesList.getSelectedValue().toString();
            } else {
                return null;
            }
        } else {
            return null;
        }
    }

    public String getBuddyIP() {
        if (namesListModel.getSize() > 0) {
            if (!NamesList.isSelectionEmpty() && buddyIPText.getText() != null) {
                return buddyIPText.getText();
            } else {
                return null;
            }
        } else {
            return null;
        }
    }

    public String getBuddyMac() {
        if (namesListModel.getSize() > 0) {
            if (!NamesList.isSelectionEmpty() && buddyMACText.getText() != null) {
                return buddyMACText.getText();
            } else {
                return null;
            }
        } else {
            return null;
        }
    }

    public String getMyIP() {
        if (myIPText.getText() != null) {
            return myIPText.getText();
        } else {
            return null;
        }
    }

    public String getMyMac() {
        if (myMacText.getText() != null) {
            return myMacText.getText();
        } else {
            return null;

        }

    }

    public String getMyName() {
        if (myNameText.getText() != null) {
            return myNameText.getText();
        } else {
            return null;
        }
    }

    public void lockTxFTP(boolean choice) {
        FTP_TX_LOCK = choice;
    }

    public void lockRxFTP(boolean choice) {
        FTP_RX_LOCK = choice;
    }

    public boolean isTxLocked() {
        return FTP_TX_LOCK;
    }

    public boolean isRxLocked() {
        return FTP_RX_LOCK;
    }

    public void setProgressBarValue(int value) {
        jProgressBar1.setValue(value);
    }

    public void setBandwidth(double value) {
        bwLabel.setText(String.valueOf(value));
    }

    public int getNumberOfNames() {
        return namesListModel.size();
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {

            public void run() {
                new MainForm().setVisible(true);
            }
        });
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JList FilesList;
    private javax.swing.JList NamesList;
    private javax.swing.JTextField buddyIPText;
    private javax.swing.JTextField buddyMACText;
    private javax.swing.JLabel bwLabel;
    private javax.swing.JButton dloadButton;
    private javax.swing.JButton dloadDirButton;
    private javax.swing.JLabel dloadDirLabel;
    private javax.swing.JLabel dloadDirLabel1;
    private javax.swing.JTextField dloadDirText;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel10;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JLabel jLabel6;
    private javax.swing.JLabel jLabel7;
    private javax.swing.JLabel jLabel8;
    private javax.swing.JLabel jLabel9;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JPanel jPanel6;
    private javax.swing.JPanel jPanel7;
    private javax.swing.JPanel jPanel8;
    private javax.swing.JProgressBar jProgressBar1;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JTextField myIPText;
    private javax.swing.JTextField myMacText;
    private javax.swing.JTextField myNameText;
    private javax.swing.JButton startButton;
    private javax.swing.JButton uploadDirButton;
    private javax.swing.JTextField uploadDirText;
    // End of variables declaration//GEN-END:variables
    private JFileChooser dloadDirChooser;
    private JFileChooser uploadDirChooser;
    private String DOWNLOAD_DIR_NAME;
    private String UPLOAD_DIR_NAME;
    private boolean ENABLE_FTP = false;
    private boolean FTP_TX_LOCK = false;
    private boolean FTP_RX_LOCK = false;
    private ControlThread cThread;
    private BuddiesUpdateThread bThread;
    private DefaultListModel namesListModel;
    private DefaultListModel filesListModel;
}
