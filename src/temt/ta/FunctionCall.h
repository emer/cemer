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

#ifndef FunctionCall_h
#define FunctionCall_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>
#include <Function>
#include <ProgArg_List>

// declare all other types mentioned but not required to include:


taTypeDef_Of(FunctionCall);

class TA_API FunctionCall: public ProgEl {
  // ##DEF_CHILD_fun_args call a function
INHERITED(ProgEl)
public:
  ProgVarRef            result_var;
  // #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser where to store the result (return value) of the function (optional -- can be NULL)
  FunctionRef           fun;
  // the function to be called
  ProgArg_List          fun_args;
  // #SHOW_TREE arguments to the function: passed when called

  virtual void          UpdateArgs();
  // #BUTTON updates the argument list based on the function being called

  bool         CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool         CvtFmCode(const String& code) override;

  taList_impl* children_() override {return &fun_args;}
  String       GetDisplayName() const override;
  String       GetTypeDecoKey() const override { return "Function"; }
  String       GetToolbarName() const override { return "fun()"; }

  PROGEL_SIMPLE_BASEFUNS(FunctionCall);
protected:
  void         UpdateAfterEdit_impl() override;
  void         CheckThisConfig_impl(bool quiet, bool& rval) override;
  void         CheckChildConfig_impl(bool quiet, bool& rval) override;
  void         GenCssBody_impl(Program* prog) override;
private:
  void  Initialize();
  void  Destroy()       {}
};

#endif // FunctionCall_h
