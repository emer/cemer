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

#include <QBoxLayout>
#include <QComboBox>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "ta_gui.h"
#include "ta_platform.h"
#include "ta_project.h"
#include "ta_seledit.h"
#include "ta_string.h"
#include "SubversionClient.h"

ClusterManager::ClusterManager(const SelectEdit *select_edit)
  : m_select_edit(select_edit)
  , m_svn_client(0)
  , m_username()
  , m_filename()
  , m_repo_url()
  , m_description()
  , m_wc_path()
  , m_repo_user_path()
  , m_wc_proj_path()
  , m_wc_submit_path()
  , m_wc_models_path()
  , m_wc_results_path()
  , m_num_mpi_nodes(0)
{
}

ClusterManager::~ClusterManager()
{
  delete m_svn_client;
  m_svn_client = 0;
}

void
ClusterManager::SetRepoUrl(const char *repo_url)
{
  m_repo_url = repo_url;
}

void
ClusterManager::SetDescription(const char *description)
{
  m_description = description;
}

void
ClusterManager::UseMpi(int num_mpi_nodes)
{
  m_num_mpi_nodes = num_mpi_nodes;
}

// Run the model on a cluster using the parameters of the SelectEdit
// provided in the constructor.
bool
ClusterManager::Run()
{
  // Can't do anything without a reference to the SelectEdit.
  if (!m_select_edit) return false;

  // Get the project's filename.
  if (!getFilename()) return false;

  // If a repository URL and description haven't been set, prompt the user
  // for these values.
  if (m_repo_url.empty() || m_description.empty()) {
    if (!showRepoDialog()) return false;
  }

  taMisc::Info("Running project", m_filename, "\n  on cluster", m_repo_url,
    "\n  Description:", m_description);

  try {
    // Create Subversion client and get the user's username.  Need the URL to
    // get the username from the cache so unfortunately we have to do this
    // after presenting the dialog.  On the other hand, the only way this can
    // fail is if the user provides an empty username, so it's hard for them
    // to screw it up.
    m_svn_client = new SubversionClient;
    m_username = m_svn_client->GetUsername(
      m_repo_url, SubversionClient::CHECK_CACHE_THEN_PROMPT_USER
    ).c_str();

    if (m_username.empty()) {
      taMisc::Error("A Subversion username is required to run on a cluster.");
      return false;
    }

    setPaths();
    ensureWorkingCopyExists();
    createSubdirs();
    saveCopyOfProject();
    createParamFile();

    // Check in all files and directories that were created or updated.
    int rev = m_svn_client->Checkin(
      "Ready to run on cluster: " + m_description);
    taMisc::Info("Submitted project to run on cluster in revision:",
      String(rev));

    return true;
  }
  catch (const SubversionClient::Exception &ex) {
    switch (ex.GetErrorCode()) {
    case SubversionClient::EMER_OPERATION_CANCELLED:
      // User probably cancelled, don't error, just inform.
      taMisc::Info("Running on cluster cancelled.", ex.what());
      break;

    case SubversionClient::EMER_FORBIDDEN:
      // Probably a commit failure.
      taMisc::Error("User", m_username, "is not authorized on this server.\n",
        ex.what());
      break;

    default:
      taMisc::Error("Error running on cluster.\n", ex.what());
      break;
    }
  }

  // There's a "return true" at the end of the try block, so the only way to
  // get here is if an exception was thrown.
  return false;
}

bool
ClusterManager::getFilename()
{
  // Get the project object.
  taProject *proj = GET_OWNER(m_select_edit, taProject);
  if (!proj) {
    // Should never happen.
    taMisc::Error("Could not get project object to run on cluster.");
    return false;
  }

  // Make sure the model is saved locally before proceeding.
  if (proj->Save()) {
    // Get the project's filename.  It should be valid since we just saved.
    m_filename = proj->file_name;
    if (!m_filename.empty()) {
      // Non-empty filename is success!
      return true;
    }
  }

  taMisc::Error(
    "Please save project locally before attempting to run on cluster.");
  return false;
}

