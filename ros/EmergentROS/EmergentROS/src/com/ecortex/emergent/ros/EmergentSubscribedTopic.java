/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros;

import com.ecortex.emergent.api.EmergentDataTableRow;
import com.ecortex.emergent.api.EmergentConnection;
import com.jilk.ros.ROSClient;

public class EmergentSubscribedTopic extends EmergentTopic implements Runnable {

    public EmergentSubscribedTopic(EmergentConnection emergent, ROSClient ros,
            String topicId, String typeName, String tableName, String program, Long period) {
        super(emergent, ros, "Subscribed Topic " + topicId, topicId, typeName, tableName, program, period);
    }
        
    @Override
    public void run() {
        topic.subscribe();
        
        // wait for messages, add to table and write
        long nextTime = System.currentTimeMillis();
        while (true) {
            try {
                RowSync message = (RowSync) topic.take();
                long now = System.currentTimeMillis();
                if (now >= nextTime) {
                    EmergentDataTableRow row = message.createRow();
                    table.add(row);
                    table.Write();  // Appends new messages
                    table.clear();
                    runProgram(program);
                    nextTime = now + period;
                }
            }
            catch (InterruptedException ex) {
                break;
            }
            if (Thread.currentThread().isInterrupted()) break;
        }
        topic.unsubscribe();
    }
}
