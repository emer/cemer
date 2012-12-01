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
#include "Subversion.h"

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
{
}

ClusterManager::~ClusterManager()
{
  delete m_svn_client;
  m_svn_client = 0;
}

// Run the model on a cluster using the parameters of the SelectEdit
// provided in the constructor.
bool
ClusterManager::Run()
{
  // Get the project's filename.
  // Prompt the user for a repository and a description for this cluster run.
  if (!getFilename() || !showRepoDialog()) {
    return false;
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
      taMisc::Error("A Subversion username is needed to run on a cluster.");
      return false;
    }

    setPaths();
    ensureWorkingCopyExists();
    createSubdirs();
    createParamFile();

    // checkin the project's directory, subdirectories and model's file created
    m_svn_client->Checkin("Ready to run on cluster: " + m_description);

    // TODO set mkdir to commit immediately after adding dirs to avoid an explicit commit
    //PrintMkdirMessage(mkdir_success, model_path);

    return true;
  }
  catch (const SubversionClient::Exception &ex) {
    if (ex.GetErrorCode() == SubversionClient::EMER_OPERATION_CANCELLED) {
      taMisc::Info("Running on cluster cancelled.", ex.what());
    }
    else {
      taMisc::Error("Error running on cluster.", ex.what());
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
    taMisc::Error("Could not get project object to run on cluster.");
    return false;
  }

  // Get the project's filename and make sure it has been saved at least once.
  // This intentionally doesn't check to see if the user has unsaved changes!
  m_filename = proj->file_name;
  if (m_filename.empty()) {
    taMisc::Error(
      "Please save project locally before attempting to run on cluster.");
    return false;
  }

  return true;
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

  String row;
  row = "instrRow";
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("Instructions", widget, row,
    "label=Please choose a cluster and enter a description for this run.\n"
    "The description will be used as a checkin comment.;");

  dlg.AddSpace(20, vbox);
  row = "clustRow";
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

  dlg.AddSpace(20, vbox);
  row = "descRow";
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("descLbl", widget, row, "label=Description: ;");

  dlg.AddStringField(&m_description, "description", widget, row,
    "tooltip=enter a description to be used as a checkin comment;");

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
  String m_wc_path = taMisc::user_app_dir + '/' + "repos" + '/' + m_username;
  m_svn_client->SetWorkingCopyPath(m_wc_path.chars());
  m_wc_path = m_svn_client->GetWorkingCopyPath().c_str();

  // Don't use PATH_SEP here, since on Windows that's '\\', which is
  // non-canonical for URLs (and svn paths) and causes errors.
  m_repo_user_path = m_repo_url + '/' + "repos" + '/' + m_username;  // path to the user's dir in the repo

  // Make a directory named based on the project name.
  String proj_name = m_filename.before(".proj");
  m_wc_proj_path = m_wc_path + '/' + proj_name;

  // Make subdirectories for various files (job params, models, results).
  m_wc_submit_path = m_wc_proj_path + '/' + "submit";
  m_wc_models_path = m_wc_proj_path + '/' + "models";
  m_wc_results_path = m_wc_proj_path + '/' + "results";

  // TODO: What is this for?
  String repo_proj_path = m_repo_user_path + '/' + proj_name;

  taMisc::Info("repo is at " + m_repo_user_path); // TODO remove this
  taMisc::Info("wc is at " + m_wc_path); // TODO remove this
}

