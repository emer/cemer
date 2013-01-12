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

#include "taUndoMgr.h"
#include <taProject>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>

using namespace std;


void taUndoMgr::Initialize() {
  cur_undo_idx = 0;
  undo_depth = taMisc::undo_depth;
  new_src_thr = taMisc::undo_new_src_thr;
#ifdef DEBUG
  save_load_file = false;       // can set to true if need to do debugging on undo
#else
  save_load_file = false;
#endif
  rec_to_diff = NULL;
  nest_count = 0;
  loop_count = 0;
}

void taUndoMgr::Nest(bool nest) {
  if (nest) {
    if (nest_count++ == 0)
      loop_count = 0;
  } else --nest_count;
}

bool taUndoMgr::SaveUndo(taBase* mod_obj, const String& action, taBase* save_top,
                         bool force_proj_save, taBase* undo_save_owner) {
  // only do the undo guy for first call when nested
  if ((nest_count > 0) && (loop_count++ > 0)) return false;
  if(!owner || !mod_obj) return false;
  if(!save_top) {
    if(force_proj_save)
      save_top = owner;
    else {
      save_top = mod_obj->GetUndoBarrier();
      if(!save_top)
        save_top = owner;
    }
  }
  if(mod_obj == save_top && mod_obj->HasOption("UNDO_SAVE_ALL")) {
    save_top = owner;           // save all instead..
  }
  if(cur_undo_idx < undo_recs.length) {
    undo_recs.length = cur_undo_idx; // lop off all the changes that were previously undone
  }
  taUndoRec* urec = new taUndoRec;
  undo_recs.CircAddLimit(urec, undo_depth);
  cur_undo_idx = undo_recs.length;
  urec->mod_obj_path = mod_obj->GetPath(NULL, owner);
  urec->mod_obj_name = mod_obj->GetName();
  urec->action = action;
  urec->mod_time.currentDateTime();
  urec->save_top = save_top;
  urec->save_top_path = save_top->GetPath(NULL, owner);

  if(taMisc::undo_debug) {
    taMisc::Info("SaveUndo of action:",urec->action,"on:",urec->mod_obj_name,
                  "at path:", urec->mod_obj_path, "saving at:", urec->save_top_path);
  }
  else {
    taMisc::LogInfo("SaveUndo of action:",urec->action,"on:",urec->mod_obj_name,
                    "at path:", urec->mod_obj_path, "saving at:", urec->save_top_path);
  }

  tabMisc::cur_undo_save_top = save_top; // let others know who we're saving for..
  tabMisc::cur_undo_mod_obj = mod_obj; // let others know who we're saving for..
  tabMisc::cur_undo_save_owner = undo_save_owner;
  ++taMisc::is_undo_saving;
  save_top->Save_String(urec->save_data);
  --taMisc::is_undo_saving;
  tabMisc::cur_undo_save_top = NULL;
  tabMisc::cur_undo_mod_obj = NULL;
  tabMisc::cur_undo_save_owner = NULL;

  // now encode diff for big saves!
  if(save_top == owner) {
    taUndoDiffSrc* cur_src = NULL;
    if(undo_srcs.length > 0)
      cur_src = undo_srcs.CircPeek(); // always grab the last guy
    if(!cur_src || cur_src->last_diff_pct > new_src_thr) {
      if(cur_src && taMisc::undo_debug) {
        taMisc::Info("SaveUndo diff pct on last save:", String(cur_src->last_diff_pct),
                     "was greater than threshold:", String(new_src_thr),
                     "saving a new src instead of the diff from previous src");
      }
      cur_src = new taUndoDiffSrc;
      undo_srcs.CircAddLimit(cur_src, undo_depth); // large depth
      cur_src->InitFmRec(urec);                    // init
      taMisc::LogInfo("Undo: New source added!");
    }
    if(diff_threads.n_running > 0)
      diff_threads.SyncThreads();       // sync now before running again..
    urec->diff_src = cur_src;   // this smartref ptr needs to be set in main task
    rec_to_diff = urec;
    diff_threads.Run(); // run diff in separate thread
  }

  PurgeUnusedSrcs();            // get rid of unused source data

  // tell project to refresh ui, because otherwise the undo action does not get enabled
  // properly
  taProject* proj = GET_MY_OWNER(taProject);
  if(proj) {
    tabMisc::DelayedFunCall_gui(proj,"UpdateUi");
  }

  return true;                  // todo: need to check result of Save_String presumably
}

