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

#ifndef DynMethod_PtrList_h
#define DynMethod_PtrList_h 1

// parent includes:
#include <taPtrList>

// member includes:
#include <ISelectable>

// declare all other types mentioned but not required to include:
class MethodDef;

class TA_API DynMethodDesc { // #IGNORE
friend class DynMethod_PtrList;
public:
  int                   idx; // list index, used as param in the action callback routine
  int                   dmd_type;
  MethodDef*            md;
protected:
  DynMethodDesc() {}
  ~DynMethodDesc() {}
};


class TA_API DynMethod_PtrList: public taPtrList<DynMethodDesc> { // #IGNORE
typedef taPtrList<DynMethodDesc> inherited;
public:
  enum DMDType {
    Type_1N,    // <methname>() method; applied to all selected object
    Type_1_2N,  // <methname>(Type2N* param) method; called on object 1 for objects 2:N
    Type_2N_1,  // <methname>(Type1* param) method; called on objects 2:N for object 1
    Type_MimeN_N // <methname>(Type1* param) method; called on objects 1:N for mime source object(s) of common subtype Type1
  };

  DynMethodDesc*        AddNew(int dmd_type, MethodDef* md); // creates new DMD and adds, returning ref
  void                  Fill(ISelectable_PtrList& sel_items,
    ISelectable::GuiContext gc_typ); // clear, then fill based on sel_items
  void                  FillForDrop(const taiMimeSource& ms,
    ISelectable* drop_item); // clear, then fill based on ms and sel_items (used for Drop operations)
  ~DynMethod_PtrList();
protected:
  override void         El_Done_(void* it) {delete (DynMethodDesc*)it;}
  override void         El_SetIndex_(void* it, int idx) {((DynMethodDesc*)it)->idx = idx; };
  // sets the element's self-index
};

#endif // DynMethod_PtrList_h
