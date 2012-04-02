// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// for debugging the qconsole, uncomment this define and it will revert to qandd console
// #define QANDD_CONSOLE 1

#include "ta_project.h"
#include "ta_platform.h"
#include "ta_dump.h"
#include "ta_plugin.h"
#include "ta_gendoc.h"
#include "ta_server.h"
#include "ta_thread.h"

#include "css_ta.h"
#include "css_console.h"

#ifdef TA_GUI
# include "ta_qt.h"
# include "ta_qtdata.h"
# include "ta_qtdialog.h"
# include "ta_qttype_def.h"
# include "colorscale.h"
# include "css_qt.h"
# include "t3node_so.h"
# ifdef HAVE_QT_CONSOLE
#   include "css_qtconsole.h"
# endif
# include <QApplication>
# include <QDesktopServices>
# include <QFileDialog>
# include <QMessageBox>
# include <QWidgetList>
# include <QGLFormat>
# include <QFileOpenEvent>
#endif

#include "inetworkaccessmanager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QPointer>
#include <QTimer>

#include <time.h>
#include <locale.h>
#include <errno.h>

#ifdef DMEM_COMPILE
#include "ta_dmem.h"
#endif

#ifdef TA_USE_INVENTOR
  #include <Quarter/Quarter.h>
#endif
#include <ode/ode.h>

#ifdef GPROF                    // turn on for profiling
extern "C" void moncontrol(int mode);
#endif

#ifdef SATURN_PROF
#include <Saturn.h>
#endif

#ifdef TA_OS_LINUX
# include <fenv.h>
#endif

//////////////////////////
//   taDoc              //
//////////////////////////

const String taDoc::init_text(
"<html>\n"
"<head></head>\n"
"<body>\n"
"== Enter Title Here ==\n"
"</body>\n"
"</html>\n");

void taDoc::Initialize() {
  auto_open = false;
  web_doc = false;
  url = "local";
  full_url = "local";
  text_size = 1.0f;
  if (!taMisc::is_loading && !taMisc::is_duplicating)
    text = init_text;
}

void taDoc::UpdateText() {
  html_text = WikiParse(text);
}

void taDoc::SetURL(const String& new_url) {
  full_url = new_url;
  if(wiki.nonempty()) {
    String base_url = taMisc::GetWikiURL(wiki, true); // index.php
    if(new_url.startsWith(base_url))
      url = new_url.after(base_url);
    else {
      wiki = _nilString;
      url = url;
    }
  }
  else {
    bool got_one = false;
    for(int i=0;i<taMisc::wikis.size; i++) {
      String wiknm = taMisc::wikis[i].name;
      String base_url = taMisc::GetWikiURL(wiknm, true); // index.php
      if(new_url.startsWith(base_url)) {
        wiki = wiknm;
        url = new_url.after(base_url);
        got_one = true;
        break;
      }
    }
    if(!got_one) {
      wiki = _nilString;
      url = new_url;
    }
  }
  UpdateAfterEdit();
}

String taDoc::GetURL() {
  if(wiki.nonempty()) {
    String wiki_url = taMisc::GetWikiURL(wiki, true); // true = add index.php
    if(TestWarning(wiki_url.empty(), "GetURL", "wiki named:", wiki,
                   "not found in global preferences/options under wiki_url settings -- using full url backup instead.")) {
      wiki = _nilString;
      url = full_url;
      return url;
    }
    return wiki_url + url;
  }
  url = taMisc::FixURL(url);
  return url;
}

void taDoc::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(url.empty() || url == "local")
    web_doc = false;
  else
    web_doc = true;
  if(web_doc) {
    full_url = GetURL();
  }
  else {
    // only do this if not a web doc -- otherwise it saves web page directly to html_text and can display that when offline..
    UpdateText();
  }
}

static String wiki_parse_str_between(const String& cl, const String& sts, const String& eds) {
  if(cl.freq(sts) == 1 && cl.freq(eds) == 1) {
    String btwn = cl.after(sts);
    btwn = btwn.before(eds);
    return btwn;
  }
  return _nilString;
}

static bool wiki_parse_check_seq(const String& cl, int cur_pos, char trg, char trg1 = '\0',
                                 char trg2 = '\0', char trg3 = '\0') {
  int ln = cl.length();
  if(cl[cur_pos] != trg) return false;
  if(trg1 == '\0') return true;
  if(cur_pos+1 >= ln || cl[cur_pos+1] != trg1) return false;
  if(trg2 == '\0') return true;
  if(cur_pos+2 >= ln || cl[cur_pos+2] != trg2) return false;
  if(trg3 == '\0') return true;
  if(cur_pos+3 >= ln || cl[cur_pos+3] != trg3) return false;
  return true;
}

static int wiki_parse_find_term(const String& cl, int cur_pos, char trg, char trg1 = '\0',
                                 char trg2 = '\0', char trg3 = '\0') {
  int ln = cl.length();
  for(int i=cur_pos; i<ln; i++) {
    if(cl[i] != trg) continue;
    if(trg1 == '\0') return i;
    if(i+1 >= ln || cl[i+1] != trg1) continue;
    if(trg2 == '\0') return i+1;
    if(i+2 >= ln || cl[i+2] != trg2) continue;
    if(trg3 == '\0') return i+2;
    if(i+3 >= ln || cl[i+3] != trg3) continue;
    return i+3;
  }
  return -1;
}

String taDoc::WikiParse(const String& in_str) {
  String rval;
  String rest = in_str;
  bool bullet1 = false;
  bool bullet2 = false;
  bool num1 = false;
  bool num2 = false;
  while(rest.contains("\n")) {
    String cl = rest.before("\n");
    rest = rest.after("\n");

    // need bullet first because it sets context with <ul> </ul>
    if(cl.startsWith("* ")) {
      if(bullet2) { cl = "</ul> <li> " + cl.after("* "); bullet2 = false; }
      else if(!bullet1) cl = "<ul><li> " + cl.after("* ");
      else cl = "<li> " + cl.after("* ");
      bullet1 = true;
    }
    else if(cl.startsWith("** ")) {
      if(!bullet2) cl = "<ul><li> " + cl.after("** ");
      else cl = "<li> " + cl.after("** ");
      bullet2 = true;
    }
    else if(cl.startsWith(":* ")) {
      if(!bullet2) cl = "<ul><li> " + cl.after(":* ");
      else cl = "<li> " + cl.after(":* ");
      bullet2 = true;
    }
    else if(cl.startsWith("# ")) {
      if(num2) { cl = "</ol> <li> " + cl.after("# "); num2 = false; }
      else if(!num1) cl = "<ol><li> " + cl.after("# ");
      else cl = "<li> " + cl.after("# ");
      num1 = true;
    }
    else if(cl.startsWith("## ")) {
      if(!num2) cl = "<ol><li> " + cl.after("## ");
      else cl = "<li> " + cl.after("## ");
      num2 = true;
    }
    else if(cl.startsWith(":# ")) {
      if(!num2) cl = "<ol><li> " + cl.after(":# ");
      else cl = "<li> " + cl.after(":# ");
      num2 = true;
    }
    else {
      if(bullet2) { cl += "</ul>"; bullet2 = false; }
      if(bullet1) { cl += "</ul>"; bullet1 = false; }

      if(num2) { cl += "</ol>"; num2 = false; }
      if(num1) { cl += "</ol>"; num1 = false; }
    }

    if(cl.empty()) {            // make a <P> for blank lines..
      rval += "<P>\n";
      continue;
    }

    // headers
    if(wiki_parse_str_between(cl, "==== ", " ====").nonempty()) {
      cl.gsub("==== ", "<h4> ");
      cl.gsub(" ====", " </h4>");
    }
    else if(wiki_parse_str_between(cl, "=== ", " ===").nonempty()) {
      cl.gsub("=== ", "<h3> ");
      cl.gsub(" ===", " </h3>");
    }
    else if(wiki_parse_str_between(cl, "== ", " ==").nonempty()) {
      cl.gsub("== ", "<h2> ");
      cl.gsub(" ==", " </h2>");
    }
    else if(wiki_parse_str_between(cl, "= ", " =").nonempty()) {
      cl.gsub("= ", "<h1> ");
      cl.gsub(" =", " </h1>");
    }

    // now process remainder of string looking for various formatting things
    int cur_pos = 0;

    while(cur_pos < cl.length()) {
      if(wiki_parse_check_seq(cl, cur_pos, '[', '[')) {
        int epos = wiki_parse_find_term(cl, cur_pos+2, ']', ']');
        if(epos > cur_pos+2) {
          String href = cl.at(cur_pos+2, epos-cur_pos-3); // 2 * delim -1
          bool ta_tag = false;
          if(href.startsWith('.')) {
            ta_tag = true;
            href = "ta:" + href;
          }
          String tag = href;
          if(tag.contains('|')) {
            href = href.before('|');
            tag = tag.after('|');
          }
          else if(ta_tag) {
            if(tag.contains("()")) {
              String fnm = tag.after('.',-1);
              tag = tag.before('.',-1);
              if(tag.contains('.')) { // should!
                tag = tag.after('.',-1);
              }
              tag += "." + fnm;
            }
            else {
              tag = tag.after('.',-1);
            }
          }
          cl = cl.before(cur_pos) + "<a href=\"" + href + "\">" + tag + "</a>" + cl.after(epos);
          cur_pos = epos+1;
          continue;
        }
      }
      if(wiki_parse_check_seq(cl, cur_pos, '\'', '\'', '\'')) { // bold
        int epos = wiki_parse_find_term(cl, cur_pos+3, '\'', '\'', '\'');
        if(epos > cur_pos+3) {
          String bld = cl.at(cur_pos+3, epos-cur_pos-5); // 2 * delim -1
          cl = cl.before(cur_pos) + " <b>" + bld + "</b> " + cl.after(epos);
          cur_pos = epos+1;
          continue;
        }
      }
      if(wiki_parse_check_seq(cl, cur_pos, '\'', '\'')) { // emph
        int epos = wiki_parse_find_term(cl, cur_pos+2, '\'', '\'');
        if(epos > cur_pos+2) {
          String bld = cl.at(cur_pos+2, epos-cur_pos-3); // 2 * delim -1
          cl = cl.before(cur_pos) + " <i>" + bld + "</i> " + cl.after(epos);
          cur_pos = epos+1;
          continue;
        }
      }
      cur_pos++;
    }

    rval += cl + "\n";
  }
  return rval;
}


//////////////////////////////////
//  UserData_DocLink            //
//////////////////////////////////

void UserData_DocLink::Initialize() {
  Own(doc, this);
}

void UserData_DocLink::SmartRef_DataDestroying(taSmartRef* ref, taBase* obj)
{
  // destroy ourself, so we don't have this stale ref left over
  if(isDestroying()) return;    // already going..
  if (ref == &doc)
    this->CloseLater();
}

//////////////////////////////////
//  Doc_Group           //
//////////////////////////////////

void Doc_Group::AutoEdit() {
  FOREACH_ELEM_IN_GROUP(taDoc, doc, *this) {
    if (doc->auto_open)
      doc->EditPanel(true, true); // true,true = new tab, pinned in place
  }
}


//////////////////////////
//   taWizard           //
//////////////////////////

void taWizard::Initialize() {
  auto_open = true;
  SetUserData("NO_CLIP", true);
  SetBaseFlag(NAME_READONLY);
}

void taWizard::InitLinks() {
  inherited::InitLinks();
  wiz_doc.SetName(name);        // same name as us..
  RenderWizDoc();
}

void taWizard::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  wiz_doc.SetName(name);        // same name as us..
}

void taWizard::RenderWizDoc() {
  RenderWizDoc_header();
  RenderWizDoc_impl();
  RenderWizDoc_footer();
  wiz_doc.UpdateText();
}

void taWizard::RenderWizDoc_header() {
  wiz_doc.text = "<html>\n<head></head>\n<body>\n";
}

void taWizard::RenderWizDoc_footer() {
  wiz_doc.text += "</body>\n</html>\n";
}

void taWizard::RenderWizDoc_impl() {
  wiz_doc.text +=
"= taWizard =\n\
this is a virtual base wizard -- not the real thing -- shouldn't see this!\n";
}

//////////////////////////////////
//      Wizard_Group            //
//////////////////////////////////

void Wizard_Group::AutoEdit() {
  FOREACH_ELEM_IN_GROUP(taWizard, wz, *this) {
    if (wz->auto_open)
      wz->EditPanel(true, true);// true,true = new tab, pinned in place
  }
}

//////////////////////////////////
//      SelectEdit_Group        //
//////////////////////////////////

void SelectEdit_Group::AutoEdit() {
  FOREACH_ELEM_IN_GROUP(SelectEdit, se, *this) {
    if(se->autoEdit())
      se->EditPanel(true, true);        // true,true = new tab, pinned in place
  }
}

//////////////////////////////////
//      Undo Stuff              //
//////////////////////////////////

void taUndoDiffSrc::Initialize() {
  last_diff_n = 0;
  last_diff_pct = 0.0f;
}

void taUndoDiffSrc::InitFmRec(taUndoRec* urec) {
  mod_time = urec->mod_time;
  save_top = urec->save_top;
  save_top_path = urec->save_top_path;
  save_data = urec->save_data;
  diff.Reset();                 // just in case..
}

void taUndoDiffSrc::EncodeDiff(taUndoRec* rec) {
  if(diff.data_a.line_st.size > 0) { // already done
    diff.ReDiffB(save_data, rec->save_data, false, false, false); // trim, no ignore case
  }
  else {                            // first time
    diff.DiffStrings(save_data, rec->save_data, false, false, false); // trim, no ignore case
  }
  diff.GetEdits(rec->diff_edits);       // save to guy
  last_diff_n = diff.GetLinesChanged(); // counts up total lines changed in diffs
  last_diff_pct = (.5f * (float)last_diff_n) / (float)diff.data_a.lines;
  // lines changed tends to double-count..
  taMisc::LogInfo("last_diff_n: ", String(last_diff_n), " pct: ", String(last_diff_pct));
  // now nuke rec's saved data!!
  rec->save_data = _nilString;
  // if need to debug, turn this off and turn on comparison below..
}

int taUndoDiffSrc::UseCount() {
  taDataLink* dl = data_link();
  if(!dl) return 0;
  int cnt = 0;
  taDataLinkItr itr;
  taSmartRef* el;
  FOR_DLC_EL_OF_TYPE(taSmartRef, el, dl, itr) {
    taBase* spo = el->GetOwner();
    if(!spo) continue;
    cnt++;
  }
  return cnt;
}


void taUndoDiffSrc_List::Initialize() {
  SetBaseType(&TA_taUndoDiffSrc);
  st_idx = 0;
  length = 0;
}

void taUndoDiffSrc_List::Copy_(const taUndoDiffSrc_List& cp) {
  st_idx = cp.st_idx;
  length = cp.length;
}

void taUndoDiffSrc_List::Reset() {
  inherited::Reset();
  st_idx = 0;
  length = 0;
}

////////////////////////////
//      taUndoRec

void taUndoRec::Initialize() {
}

String taUndoRec::GetData() {
  if(save_data.nonempty()) {
    return save_data;           // easy
  }
  String rval = diff_edits.GenerateB(diff_src->save_data); // generate against A
#if 0
  // this is no longer enabled because we are nuking the save_data for all cases..
  int oops = compare(rval, save_data); // double check!
  if(oops > 0) {
    fstream ostrm;
    ostrm.open("rec_regen.txt", ios::out);
    rval.Save_str(ostrm);
    ostrm.close();

    ostrm.open("rec_save_data.txt", ios::out);
    save_data.Save_str(ostrm);
    ostrm.close();

    taMisc::Error("taUndoRec GetData() did not recover original data -- n diffs:",
                  String(oops), "see rec_save_data.txt and rec_regen.txt for orig texts");
  }
#endif
  return rval;
}

void taUndoRec::EncodeMyDiff() {
  diff_src->EncodeDiff(this);
}

void taUndoRec_List::Initialize() {
  SetBaseType(&TA_taUndoRec);
  st_idx = 0;
  length = 0;
}

void taUndoRec_List::Copy_(const taUndoRec_List& cp) {
  st_idx = cp.st_idx;
  length = cp.length;
}

void taUndoRec_List::Reset() {
  inherited::Reset();
  st_idx = 0;
  length = 0;
}

////////////////////////////
//      undo threading


void UndoDiffTask::Initialize() {
}

void UndoDiffTask::Destroy() {
}

void UndoDiffTask::run() {
  UndoDiffThreadMgr* udtmg = mgr();
  if(!udtmg) return;
  taUndoMgr* um = udtmg->undo_mgr();
  if(!um) return;
  if(!um->rec_to_diff) return;

  if(um->isDestroying()) return; // checks owner..

  um->rec_to_diff->EncodeMyDiff();
  um->rec_to_diff = NULL;       // done, reset!
}

void UndoDiffThreadMgr::Initialize() {
  n_threads = 2;                // don't use 0, just 1..
  task_type = &TA_UndoDiffTask;
}

void UndoDiffThreadMgr::Destroy() {
}

void UndoDiffThreadMgr::Run() {
  n_threads = 2;                // don't use 0, just 1..
  InitAll();
  RunThreads();                 // just run the thread, not main guy
}

////////////////////////////
//      taUndoMgr

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

  taMisc::LogInfo("SaveUndo of action:",urec->action,"on:",urec->mod_obj_name,
                  "at path:", urec->mod_obj_path, "saving at:", urec->save_top_path);

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

  // tell project to refresh ui
  // why?????
//   taProject* proj = GET_MY_OWNER(taProject);
//   if(proj) {
//     tabMisc::DelayedFunCall_gui(proj,"UpdateUi");
//   }

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
      taMisc::DebugInfo("Undo: purging unused save rec, size: ",
                        String(urec->save_data.length()));
      undo_srcs.CircShiftLeft(1);
      did_purge = true;
      n_purges++;
      break;
      // we actually need to bail now because UseCount hangs -- only get to do 1 at a time
    }
  } while(did_purge);

  if(n_purges > 0) {
    taMisc::DebugInfo("Undo: Total Purges: ", String(n_purges), " remaining length: ",
                      String(undo_srcs.length));
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
  cout << "Undoing action: " << urec->action << " on: " << urec->mod_obj_name
       << " at path: " << urec->mod_obj_path << endl;
  taMisc::FlushConsole();

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
  if(modobj) {
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
  cout << "Redoing action: " << urec->action << " on: " << urec->mod_obj_name
       << " at path: " << urec->mod_obj_path << endl;
  taMisc::FlushConsole();

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
  cout << "Total Undo records: " << undo_recs.length << " cur_undo_idx: " << cur_undo_idx << endl;
  taMisc::FlushConsole();
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
      cout << "  " << taMisc::LeadingZeros(i, 2) << " size: " << urec->save_data.length()
           << " diffs: " << dif_lns
           << " action: " << urec->action << " on: " << urec->mod_obj_name
           << " at path: " << urec->mod_obj_path << endl;
      taMisc::FlushConsole();
      if(show_diffs && (bool)urec->diff_src && urec->save_data.empty()) {
        String diffstr = urec->diff_edits.GetDiffStr(urec->diff_src->save_data);
        for(int j=0; j<diffstr.length(); j++) {
          cout << diffstr[j];
          if(diffstr[j] == '\n')
            taMisc::FlushConsole();
        }
      }
    }
  }

  int tot_saved = 0;
  for(int i=undo_srcs.length-1; i>=0; i--) {
    taUndoDiffSrc* urec = undo_srcs.CircSafeEl(i);
    tot_saved += urec->save_data.length();
  }

  cout << "Undo memory usage: small Edit saves: " << tot_size
       << " full proj saves: " << tot_saved
       << " in: " << undo_srcs.length << " recs, "
       << " diff lines: " << tot_diff_lines << endl;
  taMisc::FlushConsole();
}

//////////////////////////
//  taProject    Helpers
//////////////////////////

#ifdef TA_GUI
class SimLogEditDialog: public taiEditDataHost {
public:
  override bool ShowMember(MemberDef* md) const {
    // just show a small subset of the members
    bool rval = (md->ShowMember(show()) && (md->im != NULL));
    if (!rval) return rval;
// note: we also include a couple of members we know are in taProject
    if (!(md->name.contains("desc") || (md->name == "version") || (md->name == "save_rmv_units")
         || (md->name == "file_name"))) return false;
    return true;
  }
  override void Constr_Methods_impl() { }       // suppress methods

  SimLogEditDialog(void* base, TypeDef* tp, bool read_only_,
        bool modal_) : taiEditDataHost(base, tp, read_only_, modal_) { };
};
#endif


void taProjVersion::SetFromString(String ver) {
  Clear();
  // parse, mj.mn.st -- just blindly go through, harmless if missings
  major = taVersion::BeforeOrOf('.', ver);
  minor = taVersion::BeforeOrOf('.', ver);
  step = taVersion::BeforeOrOf(' ', ver); // dummy
}

bool taProjVersion::GtEq(int mj, int mn, int st) {
  return (major > mj) ||
    ((major == mj) && (minor > mn)) ||
     ((major == mj) && (minor == mn) && (step >= st));
}

void taLicense::Initialize() {
  license = taMisc::license_def;
  owner = taMisc::license_owner;
  org = taMisc::license_org;
  year = String(QDate::currentDate().year());
}

void taLicense::ViewLicense() {
  lic_text = GetLicenseText(license, owner, year, org);
  if(license == taMisc::CUSTOM_LIC) lic_text += custom;
  TypeDef* td = GetTypeDef();
  MemberDef* md = td->members.FindName("lic_text");
  taiStringDataHost* host_ = new taiStringDataHost(md, this, td, true, false, NULL, false);
  // args are: read_only, modal, parent, line_nos
  host_->Constr("License Text for the Project");
  host_->Edit(false);
}

