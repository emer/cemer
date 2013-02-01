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

#include "iMethodButtonMgr.h"
#include <taiMethodData>
#include <MethodDef>
#include <taiMethod>

#include <QAbstractButton>
#include <QLayout>

#include <SigLinkSignal>
#include <taiMisc>


iMethodButtonMgr::iMethodButtonMgr(QObject* parent)
:inherited(parent)
{
  widg = NULL;
  m_lay = NULL;
  Init();
}

iMethodButtonMgr::iMethodButtonMgr(QWidget* widg_, QLayout* lay_, QObject* parent)
:inherited(parent)
{
  widg = widg_;
  m_lay = lay_;
  Init();
}

iMethodButtonMgr::~iMethodButtonMgr()
{
  Reset();
}

void iMethodButtonMgr::Init() {
  host = NULL;
  show_meth_buttons = false;
  base = NULL;
  typ = NULL;
  cur_menu_but = NULL;
//  m_lay = new iFlowLayout(this, 3, taiM->hspc_c, (Qt::AlignCenter));
}

void iMethodButtonMgr::AddMethButton(taiMethodData* mth_rep, const String& label) {
  QWidget* but = mth_rep->GetButtonRep();
  DoAddMethButton(but);
  if (label.nonempty() && but->inherits("QAbstractButton")) {
    ((QAbstractButton*)but)->setText(label);
  }
}

void iMethodButtonMgr::Reset() {
  cur_menu_but = NULL;
  while (meth_el.size > 0) {
    taiData* dat = meth_el.Pop();
    dat->Delete();
  }
  while (ta_menu_buttons.size > 0) {
    taiData* dat = ta_menu_buttons.Pop();
    dat->Delete();
  }
  setBase(NULL);
  show_meth_buttons = false;
}

void iMethodButtonMgr::Constr(QWidget* widg_, QLayout* lay_,
  taBase* base_, IDataHost* host_)
{
  widg = widg_;
  m_lay = lay_;
  Constr_impl(base_, host_);
}

void iMethodButtonMgr::Constr(taBase* base_, IDataHost* host_)
{
  Constr_impl(base_, host_);
}

void iMethodButtonMgr::Constr_impl(taBase* base_, IDataHost* host_)
{
  Reset();
  host = host_; // prob not needed
  setBase(base_);
  show_meth_buttons = false;
  Constr_Methods_impl();
}

void iMethodButtonMgr::Constr_Methods_impl() {
  if (typ == NULL) return;

  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((md->im == NULL) || (md->name == "Edit")) // don't put edit on edit dialogs..
      continue;
    if (!md->ShowMethod()) continue;
    taiMethod* im = md->im;
    if (im == NULL)
      continue;

    taiMethodData* mth_rep = NULL;
    if (md->HasOption("MENU_BUTTON")) {
      SetCurMenuButton(md);
      mth_rep = im->GetMenuMethodRep(base, host, NULL, NULL);
      mth_rep->AddToMenu(cur_menu_but);
      meth_el.Add(mth_rep);
    }
    /*
    if (md->HasOption("MENU")) {
      SetCurMenu(md);
      mth_rep = im->GetMenuMethodRep(base, host, NULL, NULL);
      mth_rep->AddToMenu(cur_menu);
      meth_el.Add(mth_rep);
    }*/
    if (md->HasOption("BUTTON")) {
      mth_rep = im->GetButtonMethodRep(base, host, NULL, widg);
      AddMethButton(mth_rep);
      meth_el.Add(mth_rep);
    }
  }
}

void iMethodButtonMgr::SigLinkDestroying(taSigLink* dl) {
  base = NULL;
  //TODO: delete the buttons etc.
}

void iMethodButtonMgr::SigLinkRecv(taSigLink* dl, int dcr, void* op1, void* op2) {
  if (dcr > SLS_ITEM_UPDATED_ND) return;
  GetImage();
}

void iMethodButtonMgr::DoAddMethButton(QWidget* but) {
  show_meth_buttons = true;
  // we use "medium" size for buttons
  but->setFont(taiM->buttonFont(taiMisc::fonMedium));
  but->setFixedHeight(taiM->button_height(taiMisc::sizMedium));
  if (but->parent() != widg) {
    widg->setParent(but);
  }
  m_lay->addWidget(but);
  but->show(); // needed when rebuilding
}

void iMethodButtonMgr::GetImage() {
  if(!widg || !widg->isVisible()) return;

  for (int i = 0; i < meth_el.size; ++i) {
    taiMethodData* mth_rep = (taiMethodData*)meth_el.SafeEl(i);
    if ( !(mth_rep->hasButtonRep())) //note: construction forced creation of all buttons
      continue;

    mth_rep->UpdateButtonRep();
  }
}

void iMethodButtonMgr::setBase(taBase* ta) {
  if (base == ta) return;
  if (base) {
    base->RemoveSigClient(this);
    base = NULL;
    typ = NULL;
  }
  base = ta;
  if (base) {
    base->AddSigClient(this);
    typ = base->GetTypeDef();
  }
}

void iMethodButtonMgr::SetCurMenuButton(MethodDef* md) {
  String men_nm = md->OptionAfter("MENU_ON_");
  if (men_nm != "") {
    cur_menu_but = ta_menu_buttons.FindName(men_nm);
    if (cur_menu_but != NULL)  return;
  }
  if (cur_menu_but != NULL)  return;

  if (men_nm == "")
    men_nm = "Misc"; //note: this description not great, but should be different from "Actions", esp. for
       // context menus in the browser (otherwise, there are 2 "Actions" menus); see also taSigLinkBase::FillContextMenu_impl
      // also, must work when it appears before the other label (ex "Misc", then "Actions" )
  cur_menu_but = taiActions::New(taiMenu::buttonmenu, taiMenu::normal, taiMisc::fonSmall,
            NULL, host, NULL, widg);
  cur_menu_but->setLabel(men_nm);
  DoAddMethButton(cur_menu_but->GetRep()); // rep is the button for buttonmenu
  ta_menu_buttons.Add(cur_menu_but);
}

