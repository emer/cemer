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
#include <iDialogTextEdit>

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
  String title = meth_name + " Return Value: \n";
  iDialogTextEdit* td = new iDialogTextEdit(true); // read only
  td->setWindowTitle(title);
  td->setText(rval->PrintStr());
  td->exec();
  td->deleteLater();
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
  
  String label = meth->GetLabel();
  // note: see explanation in iMainWindowViewer::ConstrEditMenu for why the
  // label is modified rather than call SetShortcut on the Action
  QKeySequence ks = taiMisc::GetMethodKeySequence(meth->name);
  if (!ks.isEmpty()) {
    label += "\t" + ks.toString();
  }
  iAction* act = mnu->AddItem(label, taiWidgetMenu::use_default,
                              iAction::action, this, SLOT(CallFun()) );
  menuRep = act;                // save!
  
  // add meth desc as a status item
  String statustip = meth->desc;
  if (statustip.nonempty()) {
    act->setStatusTip(statustip);
  }
  if (meth->HasOption("MENU_SEP_AFTER"))
    mnu->AddSep();
  
  // works for context menus! Yes!
  if(meth->OptionAfter("ENABLE_").nonempty()) {
    bool enab = meth->GetCondOptTest("ENABLE", typ, base);
    act->setEnabled(enab);
  }
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
    if(proj && !meth->HasOption("NO_SAVE_UNDO")) {
      proj->undo_mgr.SaveUndo(tab, "Call Method: " + meth_name, NULL, true);
      // true = force project-level save
    }
  }

  String pre_call_str = meth->OptionAfter("PRE_CALL_");
  if(pre_call_str.nonempty()) {
    MethodDef* pcmeth = typ->methods.FindName(pre_call_str);
    if(!pcmeth) {
      taMisc::Error("programmer error: PRE_CALL method not found:", pre_call_str, "in type:", typ->name);
    }
    else {
      pcmeth->CallFun(base); // call it!
    }
  }
  
  use_argc = meth->fun_argc;
  String argc_str = meth->OptionAfter("ARGC_");
  if (argc_str.nonempty())
    use_argc = (int)argc_str;
  use_argc = MIN(use_argc, meth->arg_types.size);
  use_argc = MIN(use_argc, meth->arg_names.size);
  if (use_argc > 0 && typ->IsActualTaBase()) {
    taBase* tab = (taBase*)base;
    if (!tab->ShowCallFunDialog(meth_name)) {
      use_argc = 0; // don't show dialog
    }
  }
  bool use_busy = !meth->HasOption("NO_BUSY");
  if ((use_argc == 0) && !meth->HasOption("CONFIRM")) {
    GenerateScript();
#ifdef DMEM_COMPILE
    // don't actually run the command when using gui in dmem mode: everything happens via the script!
    if (taMisc::dmem_nprocs == 1)
#endif
    {
      if (use_busy) taMisc::Busy(true);
      ++taMisc::in_gui_call;
      cssEl* rval = (*(meth->stubp))(base, 1, (cssEl**)NULL);
      --taMisc::in_gui_call;
      // make sure we still exist before doing UA!
      if (ths) {
        UpdateAfter();
      }
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
  String width = meth->OptionAfter("MIN_DLG_WIDTH_");  // remove if we can get dialogs to resize when conditional widgets get added
  int ok_can = arg_dlg->Edit(true, width.toInt());     // true = wait for a response - see min width
  if (ok_can && !arg_dlg->err_flag) {
    GenerateScript();
#ifdef DMEM_COMPILE
    // don't actually run the command when using gui in dmem mode: everything happens via the script!
    if (taMisc::dmem_nprocs == 1) {
#endif
      if(use_busy) taMisc::Busy(true);
      cssEl* rval = (*(meth->stubp))(base, arg_dlg->obj->members->size-1,
                                     arg_dlg->obj->members->els);
      // only UA if we still exist!
      if (ths)
        UpdateAfter();

      if(use_busy) taMisc::Busy(false);
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

bool taiWidgetMethod::UpdateEnabled() {
  if(!base) return false;
  if(meth->OptionAfter("ENABLE_").empty())
    return false;
  bool enab = meth->GetCondOptTest("ENABLE", typ, base);
  // taMisc::DebugInfo("meth rep enable update:", meth->name, "on:",
  //                   ((taBase*)base)->GetName(), "now:", String(enab));
  if(menuRep) {
    menuRep->setEnabled(enab);
  }
  UpdateButtonRep(enab);
  return true;
}

bool taiWidgetMethod::UpdateButtonRep(bool enab) {
  if(!buttonRep) return false;
  buttonRep->setEnabled(enab);
  if(enab) {
    buttonRep->show();        // make it real?
  }
  return true;
}

void taiWidgetMethod::ApplyBefore() {
  if (host == NULL) return;
  if (meth->HasOption("NO_APPLY_BEFORE") /*|| !host->HasChanged()*/)
    return;

/*was:  applyNow();
  // note: applyNow is async, so we have to do event loop
  taMisc::ProcessEvents(); */
  // send the RESOLVE_NOW operator to all clients
  taBase* hbase = host->Base();
  if (hbase) {
    hbase->SigEmit(SLS_RESOLVE_NOW);
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
    // note: we do NOT automatically do an UAE for each method -- methods that
    // require updating must specifically request it!
    // tabMisc::DelayedUpdateAfterEdit(tap);
    // the following appears to be a complete no-op: no method defines UPDATE_MENUS
    // and this update method calls an Update_Hook which never appears to be set!
    if (meth->HasOption("UPDATE_MENUS")) {
      taiMisc::Update(tap);
    }
    return;
  }
  // this is inside the host itself
  if ((host->GetRootTypeDef() != NULL) && host->GetRootTypeDef()->IsActualTaBase()) {
    taBase* tap = host->Base();
    if (tap) {
      // note: we do NOT automatically do an UAE for each method -- methods that
      // require updating must specifically request it!
      // tabMisc::DelayedUpdateAfterEdit(tap);
      // see above..
      if(meth->HasOption("UPDATE_MENUS")) {
        taiMisc::Update(tap);     // update menus and stuff
      }
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

