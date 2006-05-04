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

#include "ta_defaults.h"
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


class PDP_API TypeDefault_MGroup : public taGroup<TypeDefault> {
  // #DEF_PATH_$PDPDIR$/defaults group of type default objects
INHERITED(taGroup<TypeDefault>)
public:
  int	Dump_Load_Value(istream& strm, TAPtr par=NULL);
  // reset members before loading..

  void	Initialize() 		{ SetBaseType(&TA_TypeDefault); }
  void 	Destroy()		{ };
  TA_BASEFUNS(TypeDefault_MGroup);
};
#ifdef TA_GUI
// note: _MGroup name is for compatiblity with v3.2 files
class PDP_API SelectEdit_MGroup : public taGroup<SelectEdit> {
  // group of select edit dialog objects
public:
  virtual void	AutoEdit();

  void	Initialize() 		{ SetBaseType(&TA_SelectEdit); }
  void 	Destroy()		{ };
  TA_BASEFUNS(SelectEdit_MGroup);
};
#endif
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

class PDP_API Wizard : public taNBase {
  // ##BUTROWS_2 ##EDIT_WIDTH_60 wizard for automating construction of simulation objects
public:
  enum Connectivity {
    FEEDFORWARD,		// each layer projects to the next one in sequence
    BIDIRECTIONAL		// layers are bidirectionally connected in sequence (each sends and receives from its neighbors)
  };

