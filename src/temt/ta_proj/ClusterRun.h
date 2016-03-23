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
#include <ControlPanel>
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <DataTable>
#include <ParamSearchAlgo_List>
#include <ParamSearchAlgo>
#include <taDateTime>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class iDataTableEditor; //
class ClusterManager; //
class DataTable_Group; //
class iPanelSet; //
class SubversionClient; //

class ClusterRun; //
class ClusterRun_QObj; // #IGNORE

TA_SMART_PTRS(TA_API, ClusterRun); // ClusterRunRef

taTypeDef_Of(GridSearch);

taTypeDef_Of(ClusterRun);

class TA_API ClusterRun : public ControlPanel {
  // interface for running simulations remotely on a cluster-like computing resource (including cloud computing systems) through an SVN-based file exchange protocol -- cluster-side job control script must also be running
  INHERITED(ControlPanel)
public:
  
  enum PanelId {
    PANEL_CONTROL,
    PANEL_RUNNING,
    PANEL_DONE,
    PANEL_DELETED,
    PANEL_ARCHIVE,
    PANEL_FILES,
    PANEL_INFO,
    PANEL_PROPS,
    PANEL_USERDATA
  };
  static String timestamp_fmt;  // #NO_SAVE #HIDDEN time stamp format string -- yyyy_MM_dd_hh_mm_ss

  DataTable     jobs_submit;    // #NO_SAVE #EXPERT #HIDDEN_CHOOSER current set of jobs to submit
  DataTable     jobs_submitted; // #NO_SAVE #EXPERT #HIDDEN_CHOOSER jobs submitted -- just a local copy of jobs_submit
  DataTable     jobs_running;   // #SHOW_TREE #EXPERT #HIDDEN_CHOOSER jobs that are currently running
  DataTable     jobs_done;      // #SHOW_TREE #EXPERT #HIDDEN_CHOOSER jobs that have finished running
  DataTable     jobs_deleted;   // #SHOW_TREE #EXPERT #HIDDEN_CHOOSER jobs that have been deleted -- they can be recovered if needed..
  DataTable     jobs_archive;   // #SHOW_TREE #EXPERT #HIDDEN_CHOOSER jobs that have been archived -- already analyzed but possibly still relevant to look at
  DataTable     file_list;      // #SHOW_TREE #EXPERT #HIDDEN_CHOOSER list of files -- used for various operations -- transferring and deleting
  DataTable     cluster_info;   // #SHOW_TREE #EXPERT #HIDDEN_CHOOSER cluster status information and list of jobs currently running, etc
  DataTable     jobs_running_tmp;  // #NO_SAVE #HIDDEN #HIDDEN_CHOOSER temporary jobs_running, for each specific directory
  DataTable     jobs_done_tmp;  // #NO_SAVE #HIDDEN #HIDDEN_CHOOSER temporary jobs_done, for each specific directory
  DataTable     jobs_deleted_tmp;  // #NO_SAVE #HIDDEN #HIDDEN_CHOOSER temporary jobs_done, for each specific directory
  DataTable     jobs_archive_tmp;  // #NO_SAVE #HIDDEN #HIDDEN_CHOOSER temporary jobs_done, for each specific directory
  ParamSearchAlgo_List search_algos; // #SHOW_TREE #EXPERT Possible search algorithms to run on the cluster
  bool          use_search_algo;     // use search algorithm to explore across parameters -- if false, then just the current values will be used -- must also set cur_search_algo
  ParamSearchAlgoRef cur_search_algo; // #CONDEDIT_ON_use_search_algo The current search algorithm in use -- if not set, then jobs will just use current parameters, for manual param searching -- see also use_search_algo

