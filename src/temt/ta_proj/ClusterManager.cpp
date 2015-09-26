// Copyright, 2012, Regents of the University of Colorado,
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

#include "ClusterManager.h"
#include <taDataView>
#include <ClusterRun>
#include <taProject>
#include <DataTable>
#include <taGuiDialog>
#include <taGuiLayout>
#include <taDataProc>

#include <SubversionClient>

#include <taMisc>
#include <taiMisc>

#include <QBoxLayout>
#include <QComboBox>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>


ClusterManager::Exception::Exception(const char *msg)
  : std::runtime_error(msg) {
  if(taiMisc::busy_count > 0)   // if we excepted
    taMisc::DoneBusy();
  taMisc::Error(msg);
}

ClusterManager::ClusterManager(ClusterRun &cluster_run)
  : m_cluster_run(cluster_run)
  , m_valid(false)      // set true if all goes well
  , m_svn_client(0)     // initialized in ctor body
  , m_svn_other(0)
  , m_proj(0)           // initialized in ctor body
  , m_cur_svn_rev(-1)
  , m_username()
  , m_wc_path()
  , m_repo_user_url()
  , m_wc_proj_path()
  , m_wc_submit_path()
  , m_wc_models_path()
  , m_wc_results_path()
  , m_proj_copy_filename()
  , m_submit_dat_filename()
{
  m_proj = m_cluster_run.GetMyProj();
  if (!m_proj) {
    // Should never happen.
    taMisc::Error("Could not get project object to run on cluster.");
    return;
  }

  // Create Subversion client.
  try {
    m_svn_client = new SubversionClient;
    m_svn_other = new SubversionClient;
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Error creating SubversionClient.\n", ex.what());
    return;
  }

  if(HasBasicData(false))
    SetPaths(false);                 // always get paths if possible, but not if not..
  m_valid = true;
}

ClusterManager::~ClusterManager()
{
  delete m_svn_client;
  m_svn_client = 0;
  delete m_svn_other;
  m_svn_other = 0;
}


void ClusterManager::Init() {
  if(HasBasicData(false))       // no err if not
    SetPaths(false);
}

bool ClusterManager::CheckPrefs() {
  if(taMisc::cluster_svn_path.empty()) {
    taMisc::Error("preferences/options setting of cluster_svn_path is empty -- must contain a valid path to local svn checkout directory");
    return false;
  }
  if (taMisc::svn_repos.size == 0 || taMisc::cluster_names.size == 0) {
    taMisc::Error(
      "Please define at least one cluster and repository in "
      "preferences/options");
    return false;
  }
  return true;
}

// Run the model on a cluster using the parameters of the ClusterRun
// provided in the constructor.
bool
ClusterManager::BeginSearch(bool prompt_user)
{
  if (!m_valid) return false; // Ensure proper construction.
  if(!CheckPrefs()) return false;

  if (!SaveProject()) {
    return false;
  }
  // Prompt the user if the flag is set; otherwise the user is
  // only prompted if required fields are blank.
  if (prompt_user) {
    // Prompt user; quit early if they cancel the dialog.
    if (!ShowRepoDialog()) return false;
  }

  taMisc::Info("Running project", GetFilename(),
               "\n  on cluster", GetClusterName(),
               "\n  using repository", GetRepoUrl(),
               "\n  Notes:", m_cluster_run.notes);

  try {
    UpdateWorkingCopy(); // creates the working copy if needed.
    RunSearchAlgo();

    int tot_jobs_req = m_cluster_run.jobs_submit.rows;
    if(!m_cluster_run.ValidateJob(tot_jobs_req))
      return false;

    SaveSubmitTable();
    SaveCopyOfProject();
    SaveExtraFiles();
    CommitFiles("Ready to run on cluster: " + m_cluster_run.notes);
    return true;
  }
  catch (const ClusterManager::Exception &ex) {
    taMisc::Error("Could not run on cluster:", ex.what());
  }
  catch (const SubversionClient::Exception &ex) {
    HandleException(ex);
  }

  // There's a "return true" at the end of the try block, so the only way to
  // get here is if an exception was thrown.
  return false;
}

bool
ClusterManager::CommitJobSubmissionTable()
{
  if (!m_valid) return false; // Ensure proper construction.
  if(!CheckPrefs()) return false;

  try {
    InitClusterInfoTable();
    SaveSubmitTable();
    CommitFiles("Submitting new jobs_submit.dat: " + m_cluster_run.notes);
    return true;
  }
  catch (const ClusterManager::Exception &ex) {
    taMisc::Error("Could not submit jobs table:", ex.what());
  }
  catch (const SubversionClient::Exception &ex) {
    HandleException(ex);
  }
  return false;
}