String taLicense::GetLicenseText(taMisc::StdLicense lic, const String& ownr, const String& yr, const String& og) {
  String txt;
  String cpyright = "Copyright (c) " + yr + ", " + ownr + "\n\n";
  String eff_org = og;
  if(eff_org.empty()) eff_org = ownr;
  switch(lic) {
  case taMisc::NO_LIC:
    txt = "All Rights Reserved\n";
    break;
  case taMisc::GPLv2:
    txt = "The GNU General Public License (GPL)\n\
Version 2, June 1991\n\
Copyright (C) 1989, 1991 Free Software Foundation, Inc.\n\
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA\n\
\n\
Everyone is permitted to copy and distribute verbatim copies\n\
of this license document, but changing it is not allowed.\n\
\n\
Preamble\n\
\n\
The licenses for most software are designed to take away your freedom to share and change it. By contrast, the GNU General Public License is intended to guarantee your freedom to share and change free software--to make sure the software is free for all its users. This General Public License applies to most of the Free Software Foundation's software and to any other program whose authors commit to using it. (Some other Free Software Foundation software is covered by the GNU Library General Public License instead.) You can apply it to your programs, too.\n\
\n\
When we speak of free software, we are referring to freedom, not price. Our General Public Licenses are designed to make sure that you have the freedom to distribute copies of free software (and charge for this service if you wish), that you receive source code or can get it if you want it, that you can change the software or use pieces of it in new free programs; and that you know you can do these things.\n\
\n\
To protect your rights, we need to make restrictions that forbid anyone to deny you these rights or to ask you to surrender the rights. These restrictions translate to certain responsibilities for you if you distribute copies of the software, or if you modify it.\n\
\n\
For example, if you distribute copies of such a program, whether gratis or for a fee, you must give the recipients all the rights that you have. You must make sure that they, too, receive or can get the source code. And you must show them these terms so they know their rights.\n\
\n\
We protect your rights with two steps: (1) copyright the software, and (2) offer you this license which gives you legal permission to copy, distribute and/or modify the software.\n\
\n\
Also, for each author's protection and ours, we want to make certain that everyone understands that there is no warranty for this free software. If the software is modified by someone else and passed on, we want its recipients to know that what they have is not the original, so that any problems introduced by others will not reflect on the original authors' reputations.\n\
\n\
Finally, any free program is threatened constantly by software patents. We wish to avoid the danger that redistributors of a free program will individually obtain patent licenses, in effect making the program proprietary. To prevent this, we have made it clear that any patent must be licensed for everyone's free use or not licensed at all.\n\
\n\
The precise terms and conditions for copying, distribution and modification follow.\n\
\n\
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION\n\
\n\
0. This License applies to any program or other work which contains a notice placed by the copyright holder saying it may be distributed under the terms of this General Public License. The 'Program', below, refers to any such program or work, and a 'work based on the Program' means either the Program or any derivative work under copyright law: that is to say, a work containing the Program or a portion of it, either verbatim or with modifications and/or translated into another language. (Hereinafter, translation is included without limitation in the term 'modification'.) Each licensee is addressed as 'you'.\n\
\n\
Activities other than copying, distribution and modification are not covered by this License; they are outside its scope. The act of running the Program is not restricted, and the output from the Program is covered only if its contents constitute a work based on the Program (independent of having been made by running the Program). Whether that is true depends on what the Program does.\n\
\n\
1. You may copy and distribute verbatim copies of the Program's source code as you receive it, in any medium, provided that you conspicuously and appropriately publish on each copy an appropriate copyright notice and disclaimer of warranty; keep intact all the notices that refer to this License and to the absence of any warranty; and give any other recipients of the Program a copy of this License along with the Program.\n\
\n\
You may charge a fee for the physical act of transferring a copy, and you may at your option offer warranty protection in exchange for a fee.\n\
\n\
2. You may modify your copy or copies of the Program or any portion of it, thus forming a work based on the Program, and copy and distribute such modifications or work under the terms of Section 1 above, provided that you also meet all of these conditions:\n\
\n\
a) You must cause the modified files to carry prominent notices stating that you changed the files and the date of any change.\n\
\n\
b) You must cause any work that you distribute or publish, that in whole or in part contains or is derived from the Program or any part thereof, to be licensed as a whole at no charge to all third parties under the terms of this License.\n\
\n\
c) If the modified program normally reads commands interactively when run, you must cause it, when started running for such interactive use in the most ordinary way, to print or display an announcement including an appropriate copyright notice and a notice that there is no warranty (or else, saying that you provide a warranty) and that users may redistribute the program under these conditions, and telling the user how to view a copy of this License. (Exception: if the Program itself is interactive but does not normally print such an announcement, your work based on the Program is not required to print an announcement.)\n\
\n\
These requirements apply to the modified work as a whole. If identifiable sections of that work are not derived from the Program, and can be reasonably considered independent and separate works in themselves, then this License, and its terms, do not apply to those sections when you distribute them as separate works. But when you distribute the same sections as part of a whole which is a work based on the Program, the distribution of the whole must be on the terms of this License, whose permissions for other licensees extend to the entire whole, and thus to each and every part regardless of who wrote it.\n\
\n\
Thus, it is not the intent of this section to claim rights or contest your rights to work written entirely by you; rather, the intent is to exercise the right to control the distribution of derivative or collective works based on the Program.\n\
\n\
In addition, mere aggregation of another work not based on the Program with the Program (or with a work based on the Program) on a volume of a storage or distribution medium does not bring the other work under the scope of this License.\n\
\n\
3. You may copy and distribute the Program (or a work based on it, under Section 2) in object code or executable form under the terms of Sections 1 and 2 above provided that you also do one of the following:\n\
\n\
a) Accompany it with the complete corresponding machine-readable source code, which must be distributed under the terms of Sections 1 and 2 above on a medium customarily used for software interchange; or,\n\
\n\
b) Accompany it with a written offer, valid for at least three years, to give any third party, for a charge no more than your cost of physically performing source distribution, a complete machine-readable copy of the corresponding source code, to be distributed under the terms of Sections 1 and 2 above on a medium customarily used for software interchange; or,\n\
\n\
c) Accompany it with the information you received as to the offer to distribute corresponding source code. (This alternative is allowed only for noncommercial distribution and only if you received the program in object code or executable form with such an offer, in accord with Subsection b above.)\n\
\n\
The source code for a work means the preferred form of the work for making modifications to it. For an executable work, complete source code means all the source code for all modules it contains, plus any associated interface definition files, plus the scripts used to control compilation and installation of the executable. However, as a special exception, the source code distributed need not include anything that is normally distributed (in either source or binary form) with the major components (compiler, kernel, and so on) of the operating system on which the executable runs, unless that component itself accompanies the executable.\n\
\n\
If distribution of executable or object code is made by offering access to copy from a designated place, then offering equivalent access to copy the source code from the same place counts as distribution of the source code, even though third parties are not compelled to copy the source along with the object code.\n\
\n\
4. You may not copy, modify, sublicense, or distribute the Program except as expressly provided under this License. Any attempt otherwise to copy, modify, sublicense or distribute the Program is void, and will automatically terminate your rights under this License. However, parties who have received copies, or rights, from you under this License will not have their licenses terminated so long as such parties remain in full compliance.\n\
\n\
5. You are not required to accept this License, since you have not signed it. However, nothing else grants you permission to modify or distribute the Program or its derivative works. These actions are prohibited by law if you do not accept this License. Therefore, by modifying or distributing the Program (or any work based on the Program), you indicate your acceptance of this License to do so, and all its terms and conditions for copying, distributing or modifying the Program or works based on it.\n\
\n\
6. Each time you redistribute the Program (or any work based on the Program), the recipient automatically receives a license from the original licensor to copy, distribute or modify the Program subject to these terms and conditions. You may not impose any further restrictions on the recipients' exercise of the rights granted herein. You are not responsible for enforcing compliance by third parties to this License.\n\
\n\
7. If, as a consequence of a court judgment or allegation of patent infringement or for any other reason (not limited to patent issues), conditions are imposed on you (whether by court order, agreement or otherwise) that contradict the conditions of this License, they do not excuse you from the conditions of this License. If you cannot distribute so as to satisfy simultaneously your obligations under this License and any other pertinent obligations, then as a consequence you may not distribute the Program at all. For example, if a patent license would not permit royalty-free redistribution of the Program by all those who receive copies directly or indirectly through you, then the only way you could satisfy both it and this License would be to refrain entirely from distribution of the Program.\n\
\n\
If any portion of this section is held invalid or unenforceable under any particular circumstance, the balance of the section is intended to apply and the section as a whole is intended to apply in other circumstances.\n\
\n\
It is not the purpose of this section to induce you to infringe any patents or other property right claims or to contest validity of any such claims; this section has the sole purpose of protecting the integrity of the free software distribution system, which is implemented by public license practices. Many people have made generous contributions to the wide range of software distributed through that system in reliance on consistent application of that system; it is up to the author/donor to decide if he or she is willing to distribute software through any other system and a licensee cannot impose that choice.\n\
\n\
This section is intended to make thoroughly clear what is believed to be a consequence of the rest of this License.\n\
\n\
8. If the distribution and/or use of the Program is restricted in certain countries either by patents or by copyrighted interfaces, the original copyright holder who places the Program under this License may add an explicit geographical distribution limitation excluding those countries, so that distribution is permitted only in or among countries not thus excluded. In such case, this License incorporates the limitation as if written in the body of this License.\n\
\n\
9. The Free Software Foundation may publish revised and/or new versions of the General Public License from time to time. Such new versions will be similar in spirit to the present version, but may differ in detail to address new problems or concerns.\n\
\n\
Each version is given a distinguishing version number. If the Program specifies a version number of this License which applies to it and 'any later version', you have the option of following the terms and conditions either of that version or of any later version published by the Free Software Foundation. If the Program does not specify a version number of this License, you may choose any version ever published by the Free Software Foundation.\n\
\n\
10. If you wish to incorporate parts of the Program into other free programs whose distribution conditions are different, write to the author to ask for permission. For software which is copyrighted by the Free Software Foundation, write to the Free Software Foundation; we sometimes make exceptions for this. Our decision will be guided by the two goals of preserving the free status of all derivatives of our free software and of promoting the sharing and reuse of software generally.\n\
\n\
NO WARRANTY\n\
\n\
11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM 'AS IS' WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n\
\n\
12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.\n\
\n\
END OF TERMS AND CONDITIONS\n\
";
    break;
  case taMisc::GPLv3:
    txt = "GNU General Public License, version 3 (GPLv3)\n\
\n\
GNU GENERAL PUBLIC LICENSE\n\
Version 3, 29 June 2007\n\
\n\
Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>\n\
\n\
Everyone is permitted to copy and distribute verbatim copies of this license document, but changing it is not allowed.\n\
\n\
Preamble\n\
The GNU General Public License is a free, copyleft license for software and other kinds of works.\n\
\n\
The licenses for most software and other practical works are designed to take away your freedom to share and change the works. By contrast, the GNU General Public License is intended to guarantee your freedom to share and change all versions of a program--to make sure it remains free software for all its users. We, the Free Software Foundation, use the GNU General Public License for most of our software; it applies also to any other work released this way by its authors. You can apply it to your programs, too.\n\
\n\
When we speak of free software, we are referring to freedom, not price. Our General Public Licenses are designed to make sure that you have the freedom to distribute copies of free software (and charge for them if you wish), that you receive source code or can get it if you want it, that you can change the software or use pieces of it in new free programs, and that you know you can do these things.\n\
\n\
To protect your rights, we need to prevent others from denying you these rights or asking you to surrender the rights. Therefore, you have certain responsibilities if you distribute copies of the software, or if you modify it: responsibilities to respect the freedom of others.\n\
\n\
For example, if you distribute copies of such a program, whether gratis or for a fee, you must pass on to the recipients the same freedoms that you received. You must make sure that they, too, receive or can get the source code. And you must show them these terms so they know their rights.\n\
\n\
Developers that use the GNU GPL protect your rights with two steps: (1) assert copyright on the software, and (2) offer you this License giving you legal permission to copy, distribute and/or modify it.\n\
\n\
For the developers' and authors' protection, the GPL clearly explains that there is no warranty for this free software. For both users' and authors' sake, the GPL requires that modified versions be marked as changed, so that their problems will not be attributed erroneously to authors of previous versions.\n\
\n\
Some devices are designed to deny users access to install or run modified versions of the software inside them, although the manufacturer can do so. This is fundamentally incompatible with the aim of protecting users' freedom to change the software. The systematic pattern of such abuse occurs in the area of products for individuals to use, which is precisely where it is most unacceptable. Therefore, we have designed this version of the GPL to prohibit the practice for those products. If such problems arise substantially in other domains, we stand ready to extend this provision to those domains in future versions of the GPL, as needed to protect the freedom of users.\n\
\n\
Finally, every program is threatened constantly by software patents. States should not allow patents to restrict development and use of software on general-purpose computers, but in those that do, we wish to avoid the special danger that patents applied to a free program could make it effectively proprietary. To prevent this, the GPL assures that patents cannot be used to render the program non-free.\n\
\n\
The precise terms and conditions for copying, distribution and modification follow.\n\
\n\
TERMS AND CONDITIONS\n\
0. Definitions.\n\
'This License' refers to version 3 of the GNU General Public License.\n\
\n\
'Copyright' also means copyright-like laws that apply to other kinds of works, such as semiconductor masks.\n\
\n\
'The Program' refers to any copyrightable work licensed under this License. Each licensee is addressed as 'you'. 'Licensees' and 'recipients' may be individuals or organizations.\n\
\n\
To 'modify' a work means to copy from or adapt all or part of the work in a fashion requiring copyright permission, other than the making of an exact copy. The resulting work is called a 'modified version' of the earlier work or a work 'based on' the earlier work.\n\
\n\
A 'covered work' means either the unmodified Program or a work based on the Program.\n\
\n\
To 'propagate' a work means to do anything with it that, without permission, would make you directly or secondarily liable for infringement under applicable copyright law, except executing it on a computer or modifying a private copy. Propagation includes copying, distribution (with or without modification), making available to the public, and in some countries other activities as well.\n\
\n\
To 'convey' a work means any kind of propagation that enables other parties to make or receive copies. Mere interaction with a user through a computer network, with no transfer of a copy, is not conveying.\n\
\n\
An interactive user interface displays 'Appropriate Legal Notices' to the extent that it includes a convenient and prominently visible feature that (1) displays an appropriate copyright notice, and (2) tells the user that there is no warranty for the work (except to the extent that warranties are provided), that licensees may convey the work under this License, and how to view a copy of this License. If the interface presents a list of user commands or options, such as a menu, a prominent item in the list meets this criterion.\n\
\n\
1. Source Code.\n\
The 'source code' for a work means the preferred form of the work for making modifications to it. 'Object code' means any non-source form of a work.\n\
\n\
A 'Standard Interface' means an interface that either is an official standard defined by a recognized standards body, or, in the case of interfaces specified for a particular programming language, one that is widely used among developers working in that language.\n\
\n\
The 'System Libraries' of an executable work include anything, other than the work as a whole, that (a) is included in the normal form of packaging a Major Component, but which is not part of that Major Component, and (b) serves only to enable use of the work with that Major Component, or to implement a Standard Interface for which an implementation is available to the public in source code form. A 'Major Component', in this context, means a major essential component (kernel, window system, and so on) of the specific operating system (if any) on which the executable work runs, or a compiler used to produce the work, or an object code interpreter used to run it.\n\
\n\
The 'Corresponding Source' for a work in object code form means all the source code needed to generate, install, and (for an executable work) run the object code and to modify the work, including scripts to control those activities. However, it does not include the work's System Libraries, or general-purpose tools or generally available free programs which are used unmodified in performing those activities but which are not part of the work. For example, Corresponding Source includes interface definition files associated with source files for the work, and the source code for shared libraries and dynamically linked subprograms that the work is specifically designed to require, such as by intimate data communication or control flow between those subprograms and other parts of the work.\n\
\n\
The Corresponding Source need not include anything that users can regenerate automatically from other parts of the Corresponding Source.\n\
\n\
The Corresponding Source for a work in source code form is that same work.\n\
\n\
2. Basic Permissions.\n\
All rights granted under this License are granted for the term of copyright on the Program, and are irrevocable provided the stated conditions are met. This License explicitly affirms your unlimited permission to run the unmodified Program. The output from running a covered work is covered by this License only if the output, given its content, constitutes a covered work. This License acknowledges your rights of fair use or other equivalent, as provided by copyright law.\n\
\n\
You may make, run and propagate covered works that you do not convey, without conditions so long as your license otherwise remains in force. You may convey covered works to others for the sole purpose of having them make modifications exclusively for you, or provide you with facilities for running those works, provided that you comply with the terms of this License in conveying all material for which you do not control copyright. Those thus making or running the covered works for you must do so exclusively on your behalf, under your direction and control, on terms that prohibit them from making any copies of your copyrighted material outside their relationship with you.\n\
\n\
Conveying under any other circumstances is permitted solely under the conditions stated below. Sublicensing is not allowed; section 10 makes it unnecessary.\n\
\n\
3. Protecting Users' Legal Rights From Anti-Circumvention Law.\n\
No covered work shall be deemed part of an effective technological measure under any applicable law fulfilling obligations under article 11 of the WIPO copyright treaty adopted on 20 December 1996, or similar laws prohibiting or restricting circumvention of such measures.\n\
\n\
When you convey a covered work, you waive any legal power to forbid circumvention of technological measures to the extent such circumvention is effected by exercising rights under this License with respect to the covered work, and you disclaim any intention to limit operation or modification of the work as a means of enforcing, against the work's users, your or third parties' legal rights to forbid circumvention of technological measures.\n\
\n\
4. Conveying Verbatim Copies.\n\
You may convey verbatim copies of the Program's source code as you receive it, in any medium, provided that you conspicuously and appropriately publish on each copy an appropriate copyright notice; keep intact all notices stating that this License and any non-permissive terms added in accord with section 7 apply to the code; keep intact all notices of the absence of any warranty; and give all recipients a copy of this License along with the Program.\n\
\n\
You may charge any price or no price for each copy that you convey, and you may offer support or warranty protection for a fee.\n\
\n\
5. Conveying Modified Source Versions.\n\
You may convey a work based on the Program, or the modifications to produce it from the Program, in the form of source code under the terms of section 4, provided that you also meet all of these conditions:\n\
\n\
a) The work must carry prominent notices stating that you modified it, and giving a relevant date.\n\
b) The work must carry prominent notices stating that it is released under this License and any conditions added under section 7. This requirement modifies the requirement in section 4 to 'keep intact all notices'.\n\
c) You must license the entire work, as a whole, under this License to anyone who comes into possession of a copy. This License will therefore apply, along with any applicable section 7 additional terms, to the whole of the work, and all its parts, regardless of how they are packaged. This License gives no permission to license the work in any other way, but it does not invalidate such permission if you have separately received it.\n\
d) If the work has interactive user interfaces, each must display Appropriate Legal Notices; however, if the Program has interactive interfaces that do not display Appropriate Legal Notices, your work need not make them do so.\n\
A compilation of a covered work with other separate and independent works, which are not by their nature extensions of the covered work, and which are not combined with it such as to form a larger program, in or on a volume of a storage or distribution medium, is called an 'aggregate' if the compilation and its resulting copyright are not used to limit the access or legal rights of the compilation's users beyond what the individual works permit. Inclusion of a covered work in an aggregate does not cause this License to apply to the other parts of the aggregate.\n\
\n\
6. Conveying Non-Source Forms.\n\
You may convey a covered work in object code form under the terms of sections 4 and 5, provided that you also convey the machine-readable Corresponding Source under the terms of this License, in one of these ways:\n\
\n\
a) Convey the object code in, or embodied in, a physical product (including a physical distribution medium), accompanied by the Corresponding Source fixed on a durable physical medium customarily used for software interchange.\n\
b) Convey the object code in, or embodied in, a physical product (including a physical distribution medium), accompanied by a written offer, valid for at least three years and valid for as long as you offer spare parts or customer support for that product model, to give anyone who possesses the object code either (1) a copy of the Corresponding Source for all the software in the product that is covered by this License, on a durable physical medium customarily used for software interchange, for a price no more than your reasonable cost of physically performing this conveying of source, or (2) access to copy the Corresponding Source from a network server at no charge.\n\
c) Convey individual copies of the object code with a copy of the written offer to provide the Corresponding Source. This alternative is allowed only occasionally and noncommercially, and only if you received the object code with such an offer, in accord with subsection 6b.\n\
d) Convey the object code by offering access from a designated place (gratis or for a charge), and offer equivalent access to the Corresponding Source in the same way through the same place at no further charge. You need not require recipients to copy the Corresponding Source along with the object code. If the place to copy the object code is a network server, the Corresponding Source may be on a different server (operated by you or a third party) that supports equivalent copying facilities, provided you maintain clear directions next to the object code saying where to find the Corresponding Source. Regardless of what server hosts the Corresponding Source, you remain obligated to ensure that it is available for as long as needed to satisfy these requirements.\n\
e) Convey the object code using peer-to-peer transmission, provided you inform other peers where the object code and Corresponding Source of the work are being offered to the general public at no charge under subsection 6d.\n\
A separable portion of the object code, whose source code is excluded from the Corresponding Source as a System Library, need not be included in conveying the object code work.\n\
\n\
A 'User Product' is either (1) a 'consumer product', which means any tangible personal property which is normally used for personal, family, or household purposes, or (2) anything designed or sold for incorporation into a dwelling. In determining whether a product is a consumer product, doubtful cases shall be resolved in favor of coverage. For a particular product received by a particular user, 'normally used' refers to a typical or common use of that class of product, regardless of the status of the particular user or of the way in which the particular user actually uses, or expects or is expected to use, the product. A product is a consumer product regardless of whether the product has substantial commercial, industrial or non-consumer uses, unless such uses represent the only significant mode of use of the product.\n\
\n";
txt += "'Installation Information' for a User Product means any methods, procedures, authorization keys, or other information required to install and execute modified versions of a covered work in that User Product from a modified version of its Corresponding Source. The information must suffice to ensure that the continued functioning of the modified object code is in no case prevented or interfered with solely because modification has been made.\n\
\n\
If you convey an object code work under this section in, or with, or specifically for use in, a User Product, and the conveying occurs as part of a transaction in which the right of possession and use of the User Product is transferred to the recipient in perpetuity or for a fixed term (regardless of how the transaction is characterized), the Corresponding Source conveyed under this section must be accompanied by the Installation Information. But this requirement does not apply if neither you nor any third party retains the ability to install modified object code on the User Product (for example, the work has been installed in ROM).\n\
\n\
The requirement to provide Installation Information does not include a requirement to continue to provide support service, warranty, or updates for a work that has been modified or installed by the recipient, or for the User Product in which it has been modified or installed. Access to a network may be denied when the modification itself materially and adversely affects the operation of the network or violates the rules and protocols for communication across the network.\n\
\n\
Corresponding Source conveyed, and Installation Information provided, in accord with this section must be in a format that is publicly documented (and with an implementation available to the public in source code form), and must require no special password or key for unpacking, reading or copying.\n\
\n\
7. Additional Terms.\n\
'Additional permissions' are terms that supplement the terms of this License by making exceptions from one or more of its conditions. Additional permissions that are applicable to the entire Program shall be treated as though they were included in this License, to the extent that they are valid under applicable law. If additional permissions apply only to part of the Program, that part may be used separately under those permissions, but the entire Program remains governed by this License without regard to the additional permissions.\n\
\n\
When you convey a copy of a covered work, you may at your option remove any additional permissions from that copy, or from any part of it. (Additional permissions may be written to require their own removal in certain cases when you modify the work.) You may place additional permissions on material, added by you to a covered work, for which you have or can give appropriate copyright permission.\n\
\n\
Notwithstanding any other provision of this License, for material you add to a covered work, you may (if authorized by the copyright holders of that material) supplement the terms of this License with terms:\n\
\n\
a) Disclaiming warranty or limiting liability differently from the terms of sections 15 and 16 of this License; or\n\
b) Requiring preservation of specified reasonable legal notices or author attributions in that material or in the Appropriate Legal Notices displayed by works containing it; or\n\
c) Prohibiting misrepresentation of the origin of that material, or requiring that modified versions of such material be marked in reasonable ways as different from the original version; or\n\
d) Limiting the use for publicity purposes of names of licensors or authors of the material; or\n\
e) Declining to grant rights under trademark law for use of some trade names, trademarks, or service marks; or\n\
f) Requiring indemnification of licensors and authors of that material by anyone who conveys the material (or modified versions of it) with contractual assumptions of liability to the recipient, for any liability that these contractual assumptions directly impose on those licensors and authors.\n\
All other non-permissive additional terms are considered 'further restrictions' within the meaning of section 10. If the Program as you received it, or any part of it, contains a notice stating that it is governed by this License along with a term that is a further restriction, you may remove that term. If a license document contains a further restriction but permits relicensing or conveying under this License, you may add to a covered work material governed by the terms of that license document, provided that the further restriction does not survive such relicensing or conveying.\n\
\n\
If you add terms to a covered work in accord with this section, you must place, in the relevant source files, a statement of the additional terms that apply to those files, or a notice indicating where to find the applicable terms.\n\
\n\
Additional terms, permissive or non-permissive, may be stated in the form of a separately written license, or stated as exceptions; the above requirements apply either way.\n\
\n\
8. Termination.\n\
You may not propagate or modify a covered work except as expressly provided under this License. Any attempt otherwise to propagate or modify it is void, and will automatically terminate your rights under this License (including any patent licenses granted under the third paragraph of section 11).\n\
\n\
However, if you cease all violation of this License, then your license from a particular copyright holder is reinstated (a) provisionally, unless and until the copyright holder explicitly and finally terminates your license, and (b) permanently, if the copyright holder fails to notify you of the violation by some reasonable means prior to 60 days after the cessation.\n\
\n\
Moreover, your license from a particular copyright holder is reinstated permanently if the copyright holder notifies you of the violation by some reasonable means, this is the first time you have received notice of violation of this License (for any work) from that copyright holder, and you cure the violation prior to 30 days after your receipt of the notice.\n\
\n\
Termination of your rights under this section does not terminate the licenses of parties who have received copies or rights from you under this License. If your rights have been terminated and not permanently reinstated, you do not qualify to receive new licenses for the same material under section 10.\n\
\n\
9. Acceptance Not Required for Having Copies.\n\
You are not required to accept this License in order to receive or run a copy of the Program. Ancillary propagation of a covered work occurring solely as a consequence of using peer-to-peer transmission to receive a copy likewise does not require acceptance. However, nothing other than this License grants you permission to propagate or modify any covered work. These actions infringe copyright if you do not accept this License. Therefore, by modifying or propagating a covered work, you indicate your acceptance of this License to do so.\n\
\n\
10. Automatic Licensing of Downstream Recipients.\n\
Each time you convey a covered work, the recipient automatically receives a license from the original licensors, to run, modify and propagate that work, subject to this License. You are not responsible for enforcing compliance by third parties with this License.\n\
\n\
An 'entity transaction' is a transaction transferring control of an organization, or substantially all assets of one, or subdividing an organization, or merging organizations. If propagation of a covered work results from an entity transaction, each party to that transaction who receives a copy of the work also receives whatever licenses to the work the party's predecessor in interest had or could give under the previous paragraph, plus a right to possession of the Corresponding Source of the work from the predecessor in interest, if the predecessor has it or can get it with reasonable efforts.\n\
\n\
You may not impose any further restrictions on the exercise of the rights granted or affirmed under this License. For example, you may not impose a license fee, royalty, or other charge for exercise of rights granted under this License, and you may not initiate litigation (including a cross-claim or counterclaim in a lawsuit) alleging that any patent claim is infringed by making, using, selling, offering for sale, or importing the Program or any portion of it.\n\
\n\
11. Patents.\n\
A 'contributor' is a copyright holder who authorizes use under this License of the Program or a work on which the Program is based. The work thus licensed is called the contributor's 'contributor version'.\n\
\n\
A contributor's 'essential patent claims' are all patent claims owned or controlled by the contributor, whether already acquired or hereafter acquired, that would be infringed by some manner, permitted by this License, of making, using, or selling its contributor version, but do not include claims that would be infringed only as a consequence of further modification of the contributor version. For purposes of this definition, 'control' includes the right to grant patent sublicenses in a manner consistent with the requirements of this License.\n\
\n\
Each contributor grants you a non-exclusive, worldwide, royalty-free patent license under the contributor's essential patent claims, to make, use, sell, offer for sale, import and otherwise run, modify and propagate the contents of its contributor version.\n\
\n\
In the following three paragraphs, a 'patent license' is any express agreement or commitment, however denominated, not to enforce a patent (such as an express permission to practice a patent or covenant not to sue for patent infringement). To 'grant' such a patent license to a party means to make such an agreement or commitment not to enforce a patent against the party.\n\
\n\
If you convey a covered work, knowingly relying on a patent license, and the Corresponding Source of the work is not available for anyone to copy, free of charge and under the terms of this License, through a publicly available network server or other readily accessible means, then you must either (1) cause the Corresponding Source to be so available, or (2) arrange to deprive yourself of the benefit of the patent license for this particular work, or (3) arrange, in a manner consistent with the requirements of this License, to extend the patent license to downstream recipients. 'Knowingly relying' means you have actual knowledge that, but for the patent license, your conveying the covered work in a country, or your recipient's use of the covered work in a country, would infringe one or more identifiable patents in that country that you have reason to believe are valid.\n\
\n\
If, pursuant to or in connection with a single transaction or arrangement, you convey, or propagate by procuring conveyance of, a covered work, and grant a patent license to some of the parties receiving the covered work authorizing them to use, propagate, modify or convey a specific copy of the covered work, then the patent license you grant is automatically extended to all recipients of the covered work and works based on it.\n\
\n\
A patent license is 'discriminatory' if it does not include within the scope of its coverage, prohibits the exercise of, or is conditioned on the non-exercise of one or more of the rights that are specifically granted under this License. You may not convey a covered work if you are a party to an arrangement with a third party that is in the business of distributing software, under which you make payment to the third party based on the extent of your activity of conveying the work, and under which the third party grants, to any of the parties who would receive the covered work from you, a discriminatory patent license (a) in connection with copies of the covered work conveyed by you (or copies made from those copies), or (b) primarily for and in connection with specific products or compilations that contain the covered work, unless you entered into that arrangement, or that patent license was granted, prior to 28 March 2007.\n\
\n\
Nothing in this License shall be construed as excluding or limiting any implied license or other defenses to infringement that may otherwise be available to you under applicable patent law.\n\
\n\
12. No Surrender of Others' Freedom.\n\
If conditions are imposed on you (whether by court order, agreement or otherwise) that contradict the conditions of this License, they do not excuse you from the conditions of this License. If you cannot convey a covered work so as to satisfy simultaneously your obligations under this License and any other pertinent obligations, then as a consequence you may not convey it at all. For example, if you agree to terms that obligate you to collect a royalty for further conveying from those to whom you convey the Program, the only way you could satisfy both those terms and this License would be to refrain entirely from conveying the Program.\n\
\n\
13. Use with the GNU Affero General Public License.\n\
Notwithstanding any other provision of this License, you have permission to link or combine any covered work with a work licensed under version 3 of the GNU Affero General Public License into a single combined work, and to convey the resulting work. The terms of this License will continue to apply to the part which is the covered work, but the special requirements of the GNU Affero General Public License, section 13, concerning interaction through a network will apply to the combination as such.\n\
\n\
14. Revised Versions of this License.\n\
The Free Software Foundation may publish revised and/or new versions of the GNU General Public License from time to time. Such new versions will be similar in spirit to the present version, but may differ in detail to address new problems or concerns.\n\
\n\
Each version is given a distinguishing version number. If the Program specifies that a certain numbered version of the GNU General Public License 'or any later version' applies to it, you have the option of following the terms and conditions either of that numbered version or of any later version published by the Free Software Foundation. If the Program does not specify a version number of the GNU General Public License, you may choose any version ever published by the Free Software Foundation.\n\
\n\
If the Program specifies that a proxy can decide which future versions of the GNU General Public License can be used, that proxy's public statement of acceptance of a version permanently authorizes you to choose that version for the Program.\n\
\n\
Later license versions may give you additional or different permissions. However, no additional obligations are imposed on any author or copyright holder as a result of your choosing to follow a later version.\n\
\n\
15. Disclaimer of Warranty.\n\
THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM 'AS IS' WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n\
\n\
16. Limitation of Liability.\n\
IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.\n\
\n\
17. Interpretation of Sections 15 and 16.\n\
If the disclaimer of warranty and limitation of liability provided above cannot be given local legal effect according to their terms, reviewing courts shall apply local law that most closely approximates an absolute waiver of all civil liability in connection with the Program, unless a warranty or assumption of liability accompanies a copy of the Program in return for a fee.\n\
\n\
END OF TERMS AND CONDITIONS\n\
";
    break;
  case taMisc::BSD:
    txt = "All rights reserved.\n\
\n\
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:\n\
\n\
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.\n\
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.\n\
Neither the name of the " + eff_org + " nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.\n\
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\
";
    break;
  case taMisc::OSL3:
    txt = "Licensed under the Open Software License version 3.0\n\
\n\
1) Grant of Copyright License. Licensor grants You a worldwide, royalty-free, non-exclusive, sublicensable license, for the duration of the copyright, to do the following:\n\
\n\
a) to reproduce the Original Work in copies, either alone or as part of a collective work;\n\
\n\
b) to translate, adapt, alter, transform, modify, or arrange the Original Work, thereby creating derivative works ('Derivative Works') based upon the Original Work;\n\
\n\
c) to distribute or communicate copies of the Original Work and Derivative Works to the public, with the proviso that copies of Original Work or Derivative Works that You distribute or communicate shall be licensed under this Open Software License;\n\
\n\
d) to perform the Original Work publicly; and\n\
\n\
e) to display the Original Work publicly.\n\
\n\
2) Grant of Patent License. Licensor grants You a worldwide, royalty-free, non-exclusive, sublicensable license, under patent claims owned or controlled by the Licensor that are embodied in the Original Work as furnished by the Licensor, for the duration of the patents, to make, use, sell, offer for sale, have made, and import the Original Work and Derivative Works.\n\
\n\
3) Grant of Source Code License. The term 'Source Code' means the preferred form of the Original Work for making modifications to it and all available documentation describing how to modify the Original Work. Licensor agrees to provide a machine-readable copy of the Source Code of the Original Work along with each copy of the Original Work that Licensor distributes. Licensor reserves the right to satisfy this obligation by placing a machine-readable copy of the Source Code in an information repository reasonably calculated to permit inexpensive and convenient access by You for as long as Licensor continues to distribute the Original Work.\n\
\n\
4) Exclusions From License Grant. Neither the names of Licensor, nor the names of any contributors to the Original Work, nor any of their trademarks or service marks, may be used to endorse or promote products derived from this Original Work without express prior permission of the Licensor. Except as expressly stated herein, nothing in this License grants any license to Licensor's trademarks, copyrights, patents, trade secrets or any other intellectual property. No patent license is granted to make, use, sell, offer for sale, have made, or import embodiments of any patent claims other than the licensed claims defined in Section 2. No license is granted to the trademarks of Licensor even if such marks are included in the Original Work. Nothing in this License shall be interpreted to prohibit Licensor from licensing under terms different from this License any Original Work that Licensor otherwise would have a right to license.\n\
\n\
5) External Deployment. The term 'External Deployment' means the use, distribution, or communication of the Original Work or Derivative Works in any way such that the Original Work or Derivative Works may be used by anyone other than You, whether those works are distributed or communicated to those persons or made available as an application intended for use over a network. As an express condition for the grants of license hereunder, You must treat any External Deployment by You of the Original Work or a Derivative Work as a distribution under section 1(c).\n\
\n\
6) Attribution Rights. You must retain, in the Source Code of any Derivative Works that You create, all copyright, patent, or trademark notices from the Source Code of the Original Work, as well as any notices of licensing and any descriptive text identified therein as an 'Attribution Notice.' You must cause the Source Code for any Derivative Works that You create to carry a prominent Attribution Notice reasonably calculated to inform recipients that You have modified the Original Work.\n\
\n\
7) Warranty of Provenance and Disclaimer of Warranty. Licensor warrants that the copyright in and to the Original Work and the patent rights granted herein by Licensor are owned by the Licensor or are sublicensed to You under the terms of this License with the permission of the contributor(s) of those copyrights and patent rights. Except as expressly stated in the immediately preceding sentence, the Original Work is provided under this License on an 'AS IS' BASIS and WITHOUT WARRANTY, either express or implied, including, without limitation, the warranties of non-infringement, merchantability or fitness for a particular purpose. THE ENTIRE RISK AS TO THE QUALITY OF THE ORIGINAL WORK IS WITH YOU. This DISCLAIMER OF WARRANTY constitutes an essential part of this License. No license to the Original Work is granted by this License except under this disclaimer.\n\
\n\
8) Limitation of Liability. Under no circumstances and under no legal theory, whether in tort (including negligence), contract, or otherwise, shall the Licensor be liable to anyone for any indirect, special, incidental, or consequential damages of any character arising as a result of this License or the use of the Original Work including, without limitation, damages for loss of goodwill, work stoppage, computer failure or malfunction, or any and all other commercial damages or losses. This limitation of liability shall not apply to the extent applicable law prohibits such limitation.\n\
\n\
9) Acceptance and Termination. If, at any time, You expressly assented to this License, that assent indicates your clear and irrevocable acceptance of this License and all of its terms and conditions. If You distribute or communicate copies of the Original Work or a Derivative Work, You must make a reasonable effort under the circumstances to obtain the express assent of recipients to the terms of this License. This License conditions your rights to undertake the activities listed in Section 1, including your right to create Derivative Works based upon the Original Work, and doing so without honoring these terms and conditions is prohibited by copyright law and international treaty. Nothing in this License is intended to affect copyright exceptions and limitations (including 'fair use' or 'fair dealing'). This License shall terminate immediately and You may no longer exercise any of the rights granted to You by this License upon your failure to honor the conditions in Section 1(c).\n\
\n\
10) Termination for Patent Action. This License shall terminate automatically and You may no longer exercise any of the rights granted to You by this License as of the date You commence an action, including a cross-claim or counterclaim, against Licensor or any licensee alleging that the Original Work infringes a patent. This termination provision shall not apply for an action alleging patent infringement by combinations of the Original Work with other software or hardware.\n\
\n\
11) Jurisdiction, Venue and Governing Law. Any action or suit relating to this License may be brought only in the courts of a jurisdiction wherein the Licensor resides or in which Licensor conducts its primary business, and under the laws of that jurisdiction excluding its conflict-of-law provisions. The application of the United Nations Convention on Contracts for the International Sale of Goods is expressly excluded. Any use of the Original Work outside the scope of this License or after its termination shall be subject to the requirements and penalties of copyright or patent law in the appropriate jurisdiction. This section shall survive the termination of this License.\n\
\n\
12) Attorneys' Fees. In any action to enforce the terms of this License or seeking damages relating thereto, the prevailing party shall be entitled to recover its costs and expenses, including, without limitation, reasonable attorneys' fees and costs incurred in connection with such action, including any appeal of such action. This section shall survive the termination of this License.\n\
\n\
13) Miscellaneous. If any provision of this License is held to be unenforceable, such provision shall be reformed only to the extent necessary to make it enforceable.\n\
\n\
14) Definition of 'You' in This License. 'You' throughout this License, whether in upper or lower case, means an individual or a legal entity exercising rights under, and complying with all of the terms of, this License. For legal entities, 'You' includes any entity that controls, is controlled by, or is under common control with you. For purposes of this definition, 'control' means (i) the power, direct or indirect, to cause the direction or management of such entity, whether by contract or otherwise, or (ii) ownership of fifty percent (50%) or more of the outstanding shares, or (iii) beneficial ownership of such entity.\n\
\n\
15) Right to Use. You may use the Original Work in all ways not otherwise restricted or conditioned by this License or by law, and Licensor promises not to interfere with or be responsible for such uses by You.\n\
\n\
16) Modification of This License. This License is Copyright © 2005 Lawrence Rosen. Permission is granted to copy, distribute, or communicate this License without modification. Nothing in this License permits You to modify this License as applied to the Original Work or to Derivative Works. However, You may modify the text of this License and copy, distribute or communicate your modified version (the 'Modified License') and apply it to other original works of authorship subject to the following conditions: (i) You may not indicate in any way that your Modified License is the 'Open Software License' or 'OSL' and you may not use those names in the name of your Modified License; (ii) You must replace the notice specified in the first paragraph above with the notice 'Licensed under <insert your license name here>' or with a notice of your own that is not confusingly similar to the notice in this License; and (iii) You may not claim that your original works are open source software unless your Modified License has been approved by Open Source Initiative (OSI) and You comply with its license review and certification process.\n\
";
    break;
  case taMisc::AFL3:
    txt = "Licensed under the Academic Free License version 3.0\n\\n\
\n\
1) Grant of Copyright License. Licensor grants You a worldwide, royalty-free, non-exclusive, sublicensable license, for the duration of the copyright, to do the following:\n\
\n\
a) to reproduce the Original Work in copies, either alone or as part of a collective work;\n\
\n\
b) to translate, adapt, alter, transform, modify, or arrange the Original Work, thereby creating derivative works ('Derivative Works') based upon the Original Work;\n\
\n\
c) to distribute or communicate copies of the Original Work and Derivative Works to the public, under any license of your choice that does not contradict the terms and conditions, including Licensor's reserved rights and remedies, in this Academic Free License;\n\
\n\
d) to perform the Original Work publicly; and\n\
\n\
e) to display the Original Work publicly.\n\
\n\
2) Grant of Patent License. Licensor grants You a worldwide, royalty-free, non-exclusive, sublicensable license, under patent claims owned or controlled by the Licensor that are embodied in the Original Work as furnished by the Licensor, for the duration of the patents, to make, use, sell, offer for sale, have made, and import the Original Work and Derivative Works.\n\
\n\
3) Grant of Source Code License. The term 'Source Code' means the preferred form of the Original Work for making modifications to it and all available documentation describing how to modify the Original Work. Licensor agrees to provide a machine-readable copy of the Source Code of the Original Work along with each copy of the Original Work that Licensor distributes. Licensor reserves the right to satisfy this obligation by placing a machine-readable copy of the Source Code in an information repository reasonably calculated to permit inexpensive and convenient access by You for as long as Licensor continues to distribute the Original Work.\n\
\n\
4) Exclusions From License Grant. Neither the names of Licensor, nor the names of any contributors to the Original Work, nor any of their trademarks or service marks, may be used to endorse or promote products derived from this Original Work without express prior permission of the Licensor. Except as expressly stated herein, nothing in this License grants any license to Licensor's trademarks, copyrights, patents, trade secrets or any other intellectual property. No patent license is granted to make, use, sell, offer for sale, have made, or import embodiments of any patent claims other than the licensed claims defined in Section 2. No license is granted to the trademarks of Licensor even if such marks are included in the Original Work. Nothing in this License shall be interpreted to prohibit Licensor from licensing under terms different from this License any Original Work that Licensor otherwise would have a right to license.\n\
\n\
5) External Deployment. The term 'External Deployment' means the use, distribution, or communication of the Original Work or Derivative Works in any way such that the Original Work or Derivative Works may be used by anyone other than You, whether those works are distributed or communicated to those persons or made available as an application intended for use over a network. As an express condition for the grants of license hereunder, You must treat any External Deployment by You of the Original Work or a Derivative Work as a distribution under section 1(c).\n\
\n\
6) Attribution Rights. You must retain, in the Source Code of any Derivative Works that You create, all copyright, patent, or trademark notices from the Source Code of the Original Work, as well as any notices of licensing and any descriptive text identified therein as an 'Attribution Notice.' You must cause the Source Code for any Derivative Works that You create to carry a prominent Attribution Notice reasonably calculated to inform recipients that You have modified the Original Work.\n\
\n\
7) Warranty of Provenance and Disclaimer of Warranty. Licensor warrants that the copyright in and to the Original Work and the patent rights granted herein by Licensor are owned by the Licensor or are sublicensed to You under the terms of this License with the permission of the contributor(s) of those copyrights and patent rights. Except as expressly stated in the immediately preceding sentence, the Original Work is provided under this License on an 'AS IS' BASIS and WITHOUT WARRANTY, either express or implied, including, without limitation, the warranties of non-infringement, merchantability or fitness for a particular purpose. THE ENTIRE RISK AS TO THE QUALITY OF THE ORIGINAL WORK IS WITH YOU. This DISCLAIMER OF WARRANTY constitutes an essential part of this License. No license to the Original Work is granted by this License except under this disclaimer.\n\
\n\
8) Limitation of Liability. Under no circumstances and under no legal theory, whether in tort (including negligence), contract, or otherwise, shall the Licensor be liable to anyone for any indirect, special, incidental, or consequential damages of any character arising as a result of this License or the use of the Original Work including, without limitation, damages for loss of goodwill, work stoppage, computer failure or malfunction, or any and all other commercial damages or losses. This limitation of liability shall not apply to the extent applicable law prohibits such limitation.\n\
\n\
9) Acceptance and Termination. If, at any time, You expressly assented to this License, that assent indicates your clear and irrevocable acceptance of this License and all of its terms and conditions. If You distribute or communicate copies of the Original Work or a Derivative Work, You must make a reasonable effort under the circumstances to obtain the express assent of recipients to the terms of this License. This License conditions your rights to undertake the activities listed in Section 1, including your right to create Derivative Works based upon the Original Work, and doing so without honoring these terms and conditions is prohibited by copyright law and international treaty. Nothing in this License is intended to affect copyright exceptions and limitations (including 'fair use' or 'fair dealing'). This License shall terminate immediately and You may no longer exercise any of the rights granted to You by this License upon your failure to honor the conditions in Section 1(c).\n\
\n\
10) Termination for Patent Action. This License shall terminate automatically and You may no longer exercise any of the rights granted to You by this License as of the date You commence an action, including a cross-claim or counterclaim, against Licensor or any licensee alleging that the Original Work infringes a patent. This termination provision shall not apply for an action alleging patent infringement by combinations of the Original Work with other software or hardware.\n\
\n\
11) Jurisdiction, Venue and Governing Law. Any action or suit relating to this License may be brought only in the courts of a jurisdiction wherein the Licensor resides or in which Licensor conducts its primary business, and under the laws of that jurisdiction excluding its conflict-of-law provisions. The application of the United Nations Convention on Contracts for the International Sale of Goods is expressly excluded. Any use of the Original Work outside the scope of this License or after its termination shall be subject to the requirements and penalties of copyright or patent law in the appropriate jurisdiction. This section shall survive the termination of this License.\n\
\n\
12) Attorneys' Fees. In any action to enforce the terms of this License or seeking damages relating thereto, the prevailing party shall be entitled to recover its costs and expenses, including, without limitation, reasonable attorneys' fees and costs incurred in connection with such action, including any appeal of such action. This section shall survive the termination of this License.\n\
\n\
13) Miscellaneous. If any provision of this License is held to be unenforceable, such provision shall be reformed only to the extent necessary to make it enforceable.\n\
\n\
14) Definition of 'You' in This License. 'You' throughout this License, whether in upper or lower case, means an individual or a legal entity exercising rights under, and complying with all of the terms of, this License. For legal entities, 'You' includes any entity that controls, is controlled by, or is under common control with you. For purposes of this definition, 'control' means (i) the power, direct or indirect, to cause the direction or management of such entity, whether by contract or otherwise, or (ii) ownership of fifty percent (50%) or more of the outstanding shares, or (iii) beneficial ownership of such entity.\n\
\n\
15) Right to Use. You may use the Original Work in all ways not otherwise restricted or conditioned by this License or by law, and Licensor promises not to interfere with or be responsible for such uses by You.\n\
\n\
16) Modification of This License. This License is Copyright © 2005 Lawrence Rosen. Permission is granted to copy, distribute, or communicate this License without modification. Nothing in this License permits You to modify this License as applied to the Original Work or to Derivative Works. However, You may modify the text of this License and copy, distribute or communicate your modified version (the 'Modified License') and apply it to other original works of authorship subject to the following conditions: (i) You may not indicate in any way that your Modified License is the 'Academic Free License' or 'AFL' and you may not use those names in the name of your Modified License; (ii) You must replace the notice specified in the first paragraph above with the notice 'Licensed under <insert your license name here>' or with a notice of your own that is not confusingly similar to the notice in this License; and (iii) You may not claim that your original works are open source software unless your Modified License has been approved by Open Source Initiative (OSI) and You comply with its license review and certification process.\n\
";
    break;
  case taMisc::MIT:
    txt = "Permission is hereby granted, free of charge, to any person obtaining a copy\n\
of this software and associated documentation files (the 'Software'), to deal\n\
in the Software without restriction, including without limitation the rights\n\
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n\
copies of the Software, and to permit persons to whom the Software is\n\
furnished to do so, subject to the following conditions:\n\
\n\
The above copyright notice and this permission notice shall be included in\n\
all copies or substantial portions of the Software.\n\
\n\
THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n\
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n\
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n\
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n\
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n\
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n\
THE SOFTWARE.\n\
";
    break;
  case taMisc::APACHE:
    txt = "Apache License\n\
Version 2.0, January 2004\n\
http://www.apache.org/licenses/\n\
\n\
TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION\n\
\n\
1. Definitions.\n\
\n\
'License' shall mean the terms and conditions for use, reproduction, and distribution as defined by Sections 1 through 9 of this document.\n\
\n\
'Licensor' shall mean the copyright owner or entity authorized by the copyright owner that is granting the License.\n\
\n\
'Legal Entity' shall mean the union of the acting entity and all other entities that control, are controlled by, or are under common control with that entity. For the purposes of this definition, 'control' means (i) the power, direct or indirect, to cause the direction or management of such entity, whether by contract or otherwise, or (ii) ownership of fifty percent (50%) or more of the outstanding shares, or (iii) beneficial ownership of such entity.\n\
\n\
'You' (or 'Your') shall mean an individual or Legal Entity exercising permissions granted by this License.\n\
\n\
'Source' form shall mean the preferred form for making modifications, including but not limited to software source code, documentation source, and configuration files.\n\
\n\
'Object' form shall mean any form resulting from mechanical transformation or translation of a Source form, including but not limited to compiled object code, generated documentation, and conversions to other media types.\n\
\n\
'Work' shall mean the work of authorship, whether in Source or Object form, made available under the License, as indicated by a copyright notice that is included in or attached to the work (an example is provided in the Appendix below).\n\
\n\
'Derivative Works' shall mean any work, whether in Source or Object form, that is based on (or derived from) the Work and for which the editorial revisions, annotations, elaborations, or other modifications represent, as a whole, an original work of authorship. For the purposes of this License, Derivative Works shall not include works that remain separable from, or merely link (or bind by name) to the interfaces of, the Work and Derivative Works thereof.\n\
\n\
'Contribution' shall mean any work of authorship, including the original version of the Work and any modifications or additions to that Work or Derivative Works thereof, that is intentionally submitted to Licensor for inclusion in the Work by the copyright owner or by an individual or Legal Entity authorized to submit on behalf of the copyright owner. For the purposes of this definition, 'submitted' means any form of electronic, verbal, or written communication sent to the Licensor or its representatives, including but not limited to communication on electronic mailing lists, source code control systems, and issue tracking systems that are managed by, or on behalf of, the Licensor for the purpose of discussing and improving the Work, but excluding communication that is conspicuously marked or otherwise designated in writing by the copyright owner as 'Not a Contribution.'\n\
\n\
'Contributor' shall mean Licensor and any individual or Legal Entity on behalf of whom a Contribution has been received by Licensor and subsequently incorporated within the Work.\n\
\n\
2. Grant of Copyright License.\n\
\n\
Subject to the terms and conditions of this License, each Contributor hereby grants to You a perpetual, worldwide, non-exclusive, no-charge, royalty-free, irrevocable copyright license to reproduce, prepare Derivative Works of, publicly display, publicly perform, sublicense, and distribute the Work and such Derivative Works in Source or Object form.\n\
\n\
3. Grant of Patent License.\n\
\n\
Subject to the terms and conditions of this License, each Contributor hereby grants to You a perpetual, worldwide, non-exclusive, no-charge, royalty-free, irrevocable (except as stated in this section) patent license to make, have made, use, offer to sell, sell, import, and otherwise transfer the Work, where such license applies only to those patent claims licensable by such Contributor that are necessarily infringed by their Contribution(s) alone or by combination of their Contribution(s) with the Work to which such Contribution(s) was submitted. If You institute patent litigation against any entity (including a cross-claim or counterclaim in a lawsuit) alleging that the Work or a Contribution incorporated within the Work constitutes direct or contributory patent infringement, then any patent licenses granted to You under this License for that Work shall terminate as of the date such litigation is filed.\n\
\n\
4. Redistribution.\n\
\n\
You may reproduce and distribute copies of the Work or Derivative Works thereof in any medium, with or without modifications, and in Source or Object form, provided that You meet the following conditions:\n\
\n\
You must give any other recipients of the Work or Derivative Works a copy of this License; and\n\
You must cause any modified files to carry prominent notices stating that You changed the files; and\n\
You must retain, in the Source form of any Derivative Works that You distribute, all copyright, patent, trademark, and attribution notices from the Source form of the Work, excluding those notices that do not pertain to any part of the Derivative Works; and\n\
If the Work includes a 'NOTICE' text file as part of its distribution, then any Derivative Works that You distribute must include a readable copy of the attribution notices contained within such NOTICE file, excluding those notices that do not pertain to any part of the Derivative Works, in at least one of the following places: within a NOTICE text file distributed as part of the Derivative Works; within the Source form or documentation, if provided along with the Derivative Works; or, within a display generated by the Derivative Works, if and wherever such third-party notices normally appear. The contents of the NOTICE file are for informational purposes only and do not modify the License. You may add Your own attribution notices within Derivative Works that You distribute, alongside or as an addendum to the NOTICE text from the Work, provided that such additional attribution notices cannot be construed as modifying the License.\n\
You may add Your own copyright statement to Your modifications and may provide additional or different license terms and conditions for use, reproduction, or distribution of Your modifications, or for any such Derivative Works as a whole, provided Your use, reproduction, and distribution of the Work otherwise complies with the conditions stated in this License.\n\
\n\
5. Submission of Contributions.\n\
\n\
Unless You explicitly state otherwise, any Contribution intentionally submitted for inclusion in the Work by You to the Licensor shall be under the terms and conditions of this License, without any additional terms or conditions. Notwithstanding the above, nothing herein shall supersede or modify the terms of any separate license agreement you may have executed with Licensor regarding such Contributions.\n\
\n\
6. Trademarks.\n\
\n\
This License does not grant permission to use the trade names, trademarks, service marks, or product names of the Licensor, except as required for reasonable and customary use in describing the origin of the Work and reproducing the content of the NOTICE file.\n\
\n\
7. Disclaimer of Warranty.\n\
\n\
Unless required by applicable law or agreed to in writing, Licensor provides the Work (and each Contributor provides its Contributions) on an 'AS IS' BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied, including, without limitation, any warranties or conditions of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A PARTICULAR PURPOSE. You are solely responsible for determining the appropriateness of using or redistributing the Work and assume any risks associated with Your exercise of permissions under this License.\n\
\n\
8. Limitation of Liability.\n\
\n\
In no event and under no legal theory, whether in tort (including negligence), contract, or otherwise, unless required by applicable law (such as deliberate and grossly negligent acts) or agreed to in writing, shall any Contributor be liable to You for damages, including any direct, indirect, special, incidental, or consequential damages of any character arising as a result of this License or out of the use or inability to use the Work (including but not limited to damages for loss of goodwill, work stoppage, computer failure or malfunction, or any and all other commercial damages or losses), even if such Contributor has been advised of the possibility of such damages.\n\
\n\
9. Accepting Warranty or Additional Liability.\n\
\n\
While redistributing the Work or Derivative Works thereof, You may choose to offer, and charge a fee for, acceptance of support, warranty, indemnity, or other liability obligations and/or rights consistent with this License. However, in accepting such obligations, You may act only on Your own behalf and on Your sole responsibility, not on behalf of any other Contributor, and only if You agree to indemnify, defend, and hold each Contributor harmless for any liability incurred by, or claims asserted against, such Contributor by reason of your accepting any such warranty or additional liability.\n\
\n\
END OF TERMS AND CONDITIONS\n\
";
    break;
  case taMisc::RESEARCH:
    txt = "NON-PROFIT RESEARCH LICENSE AGREEMENT\n\
This license agreement allows you to use the source code for non-profit, internal research and development. This means you are studying the code and using it to create additional code for study, and that you are not:\n\
* deploying the software for use by a corporation, business or organization (commercial or non-commercial)\n\
* using it to provide a service to a customer\n\
* creating a product that will be used externally or internally, unless it is a product that will exist solely to be studied or compared with other code For-profit companies may not use this source code. If you work for a for-profit company, you may only use this software as an individual, for your personal use.\n\
\n\
This license agreement also allows you to create and own derivative products, and to re- distribute the original source code and the derivative products, as long as they are distributed under the same conditions as specified in this agreement. Derivative products may be distributed commercially if you obtain a separate commercial license with " + eff_org + ".\n\
\n\
As a condition of using this source code, you agree not to assert any patents or copyrights against " + eff_org + " or any of the OWNER's licensees for use of derivative products. You must also include attribution to the authors in any publication that results from the use of this code. This license includes other conditions that should be read carefully.\n\
\n\
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION\n\
\n\
This Research License Agreement (the 'Agreement') is between YOU and " + ownr + " (OWNER)\n\
\n\
1. DEFINITIONS\n\
\n\
'Software' means all or any portion of the human-readable source code files of the software programs including without limitation, associated flow charts, algorithms, comments and other written instructions and technical documentation, and all corrections, updates, and new versions incorporated into such programs.\n\
\n\
'Derivative Work' means a work based upon the Software, such as a revision, modification, translation, abridgement, condensation, expansion, collection, compilation, or any other form in which the Software may be recast, transformed, adapted, or distributed as a part of a larger work and which, if prepared without proper authorization would constitute a copyright infringement. If identifiable sections of that work are not derived from the Software, and can be reasonably considered independent and separate works in themselves, then they are not considered Derivative Work.\n\
\n\
'Internal Research and Development' means testing, measuring, assessing, evaluating the software and/or Derivative Work by You which is not for any commercial purpose.\n\
\n\
'You' or 'Your' means an individual or a legal entity exercising rights under this License. For legal entities, 'You' or 'Your' includes any non-profit entity which controls, is controlled by, or is under common control with, You, where 'control' means (a) the power, direct or indirect, to cause the direction or management of such entity, whether by contract or otherwise, or (b) ownership of fifty percent (50%) or more of the outstanding shares or beneficial ownership of such entity.\n\
\n\
2. GRANT OF LICENSE\n\
\n\
OWNER grants, and You accept, a personal, nonexclusive, nontransferable license:\n\
\n\
a)      to use Software, at no charge, in accordance with the terms herein, solely for Internal Research and Development; and\n\
b) to develop Derivative Works that may be used solely for Internal Research and, Development; and\n\
c)      to copy, distribute and sublicense Software and Derivative Works solely in accordance with the terms herein. Any Software or Derivative Works distributed shall be pursuant to a license agreement that contains all of the terms herein; and shall contain prominent notices stating how the Software, Derivative Works, or documentation were changed, the author and date of any such change and\n\
d) You acknowledge that the Software is a valuable, proprietary asset of OWNER. You shall not market or sell the Software or Derivative Works.\n\
\n\
3. LICENSE EXCLUSIONS\n\
\n\
a) EXCEPT AS EXPRESSLY PROVIDED HEREIN, YOU SHALL MAKE NO OTHER USE OF THE SOFTWARE.\n\
b) You must obtain permission from OWNER before receiving payment for distribution of the Software or Derivative Works.\n\
c) You shall not allege or enjoin infringement or misappropriation by OWNER in any Derivative Works, or by any third party obtaining Derivative Works, prepared by OWNER and under license from OWNER.\n\
\n\
4.      TITLE AND PROTECTION OF SOFTWARE\n\
\n\
a) OWNER retains all title, right and interest to the Software.\n\
b) Except for the Software, You retain all title, right and interest to the Derivative Works, subject to the terms of this Agreement.\n\
\n\
5. ATTRIBUTION\n\
a) You agree to retain and reproduce in all copies of Software the copyright and other proprietary notices and disclaimers as they appear in the Software, and keep intact all notices in the Software that refer to this License.\n\
b) You agree to provide attribution to the authors of this Software in any article based on research performed using Software or Derivative Works or with any distribution of Software or Derivative works.\n\
\n\
6. NO REPRESENTATIONS\n\
a) OWNER does not represent that the Software is error-free.\n\
b) OWNER DISCLAIMS ALL OTHER REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
7. DEFAULT\n\
\n\
If You fail to perform any of its obligations under this Agreement, OWNER, in addition to any other rights available to it under law or equity, may terminate this Agreement and the licenses granted hereunder by written notice to You. Unless otherwise provided in this Agreement, remedies shall be cumulative and there shall be no obligation to exercise a particular remedy.\n\
\n\
8. TERMINATION\n\
\n\
a) In the event that this Agreement is terminated, any sublicenses granted or Derivative Works distributed by Licensee shall remain in full force and effect.\n\
b) Within thirty (30) days of termination, You shall return to OWNER or certify in writing to OWNER that all copies or partial copies of Software in Your possession or control have been destroyed.\n\
c) In addition to this section, the sections entitled 'Title and Protection of Software 'Representations' 'Limitation of Liability' shall survive termination of this Agreement.\n\
\n\
9. GENERAL\n\
a) Except for fulfilling requirements of the paragraph entitled 'Attribution,' You may not use OWNER's name in any advertising, public relations or media release without prior written consent.\n\
b) This Agreement shall be governed by the laws of the OWNER's locale. Venue for any action or proceeding shall be the OWNER's locale. This Agreement constitutes the entire agreement between the parties and may only be modified by a written instrument signed by each parties authorized officers.\n\
";
    break;
  case taMisc::CUSTOM_LIC:
    break;
  }
  return cpyright + txt;
}