  bool          set_proj_name;  // set the project name to use -- overrides the default which is to use the actual name of the project -- this can be useful for running multiple variants of the same project with different local file names, all under a common cluster-run project name, so they can all share the same results etc
  String        proj_name;      // #CONDSHOW_ON_set_proj_name project name to use in lieu of the actual project name, when set_proj_name is active
  int           auto_updt_interval; // (10 default) how many seconds to wait between auto-update while waiting for information back from the cluster
  int           auto_updt_timeout; // (30 default) how many seconds to wait before giving up on receiving information back from the cluster -- when it times out, then you can just hit the update button manually
  String        cluster;        // #METHBOX_LABEL name of cluster to run job on -- see Preferences / Options settings for list of valid names -- easiest to use SelectCluster to switch between clusters, and when you Run a job you can pick from a dropdown list
  String        clusters;       // space-separated list of cluster names to include in listing jobs for this project
  String        users;          // space-separated list of user names to include in listing jobs for this project
  int           cur_svn_rev;    // #READ_ONLY #SHOW #NO_SAVE #METHBOX_LABEL the current svn revision that we've updated to (-1 if not yet updated)
  String        last_submit_time; // #READ_ONLY #SHOW #SAVE time stamp when jobs were last submitted -- important also for ensuring that there is a diff to trigger svn commit of project!
  String        notes;          // Notes about this run, used as a checkin comment and visible in job lists -- very good idea to be specific here.  Use %varname to automatically add current variable value
  String        label;          // A label that can be used when plotting data to distinguish this run from another. Use %varname to automatically add current variable value 
  String        extra_files;    // space separated list of extra files to check into the repository along with this project
  String        svn_repo;       // svn repository to use for file exchange with the cluster -- this should be the name of a svn_repo as listed in the Preferences / Options settings (when you Run a job you can pick from a dropdown list)
  String        repo_url;       // #READ_ONLY #SHOW svn repository url to use for file exchange with the cluster -- this is looked up from svn_repo name from options listed in the Preferences / Options settings
  String        queue;          // if specified, indicate a particular queue on the computing resource (optional) -- depends on cluster whether this is used
  String        run_time;       // how long will the jobs take to run -- syntax is number followed by unit indicator -- m=minutes, h=hours, d=days -- e.g., 30m, 12h, or 2d -- typically the job will be killed if it exceeds this amount of time, so be sure to not underestimate
  String        exe_cmd;        // executable command to run the project on the cluster -- defaults to taMisc::app_name (e.g., emergent) -- can be an absolute path or just an executable name that will be found on default path
  int           ram_gb;         // how many gigabytes of ram is required?  0 means do not specify this parameter for the job submission -- for large memory jobs, it can be important to specify this to ensure proper allocation of resources -- the status_info field can often show you how much a job has used in the past
  int           n_threads;      // number of parallel threads to use for running
  bool          use_mpi;        // use message-passing-inteface distributed memory executable to run across multiple nodes?
  int           mpi_nodes;      // #CONDSHOW_ON_use_mpi number of physical nodes to use for mpi run -- total number of nodes is mpi_nodes * mpi_per_node
  int           mpi_per_node;   // #CONDSHOW_ON_use_mpi number of processes (instances of emergent) to use per physical node for mpi run -- mpi_per_node * n_threads must be <= total cores per node -- can be faster to run multiple processes per physical multi-core node, where these processes communicate locally on the same node, instead of the slower inter-node communication fabric
  bool          parallel_batch; // use parallel batch processing -- run multiple runs of the same model in parallel across nodes or procs (not using mpi -- just embarassingly parallel separate runs), each on a different set of batch iterations (e.g., different initial random weights) -- this will submit a different job for each set of batches on the server (so they can all be tracked directly) -- see pb_batches and pb_n_batches_per for relevant parameters
  int           pb_batches;     // #CONDSHOW_ON_parallel_batch #MIN_2 total number of parallel batches to run, with pb_n_batches_per batches allocated per each submitted job -- total number of submitted jobs = pb_batches / pb_n_batches_per -- each submitted job will have two added args: batch_start=0:<pb_patches>:<pb_n_batches_per> (start:stop:incr range notation) and n_batches=<pb_n_batches_per> parameters added -- the Startup program must interpret these args properly -- updated for version 8.0! -- beware that this can result in very large processor counts if doing in context of a parameter search in addition, as this batch multiplier operates on each job submitted
  int           pb_n_batches_per;  // #CONDSHOW_ON_parallel_batch #MIN_1 number of batches to run per parallel batch job -- this number of batches will be run *sequentially* within a single submitted job, while pb_batches / pb_n_batches_per jobs will be submitted in parallel to cover the total pb_batches number of batches specified
  bool          nowin_x;        // use the -nowin startup command instead of -nogui and add a _x suffix to the executable command (e.g., emergent_x or emergent_x_mpi), to call a version of the program (a shell wrapper around the standard compiled executable) that opens up an XWindows connection to allow offscreen rendering and other such operations, even in batch mode
  