void taUndoMgr::PurgeUnusedSrcs() {
  if(undo_srcs.length <= 0) return; // nothing to do
  bool did_purge = false;
  int n_purges = 0;
  do {
    taUndoDiffSrc* urec = undo_srcs.CircSafeEl(0);
    if(!urec) continue;
    int cnt = urec->UseCount();
    if(cnt == 0) {
      if(taMisc::undo_debug) {
        taMisc::Info("Undo: purging unused save rec, size: ",
                     String(urec->save_data.length()));
      }
      else {
        taMisc::DebugInfo("Undo: purging unused save rec, size: ",
                          String(urec->save_data.length()));
      }
      undo_srcs.CircShiftLeft(1);
      did_purge = true;
      n_purges++;
      break;
      // we actually need to bail now because UseCount hangs -- only get to do 1 at a time
    }
  } while(did_purge);

  if(n_purges > 0) {
    if(taMisc::undo_debug) {
      taMisc::Info("Undo: Total Purges: ", String(n_purges), " remaining length: ",
                   String(undo_srcs.length));
    }
    else {
      taMisc::DebugInfo("Undo: Total Purges: ", String(n_purges), " remaining length: ",
                        String(undo_srcs.length));
    }
  }
}

bool taUndoMgr::Undo() {
  if(!owner) return false;
  if(cur_undo_idx <= 0) {
    taMisc::Error("No more steps available to undo -- increase undo_depth in Preferences if you need more in general -- requires reload of project to take effect");
    return false;
  }
  taUndoRec* urec = undo_recs.CircSafeEl(cur_undo_idx-1); // anticipate decrement
  if(!urec) return false;
  bool first_undo = false;
  if(cur_undo_idx == undo_recs.length) {
    if(urec->action == "Undo") { // already the final undo guy -- skip to earlier one
      --cur_undo_idx;
      urec = undo_recs.CircSafeEl(cur_undo_idx-1); // anticipate decrement
    }
    else {
      // this is the first undo -- we need to save this current state so we can then redo it!
      first_undo = true;
      if(urec->save_top == owner) {
        SaveUndo(owner, "Undo", owner);
      }
      else {
        MemberDef* md;
        taBase* modobj = owner->FindFromPath(urec->mod_obj_path, md);
        if(modobj)
          SaveUndo(modobj, "Undo", urec->save_top);
        else
          SaveUndo(owner, "Undo", owner); // bail to full save
      }
    }
  }
  if(!urec->save_top) {         // it was nuked -- try to reconstruct from path..
    MemberDef* md;
    taBase* st = owner->FindFromPath(urec->save_top_path, md);
    if(st) urec->save_top = st;
  }
  if(!urec->save_top) {
    taMisc::Warning("Undo action:", urec->action, "on object named:", urec->mod_obj_name,
                    "at path:", urec->mod_obj_path,
                    "cannot complete, because saved data is relative to an object that has dissappeared -- it used to live here:", urec->save_top_path);
    --cur_undo_idx;             // need to skip past to get to other levels that might work..
    if(first_undo) --cur_undo_idx;      // need an extra because of extra saveundo.
    return false;
  }
  String msg;
  msg << "Undoing action: " << urec->action << " on: " << urec->mod_obj_name
      << " at path: " << urec->mod_obj_path;
  taMisc::Info(msg);

  bool rval = LoadFromRec_impl(urec);
  if(rval) {
    --cur_undo_idx;             // only decrement on success
    if(first_undo) --cur_undo_idx;      // need an extra because of extra saveundo.
  }
  return rval;
}

