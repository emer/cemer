/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros;

import com.ecortex.emergent.api.EmergentConnection;
import com.jilk.ros.message.Message;
import com.jilk.ros.ROSClient;
import java.io.File;
import java.io.IOException;
import java.io.FileOutputStream;
import java.io.ByteArrayInputStream;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;

public abstract class EmergentInterface implements Runnable {
    protected EmergentConnection emergent;
    protected ROSClient ros;
    private String threadName;
    private Thread thread;
    
    public EmergentInterface(EmergentConnection emergent, ROSClient ros, String threadName) {
        this.emergent = emergent;
        this.ros = ros;
        this.threadName = threadName;
        thread = null;
    }
    
    protected RowSync getMessageInstance(String typeName) {
        String errorMessage = "Unable to create message type \'" + typeName + "\'.";
        try {
            Class realType = Class.forName(typeName);
            Class<? extends Message> type;
            if (Message.class.isAssignableFrom(realType))
                type = realType;
            else throw new ClassCastException();
            return (RowSync) type.newInstance();
        }
        catch (ClassNotFoundException ex) {
            error("Class " + typeName + " not found.");
            throw new EmergentROSException(errorMessage, ex);
        }
        catch (ClassCastException ex) {
            error("Class " + typeName + " does not extend Message.");
            throw new EmergentROSException(errorMessage, ex);
        }
        catch (InstantiationException ex) {
            error("Unable to create instance of class" + typeName);
            throw new EmergentROSException(errorMessage, ex);
        }
        catch (IllegalAccessException ex) {
            error("Illegal Access Exception for: " + typeName);
            throw new EmergentROSException(errorMessage, ex);
        }
    }

    protected void runProgram(String program) throws InterruptedException {
        if (!program.equals("")) {  // blank program means nothing to run
            // Make sure no other programs are running or it will error
            while (emergent.GetRunState() == emergent.GLOBAL_RUNSTATUS_RUNNING)
                Thread.sleep(10);
            emergent.RunProgram(program);
        }
    }
   
    public static String asImageFile(String prefix, String suffix, byte[] data) {
        String filename = prefix + "." + System.currentTimeMillis() + "." + suffix;
        File file = new File(filename);
        String result =  file.getAbsolutePath();
        try {
            if (file.createNewFile()) {
                ByteArrayInputStream inStream = new ByteArrayInputStream(data);
                BufferedImage image = ImageIO.read(inStream);
                ImageIO.write(image, "jpeg", file);
            }
            else result = "Duplicate filename " + filename;
        }
        catch (IOException ex) {
            result = ex.getMessage();
        }
        
        return result;
    }        
    
    @Override
    public abstract void run();

    public void start() {
        if (thread == null) {
            thread = new Thread(this);
            thread.setName(threadName);
            thread.setPriority(Thread.MAX_PRIORITY);
            thread.start();
        }
    }
    
    public void stop() {
        if (thread != null && thread.isAlive())
            thread.interrupt();
        thread = null;
    }
    
    protected void error(String error) {
        EmergentROS.error(this.getClass(), error);
    }
    
}
