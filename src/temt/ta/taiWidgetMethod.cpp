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

#include "taiWidgetMethod.h"
#include <IWidgetHost>
#include <taiWidgetActions>
#include <taProject>
#include <taiArgType>
#include <taFiler>

#include <css_machine.h>
#include <css_qtdialog.h>
#include <css_basic_types.h>
#include <css_ta.h>

#include <SigLinkSignal>
#include <taMisc>
#include <taiMisc>

#include <QToolButton>

taTypeDef_Of(istream);
taTypeDef_Of(fstream);
taTypeDef_Of(ostream);

void taiWidgetMethod::ShowReturnVal(cssEl* rval, IWidgetHost* host,
  const String& meth_name)
{
  if ((rval->GetType() == cssEl::T_TA) || (rval->GetType() == cssEl::T_Class)) {
    if (host != NULL)
      rval->Edit(host->isModal());
    else
      rval->Edit(false);
    rval->Edit(true); // always show modally
    return;
  }
  String val = meth_name + " Return Value: \n";
  val += rval->PrintStr();
  taMisc::Confirm(val);
}

taiWidgetMethod::taiWidgetMethod(void* bs, MethodDef* md, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
                             QWidget* gui_parent_, int flags_)
  : taiWidget(typ_, host_, par, gui_parent_, flags_)
{
  base = bs;
  meth = md;
  gui_parent = gui_parent_;
  if(base && typ && typ->IsActualTaBase()) {
    typ = ((taBase*)base)->GetTypeDef(); // get the *actual* type def of this object!
  }
  is_menu_item = false;
  args = NULL;
  arg_dlg = NULL;
  use_argc = 0;
  buttonRep = NULL;
}

void taiWidgetMethod::AddToMenu(taiWidgetActions* mnu) {
  if (meth->HasOption("MENU_SEP_BEFORE"))
    mnu->AddSep();
  iAction* act = mnu->AddItem(meth->GetLabel(), taiWidgetMenu::use_default,
        iAction::action, this, SLOT(CallFun()) );
  // add meth desc as a status item
  String statustip = meth->desc;
  if (statustip.nonempty())
    act->setStatusTip(statustip);
  if (meth->HasOption("MENU_SEP_AFTER"))
    mnu->AddSep();
}

bool taiWidgetMethod::CallFun_impl() {
  if ((meth->stubp == NULL) || (base == NULL))
    return true;
  QPointer<taiWidgetMethod> ths = this; // to detect us being deleted
  ApplyBefore();
  // note: this is not a great situation, whereby applying deletes us, but
  // we warn user (and should probably do something, ie a directive that you
  // have to save before)
  if (!ths) {
    taMisc::Error("This menu item or button action apparently cannot be invoked when you have not applied changes. Please try the operation again. (The developers would appreciate hearing about this situation.");
    return false;
  }
  // determine if needs rval now, before we may get deleted when callivoidng
  bool show_rval = (meth->HasOption("USE_RVAL")  ||
       (meth->HasOption("USE_RVAL_RMB") && (arg_dlg->mouse_button == Qt::RightButton)) );
  IWidgetHost* thost = host; // in case we delete
  String meth_name = meth->name; // in case we delete

  // save undo state!
  if (typ->IsActualTaBase()) {
    taBase* tab = (taBase*)base;
    taProject* proj = (taProject*)tab->GetOwner(&TA_taProject);
    if(proj) {
      proj->undo_mgr.SaveUndo(tab, "Call Method: " + meth_name, NULL, true);
      // true = force project-level save
    }
  }

  use_argc = meth->fun_argc;
  String argc_str = meth->OptionAfter("ARGC_");
  if (argc_str != "")
    use_argc = (int)argc_str;
  use_argc = MIN(use_argc, meth->arg_types.size);
  use_argc = MIN(use_argc, meth->arg_names.size);
  if ((use_argc == 0) && !meth->HasOption("CONFIRM")) {
    GenerateScript();
#ifdef DMEM_COMPILE
    // don't actually run the command when using gui in dmem mode: everything happens via the script!
    if (taMisc::dmem_nprocs == 1)
#endif
    {
      bool use_busy = !meth->HasOption("NO_BUSY");
      if (use_busy) taMisc::Busy(true);
      ++taMisc::in_gui_call;
      cssEl* rval = (*(meth->stubp))(base, 1, (cssEl**)NULL);
      --taMisc::in_gui_call;
      // make sure we still exist before doing UA!
      if (ths)void
        UpdateAfter();
      if (use_busy) taMisc::Busy(false);
      if ((show_rval && (rval != &cssMisc::Void)))
        ShowReturnVal(rval, thost, meth_name);
      if(rval) {
        cssEl::Ref(rval);
        cssEl::unRefDone(rval);
      }
    } // NOTE: end of DMEM_COMPILE
    return (bool)ths;
  }
  arg_dlg = new cssiArgDialog(meth, typ, base, use_argc, 0); //modal
  if (typ->IsActualTaBase()) {
    bool ok;
    iColor bgclr = ((taBase*)base)->GetEditColorInherit(ok);
    if (ok) arg_dlg->setBgColor(bgclr);
  }
  arg_dlg->Constr("", "");
  int ok_can = arg_dlg->Edit(true);     // true = wait for a response
  if (ok_can && !arg_dlg->err_flag) {
    GenerateScript();
#ifdef DMEM_COMPILE
    // don't actually run the command when using gui in dmem mode: everything happens via the script!
    if (taMisc::dmem_nprocs == 1) {
#endif
      taMisc::Busy(true);
      cssEl* rval = (*(meth->stubp))(base, arg_dlg->obj->members->size-1,
                                     arg_dlg->obj->members->els);
      // only UA if we still exist!
      if (ths)
        UpdateAfter();

      taMisc::Busy(false);
      if ((show_rval && (rval != &cssMisc::Void)))
        ShowReturnVal(rval, thost, meth_name);
      if(rval) {
        cssEl::Ref(rval);
        cssEl::unRefDone(rval);
      }
#ifdef DMEM_COMPILE
    }
#endif
  }
  delete arg_dlg;
  return (bool)ths;
}

