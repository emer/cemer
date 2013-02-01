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

#include "iListDataPanel.h"
#include <iTreeView>
#include <taiListDataNode>

#include <DataChangedReason>
#include <taMisc>
#include <taiMisc>



iListDataPanel::iListDataPanel(taiSigLink* dl_, const String& custom_name_)
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
}

iListDataPanel::~iListDataPanel() {
}

QWidget* iListDataPanel::firstTabFocusWidget() {
  return list;
}

void iListDataPanel::ClearList() {
  list->clear();
}

void iListDataPanel::ConfigHeader() {
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

void iListDataPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  if (dcr == DCR_LIST_ITEM_REMOVE) {
    // index will now be invalid for followers
    RenumberList();
  }
  // we handle the cases separately, since just refilling the list
  else if (dcr == DCR_LIST_ITEM_INSERT) {
    // insert at end, regardless of sort order
    taiSigLink* item = link()->GetListChild(op1_);
    if (!item) {
      taMisc::Warning("iListDataPanel::DataChanged_impl: unexpected could not find new list item");
      return;
    }
    taiListDataNode* last_child = dynamic_cast<taiListDataNode*>(
      list->item(list->itemCount() - 1));
    /*taiListDataNode* dn = */new taiListDataNode(-1, this, item, list,
      last_child, (iTreeViewItem::DNF_CAN_DRAG)); // numbered/decorated in Renumber call
    RenumberList();
  }
  // note: remember, we already handled insert and remove
  else if ((dcr >= DCR_LIST_MIN) && (dcr <= DCR_LIST_MAX)) {
    // for other list ops, esp sort or move, just reorder whole list (easy, harmless)
    RenumberList();
  }
  else if (dcr == DCR_STRUCT_UPDATE_END) {
    ConfigHeader();
  }
}

void iListDataPanel::FillList() {
  if (!link()) return; // zombie
  taiListDataNode* last_child = NULL;
  int i = 0;
  while (true) { // break when NULL child encountered
    taiSigLink* child = link()->GetListChild(i);
    if (!child) break;
    taiListDataNode* dn = new taiListDataNode(i, this, child, list,
      last_child, (iTreeViewItem::DNF_CAN_DRAG));
    dn->DecorateDataNode(); // fills in remaining columns
    last_child = dn;
    ++i;
  }
}

void iListDataPanel::RenumberList() {
  int i = 0;
  // mark all the items first, because this is the easiest, safest way/place
  // to remove items that are stale, ex. moved from our list to another list
  for (int j = 0; j < list->itemCount(); ++j) {
    taiListDataNode* dn = dynamic_cast<taiListDataNode*>(
      list->item(j));
    dn->num = -1;
  }

  // we have to iterate in proper link order, then find child, since items maybe
  // be sorted by some other column now
  for (taiSigLink* child; (child = link()->GetListChild(i)); ++i) { //iterate until no more
    if (!child) break;
    // find the item for the link
    for (int j = 0; j < list->itemCount(); ++j) {
      taiListDataNode* dn = dynamic_cast<taiListDataNode*>(
        list->item(j));
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
    taiListDataNode* dn = dynamic_cast<taiListDataNode*>(
      list->item(j));
    if (dn && (dn->num == -1)) delete dn;
  }
}

void iListDataPanel::list_itemDoubleClicked(QTreeWidgetItem* item_, int /*col*/) {
  taiListDataNode* item = dynamic_cast<taiListDataNode*>(item_);
  if (!item) return;
  taBase* ta = item->taData(); // null if n/a
  if (ta) {
//     tabMisc::DelayedFunCall_gui(ta, "BrowserSelectMe");
//    ta->BrowserSelectMe();
    // neither of the above actually update the panel view to new item -- presumably because
    // it is still in use or something..
    //    ta->EditPanel(true, false);   // new non-pinned panel -- leads to a proliferation
    // of panels and doesn't make a lot of sense.
    ta->EditDialog();           // pop up the edit dialog -- not favored, but probably
                                // the best thing for this situation
  }
}

void iListDataPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // connect the list up to the panel
  list->Connect_SelectableHostNotifySignal(itv,
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
}

String iListDataPanel::panel_type() const {
  static String str("List View");
  if (m_custom_name.nonempty())
    return m_custom_name;
  else return str;
}

void iListDataPanel::UpdatePanel_impl() {
  //NOTE: when reshowing from invisible, we need to do this full refresh
  //obs list->Refresh();
  ClearList();
  FillList();
  inherited::UpdatePanel_impl();
}

