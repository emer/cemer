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

#ifndef IObjectSelectable_h
#define IObjectSelectable_h 1

// parent includes:
#include <ISelectable>

// member includes:

// declare all other types mentioned but not required to include:
class taiClipData; // #IGNORE
class ISelectable_PtrList; // 


class TA_API IObjectSelectable: public ISelectable { // specialized for taBase object items
INHERITED(ISelectable)
public: // Interface Properties and Methods
  override taiClipData* GetClipDataSingle(int src_edit_action, bool for_drag,
    GuiContext sh_typ = GC_DEFAULT) const;
  override taiClipData* GetClipDataMulti(const ISelectable_PtrList& sel_items,
    int src_edit_action, bool for_drag, GuiContext sh_typ = GC_DEFAULT) const;
protected:
  override int          EditActionD_impl_(taiMimeSource* ms, int ea,
    GuiContext sh_typ);
  override int          EditActionS_impl_(int ea, GuiContext sh_typ);
  override void         QueryEditActionsD_impl_(taiMimeSource* ms, int& allowed,
    int& forbidden, GuiContext sh_typ) const;
  override void         QueryEditActionsS_impl_(int& allowed, int& forbidden,
    GuiContext sh_typ) const;
};

#endif // IObjectSelectable_h
