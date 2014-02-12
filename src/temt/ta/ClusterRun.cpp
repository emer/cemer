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

#include "ClusterRun.h"
#include <ClusterManager>
#include <taSigLinkItr>
#include <iDataTableEditor>
#include <iPanelOfDataTable>
#include <iDataTableView>
#include <taDataProc>
#include <DataTable_Group>
#include <taProject>
#include <SubversionClient>
#include <iSubversionBrowser>

#include <taSigLinkItr>
#include <iPanelSet>

#include <taMisc>

#include <QRegExp>
#include <QDir>

TA_BASEFUNS_CTORS_DEFN(ClusterRun);

String ClusterRun::timestamp_fmt = "yyyy_MM_dd_hh_mm_ss";

ClusterRunRef ClusterRun::wait_proc_updt;
int ClusterRun::wait_proc_trg_rev = -1;
taDateTime ClusterRun::wait_proc_start;
taDateTime ClusterRun::wait_proc_last_updt;

void ClusterRun::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ClusterRun);
  FormatTables();
}

void ClusterRun::Initialize() {
  cur_svn_rev = -1;
  ram_gb = 0;
  n_threads = 1;  // taMisc::thread_defaults.n_threads
  use_mpi = false;
  mpi_nodes = 10;
  parallel_batch = false;
  pb_batches = 10;
  pb_nodes = 0;
  nowin_x = false;
  m_cm = 0;
  svn_other = NULL;
}

void ClusterRun::Destroy() {
  delete m_cm; m_cm = 0;
  if(svn_other) delete svn_other;
  svn_other = NULL;
}

void ClusterRun::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(svn_repo.nonempty()) {
    Variant rep = taMisc::svn_repos.GetVal(svn_repo);
    if(rep.isNull()) {
      // this warning is kinda annoying
      // taMisc::Warning("ClusterRun: svn_repo:", svn_repo,
      //                 "not found in list of registered svn repositories in Preferences / Options");
    }
    else {
      repo_url = rep.toString();
    }
  }
  if(taMisc::is_loading) {
    if(jobs_submit.cols() > jobs_running.cols() ||
       jobs_submitted.cols() > jobs_running.cols()) {
      jobs_submit.Reset();          // get rid of any weirdness from prior bug
      jobs_submitted.Reset(); 
      FormatTables();
    }
  }
}

bool ClusterRun::initClusterManager(bool check_prefs) {
  if(check_prefs) {
    if(!ClusterManager::CheckPrefs())
      return false;
  }
  if(!m_cm)
    m_cm = new ClusterManager(*this);
  else
    m_cm->Init();
  return true;
}

void ClusterRun::NewSearchAlgo(TypeDef *type) {
  search_algos.New(1, type);
}

void ClusterRun::Run() {
  if(!initClusterManager())
    return;
  FormatTables();               // ensure tables are formatted properly
  jobs_submit.ResetData();      // clear the submission table
  bool prompt_user = true;      // always prompt the user on a new run.
  if (m_cm->BeginSearch(prompt_user)) {
    // Get revisions of the committed project and jobs_submit.dat files.
    String wc_proj = m_cm->GetWcProjFilename();
    String wc_submit = m_cm->GetWcSubmitFilename();
    if (!wc_proj.empty() && !wc_submit.empty()) {
      int model_rev = m_cm->GetLastChangedRevision(wc_proj);
      int submit_rev = m_cm->GetLastChangedRevision(wc_submit);

      // Put those revisions into the datatable just committed.
      // (There's no way to put them in *before* committing.)
      for (int row = 0; row < jobs_submit.rows; ++row) {
        jobs_submit.SetValColName(submit_rev, "submit_svn", row);
      }
      cur_svn_rev = submit_rev;
      // move them over to submitted now!
      taDataProc::AppendRows(&jobs_submitted, &jobs_submit);
      AutoUpdateMe();
    }
  }
}

bool ClusterRun::Update() {
  if(!initClusterManager())
    return false;
  FormatTables();            // ensure data tables are formatted properly!

  // Update the working copy and load the running/done tables.
  // save current selection information and restore at end
  int st_row_done, end_row_done;
  bool has_sel_done = SelectedRows(jobs_done, st_row_done, end_row_done);
  int st_row_archive, end_row_archive;
  bool has_sel_archive = SelectedRows(jobs_archive, st_row_archive, end_row_archive);

  bool has_updates = m_cm->UpdateTables();
  cur_svn_rev = m_cm->GetCurSvnRev();
  SortClusterInfoTable();
  jobs_done.Sort("tag", true);  // also sort jobs done by tag
  jobs_archive.Sort("tag", true);  // also sort jobs done by tag

  if(has_sel_done && st_row_done >= 0 && end_row_done >= st_row_done) {
    SelectRows(jobs_done, st_row_done, end_row_done);
  }
  if(has_sel_archive && st_row_archive >= 0 && end_row_archive >= st_row_archive) {
    SelectRows(jobs_archive, st_row_archive, end_row_archive);
  }

  if (has_updates && cur_search_algo) {
    cur_search_algo->ProcessResults();
  }
  SigEmitUpdated();
  return has_updates;
}

void ClusterRun::SortClusterInfoTable() {
  String usrname = m_cm->getUsername();
  if(usrname.nonempty()) {
    for(int i=0; i<cluster_info.rows; i++) {
      String usr = cluster_info.GetValAsString("user", i);
      if(usr.contains(usrname) || usrname.contains(usr)) {
        String state = cluster_info.GetValAsString("state", i);
        state = "<usr> " + state;
        cluster_info.SetValAsString(state, "state", i);
      }
    }
  }
  cluster_info.Sort("state", true);
}

