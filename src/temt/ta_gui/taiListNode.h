// Copyright 2013-2017, Regents of the University of Colorado,
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
  int                 num; // item number, starting from 0
  iPanelOfList*       panel; // logical parent node of the list items

  QString               text(int col) const;
  // note: this is a non-virtual inline in orig: return data(column, Qt::DisplayRole).toString(); -- if we want to redefine behavior in base, need to change data
  void                  setName(const String& value) override; // 2nd col for us

  bool                  operator<(const QTreeWidgetItem& item) const override;

  void         DecorateDataNode() override;
  taiListNode(int num_, iPanelOfList* panel_, taiSigLink* link_,
    iTreeView* parent_, taiListNode* after, int dn_flags_ = 0);
    //note: list flag automatically or'ed in
  ~taiListNode(); //

public: // ISigLinkClient interface
//  void*      This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_taiListNode;}

public: // ISelectable interface
  taiSigLink* par_link() const override; // we get from the panel, which gets from the viewer window
//obs  MemberDef*      par_md() const override; // as for par_link
protected:
  void         FillContextMenu_impl(taiWidgetActions* menu, GuiContext sh_typ) override;
  // this is the one to extend in inherited classes
};

#endif // taiListNode_h
