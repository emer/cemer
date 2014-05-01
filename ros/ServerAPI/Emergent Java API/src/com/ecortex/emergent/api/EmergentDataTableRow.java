/**
 * EmergentDataTableRow.java
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

import java.lang.reflect.Field;

/**
 *
 * @author David Jilk
 */
public abstract class EmergentDataTableRow {

    // Provides initialization for non-array fields in the row structure
    //  Strings are initialized to an empty string; numeric values are initialized to zero
    public EmergentDataTableRow() {
       Field[] Fields = getClass().getFields();
       for (int i = 0; i < Fields.length; i++) {
           Class c = Fields[i].getType();
           if (!c.isArray()) {
               try {
                   Object Value = "";
                   if (c.equals(Byte.class))
                       Value = new Byte((byte) 0);
                   else if (c.equals(Integer.class))
                       Value = new Integer((int) 0);
                   else if (c.equals(Float.class))
                       Value = new Float((float) 0);
                   else if (c.equals(Double.class))
                       Value = new Double((double) 0);
                   else if (c.equals(String.class))
                       Value = "";
                   else throw new RuntimeException("EmergentDataTableRow: Unsupported type " + c.getName());
                   Fields[i].set(this, Value);
               }
               catch (IllegalAccessException e) {
                   throw new EmergentException("Illegal access", e);
               }
           }
       }
    }
}
