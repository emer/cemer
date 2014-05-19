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

#ifndef ElseIf_h
#define ElseIf_h 1

// parent includes:
#include <CondBase>

// member includes:

// declare all other types mentioned but not required to include:
class ProgVar; // 


taTypeDef_Of(ElseIf);

class TA_API ElseIf: public CondBase {
  // ##DEF_CHILD_true_code a conditional test element: if(condition) then run true_code -- comes after a previous If or ElseIf and only runs if that previous test was false -- can be followed in turn by an Else or ElseElseIf to run if condition is false
INHERITED(CondBase)
public:
  ProgEl_List	    true_code; 	// #SHOW_TREE items to execute if condition true

  int 		ProgElChildrenCount() const override
  { return true_code.size; }

  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;

  virtual bool  CheckAfterIf();
  // check if comes after if or else if

  taList_impl*	children_() override { return &true_code; }	
  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "ProgCtrl"; }
  ProgVar*	FindVarName(const String& var_nm) const override;
  String	GetToolbarName() const override { return "else.if"; }
  
  PROGEL_SIMPLE_BASEFUNS(ElseIf);
protected:
  void		UpdateAfterEdit_impl() override;
  void		CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		CheckChildConfig_impl(bool quiet, bool& rval) override;
  void		PreGenChildren_impl(int& item_id) override;
  void		GenCssPre_impl(Program* prog) override; 
  void		GenCssBody_impl(Program* prog) override; //replaces ElseIf
  void		GenCssPost_impl(Program* prog) override; 
  const   String	GenListing_children(int indent_level) override;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

#endif // ElseIf_h
