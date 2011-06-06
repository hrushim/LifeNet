/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Project: LifeNet
Developers: Hrushikesh Mehendale, Santosh Vempala
Georgia Institute of Technology, Atlanta, USA
 */
package com.app.lifeNet;

import java.io.*;
import java.util.Arrays;

public class NetMessage implements Serializable {

    static final long serialVersionUID = 3000;
    private char[] srcIP;
    private char[] destIP;
    private int type;
    private long seq;
    private byte[] payload;

    /* Unit testing done */
    public NetMessage(char[] src_ip, char[] dest_ip, long seq_num, int payload_length, String pay_load, int m_type) {

        srcIP = src_ip;
        destIP = dest_ip;
        seq = seq_num;
        type = m_type;
        payload = pay_load.getBytes();
    }

    /* Unit testing done */
    public NetMessage(char[] src_ip, char[] dest_ip, long seq_num, byte[] pay_load, int m_type) {

        srcIP = src_ip;
        destIP = dest_ip;
        seq = seq_num;
        type = m_type;
        payload = Arrays.copyOf(pay_load, pay_load.length);
    }

    /* Unit testing done */
    public void printData() {
            System.out.println("==== NetMessage Object ====");
            System.out.println("SrcIP: " + String.valueOf(srcIP));
            System.out.println("DestIP " + String.valueOf(destIP));
            System.out.println("SeqNum: " + seq);
            System.out.println("Type: " + type);
            System.out.println("Payload: " + new String(payload));
            System.out.println("==== End NetMessage Object ====");
            System.out.flush();
    }

    /* Unit testing done */
    public int getType() {
        return type;
    }

    /* Unit testing done */
    public long getSeq() {
        return seq;
    }

    /* Unit testing done */
    public long getSerial() {
        return serialVersionUID;
    }

    /* Unit testing done */
    public String getSrcIP() {
        return String.valueOf(srcIP);
    }


    /* Unit testing done */
    public String getDestIP() {
        return String.valueOf(destIP);
    }

    /* Unit testing failed */
    public byte[] getPayload() {
        return Arrays.copyOf(payload, payload.length);
    }

    /* Unit testing done */
    public String getPayloadInString() {
        return (new String(payload));
    }

    public long getPayloadLength() {
        return payload.length;
    }
}
