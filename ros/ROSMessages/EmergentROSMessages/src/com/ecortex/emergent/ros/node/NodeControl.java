/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.node;

import com.jilk.ros.message.Message;
import com.jilk.ros.message.MessageType;
 
@MessageType(string = "emergent_msgs/NodeControl")
public class NodeControl extends Message {
    public String node;
    public String operation;
    public String args;
}
