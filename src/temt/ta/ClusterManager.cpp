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

#include <SubversionClient>

#include <taMisc>

#include <QBoxLayout>
#include <QComboBox>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>


ClusterManager::Exception::Exception(const char *msg)
  : std::runtime_error(msg) {
  taMisc::Error(msg);
}

ClusterManager::ClusterManager(ClusterRun &cluster_run)
  : m_cluster_run(cluster_run)
  , m_valid(false)      // set true if all goes well
  , m_svn_client(0)     // initialized in ctor body
  , m_proj(0)           // initialized in ctor body
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
  m_proj = GET_OWNER(&m_cluster_run, taProject);
  if (!m_proj) {
    // Should never happen.
    taMisc::Error("Could not get project object to run on cluster.");
    return;
  }

  // Create Subversion client.
  try {
    m_svn_client = new SubversionClient;
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Error creating SubversionClient.\n", ex.what());
    return;
  }

  if(HasBasicData(false))
    setPaths(false);                 // always get paths if possible, but not if not..
  m_valid = true;
}

ClusterManager::~ClusterManager()
{
  delete m_svn_client;
  m_svn_client = 0;
}


void ClusterManager::Init() {
  if(HasBasicData(false))       // no err if not
    setPaths(false);
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

  saveProject();

  // Prompt the user if the flag is set; otherwise the user is
  // only prompted if required fields are blank.
  if (prompt_user) {
    // Prompt user; quit early if they cancel the dialog.
    if (!showRepoDialog()) return false;
  }

  taMisc::Info("Running project", getFilename(),
               "\n  on cluster", getClusterName(),
               "\n  using repository", getRepoUrl(),
               "\n  Notes:", m_cluster_run.notes);

  try {
    updateWorkingCopy(); // creates the working copy if needed.
    runSearchAlgo();

    int tot_jobs_req = m_cluster_run.jobs_submit.rows;
    if(!m_cluster_run.ValidateJob(tot_jobs_req))
      return false;

    saveSubmitTable();
    saveCopyOfProject();
    saveExtraFiles();
    commitFiles("Ready to run on cluster: " + m_cluster_run.notes);
    return true;
  }
  catch (const ClusterManager::Exception &ex) {
    taMisc::Error("Could not run on cluster:", ex.what());
  }
  catch (const SubversionClient::Exception &ex) {
    handleException(ex);
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
    initClusterInfoTable();
    saveSubmitTable();
    commitFiles("Submitting new jobs_submit.dat: " + m_cluster_run.notes);
    return true;
  }
  catch (const ClusterManager::Exception &ex) {
    taMisc::Error("Could not submit jobs table:", ex.what());
  }
  catch (const SubversionClient::Exception &ex) {
    handleException(ex);
  }
  return false;
}

bool
ClusterManager::UpdateTables()
{
  if (!m_valid) return false; // Ensure proper construction.
  if (!CheckPrefs()) return false;
  if(!setPaths()) return false;

  try {
    // Get old revisions of the two tables in the working copy.
    bool quiet = true;
    int old_rev_run = GetLastChangedRevision(m_running_dat_filename, quiet);
    int old_rev_done = GetLastChangedRevision(m_done_dat_filename, quiet);

    updateWorkingCopy();

    // Get new revisions.
    int new_rev_run = GetLastChangedRevision(m_running_dat_filename, quiet);
    int new_rev_done = GetLastChangedRevision(m_done_dat_filename, quiet);

    bool updated = (new_rev_run  > old_rev_run) ||
                   (new_rev_done > old_rev_done);

    bool ok1 = loadTable(m_running_dat_filename, m_cluster_run.jobs_running);
    bool ok2 = loadTable(m_done_dat_filename, m_cluster_run.jobs_done);
    bool ok3 = loadTable(m_archive_dat_filename, m_cluster_run.jobs_archive);
    bool ok4 = loadTable(m_cluster_info_filename, m_cluster_run.cluster_info);

    // Return true as long as one of the files was updated and loaded --
    // in that case, the search algo will probably want to do something.
    return updated && (ok1 || ok2 || ok3 || ok4);
  }
  catch (const ClusterManager::Exception &ex) {
    taMisc::Error("Could not update working copy:", ex.what());
  }
  catch (const SubversionClient::Exception &ex) {
    handleException(ex);
  }
  return false;
}

