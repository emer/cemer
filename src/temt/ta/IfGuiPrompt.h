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

#ifndef IfGuiPrompt_h
#define IfGuiPrompt_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgEl_List>

// declare all other types mentioned but not required to include:
class ProgVar; // 


taTypeDef_Of(IfGuiPrompt);

class TA_API IfGuiPrompt: public ProgEl { 
  // if in gui mode, prompt user prior to performing a given operation -- if user says OK then run the code, otherwise do nothing -- if not in gui mode (e.g., running in batch mode) then always run the code -- allows interactive control over otherwise default operations
INHERITED(ProgEl)
public:
  String	prompt; 	// prompt to display to user in gui mode
  String	yes_label;	// label to display for the Yes/Ok answer
  String	no_label;	// label to display for the No/Cancel answer

  ProgEl_List	yes_code; 	// #SHOW_TREE items to execute if user says Yes/Ok to prompt in gui mode, or to always execute in nogui mode

  int 		ProgElChildrenCount() const CPP11_OVERRIDE { return yes_code.size; }

  ProgVar*	FindVarName(const String& var_nm) const CPP11_OVERRIDE;
  String	GetDisplayName() const CPP11_OVERRIDE;
  String 	GetTypeDecoKey() const CPP11_OVERRIDE { return "ProgCtrl"; }
  String	GetToolbarName() const CPP11_OVERRIDE { return "if gui prmt"; }

  PROGEL_SIMPLE_BASEFUNS(IfGuiPrompt);
protected:
  void		UpdateAfterEdit_impl() CPP11_OVERRIDE;
  void		CheckChildConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		PreGenChildren_impl(int& item_id) CPP11_OVERRIDE;
  void		GenCssPre_impl(Program* prog) CPP11_OVERRIDE; 
  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE; 
  void		GenCssPost_impl(Program* prog) CPP11_OVERRIDE; 
  const String	GenListing_children(int indent_level) CPP11_OVERRIDE;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

#endif // IfGuiPrompt_h
