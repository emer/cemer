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

  override int 		ProgElChildrenCount() const { return cases.size; }

  virtual void	    NewCase() 	{ cases.New(1); }
  // #BUTTON make a new case item
  virtual void	    CasesFmEnum();
  // #BUTTON #CONFIRM add all the cases for an enumerated type (switch_var must be either HARD_ENUM or DYN_ENUM)

  override bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const;
  override bool		CvtFmCode(const String& code);
  override bool		IsCtrlProgEl() 	{ return true; }

  override ProgVar*	FindVarName(const String& var_nm) const;
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }
  override String	GetToolbarName() const { return "switch"; }

  PROGEL_SIMPLE_BASEFUNS(Switch);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override void		GenCssPre_impl(Program* prog); 
  override void		GenCssBody_impl(Program* prog); 
  override void		GenCssPost_impl(Program* prog); 
  override const String	GenListing_children(int indent_level);

  virtual void	    CasesFmEnum_hard(); // switch_var is a hard enum
  virtual void	    CasesFmEnum_dyn();	// switch_var is a dynamic enum

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

#endif // Switch_h
