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

#ifndef StaticMethodCall_h
#define StaticMethodCall_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>
#include <ProgArg_List>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MethodDef; // 


taTypeDef_Of(StaticMethodCall);

class TA_API StaticMethodCall: public ProgEl {
  // ##DEF_CHILD_meth_args call a static method (member function) on a type
INHERITED(ProgEl)
public:
  ProgVarRef            result_var; // #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser result variable (optional -- can be NULL)
  TypeDef*              min_type; // #NO_SHOW #NO_SAVE #TYPE_taBase minimum object type to choose from -- anchors selection of object_type (derived versions can set this)
  TypeDef*              object_type; // #TYPE_ON_min_type The object type to look for methods on
  MethodDef*            method; //  #TYPE_ON_object_type the method to call
  ProgArg_List          meth_args;
  // #SHOW_TREE arguments to be passed to the method

  bool         CanCvtFmCode(const String& code, ProgEl* scope_el) const CPP11_OVERRIDE;
  bool         CvtFmCode(const String& code) CPP11_OVERRIDE;

  taList_impl* children_() CPP11_OVERRIDE {return &meth_args;}
  String       GetDisplayName() const CPP11_OVERRIDE;
  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Function"; }
  String       GetToolbarName() const CPP11_OVERRIDE { return "static()"; }

  PROGEL_SIMPLE_BASEFUNS(StaticMethodCall);
protected:
  void         UpdateAfterEdit_impl() CPP11_OVERRIDE;
  void         CheckThisConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void         CheckChildConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void         GenCssBody_impl(Program* prog) CPP11_OVERRIDE; // generate the Css body code for this object

private:
  void  Initialize();
  void  Destroy()       {CutLinks();}
};

#endif // StaticMethodCall_h
