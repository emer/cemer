/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros;

import com.ecortex.emergent.api.EmergentDataTableRow;

public interface RowSync<T extends EmergentDataTableRow> {
    public EmergentDataTableRow createRow();
    public EmergentDataTableRow blankRow();
    public void fromRow(T row);
}
