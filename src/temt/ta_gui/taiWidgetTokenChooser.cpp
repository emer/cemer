// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "taiWidgetTokenChooser.h"
#include <iHelpBrowser>
#include <iTextBrowser>
#include <taiEdit>
#include <iMainWindowViewer>
#include <taProject>
#include <iDialogItemChooser>

#include <taMisc>
#include <taiMisc>
#include <tabMisc>
#include <taRootBase>

#include <iTreeWidget>
#include <QTreeWidgetItem>
#include <MemberDef>

const int TOKEN_CHOOSER_NAME_MAX_LEN = 32;

taiWidgetTokenChooser::taiWidgetTokenChooser(TypeDef* typ_, IWidgetHost* host,
                                     taiWidget* par, QWidget* gui_parent_, int flags_,
                                     const String& flt_start_txt)
  : inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
  scope_typ = NULL;
  scope_obj.set(NULL);
  scope_ref.set(NULL);
}

void taiWidgetTokenChooser::btnHelp_clicked() {
  taBase* tok = token();
  if (!tok) return;
  TypeDef* typ = tok->GetTypeDef();
  if (!typ) return;
  iHelpBrowser::StatLoadType(typ);
}

void taiWidgetTokenChooser::EditDialog() {
  taBase* cur_base = GetValue();
  if (!cur_base) return;

  taiEdit* gc = (taiEdit*) ((taBase*)cur_base)->GetTypeDef()->ie;
  if (!gc) return; // shouldn't happen

  gc->EditDialog(cur_base, false); //TODO: ever read_only???
}

void taiWidgetTokenChooser::EditPanel() {
  taBase* cur_base = GetValue();
  if (!cur_base) return;

  iMainWindowViewer* imw = taiM->active_wins.Peek_MainWindow();
  if (!imw) return; // no viewer!
  // get a better viewer if possible
  taProject* proj = cur_base->GetMyProj();
  if(proj) {
    MainWindowViewer* vwr = proj->GetDefaultProjectBrowser();
    if(vwr && vwr->widget())
      imw = vwr->widget();
  }

  // NOTE: this negates any effects of the BrowserSelect function,
  // esp for program items that go somewhere other than in the main tree..
  // taiSigLink* dl = (taiSigLink*)cur_base->GetSigLink();
  // if (dl) {
  //   imw->EditItem(dl, true); // edit, but not in this tab
  // }
  tabMisc::DelayedFunCall_gui(cur_base, "BrowserSelectMe");
}

bool taiWidgetTokenChooser::countTokensToN(int& cnt, TypeDef* td, int n, void*& last_itm,
                                           void*& first_itm) {
  if(td->tokens.size == 0 && td->tokens.sub_tokens == 0)
    return false;

  // not gonna happen if it hasn't already
  for (int i = 0; i < td->tokens.size; ++i) {
    taBase* btmp = (taBase*)td->tokens.FastEl(i);
    if (!ShowToken(btmp, td, i)) continue;
    cnt++;
    if(cnt == 1)
      first_itm = (void*)btmp;
    last_itm = (void*)btmp;
    if(cnt >= n)
      return true;   // got it!
  }
  if(td->tokens.sub_tokens == 0) return false; // not gonna happen

  // IMPORTANT: any changes here should also be made in BuildChooser_0 below!!
  for (int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    if(!chld) continue;
    if(chld->HasOption("NO_TOKEN_CHOOSER"))
      continue;                                 // exclude from any implicit use
    if(countTokensToN(cnt, chld, n, last_itm, first_itm))
      return true;
  }
  return false;                 // didn't happen
}