//////////////////////////
//  taProject           //
//////////////////////////

void taProject::Initialize() {
  m_dirty = false;
  m_no_save = false;
  viewers.SetBaseType(&TA_TopLevelViewer);
  viewers_tmp.SetBaseType(&TA_TopLevelViewer);
}

void taProject::Destroy() {
  CutLinks();
}

void taProject::InitLinks() {
  //note: this routine is private, so subclasses must use _impl or _post
  inherited::InitLinks();
  InitLinks_impl();
  InitLinks_post();
}

void taProject::InitLinks_impl() {
  taBase::Own(version, this);
  taBase::Own(license, this);
  taBase::Own(wiki_url, this);
  taBase::Own(templates, this);
  taBase::Own(docs, this);
  taBase::Own(wizards, this);
  taBase::Own(edits, this);
  taBase::Own(data, this);
  taBase::Own(data_proc, this);
  taBase::Own(programs, this);
  taBase::Own(viewers, this);
  taBase::Own(viewers_tmp, this);
  taBase::Own(undo_mgr, this);

  // note: any derived programs should install additional guys..
  // put in NO_CLIP to suppress clip ops, since we don't want any for these guys
  FindMakeNewDataProc(&TA_taDataProc, "data_base")->SetUserData("NO_CLIP", true);
  FindMakeNewDataProc(&TA_taDataAnal, "data_anal")->SetUserData("NO_CLIP", true);
  FindMakeNewDataProc(&TA_taDataGen, "data_gen")->SetUserData("NO_CLIP", true);
  FindMakeNewDataProc(&TA_taImageProc, "image_proc")->SetUserData("NO_CLIP", true);
  // not actually useful to have these guys visible..  no user-accessible matrix objs
  // if in datatable, it should be accessible in above
//   FindMakeNewDataProc(&TA_taMath_float, "math_float")->SetUserData("NO_CLIP", true);
//   FindMakeNewDataProc(&TA_taMath_double, "math_double")->SetUserData("NO_CLIP", true);
}