bool
ClusterManager::UpdateTables()
{
  if (!m_valid) return false; // Ensure proper construction.
  if (!CheckPrefs()) return false;
  if(!SetPaths()) return false;

  try {
    // Get old revisions of the two tables in the working copy.
    bool quiet = true;
    int old_rev_run = GetLastChangedRevision(m_running_dat_filename, quiet);
    int old_rev_done = GetLastChangedRevision(m_done_dat_filename, quiet);

    UpdateWorkingCopy();

    // Get new revisions.
    int new_rev_run = GetLastChangedRevision(m_running_dat_filename, quiet);
    int new_rev_done = GetLastChangedRevision(m_done_dat_filename, quiet);

    bool updated = (new_rev_run  > old_rev_run) ||
                   (new_rev_done > old_rev_done);

    bool ok1 = LoadAllTables(m_running_dat_filename, m_cluster_run.jobs_running,
                             m_cluster_run.jobs_running_tmp);
    bool ok2 = LoadAllTables(m_done_dat_filename, m_cluster_run.jobs_done,
                             m_cluster_run.jobs_done_tmp);
    bool ok3 = LoadAllTables(m_archive_dat_filename, m_cluster_run.jobs_archive,
                             m_cluster_run.jobs_archive_tmp);
    bool ok4 = LoadTable(m_cluster_info_filename, m_cluster_run.cluster_info);

    // Return true as long as one of the files was updated and loaded --
    // in that case, the search algo will probably want to do something.
    return updated && (ok1 || ok2 || ok3 || ok4);
  }
  catch (const ClusterManager::Exception &ex) {
    taMisc::Error("Could not update working copy:", ex.what());
  }
  catch (const SubversionClient::Exception &ex) {
    HandleException(ex);
  }
  return false;
}

bool
ClusterManager::RemoveFiles(String_PArray& files, bool force, bool keep_local)
{
  if (!m_valid) return false; // Ensure proper construction.
  if(!CheckPrefs()) return false;

  try {
    UpdateWorkingCopy();
    m_svn_client->Delete(files, force, keep_local);
    CommitFiles("removing files");
    return true;
  }
  catch (const ClusterManager::Exception &ex) {
    taMisc::Error("Could not remove files:", ex.what());
  }
  catch (const SubversionClient::Exception &ex) {
    HandleException(ex);
  }
  return false;
}

bool
ClusterManager::GetProjectAtRev(int rev) {
  if (!m_valid) return false; // Ensure proper construction.
  if(!CheckPrefs()) return false;

  String_PArray files;
  files.Add(m_proj_copy_filename);
  try {
    m_svn_client->UpdateFiles(files, rev);
    String nwfnm = m_proj_copy_filename.before(".proj");
    nwfnm += "_" + String(rev) + ".proj";
    nwfnm = taMisc::GetFileFmPath(nwfnm);
    nwfnm = taMisc::GetDirFmPath(m_proj->file_name) + "/" + nwfnm; // use orig proj dir
    QFile::copy(m_proj_copy_filename, nwfnm);
    m_svn_client->UpdateFiles(files, -1); // go back to current
  }
  catch (const ClusterManager::Exception &ex) {
    taMisc::Error("Could not get project at revision:", ex.what());
  }
  catch (const SubversionClient::Exception &ex) {
    HandleException(ex);
  }
  return false;
}


String
ClusterManager::GetWcProjPath() const
{
  return m_wc_proj_path;
}

String
ClusterManager::GetWcResultsPath() const
{
  return m_wc_results_path;
}

String
ClusterManager::GetWcSubmitPath() const
{
  return m_wc_submit_path;
}

String
ClusterManager::GetWcModelsPath() const
{
  return m_wc_models_path;
}

String
ClusterManager::GetWcProjFilename() const
{
  return m_proj_copy_filename;
}

String
ClusterManager::GetWcSubmitFilename() const
{
  return m_submit_dat_filename;
}

String
ClusterManager::GetWcClusterInfoFilename() const
{
  return m_cluster_info_filename;
}

int
ClusterManager::GetLastChangedRevision(const String &path, bool quiet)
{
  if (!m_valid) return -1;

  // Get the last revision in which the passed path was changed, according
  // to the working copy.  Returns -1 on error.
  try {
    return m_svn_client->GetLastChangedRevision(path.chars());
  }
  catch (const SubversionClient::Exception &ex) {
    if (!quiet) {
      taMisc::Error("Could not get revision info.\n", ex.what());
    }
    return -1;
  }
}

