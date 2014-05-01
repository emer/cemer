/**
 * EmergentColumn.java
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

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/* The purpose of this module is to manage translations between two representations of Emergent
 * Data Tables:
 * 
 *          1. An ArrayList of arrays of strings, where each element of the ArrayList is
 *             a row and each index of the array is a column ("Lines").
 *          2. A two dimensional array of Objects, where the underlying object type matches
 *             the type of the data ("Table").
 * 
 * Both formats have a first line representing the column headers, although these are different
 * in the two formats.
 * 
 * The first representation is (close to) a native format for the Emergent Server API, while
 * the second is a natural Java format.  Thus the benefit of this module is to remove this
 * complexity from the API user, and since it is abstracted, even from the governing API wrapper
 * code.
 * 
 * The class EmergentColumn is used only in the translation process itself, which is why it
 * is private to the class.  In essence it provides a scaffolding for the translation so we
 * know what type of data we are dealing with on each side.
 * 
 * An EmergentColumn is a representation of one column of a table.  An array of such
 * colums can be in one of two modes: expanded and compact.  The individual columns differ
 * only for matrix columns.  In compact mode, a matrix is only one EmergentColumn, and the
 * Indices provide the full size of the matrix.  In expanded mode, each individual matrix cell
 * is represented separately in the array, and the Indices are the actual indices of the cell.
 * The expanded mode maps to the layout of the first (Emergent-native) representation of the
 * data, while the compact mode maps to the layout of the second (Java-native) representation.
 * 
 * We also utilize an Integer array that is a map to connect the expanded EmergentColumns in
 * an array to their corresponding compact representation.
 * 
 * The two primary public functions are Lines2Table and Table2Lines, which perform the 
 * actual translation from one representation to the other.  These functions are intended
 * to be exact inverses.  


/**
 *
 * @author David Jilk
 */
// This class is private to the Emergent API code
class EmergentColumn {
    private final static Pattern p =
            Pattern.compile("(.)([^\\[]+)(?:\\[([^\\]]*)\\](?:\\<([^\\>]*)\\>)?)?");
    private final static String DimSeparator = "[:,]"; // Careful - brackets=regex not brackets
    
    public String Name;
    public Class BaseClass;
    public Integer[] Indices;
    
    private EmergentColumn() {
        Name = null;
        BaseClass = null;
        Indices = null;
    }
    
    private EmergentColumn copy() {
        return copy(Indices);
    }
    
    private EmergentColumn copy(Integer[] NewIndices) {
        EmergentColumn NewColumn = new EmergentColumn();
        NewColumn.Name = Name;
        NewColumn.BaseClass = BaseClass;
        if (NewIndices != null) {
            NewColumn.Indices = new Integer[NewIndices.length];
            for (int i = 0; i < NewIndices.length; i++)
                NewColumn.Indices[i] = NewIndices[i];
        }
        else NewColumn.Indices = null;
        
        return NewColumn;
    }

    private Object createInstance(String Value) {
        return EmergentType.ConvertString(Value, BaseClass);
    }
    
    private Object createMatrixInstance() {
        Object Instance = null;
        
        // Returns null if it's not a matrix type
        if (Indices != null) {
            // Because the stupid newInstance function requires an int array
            int[] intIndices = new int[Indices.length];
            for (int i = 0; i < intIndices.length; i++)
                intIndices[i] = Indices[i].intValue();
            Instance = Array.newInstance(BaseClass, intIndices);
        }
        
        return Instance;
    }
    
    private void setMatrixValue(Object Matrix, String Value) {

        // Get the indicated sub-array at each dimension
        Object[] FinalArray = getFinalArray(Matrix);
        
        // For the final sub-array, set the indicated element
        FinalArray[Indices[Indices.length - 1]] = createInstance(Value);
    }
    
    private String getMatrixValue(Object Matrix) {
        
        // Get the indicated sub-array at each dimension
        Object[] FinalArray = getFinalArray(Matrix);
        
        // Return the final object after converting toString
        return EmergentType.FormatObject(FinalArray[Indices[Indices.length - 1]]);
    }
    
    private String getValue(Object Field) {
        return EmergentType.FormatObject(Field);
    }

    // This is the key to traversing a matrix of unknown dimensions
    private Object[] getFinalArray(Object Matrix) {
        
        Object CurrentObject = Matrix;
        for (int i = 0; i < Indices.length - 1; i++)
            CurrentObject = ((Object[]) CurrentObject)[Indices[i]];
        
        return (Object[]) CurrentObject;
    }        
    
