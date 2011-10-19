// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
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

// emergent_project.h

#ifndef emergent_project_h
#define emergent_project_h 1

#include "ta_project.h"
#include "netstru.h"

class taGuiDialog;
class Program;
class Program_Group; //
class RetinaProc; //

//////////////////////////////////////////////////
//			Wizard			//
//////////////////////////////////////////////////

class EMERGENT_API StdNetWizDlg : public taNBase {
  // #CAT_Wizard dialog for StdNetwork() wizard action
INHERITED(taNBase)
public:
  taGuiDialog* 	Dlg1;  
  taGuiDialog*	Dlg2;  
  NetworkRef 	network;  
  int 		n_layers;  
  DataTable*	net_config;	// network configuration data table
 
  void 	NewNetwork();
  // #MENU #MENU_ON_Actions callback for dialog
  void 	NLayersFmNetwork();
  // #MENU callback for dialog
  void 	ConfigOneLayer(int lay_no, const String& nm, const String& typ);
  // #MENU callback for dialog
  void 	NewNetDefaultConfig();
  // #MENU callback for dialog
  void 	AddNewLayerRow();
  // #MENU callback for dialog
  void 	RefreshLayerList();
  // #MENU callback for dialog

  virtual bool	DoDialog();
  // do the dialog and configure the network based on results -- returns true if network configured, false if not

  TA_SIMPLE_BASEFUNS(StdNetWizDlg);
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};
  
class EMERGENT_API Wizard : public taWizard {
  // ##BUTROWS_2 ##EDIT_WIDTH_60 wizard for automating construction of simulation objects
INHERITED(taWizard)
public:
  StdNetWizDlg		std_net_dlg; // #HIDDEN #NO_SAVE std network dialog

  virtual bool	StdEverything();
  // #MENU_BUTTON #MENU_ON_Network #MENU_SEP_AFTER make everything (Network, Data, Programs) according to the standard wizards

  virtual bool	StdNetwork();
  // #MENU_BUTTON #MENU_ON_Network configure a new or existing standard network -- user is prompted for full configuration settings

  virtual bool	RetinaProcNetwork(RetinaProc* retina_proc, Network* net=NULL);
  // #MENU_BUTTON #NULL_OK_1 #NULL_TEXT_1_NewNetwork configure the input layers of the network to accept the output of the image processing performed by retina_proc (if net == NULL, new network is created)

  virtual bool	StdData(Network* net, DataTable* data_table=NULL, int n_patterns = 0, bool group=false);
  // #MENU_BUTTON #MENU_ON_Data #NULL_OK_1 #NULL_TEXT_1_NewDataTable make standard input and output data tables: make a standard data table of input patterns according to the given network (if data_table == NULL, new datatable is created), group = create a group column for grouping inputs into sequences. also calls StdOutputData to create monitor output data, and UpdateLayerWriters to update any LayerWriter objects (typically in ApplyInputs programs) to the new data layout
  virtual bool	StdInputData(Network* net, DataTable* data_table=NULL, int n_patterns = 0, bool group=false);
  // #MENU_BUTTON #MENU_SEP_BEFORE #NULL_OK_1 #NULL_TEXT_1_NewDataTable make a standard data table of input patterns according to the given network (if data_table == NULL, new datatable is created), group = create a group column for grouping inputs into sequences.
  virtual bool	StdOutputData();
  // #MENU_BUTTON make standard set of output data (monitoring network performance) -- this just creates empty datatables in OutputData subgroup with names that standard programs look for
  virtual bool	UpdateInputDataFmNet(Network* net, DataTable* data_table);
  // #MENU_BUTTON #MENU_SEP_BEFORE update data table columns based on configuration of the network -- also calls UpdateLayerWriters to update ApplyInputs programs to work with any new changes to the input data
  virtual bool	UpdateLayerWriters(Network* net, DataTable* data_table);
  // #MENU_BUTTON update LayerWriter configuration in ApplyInputs programs to fit any changes in the network or data table -- only affects LayerWriters that are already configured to use the given network and data table

//   virtual void	TimeSeqEvents(TimeEnvironment* env, int n_seqs = 10, int events_per_seq = 4, float start_time = 0.0, float time_inc = 1.0);
//   // #MENU_BUTTON make sequences (groups) of TimeEvents, with each sequence having the same sequence of times (start + inc for each addnl event)