void
ClusterManager::HandleException(const SubversionClient::Exception &ex)
{
  switch (ex.GetErrorCode()) {
  case SubversionClient::EMER_OPERATION_CANCELLED:
    // User probably cancelled, don't error, just inform.
    taMisc::Info("Cluster operation cancelled.", ex.what());
    break;

  case SubversionClient::EMER_FORBIDDEN:
    // Probably a commit failure.
    taMisc::Error("User", m_username, "is not authorized on this server.\n",
      ex.what());
    break;

  default:
    taMisc::Error("Cluster operation error.\n", ex.what());
    break;
  }
}

bool
ClusterManager::SaveProject()
{
  if (m_proj->GetFileName().empty()) {
    int choice = taMisc::Choice("The project must be saved locally before it can be run on the cluster", "Save", "Cancel");
    if (choice == 0) {
       m_proj->Save();
    }
    else {
      return false;
    }
  }
  
  if (m_proj->GetFileName().empty()) { // was the project really saved?
    return false;
  }
  return true;
}

bool ClusterManager::HasBasicData(bool err) {
  if(m_proj->file_name.empty()) {
    if(err) {
      taMisc::Error("Cluster Manager:",m_cluster_run.name,
                    "project file name is empty -- must be specified to setup files -- try saving the project");
    }
    return false;
  }
  if(m_cluster_run.cluster.empty()) {
    if(err) {
      taMisc::Error("Cluster Manager:",m_cluster_run.name,
                    "cluster name is empty -- must be specified to setup files");
    }
    return false;
  }
  if(m_cluster_run.svn_repo.empty()) {
    if(err) {
      taMisc::Error("Cluster Manager:",m_cluster_run.name,
                    "svn repository name is empty -- must be specified -- setting to the first one listed in Preferences -- go to ClusterRun 'Properties' panel to edit");
      m_cluster_run.svn_repo = taMisc::svn_repo1_url.name;
      m_cluster_run.UpdateAfterEdit();
    }
    return false;
  }
  if(m_cluster_run.repo_url.empty()) {
    if(err) {
      taMisc::Error("Cluster Manager:",m_cluster_run.name,
                    "repository url is empty -- must be specified to setup files");
    }
    return false;
  }
  if(m_username.empty() && GetUsername().empty()) {
    if(err) {
      taMisc::Error("Cluster Manager:",m_cluster_run.name,
                    "username is empty -- this is derived from svn repository data");
    }
    return false;
  }
  return true;
}


const String
ClusterManager::GetFilename()
{
  if(m_cluster_run.set_proj_name) {
    String rval = taMisc::GetDirFmPath(m_proj->file_name) + taMisc::path_sep  +
      m_cluster_run.proj_name + ".proj";
    return rval;
  }
  return m_proj->file_name;
}

const String
ClusterManager::GetUsername()
{
  // Get username if we haven't already.
  if (m_username.empty()) {
    // Get username from cache, or prompt user.
    std::string user = m_svn_client->GetUsername(
      GetRepoUrl().chars(),
      SubversionClient::CHECK_CACHE_THEN_PROMPT_USER);
    m_username = user.c_str();

    // If still empty, can't proceed.
    if (m_username.empty()) {
      taMisc::Error("A Subversion username is required, but could not be found from the svn repository -- something went wrong with the initial repository creation -- try deleting the repository and starting over, and make sure that your user name on this machine is appropriate for using on the cluster, etc.");
    }
  }
  return m_username;
}

const String
ClusterManager::GetClusterName()
{
  return m_cluster_run.cluster;
}

const String
ClusterManager::GetSvnRepo()
{
  return m_cluster_run.svn_repo;
}

const String
ClusterManager::GetRepoUrl()
{
  return m_cluster_run.repo_url;
}

const String
ClusterManager::GetRepoUrl_UserClust(const String& user, const String& clust) {
  String repo_url = GetRepoUrl();
  const char* opt_slash = repo_url.endsWith('/') ? "" : "/";
  String uurl = repo_url + opt_slash + clust + '/' + user;
  return uurl;
}

const String
ClusterManager::GetWcPath_UserClust(const String& wc_path,
                                    const String& user, const String& clust) {
  String us_user = GetUsername();
  String rval = wc_path.before(us_user, -1); // must be from end!!!
  rval += user;
  rval += wc_path.after(us_user,-1);
  String clust_nm = GetClusterName();
  rval.gsub(clust_nm, clust);
  return rval;
}

