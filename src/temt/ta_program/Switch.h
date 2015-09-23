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
class CaseBlock; //

taTypeDef_Of(Switch);

class TA_API Switch: public ProgEl { 
  // ##DEF_CHILD_cases switches execution based on the value of given variable -- each case expression is matched to a corresponding case_code item one-to-one
INHERITED(ProgEl)
public:
  ProgVarRef        switch_var;	 // #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser variable to switch on

  ProgEl_List       cases; 	// #SHOW_TREE variable value and code to execute for each case (list of CaseBlock objects)
  int               ProgElChildrenCount() const override { return cases.size; }

  virtual void	    AddCase();
  // #BUTTON make a new case item
  virtual void	    AddDefaultCase();
  // #BUTTON make a default case item
  virtual void	    CasesFmEnum(bool add_default = false);
  // #BUTTON #CONFIRM add all the cases for an enumerated type (switch_var must be either HARD_ENUM or DYN_ENUM) -- if add_default is true then add a default case at the end as well

  bool          CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool          CvtFmCode(const String& code) override;
  bool          IsCtrlProgEl() 	override { return true; }
  bool          HasDefaultCase();

  taList_impl*	children_() override { return &cases; }
  ProgVar*      FindVarName(const String& var_nm) const override;
  String        GetDisplayName() const override;
  String        GetTypeDecoKey() const override { return "ProgCtrl"; }
  String        GetToolbarName() const override { return "switch"; }
  bool          BrowserEditTest() override;
  bool          ChooseMe() override;
// #IGNORE pop chooser for selecting variable on which to switch

  PROGEL_SIMPLE_BASEFUNS(Switch);
protected:
  void		CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		CheckChildConfig_impl(bool quiet, bool& rval) override;
  void		PreGenChildren_impl(int& item_id) override;
  void		GenCssPre_impl(Program* prog) override; 
  void		GenCssBody_impl(Program* prog) override; 
  void		GenCssPost_impl(Program* prog) override; 
  const String	GenListing_children(int indent_level) override;

  virtual void	    CasesFmEnum_hard(); // switch_var is a hard enum
  virtual void	    CasesFmEnum_dyn();	// switch_var is a dynamic enum

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

#endif // Switch_h
