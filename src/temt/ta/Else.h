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

#ifndef Else_h
#define Else_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>
#include <ProgEl_List>

// declare all other types mentioned but not required to include:
class ProgVar; // 


taTypeDef_Of(Else);

class TA_API Else: public ProgEl { 
  // ##DEF_CHILD_else_code the other half of a conditional If test element: must come after an If or an ElseIf element, and is run if the condition there is false
INHERITED(ProgEl)
public:
  ProgEl_List	    else_code;  // #SHOW_TREE items to execute for else case

  int 		ProgElChildrenCount() const override
  { return else_code.size; }

  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;
  bool		IsCtrlProgEl() 	override { return true; }

  virtual bool  CheckAfterIf();
  // check if comes after if or else if

  taList_impl*	children_() override { return &else_code; }	
  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "ProgCtrl"; }
  ProgVar*	FindVarName(const String& var_nm) const override;
  String	GetToolbarName() const override { return "else"; }
  
  PROGEL_SIMPLE_BASEFUNS(Else);
protected:
  void		UpdateAfterEdit_impl() override;
  void		CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		CheckChildConfig_impl(bool quiet, bool& rval) override;
  void		PreGenChildren_impl(int& item_id) override;
  void		GenCssPre_impl(Program* prog) override; 
  void		GenCssBody_impl(Program* prog) override; //replaces If
  void		GenCssPost_impl(Program* prog) override; 
  const String	GenListing_children(int indent_level) override;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

#endif // Else_h