bool
ClusterManager::SetPaths(bool updt_wc) {
  String prv_path = m_wc_path;

  if(!CheckPrefs()) return false;
  if(!HasBasicData(true)) return false; // this triggers err output -- if you don't want it, then test HasBasicData(false) in advance of calling SetPaths

  String username = GetUsername();
  String filename = GetFilename();
  String cluster = GetClusterName();
  String svn_repo = GetSvnRepo();
  String repo_url = GetRepoUrl();

  // Create a URL to the user's directory in the repo.
  m_repo_user_url = GetRepoUrl_UserClust(username, cluster);

  // Create paths for files/directories in the working copy:
  // taMisc::cluster_svn_path/
  //   repo_name/
  //     clustername/
  //       username/                    # m_wc_path
  //         cluster_info.dat             # m_cluster_info_filename
  //         projname/                  # m_wc_proj_path
  //           submit/                  # m_wc_submit_path
  //             jobs_submit.dat        # m_submit_dat_filename
  //             jobs_running.dat       # m_running_dat_filename
  //             jobs_done.dat          # m_done_dat_filename
  //             jobs_archive.dat       # m_archive_dat_filename
  //           models/                  # m_wc_models_path
  //             projname.proj          # m_proj_copy_filename
  //           results/                 # m_wc_results_path

  // Set the working copy path and get a canonicalized version back.
  String clust_svn = taMisc::cluster_svn_path;
  clust_svn.gsub("~/", taMisc::GetHomePath() + "/");

  if(updt_wc) {
    taMisc::MakePath(clust_svn);  // ensure good..
  }

  m_wc_path = clust_svn + '/' + svn_repo + '/' + cluster + '/' + username;

  m_svn_client->SetWorkingCopyPath(m_wc_path.chars());
  m_wc_path = m_svn_client->GetWorkingCopyPath().c_str();

  m_cluster_info_filename = m_wc_path + "/cluster_info.dat";

  // Make a directory named based on the name of the project, without
  // any path, and without the final ".proj" extension.
  QFileInfo fi(filename);
  m_wc_proj_path = m_wc_path + '/' + fi.completeBaseName();

  // Make subdirectories for various files (job params, models, results).
  m_wc_submit_path = m_wc_proj_path + "/submit";
  m_wc_models_path = m_wc_proj_path + "/models";
  m_wc_results_path = m_wc_proj_path + "/results";

  // Could create these filenames using "m_cluster_run.jobs_submit.name"
  // but seems better to just use fixed names that the cluster script
  // won't have to guess about (in case user renames tables).
  m_submit_dat_filename = m_wc_submit_path + "/jobs_submit.dat";
  m_running_dat_filename = m_wc_submit_path + "/jobs_running.dat";
  m_done_dat_filename = m_wc_submit_path + "/jobs_done.dat";
  m_archive_dat_filename = m_wc_submit_path + "/jobs_archive.dat";
  m_proj_copy_filename = m_wc_models_path + '/' + fi.fileName();

  if(updt_wc && m_wc_path != prv_path) {
    taMisc::Info("Repository is at", m_repo_user_url, "local checkout:", m_wc_proj_path);
    // make sure we've got a complete working copy here..
    try {
      UpdateWorkingCopy(); // creates the working copy if needed.
    }
    catch (const SubversionClient::Exception &ex) {
      HandleException(ex);
    }
  }
  return true;
}

int
ClusterManager::UpdateWorkingCopy() {
  if(!SetPaths()) return -1;

  String clust_nm = GetClusterName();
  String username = GetUsername();

  String_Array clusts;
  clusts.Split(m_cluster_run.clusters, " ");
  String_Array users;
  users.Split(m_cluster_run.users, " ");

  QFileInfo fi(GetFilename());
  String projname = fi.completeBaseName();

  for(int cl = 0; cl < clusts.size; cl++) {
    String clust = clusts[cl];
    for(int us = 0; us < users.size; us++) {
      String user = users[us];
      String wcp = GetWcPath_UserClust(m_wc_path, user, clust);
      bool main_svn = ((clust == clust_nm) && (user == username));
      if(main_svn) {
        m_cur_svn_rev = UpdateWorkingCopy_impl(m_svn_client, m_wc_path, user, clust, projname,
                                                main_svn);
      }
      else {
	try{
	  m_svn_other->SetWorkingCopyPath(wcp);
	  int rev = UpdateWorkingCopy_impl(m_svn_other, wcp, user, clust, projname,
					   main_svn);
	} catch (const SubversionClient::Exception &ex) {
	  //These are additional repositories, so don't worry about them too much
	  taMisc::Info("Could not update SVN working copy ", wcp, ". Ignoring secondary repository\n", ex.what());
	}
      }
    }
  }
  return m_cur_svn_rev;
}

