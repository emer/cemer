// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// pdplog.cc
#include "pdplog.h"
#include "process.h"
#include "sched_proc.h"
#include "pdpshell.h"
//#include "ta_qtgroup.h"
#include "ta_filer.h"

#ifdef TA_GUI
#include "t3viewer.h"
#include "pdplog_qtso.h"
#endif

#include <limits.h>
#include <float.h>


////////////////////////////////////////////////////////////////////////////////
// important notes on data_range, log_lines, and view_range:
// data_range reflects log_lines if logging to file, otherwise it is in lines
//   	in the data object
// data_range.min is >= 0, but data_range.max is not the _size_ of the max
// 	but the _line number_ of the max:  thus it is typically log_lines - 1
//	or data.MaxLength() - 1
// view_range is in the same units as data_range
// to translate either view or data_range into indicies in the data table (data)
// 	it is necessary to subtract data_range.min from the view_range
// 	to take into account the possiblity that the data in the table is only
// 	a portion of the data in the actual log file.
////////////////////////////////////////////////////////////////////////////////


//////////////////////////
// 	PDPLog		//
//////////////////////////

void PDPLog::Initialize() {
  log_file = taFiler_CreateInstance(".","*log*",false);
  taRefN::Ref(log_file);
  log_file->mode = taFiler::NO_AUTO;
  log_lines = 0;

  data.el_typ = &TA_float_Data;
  data_bufsz = 10000;		// default bufsz is 5000 lines
  data_shift = .20f;		// shift 20 percent
  record_proc_name = false;	// don't by default..
  data_range.min = 0;
  data_range.max = -1;
  cur_proc = NULL;
#ifdef TA_GUI
  geom.SetXYZ(4, 1, 3);
  //TODO: new ones should offset pos in viewers so they don't overlap
  pos.SetXYZ(0-geom.x, 0, 0);
#endif
}

void PDPLog::InitLinks() {
  taBase::Own(log_proc, this);
  taBase::Own(data, this);
  taBase::Own(data_range, this);
  taBase::Own(log_data, this);
  taBase::Own(display_labels, this);
#ifdef TA_GUI
  taBase::Own(pos, this);
  taBase::Own(geom, this);
#endif

  inherited::InitLinks();
  InitFile();
  log_file->Close();
}

void PDPLog::Destroy() {
  CutLinks();
  if (log_file) taRefN::unRefDone(log_file);
  log_file = NULL;
}

void PDPLog::CutLinks() {
  static bool in_repl = false;
  if(in_repl || (owner == NULL)) return; // already replacing or already dead
  cur_proc = NULL;
  RemoveAllUpdaters();
  // set any misc log pointers to NULL!
  Project* proj = GET_MY_OWNER(Project);
  if((proj != NULL) && !proj->deleting) {
    in_repl = true;
    taMisc::ReplaceAllPtrs(GetTypeDef(), (void*)this, NULL);
    in_repl = false;
  }
#ifdef TA_GUI
  geom.CutLinks();
  pos.CutLinks();
#endif
  display_labels.CutLinks();
  log_data.CutLinks();
  data_range.CutLinks();
  data.CutLinks();
  log_proc.CutLinks();
  inherited::CutLinks();
}

void PDPLog::Copy_(const PDPLog& cp) {
  if (log_file) *log_file = *(cp.log_file);
  log_lines = cp.log_lines;
  log_data = cp.log_data;

  data = cp.data;
  data_bufsz = cp.data_bufsz;
  data_shift = cp.data_shift;
  data_range = cp.data_range;
  record_proc_name = cp.record_proc_name;

  log_proc.BorrowUnique(cp.log_proc);	// this is a link group..
  cur_proc = cp.cur_proc;

  display_labels = cp.display_labels;
#ifdef TA_GUI
  pos = cp.pos;
  pos.x++; pos.y--; pos.z++; // outset it from last one
  geom = cp.geom;
#endif
}

