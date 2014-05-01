/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.rowsync;

import com.ecortex.emergent.api.EmergentDataTableRow;
import com.ecortex.emergent.ros.RowSync;
import com.ecortex.emergent.ros.message.Reward;

import com.jilk.ros.message.MessageType;

@MessageType(string = "emergent_msgs/Reward")
public class RewardMessage extends Reward implements RowSync<RewardRow> {
    
    @Override
    public EmergentDataTableRow createRow() {
        RewardRow row = (RewardRow) blankRow();
        row.amount = amount;
        return row;
    }
    
    @Override
    public EmergentDataTableRow blankRow() {
        return new RewardRow();
    }
    
    @Override
    public void fromRow(RewardRow row) {
        amount = row.amount;
    }
}