void ClusterRun::UpdtRunning() {
  if(!initClusterManager())
    return;
  
  jobs_submit.ResetData();
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("UPDTRUN", "status", dst_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
  // Commit the table.
  m_cm->CommitJobSubmissionTable();
  // todo: maybe don't update?
  AutoUpdateMe();
}

void ClusterRun::Cont() {
  if(!initClusterManager())
    return;

  // Create the next batch of jobs.
  if (cur_search_algo && cur_search_algo->CreateJobs()) {
    // Commit the table to submit the jobs.
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
}

void ClusterRun::Kill() {
  if(!initClusterManager())
    return;

  // Get the (inclusive) range of rows to kill.
  int st_row, end_row;
  if (SelectedRows(jobs_running, st_row, end_row)) {
    // Populate the jobs_submit table with CANCEL requests for the selected jobs.
    jobs_submit.ResetData();
    for (int row = st_row; row <= end_row; ++row) {
      CancelJob(row);
    }

    // Commit the table.
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
  else {
    taMisc::Warning("No rows selected -- no jobs were killed");
  }
}

void ClusterRun::GetData() {
  if(!initClusterManager())
    return;

  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (SelectedRows(jobs_running, st_row, end_row)) {
    jobs_submit.ResetData();
    for (int row = st_row; row <= end_row; ++row) {
      SubmitGetData(jobs_running, row); 
    }
    // Commit the table.
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe(false);        // keep selected so we can do Import
  }
  else if (SelectedRows(jobs_done, st_row, end_row)) {
    jobs_submit.ResetData();
    for (int row = st_row; row <= end_row; ++row) {
      SubmitGetData(jobs_done, row); 
    }
    // Commit the table.
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe(false);
  }
  else {
    taMisc::Warning("No rows selected -- no data fetched");
  }
}

void ClusterRun::ImportData(bool remove_existing) {
  if(!initClusterManager())
    return;
  // note: can't call Update here because it unselects the rows in jobs_ tables!

  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return;
  DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("ClusterRun");
  dgp->save_tables = false;     // don't save -- prevents project bloat
  if(remove_existing) {
    dgp->Reset();
  }
  int st_row, end_row;
  if (SelectedRows(jobs_running, st_row, end_row)) {
    for (int row = st_row; row <= end_row; ++row) {
      ImportData_impl(dgp, jobs_running, row); 
    }
  }
  else if (SelectedRows(jobs_done, st_row, end_row)) {
    for (int row = st_row; row <= end_row; ++row) {
      ImportData_impl(dgp, jobs_done, row); 
    }
  }
  else if (SelectedRows(jobs_archive, st_row, end_row)) {
    for (int row = st_row; row <= end_row; ++row) {
      ImportData_impl(dgp, jobs_archive, row); 
    }
  }
  else if (SelectedRows(file_list, st_row, end_row)) {
    for (int row = st_row; row <= end_row; ++row) {
      ImportData_impl(dgp, file_list, row); 
    }
  }
  else {
    taMisc::Warning("No rows selected -- no data fetched");
  }
  ClearAllSelections();       // done
}

void ClusterRun::ImportData_impl(DataTable_Group* dgp, const DataTable& table, int row) {
  String tag = table.GetValAsString("tag", row);
  if(TestWarning(tag.empty(), "ImportData", "tag is empty for row:", String(row),
                 "in table", table.name))
    return;
  String dat_files;
  String params;
  String notes;
  if(table.name == "file_list") {
    dat_files = table.GetValAsString("file_name", row);
    if(!dat_files.contains(".dat")) return; // not a dat file!
    int lkup = jobs_done.FindVal(tag, "tag");
    if(lkup >= 0) {
      params = jobs_done.GetValAsString("params", lkup);
      notes = jobs_done.GetValAsString("notes", lkup);
    }
    else {
      lkup = jobs_running.FindVal(tag, "tag");
      if(lkup >= 0) {
        params = jobs_running.GetValAsString("params", lkup);
        notes = jobs_running.GetValAsString("notes", lkup);
      }
      else {
        lkup = jobs_archive.FindVal(tag, "tag");
        if(lkup >= 0) {
          params = jobs_archive.GetValAsString("params", lkup);
          notes = jobs_archive.GetValAsString("notes", lkup);
        } 
      }
    }
  }
  else {
    dat_files = table.GetValAsString("dat_files", row);
    params = table.GetValAsString("params", row);
    notes = table.GetValAsString("notes", row);
  }
  if(TestWarning(dat_files.empty(), "ImportData", "dat_files is empty for tag:", tag))
    return;
  String tag_svn = tag.before("_");
  String tag_job = tag.after("_");
  String res_path = m_cm->GetWcResultsPath();
  String_Array files;
  files.FmDelimString(dat_files, " ");
  for(int i=0; i< files.size; i++) {
    String fl = files[i];
    String dnm = fl.before(".dat", -1);
    dnm = taMisc::StringCVar(dnm);
    DataTable* dat = dgp->FindName(dnm);
    if(!dat) {
      dat = dgp->NewEl(1);
      dat->name = dnm;
      dat->ClearDataFlag(DataTable::SAVE_ROWS); // don't save these by default!!
    }
    dat->LoadData(res_path + "/" + fl);
    AddParamsToTable(dat, tag, tag_svn, tag_job, params, notes);
  }
}

void ClusterRun::AddParamsToTable(DataTable* dat, const String& tag,
                                  const String& tag_svn, const String& tag_job,
                                  const String& params, const String& notes) {
  if(params.empty()) return;
  String_Array pars;
  pars.FmDelimString(params, " ");
  { 
    DataCol* cl = dat->FindMakeCol("tag", VT_STRING);
    cl->InitVals(tag);
  }
  { 
    DataCol* cl = dat->FindMakeCol("tag_svn", VT_STRING);
    cl->InitVals(tag_svn);
  }
  { 
    DataCol* cl = dat->FindMakeCol("tag_job", VT_STRING);
    cl->InitVals(tag_job);
  }
  { // first add the params as a whole string -- useful for grouping..
    DataCol* cl = dat->FindMakeCol("params", VT_STRING);
    cl->InitVals(params);
  }
  { 
    DataCol* cl = dat->FindMakeCol("notes", VT_STRING);
    cl->InitVals(notes);
  }
  for(int i=0; i<pars.size; i++) {
    String pv = pars[i];
    String nm = pv.before('=');
    String vl = pv.after('=');
    if(nm.empty()) {
      taMisc::Warning("AddParamsToTable: name empty in param element:", pv, "from list:", params);
      continue;
    }
    if(vl.empty()) {
      taMisc::Warning("AddParamsToTable: value empty in param element:", pv, "from list:", params);
      continue;
    }
    // todo: we need to figure out what type of data these guys really are..
    DataCol* cl = dat->FindMakeCol(nm, VT_FLOAT);
    cl->InitVals((float)vl);
  }
}

void ClusterRun::Probe() {
  if(!initClusterManager())
    return;
  String clust = m_cm->ChooseCluster("Choose a cluster to probe");
  if(clust.empty()) return;
  cluster = clust;
  initClusterManager(); // re-init with new cluster info!
  FormatTables();               // ensure tables are formatted properly
  
  jobs_submit.ResetData();
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("PROBE", "status", dst_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
  // Commit the table.
  m_cm->CommitJobSubmissionTable();
  AutoUpdateMe();
}

void ClusterRun::ListJobFiles(bool include_data) {
  if(!initClusterManager())
    return;

  file_list.ResetData();
  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (SelectedRows(jobs_running, st_row, end_row)) {
    for (int row = st_row; row <= end_row; ++row) {
      SelectFiles_impl(jobs_running, row, include_data); 
    }
  }
  else if (SelectedRows(jobs_done, st_row, end_row)) {
    for (int row = st_row; row <= end_row; ++row) {
      SelectFiles_impl(jobs_done, row, include_data); 
    }
  }
  else if (SelectedRows(jobs_archive, st_row, end_row)) {
    for (int row = st_row; row <= end_row; ++row) {
      SelectFiles_impl(jobs_archive, row, include_data); 
    }
  }
  else {
    taMisc::Warning("No rows selected -- no files selected");
  }

  ClearAllSelections();       // done
  ViewPanelNumber(4);
}

void ClusterRun::SelectFiles_impl(DataTable& table, int row, bool include_data) {
  String tag = table.GetVal("tag", row).toString();
  String dat_files = table.GetVal("dat_files", row).toString();
  String other_files = table.GetVal("other_files", row).toString();
  {                             // other files
    String_Array files;
    files.FmDelimString(other_files, " ");
    for(int i=0; i< files.size; i++) {
      String fl = files[i];
      int frow = file_list.AddBlankRow();
      GetFileInfo(fl, file_list, frow, tag);
    }
  }
  if(include_data) {
    String_Array files;
    files.FmDelimString(dat_files, " ");
    for(int i=0; i< files.size; i++) {
      String fl = files[i];
      int frow = file_list.AddBlankRow();
      GetFileInfo(fl, file_list, frow, tag);
    }
  }
}

void ClusterRun::ListAllFiles() {
  FormatTables();               // ensure tables are formatted properly
  file_list.ResetData();

  String wc_res_path = m_cm->GetWcResultsPath();

  QDir dir(wc_res_path);
  dir.setFilter(QDir::Files);
  QFileInfoList files = dir.entryInfoList();
  if(files.size() == 0) {
    taMisc::Warning("No files in results directory:", wc_res_path);
    return;
  }
  for(int i=0;i<files.size();i++) {
    QFileInfo fli = files[i];
    if(fli.isFile()) {
      int frow = file_list.AddBlankRow();
      String tag;
      GetFileInfo(files[i].filePath(), file_list, frow, tag);
    }
  }
  ViewPanelNumber(4);
}

void ClusterRun::GetFileInfo(const String& path, DataTable& table, int row, String& tag) {
  String fl = taMisc::GetFileFmPath(path);
  String wc_res_path = m_cm->GetWcResultsPath();
  String repo_path = wc_res_path.from(svn_repo + "/");

  String flpath = wc_res_path + "/" + fl;

  table.SetVal(fl, "file_name",  row);
  table.SetVal(flpath, "file_path",  row);
  table.SetVal(repo_path + "/" + fl, "svn_file_path",  row);
  table.SetVal("results/" + fl, "proj_file_path",  row);

  if(fl.endsWith(".dat") || fl.endsWith(".dat.gz"))
    table.SetVal("Data", "kind",  row);
  else if(fl.endsWith(".args"))
    table.SetVal("Startup Args", "kind",  row);
  else if(fl.endsWith(".wts") || fl.endsWith(".wts.gz"))
    table.SetVal("Weights", "kind",  row);

  if(tag.empty()) {
    String fnm = taMisc::GetFileFmPath(m_cm->getFilename());
    if(fnm.contains(".proj"))
      fnm = fnm.before(".proj", -1);
    tag = fl.between(fnm, String(".")); // this usually works..
    if(tag.startsWith('_')) tag = tag.after('_');
  }
  table.SetVal(tag, "tag",  row);
  
  if(!taMisc::FileExists(flpath)) {
    table.SetVal("<not local>", "size",  row);
    return;
  }

  QFileInfo fli(flpath);
  
  QDateTime dc = fli.created();
  QDateTime dm = fli.lastModified();
  int64_t sz = fli.size();

  String szstr = taMisc::GetSizeString(sz, 3, true); // 3 prec, power of 2
  String dcstr = dc.toString(timestamp_fmt);
  String dmstr = dm.toString(timestamp_fmt);

  table.SetVal(szstr, "size",  row);
  table.SetVal(dcstr, "date_created",  row);
  table.SetVal(dmstr, "date_modified",  row);
}


void ClusterRun::GetFiles() {
  if(!initClusterManager())
    return;

  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (SelectedRows(file_list, st_row, end_row)) {
    String_Array files;
    for (int row = st_row; row <= end_row; ++row) {
      String fl = file_list.GetVal("file_name", row).toString();
      files.Add(fl);
    }
    String files_str = files.ToDelimString(" ");
    jobs_submit.ResetData();
    SubmitGetFiles(files_str);
    // Commit the table.
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
  else {
    taMisc::Warning("No rows selected -- no files fetched");
  }
}

void ClusterRun::CleanJobFiles() {
  if(!initClusterManager())
    return;

  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (SelectedRows(jobs_done, st_row, end_row)) {
    jobs_submit.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SubmitCleanJobFiles(jobs_done, row);
    }
    // Commit the table.
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe(false);
  }
  else {
    taMisc::Warning("No rows selected -- no job files cleaned");
  }
}

void ClusterRun::RemoveFiles() {
  if(!initClusterManager())
    return;

  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (SelectedRows(file_list, st_row, end_row)) {
    String_PArray files;
    String_Array rmt_files;
    for (int row = end_row; row >= st_row; --row) {
      String fpath = file_list.GetVal("file_path", row).toString();
      if(!taMisc::FileExists(fpath)) {
        String fl = file_list.GetVal("file_name", row).toString();
        rmt_files.Add(fl);
      }
      else {
        files.Add(fpath);
      }
      file_list.RemoveRows(row);
    }
    if(files.size > 0) {
      m_cm->RemoveFiles(files, true, false); // force, keep_local
    }
    if(rmt_files.size > 0) {
      String files_str = rmt_files.ToDelimString(" ");
      jobs_submit.ResetData();
      SubmitRemoveFiles(files_str);
      // Commit the table.
      m_cm->CommitJobSubmissionTable();
    }
    AutoUpdateMe();
  }
  else {
    if (SelectedRows(jobs_done, st_row, end_row)) {
      file_list.ResetData();
      for (int row = end_row; row >= st_row; --row) {
        SelectFiles_impl(jobs_done, row, true); // include data
      }
      RemoveAllFilesInList();
    }
    else if (SelectedRows(jobs_archive, st_row, end_row)) {
      file_list.ResetData();
      for (int row = end_row; row >= st_row; --row) {
        SelectFiles_impl(jobs_archive, row, true); // include data
      }
      RemoveAllFilesInList();
    }
    else {
      taMisc::Warning("No rows selected in either file_list or jobs_done or jobs_archive -- no files removed");
    }
  }
}

void ClusterRun::RemoveNonDataFiles() {
  if(!initClusterManager())
    return;

  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (SelectedRows(jobs_done, st_row, end_row)) {
    file_list.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SelectFiles_impl(jobs_done, row, false); // NOT include data
    }
    RemoveAllFilesInList();
  }
  else if (SelectedRows(jobs_archive, st_row, end_row)) {
    file_list.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SelectFiles_impl(jobs_archive, row, false); // NOT include data
    }
    RemoveAllFilesInList();
  }
  else {
    taMisc::Warning("No rows selected in jobs_done or jobs_archive -- no files removed");
  }
}

void ClusterRun::GetProjAtRev() {
  if(!initClusterManager())
    return;

  int svn_rev = -1;
  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (SelectedRows(jobs_running, st_row, end_row)) {
    if(TestError(st_row != end_row, "GetProjAtRev", "must select only one row"))
      return;
    svn_rev = jobs_running.GetVal("submit_svn", st_row).toInt();
  }
  else if (SelectedRows(jobs_done, st_row, end_row)) {
    if(TestError(st_row != end_row, "GetProjAtRev", "must select only one row"))
      return;
    svn_rev = jobs_done.GetVal("submit_svn", st_row).toInt();
  }
  else if (SelectedRows(jobs_archive, st_row, end_row)) {
    if(TestError(st_row != end_row, "GetProjAtRev", "must select only one row"))
      return;
    svn_rev = jobs_archive.GetVal("submit_svn", st_row).toInt();
  }
  else {
    taMisc::Warning("No rows selected -- project not loaded");
  }
  if(TestError(svn_rev < 0, "GetProjAtRev", "valid svn revision not found"))
    return;
  m_cm->GetProjectAtRev(svn_rev);
  taMisc::Info("Note: GetProjAtRev does NOT require a new checkin -- do not hit Update or wait for auto-update -- the file will be in the main project directory now, or very soon.");
}

void ClusterRun::InitOtherSvn(const String& svn_wc_path, const String& svn_url) {
  if(!svn_other) {
    try {
      svn_other = new SubversionClient;
    }
    catch (const SubversionClient::Exception &ex) {
      taMisc::Error("Error creating other SubversionClient.\n", ex.what());
      return;
    }
  }

  svn_other->SetWorkingCopyPath(svn_wc_path.chars());
  svn_other_wc_path = svn_other->GetWorkingCopyPath().c_str();
  svn_other_url = svn_url;
  taMisc::Info("ClusterRun Other SVN set to url:", svn_other_url, "working copy:",
               svn_other_wc_path);
}

void ClusterRun::ListOtherSvn(int rev, bool recurse) {
  if(!svn_other) return;

  String_PArray file_names;
  String_PArray file_paths;
  int_PArray    file_sizes;
  int_PArray    file_revs;
  int_PArray    file_times;
  int_PArray    file_kinds;
  String_PArray file_authors;
  
  try {
    svn_other->List(file_names, file_paths, file_sizes, file_revs, file_times,
                    file_kinds, file_authors, svn_other_url, rev, recurse);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Error doing List in other SubversionClient.\n", ex.what());
    return;
  }

  FormatTables();               // ensure tables are formatted properly
  file_list.ResetData();
  for(int i=0; i<file_paths.size; i++) {
    if(file_sizes[i] == 0) continue; // skip dirs
    int row = file_list.AddBlankRow();
    file_list.SetVal(file_names[i], "file_name",  row);
    file_list.SetVal(file_paths[i], "file_path",  row);
    String szstr = taMisc::GetSizeString(file_sizes[i], 3, true); // 3 prec, power of 2
    file_list.SetVal(szstr, "size",  row);
    QDateTime dm = QDateTime::fromTime_t(file_times[i]);
    String dmstr = dm.toString(timestamp_fmt);
    file_list.SetVal(dmstr, "date_modified",  row);
    file_list.SetVal(String("svn_other:") + file_authors[i], "tag", row);
    file_list.SetVal(svn_other_url, "svn_file_path", row);
  }
  ViewPanelNumber(4);
}

void ClusterRun::ListOtherUserFiles(const String& user_name) {
  if(!initClusterManager())
    return;

  String url = m_cm->GetFullUrl();
  if(TestError(url.empty(), "ListOtherUserFiles", "our url is empty -- do probe or update first"))
    return;
  String us_user = m_cm->getUsername();
  String wc_path = m_cm->GetWcResultsPath();
  String proj_path = wc_path.after(us_user,-1);

  String wc_sub = wc_path.after(svn_repo);
  String wc_base = wc_path.through(svn_repo);

  wc_sub.gsub(us_user, user_name); // we don't actually use the wc anyway..
  wc_path = wc_base + "/" + wc_sub;

  url.gsub(us_user, user_name);
  url += proj_path;

  InitOtherSvn(wc_path, url);
  ListOtherSvn();               // use defaults
}

void ClusterRun::ListOtherProjFiles(const String& proj_name) {
  if(!initClusterManager())
    return;

  String url = m_cm->GetFullUrl();
  if(TestError(url.empty(), "ListOtherProjFiles", "our url is empty -- do probe or update first"))
    return;
  String us_user = m_cm->getUsername();
  String cur_proj = taMisc::GetFileFmPath(m_cm->getFilename());
  cur_proj = cur_proj.before(".proj");
  String wc_path = m_cm->GetWcResultsPath();
  String proj_path = wc_path.after(us_user,-1);
  proj_path.gsub(cur_proj, proj_name);
  url += proj_path;
  wc_path.gsub(cur_proj, proj_name);

  InitOtherSvn(wc_path, url);
  ListOtherSvn();               // use defaults
}

void ClusterRun::GetOtherFiles() {
  if(!initClusterManager())
    return;

  String wc_path = m_cm->GetWcResultsPath();

  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (SelectedRows(file_list, st_row, end_row)) {
    String_Array files;
    for (int row = st_row; row <= end_row; ++row) {
      String fl = file_list.GetVal("file_name", row).toString();
      String svnp = file_list.GetVal("svn_file_path", row).toString();
      String furl = svnp + "/" + fl;
      String ofl = wc_path + "/" + fl;
      if(taMisc::FileExists(ofl)) {
        taMisc::Error("In attempt to saving file from url:", furl, "to:", ofl,
                      "target file already exists!  Please double check and remove target file if you want to replace");
        continue;
      }
      taMisc::Info("saving file from url:", furl, "to:", ofl);
      try {
        svn_other->SaveFile(furl, ofl);
      }
      catch (const SubversionClient::Exception &ex) {
        taMisc::Error("Error doing SafeFile in other SubversionClient.\n", ex.what());
        return;
      }
    }
  }
  else {
    taMisc::Warning("No rows selected -- no files fetched");
  }
}

void ClusterRun::OpenSvnBrowser() {
  if(!initClusterManager())
    return;
  String url = m_cm->GetFullUrl();
  String us_user = m_cm->getUsername();
  String wc_path = m_cm->GetWcResultsPath();
  String wc_root = wc_path.through(us_user,-1);
  iSubversionBrowser::OpenBrowser(url, wc_root);
}

void ClusterRun::ArchiveJobs() {
  if(!initClusterManager())
    return;

  int st_row, end_row;
  if (SelectedRows(jobs_done, st_row, end_row)) {
    jobs_submit.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SubmitArchiveJob(jobs_done, row);
    }
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
  else {
    taMisc::Warning("No rows selected -- no jobs archived");
  }
}

void ClusterRun::RemoveJobs() {
  if(!initClusterManager())
    return;

  int st_row, end_row;
  if (SelectedRows(jobs_done, st_row, end_row)) {
    int chs = taMisc::Choice("RemoveJobs: Are you sure you want to remove: " + String(1 + end_row - st_row) + " jobs from the jobs_done list?", "Ok", "Cancel");
    if(chs == 1) return;
    jobs_submit.ResetData();
    file_list.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SelectFiles_impl(jobs_done, row, true); // include data
      SubmitRemoveJob(jobs_done, row);
    }
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
    RemoveAllFilesInList();
  }
  else if (SelectedRows(jobs_archive, st_row, end_row)) {
    int chs = taMisc::Choice("RemoveJobs: Are you sure you want to remove: " + String(1 + end_row - st_row) + " jobs from the jobs_archive list?", "Ok", "Cancel");
    if(chs == 1) return;
    jobs_submit.ResetData();
    file_list.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SelectFiles_impl(jobs_archive, row, true); // include data
      SubmitRemoveJob(jobs_archive, row);
    }
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
    RemoveAllFilesInList();
  }
  else {
    taMisc::Warning("No rows selected -- no jobs removed");
  }
}

