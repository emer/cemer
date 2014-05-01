/**
 * EmergentConnection.java
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
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.PrintStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;

import java.io.OutputStream;


/**
 * A wrapper class for the Emergent Server Protocol.  Each instance represents a
 * single persistent connection to the Emergent server on a TCP/IP socket.  Use of
 * this class allows the developer to use the server protocol with native Java types
 * and avoids the need to learn and manage the protocol syntax.
 * <p>
 * Almost all of the methods in this class throw EmergentException.  Assuming no
 * programming errors, these exceptions will generally be due to communication
 * failures.  
 *
 * @version 1.0
 * @see <a href="http://grey.colorado.edu/emergent/index.php/Server_Protocol">
 * Emergent Server Protocol</a>
 */
public class EmergentConnection {
    static final private Integer Timeout = 5000;
    static final private Integer DefaultPort = 5360;
    static final private String Whitespace = "\\s";
    static final private String Space = " ";
    static final private String Equals = "=";
    static final private String Tab = "\t";
    
    /** No program is running */
    public final int GLOBAL_RUNSTATUS_NOTRUNNING = 0;
    
    /** Some program is running (has status INIT, RUN, or STOP) */
    public final int GLOBAL_RUNSTATUS_RUNNING = 2;
    
    /** The program is not running */
    public final int PROGRAM_RUNSTATUS_DONE = 0;
    
    /** The program is running its init code */
    public final int PROGRAM_RUNSTATUS_INIT = 1;

    /** The program is running */
    public final int PROGRAM_RUNSTATUS_RUN = 2;
    
    /** The program is stopped (running but suspended) */
    public final int PROGRAM_RUNSTATUS_STOP = 3;
    
    /** The program has not been initialized */
    public final int RUNSTATUS_NOT_INIT = 4;
    
    private String LocalHostName;           // Name of the host or IP address for the connection
    private Integer LocalPort;              // Port for the connection
    private Socket s = null;                // Socket used after connection
    private BufferedReader ReceiveStream;   // The output stream from Emergent
    private PrintStream SendStream;         // The input stream to Emergent
    private String Version = null;          // The version number of the Emergent Server
    private Boolean Debug = false;          // Whether or not to print debugging information
    
    /** Creates a new EmergentConnection instance with the specified host/IP and port. 
     * The constructor does <b>not</b> attempt to establish the actual connection.
     * 
     * @param HostName Name of host or IP address, e.g., "grey.colorado.edu"
     * @param Port TCP/IP port on which to connect, if not using the default port
     */
    public EmergentConnection(String HostName, Integer Port) {
        LocalHostName = HostName;
        LocalPort = Port;
    }
    
    /** Creates a new EmergentConnection instance with the specified host/IP
     * on the default port (5360). The constructor does <b>not</b>
     * attempt to establish the actual connection.
     * 
     * @param HostName Name of host or IP address, e.g., "grey.colorado.edu"
     */
    public EmergentConnection(String HostName) {
        LocalHostName = HostName;
        LocalPort = DefaultPort;
    }
    
    /** Establishes a live, persistent connection to the Emergent Server. It is not strictly
     * necessary to call this function, since it is checked in all the API functions.
     * 
     * @throws com.ecortex.emergent.api.EmergentException if the connection attempt fails
     */
    public void Initialize() throws EmergentException {
        if (s == null) {
            s = new Socket();
            try {
                s.connect((SocketAddress) new InetSocketAddress(LocalHostName, LocalPort), Timeout);
                ReceiveStream = new BufferedReader(new InputStreamReader(s.getInputStream()));
                SendStream = new PrintStream(s.getOutputStream(), true);
                SetVersion(GetFirstResult());
            }
            catch (IOException e) {
                throw new EmergentException("I/O error initializing connection", e);
            }
        }
    }
    
    /** Parses the version number */
    private void SetVersion(String banner) {
        String banneritems [] = banner.split(Whitespace);
        if (banneritems.length >= 3)
            Version = banneritems[2];
    }

    /** Shuts down the connection to the Emergent Server. Since the Server allows only one
     * active connection, this method should be called when interface use is complete.
     * 
     * @throws com.ecortex.emergent.api.EmergentException if the close attempt fails.
     */
    public void Shutdown() throws EmergentException {
        if (s != null) {
            try {
                ReceiveStream.close();
                SendStream.close();
                s.close();
            }
            catch (IOException e) {
                throw new EmergentException("I/O error closing connection", e);
            }
        }
    }
    
