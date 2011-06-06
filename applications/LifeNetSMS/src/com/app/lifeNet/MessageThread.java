package com.app.lifeNet;

import java.util.Calendar;
import java.util.Hashtable;

/**
 *
 * @author hrushi
 */

public class MessageThread extends Thread {

	LifeNet form;
	private boolean RUN_FLAG = false;
	int sleepInterval;
	Hashtable seqNumHash;

	public MessageThread(LifeNet lifenet, int sleepTime) {

		sleepInterval = sleepTime;
		seqNumHash = new Hashtable();
		form = lifenet;
	}

	public void run() {
		System.out.println("Inside Listen thread on port " + LifeNet.MSG_SRC_DATA_PORT);
		System.out.flush();
		while (RUN_FLAG) {
			ChatMessage message = null;
			message = UDPSocketRx.RxMessage(LifeNet.MSG_RCV_TIMEOUT, LifeNet.MSG_SRC_DATA_PORT);
			if (message != null) {

				if (message.seq == 0) 
				{

					seqNumHash.remove(message.srcName);
					seqNumHash.put(message.srcName, new Long(0));
					String messgSource = message.srcName;
					message.type = LifeNet.MSG_TYPE_ACK;
					message.srcName = LifeNetApi.getMyName();
					UdpTx.reply(LifeNetApi.getIpFromName(messgSource), message);

				} 
				else 
				{

					if (seqNumHash.containsKey(message.srcName)) 
					{
						if (new Long(message.seq) > (Long) seqNumHash.get(message.srcName)) 
						{
							System.out.println("Check 0");
							System.out.flush();
							seqNumHash.remove(message.srcName);
							System.out.println("Check 1");
							System.out.flush();
							seqNumHash.put(message.srcName, new Long(
									message.seq));
							System.out.println("Check 2");
							System.out.flush();
							String messgSource = message.srcName;
							message.type = LifeNet.MSG_TYPE_ACK;
							message.srcName = LifeNetApi.getMyName();
							message.rxTime = Calendar.getInstance().getTime();
							System.out.println("Check 3");
							System.out.flush();
							UdpTx.reply(LifeNetApi.getIpFromName(messgSource),
									message);
							MessageQueue.unreadMessageVector.add(message);
							MessageQueue.unreadCount++;
						} 
						else 
						{
							System.out.println("Dropping packet" + " Seq: " + String.valueOf(message.seq));
							System.out.flush();
						}
					} 
					else 
					{

						seqNumHash.remove(message.srcName);
						seqNumHash.put(message.srcName, new Long(
								message.seq));
						String messgSource = message.srcName;
						message.type = LifeNet.MSG_TYPE_ACK;
						message.srcName = LifeNetApi.getMyName();
						message.rxTime = Calendar.getInstance().getTime();
						UdpTx.reply(LifeNetApi.getIpFromName(messgSource),
								message);
						MessageQueue.unreadMessageVector.add(message);
						MessageQueue.unreadCount++;
					}
				}

			}
			try {
				Thread.sleep(sleepInterval);
			} catch (InterruptedException ex) {

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