  // this group is all about enabling method buttons
  bool          enable_kill;    // #HIDDEN whether to enable the kill action
  bool          enable_load;    // #HIDDEN whether to enable import action
  bool          enable_notes;   // #HIDDEN whether to enable update notes action
  
protected:
  bool InitClusterManager(bool check_prefs = true);

  ClusterManager*       m_cm;
  SubversionClient*     svn_other; // other user or project svn client
  String                svn_other_wc_path; // working copy path
  String                svn_other_url;     // url

public:

  ////////////////////////////////////////////
  // main user GUI

  virtual void  SelectCluster();
  // #BUTTON select a cluster system to use, and update the current status of the cluster, and of all the jobs for this project -- this is the best way to switch between clusters, and it is a good idea to run this when opening a project before submitting jobs
  virtual void  Run();
  // #BUTTON Run this model on a cluster using the parameters as specified here -- commits project file to repository -- if cur_search_algo is selected then this will launch a parameter search process -- otherwise it will just run with current parameters
  virtual bool  Update();
  // #BUTTON updates jobs_running and jobs_done tables based on latest results from the cluster -- returns true if new data or status was available -- cluster will only send updates if a job was Run or a Probe was sent from this project, while the script is running
  virtual void  UpdtRunning();
  // #BUTTON pings the cluster to update status of running jobs -- outside of this command, cluster will only send updates if a job was Run, it finished, or was Killed, or some other kind of update generated by the cluster.  This will also fill in the job_out, dat_files, and other_files fields for all running jobs -- this information is only automatically recorded for the first few minutes that a job has been running, so it will miss other files generated later
  virtual void  Kill();
  // #BUTTON #CONFIRM #GHOST_ON_enable_kill:false running jobs in the jobs_running datatable (must select rows for jobs in gui)
  virtual void  LoadData(bool remove_existing = false);
  // #BUTTON #GHOST_ON_enable_load:false load the data for the selected rows in the jobs_running, jobs_done or file_list data tables (all rows if none selected) -- loads each of the job's data into data.ClusterRun datatables with file name = tag, and columns added for each of the parameter values that were set in the command -- if remove_existing is set, any existing files are removed prior to loading the new ones
  virtual void  SaveJobParams();
  // #MENU_BUTTON #MENU_ON_Jobs save the parameters for selected job(s) in jobs_running, jobs_done or jobs_archive into new ParamSet saved parameters under .param_sets -- convenient way to save different sets of good parameters for later reference -- automatically named with the job name and comments in the desc field
  virtual void  UpdtNotes();
  // #MENU_BUTTON #MENU_ON_Jobs update notes field of completed jobs -- enter a new note in the table and then do this, and it will update the table permanently -- otherwise all edits are overwritten by the version in svn which is only updated on the cluster side
  virtual void  RemoveJobs();
  // #MENU_BUTTON #MENU_ON_Jobs #CONFIRM move jobs to the deleted table, including all their data that has been checked in (according to the local contents of the repository) -- jobs in the deleted table can be undeleted or permanently deleted -- (good idea to do an Update before running this) -- for cleaning up old unneeded jobs
  virtual void  RemoveKilledJobs();
  // #MENU_BUTTON #MENU_ON_Jobs #CONFIRM remove ALL jobs in the jobs_done data table with a status of KILLED, including all their data that has been checked in (according to the local contents of the repository -- good idea to do an Update before running this)
  virtual void  UnDeleteJobs();
  // #MENU_BUTTON #MENU_ON_Jobs #CONFIRM recover jobs selected in the jobs_deleted data table into the jobs_done table, and restore the dat_files into the svn repository as well
  virtual void  ArchiveJobs();
  // #MENU_BUTTON #MENU_ON_Jobs #CONFIRM move jobs selected in the jobs_done data table into the jobs_archive table
  virtual void  Cont();
  // #MENU_BUTTON #MENU_ON_Jobs #CONFIRM Continue the search process by submitting the next batch of jobs.
  virtual ParamSearchAlgo*  NewSearchAlgo(TypeDef *type = &TA_GridSearch);
  // #MENU_BUTTON #MENU_ON_Jobs #MENU_SEP_BEFORE #TYPE_0_ParamSearchAlgo Create a search algorithm to use in this cluster run -- will automatically be selected as the current search algo and enabled (use_search_algo = true)
  virtual bool  AddCluster(const String& clust_nm);
  // #MENU_BUTTON #MENU_ON_Jobs add given cluster to list of active clusters in use -- jobs from these clusters will be displayed in all the jobs* tables -- any cluster you visit will also automatically be added to the list, which is also visible / editable in the Properties tab, in clusters field (space separated list)
  virtual bool  AddUser(const String& user_nm);
  // #MENU_BUTTON #MENU_ON_Jobs add given user to list of active users -- jobs from these users will be displayed in the jobs* tables -- note that access to other user's data is strictly read only.  current list of users is also visible / editable in the Properties tab, in users field (space separated list)

