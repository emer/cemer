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

#ifndef ProgArg_List_h
#define ProgArg_List_h 1

// parent includes:
#include <ProgArg>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class ProgVar_List; // 
class MethodDef; // 


taTypeDef_Of(ProgArg_List);

class TA_API ProgArg_List : public taList<ProgArg> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE #FIXED_SIZE ##CAT_Program list of arguments
INHERITED(taList<ProgArg>)
public:
  String_Array  saved_exprs;    // #HIDDEN #NO_SAVE saved arg exprs across changes

  virtual bool  UpdateFromVarList(ProgVar_List& targ);
  // update our list of args based on target variable list -- returns true if updated
  virtual bool  UpdateFromMethod(MethodDef* md);
  // update our list of args based on method def arguments -- returns true if updated
  virtual void  SaveExprs();
  // save expressions to saved_exprs
  virtual void  SetPrevExprs();
  // set prev_expr fm saved_exprs

  String GetTypeDecoKey() const override { return "ProgArg"; }
  virtual const String  GenCssArgs();

  virtual void  ParseArgString(const String& args);
  // parse arg string into arg expressions -- for inline code editor system

  bool         BrowserSelectMe() override;
  bool         BrowserExpandAll() override;
  bool         BrowserCollapseAll() override;

  TA_BASEFUNS_NOCOPY(ProgArg_List);
protected:
  void CheckChildConfig_impl(bool quiet, bool& rval) override;
private:
  void  Initialize();
  void  Destroy() {Reset();}
};

#endif // ProgArg_List_h
