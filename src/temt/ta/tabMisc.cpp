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

#include "tabMisc.h"

#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <taiMisc>


taRootBase* tabMisc::root = NULL;
taBase*     tabMisc::cur_undo_save_top = NULL;
taBase*     tabMisc::cur_undo_mod_obj = NULL;
taBase*     tabMisc::cur_undo_save_owner = NULL;

taBase_RefList  tabMisc::delayed_close;
taBase_RefList  tabMisc::delayed_updateafteredit;
taBase_FunCallList  tabMisc::delayed_funcalls;
ContextFlag  tabMisc::in_wait_proc;

void tabMisc::DelayedClose(taBase* obj) {
  if(taMisc::quitting) return;
  taMisc::do_wait_proc = true;
  delayed_close.AddUnique(obj); // only add once!!!
}

void tabMisc::DelayedUpdateAfterEdit(taBase* obj) {
  if(taMisc::quitting) return;
  taMisc::do_wait_proc = true;
  delayed_updateafteredit.Add(obj);
}

void tabMisc::DelayedFunCall_gui(taBase* obj, const String& fun_name) {
  if(taMisc::quitting) return;
  if(!taMisc::gui_active) return;
  taMisc::do_wait_proc = true;
  delayed_funcalls.AddBaseFun(obj, fun_name);
}

void tabMisc::DelayedFunCall_nogui(taBase* obj, const String& fun_name) {
  if(taMisc::quitting) return;
  taMisc::do_wait_proc = true;
  delayed_funcalls.AddBaseFun(obj, fun_name);
}

void tabMisc::WaitProc() {
  if (in_wait_proc) return; // already doing it!
  ++in_wait_proc;
  // prevent reentrant waitprocs!
#ifdef TA_GUI
  taiEditor::AsyncWaitProc();
  taiMisc::PurgeDialogs();
#endif
  bool did = false;
  bool rval = DoDelayedCloses();
  did |= rval;
  rval = DoDelayedUpdateAfterEdits();
  did |= rval;
  rval = DoDelayedFunCalls();
  did |= rval;

  if(!did) {
    DoAutoSave();               // only once it is quiet
  }
  --in_wait_proc;
}


/*
  Object on this list will only have refs if they are owned, ex. in a list.
  If the owner or mgr nukes the obj before we get to it, it will have been
  automatically removed from the delayed_close list. Otherwise, we will
  "nuke with extreme prejudice".
  Note that it might be possible for objects to get added to the list
  while deleting something -- we won't do those this cycle.
*/
bool tabMisc::DoDelayedCloses() {
  if (delayed_close.size == 0) return false;
  // note: this is sleazy, but very efficient -- we just completely
  // hijack the memory of the current list in this working copy
  taBase_RefList items;
  items.Hijack(delayed_close);
  // we work fifo
  while (items.size > 0) {
    // note: active items can often have many refs, and CutLinks will typically
    // resolve those (ex. Projection, which can have many cons)
    // so if item is owned, we just defer to it, otherwise
    taBase* it = items.FastEl(0);
    items.RemoveIdx(0);
    int refn = taBase::GetRefn(it);
    if ((it->GetOwner() == NULL) && (refn != 1)) {
      taMisc::DebugInfo("tabMisc::delayed_close: item had refn != 1, was=",
                        String(refn), "type=", it->GetTypeDef()->name, "name=",
                        it->GetName());
    }
    it->Close();
  }
  return true;
}

bool tabMisc::DoDelayedUpdateAfterEdits() {
  bool did_some = false;
  if (delayed_updateafteredit.size > 0) {
    while(delayed_updateafteredit.size > 0) {
      taBase* it = delayed_updateafteredit.Pop();
      it->UpdateAfterEdit();
    }
    did_some = true;
  }
  return did_some;
}

bool tabMisc::DoDelayedFunCalls() {
  bool did_some = false;
  while (delayed_funcalls.size > 0) {// note this must be fifo!
    // note: get all details before call, then remove
    FunCallItem* fci = delayed_funcalls.FastEl(0);
    taBase* it = fci->it;
    String fun_name = fci->fun_name;
    delayed_funcalls.RemoveIdx(0); // deletes fci
    if (it) {
      it->CallFun(fun_name);
    }
    did_some = true;
  }
  return did_some;
}

bool tabMisc::DoAutoSave() {
  if (!tabMisc::root) return false;
  bool did = false;
  FOREACH_ELEM_IN_GROUP(taProject, proj, tabMisc::root->projects) {
    if (proj->AutoSave()) {
      did = true;
    }
  }
  return did;
}

void tabMisc::WaitProc_Cleanup() {
  taMisc::do_wait_proc = false; // just to be sure
  delayed_updateafteredit.Reset();
  delayed_funcalls.Reset();
  delayed_close.Reset();
}

void tabMisc::DeleteRoot() {
  tabMisc::WaitProc_Cleanup();
  if (tabMisc::root) {
    delete tabMisc::root;
    tabMisc::root = NULL;
  }
}

