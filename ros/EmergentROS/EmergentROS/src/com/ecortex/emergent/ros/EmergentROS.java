/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros;

public class EmergentROS {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        new CommandLine().handle(args);
    }
    

    static void warning(Class errorClass, String message) {
        System.out.println("WARNING (" + errorClass.getName() + ") " + message);
    }

    static void error(Class errorClass, String message) {
        System.out.println("ERROR (" + errorClass.getName() + ") " + message);
    }
        
}
    
    
    /*
     * JSON file:
     * - the URI to emergent and to ROSBridge
     * - Each topic subscribed to. Need topic id, class name, table name, program to call after loading table.
     * - Each topic published. Need topic id, class name, program to call to load table, wait period.
     *         If no program specified, just poll the table
     * - Each service to be used (includes both a subscribe and publish effect on emergent side)
     *         Needs service id, input class, output class, input table, output table, poll frequency, program to call when result ready
     * 
     * Data validation:
     * - Check first to ensure that the topic / service data types verify with ROS using verify().
     * - Then add some sort of data type checking to the Emergent API or in this system to ensure that emergent tables work
     * - Do a back and forth conversion between ROS message and emergent type to ensure that it works.
     * - Finally, every inbound message data values must be checked. I need to add this sort of checking to the ROSBridge interface as well,
     *   but that belongs there.
     * 
     * General data validation: It seems like it would be very helpful to have a general-purpose object comparison method
     *    where two objects are the same class, that uses the fields (and recurses) to compare all the individual leaf fields.
     * 
     * Terminating the program: 
     * - For now, might be easiest just to detect that ROSBridge or Emergent are no longer connected.
     * 
     * Subscribed topic:
     * - Class EmergentSubscribedTopic. It's a thread and it calls take() in a loop. When take() returns, add the row,
     *   call the specified Program, and wait for the Program to complete; then clear the table.
     * 
     * Published topic:
     * - Class EmergentPublishedTopic. It's a thread and it sleeps for the indicated period each time through the loop. Then it calls
     *   the indicated Program, grabs the row(s) from the table, publishes them, and clears the table.
     * 
     * Service: 
     * - Class EmergentService. It's a thread that polls the input table on the period indicated, grabs the request and
     *   transmits to ROS, WAITS for the service to complete, posts the result in the output table, and calls the result-ready program.
     */
     