    /** Returns the current version of Emergent and the Server Interface.
     * 
     * @return the Emergent Server version number, or null if a successful Initialize
     * has not been performed
     */
    public String GetVersion() {
        return(Version);
    }
    
    /** Set the debug state of the connection.  Defaults to false; if set to true,
     * sends information (including all transmissions on the socket) to stdout.
     * 
     * @param DebugState Whether or not to provide debugging information
     */
    public void SetDebug(Boolean DebugState) {
        Debug = DebugState;
    }
    
    /** Sets an Emergent program variable to a specified value
     * 
     * @param ProgramName Name of the Emergent program in which the variable lives. Case sensitive.
     * @param Variable Name of the program variable to be changed. Case sensitive.
     * @param Value New value of the specified variable.  The underlying class of the value
     * must be the correct type for the variable.
     * @throws com.ecortex.emergent.api.EmergentException If there was a communication or data type error
     */
    public synchronized void SetVar(String ProgramName, String Variable, Object Value) throws EmergentException {
        Execute("SetVar" + Space + ProgramName + Space +
                Variable + Equals + EmergentType.FormatObject(Value));
    }
     
    /** Gets an Emergent program variable, based on a specified type class.  The preferred usage
     * is to set it to a Java variable and use that variable's class, for example:
     * 
     * Integer MyJavaVar = GetVar("MyProgram", "MyVar", MyJavaVar.getClass());
     * 
     * @param ProgramName Name of the Emergent program in which the variable lives. Case sensitive.
     * @param Variable Name of the program variable to be retrieved. Case sensitive.
     * @param Type The class of the object to be returned
     * @return The value of the variable, extracted into the class specified
     * @throws com.ecortex.emergent.api.EmergentException If there was a communication or data type error
     */
    public synchronized Object GetVar(String ProgramName, String Variable, Class<?> Type) throws EmergentException {
        String Value = Execute("GetVar" + Space + ProgramName + Space + Variable);
        return EmergentType.ConvertString(Value, Type);
    }

    /** Returns the global run state of the Emergent server
     * 
     * @return A GLOBAL_RUNSTATUS value
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication error
     */
    public synchronized Integer GetRunState() throws EmergentException {
        return (Integer) EmergentType.ConvertString(Execute("GetRunState"), Integer.class);
    }
    
    /** Returns the run state of the indicated program 
     * 
     * @param ProgramName The name of the program.  Case sensitive.
     * @return A PROGRAM_RUNSTATUS value
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication error
     */
    public synchronized Integer GetRunState(String ProgramName) throws EmergentException {
        return (Integer) EmergentType.ConvertString(Execute("GetRunState"
                + Space + ProgramName), Integer.class);
    }
   
    /** Instruct the Emergent Server to run a program synchronously, i.e., it does
     * not return the result until the program has completed (note: test this with
     * timeout)
     * 
     * @param ProgramName Name of the program to run. Case sensitive.
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication error
     */
    public synchronized void RunProgram(String ProgramName) throws EmergentException {
        Execute("RunProgram" + Space + ProgramName);
    }
    
    /** Instruct the Emergent Server to run a program asynchronously, i.e., it 
     * returns a result immediately.  The calling program must then poll for program
     * status to determine when it has completed.
     * 
     * @param ProgramName Name of the program to run. Case sensitive.
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication error
     */
    public synchronized void RunProgramAsync(String ProgramName) throws EmergentException {
        Execute("RunProgramAsync" + Space + ProgramName);
    }
    
    /* NOT YET IMPLEMENTED IN PROTOCOL
    public void StepProgram(String ProgramName) throws EmergentException {
        Execute("StepProgram" + Space + ProgramName);
    }
     */
    
    /* NOT YET IMPLEMENTED IN PROTOCOL
    public void StopProgram(String ProgramName) throws EmergentException {
        Execute("StopProgram" + Space + ProgramName);
    }
     */
    
    /** Get a data table from the Emergent Server. 
     * 
     * @param TableName Name of the Data Table. Case sensitive.
     * @return A two-dimensional array of Objects.  The first dimension is the rows,
     * the second is the columns.  The actual class of the elements matches the columns
     * returned from the table. A matrix is returned as a multi-dimensional array of the 
     * applicable type. The zero (0) row contains the column names.
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication or parsing error
     * @see EmergentDataTable
     */
    public synchronized Object[][] GetData(String TableName) throws EmergentException {
        
        // Run the command and get the response status
        Integer LineCount = GetLineCount(Execute("GetData " +
                TableName + " header" + Equals + "1"));
        
        // Retrieve the data table in array-of-string format
        ArrayList<String[]> Lines = GetLines(LineCount);

        // Convert the text of the table to an Object table format
        return EmergentColumn.Lines2Table(Lines);
    }
    
