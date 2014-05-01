/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros;

import com.ecortex.emergent.ros.node.EmergentNodeControl;
import com.jilk.ros.ROSClient;

public class EmergentROSControl extends EmergentNodeControl {
    InterfaceManager manager;    
    
    public EmergentROSControl(ROSClient ros, String name, InterfaceManager manager) {
        super(ros, name);
        this.manager = manager;
    }
    
    @Override
    public void run(String args) {
        manager.start();
    }
    
    @Override
    public void pause() {
        manager.stop();
    }
    
    @Override
    public void kill() {
        manager.kill();
    }
    
    @Override
    public boolean isOK() {
        return manager.isOK();
    }
    
    @Override
    public boolean isRunning() {
        return manager.isRunning();
    }
    
    @Override
    public String getStatusMessage() {
        return "";
    }

    @Override
    public void onError(String message) {
        EmergentROS.error(EmergentROSControl.class, message);
    }
    
}
