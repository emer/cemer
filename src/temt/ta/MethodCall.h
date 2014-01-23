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

#ifndef MethodCall_h
#define MethodCall_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>
#include <ProgArg_List>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MethodDef; // 


taTypeDef_Of(MethodCall);

class TA_API MethodCall: public ProgEl { 
  // ##DEF_CHILD_meth_args call a method (member function) on an object
INHERITED(ProgEl)
public:
  ProgVarRef		result_var;
  // #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser where to store the result of the method call (optional -- can be NULL)
  ProgVarRef		obj;
  // #ITEM_FILTER_ObjProgVarFilter program variable that points to the object with the method to call
  TypeDef*		obj_type;
  // #NO_SHOW #NO_SAVE temp copy of obj.object_type
  MethodDef*		method;
  // #TYPE_ON_obj_type the method to call on object obj
  ProgArg_List		meth_args;
  // #SHOW_TREE arguments to be passed to the method
  String		meth_sig;
  // #READ_ONLY #NO_SAVE #SHOW signature of the method, for reference
  String		meth_desc;
  // #READ_ONLY #NO_SAVE #SHOW description of the method, for reference

  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const CPP11_OVERRIDE;
  bool		CvtFmCode(const String& code) CPP11_OVERRIDE;

  taList_impl*	children_() CPP11_OVERRIDE {return &meth_args;}	
  String	GetDisplayName() const CPP11_OVERRIDE;
  String 	GetTypeDecoKey() const CPP11_OVERRIDE { return "Function"; }
  String	GetToolbarName() const CPP11_OVERRIDE { return "meth()"; }
  void 	Help() CPP11_OVERRIDE;

  PROGEL_SIMPLE_BASEFUNS(MethodCall);
protected:
  void		UpdateAfterEdit_impl() CPP11_OVERRIDE;
  void 	CheckThisConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		CheckChildConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // MethodCall_h
