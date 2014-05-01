/**
 * TestEmergentConnection.java
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

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

import Tables.StdInputDataRow;

/**
 *
 * @author david_000
 */
public class TestEmergentConnection {
    
    public TestEmergentConnection() {
    }
    
    @BeforeClass
    public static void setUpClass() {
    }
    
    @AfterClass
    public static void tearDownClass() {
    }
    
    @Before
    public void setUp() {
    }
    
    @After
    public void tearDown() {
    }
    
    @Test
    public void TestEmergentConnectionLocal() {
        EmergentConnection e = new EmergentConnection("127.0.0.1");
        e.Initialize();
        e.SetDebug(true);
        EmergentDataTable t = new EmergentDataTable("StdInputData", new StdInputDataRow(), e);
        //StdInputDataRow r = new StdInputDataRow();
        try {
            t.Read();
        }
        catch (EmergentException ex) {
            ex.printStackTrace();
        }
        for (EmergentDataTableRow r : t) {
            StdInputDataRow r1 = (StdInputDataRow) r;
            System.out.println(r1.Name + " " + r1.Input[0][0] + " " + r1.Output[1][1]);
        }
        
        t.clear();
        StdInputDataRow r = new StdInputDataRow();
        r.Name = "Test";
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                r.Input[i][j] = (float) i;
                r.Output[i][j] = (float) j;
            }
        }
        t.add(r);
        t.Write();
        
        e.Shutdown();
    }
    
    
    
}