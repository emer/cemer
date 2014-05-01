/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.rowsync;

import com.ecortex.emergent.api.EmergentDataTableRow;

public class MotionCommandRow extends EmergentDataTableRow {
    public Byte motion_type;
    public Float motion_units;

    public MotionCommandRow() {}
}
    