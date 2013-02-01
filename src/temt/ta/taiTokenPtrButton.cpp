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

#include "taiTokenPtrButton.h"
#include <iHelpBrowser>
#include <iTextBrowser>
#include <taiEdit>
#include <iMainWindowViewer>
#include <taProject>
#include <taiItemChooser>

#include <taMisc>
#include <taiMisc>
#include <tabMisc>
#include <taRootBase>


#include <QTreeWidgetItem>

taiTokenPtrButton::taiTokenPtrButton(TypeDef* typ_, IWidgetHost* host,
                                     taiData* par, QWidget* gui_parent_, int flags_,
                                     const String& flt_start_txt)
  : inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
  scope_typ = NULL;
}

void taiTokenPtrButton::btnHelp_clicked() {
  taBase* tok = token();
  if (!tok) return;
  TypeDef* typ = tok->GetTypeDef();
  if (!typ) return;
  iHelpBrowser::StatLoadType(typ);
}

void taiTokenPtrButton::EditDialog() {
  taBase* cur_base = GetValue();
  if (!cur_base) return;

  taiEdit* gc = (taiEdit*) ((taBase*)cur_base)->GetTypeDef()->ie;
  if (!gc) return; // shouldn't happen

  gc->EditDialog(cur_base, false); //TODO: ever read_only???
}

void taiTokenPtrButton::EditPanel() {
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
  tabMisc::DelayedFunCall_gui(cur_base, "BrowserSelectMe");
}

bool taiTokenPtrButton::countTokensToN(int& cnt, TypeDef* td, int n, void*& last_itm) {
  if(td->tokens.size == 0 && td->tokens.sub_tokens == 0) return false;
  // not gonna happen if it hasn't already
  for (int i = 0; i < td->tokens.size; ++i) {
    taBase* btmp = (taBase*)td->tokens.FastEl(i);
    if ((bool)scope_ref && !btmp->SameScope(scope_ref, scope_typ))
      continue;
    if (!ShowToken(btmp)) continue;
    cnt++;
    last_itm = (void*)btmp;
    if(cnt >= n) return true;   // got it!
  }
  if(td->tokens.sub_tokens == 0) return false; // not gonna happen

  for (int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    if(countTokensToN(cnt, chld, n, last_itm)) return true;
  }
  return false;                 // didn't happen
}

bool taiTokenPtrButton::hasNoItems() {
  if(!targ_typ) return false;          // shouldn't happen
  if(HasFlag(flgNullOk)) return false; // we now have 1..
  if(targ_typ->tokens.size == 0 && targ_typ->tokens.sub_tokens == 0) return true;
  int cnt = 0;
  void* last_itm = NULL;
  bool got_one = countTokensToN(cnt, targ_typ, 1, last_itm);
  if(!got_one) {
    m_sel = NULL;               // select the null!
    return true;
  }
  return false;
}

bool taiTokenPtrButton::hasOnlyOneItem() {
  if(!targ_typ) return false;          // shouldn't happen
  int cnt = 0;
  void* last_itm = NULL;
  if(HasFlag(flgNullOk)) {
    // now check that we don't have any others -- same logic as has no items
    if(targ_typ->tokens.size == 0 && targ_typ->tokens.sub_tokens == 0) return true;
    if(!countTokensToN(cnt, targ_typ, 1, last_itm)) {
      m_sel = NULL;             // select NULL
      return true;
    }
  }
  if(targ_typ->tokens.size == 0 && targ_typ->tokens.sub_tokens == 0) return false; // no way
  bool got_two = countTokensToN(cnt, targ_typ, 2, last_itm); // if we get 2, then we're bust!
  if(!got_two && cnt == 1) {
    m_sel = last_itm;           // select the one item!
    return true;
  }
  return false;
}

void taiTokenPtrButton::BuildChooser(taiItemChooser* ic, int view) {
  if(cust_chooser)
    cust_chooser(scope_ref, this);
  inherited::BuildChooser(ic, view);
  if (!targ_typ) {
    taMisc::Error("taiTokenPtrButton::BuildChooser: targ_type needed");
    return;
  }
  switch (view) {
  case 0:
    if (HasFlag(flgNullOk)) {
      // note: ' ' makes it sort to the top
      QTreeWidgetItem* item = ic->AddItem(nullText(), NULL, (void*)NULL); //note: no desc
      item->setData(1, Qt::DisplayRole, " ");
    }
    BuildChooser_0(ic, targ_typ, NULL);
    ic->items->sortItems(0, Qt::AscendingOrder);
    break;
  default: break; // shouldn't happen
  }
}

int taiTokenPtrButton::BuildChooser_0(taiItemChooser* ic, TypeDef* td,
  QTreeWidgetItem* top_item)
{
  if(!td->IsActualTaBase()) return 0;
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
    taBase* own = btmp->GetParent();
    if (own) {
      item->setData(2, Qt::DisplayRole, own->GetDisplayName()); // use disp name directly -- overriden to name for groups..
      item->setData(3, Qt::DisplayRole, own->GetColText(taBase::key_type));
    }
    item->setData(4, Qt::DisplayRole, btmp->GetPathNames());
    ++rval;
  }

  for (int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    rval += BuildChooser_0(ic, chld, top_item); //note: we don't create subnodes
  }
  return rval;
}

int taiTokenPtrButton::columnCount(int view) const {
  switch (view) {
  case 0: return 5;
  default: return 0; // not supposed to happen
  }
}

void taiTokenPtrButton::GetImage(void* cur_sel_, TypeDef* targ_typ_)
{//NOTE: this routine is needed in case clients call the old GetImage renamed to GetImageScoped
  scope_ref = NULL;
  scope_typ = NULL;
  inherited::GetImage(cur_sel_, targ_typ_);
}

void taiTokenPtrButton::GetImageScoped(taBase* ths, TypeDef* targ_typ_,
  taBase* scope_, TypeDef* scope_type_)
{
  scope_ref = scope_;
  scope_typ = scope_type_;
  inherited::GetImage((void*)ths, targ_typ_);
}

const String taiTokenPtrButton::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Name";
    case 1: return "Type";
    case 2: return "Parent Name";
    case 3: return "Parent Type";
    case 4: return "Path";
    } break;
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiTokenPtrButton::labelNameNonNull() const {
  return token()->GetDisplayName();
}

bool taiTokenPtrButton::ShowToken(taBase* obj) const {
  return ShowItemFilter(scope_ref, obj, obj->GetName());
}

const String taiTokenPtrButton::viewText(int index) const {
  switch (index) {
  case 0: return "Flat List";
  default: return _nilString;
  }
}