QWidget* taiWidgetMethod::MakeButton() {
  if (buttonRep == NULL) {
    QToolButton* newbut = new QToolButton(gui_parent);
    newbut->setFont(taiM->menuFont(defSize()));
    newbut->setText(meth->GetLabel());
    String statustip = meth->desc;
    String shortcut = meth->OptionAfter("SHORTCUT_");
    if(shortcut.nonempty()) {
      newbut->setShortcut(QKeySequence((QString)shortcut.chars()));
      statustip = String("(") + shortcut + String(") ") + statustip;
    }
    // add meth desc as a status item
    if (statustip.nonempty())
      newbut->setStatusTip(statustip);
    connect(newbut, SIGNAL(clicked()),this, SLOT(CallFun()) );
    newbut->show();
    buttonRep = newbut;
  }
  return buttonRep;
}

bool taiWidgetMethod::UpdateButtonRep() {
  if(!base || !buttonRep) return false;
  if(meth->OptionAfter("GHOST_").nonempty()) {
    // the default "true" for non-GHOST cases doesn't work here!
    bool ghost = meth->GetCondOptTest("GHOST", typ, base);
    buttonRep->setEnabled(!ghost);
    if(!ghost) {
      buttonRep->show();        // make it real?
    }
    return true;
  }
  return false;
}

void taiWidgetMethod::ApplyBefore() {
  if (host == NULL) return;
  if (meth->HasOption("NO_APPLY_BEFORE") /*|| !host->HasChanged()*/)
    return;
  // we no longer check about applying, because auto apply is supposed to be automatic!
/*obs  if (taMisc::auto_revert == taMisc::CONFIRM_REVERT) {
    int chs = taMisc::Choice("Auto Apply/Revert: You have edited the dialog--apply or revert and lose changes?", "Apply", "Revert");
    if (chs != 0) return;
  }*/

/*was:  applyNow();
  // note: applyNow is async, so we have to do event loop
  taiMiscCore::ProcessEvents(); */
  // send the RESOLVE_NOW operator to all clients
  taBase* base = host->Base();
  if (base) {
    base->SigEmit(SLS_RESOLVE_NOW);
  }
  //NOTE: small chance we could be deleted here, so no code here!
}

