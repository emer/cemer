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

#ifndef Switch_h
#define Switch_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>
#include <ProgEl_List>

// declare all other types mentioned but not required to include:
class ProgVar; // 


taTypeDef_Of(Switch);

class TA_API Switch: public ProgEl { 
  // switches execution based on the value of given variable -- each case expression is matched to a corresponding case_code item one-to-one
INHERITED(ProgEl)
public:
  ProgVarRef	    switch_var;	// #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser variable to switch on

  ProgEl_List	    cases; 	// #SHOW_TREE variable value and code to execute for each case (list of CaseBlock objects)

  int 		ProgElChildrenCount() const CPP11_OVERRIDE { return cases.size; }

  virtual void	    NewCase() 	{ cases.New(1); }
  // #BUTTON make a new case item
  virtual void	    CasesFmEnum();
  // #BUTTON #CONFIRM add all the cases for an enumerated type (switch_var must be either HARD_ENUM or DYN_ENUM)

  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const CPP11_OVERRIDE;
  bool		CvtFmCode(const String& code) CPP11_OVERRIDE;
  bool		IsCtrlProgEl() 	CPP11_OVERRIDE { return true; }

  ProgVar*	FindVarName(const String& var_nm) const CPP11_OVERRIDE;
  String	GetDisplayName() const CPP11_OVERRIDE;
  String 	GetTypeDecoKey() const CPP11_OVERRIDE { return "ProgCtrl"; }
  String	GetToolbarName() const CPP11_OVERRIDE { return "switch"; }

  PROGEL_SIMPLE_BASEFUNS(Switch);
protected:
  void		CheckThisConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		CheckChildConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		PreGenChildren_impl(int& item_id) CPP11_OVERRIDE;
  void		GenCssPre_impl(Program* prog) CPP11_OVERRIDE; 
  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE; 
  void		GenCssPost_impl(Program* prog) CPP11_OVERRIDE; 
  const String	GenListing_children(int indent_level) CPP11_OVERRIDE;

  virtual void	    CasesFmEnum_hard(); // switch_var is a hard enum
  virtual void	    CasesFmEnum_dyn();	// switch_var is a dynamic enum

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

#endif // Switch_h
