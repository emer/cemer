/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.gazebo;

import com.jilk.ros.ROSClient;
import java.util.concurrent.CountDownLatch;

public class EmergentGazeboGlue {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        CountDownLatch killSignal = new CountDownLatch(1);
        ROSClient ros = ROSClient.create("ws://127.0.0.1:9090");
        //System.out.println("Using remote host instead of localhost (debug locally).");
        //ROSClient ros = ROSClient.create("ws://162.243.149.154:9090");

        if (ros.connect()) {
            //ros.setDebug(true);
            GazeboCamera camera = new GazeboCamera(ros);
            GazeboMotion motion = new GazeboMotion(ros, camera);
            GlueControl control = new GlueControl(ros, "GazeboGlue", killSignal, motion, camera);
            control.start();
            try {
                killSignal.await();
            }
            catch (InterruptedException ex) {}
            control.pause();
            control.stop();
            ros.disconnect();
        }
        else System.out.println("Unable to connect to ROS on localhost.");
    }
}
