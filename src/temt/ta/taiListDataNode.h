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

#ifndef taiListDataNode_h
#define taiListDataNode_h 1

// parent includes:
#include <iTreeViewItem>

// member includes:

// declare all other types mentioned but not required to include:
class iListDataPanel; //
class iTreeView; //


class TA_API taiListDataNode: public iTreeViewItem {
INHERITED(iTreeViewItem)
public:
  int                   num; // item number, starting from 0
  iListDataPanel*       panel; // logical parent node of the list items

  QString               text(int col) const; // override
  override void         setName(const String& value); // 2nd col for us

  bool                  operator<(const QTreeWidgetItem& item) const; // override

  override void         DecorateDataNode();
  taiListDataNode(int num_, iListDataPanel* panel_, taiDataLink* link_,
    iTreeView* parent_, taiListDataNode* after, int dn_flags_ = 0);
    //note: list flag automatically or'ed in
  ~taiListDataNode(); //

public: // IDataLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_taiListDataNode;}

public: // ISelectable interface
  override taiDataLink* par_link() const; // we get from the panel, which gets from the viewer window
//obs  override MemberDef*      par_md() const; // as for par_link
};

#endif // taiListDataNode_h
