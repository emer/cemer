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
#include <taDateTime>
#include <taDataProc>
#include <DataTable_Group>
#include <taProject>

#include <taMisc>

#include <QRegExp>
#include <QDir>

String ClusterRun::timestamp_fmt = "yyyy_MM_dd_hh_mm_ss";

void ClusterRun::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ClusterRun);
  FormatTables();
}

void ClusterRun::Initialize() {
  ram_gb = 0;
  n_threads = taMisc::thread_defaults.n_threads;
  use_mpi = false;
  mpi_nodes = 10;
  parallel_batch = false;
  pb_batches = 10;
  pb_nodes = 0;
  nowin_x = false;
  m_cm = 0;
}

void ClusterRun::Destroy() {
  delete m_cm; m_cm = 0;
}

void ClusterRun::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(svn_repo.nonempty()) {
    Variant rep = taMisc::svn_repos.GetVal(svn_repo);
    if(rep.isNull()) {
      taMisc::Warning("ClusterRun: svn_repo:", svn_repo,
                      "not found in list of registered svn repositories in Preferences / Options");
    }
    else {
      repo_url = rep.toString();
    }
  }
}

void ClusterRun::initClusterManager() {
  if(!m_cm)
    m_cm = new ClusterManager(*this);
  else
    m_cm->Init();
}

void ClusterRun::NewSearchAlgo(TypeDef *type) {
  search_algos.New(1, type);
}

void ClusterRun::Run() {
  initClusterManager();         // ensure it has been created.
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
      // move them over to submitted now!
      taDataProc::AppendRows(&jobs_submitted, &jobs_submit);
    }
  }
}

bool ClusterRun::Update() {
  initClusterManager(); // ensure it has been created.

  // Update the working copy and load the running/done tables.
  bool has_updates = m_cm->UpdateTables();
  SortClusterInfoTable();
  if (has_updates && cur_search_algo) {
    cur_search_algo->ProcessResults();
  }
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

void ClusterRun::Cont() {
  initClusterManager(); // ensure it has been created.

  // Create the next batch of jobs.
  if (cur_search_algo && cur_search_algo->CreateJobs()) {
    // Commit the table to submit the jobs.
    m_cm->CommitJobSubmissionTable();
  }
}

void ClusterRun::Kill() {
  initClusterManager(); // ensure it has been created.
  FormatTables();               // ensure tables are formatted properly

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
  }
  else {
    taMisc::Warning("No rows selected -- no jobs were killed");
  }
}

void ClusterRun::GetData() {
  initClusterManager(); // ensure it has been created.
  FormatTables();               // ensure tables are formatted properly

  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (SelectedRows(jobs_running, st_row, end_row)) {
    jobs_submit.ResetData();
    for (int row = st_row; row <= end_row; ++row) {
      GetDataJob(jobs_running, row); 
    }
    // Commit the table.
    m_cm->CommitJobSubmissionTable();
  }
  else if (SelectedRows(jobs_done, st_row, end_row)) {
    jobs_submit.ResetData();
    for (int row = st_row; row <= end_row; ++row) {
      GetDataJob(jobs_done, row); 
    }
    // Commit the table.
    m_cm->CommitJobSubmissionTable();
  }
  else {
    taMisc::Warning("No rows selected -- no data fetched");
  }
}

void ClusterRun::ImportData(bool remove_existing) {
  initClusterManager(); // ensure it has been created.
  // note: can't call Update here because it unselects the rows in jobs_ tables!

  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return;
  DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("ClusterRun");
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
  else if (SelectedRows(file_list, st_row, end_row)) {
    for (int row = st_row; row <= end_row; ++row) {
      ImportData_impl(dgp, file_list, row); 
    }
  }
  else {
    taMisc::Warning("No rows selected -- no data fetched");
  }
}

void ClusterRun::ImportData_impl(DataTable_Group* dgp, const DataTable& table, int row) {
  String tag = table.GetValAsString("tag", row);
  String dat_files;
  String params;
  if(table.name == "file_list") {
    dat_files = table.GetValAsString("file_name", row);
    if(!dat_files.contains(".dat")) return; // not a dat file!
    int lkup = jobs_done.FindVal(tag, "tag");
    if(lkup >= 0) {
      params = jobs_done.GetValAsString("params", lkup);
    }
    else {
      lkup = jobs_running.FindVal(tag, "tag");
      if(lkup >= 0) {
        params = jobs_running.GetValAsString("params", lkup);
      } 
    }
  }
  else {
    dat_files = table.GetValAsString("dat_files", row);
    params = table.GetValAsString("params", row);
  }
  if(TestWarning(dat_files.empty(), "ImportData", "dat_files is empty for tag:", tag))
    return;
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
    AddParamsToTable(dat, params);
  }
}

void ClusterRun::AddParamsToTable(DataTable* dat, const String& params) {
  if(params.empty()) return;
  String_Array pars;
  pars.FmDelimString(params, " ");
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
  initClusterManager(); // ensure it has been created.
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
}

