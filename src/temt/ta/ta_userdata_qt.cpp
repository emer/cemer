// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ta_userdata_qt.h"

#include "ta_qt.h"
#include "ta_qtdialog.h"
#include "ta_TA_inst.h"

#include <QColor>
#include <QHeaderView>
#include <QLayout>
#include <QTableWidget>
#include <QTextEdit>


//////////////////////////
//   UserDataDelegate	//
//////////////////////////

UserDataDelegate::UserDataDelegate(UserDataItem_List* udil_,
  iUserDataDataHost* uddh_)
:inherited(uddh_)
{
  udil = udil_;
  uddh = uddh_;
}

QWidget* UserDataDelegate::createEditor(QWidget* parent, 
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  MemberDef* md = NULL;
  taBase* base = NULL;
  if (IndexToMembBase(index, md, base)) {
    if (md) {
      if (md->im == NULL) goto exit; // shouldn't happen
      // we create a wrap widget for many of these guys, mostly so that smaller
      // guys like Combo don't try to be stretched the whole way
      bool wrap = true;
      QWidget* rep_par = (wrap) ?  new QWidget(parent) : parent;
      
      dat = md->im->GetDataRep(edh, NULL, rep_par);
      dat->SetMemberDef(md);
      rep = dat->GetRep(); // note: rep may get replaced by rep_par 
      if (wrap) {
        QHBoxLayout* hbl = new QHBoxLayout(rep_par);
        hbl->setMargin(0);
        hbl->setSpacing(0);
        hbl->addWidget(rep);
        // some controls do better without stretch
        if (!(dynamic_cast<taiField*>((taiData*)dat)))
          hbl->addStretch();
        rep = rep_par;
      }
    } else { // an inline taBase -- always wrap so we can stretch for longer guys
      QWidget* rep_par = new QWidget(parent);
      QHBoxLayout* hbl = new QHBoxLayout(rep_par);
      hbl->setMargin(0);
      hbl->setSpacing(0);
      dat = taiPolyData::New(true, base->GetTypeDef(), edh, NULL,
        rep_par);
      rep = dat->GetRep();
      hbl->addWidget(rep);
      rep = rep_par;
    }
    dat->SetBase(base);
    m_dat_row = index.row();
    
    connect(rep, SIGNAL(destroyed(QObject*)),
      dat, SLOT(deleteLater()) );
    EditorCreated(parent, rep, option, index);
    return rep;
  }
exit:
  return inherited::createEditor(parent, option, index);
  
}

bool UserDataDelegate::IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const
{
  QTableWidgetItem* twi = uddh->tw->item(index.row(), index.column());
  if (twi) {
    UserDataItemBase* item = dynamic_cast<UserDataItemBase*>(
      (taBase*)(twi->data(Qt::UserRole).value<ta_intptr_t>()));
    base = item; // the item itself is the base
    if (item) {
      if (item->isSimple()) {
        mbr = item->FindMember("value"); // better be found!
      } else { // complex
      //nothing
      }
      return true;
    }
  }
  return false;
}


//////////////////////////////////
//  iUserDataDataHost		//
//////////////////////////////////

iUserDataDataHost::iUserDataDataHost(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, false, parent)
{
  Initialize();
  udil = (UserDataItem_List*)base;
}

iUserDataDataHost::~iUserDataDataHost() {
  delete udd; // hope it's ref is deleted too!
  udd = NULL;
}

void iUserDataDataHost::Initialize()
{
//  no_meth_menu = true; // only show them on outer menu, by way of Propertiesguy
  udil = NULL;
  sel_edit_mbrs = true; // note: we don't actually select members, just for removal
  tw = NULL;
  udd = new UserDataDelegate(udil, this);
  sel_item_row = -1;
}

void iUserDataDataHost::Constr_impl() {
  inherited::Constr_impl();
  // we actually want methods...
  Constr_Methods();
  frmMethButtons->setHidden(!showMethButtons());
}
  
void iUserDataDataHost::Constr_Body_impl() {
}
 
