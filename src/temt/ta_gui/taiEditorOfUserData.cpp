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

#include "taiEditorOfUserData.h"
#include <taiWidgetDelegateUserData>
#include <UserDataItem>
#include <UserDataItem_List>
#include <MemberDef>

#include <SigLinkSignal>
#include <taMisc>
#include <taiMisc>

#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>



taiEditorOfUserData::taiEditorOfUserData(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, false, parent)
{
  Initialize();
  udil = (UserDataItem_List*)base;
}

taiEditorOfUserData::~taiEditorOfUserData() {
  delete udd; // hope it's ref is deleted too!
  udd = NULL;
}

void taiEditorOfUserData::Initialize()
{
//  no_meth_menu = true; // only show them on outer menu, by way of Propertiesguy
  udil = NULL;
  sel_edit_mbrs = true; // note: we don't actually select members, just for removal
  tw = NULL;
  udd = new taiWidgetDelegateUserData(udil, this);
  sel_item_row = -1;
}

void taiEditorOfUserData::Constr_impl() {
  inherited::Constr_impl();
  // we actually want methods...
  Constr_Methods();
  frmMethButtons->setHidden(!showMethButtons());
}

void taiEditorOfUserData::Constr_Body_impl() {
}

void taiEditorOfUserData::Constr_Box() {
  row_height = taiM->max_control_height(ctrl_size); // 3 if using line between; 2 if
  if (tw) return;
  tw = new QTableWidget(widget());
  tw->setColumnCount(2);
  tw->horizontalHeader()->setVisible(false);
  tw->horizontalHeader()->setStretchLastSection(true); // note: works if header invis
  tw->verticalHeader()->setVisible(false);
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

  tw->setItemDelegateForColumn(1, udd);
  vblDialog->addWidget(tw, 1);
  connect(tw, SIGNAL(currentCellChanged(int, int, int, int)),
    this, SLOT(tw_currentCellChanged(int, int, int, int)) );
  connect(tw, SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(tw_customContextMenuRequested(const QPoint&)) );
  connect(tw, SIGNAL(itemChanged(QTableWidgetItem*)),
    this, SLOT(tw_itemChanged(QTableWidgetItem*)) );
  body = tw;
}


void taiEditorOfUserData::ClearBody_impl() {
  // note: we don't nuke control, we just clear it
  tw->clear();
}

void taiEditorOfUserData::Constr_Widget_Labels() {
  // delete all previous udil members
  membs.ResetItems();
  tw->clearSpans();
  tw->clear();
  // mark place
  String nm;
  String help_text;
  taiMemberWidgets* ms = NULL;

  int set_idx = 0;
  int row = 0;
  // iterates all non-empty groups...
  FOREACH_SUBGROUP(UserDataItem_List, grp, *udil) {
    bool def_grp = (grp == udil);// root group
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

    for (int i = 0; i < grp->size; ++i) {
      int item_flags = 0;
      UserDataItemBase* item_ = grp->FastEl(i);
      // normally don't show invisible guys, unless HIDDEN is selected
      if (!item_->isVisible() && (show() & TypeItem::NO_HIDDEN)) continue;

      tw->setRowCount(row + 1);
      QTableWidgetItem* twi = NULL;

      // force span, in case this row had a group before
      // this is now giving an error in 4.6.0 beta -- unnec?
      // but it needs the call in 4.5.x -- otherwise row is blank.
#if (QT_VERSION < 0x040600)
      tw->setSpan(row, 0, 1, 1);
#endif
      //TODO: Modal, based on type
      // data item
      twi = new QTableWidgetItem;
      item_flags = Qt::ItemIsEnabled; // |Qt::ItemIsSelectable;
      //TODO: check for READONLY
      bool ro = item_->isReadOnly();
      if (!ro)
        item_flags |= Qt::ItemIsEditable;
      twi->setFlags((Qt::ItemFlags)item_flags);
      // set &UserDataItemBase into 1.data
      twi->setData(Qt::UserRole, QVariant((ta_intptr_t)item_));
      if (item_->isSimple()) {
        //UserDataItem* item = (UserDataItem*)item_;
      // simple (Variant) user data
        MemberDef* mbr = item_->FindMember("value"); // better be found!
        ms->memb_el.Add(mbr); // keep synced
        if (!mbr || (mbr->im == NULL)) continue; // shouldn't happen

      } else {
      // complex user data -- just do an inline guy
        ms->memb_el.Add(NULL);
      }
      tw->setItem(row, 1, twi);

      // label item -- same for all types
      twi = new QTableWidgetItem;
      twi->setText(item_->GetName());
      twi->setStatusTip(item_->GetDesc());
      twi->setToolTip(taiMisc::ToolTipPreProcess(item_->GetDesc()));
      item_flags = Qt::ItemIsEnabled;
      // item is editable if renamable
      if (item_->canRename())
        item_flags |= Qt::ItemIsEditable;
      twi->setFlags((Qt::ItemFlags)item_flags);
      tw->setItem(row, 0, twi);

      tw->setRowHeight(row, row_height);
      ++row;
    }
    def_grp = false;
    ++set_idx;
  }
  tw->resizeColumnToContents(0); // note: don't do 1 here, do it in GetImage
}