  virtual void  ListJobFiles();
  // #MENU_BUTTON #MENU_ON_Files list all the other_files associated with jobs selected in the jobs_running or jobs_done or jobs_archive data table (looks in running first, then done, then archive for selected rows) -- if include_data is selected, then it includes the dat_files too -- you can then go to the file_list tab to select the specific files you want to operate on for other operations in this menu
  virtual void  ListLocalFiles();
  // #MENU_BUTTON #MENU_ON_Files list all of the project files (e.g. .dat files and .wts files) that have been transferred to your local svn repository, for the current user and cluster
  virtual void  GetFiles();
  // #MENU_BUTTON #MENU_ON_Files get the files shown in the file_list tab -- if these are not local files but are in current cluster, then tell the cluster to check in the files, and they will then be downloaded to the local working copy directory (via auto-update) -- otherwise if not local it will attempt to copy down from svn and add to local svn directory -- if already local, but they are in a different directory (e.g., from a different user / cluster, or project), then they will be copied locally, and checked into the current user / cluster project svn repository
  virtual void  CleanJobFiles();
  // #MENU_BUTTON #MENU_ON_Files #CONFIRM #MENU_SEP_BEFORE remove all the job management files associated with the jobs selected in jobs_done or jobs_archive lists -- these are the JOB.* files and the tagged copy of the project that was used to launch the job(s) -- they are also automatically removed when a job is moved to archived
  virtual void  RemoveFiles();
  // #MENU_BUTTON #MENU_ON_Files #CONFIRM remove all the files selected in the file_list tab -- or associated with selected jobs -- this does an svn remove and also removes the files locally -- for cleaning up stuff you are done with
  virtual void  RemoveNonDataFiles();
  // #MENU_BUTTON #MENU_ON_Files #CONFIRM remove all the non-data files associated with jobs selected in the jobs_done or jobs_archive lists -- these are typically larger files such as weight files, which it is good to clean up eventually
  virtual void  GetProjAtRev();
  // #MENU_BUTTON #MENU_ON_Files #MENU_SEP_BEFORE get project file at selected revision (must have one and only one job row selected in any of the jobs tables -- searches in running, done, then archive) -- saves file to projname_rev.proj -- you can then load that and revert project to it by saving back to original project file name if that is in fact what you want to do
  virtual void  ListOtherProjFiles(const String& proj_name);
  // #MENU_BUTTON #MENU_ON_Files list the files checked into svn for given other project name -- once the files are displayed, you can select files and click on GetFiles to copy those files to your directory
  virtual void  OpenSvnBrowser();
  // #MENU_BUTTON #MENU_ON_Files open subversion browser for this repository