int taiWidgetTokenChooser::setInitialSel(void* cur_sel) {
  m_sel = cur_sel;              // default is always to use current
  if(!targ_typ) {               // this should not happen
    return 0;
  }
  int cnt = 0;
  void* last_itm = NULL;
  void* first_itm = NULL;
  bool has_two_or_more = countTokensToN(cnt, targ_typ, 2, last_itm, first_itm);
  if(HasFlag(flgNullOk)) {
    if(cnt == 0) {              // no items, just null
      m_sel = NULL;             // only option
      return 1;
    }
    if(cnt == 1) {              // we just got one
      if(HasFlag(flgPreferItem)) {
        // note: we don't check cur_sel b/c it is either null or an invalid option!
        m_sel = first_itm;         // default to single item
      }
      return 2;
    }
    // cnt > 1
    if(HasFlag(flgPreferItem)) {
      if(cur_sel == NULL)
        m_sel = first_itm;      // only if null do we auto-select first item -- other could be on list somewhere..
    }
    return 2;
  }
  else {
    if(cnt == 0) {              // no items
      return 0;
    }
    if(cnt == 1) {              // we just got one
      m_sel = first_itm;        // default to ONLY option
      return 1;
    }
    // cnt > 1
    if(cur_sel == NULL)
      m_sel = first_itm;      // only if null do we auto-select first item -- other could be on list somewhere..
    return 2;
  }    
}

void taiWidgetTokenChooser::BuildChooser(iDialogItemChooser* ic, int view) {
  // TODO: note that scope_ref here is being used as a 'this' pointer I'm pretty sure..
  if(cust_chooser)
    cust_chooser(scope_ref, this);
  inherited::BuildChooser(ic, view);
  if (!targ_typ) {
    taMisc::Error("taiWidgetTokenChooser::BuildChooser: targ_type needed");
    return;
  }
  switch (view) {
    case 0:
    {
      if (HasFlag(flgNullOk)) {
        // note: ' ' makes it sort to the top
        QTreeWidgetItem* item = ic->AddItem(nullText(), NULL, (void*)NULL); //note: no desc
        item->setData(1, Qt::DisplayRole, "NULL");
      }
      if (ic->GetSelectedObject()) {  // just in case we weren't called from a program but standalone dataproc call
        if(!new1_par &&  targ_typ->IsTaBase() && targ_typ->GetInstance() &&  // if new1_par is set we already have a CREATE
           ((taBase*)targ_typ->GetInstance())->HasChooseNew()) {
          QTreeWidgetItem* item = ic->AddItem(" CREATE NEW", NULL,
                                              (void*)(taBase*)targ_typ->GetInstance());
          item->setData(0, iDialogItemChooser::NewFunRole, true); // flag to dialog to make
          item->setData(1, Qt::DisplayRole, targ_typ->name);
        }
      }
      BuildChooser_0(ic, targ_typ, NULL);
      ic->items->sortItems(0, Qt::AscendingOrder);
      break;
    }
    default: break; // shouldn't happen
  }
}

bool taiWidgetTokenChooser::ShowToken(taBase* obj, TypeDef* td, int i) const {
  if(!obj) return false;
  if(obj->isDestroying()) {  // this should not happen -- did for taMatrix, fixed that
    taMisc::Info("is destroying of type:", td->name, "token:", String(i));
    return false;
  }
  // if(targ_typ->InheritsFromName("DataTable_Group") ||
  //    td->InheritsFromName("DataTable_Group")) {
  //   taMisc::Info("got group!");
  // }
  taBase* owner = obj->GetOwner();
  if(!owner) return false;
  if(!targ_typ->InheritsFrom(&TA_taList_impl) && !targ_typ->HasOption("CHOOSE_AS_MEMBER")) {
    // not for lists which DO mostly live as members of other objects, and are
    // selected for putting things somewhere..
    if(!owner->InheritsFrom(&TA_taList_impl)) {
      // radical but simple fact: if you're not on a list, you shouldn't be selectable
      // by someone else as a token, because you are not an independent object -- you are
      // either an owned member of another token, or effectively such as a managed pointer
      // member, as in the AR() matrix on a DataCol

      if(!owner->InheritsFrom(&TA_taProject)) { // allow project members!
        return false;
      }
    }
  }
  taBase* parent = obj->GetParent(); // must have owner and not just be on some list
  if (!parent)
    return false;
  // keeps templates out of the list of actual instances
  if (owner == &tabMisc::root->templates) {
    return false;
  }

  taBase* mbrown = obj->GetMemberOwner(false);
  if(mbrown) {
    MemberDef* my_md = mbrown->FindMemberBase(obj);
    // this is important for ControlPanelMember.saved which still shows up for local configs
    if(my_md && my_md->HasOption("NO_CHOOSER")) {
      return false;
    }
  }
  
  // IMPORTANT: scope_typ CAN be NULL here -- if so, a default scope type is used -- this
  // is actually relevant for various choosers.  also scope_ref is assumed to be the base
  // obj -- this should ideally be cleaner
  if ((bool)scope_ref && !obj->SameScope(scope_ref, scope_typ))
    return false;
  if ((bool)scope_obj && !obj->IsChildOf(scope_obj))
    return false;
  return ShowItemFilter(scope_ref, obj, obj->GetName());
}

