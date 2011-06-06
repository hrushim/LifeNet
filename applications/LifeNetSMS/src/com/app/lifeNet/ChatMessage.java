package com.app.lifeNet;

import java.io.Serializable;
import java.util.Date;

/**
 *
 * @author hrushi
 */
public class ChatMessage implements Serializable {

    static final long serialVersionUID = 3000;
    public int type;
    public String srcName;
    public long seq;
    String payload;
    Date rxTime;

    public ChatMessage(String src_name, long seq_num, long payload_length, String pay_load, int m_type) {

        srcName = src_name;
        /*destName = dest_name;*/
        seq = seq_num;
        payload = pay_load;
        type = m_type;
    }

}
