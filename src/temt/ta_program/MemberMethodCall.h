// Copyright 2017, Regents of the University of Colorado,
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

#ifndef MemberMethodCall_h
#define MemberMethodCall_h 1

// parent includes:
#include <MemberProgEl>

// member includes:
#include <ProgVar>
#include <ProgArg_List>

// declare all other types mentioned but not required to include:
class MethodDef; // 
class ProgEl; // 


taTypeDef_Of(MemberMethodCall);

class TA_API MemberMethodCall: public MemberProgEl { 
  // ##DEF_CHILD_meth_args ##NO_EXPAND call a method on a member of an object -- can be any sub-object below selected object, as long as it can be found both at the time of programming and execution
INHERITED(MemberProgEl)
public:
  ProgVarRef      result_var;
  // #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser where to store the result of the method call (optional -- can be NULL)
  MethodDef*      method;
  // #TYPE_ON_obj_type the method to call on the member object that is at obj->path
  ProgArg_List	  meth_args;
  // #TREE_SHOW arguments to be passed to the method
  String          meth_sig;
  // #READ_ONLY #SHOW signature of the method, for reference
  String          meth_desc;
  // #READ_ONLY #SHOW description of the method, for reference

  bool        CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool        CvtFmCode(const String& code) override;
  bool        ChooseMe() override;
  // #IGNORE pop chooser for selecting object of this type

  taList_impl*	children_() override {return &meth_args;}	
  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "Function"; }
  String	GetToolbarName() const override { return "mbr.mth()"; }

  PROGEL_SIMPLE_BASEFUNS(MemberMethodCall);
protected:
  void		UpdateAfterEdit_impl() override;
  void 	  CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		CheckChildConfig_impl(bool quiet, bool& rval) override;
  void		GenCssBody_impl(Program* prog) override;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // MemberMethodCall_h
