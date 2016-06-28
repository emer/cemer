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

#include "taiEditorWidgets.h"
#include <taProject>
#include <ControlPanel>
#include <ParamSet>
#include <iLabel>
#include <taiWidget>
#include <iFlowLayout>
#include <iTextBrowser>
#include <iHelpBrowser>
#include <iDialogEditor>
#include <iHiLightButton>

#include <SigLinkSignal>
#include <taMisc>
#include <taiMisc>
#include <tabMisc>

#include <QMenu>
#include <QFrame>
#include <QVBoxLayout>
#include <QContextMenuEvent>


void taiEditorWidgets::DoFillLabelContextMenu_SelEdit(QMenu* menu,
  int& last_id, taBase* rbase, MemberDef* md, QWidget* menu_par,
  QObject* slot_obj, const char* slot)
{
// have to be a taBase to use SelEdit
  if (!rbase || !md) return;
  // get list of control panels
  taProject* proj = dynamic_cast<taProject*>(rbase->GetThisOrOwner(&TA_taProject));
  // if (!proj || proj->ctrl_panels.leaves == 0) return;

  // if any edits, populate menu for adding, for all seledits not already on
  QMenu* sub = menu->addMenu("Add to ControlPanel");
  connect(sub, SIGNAL(triggered(QAction*)), slot_obj, slot);
  sub->setFont(menu->font());
  QAction* act = NULL; // we need to track last one
  for (int i = 0; i < proj->ctrl_panels.leaves; ++i) {
    ControlPanel* se = proj->ctrl_panels.Leaf(i);
    act = sub->addAction(se->GetName()/*, slot_obj, slot*/); //
    act->setData(QVariant::fromValue((void*)se));
    if (se->FindMbrBase(rbase, md) >= 0)
      act->setEnabled(false);
  }
  if (sub->actions().count() == 0)
    sub->setEnabled(false); // show item for usability, but disable

  // TODO: if any edits, populate menu for removing, for all seledits already on
  sub = menu->addMenu("Remove from ControlPanel");
  connect(sub, SIGNAL(triggered(QAction*)), slot_obj, slot);
  sub->setFont(menu->font());
  for (int i = 0; i < proj->ctrl_panels.leaves; ++i) {
    ControlPanel* se = proj->ctrl_panels.Leaf(i);
    act = sub->addAction(se->GetName()/*, slot_obj, slot*/);
    act->setData(QVariant::fromValue((void*)se));
    if (se->FindMbrBase(rbase, md) < 0)
      act->setEnabled(false);
  }
  if (sub->actions().count() == 0)
    sub->setEnabled(false); // show item for usability, but disable

  sub = menu->addMenu("Add to ParamSet");
  connect(sub, SIGNAL(triggered(QAction*)), slot_obj, slot);
  sub->setFont(menu->font());
  act = NULL; // we need to track last one
  for (int i = 0; i < proj->param_sets.leaves; ++i) {
    ParamSet* ps = proj->param_sets.Leaf(i);
    act = sub->addAction(ps->GetName()/*, slot_obj, slot*/); //
    act->setData(QVariant::fromValue((void*)ps));
    if (ps->FindMbrBase(rbase, md) >= 0)
      act->setEnabled(false);
  }
  if (sub->actions().count() == 0)
    sub->setEnabled(false); // show item for usability, but disable

  // TODO: if any edits, populate menu for removing, for all seledits already on
  sub = menu->addMenu("Remove from ParamSet");
  connect(sub, SIGNAL(triggered(QAction*)), slot_obj, slot);
  sub->setFont(menu->font());
  for (int i = 0; i < proj->param_sets.leaves; ++i) {
    ParamSet* ps = proj->param_sets.Leaf(i);
    act = sub->addAction(ps->GetName()/*, slot_obj, slot*/);
    act->setData(QVariant::fromValue((void*)ps));
    if (ps->FindMbrBase(rbase, md) < 0)
      act->setEnabled(false);
  }
  if (sub->actions().count() == 0)
    sub->setEnabled(false); // show item for usability, but disable

  TypeDef* pstd = taMisc::FindTypeName("ParamStep", false);
  if(pstd) {
    sub = menu->addMenu("Add to ParamStep");
    connect(sub, SIGNAL(triggered(QAction*)), slot_obj, slot);
    sub->setFont(menu->font());
    act = NULL; // we need to track last one
    for (int i = 0; i < pstd->tokens.size; ++i) {
      void* tok = pstd->tokens[i];
      if(!tok) continue;
      ControlPanel* ps = (ControlPanel*)tok;
      if(!ps->SameScope(proj, &TA_taProject)) continue;
      act = sub->addAction(ps->GetName()/*, slot_obj, slot*/); //
      act->setData(QVariant::fromValue((void*)ps));
      if (ps->FindMbrBase(rbase, md) >= 0)
        act->setEnabled(false);
    }
    if (sub->actions().count() == 0)
      sub->setEnabled(false); // show item for usability, but disable

    sub = menu->addMenu("Remove from ParamStep");
    connect(sub, SIGNAL(triggered(QAction*)), slot_obj, slot);
    sub->setFont(menu->font());
    for (int i = 0; i < pstd->tokens.size; ++i) {
      void* tok = pstd->tokens[i];
      if(!tok) continue;
      ControlPanel* ps = (ControlPanel*)tok;
      if(!ps->SameScope(proj, &TA_taProject)) continue;
      act = sub->addAction(ps->GetName()/*, slot_obj, slot*/);
      act->setData(QVariant::fromValue((void*)ps));
      if (ps->FindMbrBase(rbase, md) < 0)
        act->setEnabled(false);
    }
    if (sub->actions().count() == 0)
      sub->setEnabled(false); // show item for usability, but disable
  }
}

