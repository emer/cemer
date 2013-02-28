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

#include <taMisc>

#include <QRegExp>

void ClusterRun::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ClusterRun);
  FormatTables();
}

void ClusterRun::Initialize() {
  ram_gb = -1;
  n_threads = taMisc::thread_defaults.n_threads;
  use_mpi = false;
  mpi_nodes = 10;
  m_cm = 0;
}

void ClusterRun::Destroy() {
  delete m_cm; m_cm = 0;
}

void ClusterRun::initClusterManager() {
  m_cm = new ClusterManager(*this);
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
        jobs_submit.SetValColName(model_rev, "model_svn", row);
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
  if (has_updates && cur_search_algo) {
    cur_search_algo->ProcessResults();
  }
  return has_updates;
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
}

void ClusterRun::ImportData() {
}

void ClusterRun::FormatTables() {
  jobs_submit.name = "jobs_submit";
  jobs_submitted.name = "jobs_submitted";
  jobs_running.name = "jobs_running";
  jobs_done.name = "jobs_done";

  FormatTables_impl(jobs_submit);
  FormatTables_impl(jobs_submitted);
  FormatTables_impl(jobs_running);
  FormatTables_impl(jobs_done);
}

void ClusterRun::FormatTables_impl(DataTable& dt) {
  DataCol* dc;

  // The cluster script populates these fields in the running/done tables.
  // The client (this code) can set them in the submit table, but there's not
  // much point since they can't be set until *after* the table is committed.
  dc = dt.FindMakeCol("model_svn", VT_STRING);
  dc->desc = "svn revision for the model";
  dc = dt.FindMakeCol("submit_svn", VT_STRING);
  dc->desc = "svn revision for the job submission commands";
  dc = dt.FindMakeCol("submit_time", VT_STRING);
  dc->desc = "svn revision for the model";

  // Cluster script populates.
  dc = dt.FindMakeCol("submit_job", VT_STRING);
  dc->desc = "index of job number within a given submission -- equal to the row number of the original set of jobs submitted in submit_svn jobs";
  dc = dt.FindMakeCol("job_no", VT_STRING);
  dc->desc = "job number on cluster -- assigned once the job is submitted to the cluster";
  dc = dt.FindMakeCol("tag", VT_STRING);
  dc->desc = "unique tag id for this job -- all files etc are named according to this tag";

  // The client sets this field in the jobs_submit table to:
  //   REQUESTED to request the job be submitted.
  //   CANCELLED to request the job indicated by job_no or tag be cancelled.
  // The cluster script sets this field in the running/done tables to:
  //   SUBMITTED after job successfully submitted to a queue.
  //   QUEUED    when the job is known to be in the cluster queue.
  //             At this point, we have a job number (job_no).
  //   RUNNING   when the job has begun.
  //   DONE      if the job completed successfully.
  //   KILLED    if the job was cancelled.
  dc = dt.FindMakeCol("status", VT_STRING);
  dc->desc = "status of job: REQUESTED, CANCELLED, SUBMITTED, QUEUED, RUNNING, DONE, KILLED";

  // Cluster script populates.
  dc = dt.FindMakeCol("job_out", VT_STRING);
  dc->desc = "job output information -- contains (top of) the job standard output and standard error output as the job is running (truncated to top 2048 characters if longer than that) -- full information available in job_out_file";
  dc = dt.FindMakeCol("job_out_file", VT_STRING);
  dc->desc = "job output file -- file name containing full job output information -- file name should be tag.out";
  dc = dt.FindMakeCol("dat_files", VT_STRING);
  dc->desc = "list of data table output files generated by model -- files are named as  tag_<extra>.out -- this list automatically generated by parsing the result files checked into svn";

  // Search algo populates these fields.
  dc = dt.FindMakeCol("command_id", VT_INT);
  dc->desc = "id for this command, assigned by the search algorithm in an algorithm-specific manner (optional)";
  dc = dt.FindMakeCol("command", VT_STRING);
  dc->desc = "emergent command line";

  // Populated from values the user enters/chooses.
  dc = dt.FindMakeCol("notes", VT_STRING);
  dc->desc = "notes for the job -- describe any specific information about the model configuration etc -- can use this for searching and sorting results";
  dc = dt.FindMakeCol("repo_url", VT_STRING);
  dc->desc = "name of repository to run job on";
  dc = dt.FindMakeCol("cluster", VT_STRING);
  dc->desc = "name of cluster to run job on";
  dc = dt.FindMakeCol("queue", VT_STRING);
  dc->desc = "if specified, indicate a particular queue on the computing resource";
  dc = dt.FindMakeCol("run_time", VT_STRING);
  dc->desc = "how long will the jobs take to run -- syntax is number followed by unit indicator -- m=minutes, h=hours, d=days -- e.g., 30m, 12h, or 2d -- typically the job will be killed if it exceeds this amount of time, so be sure to not underestimate";
  dc = dt.FindMakeCol("ram_gb", VT_INT);
  dc->desc = "how many gigabytes of ram is required?  -1 means do not specify this parameter for the job submission -- for large memory jobs, it can be important to specify this to ensure proper allocation of resources";
  dc = dt.FindMakeCol("n_threads", VT_INT);
  dc->desc = "number of parallel threads to use for running";
  dc = dt.FindMakeCol("mpi_nodes", VT_INT);
  dc->desc = "number of nodes to use for mpi run -- 0 or -1 means not to use mpi";
}

