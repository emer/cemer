// Copyright 2013-2017, Regents of the University of Colorado,
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

#ifndef LayerDataEl_h
#define LayerDataEl_h 1

// parent includes:
#include "network_def.h"
#include <taOBase>

// member includes:
#include <NetworkRef>
#include <DataTableRef>
#include <PosVector2i>

// declare all other types mentioned but not required to include:
class DataTableCols; //
class DataCol; //

// This is a workaround for a problem that can occur depending on the order of
// includes.  The problem is that the windows header file "Nb30.h" contains
// "#define GROUP_NAME 0x80".  If that header gets #included before this one,
// you get all sorts of cryptic compiler errors, since a literal 0x80 appears
// in the NetTarget enum definition.  This #undef is relatively safe, since
// the "Nb30.h" header is related to NetBIOS stuff which Emergent doesn't use.
//
// First make sure the GROUP_NAME macro has been defined, then undefine it.
// It's necessary to ensure it has been defined first, otherwise if some .cpp
// file #includes this file, then "t3viewer.h", it would end up with the macro
// defined, and would fail to compile if it used LayerDataEl::GROUP_NAME.
// The include chain, starting at t3viewer.h, is:
//   t3viewer.h -> QuarterWidget.h -> QGLWidget -> qgl.h -> qt_windows.h
//     -> windows.h -> nb30.h
#ifdef GROUP_NAME
  #pragma message("Warning: undefining GROUP_NAME macro")
  #undef GROUP_NAME
#endif


eTypeDef_Of(LayerDataEl);

class E_API LayerDataEl: public taOBase  {
  // #VIRT_BASE #NO_INSTANCE #NO_TOKENS ##CAT_Network controls the reading/writing of information to/from layers and datatables
friend class LayerDataEl_List;
friend class LayerWriter_List;
INHERITED(taOBase)
public:
  enum NetTarget {
    LAYER,			// read/write the layer information
    TRIAL_NAME,			// read/write the network trial_name field
    GROUP_NAME,			// read/write the network group_name field
  };

  bool            off;
  // #DEF_false turn this layer writer element off (for experimentation, updating, dynamic changes -- prevents warnings etc)
  DataTableRef    data;
  // #READ_ONLY #HIDDEN #NO_SAVE the datatable to write to
  String          col_name;
  // #AKA_chan_name #ADD_COMPLETER_SIMPLE of the column in the data to use
  NetTarget       net_target;
  // what to read/write from on the network
  NetworkRef      network;
  // #READ_ONLY #HIDDEN #NO_SAVE the network to operate on -- managed by owner
  String          layer_name;
  // #CONDSHOW_ON_net_target:LAYER #ADD_COMPLETER_SIMPLE the name of the Layer that will get read or written

  PosVector2i     offset;
  // #EXPERT offset in layer or unit group at which to start reading/writing
  
  virtual void    SetDataNetwork(DataTable* db, Network* net);
  // #CAT_LayerData set the data table and network pointers enable looking up columns/layer names

  virtual int     GetColIdx(DataTable* db);
  // #CAT_LayerData get column index from datatable

  virtual void  ToggleOff();
  // #MENU #MENU_ON_Object #DYN1 #CAT_Structure toggle the off status of this layer writer -- if on, turn off, if off, turn on

  String          GetName() const override		{ return col_name; }
  String          GetDisplayName() const override;
  int             GetEnabled() const    override { return !off; }
  void            SetEnabled(bool value) override { off = !value; }
  void            GetMemberCompletionList(const MemberDef* md, const String& cur_txt, Completions& completions) override;

  TA_SIMPLE_BASEFUNS(LayerDataEl);
protected:
  void  UpdateAfterEdit_impl() override;
  void 	CheckThisConfig_impl(bool quiet, bool& rval) override;

private:
  void	Initialize();
  void 	Destroy();
};

#endif // LayerDataEl_h
