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

#ifndef ControlPanelConfig_h
#define ControlPanelConfig_h 1

// parent includes:
#include <taBase>

// member includes:
#include <String_Array>

// declare all other types mentioned but not required to include:


taTypeDef_Of(ControlPanelConfig);

class TA_API ControlPanelConfig : public taBase {
  // #EDIT_INLINE ##NO_TOKENS ##CAT_Display #OBSOLETE special parameters for controlling the select edit display
  INHERITED(taBase)
public:
  bool          auto_edit;      // automatically bring up edit dialog upon loading
  String_Array  mbr_labels;     // extra labels at the start of each member label for the selected fields
  String_Array  meth_labels;    // extra labels at the start of each method label for the selected functions

  SIMPLE_LINKS(ControlPanelConfig);
  TA_BASEFUNS_NOCOPY(ControlPanelConfig);
private:
  void  Initialize() {auto_edit = false;}
  void  Destroy() {CutLinks();}
};

#endif // ControlPanelConfig_h