void
ClusterRun::AddJobRow(const String &cmd, int cmd_id) {
  taDateTime curtime;
  curtime.currentDateTime();
  last_submit_time = curtime.toString("dd_MM_yyyy_hh_mm_ss");

  int row = jobs_submit.AddBlankRow();
  // model_svn and submit_svn both filled in later -- not avail now
  jobs_submit.SetVal(last_submit_time, "submit_time",   row);
  jobs_submit.SetVal(String(row), "submit_job", row); // = row!
  // job_no will be filled in on cluster
  // tag will be filled in on cluster
  jobs_submit.SetVal("REQUESTED", "status",     row);
  // job_out, job_out_file, dat_files all generated on cluster
  jobs_submit.SetVal(cmd_id,      "command_id", row);
  jobs_submit.SetVal(cmd,         "command",    row);
  jobs_submit.SetVal(notes,       "notes",   row);

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
}

void
ClusterRun::CancelJob(int running_row)
{
  int dst_row = jobs_submit.AddBlankRow();
  jobs_submit.SetVal("CANCELLED", "status", dst_row);
  jobs_submit.CopyCell("job_no", dst_row, jobs_running, "job_no", running_row);
  jobs_submit.CopyCell("tag", dst_row, jobs_running, "tag", running_row);
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

String ClusterRun::RunCommand(bool use_cur_vals) {
  // Start command with either "emergent" or "emergent_mpi".
  String cmd(taMisc::app_name);
  if (use_mpi) {
    cmd.cat("_mpi");
  }

  // The cluster script needs to substitute the correct relative
  // filename for the project file in its working copy.  It also
  // needs to substitute the tag, which is based on the revision
  // and row number.
  cmd.cat(" -nogui -ni -p <PROJ_FILENAME> tag=<TAG>");

  // Note: cluster script sets number of mpi nodes

  if (n_threads > 0) {
    cmd.cat(" n_threads=").cat(String(n_threads));
  }

  // Add a name=val term for each parameter in the search.
  FOREACH_ELEM_IN_GROUP(EditMbrItem, mbr, mbrs) {
    const EditParamSearch &ps = mbr->param_search;
    if (ps.search) {
      cmd.cat(" ").cat(mbr->GetName()).cat("=");
      if(use_cur_vals) {
        cmd.cat(mbr->CurValAsString());
      }
      else {
        cmd.cat(String(ps.next_val));
      }
    }
  }

  return cmd;
}

void ClusterRun::CreateCurJob(int cmd_id) {
  String cmd = RunCommand(true); // use cur vals
  AddJobRow(cmd, cmd_id);
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
    taMisc::Info("no items selected");
    return false;
  }
  taMisc::Info("start row:", String(st_row), "end row:", String(end_row));
  return true;
}
