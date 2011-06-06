package com.app.lifeNet;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;


/**
*
* @author hrushi
*/
public class UDPSocketRx {

   /* Unit tested with UDPSocketTx */
   public static ChatMessage RxMessage(int timeOut, int srcP) {
       ChatMessage message = null;
       ByteArrayInputStream byteIStream;
       ObjectInputStream iStream;
       byte[] buf = new byte[4096];
       DatagramPacket packet = new DatagramPacket(buf, buf.length);
       DatagramSocket socketRcv = null;

       try {
           socketRcv = new DatagramSocket(srcP);
           System.out.flush();
       } catch (SocketException exc) {
           if (false) {
               exc.printStackTrace();
           }
       }


       if (socketRcv != null) {
           try {
               socketRcv.setSoTimeout(timeOut);
           } catch (SocketException exc) {
               if (false) {
                   exc.printStackTrace();
               }
               socketRcv.close();

           }
           try {
               socketRcv.receive(packet);
               //buf = new BASE64Decoder().decodeBuffer(new String(buf));
           } catch (IOException exc) {
               if (false) {
                   exc.printStackTrace();
               }
               socketRcv.close();
           }

           if (!socketRcv.isClosed()) {

               byteIStream = new ByteArrayInputStream(buf);
               if (byteIStream != null) {
                   try {
                       iStream = new ObjectInputStream(byteIStream);
                       try {
                           message = (ChatMessage) iStream.readObject();
                           System.out.println("UdpSocketRx::RxMessage() Receiving MSG payload:" + message.payload + " seq: " + String.valueOf(message.seq) + " type: " + String.valueOf(message.type));
                           System.out.flush();
                       } catch (ClassNotFoundException exc) {
                               exc.printStackTrace();
                           if (false) {
                               exc.printStackTrace();
                           }
                       }
                   } catch (IOException exc) {
                       if (false) {
                           exc.printStackTrace();
                       }
                   }

               }
           }
       }
       if (!socketRcv.isClosed()) {
           socketRcv.close();
       }
       return message;
   }
}
