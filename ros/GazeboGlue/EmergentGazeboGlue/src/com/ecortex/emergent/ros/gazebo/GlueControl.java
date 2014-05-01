/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.gazebo;

import com.jilk.ros.message.Empty;
import com.jilk.ros.Service;
import com.ecortex.emergent.ros.node.EmergentNodeControl;
import com.jilk.ros.ROSClient;
import java.util.concurrent.CountDownLatch;

public class GlueControl extends EmergentNodeControl {
    CountDownLatch latch;
    GazeboMotion motion;
    GazeboCamera camera;
    Service<Empty, Empty> gazeboPause;
    Service<Empty, Empty> gazeboUnPause;
    Empty empty;
    
    public GlueControl(ROSClient ros, String name, CountDownLatch latch,
            GazeboMotion motion, GazeboCamera camera) {
        super(ros, name);
        this.latch = latch;
        this.motion = motion;
        this.camera = camera;
        gazeboPause = new Service<Empty, Empty>("/gazebo/pause_physics", Empty.class, Empty.class, ros);
        gazeboUnPause = new Service<Empty, Empty>("/gazebo/unpause_physics", Empty.class, Empty.class, ros);
        empty = new Empty();
    }
    
    @Override
    public void run(String args) {
        camera.start();
        motion.start();
        gazebo(true);
    }
    
    @Override
    public void pause() {
        motion.stop();
        camera.stop();
        gazebo(false);
    }
    
    @Override
    public void kill() {
        latch.countDown();
    }
    
    @Override
    public boolean isOK() {
        return true;
    }
    
    @Override
    public boolean isRunning() {
        return (motion.isRunning() && camera.isRunning());
    }
    
    @Override
    public String getStatusMessage() {
        return "";
    }

    @Override
    public void onError(String message) {
        System.out.println(message);
    }
    
    private void gazebo(boolean run) {
        if (run)
            gazeboUnPause.call(empty);
        else gazeboPause.call(empty);
    }

    
}