    // Convert string array data to an object format
    //   First line must be the header
    public static Object[][] Lines2Table(ArrayList<String[]> Lines) {
        
        if (Lines.size() < 2)
            throw new EmergentException("Need at least a header row and one data row");
        
        // Create compact and expanded column representations from the column header strings
        EmergentColumn[] ExpandedColumns = Headers2Columns(Lines.get(0), true);
        EmergentColumn[] CompactColumns = Headers2Columns(Lines.get(0), false);
        
        // Create a column map array and check matrix fields for index consistency
        Integer[] Map = EmergentColumn.GetColumnMap(ExpandedColumns, CompactColumns);

        // Create the empty table array object
        Object[][] Table = new Object[Lines.size()][CompactColumns.length];
        
        // Format the column-name row
        Columns2Names(CompactColumns, Table[0]);

        // Finally, loop through the rows, and within that the source columns, setting
        //  data values using createInstance and setMatrixValue
        for (int row = 1; row < Lines.size(); row++) {
            
            // Create matrix instances where applicable
            for (int destcol = 0; destcol < CompactColumns.length; destcol++)
                Table[row][destcol] = CompactColumns[destcol].createMatrixInstance();
            
            // Traverse the source columns
            for (int col = 0; col < ExpandedColumns.length; col++) {
                
                // Set a matrix cell
                if (ExpandedColumns[col].Indices != null)
                    ExpandedColumns[col].setMatrixValue(Table[row][Map[col]],
                            Lines.get(row)[col]);
                
                // A simple scalar field
                else Table[row][Map[col]] =
                        ExpandedColumns[col].createInstance(Lines.get(row)[col]);
            }
        }
        
        return Table;
    }    
    
    // Parses an array of column headers into an array of structures
    private static EmergentColumn[] Headers2Columns(String[] Headers, Boolean IsExpanded) throws EmergentException{
        ArrayList<EmergentColumn> Cols = new ArrayList<EmergentColumn>();
        for (int i = 0; i < Headers.length; i++) {
            EmergentColumn NewCol = ParseColumnHeader(Headers[i], IsExpanded);
            if (NewCol != null) 
                Cols.add(NewCol);
        }
        
        return (EmergentColumn[]) Cols.toArray(new EmergentColumn[Cols.size()]);
    }
    
    // Formats the column names row of the object from the column data
    private static void Columns2Names(EmergentColumn[] Compact, Object[] Names) {
        for (int i = 0; i < Compact.length; i++)
            Names[i] = Compact[i].Name;
    }
    
    // Parses one column header into structured form; slightly different for Compact/Expanded
    private static EmergentColumn ParseColumnHeader(String Header, Boolean IsExpanded) throws EmergentException {
        Matcher m = p.matcher(Header);
        EmergentColumn c = null;
        
        // Check the header match against the required format
        if (m.matches() && (m.groupCount() == 4)) {

            // Move the parsed header into variables
            String BaseClassString = m.group(1);
            String ColName = m.group(2);
            String ExpandedDims = m.group(3);
            String CompactDims = m.group(4);

            // We need to return null for destination columns that aren't the "governing" one
            if (IsExpanded || (ExpandedDims == null) || (CompactDims != null)) {
                c = new EmergentColumn();
                c.Name = ColName;

                // Convert the type designator to a class
                if (BaseClassString.equals("$"))      // String
                    c.BaseClass = String.class;
                else if (BaseClassString.equals("%")) // Float
                    c.BaseClass = Float.class;
                else if (BaseClassString.equals("#")) // Double
                    c.BaseClass = Double.class;
                else if (BaseClassString.equals("|")) // Integer
                    c.BaseClass = Integer.class;
                else if (BaseClassString.equals("@")) // Byte
                    c.BaseClass = Byte.class;        
                else if (BaseClassString.equals("&")) // Variant
                    c.BaseClass = Object.class;         
                else c.BaseClass = Object.class;      // Unknown type - treat as Variant

                // Grab the expanded or compact Dim string, as appropriate
                //  Expanded represents true indices; Expanded represents dimensional size
                String DimString = null;
                String[] Dims = null;
                if (IsExpanded)
                    DimString = ExpandedDims;
                else DimString = CompactDims;

                if (DimString != null) {
                    Integer NumDims = 0;
                    Integer i = 0;

                    // First split the Dim string and check that the dims and size match
                    try {
                        Dims = DimString.split(DimSeparator);
                        NumDims = Integer.parseInt(Dims[0]);
                        if (!NumDims.equals(Dims.length - 1))
                            throw new EmergentException("Array dimension does not match number of indices");
                    }
                    catch (NumberFormatException e) {
                        throw new EmergentException("Invalid array dimension: " + Dims[0], e);
                    }

                    // Then move the actual indices into an array structure
                    try {
                        c.Indices = new Integer[NumDims];
                        for (i = 0; i < NumDims; i++) {
                            c.Indices[i] = Integer.parseInt(Dims[i + 1]);
                        }
                    }
                    catch (NumberFormatException e) {
                        throw new EmergentException("Invalid index: " + Dims[i + 1], e);
                    }
                }
            }
        }
        else throw new EmergentException("Column header " + Header + " is not valid.");

        return c;
    }
    
