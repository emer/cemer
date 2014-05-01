/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros;

import java.util.List;
import java.util.ArrayList;
import com.ecortex.emergent.api.EmergentConnection;
import java.io.FileReader;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import java.io.FileNotFoundException;
import java.io.IOException;
import org.json.simple.parser.ParseException;
import com.ecortex.emergent.api.EmergentException;
import com.jilk.ros.ROSClient;
import java.util.concurrent.CountDownLatch;


public class InterfaceManager {
    private final long VERSION = 2;
    private String configFilename;
    private String emergentIP;
    private Integer emergentPort;
    private String rosURI;
    private List<EmergentInterface> eInterfaces;
    private EmergentConnection emergent;
    private ROSClient ros;
    private JSONObject json;
    private CountDownLatch latch;
    private boolean running;
    private boolean ok;
    
    public InterfaceManager() {
        running = false;
        ok = false;
    }
    
    public boolean readConfig(String configFilename) {
        boolean result = false;
        this.configFilename = configFilename;
        json = readFile(configFilename);
        if ((json != null) && checkVersion(json) && getAddresses(json))
            result = true;
        return result;
    }
    
    public boolean build() {
        emergent = new EmergentConnection(emergentIP, emergentPort);
        ros = ROSClient.create(rosURI);
        eInterfaces = new ArrayList<EmergentInterface>();
        buildInterfaces(json.get("subscribe"), true, false);
        buildInterfaces(json.get("publish"), false, false);
        buildInterfaces(json.get("service"), false, true);
        return true;
    }
    
    public boolean connect() {
        try {
            emergent.Initialize();
            try {
                ros.connect();
                latch = new CountDownLatch(1);
                ok = true;
            }
            catch (Exception ex) {
                emergent.Shutdown();
                error("ROS connection error: " + ex.getMessage());
            }                    
        }
        catch (EmergentException ex) {
            error("Emergent connection error: " + ex.getMessage() + " (" + ex.getCause().getMessage() + ")");
        }
        return ok;
    }
    
    public void setDebug(boolean debug) {
        emergent.SetDebug(debug);
        ros.setDebug(debug);
    }
    
    public boolean start() {
        for (EmergentInterface eInterface : eInterfaces) 
            eInterface.start();   // error handling here maybe?
        running = true;
        return true;
    }
    
    public boolean stop() {
        for (EmergentInterface eInterface : eInterfaces) 
            eInterface.stop();
        running = false;
        return true;
    }
    
    public void kill() {
        latch.countDown();
    }
    
    public void await() {
        try {
            if (latch != null)
                latch.await();
        }
        catch (InterruptedException ex) {}
    }
    
    public void disconnect() {
        ros.disconnect();
        emergent.Shutdown();
    }
    
    public String getROSURI() {
        return rosURI;
    }
    
    public ROSClient getROS() {
        return ros;
    }
    
    public boolean isOK() {
        return ok;
    }
    
    public boolean isRunning() {
        return running;
    }
    
    private JSONObject readFile(String filename) {
        JSONObject result = null;
        try {
            FileReader r = new FileReader(filename);
            try {
                result = (JSONObject) (new JSONParser().parse(r));
            }
            catch (IOException ex) {
                fileError("I/O exception reading file.");
            }
            catch (ParseException ex) {
                fileError("Parse error: " + ex.getMessage());
            }
            finally {
                try {
                    r.close();
                }
                catch (IOException ex) {
                    fileError("I/O exception closing file.");
                }
            }
        }
        catch (FileNotFoundException ex) {
            fileError("File not found.");
        }

        return result;
    }
    
