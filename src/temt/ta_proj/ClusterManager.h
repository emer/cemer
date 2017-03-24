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
#include <QThread>
#include <QString>
#include <ClusterRun_QObj>

class ClusterRun;
class DataTable;
class taProject;
class ClusterManager;

// note: this is not processed by maketa!

class  ClusterManager_UpdtThr : public QThread {
  Q_OBJECT
private:
  ClusterManager * m_cm;
  ClusterRun_QObj * qt_object_helper;
  QStringList credentialsAvailable;
  
  
  int  UpdateWorkingCopy();
  int  UpdateWorkingCopy_impl(SubversionClient* sc, const String& wc_path,
                                            const String& user, const String& clust, const String& projname,
                              bool main_svn);
protected:
  
  SubversionClient* m_svn_other;
  void run() override;
signals:
  void UpdatedSVN();
  void sendError(const QString msg);
  void sendInfo(const QString msg);
public:
  int isUpdating;
  ClusterManager_UpdtThr (ClusterManager * cm,  ClusterRun_QObj * qt_object_helper, QObject *parent = 0);
  void EnsureSVNCredentialsAvailable();
};


class TA_API ClusterManager   {
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
  bool UpdateTables(bool do_svn_update);
  // update the jobs_running and jobs_done data tables from repo -- optionally doing an svn update to get latest
  bool RemoveFiles(String_PArray& files, bool force = true, bool keep_local = false);
  // remove given files from repository, with given options -- does the commit too
  bool GetProjectAtRev(String cluster, String username, String orig_filename, int rev);
  // do an svn update -r rev on the project file from the given svn revision number -- copies the project file to projname_rev.proj in original project directory (visible to user) and then does an update back to current revision so as to not mess up anything else
 
  String ChooseCluster(const String& prompt);
  // prompt the user to choose a cluster name -- just pulls up a simple combo-edit chooser dialog

  void  AddFile(const String& file_path);
  // add a file from working copy to svn
  void  CommitFiles(const String &commit_msg);
  // commit current working copy files
  int InitiateBackgroundSVNUpdate();
  // update current working copy files in the background on a different thread
  int   UpdateWorkingCopy();
  // update current working copy files -- returns the current svn revision number
  void  Cleanup();

  int    GetCurSvnRev() const { return m_cur_svn_rev; }
  // get the current svn revision number
  String GetFullUrl() const { return m_repo_user_url; }
  // get the full url to repository
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

  const String GetUsername();
  const String GetFilename();
  const String GetClusterName();
  const String GetSvnRepo();
  const String GetRepoUrl();
  
  const String GetRepoUrl_UserClust(const String& user, const String& clust);
  // get the svn repository URL for current project with specific user and cluster
  const String GetWcPath_UserClust(const String& wc_path,
                                   const String& user, const String& clust);
  // get the svn working copy path for specific user and cluster -- wc_path can be any specific path from various GetWc*Path methods


protected:
  ClusterRun_QObj * qt_helper_object;
  // This exception class only used internally.
  class Exception : public std::runtime_error {
  public:
    explicit Exception(const char *msg);
  };

  void HandleException(const SubversionClient::Exception &ex);

public:
  enum EmerVersion {
    VERSION_CURRENT,
    VERSION_STABLE,
    VERSION_SPECIFIC,
    VERSION_COUNT
  };
  
  bool SaveProject();

  int  UpdateWorkingCopy_impl(SubversionClient* sc, const String& wc_path,
                              const String& user, const String& clust, const String& projname,
                              bool main_svn);
  // implementation of wc update
  
  bool ShowRepoDialog();
  bool SetPaths(bool updt_wc = true);
  void RunSearchAlgo();
  void InitClusterInfoTable();
  bool LoadTable(const String &filename, DataTable &table);
  bool MergeTableToSummary(DataTable& sum_tab, DataTable& src_tab,
                           const String& clust, const String& user);
  bool LoadAllTables(const String &filename, DataTable& sum_table, DataTable& tmp_table);
  bool LoadMyRunningTable(); // for job running on cluster, true if loaded new data
  bool LoadMyRunningCmdTable(); // for job running on cluster, true if loaded new data
  bool SaveMyRunningCmdTable(); // for job running on cluster
  void SaveSubmitTable();
  void SaveCopyOfProject();
  void SaveExtraFiles();
  void SaveDoneTable();
  void DeleteFile(const String &filename);

  ClusterRun& m_cluster_run;
  ClusterManager_UpdtThr* m_updtThr;
  bool m_valid;
  SubversionClient* m_svn_client;
  SubversionClient* m_svn_other; // other user or cluster svn client
  taProject* m_proj;

  bool   m_do_svn_update;
  int    m_cur_svn_rev;
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
  String m_clusterscript_timestamp_filename;
  String m_done_dat_filename;
  String m_archive_dat_filename;
  String m_deleted_dat_filename;

};


#endif // CLUSTER_MANAGER_H_
