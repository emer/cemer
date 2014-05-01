/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.node;

import com.jilk.ros.message.Message;
import com.jilk.ros.message.MessageType;
 
@MessageType(string = "emergent_msgs/NodeStatus")
public class NodeStatus extends Message {
    public String node;
    public boolean ok;
    public boolean running;
    public String message;
}
