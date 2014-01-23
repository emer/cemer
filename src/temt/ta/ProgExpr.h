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

#ifndef ProgExpr_h
#define ProgExpr_h 1

// parent includes:
#include <ProgExprBase>

// member includes:

// declare all other types mentioned but not required to include:
class ProgVar; // 


taTypeDef_Of(ProgExpr);

class TA_API ProgExpr : public ProgExprBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_Program an expression in a program -- manages variable references so they are always updated when program variables change -- includes variable lookup functions
INHERITED(ProgExprBase)
public:
  // Signature must match that of the item_filter_fun typedef.
  static bool           StdProgVarFilter(void* base, void* var); // generic progvar filter -- excludes variables from functions if not itself in same function -- use this for most progvars in ITEM_FILTER comment directive

  ProgVar*      var_lookup;     // #NULL_OK #NO_SAVE #NO_EDIT #NO_UPDATE_POINTER #ITEM_FILTER_StdProgVarFilter lookup a program variable and add it to the current expression (this field then returns to empty/NULL)

  void  CutLinks();
  TA_BASEFUNS_NOCOPY(ProgExpr);
protected:
  void UpdateAfterEdit_impl() CPP11_OVERRIDE;
private:
  void  Initialize();
  void  Destroy();
};

#endif // ProgExpr_h
