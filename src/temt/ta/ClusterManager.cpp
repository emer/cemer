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
  : std::runtime_error(msg)
{
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

  setPaths();                   // always get paths
  m_valid = true;
}

ClusterManager::~ClusterManager()
{
  delete m_svn_client;
  m_svn_client = 0;
}

// Run the model on a cluster using the parameters of the ClusterRun
// provided in the constructor.
bool
ClusterManager::BeginSearch(bool prompt_user)
{
  if (!m_valid) return false; // Ensure proper construction.

  try {
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

    updateWorkingCopy(); // creates the working copy if needed.
    runSearchAlgo();

    int tot_jobs_req = m_cluster_run.jobs_submit.rows;
    if(!m_cluster_run.ValidateJob(tot_jobs_req))
      return false;

    saveSubmitTable();
    saveCopyOfProject();
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

  try {
    // Not necessary to update the working copy here; if it doesn't exist,
    // the user should do an Update first before trying to Kill jobs.
    //updateWorkingCopy();
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

  try {
    // Get old revisions of the two tables in the working copy.
    setPaths();
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

    // Return true as long as one of the files was updated and loaded --
    // in that case, the search algo will probably want to do something.
    return updated && (ok1 || ok2);
  }
  catch (const ClusterManager::Exception &ex) {
    taMisc::Error("Could not update working copy:", ex.what());
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
    throw Exception("Please save project locally first.");
  }
}

const String &
ClusterManager::getFilename()
{
  if (m_proj->file_name.empty()) {
    saveProject();
  }
  return m_proj->file_name;
}

const String &
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
      throw Exception("A Subversion username is required.");
    }
  }
  return m_username;
}

const String &
ClusterManager::getClusterName()
{
  return promptForString(
    m_cluster_run.cluster,
    "A cluster name is required.");
}

const String &
ClusterManager::getRepoUrl()
{
  return promptForString(
    m_cluster_run.repo_url,
    "A repository URL is required.");
}

const String &
ClusterManager::promptForString(const String &str, const char *msg)
{
  if (str.empty()) {
    showRepoDialog();
    if (str.empty()) {
      throw Exception(msg);
    }
  }
  return str;
}

void
ClusterManager::setPaths()
{
  // If already set, just return.
  if (!m_wc_path.empty()) return;

  const String &username = getUsername();
  const String &filename = getFilename();
  const String &cluster = getClusterName();
  const String &repo_url = getRepoUrl();

  // Create a URL to the user's directory in the repo.
  const char *opt_slash = repo_url.endsWith('/') ? "" : "/";
  m_repo_user_url = repo_url + opt_slash + cluster + '/' + username;

  // Create paths for files/directories in the working copy:
  //   user_app_dir/
  //     clustername/
  //       username/                    # m_wc_path
  //         projname/                  # m_wc_proj_path
  //           submit/                  # m_wc_submit_path
  //             jobs_submit.dat        # m_submit_dat_filename
  //             jobs_running.dat       # m_running_dat_filename
  //             jobs_done.dat          # m_done_dat_filename
  //           models/                  # m_wc_models_path
  //             projname.proj          # m_proj_copy_filename
  //           results/                 # m_wc_results_path

  // Set the working copy path and get a canonicalized version back.
  m_wc_path = taMisc::user_app_dir + '/' + cluster + '/' + username;
  m_svn_client->SetWorkingCopyPath(m_wc_path.chars());
  m_wc_path = m_svn_client->GetWorkingCopyPath().c_str();

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
  m_proj_copy_filename = m_wc_models_path + '/' + fi.fileName();

  taMisc::Info("Repository is at", m_repo_user_url);
}

void
ClusterManager::updateWorkingCopy()
{
  // If the user already has a working copy, update it.  Otherwise, create
  // it on the server and check it out.
  setPaths();
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

  // Clear the table and reload data from the file.
  table.ResetData();
  table.LoadAnyData(filename);
  return true;
}

void
ClusterManager::saveSubmitTable()
{
  // Save the datatable and add it to source control (if not already).
  setPaths();
  deleteFile(m_submit_dat_filename);
  m_cluster_run.jobs_submit.SaveData(m_submit_dat_filename);
  m_svn_client->Add(m_submit_dat_filename.chars());
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
ClusterManager::commitFiles(const String &commit_msg)
{
  // Ensure the working copy has been set.
  setPaths();

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
  // Make sure there's at least one repository defined.
  if (taMisc::svn_repos.size == 0 || taMisc::cluster_names.size == 0) {
    taMisc::Error(
      "Please define at least one cluster and repository in "
      "preferences/options");
    return false;
  }

  taGuiDialog dlg;
  dlg.win_title = "Run on cluster";
  dlg.prompt = "Enter parameters";

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
  int idx2 = combo2->findData(m_cluster_run.repo_url.toQString());
  if (idx2 >= 0) combo2->setCurrentIndex(idx2);
  dlg.AddStretch(row);

  row = "notesRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("notesLbl", widget, row, "label=* Notes: ;");
  dlg.AddStringField(&m_cluster_run.notes, "notes", widget, row,
    "tooltip=Notes about this run, used as a checkin comment.;");

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
    "tooltip=How long each job will take to run, e.g., 30m, 12h, or 2d. "
    "Do not underestimate!;");

  row = "ramRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("ramLbl", widget, row, "label=RAM (in GB): ;");
  dlg.AddIntField(&m_cluster_run.ram_gb, "ram", widget, row,
    "tooltip=Required RAM, in gigabytes, or -1 for unspecified.;");
  dlg.AddStretch(row);

  row = "threadsRow";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("threadsLbl", widget, row, "label=Number of hreads: ;");
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
    "tooltip=if the cluster uses alloc_by_node job allocation strategy, then this is the number of nodes to request for this job -- if you want all of your jobs to run in parallel at the same time, then this should be equal to (pb_batches * n_threads * mpi_nodes) / cpus_per_node -- setting this value to 0 will default to this allocation number.;");

  bool modal = true;
  int drval = dlg.PostDialog(modal);
  if (drval == 0) {
    taMisc::Info("Running on cluster cancelled by user.");
    return false;
  }

  m_cluster_run.cluster = combo1->itemText(combo1->currentIndex());
  m_cluster_run.repo_url =
    combo2->itemData(combo2->currentIndex()).toString();
  return true;
}