    // Get the rows of the table from Emergent and convert into arrays of strings
    private ArrayList<String[]> GetLines(Integer LineCount) {
        ArrayList<String[]> Lines = new ArrayList<String[]>();
        for (Integer i = 0; i < LineCount; i++) {
            Lines.add(ReadLine().split(Tab));
        }
        return Lines;
    }
        
    /** Append to a data table on the Emergent Server. 
     * 
     * @param TableName Name of the Data Table. Case sensitive.
     * @param TableData A two-dimensional array of Objects.  The first dimension is the rows,
     * the second is the columns.  The actual class of the elements must match the columns
     * in the table, and a matrix must be a multi-dimensional array of the 
     * applicable type. The zero (0) row must contain the column names.
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication or parsing error
     * @see EmergentDataTable
     */
    public synchronized void AppendData(String TableName, Object[][] TableData) throws EmergentException {
        
        // Convert the object format data to lines of text
        //    First line is the header
        ArrayList<String[]> Lines = EmergentColumn.Table2Lines(TableData);
        
        // Initiate the command (don't check response; we have to send the data lines first)
        ExecuteOnly("AppendData" + Space + TableName + Space +
                "rows" + Equals + (Lines.size() - 1) + Space + "header" + Equals + "1");
        
        // Send the lines to Emergent
        PutLines(Lines);
        
        // Now check the response
        GetFirstResult();
    }
    
    // Add delimiters to the arrays of strings and transmit each line to Emergent
    private void PutLines(ArrayList<String[]> Lines) {
        for (int i = 0; i < Lines.size(); i++)
            WriteLine(EmergentType.Delimit(Lines.get(i), Tab));
    }
    
    /** Remove rows from a data table on the Emergent Server. 
     * 
     * @param TableName Name of the Data Table. Case sensitive.
     * @param StartRow Starting row from which to delete. If it exceeds the last row
     * of the table, no rows are deleted.  Rows begin at zero (0).
     * @param NumRows Number of rows to delete, starting from StartRow.  It is not an error
     * if the number of available rows is exceeded.
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication or parsing error
     * @see EmergentDataTable
     */
    public synchronized void RemoveData(String TableName, Integer StartRow, Integer NumRows) throws EmergentException {
        Execute("RemoveData" + Space + TableName + Space +
                "row_from" + Equals + StartRow + Space +
                "rows" + Equals + NumRows);
    }
    
    /** Remove rows from a starting row to the end of a data table on the Emergent Server. 
     * 
     * @param TableName Name of the Data Table. Case sensitive.
     * @param StartRow Starting row from which to delete. If it exceeds the last row
     * of the table, no rows are deleted.  Rows begin at zero (0).
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication or parsing error
     * @see EmergentDataTable
     */
    public synchronized void RemoveData(String TableName, Integer StartRow) throws EmergentException {
        Execute("RemoveData" + Space + TableName + Space +
                "row_from" + Equals + StartRow);
    }
    
    /** Remove all rows from a data table on the Emergent Server. 
     * 
     * @param TableName Name of the Data Table. Case sensitive.
     * of the table, no rows are deleted.  Rows begin at zero (0).
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication or parsing error
     * @see EmergentDataTable
     */
    public synchronized void RemoveData(String TableName) throws EmergentException {
        Execute("RemoveData" + Space + TableName);
    }
    
    /** Set the value of a single data table value on the Emergent Server
     * 
     * @param TableName Name of the Data Table. Case sensitive.
     * @param Row Row of the value
     * @param Column Column of the value
     * @param Value Value to be set.  The type of this Object must match the type of the data
     * table location.
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication or parsing error
     */
    public synchronized void SetDataCell(String TableName, Integer Row, Integer Column, Object Value)
            throws EmergentException {
        Execute("SetDataCell" + Space + TableName + Space +
                Column + Space + Row + Space + EmergentType.FormatObject(Value));
    }
    