bool taUndoMgr::LoadFromRec_impl(taUndoRec* urec) {
  String udata = urec->GetData();
  if(save_load_file) {
    fstream ostrm;
    ostrm.open("undo_load_file.txt", ios::out);
    udata.Save_str(ostrm);
    ostrm.close();
  }

  // actually do the load..
  ++taMisc::is_undo_loading;
  urec->save_top->Load_String(udata);
  taMisc::ProcessEvents();      // get any post-load things *before* turning off undo flag..
  --taMisc::is_undo_loading;

  // tell project to refresh
  taProject* proj = GET_MY_OWNER(taProject);
  if(proj) {
    tabMisc::DelayedFunCall_gui(proj,"RefreshAllViews");
  }

  // finally, try select the originally modified object so it is clear what is happening!
  MemberDef* md;
  taBase* modobj = owner->FindFromPath(urec->mod_obj_path, md);
  if(modobj && !modobj->HasOption("NO_UNDO_SELECT")) {
    tabMisc::DelayedFunCall_gui(modobj, "BrowserSelectMe");
  }

  return true;
}

bool taUndoMgr::Redo() {
  if(!owner) return false;
  if(cur_undo_idx == 0) cur_undo_idx = 1;               // 0 is just err state
  if(cur_undo_idx >= undo_recs.length) {
    taMisc::Error("No more steps available to redo -- at end of undo list");
    return false;
  }
  taUndoRec* urec = undo_recs.CircSafeEl(cur_undo_idx); // always at current val for redo..
  if(!urec) return false;
  if(!urec->save_top) {         // it was nuked -- try to reconstruct from path..
    MemberDef* md;
    taBase* st = owner->FindFromPath(urec->save_top_path, md);
    if(st) urec->save_top = st;
  }
  if(!urec->save_top) {
    taMisc::Warning("Redo action:", urec->action, "on object named: ", urec->mod_obj_name,
                    "at path:", urec->mod_obj_path,
                    "cannot complete, because saved data is relative to an object that has dissappeared -- it used to live here:", urec->save_top_path);
    ++cur_undo_idx;             // need to skip past to get to other levels that might work..
    return false;
  }
  String msg;
  msg << "Redoing action: " << urec->action << " on: " << urec->mod_obj_name
      << " at path: " << urec->mod_obj_path;
  taMisc::Info(msg);

  bool rval = LoadFromRec_impl(urec);
  if(rval) {
    ++cur_undo_idx;             // only increment on success
  }
  return rval;
}

int taUndoMgr::UndosAvail() {
  return cur_undo_idx;
}

int taUndoMgr::RedosAvail() {
  return undo_recs.length - cur_undo_idx;
}

void taUndoMgr::ReportStats(bool show_list, bool show_diffs) {
  {
    String msg;
    msg << "Total Undo records: " << undo_recs.length << " cur_undo_idx: " << cur_undo_idx;
    taMisc::Info(msg);
  }
  int tot_size = 0;
  int tot_diff_lines = 0;
  for(int i=undo_recs.length-1; i>=0; i--) {
    taUndoRec* urec = undo_recs.CircSafeEl(i);
    if(!urec) continue;
    tot_size += urec->save_data.length();
    int dif_lns = 0;
    if((bool)urec->diff_src && urec->save_data.empty()) { // empty is key flag for actually ready
      dif_lns = urec->diff_edits.GetLinesChanged();
      tot_diff_lines += dif_lns;
    }
    if(show_list) {
      String msg;
      msg << "  " << taMisc::LeadingZeros(i, 2) << " size: " << urec->save_data.length()
          << " diffs: " << dif_lns
          << " action: " << urec->action << " on: " << urec->mod_obj_name
          << " at path: " << urec->mod_obj_path;
      taMisc::Info(msg);
      if(show_diffs && (bool)urec->diff_src && urec->save_data.empty()) {
        String msg;
        String diffstr = urec->diff_edits.GetDiffStr(urec->diff_src->save_data);
        for(int j=0; j<diffstr.length(); j++) {
          msg << diffstr[j];
        }
        taMisc::ConsoleOutput(msg, false, false);
      }
    }
  }

  int tot_saved = 0;
  for(int i=undo_srcs.length-1; i>=0; i--) {
    taUndoDiffSrc* urec = undo_srcs.CircSafeEl(i);
    tot_saved += urec->save_data.length();
  }

  {
    String msg;
    msg << "Undo memory usage: small Edit saves: " << tot_size
        << " full proj saves: " << tot_saved
        << " in: " << undo_srcs.length << " recs, "
        << " diff lines: " << tot_diff_lines;
    taMisc::Info(msg);
  }
}
