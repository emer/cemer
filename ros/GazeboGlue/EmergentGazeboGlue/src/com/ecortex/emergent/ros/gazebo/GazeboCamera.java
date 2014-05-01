/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.gazebo;

import com.jilk.ros.ROSClient;
import com.jilk.ros.MessageHandler;
import com.jilk.ros.Topic;

import com.ecortex.emergent.ros.message.CompressedImage;

import java.util.concurrent.CountDownLatch;

public class GazeboCamera implements MessageHandler<CompressedImage> {
    private static final String imageTopicName = "/smartphone/camera/image/compressed";
    private static final String cameraTopicName = "/emergent/camera";
    private Topic<CompressedImage> cameraTopic;
    private Topic<CompressedImage> imageTopic;
    private CountDownLatch latch;
    private CompressedImage lastImage;
    private boolean running;
    
    public GazeboCamera(ROSClient ros) {
        imageTopic = new Topic<CompressedImage>(imageTopicName, CompressedImage.class, ros);
        cameraTopic = new Topic<CompressedImage>(cameraTopicName, CompressedImage.class, ros);
    }
    
    public void start() {
        if (!running) {
            closeLatch();
            imageTopic.subscribe(this);
            cameraTopic.advertise();
            running = true;
        }
    }
    
    public void stop() {
        if (running) {
            closeLatch();
            cameraTopic.unadvertise();
            imageTopic.unsubscribe();
            running = false;
        }
    }
    
    public boolean isRunning() {
        return running;
    }
            
    @Override
    public void onMessage(CompressedImage image) {
        lastImage = image;
        if (latch != null)
            latch.countDown();
    }
    
    public void publishNext() {
        if (latch == null) {
            latch = new CountDownLatch(1);
            try {
                latch.await();
                cameraTopic.publish(lastImage);
                latch = null;
            }
            catch (InterruptedException ex) {}
        }
    }
    
    private void closeLatch() {
        if (latch != null)
            latch.countDown();
        latch = null;  // important to do this outside the 'if'
    }
}
