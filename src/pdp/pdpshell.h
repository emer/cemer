// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// pdpshell.h

#ifndef pdpshell_h
#define pdpshell_h 1

#include "ta_project.h"
#include "colorscale.h"
#ifdef TA_GUI
  #include "ta_seledit.h"
  #include "ta_qtviewer.h"
  #include "fontspec.h"
#endif

#include "netstru.h"
#include "netdata.h"
#include "pdplog.h"
#include "program.h"

//////////////////////////////////////////////////
//			Wizard			//
//////////////////////////////////////////////////

class PDP_API LayerWizEl : public taNBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER specifies basic parameters for a layer
public:
  enum InputOutput {
    INPUT,
    HIDDEN,
    OUTPUT
  };

  int		n_units;	// number of units in the layer
  InputOutput 	io_type;	// is it an input, hidden, or output layer -- determines environment patterns

  void	Initialize();
  void	Destroy() 	{ };
  SIMPLE_COPY(LayerWizEl);
  COPY_FUNS(LayerWizEl, taNBase);
  TA_BASEFUNS(LayerWizEl);
};

class PDP_API Wizard : public taWizard {
  // ##BUTROWS_2 ##EDIT_WIDTH_60 wizard for automating construction of simulation objects
INHERITED(taWizard)
public:
  enum Connectivity {
    FEEDFORWARD,		// each layer projects to the next one in sequence
    BIDIRECTIONAL		// layers are bidirectionally connected in sequence (each sends and receives from its neighbors)
  };

  int		n_layers;	// number of layers
  taBase_List	layer_cfg;	// provides configuration information for each layer
  Connectivity	connectivity;	// how to connect the layers
  TypeDef*	event_type;	// #TYPE_Event type of event to create by default

  virtual void	ThreeLayerNet();
  // #MENU_BUTTON #MENU_ON_Defaults set configuration to a standard three-layer network (input, hidden, output) -- DOESN'T MAKE NETWORK (use StdEnv!)
  virtual void	MultiLayerNet(int n_inputs = 1, int n_hiddens = 1, int n_outputs = 1);
  // #MENU_BUTTON set configuration for specified number of each type of layer -- DOESN'T MAKE NETWORK (use StdEnv!)

  virtual void	StdNetwork(Network* net=NULL);
  // #MENU_BUTTON #MENU_ON_Network #NULL_OK make a standard network according to the current settings (if net == NULL, new network is created)

