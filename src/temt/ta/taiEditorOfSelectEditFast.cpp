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

#include "taiEditorOfSelectEditFast.h"
#include <SelectEdit>
#include <taiWidgetDelegateSelectEdit>
#include <iTableWidget>
#include <iFlowLayout>
#include <taiWidgetMenuBar>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QHeaderView>

taiEditorOfSelectEditFast::taiEditorOfSelectEditFast(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, parent)
{
  Initialize();
  sele = (SelectEdit*)base;
}

taiEditorOfSelectEditFast::~taiEditorOfSelectEditFast() {
  delete sed; // hope it's ref is deleted too!
  sed = NULL;
}

void taiEditorOfSelectEditFast::Initialize()
{
  tw = NULL;
  sele = NULL;
  sel_edit_mbrs = true; // note: we don't actually select members, just for removal
  sed = new taiWidgetDelegateSelectEdit(sele, this);
}

void taiEditorOfSelectEditFast::Constr_Body_impl() {
}

void taiEditorOfSelectEditFast::Constr_Box() {
  row_height = taiM->max_control_height(ctrl_size); // 3 if using line between; 2 if
  if (tw) return;
  tw = new iTableWidget(widget());
  tw->setColumnCount(2);
  tw->horizontalHeader()->setVisible(false);
  tw->horizontalHeader()->setStretchLastSection(true); // note: works if header invis
#if (QT_VERSION >= 0x050000)
  tw->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  tw->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
  tw->verticalHeader()->setVisible(false);
#if (QT_VERSION >= 0x050000)
  tw->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  tw->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
  tw->setSortingEnabled(false);
  // don't try to scroll by item, it looks ugly, just scroll smoothly
  tw->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  // colors
  QPalette palette(tw->palette());
  palette.setColor(QPalette::Base, bg_color);
  palette.setColor(QPalette::AlternateBase, bg_color_dark);
  tw->setPalette(palette);
  tw->setAlternatingRowColors(true);
  tw->setContextMenuPolicy(Qt::CustomContextMenu);

  tw->setItemDelegateForColumn(1, sed);
  vblDialog->addWidget(tw, 1);
  connect(tw, SIGNAL(currentCellChanged(int, int, int, int)),
    this, SLOT(tw_currentCellChanged(int, int, int, int)) );
  connect(tw, SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(tw_customContextMenuRequested(const QPoint&)) );
  body = tw;
}


void taiEditorOfSelectEditFast::ClearBody_impl() {
  // we also clear all the methods, and then rebuild them
  ta_menus.Reset();
  ta_menu_buttons.Reset();
//  meth_el.Reset(); // must defer deletion of these, because the MethodWidget objects are used in menu calls, so can't be
  // somehow separate..
  if(frmMethButtons) {
    if(layMethButtons && (layMethButtons != (iFlowLayout*)frmMethButtons->layout())) {
      delete layMethButtons;
    }
    if(frmMethButtons->layout()) {
      delete frmMethButtons->layout();
    }
    layMethButtons = NULL;
    taiMisc::DeleteChildrenLater(frmMethButtons);
  }
  show_meth_buttons = false;

  // note: no show menu in this class
  cur_menu = NULL;
  cur_menu_but = NULL;
  if (menu) {
    menu->Reset();
  }
//??  inherited::ClearBody_impl();
}

void taiEditorOfSelectEditFast::Constr_Widget_Labels() {
  // delete all previous sele members
  membs.ResetItems();
  tw->clearSpans();
  tw->clear();
  // mark place
  String nm;
  String help_text;
  taiMemberWidgets* ms = NULL;

  int set_idx = 0;
  int row = 0;
  // note: iterates non-empty groups only
  FOREACH_SUBGROUP(EditMbrItem_Group, grp, sele->mbrs) {
    bool def_grp = (grp == &(sele->mbrs));// root group
    membs.SetMinSize(set_idx + 1);
    ms = membs.FastEl(set_idx);
    // make a group header
    if (!def_grp) {
      tw->setRowCount(row+1);
      int item_flags = Qt::ItemIsEnabled; //Qt::ItemIsUserCheckable;
      ms->text = grp->GetName();
      ms->show = true;
      QTableWidgetItem* twi = new QTableWidgetItem;
      twi->setText(ms->text);
      twi->setFlags((Qt::ItemFlags)item_flags);
      //TODO: Bold
      QFont f(tw->font());
      f.setBold(true);
      twi->setFont(f);
      tw->setItem(row, 0, twi);
      tw->setSpan(row, 0, 1, 2);
      tw->setRowHeight(row, row_height);
      ++row;
    }
    tw->setRowCount(row + grp->size);
    for (int i = 0; i < grp->size; ++i) {
      int item_flags = 0;
      EditMbrItem* item = grp->FastEl(i);
      MemberDef* md = item->mbr;
      if (!md || (md->im == NULL)) continue; // shouldn't happen

      ms->memb_el.Add(md);
      // force span, in case this row had a group before
      // this is now giving an error in 4.6.0 beta -- unnec?
      // but it needs the call in 4.5.x -- otherwise row is blank.
#if (QT_VERSION < 0x040600)
      tw->setSpan(row, 0, 1, 1);
#endif
      // label item
      QTableWidgetItem* twi = new QTableWidgetItem;
      twi->setText(item->caption());
      twi->setStatusTip(item->GetDesc());
      twi->setToolTip(item->GetDesc());
      item_flags = Qt::ItemIsEnabled;
      twi->setFlags((Qt::ItemFlags)item_flags);
      tw->setItem(row, 0, twi);

      // data item
      twi = new QTableWidgetItem;
      item_flags = Qt::ItemIsEnabled; // |Qt::ItemIsSelectable;
      //TODO: check for READONLY
      bool ro = false;
      if (!ro)
        item_flags |= Qt::ItemIsEditable;
      twi->setFlags((Qt::ItemFlags)item_flags);
      // set EditMbrItem into 1.data
      twi->setData(Qt::UserRole, QVariant((ta_intptr_t)item));

      tw->setItem(row, 1, twi);
      tw->setRowHeight(row, row_height);
      ++row;
    }
    def_grp = false;
    ++set_idx;
  }
  tw->resizeColumnToContents(0); // note: don't do 1 here, do it in GetImage
}

