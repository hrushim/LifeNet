package com.app.lifeNet;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

/**
 *
 * @author hrushi
 */
public class UdpTx {

	public static void send(String destIp, ChatMessage msg, int repeatCount) {

		InetAddress ipAdd = null;
		DatagramPacket packet = null;
		ByteArrayOutputStream byteStream = null;
		ObjectOutputStream oStream = null;
		byte[] buf;
		DatagramSocket socketSend = null;
		ChatMessage rcvMessg = null;
		int msgRepeatCount = 0;

		System.out.println("UdpTx::send() Sending MSG payload:" + msg.payload + " seq: " + String.valueOf(msg.seq) + " type: " + String.valueOf(msg.type));
        System.out.flush();
		
        while (msgRepeatCount < repeatCount) {
			if (msg != null) {

				try {
					socketSend = new DatagramSocket();
				} catch (SocketException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					break;
				}
				try {
					ipAdd = InetAddress.getByName(destIp);
				} catch (UnknownHostException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					break;
				}

				byteStream = new ByteArrayOutputStream();
				if (byteStream != null) {

					try {
						oStream = new ObjectOutputStream(byteStream);
						oStream.writeObject(msg);
						oStream.flush();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
						break;
					}

					buf = byteStream.toByteArray();
					packet = new DatagramPacket(buf, buf.length, ipAdd, LifeNet.MSG_SRC_DATA_PORT);

					if (packet != null) {

						try {
							socketSend.send(packet);
							byteStream.flush();
							
							rcvMessg = UDPSocketRx
									.RxMessage(LifeNet.MSG_RCV_TIMEOUT, LifeNet.MSG_SRC_ACK_PORT);
							if(rcvMessg != null)
							{
								if (rcvMessg.seq != msg.seq) {
									rcvMessg = null;
									msgRepeatCount++;
								}
								else
									break;
							}
							else
							{
								msgRepeatCount++;
								continue;
							}
							

							break;
						} catch (IOException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
							break;
						}
					}
					else
						break;
				}
				else
					break;
			}
			else
				break;
		}
	}
	
    public static void reply(String destIp, ChatMessage msg) {

		InetAddress ipAdd = null;
		DatagramPacket packet = null;
		ByteArrayOutputStream byteStream = null;
		ObjectOutputStream oStream = null;
		byte[] buf;
		DatagramSocket socketSend = null;

		System.out.println("UdpTx::reply() Sending MSG payload:" + msg.payload + " seq: " + String.valueOf(msg.seq) + " type: " + String.valueOf(msg.type));
        System.out.flush();

		if (msg != null) {

			try {
				socketSend = new DatagramSocket();
			} catch (SocketException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			try {
				ipAdd = InetAddress.getByName(destIp);
			} catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			byteStream = new ByteArrayOutputStream();
			if (byteStream != null) {

				try {
					oStream = new ObjectOutputStream(byteStream);
					oStream.writeObject(msg);
					oStream.flush();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

				buf = byteStream.toByteArray();
				packet = new DatagramPacket(buf, buf.length, ipAdd, LifeNet.MSG_SRC_ACK_PORT);

				if(packet!=null) {

					try {
						socketSend.send(packet);
						byteStream.flush();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
		}
	}
}
