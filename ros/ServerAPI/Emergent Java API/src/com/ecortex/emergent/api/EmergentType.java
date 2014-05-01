/**
 * EmergentType.java
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

/**
 *
 * @author David Jilk
 */
// This class is private to the Emergent API code
class EmergentType {
    
    static final private String Quote = "\"";
    
    // Converts a string from Emergent into the native form of the indicated class
    //    Centralizing this operation avoids exception handling everywhere,
    //    and for clarity we want to re-throw the NumberFormatException.
    public static Object ConvertString(String Value, Class<?> ConvertClass) {
        Object Result = null;
        try {
            if (ConvertClass.equals(String.class))
                Result = RemoveQuotes(Value);
            else if (ConvertClass.equals(Integer.class))
                Result = Integer.parseInt(Value);
            else if (ConvertClass.equals(Float.class))
                Result = Float.parseFloat(Value);
            else if (ConvertClass.equals(Double.class))
                Result = Double.parseDouble(Value);
            else if (ConvertClass.equals(Byte.class))
                Result = Byte.parseByte(Value);
            else Result = Value;
        }
        catch (NumberFormatException e) {
            throw new EmergentException("Returned data (" + Value + ") is not of type " +
                    ConvertClass.toString(), e);
        }
        return Result;
    }
    
    // Format an Object based on its class
    public static String FormatObject(Object Value) {
        String Result = " ";
        if (Value != null) {
            Result = Value.toString();
            if (Value.getClass().equals(String.class))
                Result = AddQuotes(Result);
        }
        return Result;
    }
        
    
    // Get the number of dimensions of an array (or zero if not an array)
    public static Integer ArrayDimensions(Object a) {
        
        // Get the sub component type until a non-array is reached; count the steps
        Integer Dimensions = 0;
        Class c = a.getClass();
        while (c.isArray()) {
            Dimensions++;
            c = c.getComponentType();
        }
        return Dimensions;
    }
    
    // Determine the base class of an object (whether or not it is an array)
    public static Class GetBaseClass(Object a) {
        
        // Get the sub component type until a non-array is reached; return that class
        Class c = a.getClass();
        while (c.isArray())
            c = c.getComponentType();
        return c;
    }
    
    // Concatenate an array of strings, delimited as specified
    public static String Delimit(String[] Elements, String Delimiter) {
        String Result = "";
        for (int i = 0; i < Elements.length; i++) {
            if (i > 0)
                Result += Delimiter;
            Result += Elements[i];
        }
        return Result;
    }
    
    // Remove the surrounding quotes from an Emergent string (only if they are present)
    public static String RemoveQuotes(String Raw) {
        String Out = Raw;
        if (Out.startsWith(Quote))
            Out = Out.substring(1);
        if (Out.endsWith(Quote))
            Out = Out.substring(0, Out.length() - 1);
        return Out;
    }
    
    // Surround a native string with quotes for sending to Emergent
    public static String AddQuotes(String Raw) {
        return Quote + Raw + Quote;
    }
}
