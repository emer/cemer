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
#endif

#include "inetworkaccessmanager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QPointer>
#include <QTimer>

#include <time.h>
#include <locale.h>

#ifdef DMEM_COMPILE
#include "ta_dmem.h"
#endif

#ifdef TA_USE_INVENTOR
  #include <Quarter/Quarter.h>
#endif
#include <ode/ode.h>

#ifdef GPROF			// turn on for profiling
extern "C" void moncontrol(int mode);
#endif

#ifdef SATURN_PROF
#include <Saturn.h>
#endif

#ifdef TA_OS_LINUX
# include <fenv.h>
#endif

//////////////////////////
//   taDoc		//
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
    else {
      if(bullet2) { cl += "</ul>"; bullet2 = false; }
      if(bullet1) { cl += "</ul>"; bullet1 = false; }

      if(num2) { cl += "</ol>"; num2 = false; }
      if(num1) { cl += "</ol>"; num1 = false; }
    }

    if(cl.empty()) {		// make a <P> for blank lines..
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
//  UserData_DocLink		//
//////////////////////////////////

void UserData_DocLink::Initialize() {
  Own(doc, this);
}

void UserData_DocLink::SmartRef_DataDestroying(taSmartRef* ref, taBase* obj)
{
// destroy ourself, so we don't have this stale ref left over
  if (ref == &doc)
    this->CloseLater();
}

//////////////////////////////////
//  Doc_Group		//
//////////////////////////////////

void Doc_Group::AutoEdit() {
  taDoc* doc;
  taLeafItr i;
  FOR_ITR_EL(taDoc, doc, this->, i) {
    if (doc->auto_open)
      doc->EditPanel(true, true); // true,true = new tab, pinned in place
  }
}


//////////////////////////
//   taWizard		//
//////////////////////////

void taWizard::Initialize() {
  auto_open = true;
  SetUserData("NO_CLIP", true);
  SetBaseFlag(NAME_READONLY);
}

//////////////////////////////////
// 	Wizard_Group		//
//////////////////////////////////

void Wizard_Group::AutoEdit() {
  taWizard* wz;
  taLeafItr i;
  FOR_ITR_EL(taWizard, wz, this->, i) {
    if (wz->auto_open)
      wz->EditPanel(true, true);// true,true = new tab, pinned in place
  }
}

//////////////////////////////////
//	SelectEdit_Group	//
//////////////////////////////////

void SelectEdit_Group::AutoEdit() {
  taLeafItr i;
  SelectEdit* se;
  FOR_ITR_EL(SelectEdit, se, this->, i) {
    if(se->autoEdit())
      se->EditPanel(true, true);	// true,true = new tab, pinned in place
  }
}

//////////////////////////////////
//	Undo Stuff		//
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
  diff.Reset();			// just in case..
}