void taProject::InitLinks_post() {
  if (!taMisc::is_loading) {
    DoView();
  }
}

void taProject::CutLinks() {
  CutLinks_impl();
  inherited::CutLinks();
}

void taProject::CutLinks_impl() {
  viewers_tmp.CutLinks();
  viewers.CutLinks();
  programs.CutLinks();
  data_proc.CutLinks();
  data.CutLinks();
  edits.CutLinks();
  wizards.CutLinks();
  docs.CutLinks();
  templates.CutLinks();
}

void taProject::Copy_(const taProject& cp) {
  // delete things first, to avoid dangling references
  programs.Reset();
  viewers_tmp.Reset();
  viewers.Reset();
  data.Reset();

  edits.Reset();

  tags = cp.tags;
  templates = cp.templates;
  docs = cp.docs;
  wizards = cp.wizards;
  edits = cp.edits;
  data = cp.data;
  data_proc = cp.data_proc;
  viewers = cp.viewers;         // todo: open windows here etc
  programs = cp.programs;
  // NOTE: once a derived project has all the relevant stuff copied, it needs to call this:
  // UpdatePointers_NewPar(&cp, this); // update pointers within entire project..
  setDirty(true);
}

void taProject::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}

taBase* taProject::FindMakeNewDataProc(TypeDef* typ, const String& nm) {
  taBase* rval = data_proc.FindType(typ);
  if(rval) return rval;
  rval = data_proc.NewEl(1, typ);
  rval->SetName(nm);
  rval->DataChanged(DCR_ITEM_UPDATED);
  return rval;
}

SelectEdit* taProject::FindMakeSelectEdit(const String& nm) {
  SelectEdit* rval = edits.FindName(nm);
  if(rval) return rval;
  rval = (SelectEdit*)edits.New(1);
  rval->SetName(nm);
  rval->DataChanged(DCR_ITEM_UPDATED);
  return rval;
}

taDoc* taProject::FindMakeDoc(const String& nm, const String& wiki_nm, const String& web_url) {
  taDoc* rval = docs.FindName(nm);
  if(!rval) {
    rval = (taDoc*)docs.New(1);
    rval->SetName(nm);
  }
  if(web_url.nonempty()) {
    if(wiki_nm.nonempty()) {
      rval->wiki = wiki_nm;
      rval->url = web_url;
    }
    else {
      rval->SetURL(web_url);
    }
  }
  rval->UpdateAfterEdit();
  return rval;
}

MainWindowViewer* taProject::GetDefaultProjectBrowser() {
//NOTE: doesn't really work properly in 2x2
  // try official default first
//  MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.DefaultEl());
//  if (vwr) return vwr;

//TODO: this is not really that good, becaus
  MainWindowViewer* vwr = NULL;
  // iterate to find 1st Browser -- will actually be 2nd item in 2x2
  for (int i = 0; i < viewers.size; ++i) {
    vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    //if (vwr && (vwr->GetName() == "DefaultProjectBrowser")) return vwr;
    if (vwr && vwr->isProjBrowser()) return vwr;
  }
  return NULL;
}

MainWindowViewer* taProject::GetDefaultProjectViewer() {
// get the default T3 guy
  MainWindowViewer* vwr = NULL;
  // iterate to find 1st Viewer -- will actually be 1st item in 2x2
  for (int i = 0; i < viewers.size; ++i) {
    vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    //if (vwr && (vwr->GetName() == "DefaultProjectBrowser")) return vwr;
    if (vwr && vwr->isProjViewer()) return vwr;
  }
  return NULL;
}

void taProject::Dump_Load_post() {
  inherited::Dump_Load_post();
  if(taMisc::is_undo_loading) return; // none of this.
  OpenProjectLog();
  DoView();
  setDirty(false);              // nobody should start off dirty!
  if(!cssMisc::init_interactive) {
    bool startup_run = programs.RunStartupProgs();      // run startups now..
    if(!taMisc::gui_active && startup_run) taiMC_->Quit();
  }
}

void taProject::DoView() {
  if (!taMisc::gui_active || taMisc::is_undo_loading) return;
  MainWindowViewer* vwr = AssertDefaultProjectBrowser(true);
#ifdef TA_OS_WIN
  taMisc::ProcessEvents(); // needed for Windows
#endif
  // note: we want a doc to be the default item, if possible
  docs.AutoEdit();
  wizards.AutoEdit();
  edits.AutoEdit();
  // this is very hacky... select the 2nd tab, which will
  // be the first auto guy if there were any
  taiMiscCore::ProcessEvents();
  vwr->SelectPanelTabNo(1);
}

MainWindowViewer* taProject::AssertDefaultProjectBrowser(bool auto_open) {
  MainWindowViewer* vwr = NULL;
  // get the default one, if there is one unopened
  vwr = GetDefaultProjectBrowser();
  if (!vwr) {
    vwr = MakeProjectBrowser_impl();
  }
  else {
    vwr->SetData(this);
  }
  if (auto_open) {
    OpenViewers();
  }
  return vwr;
}

MainWindowViewer* taProject::MakeProjectBrowser_impl() {
  MainWindowViewer* vwr =  MainWindowViewer::NewProjectBrowser(this); // added to viewers
  return vwr;
}

MainWindowViewer* taProject::NewProjectBrowser() {
  MainWindowViewer* vwr = NULL;
  // get the default one, if there is one unopened
  vwr = GetDefaultProjectBrowser();
  if (!vwr || vwr->isMapped()) {
    vwr = MakeProjectBrowser_impl();
  }
  return vwr;
}

void taProject::OpenNewProjectBrowser(String viewer_name) {
  MainWindowViewer* vwr =  MakeProjectBrowser_impl();
  if (viewer_name != "(default name)") {
    vwr->SetName(viewer_name);
    vwr->DataChanged(DCR_ITEM_UPDATED);
  }
  OpenViewers(); // opens both 2x2 if we made those
}

void taProject::OpenNewProjectViewer(String viewer_name) {
  MainWindowViewer* vwr =  MainWindowViewer::NewProjectViewer(this); // added to viewers
  if (viewer_name != "(default name)") {
    vwr->SetName(viewer_name);
    vwr->DataChanged(DCR_ITEM_UPDATED);
  }
  vwr->ViewWindow();
}

void taProject::OpenViewers() {
  for (int i = 0; i < viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (!vwr) continue;
    //TODO: add a auto_open flag to viewers, but allow prol UserData soln
    if (vwr->GetUserDataDef("auto_open", true).toBool())
      vwr->ViewWindow(); // noop if already open
  }
}

void taProject::RefreshAllViews() {
  if(!taMisc::gui_active) return;
  for (int i = 0; i < viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
    imwv->viewRefresh();
  }
}

void taProject::UpdateUi() {
  if(!taMisc::gui_active) return;
  for (int i = 0; i < viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
    imwv->UpdateUi();
  }
}

void taProject::SelectT3ViewTabNo(int tab_idx) {
  MainWindowViewer* proj_view = GetDefaultProjectViewer();
  if(!proj_view || !proj_view->SelectT3ViewTabNo(tab_idx)) {
    taMisc::Warning("could not activate 3D View Tab number:", String(tab_idx));
  }
}

void taProject::SelectT3ViewTabName(const String& tab_nm) {
  MainWindowViewer* proj_view = GetDefaultProjectViewer();
  if(!proj_view || !proj_view->SelectT3ViewTabName(tab_nm)) {
    taMisc::Warning("could not activate 3D View Tab named:", tab_nm);
  }
}

DataTable* taProject::GetNewInputDataTable(const String& nw_nm, bool msg) {
  DataTable_Group* dgp = (DataTable_Group*)data.FindMakeGpName("InputData");
  DataTable* rval = NULL;
  if(!nw_nm.empty()) {
    rval = dgp->FindName(nw_nm);
    if(rval) return rval;
  }
  rval = dgp->NewEl(1, &TA_DataTable);
  if(!nw_nm.empty()) {
    rval->name = nw_nm;
    rval->DataChanged(DCR_ITEM_UPDATED);
  }
  if(msg)
    taMisc::Info("Note: created new data table named:", rval->name, "in .data.InputData");
  return rval;
}

DataTable* taProject::GetNewOutputDataTable(const String& nw_nm, bool msg) {
  DataTable_Group* dgp = (DataTable_Group*)data.FindMakeGpName("OutputData");
  DataTable* rval = NULL;
  if(!nw_nm.empty()) {
    rval = dgp->FindName(nw_nm);
    if(rval) return rval;
  }
  rval = dgp->NewEl(1, &TA_DataTable);
  if(!nw_nm.empty()) {
    rval->name = nw_nm;
    rval->DataChanged(DCR_ITEM_UPDATED);
  }
  if(msg)
    taMisc::Info("Note: created new data table named:", rval->name, "in .data.OutputData");
  return rval;
}

DataTable* taProject::GetNewAnalysisDataTable(const String& nw_nm, bool msg) {
  DataTable_Group* dgp = (DataTable_Group*)data.FindMakeGpName("AnalysisData");
  DataTable* rval = NULL;
  if(!nw_nm.empty()) {
    rval = dgp->FindName(nw_nm);
    if(rval) return rval;
  }
  rval = dgp->NewEl(1, &TA_DataTable);
  if(!nw_nm.empty()) {
    rval->name = nw_nm;
    rval->DataChanged(DCR_ITEM_UPDATED);
  }
  if(msg)
    taMisc::Info("Note: created new data table named:", rval->name, "in .data.AnalysisData");
  return rval;
}

bool taProject::SetFileName(const String& val) {
  if (GetFileName() == val) return true;
  inherited::SetFileName(val);
  MainWindowViewer* vwr = GetDefaultProjectBrowser();
  if(vwr) {
    vwr->SetWinName();
  }
  // note: too dangerous to save root, since we are still saving project...
  // BUT changes should get saved when we close the filer anyway
  if(taMisc::gui_active) {
    if(!file_name.contains("proj_templates")) { // exclude new from template guys
      tabMisc::root->AddRecentFile(file_name, true); // use file_name instead of val b/c it is cannonicalized!
    }
  }
  return true;
}

int taProject::Save_strm(ostream& strm, taBase* par, int indent) {
  taMisc::save_use_name_paths = true; // project is one guy that DOES use name paths!
  int rval = GetTypeDef()->Dump_Save(strm, (void*)this, par, indent);
  setDirty(false);
  taMisc::save_use_name_paths = false; // default is off, so restore to default for everything else
  return rval;
}

int taProject::Save() {
  String fname = GetFileName(); // empty if 1st or not supported
  if(fname.contains("_recover")) {
    int chs = taMisc::Choice("This appears to be a recover file that was saved during a previous crash -- you may not want to save to this file name", "Save to this _recover file", "Let me choose a new name", "Save to non-_recover version of this file");
    if(chs == 1) fname = "";    // this will prompt for name
    else if(chs == 2) {
      fname = fname.before("_recover") + fname.from(".",-1);
    }
  }
  if(fname.contains("_autosave")) {
    int chs = taMisc::Choice("This appears to be an auto-save file -- you may not want to save to this file name", "Save to this _autosave file", "Let me choose a new name", "Save to non-_autosave version of this file");
    if(chs == 1) fname = "";    // this will prompt for name
    else if(chs == 2) {
      fname = fname.before("_autosave") + fname.from(".",-1);
    }
  }
  return SaveAs(fname);
}

int taProject::SaveAs(const String& fname) {
  int rval = false;
  taFiler* flr = GetSaveFiler(fname, _nilString, -1, _nilString);
  if (flr->ostrm) {
    QFileInfo fi(flr->FileName()); // set to current working dir
    QDir::setCurrent(fi.absolutePath());
    Save_strm(*(flr->ostrm));
    flr->Close();
    rval = true;
    // remove autosave and recover files after successful save of non-recover version
    String fnm = flr->FileName();
    if(!fnm.contains("_recover") && !fnm.contains("_autosave")) {
      CleanFiles();
    }
    OpenProjectLog();
  }
  taRefN::unRefDone(flr);
  DataChanged(DCR_ITEM_UPDATED_ND);
  return rval;
}

String taProject::GetProjTemplatePath(ProjLibs library) {
  if(library == SEARCH_LIBS) {
    taMisc::Error("Cannot do SEARCH_LIBS for saving -- program saved in local directory!");
    return "";
  }
  String path = "./";
  if(library == USER_LIB)
    path = taMisc::proj_template_paths.GetVal("UserLib").toString();
  else if(library == SYSTEM_LIB) {
    if(taMisc::in_dev_exe) {
      String top_lev_path = taMisc::GetDirFmPath(taMisc::exe_path, 1); // go up 1
      path = top_lev_path + "/proj_templates";
    }
    else {
      path = taMisc::proj_template_paths.GetVal("SystemLib").toString();
    }
  }
  else if(library == WEB_LIB)
    path = taMisc::proj_template_paths.GetVal("WebLib").toString();
  if(library != WEB_LIB) {
    QFileInfo qfi(path);
    if(!qfi.isDir()) {
      QDir qd;
      qd.mkpath(path);          // attempt to make it..
      taMisc::Warning("Note: did mkdir for project template library directory:", path);
    }
  }
  return path;
}

