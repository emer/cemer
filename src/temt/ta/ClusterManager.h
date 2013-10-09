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

#ifndef CLUSTER_MANAGER_H_
#define CLUSTER_MANAGER_H_

#include "ta_def.h"
#include <taString>
#include <SubversionClient>

class ClusterRun;
class DataTable;
class taProject;


class TA_API ClusterManager {
  // The ClusterManager class handles all Subversion operations and doesn't know anything about the contents of the job DataTables. The ClusterRun class (and its search algorithm) take care of everything DataTable related and for the most part do no Subversion operations.
public:
  ClusterManager(ClusterRun &cluster_run);
  ~ClusterManager();

  void  Init();
  // initialize state for current ClusterRun settings, etc..
  static bool CheckPrefs();
  // check state of options/preferences 

  bool BeginSearch(bool prompt_user);
  // starts a run and pulls up the dialog to fill in parameters based on ClustRun obj
  bool CommitJobSubmissionTable();
  // commit the jobs_submit.dat file with latest changes
  bool UpdateTables();
  // update the jobs_running and jobs_done data tables from repo
  bool RemoveFiles(String_PArray& files, bool force = true, bool keep_local = false);
  // remove given files from repository, with given options -- does the commit too
  bool GetProjectAtRev(int rev);
  // do an svn update -r rev on the project file from the given svn revision number -- copies the project file to projname_rev.proj in original project directory (visible to user) and then does an update back to current revision so as to not mess up anything else
 
  String ChooseCluster(const String& prompt);
  // prompt the user to choose a cluster name -- just pulls up a simple combo-edit chooser dialog

  void commitFiles(const String &commit_msg);
  // commit current working copy files
  void updateWorkingCopy();
  // update current working copy files

  String GetWcProjPath() const;
  // full path to cluster_svn_path/svn_repo/clustername/username/projname/ -- root of the svn repo for this project
  String GetWcResultsPath() const;
  // full path to cluster_svn_path/svn_repo/clustername/username/projname/results 
  String GetWcSubmitPath() const;
  // full path to cluster_svn_path/svn_repo/clustername/username/projname/submit
  String GetWcModelsPath() const;
  // full path to cluster_svn_path/svn_repo/clustername/username/projname/models
  String GetWcProjFilename() const;
  // full path to cluster_svn_path/svn_repo/clustername/username/projname/models/projname.proj
  String GetWcSubmitFilename() const;
  // full path to cluster_svn_path/svn_repo/clustername/username/projname/submit/jobs_submit.dat
  String GetWcClusterInfoFilename() const;
  // full path to cluster_svn_path/svn_repo/clustername/username/cluster_info.dat
  int GetLastChangedRevision(const String &path, bool quiet = false);

  bool  HasBasicData(bool err = false);
  // check that the current config has all the basic data needed to set paths and manage svn files: username, filename, clustername, svn repo, repo url -- if err then issue error for missing items

  const String getUsername();
  const String getFilename();
  const String getClusterName();
  const String getSvnRepo();
  const String getRepoUrl();

protected:
  // This exception class only used internally.
  class Exception : public std::runtime_error {
  public:
    explicit Exception(const char *msg);
  };

  void handleException(const SubversionClient::Exception &ex);
  void saveProject();

  bool showRepoDialog();
  bool setPaths(bool updt_wc = true);
  void runSearchAlgo();
  void initClusterInfoTable();
  bool loadTable(const String &filename, DataTable &table);
  void saveSubmitTable();
  void saveCopyOfProject();
  void saveExtraFiles();
  void saveDoneTable();
  void deleteFile(const String &filename);

  ClusterRun &m_cluster_run;
  bool m_valid;
  SubversionClient *m_svn_client;
  taProject *m_proj;

  String m_username;
  String m_wc_path;
  String m_repo_user_url;
  String m_wc_proj_path;
  String m_wc_submit_path;
  String m_wc_models_path;
  String m_wc_results_path;
  String m_proj_copy_filename;
  String m_submit_dat_filename;
  String m_running_dat_filename;
  String m_cluster_info_filename;
  String m_done_dat_filename;
  String m_archive_dat_filename;
};

#endif // CLUSTER_MANAGER_H_