    /** Set the value of a single data table matrix cell value on the Emergent Server
     * 
     * @param TableName Name of the Data Table. Case sensitive.
     * @param Row Row of the value
     * @param Column Column of the value
     * @param Cell Index of the matrix value.  Uses a flat index, calculated in little-endian
     * order from the size of the matrix dimensions
     * @param Value Value to be set.  The type of this Object must match the type of the data
     * table cell.
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication or parsing error
     */
    public synchronized void SetDataMatrixCell(String TableName,
            Integer Row, Integer Column, Integer Cell, Object Value)
            throws EmergentException {
        Execute("SetDataMatrixCell" + Space + TableName + Space +
                Column + Space + Row + Space + Cell + Space + EmergentType.FormatObject(Value));
    }
    
    /** Get the value of a single data table value from the Emergent Server. The preferred usage
     * is to set it to a Java variable and use that variable's class, for example:
     * 
     * Integer MyJavaVar = GetDataCell("MyTable", Row, Column, MyJavaVar.getClass());     
     * 
     * @param TableName Name of the Data Table. Case sensitive.
     * @param Row Row of the value
     * @param Column Column of the value
     * @param Type The class of the object to be returned
     * @return An object of the specified type representing the extracted value
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication or parsing error
     */
    public synchronized Object GetDataCell(String TableName, Integer Row, Integer Column, Class<?> Type)
             throws EmergentException {
        String Value = Execute("GetDataCell" + Space + TableName + Space + 
                Column + Space + Row);
        return EmergentType.ConvertString(Value, Type);
    }
    
    /** Get the value of a single data table matrix cell value from the Emergent Server. The preferred usage
     * is to set it to a Java variable and use that variable's class, for example:
     * 
     * Integer MyJavaVar = GetDataMatrixCell("MyTable",
     *              Row, Column, Index, MyJavaVar.getClass());     
     * 
     * 
     * @param TableName Name of the Data Table. Case sensitive.
     * @param Row Row of the value
     * @param Column Column of the value
     * @param Cell Index of the matrix value.  Uses a flat index, calculated in little-endian
     * order from the size of the matrix dimensions
     * @return An object of the specified type representing the extracted value
     * @throws com.ecortex.emergent.api.EmergentException If there is a communication or parsing error
     */
    public synchronized Object GetDataMatrixCell(String TableName,
            Integer Row, Integer Column, Integer Cell, Class<?> Type)
             throws EmergentException {
        String Value = Execute("GetDataMatrixCell" + Space + TableName + Space + 
                Column + Space + Row + Space + Cell);
        return EmergentType.ConvertString(Value, Type);
    }
    
    // Transmit a command and process the first line of the response
    private String Execute(String command) throws EmergentException {
        ExecuteOnly(command);
        return GetFirstResult();
    }
    
    // Transmit a command without looking at the response
    private void ExecuteOnly(String command) throws EmergentException {
        Initialize();  // Does nothing if already initalized
        WriteLine(command);
    }

    // From the first line of result data of a multiline response, get the number of lines
    private Integer GetLineCount(String LineData) throws EmergentException {
        String LineTokens[] = LineData.split(Equals);
        Integer Lines = 0;
        if ((LineTokens.length == 2) && 
                (LineTokens[0].toLowerCase().equals("line") ||  // Bug in initial version
                 LineTokens[0].toLowerCase().equals("lines"))) {
            try {
                Lines = (Integer) EmergentType.ConvertString(LineTokens[1], Integer.class);
            }
            catch (EmergentException e) {
                throw new EmergentException("Error in line count", e);
            }
        }
        else throw new EmergentException("Unexpected line data: " + LineData);
        
        return Lines;
    }
    
    // Get the first line of command response data and check status
    private String GetFirstResult() throws EmergentException {
        String Result = "";
        String Output = ReadLine();
        String[] ResultLine = Output.split(Whitespace, 2);
        String ResultCode = ResultLine[0];
        String ResultValue = "";
        if (ResultLine.length > 1)
            ResultValue = ResultLine[1];
        if (ResultCode.toLowerCase().equals("error"))
                throw new EmergentException(ResultValue);
        else if (ResultCode.toLowerCase().equals("ok"))
            Result = ResultValue;
        else Result = Output;
        return Result;
    }
    
    // Reads a line from the socket, and trims the newline
    private String ReadLine() throws EmergentException {
        String Output = null;
        try {
            Output = ReceiveStream.readLine().trim();
        }
        catch (IOException e) {
            throw new EmergentException("Error receiving data", e);
        }
        if (Debug) System.out.println("< " + Output);
        return Output;
    }
    
    // Writes a line to the socket, checking for errors
    private void WriteLine(String Line) throws EmergentException {
        if (Debug) System.out.println("> " + Line);
        SendStream.println(Line);
        if (SendStream.checkError())
            throw new EmergentException("Error sending data");
    }
    
}
