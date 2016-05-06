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

#include "iPanelOfList.h"
#include <iPanelSet>
#include <iTreeView>
#include <taiListNode>
#include <SigLinkSignal>
#include <taMisc>
#include <taiMisc>

#include <QApplication>

taTypeDef_Of(iPanelOfDataTable);


iPanelOfList::iPanelOfList(taiSigLink* dl_, const String& custom_name_)
:inherited(dl_)
{
  m_custom_name = custom_name_; // optional
  // note: just autoexpands the first fill, user must adjust after that
  list = new iTreeView(this, iTreeView::TV_AUTO_EXPAND);
  setCentralWidget(list);
  list->setSelectionMode(QTreeWidget::ExtendedSelection);
  list->setSortingEnabled(true);
  //enable dnd support, at least as source
  list->setDragEnabled(true);
//  list->setAcceptDrops(true);
//  list->setDropIndicatorShown(true);
  connect(list, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
    this, SLOT(list_itemDoubleClicked(QTreeWidgetItem*, int)) );
  ConfigHeader();
  // set default sort order to ASC -- don't know why def is DESC!
  list->sortItems(0, Qt::AscendingOrder);
  FillList();
  list->resizeColumnToContents(1);  // the second column is the key column - resize the first time - then user has control

}

iPanelOfList::~iPanelOfList() {
}

QWidget* iPanelOfList::firstTabFocusWidget() {
  return list;
}

void iPanelOfList::ClearList() {
  list->clear();
}

void iPanelOfList::ConfigHeader() {
  // set up number of cols, based on link, ok to repeat this
  list->setColumnCount(link()->NumListCols() + 1);
  QTreeWidgetItem* hdr = list->headerItem();
  hdr->setText(0, "#"); //note: we don't need a key, because we manage the text ourself
  for (int i = 0; i < link()->NumListCols(); ++i) {
    int hdr_idx = i + 1;
    KeyString key = link()->GetListColKey(i);
    hdr->setText(hdr_idx, link()->GetColHeading(key));
    hdr->setData(hdr_idx, iTreeView::ColKeyRole, key);
  }
}

void iPanelOfList::SigEmit_impl(int sls, void* op1_, void* op2_) {
  inherited::SigEmit_impl(sls, op1_, op2_);
  if (sls == SLS_LIST_ITEM_REMOVE) {
    // index will now be invalid for followers
    RenumberList();
  }
  // we handle the cases separately, since just refilling the list
  else if (sls == SLS_LIST_ITEM_INSERT) {
    // insert at end, regardless of sort order
    taiSigLink* item = link()->GetListChild(op1_);
    if (!item) {
      taMisc::Warning("iPanelOfList::SigEmit_impl: unexpected could not find new list item");
      return;
    }
    taiListNode* last_child = dynamic_cast<taiListNode*>(
      list->item(list->itemCount() - 1));
    /*taiListNode* dn = */new taiListNode(-1, this, item, list,
      last_child, (iTreeViewItem::DNF_CAN_DRAG)); // numbered/decorated in Renumber call
    RenumberList();
  }
  // note: remember, we already handled insert and remove
  else if ((sls >= SLS_LIST_MIN) && (sls <= SLS_LIST_MAX)) {
    // for other list ops, esp sort or move, just reorder whole list (easy, harmless)
    RenumberList();
  }
  else if (sls == SLS_STRUCT_UPDATE_END) {
    ConfigHeader();
  }
}

void iPanelOfList::FillList() {
  if (!link()) return; // zombie
  taiListNode* last_child = NULL;
  int i = 0;
  while (true) { // break when NULL child encountered
    taiSigLink* child = link()->GetListChild(i);
    if (!child) break;
    taiListNode* dn = new taiListNode(i, this, child, list,
                                      last_child, (iTreeViewItem::DNF_CAN_DRAG));
    dn->DecorateDataNode(); // fills in remaining columns
    last_child = dn;
    ++i;
  }
}

void iPanelOfList::RenumberList() {
  int i = 0;
  // mark all the items first, because this is the easiest, safest way/place
  // to remove items that are stale, ex. moved from our list to another list
  for (int j = 0; j < list->itemCount(); ++j) {
    taiListNode* dn = dynamic_cast<taiListNode*>(list->item(j));
    if(dn)
      dn->num = -1;
  }

  // we have to iterate in proper link order, then find child, since items maybe
  // be sorted by some other column now
  for (taiSigLink* child; (child = link()->GetListChild(i)); ++i) { //iterate until no more
    // find the item for the link
    for (int j = 0; j < list->itemCount(); ++j) {
      taiListNode* dn = dynamic_cast<taiListNode*>(list->item(j));
      if (dn && (dn->link() == child)) {
        dn->num = i;
        dn->DecorateDataNode(); // fills in remaining columns
        break;
      }
    }
  }

  // now delete stales -- note: an item that is deleting would have deleted
  // its node, but doing so now is harmless
  for (int j = list->itemCount() - 1; j >=0; --j) {
    taiListNode* dn = dynamic_cast<taiListNode*>(list->item(j));
    if (dn && (dn->num == -1)) delete dn;
  }
}

void iPanelOfList::list_itemDoubleClicked(QTreeWidgetItem* item_, int /*col*/) {
  taiListNode* item = dynamic_cast<taiListNode*>(item_);
  if (!item)
    return;
  taBase* ta = item->taData(); // null if n/a
  if (ta) {  // switch to table view and select the same column
    if(QApplication::keyboardModifiers() & Qt::ControlModifier) {  // command key on mac
      data_panel_set()->SetPanelOfDataTable(item->num);  //  pass the column num
    }
    else {
      ta->OpenInWindow(); // (jar - what does this mean?) pop up the edit dialog -- not favored, but probably the best thing for this situation
    }
  }
}

void iPanelOfList::OnWindowBind_impl(iPanelViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // connect the list up to the panel
  list->Connect_SelectableHostNotifySignal(itv, SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
}

String iPanelOfList::panel_type() const {
  static String str("List View");
  if (m_custom_name.nonempty())
    return m_custom_name;
  else return str;
}

void iPanelOfList::UpdatePanel_impl() {
  //NOTE: when reshowing from invisible, we need to do this full refresh
  //obs list->Refresh();
  ClearList();
  FillList();
  inherited::UpdatePanel_impl();
}

