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

#include "taiWidgetItemChooser.h"
#include <String_Array>
#include <iDialogItemChooser>

#include <QHBoxLayout>
#include <QToolButton>
#include <QTreeWidgetItem>

#include <taiMisc>



taiWidgetItemChooser::taiWidgetItemChooser(TypeDef* typ_,
                               IWidgetHost* host_, taiWidget* par,
                               QWidget* gui_parent_, int flags_,
                               const String& flt_start_txt)
  : taiWidget(typ_, host_, par, gui_parent_, flags_)
{
  item_filter = NULL;
  cust_chooser = NULL;
  filter_start_txt = flt_start_txt;
  targ_typ = NULL; // gets set later
  m_sel = NULL;
  cats = NULL;
  null_text = " NULL";
  new1_par = NULL;
  new2_par = NULL;
  btnEdit = NULL;
  btnHelp = NULL;
  // if we need more than one control, we make a widg w/layout
  QWidget* act_par = NULL;
  QHBoxLayout* lay = NULL;
  if (HasFlag(flgEditDialog) || !HasFlag(flgNoHelp)) {
    act_par = MakeLayoutWidget(gui_parent_);
    lay = new QHBoxLayout(act_par);
    lay->setMargin(0);
    lay->setSpacing(1);
    // put the stuff in the gui
    m_but = new QToolButton(act_par);
//    taiM->FormatButton(m_but, _nilString, defSize());
//    m_but->setFixedHeight(taiM->button_height(defSize()));
    lay->addWidget(m_but, 1);
    SetRep(act_par);
  }
  else {
    m_but = new QToolButton(gui_parent_);
    SetRep(m_but);
  }
  taiM->FormatButton(m_but, _nilString, defSize());
  m_but->setFixedHeight(taiM->button_height(defSize()));

  if (HasFlag(flgEditDialog)) {
    if (!HasFlag(flgReadOnly)) {
      btnEdit = new QToolButton(act_par);
      btnEdit->setIcon( QIcon( QPixmap(":/images/editedit.png") ) );
      btnEdit->setToolTip("edit this item in another panel");
      connect(btnEdit, SIGNAL(clicked()), this, SLOT(EditPanel()) );
      btnEdit->setFixedHeight(taiM->text_height(defSize()));
      btnEdit->setFocusPolicy(Qt::ClickFocus); // skip over for normal tab chain
      lay->addWidget(btnEdit);
    }
  }

  if (!HasFlag(flgNoHelp)) {
    btnHelp = new QToolButton(act_par);
    btnHelp->setText("?");
    btnHelp->setToolTip("get Help for this item");
//    taiM->FormatButton(btnHelp, _nilString, defSize());
    btnHelp->setFixedHeight(taiM->button_height(defSize()));
    btnHelp->setFocusPolicy(Qt::ClickFocus); // skip over for normal tab chain
    lay->addWidget(btnHelp);
    connect(btnHelp, SIGNAL(clicked()), this, SLOT(btnHelp_clicked()) );
  }
  if (lay) {
    lay->addStretch();
  }
  // disable button if ro or no tokens available
  if (HasFlag(flgReadOnly) || HasFlag(flgNoTokenDlg)) {
    m_but->setEnabled(false);
  }
  else {
    connect(m_but, SIGNAL(clicked()), this, SLOT(OpenChooser()) );
  }
}

taiWidgetItemChooser::~taiWidgetItemChooser() {
  delete cats;
  cats = NULL;
}

void taiWidgetItemChooser::BuildChooser(iDialogItemChooser* ic, int view) {
  if(new1_par) {
    QTreeWidgetItem* item = ic->AddItem(new1_text, NULL, (void*)new1_par);
    item->setData(0, iDialogItemChooser::NewFunRole, true);
    item->setData(1, Qt::DisplayRole, "Make a new object"); //note: no desc
  }
  if(new2_par) {
    QTreeWidgetItem* item = ic->AddItem(new2_text, NULL, (void*)new2_par);
    item->setData(0, iDialogItemChooser::NewFunRole, true);
    item->setData(1, Qt::DisplayRole, "Make a new object"); //note: no desc
  }
}

void taiWidgetItemChooser::BuildCategories() {
  if (!isValid()) return;
  BuildCategories_impl();
}

int taiWidgetItemChooser::catCount() const {
  // if only item is blank, then return 0
  if (cats) {
    if ((cats->size == 1) && (cats->FastEl(0).empty())) return 0;
    else return cats->size;
  }
  else return 0;
}

const String taiWidgetItemChooser::catText(int index) const {
  return (cats) ? cats->SafeEl(index) : _nilString;
}

void taiWidgetItemChooser::GetImage(void* cur_sel, TypeDef* targ_typ_) {
  if(targ_typ_ != NULL)
    targ_typ = targ_typ_;
  UpdateImage(cur_sel);
}

const String taiWidgetItemChooser::labelText() {
  String nm;
//note: don't include itemTag with name -- label is usually descriptive
  if (m_sel) nm = labelNameNonNull();
  else       nm = itemTag() + nullText();
  return nm ;
}

const String taiWidgetItemChooser::titleText() {
  String chs_title = "Choose " + itemTag();
  if(targ_typ) chs_title += " from: " + targ_typ->name;
  return chs_title;
}

bool taiWidgetItemChooser::OpenChooser() {
  bool rval = false;
  BuildCategories(); // for subtypes that use categories
  String chs_title = titleText();
  iDialogItemChooser* ic = iDialogItemChooser::New(chs_title, this);
  if (ic->Choose(this)) {
    rval = true;                // hit ok
    // always update even if it says the item was the same, because we could have set a 
    // default item during construction..
    UpdateImage(ic->selObj());
    if (mflags & flgAutoApply)
      applyNow();
    else
      SigEmit();
  }
  delete ic;
  // return focus to rep and window
  rep()->window()->setFocus();
  rep()->setFocus();
  return rval;
}

bool taiWidgetItemChooser::ShowItemFilter(void* base, void* item, const String& itnm) const {
//   if(filter_start_txt.nonempty() && !itnm.startsWith(filter_start_txt)) return false;
  if (item_filter)
    return item_filter(base, item);
  return true;
}

void taiWidgetItemChooser::UpdateImage(void* cur_sel) {
  // note: don't optimize this if same msel, since we use it to set label
  m_sel = cur_sel;
  if(hasNoItems()) {
    rep()->setEnabled(false);   // if no options
    rep()->setText("No Items Available to Choose!");
    return;
  }
  if(hasOnlyOneItem()) {
    rep()->setEnabled(false);   // if only one option, this is it -- will override m_sel!
  }
  else {
    rep()->setEnabled(true);
  }
  rep()->setText(labelText());
}

