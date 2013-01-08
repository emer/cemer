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


class TA_API ProgArg_List : public taList<ProgArg> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE #FIXED_SIZE ##CAT_Program list of arguments
INHERITED(taList<ProgArg>)
public:

  virtual bool  UpdateFromVarList(ProgVar_List& targ);
  // update our list of args based on target variable list -- returns true if updated
  virtual bool  UpdateFromMethod(MethodDef* md);
  // update our list of args based on method def arguments -- returns true if updated

  override String GetTypeDecoKey() const { return "ProgArg"; }
  virtual const String  GenCssArgs();

  virtual void  UpdateProgExpr_NewOwner();
  // calls UpdateProgExpr_NewOwner() on all the prog expr's in the list

  override bool         BrowserSelectMe();
  override bool         BrowserExpandAll();
  override bool         BrowserCollapseAll();

  TA_BASEFUNS_NOCOPY(ProgArg_List);
protected:
  override void CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void  Initialize();
  void  Destroy() {Reset();}
};

#endif // ProgArg_List_h