void PDPLog::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  SyncLogViewUpdaters();

  //NOTE: in 4.0, UAE logic for viewers has been moved to the change handler in the viewer
  if(log_proc.size > 1)		// always record proc name if more than one..
    record_proc_name = true;
  if(log_file != NULL) {
    log_file->AutoOpen();
    log_file->mode = taFiler::NO_AUTO; // prevent this from happening again..
  }
  if(log_file->IsOpen()) {
    if(log_lines == 0) {
      GetFileLength();
      if(log_file->ostrm != NULL) {
	HeadToFile();
      }
    }
  }
  else {
    log_lines = 0;
    data_range.max = data.MaxLength()-1;
    data_range.min = 0;
  }
  if((log_proc.size > 0) && (GetName().empty() || GetName().contains(GetTypeDef()->name))) {
    SetName(((SchedProcess*)log_proc[0])->GetName() + "_" + GetTypeDef()->name);
  }
#ifdef TA_GUI
  taListItr i; LogView* lv;
  FOR_ITR_EL(LogView, lv, views., i) {
    lv->LogUpdateAfterEdit();
  }
#endif
}

void PDPLog::AddUpdater(SchedProcess* sp) {
  if(sp == NULL) return;
  bool was_linked = false;
  was_linked = sp->logs.LinkUnique(this);	// add this to the list on the process
  was_linked = (log_proc.LinkUnique(sp) || was_linked); // add the process to the list on this;
  if(was_linked) {
    SetName(sp->GetName() + "_" + GetTypeDef()->name);
    SyncLogViewUpdaters();
    GetHeaders();
  }
}

void PDPLog::DataViewAdding(taDataView* dv) {
  inherited::DataViewAdding(dv);
#ifdef TA_GUI
  if (dv->GetTypeDef()->InheritsFrom(TA_LogView)) {
    views.AddUnique((LogView*)dv);
  }
#endif
}

void PDPLog::DataViewRemoving(taDataView* dv) {
#ifdef TA_GUI
  views.Remove_(dv); // may not be in list -- ok
#endif
  inherited::DataViewRemoving(dv);
}

void PDPLog::RemoveUpdater(SchedProcess* sp) {
  if(sp == NULL) return;
  sp->logs.RemoveLeaf(this);	// remove this to the list on the process
  log_proc.RemoveLeaf(sp);     	// remove the process to the list on this;
  SyncLogViewUpdaters();
  GetHeaders();
}

void PDPLog::RemoveAllUpdaters() {
  int i;
  for(i=0; i<log_proc.size; i++) {
    ((SchedProcess*)log_proc.FastEl(i))->logs.RemoveLeaf(this);
  }
  log_proc.RemoveAll();
  SyncLogViewUpdaters();
}

void PDPLog::SyncLogViewUpdaters() {
#ifdef TA_GUI
/*TODO:fixup  LogView* lv;
  taListItr i;
  FOR_ITR_EL(LogView, lv, views()->, i) {
    lv->updaters.Reset();
    lv->updaters.BorrowUnique(log_proc);
  } */
#endif
}

int PDPLog::Dump_Save_Value(ostream& strm, TAPtr par, int indent) {
  if(log_file->IsOpen())
    log_file->mode = taFiler::APPEND; // auto-open in append mode..
  else
    log_file->mode = taFiler::NO_AUTO;
  return inherited::Dump_Save_Value(strm, par, indent);
}

void PDPLog::InitFile() {
  log_file->Init(".","*log*",false);
  log_file->fname = "";
}
#ifdef TA_GUI
LogView* PDPLog::NewLogView() {
  LogView* rval = (LogView*)taBase::MakeToken(views.el_typ);
  return rval;
}

void PDPLog::ShowInViewer(T3DataViewer* vwr)
{
  //TODO: we really need a wizard to pop up here, to let user configure before rendering
  if (!vwr) { // show in a new viewer
    Project* prj = GET_MY_OWNER(Project);
    if (!prj) return;
    vwr = prj->NewViewer();
    if (!vwr) return;
    LogView* lv = NewLogView();
    if (!lv) return;
    this->AddDataView(lv);
    vwr->AddView(lv);
    lv->BuildAll();
    vwr->ViewWindow();
  } else { // show in an existing viewer
    // check if already viewing this log there, warn user
    T3DataView* dv;
    for (int i = 0; i < vwr->root_view.children.size; ++i) {
      dv = vwr->root_view.children[i];
      if (dv->GetTypeDef()->InheritsFrom(TA_LogView)) {
        LogView* lv = (LogView*)dv;
        if (lv->log() == this) {
          if (taMisc::Choice("This log is already shown in that viewer -- are you sure you"
              " want to show it there again?", "&Ok", "&Cancel") != 0) return;
          break;
        }
      }
    }
    LogView* lv = NewLogView();
    if (!lv) return;
    this->AddDataView(lv);
    vwr->AddView(lv);
    lv->BuildAll();
    vwr->Render();
  }
}
#endif
void PDPLog::SetSaveFile(const char* nm, bool no_dlg) {
  log_file->SaveAs(nm, no_dlg);
  GetFileLength();
  HeadToFile();
//obs  if(taMisc::gui_active)
//    SetWinName();
}

