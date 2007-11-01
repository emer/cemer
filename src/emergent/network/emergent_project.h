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

class EMERGENT_API LayerWizEl : public taNBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Wizard specifies basic parameters for a layer
INHERITED(taNBase)
public:
  enum InputOutput {
    INPUT,
    HIDDEN,
    OUTPUT
  };

  int		n_units;	// number of units in the layer
  InputOutput 	io_type;	// is it an input, hidden, or output layer -- determines environment patterns

  override String 	GetDecorateKey() const { return "Wizard"; }

  SIMPLE_COPY(LayerWizEl);
  TA_BASEFUNS(LayerWizEl);
private:
  void	Initialize();
  void	Destroy() 	{ };
};

class EMERGENT_API LayerWizElList : public taList<LayerWizEl> {
  // ##CAT_Wizard a list of layer wiz elements
INHERITED(taList<LayerWizEl>)
public:
  TA_BASEFUNS_NOCOPY(LayerWizElList);
private:
  void	Initialize() 		{ SetBaseType(&TA_LayerWizEl); }
  void 	Destroy()		{ };
};

class EMERGENT_API Wizard : public taWizard {
  // ##BUTROWS_2 ##EDIT_WIDTH_60 wizard for automating construction of simulation objects
INHERITED(taWizard)
public:
  enum Connectivity {
    FEEDFORWARD,		// each layer projects to the next one in sequence
    BIDIRECTIONAL		// layers are bidirectionally connected in sequence (each sends and receives from its neighbors)
  };

  int		n_layers;	// number of layers
  LayerWizElList layer_cfg;	// provides configuration information for each layer
  Connectivity	connectivity;	// how to connect the layers

  virtual bool	ThreeLayerNet();
  // #MENU_BUTTON #MENU_ON_Defaults set configuration to a standard three-layer network (input, hidden, output) -- DOESN'T MAKE NETWORK (use StdEnv!)
  virtual bool	MultiLayerNet(int n_inputs = 1, int n_hiddens = 1, int n_outputs = 1);
  // #MENU_BUTTON set configuration for specified number of each type of layer -- DOESN'T MAKE NETWORK (use StdEnv!)

  virtual bool	StdNetwork(Network* net=NULL);
  // #MENU_BUTTON #MENU_ON_Network #NULL_OK #NULL_TEXT_NewNetwork make a standard network according to the current settings (if net == NULL, new network is created)
  virtual bool	RetinaSpecNetwork(RetinaSpec* retina_spec, Network* net=NULL);
  // #MENU_BUTTON #NULL_OK_1 #NULL_TEXT_1_NewNetwork configure the input layers of the network to accept the output of the image processing performed by retina_spec (if net == NULL, new network is created)

  virtual bool	StdData(Network* net, DataTable* data_table=NULL, int n_patterns = 0, bool group=false);
  // #MENU_BUTTON #MENU_ON_Data #NULL_OK_1 #NULL_TEXT_1_NewDataTable make standard input and output data tables: make a standard data table of input patterns according to the given network (if data_table == NULL, new datatable is created), group = create a group column for grouping inputs into sequences.  also make standard output data to monitor network output
  virtual bool	StdInputData(Network* net, DataTable* data_table=NULL, int n_patterns = 0, bool group=false);
  // #MENU_BUTTON #MENU_SEP_BEFORE #NULL_OK_1 #NULL_TEXT_1_NewDataTable make a standard data table of input patterns according to the given network (if data_table == NULL, new datatable is created), group = create a group column for grouping inputs into sequences.  also calls StdOutputData to create monitor output data
  virtual bool	StdOutputData();
  // #MENU_BUTTON make standard set of output data (monitoring network performance) -- this just creates empty datatables in OutputData subgroup with names that standard programs look for
  virtual bool	UpdateInputDataFmNet(Network* net, DataTable* data_table);
  // #MENU_BUTTON #MENU_SEP_BEFORE update data table columns based on configuration of the network 

//   virtual void	TimeSeqEvents(TimeEnvironment* env, int n_seqs = 10, int events_per_seq = 4, float start_time = 0.0, float time_inc = 1.0);
//   // #MENU_BUTTON make sequences (groups) of TimeEvents, with each sequence having the same sequence of times (start + inc for each addnl event)

  virtual bool	StdProgs();
  // #MENU_BUTTON #MENU_ON_Programs #CONFIRM create a standard set of programs for running the algorithm specified by this project

  virtual Program_Group* StdProgs_impl(const String& prog_nm); // #IGNORE impl that actually loads progs from proglib with given name; returns new program group or NULL if not found

  void	UpdateAfterEdit();
  void 	InitLinks();
  void	CutLinks();
  SIMPLE_COPY(Wizard);
  TA_BASEFUNS(Wizard);
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

  override void 	AssertDefaultWiz(bool auto_opn);
  // make the default wizard(s)

  override void		SaveRecoverFile_strm(ostream& strm);

  virtual void	AutoBuildNets();
  // build networks according to their own individual auto_bulid modes
  
  override void	Dump_Load_post();
  
  void	UpdateAfterEdit();
  void 	InitLinks_impl(); // special, for this class only
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
  TAPtr	Browse(const char* init_path=NULL);
  // #MENU #ARGC_0 #USE_RVAL #NO_REVERT_AFTER use object browser to find an object, starting with initial path if given
#endif

  TA_BASEFUNS_NOCOPY(EmergentRoot);
private:
  void 	Initialize();
  void 	Destroy() { };
};


#endif // emergent_project_h
