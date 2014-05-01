/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros;

import com.ecortex.emergent.ros.node.EmergentNodeControl;
import com.jilk.ros.ROSClient;
import com.jilk.ros.Topic;
import com.jilk.ros.MessageHandler;
import com.ecortex.emergent.ros.node.NodeControl;
import com.ecortex.emergent.ros.node.NodeStatus;

public class CommandLine implements MessageHandler<NodeStatus> {
    private int infoCount;
    private static final String defaultFilename = "default.json";
    private static final String moduleAll = "all";
    private static final String operationLaunch = "launch";
    
    public CommandLine() {        
    }
    
    public void handle(String[] args) {
        if (args.length == 0) {
            printHelp();
        }
        else if (args.length > 0 && args.length < 4) {
            String filename;
            String module;
            
            if (args.length == 1) {
                module = moduleAll;
                filename = defaultFilename;
            }
            else if (args.length == 2) {
                module = args[1];
                filename = defaultFilename;
            }
            else {
                module = args[1];
                filename = args[2];
            }
            EmergentNodeControl.Operation operation = EmergentNodeControl.Operation.fromString(args[0]);
            if (operation != null)
                doCommand(operation, module, filename);
            else if (args[0].toUpperCase().equals(operationLaunch.toUpperCase())) {
                launch(filename);
            }
            else {
                System.out.println("Error: \'" + args[0] + "\' is not a valid command.");
                System.out.println();
                printHelp();
            }
        }
        else {
            System.out.println("Error: too many arguments");
            System.out.println();
            printHelp();
        }
    }
    
    private void printHelp() {
            System.out.println("Emeros manages Emergent-related ROS services.");
            System.out.println("    Usage: emeros <command> [<module>|all [filename]]");
            System.out.println("           where [filename] is a JSON format file with certain required elements.");
            System.out.println("           If it is omitted, the file \"default.json\" is used.");
            System.out.println("           Commands and module names are case insensitive.");
            System.out.println();
            System.out.println("Commands:");
            System.out.println("    launch:  launch a new Emergent-ROS interface (ignores module)");
            System.out.println("    run:     start managed service(s) running");
            System.out.println("    pause:   pause running service(s) without exiting");
            System.out.println("    kill:    cause managed service(s) to exit");
            System.out.println("    info:    get info on managed service(s)");
            System.out.println();
    }
    
    private void doCommand(EmergentNodeControl.Operation operation, String module, String filename) {
        InterfaceManager manager = new InterfaceManager();
        if (manager.readConfig(filename)) {
            ROSClient ros = ROSClient.create(manager.getROSURI());
            ros.connect();
            Topic<NodeControl> topic = new Topic(EmergentNodeControl.controlTopicName, NodeControl.class, ros);
            topic.advertise();
            NodeControl message = new NodeControl();
            if (module.toUpperCase().equals(moduleAll.toUpperCase()))
                message.node = "";
            else message.node = module;
            message.operation = operation.toString();
            message.args = "";
            if (operation.equals(EmergentNodeControl.Operation.INFO))
                info(ros, topic, message);
            else topic.publish(message);
            topic.unadvertise();
            ros.disconnect();
        }
        else EmergentROS.error(CommandLine.class, "Error reading configuration file.");
    }

    private void info(ROSClient ros, Topic<NodeControl> commandTopic, NodeControl message) {
        infoCount = 0;
        Topic<NodeStatus> infoTopic = new Topic(EmergentNodeControl.infoTopicName, NodeStatus.class, ros);
        infoTopic.subscribe(this);
        try {Thread.sleep(100);} catch (InterruptedException ex) {}  // this wait ensures that the responses come after the request
        commandTopic.publish(message);
        System.out.println("Waiting for responses...");
        System.out.println();
        try {
            Thread.sleep(4000); // 4 secs should be enough
        }
        catch (InterruptedException ex) {}
        if (infoCount == 0)
            System.out.println("No nodes responded.");
        infoTopic.unsubscribe();
    }
    
    
    @Override
    public void onMessage(NodeStatus info) {
        String empty = "";
        if (infoCount == 0) {
            printColumn("Node");
            printColumn("OK");
            printColumn("Running");
            printColumn("Message");
            System.out.println();
            for (int i = 0; i < 4; i++)
                printColumn("-------");
            System.out.println();
        }
        printColumn(info.node);
        if (info.ok)
            printColumn("X");
        else printColumn(empty);
        if (info.running)
            printColumn("X");
        else printColumn(empty);
        printColumn(info.message);
        System.out.println();
        infoCount++;
    }
    
    private void printColumn(String column) {
        String blanks = "                ";
        System.out.print(column);
        int remainder = blanks.length() - column.length();
        if (remainder > 0)
            System.out.print(blanks.substring(0, remainder));
    }
    
    private void launch(String filename) {
        try {
            InterfaceManager manager = new InterfaceManager();
            if (manager.readConfig(filename) && manager.build() && manager.connect()) {
                EmergentROSControl control = new EmergentROSControl(manager.getROS(), "EmergentROS", manager);
                control.start();
                try {
                    manager.setDebug(false);   // It appears that printing the big file contents creates huge
                                                // delays (about 20 sec) - must be false to run.
                    
                    manager.await();                   // wait until kill signal arrives
                }
                catch (Exception ex) {
                    EmergentROS.error(CommandLine.class, "Unhandled exception starting topics.");
                    ex.printStackTrace();
                }
                finally {
                    control.stop();
                    manager.stop();
                    manager.disconnect();
                }          
            }
            else EmergentROS.error(CommandLine.class, "InterfaceManager did not initialize.");
        }
        catch (Exception ex) {
            EmergentROS.error(CommandLine.class, "Unhandled exception creating InterfaceManager.");
            ex.printStackTrace();
        }
    }
}
