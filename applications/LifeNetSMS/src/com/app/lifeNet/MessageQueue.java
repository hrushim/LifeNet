package com.app.lifeNet;

import java.util.Vector;

public class MessageQueue {

	public static boolean newMsgReadFlag = false;

	public static int unreadCount;
	public static Vector unreadMessageVector = new Vector();

	public static int readCount;
	public static Vector readMessageVector = new Vector();
}
