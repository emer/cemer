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

#include "iBrowseHistory.h"
#include <ISelectableHost>
#include <iTreeView>
#include <iTreeViewItem>

#include <taMisc>



iBrowseHistory::iBrowseHistory(QObject* parent)
  : inherited(parent)
{
  max_items = taMisc::num_browse_history;
  if (max_items < 10) max_items = 10; // sanity
  cur_item = -1;
}

iBrowseHistory::~iBrowseHistory() {
  reset();
}

void iBrowseHistory::addItem(taiDataLink* dl) {
  if (items.size >= max_items) {
    taiDataLink* it = items[0];
    items.RemoveIdx(0);
    itemRemoved(it);
  }
  itemAdding(dl);
  items.Add(dl);
  cur_item = items.size - 1;
  doEnabling();
}

void iBrowseHistory::back() {
  if (cur_item <= 0) goto exit;
  --cur_item;
  if (cur_item >= items.size) goto exit;
  ++navigating; {
    taiDataLink* dl = items[cur_item];
    emit select_item(dl);
  } --navigating;

exit:
  doEnabling();
}

void iBrowseHistory::forward() {
  if (cur_item >= items.size) goto exit;
  ++cur_item;
  if (cur_item >= items.size) goto exit;
  ++navigating; {
    taiDataLink* dl = items[cur_item];
    emit select_item(dl);
  } --navigating;
exit:
  doEnabling();
}

void iBrowseHistory::doEnabling() {
  bool be = ((cur_item > 0) && (items.size > 1));
  emit back_enabled(be);
  bool fe = ((cur_item < (items.size - 1)) && (items.size > 1));
  emit forward_enabled(fe);
}

void iBrowseHistory::DataLinkDestroying(taDataLink* dl) {
  for (int i = items.size - 1; i >= 0; --i) {
    if (dl == items[i]) {
      if (cur_item > i) -- cur_item;
      else if (cur_item == i) cur_item = items.size; // adjust later
      items.RemoveIdx(i);
    }
  }
  doEnabling();
}

void iBrowseHistory::itemAdding(taiDataLink* dl) {
  for (int i = 0; i < items.size; ++i) {
    if (dl == items[i]) return;
  }
  dl->AddDataClient(this);
}

void iBrowseHistory::itemRemoved(taiDataLink* dl) {
  for (int i = 0; i < items.size; ++i) {
    if (dl == items[i]) return;
  }
  dl->RemoveDataClient(this);
}

void iBrowseHistory::SelectableHostNotifying(ISelectableHost* src_host, int op)
{
  if (navigating) return;
  if (op != ISelectableHost::OP_SELECTION_CHANGED) return;
  iTreeView* tvw = qobject_cast<iTreeView*>(src_host->widget());
  if (!tvw) return;
  iTreeViewItem* tvi = dynamic_cast<iTreeViewItem*>(tvw->curItem());
  if (!tvi) return;
  ItemSelected(tvi);
}

void iBrowseHistory::ItemSelected(iTreeViewItem* tvi) {
  if (navigating) return;
  taiDataLink* dl = tvi->link();
  if (!dl) return;
  addItem(dl);
}

void iBrowseHistory::reset() {
  for (int i = items.size - 1; i >= 0; --i) {
    taiDataLink* dl = items[i];
    items.RemoveIdx(i);
    itemRemoved(dl);
  }
  cur_item = 0;
  //enabled?
}

