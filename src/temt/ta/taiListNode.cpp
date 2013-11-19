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

#include "taiListNode.h"
#include <iPanelOfList>
#include <taiWidgetMenu>


taiListNode::taiListNode(int num_, iPanelOfList* panel_,
   taiSigLink* link_, iTreeView* parent_, taiListNode* after, int dn_flags_)
:inherited(link_, NULL, parent_, after, String(num_), (dn_flags_ | DNF_IS_LIST_NODE))
{
  num = num_;
  panel = panel_;
  setData(0, Qt::TextAlignmentRole, Qt::AlignRight);
}

taiListNode::~taiListNode() {
}


bool taiListNode::operator<(const QTreeWidgetItem& item) const
{ //NOTE: it was tried to set display data as an int QVariant, but sorting was still lexographic
  QTreeWidget* tw = treeWidget();
  if (!tw) return false; // shouldn't happen
  int col = tw->sortColumn();
  if (col > 0)
    return inherited::operator<(item);
  else {
    taiListNode* ldn = (taiListNode*)&item;
    return (num < ldn->num);
  }
}

void taiListNode::DecorateDataNode() {
  inherited::DecorateDataNode();
  setText(0, String(num)); // in case changed via renumber
}

taiSigLink* taiListNode::par_link() const {
  // in case we decide to support trees in list views, check for an item parent:
  taiSigLink* rval = inherited::par_link();
  if (rval) return rval;
  if (panel) return panel->link();
  return NULL;
}

void taiListNode::setName(const String& value) {
  if (columnCount() >= 2) // s/always be true!
    this->setText(1, value);
}

QString taiListNode::text(int col) const {
  if (col > 0)
    return inherited::text(col);
  else
    return QString::number(num);
}

void taiListNode::FillContextMenu_impl(taiWidgetActions* menu,
                                       GuiContext sh_typ) {
  inherited::FillContextMenu_impl(menu, sh_typ);
}