    private void buildInterfaces(Object interfaceArray,
            boolean isSubscription, boolean isService) {
        if (interfaceArray != null) {
            if (interfaceArray instanceof JSONArray) {
                for (Object item : (JSONArray) interfaceArray) {
                    try {
                        JSONObject json = (JSONObject) item;
                        if (isService)
                            buildService(json);
                        else buildTopic(json, isSubscription);
                    }
                    catch (ClassCastException ex) {
                        String whichArray = "Published topics";
                        if (isService)
                            whichArray = "Services";
                        else if (isSubscription)
                            whichArray = "Subscribed topics";
                        fileError(whichArray + " array: element was not JSON object.");
                    }

                }
            }
            else fileError("Topic list is not an array.");
        }
    }
    
    private void buildTopic(JSONObject json, boolean isSubscription) {
        String topic = (String) getValidated(json, "topic", String.class);
        String message = (String) getValidated(json, "message", String.class);
        String table = (String) getValidated(json, "table", String.class);
        String program = (String) getValidated(json, "program", String.class);
        Long period = (Long) getValidated(json, "period", Long.class);
        if (topic != null && message != null && table != null &&
                program != null && period != null) {
            try {
                if (isSubscription)
                    eInterfaces.add(new EmergentSubscribedTopic(emergent, ros,
                            topic, message, table, program, period));
                else {
                    if (period != null)
                        eInterfaces.add(new EmergentPublishedTopic(emergent, ros,
                                topic, message, table, program, period));
                }
            }
            catch (EmergentROSException ex) {
                fileError("for topic \'" + topic + "\', " + ex.getMessage());
            }
        }
    }

    private void buildService(JSONObject json) {
        String service = (String) getValidated(json, "service", String.class);
        String requestMessage = (String) getValidated(json, "requestmessage", String.class);
        String responseMessage = (String) getValidated(json, "responsemessage", String.class);
        String requestTable = (String) getValidated(json, "requesttable", String.class);
        String responseTable = (String) getValidated(json, "responsetable", String.class);
        String requestProgram = (String) getValidated(json, "requestprogram", String.class);
        String responseProgram = (String) getValidated(json, "responseprogram", String.class);
        Long period = (Long) getValidated(json, "period", Long.class);
        if (service != null &&
                requestMessage != null &&
                requestTable != null &&
                responseMessage != null &&
                responseTable != null &&
                requestProgram != null &&
                responseProgram != null &&
                period != null) {
            try {
                eInterfaces.add(new EmergentService(emergent, ros, service,
                        requestMessage, requestTable, requestProgram,
                        responseMessage, responseTable, responseProgram,
                        period));
            }
            catch (EmergentROSException ex) {
                fileError("for service \'" + service + "\', " + ex.getMessage());
            }
        }
    }

    private boolean checkVersion(JSONObject json) {
        boolean result = false;
        String fileType = (String) getValidated(json, "filetype", String.class);
        Long version = (Long) getValidated(json, "version", Long.class);
        if (fileType != null && version != null) {
            if (!fileType.equals("emergent-ros"))
                fileError("JSON file type was \'" + fileType + "\', should be \'emergent-ros\'");
            else if (version != VERSION)
                fileError("Version " + VERSION + " is required, was " + version);
            else result = true;
        }
        
        return result;
    }
    
    private boolean getAddresses(JSONObject json) {
        emergentIP = (String) getValidated(json, "emergent-ip", String.class);
        emergentPort = ((Long) getValidated(json, "emergent-port", Long.class)).intValue();
        rosURI = (String) getValidated(json, "ros-uri", String.class);
        
        return (emergentIP != null && emergentPort != null && rosURI != null);
    }
    
    private Object getValidated(JSONObject json, String fieldName, Class fieldClass) {
        Object result = null;
        try {
            result = fieldClass.cast(json.get(fieldName));
            if (result == null)
                fileError(fieldName + " not found.");
        }
        catch (ClassCastException ex) {
            fileError("Invalid type for " + fieldName);
        }
        
        return result;
    }
    
    
    private void fileError(String error) {
        error("For file \'" + configFilename + "\', " + error);
    }
    
    private void error(String error) {
        EmergentROS.error(this.getClass(), error);
    }

}