    // Convert object format to text lines.  
    public static ArrayList<String[]> Table2Lines(Object[][] TableData) {
        ArrayList<String[]> Lines = new ArrayList<String[]>();
        
        // Need to have at least a header row and a first data row
        if (TableData.length < 2)
            throw new EmergentException("Need at least a header row and one data row");
        
        // Create the compact and expanded column representations
        EmergentColumn[] CompactColumns = Object2Columns(TableData[0], TableData[1]);
        EmergentColumn[] ExpandedColumns = ExpandColumns(CompactColumns);

        // Create the map (inefficient - info is available during ExpandColumns)
        Integer[] Map = GetColumnMap(ExpandedColumns, CompactColumns);
        
        // Format and add the header line
        Lines.add(Columns2Headers(ExpandedColumns, CompactColumns, Map));
        
        // Format and add the individual rows/columns
        for (int row = 1; row < TableData.length; row++) {
            
            // Create the row as a String array
            String[] NewRow = new String[ExpandedColumns.length];
            
            // Loop through the expanded columns
            for (int col = 0; col < ExpandedColumns.length; col++) {
                
                // If it's a matrix cell, retrieve the value using getMatrixValue
                if (ExpandedColumns[col].Indices != null) {
                    NewRow[col] =
                        ExpandedColumns[col].getMatrixValue(TableData[row][Map[col]]);
                }
                
                // Otherwise just format it directly
                else NewRow[col] = ExpandedColumns[col].getValue(TableData[row][Map[col]]);
            }
            
            // Add the row to the output
            Lines.add(NewRow);
        }

        return Lines;
    }

    // Convert object format to (compact) column array
    private static EmergentColumn[] Object2Columns(Object[] ColumnNames, Object[] RowData) {
        
        // Create the output array 
        EmergentColumn[] ColumnArray = new EmergentColumn[ColumnNames.length];
        for (int n = 0; n < ColumnArray.length; n++)
            ColumnArray[n] = new EmergentColumn();

        // Process each column in the data
        for (int i = 0; i < ColumnNames.length; i++) {

            // Get the data object and store its name and class
            Object ColData = RowData[i];
            ColumnArray[i].Name = (String) ColumnNames[i];
            ColumnArray[i].BaseClass = EmergentType.GetBaseClass(ColData);
            
            // For array objects we have to determine the dimensions and indices
            if (ColData.getClass().isArray()) {
                Integer NumDims = EmergentType.ArrayDimensions(ColData);
                ColumnArray[i].Indices = new Integer[NumDims];
                Object CurrentObject = ColData;
                for (int d = 0; d < NumDims; d++) {
                    ColumnArray[i].Indices[d] = ((Object[]) CurrentObject).length;
                    CurrentObject = ((Object[]) CurrentObject)[0];
                }
            }
            
            // Non-array objects have null Indices
            else ColumnArray[i].Indices = null;
        }
        
        return ColumnArray;
    }
    
    // Expand a compact column array to an expanded column array
    private static EmergentColumn[] ExpandColumns(EmergentColumn[] Compact) {
        ArrayList<EmergentColumn> Expanded = new ArrayList<EmergentColumn>();
        for (int i = 0; i < Compact.length; i++) {
            if (Compact[i].Indices != null) {
                Integer[] CurrentIndices = new Integer[Compact[i].Indices.length];
                RecursiveMatrixExpansion(Compact[i],
                        CurrentIndices, 0, Expanded);
            }
            else Expanded.add(Compact[i].copy());
        }
        
        return (EmergentColumn []) Expanded.toArray(new EmergentColumn[Expanded.size()]);
    }
    