int
ClusterManager::UpdateWorkingCopy_impl(SubversionClient* sc, const String& wc_path,
                                       const String& user, const String& clust, const String& projname,
                                       bool main_svn)
{
  // If the user already has a working copy, update it.  Otherwise, create
  // it on the server and check it out.
  int rev_rval = 0;
  QFileInfo fi_wc(wc_path.chars());
  if (!fi_wc.exists()) {
    String uurl = GetRepoUrl_UserClust(user, clust);
    if(main_svn) {
      // This could be the first time the user has used Click-to-cluster.
      taMisc::Info("Working copy not found; will try to create at:", wc_path);

      // Create the directory on the repository, if not already present.
      String comment = "Creating cluster directory for user: ";
      comment += user;
      sc->TryMakeUrlDir(uurl.chars(), comment.chars());
    }

    // Check out a working copy (possibly just an empty directory if we
    // just created it for the first time).
    taMisc::Info("Checking out repository url:", uurl, "to wc path:", wc_path,
                 "can take a while..");
    taMisc::Busy();
    rev_rval = sc->Checkout(uurl, wc_path);
    taMisc::DoneBusy();
    if(main_svn) {
      taMisc::Info("Working copy checked out for revision", String(rev_rval));
    }
  }
  else {
    // Update the existing wc.
    taMisc::Busy();
    if (projname.length() > 0) {
      //If we have a specific project, only check out a sub section of the repository to save time
      //We also need the cluster_info.dat from the top level directory though.
      String_PArray files;
      files.Add(wc_path + "/cluster_info.dat");
      try {
	sc->UpdateFiles(files, -1);
      } catch (const ClusterManager::Exception &ex) {
	taMisc::Error("Could not get cluster_info", ex.what());
      }
      sc->SetWorkingCopyPath(wc_path + "/" + projname);
    } 
    rev_rval = sc->Update();
    taMisc::DoneBusy();
    if(main_svn) {
      taMisc::Info("Working copy was updated to revision", String(rev_rval));
    }
  }

  if(main_svn) {
    InitClusterInfoTable();
  }
  
  // We could check if these directories already exist, but it's easier
  // to just try to create them all and ignore any creation errors.
  if(main_svn) {
    sc->TryMakeDir(m_wc_proj_path);
    sc->TryMakeDir(m_wc_submit_path);
    sc->TryMakeDir(m_wc_models_path);
    sc->TryMakeDir(m_wc_results_path);
  }
  return rev_rval;
}

void
ClusterManager::RunSearchAlgo()
{
  if (!m_cluster_run.cur_search_algo || !m_cluster_run.use_search_algo) {
    taMisc::Info(m_cluster_run.name, "no search algorithm set or not using it -- running on current values");
    m_cluster_run.CreateCurJob(); // just run on current values
    return;
  }

  // Tell the chosen search algorithm to populate the jobs_submit table
  // for the first batch of jobs.
  bool start_ok = m_cluster_run.cur_search_algo->StartSearch();
  if(!start_ok) {
    throw Exception("Search algorithm could not start any jobs.");
  }
  bool created = m_cluster_run.cur_search_algo->CreateJobs();
  if (!created || m_cluster_run.jobs_submit.rows == 0) {
    throw Exception("Search algorithm did not produce any jobs.");
  }
}

bool ClusterManager::MergeTableToSummary(DataTable& sum_tab, DataTable& src_tab,
                                         const String& clust, const String& user) {
  int cur_row = sum_tab.rows;
  taDataProc::CopyCommonColData(&sum_tab, &src_tab);
  int n_rows = sum_tab.rows;
  for(int i=cur_row; i<n_rows; i++) {
    sum_tab.SetVal(clust, "cluster", i);
    sum_tab.SetVal(user, "user", i);
  }
  return true;
}

