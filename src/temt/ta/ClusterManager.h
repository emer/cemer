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
  bool BeginSearch(bool prompt_user);
  bool CommitJobSubmissionTable();
  bool UpdateTables();
  String GetWcProjFilename() const;
  String GetWcSubmitFilename() const;
  int GetLastChangedRevision(const String &path, bool quiet = false);

protected:
  // This exception class only used internally.
  class Exception : public std::runtime_error {
  public:
    explicit Exception(const char *msg);
  };

  void handleException(const SubversionClient::Exception &ex);
  void saveProject();
  const String & getFilename();
  const String & getUsername();
  const String & getClusterName();
  const String & getRepoUrl();
  const String & promptForString(const String &str, const char *msg);
  bool showRepoDialog();
  void setPaths();
  void updateWorkingCopy();
  void runSearchAlgo();
  bool loadTable(const String &filename, DataTable &table);
  void saveSubmitTable();
  void saveCopyOfProject();
  void commitFiles(const String &commit_msg);
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
  String m_done_dat_filename;
};

#endif // CLUSTER_MANAGER_H_
