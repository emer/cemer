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

#ifndef ProgExpr_List_h
#define ProgExpr_List_h 1

// parent includes:
#include <ProgExpr>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(ProgExpr_List);

class TA_API ProgExpr_List : public taList<ProgExpr> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of program expressions
INHERITED(taList<ProgExpr>)
public:

  override String GetTypeDecoKey() const { return "ProgExpr"; }

  virtual void  UpdateProgExpr_NewOwner();
  // calls UpdateProgExpr_NewOwner() on all the prog expr's in the list

  TA_BASEFUNS_NOCOPY(ProgExpr_List);
protected:
  override void CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void  Initialize();
  void  Destroy() {Reset();}
};

#endif // ProgExpr_List_h