  ////////////////////////////////////////////
  // useful helper routines for above

  virtual void LoadData_impl(DataTable_Group* dgp, const DataTable& table, int row);
  // #IGNORE actually do the import -- row is row in given table (jobs_running or jobs_done) with info for data files
  virtual void GetFileInfo(const String& path, DataTable& table, int row, String& tag,
                           const String& user, const String& clust);
  // #IGNORE get file info from given full path to file into file_list formatted data table at given row -- if tag is empty it will attempt to set it from the file name, and set it, also returning it
  virtual void SelectFiles_impl(DataTable& table, int row, bool include_data);
  // #IGNORE add files from row in table to file_list
  virtual void  RemoveAllFilesInList();
  // #IGNORE svn remove all the files listed in file_list
  virtual void SaveJobParams_impl(DataTable& table, int row);
  // #IGNORE save job parameters to new param_sets saved params
  void SortClusterInfoTable();
  // #IGNORE sort the cluster_info table putting user's jobs and summary info at the top

  // statics -- should move to a more central location 

  static void AddParamsToTable(DataTable* dat, 
                               const String& tag, const String& tag_svn, 
                               const String& tag_job, const String& params,
                               const String& notes, const String& label);
  // add parameter values to data table as extra columns -- params is space-separated list of name=value pairs -- also adds the tag and two separate sub-tag columns: tag_svn, tag_job, notes

  ////////////////////////////////////////////
  //  These APIs are mainly for the search algos to use to run jobs

  virtual void  FormatTables();
  // format all the jobs tables to contain proper columns

  virtual bool  ValidateJob(int n_jobs_to_sub = 1);
  // validate all the current parameters and ensure that they make sense for selected cluster, etc -- arg is number of jobs that will be submitted of this form
  virtual String CurTimeStamp();
  // get a timestamp string for the current time
  virtual void FillInRunningTime(DataTable* table);
  // calculate the running time and write the result into the running_time column
  virtual void FillInElapsedTime(DataTable* table);
  // for jobs still running calculate the elapsed time and write the result into the running_time column
  virtual void  AddJobRow(const String& cmd, const String& params, int& cmd_id);
  // add a new job row with given command and arbitrary id number, which is typically the iteration of the search algorithm -- it is always incremented here, and can be incremented by pb_batches for parallel batch mode
  virtual void  AddJobRow_impl(const String& cmd, const String& params, int cmd_id);
    // #IGNORE impl
  virtual void  CancelJob(int running_row);
  // cancel a job at the given row of the jobs_running data table

  virtual bool  CheckLocalClustUser(const DataTable& table, int row, bool warn = true);
  // make sure that given row of table is for current cluster and user -- otherwise issue info message and return false -- submit functions can only operate on current user and cluster
  virtual bool  CheckLocalClustUserRows(const DataTable& table, int start_row, int end_row);
  // make sure that all rows selected are for current cluster and user -- otherwise ask user if they want to stop or continue -- submit functions can only operate on current user and cluster
  virtual void  SubmitRemoveJob(const DataTable& table, int row);
  // add to jobs_submit for remove job for job at the given row of the given table
  virtual void  SubmitArchiveJob(const DataTable& table, int row);
  // add to jobs_submit for move job to archive for job at the given row of the given table
  virtual void  SubmitUnDeleteJob(const DataTable& table, int row);
  // add to jobs_submit for move job to done from deleted for job at the given row of the given table
  virtual void  SubmitRemoveDelJob(const DataTable& table, int row);
  // add to jobs_submit for remove deleted job for job at the given row of the given table
  virtual void  SubmitCleanJobFiles(const DataTable& table, int row);
  // add to jobs_submit for clean job files for job at the given row of the given table
  virtual void  SubmitGetFiles(const String& files);
  // add to jobs_submit for get files for given list of files (space separated)
  virtual void  SubmitRemoveFiles(const String& files);
  // add to jobs_submit for remove files for given list of files (space separated)
  virtual int   CountJobs(const DataTable& table, const String &status_regexp);
  // count the number of jobs in given table with given status value 
  virtual void  SubmitUpdateNote(const DataTable& table, int row);
  // add to jobs_submit for update of 'done' job
  
