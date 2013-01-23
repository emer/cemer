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

#ifndef DataTableEditorOptions_h
#define DataTableEditorOptions_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(DataTableEditorOptions);

class TA_API DataTableEditorOptions : public taOBase {
  // for specifying and saving params for editing options
INHERITED(taOBase)
public:
  enum ViewMode {
    VM_CellDetail,      // #LABEL_Cell_Detail separate panel for cell detail editing
    VM_Flat             // #LABEL_Flat show all data in the main table, using dummy cells
  };

  TA_BASEFUNS_NOCOPY(DataTableEditorOptions);

private:
  void  Initialize() {}
  void  Destroy() {}
};

#endif // DataTableEditorOptions_h