void taiEditorWidgets::GetName(MemberDef* md, String& name, String& help_text) {
  name = md->GetLabel();
  help_text = ""; // just in case
  MemberDef::GetMembDesc(md, help_text, "");
}

taiEditorWidgets::taiEditorWidgets(TypeDef* typ_, bool read_only_, bool modal_, QObject* parent)
:inherited(typ_, read_only_, modal_, parent)
{
  InitGuiFields(false);

  sel_item_dat = NULL;
  sel_item_mbr = NULL;
  sel_item_base = NULL;
  rebuild_body = false;
  sel_edit_mbrs = true; // inherited guys can turn off
}

taiEditorWidgets::~taiEditorWidgets() {
}

// note: called non-virtually in our ctor, and virtually in WidgetDeleting
void taiEditorWidgets::InitGuiFields(bool virt) {
  if (virt)  inherited::InitGuiFields(virt);
  frmMethButtons = NULL;
  layMethButtons = NULL;
  show_meth_buttons = false;
}

const iColor taiEditorWidgets::colorOfRow(int row) const {
  if ((row % 2) == 0) {
    return bg_color;
  } else {
    return bg_color_dark;
  }
}

void taiEditorWidgets::Cancel_impl() { //note: taiEditorOfClass takes care of cancelling panels
  inherited::Cancel_impl();
  // delete any methods
  if (frmMethButtons) {
    QWidget* t = frmMethButtons; // avoid any possible callback issues
    frmMethButtons = NULL;
    delete t;
  }

  warn_clobber = false; // just in case
}

void taiEditorWidgets::Constr_Methods() {
  Constr_Methods_impl();
}