  /* 
  virtual void	StdConduit(NetConduit* cond=NULL, Network* net = NULL);
  // #MENU_BUTTON #MENU_ON_Environment #NULL_OK make a standard network conduit according to the current settings (if cond == NULL, new environment is created)
  virtual void	StdEnv(Environment* env=NULL, int n_events = 0);
  // #MENU_BUTTON #MENU_SEP_BEFORE #NULL_OK make a standard environment according to the current settings (if env == NULL, new environment is created)
  virtual void	UpdateEnvFmNet(Environment* env);
  // #MENU_BUTTON #MENU_SEP_BEFORE update environment (event specs controlling layout of events) based on current configuration of network (default network if multiple exist)
  virtual void	SequenceEvents(Environment* env, int n_seqs = 10, int events_per_seq = 4);
  // #MENU_BUTTON #MENU_SEP_BEFORE make sequences (groups) of events, for use with SequenceEpoch and SequenceProcess
  virtual void	TimeSeqEvents(TimeEnvironment* env, int n_seqs = 10, int events_per_seq = 4, float start_time = 0.0, float time_inc = 1.0);
  // #MENU_BUTTON make sequences (groups) of TimeEvents, with each sequence having the same sequence of times (start + inc for each addnl event)

  virtual void	StdProcs();
  // #MENU_BUTTON #MENU_ON_Processes #CONFIRM create a standard set of processes, starting with a batch process
  virtual void	NetAutoSave(SchedProcess* process_level_to_save_at, bool just_weights = false);
  // #MENU_BUTTON #MENU_SEP_BEFORE make the given process save a network when it is complted (just_weights = just save the weights)
  virtual EpochProcess*	AutoTestProc(SchedProcess* training_process, Environment* test_env);
  // #MENU_BUTTON create an automatic testing epoch process that runs on the test environment, and is called automatically from the training process
  virtual EpochProcess*	CrossValidation(SchedProcess* training_process, Environment* test_env);
  // #MENU_BUTTON create a cross-validation setup where training stops when testing on the test environment goes below a threshold
  virtual void	ToSequenceEvents(SchedProcess* process);
  // #MENU_BUTTON #MENU_SEP_BEFORE make the given process hierarchy work with event groups (sequences) (process can be any one in hierarchy)
  virtual void	NoSequenceEvents(SchedProcess* process);
  // #MENU_BUTTON get rid of sequence-level processing in given process hierarchy  (process can be any one in hierarchy)

  virtual MonitorStat* RecordLayerValues(SchedProcess* process_level_record_at, SchedProcess::StatLoc at_level, Layer* layer, const char* var = "act");
  // #MENU_BUTTON #MENU_ON_Stats record (e.g., in the log associated with given process) at the given level (loop or final) the given variable for the given layer (e.g., Trial FINAL = record at end of each trial)
  virtual CopyToEnvStat* SaveValuesInDataEnv(MonitorStat* stat);
  // #MENU_BUTTON save the values recorded by the given monitor statistic into a data environment (for viewing, clustering, distance computations, etc)
  virtual DispDataEnvProc* AutoAnalyzeDataEnv(Environment* data_env, int pattern_no, DispDataEnvProc::DispType analysis_disp, SchedProcess* process_level_analyze_at, SchedProcess::ProcLoc at_level);
  // #MENU_BUTTON automatically analyze (and display results) on given data environment and pattern number, at given processing level (e.g., Epoch FINAL = at end of each epoch)
  virtual DispDataEnvProc* AnalyzeNetLayer(SchedProcess* process_level_record_at, SchedProcess::StatLoc rec_at_level, Layer* layer, const char* var = "act",
		   DispDataEnvProc::DispType analysis_disp = DispDataEnvProc::CLUSTER_PLOT, SchedProcess* process_level_analyze_at = NULL,
		   SchedProcess::ProcLoc analyze_at_level = SchedProcess::FINAL_PROCS);
  // #MENU_BUTTON record data from the named layer, variable at given level (e.g., Trial FINAL) (RecordLayerValues), and send it to a data environment (SaveValuesInDataEnv), and then automatically analyze the data at given processing level (e.g., Epoch FINAL) (AutoAnalyzeDataEnv)
  virtual UnitActRFStat* ActBasedReceptiveField(SchedProcess* process_level_record_at, SchedProcess::StatLoc rec_at_level,
		Layer* recv_layer, Layer* send_layer, Layer* send2_layer,
		SchedProcess* process_level_disp_rfs_at, SchedProcess::ProcLoc disp_at_level = SchedProcess::FINAL_PROCS);
  // #MENU_BUTTON #NULL_OK record activation-based data from the named layer, variable at given level (e.g., Trial FINAL) (RecordLayerValues), and send it to a data environment (SaveValuesInDataEnv), and then automatically analyze the data at given processing level (e.g., Epoch FINAL) (AutoAnalyzeDataEnv)
  virtual DispNetWeightsProc* DisplayNetWeights(Layer* recv_layer, Layer* send_layer,
		SchedProcess* process_level_disp_wts_at, SchedProcess::ProcLoc disp_at_level = SchedProcess::FINAL_PROCS);
  // #MENU_BUTTON #NULL_OK automatically display network weight values from send_layer to recv_layer in given process and level (e.g., Epoch FINAL)
  virtual void	StopOnActThresh(SchedProcess* process_to_stop, Layer* layer, float thresh = .75);
  // #MENU_BUTTON #MENU_SEP_BEFORE make the given process stop (e.g., settle process) when activations in given layer get above given threshold
  virtual void	AddCountersToTest(SchedProcess* testing_process, SchedProcess* training_process);
  // #MENU_BUTTON add training process counters (e.g., epoch, batch) to the given testing process log output (via ProcCounterStat)
  virtual void	GetStatsFromProc(SchedProcess* proc_with_stats, SchedProcess* proc_to_get_stats, SchedProcess::StatLoc trg_stat_loc, Aggregate::Operator agg_op = Aggregate::LAST);
  // #MENU_BUTTON have proc_to_get_stats get all the stats (into its trg_stat_loc) from the proc_with_stats (using given aggregation operator)
  virtual TimeCounterStat* AddTimeCounter(SchedProcess* proc_where_time_incr, SchedProcess::StatLoc inc_at_level, SchedProcess* proc_reset_time);
  // #MENU_BUTTON add a time counter statistic to given process (useful as an X axis in graph plotting over longer time scales), resetting the time counter in given process

  // compare successive events (two mon stats with offsetting mon.off, compare stat, etc)

  virtual void	StdLogs(SchedProcess* process);
  // #MENU_BUTTON #MENU_ON_Logs #CONFIRM create standard logs (Trial TextLog, Epoch GraphLog, Batch TextLog) for given process hierarchy (process can be any proc in hierarchy)
  virtual void	LogProcess(SchedProcess* process, TypeDef* log_type);
  // #MENU_BUTTON #MENU_SEP_BEFORE #TYPE_PDPLog create log of a given type for given process
  // todo: various ways of configuring specific types of logs?? should probably be on logs themselves
  // what about making everything black & white vs. color?
*/
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase),&TA_Wizard); }
#endif
  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  SIMPLE_COPY(Wizard);
  COPY_FUNS(Wizard, taWizard);
  TA_BASEFUNS(Wizard);
};

