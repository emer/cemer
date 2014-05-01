/**
 * EmergentException.java
 * Copyright (c) 2009-2014 eCortex, Inc.
 * 
 * This file is part of the Emergent Server Java API.
 *
 * The Emergent Server Java API is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The Emergent Server Java API is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Emergent Server Java API.  If not, see http://www.gnu.org/licenses/.
 *
 * Emergent Server Java API
 * Version 0.9
 * Compatible with Emergent 6.2+
 * 
 */

package com.ecortex.emergent.api;

/** A general exception class for the Emergent Server API.  Because most errors,
 * particularly communication failures, 
 * are not really recoverable, EmergentException derives from the @link RuntimeException
 * class and does not need to be caught or specified (but can be if desired, for example, 
 * when initializing a connection).
 */
public class EmergentException extends RuntimeException {
 
    /** Create a new exception with just a message
     * 
     * @param message The summary message for the error
     */
    public EmergentException (String message) {
        super(message);
    }
    
    /** Create a new exception with a message and a cause.
     * 
     * @param message The summary message for the error
     * @param cause The underlying exception that caused the error, for example, IOException
     */
    public EmergentException (String message, Throwable cause) {
        super(message, cause);
    }
}
