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

#ifndef taiListNode_h
#define taiListNode_h 1

// parent includes:
#include <iTreeViewItem>

// member includes:

// declare all other types mentioned but not required to include:
class iPanelOfList; //
class iTreeView; //
class QTreeWidgetItem; //

taTypeDef_Of(taiListNode);

class TA_API taiListNode: public iTreeViewItem {
INHERITED(iTreeViewItem)
public:
  int                   num; // item number, starting from 0
  iPanelOfList*       panel; // logical parent node of the list items

  QString               text(int col) const; // override
  override void         setName(const String& value); // 2nd col for us

  bool                  operator<(const QTreeWidgetItem& item) const; // override

  override void         DecorateDataNode();
  taiListNode(int num_, iPanelOfList* panel_, taiSigLink* link_,
    iTreeView* parent_, taiListNode* after, int dn_flags_ = 0);
    //note: list flag automatically or'ed in
  ~taiListNode(); //

public: // ISigLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_taiListNode;}

public: // ISelectable interface
  override taiSigLink* par_link() const; // we get from the panel, which gets from the viewer window
//obs  override MemberDef*      par_md() const; // as for par_link
protected:
  override void         FillContextMenu_impl(taiWidgetActions* menu, GuiContext sh_typ);
  // this is the one to extend in inherited classes
};

#endif // taiListNode_h