void taiEditorOfSelectEditFast::GetImage_Item(int row) {
  if (row < 0) return;
  QTableWidgetItem* it = tw->item(row, 1);
  QTableWidgetItem* lbl = tw->item(row, 0); // not always needed
  if (!it) return;
  EditMbrItem* item = dynamic_cast<EditMbrItem*>(
    (taBase*)(it->data(Qt::UserRole).value<ta_intptr_t>()));
  if ((item == NULL) || (item->base == NULL) ||  (item->mbr == NULL)) return;
  if(!item->base->InheritsFrom((TypeDef*)item->mbr->owner->owner)) {
    taMisc::Warning("type mismatch in select edit", item->label, "-- should be removed asap",
                    ((TypeDef*)item->mbr->owner->owner)->name, "!=",
                    item->base->GetTypeDef()->name);
    return;
  }
  void* off = item->mbr->GetOff(item->base);
  String txt = item->mbr->type->GetValStr(off, item->base,
                                          item->mbr, TypeDef::SC_DISPLAY, true);
  if(item->is_numeric) {
    txt += "&nbsp;&nbsp;|&nbsp;&nbsp;" + TA_EditParamSearch.GetValStr(&(item->param_search), NULL, NULL, TypeDef::SC_DISPLAY, true);
  }

  // true = force inline
  // augment plain non-class vals with bg color
  if(!txt.contains("<font style=\"background-color:")) {
    if(item->mbr->type->IsEnum() ||      
       item->mbr->type->DerivesFrom(TA_taSmartPtr) ||
       item->mbr->type->DerivesFrom(TA_taSmartRef) ||
       item->mbr->type->IsAnyPtr())
      txt = "<font style=\"background-color: LightGrey\">&nbsp;&nbsp;" + txt + "&nbsp;&nbsp;</font>";
    else
      txt = "<font style=\"background-color: white\">&nbsp;&nbsp;" + txt + "&nbsp;&nbsp;</font>";
  }

  it->setText(txt);
  it->setToolTip(txt); // for when over

  // default highlighting
  switch (item->mbr->GetDefaultStatus(item->base)) {
  case MemberDef::NOT_DEF:
    lbl->setData(Qt::BackgroundRole, QColor(Qt::yellow));
    break;
  case MemberDef::EQU_DEF:
    //note: setting nil Variant will force it to ignore and use bg
    lbl->setData(Qt::BackgroundRole, QVariant());
    break;
  default: break; // compiler food
  }
}

void taiEditorOfSelectEditFast::GetImage_Membs_def() {
  for (int row = 0; row < tw->rowCount(); ++row) {
    GetImage_Item(row);
  }
  sed->GetImage(); // if a ctrl is active
  // note: this is taking the bulk of the compute time for refreshing during running:
  // it seems fine to leave it off for now -- once you click on something it resizes
  // and that seems good enough..
  //  tw->resizeColumnToContents(1);
}

void taiEditorOfSelectEditFast::DoRemoveSelEdit() {
   // removes the sel_item_index item
  int sel_item_index = -1;
  sele->mbrs.FindItemBase(sel_item_base, sel_item_mbr, sel_item_index);
  if (sel_item_index >= 0) {
    sele->RemoveField(sel_item_index);
  }
  else
    taMisc::DebugInfo("taiEditorOfSelectEditFull::DoRemoveSelEdit: could not find item");
}

void taiEditorOfSelectEditFast::FillLabelContextMenu_SelEdit(QMenu* menu,
  int& last_id)
{
  int sel_item_index = -1;
  sele->mbrs.FindItemBase(sel_item_base, sel_item_mbr, sel_item_index);
  if (sel_item_index < 0) return;
  //QAction* act =
  menu->addAction("Remove from SelectEdit", this, SLOT(DoRemoveSelEdit()));
}

void taiEditorOfSelectEditFast::GetValue_Membs_def() {
  sed->GetValue();
}

void taiEditorOfSelectEditFast::tw_currentCellChanged(int row,
    int col, int previousRow, int previousColumn)
{
  if ((row < 0) || (col < 1)) return;
  QTableWidgetItem* twi = tw->item(row, col);
  if (!twi) return;
  tw->editItem(twi);
}

void taiEditorOfSelectEditFast::tw_customContextMenuRequested(const QPoint& pos)
{
  int row = tw->rowAt(pos.y());
  int col = tw->columnAt(pos.x());
  if ((row < 0) || (col != 0)) return;
  // we want the data item for the label, to get its goodies...
  QTableWidgetItem* twi = tw->item(row, 1);
  if (!twi) return; // ex. right clicking on a section

  EditMbrItem* item = dynamic_cast<EditMbrItem*>(
    (taBase*)(twi->data(Qt::UserRole).value<ta_intptr_t>()));
  if ((item == NULL) || (item->base == NULL)) return;

  sel_item_mbr = item->mbr;
  sel_item_base = item->base;
  QMenu* menu = new QMenu(widget());
  int last_id = -1;
  FillLabelContextMenu(menu, last_id);
  if (menu->actions().count() > 0)
    menu->exec(tw->mapToGlobal(pos));
  delete menu;

}


