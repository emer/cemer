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
class DataTable_Group; //
taTypeDef_Of(GridSearch);


taTypeDef_Of(ClusterRun);

class TA_API ClusterRun : public SelectEdit {
  // interface for running simulations remotely on a cluster-like computing resource (including cloud computing systems) through an SVN-based file exchange protocol -- cluster-side job control script must also be running
  INHERITED(SelectEdit)
public:
  static String timestamp_fmt;  // #NO_SAVE #HIDDEN time stamp format string -- yyyy_MM_dd_hh_mm_ss

  DataTable     jobs_submit;    // #NO_SAVE #EXPERT current set of jobs to submit
  DataTable     jobs_submitted; // #NO_SAVE #EXPERT jobs submitted -- just a local copy of jobs_submit
  DataTable     jobs_running;   // #SHOW_TREE #EXPERT #NO_SAVE jobs that are currently running
  DataTable     jobs_done;      // #SHOW_TREE #EXPERT #NO_SAVE jobs that have finished running
  DataTable     jobs_archive;   // #SHOW_TREE #EXPERT #NO_SAVE jobs that have been archived -- already analyzed but possibly still relevant to look at
  DataTable     file_list;      // #SHOW_TREE #EXPERT #NO_SAVE list of files -- used for various operations -- transferring and deleting
  DataTable     cluster_info;   // #SHOW_TREE #EXPERT #NO_SAVE cluster status information and list of jobs currently running, etc
  ParamSearchAlgo_List search_algos; // #SHOW_TREE #EXPERT Possible search algorithms to run on the cluster
  ParamSearchAlgoRef cur_search_algo; // The current search algorithm in use -- if not set, then jobs will just use current parameters, for manual param searching

  String        last_submit_time; // #READ_ONLY #SHOW #SAVE time stamp when jobs were last submitted -- important also for ensuring that there is a diff to trigger svn commit of project!
  String        notes;          // notes for the job -- describe any specific information about the model configuration etc -- can use this for searching and sorting results
  String        extra_files;    // space separated list of extra files to check into the repository along with this project
  String        svn_repo;       // svn repository to use for file exchange with the cluster -- this should be the name of a svn_repo as listed in the Preferences / Options settings (when you Run a job you can pick from a dropdown list)
  String        repo_url;       // #READ_ONLY #SHOW svn repository url to use for file exchange with the cluster -- this is looked up from svn_repo name from options listed in the Preferences / Options settings
  String        cluster;        // name of cluster to run job on -- see Preferences / Options settings for list of valid names  (when you Run a job you can pick from a dropdown list)
  String        queue;          // if specified, indicate a particular queue on the computing resource
  String        run_time;       // how long will the jobs take to run -- syntax is number followed by unit indicator -- m=minutes, h=hours, d=days -- e.g., 30m, 12h, or 2d -- typically the job will be killed if it exceeds this amount of time, so be sure to not underestimate
  int           ram_gb;         // how many gigabytes of ram is required?  0 means do not specify this parameter for the job submission -- for large memory jobs, it can be important to specify this to ensure proper allocation of resources -- the status_info field can often show you how much a job has used in the past
  int           n_threads;      // number of parallel threads to use for running
  bool          use_mpi;        // use message-passing-inteface distributed memory executable to run across multiple nodes?
  int           mpi_nodes;      // #CONDSHOW_ON_use_mpi number of nodes to use for mpi run
  bool          parallel_batch; // use parallel batch processing -- run multiple runs of the same model in parallel across nodes or procs (not using mpi -- just embarassingly parallel separate runs), each on a different batch iteration (e.g., different initial random weights) -- this will submit a different job for each batch here on the client (so they can all be tracked directly), unless the cluster has allocate_by_node checked, in which case the params will be sent up to the server to manage as a single meta-job
  int           pb_batches;     // #CONDSHOW_ON_parallel_batch number of parallel batches to run per job -- beware that this can result in very large processor counts if doing in context of a parameter search on top, as this batch multiplier operates on each job submitted
  int           pb_nodes;       // #CONDSHOW_ON_parallel_batch if the cluster uses by_node job allocation strategy, then this is the number of nodes to request for this job -- if you want all of your jobs to run in parallel at the same time, then this should be equal to (pb_batches * n_threads * mpi_nodes) / procs_per_node -- setting this value to 0 will default to this allocation number
  bool          nowin_x;        // use the -nowin startup command instead of -nogui and add a _x suffix to the executable command (e.g., emergent_x or emergent_x_mpi), to call a version of the program (a shell wrapper around the standard compiled executable) that opens up an XWindows connection to allow offscreen rendering and other such operations, even in batch mode

protected:
  bool initClusterManager();
  ClusterManager *m_cm;

public:

