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
#include "ta_string.h"

class ClusterRun;
class SubversionClient;
class taProject;

class TA_API ClusterManager
{
public:
  ClusterManager(ClusterRun &cluster_run);
  ~ClusterManager();
  void SetRepoUrl(const char *repo_url);
  void SetDescription(const char *description);
  void UseMpi(int num_mpi_nodes);
  bool Run(bool prompt_user);

private:
  bool saveProject();
  bool showRepoDialog();
  void setPaths();
  void ensureWorkingCopyExists();
  void createSubdirs();
  void runSearchAlgo();
  void saveCopyOfProject();
  void createParamFile();

  ClusterRun &m_cluster_run;
  SubversionClient *m_svn_client;
  taProject *m_proj;
  String m_filename;
  String m_username;
  String m_wc_path;
  String m_repo_user_path;
  String m_wc_proj_path;
  String m_wc_submit_path;
  String m_wc_models_path;
  String m_wc_results_path;
};

#endif // CLUSTER_MANAGER_H_
