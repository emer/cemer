/**
 * StdInputDataRow.java
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

package Tables;

import com.ecortex.emergent.api.EmergentDataTableRow;

/**
 *
 * @author David J. Jilk
 */
public class StdInputDataRow extends EmergentDataTableRow {
    public String Name;
    public Float[][] Input;
    public Float[][] Output;

    public StdInputDataRow() {
        Input = new Float[5][5];
        Output = new Float[5][5];
    }
}
    