    // Deals with the expansion of a matrix column; recursive because we don't know dimensions!
    private static void RecursiveMatrixExpansion(EmergentColumn Compact,
            Integer[] CurrentIndices, Integer Dimension, ArrayList<EmergentColumn> Expanded) {
        
            // Note: The Emergent Server seems like it might process the array in opposite
            //   order.  We'll see if it can handle this order instead (it should, since
            //   the indices are specified on each column)
        
        // Because Emergent requires most-significant first, we reverse the dimension
        // If this gets changed/fixed, just make WorkingDimension=Dimension.
        Integer WorkingDimension = Compact.Indices.length - Dimension - 1;
        
        // If not at the bottom, recurse for each value of the current index
        if (Dimension < Compact.Indices.length) {
            for (CurrentIndices[WorkingDimension] = 0;
                    CurrentIndices[WorkingDimension] < Compact.Indices[WorkingDimension];
                    CurrentIndices[WorkingDimension]++)
                RecursiveMatrixExpansion(Compact, CurrentIndices, Dimension + 1, Expanded);
        }
        
        // If at the bottom, add the column
        else Expanded.add(Compact.copy(CurrentIndices));
    }
    
    // Translates an expanded column structure into the formatted column headers
    private static String[] Columns2Headers(EmergentColumn[] ExpandedColumns,
            EmergentColumn[] CompactColumns, Integer[] Map) {

        // Create the header structure, plus a hashset to determine "governing" status, 
        HashSet<String> h = new HashSet<String>();
        ArrayList<String> Headers = new ArrayList<String>();
        
        // Kludge for Emergent bug
        //Headers.add("_H:");
        
        // Loop through the expanded columns
        for (int i = 0; i < ExpandedColumns.length; i++) {
            if (ExpandedColumns[i].Indices != null) {
                Boolean Governing = h.add(ExpandedColumns[i].Name);
                Headers.add(ExpandedColumns[i].FormatMatrix(Governing, CompactColumns[Map[i]]));
            }
            else Headers.add(ExpandedColumns[i].Format());
        }
        
        return (String[]) Headers.toArray(new String[Headers.size()]);
    }

    // Format the full header for a matrix column
    private String FormatMatrix(Boolean Governing, EmergentColumn Compact) {
        String Result = Format();
        Result += "[" + FormatIndex() + "]";
        if (Governing)
            Result += "<" + Compact.FormatIndex() + ">";
        
        return Result;
    }
    
    // Format the indices into the form "d:a,b,c"
    private String FormatIndex() {
        String Result = Indices.length + ":";
        for (int i = 0; i < Indices.length; i++) {
            if (i > 0)
                Result += ",";
            Result += Indices[i];
        }
        return Result;
    }
    
    // Format the header label for a base field, or for the beginning of a matrix format
    private String Format() {
        
        String Prefix = "&";
        if (BaseClass.equals(String.class))
            Prefix = "$";
        else if (BaseClass.equals(Integer.class))
            Prefix = "|";
        else if (BaseClass.equals(Byte.class))
            Prefix = "@";
        else if (BaseClass.equals(Float.class))
            Prefix = "%";
        else if (BaseClass.equals(Double.class))
            Prefix = "#";
        else if (BaseClass.equals(Object.class))
            Prefix = "&";
        
        return Prefix + Name;
    }
    
    /*
    public String toString() {
        String Result = Name + ": " + BaseClass.getName();
        if (Indices != null) {
            Result += "(";
            for (int i = 0; i < Indices.length; i++) {
                if (i > 0)
                    Result += ",";
                Result += Indices[i];
            }
            Result += ")";
        }
        return Result;
    }
     */

    // Create a map from an expanded column array to a compact array; used by both directions
    private static Integer[] GetColumnMap(EmergentColumn[] e, EmergentColumn[] c) {
        
        // Load a hash with the destination columns
        HashMap<String,Integer> h = new HashMap<String,Integer>();
        for (int i = 0; i < c.length; i++) 
            h.put(c[i].Name, i);
        
        // Create a map for the source columns to the destination; and verify indices/dimensions
        Integer[] Map = new Integer[e.length];
        for (int i = 0; i < e.length; i++) {
            
            // Map based on the hash table
            Map[i] = h.get(e[i].Name);
            
            // If it's a matrix and dimensionality matches
            if (e[i].Indices != null) {
                if (e[i].Indices.length == c[Map[i]].Indices.length) {
                
                    // Loop through the indices and check against size
                    for (int j = 0; j < e[i].Indices.length; j++) {
                        if (e[i].Indices[j] >= c[Map[i]].Indices[j]) {
                            throw new EmergentException("Array index out of bounds on column \"" +
                                    e[i].Name + "\"");
                        }
                    }
                }
                else throw new EmergentException("Dimensions of matrix column \"" +
                    c[Map[i]].Name + "\" do not match data object \"" + e[i].Name + "\"");
            }
        }
        
        return Map;
    }
}