int taiWidgetTokenChooser::BuildChooser_0(iDialogItemChooser* ic, TypeDef* td,
                                          QTreeWidgetItem* top_item)
{
  if(!td || !td->IsActualTaBase())
    return 0;
  int rval = 0;
  
  //NOTES:
  // if !tokens.keep then tokens.size==0

  for (int i = 0; i < td->tokens.size; ++i) {
    taBase* btmp = (taBase*)td->tokens.FastEl(i);
    if (!ShowToken(btmp, td, i)) continue;
    // note: IMPORTANT to put all the show logic in one method, shared by count tokens

    // todo: need to get a more globally unique name, maybe key_unique_name
    QTreeWidgetItem* item = ic->AddItem
      (btmp->GetColText(taBase::key_disp_name).elidedTo(TOKEN_CHOOSER_NAME_MAX_LEN),
       top_item, (void*)btmp);
    item->setData(1, Qt::DisplayRole, btmp->GetTypeDef()->name);
    taBase* own = btmp->GetParent();
    if (own) {
      item->setData(2, Qt::DisplayRole, own->GetDisplayName()); // use disp name directly -- overriden to name for groups..
      item->setData(3, Qt::DisplayRole, own->GetColText(taBase::key_type));
    }
    item->setData(4, Qt::DisplayRole, btmp->DisplayPath());
    ++rval;
  }

  // IMPORTANT: any changes here should also be made in countTokensToN above!!
  for (int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    if(!chld) continue;         // shouldn't happen
    if(chld->HasOption("NO_TOKEN_CHOOSER"))
      continue;                                 // exclude from any implicit use
    rval += BuildChooser_0(ic, chld, top_item); //note: we don't create subnodes
  }
  return rval;
}

int taiWidgetTokenChooser::columnCount(int view) const {
  switch (view) {
  case 0: return 5;
  default: return 0; // not supposed to happen
  }
}

void taiWidgetTokenChooser::GetImage(void* cur_sel_, TypeDef* targ_typ_)
{//NOTE: this routine is needed in case clients call the old GetImage renamed to GetImageScoped
  // it is actually called by iDialogItemChooser::Refresh which happens during
  // user activation -- just use the current values and DON'T reset anything!!
  inherited::GetImage(cur_sel_, targ_typ_);
}

void taiWidgetTokenChooser::GetImageScoped(taBase* ths, TypeDef* targ_typ_,
  taBase* scope_, TypeDef* scope_type_)
{
  scope_obj.set(NULL);
  scope_ref = scope_;
  scope_typ = scope_type_;
  inherited::GetImage((void*)ths, targ_typ_);
}

void taiWidgetTokenChooser::GetImageScopeObj(taBase* ths, TypeDef* targ_typ_,
  taBase* scope_obj_)
{
  scope_obj = scope_obj_;
  scope_ref.set(NULL);
  scope_typ = NULL;
  inherited::GetImage((void*)ths, targ_typ_);
}

const String taiWidgetTokenChooser::headerText(int index, int view) const {
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

const String taiWidgetTokenChooser::labelNameNonNull() const {
  return token()->GetDisplayName().elidedTo(TOKEN_CHOOSER_NAME_MAX_LEN); // not 
}

const String taiWidgetTokenChooser::viewText(int index) const {
  switch (index) {
  case 0: return "Flat List";
  default: return _nilString;
  }
}