bool
ClusterManager::RemoveFiles(String_PArray& files, bool force, bool keep_local)
{
  if (!m_valid) return false; // Ensure proper construction.
  if(!CheckPrefs()) return false;

  try {
    updateWorkingCopy();
    m_svn_client->Delete(files, force, keep_local);
    commitFiles("removing files");
    return true;
  }
  catch (const ClusterManager::Exception &ex) {
    taMisc::Error("Could not remove files:", ex.what());
  }
  catch (const SubversionClient::Exception &ex) {
    handleException(ex);
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
    handleException(ex);
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
ClusterManager::handleException(const SubversionClient::Exception &ex)
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

void
ClusterManager::saveProject()
{
  // Save the model locally.
  m_proj->Save();

  // If filename is still empty, save failed somehow.
  if (m_proj->file_name.empty()) {
    taMisc::Error("The project does not have a file name -- please save project locally first.");
  }
}

bool ClusterManager::HasBasicData(bool err) {
  if(m_proj->file_name.empty()) {
    if(err) {
      taMisc::Error("Cluster Manager for:",m_cluster_run.name,
                    "project file name is empty -- must be specified to setup files -- try saving the project");
    }
    return false;
  }
  if(m_cluster_run.cluster.empty()) {
    if(err) {
      taMisc::Error("Cluster Manager for:",m_cluster_run.name,
                    "cluster name is empty -- must be specified to setup files");
    }
    return false;
  }
  if(m_cluster_run.svn_repo.empty()) {
    if(err) {
      taMisc::Error("Cluster Manager for:",m_cluster_run.name,
                    "svn repository name is empty -- must be specified to setup files");
    }
    return false;
  }
  if(m_cluster_run.svn_repo.empty()) {
    if(err) {
      taMisc::Error("Cluster Manager for:",m_cluster_run.name,
                    "svn repository name is empty -- must be specified to setup files");
    }
    return false;
  }
  if(m_cluster_run.repo_url.empty()) {
    if(err) {
      taMisc::Error("Cluster Manager for:",m_cluster_run.name,
                    "repository url is empty -- must be specified to setup files");
    }
    return false;
  }
  if(m_username.empty() && getUsername().empty()) {
    if(err) {
      taMisc::Error("Cluster Manager for:",m_cluster_run.name,
                    "username is empty -- this is derived from svn repository data");
    }
    return false;
  }
  return true;
}


const String
ClusterManager::getFilename()
{
  return m_proj->file_name;
}

const String
ClusterManager::getUsername()
{
  // Get username if we haven't already.
  if (m_username.empty()) {
    // Get username from cache, or prompt user.
    std::string user = m_svn_client->GetUsername(
      getRepoUrl().chars(),
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
ClusterManager::getClusterName()
{
  return m_cluster_run.cluster;
}

const String
ClusterManager::getSvnRepo()
{
  return m_cluster_run.svn_repo;
}

const String
ClusterManager::getRepoUrl()
{
  return m_cluster_run.repo_url;
}

bool
ClusterManager::setPaths(bool updt_wc) {
  String prv_path = m_wc_path;

  if(!CheckPrefs()) return false;
  if(!HasBasicData(true)) return false; // this triggers err output -- if you don't want it, then test HasBasicData(false) in advance of calling setPaths

  String username = getUsername();
  String filename = getFilename();
  String cluster = getClusterName();
  String svn_repo = getSvnRepo();
  String repo_url = getRepoUrl();

  // Create a URL to the user's directory in the repo.
  const char *opt_slash = repo_url.endsWith('/') ? "" : "/";
  m_repo_user_url = repo_url + opt_slash + cluster + '/' + username;

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
      updateWorkingCopy(); // creates the working copy if needed.
    }
    catch (const SubversionClient::Exception &ex) {
      handleException(ex);
    }
  }
  return true;
}

void
ClusterManager::updateWorkingCopy()
{
  // If the user already has a working copy, update it.  Otherwise, create
  // it on the server and check it out.
  if(!setPaths()) return;
  QFileInfo fi_wc(m_wc_path.chars());
  if (!fi_wc.exists()) {
    // This could be the first time the user has used Click-to-cluster.
    taMisc::Info("Working copy not found; will try to create at", m_wc_path);

    // Create the directory on the repository, if not already present.
    String comment = "Creating cluster directory for user ";
    comment += getUsername();
    m_svn_client->TryMakeUrlDir(m_repo_user_url.chars(), comment.chars());

    // Check out a working copy (possibly just an empty directory if we
    // just created it for the first time).
    int rev = m_svn_client->Checkout(m_repo_user_url);
    taMisc::Info("Working copy checked out for revision", String(rev));
  }
  else {
    // Update the existing wc.
    int rev = m_svn_client->Update();
    taMisc::Info("Working copy was updated to revision", String(rev));
  }

  initClusterInfoTable();
  
  // We could check if these directories already exist, but it's easier
  // to just try to create them all and ignore any creation errors.
  m_svn_client->TryMakeDir(m_wc_proj_path);
  m_svn_client->TryMakeDir(m_wc_submit_path);
  m_svn_client->TryMakeDir(m_wc_models_path);
  m_svn_client->TryMakeDir(m_wc_results_path);
}

void
ClusterManager::runSearchAlgo()
{
  if (!m_cluster_run.cur_search_algo) {
    taMisc::Info(m_cluster_run.name, "no search algorithm set -- running on current values");
    m_cluster_run.CreateCurJob(); // just run on current values
    return;
  }

  // Tell the chosen search algorithm to populate the jobs_submit table
  // for the first batch of jobs.
  m_cluster_run.cur_search_algo->Reset();
  bool created = m_cluster_run.cur_search_algo->CreateJobs();

  if (!created || m_cluster_run.jobs_submit.rows == 0) {
    throw Exception("Search algorithm did not produce any jobs.");
  }
}

bool
ClusterManager::loadTable(const String &filename, DataTable &table)
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
  table.LoadAnyData(filename);
  table.StructUpdate(false);
  if(has_sel && st_row >= 0 && end_row >= st_row) {
    m_cluster_run.SelectRows(table, st_row, end_row);
  }
  return true;
}

void
ClusterManager::saveSubmitTable()
{
  // Save the datatable and add it to source control (if not already).
  if(!setPaths()) return;
  deleteFile(m_submit_dat_filename);
  m_cluster_run.jobs_submit.SaveData(m_submit_dat_filename);
  m_svn_client->Add(m_submit_dat_filename.chars());
}

void
ClusterManager::saveDoneTable()
{
  // Save the datatable and add it to source control (if not already).
  if(!setPaths()) return;
  deleteFile(m_done_dat_filename);
  m_cluster_run.jobs_done.SaveData(m_done_dat_filename);
  m_svn_client->Add(m_done_dat_filename.chars());
}

void
ClusterManager::initClusterInfoTable()
{
  // Save a cluster info table to get format into server
  if(!setPaths()) return;
  QFileInfo fi_wc(m_cluster_info_filename.chars());
  if(!fi_wc.exists()) {
    m_cluster_run.cluster_info.SaveData(m_cluster_info_filename);
    m_svn_client->Add(m_cluster_info_filename.chars());
  }
}

void
ClusterManager::saveCopyOfProject()
{
  // Save any changes to the project (from dialog or search algo).
  m_proj->Save();

  // Copy the project from its local path to our cluster working copy.
  // Delete first, since QFile::copy() won't overwrite.
  deleteFile(m_proj_copy_filename);
  QFile::copy(m_proj->file_name, m_proj_copy_filename);
  m_svn_client->Add(m_proj_copy_filename.chars());
}

void
ClusterManager::saveExtraFiles()
{
  String_Array files;
  files.FmDelimString(m_cluster_run.extra_files);
  for(int i=0; i < files.size; i++) {
    // Copy the extra files from local path to our cluster working copy.
    // Delete first, since QFile::copy() won't overwrite.
    String srcfn = files[i];
    String fnm = taMisc::GetFileFmPath(srcfn); // just get the file name
    String wc_fnm = m_wc_models_path + "/" + fnm;
    deleteFile(wc_fnm);
    QFile::copy(srcfn, wc_fnm);
    m_svn_client->Add(wc_fnm.chars());
  }
}

void
ClusterManager::commitFiles(const String &commit_msg)
{
  // Ensure the working copy has been set.
  if(!setPaths()) return;

  // Check in all files and directories that were created or updated.
  int rev = m_svn_client->Checkin(commit_msg);
  taMisc::Info("Committed files in revision:", String(rev));
}

void
ClusterManager::deleteFile(const String &filename)
{
  QString q_filename = filename.toQString();
  if (QFile::exists(q_filename)) {
    QFile::remove(q_filename);
  }
}

bool
ClusterManager::showRepoDialog()
{
  taGuiDialog dlg;
  dlg.win_title = "Run on cluster";
  dlg.prompt = "Enter parameters";
  dlg.width = 300;
  dlg.height = 500;

  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);

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
                      static_cast<QString>(taMisc::svn_repos[idx].value));
    }
    hbox->addWidget(combo2);
  }
  int idx2 = combo2->findText(m_cluster_run.svn_repo.toQString());
  if (idx2 >= 0) combo2->setCurrentIndex(idx2);
  dlg.AddStretch(row);

  row = "notesRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("notesLbl", widget, row, "label=* Notes: ;");
  dlg.AddStringField(&m_cluster_run.notes, "notes", widget, row,
    "tooltip=Notes about this run, used as a checkin comment and visible in job lists -- very good idea to be specific here.;");

  row = "queueRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("queueLbl", widget, row, "label=Queue: ;");
  dlg.AddStringField(&m_cluster_run.queue, "queue", widget, row,
    "tooltip=Choose a queue on the computing resource (optional).;");

  row = "runtimeRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("runtimeLbl", widget, row, "label=Run time: ;");
  dlg.AddStringField(&m_cluster_run.run_time, "runtime", widget, row,
    "tooltip=how long will the jobs take to run -- syntax is number followed by unit indicator -- m=minutes, h=hours, d=days -- e.g., 30m, 12h, or 2d -- typically the job will be killed if it exceeds this amount of time, so be sure to not underestimate!;");

  row = "ramRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("ramLbl", widget, row, "label=RAM (in GB): ;");
  dlg.AddIntField(&m_cluster_run.ram_gb, "ram", widget, row,
    "tooltip=how many gigabytes of ram is required?  0 means do not specify this parameter for the job submission -- for large memory jobs, it can be important to specify this to ensure proper allocation of resources -- the status_info field can often show you how much a job has used in the past.;");
  dlg.AddStretch(row);

  row = "threadsRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("threadsLbl", widget, row, "label=Number of threads: ;");
  dlg.AddIntField(&m_cluster_run.n_threads, "threads", widget, row,
    "tooltip=Number of parallel threads to use for running.;");
  dlg.AddStretch(row);

  row = "mpi";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("mpiLbl", widget, row, "label=Use MPI: ;");
  dlg.AddBoolCheckbox(&m_cluster_run.use_mpi, "usempi", widget, row,
    "tooltip=Use MPI (message-passing-inteface distributed memory executable to run across multiple nodes) on the cluster?;");
  dlg.AddStretch(row);
  dlg.AddLabel("nodesLbl", widget, row, "label=MPI nodes: ;");
  dlg.AddIntField(&m_cluster_run.mpi_nodes, "numnodes", widget, row,
    "tooltip=The number of MPI nodes to use per each model.;");

  row = "pb";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("pbLbl", widget, row, "label=Use parallel_batch: ;");
  dlg.AddBoolCheckbox(&m_cluster_run.parallel_batch, "usepb", widget, row,
                      "tooltip=use parallel batch processing -- run multiple runs of the same model in parallel across nodes or cpus (not using mpi -- just embarassingly parallel separate runs), each on a different batch iteration (e.g., different initial random weights).;");
  dlg.AddStretch(row);
  dlg.AddLabel("batchesLbl", widget, row, "label=pb_batches: ;");
  dlg.AddIntField(&m_cluster_run.pb_batches, "numbatches", widget, row,
    "tooltip=The number of parallel batches to run.;");
  dlg.AddLabel("pbnodesLbl", widget, row, "label=pb_nodes: ;");
  dlg.AddIntField(&m_cluster_run.pb_nodes, "numpbnodes", widget, row,
    "tooltip=if the cluster uses by_node job allocation strategy, then this is the number of nodes to request for this job -- if you want all of your jobs to run in parallel at the same time, then this should be equal to (pb_batches * n_threads * mpi_nodes) / cpus_per_node -- setting this value to 0 will default to this allocation number.;");

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
