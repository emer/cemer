// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef Wizard_h
#define Wizard_h 1

// parent includes:
#include "network_def.h"
#include <taWizard>

#include <Network>

// member includes:

// declare all other types mentioned but not required to include:
class DataTable; //
class Program; //
class Program_Group; //
class StdNetWizDlg; //


eTypeDef_Of(Wizard);

class E_API Wizard : public taWizard {
  // ##BUTROWS_2 ##EDIT_WIDTH_60 wizard for automating construction of simulation objects
INHERITED(taWizard)
public:
  StdNetWizDlg*         std_net_dlg;   // #HIDDEN #NO_SAVE std network dialog
  bool            std_data_ok;   // #HIDDEN #NO_SAVE CallFun doesn't return a value so check this value
  NetworkRef      network;       // #HIDDEN #NO_SAVE need this for StdProgs_impl

  virtual bool  StdEverything();
  // #MENU_BUTTON #MENU_ON_Network #MENU_SEP_AFTER make everything (Network, Data, Programs) according to the standard wizards

  virtual bool  StdNetwork();
  // #MENU_BUTTON #MENU_ON_Network configure a new or existing standard network -- user is prompted for full configuration settings; if we will need the NetworkRef don't call CutLinks at end - caller will be responsible!

  virtual bool  StdData(Network* net, DataTable* data_table=NULL, int n_patterns = 0, bool group=false);
  // #MENU_BUTTON #MENU_ON_Data #NULL_OK_1 #NULL_TEXT_1_NewDataTable make standard input and output data tables: make a standard data table of input patterns according to the given network (if data_table == NULL, new datatable is created), group = create a group column for grouping inputs into sequences. also calls StdOutputData to create monitor output data, and UpdateLayerWriters to update any LayerWriter objects (typically in ApplyInputs programs) to the new data layout
  virtual bool  StdInputData(Network* net, DataTable* data_table=NULL, int n_patterns = 0, bool group=false);
  // #MENU_BUTTON #MENU_SEP_BEFORE #NULL_OK_1 #NULL_TEXT_1_NewDataTable make a standard data table of input patterns according to the given network (if data_table == NULL, new datatable is created), group = create a group column for grouping inputs into sequences.
  virtual bool  StdOutputData();
  // #MENU_BUTTON make standard set of output data (monitoring network performance) -- this just creates empty datatables in OutputData subgroup with names that standard programs look for
  virtual bool  UpdateInputDataFmNet(Network* net, DataTable* data_table);
  // #MENU_BUTTON #MENU_SEP_BEFORE update data table columns based on configuration of the network -- also calls UpdateLayerWriters to update ApplyInputs programs to work with any new changes to the input data
  virtual bool  UpdateLayerWriters(Network* net, DataTable* data_table);
  // #MENU_BUTTON update LayerWriter configuration in ApplyInputs programs to fit any changes in the network or data table -- only affects LayerWriters that are already configured to use the given network and data table

//   virtual void       TimeSeqEvents(TimeEnvironment* env, int n_seqs = 10, int events_per_seq = 4, float start_time = 0.0, float time_inc = 1.0);
//   // #MENU_BUTTON make sequences (groups) of TimeEvents, with each sequence having the same sequence of times (start + inc for each addnl event)

  virtual bool  StdProgs();
  // #MENU_BUTTON #MENU_ON_Programs #CONFIRM create a standard set of programs for running the algorithm specified by this project
  virtual bool  TestProgs(Program* call_test_from, bool call_in_loop=true, int call_modulus=1);
  // #MENU_BUTTON #MENU_SEP_BEFORE #CONFIRM #NULL_OK_0 create a standard set of testing programs for testing the network -- the call_test_from argument specifies a training program to call the testing program from (NULL for none), and call_in_loop & call_modulus specify whether to call in the loop of that program (else at the end), and how frequently to call it (modulus -- in terms of network.epoch by default -- can change to loop counter or anything else, and modulus is hard-coded -- could make it a variable instead).  Testing versions of std OutputData tables are made -- test programs use 3rd & 4th tables in data.gp.OuputData for output by default, so check that!

  virtual Program_Group* StdProgs_impl(const String& prog_nm);
  // #IGNORE impl that actually loads progs from proglib with given name; returns new program group or NULL if not found
  virtual Program_Group* TestProgs_impl(const String& prog_nm, Program* call_test_from,
                                        bool call_in_loop=true, int call_modulus=1);
  // #IGNORE impl that actually loads progs from proglib with given name; returns new program group or NULL if not found
  virtual bool  FixOldProgs();
  // #MENU_BUTTON #MENU_ON_Programs #MENU_SEP_BEFORE #CONFIRM fix programs from versions earlier than 5.0.2 to have default NO_STOP_STEP flags and short names..

  void RenderWizDoc() override;

  SIMPLE_INITLINKS(Wizard);
  void  CutLinks() override;
  SIMPLE_COPY(Wizard);
  TA_BASEFUNS(Wizard);
protected:
  void RenderWizDoc_impl() override;
  virtual String RenderWizDoc_intro(); // introductory code
  virtual String RenderWizDoc_network(); // network-level functions
  virtual String RenderWizDoc_data(); // datatable functions
  virtual String RenderWizDoc_program(); // program functions

private:
  void  Initialize();
  void  Destroy();
};

#endif // Wizard_h