void ClusterRun::RemoveKilledJobs() {
  if(!initClusterManager())
    return;
  jobs_submit.ResetData();
  file_list.ResetData();
  for (int row = jobs_done.rows-1; row >= 0; --row) {
    String status = jobs_done.GetVal("status", row).toString();
    if(status != "KILLED") continue;
    SelectFiles_impl(jobs_done, row, true); // include data
    SubmitRemoveJob(jobs_done, row);
  }
  if(jobs_submit.rows > 0) {
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
  RemoveAllFilesInList();
}

void ClusterRun::RemoveAllFilesInList() {
  String_PArray files;
  String_Array rmt_files;
  for(int i=0;i<file_list.rows; i++) {
    String fpath = file_list.GetVal("file_path", i).toString();
    if(!taMisc::FileExists(fpath)) {
      String fl = file_list.GetVal("file_name", i).toString();
      rmt_files.Add(fl);
    }
    else {
      files.Add(fpath);
    }
  }
  file_list.ResetData();
  bool updt = false;
  if(files.size > 0) {
    m_cm->RemoveFiles(files, true, false); // force, keep_local
    updt = true;
  }
  if(rmt_files.size > 0) {
    String files_str = rmt_files.ToDelimString(" ");
    jobs_submit.ResetData();
    SubmitRemoveFiles(files_str);
    // Commit the table.
    m_cm->CommitJobSubmissionTable();
    updt = true;
  }
  if(updt) {
    AutoUpdateMe();
  }
}

void ClusterRun::FormatTables() {
  jobs_submit.name = "jobs_submit";
  jobs_submitted.name = "jobs_submitted";
  jobs_running.name = "jobs_running";
  jobs_done.name = "jobs_done";
  jobs_archive.name = "jobs_archive";

  FormatJobTable(jobs_submit);
  FormatJobTable(jobs_submitted);
  FormatJobTable(jobs_running);
  FormatJobTable(jobs_done);
  FormatJobTable(jobs_archive);

  file_list.name = "file_list";
  FormatFileListTable(file_list);

  cluster_info.name = "cluster_info";
  FormatClusterInfoTable(cluster_info);
}

void ClusterRun::FormatJobTable(DataTable& dt) {
  DataCol* dc;

  dt.ClearDataFlag(DataTable::SAVE_ROWS);

  dc = dt.FindMakeCol("tag", VT_STRING);
  dc->desc = "unique tag id for this job -- all files etc are named according to this tag";
  dc = dt.FindMakeCol("notes", VT_STRING);
  dc->desc = "notes for the job -- describe any specific information about the model configuration etc -- can use this for searching and sorting results";
  dc = dt.FindMakeCol("params", VT_STRING);
  dc->desc = "emergent parameters based on currently selected items in the ClusterRun";

  // The client sets this field in the jobs_submit table to:
  //   REQUESTED to request the job be submitted.
  //   CANCELLED to request the job indicated by job_no or tag be cancelled.
  //   PROBE     probe to get the cluster to track this project, and update all running
  //   GETDATA   get the data for the associated tag -- causes cluster to check in dat_files
  //   GETFILES  tell cluster to check in all files listed in this other_files entry
  //   REMOVEJOB tell cluster to remove given tags from jobs_done
  //   CLEANJOBFILES tell cluster to remove job files associated with tags
  //   REMOVEFILES tell cluster to remove specific files listed in this other_files entry
  //   ARCHIVEJOB tell cluster to move given tags from jobs_done into jobs_archive
  // The cluster script sets this field in the running/done tables to:
  //   SUBMITTED after job successfully submitted to a queue.
  //   QUEUED    when the job is known to be in the cluster queue.
  //             At this point, we have a job number (job_no).
  //   RUNNING   when the job has begun.
  //   ABSENT_x  multiple iterations of not finding a status for the job
  //   DONE      if the job completed successfully (after multiple iterations)
  //   KILLED    if the job was cancelled.

  // NOTE: it is essential that we only ever send jobs_submit to cluster, and it
  // sends back jobs_running, jobs_done, jobs_archive, etc -- if we attempt
  // to manipulate one of those other tables, we run the very real risk of 
  // stepping on each others toes and creating svn conflicts.. 
  // this means we have to rely on cluster to do simple things like moving to 
  // archive.  it owns all the jobs files..
  // we can however directly control the data files, as they are not routinely
  // updated

  dc = dt.FindMakeCol("status", VT_STRING);
  dc->desc = "status of job: REQUESTED, CANCELLED, SUBMITTED, QUEUED, RUNNING, DONE, KILLED";

  dc = dt.FindMakeCol("status_info", VT_STRING);
  dc->desc = "more detailed information about status";

  dc = dt.FindMakeCol("submit_time", VT_STRING);
  dc->desc = "when was the job submitted (tracks time from emergent client submission)";
  dc = dt.FindMakeCol("start_time", VT_STRING);
  dc->desc = "when did the job actually start running";
  dc = dt.FindMakeCol("end_time", VT_STRING);
  dc->desc = "when did the job finish running";

  dc = dt.FindMakeCol("job_no", VT_STRING);
  dc->desc = "job number on cluster -- assigned once the job is submitted to the cluster";
  dc = dt.FindMakeCol("job_out", VT_STRING);
  dc->desc = "job output information -- contains (top of) the job standard output and standard error output as the job is running (truncated to top 2048 characters if longer than that) -- full information available in job_out_file";
  dc = dt.FindMakeCol("job_out_file", VT_STRING);
  dc->desc = "job output file -- file name containing full job output information -- file name should be tag.out";
  dc = dt.FindMakeCol("dat_files", VT_STRING);
  dc->desc = "list of data table output (results) files generated by model (space separated) -- these files have the tag in their name, and end in .dat";
  dc = dt.FindMakeCol("other_files", VT_STRING);
  dc->desc = "list of other output (results) files generated by model (space separated) -- these files have the tag in their name but do not end in .dat";

  // Search algo populates these fields.
  dc = dt.FindMakeCol("command_id", VT_INT);
  dc->desc = "id for this command, assigned by the search algorithm in an algorithm-specific manner (optional)";
  dc = dt.FindMakeCol("command", VT_STRING);
  dc->desc = "emergent command line, up to point of parameters";

  // Populated from values the user enters/chooses.
  dc = dt.FindMakeCol("repo_url", VT_STRING);
  dc->desc = "name of repository to run job on";
  dc = dt.FindMakeCol("cluster", VT_STRING);
  dc->desc = "name of cluster to run job on";
  dc = dt.FindMakeCol("queue", VT_STRING);
  dc->desc = "if specified, indicate a particular queue on the computing resource";
  dc = dt.FindMakeCol("run_time", VT_STRING);
  dc->desc = "how long will the jobs take to run -- syntax is number followed by unit indicator -- m=minutes, h=hours, d=days -- e.g., 30m, 12h, or 2d -- typically the job will be killed if it exceeds this amount of time, so be sure to not underestimate";
  dc = dt.FindMakeCol("ram_gb", VT_INT);
  dc->desc = "how many gigabytes of ram is required?  0 means do not specify this parameter for the job submission -- for large memory jobs, it can be important to specify this to ensure proper allocation of resources";
  dc = dt.FindMakeCol("n_threads", VT_INT);
  dc->desc = "number of parallel threads to use for running";
  dc = dt.FindMakeCol("mpi_nodes", VT_INT);
  dc->desc = "number of nodes to use for mpi run -- 0 or -1 means not to use mpi";
  dc = dt.FindMakeCol("pb_batches", VT_INT);
  dc->desc = "if > 0, use parallel batch mode with this number of batches";
  dc = dt.FindMakeCol("pb_nodes", VT_INT);
  dc->desc = "if doing parallel batch mode, and cluster has by_node policy, then this is the number of nodes to allocate to the overall job";
  
  // these two comprise the tag -- internal stuff user doesn't need to see
  dc = dt.FindMakeCol("submit_svn", VT_STRING);
  dc->desc = "svn revision for the original job submission";
  dc = dt.FindMakeCol("submit_job", VT_STRING);
  dc->desc = "index of job number within a given submission -- equal to the row number of the original set of jobs submitted in submit_svn jobs";
}

void ClusterRun::FormatFileListTable(DataTable& dt) {
  DataCol* dc;

  dt.ClearDataFlag(DataTable::SAVE_ROWS);

  dc = dt.FindMakeCol("file_name", VT_STRING);
  dc->desc = "name of file -- does not include any path information";

  dc = dt.FindMakeCol("tag", VT_STRING);
  dc->desc = "job tag associated with this file";

  dc = dt.FindMakeCol("size", VT_STRING);
  dc->desc = "size of file -- with typical suffixes (K = kilobytes, M = megabytes, G = gigabytes)";

  dc = dt.FindMakeCol("kind", VT_STRING);
  dc->desc = "type of file";

  dc = dt.FindMakeCol("date_modified", VT_STRING);
  dc->desc = "timestamp for when the file was last modified";

  dc = dt.FindMakeCol("date_created", VT_STRING);
  dc->desc = "timestamp for when the file was first created";

  dc = dt.FindMakeCol("svn_file_path", VT_STRING);
  dc->desc = "path to file in SVN repository, relative to root of svn_repo repository";

  dc = dt.FindMakeCol("proj_file_path", VT_STRING);
  dc->desc = "path to file relative to the parent project directory -- e.g., results/filename.dat";

  dc = dt.FindMakeCol("file_path", VT_STRING);
  dc->desc = "full path to file on local file system, including all parent directories and name of file -- takes you directly to the file";

}

void ClusterRun::FormatClusterInfoTable(DataTable& dt) {
  DataCol* dc;

  dt.ClearDataFlag(DataTable::SAVE_ROWS);

  dc = dt.FindMakeCol("queue", VT_STRING);
  dc->desc = "queue that this info relates to";

  dc = dt.FindMakeCol("job_no", VT_STRING);
  dc->desc = "job number or total number of actve jobs";

  dc = dt.FindMakeCol("user", VT_STRING);
  dc->desc = "user name";

  dc = dt.FindMakeCol("state", VT_STRING);
  dc->desc = "current scheduler state -- or description of global state info";

  dc = dt.FindMakeCol("procs", VT_STRING);
  dc->desc = "number of processors for this job or for global state info";

  dc = dt.FindMakeCol("start_time", VT_STRING);
  dc->desc = "timestamp for when the job was submitted or started running";

}

String
ClusterRun::CurTimeStamp() {
  taDateTime curtime;
  curtime.currentDateTime();
  return curtime.toString(timestamp_fmt);
}

bool
ClusterRun::ValidateJob(int n_jobs_to_sub) {
  if(cluster.empty()) {
    taMisc::Error("cluster name is empty -- must specify a cluster to run on");
    return false;
  }
  if(svn_repo.empty()) {
    taMisc::Error("svn_repo name is empty -- must specify an svn repository to use");
    return false;
  }
  if(repo_url.empty()) {
    taMisc::Error("repo_url is empty -- must specify an svn repository to use with a valid path");
    return false;
  }

  int csi = taMisc::clusters.FindName(cluster);
  if(csi < 0) {
    taMisc::Error("Can't find cluster named:", cluster);
    return false;
  }
  ClusterSpecs& cs = taMisc::clusters[csi];

  if(run_time.empty()) {
    taMisc::Error("run_time is blank -- you MUST specify a run time -- syntax is number followed by unit indicator -- m=minutes, h=hours, d=days -- e.g., 30m, 12h, or 2d -- typically the job will be killed if it exceeds this amount of time, so be sure to not underestimate!");
    return false;
  }
  int rth = 0;                      // run time in hours
  if(run_time.endsWith('m')) {
    rth = (int)run_time.before('m');
    if(rth == 0) {
      taMisc::Error("run_time in minutes is 0 -- you MUST specify a non-zero run time -- syntax is number followed by unit indicator -- m=minutes, h=hours, d=days -- e.g., 30m, 12h, or 2d -- typically the job will be killed if it exceeds this amount of time, so be sure to not underestimate!");
      return false;
    }
    rth /= 60;
    if(rth < 1) rth = 1;
  }
  else if(run_time.endsWith('h')) {
    rth = (int)run_time.before('h');
  }
  else if(run_time.endsWith('d')) {
    rth = (int)run_time.before('d') * 24;
  }
  if(rth == 0) {
    taMisc::Error("run_time is 0 -- you MUST specify a non-zero run time -- syntax is number followed by unit indicator -- m=minutes, h=hours, d=days -- e.g., 30m, 12h, or 2d -- typically the job will be killed if it exceeds this amount of time, so be sure to not underestimate!");
    return false;
  }

  if(cs.max_time > 0 && rth > cs.max_time)  {
    int chs = taMisc::Choice("You are requesting to run more than listed max run time on cluster: " + cluster + " -- run time in hours requested: " + String(rth) + " max: " +
                             String(cs.max_time), "Continue Anyway (NOT recommended)", "Cancel");
    if(chs == 1) return false;
  }

  if(use_mpi) {
    if(!cs.mpi) {
      taMisc::Error("Job requests to use MPI but cluster says mpi is NOT available on cluster:", cluster);
      return false;
    }
    if(mpi_nodes <= 1) {
      taMisc::Error("Job requests to use MPI but mpi_nodes is <= 1", String(mpi_nodes));
      return false;
    }
    if(mpi_nodes > cs.nodes) {
      taMisc::Error("Job requests to use MPI with more nodes than is available on cluster:", cluster, "mpi_nodes requested:", String(mpi_nodes), "avail on cluster:", String(cs.nodes));
      return false;
    }
  }

  if(nowin_x && cs.gpus <= 0) {
    taMisc::Error("Job requests to use -nowin and XWindows, but this requires GPUs on the cluster nodes -- currently it is listed as having none -- either fix the cluster config or select a different config");
    return false;
  }

  if(parallel_batch) {
    if(pb_batches <= 1) {
      taMisc::Error("Job requests to use parallel_batches but pb_batches is <= 1",
                    String(pb_batches));
      return false;
    }
  }
  if(cs.by_node && cs.procs_per_node <= 1) {
    taMisc::Error("Cluster:", cluster, "says allocate by node but procs_per_node is not set -- must set this parameter in Preferences / Options settings");
    return false;
  }
  if(cs.by_node && cs.procs_per_node > 4 && !(use_mpi || parallel_batch)) {
    taMisc::Error("Cluster:", cluster, "has allocate by node policy, but you are not requesting either an mpi or a parallel batch job -- this will waste compute allocations -- please use a cluster that is more appropriate for single-shot single-processor jobs");
    return false;
  }
  if(cs.max_jobs > 0) {
    if(n_jobs_to_sub > cs.max_jobs) {
      int chs = taMisc::Choice("You are requesting to run more than listed max number of jobs on cluster: " + cluster + " -- jobs: " + String(n_jobs_to_sub) + " max: " +
                               String(cs.max_jobs), "Continue Anyway", "Cancel");
      if(chs == 1) return false;
    }
  }
  int tot_procs = n_jobs_to_sub * n_threads;
  if(use_mpi) tot_procs *= mpi_nodes;
  if(parallel_batch && cs.by_node) { // note: regular non-by-node pb already reflected in n_jobs_to_sub!
    tot_procs *= pb_batches;
  }

  taMisc::Info("total procs requested for this job:", String(tot_procs));

  if(cs.max_procs > 0) {
    if(tot_procs > cs.max_procs) {
      int chs = taMisc::Choice("You are requesting to run more than listed max number of processors on cluster: " + cluster + " -- procs requested: " + String(tot_procs) + " max: " +
                               String(cs.max_procs), "Continue Anyway", "Cancel");
      if(chs == 1) return false;
    }
  }

  if(cs.min_procs > 0) {
    if(tot_procs < cs.min_procs) {
      int chs = taMisc::Choice("You are requesting to run LESS than listed min number of processors on cluster: " + cluster + " -- procs requested: " + String(tot_procs) + " min: " +
                               String(cs.min_procs), "Continue Anyway", "Cancel");
      if(chs == 1) return false;
    }
  }

  if(tot_procs > cs.procs) {
    taMisc::Error("You are requesting to run more than listed TOTAL number of processors on cluster: " + cluster + " -- procs requested: " + String(tot_procs) + " procs: " +
                  String(cs.procs));
    return false;
  }

  if(cs.max_ram > 0 && ram_gb > 0 && ram_gb > cs.max_ram)  {
    int chs = taMisc::Choice("You are requesting to run more than listed max ram on cluster: " + cluster + " -- ram in Gb requested: " + String(ram_gb) + " max: " +
                             String(cs.max_ram), "Continue Anyway (NOT recommended)", "Cancel");
    if(chs == 1) return false;
  }

  // final sanity check for large-ish jobs..
  if(tot_procs > 24) {
    int chs = taMisc::Choice("You are requesting to run a job using: " + String(tot_procs) + " total processors on cluster: " + cluster + " -- please confirm!", "Run", "Cancel");
    if(chs == 1) return false;
  }

  return true;
}


void
ClusterRun::AddJobRow_impl(const String& cmd, const String& params, int cmd_id) {
  int csi = taMisc::clusters.FindName(cluster);
  ClusterSpecs& cs = taMisc::clusters[csi];

  int row = jobs_submit.AddBlankRow();
  // submit_svn filled in later -- not avail now
  jobs_submit.SetVal(last_submit_time, "submit_time",   row);
  jobs_submit.SetVal(String(row), "submit_job", row); // = row!
  // job_no will be filled in on cluster
  // tag will be filled in on cluster
  jobs_submit.SetVal("REQUESTED", "status",     row);
  // job_out, job_out_file, dat_files all generated on cluster
  jobs_submit.SetVal(cmd_id,      "command_id", row);
  jobs_submit.SetVal(cmd,         "command",    row);
  jobs_submit.SetVal(params,      "params",     row);
  jobs_submit.SetVal(notes,       "notes",      row);

  jobs_submit.SetVal(repo_url,    "repo_url",   row);
  jobs_submit.SetVal(cluster,     "cluster",    row);
  jobs_submit.SetVal(queue,       "queue",      row);
  jobs_submit.SetVal(run_time,    "run_time",   row);
  jobs_submit.SetVal(ram_gb,      "ram_gb",     row);
  jobs_submit.SetVal(n_threads,   "n_threads",  row);
  if(use_mpi)
    jobs_submit.SetVal(mpi_nodes,   "mpi_nodes",  row);
  else
    jobs_submit.SetVal(0,   "mpi_nodes",  row);

  if(parallel_batch && pb_batches > 0) {
    jobs_submit.SetVal(pb_batches,   "pb_batches",  row);
    if(cs.by_node) {
      int eff_nodes = pb_nodes;
      if(pb_nodes == 0) {
        int tot_procs = pb_batches * n_threads;
        if(use_mpi)
          tot_procs *= mpi_nodes;
        eff_nodes = tot_procs / cs.procs_per_node;
        if(eff_nodes * cs.procs_per_node < tot_procs)
          eff_nodes++;
      }
      jobs_submit.SetVal(eff_nodes,   "pb_nodes",  row);
    }
    else {
      jobs_submit.SetVal(0,   "pb_nodes",  row);
    }
  }
  else {
    jobs_submit.SetVal(0,   "pb_batches",  row);
    jobs_submit.SetVal(0,   "pb_nodes",  row);
  }
}

void
ClusterRun::AddJobRow(const String& cmd, const String& params, int& cmd_id) {
  // this will trigger a guaranteed commit of the project and the log file
  last_submit_time = CurTimeStamp();

  int csi = taMisc::clusters.FindName(cluster);
  if(csi < 0) {
    taMisc::Error("Can't find cluster named:", cluster); // shouldn't happen
    return;
  }
  ClusterSpecs& cs = taMisc::clusters[csi];

  if(parallel_batch && pb_batches > 0 && !cs.by_node) {
    // we stream off the jobs ourselves
    for(int i=0; i<pb_batches; i++) {
      // only support single-increment pb, assume the two args per existing convention
      String cmd_pb = cmd + " b_start=" + String(i) + " b_end=" + String(i+1);
      AddJobRow_impl(cmd_pb, params, cmd_id++);
    }
  }
  else {
    AddJobRow_impl(cmd, params, cmd_id++);
  }
}

void
ClusterRun::CancelJob(int running_row)
{
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("CANCELLED", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, jobs_running, "job_no", running_row);
  jobs_submit.CopyCell("tag", dst_row, jobs_running, "tag", running_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::SubmitGetData(const DataTable& table, int tab_row)
{
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("GETDATA", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, table, "job_no", tab_row);
  jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::SubmitRemoveJob(const DataTable& table, int tab_row)
{
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("REMOVEJOB", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, table, "job_no", tab_row);
  jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::SubmitArchiveJob(const DataTable& table, int tab_row)
{
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("ARCHIVEJOB", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, table, "job_no", tab_row);
  jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::SubmitCleanJobFiles(const DataTable& table, int tab_row)
{
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("CLEANJOBFILES", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, table, "job_no", tab_row);
  jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::SubmitGetFiles(const String& files) {
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("GETFILES", "status", dst_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
  jobs_submit.SetVal(files, "other_files",  dst_row); 
}

void
ClusterRun::SubmitRemoveFiles(const String& files) {
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("REMOVEFILES", "status", dst_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
  jobs_submit.SetVal(files, "other_files",  dst_row); 
}

int
ClusterRun::CountJobs(const DataTable &table, const String &status_regexp)
{
  int count = 0;
  QRegExp re(status_regexp.toQString());
  for (int row = 0; row < table.rows; ++row) {
    QString status = table.GetValAsString("status", row).toQString();
    if (status.contains(re)) ++count;
  }
  return count;
}

void ClusterRun::RunCommand(String& cmd, String& params, bool use_cur_vals) {
  // Start command with either "emergent" or "emergent_mpi".
  cmd = taMisc::app_name;
  if(nowin_x)
    cmd += "_x";
  if (use_mpi) {
    cmd.cat("_mpi");
  }

  // The cluster script needs to substitute the correct relative
  // filename for the project file in its working copy.  It also
  // needs to substitute the tag, which is based on the revision
  // and row number.
  if(nowin_x)
    cmd.cat(" -nowin");
  else
    cmd.cat(" -nogui");
  cmd.cat(" -ni -p <PROJ_FILENAME> tag=<TAG>");

  // Note: cluster script sets number of mpi nodes

  if (n_threads > 0) {
    cmd.cat(" n_threads=").cat(String(n_threads));
  }

  params="";
  // Add a name=val term for each parameter in the search.
  bool first = true;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, mbr, mbrs) {
    const EditParamSearch &ps = mbr->param_search;
    if (ps.srch != EditParamSearch::NO) {
      if(!first)
        params.cat(" "); // sep
      else
        first = false;
      params.cat(mbr->GetName()).cat("=");
      if(use_cur_vals || !mbr->is_numeric) {
        params.cat(mbr->CurValAsString());
      }
      else {
        params.cat(String(ps.next_val));
      }
    }
  }
}

void ClusterRun::CreateCurJob(int cmd_id) {
  String cmd;
  String params;
  RunCommand(cmd, params, true); // use cur vals
  AddJobRow(cmd, params, cmd_id);
}

iDataTableEditor* ClusterRun::DataTableEditor(DataTable& dt) {
  // String strm;
  // dt.ListSigClients(strm);
  // taMisc::ConsoleOutput(strm);
  taSigLink* dl = dt.sig_link();
  if(dl) {
    taSigLinkItr itr;
    iPanelOfDataTable* el;
    FOR_DLC_EL_OF_TYPE(iPanelOfDataTable, el, dl, itr) {
      return el->dte;
    }
  }
  return NULL;
}

bool ClusterRun::SelectedRows(DataTable& dt, int& st_row, int& end_row) {
  st_row = -1;
  end_row = -1;
  iDataTableEditor* ed = DataTableEditor(dt);
  if(!ed || !ed->tvTable) return false;
  bool rval = ed->tvTable->SelectedRows(st_row, end_row);
  if(!rval) {
    // taMisc::Info("no items selected");
    return false;
  }
  // taMisc::Info("start row:", String(st_row), "end row:", String(end_row));
  return true;
}

bool ClusterRun::SelectRows(DataTable& dt, int st_row, int end_row) {
  iDataTableEditor* ed = DataTableEditor(dt);
  if(!ed || !ed->tvTable) return false;
  bool rval = ed->tvTable->SelectRows(st_row, end_row);
  return rval;
}

void ClusterRun::ClearSelection(DataTable& dt) {
  iDataTableEditor* ed = DataTableEditor(dt);
  if(!ed || !ed->tvTable) return;
  ed->tvTable->clearExtSelection();
}

void ClusterRun::ClearAllSelections() {
  ClearSelection(jobs_submit);
  ClearSelection(jobs_running);
  ClearSelection(jobs_done);
  ClearSelection(jobs_archive);
  ClearSelection(file_list);
}

///////////////////////////

String ClusterRun::GetSvnPath() {
  if(!initClusterManager(false)) // fail silently, don't check prefs..
    return _nilString;
  return m_cm->GetWcProjPath();
}

iPanelSet* ClusterRun::FindMyPanelSet() {
  if(!taMisc::gui_active) return NULL;
  taSigLink* link = sig_link();
  if(!link) return NULL;
  taSigLinkItr itr;
  iPanelSet* el;
  FOR_DLC_EL_OF_TYPE(iPanelSet, el, link, itr) {
    return el;
  }
  return NULL;
}

bool ClusterRun::ViewPanelNumber(int panel_no) {
  iPanelSet* dps = FindMyPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(panel_no);
  return true;
}


void ClusterRun::AutoUpdateMe(bool clear_sels) {
  if(clear_sels) {
    ClearAllSelections();
  }
  cur_svn_rev = m_cm->GetCurSvnRev(); // always update to latest..
  wait_proc_updt = this;
  wait_proc_trg_rev = cur_svn_rev + 1;
  wait_proc_start.currentDateTime();
  wait_proc_last_updt.currentDateTime();
  SigEmitUpdated();             // get the latest revision
}

bool ClusterRun::WaitProcAutoUpdate() {
  if(!wait_proc_updt) return false;
  taDateTime curtime;
  curtime.currentDateTime();
  int delay = wait_proc_last_updt.secsTo(curtime);
  if(delay < 10) {
    return false;
  }
  wait_proc_updt->Update();
  wait_proc_updt->SigEmitUpdated();
  wait_proc_last_updt.currentDateTime();
  if(wait_proc_updt->cur_svn_rev >= wait_proc_trg_rev) {
    taMisc::Info("ClusterRun: updated to target revision:", wait_proc_updt->name);
    wait_proc_updt = NULL;
    wait_proc_trg_rev = -1;
    return true;
  }
  if(wait_proc_start.secsTo(curtime) > 120) {
    taMisc::Info("ClusterRun: time out on updating cluster run:", wait_proc_updt->name);
    wait_proc_updt = NULL;
    wait_proc_trg_rev = -1;
    return true;
  }
  return true;
}
