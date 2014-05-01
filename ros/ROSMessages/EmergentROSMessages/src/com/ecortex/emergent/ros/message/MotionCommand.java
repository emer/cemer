/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.message;

import com.jilk.ros.message.Message;
import com.jilk.ros.message.MessageType;
 
@MessageType(string = "emergent_msgs/MotionCommand")
public class MotionCommand extends Message {
    public boolean motion_type;
    public float motion_units;
}
