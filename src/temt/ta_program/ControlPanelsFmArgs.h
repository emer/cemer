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

#ifndef ControlPanelsFmArgs_h
#define ControlPanelsFmArgs_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:
class ControlPanel; // 


taTypeDef_Of(ControlPanelsFmArgs);

class TA_API ControlPanelsFmArgs: public ProgEl { 
  // #AKA_SelectEditsFmArgs sets ControlPanel value(s) based on startup arguments of the name of the control panel item -- items must be single atomic value items -- not classes
INHERITED(ProgEl)
public:
  ProgVarRef		ctrl_panel_var;	// #ITEM_FILTER_ObjProgVarFilter #AKA_sel_edit_var program variable pointing to control panel that contains the items to access with startup arguments

  virtual ControlPanel* 	GetControlPanel() const;
  // get actual control panel from variable

  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "ProgVar"; }
  String	GetToolbarName() const override { return "ctrl=args"; }
  bool    CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool    CvtFmCode(const String& code) override;

  void	GenRegArgs(Program* prog);
  // #IGNORE generate RegisterArgs code

  PROGEL_SIMPLE_BASEFUNS(ControlPanelsFmArgs);
protected:
  void UpdateAfterEdit_impl() override;
  void CheckThisConfig_impl(bool quiet, bool& rval) override;
  void	GenCssBody_impl(Program* prog) override;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // ControlPanelsFmArgs_h