bool
ClusterManager::LoadAllTables(const String& filename, DataTable& sum_table,
                              DataTable& tmp_table)
{
  String_Array clusts;
  clusts.Split(m_cluster_run.clusters, " ");
  String_Array users;
  users.Split(m_cluster_run.users, " ");

  sum_table.StructUpdate(true);
  // tmp_table.StructUpdate(true);
  sum_table.ResetData();
  
  for(int cl = 0; cl < clusts.size; cl++) {
    String clust = clusts[cl];
    for(int us = 0; us < users.size; us++) {
      String user = users[us];
      String wcp = GetWcPath_UserClust(filename, user, clust);
      bool ok = LoadTable(wcp, tmp_table);
      if(ok) {
        MergeTableToSummary(sum_table, tmp_table, clust, user);
      }
    }
  }
  sum_table.StructUpdate(false);
  // tmp_table.StructUpdate(false);
  return true;
}

bool
ClusterManager::LoadTable(const String &filename, DataTable &table)
{
  // Ensure the file exists.
  if (!QFileInfo(filename.chars()).exists()) {
    return false;
  }

  // Clear the table and reload data from the file -- always try to preserve selections
  int st_row, end_row;
  bool has_sel = m_cluster_run.SelectedRows(table, st_row, end_row);
  table.StructUpdate(true);
  table.ResetData();
  table.LoadData(filename);
  table.StructUpdate(false);
  if(has_sel && st_row >= 0 && end_row >= st_row) {
    m_cluster_run.SelectRows(table, st_row, end_row);
  }
  return true;
}

void
ClusterManager::SaveSubmitTable()
{
  // Save the datatable and add it to source control (if not already).
  if(!SetPaths()) return;
  DeleteFile(m_submit_dat_filename);
  m_cluster_run.jobs_submit.SaveData(m_submit_dat_filename);
  m_svn_client->Add(m_submit_dat_filename.chars());
}

void
ClusterManager::SaveDoneTable()
{
  // Save the datatable and add it to source control (if not already).
  if(!SetPaths()) return;
  DeleteFile(m_done_dat_filename);
  m_cluster_run.jobs_done.SaveData(m_done_dat_filename);
  m_svn_client->Add(m_done_dat_filename.chars());
}

void
ClusterManager::InitClusterInfoTable()
{
  // Save a cluster info table to get format into server
  if(!SetPaths()) return;
  QFileInfo fi_wc(m_cluster_info_filename.chars());
  if(!fi_wc.exists()) {
    m_cluster_run.cluster_info.SaveData(m_cluster_info_filename);
    m_svn_client->Add(m_cluster_info_filename.chars());
  }
}

void
ClusterManager::SaveCopyOfProject()
{
  // Save any changes to the project (from dialog or search algo).
  m_proj->Save();

  // Copy the project from its local path to our cluster working copy.
  // Delete first, since QFile::copy() won't overwrite.
  DeleteFile(m_proj_copy_filename);
  QFile::copy(m_proj->file_name, m_proj_copy_filename);
  m_svn_client->Add(m_proj_copy_filename.chars());
}

void
ClusterManager::SaveExtraFiles()
{
  String_Array files;
  files.FmDelimString(m_cluster_run.extra_files);
  for(int i=0; i < files.size; i++) {
    // Copy the extra files from local path to our cluster working copy.
    // Delete first, since QFile::copy() won't overwrite.
    String srcfn = files[i];
    String fnm = taMisc::GetFileFmPath(srcfn); // just get the file name
    String wc_fnm = m_wc_models_path + "/" + fnm;
    DeleteFile(wc_fnm);
    QFile::copy(srcfn, wc_fnm);
    m_svn_client->Add(wc_fnm.chars());
  }
}

void
ClusterManager::AddFile(const String& file_path)
{
  m_svn_client->Add(file_path.chars());
}

void
ClusterManager::CommitFiles(const String &commit_msg)
{
  // Ensure the working copy has been set.
  if(!SetPaths()) return;

  // Check in all files and directories that were created or updated.
  m_cur_svn_rev = m_svn_client->Checkin(commit_msg);
  taMisc::Info("Committed files in revision:", String(m_cur_svn_rev));
}

void
ClusterManager::DeleteFile(const String &filename)
{
  QString q_filename = filename.toQString();
  if (QFile::exists(q_filename)) {
    QFile::remove(q_filename);
  }
}