void taUndoDiffSrc::EncodeDiff(taUndoRec* rec) {
  if(diff.data_a.line_st.size > 0) { // already done
    diff.ReDiffB(save_data, rec->save_data, false, false, false); // trim, no ignore case
  }
  else {			    // first time
    diff.DiffStrings(save_data, rec->save_data, false, false, false); // trim, no ignore case
  }
  diff.GetEdits(rec->diff_edits);	// save to guy
  last_diff_n = diff.GetLinesChanged(); // counts up total lines changed in diffs
  last_diff_pct = (.5f * (float)last_diff_n) / (float)diff.data_a.lines;
  // lines changed tends to double-count..
#ifdef DEBUG	
  cout << "last_diff_n: " << last_diff_n << " pct: " << last_diff_pct << endl;
#endif  
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
// 	taUndoRec

void taUndoRec::Initialize() {
}

String taUndoRec::GetData() {
  if(save_data.nonempty()) {
    return save_data;		// easy
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
// 	undo threading


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

  um->rec_to_diff->EncodeMyDiff();
  um->rec_to_diff = NULL;	// done, reset!
}

void UndoDiffThreadMgr::Initialize() {
  n_threads = 2;		// don't use 0, just 1..
  task_type = &TA_UndoDiffTask;
}

void UndoDiffThreadMgr::Destroy() {
}

void UndoDiffThreadMgr::Run() {
  n_threads = 2;		// don't use 0, just 1..
  InitAll();
  RunThreads();			// just run the thread, not main guy
}

////////////////////////////
// 	taUndoMgr

void taUndoMgr::Initialize() {
  cur_undo_idx = 0;
  undo_depth = taMisc::undo_depth;
  new_src_thr = taMisc::undo_new_src_thr;
#ifdef DEBUG
  save_load_file = false;	// can set to true if need to do debugging on undo
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

bool taUndoMgr::SaveUndo(taBase* mod_obj, const String& action, taBase* save_top) {
  // only do the undo guy for first call when nested
  if ((nest_count > 0) && (loop_count++ > 0)) return false;
  if(!owner || !mod_obj) return false;
  if(!save_top) save_top = owner;
  if(mod_obj == save_top && mod_obj->HasOption("UNDO_SAVE_ALL")) {
    save_top = owner;		// save all instead..
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

#ifdef DEBUG
  cout << "SaveUndo of action: " << urec->action << " on: " << urec->mod_obj_name
       << " at path: " << urec->mod_obj_path << endl;
#endif

  tabMisc::cur_undo_save_top = save_top; // let others know who we're saving for..
  tabMisc::cur_undo_mod_obj = mod_obj; // let others know who we're saving for..
  ++taMisc::is_undo_saving;
  save_top->Save_String(urec->save_data);
  --taMisc::is_undo_saving;
  tabMisc::cur_undo_save_top = NULL;
  tabMisc::cur_undo_mod_obj = NULL;

  // now encode diff for big saves!
  if(save_top == owner) {
    taUndoDiffSrc* cur_src = NULL;
    if(undo_srcs.length > 0)
      cur_src = undo_srcs.CircPeek(); // always grab the last guy
    if(!cur_src || cur_src->last_diff_pct > new_src_thr) {
      cur_src = new taUndoDiffSrc;
      undo_srcs.CircAddLimit(cur_src, undo_depth); // large depth
      cur_src->InitFmRec(urec);			   // init
#ifdef DEBUG
      cerr << "New source added!" << endl;
#endif
    }
    if(diff_threads.n_running > 0)
      diff_threads.SyncThreads();	// sync now before running again..
    urec->diff_src = cur_src;	// this smartref ptr needs to be set in main task
    rec_to_diff = urec;
    diff_threads.Run();	// run diff in separate thread
  }

  PurgeUnusedSrcs();		// get rid of unused source data

  // tell project to refresh ui
  taProject* proj = GET_MY_OWNER(taProject);
  if(proj) {
    tabMisc::DelayedFunCall_gui(proj,"UpdateUi");
  }

  return true;			// todo: need to check result of Save_String presumably
}

void taUndoMgr::PurgeUnusedSrcs() {
  bool did_purge = false;
  int n_purges = 0;
  do {
    taUndoDiffSrc* urec = undo_srcs.CircSafeEl(0);
    if(!urec) continue;
    int cnt = urec->UseCount();
    if(cnt == 0) {
#ifdef DEBUG
      cout << "purging unused save rec, size: " << urec->save_data.length() << endl;
      taMisc::FlushConsole();
#endif
      undo_srcs.CircShiftLeft(1);
      did_purge = true;
      n_purges++;
      break;
      // we actually need to bail now because UseCount hangs -- only get to do 1 at a time
    }
  } while(did_purge);

#ifdef DEBUG
  if(n_purges > 0) {
    cout << "Total Purges: " << n_purges << " remaining length: " << undo_srcs.length << endl;
    taMisc::FlushConsole();
  }
#endif
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
      SaveUndo(owner, "Undo", owner);
    }
  }
  if(!urec->save_top) {		// it was nuked -- try to reconstruct from path..
    MemberDef* md;
    taBase* st = owner->FindFromPath(urec->save_top_path, md);
    if(st) urec->save_top = st;
  }
  if(!urec->save_top) {
    taMisc::Warning("Undo action:", urec->action, "on object named:", urec->mod_obj_name,
		    "at path:", urec->mod_obj_path,
		    "cannot complete, because saved data is relative to an object that has dissappeared -- it used to live here:", urec->save_top_path);
    --cur_undo_idx;		// need to skip past to get to other levels that might work..
    if(first_undo) --cur_undo_idx;	// need an extra because of extra saveundo.
    return false;
  }
  cout << "Undoing action: " << urec->action << " on: " << urec->mod_obj_name
       << " at path: " << urec->mod_obj_path << endl;
  taMisc::FlushConsole();

  bool rval = LoadFromRec_impl(urec);
  if(rval) {
    --cur_undo_idx;		// only decrement on success
    if(first_undo) --cur_undo_idx;	// need an extra because of extra saveundo.
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
  taMisc::ProcessEvents();	// get any post-load things *before* turning off undo flag..
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
  if(cur_undo_idx == 0) cur_undo_idx = 1;		// 0 is just err state
  if(cur_undo_idx >= undo_recs.length) {
    taMisc::Error("No more steps available to redo -- at end of undo list");
    return false;
  }
  taUndoRec* urec = undo_recs.CircSafeEl(cur_undo_idx); // always at current val for redo..
  if(!urec) return false;
  if(!urec->save_top) {		// it was nuked -- try to reconstruct from path..
    MemberDef* md;
    taBase* st = owner->FindFromPath(urec->save_top_path, md);
    if(st) urec->save_top = st;
  }
  if(!urec->save_top) {
    taMisc::Warning("Redo action:", urec->action, "on object named: ", urec->mod_obj_name,
		    "at path:", urec->mod_obj_path,
		    "cannot complete, because saved data is relative to an object that has dissappeared -- it used to live here:", urec->save_top_path);
    ++cur_undo_idx;		// need to skip past to get to other levels that might work..
    return false;
  }
  cout << "Redoing action: " << urec->action << " on: " << urec->mod_obj_name
       << " at path: " << urec->mod_obj_path << endl;
  taMisc::FlushConsole();

  bool rval = LoadFromRec_impl(urec);
  if(rval) {
    ++cur_undo_idx;		// only increment on success
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
//  taProject		//
//////////////////////////

#ifdef TA_GUI
class SimLogEditDialog: public taiEditDataHost {
public:
  override bool	ShowMember(MemberDef* md) const {
    // just show a small subset of the members
    bool rval = (md->ShowMember(show()) && (md->im != NULL));
    if (!rval) return rval;
// note: we also include a couple of members we know are in taProject
    if (!(md->name.contains("desc") || (md->name == "version") || (md->name == "save_rmv_units")
	 || (md->name == "file_name"))) return false;
    return true;
  }
  override void	Constr_Methods_impl() { }	// suppress methods

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

void taProject::Initialize() {
  m_dirty = false;
  m_no_save = false;
  viewers.SetBaseType(&TA_TopLevelViewer);
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
  taBase::Own(wiki_url, this);
  taBase::Own(templates, this);
  taBase::Own(docs, this);
  taBase::Own(wizards, this);
  taBase::Own(edits, this);
  taBase::Own(data, this);
  taBase::Own(data_proc, this);
  taBase::Own(programs, this);
  taBase::Own(viewers, this);
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
  viewers = cp.viewers;
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

void taProject::PostLoadAutos() {
  DoView();
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
  tabMisc::root->AddRecentFile(val, true);
  return true;
}

int taProject::Save() { 
  String fname = GetFileName(); // empty if 1st or not supported
  if(fname.contains("_recover")) {
    int chs = taMisc::Choice("This appears to be a recover file that was saved during a previous crash -- you may not want to save to this file name", "Save to this _recover file", "Let me choose a new name", "Save to non-_recover version of this file");
    if(chs == 1) fname = "";	// this will prompt for name
    else if(chs == 2) {
      fname = fname.before("_recover") + fname.from(".",-1);
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
  }
  taRefN::unRefDone(flr);
  DataChanged(DCR_ITEM_UPDATED_ND);
  return rval;
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
  // note: inherited only forwards 'dirty' up the chain, not '!dirty'
  inherited::setDirty(value);
  m_dirty = value;
  if (!value) m_no_save = false;
}


void taProject::UpdateChangeLog() {
#ifdef TA_GUI
  version.step++;		// increment the step always
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
    if(!last_change_desc.empty()) nw_txt += "  " + last_change_desc + "\n";

    taDoc* doc = docs.FindName("ChangeLog");
    if(!doc) {
      doc = docs.NewEl(1);
      doc->name = "ChangeLog";
      doc->text = "<html>\n<head>ChangeLog</head>\n<body>\n<h1>ChangeLog</h1>\n<ul>\n";
      doc->text += nw_txt;
      doc->text += "</ul>\n</body>\n</html>\n";
    }
    else {
      String hdr = doc->text.through("<ul>\n");
      String trl = doc->text.after("<ul>\n");
      doc->text = hdr + nw_txt + trl;
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

void taProject::SaveRecoverFile() {
  String prfx;
  String sufx = ".proj";
  String recv = "_recover";
  if(file_name.empty()) {
    if(name.empty()) {
      prfx = GetTypeDef()->name;
    }
    else {
      prfx = name;
    }
  }
  else {
    if(file_name.contains(sufx)) {
      prfx = file_name.before(sufx, -1);
    }
    else {
      prfx = file_name;		// whatever
    }
  }
  if(prfx.contains(recv))
    prfx = prfx.through(recv, -1);
  else
    prfx += recv;
  int cnt = taMisc::GetUniqueFileNumber(0, prfx, sufx);
  String fnm = prfx + String(cnt) + sufx; // note: this is a full path!
  int acc = access(fnm, W_OK); 	// can we save this file?
  if(acc != 0) {
    fnm = taMisc::user_dir + PATH_SEP + taMisc::GetFileFmPath(fnm);
  }
  taFiler* flr = GetSaveFiler(fnm, _nilString, -1, _nilString);
  bool saved = false;
  if(flr->ostrm) {
    SaveRecoverFile_strm(*flr->ostrm);
    saved = true;
  }
  if(acc != 0) {
#ifdef DEBUG // NOTE: really only works on Linux, and is so marginal...
      cerr << "Error saving recover file in original location -- now saved in user directory: " << fnm << endl;
      taMisc::FlushConsole();
#endif
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
      cfnm.gsub((const char*)sufx, ".txt");
      QcssConsole* qcons = QcssConsole::getInstance();
      if(qcons)
	qcons->saveContents(cfnm);
    }
  }
#endif
}


//////////////////////////
//   Project_Group	//
//////////////////////////

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
  for(int i=prj_sz;i<leaves;i++) {
    taProject* prj = Leaf(i);
    tabMisc::DelayedFunCall_gui(prj,"PostLoadAutos");
    // do it delayed to allow everything to happen first
    //    prj->PostLoadAutos();
  }
  return rval;
}


//////////////////////////
//   taRootBaseAdapter	//
//////////////////////////

void taRootBaseAdapter::Startup_ProcessArgs() {
  taRootBase::Startup_ProcessArgs();
}

void taRootBaseAdapter::Startup_RunStartupScript() {
  taRootBase::Startup_RunStartupScript();
}

#ifdef DMEM_COMPILE
void taRootBaseAdapter::DMem_SubEventLoop() {
  taRootBase::DMem_SubEventLoop();
}
#endif // DMEM_COMPILE

//////////////////////////
//   taRoot		//
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
  taiMimeFactory_List::setInstance(&mime_factories);
  AddTemplates(); // note: ok that this will be called here, before subclass has finished its own
  AddDocs(); // note: ok that this will be called here, before subclass has finished its own
}

void taRootBase::CutLinks() {
  recent_paths.CutLinks();
  recent_files.CutLinks();
  objs.CutLinks();
  colorspecs.CutLinks();
  mime_factories.CutLinks();
  plugin_deps.CutLinks();
//TODO: we should save the plugin state!
  plugin_state.CutLinks();
  plugins.CutLinks();
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

#ifdef GPROF			// turn on for profiling
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
      // see if already listedbool		VerifyHasPlugins()
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

  info += "Copyright (c) 1995-2009, Regents of the University of Colorado,\n\
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
  taMisc::Choice(info, "Ok");
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
  taLeafItr i;
  taProject* pr;
  FOR_ITR_EL(taProject, pr, projects., i) {
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
    if(taMisc::gui_active && (console_type == taMisc::CT_GUI)) {  
      QcssConsole* con = QcssConsole::getInstance(NULL, cssMisc::TopShell);
      if(taMisc::log_console_out) {
	con->setStdLogfile("css_console_output.log");
      }
      else {
	con->setStdLogfile("");
      }
    }
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
// 	startup code


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
  // 	All the multi-threading stuff has standard default startup args
  
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
  // 	Server variables
  
  taMisc::AddArgName("--server", "Server");
  taMisc::AddArgNameDesc("Server", "\
 -- Run the app as a tcp server");

  taMisc::AddArgName("--port", "Port");
  taMisc::AddArgName("port=", "Port");
  taMisc::AddArgNameDesc("Port", "\
 -- Specifies the tcp port for server mode (def=5360");

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
  if(taMisc::CheckArgByName("GenDoc")) { // auto nogui by default
    taMisc::use_gui = false;
    cssMisc::init_interactive = false;
  }

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
    new QApplication(argc, (char**)argv); // accessed as qApp
    SIM::Coin3D::Quarter::Quarter::init();
//     SoQt::init(argc, (char**)argv, cssMisc::prompt.chars()); // creates a special Coin QApplication instance
    milestone |= (SM_QAPP_OBJ | SM_SOQT_INIT);
# else
    new QApplication(argc, (char**)argv); // accessed as qApp
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

bool isPluginDir(const String& path) {
//NOTE: this is a test that is supposed to confirm a dir is a plugin dir
  return QDir(path).exists();
}

bool isAppDir(const String& path, String* plugin_path = NULL) {
//NOTE: this is a test that is supposed to confirm a dir is an app dir
// our first version checks for the prog_lib folder
// If requested, we also check if there is a plugin folder, and set that
// thus leaving it valid for dev installs 
  QDir dir(path);
  bool rval = dir.exists("prog_lib");
  if (rval) {
    if (plugin_path) {
      if (dir.exists("plugins"))
        *plugin_path = path + PATH_SEP + "plugins";
    }
  }
#ifdef DEBUG // don't clutter with success, just failures
  else
    taMisc::Info("Did not find app_dir as:", path);
#endif
  return rval;
}


#ifndef TA_OS_WIN
const char* def_prefixes[] = {
  "/usr/local", "/usr", "/opt/local", "/opt"};
const int def_prefix_n = 4;
#endif
  
// hairy, modal, issue-prone -- we put in its own routine
bool taRootBase::Startup_InitTA_AppFolders() {
#ifndef TA_OS_WIN
  String prefix_dir; // empty unless we found app_dir under here
#endif
  
/* We search for app path in following order:
   1. app_dir command line switch (may require app_plugin_dir switch too)
   2. "in-place" development (this is either the same as
      or never conflicts with the installed production version)
   3. EMERGENTDIR (Windows) or EMERGENT_PREFIX_DIR (Unix) variable
   4. a platform-specific heuristic search
   NOTE: for "in-place" contexts, plugin dir is local, else look independently
*/
//WARNING: cannot use QCoreApplication::applicationDirPath() at this point because
// QCoreApplication has not been instantiated yet
  
  //note: this is not how Qt does it, but it seems windows follows normal rules
  // and passes the arg[0] as the full path to the executable, so we just get path
  QFileInfo fi(taMisc::args_raw.SafeEl(0));
  //note: argv[0] can contain a relative path, so we need to absolutize
  // but *don't* dereference links, because we typically want to use the 
  // link file, not the target, which for dev contexts may be buried somewhere
  String bin_dir = fi.absolutePath();
  
  String app_plugin_dir; // will get set for in-place contexts (Windows and dev)
  String app_dir = taMisc::FindArgByName("AppDir");
  if (app_dir.nonempty() && isAppDir(app_dir))
    goto have_app_dir;

#ifdef TA_OS_WIN
/*
  {app_dir}\bin\xxx (MSVS build) 
  {app_dir}\bin (normal release, nmake build)
*/
  // note: Qt docs say it returns the '/' version...
  bin_dir.gsub("/", "\\");
  if (bin_dir.contains("\\bin\\")) {
    app_dir = bin_dir.before("\\bin\\");
    if (isAppDir(app_dir)) goto have_app_dir;
  }
  app_dir = getenv("EMERGENTDIR");
  if (app_dir.nonempty() && isAppDir(app_dir)) {
    goto have_app_dir;
  }
  if (bin_dir.endsWith("\\bin")) {
    app_dir = bin_dir.at(0, bin_dir.length() - 4);
    if (isAppDir(app_dir)) goto have_app_dir;
  }
#else // Mac and Unix -- defaults
# if defined(TA_OS_MAC)
/* Note: for Mac, if the bin is in a bundle, then it will be a link
  to the actual file, so in this case, we dereference it
  {app_dir}/{appname.app}/Contents/MacOS (bundle in app root)
  {app_dir}/bin/{appname.app}/Contents/MacOS (bundle in app bin)
  {app_dir}/bin (typically non-gui only, since gui must run from bundle)
*/
  if (bin_dir.endsWith("/Contents/MacOS")) {
    bin_dir = fi.canonicalPath();
  }
  // seemingly not in a bundle, so try Unix defaults...
# endif // Mac
/*
  {app_dir}/build[{SUFF}]/bin (cmake development) TEST FIRST!
  {app_dir}/bin (legacy development)
  {prefix_dir}/bin (standard Unix deployment)
*/
  if (bin_dir.endsWith("/bin")) {
    if (bin_dir.contains("/build")) {
      app_dir = bin_dir.before("/build");
      if (isAppDir(app_dir, &app_plugin_dir)) goto have_app_dir;
    } else { 
      app_dir = bin_dir.at(0, bin_dir.length() - 4);
      if (isAppDir(app_dir, &app_plugin_dir)) goto have_app_dir;
    }
  }
  // positional heuristics of prefix_dir:
  // -2:env var; -1: default prefix implied by bin dir
  // else, try the defaults in order
  for (int i = -2; i < def_prefix_n; ++i) {
    if (i == -2) {
      prefix_dir = getenv("EMERGENT_PREFIX_DIR");
      if (prefix_dir.empty()) continue;
    } else if (i == -1) {
      if (bin_dir.endsWith("/bin"))
        prefix_dir =  bin_dir.before("/bin");
      else continue;
    } else {
      prefix_dir = def_prefixes[i];
    }
    app_dir = prefix_dir + "/share/" + taMisc::default_app_install_folder_name;
    if (isAppDir(app_dir))  goto have_app_dir;
  }
  prefix_dir = _nilString;

#endif // all modality

  // inability to find the app is fatal in 4.0.19
  taMisc::Error("The application install directory could not be found. Please see:\n"
    "http://grey.colorado.edu/emergent/index.php/User_Guide\n"
    "for instructions on setting command line switches and/or environment\n"
    "variables for non-standard installations of the application.\n");
  // use a Choice so console etc. doesn't disappear immediately, ex. on Windows
  taMisc::Choice("The application will now terminate.");
  return false;
  
have_app_dir:

  // initialize the key folders
  taMisc::app_dir = app_dir;
#ifdef TA_OS_WIN
  taMisc::app_dir.gsub("/", "\\"); // clean it up, so it never causes issues
#endif

/* We search for plugin path in following order:
   1. app_plugin_dir command line switch
   3. EMERGENT_PLUGIN_DIR variable
   2. previously established "in-place" location (only for Windows
      and dev contexts)
   4. Unix/Mac: {EMERGENT_PREFIX_DIR}/lib/Emergent (if env var set)
   4. a platform-specific heuristic search
   NOTE: for "in-place" contexts, plugin dir is local, else look independently
*/


  String app_plugin_dir_cmd = taMisc::FindArgByName("AppPluginDir");
  if (app_plugin_dir_cmd.nonempty()) {
    app_plugin_dir = app_plugin_dir_cmd;
    goto check_plugin_dir;
  }
  if (app_plugin_dir.nonempty()) goto check_plugin_dir;
  app_plugin_dir = getenv("EMERGENT_PLUGIN_DIR");
  if (app_plugin_dir.nonempty()) goto check_plugin_dir;
  
#ifdef TA_OS_WIN
  app_plugin_dir = app_dir + "\\plugins"; // better be!!!
#else
  // Unix and Mac
  // only got here because not in-place -- we first try the folder
  // that corresponds to the share prefix, otherwise we do the cascade
  // -1: default prefix previously discovered -- that would have used
  //   EMERGENT_PREFIX_DIR as its first default, so we don't recheck here
  // else, try the defaults in order
  for (int i = -1; i < def_prefix_n; ++i) {
    if (i < 0) {
      if (prefix_dir.empty()) continue;
    } else {
      prefix_dir = def_prefixes[i];
    }
    app_plugin_dir = prefix_dir + "/lib/" + taMisc::default_app_install_folder_name + "/plugins";
    if (isPluginDir(app_plugin_dir))  goto have_plugin_dir;
  }
  goto warn_no_plugin_dir;
#endif
check_plugin_dir:
  if (isPluginDir(app_plugin_dir)) goto have_plugin_dir;
warn_no_plugin_dir:
  taMisc::Error("Expected application plugin folder", 
    taMisc::app_plugin_dir, "does not exist! You should check your installation and/or create this folder, otherwise runtime errors may occur.");

have_plugin_dir:
  taMisc::app_plugin_dir = app_plugin_dir; 
  return true;
}

bool FileWithContentExists(const String& in1, const String& fname) {
  bool rval = false;
  fstream in2;

  int i1 = 0;
  in2.open(fname, ios::in | ios::binary);
  if (!in2.is_open()) goto exit1;
  char c1;  char c2;
  bool g1;  bool g2;
  while (true) {
    g1 = (i1 < in1.length()); // we have at least one more char
    g2 = in2.get(c2);
    if (!g1 && !g2) break; // same size, done 
    if (!(g1 && g2)) goto exit; // different sizes
    c1 = in1.elem(i1++);
    if (c1 != c2) goto exit;  // different content
  }
  rval = true; 

exit:
  in2.close();
exit1:
  return rval;
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
  dir.mkpath(taMisc::user_app_dir + PATH_SEP + "plugins");
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
  taMisc::Init_Hooks();	// client dlls register init hooks -- this calls them!
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
  
  taMisc::user_plugin_dir = taMisc::user_app_dir + PATH_SEP + "plugins";
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
//     	emergent_bitmap_height, emergent_bitmap_bits);
//    qApp->setWindowIcon(QIcon(*(taiM->icon_bitmap)));

    if(taMisc::gui_no_win)
      taMisc::gui_active = false;	// in effect, we start as use_gui but never get to gui_active -- everything is initialized but no windows are created
    else
      taMisc::gui_active = true;	// officially active!
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
 comment	Firebrick
 string		RosyBrown
 keyword	Purple
 builtin	Orchid (also preprocessor)
 function-name	Blue1
 variable-name	DarkGoldenrod
 type		ForestGreen
 constant	CadetBlue
 warning	Red1
*/

bool taRootBase::Startup_InitViewColors() {
  if(!taMisc::view_colors) {
    taMisc::view_colors = new ViewColor_List;
    taMisc::view_colors->BuildHashTable(100); // speed this one up
  }
  taMisc::view_colors->FindMakeViewColor("NotEnabled", "State: !isEnabled",
					 false, _nilString, true, "grey80");
  taMisc::view_colors->FindMakeViewColor("ThisInvalid", "State: item not valid according to CheckConfig",
					 false, _nilString, true, "red1");
  taMisc::view_colors->FindMakeViewColor("ChildInvalid", "State: child not valid according to CheckConfig",
					 false, _nilString, true, "orange1");
  taMisc::view_colors->FindMakeViewColor("ProgElNonStd", "State: program element is not standard",
					 false, _nilString, true, "yellow1");
  taMisc::view_colors->FindMakeViewColor("ProgElNewEl", "State: program element is newly added",
					 false, _nilString, true, "SpringGreen1");
  taMisc::view_colors->FindMakeViewColor("ProgElVerbose", "State: program element is verbose",
					 false, _nilString, true, "khaki");
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
  	
bool taRootBase::Startup_LoadPlugins() {
  if (!tabMisc::root) return false; // should be made
  if (!taMisc::use_plugins) return true;
  tabMisc::root->plugins.LoadPlugins();
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
    if(taMisc::dmem_nprocs > 1)	// nogui dmem is *ALWAYS* batch for all procs
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
  MainWindowViewer* vwr = MainWindowViewer::NewBrowser(tabMisc::root, NULL, true);
  // try to size fairly large to avoid scrollbars
  vwr->SetUserData("view_win_wd", 0.6f);
  float ht = 0.5f; // no console
//  iSize s(1024, 480); // no console  (note: values obtained empirically)
  if ((console_type == taMisc::CT_GUI) && (!(console_options & taMisc::CO_GUI_TRACKING))) {
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
    if(taMisc::log_console_out) {
      con->setStdLogfile("css_console_output.log");
    }
    if (console_options & taMisc::CO_GUI_TRACKING) {
      QMainWindow* cwin = new QMainWindow();
      cwin->setWindowTitle("css Console");
      cwin->setCentralWidget((QWidget*)con);
      cwin->resize((int)(.95 * taiM->scrn_s.w), (int)(.25 * taiM->scrn_s.h));
      cwin->move((int)(.025 * taiM->scrn_s.w), (int)(.7 * taiM->scrn_s.h));
      cwin->show();
      taMisc::console_win = cwin; // note: uses a guarded QPointer

      MainWindowViewer* db = (MainWindowViewer*)tabMisc::root->viewers[0];
      db->ViewWindow();		// make sure root guy is on top
    }
  }
#endif
  cssMisc::TopShell->StartupShellInit(cin, cout, console_type);

  return true;
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
  if(taMisc::CheckArgByName("Version")) {
    tabMisc::root->About();
  }
  if(taMisc::CheckArgByName("Help")) {
    taMisc::HelpMsg();
  }
  if(taMisc::CheckArgByName("GenDoc")) {
    taGenDoc::GenDoc(&(taMisc::types), 1); // 1 = regular mode
  }

  // just load the thing!?
  String proj_ld = taMisc::FindArgByName("Project");
  if(proj_ld.empty())
    proj_ld = taMisc::FindArgValContains(".proj");

  if(!proj_ld.empty()) {
    tabMisc::root->projects.Load(proj_ld);
  }

  // chain the next step -- this will hopefully happen *after* any post-loading
  // events triggered by the projects.load 
  QTimer::singleShot(0, root_adapter, SLOT(Startup_RunStartupScript()));

  return true;
}

bool taRootBase::Startup_RunStartupScript() {
  cssMisc::TopShell->RunStartupScript();

  bool startup_run = false;
  if(tabMisc::root->projects.size == 1) {
    taProject* prj = tabMisc::root->projects[0];
    startup_run = prj->programs.RunStartupProgs();
  }
  if(!cssMisc::init_interactive || (!taMisc::gui_active && startup_run)) taiMC_->Quit();

  return true;
}

bool taRootBase::Startup_Main(int& argc, const char* argv[], ta_void_fun ta_init_fun, 
			      TypeDef* root_typ) 
{
  ++in_init;
  root_type = root_typ;
#ifdef GPROF
  moncontrol(0);		// turn off at start
#endif
#ifdef SATURN_PROF
  // this actually seems bad: get a warning
  // initSaturn("");		// store in current wd
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
  if(taMisc::gui_active && (taMisc::dmem_proc == 0))	// only guy and don't have all the other nodes save
    instance()->Save();
  return true;
  
startup_failed:
  Cleanup_Main();
  return false;
}

///////////////////////////////////////////////////////////////////////////
//	Run & Cleanup

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
  extern void rl_cleanup_after_signal(void);
}

// todo: could partition these out into separate guys..  	
void taRootBase::Cleanup_Main() {
  // remove sig handler -- very nasty when baddies happen after this point
  if (milestone & SM_REG_SIG) {
    taMisc::Register_Cleanup(SIG_DFL); // replace back to using default
  }
  cssMisc::Shutdown();		// shut down css..
  if (milestone & SM_ROOT_CREATE)
    tabMisc::DeleteRoot();
  if (milestone & SM_TYPES_INIT)
    taMisc::types.RemoveAll();	// get rid of all the types before global dtor!

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
#ifdef DMEM_COMPILE
  if (milestone & SM_MPI_INIT)
    MPI_Finalize();
#endif

#ifndef TA_OS_WIN
  // only if using readline-based console, reset tty state
  if((console_type == taMisc::CT_NONE) && (taMisc::gui_active || cssMisc::init_interactive)) {
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
// 		Recover File Handler

//#ifndef TA_OS_WIN

#include <signal.h>
#include <memory.h>
#include <sstream>

// for saving a recovery file if program crashes, is killed, etc.
void taRootBase::SaveRecoverFileHandler(int err) {
  static bool has_crashed = false;
  signal(err, SIG_DFL);		// disable catcher

  if(has_crashed) {
    cerr << "Unable to save recover file (multiple errors)...sorry" << endl;
    exit(err);
  }
  has_crashed = true;		// to prevent recursive crashing..

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
    taiMisc::Cleanup(err);	// cleanup stuff in tai
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
      exit(err);		// we need to forcibly exit on this one
    }
    else {
      kill(getpid(), err);	// activate signal
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