void PDPLog::SetAppendFile(const char* nm, bool no_dlg) {
  log_file->Append(nm, no_dlg);
  GetFileLength();
  HeadToFile();
//obs  if(taMisc::gui_active)
//    SetWinName();
}

void PDPLog::LoadFile(const char* nm, bool no_dlg) {
  StructUpdate(true);
  log_file->Open(nm, no_dlg);
  GetFileLength();
  ReadNewLogFile();
  StructUpdate(false);
//obs  if(taMisc::gui_active)
//    SetWinName();
}

void PDPLog::CloseFile() {
  log_file->Close();
  log_lines = 0;
//obs  if(taMisc::gui_active)
//    SetWinName();
}


//////////////////////////
//	Headers 	//
//////////////////////////

void PDPLog::NewHead(LogData& ld, SchedProcess* sproc) {
  display_labels.EnforceSize(ld.items.size);

  // null is a sign to regenerate regardless
  if((sproc != NULL) && (cur_proc == sproc) && (ld.items.size == data.leaves))
    return;

  HeadToBuffer(ld);
  LogDataFromBuffer();		// get the log data corresponding to the buffer
  UpdateViewHeaders();		// tells views that header is updated

  if((sproc != NULL) && (sproc->network != NULL)) {	// only send to log if its real
    cur_proc = sproc;

#ifdef DMEM_COMPILE
    int netsz = 0; MPI_Comm_size(sproc->network->dmem_share_units.comm, &netsz);
    if(taMisc::dmem_proc % netsz == 0)
      HeadToFile();		// use buffer version: has more accurate state info
#else
    HeadToFile();
#endif
  }
}

void PDPLog::UpdateViewHeaders() {
#ifdef TA_GUI
  LogView* lv;
  taDataLinkItr i;
  FOR_DLC_EL_OF_TYPE(LogView, lv, data_link(), i) {
    lv->NewHead();
  }
#endif
}

void PDPLog::HeadToBuffer(LogData& ld) {
  data.StructUpdate(true);
  data.SetCols(ld);
  data.AllocRows(128); // avoid a lot of unnecessary data copying by starting w/ reasonable size
  data.StructUpdate(false);
}

void PDPLog::HeadToLogFile(LogData& ld) { // this function is deprecated in favor of HeadToFile
  if(!log_file->IsOpen() || (log_file->ostrm == NULL))
    return;

  ostream& strm = *(log_file->ostrm);

  if(record_proc_name && (cur_proc != NULL))
    strm << cur_proc->name;
  strm << "_H:\t";
  int i;
  for(i=0; i < ld.items.size; i++) {
    DataItem* it = ld.items.FastEl(i);
    String hdnm = it->name;
    if((display_labels.size > i) && !display_labels[i].empty())
      hdnm = display_labels[i];
    if(it->HasDispOption(" NARROW,"))
      LogColumn(strm, hdnm, 1); // indicates one column
    else
      LogColumn(strm, hdnm, 2); // default is 2 columns
  }
  strm << "\n";
  strm.flush();
}

DataItem* PDPLog::DataItemFromDataArray(DataArray_impl* da) {
  DataItem* it = new DataItem();
  it->name = da->name;
  it->disp_opts = da->disp_opts;
  if(da->InheritsFrom(TA_String_Data))
    it->is_string = true;
  log_data.items.Add(it);
  return it;
}

