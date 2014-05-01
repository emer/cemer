/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros;

import com.ecortex.emergent.api.EmergentConnection;
import com.ecortex.emergent.api.EmergentDataTable;
import com.ecortex.emergent.api.EmergentDataTableRow;
import com.ecortex.emergent.api.EmergentException;
import com.jilk.ros.message.Message;
import com.jilk.ros.ROSClient;
import com.jilk.ros.Service;

public class EmergentService extends EmergentInterface {
    protected Service<Message, Message> service;
    protected EmergentDataTable requestTable, responseTable;
    protected Class<? extends Message> requestType, responseType;
    protected String requestProgram, responseProgram;
    protected long requestPeriod;
    
    public EmergentService(EmergentConnection emergent, ROSClient ros, 
            String serviceName,
            String requestTypeName, String requestTableName, String requestProgram,
            String responseTypeName, String responseTableName, String responseProgram,
            long requestPeriod) {
        super(emergent, ros, "Service " + serviceName);
        
        RowSync requestMessage = getMessageInstance(requestTypeName);
        RowSync responseMessage = getMessageInstance(responseTypeName);
        this.requestType = (Class<? extends Message>) requestMessage.getClass();
        this.responseType = (Class<? extends Message>) responseMessage.getClass();
        this.service = new Service<Message, Message>(serviceName, requestType, responseType, ros);
        this.requestTable = new EmergentDataTable(requestTableName, requestMessage.blankRow(), emergent);
        this.responseTable = new EmergentDataTable(responseTableName, responseMessage.blankRow(), emergent);
        this.requestProgram = requestProgram;
        this.responseProgram = responseProgram;
        this.requestPeriod = requestPeriod;
    }
    
    @Override
    public void run() {
        // wait for messages, add to table and write
        while (true) {
            try {
                runProgram(requestProgram);

                try {
                    requestTable.Read();
                }
                catch (EmergentException ex) {
                    if (!ex.getMessage().equals("row_from '0' out of bounds"))  // kludge to handle the Emergent bug when no data
                        throw(ex);
                }
                for (EmergentDataTableRow requestRow : requestTable) {  // Processes all requests in table
                    RowSync message = (RowSync) requestType.newInstance();
                    message.fromRow(requestRow);
                    RowSync response = (RowSync) service.callBlocking((Message) message);
                    EmergentDataTableRow responseRow = response.createRow();
                    responseTable.add(responseRow);
                }
                
                if (requestTable.size() > 0) {
                    responseTable.Write();  // Appends all responses
                    responseTable.clear();
                    runProgram(responseProgram);
                    requestTable.clear();
                    requestTable.Overwrite();
                }
                
                Thread.sleep(requestPeriod);
            }
            catch (InstantiationException ex) {
                error("Unable to create instance of " + requestType.getName());
                break;
            }
            catch (IllegalAccessException ex) {
                error("Unable to access " + requestType.getName());
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
    }

}
