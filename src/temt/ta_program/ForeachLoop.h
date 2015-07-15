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

#ifndef ForeachLoop_h
#define ForeachLoop_h 1

// parent includes:
#include <Loop>

// member includes:
#include <ProgVar>
#include <ProgExprBase>

// declare all other types mentioned but not required to include:
class ProgEl; // 


taTypeDef_Of(ForeachLoop);

class TA_API ForeachLoop: public Loop { 
  // iterate over each item in a container object (List, Group, Matrix), performing loop_code on each element, which is encoded in the el_var -- in expression can be any kind of selection over items 
INHERITED(Loop)
public:
  ProgVarRef	    	el_var; // #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser variable that will represent each element in the list of items
  ProgExprBase	    	in; 	// #BROWSER_EDIT_LOOKUP expression for container list of items to iterate over -- typically a [ ] selection over items in a List, Group, or Matrix

  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;
  bool		IsCtrlProgEl() 	override { return true; }

  String	GetDisplayName() const override;
  void		SetProgExprFlags() override;
  String	GetToolbarName() const override { return "foreach"; }

  PROGEL_SIMPLE_BASEFUNS(ForeachLoop);
protected:
  void	UpdateAfterEdit_impl() override;
  bool		ParentForeachLoopVarClashes(); // true if a parent For loop is also using the loop_var
  void	CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		GenCssPre_impl(Program* prog) override; 
  void		GenCssPost_impl(Program* prog) override; 
private:
  void	Initialize();
  void	Destroy()	{}
};

#endif // ForeachLoop_h
