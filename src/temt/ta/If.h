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

#ifndef If_h
#define If_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>
#include <ProgEl_List>

// declare all other types mentioned but not required to include:
class ProgVar; // 


taTypeDef_Of(If);

class TA_API If: public ProgEl { 
  // #AKA_IfElse ##DEF_CHILD_true_code a conditional test element: if(condition) then run true_code -- can be followed by an Else or ElseIf to run if condition is false
INHERITED(ProgEl)
public:
  ProgExpr	    cond; 	// #BROWSER_EDIT_LOOKUP condition expression to test for true or false
  ProgEl_List	    true_code; 	// #SHOW_TREE items to execute if condition true

  ProgEl_List	    false_code; // #HIDDEN #NO_SAVE #OBSOLETE items to execute if condition false (now moved to a separate Else code)

  int 		ProgElChildrenCount() const override
  { return true_code.size; }

  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;
  bool		IsCtrlProgEl() 	override { return true; }

  taList_impl*	children_() override { return &true_code; }	
  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "ProgCtrl"; }
  ProgVar*	FindVarName(const String& var_nm) const override;
  String	GetToolbarName() const override { return "if"; }
  
  PROGEL_SIMPLE_BASEFUNS(If);
protected:
  void		UpdateAfterEdit_impl() override;
  void		CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		CheckChildConfig_impl(bool quiet, bool& rval) override;
  void		PreGenChildren_impl(int& item_id) override;
  void		GenCssPre_impl(Program* prog) override; 
  void		GenCssBody_impl(Program* prog) override; //replaces If
  void		GenCssPost_impl(Program* prog) override; 
  const String	GenListing_children(int indent_level) override;

  void          ConvertFromIfElse(); // #IGNORE

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

#endif // If_h
