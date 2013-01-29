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

#include "taiObjChooser.h"
#include <iMainWindowViewer>
#include <iLineEdit>
#include <MemberDef>

#include <taMisc>
#include <taiMisc>
#include <tabMisc>
#include <taRootBase>

#include <QListWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>

taiObjChooser* taiObjChooser::createInstance(taBase* parob, const char* captn, bool selonly, QWidget* par_window_) {
  if (par_window_ == NULL)
    par_window_ = taiMisc::main_window;
  return new taiObjChooser(parob, captn, selonly, par_window_);
}

taiObjChooser* taiObjChooser::createInstance(TypeDef* tpdf, const char* captn, taBase* scope_ref_, QWidget* par_window_) {
  if (par_window_ == NULL)
    par_window_ = taiMisc::main_window;
  return new taiObjChooser(tpdf, captn, scope_ref_, par_window_);
}

void taiObjChooser::init(const char* captn, bool selonly, QWidget* par_window_) {
  lst_par_obj = NULL;
  reg_par_obj = NULL;
  typ_par_obj = NULL;
  scope_ref = NULL;
  caption = captn;
  select_only = selonly;
  msel_obj = NULL;
  setWindowTitle(caption);
  setFont(taiM->dialogFont(taiMisc::fonSmall));
  resize(taiM->dialogSize(taiMisc::hdlg_s));
}

taiObjChooser::taiObjChooser(taBase* parob, const char* captn, bool selonly, QWidget* par_window_)
{
  init(captn, selonly, par_window_);

  if(parob->InheritsFrom(&TA_taList_impl))
    lst_par_obj = (taList_impl*)parob;
  else
    reg_par_obj = parob;

  Build();
}

taiObjChooser::taiObjChooser(TypeDef* td, const char* captn, taBase* scope_ref_, QWidget* par_window_)
: iDialog(par_window_)
{
  setModal(true);
  init(captn, true, par_window_); //select_only = true always true for typedef!

  if(!td->IsActualTaBase()) {
    taMisc::Warning("*** warning, will not be able to select non-taBase tokens in chooser");
  }
  typ_par_obj = td;

  Build();
}

void taiObjChooser::setSel_obj(const taBase* value) {
  if (msel_obj == value) return;
  msel_obj = const_cast<taBase*>(value);
  for (int i = 0; i < browser->count(); ++i) {
    QListWidgetItem* lbi = browser->item(i);
    if (lbi->data(Qt::UserRole).value<ta_intptr_t>() == (ta_intptr_t)value) {
      browser->setCurrentRow(i);
      return;
    }
  }
  browser->setCurrentRow(-1);   // 0 = default is to select first item!  not sure about that
}

bool taiObjChooser::Choose() {
  return (exec() == iDialog::Accepted);
}

void taiObjChooser::GetPathStr() {
  if(lst_par_obj != NULL)
    path_str = lst_par_obj->GetPathNames();
  else if(reg_par_obj != NULL)
    path_str = reg_par_obj->GetPathNames();
  else if(typ_par_obj != NULL)
    path_str = typ_par_obj->name;
}

void taiObjChooser::ReRead() {
  GetPathStr();
  editor->setText((const char*)path_str);
  Clear();
  Load();
}

void taiObjChooser::Build() {
//Qt3  layOuter = new QGridLayout(this, 3, 1, taiM->vsep_c, taiM->vspc_c); // rows, cols, margin, space
  layOuter = new QGridLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);
  browser = new QListWidget(this);
  layOuter->addWidget(browser, 1, 0);
  layOuter->setRowStretch(1, 1); // list is item to expand in host
  layOuter->setRowMinimumHeight(1, 100); // don't shrink to nothing
  browser->setFocus();

  layButtons = new QHBoxLayout();
  layButtons->addStretch();
  btnOk = new QPushButton("&Ok", this);
  btnOk->setDefault(true);
  layButtons->addWidget(btnOk);
  layButtons->addSpacing(taiM->vsep_c);
  btnCancel = new QPushButton("&Cancel", this);
  layButtons->addWidget(btnCancel);
  layOuter->addLayout(layButtons, 2, 0);

  //note: create editor last so it is last on tab order
  GetPathStr();
  editor = new iLineEdit(path_str, this);
  layOuter->addWidget(editor, 0, 0);

  connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  connect(browser, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
      this, SLOT(browser_itemDoubleClicked(QListWidgetItem*)));
  connect(browser, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
      this, SLOT(browser_currentItemChanged(QListWidgetItem*, QListWidgetItem*)));
  connect(editor, SIGNAL(returnPressed()),
      this, SLOT(AcceptEditor()) );

  Load();
}

void taiObjChooser::Clear() {
  browser->clear();
  items.Reset();
  editor->clear();
}

void taiObjChooser::Load() {
  if(lst_par_obj != NULL) {
    if(!select_only) {
      AddItem("..");
      AddObjects(lst_par_obj);
    }
    int i;
    for (i=0; i<lst_par_obj->size; i++) {
      taBase* ob = (taBase*)lst_par_obj->FastEl_(i);
      String lbl = ob->GetName();
      if(lbl.length() == 0)
        lbl = String("[") + String(i) + "]";
      AddItem((const char*)lbl, ob);
    }
  }
  else if(reg_par_obj != NULL) {
    if(!select_only) {
      AddItem("..");
    }
    AddObjects(reg_par_obj);
  }
  else if(typ_par_obj != NULL) {
    AddTokens(typ_par_obj);
  }
}