void
ClusterManager::ensureWorkingCopyExists()
{
  // check if the user has a wc. checkout a wc if needed
  QFileInfo fi_wc(m_wc_path.chars());
  if (!fi_wc.exists()) {
    // User never used c2c or at least never used it on this emergent instance.
    taMisc::Info("wc wasn't found at " + m_wc_path); // TODO remove this
    // checkout the user's dir
    int co_rev = 0;
    //co_rev = m_svn_client->Checkout(m_repo_url, false);  // the m_repo_url directory with no content will be checked out

    taMisc::Info("will try to mkdir " + m_repo_user_path); // TODO remove this
    String comment = "Creating cluster directory for user ";
    comment += m_username;
    co_rev = m_svn_client->MakeUrlDir(m_repo_user_path.chars(), comment.chars());

    //m_svn_client->Checkin(); // commit the created wc
    taMisc::Info("user's dir created at " + m_repo_user_path); // TODO remove this
    co_rev = m_svn_client->Checkout(m_repo_user_path);
    taMisc::Info("working copy checked out at " + m_wc_path); // TODO remove this
    //PrintCheckoutMessage(co_rev, m_repo_user_path, m_wc_path);

    // check if the user has a dir in the repo. create it for her if needed
    if (!fi_wc.exists()) {  // user doesn't have a dir in the repo
      // TODO: create a dir for the user in the repo directly
      co_rev = m_svn_client->Checkout(m_repo_user_path);
      //PrintCheckoutMessage(co_rev, m_repo_user_path, m_wc_path);
      taMisc::Info("user's dir was created on the repo and checked out"); // TODO remove this
    }
  }
  else {
    // update the existing wc
    bool update_success = m_svn_client->Update();
    //PrintUpdateMessage(update_success, m_wc_proj_path);
    taMisc::Info("existing wc was updated"); // TODO remove this
  }
}

void
ClusterManager::createSubdirs()
{
  // Check if the project's dir already exists.
  // Create the project's dir and subdirs if needed.
  QFileInfo fi_proj(m_wc_proj_path);
  if (!fi_proj.exists()) {
    // It's a new project, create a dir and subdirs for it.
    try {
      bool mkdir_success = false;
      mkdir_success = m_svn_client->MakeDir(m_wc_proj_path);
      mkdir_success = m_svn_client->MakeDir(m_wc_submit_path);
      mkdir_success = m_svn_client->MakeDir(m_wc_models_path);
      mkdir_success = m_svn_client->MakeDir(m_wc_results_path);
      taMisc::Info("new project directory was created"); // TODO remove this
    }
    catch (const SubversionClient::Exception &ex) {
      if (ex.GetErrorCode() == SubversionClient::EMER_ERR_ENTRY_EXISTS) {
        // Not a show stopper if the directories already exist.
        // TODO: Wrap each MakeDir() with try/catch, or better, make a
        // TryMakeDir() API.  No need for this Info() log in that case.
        taMisc::Info("Directory already exist.", ex.what());
      }
      else {
        throw;
      }
    }
  }
  else {
    // the project already exists (possibilities: user is running the same
    // project, running the same project with different parameters, duplicate
    // submission)
    // TODO: warn user. what should be done here? (options: create a new dir
    // for the project with a version number like "/project_2", use the
    // existing project dir and attach a version number to the model file
    // names, replace the old project with the new one)
    taMisc::Info("the project already exsits"); // TODO remove this
  }
}

void
ClusterManager::createParamFile()
{
  // generate a txt file containing the model parameters
  String model_filename = "model.txt";  // TODO might need a version number
  String wc_model_path = m_wc_models_path + '/' + model_filename;
  QFile file(wc_model_path);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  QTextStream out(&file);
  // TODO Where can I get the model parameters from?
  // DPF: If this is supposed to be the project file, probably makes sense to
  // save it in its regular place, then do a Qt-based file copy over to this
  // location.  If you instead did a SaveAs, then emergent would remember
  // this location, which could confuse the user.
  // On the other hand, if this is supposed to be a param file, then it needs
  // to be generated based on m_select_edit.
  out << "<CONTENT OF THE MODEL>";
  file.close(); // close manually before committing

  // add the generated model file to the wc
  bool mkdir_success = false;
  // TODO: MakeDir() looks like the wrong call since this is a file.  Add()?
  mkdir_success = m_svn_client->MakeDir(wc_model_path);
}