  ////////////////////////////////////////////
  // main user GUI

  virtual void  Probe();
  // #BUTTON probe the cluster you select to update the current status of the cluster, and of all the jobs for this project, and triggers continued updating on status going forward -- a good idea to run this when opening a project before submitting jobs -- this is necessary for example if the cluster script has been restarted since the last job was run on this project -- this will also fill in the job_out, dat_files, and other_files fields for all running jobs -- this information is only automatically recorded for the first few minutes that a job has been running, so it will miss other files generated later
  virtual void  Run();
  // #BUTTON Run this model on a cluster using the parameters as specified here -- commits project file to repository -- if cur_search_algo is selected then this will launch a parameter search process -- otherwise it will just run with current parameters
  virtual bool  Update();
  // #BUTTON updates jobs_running and jobs_done tables based on latest results from the cluster -- returns true if new data or status was available -- cluster will only send updates if a job was Run or a Probe was sent from this project, while the script is running
  virtual void  Cont();
  // #BUTTON Continue the search process by submitting the next batch of jobs.
  virtual void  Kill();
  // #BUTTON #CONFIRM kill running jobs in the jobs_running datatable (must select rows for jobs in gui)
  virtual void  GetData();
  // #BUTTON tell the cluster to check in the data for the selected rows in the jobs_running or jobs_done data table (looks in running first, then done for selected rows) -- do Update to get data locally after enough time for the cluster to have checked in the data (depends on size of data and cluster responsiveness and poll interval) -- then do ImportData on selected jobs to import data into project
  virtual void  ImportData(bool remove_existing = true);
  // #BUTTON import the data for the selected rows in the jobs_running or jobs_done or file_list data tables -- imports each of the job's data into data.ClusterRun datatables with file name = tag, and columns added for each of the parameter values that were set in the command -- if remove_existing is set, any existing files are removed prior to loading the new ones

  virtual void  SelectFiles(bool include_data = false);
  // #MENU_BUTTON #MENU_ON_Files list all the other_files associated with jobs selected in the jobs_running or jobs_done or jobs_archive data table (looks in running first, then done, then archive for selected rows) -- if include_data is selected, then it includes the dat_files too -- you can then go to the file_list tab to select the specific files you want to operate on for other operations in this menu
  virtual void  ListAllFiles();
  // #MENU_BUTTON #MENU_ON_Files list all the files currently in the results subdirectory of this project's svn repository -- you can then go to the file_list tab to select the specific files you want to operate on for other operations in this menu
  virtual void  GetFiles();
  // #MENU_BUTTON #MENU_ON_Files tell the cluster to check in the files selected in file_list tab -- you can then do Update after enough time for the cluster to have checked in the data (depends on size of data and cluster responsiveness and poll interval), and then access the files as you wish
  virtual void  RemoveFiles();
  // #MENU_BUTTON #MENU_ON_Files #CONFIRM remove all the files selected in the file_list tab -- this does an svn remove and also removes the files locally -- for cleaning up stuff you are done with
  virtual void  GetProjAtRev();
  // #MENU_BUTTON #MENU_ON_Files #MENU_SEP_BEFORE get project file at selected revision (must have one and only one job row selected in any of the jobs tables -- searches in running, done, then archive) -- saves file to projname_rev.proj -- you can then load that and revert project to it by saving back to original project file name if that is in fact what you want to do
  virtual void  ArchiveJobs();
  // #MENU_BUTTON #MENU_ON_Jobs #CONFIRM move jobs selected in the jobs_done data table into the jobs_archive table
  virtual void  RemoveJobs();
  // #MENU_BUTTON #MENU_ON_Jobs #CONFIRM remove jobs selected in the jobs_done or jobs_archive data tables, including all their data that has been checked in (according to the local contents of the repository -- good idea to do an Update before running this) -- for cleaning up old unneeded jobs
  virtual void  RemoveKilledJobs();
  // #MENU_BUTTON #MENU_ON_Jobs #CONFIRM remove ALL jobs in the jobs_done data table with a status of KILLED, including all their data that has been checked in (according to the local contents of the repository -- good idea to do an Update before running this)
  virtual void  NewSearchAlgo(TypeDef *type = &TA_GridSearch);
  // #MENU_BUTTON #MENU_ON_Jobs #TYPE_0_ParamSearchAlgo Choose a search algorithm to use in this cluster run.