void taiObjChooser::AddObjects(taBase* obj) {
  TypeDef* td = obj->GetTypeDef();
  for (int i = 0; i < td->members.size; ++i) {
    MemberDef* md = td->members.FastEl(i);
    if(!md->type->IsActualTaBase()) continue;
    taBase* mb = (taBase*)md->GetOff((void*)obj);
    if(mb->GetOwner() == NULL) continue; // not going to be a good selection item
    AddItem((const char*)md->name, md);
  }
}

void taiObjChooser::AddTokens(TypeDef* td) {
  int i;
  for(i=0; i<td->tokens.size; i++) {
    void* tmp = td->tokens.FastEl(i);
    String adrnm = String((long)tmp);
    if(td->IsActualTaBase()) {
      taBase* btmp = (taBase*)tmp;
      if((scope_ref != NULL) && !btmp->SameScope(scope_ref))
        continue;
      if(!btmp->GetName().empty()) {
        AddItem(btmp->GetName(), tmp);
        items[items.size - 1] = adrnm;  // always store the actual address in the string!
      }
      else {
        AddItem(adrnm, tmp);
      }
    }
    else {
      AddItem(adrnm, tmp);
    }
  }

  for(i=0; i<td->children.size; i++) {
    TypeDef* chld = td->children[i];
    if(chld->IsAnyPtr())
      continue;
    if((chld->tokens.size == 0) && (chld->tokens.sub_tokens == 0))
      continue;
    if((chld->tokens.size > 0) || (chld->children.size > 0))
      AddTokens(chld);
  }
}

void taiObjChooser::AddItem(const char* itm, const void* data_) {
  items.Add(itm);
  QListWidgetItem* lwi = new QListWidgetItem(QString(itm), browser);
  lwi->setData(Qt::UserRole, (ta_intptr_t)data_);
  //browser->insertItem(new ocListBoxItem(, data_));
  if (msel_obj == data_)
    browser->setCurrentRow(browser->count() - 1);
}

void taiObjChooser::UpdateFmSelStr() {
  msel_obj = NULL;
  MemberDef* md;
  if(lst_par_obj != NULL) {
    if(msel_str == "root")
      msel_obj = tabMisc::root;
    else if(msel_str == "..")
      msel_obj = lst_par_obj->GetOwner();
    else
      msel_obj = lst_par_obj->FindFromPath(msel_str, md);
  }
  else if(reg_par_obj != NULL) {
    if(msel_str == "root")
      msel_obj = tabMisc::root;
    else if(msel_str == "..")
      msel_obj = reg_par_obj->GetOwner();
    else
      msel_obj = reg_par_obj->FindFromPath(msel_str, md);
  }
  else if(typ_par_obj != NULL) {
    if(!typ_par_obj->IsActualTaBase())
      return;
    long adr = (long)msel_str;
    msel_obj = (taBase*)adr;
    if((msel_obj != NULL) && !msel_obj->GetName().empty())
      msel_str = msel_obj->GetName();
  }
  if(msel_obj == NULL) {
    taMisc::Error("Could not find object:", msel_str, "in path:", path_str,"try again");
    return;
  }
}

void taiObjChooser::accept() {
  int i = browser->currentRow();
  if (i == -1) {
    if (!select_only)
      AcceptEditor_impl(NULL);  // null is clue to not fork to descend!
    else
      AcceptEditor_impl(editor);
    return;
  }
  msel_str = items.FastEl(i);

  UpdateFmSelStr();             // get the new selection based on that!
  if (msel_obj != NULL)
    iDialog::accept();
}

void taiObjChooser::browser_itemDoubleClicked(QListWidgetItem* itm) {
  if (select_only)
    accept();
  else
    DescendBrowser();
}

void taiObjChooser::browser_currentItemChanged(QListWidgetItem* itm, QListWidgetItem*) {
  GetPathStr();
  if (itm == NULL)
    msel_obj = NULL;
  else
    msel_obj = (taBase*)(itm->data(Qt::UserRole).value<ta_intptr_t>());

  String nw_txt;
  if (msel_obj == NULL)
    nw_txt = path_str;
  else
    nw_txt = msel_obj->GetPathNames();

  editor->setText(nw_txt);
}

void taiObjChooser::DescendBrowser() {
  int i = browser->currentRow();
  if (i == -1) {
    msel_str = editor->text();
    if (select_only) {
      msel_str = msel_str.after(path_str);
    }
    else {
      reg_par_obj = tabMisc::root;
      lst_par_obj = NULL;
    }
  }
  else {
    msel_str = items.FastEl(i);
  }

  UpdateFmSelStr();             // get the new selection based on that!
  if (msel_obj == NULL) return;

  String nw_txt = msel_obj->GetPathNames();
  editor->setText((const char*)nw_txt);

  if(msel_obj->InheritsFrom(&TA_taList_impl)) {
    lst_par_obj = (taList_impl*)msel_obj;
    reg_par_obj = NULL;
  }
  else {
    reg_par_obj = msel_obj;
    lst_par_obj = NULL;
  }
  ReRead();
}

void taiObjChooser::reject() {
  msel_obj = NULL;
  iDialog::reject();
}

void taiObjChooser::AcceptEditor() {
  AcceptEditor_impl(editor);
}

void taiObjChooser::AcceptEditor_impl(QLineEdit* e) {
  if(!select_only && (e != NULL)) {
    DescendBrowser();
    return;
  }
  if(e == NULL) e = editor;
  msel_str = e->text();
  if (select_only) {
    msel_str = msel_str.after(path_str);
    UpdateFmSelStr();           // get the new selection based on that!
    if (msel_obj != NULL)
      iDialog::accept();
  } else {
    reg_par_obj = tabMisc::root;
    lst_par_obj = NULL;
    UpdateFmSelStr();
    if(msel_obj != NULL)
      iDialog::accept();
  }
}