void taProject::SaveAsTemplate(const String& template_name, const String& desc,
                          const String& tags, ProjLibs library) {
  String path = GetProjTemplatePath(library);
  String fname = path + "/" + template_name + ".proj";
  QFileInfo qfi(fname);
  if(qfi.isFile()) {
    int chs = taMisc::Choice("Project template file: " + fname + " already exists: Overwrite?",
                             "Ok", "Cancel");
    if(chs == 1) return;
  }
  SaveAs(fname);

  String infofnm = fname.before(".proj",-1) + ".tmplt"; // template info
  fstream strm;
  strm.open(infofnm, ios::out);
  if(strm.bad() || strm.eof()) {
    taMisc::Error("Project::SaveAsTemplate: could not open template info file name for saving:",
                  infofnm);
    strm.close();
    return;
  }
  strm << "name=" + template_name << endl;
  strm << "tags=" + tags << endl;
  strm << "desc=" + desc << endl;
  strm.close();

  Project_Group::proj_templates.FindProjects();
}

int taProject::Load(const String& fname, taBase** loaded_obj_ptr) {
  TestError(true, "Load", "Cannot load a new project file on top of an existing project -- must load an entirely new project");
  return 0;
}

void taProject::OpenProjectLog() {
  if(!taMisc::gui_active) return; //  only for gui..
  if(file_name.empty()) return;
  String log_fn = file_name;
  if(log_fn.contains(".proj"))
    log_fn = log_fn.before(".proj",-1);
  log_fn += ".plog";
  taMisc::SetLogFile(log_fn);
}

bool taProject::CleanFiles() {
  bool got_one = false;
  String fnm = file_name.before(".proj");
  String autosave = fnm + "_autosave.proj";
  got_one |= QFile::remove(autosave.chars());
  tabMisc::root->recent_files.RemoveEl(autosave);
  for(int i=0;i<100;i++) {
    String recover = fnm + "_recover" + String(i) + ".proj";
    got_one |= QFile::remove(recover.chars());
    tabMisc::root->recent_files.RemoveEl(recover);

    String console = fnm + "_console" + String(i) + ".txt";
    QFile::remove(console.chars());

    recover = fnm + "_autosave_recover" + String(i) + ".proj";
    got_one |= QFile::remove(recover.chars());
    tabMisc::root->recent_files.RemoveEl(recover);

    console = fnm + "_autosave_console" + String(i) + ".txt";
    QFile::remove(console.chars());
  }
  if(got_one && taMisc::dmem_proc == 0) {
    tabMisc::root->Save();      // save with updated files list
  }
  return got_one;
}

void taProject::ViewLicense() {
  license.ViewLicense();
}

void taProject::ViewProjLog_Editor() {
  String fnm = file_name.before(".proj");
  fnm += ".plog";
  taMisc::EditFile(fnm);
}

void taProject::ViewProjLog() {
  String fnm = file_name.before(".proj");
  fnm += ".plog";
  fstream fh;
  fh.open(fnm, ios::in);
  if(fh.good()) {
    view_plog.Load_str(fh);
    TypeDef* td = GetTypeDef();
    MemberDef* md = td->members.FindName("view_plog");
    taiStringDataHost* host_ = new taiStringDataHost(md, this, td, true, false, NULL, true);
    // args are: read_only, modal, parent, line_nos
    host_->Constr("Project Log for Project: " + name);
    host_->Edit(false);
  }
}

int taProject::SaveNoteChanges() {
  UpdateChangeLog();
  return Save();
}

int taProject::SaveAsNoteChanges(const String& fname) {
  UpdateChangeLog();
  return SaveAs(fname);
}

void taProject::setDirty(bool value) {
  // Set the window state to modified/unmodified.  Only attempt if the dirty
  // flag has actually changed, since this may be expensive and called often.
  if (value != m_dirty) {
    if (MainWindowViewer *browser = GetDefaultProjectBrowser()) {
      if (QWidget *widget = browser->widget()) {
        widget->setWindowModified(value);
      }
    }
  }

  // note: inherited only forwards 'dirty' up the chain, not '!dirty'
  inherited::setDirty(value);
  m_dirty = value;
  if (!value) m_no_save = false;
}


void taProject::UpdateChangeLog() {
#ifdef TA_GUI
  version.step++;               // increment the step always
  TypeDef* td = GetTypeDef();
  MemberDef* md = td->members.FindName("last_change_desc");
  taiStringDataHost* dlg = new taiStringDataHost(md, this, td, false); // false = not read only
  dlg->Constr("Please enter a detailed description of the changes made to the project since it was last saved -- this will be recorded in a docs object called ChangeLog.  You can use self-contained HTML formatting tags.  <b>NOTE: Cancel</b> here is <i>only</i> for the change log entry -- not for the project save!");
  if(dlg->Edit(true)) {
    time_t tmp = time(NULL);
    String tstamp = ctime(&tmp);
    tstamp = tstamp.before('\n');

    String vers = version.GetString();

    String user = taPlatform::userName();
    String host = taPlatform::hostName();
    if (host.nonempty()) user += String("@") + host;

    String cur_fname = taMisc::GetFileFmPath(file_name);
    String prv_fname = taMisc::GetFileFmPath(GetFileName());

    if(prv_fname == cur_fname) prv_fname = "";
    else prv_fname = "(was: <code>" + prv_fname + "</code>)";

    String nw_txt = "\n<li>" + tstamp
      + " version: " + vers + " user: " + user + " file_name: <code>" + cur_fname
      + "</code> " + prv_fname + "<br>\n";
    if(!last_change_desc.empty()) nw_txt += last_change_desc + "\n";

    taDoc* doc = docs.FindName("ChangeLog");
    if(!doc) {
      doc = docs.NewEl(1);
      doc->name = "ChangeLog";
      doc->text = "<html>\n<head>ChangeLog</head>\n<body>\n<h1>ChangeLog</h1>\n<ul>\n";
      doc->text += nw_txt;
      doc->text += "</ul>\n</body>\n</html>\n";
    }
    else {
      String hdr = trim(doc->text.through("<ul>"));
      String trl = trim(doc->text.after("<ul>"));
      doc->text = hdr + "\n\n" + nw_txt + trl;
    }
    doc->UpdateText();
    doc->DataChanged(DCR_ITEM_UPDATED);
  }
  delete dlg;
#endif
}

void taProject::UndoStats(bool show_list, bool show_diffs) {
  undo_mgr.ReportStats(show_list, show_diffs);
}


void taProject::ReplaceString(const String& srch, const String& repl) {
  undo_mgr.SaveUndo(this, "ReplaceString", NULL, false, this); // global save

  String proj_str;
  tabMisc::cur_undo_save_top = this; // let others know who we're saving for..
  tabMisc::cur_undo_mod_obj = this; // let others know who we're saving for..
  tabMisc::cur_undo_save_owner = this;
  ++taMisc::is_undo_saving;
  Save_String(proj_str);
  --taMisc::is_undo_saving;
  tabMisc::cur_undo_save_top = NULL;
  tabMisc::cur_undo_mod_obj = NULL;
  tabMisc::cur_undo_save_owner = NULL;

  proj_str.gsub(srch, repl);

  ++taMisc::is_undo_loading;
  Load_String(proj_str);
  taMisc::ProcessEvents();      // get any post-load things *before* turning off undo flag..
  --taMisc::is_undo_loading;

  tabMisc::DelayedFunCall_gui(this,"RefreshAllViews");
}

String taProject::GetAutoFileName(const String& suffix, const String& ftype_ext) {
  String rval;
  if(file_name.empty()) {
    rval = taMisc::user_dir + PATH_SEP;
    if(name.empty()) {
      rval += GetTypeDef()->name;
    }
    else {
      rval += name;
    }
  }
  else {
    if(file_name.contains(ftype_ext)) {
      rval = file_name.before(ftype_ext, -1);
    }
    else {
      rval = file_name;         // whatever
    }
  }
  if(rval.contains(suffix))
    rval = rval.through(suffix, -1);
  else
    rval += suffix;
  return rval;
}

void taProject::SaveRecoverFile_strm(ostream& strm) {
  taMisc::save_use_name_paths = false; // no name paths for recover files
  int rval = GetTypeDef()->Dump_Save(strm, (void*)this);
  //  setDirty(false);  // definitely not
}

void taProject::SaveRecoverFile() {
  String ftype_ext = ".proj";
  String newfm = GetAutoFileName("_recover", ftype_ext);
  int cnt = taMisc::GetUniqueFileNumber(0, newfm, ftype_ext);
  String fnm = newfm + String(cnt) + ftype_ext; // note: this is a full path!
  QFileInfo qfi(fnm);
  QFileInfo qfd(qfi.path());
  bool usr_fl = false;
  if(!qfd.isWritable()) {
    usr_fl = true;
    fnm = taMisc::user_dir + PATH_SEP + taMisc::GetFileFmPath(fnm);
  }
  taFiler* flr = GetSaveFiler(fnm, _nilString, -1, _nilString);
  bool saved = false;
  if(flr->ostrm) {
    SaveRecoverFile_strm(*flr->ostrm);
    saved = true;
  }
  if(usr_fl) {
    taMisc::DebugInfo("Error saving recover file in original location -- now saved in user directory: ", fnm);
  }
  flr->Close();
  taRefN::unRefDone(flr);
  // log filename -- get abs path
  if (saved) {
    // save root, which wasn't saved
    tabMisc::root->Save();
  }

#ifdef HAVE_QT_CONSOLE
  // now try to save console
  if(saved) {
   if(cssMisc::TopShell->console_type == taMisc::CT_GUI) {
      String cfnm = fnm;
      cfnm.gsub("_recover", "_console");
      cfnm.gsub((const char*)ftype_ext, ".txt");
      QcssConsole* qcons = QcssConsole::getInstance();
      if(qcons)
        qcons->saveContents(cfnm);
    }
  }
#endif
}

bool taProject::AutoSave(bool force) {
  if(!force) {
    if(!isDirty()) {                    // don't save until dirty
      auto_save_timer.StartTimer(true); // reset
      return false;
    }
    if(auto_save_timer.start.tot == 0) { // first timer..
      auto_save_timer.StartTimer(true); // reset
      return false;
    }
    auto_save_timer.EndTimer();
    if(auto_save_timer.s_used < (double)taMisc::auto_save_interval) {
      auto_save_timer.StartTimer(false); // don't reset!
      return false;             // not yet
    }
    // ok, times up!
  }

  String orig_fnm = file_name;

  String ftype_ext = ".proj";
  String newfnm = GetAutoFileName("_autosave", ftype_ext) + ftype_ext;
  String fnm = newfnm; // note: this is a full path!
  QFileInfo qfi(fnm);
  QFileInfo qfd(qfi.path());
  if(!qfd.isWritable()) {       // use path!
    fnm = taMisc::user_dir + PATH_SEP + taMisc::GetFileFmPath(fnm);
    taMisc::DebugInfo("Error saving auto save file in original location:", newfnm,
                      " -- now saved in user directory:", fnm);
  }
  taFiler* flr = GetSaveFiler(fnm, _nilString, -1, _nilString);
  bool saved = false;
  if(flr->ostrm) {
    taMisc::save_use_name_paths = false; // don't use name paths for autosave!
    int rval = GetTypeDef()->Dump_Save(*flr->ostrm, (void*)this);
    // note: not using Save_strm to preserve the dirty bit!
    saved = true;
  }
  flr->Close();
  taRefN::unRefDone(flr);

  auto_save_timer.StartTimer(true); // start it up for next time around..

  taMisc::DebugInfo("Saved auto save file:", fnm);

  // restore original:
  file_name = orig_fnm;
  MainWindowViewer* vwr = GetDefaultProjectBrowser();
  if(vwr) {
    vwr->SetWinName();
  }
  return true;
}

//////////////////////////
//   Project_Group      //
//////////////////////////

void Project_Group::InitLinks() {
  inherited::InitLinks();
  if(proj_templates.not_init) {
    taBase::Ref(proj_templates);
    proj_templates.FindProjects();
  }
}


int Project_Group::Load(const String& fname, taBase** loaded_obj_ptr) {
  // chg working dir to that of project -- simplifies lots of stuff immensely
  QFileInfo fi(fname);
  QDir::setCurrent(fi.absolutePath());
  int rval = inherited::Load(fname, loaded_obj_ptr);
  return rval;
}

int Project_Group::Load_strm(istream& strm, taBase* par, taBase** loaded_obj_ptr) {
  int prj_sz = leaves;
  int rval = inherited::Load_strm(strm, par, loaded_obj_ptr);
  // note: used to do Dump_Load_post here but now it is done where it should be..
  return rval;
}

ProjTemplates Project_Group::proj_templates;

taProject* Project_Group::NewFromTemplate(ProjTemplateEl* proj_type) {
  return proj_templates.NewProject(proj_type, this);
}

taProject* Project_Group::NewFromTemplateByName(const String& prog_nm) {
  return proj_templates.NewProjectFmName(prog_nm, this);
}

////////////////////////////////////
//  ProjTemplate

void ProjTemplateEl::Initialize() {
}

void ProjTemplateEl::Destroy() {
}

taProject* ProjTemplateEl::NewProject(Project_Group* new_owner) {
  // todo: need to support full URL types -- assumed to be file right now
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  taBase* pg;
//   String oldpath = QDir::currentPath();
//   taMisc::Info("oldpath", oldpath);
  new_owner->taGroup<taProject>::Load(path, &pg); // avoid setting current wd based on load
  if(pg) {
    pg->SetFileName("");        // nuke association with template file!
  }
//   QDir::setCurrent(oldpath);
  return (taProject*)pg;
}

bool ProjTemplateEl::LoadProject(taProject* proj) {
  // todo: need to support full URL types -- assumed to be file right now
  // also this barfs and is not supported!
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  proj->Load(path);
  proj->SetFileName("");
  return true;
}

bool ProjTemplateEl::ParseProjFile(const String& fnm, const String& path) {
  filename = fnm;
  URL = "file:" + path + "/" + filename;
  String infofnm = filename.before(".proj") + ".tmplt"; // template info
  String openfnm = path + "/" + infofnm;
  fstream strm;
  strm.open(openfnm, ios::in);
  if(strm.bad() || strm.eof()) {
    taMisc::Error("ProjTemplateEl::ParseProgFile: could not open file name:", openfnm);
    return false;
  }
  bool rval = false;
  int c = taMisc::read_till_eol(strm);
  while((c != EOF) && !strm.eof() && !strm.bad()) {
    if(taMisc::LexBuf.contains("name=")) {
      name = taMisc::LexBuf.after("name=");
      name.gsub("\"", "");
      if(name.lastchar() == ';') name = name.before(';');
    }
    if(taMisc::LexBuf.contains("tags=")) {
      tags = taMisc::LexBuf.after("tags=");
      tags.gsub("\"", "");
      if(tags.lastchar() == ';') tags = tags.before(';');
      ParseTags();
    }
    if(taMisc::LexBuf.contains("desc=")) {
      desc = taMisc::LexBuf.after("desc=");
      desc.gsub("\"", "");
      if(desc.lastchar() == ';') desc = desc.before(';');
      rval = true;
      break;
    }
    c = taMisc::read_till_eol(strm);
  }
  strm.close();
  // todo: should use QUrlInfo instead -- need QtNetwork module access!
  QFileInfo urlinfo(openfnm);
  QDateTime mod = urlinfo.lastModified();
  date = mod.toString(Qt::ISODate);
  return rval;
}

void ProjTemplateEl::ParseTags() {
  tags_array.Reset();
  if(tags.empty())
    return;
  String tmp = tags;
  while(tmp.contains(',')) {
    String tag = tmp.before(',');
    tag.gsub(" ","");           // nuke spaces
    tags_array.Add(tag);
    tmp = tmp.after(',');
  }
  if(!tmp.empty()) {
    tmp.gsub(" ","");           // nuke spaces
    tags_array.Add(tmp);
  }
}

void ProjTemplateEl_List::Initialize() {
}

void ProjTemplates::Initialize() {
  not_init = true;
}

void ProjTemplates::FindProjects() {
  Reset();                      // clear existing
  for(int pi=0; pi< taMisc::proj_template_paths.size; pi++) {
    NameVar pathvar = taMisc::proj_template_paths[pi];
    String path = pathvar.value.toString();
    String lib_name = pathvar.name;
    QDir dir(path);
    QStringList files = dir.entryList();
    for(int i=0;i<files.size();i++) {
      String fl = files[i];
      if(!fl.contains(".proj")) continue;
      ProjTemplateEl* pe = new ProjTemplateEl;
      pe->lib_name = lib_name;
      if(pe->ParseProjFile(fl, path))
        Add(pe);
      else
        delete pe;
    }
  }
  not_init = false;
}

taProject* ProjTemplates::NewProject(ProjTemplateEl* proj_type, Project_Group* new_owner) {
  if(proj_type == NULL) return NULL;
  return proj_type->NewProject(new_owner);
}

taProject* ProjTemplates::NewProjectFmName(const String& proj_nm, Project_Group* new_owner) {
  return NewProject(FindName(proj_nm), new_owner);
}


//////////////////////////
//   taApplication      //
//////////////////////////

taApplication::taApplication(int & argc, char ** argv) : QApplication(argc, argv) {
}

bool taApplication::event(QEvent *event) {
  if(event->type() == QEvent::FileOpen) {
    String fname = static_cast<QFileOpenEvent*>(event)->file();
    taRootBase::instance()->projects.Load(fname);
    return true;
  }
  return QApplication::event(event);
}

//////////////////////////
//   taRootBaseAdapter  //
//////////////////////////

void taRootBaseAdapter::Startup_ProcessArgs() {
  taRootBase::Startup_ProcessArgs();
}

void taRootBaseAdapter::Startup_RunStartupScript() {
  taRootBase::Startup_RunStartupScript();
}

void taRootBaseAdapter::ConsoleNewStdin(int n_lines) {
  taRootBase::instance()->ConsoleNewStdin(n_lines);
}


#ifdef DMEM_COMPILE
void taRootBaseAdapter::DMem_SubEventLoop() {
  taRootBase::DMem_SubEventLoop();
}
#endif // DMEM_COMPILE

//////////////////////////
//   taRoot             //
//////////////////////////

int taRootBase::milestone;
TypeDef* taRootBase::root_type;
taMisc::ConsoleType taRootBase::console_type;
int taRootBase::console_options;
ContextFlag taRootBase::in_init;

// note: not static class to avoid need qpointer in header
QPointer<taRootBaseAdapter> root_adapter;

taRootBase* taRootBase::instance() {
  if (!tabMisc::root) {
    taRootBase* rb = (taRootBase*)root_type->GetInstance();
    if (!rb) {
      taMisc::Error("Startup_MakeRoot: Error -- no instance of root type!");
      return NULL;
    }
    tabMisc::root = (taRootBase*)rb->MakeToken();
    tabMisc::root->SetName("root");
    taBase::Ref(tabMisc::root);
    tabMisc::root->InitLinks();
  }
  return tabMisc::root;
}

void taRootBase::Initialize() {
  version = taMisc::version;
  projects.SetName("projects");
  plugin_deps.SetBaseType(&TA_taPluginDep);
  console_type = taMisc::console_type;
  console_options = taMisc::console_options;
#ifdef TA_OS_LINUX
  fpe_enable = FPE_0; //GetFPEFlags(fegetexcept());
#endif
}

void taRootBase::Destroy() {
  CutLinks(); // note: will prob already have been done
  bool we_are_root = (tabMisc::root == this); // 'true' if we are the one and only root app object
  if (we_are_root) {
    tabMisc::root = NULL; //TODO: maybe for cleanness we should do a SetPointer thingy, since we set it that way...
#ifdef DMEM_COMPILE
    if(taMisc::dmem_nprocs > 1) {
      taMisc::RecordScript(".Quit();\n");
    }
#endif
  }
}

void taRootBase::InitLinks() {
  inherited::InitLinks();
  version = taMisc::version;
  taBase::Own(templates, this);
  taBase::Own(docs, this);
  taBase::Own(wizards, this);
  taBase::Own(projects, this);
  taBase::Own(viewers, this);
  taBase::Own(viewers_tmp, this);
  taBase::Own(plugins, this);
  taBase::Own(plugin_state, this);
  taBase::Own(plugin_deps, this);
  taBase::Own(mime_factories, this);
  taBase::Own(colorspecs, this);
  // create colorspecs even if nogui, since they are referenced in projects
  colorspecs.SetDefaultColor();
  taBase::Own(objs, this);
  taBase::Own(recent_files, this);
  taBase::Own(recent_paths, this);
  taBase::Own(sidebar_paths, this);
  taiMimeFactory_List::setInstance(&mime_factories);
  AddTemplates(); // note: ok that this will be called here, before subclass has finished its own
  AddDocs(); // note: ok that this will be called here, before subclass has finished its own
}

void taRootBase::CutLinks() {
  sidebar_paths.CutLinks();
  recent_paths.CutLinks();
  recent_files.CutLinks();
  objs.CutLinks();
  colorspecs.CutLinks();
  mime_factories.CutLinks();
  plugin_deps.CutLinks();
//TODO: we should save the plugin state!
  plugin_state.CutLinks();
  plugins.CutLinks();
  viewers_tmp.CutLinks();
  viewers.CutLinks();
  projects.CutLinks();
  wizards.CutLinks();
  templates.CutLinks();
  inherited::CutLinks();
}

#ifdef TA_OS_LINUX
int taRootBase::GetFEFlags(FPExceptFlags fpef) {
  int rval = 0;
  if (fpef & FPE_INEXACT) rval |= FE_INEXACT;
  if (fpef & FPE_DIVBYZERO) rval |= FE_DIVBYZERO;
  if (fpef & FPE_UNDERFLOW) rval |= FE_UNDERFLOW;
  if (fpef & FPE_OVERFLOW) rval |= FE_OVERFLOW;
  if (fpef & FPE_INVALID) rval |= FE_INVALID;
  return rval;
}

taRootBase::FPExceptFlags taRootBase::GetFPEFlags(int fef) {
  int rval = 0;
  if (fef & FE_INEXACT) rval |= FPE_INEXACT;
  if (fef & FE_DIVBYZERO) rval |= FPE_DIVBYZERO;
  if (fef & FE_UNDERFLOW) rval |= FPE_UNDERFLOW;
  if (fef & FE_OVERFLOW) rval |= FPE_OVERFLOW;
  if (fef & FE_INVALID) rval |= FPE_INVALID;
  return (FPExceptFlags)rval;
}
#endif

void taRootBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
#ifdef TA_OS_LINUX
  // just remove all, then set selected ones
  fedisableexcept(FE_ALL_EXCEPT);
  if (fpe_enable) {
    feenableexcept(GetFEFlags(fpe_enable));
  }
#endif
}

#ifdef GPROF                    // turn on for profiling
void taRootBase::MonControl(bool on) {
  moncontrol(on);
}
#else
#ifdef SATURN_PROF
void taRootBase::MonControl(bool on) {
  if(on) {
    startSaturn();
  }
  else {
    stopSaturn();
  }
}
#else
void taRootBase::MonControl(bool on) {
  // nop
}
#endif
#endif

void taRootBase::AddRecentFile(const String& value, bool no_save) {
  if (value.empty()) return; // oops...
// never save for dmem>0
  no_save = no_save || (taMisc::dmem_proc > 0);
  bool save = AddRecentFile_impl(value);
  QFileInfo fi(value);
  String path = fi.path();
  if (AddRecentPath_impl(path))
    save = true;
  if (save && !no_save)
    Save();
}

bool taRootBase::AddRecentFile_impl(const String& value) {
  // first, see if already there, if so, then just move it to the top
  if (taMisc::num_recent_files <= 0) {
    if (recent_files.size > 0) {
      recent_files.Reset();
      return true;
    } else return false;
  }
  int idx = recent_files.FindEl(value);
  if (idx == 0) return false; // already at top, no need to save either!
  else if (idx > 0) {
    recent_files.MoveIdx(idx, 0);
  } else {
    // not there; if full, then nuke a guy
    if (recent_files.size >= taMisc::num_recent_files)
      recent_files.SetSize(taMisc::num_recent_files - 1);
    // insert it
    recent_files.Insert(value, 0);
  }
  return true;
}

void taRootBase::AddRecentPath(const String& value, bool no_save) {
  if (value.empty()) return; // oops...
// never save for dmem>0
  no_save = no_save || (taMisc::dmem_proc > 0);
  if (AddRecentPath_impl(value) && !no_save)
    Save();
}

