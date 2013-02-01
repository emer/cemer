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

#include "taiTokenPtrMultiTypeButton.h"
#include <taiEdit>
#include <iMainWindowViewer>
#include <taProject>
#include <taiItemChooser>

#include <taMisc>
#include <taiMisc>

#include <QTreeWidget>
#include <QTreeWidgetItem>



taiTokenPtrMultiTypeButton::taiTokenPtrMultiTypeButton(TypeDef* typ_, IWidgetHost* host,
                                     taiWidget* par, QWidget* gui_parent_, int flags_,
                                     const String& flt_start_txt)
  : inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
  scope_typ = NULL;
}

void taiTokenPtrMultiTypeButton::EditDialog() {
  taBase* cur_base = GetValue();
  if (!cur_base) return;

  taiEdit* gc = (taiEdit*) ((taBase*)cur_base)->GetTypeDef()->ie;
  if (!gc) return; // shouldn't happen

  gc->EditDialog(cur_base, false); //TODO: ever read_only???
}

void taiTokenPtrMultiTypeButton::EditPanel() {
  taBase* cur_base = GetValue();
  if (!cur_base) return;

  iMainWindowViewer* imw = taiM->active_wins.Peek_MainWindow();
  if (!imw) return; // no viewer!
  // get a better viewer if possible
  taProject* proj = GET_OWNER(cur_base, taProject);
  if(proj) {
    MainWindowViewer* vwr = proj->GetDefaultProjectBrowser();
    if(vwr && vwr->widget())
      imw = vwr->widget();
  }

  taiSigLink* dl = (taiSigLink*)cur_base->GetSigLink();
  if (dl) {
    imw->EditItem(dl, true); // edit, but not in this tab
  }
}

void taiTokenPtrMultiTypeButton::BuildChooser(taiItemChooser* ic, int view) {
  if(cust_chooser)
    cust_chooser(scope_ref, this);

  inherited::BuildChooser(ic, view);

  if (!targ_typ) {
    taMisc::Error("taiTokenPtrMultiTypeButton::BuildChooser: targ_type needed");
    return;
  }

  int st_typ = 0;
  int ed_typ = type_list.size-1;
  if(view > 0) {
    st_typ = ed_typ = view-1;
  }
  if (HasFlag(flgNullOk)) {
    // note: ' ' makes it sort to the top
    QTreeWidgetItem* item = ic->AddItem(nullText(), NULL, (void*)NULL); //note: no desc
    item->setData(1, Qt::DisplayRole, " ");
  }
  for(int i = st_typ; i<= ed_typ; i++) {
    TypeDef* td = type_list.SafeEl(i);
    BuildChooser_0(ic, td, NULL);
  }
  ic->items->sortItems(0, Qt::AscendingOrder);
}

int taiTokenPtrMultiTypeButton::BuildChooser_0(taiItemChooser* ic, TypeDef* td,
  QTreeWidgetItem* top_item)
{
  if (!td->IsActualTaBase()) return 0;
  int rval = 0;

  //NOTES:
  // if !tokens.keep then tokens.size==0

  for (int i = 0; i < td->tokens.size; ++i) {
    taBase* btmp = (taBase*)td->tokens.FastEl(i);
    if ((bool)scope_ref && !btmp->SameScope(scope_ref, scope_typ))
      continue;
    if (!ShowToken(btmp)) continue;
    //todo: need to get a more globally unique name, maybe key_unique_name
    QTreeWidgetItem* item = ic->AddItem(btmp->GetColText(taBase::key_disp_name),
      top_item, (void*)btmp);
    item->setData(1, Qt::DisplayRole, btmp->GetTypeDef()->name);
    taBase* own = btmp->GetOwner();
    if (own) {
      item->setData(2, Qt::DisplayRole, own->GetColText(taBase::key_disp_name));
      item->setData(3, Qt::DisplayRole, own->GetColText(taBase::key_type));
    }
    ++rval;
  }

  for (int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    rval += BuildChooser_0(ic, chld, top_item); //note: we don't create subnodes
  }
  return rval;
}

int taiTokenPtrMultiTypeButton::columnCount(int view) const {
  return 4;
}

void taiTokenPtrMultiTypeButton::GetImage(void* cur_sel_, TypeDef* targ_typ_)
{//NOTE: this routine is needed in case clients call the old GetImage renamed to GetImageScoped
  scope_ref = NULL;
  scope_typ = NULL;
  inherited::GetImage(cur_sel_, targ_typ_);
}

void taiTokenPtrMultiTypeButton::GetImageScoped(taBase* ths, TypeDef* targ_typ_,
  taBase* scope_, TypeDef* scope_type_)
{
  scope_ref = scope_;
  scope_typ = scope_type_;
  inherited::GetImage((void*)ths, targ_typ_);
}

const String taiTokenPtrMultiTypeButton::headerText(int index, int view) const {
  switch (index) {
  case 0: return "Name";
  case 1: return "Type";
  case 2: return "Owner Name";
  case 3: return "Owner Type";
  }
  return _nilString; // shouldn't happen
}

const String taiTokenPtrMultiTypeButton::labelNameNonNull() const {
  return token()->GetDisplayName();
}

bool taiTokenPtrMultiTypeButton::ShowToken(taBase* obj) const {
  return ShowItemFilter(scope_ref, obj, obj->GetName());
}

int taiTokenPtrMultiTypeButton::viewCount() const {
  return type_list.size + 1;
}

const String taiTokenPtrMultiTypeButton::viewText(int index) const {
  if(index == 0) return "All Types";
  return type_list.PosSafeEl(index-1)->name;
}

