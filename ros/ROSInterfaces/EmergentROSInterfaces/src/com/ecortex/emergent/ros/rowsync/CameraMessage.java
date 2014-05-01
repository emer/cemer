/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.rowsync;

import com.ecortex.emergent.api.EmergentDataTableRow;
import com.ecortex.emergent.ros.EmergentInterface;
import com.ecortex.emergent.ros.RowSync;
import com.ecortex.emergent.ros.message.CompressedImage;

import com.jilk.ros.message.MessageType;

@MessageType(string = "sensor_msgs/CompressedImage")
public class CameraMessage extends CompressedImage implements RowSync<CameraRow> {
    
    @Override
    public EmergentDataTableRow createRow() {
        CameraRow row = (CameraRow) blankRow();
        row.filename = EmergentInterface.asImageFile("Camera", "jpg", data);
        return row;
    }
    
    @Override
    public EmergentDataTableRow blankRow() {
        return new CameraRow();
    }
    
    @Override
    public void fromRow(CameraRow row) {
        // not implemented
    }

}
