// Co2018ght 2013-2017, Regents of the University of Colorado,
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


void taiEditorWidgets::DoFillLabelContextMenu_CtrlPanel
(QMenu* menu, int& last_id, taBase* rbase, MemberDef* md, QWidget* menu_par,
 QObject* slot_obj, const char* add_slot, const char* rmv_slot, const char* add_short_slot,
   const char* goto_slot)
{
  // have to be a taBase to use ControlPanel
  if (!rbase || !md) return;
  // get list of control panels
  taProject* proj = (taProject*)rbase->GetThisOrOwner(&TA_taProject);
  if (!proj) return;

  // no sense having args on control panels other than the program's own control panel
  if(rbase->InheritsFrom(&TA_ProgVar)) {
    ProgVar* prog_var = (ProgVar*)rbase;
    if (prog_var && prog_var->HasVarFlag(ProgVar::PGRM_ARG)) {
      return;
    }
  }

  QAction* act = menu->addAction("Go To ControlPanel", slot_obj, goto_slot);
  if(!rbase->MemberControlPanel(md->name)) {
    act->setEnabled(false);
  }

  // if any edits, populate menu for adding, for all ctrl panels not already on
  QMenu* add_sub = menu->addMenu("Add to ControlPanel");
  connect(add_sub, SIGNAL(triggered(QAction*)), slot_obj, add_slot);
  add_sub->setFont(menu->font());
  QAction* add_act = NULL; // we need to track last one

  QMenu* add_short_sub = NULL;
  QAction* add_short_act = NULL; // we need to track last one
  if(add_short_slot) {
    add_short_sub = menu->addMenu("Add to ControlPanel (short label)");
    connect(add_short_sub, SIGNAL(triggered(QAction*)), slot_obj, add_short_slot);
    add_short_sub->setFont(menu->font());
  }

  QMenu* rmv_sub = menu->addMenu("Remove from ControlPanel");
  connect(rmv_sub, SIGNAL(triggered(QAction*)), slot_obj, rmv_slot);
  rmv_sub->setFont(menu->font());
  QAction* rmv_act = NULL; // we need to track last one

  // todo: would be cleaner to generalize this -- also used in taiEditorOfControlPanelFull
  const int n_types = 7;
  const char* type_names[n_types] = {"ControlPanel", "ClusterRun", "ParamSet",
                                     "ParamStep", "ControlPanel_Group", "ParamSet_Group",
                                     "ParamStep_Group"
  };

  for(int ti=0; ti<n_types; ti++) {
    TypeDef* cptd = taMisc::FindTypeName(type_names[ti], false);
    if(cptd) {
      for (int i = 0; i < cptd->tokens.size; ++i) {
        void* tok = cptd->tokens[i];
        if(!tok) continue;
        taBase* cp = (taBase*)tok;
        if(!cp->SameScope(proj, &TA_taProject)) continue;
        taBase* agp = cp->GetOwner(&TA_ArchivedParams_Group);
        if(agp) continue;       // skip all archived elements
        String nm = cp->GetName();
        bool is_cp = false;
        ControlPanel* cpr = NULL;
        ControlPanel_Group* gp = NULL;
        if(cp->InheritsFrom(&TA_ControlPanel)) {
          is_cp = true;
          cpr = (ControlPanel*)cp;
          if(cpr->IsClone()) continue; // don't clutter with clones!
        }
        else {                  // group
          gp = (ControlPanel_Group*)cp; // could omit master/clone groups but may be easier
          // to think in terms of group or master element, depending..
          if(gp == &(proj->active_params)) continue;
          nm += " (Group)";
        }
        add_act = add_sub->addAction(nm);
        add_act->setData(QVariant::fromValue((void*)cp));
        if(add_short_sub) {
          add_short_act = add_short_sub->addAction(nm);
          add_short_act->setData(QVariant::fromValue((void*)cp));
        }
        rmv_act = rmv_sub->addAction(nm);
        rmv_act->setData(QVariant::fromValue((void*)cp));
        if(is_cp) {
          if (cpr->FindMbrBase(rbase, md) >= 0) {
            add_act->setEnabled(false);
            if(add_short_act) {
              add_short_act->setEnabled(false);
            }
          }
          else {
            rmv_act->setEnabled(false);
          }
        }
        else {
          if(gp->leaves > 0) {
            ControlPanel* cprl = gp->Leaf(0);
            if (cprl->FindMbrBase(rbase, md) >= 0) {
              add_act->setEnabled(false);
              if(add_short_act) {
                add_short_act->setEnabled(false);
              }
            }
            else {
              rmv_act->setEnabled(false);
            }
          }
        }
      }
    }
  }
  if (add_sub->actions().count() == 0)
    add_sub->setEnabled(false); // show item for usability, but disable
  if(add_short_sub) {
    if (add_short_sub->actions().count() == 0)
      add_short_sub->setEnabled(false); // show item for usability, but disable
  }
  if (rmv_sub->actions().count() == 0)
    rmv_sub->setEnabled(false); // show item for usability, but disable
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

  ctrl_panel_dat = NULL;
  ctrl_panel_mbr = NULL;
  ctrl_panel_base = NULL;
  rebuild_body = false;
  ctrl_panel_mbrs = true; // inherited guys can turn off
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
  ctrl_panel_dat = (taiWidget*)qvariant_cast<ta_intptr_t>(sender->userData()); // pray!!!
  if (ctrl_panel_dat) {
    ctrl_panel_mbr = ctrl_panel_dat->mbr;
    ctrl_panel_base = ctrl_panel_dat->Base();
  } else {
    ctrl_panel_mbr = NULL;
    ctrl_panel_base = NULL;
  }
  FillLabelContextMenu(menu, last_id);
  if (menu->actions().count() > 0) {
    taMisc::in_eventproc++;       // this is an event proc!
    menu->exec(sender->mapToGlobal(e->pos()));
    taMisc::in_eventproc--;
  }
  delete menu;
}

void taiEditorWidgets::FillLabelContextMenu(QMenu* menu, int& last_id) {
  // only add member help if exists
  if (ctrl_panel_mbr) {
    menu->addAction("&Help", this, SLOT(helpMenu_triggered()));
  }
}

void taiEditorWidgets::helpMenu_triggered() {
  iHelpBrowser::StatLoadMember(ctrl_panel_mbr);
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

  if(typ->IsTaBase() && !link()) return;
  // non taBase objects have NULL link but need to reconstr to see Expert / Hidden
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

