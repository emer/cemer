// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef ClusterRun_h
#define ClusterRun_h 1

// parent includes:
#include <SelectEdit>

// member includes:
#include <DataTable>
#include <ParamSearchAlgo_List>
#include <ParamSearchAlgo>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class iDataTableEditor; //
class ClusterManager; //
taTypeDef_Of(GridSearch);


taTypeDef_Of(ClusterRun);

class TA_API ClusterRun : public SelectEdit {
  // interface for running simulations remotely on a cluster-like computing resource (including cloud computing systems) through an SVN-based file exchange protocol -- cluster-side job control script must also be running
  INHERITED(SelectEdit)
public:
  DataTable     jobs_submit;    // #NO_SAVE current set of jobs to submit
  DataTable     jobs_submitted; // #NO_SAVE jobs submitted -- just a local copy of jobs_submit
  DataTable     jobs_running;   // #SHOW_TREE jobs that are currently running
  DataTable     jobs_done;      // #SHOW_TREE jobs that have finished running
  ParamSearchAlgo_List search_algos; // #SHOW_TREE Possible search algorithms to run on the cluster
  ParamSearchAlgoRef cur_search_algo; // The current search algorithm in use

  String        last_submit_time; // #READ_ONLY #SAVE time stamp when jobs were last submitted -- important also for ensuring that there is a diff to trigger svn commit of project!
  String        notes;          // notes for the job -- describe any specific information about the model configuration etc -- can use this for searching and sorting results
  String        repo_url;       // svn repository url to use for file exchange with the cluster -- this should be the name of a cluster as listed in the Preferences / Options settings
  String        cluster;        // name of cluster to run job on
  String        queue;          // if specified, indicate a particular queue on the computing resource
  String        run_time;       // how long will the jobs take to run -- syntax is number followed by unit indicator -- m=minutes, h=hours, d=days -- e.g., 30m, 12h, or 2d -- typically the job will be killed if it exceeds this amount of time, so be sure to not underestimate
  int           ram_gb;         // how many gigabytes of ram is required?  -1 means do not specify this parameter for the job submission -- for large memory jobs, it can be important to specify this to ensure proper allocation of resources
  int           n_threads;      // number of parallel threads to use for running
  bool          use_mpi;        // use message-passing-inteface distributed memory executable to run across multiple nodes?
  int           mpi_nodes;      // #CONDSHOW_ON_use_mpi number of nodes to use for mpi run

protected:
  void initClusterManager();
  ClusterManager *m_cm;

public:
  virtual void  NewSearchAlgo(TypeDef *type = &TA_GridSearch);
  // #BUTTON #TYPE_0_ParamSearchAlgo Choose a search algorithm to use in this cluster run.
  virtual void  Run();
  // #BUTTON Run this model on a cluster using the parameters as specified here -- commits project file to repository.
  virtual bool  Update();
  // #BUTTON poll for job status of running jobs, grabbing any current results, and moving any completed jobs over to jobs_done -- returns true if new data or status was available
  virtual void  Cont();
  // #BUTTON Continue the search process by submitting the next batch of jobs.
  virtual void  Kill();
  // #BUTTON kill running jobs in the jobs_running datatable (must select rows for jobs in gui)
  virtual void  ImportData();
  // #BUTTON import the data for the selected rows in the jobs_done data table

  virtual void  FormatTables(); // format all the jobs tables to contain proper columns

  // These APIs are mainly for the search algos to use.
  virtual void  AddJobRow(const String &cmd, int cmd_id);
  // add a new job row with given command and arbitrary id number, which is typically the iteration of the search algorithm
  virtual void  CancelJob(int running_row);
  // cancel a job at the given row of the jobs_running data table
  virtual int   CountJobs(const DataTable &table, const String &status_regexp);
  // count the number of jobs in given table with given status value 

  virtual String RunCommand(bool use_cur_vals = false);
  // get the run command based on the currently selected search args in this select edit, and other parameters -- if use_cur_vals, then it passes the current values of the items, otherwise it uses the next_val setting, which should be set by the search algorithm prior to calling this function
  virtual void  CreateCurJob(int cmd_id = 0);
  // AddJobRow for the current parameter values as listed in the select edit, optionally with given command id number

  SIMPLE_COPY(ClusterRun);
  SIMPLE_CUTLINKS(ClusterRun);
  void InitLinks();
  TA_BASEFUNS(ClusterRun);
protected:
  virtual void FormatTables_impl(DataTable& dt);
  // all tables have the same format -- this ensures it
  virtual iDataTableEditor* DataTableEditor(DataTable& dt);
  // get editor for data table
  virtual bool  SelectedRows(DataTable& dt, int& st_row, int& end_row);
  // get selected rows in editor

private:
  void  Initialize();
  void  Destroy();
};

#endif // ClusterRun_h
