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

#include "ISelectable.h"

ISelectable::~ISelectable() {
  ISelectableHost::ItemDeleting(this);
}

QObject* ISelectable::clipHandlerObj() const {
  ISelectableHost* host_ = host();
  return (host_) ? host_->clipHandlerObj() : NULL;
}

taiDataLink* ISelectable::clipParLink(GuiContext sh_typ) const {
  return par_link();
}

// called from ui to handle drops
void ISelectable::DropHandler(const QMimeData* mime, const QPoint& pos,
    int mods, int where)
{
//Note: on Mac, "Ctrl" and test bits always refer to Command key (not Ctrl key)
  taiMimeSource* ms = taiMimeSource::New(mime);
  ISelectableHost* host_ = host(); //cache

  // set for the menu callbacks
  host_->ctxt_ms = ms;
  host_->ctxt_item = this;

  int ea = QueryEditActions_(ms, GC_DEFAULT);
  int key_mods = mods & (Qt::ShiftModifier | Qt::ControlModifier |
    Qt::AltModifier);
  // only honor if user has chosen 1 and only 1 mod
  // and its shortcut ops is ambiguous and available
  if (key_mods == Qt::ShiftModifier) { // Move
    if ((ea & taiClipData::EA_DROP_MOVE2) == taiClipData::EA_DROP_MOVE2)
      goto show_menu;
    host_->helperObj()->DropEditAction(ea & taiClipData::EA_DROP_MOVE2); // is only one or the other
    goto exit;
  } else if (key_mods == Qt::ControlModifier) { // Copy
    if ((ea & taiClipData::EA_DROP_COPY2) == taiClipData::EA_DROP_COPY2)
      goto show_menu;
    host_->helperObj()->DropEditAction(ea & taiClipData::EA_DROP_COPY2); // is only one or the other
    goto exit;
  } else if (key_mods == Qt::AltModifier) { // Link
    if ((ea & taiClipData::EA_DROP_LINK2) == taiClipData::EA_DROP_LINK2)
      goto show_menu;
    host_->helperObj()->DropEditAction(ea & taiClipData::EA_DROP_LINK2); // is only one or the other
    goto exit;
  }


  // always show menu, for consistency
  // all following implicitly use the GC_DEFAULT code
show_menu:
  { // block for jump
  taiMenu* menu = new taiMenu(widget(), taiMenu::normal, 0);
  QAction* act = NULL;
  // when dropping "at end", we force the term "here" instead of "into"
  // but we still use the INTO semantics
  bool force_here = (where == iTreeWidgetItem::WI_AT_END);
  String IntoOrHere = force_here ? "Here" : "Into";
// NOTE: "Xxx+" shortcuts are dummies, to hint the shortcircuit drop key
// if has both variants of an op, no shortcuts and both, else yes
// NOTE: for the "between/end" cases, we can't allow the "Into" (ie,
//  typically for a group) because the situation is ambiguous, and
// the result is not what might be expected, since it gets put into
// the next item, not as a child of the previous item
  if ((ea & taiClipData::EA_DROP_MOVE2) == taiClipData::EA_DROP_MOVE2) {
    act = menu->AddItem("Move Here", taiAction::int_act,
      host_->helperObj(),  SLOT(DropEditAction(int)),
      taiClipData::EA_DROP_MOVE, QKeySequence());
    if (where == iTreeWidgetItem::WI_ON) {
      act = menu->AddItem("Move Into", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_MOVE_INTO, QKeySequence());
    }
  } else {
    if (ea & taiClipData::EA_DROP_MOVE)
      act = menu->AddItem("&Move Here", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_MOVE, QKeySequence("Shift+"));
    else if (ea & taiClipData::EA_DROP_MOVE_INTO)
      act = menu->AddItem("&Move "+IntoOrHere, taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_MOVE_INTO, QKeySequence("Shift+"));
  }

  act = NULL;
  if ((ea & taiClipData::EA_DROP_COPY2) == taiClipData::EA_DROP_COPY2) {
    act = menu->AddItem("Copy Here", taiAction::int_act,
      host_->helperObj(),  SLOT(DropEditAction(int)),
      taiClipData::EA_DROP_COPY, QKeySequence());
    if (where == iTreeWidgetItem::WI_ON) {
      act = menu->AddItem("Copy Into", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_COPY_INTO, QKeySequence());
    }
  } else {
    if (ea & taiClipData::EA_DROP_COPY)
      act = menu->AddItem("&Copy Here", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_COPY, QKeySequence("Ctrl+"));
    else if (ea & taiClipData::EA_DROP_COPY_INTO)
      act = menu->AddItem("&Copy "+IntoOrHere, taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_COPY_INTO, QKeySequence("Ctrl+"));
  }

  act = NULL;
  // Assign only applicable for "On" drops
  if ((where == iTreeWidgetItem::WI_ON) &&
     (ea & taiClipData::EA_DROP_ASSIGN))
  {
    act = menu->AddItem("Assign To", taiAction::int_act,
      host_->helperObj(),  SLOT(DropEditAction(int)),
      taiClipData::EA_DROP_ASSIGN, QKeySequence());
  }

  act = NULL;
  if ((ea & taiClipData::EA_DROP_LINK2) == taiClipData::EA_DROP_LINK2) {
    act = menu->AddItem("Link Here", taiAction::int_act,
      host_->helperObj(),  SLOT(DropEditAction(int)),
      taiClipData::EA_DROP_LINK, QKeySequence());
    if (where == iTreeWidgetItem::WI_ON) {
      act = menu->AddItem("Link Into", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_LINK_INTO, QKeySequence());
    }
  } else {
    if (ea & taiClipData::EA_DROP_LINK)
      act = menu->AddItem("&Link Here", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_LINK, QKeySequence("Alt+"));
    else if (ea & taiClipData::EA_DROP_LINK_INTO)
      act = menu->AddItem("&Link "+IntoOrHere, taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_LINK_INTO, QKeySequence("Alt+"));
  }

  // if any appropriate drop actions, then add them!
  menu->AddSep();
  host_->UpdateMethodsActionsForDrop();
  host_->AddDynActions(menu, 0);

  menu->AddSep();
  act = menu->AddItem("C&ancel", -1);
  act->setShortcut(QKeySequence("Esc"));

  // get current mouse position
  QPoint men_pos = widget()->mapToGlobal(pos);

  act = menu->menu()->exec(men_pos);
  //menu->deleteLater();
  delete menu;
  } // block for jump
exit:
  host_->ctxt_ms = NULL;
  host_->ctxt_item = NULL;
  delete ms;
}

