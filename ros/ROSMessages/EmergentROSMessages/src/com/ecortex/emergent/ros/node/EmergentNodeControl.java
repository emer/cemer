/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.node;

import java.util.Map;
import java.util.HashMap;

import com.jilk.ros.ROSClient;
import com.jilk.ros.MessageHandler;
import com.jilk.ros.Topic;

public abstract class EmergentNodeControl implements MessageHandler<NodeControl> {
    public static enum Operation {
        RUN,
        PAUSE,
        KILL,
        INFO;
    
        private static final Map<String, Operation> stringMap =
            new HashMap<String, Operation>();

        static {
            for (Operation operation : Operation.values())
                stringMap.put(operation.toString(), operation);
        }    

        public static Operation fromString(String string) {
            return stringMap.get(string.toUpperCase());
        }
    }
    
    public final static String controlTopicName = "/emergent/control";
    public final static String infoTopicName = "/emergent/info";
    
    private String name;
    private Topic<NodeControl> controlTopic;
    private Topic<NodeStatus> infoTopic;
    
    public EmergentNodeControl(ROSClient ros, String name) {
        this.name = name;
        controlTopic = new Topic<NodeControl>(controlTopicName, NodeControl.class, ros);
        infoTopic = new Topic<NodeStatus>(infoTopicName, NodeStatus.class, ros);
    }
    
    public void start() {
        controlTopic.subscribe(this);
        infoTopic.advertise();
    }
    
    public void stop() {
        controlTopic.unsubscribe();
        infoTopic.unadvertise();
    }
    
    @Override
    public void onMessage(NodeControl controlMessage) {
        if (controlMessage.node.length() == 0 || controlMessage.node.toUpperCase().equals(name.toUpperCase())) {
            Operation operation = Operation.fromString(controlMessage.operation);
            if (operation != null) {
                switch (operation) {
                    case RUN:
                        run(controlMessage.args);
                        break;
                    case PAUSE:
                        pause();
                        break;
                    case KILL:
                        kill();
                        break;
                    case INFO:
                        info();
                        break;
                }
            }
            else onError("EmergentNodeControl: unknown operation \'" + controlMessage.operation + "\'.");
        }
    }
    
    private void info() {
        NodeStatus status = new NodeStatus();
        status.node = name;
        status.ok = isOK();
        status.running = isRunning();
        status.message = getStatusMessage();
        infoTopic.publish(status);
    }
    
    public abstract void run(String args);
    public abstract void pause();
    public abstract void kill();
    public abstract boolean isOK();
    public abstract boolean isRunning();
    public abstract String getStatusMessage();
    public abstract void onError(String message);
}
