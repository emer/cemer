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

#ifndef Function_h
#define Function_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>
#include <ProgVar_List>
#include <ProgEl_List>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class ProgVar; // 


taTypeDef_Of(Function);

class TA_API Function: public ProgEl {
  // a user-defined function that can be called within the program where it is defined -- must live in the functions of a Program, not in init_code or prog_code
INHERITED(ProgEl)
public:
  String                name;
  // The function name
  ProgVar::VarType      return_type;
  // The return type for the function -- what kind of variable does it return
  TypeDef*              object_type;    // #CONDSHOW_ON_return_type:T_Object #NO_NULL #TYPE_taBase for Object* return types, the type of object to return
  ProgVar_List          args;
  // The arguments to the function
  ProgEl_List           fun_code;
  // the function code (list of program elements)

  int          ProgElChildrenCount() const CPP11_OVERRIDE { return fun_code.size; }

  virtual void  UpdateCallerArgs();
  // #BUTTON #CAT_Code run UpdateArgs on all the function calls to me, and also display all these calls in the Find dialog (searching on this function's name) so you can make sure the args are correct for each call

  ProgVar*     FindVarName(const String& var_nm) const CPP11_OVERRIDE;
  String       GetDisplayName() const CPP11_OVERRIDE;
  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Function"; }
  String       GetToolbarName() const CPP11_OVERRIDE { return "fun def"; }

  // below from taNBase for name:
  bool		HasName() const CPP11_OVERRIDE { return true; }
  bool         SetName(const String& nm) CPP11_OVERRIDE;
  String       GetName() const CPP11_OVERRIDE { return name; }
  void 	SetDefaultName() CPP11_OVERRIDE;
  void 	MakeNameUnique() CPP11_OVERRIDE;

  void         InitLinks() CPP11_OVERRIDE;
  void Copy_(const Function& cp);
  TA_BASEFUNS(Function);
protected:
  void         UpdateAfterEdit_impl() CPP11_OVERRIDE;
  void         UpdateAfterCopy(const ProgEl& cp) CPP11_OVERRIDE;
  void         CheckChildConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void         CheckThisConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void         PreGenChildren_impl(int& item_id) CPP11_OVERRIDE;
  void         GenCssBody_impl(Program* prog) CPP11_OVERRIDE;
  const String GenListing_children(int indent_level) CPP11_OVERRIDE;

private:
  void  Initialize();
  void  Destroy()       {CutLinks();}
};

SmartRef_Of(Function);

#endif // Function_h