bool taRootBase::AddRecentPath_impl(const String& value) {
  // first, see if already there, if so, then just move it to the top
  if (taMisc::num_recent_paths <= 0) {
    if (recent_paths.size > 0) {
      recent_paths.Reset();
      return true;
    } else return false;
  }
  int idx = recent_paths.FindEl(value);
  if (idx == 0) return false; // already at top, no need to save either!
  else if (idx > 0) {
    recent_paths.MoveIdx(idx, 0);
  } else {
    // not there; if full, then nuke a guy(s)
    if (recent_paths.size >= taMisc::num_recent_paths)
      recent_paths.SetSize(taMisc::num_recent_paths - 1);
    // insert it
    recent_paths.Insert(value, 0);
  }
  return true;
}

int taRootBase::Save() {
  if (in_init) return false; // no spurious, and also suppresses for dmem
  ++taFiler::no_save_last_fname;
  int rval = inherited::Save();
  --taFiler::no_save_last_fname;
  return rval;
}

int taRootBase::SavePluginState() {
//TODO:
// iterate the plugin_state collection, making a file for each guy
// in the user data area
  return 0;
}

int taRootBase::LoadPluginState() {
//TODO:
// iterate the plugin_state collection, loading data for each guy
// from the user data area
  return 0;
}

bool taRootBase::CheckAddPluginDep(TypeDef* td) {
  if (!td) return false;
  TypeDef* pl_td = td->plugin;
  if (!pl_td) return false;
  bool rval = false;
  for (int i = 0; i < plugins.size; ++i) {
    taPlugin* pl = plugins.FastEl(i);
    if (!pl->plugin ) continue; // not loaded
    IPlugin* ipl = pl->plugin->plugin();
    if (!ipl) continue; // not loaded
    if (ipl->GetTypeDef() == pl_td) {
      // this is the guy!
      rval = true;
      // see if already listedbool              VerifyHasPlugins()
      if (plugin_deps.FindName(pl->GetName())) break;
      // otherwise, clone a dep, and add
      taPluginDep* pl_dep = new taPluginDep;
      static_cast<taPluginBase*>(pl_dep)->Copy(*pl);
      plugin_deps.Add(pl_dep);
      break;
    }
  }

  return rval;
}

bool taRootBase::VerifyHasPlugins() {
  int miss_cnt = 0;
  for (int i = 0; i < plugin_deps.size; ++i) {
    taPluginDep* pl_dep = (taPluginDep*)plugin_deps.FastEl(i);
    taPlugin* pl = (taPlugin*)plugins.FindName(pl_dep->GetName());
    if (pl) {
      if (!pl->loaded)
        pl_dep->dep_check = taPluginDep::DC_NOT_LOADED;
      //else if...
      else continue; // ok
    } else {
      pl_dep->dep_check = taPluginDep::DC_MISSING;
    }
    ++miss_cnt;
  }
  if (miss_cnt == 0) return true;
  // highlight load issues
  plugin_deps.CheckConfig();
  String msg = "Would you like to Abort or Continue loading?";
  int chs = taMisc::Choice(msg, "Abort", "Continue");
  return (chs == 1);

}

void taRootBase::About() {
  String info;
  info += "The Emergent Toolbox (TEMT) Info\n";
  info += "This is the TEMT software package, version: ";
  info += taMisc::version;
  info += "\n\n";
  info += "WWW Page: http://grey.colorado.edu/temt\n";
  info += "\n\n";

  info += "Copyright (c) 1995-2010, Regents of the University of Colorado,\n\
 Carnegie Mellon University, Princeton University.\n\
 \n\
 TEMT is free software; you can redistribute it and/or modify\n\
 it under the terms of the GNU General Public License as published by\n\
 the Free Software Foundation; either version 2 of the License, or\n\
 (at your option) any later version.\n\
 \n\
 TEMT is distributed in the hope that it will be useful,\n\
 but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
 GNU General Public License for more details.\n\
 \n\
 Note that the taString class was derived from the GNU String class\n\
 Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and\n\
 is covered by the GNU General Public License, see ta_string.h\n";
  if(cssMisc::init_interactive)
    taMisc::Choice(info, "Ok");
  else
    taMisc::Info(info);
}

void taRootBase::AddTemplates() {
  templates.Add(Program::MakeTemplate());
}

void taRootBase::AddDocs() {
  taDoc* doc = FindMakeDoc("web_home", "", taMisc::web_home);
  doc->auto_open = true;
}

taDoc* taRootBase::FindMakeDoc(const String& nm, const String& wiki_nm, const String& web_url) {
  taDoc* rval = docs.FindName(nm);
  if(!rval) {
    rval = (taDoc*)docs.New(1);
    rval->SetName(nm);
  }
  if(web_url.nonempty()) {
    if(wiki_nm.nonempty()) {
      rval->wiki = wiki_nm;
      rval->url = web_url;
    }
    else {
      rval->SetURL(web_url);
    }
  }
  rval->UpdateAfterEdit();
  return rval;
}

taBase* taRootBase::FindGlobalObject(TypeDef* base_type,
    const String& name)
{
  for (int i = 0; i < objs.size; ++i) {
    taBase* rval = objs.FastEl(i);
    if (!rval || !rval->InheritsFrom(base_type))
      continue;
    if (name.empty() || (rval->GetName() == name))
      return rval;
  }
  return NULL;
}

void taRootBase::OpenRemoteServer(ushort port) {
  TemtServer* srv = (TemtServer*)FindGlobalObject(&TA_TemtServer);
  if (!srv) {
    srv = (TemtServer*)objs.New(1, &TA_TemtServer);
  }
  if (srv->isOpen()) return;
  srv->port = port;
  if (!srv->OpenServer()) {
    taMisc::Error("Could not open server");
  }

}

void taRootBase::CloseRemoteServer() {
  TemtServer* srv = (TemtServer*)FindGlobalObject(&TA_TemtServer);
  if (!srv) return; // if doesn't exist, don't create!
  if (!srv->isOpen()) return;
  srv->CloseServer();
}

taBase* taRootBase::GetTemplateInstance(TypeDef* typ) {
  return GetTemplateInstance_impl(typ, &templates);
}

taBase* taRootBase::GetTemplateInstance_impl(TypeDef* typ, taBase* base) {
  taBase* rval = NULL;
  TypeDef* btyp = base->GetTypeDef();
  if (btyp->name == typ->name) return base;

  // if it is a list, check its children first (vastly more likely than member pointers)
  if (btyp->InheritsFrom(&TA_taList_impl)) {
    taList_impl* lst = (taList_impl*)base;
    // check all the children
    for (int j = 0; j < lst->size; ++j) {
      taBase* chld = (taBase*)lst->FastEl_(j);
      if (!chld) continue;
      rval = GetTemplateInstance_impl(typ, chld);
      if (rval) return rval;
    }
  }

  // check all taBase* members (but NOT embedded objects) and list children
  for (int i = 0; i < btyp->members.size; ++i) {
    MemberDef* md = btyp->members.FastEl(i);
    // if base is an embedded list, then check all its children
    if (md->type->InheritsFrom(&TA_taList_impl) &&
      (md->type->ptr == 0))
    {
      taList_impl* lst = (taList_impl*)md->GetOff(base);
      rval = GetTemplateInstance_impl(typ, lst);
      if (rval) return rval;
    }

    //TODO: if we find it is needed, then also check taBase* ptrs, or ptrs to lists
  }
  return NULL;

}

void taRootBase::SaveAll() {
  FOREACH_ELEM_IN_GROUP(taProject, pr, projects) {
    pr->Save(); // does SaveAs if no filename
  }
}

void taRootBase::Options() {
#ifdef TA_GUI
  if (!taMisc::gui_active) return;
  taiEdit* ie =  TA_taMisc.ie;
  if (!ie) return;
  taMisc* inst = (taMisc*)TA_taMisc.GetInstance();
  int accepted = ie->EditDialog(inst, false, true, taiTypeBase::def_color,
                                800, 600); // r/w, modal, min width, height
  if (accepted) {
    inst->SaveConfig();
  }
#endif
}

void taRootBase::MakeWizards() {
  MakeWizards_impl();
}

void taRootBase::MakeWizards_impl() {
  // plugins
  wizards.New(1, &TA_PluginWizard, "PluginWizard");
}


/////////////////////////////////////////
//      startup code


bool taRootBase::Startup_InitDMem(int& argc, const char* argv[]) {
#ifdef DMEM_COMPILE
  taMisc::Init_DMem(argc, argv);
  milestone |= SM_MPI_INIT;
#endif
  return true;
}

bool taRootBase::Startup_InitArgs(int& argc, const char* argv[]) {
  taMisc::AddArgName("-nogui", "NoGui");
  taMisc::AddArgName("--nogui", "NoGui");
  taMisc::AddArgNameDesc("NoGui", "\
 -- Disables the GUI (graphical user interface), for running in background");

  taMisc::AddArgName("-gui", "Gui");
  taMisc::AddArgName("--gui", "Gui");
  taMisc::AddArgNameDesc("Gui", "\
 -- Enables the GUI (graphical user interface) -- it is on by default in most programs except css");

  taMisc::AddArgName("-nowin", "NoWin");
  taMisc::AddArgName("--nowin", "NoWin");
  taMisc::AddArgNameDesc("NoWin", "\
 -- does not open any windows, but does start the basic GUI infrastructure, as a way of doing offscreen rendering");

  taMisc::AddArgName("-attachwait", "AttachWait");
  taMisc::AddArgName("--attachwait", "AttachWait");
  taMisc::AddArgName("attachwait=", "AttachWait");
  taMisc::AddArgNameDesc("AttachWait", "\
 -- after startup, before event loop, enter a wait loop so that you can attach to process with gdb, and then debug it -- very useful for dmem (MPI) debugging -- when passed with a value, value is process number to wait (otherwise all wait) -- after attaching to process, do set var i = 1 to get out of loop");

  taMisc::AddArgName("-a", "AppDir");
  taMisc::AddArgName("--app_dir", "AppDir");
  taMisc::AddArgName("app_dir=", "AppDir");
  taMisc::AddArgNameDesc("AppDir", "\
 -- explicitly specifies location of the app directory (prog libs, etc.)");

  taMisc::AddArgName("-ap", "AppPluginDir");
  taMisc::AddArgName("--app_plugin_dir", "AppPluginDir");
  taMisc::AddArgName("app_plugin_dir=", "AppPluginDir");
  taMisc::AddArgNameDesc("AppPluginDir", "\
 -- explicitly specifies location of the System plugin directory");

  taMisc::AddArgName("-version", "Version");
  taMisc::AddArgName("--version", "Version");
  taMisc::AddArgNameDesc("Version", "\
 -- Prints out version and other information");

  taMisc::AddArgName("-h", "Help");
  taMisc::AddArgName("--help", "Help");
  taMisc::AddArgNameDesc("Help", "\
 -- Prints out help on startup arguments and other usage information");

  taMisc::AddArgName("-gendoc", "GenDoc");
  taMisc::AddArgName("--gendoc", "GenDoc");
  taMisc::AddArgNameDesc("GenDoc", "\
 -- generates documentation source information in XML based on type information scanned by maketa");

  taMisc::AddArgName("-p", "Project");
  taMisc::AddArgName("--proj", "Project");
  taMisc::AddArgName("proj=", "Project");
  taMisc::AddArgNameDesc("Project", "\
 <projname.proj> -- Specifies a project file to be loaded upon startup");

  taMisc::AddArgName("-f", "CssScript");
  taMisc::AddArgName("--file", "CssScript");
  taMisc::AddArgName("file=", "CssScript");
  taMisc::AddArgName("-s", "CssScript");
  taMisc::AddArgName("--script", "CssScript");
  taMisc::AddArgName("script=", "CssScript");
  taMisc::AddArgNameDesc("CssScript", "\
 <scriptname.css> -- Specifies a css script file to be loaded and executed upon startup");

  taMisc::AddArgName("-e", "CssCode");
  taMisc::AddArgName("--exec", "CssCode");
  taMisc::AddArgName("exec=", "CssCode");
  taMisc::AddArgNameDesc("CssCode", "\
 <scriptcode> Specifies css script code to be executed upon startup");

  taMisc::AddArgName("-i", "CssInteractive");
  taMisc::AddArgName("--interactive", "CssInteractive");
  taMisc::AddArgNameDesc("CssInteractive", "\
 -- Specifies that the css console should remain active after running a css script file upon startup");

  taMisc::AddArgName("-ni", "CssNonInteractive");
  taMisc::AddArgName("--non-interactive", "CssNonInteractive");
  taMisc::AddArgNameDesc("CssNonInteractive", "\
 -- Specifies that the css console should NOT be activated at all during running (e.g., if a STARTUP_RUN program is present that will run and then quit out)");

  taMisc::AddArgName("-u", "UserDir");
  taMisc::AddArgName("--user_dir", "UserDir");
  taMisc::AddArgName("user_dir=", "UserDir");
  taMisc::AddArgNameDesc("UserDir", "\
 -- explicitly specifies location of user home folder (should normally not need to override)");

  taMisc::AddArgName("-ua", "UserAppDir");
  taMisc::AddArgName("--user_app_dir", "UserAppDir");
  taMisc::AddArgName("user_app_dir=", "UserAppDir");
  taMisc::AddArgNameDesc("UserAppDir", "\
 -- explicitly specifies location of user app folder (should normally not need to override)");

  taMisc::AddArgName("-up", "UserPluginDir");
  taMisc::AddArgName("--user_plugin_dir", "UserPluginDir");
  taMisc::AddArgName("user_plugin_dir=", "UserPluginDir");
  taMisc::AddArgNameDesc("UserPluginDir", "\
 -- explicitly specifies location of user plugin folder (should normally not need to override)");

  taMisc::AddArgName("-v", "CssDebug");
  taMisc::AddArgName("--verbose", "CssDebug");
  taMisc::AddArgName("verbose=", "CssDebug");
  taMisc::AddArgNameDesc("CssDebug", "\
 -- Specifies an initial debug level for css upon startup");

  taMisc::AddArgName("-b", "CssBreakpoint");
  taMisc::AddArgName("--breakpoint", "CssBreakpoint");
  taMisc::AddArgName("breakpoint=", "CssBreakpoint");
  taMisc::AddArgNameDesc("CssBreakpoint", "\
 <line_no> -- Specifies an initial breakpoint at given line number of the startup script file");

  taMisc::AddArgName("-rct", "CssRefCountTrace");
  taMisc::AddArgName("--ref_count_trace", "CssRefCountTrace");
  taMisc::AddArgNameDesc("CssRefCountTrace", "\
 -- Specifies that css reference count tracing should be performed (debugging tool)");

  ////////////////////////////////////////////////////
  //    All the multi-threading stuff has standard default startup args

  taMisc::AddArgName("--max_cpus", "MaxCpus");
  taMisc::AddArgName("max_cpus=", "MaxCpus");
  taMisc::AddArgNameDesc("MaxCpus", "\
 -- Maximum number of cpus -- this should normally be detected automatically, but in case it isn't -- see n_threads for the actual number to use");

  taMisc::AddArgName("--n_threads", "NThreads");
  taMisc::AddArgName("n_threads=", "NThreads");
  taMisc::AddArgNameDesc("NThreads", "\
 -- Target number of threads to use in multi-threaded code -- should be <= max_cpus and it often is more efficient to use less than what is maximally available");

  taMisc::AddArgName("--thread_alloc_pct", "ThreadAllocPct");
  taMisc::AddArgName("thread_alloc_pct=", "ThreadAllocPct");
  taMisc::AddArgNameDesc("ThreadAllocPct", "\
 -- proportion (0-1) of total to process by pre-allocating a set of computations to a given thread -- the remainder of the load is allocated dynamically through a nibbling mechanism, where each thread takes a nibble_chunk at a time until the job is done.  current experience is that this should be no greater than .2, unless the load is quite large, as there is a high degree of variability in thread start times, so the automatic load balancing of nibbling is important, and it has very little additional overhead.");

  taMisc::AddArgName("--thread_nibble_chunk", "ThreadNibbleChunk");
  taMisc::AddArgName("thread_nibble_chunk=", "ThreadNibbleChunk");
  taMisc::AddArgNameDesc("ThreadNibbleChunk", "\
 -- how many units does each thread grab to process while nibbling?  Too small a value results in increased contention and inefficiency, while too large a value results in poor load balancing across processors.");

  taMisc::AddArgName("--thread_compute_thr", "ThreadComputeThr");
  taMisc::AddArgName("thread_compute_thr=", "ThreadComputeThr");
  taMisc::AddArgNameDesc("ThreadComputeThr", "\
 -- threshold value for amount of computation in a given function to actually deploy on threads, as opposed to just running it on main thread -- value is normalized (0-1) with 1 being the most computationally intensive task, and 0 being the least -- as with min_units, it may not be worth it to parallelize very lightweight computations.  See Thread_Params page on emergent wiki for relevant comparison values.");

  taMisc::AddArgName("--thread_min_units", "ThreadMinUnits");
  taMisc::AddArgName("thread_min_units=", "ThreadMinUnits");
  taMisc::AddArgNameDesc("ThreadMinUnits", "\
 -- minimum number of computational units (e.g., network units) to apply parallel threading to -- if less than this number, all will be computed on the main thread to avoid threading overhead which may be more than what is saved through parallelism, if there are only a small number of things to compute.");

  ////////////////////////////////////////////////////
  //    Server variables

  taMisc::AddArgName("--server", "Server");
  taMisc::AddArgName("-server", "Server");
  taMisc::AddArgNameDesc("Server", "\
 -- Run the app as a tcp server");

  taMisc::AddArgName("--port", "Port");
  taMisc::AddArgName("-port", "Port");
  taMisc::AddArgName("port=", "Port");
  taMisc::AddArgNameDesc("Port", "\
 -- Specifies the tcp port for server mode (def=5360");


  ////////////////////////////////////////////////////
  //    Plugin making

  taMisc::AddArgName("--no_plugins", "NoPlugins");
  taMisc::AddArgNameDesc("NoPlugins", "\
 -- do not load any plugins -- can be useful if some plugins are misbehaving -- may be better to re-make all the plugins (--make_all_plugins) to fix plugin problems though");

  taMisc::AddArgName("--make_all_plugins", "MakeAllPlugins");
  taMisc::AddArgName("--make_plugins", "MakeAllPlugins");
  taMisc::AddArgNameDesc("MakeAllPlugins", "\
 -- (re)make all the plugins located in the user AND system plugin directories -- these are typically installed with a make install from wherever original source is located, and source is installed to same plugin directory -- make will make from this installed source");

  taMisc::AddArgName("--make_all_user_plugins", "MakeAllUserPlugins");
  taMisc::AddArgNameDesc("MakeAllUserPlugins", "\
 -- (re)make all the plugins located in the user plugin directory -- these are typically installed with a make install from wherever original source is located, and source is installed to user plugin directory -- make will make from this installed source");

  taMisc::AddArgName("--make_all_system_plugins", "MakeAllSystemPlugins");
  taMisc::AddArgNameDesc("MakeAllSystemPlugins", "\
 -- (re)make all the plugins located in the system plugin directory -- these are typically installed with a make install from wherever original source is located, and source is installed to system plugin directory -- make will make from this installed source");

  taMisc::AddArgName("--clean_all_plugins", "CleanAllPlugins");
  taMisc::AddArgName("--clean_plugins", "CleanAllPlugins");
  taMisc::AddArgNameDesc("CleanAllPlugins", "\
 -- clean (remove) all the plugins (just the library files, not the source) located in the user AND system plugin directories");

  taMisc::AddArgName("--clean_all_user_plugins", "CleanAllUserPlugins");
  taMisc::AddArgNameDesc("CleanAllUserPlugins", "\
 -- clean (remove) all the plugins (just the library files, not the source) located in the user plugin directory");

  taMisc::AddArgName("--clean_all_system_plugins", "CleanAllSystemPlugins");
  taMisc::AddArgNameDesc("CleanAllSystemPlugins", "\
 -- clean (remove) all the plugins (just the library files, not the source) located in the system plugin directory");

  taMisc::AddArgName("--make_user_plugin", "MakeUserPlugin");
  taMisc::AddArgName("make_user_plugin=", "MakeUserPlugin");
  taMisc::AddArgNameDesc("MakeUserPlugin", "\
 -- (re)make specified plugin located in the user plugin directory -- these are typically installed with a make install from wherever original source is located, and source is installed to user plugin directory -- make will make from this installed source");

  taMisc::AddArgName("--make_system_plugin", "MakeSystemPlugin");
  taMisc::AddArgName("make_system_plugin=", "MakeSystemPlugin");
  taMisc::AddArgNameDesc("MakeSystemPlugin", "\
 -- (re)make specified plugin located in the system plugin directory -- these are typically installed with a make install from wherever original source is located, and source is installed to system plugin directory -- make will make from this installed source");

  taMisc::AddArgName("--list_plugins", "ListAllPlugins");
  taMisc::AddArgNameDesc("ListAllPlugins", "\
 -- list all of the plugins and their current status.");

  taMisc::AddArgName("--enable_all_plugins", "EnableAllPlugins");
  taMisc::AddArgNameDesc("EnableAllPlugins", "\
 -- mark all the available plugins as enabled -- useful for batch run environments where you cannot enable them via the gui.");

  taMisc::Init_Args(argc, argv);
  return true;
}

bool taRootBase::Startup_ProcessGuiArg(int argc, const char* argv[]) {
#ifdef TA_GUI
  taMisc::use_gui = true;
#else
  taMisc::use_gui = false;
#endif

  // process gui flag right away -- has other implications
  if(taMisc::CheckArgByName("GenDoc") || taMisc::CheckArgByName("Version")
     || taMisc::CheckArgByName("Help")) {
    taMisc::use_plugins = false;                      // no need for plugins for these..
    taMisc::use_gui = false;
    cssMisc::init_interactive = false;
  }

  if(taMisc::CheckArgByName("MakeAllUserPlugins")
     || taMisc::CheckArgByName("MakeAllSystemPlugins")
     || taMisc::CheckArgByName("MakeAllPlugins")
     || taMisc::CheckArgByName("CleanAllPlugins")
     || taMisc::CheckArgByName("CleanAllUserPlugins")
     || taMisc::CheckArgByName("CleanAllSystemPlugins")
     || taMisc::CheckArgByName("CleanAllPlugins")
     || taMisc::CheckArgByName("MakeUserPlugin")
     || taMisc::CheckArgByName("MakeSystemPlugin")) { // auto nogui by default
    taMisc::use_plugins = false;                      // don't use if making
    taMisc::use_gui = false;
    cssMisc::init_interactive = false;
  }

  if(taMisc::CheckArgByName("ListAllPlugins")
     || taMisc::CheckArgByName("EnableAllPlugins")) {
    taMisc::use_gui = false;
    cssMisc::init_interactive = false;
  }

  if(taMisc::CheckArgByName("NoPlugins"))
    taMisc::use_plugins = false;                      // don't use if making

  // need to use Init_Args and entire system because sometimes flags get munged together
  if(taMisc::CheckArgByName("NoGui"))
    taMisc::use_gui = false;
  else if(taMisc::CheckArgByName("Gui"))
    taMisc::use_gui = true;

  if(taMisc::CheckArgByName("NoWin"))
    taMisc::gui_no_win = true;
  else
    taMisc::gui_no_win = false;

#ifndef TA_GUI
  if(taMisc::use_gui) {
    taMisc::Error("Startup_InitArgs: cannot specify '-gui' switch when compiled without gui support");
    return false;
  }
#endif
  return true;
}

static CoinImageReaderCB* coin_image_reader_cb_obj = NULL;

