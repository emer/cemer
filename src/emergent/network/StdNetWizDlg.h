// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef StdNetWizDlg_h
#define StdNetWizDlg_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// member includes:
#include <Network>

// declare all other types mentioned but not required to include:
class taGuiDialog; //
class DataTable; //


TypeDef_Of(StdNetWizDlg);

class EMERGENT_API StdNetWizDlg : public taNBase {
  // #CAT_Wizard dialog for StdNetwork() wizard action
INHERITED(taNBase)
public:
  taGuiDialog* 	Dlg1;  
  taGuiDialog*	Dlg2;  
  NetworkRef 	network;  
  int 		n_layers;  
  DataTable*	net_config;	// network configuration data table
 
  void 	NewNetwork();
  // #MENU #MENU_ON_Actions callback for dialog
  void 	NLayersFmNetwork();
  // #MENU callback for dialog
  void 	ConfigOneLayer(int lay_no, const String& nm, const String& typ);
  // #MENU callback for dialog
  void 	NewNetDefaultConfig();
  // #MENU callback for dialog
  void 	AddNewLayerRow();
  // #MENU callback for dialog
  void 	RefreshLayerList();
  // #MENU callback for dialog

  virtual bool	DoDialog();
  // do the dialog and configure the network based on results -- returns true if network configured, false if not

  TA_SIMPLE_BASEFUNS(StdNetWizDlg);
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};
  
#endif // StdNetWizDlg_h
