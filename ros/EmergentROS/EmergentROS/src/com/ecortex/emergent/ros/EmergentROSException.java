/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros;

public class EmergentROSException extends RuntimeException {
 
    /** Create a new exception with just a message
     * 
     * @param message The summary message for the error
     */
    public EmergentROSException (String message) {
        super(message);
    }
    
    /** Create a new exception with a message and a cause.
     * 
     * @param message The summary message for the error
     * @param cause The underlying exception that caused the error, for example, IOException
     */
    public EmergentROSException (String message, Throwable cause) {
        super(message, cause);
    }
}

