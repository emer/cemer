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

#ifndef DataView_List_h
#define DataView_List_h 1

// parent includes:
#include <taDataView>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataView_List);

class TA_API DataView_List: public taList<taDataView> {
  // #NO_TOKENS ##CAT_Display
INHERITED(taList<taDataView>)
public:
  void         SigEmit(int sls, void* op1 = NULL, void* op2 = NULL) CPP11_OVERRIDE;
    // we send to an owner DataView SigEmit_Child

  virtual void          DoAction(taDataView::DataViewAction act);
   // do a single action on all items; we also do self->Reset on Reset_impl

  taBase* SetOwner(taBase*) CPP11_OVERRIDE; // #IGNORE
  TA_DATAVIEWLISTFUNS(DataView_List, taList<taDataView>, taDataView) //

protected:
  taDataView*           data_view; // #IGNORE our owner, when owned by a taDataView, for efficiency

  void*        El_Own_(void* it) CPP11_OVERRIDE;
  void         El_disOwn_(void* it) CPP11_OVERRIDE;
  void         ChildQueryEditActionsL_impl(const MemberDef* md,
    const taBase* lst_itm, const taiMimeSource* ms,
    int& allowed, int& forbidden) CPP11_OVERRIDE; // also forwards to dv owner; in ta_qtclipdata.cpp
  virtual void          DV_ChildQueryEditActionsL_impl(const MemberDef* md,
    const taBase* lst_itm, const taiMimeSource* ms,
    int& allowed, int& forbidden); // specialized guys for DV -- can be replaced

private:
  NOCOPY(DataView_List)
  void  Initialize() { SetBaseType(&TA_taDataView); data_view = NULL;}
  void  Destroy() {}
};

#endif // DataView_List_h