void iUserDataDataHost::Constr_Box() {
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
 

void iUserDataDataHost::ClearBody_impl() {
  // note: we don't nuke control, we just clear it
  tw->clear();
}

void iUserDataDataHost::Constr_Data_Labels() {
  // delete all previous udil members
  membs.ResetItems();
  tw->clearSpans();
  tw->clear();
  // mark place
  String nm;
  String help_text;
  MembSet* ms = NULL;
  
  taGroupItr itr;
  UserDataItem_List* grp;
  int set_idx = 0;
  int row = 0;
  // iterates all non-empty groups...
  FOR_ITR_GP(UserDataItem_List, grp, udil->, itr) {
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
      if (!item_->isVisible() && (show() & taMisc::NO_HIDDEN)) continue;
      
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
      twi->setToolTip(item_->GetDesc());
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

void iUserDataDataHost::GetImage_Item(int row) {
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
    void* off = mbr->GetOff(item);
    String txt = mbr->type->GetValStr(off, item, mbr, TypeDef::SC_DISPLAY, true); 
    // augment plain non-class vals with bg color
    if(!txt.contains("<font style=\"background-color:")) {
      if(mbr->type->DerivesFormal(TA_enum) || mbr->type->DerivesFrom(TA_taSmartPtr)
          || mbr->type->DerivesFrom(TA_taSmartRef) || mbr->type->ptr > 0)
        txt = "<font style=\"background-color: LightGrey\">&nbsp;&nbsp;" + txt + "&nbsp;&nbsp;</font>";
      else
        txt = "<font style=\"background-color: white\">&nbsp;&nbsp;" + txt + "&nbsp;&nbsp;</font>";
    }
    it->setText(txt);
    it->setToolTip(txt); // for when over
  
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
    it->setToolTip(txt); // for when over
  }
}

void iUserDataDataHost::GetImage_Membs_def() {
  for (int row = 0; row < tw->rowCount(); ++row) {
    GetImage_Item(row);
  }
  udd->GetImage(); // if a ctrl is active
  tw->resizeColumnsToContents(); // do all cols, because names could change
}

void iUserDataDataHost::Constr_Methods_impl() {
  inherited::Constr_Methods_impl();
  Insert_Methods();
}

void iUserDataDataHost::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
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
  if ((dcr == DCR_GROUP_ITEM_UPDATE))
  {
    Refresh_impl(false);
  } else if ((dcr > DCR_GROUP_ITEM_UPDATE) &&
    (dcr <= DCR_GROUP_LIST_SORTED)) 
  {
    Refresh_impl(true);
  } 
}

void iUserDataDataHost::DoDeleteUserDataItem() {
   // removes the sel_item_index item 
  if (udil->RemoveLeafEl(sel_item_base)) {
  }
  else
    taMisc::DebugInfo("iUserDataDataHost::DoDeleteUserDataItem: could not find item");
}

void iUserDataDataHost::DoRenameUserDataItem() {
//TODO: start edit of sel_item_row
  if (sel_item_row < 0) return;
  QTableWidgetItem* item = tw->item(sel_item_row, 0);
  if (item) tw->editItem(item);
}

void iUserDataDataHost::FillLabelContextMenu_SelEdit(QMenu* menu,
  int& last_id)
{
  UserDataItemBase* item = dynamic_cast<UserDataItemBase*>(sel_item_base);
  if ((item == NULL) ) return;

  int sel_item_index = udil->FindLeafEl(item);
  if (sel_item_index < 0) return;  //huh?
  if (item->canDelete())
    //QAction* act = 
    menu->addAction("Delete UserDataItem", this, SLOT(DoDeleteUserDataItem()));
  if (item->canRename())
    //QAction* act = 
    menu->addAction("Rename UserDataItem", this, SLOT(DoRenameUserDataItem()));
}

UserDataItemBase* iUserDataDataHost::GetUserDataItem(int row) {
  if ((row < 0) || (row >= tw->rowCount())) return NULL;
  QTableWidgetItem* twi = tw->item(row, 1);
  if (!twi) return NULL; // ex. right clicking on a section
  UserDataItemBase* item = dynamic_cast<UserDataItemBase*>(
    (taBase*)(twi->data(Qt::UserRole).value<ta_intptr_t>()));
  return item;
}

void iUserDataDataHost::GetValue_Membs_def() {
  udd->GetValue();
  if(udil && udil->owner)
    udil->owner->UpdateAfterEdit(); // update owner too
}

void iUserDataDataHost::tw_currentCellChanged(int row, 
    int col, int previousRow, int previousColumn)
{
  if ((row < 0) || (col < 1)) return;
  // edit of item
  QTableWidgetItem* twi = tw->item(row, col);
  if (!twi) return;
  tw->editItem(twi);
}

void iUserDataDataHost::tw_customContextMenuRequested(const QPoint& pos)
{
  int row = tw->rowAt(pos.y());
  int col = tw->columnAt(pos.x());
  if ((row < 0) || (col != 0)) return;
  // we want the data item for the label, to get its goodies...
  
  UserDataItemBase* item = GetUserDataItem(row);
  if ((item == NULL) ) return;
    
  //na sel_item_mbr = item->mbr;
  sel_item_row = row;
  sel_item_base = item;
  QMenu* menu = new QMenu(widget());
  int last_id = -1;
  FillLabelContextMenu(menu, last_id);
  if (menu->actions().count() > 0)
    menu->exec(tw->mapToGlobal(pos));
  delete menu;
  
}

void iUserDataDataHost::tw_itemChanged(QTableWidgetItem* item)
{
  if (item->column() > 0) return;

  UserDataItemBase* udi = GetUserDataItem(item->row());
  if ((udi == NULL) ) return;
  udi->SetName(item->text());
  Refresh_impl(false);
}

//////////////////////////
//   iUserDataPanel 	//
//////////////////////////

iUserDataPanel::iUserDataPanel(taiDataLink* dl_)
:inherited(dl_)
{
  UserDataItem_List* se_ = udil();
  se = NULL;
  if (se_) {
    switch (taMisc::select_edit_style) { // NOTE: the two below look identical to me...
    case taMisc::ES_ALL_CONTROLS:
      se = new iUserDataDataHost(se_, se_->GetTypeDef()); 
      break;
    case taMisc::ES_ACTIVE_CONTROL:
      se = new iUserDataDataHost(se_, se_->GetTypeDef()); 
      break;
    }
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bool ok;
      iColor bgcol = se_->GetEditColorInherit(ok);
      if (ok) se->setBgColor(bgcol);
    }
  }
}

iUserDataPanel::~iUserDataPanel() {
  if (se) {
    delete se;
    se = NULL;
  }
}

void iUserDataPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
}

bool iUserDataPanel::HasChanged() {
  return se->HasChanged();
}

bool iUserDataPanel::ignoreDataChanged() const {
  return !isVisible();
}

void iUserDataPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  se->ConstrEditControl();
  setCentralWidget(se->widget()); //sets parent
  setButtonsWidget(se->widButtons);
}

void iUserDataPanel::UpdatePanel_impl() {
  if (se) se->ReShow_Async();
}

void iUserDataPanel::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (se && se->HasChanged()) {
    se->Apply();
  }
}