bool taRootBase::Startup_InitApp(int& argc, const char* argv[]) {
  setlocale(LC_ALL, "");

#ifdef TA_GUI
  if(taMisc::use_gui) {
    // get optional style override
# ifdef TA_OS_WIN
    // Vista style only available on Vista+, so force down if not
    // NOTE: this may not work with Windows 7 and Qt 4.5+ -- see QtSysInfo at that time
    if ((taMisc::gui_style == taMisc::GS_WINDOWSVISTA) && (
      QSysInfo::WindowsVersion != QSysInfo::WV_VISTA))
      taMisc::gui_style = taMisc::GS_WINDOWSXP;
# endif // TA_OS_WIN
    String gstyle;
    if(taMisc::gui_style != taMisc::GS_DEFAULT) {
      gstyle = TA_taMisc.GetEnumString("GuiStyle", taMisc::gui_style).after("GS_").downcase();
    }
// quasi-temp hack because Mac style on Mac breaks layouts in 4.3.1
// # if defined(TA_OS_MAC) && (QT_VERSION >= 0x040300) // && (QT_VERSION < 0x040400)
//     if (gstyle.empty()) gstyle = "windows"; // this looks nice and works
// # endif
# ifdef TA_USE_INVENTOR
    new taApplication(argc, (char**)argv); // accessed as qApp
    SIM::Coin3D::Quarter::Quarter::init();
//     SoQt::init(argc, (char**)argv, cssMisc::prompt.chars()); // creates a special Coin QApplication instance
    milestone |= (SM_QAPP_OBJ | SM_SOQT_INIT);
# else
    new taApplication(argc, (char**)argv); // accessed as qApp
    milestone |= SM_QAPP_OBJ;
# endif // TA_USE_INVENTOR
    if(gstyle.nonempty()) {
      QApplication::setStyle(gstyle.toQString());
    }
    QString app_ico_nm = ":/images/" + taMisc::app_name + "_32x32.png";
    QPixmap app_ico(app_ico_nm);
    QApplication::setWindowIcon(app_ico);

    // test for various GL compatibilities now, before we get bitten later!
    if(!QGLFormat::hasOpenGL()) {
      cerr << "This display does NOT have OpenGL support, which is required for 3d displays!\n"
           << "Please read the emergent manual for required 3D graphics driver information.\n"
           << "If you open a project with a 3D display, or create one, the program will likely crash!" << endl;
    }

#ifdef TA_USE_INVENTOR
# if COIN_MAJOR_VERSION >= 3
    // this installs the callback to eliminate dependency on simage
    coin_image_reader_cb_obj = new CoinImageReaderCB;
# endif
#endif
  } else
#endif // TA_GUI

  {
    new QCoreApplication(argc, (char**)argv); // accessed as qApp
    QFileInfo fi(argv[0]);
    milestone |= SM_QAPP_OBJ;
  }
  QCoreApplication::instance()->setApplicationName(taMisc::app_name);
  // probably as good a place as any to init ODE
  dInitODE();
  return true;
}

// helper functions for taRootBase::Startup_InitTA_AppFolders()
namespace { // anon
  // Determine if a dir is a plugin dir.
  bool IsPluginDir(const String& path) {
    return QDir(path).exists();
  }

  // Determine if a dir is an app dir.
  bool IsAppDir(const String& path, String* plugin_path = NULL) {
    // First check if the path contains a prog_lib subdirectory.
    QDir dir(path);
    if (dir.exists("prog_lib")) {
      if (plugin_path) {
        // If requested, check if there is a plugin subdirectory; if so,
        // set the out-parameter to it (valid for dev installs).
        String plugin_dir = taMisc::GetSysPluginDir();
        if (dir.exists(plugin_dir)) {
          *plugin_path = path + PATH_SEP + plugin_dir;
        }
        else {
          // Doesn't disqualify the directory as an app dir, so just inform.
          taMisc::DebugInfo("Did not find", plugin_dir, "subdir in", path);
        }
      }

      return true;
    }

    taMisc::DebugInfo("Did not find app_dir as:", path);
    return false;
  }

  // Setup the global variables taMisc::exe_cmd and taMisc::exe_path
  void InitExecCmdPath()
  {
    //note: this is not how Qt does it, but it seems windows follows normal rules
    // and passes the arg[0] as the full path to the executable, so we just get path
    taMisc::exe_cmd = taMisc::args_raw.SafeEl(0);
    QFileInfo fi(taMisc::exe_cmd);

    //note: argv[0] can contain a relative path, so we need to absolutize
    // but *don't* dereference links, because we typically want to use the
    // link file, not the target, which for dev contexts may be buried somewhere
    taMisc::exe_path = fi.absolutePath();

  #if defined(TA_OS_MAC)
    /* Note: for Mac, if the bin is in a bundle, then it will be a link
       to the actual file, so in this case, we dereference it
       {app_dir}/{appname.app}/Contents/MacOS (bundle in app root)
       {app_dir}/bin/{appname.app}/Contents/MacOS (bundle in app bin)
       {app_dir}/bin (typically non-gui only, since gui must run from bundle)
    */
    if (taMisc::exe_path.endsWith("/Contents/MacOS")) {
      taMisc::exe_path = fi.canonicalPath();
    }
    // seemingly not in a bundle, so use Unix defaults...
  #endif // Mac
  }

#ifndef TA_OS_WIN
  // Paths to search for an emergent installation.
  const char* DEF_PREFIXES[] = { "/usr/local", "/usr", "/opt/local", "/opt" };
  const int DEF_PREFIX_N = 4;
#endif

  // Determine the application directory to use.  Return false on failure.
  // On success, return true, and set app_dir out parameter.  Out parameter
  // app_plugin_dir may be set for in-place contexts (Windows and dev).
  // Out parameter prefix_dir may be set for non-windows platforms.
  // Side effect: sets taMisc::in_dev_exe and taMisc::use_plugins.
  bool GetAppDir(String &app_dir, String &app_plugin_dir, String &prefix_dir)
  {
    // Search for app path in following order:
    // 1. app_dir command line switch (may require app_plugin_dir switch too)
    // 2. "in-place" development (this is either the same as
    //    or never conflicts with the installed production version)
    // 3. EMERGENTDIR (Windows) or EMERGENT_PREFIX_DIR (Unix) variable
    // 4. a platform-specific heuristic search
    // NOTE: for "in-place" contexts, plugin dir is local, else look independently

    app_dir = taMisc::FindArgByName("AppDir");
    if (app_dir.nonempty() && IsAppDir(app_dir)) {
      return true;
    }

  #ifdef TA_OS_WIN

    // {app_dir}\bin\xxx (MSVS build)
    // {app_dir}\bin (normal release, nmake build)
    String bin_dir = taMisc::exe_path; // tmp to work on..
    bin_dir.gsub("/", "\\");
    if (bin_dir.contains("\\bin\\")) {
      if (bin_dir.contains("\\build")) {
        app_dir = bin_dir.before("\\build");
        if (IsAppDir(app_dir, &app_plugin_dir)) {
          taMisc::Info("Note: running development executable: not loading plugins.");
          taMisc::in_dev_exe = true;
          taMisc::use_plugins = false;
          return true;
        }
      }

      app_dir = bin_dir.before("\\bin\\");
      if (IsAppDir(app_dir, &app_plugin_dir)) {
        return true;
      }
    }

    app_dir = getenv("EMERGENTDIR");
    if (app_dir.nonempty() && IsAppDir(app_dir)) {
      return true;
    }

    if (bin_dir.endsWith("\\bin")) {
      app_dir = bin_dir.at(0, bin_dir.length() - 4);
      if (IsAppDir(app_dir)) {
        return true;
      }
    }

  #else // Unix/Mac

    // {app_dir}/build[{SUFF}]/bin (cmake development) TEST FIRST!
    // {app_dir}/bin (legacy development)
    // {prefix_dir}/bin (standard Unix deployment)
    String bin_dir = taMisc::exe_path; // tmp to work on..
    if (bin_dir.endsWith("/bin")) {
      if (bin_dir.contains("/build")) {
        app_dir = bin_dir.before("/build");
        if (IsAppDir(app_dir, &app_plugin_dir)) {
          taMisc::Info("Note: running development executable: not loading plugins.");
          taMisc::in_dev_exe = true;
          taMisc::use_plugins = false;
          return true;
        }
      }

      // always try to find share install guy first
      String tmp_dir = bin_dir.at(0, bin_dir.length() - 4);
      app_dir = tmp_dir + "/share/" + taMisc::default_app_install_folder_name;
      if (IsAppDir(app_dir, &app_plugin_dir)) {
        return true;
      }

      app_dir = tmp_dir;
      if (IsAppDir(app_dir, &app_plugin_dir)) {
        return true;
      }
    }

    // Build a list of directories to search:
    // * environment variable (if present)
    // * the prefix implied by bin_dir (if ends with /bin)
    // * the usual locations (/usr/local etc.)
    std::vector<String> search_prefixes;
    String temp = getenv("EMERGENT_PREFIX_DIR");
    if (!temp.empty()) {
      search_prefixes.push_back(temp);
    }
    if (bin_dir.endsWith("/bin")) {
      search_prefixes.push_back(bin_dir.before("/bin"));
    }
    search_prefixes.insert(
      search_prefixes.end(), DEF_PREFIXES, DEF_PREFIXES + DEF_PREFIX_N);

    // Search the directories.
    for (int i = 0; i < search_prefixes.size(); ++i) {
      prefix_dir = search_prefixes[i];
      app_dir = prefix_dir + "/share/" + taMisc::default_app_install_folder_name;
      if (IsAppDir(app_dir)) {
        // Note: the prefix_dir that successfully produced app_dir will
        // be returned by out-parameter.
        return true;
      }
    }

  #endif // Unix/Mac

    // No valid app_dir found, so clear out-parameters.
    app_dir = _nilString;
    app_plugin_dir = _nilString;
    prefix_dir = _nilString;
    return false;
  }

  bool GetAppPluginDir(String &app_plugin_dir, const String &prefix_dir) {
    // We search for plugin path in following order:
    // 1. app_plugin_dir command line switch
    // 2. previously established "in-place" location (only for Windows
    //    and dev contexts)
    // 3. EMERGENT_PLUGIN_DIR variable
    // 4. Unix/Mac: {EMERGENT_PREFIX_DIR}/lib/Emergent (if env var set)
    // 5. a platform-specific heuristic search
    // NOTE: for "in-place" contexts, plugin dir is local, else look independently

    // Check for command line arg.
    String app_plugin_dir_cmd_line = taMisc::FindArgByName("AppPluginDir");
    if (app_plugin_dir_cmd_line.nonempty()) {
      app_plugin_dir = app_plugin_dir_cmd_line;
      return true;
    }

    // Check "in-place" location.
    if (app_plugin_dir.nonempty()) {
      return true;
    }

    // Check environment variable.
    app_plugin_dir = getenv("EMERGENT_PLUGIN_DIR");
    if (app_plugin_dir.nonempty()) {
      return true;
    }

  #ifdef TA_OS_WIN
    // This directory was created when emergent was installed.
    app_plugin_dir = taMisc::app_dir + "\\" + taMisc::GetSysPluginDir();
    return true;
  #else // Unix/Mac
    // Only got here because no command line arg, no in-place location,
    // and no environment variable.

    // Build a list of directories to search:
    // * default prefix previously discovered
    // * the usual locations (/usr/local etc.)
    std::vector<String> search_prefixes;
    if (!prefix_dir.empty()) {
      search_prefixes.push_back(prefix_dir);
    }
    search_prefixes.insert(
      search_prefixes.end(), DEF_PREFIXES, DEF_PREFIXES + DEF_PREFIX_N);

    // Search the directories.
    for (int i = 0; i < search_prefixes.size(); ++i) {
      app_plugin_dir = search_prefixes[i] + "/lib/" +
        taMisc::default_app_install_folder_name + "/" + taMisc::GetSysPluginDir();
      if (IsPluginDir(app_plugin_dir)) {
        return true;
      }
    }

    // App plugin directory not found.
    app_plugin_dir = _nilString;
    return false;
  #endif // Unix/Mac
  }
}

// hairy, modal, issue-prone -- we put in its own routine
// Sets:
// * taMisc::exe_cmd
// * taMisc::exe_path
// * taMisc::app_dir
// * taMisc::in_dev_exe
// * taMisc::use_plugins
bool taRootBase::Startup_InitTA_AppFolders() {
  // WARNING: cannot use QCoreApplication::applicationDirPath() at this point
  // because QCoreApplication has not been instantiated yet

  // Set taMisc::exe_cmd and taMisc::exe_path.
  InitExecCmdPath();

  // Initialize the key folders.
  String app_dir;
  String app_plugin_dir;
  String prefix_dir; // only applicable for mac/unix.

  // Find the application directory (and if possible, also get the
  // plugin directory and prefix for the application directory).
  bool found = GetAppDir(app_dir, app_plugin_dir, prefix_dir);

  // If found, set taMisc::app_dir.
  if (found) {
    taMisc::app_dir = app_dir;
    #ifdef TA_OS_WIN
      taMisc::app_dir.gsub("/", "\\"); // clean it up, so it never causes issues
    #endif
  }
  else {
    // inability to find the app is fatal in 4.0.19
    taMisc::Error(
      "The application install directory could not be found. Please see:\n"
      "http://grey.colorado.edu/emergent/index.php/User_Guide\n"
      "for instructions on setting command line switches and/or environment\n"
      "variables for non-standard installations of the application.\n");

    // use a Choice so console etc. doesn't disappear immediately, ex. on Windows
    taMisc::Choice("The application will now terminate.");
    return false;
  }

  // Determine which directory to use as the plugin directory.
  found = GetAppPluginDir(app_plugin_dir, prefix_dir);

  // If no plugin directory found, warn the user.
  if (!found || !IsPluginDir(app_plugin_dir)) {
    taMisc::Error("Expected application plugin folder",
      app_plugin_dir, "does not exist! You should check your installation "
      "and/or create this folder, otherwise runtime errors may occur.");
    // Missing plugin directory isn't fatal, so continue on.
  }

  taMisc::app_plugin_dir = app_plugin_dir;
  return true;
}

bool taRootBase::Startup_InitTA_InitUserAppDir() {
  // make sure the folder exists
  // make sure the standard user subfolders exist:
  QDir dir(taMisc::user_app_dir);
  if (!dir.exists()) {
    if (!dir.mkpath(taMisc::user_app_dir)) {
      taMisc::Error("Could not find or make the user dir:", taMisc::user_app_dir,
        "Please make sure this directory exists and is readable, and try again"
        " -- or use the -UserAppDir= command line option.");
      return false;
    }
  }
  //NOTE: we could get excessively anal, and check all of these, but if we
  // can make/read the user folder, then very unlikely will these fail
  dir.mkpath(taMisc::user_app_dir + PATH_SEP + "css_lib");
  dir.mkpath(taMisc::user_app_dir + PATH_SEP + "log");
  dir.mkpath(taMisc::user_app_dir + PATH_SEP + taMisc::GetUserPluginDir());
  dir.mkpath(taMisc::user_app_dir + PATH_SEP + "prefs");
  dir.mkpath(taMisc::user_app_dir + PATH_SEP + "prog_lib");
  return true;
}

//NOTE: this can be nuked post 4.0.20-ish
void Startup_InitTA_MoveLegacyUserFiles() {
  String msg;
  // Preferences directory
  // 1. we check for an the old one (<= 4.0.18) -- we'll move contents silently...
  String prefs_dir = taPlatform::getAppDataPath(taMisc::app_prefs_key);
  if (!taPlatform::fileExists(prefs_dir + "/options"))
    return;

  // we only automatically handle the default folder case
  String legacy_uad;
#ifdef TA_OS_WIN
  legacy_uad = taPlatform::getDocPath() + "\\emergent_user";
#else
  legacy_uad = taPlatform::getHomePath() + "/emergent_user";
#endif

  // fixup if possible -- just move legacy to most recent user_app_dir folder
  QDir uad(legacy_uad);
  if (!uad.exists()) return;

  taMisc::Info("Moving preferences to new default location...");
  // note, 'prefs' subfolder shouldn't exist yet...
  if (uad.exists("prefs")) {
    if (!uad.rename("prefs", "prefs.old"))
      goto prefs_move_failed;
  }
  if (uad.rename(prefs_dir, "prefs")) {
    taMisc::Info("...preferences moved.");
    goto move_uad;
  }

prefs_move_failed:
  taMisc::Error("PREFERENCES COULD NOT BE MOVED -- your preferences may revert to defaults.");

move_uad:

  String user_app_dir = taPlatform::getAppDocPath(taMisc::app_prefs_key);
  taMisc::Info("Moving user data dir to new default location...");
  // note, Emergent folder shouldn't exist yet...
  if (QDir(user_app_dir).exists())
    goto uad_move_failed;
  if (!taPlatform::mv(legacy_uad, user_app_dir))
    goto uad_move_failed;
  msg = "...your emergent user data folder was moved\n"
    "from: " + legacy_uad + "\n"
    "  to: " + user_app_dir + "\n\n";
  taMisc::Info(msg);

  return;

uad_move_failed:
  msg = "Your current emergent data folder could not be moved\n"
    "from: " + legacy_uad + "\n"
    "  to: " + user_app_dir + "\n\n"
    " -- please move it manually.";
  taMisc::Error(msg);
}

bool taRootBase::Startup_InitTA(ta_void_fun ta_init_fun) {
  // first initialize the types
  if(ta_init_fun)
    (*ta_init_fun)();
  taMisc::Init_Hooks(); // client dlls register init hooks -- this calls them!
  milestone |= SM_TYPES_INIT;

  // user directory, aka Home folder -- we don't necessarily use it as a base here though
  // cmd line override of UserDir takes preference
  taMisc::user_dir = taMisc::FindArgByName("UserDir");
  if (taMisc::user_dir.empty()) {
    taMisc::user_dir = taPlatform::getHomePath();
  }

  // move legacy prefs into current default location
  Startup_InitTA_MoveLegacyUserFiles();
  // we can assume files are in their default location now

  // Application folder
  // env var overrides default
  String user_app_dir_env_var = upcase(taMisc::app_prefs_key) + "_USER_APP_DIR";
  String user_app_dir = getenv(user_app_dir_env_var);
  if (user_app_dir.empty()) {
    user_app_dir = taPlatform::getAppDocPath(taMisc::app_prefs_key);
  }
  taMisc::user_app_dir = user_app_dir;
  if (!Startup_InitTA_InitUserAppDir()) return false;

  // Preferences directory
  taMisc::prefs_dir = taMisc::user_app_dir + PATH_SEP + "prefs";
  taMisc::Init_Defaults_PreLoadConfig();
  // then load configuration info: sets lots of user-defined config info
  ((taMisc*)TA_taMisc.GetInstance())->LoadConfig();
  // ugh: reload because legacy options file will load its value
  taMisc::user_app_dir = user_app_dir;

  taMisc::user_plugin_dir = taMisc::user_app_dir + PATH_SEP +
    taMisc::GetUserPluginDir();
  taMisc::user_log_dir = taMisc::user_app_dir + PATH_SEP + "log";

  // System (Share) Folder, System Plugins
  if (!Startup_InitTA_AppFolders()) return false;

  taMisc::Init_Defaults_PostLoadConfig();

  console_type = taMisc::console_type;
  console_options = taMisc::console_options;

  taMisc::default_scope = &TA_taProject; // this is general default

  // load prefs values for us
  taRootBase* inst = instance();
  milestone |= SM_ROOT_CREATE;
  inst->SetFileName(taMisc::prefs_dir + "/root");
  if (QFile::exists(inst->GetFileName())) {
    ++taFiler::no_save_last_fname;
    inst->Load();
    --taFiler::no_save_last_fname;
  }

  if(taMisc::default_proj_type)
    inst->projects.el_typ = taMisc::default_proj_type;

  // make sure the app dir is on the recent paths
  if (instance()->recent_paths.FindEl(taMisc::app_dir) < 0) {
    instance()->AddRecentPath(taMisc::app_dir);
  }

  // and sidebar paths
  instance()->sidebar_paths.AddUnique(taMisc::app_dir);
  String desktop_path = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  String docs_path = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
  instance()->sidebar_paths.AddUnique(desktop_path);
  instance()->sidebar_paths.AddUnique(docs_path);

  // start recording stuff at this point -- only with gui active
  if(taMisc::use_gui) {
    String bkup_fn = taMisc::user_log_dir + "/default_project_log.plog";
    taMisc::SetLogFile(bkup_fn);
  }
  return true;
}

bool taRootBase::Startup_EnumeratePlugins() {
  if (!taMisc::use_plugins) return true;
  String plug_log;
  if (taMisc::build_str.empty()) {
    plug_log = "plugins.log";
  } else {
    plug_log = "plugins_" + taMisc::build_str + ".log";
  }
  // add plugin folders
  taPlugins::AddPluginFolder(taMisc::app_plugin_dir);
  taPlugins::AddPluginFolder(taMisc::user_plugin_dir);

  taPlugins::InitLog(taMisc::user_log_dir + PATH_SEP + plug_log);
  taPlugins::EnumeratePlugins();

  if(taMisc::CheckArgByName("ListAllPlugins")) {
    tabMisc::root->plugins.ListAllPlugins();
  }
  if(taMisc::CheckArgByName("EnableAllPlugins")) {
    tabMisc::root->plugins.EnableAllPlugins();
    --in_init;                  // allow it to save!
    tabMisc::root->Save();      // save after enabling
    ++in_init;
  }
  return true;
}

bool taRootBase::Startup_LoadPlugins() {
  if (!tabMisc::root) return false; // should be made
  if (!taMisc::use_plugins) return true;
  tabMisc::root->plugins.LoadPlugins();
  return true;
}

bool taRootBase::Startup_InitTypes() {
  taMisc::Init_Types();
  return true;
}

bool taRootBase::Startup_InitCss() {
  return cssMisc::Initialize();
}

bool taRootBase::Startup_InitGui() {
#ifdef TA_GUI
  if(taMisc::use_gui && (taMisc::dmem_proc == 0)) {
    taiM_ = taiMisc::New(taMisc::use_gui);
    taiMC_ = taiM_;
    // the following should be done in the specific app's Main function
//     taiM->icon_bitmap = new QBitmap(emergent_bitmap_width,
//      emergent_bitmap_height, emergent_bitmap_bits);
//    qApp->setWindowIcon(QIcon(*(taiM->icon_bitmap)));

    if(taMisc::gui_no_win)
      taMisc::gui_active = false;       // in effect, we start as use_gui but never get to gui_active -- everything is initialized but no windows are created
    else
      taMisc::gui_active = true;        // officially active!
    Startup_InitViewColors();
  }
  else
#endif // TA_GUI
  {
    taiMC_ = taiMiscCore::New();
  }
  milestone |= SM_APP_OBJ;
  return true;
}

/* emacs colors:
 comment        Firebrick
 string         RosyBrown
 keyword        Purple
 builtin        Orchid (also preprocessor)
 function-name  Blue1
 variable-name  DarkGoldenrod
 type           ForestGreen
 constant       CadetBlue
 warning        Red1
*/

bool taRootBase::Startup_InitViewColors() {
  if(!taMisc::view_colors) {
    taMisc::view_colors = new ViewColor_List;
    taMisc::view_colors->BuildHashTable(100); // speed this one up
  }

  // args are: name, description, foreground, fg_color_name, background, bg_color_name

  taMisc::view_colors->FindMakeViewColor("NotEnabled", "State: !isEnabled",
                                         false, _nilString, true, "grey80");
  taMisc::view_colors->FindMakeViewColor("ThisInvalid", "State: item not valid according to CheckConfig",
                                         false, _nilString, true, "red1");
  taMisc::view_colors->FindMakeViewColor("ChildInvalid", "State: child not valid according to CheckConfig",
                                         false, _nilString, true, "orange1");
  taMisc::view_colors->FindMakeViewColor("SpecialState1", "SpecialState = 1",
                                         false, _nilString, true, "lavender");
  taMisc::view_colors->FindMakeViewColor("SpecialState2", "SpecialState = 2",
                                         false, _nilString, true, "LightYellow");
  taMisc::view_colors->FindMakeViewColor("SpecialState3", "SpecialState = 3",
                                         false, _nilString, true, "PaleGreen");
  taMisc::view_colors->FindMakeViewColor("SpecialState4", "SpecialState = 4",
                                         false, _nilString, true, "MistyRose");
  taMisc::view_colors->FindMakeViewColor("ProgElNonStd", "State: program element is not standard",
                                         false, _nilString, true, "yellow1");
  taMisc::view_colors->FindMakeViewColor("ProgElNewEl", "State: program element is newly added",
                                         false, _nilString, true, "SpringGreen1");
  taMisc::view_colors->FindMakeViewColor("ProgElVerbose", "State: program element is verbose",
                                         false, _nilString, true, "khaki");
  taMisc::view_colors->FindMakeViewColor("ProgElError", "State: program element has Error",
                                         false, _nilString, true, "red2");
  taMisc::view_colors->FindMakeViewColor("ProgElWarning", "State: program element has Warning",
                                         false, _nilString, true, "orange2");
  taMisc::view_colors->FindMakeViewColor("ProgElBreakpoint", "State: program element is set for a breakpoint",
                                         false, _nilString, true, "violet");
  taMisc::view_colors->FindMakeViewColor("Comment", "Program comment",
                                         true, "firebrick", true, "firebrick1");
  taMisc::view_colors->FindMakeViewColor("ProgCtrl", "Program keyword",
                                         true, "purple2", true, "MediumPurple1");
  taMisc::view_colors->FindMakeViewColor("Function", "Program function",
                                         true, "blue1", true, "LightBlue2");
  taMisc::view_colors->FindMakeViewColor("ProgType", "Program type",
                                         true, "lime green", true, "lime green");
  taMisc::view_colors->FindMakeViewColor("ProgVar", "Program variable",
                                         true, "dark goldenrod");
  taMisc::view_colors->FindMakeViewColor("ProgArg", "Program argument",
                                         true, "goldenrod");
  taMisc::view_colors->FindMakeViewColor("Program", "Program itself",
                                         true, "coral2", true, "coral1");
  taMisc::view_colors->FindMakeViewColor("DataTable", "DataTable and associated objects",
                                         true, "forest green", true, "pale green");
  taMisc::view_colors->FindMakeViewColor("Wizard", "Wizard and associated objects",
                                         true, "azure4", true, "azure1");
  taMisc::view_colors->FindMakeViewColor("SelectEdit", "SelectEdit -- editor for selected variables across different objects",
                                         true, "azure4", true, "azure1");
  taMisc::view_colors->FindMakeViewColor("Doc", "Documentation object",
                                         true, "azure4", true, "azure1");
  return true;
}