void PDPLog::LogDataFromDataTable(DataTable* dt, int st_idx) {
  int i;
  for(i=st_idx; i < dt->size; i++) {
    DataArray_impl* da = dt->FastEl(i);
    DataItemFromDataArray(da);
  }
  if(dt->gp.size > 0) {
    for(i=0; i<dt->gp.size; i++) {
      DataTable* ndt = (DataTable*)dt->FastGp(i);
      if(ndt->size > 0) {
	DataArray_impl* da = ndt->FastEl(0);
	DataItem* it = DataItemFromDataArray(da);
	it->vec_n = ndt->size;
	LogDataFromDataTable(ndt, 1); // start from 1 since 0 was already done..
      }
    }
  }
}

void PDPLog::LogDataFromBuffer() {
  log_data.Reset();
  LogDataFromDataTable(&data);
  log_data.InitBlankData();
}

//////////////////////////
//	  Data 		//
//////////////////////////


void PDPLog::NewData(LogData& ld, SchedProcess* sproc) {
  NewHead(ld, sproc);		// see if header info is new..

  // buffer always reflects latest contents (views can scroll within)
  if(log_file->IsOpen() && (data_range.max < log_lines -1))
    Buffer_FF();

  DataToBuffer(ld);

  if((sproc != NULL) && (sproc->network != NULL)) {
#ifdef DMEM_COMPILE
    // only write to log for first proc of each network processing group
    int netsz = 0; MPI_Comm_size(sproc->network->dmem_share_units.comm, &netsz);
    if(taMisc::dmem_proc % netsz == 0)
      DataToLogFile(ld);
#else
    DataToLogFile(ld);
#endif
  }

  int max_lns = data.MaxLength() -1; // get new size
  if (max_lns < data_range.Range()) { // must have shifted buffers..
    if (log_file->IsOpen()) {
      data_range.max++;
      data_range.min = data_range.max - max_lns;
      data_range.MinGT(0);
    } else {
      data_range.min = 0;
      data_range.max = max_lns;
    }
#ifdef TA_GUI
    if(taMisc::gui_active) {
      LogView* lv;
      taDataLinkItr i;
      FOR_DLC_EL_OF_TYPE(LogView, lv, data_link(), i) {
	lv->View_FF();		// catch the views up with the new range
      }
    }
#endif
  } else {
    data_range.max++;
#ifdef TA_GUI
    LogView* lv;
    taDataLinkItr i;
    FOR_DLC_EL_OF_TYPE(LogView, lv, data_link(), i) {
      lv->NewData();		// just update new line
    }
#endif
  }
}

void PDPLog::DataToBuffer(LogData& ld) {
  data.DataUpdate(true);
  // if buffer will overflow, shift data
  if (data.rows >= data_bufsz) {
    int shift = (int)(data_bufsz * data_shift);
    data.ShiftUp(shift);
    //note: data shift is detected in NewData() and data_range fixed up there
  }
  data.AddRow(ld);
  data.DataUpdate(false);
}

void PDPLog::DataToLogFile(LogData&) {
  if(!log_file->IsOpen() || (log_file->ostrm == NULL))
    return;

  ostream& strm = *(log_file->ostrm);

  if(record_proc_name && (cur_proc != NULL))
    strm << cur_proc->name;
  strm << "_D:\t";

  taLeafItr i;
  DataArray_impl* da;
  FOR_ITR_EL(DataArray_impl, da, data., i) {
    if(!da->save_to_file) continue;
    String el;
    taArray_base* ar = da->AR();
    if((ar != NULL) && (ar->size > 0))
      el = ar->FastElAsStr(ar->size-1);
    else
      el = "n/a";

    if(da->HasDispOption(" NARROW,"))
      LogColumn(strm, el, 1);
    else
      LogColumn(strm, el, 2);
  }
  strm << "\n";
  strm.flush();
  log_lines++;
}

