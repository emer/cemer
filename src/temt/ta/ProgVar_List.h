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

#ifndef ProgVar_List_h
#define ProgVar_List_h 1

// parent includes:
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class Program; // 
class taNBase; // 
class DataTable; // 
class ProgVar; //

class TA_API ProgVar_List : public taList<ProgVar> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of script variables
INHERITED(taList<ProgVar>)
public:
  enum VarContext {
    VC_ProgVars,  // #LABEL_ProgramVariables program variables
    VC_FuncArgs  //  #LABEL_FunctionArguments function arguments
  };

  VarContext    var_context; // #DEF_VC_ProgVars #HIDDEN #NO_SAVE context of vars, set by owner

  virtual const String  GenCss_FuncArgs() const; // generate css script code for func args
  virtual void          GenCss_ProgVars(Program* prog) const; // generate css script code for prog vars
  virtual void          GenCssInitFrom(Program* prog) const;
  // init_from code for all vars in list
  virtual const String  GenListing(int indent_level) const; // generate listing of program

  virtual void  AddVarTo(taNBase* src);
  // #DROPN add a var to the given object
  virtual void  CreateDataColVars(DataTable* src);
  // #DROP1 #BUTTON create column variables for given database object (only for scalar vals -- not matrix ones)

  virtual ProgVar* FindVarType(int vart, TypeDef* td = NULL);
  // find first variable of given type (ProgVar::VarType) (if hard enum or object type, td specifies type of object to find if not null)

  override String GetTypeDecoKey() const { return "ProgVar"; }

  override bool         BrowserSelectMe();
  override bool         BrowserExpandAll();
  override bool         BrowserCollapseAll();

  void  setStale();
  TA_BASEFUNS(ProgVar_List);

protected:
  override void El_SetIndex_(void*, int);

private:
  void  Copy_(const ProgVar_List& cp);
  void  Initialize();
  void  Destroy() {Reset();}
};

#endif // ProgVar_List_h