  bool		auto_open;	// open this wizard dialog upon startup
  int		n_layers;	// number of layers
  taBase_List	layer_cfg;	// provides configuration information for each layer
  Connectivity	connectivity;	// how to connect the layers
  TypeDef*	event_type;	// #TYPE_Event type of event to create by default
/*TEMP
  virtual void	ThreeLayerNet();
  // #MENU_BUTTON #MENU_ON_Defaults set configuration to a standard three-layer network (input, hidden, output) -- DOESN'T MAKE NETWORK (use StdEnv!)
  virtual void	MultiLayerNet(int n_inputs = 1, int n_hiddens = 1, int n_outputs = 1);
  // #MENU_BUTTON set configuration for specified number of each type of layer -- DOESN'T MAKE NETWORK (use StdEnv!)

  virtual void	StdNetwork(Network* net=NULL);
  // #MENU_BUTTON #MENU_ON_Network #NULL_OK make a standard network according to the current settings (if net == NULL, new network is created)

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
  COPY_FUNS(Wizard, taNBase);
  TA_BASEFUNS(Wizard);
};

// note: _MGroup name is for compatiblity with v3.2 files
class PDP_API Wizard_MGroup : public taGroup<Wizard> {
  // group of wizard objects
public:
  virtual void	AutoEdit();

  void	Initialize() 		{ SetBaseType(&TA_Wizard); }
  void 	Destroy()		{ };
  TA_BASEFUNS(Wizard_MGroup);
};


class PDP_API ProjectBase : public taFBase {
  // ##EXT_proj ##COMPRESS #HIDDEN A ProjectBase has everything
INHERITED(taFBase)
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

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


  TypeDefault_MGroup	defaults;	// #NO_FIND #NO_SAVE default initial settings for objects
  Wizard_MGroup    	wizards;	// Wizards for automatically configuring simulation objects
  BaseSpec_MGroup     	specs;		// Specifications for network parameters
  Network_MGroup	networks;	// Networks of interconnected units
  DataTable_MGroup	data;		// Misc data, such as patterns for network input
  PDPLog_MGroup		logs;		// Logs to display statistics in processes
  Program_MGroup	programs;	// Gui-based programs to run simulations and other processing
  Script_MGroup		scripts;	// Scripts to control arbitrary actions
#ifdef TA_GUI
  SelectEdit_MGroup	edits;		// special edit dialogs for selected elements
  DataViewer_List	viewers;	// any open viewers TODO: make HIDDEN in release version
#ifdef DEBUG
  taBase_List		test_objs;	// just for testing, for any kind of objs
#endif
#endif
  bool			save_rmv_units; // remove units from network before saving (makes project file much smaller!)
  bool			use_sim_log; 	// record project changes in the SimLog file
  String		prev_file_nm; 	// #READ_ONLY #SHOW previous file name for this project
  String		desc1;		// description of the project
  String		desc2;
  String		desc3;
  String		desc4;

  // parameters for controlling the view
//TODO  ProjEditor*		editor;		// #IGNORE controls display of project information

  TAColor_List		the_colors; 	// #IGNORE actual colors
  RGBA_List		view_colors; 	// colors to use in the project view
  bool			mnu_updating; 	// #READ_ONLY #NO_SAVE if menu is already being updated (don't init display)
  bool			deleting; 	// #READ_ONLY #NO_SAVE if object is currently being deleted
  String		defaults_str;
  // #READ_ONLY #NO_SAVE string representation of basic defaults for the subclass; set in constructor
  String		defaults_file; 	// #READ_ONLY #NO_SAVE default name of defaults file, typically like "bp.def"

  virtual void	LoadDefaults();
  // load defaults according to root::default_file or precompiled defaults

  virtual const iColor* GetObjColor(TypeDef* td); // #IGNORE get default color for object (for edit, project view)
  virtual const iColor* GetObjColor(ViewColors vc); // #IGNORE get default color for object (for edit, project view)
  virtual void	UpdateColors();	// #BUTTON update the actual colors based on settings (
  virtual void	GetDefaultColors(); // #BUTTON get default colors for various project objects (in view and edit dialogs)
  override bool SetFileName(const String& val);

  // wizard construction functions:
  virtual void MakeDefaultWiz(bool auto_opn); // make the default wizard(s)
  virtual BaseSpec_MGroup* FindMakeSpecGp(const char* nm, bool& nw_itm = nw_itm_def_arg); // find a given spec group and if not found, make it

  int	Load(istream& strm, TAPtr par=NULL);
  int	Save(ostream& strm, TAPtr par=NULL, int indent=0);
  int 	SaveAs(ostream& strm, TAPtr par=NULL, int indent=0);

#ifdef TA_GUI
  virtual pdpDataViewer* NewViewer(); // create a new, empty viewer -- note: window not opened yet
  virtual void	OpenNetworkViewer(Network* net = NULL);
  // #MENU open a viewer on this indicated network
  virtual void	UpdateSimLog();
  // #MENU update simulation log (SimLog) for this project, storing the name of the project and the description as entered here.  click off use_simlog if you are not using this feature

#endif
  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const ProjectBase& cp);
  COPY_FUNS(ProjectBase, taFBase);
  TA_BASEFUNS(ProjectBase);
};


// note: _MGroup name is for compatiblity with v3.2 files
class PDP_API Project_MGroup : public taGroup<ProjectBase> {
public:
  ColorScaleSpec_MGroup*	colorspecs;	// #HIDDEN #NO_SAVE -- aliased from projects

  int		Load(istream& strm, TAPtr par=NULL); // call reconnect on nets afterwards

  void	Initialize() 		{SetBaseType(&TA_ProjectBase);}
  void 	Destroy()		{ };
  TA_BASEFUNS(Project_MGroup);
};


class PDP_API PDPRoot : public taNBase, IApp {
  // structural root of object hierarchy
public:
  String		version_no; 	// #READ_ONLY #SHOW current version number
  Project_MGroup	projects; 	// The projects
  ColorScaleSpec_MGroup colorspecs;	// Color Specs -- aliased in projects (for browser)

//nn  bool	ThisMenuFilter(MethodDef* md); // don't include saving and loading..
//obs  void	SetWinName();
//obs?  void	GetWindow();		// make an app-window
  override void  Settings();		// #MENU #MENU_ON_Object edit global settings/parameters (taMisc)
  override void	SaveConfig();		// #MENU #CONFIRM save current configuration to file ~/.pdpconfig that is automatically loaded at startup: IMPORTANT: DO NOT HAVE A PROJECT LOADED!
  override void	LoadConfig();		// #MENU #CONFIRM load current configuration from file ~/.pdpconfig that is automatically loaded at startup
  override void	Info();			// #MENU get information/copyright notice
#ifdef TA_GUI
  TAPtr	Browse(const char* init_path=NULL);
  // #MENU #ARGC_0 #USE_RVAL #NO_REVERT_AFTER use object browser to find an object, starting with initial path if given
#endif
  override void	Quit();
  // #MENU #CONFIRM #MENU_SEP_BEFORE #NO_REVERT_AFTER quit from software..
  override void	SaveAll(); // saves all the projects
  // following use project to store position info!
//obs  void	SetWinPos(float left=-1.0f, float bottom=-1.0f, float width=-1.0f, float height=-1.0f);
//obs  void	GetWinPos();

  void	UpdateAfterEdit(); // keep projects alias fields in sync
  void 	Initialize();
  void	InitLinks();
  void	CutLinks();
  void 	Destroy();
  TA_BASEFUNS(PDPRoot);

/*obs protected:
  override void WindowClosing(bool& cancel); */
};

#ifdef DEBUG
#include "ta_variant.h"
// just a temp test object
class PDP_API TestObj: public taNBase {
INHERITED(taNBase)
public:
  bool			b; // #DEF_false
  char			c;
  signed char		sc;
  unsigned char		uc;
  byte			byt; //note: s/b same as an unsigned char
  short			sh;
  signed short		ssh;
  unsigned short	ush;
  int			i; // #DEF_0
  signed int		si;
  signed		s;
  unsigned int		ui;
  unsigned		u;
  long			l;
  signed long		sl;
  unsigned long		ul;
  int64_t		i64;
  long long		ll;
  signed long long	sll;
  uint64_t		u64;
  unsigned long long	ull;
  intptr_t		intptr;
  
