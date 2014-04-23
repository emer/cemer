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

  int          ProgElChildrenCount() const override { return fun_code.size; }

  virtual void  UpdateCallerArgs();
  // #BUTTON #CAT_Code run UpdateArgs on all the function calls to me, and also display all these calls in the Find dialog (searching on this function's name) so you can make sure the args are correct for each call

  virtual String GetFunDecl();
  // get the function declaration (c++ code string for function, with return type and args)
  virtual void  GenCss_Decl(Program* prog);
  // generate forward declarations for all functions -- allows them to appear in any order

  ProgVar*     FindVarName(const String& var_nm) const override;
  String       GetDisplayName() const override;
  String       GetTypeDecoKey() const override { return "Function"; }
  String       GetToolbarName() const override { return "fun def"; }

  // below from taNBase for name:
  bool	       HasName() const override { return true; }
  bool         SetName(const String& nm) override;
  String       GetName() const override { return name; }
  void 	       SetDefaultName() override;
  void 	       MakeNameUnique() override;

  bool         CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool         CvtFmCode(const String& code) override;
  bool         BrowserEditSet(const String& code, int move_after = 0) override;

  void         InitLinks() override;
  void         Copy_(const Function& cp);
  TA_BASEFUNS(Function);
protected:
  void         UpdateAfterEdit_impl() override;
  void         UpdateAfterCopy(const ProgEl& cp) override;
  void         CheckChildConfig_impl(bool quiet, bool& rval) override;
  void         CheckThisConfig_impl(bool quiet, bool& rval) override;
  void         PreGenChildren_impl(int& item_id) override;
  void         GenCssBody_impl(Program* prog) override;
  const String GenListing_children(int indent_level) override;

private:
  void  Initialize();
  void  Destroy()       {CutLinks();}
};

SMARTREF_OF(TA_API, Function);

#endif // Function_h
