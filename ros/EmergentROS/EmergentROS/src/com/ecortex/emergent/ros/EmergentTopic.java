/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros;

import com.ecortex.emergent.api.EmergentConnection;
import com.ecortex.emergent.api.EmergentDataTable;
import com.jilk.ros.Topic;
import com.jilk.ros.message.Message;
import com.jilk.ros.ROSClient;

public abstract class EmergentTopic extends EmergentInterface {
    protected Topic<Message> topic;
    protected EmergentDataTable table;
    protected Class<? extends Message> type;
    protected String program;
    protected long period;
    
    public EmergentTopic(EmergentConnection emergent, ROSClient ros, String threadName,
            String topicId, String typeName, String tableName, String program, Long period) {
        super(emergent, ros, threadName);
        
        RowSync message = getMessageInstance(typeName);
        this.type = (Class<? extends Message>) message.getClass();
        this.topic = new Topic<Message>(topicId, type, ros);
        this.table = new EmergentDataTable(tableName, message.blankRow(), emergent);
        this.program = program;
        this.period = period;
    }    
}