void PDPLog::GetHeaders(bool keep_display_settings) {
  display_labels.Reset();	// these get re-generated from the viewspecs..
  data.Reset();		// clear out old data...

#ifdef TA_GUI
  LogView* lv;
  taDataLinkItr vi;
  if(!keep_display_settings) {
    FOR_DLC_EL_OF_TYPE(LogView, lv, data_link(), vi) {
      if(lv->viewspec != NULL)
	lv->viewspec->Reset();	// clear all viewspecs too
      lv->view_range.min =0; lv->view_range.max = -1;
      lv->Render();
    }
  }
#endif

  taLeafItr i;
  SchedProcess* sproc;
  FOR_ITR_EL(SchedProcess, sproc, log_proc., i) {
    sproc->GenLogData();
    NewHead(sproc->log_data, NULL); // this is a "fake" head, not from a log directly
    HeadToFile();
  }

  // might need to manually rebuild if sizes did not change..
#ifdef TA_GUI
  if (keep_display_settings) {
    FOR_DLC_EL_OF_TYPE(LogView, lv, data_link(), vi) {
      if(lv->viewspec == NULL) continue;
      if((lv->viewspec->size == data.size) && (lv->viewspec->leaves == data.leaves)
	 && (lv->viewspec->gp.size == data.gp.size)) {
	lv->viewspec->ReBuildFromDataTable();
	lv->Render();
      }
    }
  }
#endif
}

ostream& PDPLog::LogColumn(ostream& strm, String& str, int tabs) {
  strm << str;
  if(tabs == 2) {		// only 2 options (1 or 2)
    if(str.length() < 8)
      strm << "\t\t";
    else if(str.length() < 16)
      strm << "\t";
    else
      strm << "\t";		// always tab delimited..
  }
  else {
    if(str.length() < 8)
      strm << "\t";
    else
      strm << "\t";
  }
  return strm;
}


int PDPLog::virt_lines() {
  if (log_lines > 0)
    return log_lines;
  else
    return data_range.Count();
}

//////////////////////////
// 	Buffers		//
//////////////////////////

// checks for comments and headers,
// 0 = comment, 1 = header, 2 = data

int PDPLog::LogLineType(char* lnc) {
  String ln = String(lnc);

  // skip leading spaces if any
  while(ln.firstchar() == ' ') {
    ln = ln.after(' ');
  }
  if(ln.empty())  return 0;

  if((ln.firstchar() == '#') || (ln.before(2) == "//")) {
    return 0;
  }

  String id = ln.before('\t');
  if(id .empty()) return 0;

  String id_sufx = id.after('_', -1); // find after last underbar
  if(id_sufx == "H:") return 1;
  if(id_sufx == "D:") return 2;
  return 2;			// assume its data
}

// reads the file up to just before the dataline, strm must be just opened
// returns the maximum dataline number it could read

int PDPLog::FileScanTo(istream& strm, int log_ln) {
  if(log_ln <= 0) return 0;	// ready to read line 0

  int count = 0;
  String templine;

  while(readline(strm,templine)) {
    if(LogLineType(templine) != 2) // only count data lines
      continue;
    count++;
    if (count >= log_ln) break;
  }
  return count;		// the number of the line were ready to read
}


int PDPLog::GetFileLength() {
  int rval = 0;
  taFiler* gf = NULL;		// use a getfile for compressed reads..
  if(!log_file->IsOpen()) return 0;

  gf = taFiler_CreateInstance();		// use a getfile for compressed reads..
  taRefN::Ref(gf);
  gf->fname = log_file->fname;
  istream* strm = gf->open_read();
  if((strm == NULL) || strm->bad())
    goto exit;

  rval = FileScanTo(*strm, INT_MAX); // read as far as we can..
  gf->Close();

  log_lines = rval;
  data_range.MaxLT(log_lines -1);
exit:
  if (gf) taRefN::unRefDone(gf);
  return rval;
}

void PDPLog::HeadFromLogFile(String& hdln) { // argument is mutable..
  int itm_cnt = 0;
  if(hdln.lastchar() != '\t') hdln += "\t";
  while(hdln.contains('\t')) {
    String f = hdln.before('\t');
    if((!f.empty()) && !f.contains("_H:")) {
      DataItem* it;
      if(itm_cnt >= log_data.items.size) {
	it = new DataItem();
	log_data.items.Add(it);
      }
      else
	it = log_data.items.FastEl(itm_cnt);
      if(f != it->name) {	// names don't match
	it->Initialize();	// reset info on
	it->name = f;
	if(f[0] == '$') {
	  it->is_string = true;
	  if(f[1] == '<') {
	    String vec_n = f.after('<');
	    vec_n = vec_n.before('>');
	    it->vec_n = (int)vec_n;
	  }
	}
	else if(f[0] == '|') {
	  it->AddDispOption("NARROW");
	}
	else if(f[0] == '<') {
	  String vec_n = f.after('<');
	  vec_n = vec_n.before('>');
	  it->vec_n = (int)vec_n;
	}
      }
      itm_cnt++;
    }
    hdln=hdln.after('\t');
  }
  if(log_data.indexSize() != log_data.items.size)
    log_data.InitBlankData();
  NewHead(log_data, NULL);
}