bool
ClusterManager::ShowRepoDialog()
{
  taGuiDialog dlg;
  dlg.win_title = "Run on cluster";
  dlg.prompt = "Enter parameters";
  dlg.width = 300;
  dlg.height = 600;

  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);

  String tt;

  String row = "instrRow";
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("Instructions", widget, row,
    "wrap=on;"
    "label=Please choose a cluster and repository, enter notes for this run, "
    "and set other parameters. The notes will be used as a checkin comment. "
    "Required fields are indicated with an * (asterisk). Additional clusters "
    "and repositories may be defined in emergent's options/preferences.;");

  row = "clustRow";
  int space = 5;
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("clustLbl", widget, row, "label=* Cluster: ;");

  QComboBox *combo1 = new QComboBox;
  {
    // Get the hbox for this row so we can add our combobox to it.
    taGuiLayout *hboxEmer = dlg.FindLayout(row);
    if (!hboxEmer) return false;
    QBoxLayout *hbox = hboxEmer->layout;
    if (!hbox) return false;

    for (int idx = 0; idx < taMisc::cluster_names.size; ++idx) {
      combo1->addItem(taMisc::cluster_names[idx].chars());
    }
    hbox->addWidget(combo1);
  }
  int idx1 = combo1->findText(m_cluster_run.cluster.toQString());
  if (idx1 >= 0) combo1->setCurrentIndex(idx1);

  dlg.AddSpace(space, row);

  String srchalgo = "none";
  if(m_cluster_run.cur_search_algo) {
    srchalgo = m_cluster_run.cur_search_algo->name;
    tt = "tooltip=Select whether to use the given search algorithm -- to select a search algorithm you must first make one of an appropriate type (Jobs/New Search Algo menu), or edit the properties tab of the ControlPanel to select a different one;"; 
    dlg.AddLabel("srchAlgo", widget, row, "label= Search Algo: " + srchalgo + " : use it? ;" + tt);
    dlg.AddSpace(space, row);
    dlg.AddBoolCheckbox(&m_cluster_run.use_search_algo, "usesrch", widget, row, tt);
    dlg.AddStretch(row);
  }
  else {
    tt = "tooltip=No search algorithm has been selected -- to select a search algorithm you must first make one of an appropriate type (Jobs/New Search Algo menu), or edit the properties tab of the ControlPanel to select a different one;"; 
    dlg.AddLabel("srchAlgo", widget, row, "label= Search Algo: " + srchalgo + " ;" + tt);
    dlg.AddStretch(row);
  }

  row = "emer_exe_row";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("exe_cmd")->desc + ";";
  dlg.AddLabel("exe_lbl", widget, row, "label=* Executable Cmd: ;" + tt);
  
  dlg.AddStringField(&m_cluster_run.exe_cmd, "", widget, row, tt);
  dlg.AddStretch(row);
  
  row = "repoRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("repoLbl", widget, row, "label=* Repository: ;");

  QComboBox *combo2 = new QComboBox;
  {
    // Get the hbox for this row so we can add our combobox to it.
    taGuiLayout *hboxEmer = dlg.FindLayout(row);
    if (!hboxEmer) return false;
    QBoxLayout *hbox = hboxEmer->layout;
    if (!hbox) return false;

    for (int idx = 0; idx < taMisc::svn_repos.size; ++idx) {
      combo2->addItem(taMisc::svn_repos[idx].name.chars(),
                      taMisc::svn_repos[idx].value.toQString());
    }
    hbox->addWidget(combo2);
  }
  int idx2 = combo2->findText(m_cluster_run.svn_repo.toQString());
  if (idx2 >= 0) combo2->setCurrentIndex(idx2);
  dlg.AddStretch(row);

  row = "notesRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("notes")->desc + ";";
  dlg.AddLabel("notesLbl", widget, row, "label=* Notes: ;" + tt);
  dlg.AddStringField(&m_cluster_run.notes, "notes", widget, row, tt);
  
  row = "labelRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("label")->desc + ";";
  dlg.AddLabel("labelLbl", widget, row, "label=Label: ;" + tt);
  dlg.AddStringField(&m_cluster_run.label, "label", widget, row, tt);
  
  row = "queueRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("queue")->desc + ";";
  dlg.AddLabel("queueLbl", widget, row, "label=Queue: ;" + tt);
  dlg.AddStringField(&m_cluster_run.queue, "queue", widget, row, tt);

  row = "runtimeRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("run_time")->desc + ";";
  dlg.AddLabel("runtimeLbl", widget, row, "label=Run time: ;" + tt);
  dlg.AddStringField(&m_cluster_run.run_time, "runtime", widget, row, tt);

  row = "ramRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("ram_gb")->desc + ";";
  dlg.AddLabel("ramLbl", widget, row, "label=RAM (in GB): ;" + tt);
  dlg.AddIntField(&m_cluster_run.ram_gb, "ram", widget, row, tt);
  dlg.AddStretch(row);

  row = "threadsRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("n_threads")->desc + ";";
  dlg.AddLabel("threadsLbl", widget, row, "label=Number of threads: ;" + tt);
  dlg.AddIntField(&m_cluster_run.n_threads, "threads", widget, row, tt);
  dlg.AddStretch(row);

  row = "mpi";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("use_mpi")->desc + ";";
  dlg.AddLabel("mpiLbl", widget, row, "label=Use MPI: ;" + tt);
  dlg.AddBoolCheckbox(&m_cluster_run.use_mpi, "usempi", widget, row, tt);

  dlg.AddStretch(row);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("mpi_nodes")->desc + ";";
  dlg.AddLabel("nodesLbl", widget, row, "label=MPI nodes: ;" + tt);
  dlg.AddIntField(&m_cluster_run.mpi_nodes, "numnodes", widget, row, tt);

  dlg.AddStretch(row);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("mpi_per_node")->desc + ";";
  dlg.AddLabel("pernodesLbl", widget, row, "label=MPI per_node: ;" + tt);
  dlg.AddIntField(&m_cluster_run.mpi_per_node, "numpernode", widget, row, tt);
  dlg.AddStretch(row);

  row = "pb";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("parallel_batch")->desc + ";";
  dlg.AddLabel("pbLbl", widget, row, "label=Use parallel_batch: ;" + tt);
  dlg.AddBoolCheckbox(&m_cluster_run.parallel_batch, "usepb", widget, row, tt);

  dlg.AddStretch(row);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("pb_batches")->desc + ";";
  dlg.AddLabel("batchesLbl", widget, row, "label=pb_batches: ;" + tt);
  dlg.AddIntField(&m_cluster_run.pb_batches, "numbatches", widget, row, tt);

  dlg.AddStretch(row);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("pb_n_batches_per")->desc + ";";
  dlg.AddLabel("nbatchesLbl", widget, row, "label=pb_n_batches_per: ;" + tt);
  dlg.AddIntField(&m_cluster_run.pb_n_batches_per, "nbatchesper", widget, row, tt);
  dlg.AddStretch(row);

  row = "misc";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  tt = "tooltip=" + TA_ClusterRun.members.FindName("nowin_x")->desc + ";";
  dlg.AddLabel("nowinxLbl", widget, row, "label=Use -nowin: ;" + tt);
  dlg.AddBoolCheckbox(&m_cluster_run.nowin_x, "nowin", widget, row, tt);
  dlg.AddStretch(row);

  
  bool modal = true;
  int drval = dlg.PostDialog(modal);
  if (drval == 0) {
    taMisc::Info("Running on cluster cancelled by user.");
    return false;
  }

  m_cluster_run.cluster = combo1->itemText(combo1->currentIndex());
  m_cluster_run.svn_repo = combo2->itemText(combo2->currentIndex());
  m_cluster_run.UpdateAfterEdit();
  return true;
}

