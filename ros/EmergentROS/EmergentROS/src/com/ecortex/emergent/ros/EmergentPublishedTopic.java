/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros;

import com.ecortex.emergent.api.EmergentConnection;
import com.ecortex.emergent.api.EmergentDataTableRow;
import com.jilk.ros.ROSClient;
import com.jilk.ros.message.Message;
import com.ecortex.emergent.api.EmergentException;

public class EmergentPublishedTopic extends EmergentTopic implements Runnable {
    
    public EmergentPublishedTopic(EmergentConnection emergent, ROSClient ros,
            String topicId, String typeName, String tableName, String program, long period) {
        super(emergent, ros, "Published Topic " + topicId, topicId, typeName, tableName, program, period);
    }
        
    @Override
    public void run() {
        topic.advertise();
        
        // wait for messages, add to table and write
        while (true) {
            try {
                runProgram(program);

                try {
                    table.Read();
                }
                catch (EmergentException ex) {
                    if (!ex.getMessage().equals("row_from '0' out of bounds"))  // kludge to handle the Emergent bug when no data
                        throw(ex);
                }
                for (EmergentDataTableRow row : table) {     // publishes all messages in table
                    RowSync message = (RowSync) type.newInstance();
                    message.fromRow(row);
                    topic.publish((Message) message);
                }
                if (table.size() > 0) {
                    table.clear();
                    table.Overwrite();
                }
                Thread.sleep(period);
            }
            catch (InstantiationException ex) {
                error("Unable to create instance of " + type.getName());
                break;
            }
            catch (IllegalAccessException ex) {
                error("Unable to access " + type.getName());
                break;
            }
            catch (EmergentException ex) {
                error("Emergent API exception: " + ex.getMessage());            
                break;
            }
            catch (InterruptedException ex) {
                break;
            }
            if (Thread.currentThread().isInterrupted()) break;
        }
        topic.unadvertise();
    }

}

