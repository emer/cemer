/**
 * EmergentDataTable.java
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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.lang.reflect.Field;
import java.lang.reflect.AccessibleObject;

/**
 * The most convenient format for a Data Table in Java is an ArrayList of a row-definition
 *    class that maps directly to the column fields.  EmergentDataTable supports that
 *    functionality by accepting such a row-definition class as a factory.  Viewed another way,
 *    this is a further wrapper on top of the generic Object array format used by the 
 *    EmergentConnection methods.
 */
public class EmergentDataTable extends ArrayList<EmergentDataTableRow> {
    private String TableName = null;
    private EmergentDataTableRow RowFactory = null;
    private EmergentConnection TableConnection = null;
    
    // Create a new Data Table with the given name and an object that creates new rows
    //    We have to pass in the name because multiple tables could have the same structure
    public EmergentDataTable(String Name,
            EmergentDataTableRow Factory, EmergentConnection Connection) {
        TableName = Name;
        RowFactory = Factory;
        TableConnection = Connection;
    }
    
    /** Reads the data table from Emergent and appends to this table.  You must call clear()
      * first if you already have records in the local table and want it to match the
      * Emergent side.
      * @throws EmergentException If there is a communication or formatting error
      */
    public void Read() {
        fromGeneric(TableConnection.GetData(TableName));
    }
    
    /** Append the entire table to the existing Data Table in Emergent.
      * @throws EmergentException If there is a communication or formatting error
      */
    public void Write() throws EmergentException {
        if (size() > 0)  // DJJ 2014/01/27
            TableConnection.AppendData(TableName, toGeneric());
    }
    
    /** Overwrite the data table in Emergent with this table.
      * @throws EmergentException If there is a communication or formatting error
      */
    public void Overwrite() {
        TableConnection.RemoveData(TableName);
        Write();
    }
    
    // Translate the EmergentDataTable structure to the generic Object array format used
    //   by the EmergentConnection methods.  It uses a field map generated from looking
    //   into the actual object structure of the row's base class
    private Object[][] toGeneric() {
        HashMap<Field,Integer> FieldMap = MakeFieldMap();
        Object[][] Generic = new Object[size() + 1][FieldMap.size()];

        // The header
        Iterator<Field> iField1 = FieldMap.keySet().iterator();
        while (iField1.hasNext()) {
            Field CurrentField = iField1.next();
            Integer CurrentColumn = FieldMap.get(CurrentField);
            Generic[0][CurrentColumn] = CurrentField.getName();
        }
        
        // The rows
        for (int Row = 1; Row < Generic.length; Row++) {
            EmergentDataTableRow CurrentRow = get(Row - 1);
            Iterator<Field> iField2 = FieldMap.keySet().iterator();
            while (iField2.hasNext()) {
                Field CurrentField = iField2.next();
                Integer Column = FieldMap.get(CurrentField);
                try {
                    Generic[Row][Column] = CurrentField.get(CurrentRow);
                }
                catch (IllegalAccessException e) {
                    throw new EmergentException("Invalid access", e);
                }
            }
        }
        
        return Generic;
    }
    
    // Translate from generic Object array format to the EmergentDataTable structure
    private void fromGeneric(Object[][] Generic) {
        HashMap<Field,Integer> FieldMap = MakeFieldMap(Generic[0]);
        EmergentDataTableRow NewRow = null;
        for (int Row = 1; Row < Generic.length; Row++) {
            try {
                NewRow = RowFactory.getClass().newInstance();
            }
            catch (Exception e) {
                throw new EmergentException("Error creating row", e);
            }
            Iterator<Field> iField = FieldMap.keySet().iterator();
            while (iField.hasNext()) {
                Field CurrentField = iField.next();
                Integer Column = FieldMap.get(CurrentField);
                try {
                    CurrentField.set(NewRow, Generic[Row][Column]);
                }
                catch (IllegalAccessException e) {
                    throw new EmergentException("Invalid access", e);
                }
            }
            add(NewRow);
        }
    }
    
    // When creating a field map from an existing table
    private HashMap<Field,Integer> MakeFieldMap(Object[] Headers) {
        
        // First create a map of the column names to column numbers
        HashMap<String,Integer> ColumnMap = new HashMap<String,Integer>();
        for (int i = 0; i < Headers.length; i++)
            ColumnMap.put((String) Headers[i], i);
        
        return MakeFieldMap(ColumnMap);
    }
    
    // When creating a field map for a new table
    private HashMap<Field,Integer> MakeFieldMap() {
        return MakeFieldMap((HashMap<String,Integer>) null);
    }
    
    // Create the field map from a column map (or null)
    private HashMap<Field,Integer> MakeFieldMap(HashMap<String,Integer> ColumnMap) {
        HashMap<Field,Integer> FieldMap = new HashMap<Field,Integer>();
        Field[] Fields = RowFactory.getClass().getDeclaredFields();
        AccessibleObject.setAccessible(Fields, true);
        for (int i = 0; i < Fields.length; i++) {
            Integer MapColumn = i;
            if (ColumnMap != null)
                MapColumn = ColumnMap.get(Fields[i].getName());
            FieldMap.put(Fields[i], MapColumn);
        }
        return FieldMap;
    }
}

