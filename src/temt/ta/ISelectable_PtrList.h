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

#ifndef ISelectable_PtrList_h
#define ISelectable_PtrList_h 1

// parent includes:
#include <ISelectable>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 


class TA_API ISelectable_PtrList: public taPtrList<ISelectable> { // for selection lists
INHERITED(taPtrList<ISelectable>)
friend class ISelectableHost;
public:
  TypeDef*      Type1(ISelectable::GuiContext gc_typ = ISelectable::GC_DEFAULT);
    // data type of 1st item
  TypeDef*      CommonSubtype1N(ISelectable::GuiContext gc_typ = ISelectable::GC_DEFAULT);
    // greatest common data subtype of items 1-N
  TypeDef*      CommonSubtype2N(ISelectable::GuiContext gc_typ = ISelectable::GC_DEFAULT);
    // greatest common data subtype of items 2-N

  ISelectable_PtrList() {Initialize();}
  ISelectable_PtrList(const ISelectable_PtrList& cp);
  ~ISelectable_PtrList();
protected:
  static taPtrList_impl* insts; // global lists of all insts, to enable removing deleting items
private:
  void  Initialize();
};

#endif // ISelectable_PtrList_h
