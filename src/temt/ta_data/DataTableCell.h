// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef DataTableCell_h
#define DataTableCell_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:
class DataCol; //

taTypeDef_Of(DataTableCell);

class TA_API DataTableCell : public taOBase {
  // ##INLINE ##NO_TOKENS this class serves to represent a single cell within a data column because data table cells are not themselves objects. These DataTableCells (DTCs) are used to link a cell to a control panel item   which normally points to an object instance member. The DataTable class keeps a list of these DTCs which are created as needed. If dtc_is_column_type is true then then DTC is not linked to a specific column and row but to a column with the row being able to change dynamically based on the row chosen in the 'row_lookup_col'.
INHERITED(taOBase)
public:
  bool                dtc_is_column_type;  // #READ_ONLY #SHOW - false if the cell is tied to a specific row and true if the row is set using lookup where lookup is the matching of 'row_lookup_value' in 'row_lookup_col'.
  DataCol*            value_column;     // the column with editable data table cell
  int                 view_row;         // the row as the user sees it
  int                 index_row;        // #READ_ONLY the row in the underlying matrix that holds all rows, visible and hidden
  String              value;            // #NO_SAVE this is the current value of the data table cell, reflected in the control panel item, and can be changed by editing the control panel value and applying. If the DataTableCell is of the 'column type' the value will be changed when MasterTrain runs so you won't see the change until that time. If you are running the program using ClusterRun the table value will be changed when MasterStartup runs.

  DataCol*            row_lookup_col;   // the column that will be used for row lookup when dtc_is_column_type is true
  String              row_lookup_value; // this value is used to find the row for which the user wants to set the cell value - the value will be matched against the values in the row_lookup_col
  
  ControlPanel*       control_panel;    // #READ_ONLY #SHOW the control panel that includes this cell in its member list
  bool                enabled;          // #READ_ONLY if a DTC is in a row that is currently deleted or filtered and thus not visible the enabled flag is set to false so that the control panel can disable the item but keep it visible
  
  void                GetControlPanelText(MemberDef* mbr, const String& xtra_lbl, String& full_lbl, String& desc) const override;
  // #IGNORE provides the canonical full label and (if empty) desc for control panel item
  void                GetControlPanelLabel(MemberDef* mbr, String& full_lbl) const override;
  // #IGNORE generate the label for the control panel item based on column and row
  void                SetControlPanelEnabled(bool do_enable);     // #IGNORE set for control panel to show enabled/disabled -- sets the taBase::BF_GUI_READ_ONLY so the control panel item will be disabled -- used when rows are hidden
  
  TA_BASEFUNS_NOCOPY(DataTableCell);
  
protected:
  void UpdateAfterEdit_impl() override;

private:
  void Initialize();
  void Destroy()     { };
};

#endif // DataTableCell_h
