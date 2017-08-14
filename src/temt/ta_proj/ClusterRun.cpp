// Copyright 2017, Regents of the University of Colorado,
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
#include <ClusterRun_QObj>
#include <ClusterManager>
#include <ClusterRunJob>
#include <taSigLinkItr>
#include <iDataTableEditor>
#include <iPanelOfDataTable>
#include <iClusterTableView>
#include <taDataProc>
#include <DataTable_Group>
#include <DataGroupSpec>
#include <DataGroupEl>
#include <taProject>
#include <SubversionClient>
#include <iSubversionBrowser>
#include <ParamSet>
#include <taRootBase>
#include <taSigLinkItr>
#include <iPanelSet>
#include <taMisc>
#include <taiMisc>

#include <QRegExp>
#include <QDir>
#include <QApplication>

#include <QObject> //


TA_BASEFUNS_CTORS_DEFN(ClusterRun);
SMARTREF_OF_CPP(ClusterRun); // ClusterRunRef


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
  set_proj_name = false;
  auto_updt_interval = 10;
  auto_updt_timeout = 30;
  cur_svn_rev = -1;
  last_backend_checkin = "Unknown";
  is_updating = false;
  exe_cmd = taMisc::app_name;
  use_search_algo = false;
  ram_gb = 0;
  n_threads = 1;  // taMisc::thread_defaults.n_threads
  use_mpi = false;
  use_cuda = false;
  mpi_nodes = 10;
  mpi_per_node = 1;
  parallel_batch = false;
  pb_batches = 10;
  pb_n_batches_per = 1;
  nowin_x = false;
  m_cm = 0;
  svn_other = NULL;
  auto_edit = false;
  
  enable_kill = false;
  enable_load = false;
  
  qt_object_helper = new ClusterRun_QObj(this);
  helper_is_connected = false;

  jobs_submit.row_height = 2;
  jobs_submitted.row_height = 2;
  jobs_running.row_height = 2;
  jobs_done.row_height = 2;
  jobs_deleted.row_height = 2;
  jobs_archive.row_height = 2;
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
  if(set_proj_name) {
    if(proj_name.empty()) {
      taProject* proj = GetMyProj();
      if(proj) {
        proj_name = taMisc::GetFileFmPath(proj->file_name);
      }
    }
    if(proj_name.contains(".proj")) {
      proj_name = proj_name.before(".proj");
    }
  }

  if(taMisc::is_loading) {
    if(jobs_submit.cols() > jobs_running.cols() ||
       jobs_submitted.cols() > jobs_running.cols()) {
      jobs_submit.Reset();          // get rid of any weirdness from prior bug
      jobs_submitted.Reset(); 
    }
    FormatTables();
    taVersion v784(7, 8, 4);
    if (taMisc::loading_version < v784) {
      if(cur_search_algo) {
        use_search_algo = true;
      }
    }
  }
}

bool ClusterRun::AddCluster(const String& clust_nm) {
  if(clusters.contains(clust_nm))
    return false;
  if(!clusters.empty())
    clusters << " ";
  clusters << clust_nm;
  return true;
}

bool ClusterRun::AddUser(const String& user_nm) {
  if(users.contains(user_nm))
    return false;
  if(!users.empty())
    users << " ";
  users << user_nm;
  return true;
}

