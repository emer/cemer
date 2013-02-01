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

#ifndef DynEnumItem_List_h
#define DynEnumItem_List_h 1

// parent includes:
#include <DynEnumItem>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(DynEnumItem_List);

class TA_API DynEnumItem_List : public taList<DynEnumItem> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program ##SCOPE_Program list of dynamic enumerated type items
INHERITED(taList<DynEnumItem>)
public:

  virtual int   FindNumIdx(int val) const; // find index of given numerical value

  virtual void  OrderItems();
  // ensure that the item values are sequentially increasing

  override void SigEmit(int dcr, void* op1 = NULL, void* op2 = NULL);

  override int  El_Compare_(void* a, void* b) const
  { int rval=-1; if(((DynEnumItem*)a)->value > ((DynEnumItem*)b)->value) rval=1;
    else if(((DynEnumItem*)a)->value == ((DynEnumItem*)b)->value) rval=0; return rval; }

  override bool         BrowserSelectMe();
  override bool         BrowserExpandAll();
  override bool         BrowserCollapseAll();

  TA_BASEFUNS_NOCOPY(DynEnumItem_List);
private:
  void  Initialize();
  void  Destroy() {Reset();}
};

#endif // DynEnumItem_List_h
