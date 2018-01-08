// Copyright 2013-2018, Regents of the University of Colorado,
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
  // ##DEF_CHILD_meth_args ##NO_EXPAND call a static method (member function) on a type
INHERITED(ProgEl)
public:
  ProgVarRef            result_var; // #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser result variable (optional -- can be NULL)
  TypeDef*              min_type; // #HIDDEN #NO_SAVE #TYPE_taBase minimum object type to choose from -- anchors selection of object_type (derived versions can set this)
  TypeDef*              object_type; // #TYPE_ON_min_type The object type to look for methods on
  MethodDef*            method; //  #TYPE_ON_object_type the method to call
  ProgArg_List          meth_args;
  // #TREE_SHOW arguments to be passed to the method

  bool         CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool         CvtFmCode(const String& code) override;

  taList_impl* children_() override {return &meth_args;}
  String       GetDisplayName() const override;
  String       GetTypeDecoKey() const override { return "Function"; }
  String       GetToolbarName() const override { return "static()"; }
  
  bool        ChooseMe() override;
  // #IGNORE pop chooser for selecting a method

  PROGEL_SIMPLE_BASEFUNS(StaticMethodCall);
protected:
  void         UpdateAfterEdit_impl() override;
  void         CheckThisConfig_impl(bool quiet, bool& rval) override;
  void         CheckChildConfig_impl(bool quiet, bool& rval) override;
  bool         GenCssBody_impl(Program* prog) override; // generate the Css body code for this object

private:
  void  Initialize();
  void  Destroy()       {CutLinks();}
};

#endif // StaticMethodCall_h