  virtual bool	StdProgs();
  // #MENU_BUTTON #MENU_ON_Programs #CONFIRM create a standard set of programs for running the algorithm specified by this project
  virtual bool	TestProgs(Program* call_test_from, bool call_in_loop=true, int call_modulus=1);
  // #MENU_BUTTON #MENU_SEP_BEFORE #CONFIRM #NULL_OK_0 create a standard set of testing programs for testing the network -- the call_test_from argument specifies a training program to call the testing program from (NULL for none), and call_in_loop & call_modulus specify whether to call in the loop of that program (else at the end), and how frequently to call it (modulus -- in terms of network.epoch by default -- can change to loop counter or anything else, and modulus is hard-coded -- could make it a variable instead).  Testing versions of std OutputData tables are made -- test programs use 3rd & 4th tables in data.gp.OuputData for output by default, so check that!

  virtual Program_Group* StdProgs_impl(const String& prog_nm);
  // #IGNORE impl that actually loads progs from proglib with given name; returns new program group or NULL if not found
  virtual Program_Group* TestProgs_impl(const String& prog_nm, Program* call_test_from,
					bool call_in_loop=true, int call_modulus=1);
  // #IGNORE impl that actually loads progs from proglib with given name; returns new program group or NULL if not found
  virtual bool	FixOldProgs();
  // #MENU_BUTTON #MENU_ON_Programs #MENU_SEP_BEFORE #CONFIRM fix programs from versions earlier than 5.0.2 to have default NO_STOP_STEP flags and short names..

  override void RenderWizDoc();

  void	UpdateAfterEdit();
  TA_SIMPLE_BASEFUNS(Wizard);
protected:
  override void RenderWizDoc_impl();
  virtual String RenderWizDoc_intro(); // introductory code
  virtual String RenderWizDoc_network(); // network-level functions
  virtual String RenderWizDoc_data(); // datatable functions
  virtual String RenderWizDoc_program(); // program functions

private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};

class EMERGENT_API ProjectBase : public taProject {
  // ##FILETYPE_Project ##EXT_proj ##COMPRESS #VIRT_BASE Base class for a pdp project (do not create one of these -- create an algorithm-specific version)
INHERITED(taProject)
public:
  Network_Group		networks;	// Networks of interconnected units

  virtual Network* 	GetNewNetwork(TypeDef* typ = NULL);
  // get a new network object
  virtual Network* 	GetDefNetwork();
  // get default network from project

  override void		SaveRecoverFile_strm(ostream& strm);

  virtual void	AutoBuildNets();
  // build networks according to their own individual auto_bulid modes
  
  override void	Dump_Load_post();

  void	UpdateAfterEdit();
  void 	InitLinks_impl(); // special, for this class only
  void 	InitLinks_post(); // special, for this class only
  void	CutLinks_impl(); // special, so we can do base class guys first (esp. viewers)
  void	Copy_(const ProjectBase& cp);
  TA_BASEFUNS(ProjectBase);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
};

class EMERGENT_API EmergentRoot : public taRootBase {
  // structural root of object hierarchy
INHERITED(taRootBase)
public:

  override void	About();
#ifdef TA_GUI
  taBase*	Browse(const char* init_path=NULL);
  // #MENU #ARGC_0 #USE_RVAL #NO_REVERT_AFTER use object browser to find an object, starting with initial path if given
#endif

  TA_BASEFUNS_NOCOPY(EmergentRoot);
private:
  void 	Initialize();
  void 	Destroy() { };
};


#endif // emergent_project_h