void taiEditorWidgets::Constr_Methods_impl() { //note: conditional constructions used by ControlPanelHost to rebuild methods
  QFrame* tmp = frmMethButtons;
  if (!frmMethButtons) {
    show_meth_buttons = false; // set true if any created
    tmp = new QFrame(); // tmp = new QFrame(widget());
    tmp->setVisible(false); // prevents it showing as global win in some situations
    tmp->setAutoFillBackground(true); // for when disconnected from us
    QPalette pal = tmp->palette();
    pal.setColor(QPalette::Background, bg_color);
    tmp->setPalette(pal); 
    tmp->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    tmp->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
  }
  if (!layMethButtons) {
    layMethButtons = new iFlowLayout(tmp, 3, taiM->hspc_c, (Qt::AlignCenter)); // margin, space, align
  }
  if (!frmMethButtons) {
    frmMethButtons = tmp;
    //TODO: if this is a ControlPanel, and this is a rebuild for first time,
    // we will need to do the Insert_Methods call
  }
}

taBase* taiEditorWidgets::GetMembBase_Flat(int) {
  return (taBase*)root; // pray!
}

taBase* taiEditorWidgets::GetMethBase_Flat(int) {
  return (taBase*)root; // pray!
}

void taiEditorWidgets::Insert_Methods() {
  //NOTE: for taiEditorOfClass, menus are always put in widget() even in deferred
  if (frmMethButtons && !frmMethButtons->parentWidget()) {
    // meth buttons always at bottom of inner layout
    vblDialog->addSpacing(2);
    vblDialog->addWidget(frmMethButtons);
    frmMethButtons->setVisible(show_meth_buttons); // needed for deferred insert
  }
}

void taiEditorWidgets::SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) {
//note: nothing in base, by design
  //NOTE: list/group subclasses typically detect changes in their GetImage routine
  //  so we don't really subclass this routine or explicitly detect the list/group notifies
  // note: because of deferred construction, we may still need to update buttons/menus

  if (state == DEFERRED1) {
    Refresh_impl(false);
    return;
  }
  // note: we should have unlinked if cancelled, but if not, ignore if cancelled
  if (!isConstructed()) return;

  if (updating) return; // it is us that caused this
  if (sls == SLS_STRUCT_UPDATE_END) {
    Refresh_impl(true);
  }
  // RESOLVE_NOW is typically invoked by a button method
  else if (sls == SLS_RESOLVE_NOW)
  {
    CancelOp cancel_op = CO_PROCEED; //note: may make more sense to be CO_NOT_CANCELLABLE
    ResolveChanges(cancel_op);
  }
  // we really just want to ignore the BEGIN-type guys, otherwise accept all others
  else if (!((sls == SLS_STRUCT_UPDATE_BEGIN) ||
    (sls == SLS_DATA_UPDATE_BEGIN)))
  {
    Refresh_impl(false);
  }
}

void taiEditorWidgets::Refresh_impl(bool reshow) {
  // if no changes have been made in this instance, then just refresh,
  // otherwise, user will have to decide what to do, i.e., revert
  if (HasChanged()) {
    warn_clobber = true;
    if (reshow) defer_reshow_req = true; // if not already set
  } else {
    if (reshow) {
//      if (tabMisc::delayed_funcalls.size == 0) {
        ReShow();                 // this must NOT be _Async -- otherwise doesn't work with carbon qt on mac
//      }
    }
    else {
      GetImage_Async();
    }
  }
}

void taiEditorWidgets::label_contextMenuInvoked(iLabel* sender, QContextMenuEvent* e) {
  QMenu* menu = new QMenu(widget());
  //note: don't use body for menu parent, because some context menu choices cause ReShow, which deletes body items!
  Q_CHECK_PTR(menu);
  int last_id = -1;
  sel_item_dat = (taiWidget*)qvariant_cast<ta_intptr_t>(sender->userData()); // pray!!!
  if (sel_item_dat) {
    sel_item_mbr = sel_item_dat->mbr;
    sel_item_base = sel_item_dat->Base();
  } else {
    sel_item_mbr = NULL;
    sel_item_base = NULL;
  }
  FillLabelContextMenu(menu, last_id);
  if (menu->actions().count() > 0)
    menu->exec(sender->mapToGlobal(e->pos()));
  delete menu;
}