void PDPLog::DataFromLogFile(String& dtln) {
  // log data heads are assumed to be current..
  int cnt = 0;
  if(dtln.lastchar() != '\t') dtln += "\t";
  if(log_data.indexSize() != log_data.items.size) { // make sure these line up..
    log_data.InitBlankData();			   // if not, reset data
  }
  while(dtln.contains('\t') && (cnt < log_data.items.size)) {
    String f = dtln.before('\t');
    if((!f.empty()) && !f.contains("_D:")) {
      if(log_data.IsString(cnt))
	log_data.GetString(cnt) = f; // these don't existe..
      else
	log_data.GetFloat(cnt) = atof(f);
      cnt++;
    }
    dtln=dtln.after('\t');
  }
  DataToBuffer(log_data);
}


void PDPLog::LogFileToBuffer() {
  int act_lines = 0;
  int lno = 0;
  String templn;
  int rval;
  taFiler* gf = NULL;
  if(!log_file->IsOpen()) return;

  data.ResetData();		// clear out the arrays for new data
  data_range.MinGT(0);		// min must be greater than 0

  gf = taFiler_CreateInstance();		// use a getfile for compressed reads..
  taRefN::Ref(gf);
  gf->fname = log_file->fname;
  istream* strm = gf->open_read();

  if((strm == NULL) || strm->bad())
    goto exit;

  act_lines = FileScanTo(*strm, (int)data_range.min);
  if(act_lines != data_range.min) {
    log_lines = act_lines;	// we adjust to the actual size of the file..
    data_range.MaxLT(log_lines-1);
    data_range.min = data_range.max - data_bufsz + 1;
    data_range.MinGT(0);
    if(log_lines == 0) {
      gf->Close();
      goto exit;
    }
  }

  lno = data_range.min;
  while((rval = readline(*strm,templn)) && (lno <= data_range.max)) {
    int ltyp = LogLineType(templn);
    if(ltyp == 0)
      continue;
    if(ltyp == 1) {
      HeadFromLogFile(templn);
    }
    if(ltyp == 2) {
      DataFromLogFile(templn);
      lno++;
    }
  }
  if(rval == 0) {
    log_lines = lno;
    data_range.MaxLT(lno -1 );	// reset max based on what we actually got
  }
  gf->Close();
exit:
  if (gf) taRefN::unRefDone(gf);
}

void PDPLog::HeadToFile() {	// based on buffer
  if(!log_file->IsOpen() || (log_file->ostrm == NULL))
    return;

  ostream& strm = *(log_file->ostrm);

  if(record_proc_name && (cur_proc != NULL))
    strm << cur_proc->name;
  strm << "_H:\t";
  int i;
  for(i=0; i < data.leaves; i++) {
    DataArray_impl* da = data.Leaf(i);
    if(!da->save_to_file) continue;
    String hdnm = da->name;
    if((display_labels.size > i) && !display_labels[i].empty())
      hdnm = display_labels[i];
    int wdth = 2;
    if(da->HasDispOption(" NARROW,"))
      wdth = 1;
    LogColumn(strm, hdnm, wdth);
  }
  strm << "\n";
  strm.flush();
}

void PDPLog::BufferToFile(const char* nm, bool no_dlg) {
  bool new_open = false;
  if(!log_file->IsOpen() || (log_file->ostrm == NULL) || (nm != NULL)) {
    SetSaveFile(nm, no_dlg);
    if(log_file->ostrm == NULL) return;
    new_open = true;
  }

  ostream& strm = *(log_file->ostrm);

  int max_lns = data.MaxLength()-1;
  int ln;
  for(ln=0; ln <= max_lns; ln++) {
    if(record_proc_name && (cur_proc != NULL))
      strm << cur_proc->name;
    strm << "_D:\t";
    int from_end = max_lns - ln ;
    taLeafItr i;
    DataArray_impl* da;
    FOR_ITR_EL(DataArray_impl, da, data., i) {
      if(!da->save_to_file) continue;
      String el;
      taArray_base* ar = da->AR();
      if((ar != NULL) && (ar->size > from_end) && (from_end >= 0))
	el = ar->FastElAsStr(ar->size-1-from_end);
      else
	el = "n/a";

      if(da->HasDispOption(" NARROW,"))
	LogColumn(strm, el, 1);
      else
	LogColumn(strm, el, 2);
    }
    strm << "\n";
  }
  strm.flush();
  if(new_open) {
    CloseFile();
  }
}