taiDataLink* ISelectable::effLink(GuiContext sh_typ) const {
  if (sh_typ == GC_DEFAULT)
    sh_typ = shType();
  if (sh_typ <= GC_DUAL_DEF_DATA) return link();
  return viewLink();
}

void ISelectable::FillContextMenu(ISelectable_PtrList& sel_items,
  taiActions* menu, GuiContext sh_typ)
{
  FillContextMenu_impl(menu, sh_typ);
  int allowed = QueryEditActions_(sel_items, sh_typ);
  FillContextMenu_EditItems_impl(menu, allowed, sh_typ);
  if (sel_items.size == 1) {
    taiDataLink* link = this->effLink(sh_typ);
    if (link) link->FillContextMenu(menu);
  }
}

void ISelectable::FillContextMenu_EditItems_impl(taiActions* menu,
  int ea, GuiContext sh_typ)
{
  if (ea == 0) return;
  if (menu->count() > 0)
    menu->AddSep();
//  cut copy paste link delete
  taiAction* mel;
  if (ea & taiClipData::EA_CUT) {
    mel = menu->AddItem("Cu&t", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_CUT;
    mel->setData(sh_typ);
  }
  if (ea & taiClipData::EA_COPY) {
    mel = menu->AddItem("&Copy", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_COPY;
    mel->setData(sh_typ);
  }
  if (ea & taiClipData::EA_DUPE) {
    mel = menu->AddItem("Duplicate  (Ctrl+M)", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_DUPE;
    mel->setData(sh_typ);
  }

  // Paste and Link guys are slightly complicated, because we can have
  // OP/OP_INTO variants, so we can't have shortcuts with both
  int paste_cnt = 0;
  if (ea & taiClipData::EA_PASTE) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_INTO) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_ASSIGN) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_APPEND) ++paste_cnt;
  String txt;
  if (ea & taiClipData::EA_PASTE) {
    if (paste_cnt > 1) txt = "Paste"; else txt = "&Paste";
    mel = menu->AddItem(txt, taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_PASTE;
    mel->setData(sh_typ);
  }
  if (ea & taiClipData::EA_PASTE_INTO) {
    if (paste_cnt > 1) txt = "Paste Into"; else txt = "&Paste Into";
    mel = menu->AddItem(txt, taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_PASTE_INTO;
    mel->setData(sh_typ);
  }
  if (ea & taiClipData::EA_PASTE_ASSIGN) {
    if (paste_cnt > 1) txt = "Paste Assign"; else txt = "&Paste Assign";
    mel = menu->AddItem(txt, taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_PASTE_ASSIGN;
    mel->setData(sh_typ);
  }
  if (ea & taiClipData::EA_PASTE_APPEND) {
    if (paste_cnt > 1) txt = "Paste Append"; else txt = "&Paste Append";
    mel = menu->AddItem(txt, taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_PASTE_APPEND;
    mel->setData(sh_typ);
  }

  if (ea & taiClipData::EA_DELETE) {
    mel = menu->AddItem("&Delete  (Ctrl+D)", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_DELETE;
    mel->setData(sh_typ);
  }
  if ((ea & taiClipData::EA_LINK) == taiClipData::EA_LINK) {
    mel = menu->AddItem("Link", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_LINK;
    mel->setData(sh_typ);
    mel = menu->AddItem("Link Into", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_LINK_INTO;
    mel->setData(sh_typ);
  }
  else if (ea & taiClipData::EA_LINK) {
    mel = menu->AddItem("&Link", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_LINK;
    mel->setData(sh_typ);
  }
  else if (ea & taiClipData::EA_LINK_INTO) {
    mel = menu->AddItem("&Link Into", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_LINK_INTO;
    mel->setData(sh_typ);
  }

  if (ea & taiClipData::EA_UNLINK) {
    mel = menu->AddItem("&Unlink", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_UNLINK;
    mel->setData(sh_typ);
  }
  taiDataLink* link = this->effLink(sh_typ);
  if (link) link->FillContextMenu_EditItems(menu, ea);
}

taiClipData* ISelectable::GetClipData(const ISelectable_PtrList& sel_items,
  int src_edit_action, bool for_drag, GuiContext sh_typ) const
{
  if (sel_items.size <= 1) {
    return GetClipDataSingle(src_edit_action, for_drag, sh_typ);
  } else {
    return GetClipDataMulti(sel_items,src_edit_action, for_drag, sh_typ);
  }
}

void ISelectable::GetContextCaptions(String& view_cap, String& obj_cap) {
  taBase* tab = taData(GC_DUAL_DEF_VIEW);
  if (tab) view_cap = tab->GetTypeName();
  tab = taData(GC_DUAL_DEF_DATA);
  if (tab) obj_cap = tab->GetTypeName();
}

TypeDef* ISelectable::GetEffDataTypeDef(GuiContext sh_typ) const {
  taDataLink* link_ = (taDataLink*)effLink(sh_typ);
  return (link_) ? link_->GetDataTypeDef() : NULL;
}

taiDataLink* ISelectable::own_link(GuiContext sh_typ) const {
  taiDataLink* link = this->effLink(sh_typ);
  return (link) ? link->ownLink() : NULL;
}

MemberDef* ISelectable::par_md() const {
  // to get the par_md, we have to go up the grandparent, and ask for member addr
  MemberDef* rval = NULL;
  taBase* par_tab = NULL; //note: still only got the guy, not par
  taBase* gpar_tab = NULL;
  taiDataLink* link = this->effLink();
  if (!link) goto exit;
  par_tab = link->taData(); //note: still only got the guy, not par
  if (!par_tab) goto exit;
  par_tab = par_tab->GetOwner(); // now we have par
  if (!par_tab) goto exit;
  gpar_tab = par_tab->GetOwner();
  if (!gpar_tab) goto exit;
  rval = gpar_tab->FindMemberPtr(par_tab);
exit:
   return rval;
}

taiDataLink* ISelectable::par_link() const {
  ISelectable* par = this->par();
  if (par) return par->link();
  else     return NULL;
}

// called from Ui for cut/paste etc. -- not called for drag/drop ops
int ISelectable::EditAction_(ISelectable_PtrList& sel_items, int ea,
  GuiContext sh_typ)
{
  taiMimeSource* ms = NULL;
  taiClipData* cd = NULL;
  int rval = taiClipData::ER_IGNORED; //not really used, but 0 is ignored, 1 is done, -1 is forbidden, -2 is error

  // get the appropriate data, either clipboard data, or item data, depending on op
  if  (ea & (taiClipData::EA_SRC_OPS)) { // no clipboard data
    // we handle cut and copy
    if ((ea & (taiClipData::EA_CUT | taiClipData::EA_COPY))) { // copy-like op, get item data
      cd = GetClipData(sel_items, taiClipData::ClipOpToSrcCode(ea), false, sh_typ);
      // note that a Cut is a Copy, possibly followed later by a xxx_data_taken command, if client pastes it
      QApplication::clipboard()->setMimeData(cd, QClipboard::Clipboard);
      cd = NULL; // clipboard now owns it
      rval = taiClipData::ER_OK;
      // clear the emacs-like extended selection after copy -- makes it clear that copy
      // took effect too
      iMainWindowViewer* imwv = host()->mainWindow();
      if(imwv) {
        imwv->GetCurTreeView()->clearExtSelection();
      }
    }
    else { // other ops, like Duplicate, Clear or Unlink
      taBase* tab = taData();
      taProject* proj = NULL;
      if(tab) {
        proj = (taProject*)tab->GetThisOrOwner(&TA_taProject);
        if (proj) {
          proj->undo_mgr.Nest(true);
        }
      }
      bool multi = false;
      bool multi_off = false;
      if(sel_items.size > 1) {
        ++taMisc::in_gui_multi_action;
        multi = true;
      }
      for (int i = 0; i < sel_items.size; ++i) {
        if(multi && i == sel_items.size-1) {
          --taMisc::in_gui_multi_action;
          multi_off = true;
        }
        ISelectable* is = sel_items.SafeEl(i);
        if (!is) continue;
        int trval = is->EditActionS_impl_(ea, GC_DEFAULT);
        if (trval == 0) continue;
        rval = trval;
        if (rval < 0) break; // forbidden or error
      }
      if(multi && !multi_off) // didn't reach end
        --taMisc::in_gui_multi_action;
      if (proj) {
        proj->undo_mgr.Nest(false);
      }
    }
  }
  else { // paste-like op, get item data
    // confirm only 1 item selected for dst op -- Error is diagnostic, not operational
    if (sel_items.size > 1) {
      taMisc::Error("Paste-like clip operations only allowed for a single dest item");
    } else {
      ms = taiMimeSource::New(QApplication::clipboard()->mimeData(QClipboard::Clipboard));
      rval = EditActionD_impl_(ms, ea, sh_typ);
    }
  }
  if (ms) delete ms;
  if (cd) delete cd;
  return rval;
}

int ISelectable::QueryEditActions_(taiMimeSource* ms, GuiContext sh_typ) const {
  int allowed = 0;
  int forbidden = 0;
  // if src is readonly, then forbid certain dst ops
  if (ms->srcAction() & taiClipData::EA_SRC_READONLY)
    forbidden |= taiClipData::EA_FORB_ON_SRC_READONLY;
  QueryEditActionsD_impl_(ms, allowed, forbidden, sh_typ);
  return (allowed & (~forbidden));
}

int ISelectable::QueryEditActions_(const ISelectable_PtrList& sel_items,
  GuiContext sh_typ) const
{
  int allowed = 0;
  int forbidden = 0;
  if (sel_items.size <= 1) { // single select
    QueryEditActionsS_impl_(allowed, forbidden, sh_typ);
    taiMimeSource* ms = taiMimeSource::New(
      QApplication::clipboard()->mimeData(QClipboard::Clipboard));
    QueryEditActionsD_impl_(ms, allowed, forbidden, sh_typ);
    delete ms;
  } else { // multi select -- no dst ops allowed
    int allowed_accum = 0; // add allowed to this guy
    int allowed_knockout = -1; // subtract allowed from this guy
    for (int i = 0; i < sel_items.size; ++i) {
      ISelectable* is = sel_items.SafeEl(i);
      if (is) {
        int item_allowed = 0;
        is->QueryEditActionsS_impl_(item_allowed, forbidden, GC_DEFAULT);
        allowed_accum |= item_allowed;
        allowed_knockout &= item_allowed;
      }
    }
    allowed |= (allowed_accum & allowed_knockout);
  }
  return (allowed & (~forbidden));
}

taBase* ISelectable::taData(GuiContext sh_typ) const {
  taiDataLink* link = this->effLink(sh_typ);
  if (link)
    return link->taData();
  else return NULL;
}

QWidget* ISelectable::widget() const {
  ISelectableHost* host_ = host();
  return (host_) ? host_->widget() : NULL;
}