  ////////////////////////////////////////////
  // useful helper routines for above

  virtual void ImportData_impl(DataTable_Group* dgp, const DataTable& table, int row);
  // #IGNORE actually do the import -- row is row in given table (jobs_running or jobs_done) with info for data files
  virtual void GetFileInfo(const String& path, DataTable& table, int row, String& tag);
  // #IGNORE get file info from given full path to file into file_list formatted data table at given row -- if tag is empty it will attempt to set it from the file name, and set it, also returning it
  virtual void SelectFiles_impl(DataTable& table, int row, bool include_data);
  // #IGNORE add files from row in table to file_list
  virtual void  RemoveAllFilesInList();
  // #IGNORE svn remove all the files listed in file_list
  void SortClusterInfoTable();
  // #IGNORE sort the cluster_info table putting user's jobs and summary info at the top

  // statics -- should move to a more central location 

  static void AddParamsToTable(DataTable* dat, 
                               const String& tag, const String& tag_svn, 
                               const String& tag_job, const String& params);
  // add parameter values to data table as extra columns -- params is space-separated list of name=value pairs -- also adds the tag and two separate sub-tag columns: tag_svn, tag_job


  ////////////////////////////////////////////
  //  These APIs are mainly for the search algos to use to run jobs

  virtual void  FormatTables();
  // format all the jobs tables to contain proper columns

  virtual bool  ValidateJob(int n_jobs_to_sub = 1);
  // validate all the current parameters and ensure that they make sense for selected cluster, etc -- arg is number of jobs that will be submitted of this form
  virtual String CurTimeStamp();
  // get a timestamp string for the current time 
  virtual void  AddJobRow(const String& cmd, const String& params, int& cmd_id);
  // add a new job row with given command and arbitrary id number, which is typically the iteration of the search algorithm -- it is always incremented here, and can be incremented by pb_batches for parallel batch mode
    virtual void  AddJobRow_impl(const String& cmd, const String& params, int cmd_id);
    // #IGNORE impl
  virtual void  CancelJob(int running_row);
  // cancel a job at the given row of the jobs_running data table
  virtual void  GetDataJob(const DataTable& table, int row);
  // add to jobs_submit for get data for job at the given row of the given table
  virtual void  GetRemoveJob(const DataTable& table, int row);
  // add to jobs_submit for remove job for job at the given row of the given table
  virtual void  GetArchiveJob(const DataTable& table, int row);
  // add to jobs_submit for move job to archive for job at the given row of the given table
  virtual void  GetFilesJob(const String& files);
  // add to jobs_submit for get files for given list of files (space separated)
  virtual int   CountJobs(const DataTable& table, const String &status_regexp);
  // count the number of jobs in given table with given status value 

  virtual void  RunCommand(String& cmd, String& params, bool use_cur_vals = false);
  // get the run command and params based on the currently selected search args in this select edit, and other parameters -- if use_cur_vals, then it passes the current values of the items, otherwise it uses the next_val setting, which should be set by the search algorithm prior to calling this function
  virtual void  CreateCurJob(int cmd_id = 0);
  // AddJobRow for the current parameter values as listed in the select edit, optionally with given command id number

  // MISC impl

  virtual void  FormatJobTable(DataTable& dt);
  // all job tables have the same format -- this ensures it
  virtual void  FormatFileListTable(DataTable& dt);
  // for file_list table
  virtual void  FormatClusterInfoTable(DataTable& dt);
  // for cluster_info table
  virtual iDataTableEditor* DataTableEditor(DataTable& dt);
  // get editor for data table
  virtual bool  SelectedRows(DataTable& dt, int& st_row, int& end_row);
  // get selected rows in editor
  virtual bool  SelectRows(DataTable& dt, int st_row, int end_row);
  // select range of rows in given data table

  SIMPLE_COPY(ClusterRun);
  SIMPLE_CUTLINKS(ClusterRun);
  void InitLinks();
  TA_BASEFUNS(ClusterRun);
protected:
  override void UpdateAfterEdit_impl();

private:
  void  Initialize();
  void  Destroy();
};

#endif // ClusterRun_h
