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



// pdp_project.h

#ifndef pdp_project_h
#define pdp_project_h 1

#include "ta_fontspec.h"
#include "ta_project.h"
#include "ta_program.h"
#include "ta_imgproc.h"

#ifdef TA_GUI
# include "ta_seledit.h"
# include "ta_qtviewer.h"
#endif

#include "netstru.h"

//////////////////////////////////////////////////
//			Wizard			//
//////////////////////////////////////////////////

class PDP_API LayerWizEl : public taNBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Wizard specifies basic parameters for a layer
public:
  enum InputOutput {
    INPUT,
    HIDDEN,
    OUTPUT
  };

  int		n_units;	// number of units in the layer
  InputOutput 	io_type;	// is it an input, hidden, or output layer -- determines environment patterns

  override String 	GetDecorateKey() const { return "Wizard"; }

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

  virtual void	ThreeLayerNet();
  // #MENU_BUTTON #MENU_ON_Defaults set configuration to a standard three-layer network (input, hidden, output) -- DOESN'T MAKE NETWORK (use StdEnv!)
  virtual void	MultiLayerNet(int n_inputs = 1, int n_hiddens = 1, int n_outputs = 1);
  // #MENU_BUTTON set configuration for specified number of each type of layer -- DOESN'T MAKE NETWORK (use StdEnv!)

  virtual void	StdNetwork(Network* net=NULL);
  // #MENU_BUTTON #MENU_ON_Network #NULL_OK #NULL_TEXT_NewNetwork make a standard network according to the current settings (if net == NULL, new network is created)
  virtual void	RetinaSpecNetwork(RetinaSpec* retina_spec, Network* net=NULL);
  // #MENU_BUTTON #MENU_ON_Network #NULL_OK_1 #NULL_TEXT_1_NewNetwork configure the input layers of the network to accept the output of the image processing performed by retina_spec (if net == NULL, new network is created)

  virtual void	StdInputData(Network* net, DataTable* data_table=NULL, int n_patterns = 0, bool group=false);
  // #MENU_BUTTON #MENU_ON_Data #NULL_OK_1 #NULL_TEXT_1_NewDataTable make a standard data table of input patterns according to the given network (if data_table == NULL, new datatable is created), group = create a group column for grouping inputs into sequences 
  virtual void	UpdateInputDataFmNet(Network* net, DataTable* data_table);
  // #MENU_BUTTON #MENU_SEP_BEFORE update data table columns based on configuration of the network 
  virtual void	StdOutputData();
  // #MENU_BUTTON #MENU_SEP_BEFORE make standard set of output data (monitoring network performance) -- this just creates empty datatables in OutputData subgroup with names that standard programs look for

//   virtual void	TimeSeqEvents(TimeEnvironment* env, int n_seqs = 10, int events_per_seq = 4, float start_time = 0.0, float time_inc = 1.0);
//   // #MENU_BUTTON make sequences (groups) of TimeEvents, with each sequence having the same sequence of times (start + inc for each addnl event)

  virtual void	StdProgs();
  // #MENU_BUTTON #MENU_ON_Programs #CONFIRM create a standard set of programs for running the algorithm specified by this project

  virtual Program_Group* StdProgs_impl(const String& prog_nm); // #IGNORE impl that actually loads progs from proglib with given name; returns new program group or NULL if not found

  /*  virtual void	NetAutoSave(SchedProcess* process_level_to_save_at, bool just_weights = false);
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
  // ##FILETYPE_Project ##EXT_proj ##COMPRESS #VIRT_BASE Base class for a pdp project (do not create one of these -- create an algorithm-specific version)
INHERITED(taProject)
public:
  Network_Group		networks;	// Networks of interconnected units

  DataTable_Group*	analysisDataGroup();
  // returns default group used for auto-created analysis data
  override void 	AssertDefaultWiz(bool auto_opn);
  // make the default wizard(s)

  virtual void	AutoBuildNets();
  // build networks according to their own individual auto_bulid modes
  
  void	UpdateAfterEdit();
  void 	InitLinks_impl(); // special, for this class only
  void	CutLinks_impl(); // special, so we can do base class guys first (esp. viewers)
  void	Copy_(const ProjectBase& cp);
  COPY_FUNS(ProjectBase, taProject);
  TA_BASEFUNS(ProjectBase);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
};


class PDP_API PDPRoot : public taRootBase {
  // structural root of object hierarchy
INHERITED(taRootBase)
public:

  override void	About();
#ifdef TA_GUI
  TAPtr	Browse(const char* init_path=NULL);
  // #MENU #ARGC_0 #USE_RVAL #NO_REVERT_AFTER use object browser to find an object, starting with initial path if given
#endif

  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(PDPRoot);
private:
  void 	Initialize();
  void 	Destroy();
};


#endif // pdp_project_h
