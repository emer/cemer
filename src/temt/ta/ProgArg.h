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

#ifndef ProgArg_h
#define ProgArg_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class ProgVar; // 


class TA_API ProgArg: public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_Program a program or method argument
INHERITED(taOBase)
public:
  TypeDef*              arg_type; // #READ_ONLY typedef of the target arg, where available
  String                type; // #SHOW #READ_ONLY the type of the argument (automatically set from the target function)
  String                name; // #SHOW #READ_ONLY the name of the argument (automatically set from the target function)
  bool                  required; // #SHOW #READ_ONLY if a value is required (i.e., it is not a default argument)
  String                def_val; // #SHOW #READ_ONLY for default arguments, what will get passed by default -- this is for reference only (leave expr blank for default)
  ProgExpr              expr; // the expression to compute and pass as the argument -- enter <no_arg> to specify a null or empty argument for program calls -- does not set this arg value

  virtual void          SetVarAsExpr(ProgVar* prog_var);
  // #DROP1 set given variable as the expression value for this arg
  virtual bool          UpdateFromVar(const ProgVar& cp);
  // updates our type information given variable that we apply to -- returns true if any changes
  virtual bool          UpdateFromType(TypeDef* td);
  // updates our type information from method typedef that we apply to -- returns true if any changes

  bool          SetName(const String& nm)       { name = nm; return true; }
  String        GetName() const                 { return name; }

  override String GetDisplayName() const;
  override String GetTypeDecoKey() const { return "ProgArg"; }

  override bool         BrowserSelectMe();
  override bool         BrowserExpandAll();
  override bool         BrowserCollapseAll();

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(ProgArg);
protected:
  override void         CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void  Copy_(const ProgArg& cp);
  void  Initialize();
  void  Destroy();
};

#endif // ProgArg_h