  virtual void  RunCommand(String& cmd, String& params, bool use_cur_vals = false);
  // get the run command and params based on the currently selected search args in this control panel, and other parameters -- if use_cur_vals, then it passes the current values of the items, otherwise it uses the next_val setting, which should be set by the search algorithm prior to calling this function
  virtual void  CreateCurJob(int cmd_id = 0);
  // AddJobRow for the current parameter values as listed in the control panel, optionally with given command id number
  virtual void  DoClusterOp(String do_this);
  // execute the specified method - called by the view
  
  // MISC impl

  virtual void      FormatJobTable(DataTable& dt, bool clust_user = false);
  // all job tables have the same format -- this ensures it -- clust_user adds cluster and user fields, for user-visible jobs tables
  virtual void      FormatFileListTable(DataTable& dt);
  // for file_list table
  virtual void      FormatClusterInfoTable(DataTable& dt);
  // for cluster_info table
  virtual iDataTableEditor* DataTableEditor(DataTable& dt);
  // get editor for data table
  virtual bool      SelectedRows(DataTable& dt, int& st_row, int& end_row);
  // get selected rows in editor
  virtual bool      SelectRows(DataTable& dt, int st_row, int end_row);
  // select range of rows in given data table
  virtual void      ClearSelection(DataTable& dt);
  // clear any existing selection
  virtual void      ClearAllSelections();
  // clear any existing selection for all cluster run tables

  virtual String    GetSvnPath();
  // returns the svn repository path currently in effect -- i.e., ClusterManager->GetWcProjPath()

  virtual void      InitOtherSvn(const String& svn_wc_path, const String& svn_url);
  // initialize the svn_other subversion setup for accessing other subversion info
  virtual void      ListOtherSvn(int rev=-1, bool recurse=true);
  // list files in other svn at given revision (-1 for current), and wether to recurse into subdirectories

  // view panel sets etc

  virtual iPanelSet*  FindMyPanelSet();
  // #IGNORE find my panel set, which contains all the more specific view panels (diff data tables)
  virtual bool        ViewPanelNumber(int panel_no);
  // #CAT_Display select the edit/middle panel view of this object to be the given number (0 = ControlPanel, 1 = jobs_running, 2 = jobs_done, 3 = jobs_archive, 4 = file_list, 5 = cluster_info, 6 = Properties)
  virtual void        AutoUpdateMe(bool clear_sels = true);
  // set this cluster run to auto-update to the next revision after one that was just committed -- if clear_sels then clear all selections in tables (action is done -- generally should be true)
  static bool         WaitProcAutoUpdate();
  // auto update to given target revision
  virtual void        UpdateUI();
  // a chance to enable/disable buttons when panel changes or selection changes
  virtual DataTable*  GetCurDataTable();
  virtual String      ReplaceVars(const String& str);
  // #IGNORE replace %varname with variable values

  
  SIMPLE_COPY(ClusterRun);
  SIMPLE_CUTLINKS(ClusterRun);
  void InitLinks() override;
  TA_BASEFUNS(ClusterRun);
protected:
  void                  UpdateAfterEdit_impl() override;

  ClusterRun_QObj*      qt_object_helper;
  bool                  helper_is_connected;

  static ClusterRunRef  wait_proc_updt; // this cluster run object is in auto-update mode
  static int            wait_proc_trg_rev; // this is target revision for it
  static taDateTime     wait_proc_start; // when we first started
  static taDateTime     wait_proc_last_updt; // last time we did an update

private:
  void  Initialize();
  void  Destroy();
};

#endif // ClusterRun_h
