/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.message;

import com.jilk.ros.message.Message;
import com.jilk.ros.message.MessageType;
import com.jilk.ros.message.Header;
import com.jilk.ros.rosbridge.indication.Base64Encoded;

@MessageType(string = "sensor_msgs/CompressedImage")
public class CompressedImage extends Message {
    public Header header;
    public String format;
    @Base64Encoded public byte[] data;
}
