/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.rowsync;

import com.jilk.ros.message.MessageType;
 
import com.ecortex.emergent.api.EmergentDataTableRow;
import com.ecortex.emergent.ros.RowSync;

import com.ecortex.emergent.ros.message.MotionCommand;

@MessageType(string = "emergent_msgs/MotionCommand")
public class MotionCommandMessage extends MotionCommand implements RowSync<MotionCommandRow> {
    private static final byte trueByte = 1;
    private static final byte falseByte = 0;
    
    @Override
    public EmergentDataTableRow createRow() {
        MotionCommandRow row = (MotionCommandRow) blankRow();
        if (motion_type)
            row.motion_type = trueByte;
        else row.motion_type = falseByte;
        row.motion_units = motion_units;
        return row;
    }
    
    @Override
    public EmergentDataTableRow blankRow() {
        return new MotionCommandRow();
    }
    
    @Override
    public void fromRow(MotionCommandRow row) {
        if (row.motion_type == trueByte)
            motion_type = true;
        else motion_type = false;
        motion_units = row.motion_units;
    }
}