String
ClusterManager::ChooseCluster(const String& prompt) {
  // Make sure there's at least one repository defined.
  if (taMisc::svn_repos.size == 0 || taMisc::cluster_names.size == 0) {
    taMisc::Error(
      "Please define at least one cluster and repository in "
      "preferences/options");
    return false;
  }

  taGuiDialog dlg;
  dlg.win_title = "Choose a Cluster";
  dlg.prompt = prompt;
  dlg.width = 200;
  dlg.height = 100;

  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);

  String row = "clustRow";
  int space = 5;
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("clustLbl", widget, row, "label=* Cluster: ;");

  QComboBox *combo1 = new QComboBox;
  {
    // Get the hbox for this row so we can add our combobox to it.
    taGuiLayout *hboxEmer = dlg.FindLayout(row);
    if (!hboxEmer) return false;
    QBoxLayout *hbox = hboxEmer->layout;
    if (!hbox) return false;

    for (int idx = 0; idx < taMisc::cluster_names.size; ++idx) {
      combo1->addItem(taMisc::cluster_names[idx].chars());
    }
    hbox->addWidget(combo1);
  }
  int idx1 = combo1->findText(m_cluster_run.cluster.toQString());
  if (idx1 >= 0) combo1->setCurrentIndex(idx1);
  dlg.AddStretch(row);
  dlg.AddSpace(space, vbox);

  bool modal = true;
  int drval = dlg.PostDialog(modal);
  if (drval == 0) {
    return _nilString;
  }
  
  String rval = combo1->itemText(combo1->currentIndex());
  return rval;
}