void taiEditorWidgets::FillLabelContextMenu(QMenu* menu, int& last_id) {
  // only add member help if exists
  if (sel_item_mbr) {
    menu->addAction("&Help", this, SLOT(helpMenu_triggered()));
  }
}

void taiEditorWidgets::helpMenu_triggered() {
  iHelpBrowser::StatLoadMember(sel_item_mbr);
}

void taiEditorWidgets::Iconify(bool value) {
  if (!dialog) return;
  if (value) dialog->iconify();
  else       dialog->deiconify();
}

void taiEditorWidgets::Ok_impl() { //note: only used for Dialogs
  // NOTE: we herein might be bypassing the clobber warn, but shouldn't really
  //be possible to modify ourself externally inside a dialog
  inherited::Ok_impl();
  if (HasChanged()) {
    GetValue();
    Unchanged();
  }
}

void taiEditorWidgets::ClearBody(bool waitproc) {
  StartEndLayout(true);
  ClearBody_impl();
  if (!(state & SHOW_CHANGED)) return; // probably just destroying
  // can actually just do this live here
  ReConstr_Body();
  state &= ~SHOW_CHANGED;
  reconstr_req = false;
  StartEndLayout(false);
}

void taiEditorWidgets::ClearBody_impl() {
  taiMisc::DeleteChildrenLater(body);
}

void taiEditorWidgets::ReConstr_Body() {
  if (!isConstructed()) return;
  // the "taMisc" exception is to get hidden and expert preferences to appear
  // because they are in a dialog and not a panel the link is NULL
  // see bug 2581 (rohrlich 4/11/16)
  if (!link() && typ->name != "taMisc" ) return;
  rebuild_body = true;
  ++updating;                   // prevents spurious changed flags from coming in
  Constr_Body();
  {             // this is key for controlpanel rebuilding on bool toggles, for example
    taMisc::RunPending();
    taMisc::RunPending();
    taMisc::RunPending();
  }
  GetImage_Async();             // async all the way -- otherwise doesn't work
//   GetImage(false);
  rebuild_body = false;         // in case..
  --updating;
}

bool taiEditorWidgets::ReShow(bool force) {
// if not visible, we may refresh the buttons if visible, otherwise nothing else
  if (!mwidget) return false;//. huh?
  //note: extremely unlikely to be updating if invisible, so we do this test here
  if (!mwidget->isVisible()) {
    defer_reshow_req = true;
    GetImage(false); // no-force; invisible-friendly
    return false;    // that's it -- otherwise clears when not visible! -- absence of this was source of major crazy bug fixed 11/18/09..
  }
//note: only called with force from ReShowEdits, typ only from a SelEdit dialog
  if (!updating) {
    // was not us that caused this...
    if (force) {
        // get confirmation if changed, and possibly exit
      if (HasChanged()) {
        int chs = taMisc::Choice("Changes must be applied before rebuilding", "&Apply", "&Cancel");
        switch (chs) {
        case  1: // just ignore and exit
          return false;
          break;
        case  0:
        default:
          Apply();
          break;
        }
      }
    } else { // not forced, normal situation for datachanged notifies
      if (HasChanged()) {
        warn_clobber = true;
        defer_reshow_req = true;
        return false;
      }
    }
  }
  state |= SHOW_CHANGED;
  ClearBody(); // rebuilds body after clearing -- but SHOW_CHANGED prevents GetImage...
  defer_reshow_req = false; // if it was true
  return true;
}

TypeItem::ShowMembs taiEditorWidgets::show() const {
  return taMisc::show_gui;
}

void taiEditorWidgets::SetRevert(){
  if (updating || (taMisc::is_loading)) return;
  if (!revert_but) return;
  revert_but->setHiLight(true);
  revert_but->setEnabled(true);
}

void taiEditorWidgets::UnSetRevert(){
  if (!revert_but) return;
  revert_but->setHiLight(false);
  revert_but->setEnabled(false);
}