bool
ClusterManager::showRepoDialog()
{
  // Make sure there's at least one repository defined.
  if (taMisc::svn_repos.size == 0) {
    taMisc::Error(
      "Please define at least one repository in preferences/options first");
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
    "label=Please choose a cluster and enter a description for this run.\n"
    "The description will be used as a checkin comment.;");

  row = "clustRow";
  dlg.AddSpace(10, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("clustLbl", widget, row, "label=Cluster: ;");

  // Get the hbox for this row so we can add our combobox to it.
  taGuiLayout *hboxEmer = dlg.FindLayout(row);
  if (!hboxEmer) return false;
  QBoxLayout *hbox = hboxEmer->layout;
  if (!hbox) return false;

  QComboBox *combo = new QComboBox;
  for (int idx = 0; idx < taMisc::svn_repos.size; ++idx) {
    combo->addItem(taMisc::svn_repos[idx].name.chars(),
                   static_cast<QString>(taMisc::svn_repos[idx].value));
  }
  hbox->addWidget(combo);
  dlg.AddStretch(row);

  row = "descRow";
  dlg.AddSpace(10, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("descLbl", widget, row, "label=Description: ;");
  dlg.AddStringField(&m_description, "description", widget, row,
    "tooltip=description to be used as a checkin comment;");

  row = "mpi";
  bool use_mpi = false;
  dlg.AddSpace(10, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("mpiLbl", widget, row, "label=Use MPI: ;");
  dlg.AddBoolCheckbox(&use_mpi, "usempi", widget, row,
    "tooltip=use MPI on the cluster;");
  dlg.AddStretch(row);
  dlg.AddLabel("nodesLbl", widget, row, "label=Number of nodes: ;");
  dlg.AddIntField(&m_num_mpi_nodes, "numnodes", widget, row,
    "tooltip=the number of MPI nodes to use;");

  bool modal = true;
  int drval = dlg.PostDialog(modal);
  if (drval == 0) {
    taMisc::Info("Running on cluster cancelled by user.");
    return false;
  }

  m_repo_url = combo->itemData(combo->currentIndex()).toString();
  return true;
}

void
ClusterManager::setPaths()
{
  // Set the working copy path and get a canonicalized version back.
  m_wc_path = taMisc::user_app_dir + '/' + "repos" + '/' + m_username;
  m_svn_client->SetWorkingCopyPath(m_wc_path.chars());
  m_wc_path = m_svn_client->GetWorkingCopyPath().c_str();

  // Don't use PATH_SEP here, since on Windows that's '\\', which is
  // non-canonical for URLs (and svn paths) and causes errors.

  // This is the path to the user's directory in the repo.
  m_repo_user_path = m_repo_url + '/' + "repos" + '/' + m_username;

  // Make a directory named based on the name of the project, without
  // any path, and without the final ".proj" extension.
  QString proj_name = QFileInfo(m_filename).completeBaseName();
  m_wc_proj_path = m_wc_path + '/' + proj_name;

  // Make subdirectories for various files (job params, models, results).
  m_wc_submit_path = m_wc_proj_path + '/' + "submit";
  m_wc_models_path = m_wc_proj_path + '/' + "models";
  m_wc_results_path = m_wc_proj_path + '/' + "results";

  taMisc::Info("Repository is at", m_repo_user_path); // TODO remove this
}

void
ClusterManager::ensureWorkingCopyExists()
{
  // check if the user has a wc. (create and) checkout a wc if needed
  QFileInfo fi_wc(m_wc_path.chars());
  if (!fi_wc.exists()) {
    // This could be the first time the user has used Click-to-cluster.
    taMisc::Info("Working copy wasn't found; will create at", m_wc_path);

    // Create the directory on the repository.
    String comment = "Creating cluster directory for user ";
    comment += m_username;
    m_svn_client->TryMakeUrlDir(m_repo_user_path.chars(), comment.chars());

    // Check out the directory we just created.
    int rev = m_svn_client->Checkout(m_repo_user_path);
    taMisc::Info("Working copy checked out for revision", String(rev));
  }
  else {
    // Update the existing wc.
    int rev = m_svn_client->Update();
    taMisc::Info("Working copy was updated to revision", String(rev));
  }
}

void
ClusterManager::createSubdirs()
{
  // We could check if these directories already exist, but it's easier
  // to just try to create them all and ignore any creation errors.
  m_svn_client->TryMakeDir(m_wc_proj_path);
  m_svn_client->TryMakeDir(m_wc_submit_path);
  m_svn_client->TryMakeDir(m_wc_models_path);
  m_svn_client->TryMakeDir(m_wc_results_path);
}

void
ClusterManager::saveCopyOfProject()
{
  // Copy the project from its local path to our cluster working copy.
  QString new_filename(m_wc_models_path.chars());
  new_filename.append('/').append(QFileInfo(m_filename).fileName());

  // If the project already exists in the wc, delete it first,
  // since QFile::copy() won't overwrite.
  if (QFile::exists(new_filename)) {
    QFile::remove(new_filename);
  }

  // Copy the file and add it to the working copy.
  QFile::copy(m_filename, new_filename);
  m_svn_client->Add(qPrintable(new_filename));
}

void
ClusterManager::createParamFile()
{
  // TODO:
  // There should be no need to associate version numbers with any of these
  // files, since our commit will be atomic, and the cluster-side script's
  // svn update will be atomic.  On the other hand, if the user kicks off
  // another cluster run before the first has finished, the cluster-side
  // script will blow away the currently running model file in the cluster's
  // working copy.

  // Generate a text file containing the model parameters from the SelectEdit.
  QString param_filename(m_wc_submit_path.chars());
  param_filename.append("/submit.txt");

  // If the param file already exists in the wc, delete it first.
  if (QFile::exists(param_filename)) {
    QFile::remove(param_filename);
  }

  // Open a stream to write to the file.
  QFile file(param_filename);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  QTextStream out(&file);

  // For each parameter to be searched, make a section with its range.
  String all_params;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, mbr, m_select_edit->mbrs) {
    const EditParamSearch &ps = mbr->param_search;
    if (ps.search) {
      String name = mbr->GetName();
      all_params.cat(name).cat(',');
      out << "[" << name.chars() << "]";
      out << "\nmin_val = " << ps.min_val;
      out << "\nmax_val = " << ps.max_val;
      out << "\nnext_val = " << ps.next_val;
      out << "\nincr = " << ps.incr << "\n\n";
    }
  }

  // Strip trailing comma.
  if (!all_params.empty()) {
    all_params.truncate(all_params.length() - 1);
  }

  // Write a timestamp to ensure the file is modified and will get
  // checked in.  Also write the list of parameter names.
  out << "[GENERAL_PARAMS]";
  out << "\norig_filename = " << m_filename.chars();
  out << "\nselect_edit_name = " << m_select_edit->name.chars();
  out << "\nrelative_filename = ../models/"
      << qPrintable(QFileInfo(m_filename).fileName());
  out << "\ntimestamp = "
      << qPrintable(QDateTime::currentDateTime().toString());
  out << "\ndescription = " << m_description.chars();
  out << "\nnum_mpi_nodes = " << m_num_mpi_nodes;
  out << "\nparameters = " << all_params.chars();
  out << "\n\n";

  // Add the parameters file to the wc.
  file.close(); // close manually before adding
  m_svn_client->Add(qPrintable(param_filename));
}
