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

#ifndef ControlPanel_Group_h
#define ControlPanel_Group_h 1

// parent includes:
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class ControlPanel; //

taTypeDef_Of(ControlPanel);
taTypeDef_Of(ControlPanel_Group);

class TA_API ControlPanel_Group : public taGroup<ControlPanel> {
  // #AKA_SelectEdit_Group  ##CAT_Display ##EXPAND_DEF_1 group of control panel dialog objects
INHERITED(taGroup<ControlPanel>)
public:
  virtual void  AutoEdit();
  virtual void  RestorePanels();
  // if the panel was pinned when the program was saved redisplay it on project open

  String       GetTypeDecoKey() const override { return "ControlPanel"; }

  TA_BASEFUNS(ControlPanel_Group);
private:
  NOCOPY(ControlPanel_Group)
  void  Initialize() { SetBaseType(&TA_ControlPanel);}
  void  Destroy()               { };
};

#endif // ControlPanel_Group_h