class PDP_API ProjectBase : public taProject {
  // ##FILETYPE_Project ##EXT_proj ##COMPRESS #HIDDEN A ProjectBase has everything
INHERITED(taProject)
public:
  //note: this enum must be duplicated in pdpMisc
  enum ViewColors {		// indicies for view_colors
    TEXT,
    BACKGROUND,
    NETWORK,
    ENVIRONMENT, //note: also used for Events
    SCHED_PROC,
    STAT_GROUP,
    SUBPROC_GROUP,
    STAT_PROC,
    OTHER_PROC,
    PDPLOG,
    STAT_AGG,
    GEN_GROUP,
    INACTIVE,
    STOP_CRIT,
    AGG_STAT,
    CON_SPEC,
    UNIT_SPEC,
    PRJN_SPEC,
    LAYER_SPEC,
    WIZARD,
    COLOR_COUNT
  };

  Network_Group		networks;	// Networks of interconnected units
  DataTable_Group	data;		// Misc data, such as patterns for network input

  bool			save_rmv_units; // #DEF_true don't include units in network when saving (makes project file much smaller!)
  TAColor_List		the_colors; 	// #IGNORE actual colors
  RGBA_List		view_colors; 	// colors to use in the project view
  
  virtual void	UpdateColors();	// #BUTTON update the actual colors based on settings (
  virtual void	GetDefaultColors(); // #BUTTON get default colors for various project objects (in view and edit dialogs)
  
  override const iColor* GetObjColor(TypeDef* td); // #IGNORE get default color for object (for edit, project view)
  override const iColor* GetObjColor(int vc); // #IGNORE get default color for object (for edit, project view)
  override void 	AssertDefaultWiz(bool auto_opn); // make the default wizard(s)

  void	UpdateAfterEdit();
  void 	InitLinks_impl(); // special, for this class only
  void	CutLinks_impl(); // special, so we can do base class guys first (esp. viewers)
  void	Copy_(const ProjectBase& cp);
  COPY_FUNS(ProjectBase, taProject);
  TA_BASEFUNS(ProjectBase);
private:
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
};


class PDP_API PDPRoot : public taRootBase {
  // structural root of object hierarchy
INHERITED(taRootBase)
public:
  ColorScaleSpec_Group 	colorspecs;	// Color Specs

  override void  Settings();		// #MENU #MENU_ON_Object edit global settings/parameters (taMisc)
  override void	SaveConfig();		// #MENU #CONFIRM save current configuration to file ~/.pdpconfig that is automatically loaded at startup: IMPORTANT: DO NOT HAVE A PROJECT LOADED!
  override void	LoadConfig();		// #MENU #CONFIRM load current configuration from file ~/.pdpconfig that is automatically loaded at startup
  override void	Info();			// #MENU get information/copyright notice
#ifdef TA_GUI
  TAPtr	Browse(const char* init_path=NULL);
  // #MENU #ARGC_0 #USE_RVAL #NO_REVERT_AFTER use object browser to find an object, starting with initial path if given
#endif
  override void	SaveAll(); // saves all the projects

  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(PDPRoot);
private:
  void 	Initialize();
  void 	Destroy();
};


#endif // pdpshell_h