void PDPLog::Buffer_F() {
  if(!log_file->IsOpen()) return;
  int shft = (int)((float)data_bufsz * data_shift);
  data_range.max += shft;
  data_range.MaxLT(log_lines - 1);
  data_range.min = data_range.max - data_bufsz + 1;
  data_range.MinGT(0);
  LogFileToBuffer();
}

void PDPLog::Buffer_FF() {
  if(!log_file->IsOpen()) return;
  data_range.max = log_lines - 1;
  data_range.min = data_range.max - data_bufsz + 1;
  data_range.MinGT(0);
  LogFileToBuffer();
}

void PDPLog::Buffer_R() {
  if(!log_file->IsOpen()) return;
  int shft = (int)((float)data_bufsz * data_shift);
  data_range.min -= shft;
  data_range.MinGT(0);
  data_range.max = data_range.min + data_bufsz - 1;
  data_range.MaxLT(log_lines - 1);
  LogFileToBuffer();
}

void PDPLog::Buffer_FR() {
  if(!log_file->IsOpen()) return;
  data_range.min = 0;
  data_range.max = data_range.min + data_bufsz -1;
  data_range.MaxLT(log_lines - 1);
  LogFileToBuffer();
}

void PDPLog::Buffer_SeekForView(const MinMaxInt& view_range) {
  if (!log_file->IsOpen()) return;
  if (data_range.RangeTestEq(view_range.min) && data_range.RangeTestEq(view_range.max))
    return; // view range is entirely contained in data_range, so no change needed
  data_range.min = view_range.min;
  data_range.max = data_range.min + data_bufsz -1;
  data_range.MaxLT(log_lines - 1);
  LogFileToBuffer();
}

void PDPLog::ReadNewLogFile() {
  Buffer_FF();		// go to end
#ifdef TA_GUI
  if(!taMisc::gui_active) return;
  LogView* lv;
  taDataLinkItr i;
  FOR_DLC_EL_OF_TYPE(LogView, lv, data_link(), i) {
    lv->View_FF();		// same for all the views
  }
#endif
}

void PDPLog::Clear() {
  data.ResetData();
  data_range.min = 0; data_range.max = -1;
#ifdef TA_GUI
  if(!taMisc::gui_active) return;
  LogView* lv;
  taDataLinkItr i;
  FOR_DLC_EL_OF_TYPE(LogView, lv, data_link(), i)
    lv->Log_Clear();
#endif
}

void PDPLog::ViewAllData() {
  if (data.leaves == 0) return;
  data_range.min = 0;
  data_range.max = data.rows - 1;
#ifdef TA_GUI
  LogView* lv;
  taDataLinkItr i;
  FOR_DLC_EL_OF_TYPE(LogView, lv, data_link(), i) {
    lv->viewspec->Reset();
    lv->Render();
  }
#endif
}


//////////////////////////
// 	PDPLog_MGroup 	//
//////////////////////////

bool PDPLog_MGroup::nw_itm_def_arg = false;

PDPLog* PDPLog_MGroup::FindMakeLog(const char* nm, TypeDef* td, bool& nw_itm) {
  PDPLog* log = NULL;
  nw_itm = false;
  if (nm != NULL)
    log = SafeEl(Find(nm));
  else
    log = (PDPLog*)Find(td);
  if (log == NULL) {
    log = (PDPLog*)New(1, td);
    if (nm != NULL)
      log->SetName(nm);
    nw_itm = true;
  }
  return log;
}

//////////////////////////
// 	NetLog 	//
//////////////////////////

void NetLog::SetNetwork(Network* net) {
#ifdef TA_GUI
  NetLogView* lv;
  taDataLinkItr i;
  FOR_DLC_EL_OF_TYPE(NetLogView, lv, data_link(), i)
    lv->SetNetwork(net);
#endif
}