void taiEditorOfUserData::GetImage_Item(int row) {
  QTableWidgetItem* it = tw->item(row, 1);
  QTableWidgetItem* lbl = tw->item(row, 0); // not always needed
  if (!it) return;
  UserDataItemBase* item_ = dynamic_cast<UserDataItemBase*>(
    (taBase*)(it->data(Qt::UserRole).value<ta_intptr_t>()));
  if (item_ == NULL) return;
  if (item_->isSimple()) {
    UserDataItem* item = (UserDataItem*)item_;
    MemberDef* mbr = item->FindMember("value"); // better be found!
    if (!mbr) return; // shouldn't happen
    String txt = mbr->GetValStr(item, TypeDef::SC_DISPLAY, true);
    // augment plain non-class vals with bg color
    if(!txt.contains("<font style=\"background-color:")) {
      if(mbr->type->IsEnum() || mbr->type->DerivesFrom(TA_taSmartPtr)
          || mbr->type->DerivesFrom(TA_taSmartRef) || mbr->type->IsAnyPtr())
        txt = "<font style=\"background-color: LightGrey\">&nbsp;&nbsp;" + txt + "&nbsp;&nbsp;</font>";
      else
        txt = "<font style=\"background-color: white\">&nbsp;&nbsp;" + txt + "&nbsp;&nbsp;</font>";
    }
    it->setText(txt);
    it->setToolTip(taiMisc::ToolTipPreProcess(txt)); // for when over

    // default highlighting
    switch (mbr->GetDefaultStatus(item)) {
    case MemberDef::NOT_DEF:
      lbl->setData(Qt::BackgroundRole, QColor(Qt::yellow));
      break;
    case MemberDef::EQU_DEF:
      //note: setting nil Variant will force it to ignore and use bg
      lbl->setData(Qt::BackgroundRole, QVariant());
      break;
    default: break; // compiler food
    }
  } else {
  //complex
    String txt = item_->GetTypeDef()->GetValStr(item_, NULL,
      NULL, TypeDef::SC_DISPLAY, true);
    it->setText(txt);
    it->setToolTip(taiMisc::ToolTipPreProcess(txt)); // for when over
  }
}

void taiEditorOfUserData::GetImage_Membs_def() {
  for (int row = 0; row < tw->rowCount(); ++row) {
    GetImage_Item(row);
  }
  udd->GetImage(); // if a ctrl is active
  tw->resizeColumnsToContents(); // do all cols, because names could change
}

void taiEditorOfUserData::Constr_Methods_impl() {
  inherited::Constr_Methods_impl();
  Insert_Methods();
}

