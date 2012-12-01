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

namespace { // anonymous
  bool ShowRunOnClusterDialog(String &repo_url, String &description)
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

    dlg.AddStringField(&description, "description", widget, row,
      "tooltip=enter a description to be used as a checkin comment;");

    bool modal = true;
    int drval = dlg.PostDialog(modal);
    if (drval == 0) return false; // User cancelled.

    repo_url = combo->itemData(combo->currentIndex()).toString();
    return true;
  }
}

ClusterManager::ClusterManager(const SelectEdit *select_edit)
  : m_select_edit(select_edit)
{
}

bool
ClusterManager::Run()
{
  // Get the project object.
  taProject *proj = GET_OWNER(m_select_edit, taProject);
  if (!proj) {
    taMisc::Error("Could not get project object to run on cluster.");
    return false;
  }

  // Get the project's filename and make sure it has been saved at least once.
  // This intentionally doesn't check to see if the user has unsaved changes!
  const String filename = proj->file_name;
  if (filename.empty()) {
    taMisc::Error(
      "Please save project locally before attempting to run on cluster.");
    return false;
  }

  // Prompt the user for a repository and a description for this cluster run.
  String repo_url;
  String description;
  if (!ShowRunOnClusterDialog(repo_url, description)) {
    taMisc::Info("Running on cluster cancelled.");
    return false;
  }

  taMisc::Info("Running project", filename, "\n  on cluster", repo_url,
    "\n  Description:", description);

  try {
    // Run this model on a cluster using the parameters of this SelectEdit.
    taMisc::Info("RunOnCluster() test");

    // Create Subversion client and get the user's username.  Need the URL to
    // get the username from the cache so unfortunately we have to do this
    // after presenting the dialog.  On the other hand, the only way this can
    // fail is if the user provides an empty username, so it's hard for them
    // to screw it up.
    SubversionClient svnClient;
    String username = svnClient.GetUsername(
      repo_url, SubversionClient::CHECK_CACHE_THEN_PROMPT_USER
    ).c_str();

    if (username.empty()) {
      taMisc::Error("A Subversion username is needed to run on a cluster.");
      return false;
    }

    // Set the working copy path and get a canonicalized version back.
    String wc_path = taMisc::user_app_dir + '/' + "repos" + '/' + username;
    svnClient.SetWorkingCopyPath(wc_path.chars());
    wc_path = svnClient.GetWorkingCopyPath().c_str();

    // Don't use PATH_SEP here, since on Windows that's '\\', which is
    // non-canonical for URLs (and svn paths) and causes errors.
    String repo_user_path = repo_url + '/' + "repos" + '/' + username;  // path to the user's dir in the repo
    String proj_name = filename.before(".proj");
    String wc_proj_path = wc_path + '/' + proj_name;
    String wc_submit_path = wc_proj_path + '/' + "submit";  // a subdir of the project
    String wc_models_path = wc_proj_path + '/' + "models";  // a subdir of the project to contain model files
    String wc_results_path = wc_proj_path + '/' + "results";  // a subdir of the project to contain results
    String repo_proj_path = repo_user_path + '/' + proj_name;

    taMisc::Info("repo is at " + repo_user_path); // TODO remove this
    taMisc::Info("wc is at " + wc_path); // TODO remove this

    // check if the user has a wc. checkout a wc if needed
    QFileInfo fi_wc(wc_path.chars());
    if (!fi_wc.exists()) {  // user never used c2c or at least never used it on this emergent instance
      taMisc::Info("wc wasn't found at " + wc_path); // TODO remove this
      // checkout the user's dir
      int co_rev = 0;
      //co_rev = svnClient.Checkout(repo_url, false);  // the repo_url directory with no content will be checked out

      taMisc::Info("will try to mkdir " + repo_user_path); // TODO remove this
      String comment = "Creating cluster directory for user ";
      comment += username;
      co_rev = svnClient.MakeUrlDir(repo_user_path.chars(), comment.chars());

      //svnClient.Checkin(); // commit the created wc
      taMisc::Info("user's dir created at " + repo_user_path); // TODO remove this
      co_rev = svnClient.Checkout(repo_user_path);
      taMisc::Info("working copy checked out at " + wc_path); // TODO remove this
      //PrintCheckoutMessage(co_rev, repo_user_path, wc_path);

      // check if the user has a dir in the repo. create it for her if needed
      if (!fi_wc.exists()) {  // user doesn't have a dir in the repo
        // TODO: create a dir for the user in the repo directly
        co_rev = svnClient.Checkout(repo_user_path);
        //PrintCheckoutMessage(co_rev, repo_user_path, wc_path);
        taMisc::Info("user's dir was created on the repo and checked out"); // TODO remove this
      }
    }
    else {  // update the existing wc
      bool update_success = svnClient.Update();
      //PrintUpdateMessage(update_success, wc_proj_path);
      taMisc::Info("existing wc was updated"); // TODO remove this
    }

    // check if the project's dir already exists. create the project's dir and subdirs if needed
    QFileInfo fi_proj(wc_proj_path);
    if (!fi_proj.exists()) {  // it's a new project, create a dir and subdirs for it
      try {
        bool mkdir_success = false;
        mkdir_success = svnClient.MakeDir(wc_proj_path);
        mkdir_success = svnClient.MakeDir(wc_submit_path);
        mkdir_success = svnClient.MakeDir(wc_models_path);
        mkdir_success = svnClient.MakeDir(wc_results_path);
        taMisc::Info("new project directory was created"); // TODO remove this
      }
      catch (const SubversionClient::Exception &ex) {
        if (ex.GetErrorCode() == SubversionClient::EMER_ERR_ENTRY_EXISTS) {
          taMisc::Info("Directory already exist.", ex.what());
        }
        else {
          throw;
        }
      }
    }
    else { // the project already exists (possibilities: user is running the same project, running the same project with different parameters, duplicate submission)
      // TODO warn user. what should be done here? (options: create a new dir for the project with a version number like "/project_2", use the existing project dir and attach a version number to the model file names, replace the old project with the new one)
      taMisc::Info("the project already exsits"); // TODO remove this
    }

    // generate a txt file containing the model parameters
    String model_filename = "model.txt";  // TODO might need a version number
    String wc_model_path = wc_models_path + '/' + model_filename;
    QFile file(wc_model_path);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "<CONTENT OF THE MODEL>";  // TODO Where can I get the model parameters from?
    // optional, as QFile destructor will already do it
    file.close();

    // add the generated model file to the wc and commit it
    bool mkdir_success = false;
    mkdir_success = svnClient.MakeDir(wc_model_path);

    // checkin the project's directory, subdirectories and model's file created
    svnClient.Checkin("project's directory and model's file created");

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