bool taRootBase::Startup_ConsoleType() {
  // arbitrate console options
  // first, make sure requested console_type is a legal value for this platform

  // note: is_batch could be extended to include "headless" cmd line invocation
  //   it would also include contexts such as piping or other stdin/out redirects
  bool is_batch = !cssMisc::init_interactive;
#ifdef DMEM_COMPILE
  if(taMisc::gui_active) {
    if((taMisc::dmem_nprocs > 1) && (taMisc::dmem_proc > 0)) // non-first procs batch
      is_batch = true;
  }
  else {
    if(taMisc::dmem_nprocs > 1) // nogui dmem is *ALWAYS* batch for all procs
      is_batch = true;
  }
#endif

  if (is_batch) {
    console_type = taMisc::CT_NONE;
    console_options &= ~(taMisc::CO_USE_PAGING_GUI | taMisc::CO_USE_PAGING_NOGUI); // damn well better not use paging!!!
  } else if (taMisc::gui_active) {
#ifdef HAVE_QT_CONSOLE
    if (!((console_type == taMisc::CT_OS_SHELL) ||
         (console_type == taMisc::CT_GUI) ||
         (console_type == taMisc::CT_NONE))
    ) console_type = taMisc::CT_GUI;
#else
    if (!((console_type == taMisc::CT_OS_SHELL) ||
         (console_type == taMisc::CT_NONE))
    ) console_type = taMisc::CT_OS_SHELL;
#endif
  } else { // not a gui context, can only use a non-gui console
    if (!((console_type == taMisc::CT_OS_SHELL) ||
         (console_type == taMisc::CT_NONE))
    ) console_type = taMisc::CT_OS_SHELL;
  }
  return true; // always works
}

bool taRootBase::Startup_MakeWizards() {
  tabMisc::root->MakeWizards();
  return true;
}

bool taRootBase::Startup_InitPlugins() {
  if (!taMisc::use_plugins) return true;
  if (!tabMisc::root) return false; // should be made
  tabMisc::root->plugins.InitPlugins();
  return true;
}

bool taRootBase::Startup_MakeMainWin() {
  tabMisc::root->version = taMisc::version;
  if(!taMisc::gui_active) return true;
#ifdef TA_GUI
  // TODO: need to better orchestrate the "OpenWindows" call below with
  // create the default application window
  MainWindowViewer* vwr;
  if(tabMisc::root->viewers.size >= 1) {
    vwr = (MainWindowViewer*)tabMisc::root->viewers[0];
  }
  else {
    vwr = MainWindowViewer::NewBrowser(tabMisc::root, NULL, true);
  }
  // try to size fairly large to avoid scrollbars
  vwr->SetUserData("view_win_wd", 0.7f);
  float ht = 0.5f; // no console
//  iSize s(1024, 480); // no console  (note: values obtained empirically)
  if((console_type == taMisc::CT_GUI) && (console_options & taMisc::CO_GUI_DOCK)) {
    ht = 0.8f; // console
    ConsoleDockViewer* cdv = new ConsoleDockViewer;
    vwr->docks.Add(cdv);
  }
  vwr->SetUserData("view_win_ht", ht);
  vwr->ViewWindow();
#ifdef TA_OS_WIN
  taMisc::ProcessEvents(); // may be needed for Windows (see taProject::DoView)
#endif
  tabMisc::root->docs.AutoEdit();
  tabMisc::root->wizards.AutoEdit();

  iMainWindowViewer* bw = vwr->viewerWindow();
  if (bw) { //note: already constrained to max screen size, so we don't have to check
    // main win handle internal app urls
    taiMisc::main_window = bw;
    taiMisc::net_access_mgr->setMainWindow(bw);
    QDesktopServices::setUrlHandler("ta", bw, "taUrlHandler");
    QDesktopServices::setUrlHandler("http", bw, "httpUrlHandler");
    bw->show(); // when we start event loop
  }
  //TODO: following prob not necessary
  if (taMisc::gui_active) taiMisc::OpenWindows();
#endif // TA_GUI
  return true;
}

void taRootBase::WindowShowHook() {
  bool static done = false;
  if (!done) {
    done = true; // set now in case ProcessEvents recurses
    // this is very hacky... select the 2nd tab, which will
    // be the first auto guy if there were any
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.SafeEl(0)); // always the default
    if (vwr) {
      vwr->SelectPanelTabNo(1);
      taiMiscCore::ProcessEvents();
    }
  }
//why is this needed? see bugID:723
//  if(docs.size > 0)
//    docs[0]->EditPanel(true, true); // pin, new tab
}

bool taRootBase::Startup_Console() {
#ifdef HAVE_QT_CONSOLE
  if(taMisc::gui_active && (console_type == taMisc::CT_GUI)) {
    //note: nothing else to do here for gui_dockable
    QcssConsole* con = QcssConsole::getInstance(NULL, cssMisc::TopShell);

    QObject::connect(con, SIGNAL(receivedNewStdin(int)), root_adapter, SLOT(ConsoleNewStdin(int)));
    // get notified

    if(!(console_options & taMisc::CO_GUI_DOCK)) {
      QMainWindow* cwin = new QMainWindow();
      cwin->setWindowTitle("css Console");
      cwin->setCentralWidget((QWidget*)con);
      cwin->resize((int)(.95 * taiM->scrn_s.w), (int)(.25 * taiM->scrn_s.h));
      cwin->move((int)(.025 * taiM->scrn_s.w), (int)(.7 * taiM->scrn_s.h));
      cwin->show();
      taMisc::console_win = cwin; // note: uses a guarded QPointer

      if(tabMisc::root->viewers.size >= 1) {
        taMisc::ProcessEvents();
        MainWindowViewer* db = (MainWindowViewer*)tabMisc::root->viewers[0];
        db->ViewWindow();               // make sure root guy is on top
      }
    }
  }
#endif
  cssMisc::TopShell->StartupShellInit(cin, cout, console_type);

  return true;
}

void taRootBase::ConsoleNewStdin(int n_lines) {
#ifdef HAVE_QT_CONSOLE
  if(!taMisc::gui_active || (console_type != taMisc::CT_GUI)) return;
  if(taMisc::console_win) {
    QApplication::alert(taMisc::console_win);
  }
  else {
    // assume dock..
    if(tabMisc::root->viewers.size >= 1) {
      MainWindowViewer* db = (MainWindowViewer*)tabMisc::root->viewers[0];
      QApplication::alert(db->widget());
    }
  }
#endif
}


bool taRootBase::Startup_RegisterSigHandler() {
// #if (!defined(DMEM_COMPILE))
  // let's see if this works now!
  taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) SaveRecoverFileHandler);
  milestone |= SM_REG_SIG;
// #endif
  return true;
}

bool taRootBase::Startup_ProcessArgs() {
  bool run_startup = true;
  if(taMisc::CheckArgByName("Version")) {
    tabMisc::root->About();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("Help")) {
    taMisc::HelpMsg();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("GenDoc")) {
    taMisc::help_detail = taMisc::HD_DEFAULT; // always render default
    taGenDoc::GenDoc(&(taMisc::types));
    run_startup = false;
  }
  if(taMisc::CheckArgByName("MakeAllPlugins")) {
    taPlugins::MakeAllPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("MakeAllUserPlugins")) {
    taPlugins::MakeAllUserPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("MakeUserPlugin")) {
    String plugnm = taMisc::FindArgByName("MakeUserPlugin");
    if(plugnm.nonempty()) {
      taPlugins::MakeUserPlugin(plugnm);
    }
    run_startup = false;
  }
  if(taMisc::CheckArgByName("MakeAllSystemPlugins")) {
    taPlugins::MakeAllSystemPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("MakeSystemPlugin")) {
    String plugnm = taMisc::FindArgByName("MakeSystemPlugin");
    if(plugnm.nonempty()) {
      taPlugins::MakeSystemPlugin(plugnm);
    }
    run_startup = false;
  }
  if(taMisc::CheckArgByName("CleanAllPlugins")) {
    taPlugins::CleanAllPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("CleanAllUserPlugins")) {
    taPlugins::CleanAllUserPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("CleanAllSystemPlugins")) {
    taPlugins::CleanAllSystemPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("ListAllPlugins") || taMisc::CheckArgByName("EnableAllPlugins")) {
    run_startup = false;
  }

  // just load the thing!?
  String proj_ld = taMisc::FindArgByName("Project");
  if(proj_ld.empty())
    proj_ld = taMisc::FindArgValContains(".proj");

  if(!proj_ld.empty()) {
    tabMisc::root->projects.Load(proj_ld);
  }

  if(run_startup) {
    // chain the next step -- this will hopefully happen *after* any post-loading
    // events triggered by the projects.load
    QTimer::singleShot(0, root_adapter, SLOT(Startup_RunStartupScript()));
  }
  else {
    taiMC_->Quit();
  }

  return true;
}

bool taRootBase::Startup_RunStartupScript() {
  bool ran = cssMisc::TopShell->RunStartupScript();
  if(ran && !taMisc::gui_active && !cssMisc::init_interactive)
    taiMC_->Quit();
  return true;
}

bool taRootBase::Startup_Main(int& argc, const char* argv[], ta_void_fun ta_init_fun,
                              TypeDef* root_typ)
{
  ++in_init;
  root_type = root_typ;
#ifdef GPROF
  moncontrol(0);                // turn off at start
#endif
#ifdef SATURN_PROF
  // this actually seems bad: get a warning
  // initSaturn("");            // store in current wd
#endif

#ifdef TA_OS_MAC
  // this is necessary to get coin to use system fonts wit freetype
  setenv("COIN_FONT_PATH", "/Library/Fonts", 1);
#endif

  // just create the adapter obj, whether needed or not
  root_adapter = new taRootBaseAdapter;
  cssMisc::prompt = taMisc::app_name; // the same
  if (taMisc::app_prefs_key.empty())
    taMisc::app_prefs_key = taMisc::app_name;
  if(!Startup_InitDMem(argc, argv)) goto startup_failed;
  if(!Startup_InitArgs(argc, argv)) goto startup_failed;
  if(!Startup_ProcessGuiArg(argc, argv)) goto startup_failed;
  if(!Startup_InitTA(ta_init_fun)) goto startup_failed;
  if(!Startup_InitApp(argc, argv)) goto startup_failed;
  if(!Startup_InitTypes()) goto startup_failed;
  if(!Startup_EnumeratePlugins()) goto startup_failed;
  if(!Startup_LoadPlugins()) goto startup_failed; // loads those enabled, and does type integration
  if(!Startup_InitCss()) goto startup_failed;
  if(!Startup_InitGui()) goto startup_failed; // note: does the taiType bidding
  if(!Startup_ConsoleType()) goto startup_failed;
  Startup_MakeWizards(); // supposedly can't fail...
  if(!Startup_InitPlugins()) goto startup_failed; // state, wizards, etc.
  if(!Startup_MakeMainWin()) goto startup_failed;
  if(!Startup_Console()) goto startup_failed;
  if(!Startup_RegisterSigHandler()) goto startup_failed;
  // note: Startup_ProcessArgs() is called after having entered the event loop
  // note: don't call event loop yet, because we haven't initialized main event loop
  // happens in Startup_Run()
  --in_init;
  if(taMisc::gui_active && (taMisc::dmem_proc == 0))    // only guy and don't have all the other nodes save
    instance()->Save();

  if(taMisc::CheckArgByName("AttachWait")) {
#ifdef DMEM_COMPILE
    String awval = taMisc::FindArgByName("AttachWait");
    if(awval.nonempty()) {
      int procno = (int)awval;
      if(taMisc::dmem_proc != procno) return true; // bail
    }
#endif
    volatile int i = 0;
    printf("PID %d on %s ready for attach\n", taPlatform::processId(),
           taPlatform::hostName().chars());
    fflush(stdout);
    // NOTE to programmer: in gdb debugger, do: set var i = 1  then continue -- this will break out of
    // following infinite loop and allow code to continue execution
    while (0 == i) {
      taPlatform::sleep(5);
    }
  }

  return true;

startup_failed:
  Cleanup_Main();
  return false;
}

///////////////////////////////////////////////////////////////////////////
//      Run & Cleanup

bool taRootBase::Startup_Run() {
#ifdef DMEM_COMPILE
//TODO: make Run_GuiDMem dispatched after event loop, and fall through
  if((taMisc::dmem_nprocs > 1) && taMisc::gui_active) {
    Run_GuiDMem(); // does its own eventloop dispatch
    return true;
  }
#endif

  // if in server mode, make it now!
  if (taMisc::args.FindName("Server") >= 0) {
    TemtServer* server = (TemtServer*)instance()->objs.New(1, &TA_TemtServer);
    server->port = (ushort)taMisc::args.GetValDef("Port", 5360).toUInt();
    if (server->InitServer() && server->OpenServer()) {
      cerr << "TemtServer is now running and waiting for connections\n";
    } else {
      cerr << "ERROR: could not Initialize or Open TemtServer\n";
    }
  }


  // first thing to do upon entering event loop:
  QTimer::singleShot(0, root_adapter, SLOT(Startup_ProcessArgs()));

  if (taMisc::gui_active || cssMisc::init_interactive) {
    // next thing is to start the console if interactive
    if (console_type == taMisc::CT_NONE) {
      QTimer::singleShot(0, cssMisc::TopShell, SLOT(Shell_NoConsole_Run()));
    }
  }

  // now everyone goes into the event loop
  taiMC_->Exec();

  Cleanup_Main();
  return true;
}

extern "C" {
  extern void rl_free_line_state(void);
  extern void rl_cleanup_after_signal(void);
}

// todo: could partition these out into separate guys..
void taRootBase::Cleanup_Main() {
  taMisc::in_shutdown++;
  taMisc::aka_types.Reset();    // errs happen when this gets reset out of order
  // remove sig handler -- very nasty when baddies happen after this point
  if (milestone & SM_REG_SIG) {
    taMisc::Register_Cleanup(SIG_DFL); // replace back to using default
  }
  cssMisc::Shutdown();          // shut down css..
  if (milestone & SM_ROOT_CREATE)
    tabMisc::DeleteRoot();
  if (milestone & SM_TYPES_INIT)
    taMisc::types.RemoveAll();  // get rid of all the types before global dtor!

#ifdef TA_USE_INVENTOR
  if(taMisc::gui_active && (milestone & SM_SOQT_INIT)) {
#if COIN_MAJOR_VERSION >= 3
    if(coin_image_reader_cb_obj) {
      delete coin_image_reader_cb_obj;
      coin_image_reader_cb_obj= NULL;
    }
#endif
    SIM::Coin3D::Quarter::Quarter::clean();
  }
#endif
  // this may be redundante -- getting errors at end of jobs..
// #ifdef DMEM_COMPILE
//   if (milestone & SM_MPI_INIT)
//     MPI_Finalize();
// #endif
  taThreadMgr::TerminateAllThreads(); // don't leave any active threads lying around

#ifndef TA_OS_WIN
  // only if using readline-based console, reset tty state
  if((console_type == taMisc::CT_NONE) && (taMisc::gui_active || cssMisc::init_interactive)) {
    rl_free_line_state();
    rl_cleanup_after_signal();
  }
#endif
}

#ifdef DMEM_COMPILE

bool taRootBase::Run_GuiDMem() {
  if (taMisc::dmem_proc == 0) { // master dmem
    DMemShare::InitCmdStream();
    // need to have some initial string in the stream, otherwise it goes EOF and is bad!
    *(DMemShare::cmdstream) << "cerr << \"proc no: \" << taMisc::dmem_proc << endl;" << endl;
    taMisc::StartRecording((ostream*)(DMemShare::cmdstream));
    taiMC_->Exec();  // normal run..
    DMemShare::CloseCmdStream();
    cerr << "proc: 0 quitting!" << endl;
  } else { // slave dmems
    cssMisc::init_interactive = false; // don't stay in startup shell
    QTimer::singleShot(0, root_adapter, SLOT(DMem_SubEventLoop()));
    taiMC_->Exec();  // event loop
    cerr << "proc: " << taMisc::dmem_proc << " quitting!" << endl;
  }
  return true;
}

static cssProgSpace* dmem_space1 = NULL;
static cssProgSpace* dmem_space2 = NULL;

void taRootBase::DMem_WaitProc(bool send_stop_to_subs) {
  if(dmem_space1 == NULL) dmem_space1 = new cssProgSpace;
  if(dmem_space2 == NULL) dmem_space2 = new cssProgSpace;

  if(DMemShare::cmdstream->bad() || DMemShare::cmdstream->eof()) {
    taMisc::Error("DMem: Error! cmstream is bad or eof.",
                  "Software will not respond to any commands, must quit!!");
  }
  while(DMemShare::cmdstream->tellp() > DMemShare::cmdstream->tellg()) {
    DMemShare::cmdstream->seekg(0, ios::beg);
    string str = DMemShare::cmdstream->str();
    String cmdstr = str.c_str();
    cmdstr = cmdstr.before((int)(DMemShare::cmdstream->tellp() - DMemShare::cmdstream->tellg()));
    // make sure to only get the part that is current -- other junk might be in there.
    cmdstr += '\n';
    if(taMisc::dmem_debug) {
      cerr << "proc 0 sending cmd: " << cmdstr;
    }
    DMemShare::cmdstream->seekp(0, ios::beg);

    int cmdlen = cmdstr.length();

    DMEM_MPICALL(MPI_Bcast((void*)&cmdlen, 1, MPI_INT, 0, MPI_COMM_WORLD),
                 "Proc 0 WaitProc", "MPI_Bcast - cmdlen");

    DMEM_MPICALL(MPI_Bcast((void*)(const char*)cmdstr, cmdlen, MPI_CHAR, 0, MPI_COMM_WORLD),
                 "Proc 0 WaitProc", "MPI_Bcast - cmd");

    if(taMisc::dmem_debug) {
      cerr << "proc 0 running cmd: " << cmdstr << endl;
    }
    // now run the command: it wasn't run before!
    cssProgSpace* sp = dmem_space1; // if first space is currently running, use another
    if(sp->state & cssProg::State_Run) {
      if(taMisc::dmem_debug)
        cerr << "proc 0 using 2nd space!" << endl;
      sp = dmem_space2;
    }

    sp->CompileCode(cmdstr);
    sp->Run();
    sp->ClearAll();
  }
  if(send_stop_to_subs) {
    String cmdstr = "stop";
    int cmdlen = cmdstr.length();
    DMEM_MPICALL(MPI_Bcast((void*)&cmdlen, 1, MPI_INT, 0, MPI_COMM_WORLD),
                 "Proc 0 WaitProc, SendStop", "MPI_Bcast - cmdlen");
    DMEM_MPICALL(MPI_Bcast((void*)(const char*)cmdstr, cmdlen, MPI_CHAR, 0, MPI_COMM_WORLD),
                 "Proc 0 WaitProc, SendStop", "MPI_Bcast - cmdstr");
  }
}

int taRootBase::DMem_SubEventLoop() {
  if(taMisc::dmem_debug) {
    cerr << "proc: " << taMisc::dmem_proc << " event loop start" << endl;
  }

  if(dmem_space1 == NULL) dmem_space1 = new cssProgSpace;
  if(dmem_space2 == NULL) dmem_space2 = new cssProgSpace;

  while(true) {
    int cmdlen;
    DMEM_MPICALL(MPI_Bcast((void*)&cmdlen, 1, MPI_INT, 0, MPI_COMM_WORLD),
                 "Proc n SubEventLoop", "MPI_Bcast");
    char* recv_buf = new char[cmdlen+2];
    DMEM_MPICALL(MPI_Bcast(recv_buf, cmdlen, MPI_CHAR, 0, MPI_COMM_WORLD),
                 "Proc n SubEventLoop", "MPI_Bcast");
    recv_buf[cmdlen] = '\0';
    String cmd = recv_buf;
    delete[] recv_buf;
    recv_buf = NULL; // defensive

    if(cmd.length() > 0) {
      if(taMisc::dmem_debug) {
       cerr << "proc " << taMisc::dmem_proc << " recv cmd: " << cmd << endl << endl;
      }
      if(cmd == "stop") {
        if(taMisc::dmem_debug)
          cerr << "proc " << taMisc::dmem_proc << " got stop command, stopping out of sub event processing loop." << endl;
        return 1;
      }
      else if(!cmd.contains("Save(") && !cmd.contains("SaveAs(")) {
        if(taMisc::dmem_debug) {
          cerr << "proc " << taMisc::dmem_proc << " running cmd: " << cmd << endl;
        }

        cssProgSpace* sp = dmem_space1; // if first space is currenntly running, use another
        if(sp->state & cssProg::State_Run) {
          if(taMisc::dmem_debug)
            cerr << "proc " << taMisc::dmem_proc << " using 2nd space!" << endl;
          sp = dmem_space2;
        }

        sp->CompileCode(cmd);
        sp->Run();
        sp->ClearAll();

        if(cmd.contains("Quit()")) {
          if(taMisc::dmem_debug)
            cerr << "proc " << taMisc::dmem_proc << " got quit command, quitting." << endl;
          taiMiscCore::Quit(); // unconditional
          return 1;
        }
      }
    }
    else {
      cerr << "proc " << taMisc::dmem_proc << " received null command!" << endl;
    }
    // do basic wait proc here..
    tabMisc::WaitProc();
  }
  return 0;
}

#endif // DMEM

//////////////////////////////////////////////////////////////////////////////
//              Recover File Handler

//#ifndef TA_OS_WIN

#include <signal.h>
#include <memory.h>
#include <sstream>

// for saving a recovery file if program crashes, is killed, etc.
void taRootBase::SaveRecoverFileHandler(int err) {
  static bool has_crashed = false;
  signal(err, SIG_DFL);         // disable catcher

  if(has_crashed) {
    cerr << "Unable to save recover file (multiple errors)...sorry" << endl;
    exit(err);
  }
  has_crashed = true;           // to prevent recursive crashing..

#ifdef TA_OS_WIN
  bool non_term_sig = false;
#else
  bool non_term_sig = ((err == SIGALRM) || (err == SIGUSR1) || (err == SIGUSR2));
#endif

  if(!non_term_sig) {
#ifdef DMEM_COMPILE
    MPI_Finalize();
#endif
    taThreadMgr::TerminateAllThreads(); // don't leave any active threads lying around
#ifdef TA_GUI
    taiMisc::Cleanup(err);      // cleanup stuff in tai
#endif
  }

#ifndef TA_OS_WIN // MS CRT doesn't handle these signals...
  if(non_term_sig) {
    cerr << "Saving project file(s) from signal: ";
  } else
#endif // !TA_OS_WIN
  {
    cerr << "Saving recover file(s) and exiting from signal: ";
  }
  taMisc::Decode_Signal(err);
  cerr << endl;

  if (tabMisc::root) for (int i = 0; i < tabMisc::root->projects.size; ++i) {
    taProject* prj = tabMisc::root->projects.FastEl(i);
    prj->SaveRecoverFile();
  }

#ifdef TA_OS_WIN // MS CRT doesn't handle these signals...
  exit(err);
#else // TA_OS_WIN // MS CRT doesn't handle these signals...
  if(non_term_sig) {
    taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) SaveRecoverFileHandler);
    has_crashed = false;
  } else {
    if(err == SIGTERM) {
      exit(err);                // we need to forcibly exit on this one
    }
    else {
      kill(getpid(), err);      // activate signal
    }
  }
#endif //

}

//#endif // TA_OS_WIN

// NOTE: PluginWizard code is in ta_plugin_p.cpp


void TestWizard::LaunchWizard() {

  TypeDef* td = GetTypeDef();
  taiWizardDataHost* dlg = new taiWizardDataHost(this, td, false); // false = not read only
  dlg->Constr("Wizard");
  if(dlg->Edit(true)) {
  }
}