bool ClusterRun::InitClusterManager(bool check_prefs) {
  if(is_updating) {
    taMisc::Choice("Currently updating -- cannot submit a new action -- retry when is_updating = false on cluster run panel", "OK");
    return false;
  }
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

ParamSearchAlgo* ClusterRun::NewSearchAlgo(TypeDef *type) {
  ParamSearchAlgo* algo = (ParamSearchAlgo*)search_algos.New(1, type);
  cur_search_algo = algo;       // always select by default
  use_search_algo = true;
  return algo;
}

void ClusterRun::Run() {
  if(!InitClusterManager())
    return;
  Update(); //Update the SVN repository in the background
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
  if (is_updating) {
    taMisc::Info("Cluster run is already updating in the background. Skipping new update call");
    return 1;
  }
  if(!InitClusterManager())
    return false;
  AddCluster(cluster);
  AddUser(m_cm->GetUsername());
  m_cm->InitiateBackgroundSVNUpdate();
  return true;
}

bool ClusterRun::LoadJobs() {
  return Update_impl(false);
}

bool ClusterRun::Update_impl(bool do_svn_update) {
  if(QApplication::activeModalWidget() != NULL) {
    taMisc::Info("ClusterRun: modal window is now open, cancelling auto-update");
    wait_proc_updt = NULL;
    wait_proc_trg_rev = -1;
    return false;               // don't update while a window is open!
  }
  
  if(!InitClusterManager())
    return false;
  
  FormatTables();            // ensure data tables are formatted properly!

  // Update the working copy and load the running/done tables.
  // save current selection information and restore at end
  int st_row_done, end_row_done;
  bool has_sel_done = SelectedRows(jobs_done, st_row_done, end_row_done);
  int st_row_archive, end_row_archive;
  bool has_sel_archive = SelectedRows(jobs_archive, st_row_archive, end_row_archive);

  bool has_updates = m_cm->UpdateTables(do_svn_update);
  cur_svn_rev = m_cm->GetCurSvnRev();
  QDateTime clusterrun_backend_script_timestamp = QDateTime::fromString(clusterscript_timestamp.GetDataByName("timestamp").toQString(), Qt::ISODate);
  clusterrun_backend_script_timestamp.setTimeSpec(Qt::UTC);
  last_backend_checkin = clusterrun_backend_script_timestamp.toLocalTime().toString();
  CheckBackendRunning();
  SortClusterInfoTable();
  
  if (jobs_done.HasBeenFiltered()) {
    jobs_done.FilterAgain();
  }
  if (jobs_running.HasBeenFiltered()) {
    jobs_running.FilterAgain();
  }
  if (jobs_archive.HasBeenFiltered()) {
    jobs_archive.FilterAgain();
  }
  if (jobs_deleted.HasBeenFiltered()) {
    jobs_deleted.FilterAgain();
  }

  if (jobs_done.HasBeenSorted()) {
    jobs_done.SortAgain();
  }
  else {
    jobs_done.Sort("tag", true);
  }
  
  if (jobs_running.HasBeenSorted()) {
    jobs_running.SortAgain();
  }
  else {
    jobs_running.Sort("tag", true);
  }
  
  if (jobs_archive.HasBeenSorted()) {
    jobs_archive.SortAgain();
  }
  else {
    jobs_archive.Sort("tag", true);
  }
  
  if (jobs_deleted.HasBeenSorted()) {
    jobs_deleted.SortAgain();
  }
  else {
    jobs_deleted.Sort("tag", true);
  }
  
  if(has_sel_done && st_row_done >= 0 && end_row_done >= st_row_done) {
    SelectRows(jobs_done, st_row_done, end_row_done);
  }
  if(has_sel_archive && st_row_archive >= 0 && end_row_archive >= st_row_archive) {
    SelectRows(jobs_archive, st_row_archive, end_row_archive);
  }

  if (has_updates && cur_search_algo && use_search_algo) {
    cur_search_algo->ProcessResults();
  }
  
  FillInRunningTime(&jobs_done);
  FillInRunningTime(&jobs_archive);
  FillInRunningTime(&jobs_deleted);
  FillInElapsedTime(&jobs_running);

  if (wait_proc_updt) {
    taDateTime curtime;
    curtime.currentDateTime();
    if(wait_proc_updt->cur_svn_rev >= wait_proc_trg_rev) {
      taMisc::Info("ClusterRun: updated to target revision:",
                   String(wait_proc_updt->cur_svn_rev));
      wait_proc_updt = NULL;
      wait_proc_trg_rev = -1;
      SigEmitUpdated();
      UpdateUI();
      return true;
    }
    if(wait_proc_start.secsTo(curtime) > wait_proc_updt->auto_updt_timeout) {
      taMisc::Info("ClusterRun: time out on updating cluster run -- press the Update button manually to get the updates, cur rev:",
                   String(wait_proc_updt->cur_svn_rev));
    
      wait_proc_updt = NULL;
      wait_proc_trg_rev = -1;
      SigEmitUpdated();
      UpdateUI();
      return true;
    }
  }
  taMisc::Info("ClusterRun: updated to revision: ", String(cur_svn_rev));
  SigEmitUpdated();
  UpdateUI();
  return has_updates;
}

void ClusterRun::SortClusterInfoTable() {
  String usrname = m_cm->GetUsername();
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
  if(!InitClusterManager())
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
  if(!InitClusterManager())
    return;

  // Create the next batch of jobs.
  if (cur_search_algo && use_search_algo) {
    if (cur_search_algo->CreateJobs()) {
      // Commit the table to submit the jobs.
      m_cm->CommitJobSubmissionTable();
      AutoUpdateMe();
    }
  }
}

void ClusterRun::Kill() {
  if(!InitClusterManager())
    return;

  // Get the (inclusive) range of rows to kill.
  int st_row, end_row;
  if (SelectedRows(jobs_running, st_row, end_row)) {
    if (!CheckLocalClustUserRows(jobs_running, st_row, end_row)) {
      return;
    }

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

void ClusterRun::UpdtNotes() {
  if(!InitClusterManager())
    return;
  
  jobs_submit.ResetData();
  
  DataTable* cur_table = GetCurDataTable();
  
  // Get the (inclusive) range of rows to update notes.
  int st_row, end_row;
  if (cur_table == &jobs_running || cur_table == &jobs_done || cur_table == &jobs_archive) {
    if (SelectedRows(*cur_table, st_row, end_row)) {
      if (!CheckLocalClustUserRows(*cur_table, st_row, end_row)) {
        return;
      }
      // Populate the jobs_submit table with UPDATENOTE requests for the selected jobs.
      for (int row = st_row; row <= end_row; ++row) {
        SubmitUpdateNote(*cur_table, row);
      }
      m_cm->CommitJobSubmissionTable();
      AutoUpdateMe();
    }
    else {
      taMisc::Warning("No rows selected -- no notes were updated");
    }
  }
}

void ClusterRun::SaveState() {
  if(!InitClusterManager())
    return;
  
  jobs_submit.ResetData();
  
  DataTable* cur_table = GetCurDataTable();
  
  // Get the (inclusive) range of rows to update notes.
  int st_row, end_row;
  if (cur_table == &jobs_running) {
    if (SelectedRows(*cur_table, st_row, end_row)) {
      if (!CheckLocalClustUserRows(*cur_table, st_row, end_row)) {
        return;
      }
      // Populate the jobs_submit table with SAVESTATE requests for the selected jobs.
      for (int row = st_row; row <= end_row; ++row) {
        SubmitSaveState(*cur_table, row);
      }
      m_cm->CommitJobSubmissionTable();
      AutoUpdateMe();
    }
    else {
      taMisc::Warning("No rows selected -- no save state requests sent");
    }
  }
}

void ClusterRun::LoadData(bool remove_existing) {
  // note: can't call Update here because it unselects the rows in jobs_ tables!
  
  DataTable* cur_table = GetCurDataTable();
  
  taProject* proj = GetMyProj();
  if(!proj) return;
  DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("ClusterRun");
  dgp->save_tables = false;     // don't save -- prevents project bloat
  dgp->StructUpdate(true);
  if(remove_existing) {
    dgp->Reset();
  }
  
  int st_row, end_row;
  if (cur_table == &jobs_running) {
    if (SelectedRows(jobs_running, st_row, end_row)) {
      for (int row = st_row; row <= end_row; ++row) {
        LoadData_impl(dgp, jobs_running, row);
      }
    }
    else {
      for (int row = 0; row <jobs_running.rows; ++row) {
        LoadData_impl(dgp, jobs_running, row);
      }
    }
  }
  else if (cur_table == &jobs_done || cur_table == &jobs_archive || cur_table == &file_list) {
    if (SelectedRows(*cur_table, st_row, end_row)) {
      for (int row = st_row; row <= end_row; ++row) {
        LoadData_impl(dgp, *cur_table, row);
      }
    }
    else {
      taMisc::Warning("No rows selected -- no data loaded");
    }
  }
  dgp->StructUpdate(false);
  ClearAllSelections();       // done
}

void ClusterRun::LoadData_impl(DataTable_Group* dgp, const DataTable& table, int row) {
  String tag = table.GetValAsString("tag", row);
  if(TestWarning(tag.empty(), "LoadData", "tag is empty for row:", String(row),
                 "in table", table.name))
    return;
  String dat_files;
  String params;
  String lnotes;
  String llabel;
  String user = m_cm->GetUsername();
  String clust = cluster;
  if(table.name == "file_list") {
    dat_files = table.GetValAsString("file_name", row);
    if(!dat_files.contains(".dat")) return; // not a dat file!
    int lkup = jobs_done.FindVal(tag, "tag");
    if(lkup >= 0) {
      params = jobs_done.GetValAsString("params", lkup);
      lnotes = jobs_done.GetValAsString("notes", lkup);
      llabel = jobs_done.GetValAsString("label", lkup);
    }
    else {
      lkup = jobs_running.FindVal(tag, "tag");
      if(lkup >= 0) {
        params = jobs_running.GetValAsString("params", lkup);
        lnotes = jobs_running.GetValAsString("notes", lkup);
        llabel = jobs_running.GetValAsString("label", lkup);
      }
      else {
        lkup = jobs_archive.FindVal(tag, "tag");
        if(lkup >= 0) {
          params = jobs_archive.GetValAsString("params", lkup);
          lnotes = jobs_archive.GetValAsString("notes", lkup);
          llabel = jobs_archive.GetValAsString("label", lkup);
        }
      }
    }
  }
  else {
    dat_files = table.GetValAsString("dat_files", row);
    params = table.GetValAsString("params", row);
    lnotes = table.GetValAsString("notes", row);
    llabel = table.GetValAsString("label", row);
    user = table.GetValAsString("user", row);
    clust = table.GetValAsString("cluster", row);
  }
  if(TestWarning(dat_files.empty(), "LoadData", "dat_files is empty for tag:", tag))
    return;
  String tag_svn = tag.before("_");
  String tag_job = tag.after("_");
  String res_path = m_cm->GetWcResultsPath();
  res_path = m_cm->GetWcPath_UserClust(res_path, user, clust);
  String_Array files;
  files.FmDelimString(dat_files, " ");
  for(int i=0; i< files.size; i++) {
    String fl = files[i];
    String dnm = fl.before(".dat", -1);
    dnm = taMisc::StringCVar(dnm);
    DataTable* dat = dgp->FindName(dnm);
    if(dat) {
      dat->ResetData();
    }
    else {
      dat = dgp->NewEl(1);
      dat->name = dnm;
      dat->ClearDataFlag(DataTable::SAVE_ROWS); // don't save these by default!!
    }
    dat->LoadData(res_path + "/" + fl);
    AddParamsToTable(dat, tag, tag_svn, tag_job, params, lnotes, llabel);
  }
}

void ClusterRun::AddParamsToTable(DataTable* dat, const String& tag,
                                  const String& tag_svn, const String& tag_job,
                                  const String& params, const String& notes, const String& label) {
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
  {
    DataCol* cl = dat->FindMakeCol("notes", VT_STRING);
    cl->InitVals(notes);
  }
  {
    DataCol* cl = dat->FindMakeCol("label", VT_STRING);
    cl->InitVals(tag + ": " + label);
  }
  {
    DataCol* cl = dat->FindMakeCol("label_notag", VT_STRING);
    cl->InitVals(label);
  }
  
  String_Array pars;
  pars.FmDelimString(params, " ");
  if(params.empty())
    return;
  
  { // first add the params as a whole string -- useful for grouping..
    DataCol* cl = dat->FindMakeCol("params", VT_STRING);
    cl->InitVals(params);
  }
  
  for(int i=0; i<pars.size; i++) {
    String pv = pars[i];
    String nm = pv.before('=');
    String vl = pv.after('=');
    if(nm.empty()) {
      // taMisc::Warning("AddParamsToTable: name empty in param element:", pv, "from list:", params);
      continue;
    }
    if(vl.empty()) {
      // don't really need a warning here
      // taMisc::Warning("AddParamsToTable: value empty in param element:", pv, "from list:", params);
      continue;
    }
    // todo: we need to figure out what type of data these guys really are..
    DataCol* cl = dat->FindMakeCol(nm, VT_FLOAT);
    cl->InitVals((float)vl);
  }
}

void ClusterRun::CheckBackendRunning() {
  QDateTime clusterrun_backend_script_timestamp =
    QDateTime::fromString(clusterscript_timestamp.GetDataByName("timestamp").toQString(),
                          Qt::ISODate);
  clusterrun_backend_script_timestamp.setTimeSpec(Qt::UTC);
  if (clusterrun_backend_script_timestamp.secsTo(QDateTime().currentDateTime()) >
      60*60) {
    taMisc::Confirm("The last update from the cluster (", cluster, ") was at ", last_backend_checkin, " which is more than an hour ago. Please check that your script is actually running on the cluster");
  }
}

void ClusterRun::SelectCluster() {
  if(!InitClusterManager())
    return;
  String clust = m_cm->ChooseCluster("Select a cluster to use for this project:");
  if(clust.empty()) return;
  cluster = clust;
  
  
  
  if (m_cm->m_do_svn_update) {
    jobs_submit.ResetData();
    int dst_row = jobs_submit.AddBlankRow();
    jobs_submit.SetVal("PROBE", "status", dst_row);
    jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
    // Commit the table.
    m_cm->CommitJobSubmissionTable();
  }
  // this is not worth the risks if things are not configured properly:
  //  AutoUpdateMe();
  if (m_cm->m_do_svn_update) {
    Update();
  } else {
    Update_impl(m_cm->m_do_svn_update);
  }
  UpdateUI();
}

void ClusterRun::ComputeStats() {
  taProject* proj = GetMyProj();
  if(!proj) return;
  DataTable* stat_data = proj->GetNewAnalysisDataTable("ClusterRunCpuHrs", true);
  stat_data->StructUpdate(true);
  stat_data->ResetData();

  DataCol* dc;
  int idx;
  dc = stat_data->FindMakeCol("cluster", VT_STRING);
  dc->desc = "name of cluster to run job on";
  dc = stat_data->FindMakeCol("tag", VT_STRING);
  dc->desc = "unique tag id for this job -- all files etc are named according to this tag";
  dc = stat_data->FindMakeCol("cpu_hr", VT_FLOAT);
  dc->desc = "main stat: total cpu hours used: total_cpus * run_time_hr";
  dc = stat_data->FindMakeCol("run_time_hr", VT_FLOAT);
  dc->desc = "total running time in hours";
  dc = stat_data->FindMakeCol("n_threads", VT_FLOAT);
  dc->desc = "number of parallel threads to use for running";
  dc = stat_data->FindMakeCol("use_cuda", VT_BOOL);
  dc->desc = "if the job should be run with CUDA";
  dc = stat_data->FindMakeCol("mpi_nodes", VT_FLOAT);
  dc->desc = "number of physical nodes to use for mpi run -- 0 or -1 means not to use mpi";
  dc = stat_data->FindMakeCol("mpi_per_node", VT_FLOAT);
  dc->desc = "number of processes to use per MPI node to use for mpi run - total nodes is mpi_nodes * mpi_per_node";
  dc = stat_data->FindMakeCol("total_nodes", VT_FLOAT);
  dc->desc = "total nodes is mpi_nodes * mpi_per_node";
  dc = stat_data->FindMakeCol("total_cpus", VT_FLOAT);
  dc->desc = "total_nodes * total_threads";

  DataTable* cur_table = GetCurDataTable();
  
  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (cur_table == &jobs_running || cur_table == &jobs_done || cur_table == &jobs_archive) {
    if (SelectedRows(*cur_table, st_row, end_row)) {
      for (int row = st_row; row <= end_row; ++row) {
        ComputeStats_impl(stat_data, cur_table, row);
      }
    }
    else {
      ComputeStats_impl(stat_data, &jobs_done, -1); // all rows
      ComputeStats_impl(stat_data, &jobs_archive, -1); // all rows
      ComputeStats_impl(stat_data, &jobs_deleted, -1); // all rows
    }
  }
  stat_data->StructUpdate(false);

  DataTable* stats = proj->GetNewAnalysisDataTable("ClusterRunStats", true);

  DataGroupSpec gp_spec;
  gp_spec.append_agg_name = true;
  gp_spec.SetDataTable(stat_data);
  // DataGroupEl* cond_gp = (DataGroupEl*)gp_spec.AddColumn(, src_data);
  // cond_gp->agg.op = Aggregate::GROUP;

  String data_col_nm = "cpu_hr";
  DataGroupEl* sum_gp = (DataGroupEl*)gp_spec.AddColumn(data_col_nm, stat_data);
  sum_gp->agg.op = Aggregate::SUM;
  DataGroupEl* n_gp = (DataGroupEl*)gp_spec.AddColumn(data_col_nm, stat_data);
  n_gp->agg.op = Aggregate::N;
  DataGroupEl* mean_gp = (DataGroupEl*)gp_spec.AddColumn(data_col_nm, stat_data);
  mean_gp->agg.op = Aggregate::MEAN;
  mean_gp = (DataGroupEl*)gp_spec.AddColumn("run_time_hr", stat_data);
  mean_gp->agg.op = Aggregate::MEAN;
  mean_gp = (DataGroupEl*)gp_spec.AddColumn("total_cpus", stat_data);
  mean_gp->agg.op = Aggregate::MEAN;
  taDataProc::Group(stats, stat_data, &gp_spec);
}

void ClusterRun::ComputeStats_impl(DataTable* stats, DataTable* table, int row) {
  int st_row = row;
  int ed_row = row+1;
  if(row < 0) {
    st_row = 0;
    ed_row = table->rows;
  }
  for(int i=st_row; i<ed_row; i++) {
    stats->AddRow();
    String clust = table->GetValAsString("cluster", i);
    stats->SetValAsString(clust, "cluster", -1);
    String tag = table->GetValAsString("tag", i);
    stats->SetValAsString(tag, "tag", -1);
    float n_thr = table->GetValAsFloat("n_threads", i);
    stats->SetValAsFloat(n_thr, "n_threads", -1);
    float mpi_nod = table->GetValAsFloat("mpi_nodes", i);
    stats->SetValAsFloat(mpi_nod, "mpi_nodes", -1);
    float mpi_per_nod = table->GetValAsFloat("mpi_per_node", i);
    stats->SetValAsFloat(mpi_per_nod, "mpi_per_node", -1);
    float total_nodes = mpi_nod * mpi_per_nod;
    stats->SetValAsFloat(total_nodes, "total_nodes", -1);
    float total_cpus = n_thr * total_nodes;
    stats->SetValAsFloat(total_cpus, "total_cpus", -1);

    float secs = RunningTimeSecs(table, i);
    float hrs = secs / 3600.0f;
    stats->SetValAsFloat(hrs, "run_time_hr", -1);

    float cpu_hr = total_cpus * hrs;
    stats->SetValAsFloat(cpu_hr, "cpu_hr", -1);
    stats->WriteClose();
  }
}

void ClusterRun::ListJobFiles() {
  if(!InitClusterManager())
    return;
  bool include_data = true;     // doesn't hurt..
  file_list.ResetData();
  
  DataTable* cur_table = GetCurDataTable();
  
  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (cur_table == &jobs_running || cur_table == &jobs_done || cur_table == &jobs_archive) {
    if (SelectedRows(*cur_table, st_row, end_row)) {
      for (int row = st_row; row <= end_row; ++row) {
        SelectFiles_impl(*cur_table, row, include_data);
      }
    }
    else {
      taMisc::Warning("No rows selected -- no files selected");
    }
  }
  
  ClearAllSelections();       // done
  ViewPanelNumber(5);
}

void ClusterRun::SelectFiles_impl(DataTable& table, int row, bool include_data) {
  String tag = table.GetValAsString("tag", row);
  String dat_files = table.GetValAsString("dat_files", row);
  String other_files = table.GetValAsString("other_files", row);
  String user = table.GetValAsString("user", row);
  String clust = table.GetValAsString("cluster", row);
  {                             // other files
    String_Array files;
    files.FmDelimString(other_files, " ");
    for(int i=0; i< files.size; i++) {
      String fl = files[i];
      int frow = file_list.AddBlankRow();
      GetFileInfo(fl, file_list, frow, tag, user, clust);
    }
  }
  if(include_data) {
    String_Array files;
    files.FmDelimString(dat_files, " ");
    for(int i=0; i< files.size; i++) {
      String fl = files[i];
      int frow = file_list.AddBlankRow();
      GetFileInfo(fl, file_list, frow, tag, user, clust);
    }
  }
}

void ClusterRun::ListLocalFiles() {
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
      GetFileInfo(files[i].filePath(), file_list, frow, tag, m_cm->GetUsername(), cluster);
    }
  }
  ViewPanelNumber(5);
}

void ClusterRun::GetFileInfo(const String& path, DataTable& table, int row, String& tag,
                             const String& user, const String& clust) {
  String fl = taMisc::GetFileFmPath(path);
  String wc_res_path = m_cm->GetWcResultsPath();
  wc_res_path = m_cm->GetWcPath_UserClust(wc_res_path, user, clust);
  String repo_path = wc_res_path.from(svn_repo + "/");

  String flpath = wc_res_path + "/" + fl;

  table.SetVal(fl, "file_name",  row);
  table.SetVal(flpath, "file_path",  row);
  table.SetVal(repo_path + "/" + fl, "svn_file_path",  row);
  table.SetVal("results/" + fl, "proj_file_path",  row);
  table.SetVal(user, "user", row);
  table.SetVal(clust, "cluster", row);

  if(fl.endsWith(".dat") || fl.endsWith(".dat.gz"))
    table.SetVal("Data", "kind",  row);
  else if(fl.endsWith(".args"))
    table.SetVal("Startup Args", "kind",  row);
  else if(fl.endsWith(".wts") || fl.endsWith(".wts.gz"))
    table.SetVal("Weights", "kind",  row);

  if(tag.empty()) {
    String fnm = taMisc::GetFileFmPath(m_cm->GetFilename());
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
  if(!InitClusterManager())
    return;

  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if(!SelectedRows(file_list, st_row, end_row)) {
    taMisc::Warning("No rows selected -- no files fetched");
    return;
  }

  String wc_path = m_cm->GetWcResultsPath();

  String_Array sub_files;
  for (int row = st_row; row <= end_row; ++row) {
    String fl = file_list.GetValAsString("file_name", row);
    String fp = file_list.GetValAsString("file_path", row);
    if(taMisc::FileExists(fp)) {
      String local_fl = wc_path + "/" + fl;
      taMisc::Info("Copying local file from:", fp, "to:", local_fl, "and adding to svn");
      if(QFile::copy(fp, local_fl)) {
        taMisc::Busy();
        m_cm->AddFile(local_fl);
        m_cm->CommitFiles(String("added file from another svn project / cluster / user to this project's results files: ") + fl);
        taMisc::DoneBusy();
      }
      else {
        taMisc::RemoveFile(local_fl); // don't leave bad one lying around
      }
    }
    else {
      if(CheckLocalClustUser(file_list, row, false)) { // false = no warnings
        // not-local, current user and cluster, just get it from cluster
        taMisc::Info("Getting remote svn file from cluster:", fl);
        sub_files.Add(fl);
      }
      else {
        // not-local, not current user etc -- copy it to us
        String svnp = file_list.GetValAsString("svn_file_path", row);
        String svnurl = taMisc::GetDirFmPath(svnp);
        String local_fl = wc_path + "/" + fl;
        taMisc::Info("Getting non-local svn file from:", svnp, "to:", local_fl,
                     "and adding to local svn");
        InitOtherSvn(wc_path, m_cm->GetFullUrl()); // just use current..
        taMisc::Busy();
        try {
          svn_other->SaveFile(svnp, local_fl);
        }
        catch (const SubversionClient::Exception &ex) {
          taMisc::Error("Error getting file.\n\nPossible reason - If file belongs to another user and it is not one automatically checked into the svn repository the 'get' will fail if the other user has not already retrieved the file themself - but no harm in trying\n\n", ex.what());
          taMisc::RemoveFile(local_fl); // don't leave bad one lying around
          taMisc::DoneBusy();
          return;
        }
        m_cm->AddFile(local_fl);
        m_cm->CommitFiles(String("added file from another svn project / cluster / user to this project's results files: ") + fl);
        taMisc::DoneBusy();
      }
    }
  }
  if(sub_files.size > 0) {
    String files_str = sub_files.ToDelimString(" ");
    jobs_submit.ResetData();
    SubmitGetFiles(files_str);
    // Commit the table.
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
}

void ClusterRun::CleanJobFiles() {
  if(!InitClusterManager())
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

void ClusterRun::DeleteFiles() {
  if(!InitClusterManager())
    return;

  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (SelectedRows(file_list, st_row, end_row)) {
    String_PArray files;
    String_Array rmt_files;
    for (int row = end_row; row >= st_row; --row) {
      String fpath = file_list.GetValAsString("file_path", row);
      if(!taMisc::FileExists(fpath)) {
        String fl = file_list.GetValAsString("file_name", row);
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
      SubmitDeleteFiles(files_str);
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
      DeleteAllFilesInList();
    }
    else if (SelectedRows(jobs_archive, st_row, end_row)) {
      file_list.ResetData();
      for (int row = end_row; row >= st_row; --row) {
        SelectFiles_impl(jobs_archive, row, true); // include data
      }
      DeleteAllFilesInList();
    }
    else {
      taMisc::Warning("No rows selected in either file_list or jobs_done or jobs_archive -- no files removed");
    }
  }
}

void ClusterRun::DeleteNonDataFiles() {
  if(!InitClusterManager())
    return;
  
  DataTable* cur_table = GetCurDataTable();
  
  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (cur_table == &jobs_done || cur_table == &jobs_archive) {
    if (SelectedRows(*cur_table, st_row, end_row)) {
      file_list.ResetData();
      for (int row = end_row; row >= st_row; --row) {
        SelectFiles_impl(*cur_table, row, false); // NOT include data
      }
      DeleteAllFilesInList();
    }
    else {
      taMisc::Warning("No rows selected in jobs_done or jobs_archive -- no files removed");
    }
  }
}

void ClusterRun::GetProjAtRev() {
  if(!InitClusterManager())
    return;

  DataTable* cur_table = GetCurDataTable();
  
  int svn_rev = -1;
  String user_name;
  String pcluster;
  String orig_filename;
  // Get the (inclusive) range of rows to process
  int st_row, end_row;
  if (cur_table == &jobs_running || cur_table == &jobs_done || cur_table == &jobs_archive) {
    if (SelectedRows(*cur_table, st_row, end_row)) {
      if(TestError(st_row != end_row, "GetProjAtRev", "must select only one row"))
        return;
      svn_rev = cur_table->GetVal("submit_svn", st_row).toInt();
      user_name = cur_table->GetVal("user", st_row).toString();
      pcluster = cur_table->GetVal("cluster", st_row).toString();
      orig_filename = cur_table->GetVal("filename", st_row).toString();
    }
    else {
      taMisc::Warning("No rows selected -- project not loaded");
    }
  }
  if(TestError(svn_rev < 0, "GetProjAtRev", "valid svn revision not found"))
    return;
  m_cm->GetProjectAtRev(pcluster, user_name, orig_filename, svn_rev);
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
  // taMisc::Info("ClusterRun Other SVN set to url:", svn_other_url, "working copy:",
  //              svn_other_wc_path);
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
  ViewPanelNumber(5);
}

void ClusterRun::ListOtherProjFiles(const String& proj_nm) {
  if(!InitClusterManager())
    return;

  String url = m_cm->GetFullUrl();
  if(TestError(url.empty(), "ListOtherProjFiles", "our url is empty -- do probe or update first"))
    return;
  String us_user = m_cm->GetUsername();
  String cur_proj = taMisc::GetFileFmPath(m_cm->GetFilename());
  cur_proj = cur_proj.before(".proj");
  String wc_path = m_cm->GetWcResultsPath();
  String proj_path = wc_path.after(us_user,-1);
  proj_path.gsub(cur_proj, proj_nm);
  url += proj_path;
  wc_path.gsub(cur_proj, proj_nm);

  InitOtherSvn(wc_path, url);
  ListOtherSvn();               // use defaults
}

void ClusterRun::Cleanup() {
  if(!InitClusterManager())
    return;
  m_cm->Cleanup();
}

void ClusterRun::OpenSvnBrowser() {
  if(!InitClusterManager())
    return;
  String url = m_cm->GetFullUrl();
  String us_user = m_cm->GetUsername();
  String wc_path = m_cm->GetWcResultsPath();
  String wc_root = wc_path.through(us_user,-1);
  iSubversionBrowser::OpenBrowser(url, wc_root);
}

void ClusterRun::SaveJobParams() {
  DataTable* cur_table = GetCurDataTable();
  
  int st_row, end_row;
  if (cur_table == &jobs_running || cur_table == &jobs_done || cur_table == &jobs_archive) {
    if (SelectedRows(*cur_table, st_row, end_row)) {
      for (int row = st_row; row <= end_row; ++row) {
        SaveJobParams_impl(*cur_table, row);
      }
    }
    else {
      taMisc::Warning("No rows selected -- no job parameters saved");
    }
  }
}

void ClusterRun::SaveJobParams_impl(DataTable& table, int row) {
  taProject* proj = GetMyProj();
  if(!proj) return;
  ParamSet* ps = proj->archived_params.NewArchive(); // insert at top

  String tag = table.GetValAsString("tag", row);
  String params = table.GetValAsString("params", row);
  String lnotes = table.GetValAsString("notes", row);
  // String llabel = table.GetValAsString("label", row); // todo: unused!?

  ps->name = String("tag_") + tag;
  ps->desc = lnotes;

  SaveNameValueMembers(ps, params);

  ps->UpdateAfterEdit();
  ps->BrowserSelectMe();        // sure..
}

void ClusterRun::ArchiveJobs() {
  if(!InitClusterManager())
    return;

  int st_row, end_row;
  if (SelectedRows(jobs_done, st_row, end_row)) {
    if (!CheckLocalClustUserRows(jobs_done, st_row, end_row)) {
      return;
    }
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

void ClusterRun::UnDeleteJobs() {
  if(!InitClusterManager())
    return;

  int st_row, end_row;
  if (SelectedRows(jobs_deleted, st_row, end_row)) {
    if (!CheckLocalClustUserRows(jobs_deleted, st_row, end_row)) {
      return;
    }

    jobs_submit.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SubmitUnDeleteJob(jobs_deleted, row);
    }
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
  else {
    taMisc::Warning("No rows selected -- no jobs archived");
  }
}

void ClusterRun::DeleteJobs() {
  if(!InitClusterManager())
    return;
  
  int st_row, end_row;
  if (SelectedRows(jobs_done, st_row, end_row)) {
    if (!CheckLocalClustUserRows(jobs_done, st_row, end_row)) {
      return;
    }
    int chs = taMisc::Choice("DeleteJobs: Are you sure you want to remove: " + String(1 + end_row - st_row) + " jobs from the jobs_done list (can only be your jobs, on current cluster)?", "Ok", "Cancel");
    if(chs == 1) return;
    jobs_submit.ResetData();
    file_list.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SubmitDeleteJob(jobs_done, row);
    }
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
  else if (SelectedRows(jobs_archive, st_row, end_row)) {
    if (!CheckLocalClustUserRows(jobs_archive, st_row, end_row)) {
      return;
    }
    int chs = taMisc::Choice("DeleteJobs: Are you sure you want to remove: " + String(1 + end_row - st_row) + " jobs from the jobs_archive list?", "Ok", "Cancel");
    if(chs == 1) return;
    jobs_submit.ResetData();
    file_list.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SubmitDeleteJob(jobs_archive, row);
    }
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
  else if (SelectedRows(jobs_deleted, st_row, end_row)) {
    if (!CheckLocalClustUserRows(jobs_deleted, st_row, end_row)) {
      return;
    }
    int chs = taMisc::Choice("DeleteJobs: Are you sure you want to remove: " + String(1 + end_row - st_row) + " jobs from the jobs_deleted list?", "Ok", "Cancel");
    if(chs == 1) return;
    jobs_submit.ResetData();
    file_list.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SubmitDeleteDelJob(jobs_deleted, row);
    }
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
  else {
    taMisc::Warning("No rows selected -- no jobs removed");
  }
}

void ClusterRun::NukeJobs() {
  if(!InitClusterManager())
    return;
  
  int st_row, end_row;
  if (SelectedRows(jobs_done, st_row, end_row)) {
    if (!CheckLocalClustUserRows(jobs_done, st_row, end_row)) {
      return;
    }
    int chs = taMisc::Choice("NukeJobs: Are you sure you want to nuke: " + String(1 + end_row - st_row) + " jobs from the jobs_done list (can only be your jobs, on current cluster)?", "Ok", "Cancel");
    if(chs == 1) return;
    jobs_submit.ResetData();
    file_list.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SubmitNukeJob(jobs_done, row);
    }
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
  else if (SelectedRows(jobs_archive, st_row, end_row)) {
    if (!CheckLocalClustUserRows(jobs_archive, st_row, end_row)) {
      return;
    }
    int chs = taMisc::Choice("NukeJobs: Are you sure you want to nuke: " + String(1 + end_row - st_row) + " jobs from the jobs_archive list?", "Ok", "Cancel");
    if(chs == 1) return;
    jobs_submit.ResetData();
    file_list.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SubmitNukeJob(jobs_archive, row);
    }
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
  else if (SelectedRows(jobs_deleted, st_row, end_row)) {
    if (!CheckLocalClustUserRows(jobs_deleted, st_row, end_row)) {
      return;
    }
    int chs = taMisc::Choice("NukeJobs: Are you sure you want to nuke: " + String(1 + end_row - st_row) + " jobs from the jobs_deleted list?", "Ok", "Cancel");
    if(chs == 1) return;
    jobs_submit.ResetData();
    file_list.ResetData();
    for (int row = end_row; row >= st_row; --row) {
      SubmitDeleteDelJob(jobs_deleted, row);
    }
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
  else {
    taMisc::Warning("No rows selected -- no jobs nuked");
  }
}

void ClusterRun::DeleteKilledJobs() {
  if(!InitClusterManager())
    return;
  jobs_submit.ResetData();
  file_list.ResetData();
  for (int row = jobs_done.rows-1; row >= 0; --row) {
    String status = jobs_done.GetValAsString("status", row);
    if(status != "KILLED") continue;
    SubmitDeleteJob(jobs_done, row);
  }
  if(jobs_submit.rows > 0) {
    m_cm->CommitJobSubmissionTable();
    AutoUpdateMe();
  }
}

void ClusterRun::DeleteAllFilesInList() {
  String_PArray files;
  String_Array rmt_files;
  for(int i=0;i<file_list.rows; i++) {
    String fpath = file_list.GetValAsString("file_path", i);
    if(!taMisc::FileExists(fpath)) {
      String fl = file_list.GetValAsString("file_name", i);
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
    SubmitDeleteFiles(files_str);
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

  // note: saving a bit of space with all the tabs at the top:
  jobs_running.name = "running";
  jobs_done.name = "done";
  jobs_deleted.name = "deleted";
  jobs_archive.name = "archive";

  jobs_running_tmp.name = "jobs_running_tmp";
  jobs_done_tmp.name = "jobs_done_tmp";
  jobs_archive_tmp.name = "jobs_archive_tmp";
  jobs_running_cmd.name = "jobs_running_cmd";

  FormatJobTable(jobs_submit);
  FormatJobTable(jobs_submitted);

  FormatJobTable(jobs_running, true); // extra fields
  FormatJobTable(jobs_done, true);
  FormatJobTable(jobs_deleted, true);
  FormatJobTable(jobs_archive, true);

  FormatJobTable(jobs_running_tmp);
  FormatJobTable(jobs_done_tmp);
  FormatJobTable(jobs_deleted_tmp);
  FormatJobTable(jobs_archive_tmp);
  FormatJobTable(jobs_running_cmd);

  file_list.name = "files";
  FormatFileListTable(file_list);

  cluster_info.name = "cluster";
  clusterscript_timestamp.name = "clusterscript_timestamp";
  FormatClusterInfoTable(cluster_info);
  FormatClusterScriptTimestampTable(clusterscript_timestamp);
}

void ClusterRun::FormatJobTable(DataTable& dt, bool clust_user) {
  DataCol* dc;
  int idx;

  // NOTE: please update ClusterRunJob with any updates to job tables!

  if(taMisc::is_loading) {
    taVersion v803(8, 0, 3);
    if(taMisc::loading_version < v803) {
      dt.row_height = 2;           // good default now that it is avail
    }
  }

  dt.ClearDataFlag(DataTable::SAVE_ROWS);

  if(clust_user) {
    dc = dt.FindMakeCol("cluster", VT_STRING);
    dc->desc = "cluster where this job was submitted / run";
    idx = dc->col_idx;
    if(idx != 0) {
      dt.MoveCol(idx, 0);
    }

    dc = dt.FindMakeCol("user", VT_STRING);
    dc->desc = "user who ran this job";
    idx = dc->col_idx;
    if(idx != 1) {
      dt.MoveCol(idx, 1);
    }
  }
  
  dc = dt.FindMakeCol("tag", VT_STRING);
  dc->desc = "unique tag id for this job -- all files etc are named according to this tag";
  int tag_idx = dc->col_idx;
  dc = dt.FindMakeCol("label", VT_STRING);
  dc->desc = "label for the job -- a brief description that you can use to label this job's results on your graph -- in general notes should have more general info and then label should JUST describe what is unique about the current run";
  int label_idx = dc->col_idx;
  dc = dt.FindMakeCol("notes", VT_STRING);
  dc->desc = "notes for the job -- describe any specific information about the model configuration etc -- can use this for searching and sorting results";
  int notes_idx = dc->col_idx;
  if(label_idx > notes_idx) {   // move label in front of notes!
    dt.MoveCol(label_idx, notes_idx);
  }
  dc = dt.FindMakeCol("filename", VT_STRING);
  dc->desc = "name of the specific project used for this job -- because multiple versions of a model are often run under the same project name";
  dc = dt.FindMakeCol("params", VT_STRING);
  dc->desc = "emergent parameters based on currently selected items in the ClusterRun";

  // The client sets this field in the jobs_submit table to:
  //   REQUESTED to request the job be submitted.
  //   CANCELLED to request the job indicated by job_no or tag be cancelled.
  //   PROBE     probe to get the cluster to track this project, and update all running
  //   GETDATA   get the data for the associated tag -- causes cluster to check in dat_files
  //   GETFILES  tell cluster to check in all files listed in this other_files entry
  //   REMOVEJOB tell cluster to remove given tags from jobs_done and add to jobs_deleted
  //   NUKEJOB   remove given tag job from jobs done immediately -- do not add to jobs deleted
  //   CLEANJOBFILES tell cluster to remove job files associated with tags
  //   REMOVEFILES tell cluster to remove specific files listed in this other_files entry
  //   UPDATENOTE update the notes, label field of a job
  //   ARCHIVEJOB tell cluster to move given tags from jobs_done into jobs_archive
  //   UNDELETEJOB tell cluster to move given tags from jobs_delete back into jobs_done, and recover dat files associated with those jobs back into the current svn directory
  //   REMOVEDELJOB tell cluster to fully remove job from jobs_delete
  //   SAVESTATE send a command to running job through jobs_running_cmd.dat file to save state
  
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
  if(clust_user) {
    if(dc->col_idx != tag_idx + 1)
      dt.MoveCol(dc->col_idx, tag_idx+1);
  }
  dc = dt.FindMakeCol("status_info", VT_STRING);
  dc->desc = "more detailed information about status";
  // if(clust_user) {
  //   if(dc->col_idx != tag_idx + 2)
  //     dt.MoveCol(dc->col_idx, tag_idx+2);
  // }

  dc = dt.FindMakeCol("submit_time", VT_STRING);
  dc->desc = "when was the job submitted (tracks time from emergent client submission)";
  dc = dt.FindMakeCol("start_time", VT_STRING);
  dc->desc = "when did the job actually start running";
  dc = dt.FindMakeCol("end_time", VT_STRING);
  dc->desc = "when did the job finish running";
  if (!dt.name.contains_ci("tmp")) {
    dc = dt.FindMakeCol("running_time", VT_STRING);
    dc->desc = "total running time in days/hours/minutes";
  }
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
  dc = dt.FindMakeCol("use_cuda", VT_INT);
  dc->desc = "if the job should be run with CUDA";
  dc = dt.FindMakeCol("mpi_nodes", VT_INT);
  dc->desc = "number of physical nodes to use for mpi run -- 0 or -1 means not to use mpi";
  int nodes_idx = dt.FindColNameIdx(dc->name);
  dc = dt.FindMakeCol("mpi_per_node", VT_INT);
  dc->desc = "number of processes to use per MPI node to use for mpi run - total nodes is mpi_nodes * mpi_per_node";
  idx = dt.FindColNameIdx(dc->name);
  if(idx != nodes_idx + 1) {
    dt.MoveCol(idx, nodes_idx + 1);
  }
  dc = dt.FindMakeCol("pb_batches", VT_INT);
  dc->desc = "if > 0, use parallel batch mode with this number of batches";
  dc = dt.FindMakeCol("pb_nodes", VT_INT);
  dc->desc = "actually pb_n_batches_per -- re-using this parameter that was previously used for a different purpose -- number of batches to run sequentially within a single job";
  
  // these two comprise the tag -- internal stuff user doesn't need to see
  dc = dt.FindMakeCol("submit_svn", VT_STRING);
  dc->desc = "svn revision for the original job submission";
  dc = dt.FindMakeCol("submit_job", VT_STRING);
  dc->desc = "index of job number within a given submission -- equal to the row number of the original set of jobs submitted in submit_svn jobs";
  dc = dt.FindMakeCol("done_svn", VT_STRING);
  dc->desc = "svn revision when this job was moved from running to done -- this will contain full set of files generated when running -- for deleted jobs can also recover to this";
  dc = dt.FindMakeCol("last_svn", VT_STRING);
  dc->desc = "last svn revision for command submission that affected this job in some significant way -- for deleted jobs, this is the svn revision that we recover to";
  
  for (int i=0; i<dt.data.size; i++) {
    dc = dt.data.SafeEl(i);
    dc->SetColFlag(DataCol::READ_ONLY);
  }
  dc = dt.FindColName("notes");
  dc->ClearColFlag(DataCol::READ_ONLY);
  dc = dt.FindColName("label");
  dc->ClearColFlag(DataCol::READ_ONLY);
}

bool ClusterRun::LoadMyRunningTable() {
  if(!InitClusterManager())
    return false;
  return m_cm->LoadMyRunningTable();
}

bool ClusterRun::LoadMyRunningCmdTable() {
  if(!InitClusterManager())
    return false;
  return m_cm->LoadMyRunningCmdTable();
}

bool ClusterRun::SaveMyRunningCmdTable() {
  if(!InitClusterManager())
    return false;
  return m_cm->SaveMyRunningCmdTable();
}

bool ClusterRun::GetCurJobData(const String& tag) const {
  ClusterRunJob::MakeCurJobObj();
  return GetJobData(*(ClusterRunJob::cur_job), tag);
}

bool ClusterRun::GetJobData(ClusterRunJob& job_data, const String& tag) const {
  if(GetJobData_impl(job_data, tag, jobs_running)) return true;
  if(GetJobData_impl(job_data, tag, jobs_done)) return true;
  if(GetJobData_impl(job_data, tag, jobs_deleted)) return true;
  if(GetJobData_impl(job_data, tag, jobs_archive)) return true;
  return false;
}

bool ClusterRun::GetJobData_impl(ClusterRunJob& job_data, const String& tag, const DataTable& dt) const {
  int row = dt.FindVal(tag, "tag", 0, false);
  if(row < 0) return false;

  job_data.tag = dt.GetValAsString("tag", row);
  job_data.label = dt.GetValAsString("label", row);
  job_data.notes = dt.GetValAsString("notes", row);
  job_data.filename = dt.GetValAsString("filename", row);
  job_data.params = dt.GetValAsString("params", row);
    
  job_data.status = dt.GetValAsString("status", row);
  job_data.status_info = dt.GetValAsString("status_info", row);
  job_data.submit_time.fromString(dt.GetValAsString("submit_time", row), timestamp_fmt);
  job_data.start_time.fromString(dt.GetValAsString("start_time", row), timestamp_fmt);
  job_data.end_time.fromString(dt.GetValAsString("end_time", row), timestamp_fmt);
  job_data.running_time = dt.GetValAsString("running_time", row);
  job_data.job_no = dt.GetValAsString("job_no", row);
  job_data.job_out = dt.GetValAsString("job_out", row);
  job_data.job_out_file = dt.GetValAsString("job_out_file", row);
  job_data.dat_files = dt.GetValAsString("dat_files", row);
  job_data.other_files = dt.GetValAsString("other_files", row);

  job_data.command_id = dt.GetValAsInt("command_id", row);
  job_data.command = dt.GetValAsString("command", row);

  job_data.repo_url = dt.GetValAsString("repo_url", row);
  job_data.cluster = dt.GetValAsString("cluster", row);
  job_data.user = dt.GetValAsString("user", row);
  job_data.queue = dt.GetValAsString("queue", row);
  job_data.run_time = dt.GetValAsString("run_time", row);
  job_data.ram_gb = dt.GetValAsInt("ram_gb", row);
  job_data.n_threads = dt.GetValAsInt("n_threads", row);
  job_data.mpi_nodes = dt.GetValAsInt("mpi_nodes", row);
  job_data.mpi_per_node = dt.GetValAsInt("mpi_per_node", row);
  job_data.pb_batches = dt.GetValAsInt("pb_batches", row);
  job_data.pb_nodes = dt.GetValAsInt("pb_nodes", row);

  job_data.submit_svn = dt.GetValAsString("submit_svn", row);
  job_data.submit_job = dt.GetValAsString("submit_job", row);
  job_data.done_svn = dt.GetValAsString("done_svn", row);
  job_data.last_svn = dt.GetValAsString("last_svn", row);

  job_data.UpdateAfterEdit();
  
  return true;
}

int ClusterRun::RunTimeMins(const String& run_time) {
  int rtm = 0;                      // run time in minutes
  if(run_time.endsWith('m')) {
    rtm = (int)run_time.before('m');
  }
  else if(run_time.endsWith('h')) {
    rtm = (int)run_time.before('h') * 60;
  }
  else if(run_time.endsWith('d')) {
    rtm = (int)run_time.before('d') * 24 * 60;
  }
  return rtm;
}

void ClusterRun::FormatFileListTable(DataTable& dt) {
  DataCol* dc;

  dt.ClearDataFlag(DataTable::SAVE_ROWS);

  dc = dt.FindMakeCol("cluster", VT_STRING);
  dc->desc = "cluster where this job was submitted / run";
  int idx = dt.FindColNameIdx(dc->name);
  if(idx != 0) {
    dt.MoveCol(idx, 0);
  }

  dc = dt.FindMakeCol("user", VT_STRING);
  dc->desc = "user who ran this job";
  idx = dt.FindColNameIdx(dc->name);
  if(idx != 1) {
    dt.MoveCol(idx, 1);
  }

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
  
  for (int i=0; i<dt.data.size; i++) {
    dc = dt.data.SafeEl(i);
    dc->SetColFlag(DataCol::READ_ONLY);
  }
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
  
  for (int i=0; i<dt.data.size; i++) {
    dc = dt.data.SafeEl(i);
    dc->SetColFlag(DataCol::READ_ONLY);
  }
}

void ClusterRun::FormatClusterScriptTimestampTable(DataTable& dt) {
  DataCol* dc;
  
  dt.ClearDataFlag(DataTable::SAVE_ROWS);
  
  dc = dt.FindMakeCol("timestamp", VT_STRING);
  dc->desc = "Timestamp of last interaction from the cluster run script";
  
  dc = dt.FindMakeCol("version", VT_INT);
  dc->desc = "Version number of cluster run script ";
  
  for (int i=0; i<dt.data.size; i++) {
    dc = dt.data.SafeEl(i);
    dc->SetColFlag(DataCol::READ_ONLY);
  }
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
  int rtm = RunTimeMins(run_time);
  if(rtm == 0) {
    taMisc::Error("run_time is 0 -- you MUST specify a non-zero run time -- syntax is number followed by unit indicator -- m=minutes, h=hours, d=days -- e.g., 30m, 12h, or 2d -- typically the job will be killed if it exceeds this amount of time, so be sure to not underestimate!");
    return false;
  }

  int rth = rtm / 60;           // hours
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
    if(mpi_nodes < 1) {
      taMisc::Error("Job requests to use MPI but mpi_nodes is less than 1 -- must be at least 1", String(mpi_nodes));
      return false;
    }
    if(mpi_per_node < 1) {
      taMisc::Error("Job requests to use MPI but mpi_per_node is less than 1 -- must be at least 1", String(mpi_per_node));
      return false;
    }
    if(mpi_nodes > cs.nodes) {
      taMisc::Error("Job requests to use MPI with more nodes than is available on cluster:", cluster, "mpi_nodes requested:", String(mpi_nodes), "avail on cluster:", String(cs.nodes));
      return false;
    }
    if(mpi_per_node * n_threads > cs.procs_per_node) {
      taMisc::Error("Job requests to use MPI with more per_node * n_threads processors than is available on cluster:", cluster, "mpi_per_node * n_threads requested:", String(mpi_per_node * n_threads), "avail on cluster:", String(cs.procs_per_node));
      return false;
    }
  }

  if(nowin_x && cs.gpus <= 0) {
    taMisc::Error("Job requests to use -nowin and XWindows, but this requires GPUs on the cluster nodes -- currently it is listed as having none -- either fix the cluster config or select a different config");
    return false;
  }

  if(parallel_batch) {
    if(pb_batches <= 1) {
      taMisc::Error("Job requests to use parallel_batches but pb_batches is less than or equal to 1",
                    String(pb_batches));
      return false;
    }
    if(pb_n_batches_per < 1) {
      taMisc::Error("pb_n_batches must be greater than or equal to 1",
                    String(pb_n_batches_per));
      return false;
    }
    if(pb_batches % pb_n_batches_per != 0) {
      taMisc::Warning("pb_batches (", String(pb_batches),
                      ") is not evenly divisible by pb_n_batches (",
                      String(pb_n_batches_per), ") -- still works but is less efficient");
    }
  }
  // if(cs.by_node && cs.procs_per_node <= 1) {
  //   taMisc::Error("Cluster:", cluster, "says allocate by node but procs_per_node is not set -- must set this parameter in Preferences / Options settings");
  //   return false;
  // }
  // if(cs.by_node && cs.procs_per_node > 4 && !(use_mpi || parallel_batch)) {
  //   taMisc::Error("Cluster:", cluster, "has allocate by node policy, but you are not requesting either an mpi or a parallel batch job -- this will waste compute allocations -- please use a cluster that is more appropriate for single-shot single-processor jobs");
  //   return false;
  // }
  if(cs.max_jobs > 0) {
    if(n_jobs_to_sub > cs.max_jobs) {
      int chs = taMisc::Choice("You are requesting to run more than listed max number of jobs on cluster: " + cluster + " -- jobs: " + String(n_jobs_to_sub) + " max: " +
                               String(cs.max_jobs), "Continue Anyway", "Cancel");
      if(chs == 1) return false;
    }
  }
  int tot_procs = n_jobs_to_sub * n_threads;
  if(use_mpi) tot_procs *= mpi_nodes * mpi_per_node;
  bool already_said_yes = false;

  taMisc::Info("total procs requested for this job:", String(tot_procs));

  if(cs.max_procs > 0) {
    if(tot_procs > cs.max_procs) {
      int chs = taMisc::Choice("You are requesting to run more than listed max number of processors on cluster: " + cluster + " -- procs requested: " + String(tot_procs) + " max: " +
                               String(cs.max_procs), "Continue Anyway", "Cancel");
      if(chs == 1) return false;
      already_said_yes = true;
    }
  }

  if(cs.min_procs > 0) {
    if(tot_procs < cs.min_procs) {
      int chs = taMisc::Choice("You are requesting to run LESS than listed min number of processors on cluster: " + cluster + " -- procs requested: " + String(tot_procs) + " min: " +
                               String(cs.min_procs), "Continue Anyway", "Cancel");
      if(chs == 1) return false;
    }
  }

  if(!already_said_yes && tot_procs > cs.procs) {
    int chs = taMisc::Choice("You are requesting to run more than listed TOTAL number of processors on cluster: " + cluster + " -- procs requested: " + String(tot_procs) + " procs: " +  String(cs.procs), "Continue Anyway", "Cancel");
    if(chs == 1) return false;
    already_said_yes = true;
  }

  if(cs.max_ram > 0 && ram_gb > 0 && ram_gb > cs.max_ram)  {
    int chs = taMisc::Choice("You are requesting to run more than listed max ram on cluster: " + cluster + " -- ram in Gb requested: " + String(ram_gb) + " max: " +
                             String(cs.max_ram), "Continue Anyway (NOT recommended)", "Cancel");
    if(chs == 1) return false;
  }

  // final sanity check for large-ish jobs..
  int tot_tasks = tot_procs / n_threads;
  if(!already_said_yes && tot_tasks > 24) {
    int chs = taMisc::Choice("You are requesting to run a job using: " + String(tot_tasks) + " total processes on cluster: " + cluster + " using a total of: " + String(tot_procs) + " cores -- please confirm!", "Run", "Cancel");
    if(chs == 1) return false;
  }

  return true;
}

/**
 * Expand the label to replace variables with their respective value. Particularly useful to label search runs.
 */
String ClusterRun::ReplaceVars(const String& str) {
  int start_pos = 0;
  int end_pos = 0;
  int idx = -1;
  String label_expanded = str;

  idx = label_expanded.index('%',start_pos);

  //Iterate over all % in the label string
  while (idx >= 0) {
    end_pos = label_expanded.index(' ', idx);
    if (end_pos < 0)
      end_pos = label_expanded.length();
    //Extract the variable name to be expanded out.
    String var_name = label_expanded.at(idx + 1, end_pos - idx - 1);
    if(var_name.endsWith(',') || var_name.endsWith('.')) {
      var_name = var_name.before(var_name.length()-1);
    }
                         
    //Search over all parameters in cluster run to find the variable by name
    FOREACH_ELEM_IN_GROUP(ControlPanelMember, mbr, mbrs) {
      if (var_name == mbr->GetName()) {
        String variable_value;
        const ControlPanelMemberData &ps = mbr->data;

        //If we are in a search algorithm, then we need to use the value
        //set in the search parameters
        if (!use_search_algo || !cur_search_algo || !mbr->data.is_numeric || !ps.IsSearch()) {
          variable_value = mbr->CurValAsString();
        }
        else {
          variable_value = String(ps.next_val);
        }
        label_expanded = label_expanded.before(idx) + variable_value
          + label_expanded.after(end_pos - 1);
      }
    }

    start_pos = idx + 1;
    idx = label_expanded.index('%', start_pos);
  }

  return label_expanded;
}

void
ClusterRun::AddJobRow_impl(const String& cmd, const String& params, int cmd_id) {
  int csi = taMisc::clusters.FindName(cluster);
  ClusterSpecs& cs = taMisc::clusters[csi];

  int row = jobs_submit.AddBlankRow();
  // submit_svn filled in later -- not avail now
  String filename = GetMyProj()->name;
  jobs_submit.SetVal(filename,    "filename", row);
  jobs_submit.SetVal(last_submit_time, "submit_time",   row);
  jobs_submit.SetVal(String(row), "submit_job", row); // = row!
  // job_no will be filled in on cluster
  // tag will be filled in on cluster
  jobs_submit.SetVal("REQUESTED", "status",     row);
  // job_out, job_out_file, dat_files all generated on cluster
  jobs_submit.SetVal(cmd_id,      "command_id", row);
  jobs_submit.SetVal(cmd,         "command",    row);
  jobs_submit.SetVal(params,      "params",     row);
  jobs_submit.SetVal(ReplaceVars(notes), "notes",      row);
  jobs_submit.SetVal(ReplaceVars(label), "label",      row);
  
  jobs_submit.SetVal(repo_url,    "repo_url",   row);
  jobs_submit.SetVal(cluster,     "cluster",    row);
  jobs_submit.SetVal(queue,       "queue",      row);
  jobs_submit.SetVal(run_time,    "run_time",   row);
  jobs_submit.SetVal(ram_gb,      "ram_gb",     row);
  jobs_submit.SetVal(n_threads,   "n_threads",  row);
  jobs_submit.SetVal(use_cuda,    "use_cuda",   row);
  if(use_mpi) {
    jobs_submit.SetVal(mpi_nodes,   "mpi_nodes",  row);
    jobs_submit.SetVal(mpi_per_node, "mpi_per_node",  row);
  }
  else {
    jobs_submit.SetVal(0, "mpi_nodes",  row);
    jobs_submit.SetVal(0, "mpi_per_node",  row);
  }

  if(parallel_batch && pb_batches > 0) {
    jobs_submit.SetVal(pb_batches,   "pb_batches",  row);
    jobs_submit.SetVal(pb_n_batches_per,   "pb_nodes",  row); // re-using for now..
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

  if(parallel_batch && pb_batches > 0) {
    // we stream off the jobs ourselves
    for(int i=0; i<pb_batches; i+= pb_n_batches_per) {
      int batch_start = i;
      int batch_end = i + pb_n_batches_per;
      int n_batches = pb_n_batches_per;
      if(batch_end > pb_batches) {
        batch_end = pb_batches;
        n_batches = pb_batches - i;
      }
      String cmd_pb = cmd + " batch_start=" + String(batch_start) + " n_batches=" +
        String(n_batches);
      // note: including these legacy params will cause "arg not used" errors to trigger!!!
      // + " b_start=" + String(batch_start) + " b_end=" + String(batch_end);
      AddJobRow_impl(cmd_pb, params, cmd_id++);
    }
  }
  else {
    AddJobRow_impl(cmd, params, cmd_id++);
  }
}

bool ClusterRun::CheckLocalClustUserRows(const DataTable& table, int start_row, int end_row) {

  bool has_not_owned_jobs = false;
  for (int row = start_row; row <= end_row; row++) {
    String clust = table.GetValAsString("cluster", row);
    String user = table.GetValAsString("user", row);
    String tag = table.GetValAsString("tag", row);
    
    if(clust != cluster) {
      has_not_owned_jobs = true;
    }
    if(user != m_cm->GetUsername()) {
      has_not_owned_jobs = true;
    }
  }

  if(has_not_owned_jobs) {
    String message = "One or more jobs are not on the " + cluster + " cluster and/or are not owned by user " + m_cm->GetUsername() + ". You must select each cluster to operate on, and can only operate on your data. Stop or Continue?";
    int choice = taMisc::Choice(message, "Continue", "Stop");
    if (choice > 0) {
      return false;
    }
  }

  return true;
}

bool ClusterRun::CheckLocalClustUser(const DataTable& table, int tab_row, bool warn) {
  String clust = table.GetValAsString("cluster", tab_row);
  String user = table.GetValAsString("user", tab_row);
  String tag = table.GetValAsString("tag", tab_row);
  
  if(clust != cluster) {
    if(warn) {
      taMisc::Info("not processing job tag:", tag, "on cluster:", clust,
                   "must select that cluster instead of:", cluster);
    }
    return false;
  }
  if(user != m_cm->GetUsername()) {
    if(warn) {
      taMisc::Info("not processing job tag:", tag, "on cluster:", clust,
                   "for user:", user, "cannot modify other user's data!");
    }
    return false;
  }
  return true;
}

void
ClusterRun::SubmitUpdateNote(const DataTable& table, int tab_row)
{
    if(!CheckLocalClustUser(table, tab_row))
      return;
    int dst_row = jobs_submit.AddBlankRow();
    jobs_submit.SetVal("UPDATENOTE", "status", dst_row);
    jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
    jobs_submit.CopyCell("notes", dst_row, table, "notes", tab_row);
    jobs_submit.CopyCell("label", dst_row, table, "label", tab_row);
    jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::CancelJob(int running_row)
{
  if(!CheckLocalClustUser(jobs_running, running_row))
    return;
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("CANCELLED", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, jobs_running, "job_no", running_row);
  jobs_submit.CopyCell("tag", dst_row, jobs_running, "tag", running_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::SubmitDeleteJob(const DataTable& table, int tab_row)
{
  if(!CheckLocalClustUser(table, tab_row))
    return;
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("REMOVEJOB", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, table, "job_no", tab_row);
  jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::SubmitNukeJob(const DataTable& table, int tab_row)
{
  if(!CheckLocalClustUser(table, tab_row))
    return;
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("NUKEJOB", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, table, "job_no", tab_row);
  jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::SubmitDeleteDelJob(const DataTable& table, int tab_row)
{
  if(!CheckLocalClustUser(table, tab_row))
    return;
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("REMOVEDELJOB", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, table, "job_no", tab_row);
  jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::SubmitArchiveJob(const DataTable& table, int tab_row)
{
  if(!CheckLocalClustUser(table, tab_row))
    return;
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("ARCHIVEJOB", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, table, "job_no", tab_row);
  jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::SubmitUnDeleteJob(const DataTable& table, int tab_row)
{
  if(!CheckLocalClustUser(table, tab_row))
    return;
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("UNDELETEJOB", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, table, "job_no", tab_row);
  jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
}

void
ClusterRun::SubmitCleanJobFiles(const DataTable& table, int tab_row)
{
  if(!CheckLocalClustUser(table, tab_row))
    return;
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
ClusterRun::SubmitDeleteFiles(const String& files) {
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("REMOVEFILES", "status", dst_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
  jobs_submit.SetVal(files, "other_files",  dst_row); 
}

void
ClusterRun::SubmitSaveState(const DataTable& table, int tab_row)
{
  if(!CheckLocalClustUser(table, tab_row))
    return;
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("SAVESTATE", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, table, "job_no", tab_row);
  jobs_submit.CopyCell("tag", dst_row, table, "tag", tab_row);
  jobs_submit.SetVal(CurTimeStamp(), "submit_time",  dst_row); // # guarantee submit
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
  cmd = exe_cmd;
  if(nowin_x)
    cmd += "_x";
  if (use_cuda) {
    cmd.cat("_cuda");
  }
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
  cmd.cat(" -ni -p <PROJ_FILENAME> -cluster_run tag=<TAG>");

  // Note: cluster script sets number of mpi nodes

  if (n_threads > 0) {
    cmd.cat(" n_threads=").cat(String(n_threads));
  }

  params=ExploreMembersToString(!use_cur_vals);
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
  if (taMisc::gui_active && !helper_is_connected) {
    iPanelSet* ps = FindMyPanelSet();
    if(ps) {
      QObject::connect(ps, SIGNAL(qt_sig_PanelChanged(int)), qt_object_helper,
                       SLOT(UpdateEnabling(int)));
      helper_is_connected = true;
    }
  }
  
  taSigLink* dl = dt.sig_link();
  if(dl) {
    taSigLinkItr itr;
    iPanelOfDataTable* el;
    FOR_DLC_EL_OF_TYPE(iPanelOfDataTable, el, dl, itr) {
      if (el->dte) {
        QObject::connect(el->dte->tvTable, SIGNAL(sig_currentChanged(const QModelIndex&)), qt_object_helper, SLOT(SelectionChanged(const QModelIndex&)));
      }
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
  ClearSelection(jobs_deleted);
  ClearSelection(file_list);
}

///////////////////////////

String ClusterRun::GetSvnPath() {
  if(!InitClusterManager(false)) // fail silently, don't check prefs..
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
}

bool ClusterRun::WaitProcAutoUpdate() {
  if(!wait_proc_updt) return false;
  taDateTime curtime;
  curtime.currentDateTime();
  int delay = wait_proc_last_updt.secsTo(curtime);
  if(delay < wait_proc_updt->auto_updt_interval) {
    return false;
  }
  if(QApplication::activeModalWidget() != NULL) {
    taMisc::Info("ClusterRun: modal window is now open, cancelling auto-update");
    wait_proc_updt = NULL;
    wait_proc_trg_rev = -1;
    return false;               // don't update while a window is open!
  }
  wait_proc_updt->Update();
  //wait_proc_updt->SigEmitUpdated();
  wait_proc_last_updt.currentDateTime();
  
  return true;
}

DataTable* ClusterRun::GetCurDataTable() {
  iPanelSet* ps = FindMyPanelSet();
  PanelId panel_id = static_cast<PanelId>(ps->cur_panel_id);

  if (panel_id == PANEL_RUNNING) {
    return &jobs_running;
  }
  else if (panel_id == PANEL_DONE) {
    return &jobs_done;
  }
  else if (panel_id == PANEL_DELETED) {
    return &jobs_deleted;
  }
  else if (panel_id == PANEL_ARCHIVE) {
    return &jobs_archive;
  }
  else if (panel_id == PANEL_FILES) {
    return &file_list;
  }
  else if (panel_id == PANEL_INFO) {
    return &cluster_info;
  }
  else {
    return NULL;
  }
}

void ClusterRun::DoClusterOp(String do_this) {
  CallFun(do_this);
}

void ClusterRun::UpdateUI() {
  iPanelSet* ps = FindMyPanelSet();
  DataTable* cur_table = GetCurDataTable();
  
  enable_kill = false;
  enable_load = false;
  enable_notes = false;
  
  if (cur_table != NULL && cur_table != &cluster_info) {
    enable_kill = (cur_table == &jobs_running);
    enable_load = (cur_table == &jobs_running || cur_table == &jobs_done || cur_table == &jobs_archive);
    enable_notes = (cur_table == &jobs_done);
  }
  ps->UpdateMethodsEnabled();
}

int ClusterRun::RunningTimeSecs(DataTable* table, int row) {
  String start_time_str;
  String end_time_str;
  start_time_str = table->GetValAsString("start_time", row);
  end_time_str = table->GetValAsString("end_time", row);
        
  taDateTime start_time;
  taDateTime end_time;
  start_time.fromString(start_time_str, timestamp_fmt);
  end_time.fromString(end_time_str, timestamp_fmt);
  int secs = start_time.secsTo(end_time);
  return secs;
}

void ClusterRun::FillInRunningTime(DataTable* table) {
  DataCol* running_time_col = table->GetColData("running_time", true); // true means quiet
  if (running_time_col) {
    for (int i=0; i<table->rows; i++) {
      int secs = RunningTimeSecs(table, i);
      running_time_col->SetVal(taDateTime::SecondsToDHM(secs), i);
    }
  }
}

void ClusterRun::FillInElapsedTime(DataTable* table) {
  DataCol* running_time_col = table->GetColData("running_time", true); // true means quiet
  if (running_time_col) {
    DataCol* start_col = table->GetColData("start_time", true);
    if (start_col) {
      for (int i=0; i<table->rows; i++) {
        String start_time_str;
        start_time_str = start_col->GetValAsString(i);
        if (start_time_str.empty()) {
          running_time_col->SetVal(taDateTime::SecondsToDHM(0), i);
        }
        else {
          taDateTime start_time;
          start_time.fromString(start_time_str, timestamp_fmt);
          taDateTime cur_time;
          cur_time.currentDateTime();
          uint cur_secs = cur_time.toTime_t();
          int secs = start_time.secsTo(cur_time);
          running_time_col->SetVal(taDateTime::SecondsToDHM(secs), i);
        }
      }
    }
  }
}

bool ClusterRun::ShowCallFunDialog(const String& method_name) {
  taProject* proj = GetMyProj();
  if(!proj) return true;
  
  if (method_name == "LoadData") {
    DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("ClusterRun");
    if (dgp->size == 0) {
      return false;
    }
  }
  return true;
}

