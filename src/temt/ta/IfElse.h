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

#ifndef IfElse_h
#define IfElse_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>
#include <ProgEl_List>

// declare all other types mentioned but not required to include:
class ProgVar; // 


taTypeDef_Of(IfElse);

class TA_API IfElse: public ProgEl { 
  // #AKA_If ##HAS_CONDTREE a conditional test element: if(condition) then true_code; else false_code
INHERITED(ProgEl)
public:
  ProgExpr	    cond; 	// condition expression to test for true or false
  bool		    show_false_code; // display the false_code, which is run when the cond condition is false -- forced to be true if there is any existing content in false_code
  ProgEl_List	    true_code; 	// #SHOW_TREE items to execute if condition true
  ProgEl_List	    false_code; // #SHOW_TREE #CONDTREE_ON_show_false_code items to execute if condition false

  int 		ProgElChildrenCount() const CPP11_OVERRIDE
  { return true_code.size + false_code.size; }

  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const CPP11_OVERRIDE;
  bool		CvtFmCode(const String& code) CPP11_OVERRIDE;
  bool		IsCtrlProgEl() 	CPP11_OVERRIDE { return true; }

  String	GetDisplayName() const CPP11_OVERRIDE;
  String 	GetTypeDecoKey() const CPP11_OVERRIDE { return "ProgCtrl"; }
  ProgVar*	FindVarName(const String& var_nm) const CPP11_OVERRIDE;
  String	GetToolbarName() const CPP11_OVERRIDE { return "if.else"; }
  
  PROGEL_SIMPLE_BASEFUNS(IfElse);
protected:
  void		UpdateAfterEdit_impl() CPP11_OVERRIDE;
  void		CheckThisConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		CheckChildConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		PreGenChildren_impl(int& item_id) CPP11_OVERRIDE;
  void		GenCssPre_impl(Program* prog) CPP11_OVERRIDE; 
  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE; //replaces If
  void		GenCssPost_impl(Program* prog) CPP11_OVERRIDE; 
  const String	GenListing_children(int indent_level) CPP11_OVERRIDE;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

#endif // IfElse_h