void ClusterRun::SelectFiles(bool include_data) {
  initClusterManager(); // ensure it has been created.

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
  else {
    taMisc::Warning("No rows selected -- no files selected");
  }
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
  initClusterManager(); // ensure it has been created.
  FormatTables();               // ensure tables are formatted properly

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
    GetFilesJob(files_str);
    // Commit the table.
    m_cm->CommitJobSubmissionTable();
  }
  else {
    taMisc::Warning("No rows selected -- no files fetched");
  }
}

void ClusterRun::RemoveFiles() {
  initClusterManager(); // ensure it has been created.

  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (SelectedRows(file_list, st_row, end_row)) {
    String_PArray files;
    for (int row = end_row; row >= st_row; --row) {
      String fpath = file_list.GetVal("file_path", row).toString();
      if(!taMisc::FileExists(fpath))
        continue;               // skip any files that don't actually exist locally
      files.Add(fpath);
      file_list.RemoveRows(row);
    }
    m_cm->RemoveFiles(files, true, false); // force, keep_local
  }
  else {
    taMisc::Warning("No rows selected -- no files fetched");
  }
}

void ClusterRun::RemoveJobs() {
  initClusterManager(); // ensure it has been created.

  int st_row, end_row;
  if (SelectedRows(jobs_done, st_row, end_row)) {
    // avoid conflict in writing this table -- can be written by host so we need to 
    // get in and out quickly -- grab current now -- won't affect selection!
    m_cm->UpdateTables();

    file_list.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SelectFiles_impl(jobs_done, row, true); // include data
      jobs_done.RemoveRows(row);
    }
    RemoveAllFilesInList();
    m_cm->CommitJobsDoneTable();
  }
  else {
    taMisc::Warning("No rows selected -- no jobs removed");
  }
}

void ClusterRun::RemoveKilledJobs() {
  initClusterManager(); // ensure it has been created.
  // avoid conflict in writing this table -- can be written by host so we need to 
  // get in and out quickly -- grab current now -- won't affect selection!
  m_cm->UpdateTables();

  file_list.ResetData();
  for (int row = jobs_done.rows-1; row >= 0; --row) {
    String status = jobs_done.GetVal("status", row).toString();
    if(status != "KILLED") continue;
    SelectFiles_impl(jobs_done, row, true); // include data
    jobs_done.RemoveRows(row);
  }
  RemoveAllFilesInList();
  m_cm->CommitJobsDoneTable();
}

void ClusterRun::RemoveAllFilesInList() {
  String_PArray files;
  for(int i=0;i<file_list.rows; i++) {
    String fpath = file_list.GetVal("file_path", i).toString();
    if(!taMisc::FileExists(fpath))
      continue;               // skip any files that don't actually exist locally
    files.Add(fpath);
  }
  file_list.ResetData();
  if(files.size > 0)
    m_cm->RemoveFiles(files, true, false); // force, keep_local
}

void ClusterRun::FormatTables() {
  jobs_submit.name = "jobs_submit";
  jobs_submitted.name = "jobs_submitted";
  jobs_running.name = "jobs_running";
  jobs_done.name = "jobs_done";

  FormatJobTable(jobs_submit);
  FormatJobTable(jobs_submitted);
  FormatJobTable(jobs_running);
  FormatJobTable(jobs_done);

  file_list.name = "file_list";
  FormatFileListTable(file_list);

  cluster_info.name = "cluster_info";
  FormatClusterInfoTable(cluster_info);
}

void ClusterRun::FormatJobTable(DataTable& dt) {
  DataCol* dc;

  dc = dt.FindMakeCol("tag", VT_STRING);
  dc->desc = "unique tag id for this job -- all files etc are named according to this tag";
  dc = dt.FindMakeCol("notes", VT_STRING);
  dc->desc = "notes for the job -- describe any specific information about the model configuration etc -- can use this for searching and sorting results";

  // The client sets this field in the jobs_submit table to:
  //   REQUESTED to request the job be submitted.
  //   CANCELLED to request the job indicated by job_no or tag be cancelled.
  //   PROBE     probe to get the cluster to track this project, and update all running
  //   GETDATA   get the data for the associated tag -- causes cluster to check in dat_files
  //   GETFILES  tell cluster to check in all files listed in this other_files entry
  // The cluster script sets this field in the running/done tables to:
  //   SUBMITTED after job successfully submitted to a queue.
  //   QUEUED    when the job is known to be in the cluster queue.
  //             At this point, we have a job number (job_no).
  //   RUNNING   when the job has begun.
  //   DONE      if the job completed successfully.
  //   KILLED    if the job was cancelled.
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
  dc = dt.FindMakeCol("params", VT_STRING);
  dc->desc = "emergent parameters based on currently selected items in the ClusterRun";

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
  int rth;                      // run time in hours
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
  if(parallel_batch) tot_procs *= pb_batches;

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
ClusterRun::GetDataJob(const DataTable& table, int tab_row)
{
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("GETDATA", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, table, "job_no", tab_row);
  jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void ClusterRun::GetFilesJob(const String& files) {
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("GETFILES", "status", dst_row);
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
    if (ps.search) {
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