void taiWidgetMethod::UpdateAfter() {
  if (meth->HasOption("NO_REVERT_AFTER"))
    return;
  // this is for stuff just called from menus, not host
  if ((host == NULL) ||
    (host->GetTypeDef()->InheritsFrom(&TA_taiEditorWidgetsMain) &&
    (((taiEditorWidgetsMain*)host->This())->state != taiEditorWidgetsMain::ACTIVE)) )
  {
    if(base == NULL) return;
    taBase* tap = (taBase*)base;
    if (meth->HasOption("UPDATE_MENUS"))
      taiMisc::Update(tap);
    return;
  }
  // this is inside the host itself
  if ((host->GetRootTypeDef() != NULL) && host->GetRootTypeDef()->IsActualTaBase()) {
    taBase* tap = host->Base();
    if (tap && meth->HasOption("UPDATE_MENUS")) {
      taiMisc::Update(tap);     // update menus and stuff
      tap->UpdateAllViews(); // tell others to update
    }
  }
}

void taiWidgetMethod::GenerateScript() {
  if(!taMisc::record_on || !typ->IsActualTaBase())
    return;

#ifndef DMEM_COMPILE
  // dmem requires everything to be scripted to share commands!
  if(meth->HasOption("NO_SCRIPT")) {
    return;
  }
#endif

  taBase* tab = (taBase*)base;

  if ((use_argc == 0) || (arg_dlg == NULL)) {
    String rscr = tab->GetPathNames() + "." + meth->name + "();";
#ifdef DMEM_COMPILE
    if(taMisc::dmem_debug)
      taMisc::Info("proc:", String(taMisc::dmem_proc), "recording fun call:", rscr);
#endif
    taMisc::record_script << rscr << "\n";
    return;
  }

  int_Array tmp_objs;           // indicies of the temp objects
  for (int i = 0; i < arg_dlg->type_el.size; ++i) {
    taiArgType* art = (taiArgType*)arg_dlg->type_el.FastEl(i);
    if(art->arg_typ->IsActualTaBase())
      tmp_objs.Add(i+1);
  }
  if (tmp_objs.size > 0) {
    taMisc::record_script << "{ "; // put in context
    for (int i = 0; i < tmp_objs.size; ++i) {
      taiArgType* art = (taiArgType*)arg_dlg->type_el.FastEl(tmp_objs.FastEl(i) - 1);
      cssEl* argv = arg_dlg->obj->members->FastEl(tmp_objs.FastEl(i));
      taMisc::record_script << art->arg_typ->name << " tmp_" << i
                            << " = new " << art->arg_typ->name << "; \t tmp_" << i
                            << " = \"" << argv->GetStr() << "\";\n  ";
    }
  }

  String arg_str;
  for (int i = 1; i < arg_dlg->obj->members->size; ++i) {
    if (i > 1) {
      arg_str += ", ";
    }
    int idx = tmp_objs.FindEl(i);
    if (idx >= 0) {
      arg_str += "tmp_" + String(idx);
    }
    else {
      cssEl* argv = arg_dlg->obj->members->FastEl(i);
      if(argv->GetType() == cssEl::T_String) {
        arg_str += "\"";
        arg_str += argv->PrintFStr();
        arg_str += "\"";
      }
      else if(argv->GetType() == cssEl::T_TA) {
        cssTA* cssta = (cssTA*)argv;
        TypeDef* csstd = cssta->type_def;
        if(csstd->InheritsFrom(&TA_istream) || csstd->InheritsFrom(&TA_fstream)
           || csstd->InheritsFrom(&TA_ostream))
          arg_str += "\"" + taFiler::last_fname + "\"";
        else {
          arg_str += argv->PrintFStr();
        }
      }
      else {
        arg_str += argv->PrintFStr();
      }
    }
  }

  String rscr = tab->GetPathNames() + "." + meth->name + "(" + arg_str + ");\n";
  taMisc::record_script << rscr;
#ifdef DMEM_COMPILE
    if(taMisc::dmem_debug)
      taMisc::Info("proc:", String(taMisc::dmem_proc), "recording fun call:", rscr);
#endif
  if (tmp_objs.size > 0) {
    for (int i = 0; i < tmp_objs.size; ++i) {
      taMisc::record_script << "  delete tmp_" << i << ";\n";
    }
    taMisc::record_script << "}\n"; // put in context
  }
}