void taiEditorOfUserData::SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) {
//note: we completely replace default, and basically rebuild on any Group notify,
// and ignore the other notifies (i.e the List guys, which will be echoes of a Group

  // note: because of deferred construction, we may still need to update buttons/menus
  if (state == DEFERRED1) {
    Refresh_impl(false);
    return;
  }
  // note: we should have unlinked if cancelled, but if not, ignore if cancelled
  if (!isConstructed()) return;

  if (updating) return; // it is us that caused this
  // only do simple refresh if an item is updated, otherwise, the major changes
  // of Insert, Remove, etc. need full refresh
  if (sls == SLS_GROUP_ITEM_UPDATE)
  {
    Refresh_impl(false);
  } else if ((sls > SLS_GROUP_ITEM_UPDATE) &&
    (sls <= SLS_GROUP_LIST_SORTED))
  {
    Refresh_impl(true);
  }
}

void taiEditorOfUserData::DoDeleteUserDataItem() {
   // removes the sel_item_index item
  if (udil->RemoveLeafEl(sel_item_base)) {
  }
  else
    taMisc::DebugInfo("taiEditorOfUserData::DoDeleteUserDataItem: could not find item");
}

void taiEditorOfUserData::DoRenameUserDataItem() {
//TODO: start edit of sel_item_row
  if (sel_item_row < 0) return;
  QTableWidgetItem* item = tw->item(sel_item_row, 0);
  if (item) tw->editItem(item);
}

void taiEditorOfUserData::FillLabelContextMenu_SelEdit(QMenu* menu,
  int& last_id)
{
  UserDataItemBase* item = dynamic_cast<UserDataItemBase*>(sel_item_base);
  if (item == NULL) return;

  int sel_item_index = udil->FindLeafEl(item);
  if (sel_item_index < 0) return;  //huh?
  if (item->canDelete())
    //QAction* act =
    menu->addAction("Delete UserDataItem", this, SLOT(DoDeleteUserDataItem()));
  if (item->canRename())
    //QAction* act =
    menu->addAction("Rename UserDataItem", this, SLOT(DoRenameUserDataItem()));
}

UserDataItemBase* taiEditorOfUserData::GetUserDataItem(int row) {
  if ((row < 0) || (row >= tw->rowCount())) return NULL;
  QTableWidgetItem* twi = tw->item(row, 1);
  if (!twi) return NULL; // ex. right clicking on a section
  UserDataItemBase* item = dynamic_cast<UserDataItemBase*>(
    (taBase*)(twi->data(Qt::UserRole).value<ta_intptr_t>()));
  return item;
}

void taiEditorOfUserData::GetValue_Membs_def() {
  udd->GetValue();
  if(udil && udil->owner)
    udil->owner->UpdateAfterEdit(); // update owner too
}

void taiEditorOfUserData::tw_currentCellChanged(int row,
    int col, int previousRow, int previousColumn)
{
  if ((row < 0) || (col < 1)) return;
  // edit of item
  QTableWidgetItem* twi = tw->item(row, col);
  if (!twi) return;
  tw->editItem(twi);
}

void taiEditorOfUserData::tw_customContextMenuRequested(const QPoint& pos)
{
  int row = tw->rowAt(pos.y());
  int col = tw->columnAt(pos.x());
  if ((row < 0) || (col != 0)) return;
  // we want the data item for the label, to get its goodies...

  UserDataItemBase* item = GetUserDataItem(row);
  if (item == NULL) return;

  //na sel_item_mbr = item->mbr;
  sel_item_row = row;
  sel_item_base = item;
  QMenu* menu = new QMenu(widget());
  int last_id = -1;
  FillLabelContextMenu(menu, last_id);
  if (menu->actions().count() > 0) {
    taMisc::in_eventproc++;       // this is an event proc!
    menu->exec(tw->mapToGlobal(pos));
    taMisc::in_eventproc--;
  }
  delete menu;

}

void taiEditorOfUserData::tw_itemChanged(QTableWidgetItem* item)
{
  if (item->column() > 0) return;

  UserDataItemBase* udi = GetUserDataItem(item->row());
  if (udi == NULL) return;
  udi->SetName(item->text());
  Refresh_impl(false);
}