  Variant		v; // used for dynamic testing in the object
  Variant		v_invalid; 
  Variant		v_b;
  Variant		v_c;
  Variant		v_i;
  Variant		v_i_ro; // #READ_ONLY #SHOW #NO_SAVE
  Variant		v_ui;
  Variant		v_i64;
  Variant		v_u64;
  Variant		v_d;
  Variant		v_str;
  void*			s_ptr;
  String		typ_s_ptr; // #READ_ONLY #NO_SAVE #SHOW
  Variant		v_ptr; 
  String		typ_v_ptr; // #READ_ONLY #NO_SAVE #SHOW
  Variant		v_tab; 
  taBase*		s_tab;
  String		typ_s_tab; // #READ_ONLY #NO_SAVE #SHOW
  String		typ_v_tab; // #READ_ONLY #NO_SAVE #SHOW
  taMatrix*		s_mat;
  String		typ_s_mat; // #READ_ONLY #NO_SAVE #SHOW
  Variant		v_mat; 
  String		typ_v_mat; // #READ_ONLY #NO_SAVE #SHOW
  taBase*		s_own_tab; // #OWN_POINTER an object we create and own ourself
  Variant		v_own_tab; // #OWN_POINTER an object we create and own ourself, in a variant
  
  void			InitObj(); // #MENU #MENU_CONTEXT set the values to non-clear values
  
  virtual bool		TestMethod1(
    const Variant&	v, 
    int64_t		i64 = -5,
    long long		ll = -6,
    signed long long	sll = -7,
    uint64_t		u64 = 8,
    unsigned long long	ull = 9,
    bool		b = false, 
    char		c = 65, 
    signed char		sc = -127,
    unsigned char	uc = 255,
    byte		byt = 254, 
    short		sh = -32767,
    signed short	ssh = -32766,
    unsigned short	ush = 65535,
    int			i = 2147483647, 
    signed int		si = 2147483646,
    signed		s = 2147483645,
    unsigned int	ui = 2,
    unsigned		u = 3,
    long		l = 2,
    signed long		sl = 3,
    unsigned long	ul = 4,
    taBase*		s_tab = NULL
  ); // #MENU #MENU_CONTEXT a method used to test the various argument types; returns b
  
  virtual bool		TestMethod_ProblemDefs(
    char		c = 'a',
    unsigned int	ui = 4294967295U,
    unsigned		u = 4294967294U,
    long		l = 2147483647L,
    signed long		sl = 2147483646L,
    unsigned long	ul = 4294967295UL,
    int64_t		i64 = 9223372036854775807LL,
    long long		ll = 9223372036854775806LL,
    signed long long	sll = 9223372036854775805LL,
    uint64_t		u64 = 18446744073709551615ULL,
    unsigned long long	ull = 18446744073709551614ULL,
    const Variant&	v = _nilVariant, 
    taBase*		s_tab = NULL
  ); // #MENU #MENU_CONTEXT a method used to test the various problematic default argument types; returns b
  
  void	InitLinks();
  void	CutLinks();
  void	UpdateAfterEdit();
  SIMPLE_COPY(TestObj)
  COPY_FUNS(TestObj, taNBase)
  TA_BASEFUNS(TestObj);
protected:
  void			UpdateTypeDefVars();
private:
  void 			Initialize();
  void			Destroy() {CutLinks();}
};

class PDP_API TestObj2: public taNBase {
INHERITED(taNBase)
public:
  int			int_val;
  TestObj
  		test_obj;
  void	InitLinks();
  void	CutLinks();
  void	UpdateAfterEdit();
  void	Copy_(const TestObj2& cp);
  COPY_FUNS(TestObj2, taNBase)
  TA_BASEFUNS(TestObj2);
private:
  void 			Initialize();
  void			Destroy() {CutLinks();}
};

class PDP_API TestObj3: public taNBase {
INHERITED(taNBase)
public:
  int			i;
  taBase*		s_own_tab; // #OWN_POINTER #IGNORE
  float			f;
  
  void	InitLinks();
  void	CutLinks();
  void	UpdateAfterEdit();
  void	Copy_(const TestObj3& cp);
  COPY_FUNS(TestObj3, taNBase)
  TA_BASEFUNS(TestObj3);
private:
  void 			Initialize();
  void			Destroy() {CutLinks();}
};

class FloatTransform;
class PDP_API TestOwnObj: public taNBase {
INHERITED(taNBase)
public:
  FloatTransform	ft_inst;
  FloatTransform*	ft_own;
  
  void	InitLinks();
  void	CutLinks();
  void	UpdateAfterEdit();
  void	Copy_(const TestOwnObj& cp);
  COPY_FUNS(TestOwnObj, taNBase)
  TA_BASEFUNS(TestOwnObj);
private:
  void 			Initialize();
  void			Destroy() {CutLinks();}
};

class DataTable;
class PDP_API NetHelper: public taNBase {
INHERITED(taNBase)
public:
  
  void			InitXorNetEnviroTable(DataTable* dt); // #MENU #MENU_CONTEXT initialize table for Xor data pattern
  
  TA_BASEFUNS(NetHelper);
private:
  void 			Initialize() {}
  void			Destroy() {}
};


#endif


#endif // pdpshell_h
