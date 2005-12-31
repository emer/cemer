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



// netdata.h

#ifndef NETDATA_H
#define NETDATA_H

#include "pdpbase.h"
#include "spec.h"
//#include "datatable.h"
#include "ta_data.h"

// forwards this file
class MatrixGroup_List;
class DataSet;
class DataSet_MGroup;

class MatrixGroup_List: public taList<taMatrix_Group> { // list of group of matrixes, i.e. table of data
INHERITED(taList<taMatrix_Group>)
public:
  
  TA_BASEFUNS(MatrixGroup_List); //
  
protected:
  override void*	El_Own_(void* it); // for blank sets, initialize from DataSet

private:
  void	Initialize();
  void 	Destroy() {}
};

class DataSet: public taNBase, public IDataSource {
INHERITED(taNBase)
public:
  SourceChannel_Group	source_channels;
  MatrixGroup_List	data;
  
  void			InitDataItem(taMatrix_Group* item); // initializes a blank item with proper complement of data
  
  void			InitFromConduit(NetConduit* cond = NULL); // #MENU_ON_Object initialize this data set to be compatible with the given conduit
  
  virtual void		Reset(); // clears all the channels and data
  virtual void		ResetData(); // clears all the data, leaving the schema
  
  void  InitLinks();
  void	CutLinks();
  void 	Copy_(const DataSet& cp);
  TA_BASEFUNS(DataSet); //

protected:
  virtual void		InitDataItem_impl(SourceChannel_Group* ch_gp, taMatrix_Group* data_gp); // initializes a group, then recursively calls for subgroups
  virtual void		InitMatrix(SourceChannel* chan, taMatrix_impl* mat); 
private:
  void	Initialize();
  void 	Destroy();
  
public: // IDataSource i/f
  override int		source_channel_count() {return source_channels.leaves;}
  override SourceChannel* source_channel(int idx) {return (SourceChannel*)source_channels.Leaf(idx);}
protected: 
  override void 	DoProduceData(SourceChannel* ch, ptaMatrix_impl& data, bool& handled);
};


class DataSet_MGroup : public taGroup<DataSet> {
  // group of data objects
INHERITED(taGroup<DataSet>)
public:
//  virtual void	AutoEdit();

  TA_BASEFUNS(DataSet_MGroup);
  
private:
  void	Initialize() 		{ SetBaseType(&TA_DataSet); }
  void 	Destroy()		{ };
};



/* obsolete
////////////////////////////
//   DataItem/DataSetSpec    //
////////////////////////////

#define FOR_ITR_PAT_SPEC(pT, pel, pgrp, pitr, sT, sel, sgrp, sitr) \
for(pel = (pT*) pgrp FirstEl(pitr), sel = (sT*) sgrp FirstEl(sitr); \
    pel && sel; \
    pel = (pT*) pgrp NextEl(pitr), sel = (sT*) sgrp NextEl(sitr))

// forwards declared this file:
class DataItem;
class DataItem_Group;
class PSChannel; // #IGNORE impl class for SourceChannel

class DataItemSpec : public BaseSubSpec {
  // ##SCOPE_Environment sub-spec for patterns within an eventspec
INHERITED(BaseSubSpec)
friend class Environment;
public:
  enum PatTypes {
    INACTIVE,			// not presented to network
    INPUT,			// input pattern
    TARGET,			// target (output) pattern
    COMPARE 			// comparison pattern (for error only)
  };

  enum PatLayer {
    FIRST,			// first layer in the network
    LAST,			// last layer in the network
    LAY_NAME,			// specify layer by name
    LAY_NUM 			// specify layer by number
  };

  enum PatFlags {		// bit-flags for the flag field on pattern
    NO_FLAGS 		= 0x00,	// no flags on pattern value, apply as normal
    TARG_FLAG 		= 0x01,	// unit's TARG flag is set
    EXT_FLAG 		= 0x02,	// unit's EXT flag is set
    TARG_EXT_FLAG 	= 0x03,	// unit's TARG and EXT flags are set
    COMP_FLAG 		= 0x04,	// unit's COMP flag is set
    COMP_TARG_FLAG	= 0x05,	// unit's COMP and TARG flags are set
    COMP_EXT_FLAG	= 0x06,	// unit's COMP and EXT flags are set
    COMP_TARG_EXT_FLAG  = 0x07,	// unit's COMP, TARG, and EXT flags are set
    TARG_VALUE 		= 0x10,	// pattern value goes to the unit targ field
    EXT_VALUE 		= 0x20,	// pattern value goes to the unit ext field
    TARG_EXT_VALUE 	= 0x30,	// pattern value goes to the unit targ & ext fields
    NO_UNIT_FLAG 	= 0x40,	// no unit flags are set, but value is set as normal
    NO_UNIT_VALUE 	= 0x80,	// don't set the unit's value, but flag as normal
    NO_APPLY 		= 0x100 // don't apply this value to unit (no flags either)
  };

  enum LayerFlags {		// how to flag the layer's external input status
    DEFAULT 		= 0x00,	// set default layer flags based on pattern type
    TARG_LAYER 		= 0x01,	// as a target layer
    EXT_LAYER 		= 0x02,	// as an external input layer
    TARG_EXT_LAYER 	= 0x03,	// as both external input and target layer
    COMP_LAYER		= 0x04,	// as a comparison layer
    COMP_TARG_LAYER	= 0x05,	// as a comparision and target layer
    COMP_EXT_LAYER	= 0x06,	// as a comparison and external input layer
    COMP_TARG_EXT_LAYER = 0x07,	// as a comparison, target, and external input layer
    NO_LAYER_FLAGS	= 0x10 	// don't set any layer flags at all
  };

  enum PatUseFlags {            // control use of the flag field
    USE_NO_FLAGS,               // no flags on pattern value, apply as normal
    USE_PATTERN_FLAGS,          // use flags from pattern
    USE_GLOBAL_FLAGS,           // use global flags from pattern spec
    USE_PAT_THEN_GLOBAL_FLAGS   // use flags from pattern if avail, else global flags
  };

  PatTypes	type;    	// #ENVIROVIEW Type of pattern
  PatLayer  	to_layer;    	// #ENVIROVIEW which network layer to present pattern to
  String        layer_name;  	// #ENVIROVIEW #CONDEDIT_ON_to_layer:LAY_NAME name of layer
  int           layer_num;   	// #ENVIROVIEW #CONDEDIT_ON_to_layer:LAY_NUM number of layer
  Layer* 	layer;		// #READ_ONLY #NO_SAVE Pointer to Layer presented to

  TypeDef*	pattern_type;	// #TYPE_DataItem type of pattern to use
  LayerFlags	layer_flags;	// #ENVIROVIEW how to flag the layer's external input status
  PatUseFlags 	use_flags;	// how to use the flags (on each pattern or global_flags)
  int		n_vals;		// number of values in pattern
  PosTDCoord    geom;		// geometry of pattern in EnviroView
  PosTDCoord    pos;		// position of pattern in EnviroView
  float		initial_val;	// Initial value for pattern values
  Random	noise;		// #ENVIROVIEW Noise added to values when applied
  String_Array  value_names;	// display names of the individual pattern values
  int_Array 	global_flags;	// #CONDEDIT_ON_use_flags:USE_GLOBAL_FLAGS,USE_PAT_THEN_GLOBAL_FLAGS these are global flags for all events (cf use_flags)

  virtual bool 	SetLayer(Network* net);
  // set layer pointer to point to the target layer
  virtual void 	UnSetLayer();
  // when done, don't keep pointing to it.
  virtual void	FlagLayer();
  // set layer flag to reflect the kind of input received
  virtual float Value(DataItem* pat, int index);
  // return value at given index from pattern (order can be changed, eg GroupDataItemSpec)
  virtual int	Flag(PatUseFlags flag_type, DataItem* pat, int index);
  // return flag at given index from pattern (order can be changed, eg GroupDataItemSpec)
  virtual void 	ApplyValue(DataItem* pat, Unit* uni, int index);
  // assign unit value and ext_flag based on pattern at given index
  virtual void	ApplyValue_impl(Unit* uni, float val, int flags);
  // implementation of apply value, taking just the value and the flags
  virtual void	ApplyValueWithFlags(Unit* uni, float val, int flags);
  // assign unit value and ext_flag with pattern flags
  virtual void	ApplyDataItem(DataItem* pat);
  // apply given pattern to all units (layer must already be set)

  virtual DataItem* NewDataItem(DataSet* ev, DataItem_Group* par);
  // creates a new pattern in my image in event in parent group at index
  virtual void	UpdateDataItem(DataSet* ev, DataItem* pat);
  // updates existing pattern to current spec settings

  virtual void 	SetToLayer(Layer* lay=NULL);
  // #BUTTON #NULL_OK set configuration of the pattern spec based on layer, and set to go to that layer (NULL = update to current layer)
  virtual void 	SetToLayName(const char* lay_nm);
  // set to_layer=LAY_NAME and name and layer_name = lay_nm
  virtual void	UpdateAllDataSets();
  // #BUTTON update all events using pattern spec
  virtual void 	ApplyNames();
  // #BUTTON set the names of units in the network according to the current value_names

  virtual Network* GetDefaultNetwork();
  // #IGNORE get default network from project
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Environment); }
#endif
  void	UpdateAfterEdit();	// gets information off of the layer, etc
  void	Initialize();
  void 	Destroy();
  void  InitLinks();
  void	CutLinks();
  void 	Copy_(const DataItemSpec& cp);
  COPY_FUNS(DataItemSpec, BaseSubSpec);
  TA_BASEFUNS(DataItemSpec); //
  
public: // added to support 4.0 data channels
  SourceChannel		src_channel; // #BROWSE source channel, for supplying the patterns as data
  void			UpdateChannel(); //#IGNORE called in UAE
};

class DataItemSpec_Group : public taBase_Group {
  // ##SCOPE_Environment group of pattern specs (acts like a template for pattern groups)
public:
  TypeDef*	pat_gp_type;	// #TYPE_DataItem_Group type of pattern group to use

  // overload standard operators to have corresponding effect on patterns
  TAPtr		NewEl(int n_els=0, TypeDef* type=NULL);
  taGroup<taBase>* NewGp(int n_gps=0, TypeDef* type=NULL);
  TAPtr		New(int n_objs=0, TypeDef* type=NULL);
  bool		Remove(const char* it)	{ return taGroup_impl::Remove(it); }
  bool		Remove(TAPtr it)	{ return taGroup_impl::Remove(it); }
  bool		Remove(int i);

  virtual DataItem_Group* NewDataItemGroup(DataSet* ev, DataItem_Group* par);
  // creates a new pattern_group in my image in event in parent group at index
  virtual void	UpdateDataItemGroup(DataSet* ev, DataItem_Group* pg);
  // updates existing pattern group to current spec settings

  void	UpdateAfterEdit();	// call DataSetSpec's UpdateAllDataSets()

  virtual void	LinearLayout();
  // #MENU #MENU_ON_Actions Layout DataItemSpecs linearly based according to event spec default layout

#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Environment); }
#endif
  void	Initialize();
  void 	Destroy();
  void	CutLinks();
  void 	Copy_(const DataItemSpec_Group& cp);
  COPY_FUNS(DataItemSpec_Group, taBase_Group);
  TA_BASEFUNS(DataItemSpec_Group);
};

class DataSetSpec : public BaseSpec {
  // ##SCOPE_Environment ##MEMB_IN_GPMENU ##IMMEDIATE_UPDATE event specification
public:
  enum DataItemLayout {
    DEFAULT,
    HORIZONTAL,
    VERTICAL
  };

  Network*		last_net;
  // #READ_ONLY #NO_SAVE last network events applied to
  DataItemSpec_Group 	patterns;
  // #IN_GPMENU #NO_INHERIT group of pattern specs (one-to-one with patterns)
  DataItemLayout		pattern_layout;
  // determines the layout of patterns within an event (just for display purposes)

  virtual void 	ApplyDataItems(DataSet* ev, Network* net);
  // apply patterns to the network

  virtual void	SetLayers(Network* net); // set layers according to given net
  virtual void	UnSetLayers(); 		 // clear layer pointers (and last_net)

  int	MaxX();			// maximum X coordinate of patterns
  int	MaxY();			// maximum Y coordinate of patterns

  virtual void	NewDataSet(DataSet* ev);
  // defines a new event in my image
  virtual void	UpdateDataSet(DataSet* ev);
  // updates existing event to current spec settings
  virtual void	UpdateAllDataSets();
  // #BUTTON update all events using this event spec
  virtual void 	UpdateFromLayers();
  // #BUTTON set configuration of all pattern specs based on their corresponding layer in the network
  virtual void 	ApplyNames(Network* net);
  // #BUTTON set the names of units in the network according to the value_names on the pattern specs

  virtual void	LinearLayout(DataItemLayout pat_layout = DEFAULT);
  // #MENU #MENU_ON_Actions Layout DataItemSpecs in a line going either horizontal or vertical (just for display purposes)

  virtual void	AutoNameDataSet(DataSet* ev, float act_thresh = .5f, int max_pat_nm = 3, int max_val_nm = 3);
  // automatically name event based on the pattern names and value (unit) names for those units above act_thresh, e.g., Inp:vl1_vl2,Out:vl1_vl2
  virtual void	AutoNameAllDataSets(float act_thresh = .5f, int max_pat_nm = 3, int max_val_nm = 3);
  // #BUTTON automatically name all events that use this spec based on the pattern names and value (unit) names for those units above act_thresh, e.g., Inp:vl1_vl2,Out:vl1_vl2

  virtual bool	DetectOverlap();
  // determine if the patterns overlap on top of each other

  virtual void	AddToView();		// add event to view(s)
  virtual void	RemoveFromView();	// remove event from view(s)

  void		UpdateSubSpecs();
  void          UpdateSpec();
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Environment); }
#endif
  void  UpdateAfterEdit();
  void	Initialize();
  void 	Destroy() 	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void 	Copy(const DataSetSpec& cp);
  TA_BASEFUNS(DataSetSpec);
};

class DataSetSpec_SPtr : public SpecPtr<DataSetSpec> {
public:
  BaseSpec_MGroup*	GetSpecGroup();	// event specs go in environment
  void 	Initialize() 		{ };
  void	Destroy()		{ };
  TA_BASEFUNS(DataSetSpec_SPtr);
};


////////////////////////
//   DataItem/DataSet    //
////////////////////////

class DataItem : public taOBase {
  // ##SCOPE_Environment ##EXT_pat ##NO_TOKENS ##NO_UPDATE_AFTER Contains activation values to be applied to a network layer
public:
  float_RArray 	value;  	// Values of DataItem
  int_Array   	flag;  		// Flags of DataItem
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Environment); }
#endif
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const DataItem& cp);
  COPY_FUNS(DataItem, taOBase);
  TA_BASEFUNS(DataItem);
};

BaseGroup_of(DataItem);


class DataSet : public taNBase {
  // ##SCOPE_Environment ##EXT_evt ##NO_TOKENS ##NO_UPDATE_AFTER Contains patterns of activation for different layers in the network specifying one event
public:
  int			index;		// #NO_SAVE #READ_ONLY Index of this event within group
  DataItem_Group 	patterns;  	// #NO_SAVE_PATH_R group of patterns
  DataSetSpec_SPtr	spec;		// determines the configuration of patterns and how they are presented to the network

  void 		ApplyDataItems(Network* net) { spec->ApplyDataItems(this, net); }

  virtual void	GetLocalSpec();	// get event spec that is local to enviro
  virtual void	SetSpec(DataSetSpec* es);	// set the spec to this spec, and update the event to fit this spec
  virtual void	UpdateFmSpec();	// #BUTTON update event configuration to fit current spec

  virtual String GetDisplayName(); // get a name for displaying event clearly

  virtual void	AddToView();		// add event to view(s)
  virtual void	RemoveFromView();	// remove event from view(s)

  virtual void	PresentDataSet(TrialProcess* trial_proc, bool new_init=false);
  // #BUTTON present this event to the given trial process, which is ReInit (or new_init) and run

  virtual void	AutoNameDataSet(float act_thresh = .5f, int max_pat_nm = 3, int max_val_nm = 3);
  // #BUTTON automatically name event based on the pattern names and value (unit) names for those units above act_thresh, e.g., Inp:vl1_vl2,Out:vl1_vl2
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Environment); }
#endif
  void	UpdateAfterEdit();

  void	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void 	Copy(const DataSet& cp);
  TA_BASEFUNS(DataSet);
};

// environment presents the following model: either a flat list of events
// (interface = DataSetCount() && GetDataSet()) or a set of event-groups (leaf groups)
// (interface = GroupCount() && GetGroup())
// when events are generated algorithmically, the results are put in a set of event
// structures, and the same interface can be used (see ProcEnvironment below)
// other models are definable, but the standard EpochProcess will not
// understand them.

class DataSet_MGroup : public taGroup<DataSet> {
  // ##SCOPE_Environment Group of events
protected:
  void	El_SetIndex_(void* base, int idx) { ((DataSet*)base)->index = idx; }
public:

  virtual void	InitDataSets(Environment*)	{ };
  // initialize events at the level of each event group (can be called by enviro)

  virtual int	 DataSetCount()		{ return leaves; }
  // #MENU #MENU_ON_Actions #USE_RVAL number of events in environment
  virtual DataSet* GetDataSet(int i)	{ return (DataSet*)Leaf_(i); }

  DataSet* 	New(int n_objs=0, TypeDef* typ = NULL);    // make using default spec..
  DataSet* 	NewEl(int n_els=0, TypeDef* typ = NULL);  // make using default spec..
  DataSet*	NewFmSpec(int n_objs, TypeDef* typ = NULL, DataSetSpec* es = NULL);
  // #MENU #MENU_ON_Object #TYPE_ON_el_base #UPDATE_MENUS make using given event spec
  virtual void	NewEl_impl(int old_sz, DataSetSpec* es=NULL);
  // #IGNORE implements event spec copying (if es is NULL, uses default)

  virtual String GetDisplayName(); // get a name for displaying event clearly
  virtual void	AddToView();
  virtual void	RemoveFromView();
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Environment); }
#endif
  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(DataSet_MGroup);
}; //

////////////////////////
//   Environment      //
////////////////////////

// note: we just implement ISequenable directly on ourself, a bit sleezy, but works
class Environment : public taNBase, public IDataSource, protected ISequencable {
  // ##EXT_env ##COMPRESS basic environment: contains events to present to the network, and can be used to hold data for analysis
INHERITED(taNBase)
public:
  enum TextFmt {
    NAME_FIRST,			// save file with names as first column
    NAME_LAST,			// save file with names as last column
    NO_NAME			// no names at all in file..
  };

  enum DistMatFmt {		// distance matrix format
    STD_PRINT,			// standard printout format
    PRINT_NO_LABELS,		// standard printout with no event names or other labels
    GRID_LOG			// grid log format for importing into a grid log
  };

  BaseSpec_MGroup 	event_specs;	// specs for events: controls the layout and configuration of events
  DataSet_MGroup 		events;		// the events, contain patterns that map onto layers of the network
  int			event_ctr; 	// #READ_ONLY #SHOW counter for interactive interface with environment: number of events processed since last InitDataSets()

  virtual void	InitDataSets()	{ event_ctr = 0; }
  // #MENU #MENU_ON_Actions #UPDATE_MENUS initialize events for an epoch (eg, if algorithmically created)
  virtual void	UnSetLayers();
  // reset layer pointers on patterns so they are automatically recomputed when events are presented

  virtual void	UpdateAllDataSets();
  // #MENU #MENU_ON_Actions #CONFIRM update all events from their event specs
  virtual void	UpdateAllDataSetSpecs();
  // #MENU #CONFIRM update all event specs based on the current configuration of the default network

  // Note: there are 3 interfaces to the environment:
  // 1. A flat list of events accessed by event index (supported by standard EpochProcess)
  // 2. Groups of events accessed first by group index, then by index of event within group
  // 	(supported by SequenceEpoch and SequenceProcess)
  // 3. An 'interactive' model that doesn't depend on indicies, just InitDataSets() at start and GetNextDataSet() until NULL
  //    (GetNextDataSet is hook for generating new events based on current state; supported by InteractiveEpoch)

  // the flat event list model of the environment
  virtual int	DataSetCount()		{ return events.leaves; }
  // #MENU #MENU_ON_Actions #USE_RVAL #MENU_SEP_BEFORE number of events in environment
  virtual DataSet* GetDataSet(int ev_index)	{ return (DataSet*)events.Leaf_(ev_index); }
  // get the event at given index in a flat list of all events in the environment

  // the leaf-group model of the environment
  virtual int	GroupCount();
  // #MENU #USE_RVAL number of event groups in environment
  virtual DataSet_MGroup* GetGroup(int gp_index);
  // get the event group (collection of events) at the specified index of all groups in the environment

  // the interactive model of the environment: just GetNextDataSet() until it returns NULL
  virtual DataSet* GetNextDataSet();
  // #MENU return the next event for processing (or NULL to end epoch): interface for the interactive environment model (hook for generating new event based on current state)

  virtual void  UnitNamesToNet(DataSetSpec* event_spec_with_names = NULL, Network* network = NULL);
  // #MENU #MENU_SEP_BEFORE #NULL_OK copy names from pattern spec value_names to corresponding units in the network.  uses default event spec and network if NULL
  virtual void  MakeNetIOLayers(DataSetSpec* event_spec, Network* network);
  // #MENU configure network input/output layers based on patterns in event_spec
  virtual void	AutoNameAllDataSets(float act_thresh = .5f, int max_pat_nm = 3, int max_val_nm = 3);
  // #MENU automatically name all events based on the pattern names and value (unit) names for those units above act_thresh, e.g., Inp:vl1_vl2,Out:vl1_vl2

  virtual void 	ReadText(istream& strm, DataSetSpec* es, TextFmt fmt = NAME_FIRST);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_strm_pat #UPDATE_MENUS Read text-formatted event/pattern files, including old pdp files, (using given event spec, -1=default)
  virtual void	WriteText(ostream& strm, int pat_no = -1, TextFmt fmt = NAME_FIRST);
  // #MENU #EXT_strm_pat Save enviro in text format: pat_no of -1 gives all pats, fmt for name
  virtual void	ReadBinary(istream& strm, DataSetSpec* es);
  // #MENU #EXT_strm_pat read event pattern information in binary format (just the numbers only)
  virtual void	WriteBinary(ostream& strm);
  // #MENU #EXT_strm_pat write event pattern information in binary format (just the numbers only)

  // the following are simple pattern generation routines

  virtual void	ReplicateDataSets(int n_replicas, bool make_groups=false);
  // #MENU #MENU_ON_Generate #UPDATE_MENUS replicate existing events, optionally placing in groups

  virtual void	PermutedBinary(int pat_no, int n_on);
  // #MENU #MENU_SEP_BEFORE create permuted binary patterns of n_on 1's and rest 0's (pat_no: -1 = all pats)
  virtual void	PermutedBinary_MinDist(int pat_no, int n_on, float dist,
				       float_RArray::DistMetric metric=float_RArray::HAMMING,
				       bool norm=false, float tol=0.0f);
  // #MENU create permuted binary patterns with dist minimum hamming distance (or dist max_correl) (pat_no: -1 = all pats)
  virtual void  FlipBits(int pat_no, int n_off, int n_on);
  // #MENU flip n_off bits from 1's to 0's, and n_on bits from 0's to 1's (pat_no: -1 = all pats)
  virtual void  FlipBits_MinMax(int pat_no, int n_off, int n_on, float min_dist, float max_dist,
				float_RArray::DistMetric metric=float_RArray::HAMMING,
				bool norm=false, float tol=0.0f);
  // #MENU flip bits, ensuring range within min and max distances (pat_no: -1 = all pats)
  virtual void  FlipBits_GpMinMax(int pat_no, int n_off, int n_on, float within_min_dist,
				  float within_max_dist, float between_dist,
				  float_RArray::DistMetric metric=float_RArray::HAMMING,
				  bool norm=false, float tol=0.0f, int st_gp=0, int ed_gp=-1);
  // #MENU flip bits, ensuring within-group min and max distances, and between-group min dist (pat_no: -1 = all pats)

  // helper pattern-wise functions for above generation routines
  static void	PermutedBinaryPat(DataItem* pat, int n_on, float on_val=1.0f, float off_val=0.0f);
  // set pat values to permuted binary pattern of n_on on_vals and rest off_vals (pat_no: -1 = all pats)
  static void	FlipBitsPat(DataItem* pat, int n_off, int n_on);
  // flip n_off of the 1 bits into the 0 state, and n_on of the 0 bits to the 1 state
  static void	AddNoisePat(DataItem* pat, const Random& rnd_spec);
  // add random noise to given pattern
  virtual float LastMinDist(int n, int pat_no, float_RArray::DistMetric metric=float_RArray::HAMMING,
			    bool norm=false, float tol=0.0f);
  // returns minimum distance (or max correl) between last (n th) pattern and all previous
  virtual float LastMinMaxDist(int n, int pat_no, float& max_dist,
			       float_RArray::DistMetric metric=float_RArray::HAMMING,
			       bool norm=false, float tol=0.0f);
  // returns min and max distance between last (n th) pattern and all previous
  virtual float GpWithinMinMaxDist(DataSet_MGroup* gp, int n, int pat_no, float& max_dist,
				   float_RArray::DistMetric metric=float_RArray::HAMMING,
				   bool norm=false, float tol=0.0f);
  // returns min and max distance between last (n th) pattern and all previous within group
  virtual float GpLastMinMaxDist(int gp_no, DataItem* trg_pat, int pat_no, float& max_dist,
				 float_RArray::DistMetric metric=float_RArray::HAMMING,
				 bool norm=false, float tol=0.0f, int st_gp=0);
  // returns min and max distance between patterns in all groups up to gp_no for pattern pat
  virtual float GpMinMaxDist(DataSet_MGroup* gp, DataItem* trg_pat, int pat_no, float& max_dist,
			     float_RArray::DistMetric metric=float_RArray::HAMMING,
			     bool norm=false, float tol=0.0f);
  // returns min and max distance between probe pattern and all in group

  virtual void	Clear(int pat_no=-1, float val = 0.0);
  // #MENU #MENU_SEP_BEFORE #CONFIRM clear out given pattern number (set to given val) (pat_no: -1 = all pats)

  virtual void	AddNoise(int pat_no, const Random& rnd_spec);
  // #MENU add random noise of specified type to the patterns (pat_no: -1 = all pats)

  virtual void	TransformPats(int pat_no, const SimpleMathSpec& trans);
  // #MENU Apply given transformation to pattern pat_no of all events (pat_no: -1 = all pats)

  ////////////////////////////////////
  // 	Analyze
  ////////////////////////////////////
  virtual void	DistMatrix(ostream& strm, int pat_no, float_RArray::DistMetric metric=float_RArray::HAMMING,
			   bool norm=false, float tol=0.0f, DistMatFmt format=STD_PRINT, int precision = -1);
  // #MENU #EXT_strm_dstmat #MENU_ON_Analyze output distance matrix for events based on pattern pat_no
  virtual void	DistArray(float_RArray& dist_ary, int pat_no,
			  float_RArray::DistMetric metric=float_RArray::HAMMING,
			  bool norm=false, float tol=0.0f);
  // get distance matrix as an upper-triangular matrix (including diagonals) for events based on pattern pat_no
  virtual void	GpDistArray(float_RArray& within_dist_ary, float_RArray& between_dist_ary, int pat_no,
			    float_RArray::DistMetric metric=float_RArray::HAMMING,
			    bool norm=false, float tol=0.0f);
  // get within group and between group distance matricies as arrays for events based on pattern pat_no

  virtual void	CmpDistMatrix(ostream& strm, int pat_no, Environment* cmp_env, int cmp_pat_no,
			      float_RArray::DistMetric metric=float_RArray::HAMMING,
			      bool norm=false, float tol=0.0f, DistMatFmt format=STD_PRINT);
  // #MENU #EXT_strm_dstmat comparative distance array between two environments
  virtual void	CmpDistArray(float_RArray& dist_ary, int pat_no, Environment* cmp_env, int cmp_pat_no,
			     float_RArray::DistMetric metric=float_RArray::HAMMING,
			     bool norm=false, float tol=0.0f);
  // comparative distance array between two environments

  virtual void	CmpDistArrayPat(float_RArray& dist_ary, DataItem* trg_pat,
				int cmp_pat_no, float_RArray::DistMetric metric=float_RArray::HAMMING,
				bool norm=false, float tol=0.0f);
  // compute comparative distance array, one pattern against this environment
  virtual void	ValOverDataSetsArray(float_RArray& ary, int pat_no, int val_no);
  // extract an array containing values for given value index in pattern pat_no across events
  virtual void	CorrelMatrix(float_RArray& mat, int pat_no, int& dim);
  // generate a correlation matrix for all patterns in pat_no in the environment (e.g., correlation of unit 1 with all other units across patterns, etc); dim = dimensionality of correl matrix = no. of vals in pattern
  virtual void	PCAEigens(float_RArray& evecs, float_RArray& evals, int pat_no, int& dim);
  // get principal components analysis eigenvectors and eigenvalues of correlation matrix across events for pattern pat_no (dim = dimensionality of correl matrix = no. of vals in pattern)
  virtual void	ProjectDataItems(const float_RArray& prjn_vector, float_RArray& vals, int pat_no);
  // project patterns in pat_no onto prjn_vector (dot product), and store resulting array of vals in vals (length = no. of events)
  virtual void	PatFreqArray(float_RArray& freqs, int pat_no, float act_thresh = .5f, bool proportion = false);
  // get frequency (proportion) of pattern activations greater than act_thresh across events
  virtual void	PatFreqText(float act_thresh = .5f, bool proportion = false, ostream& strm = cerr);
  // #MENU #ARGC_2 report frequency (proportion) of pattern values greater than act_thresh across events, to a text output (most useful if pattern values are named in value_names)
  virtual void	PatAggArray(float_RArray& agg_vals, int pat_no, Aggregate& agg);
  // aggregate pattern pat_no values over events to given array object
  virtual void	PatAggText(Aggregate& agg, ostream& strm = cerr);
  // #MENU #ARGC_1 aggregate patterns over events and print aggregated results to a text output (most useful if pattern values are named in value_names)
  virtual void 	DataSetFreqText(bool proportion = false, ostream& strm = cerr);
  // #MENU #ARGC_1 report frequency (proportion) of event names in the environment

  ////////////////////////////////////
  // 	Graphical Analyze
  ////////////////////////////////////

#ifdef TA_GUI
  virtual void	DistMatrixGrid(GridLog* disp_log, int pat_no, float_RArray::DistMetric metric=float_RArray::HAMMING,
			   bool norm=false, float tol=0.0f);
  // #MENU #NULL_OK output to grid log distance matrix for events based on pattern pat_no
  virtual void	CmpDistMatrixGrid(GridLog* disp_log, int pat_no, Environment* cmp_env, int cmp_pat_no,
			      float_RArray::DistMetric metric=float_RArray::HAMMING,
			      bool norm=false, float tol=0.0f);
  // #MENU #NULL_OK output to grid log comparative distance matrix between two environments
  virtual void	ClusterPlot(GraphLog* disp_log, int pat_no,
			    float_RArray::DistMetric metric=float_RArray::EUCLIDIAN,
			    bool norm=false, float tol=0.0f);
  // #MENU #MENU_SEP_BEFORE #NULL_OK produce a cluster plot (in graph log, NULL = make a new one) of the given pat_no across events
  virtual void	CorrelMatrixGrid(GridLog* disp_log, int pat_no);
  // #MENU #NULL_OK generate a correlation matrix for all patterns in pat_no in the environment (e.g., correlation of unit 1 with all other units across patterns, etc) and plot result in grid log (NULL = new log)
  virtual void	PCAEigenGrid(GridLog* disp_log, int pat_no, bool print_eigen_vals = false);
  // #MENU #NULL_OK perform principal components analysis of the correlations of patterns in pat_no across events, plotting all eigenvectors in the grid log (NULL = new log)
  virtual void	PCAPrjnPlot(GraphLog* disp_log, int pat_no, int x_axis_component=0, int y_axis_component=1, bool print_eigen_vals = false);
  // #MENU #NULL_OK perform principal components analysis of the correlations of patterns in pat_no across events, plotting projections of patterns on the given principal components in the graph log (NULL = new log)
  virtual void	MDSPrjnPlot(GraphLog* disp_log, int pat_no, int x_axis_component=0, int y_axis_component=1,
			    float_RArray::DistMetric metric=float_RArray::EUCLIDIAN,
			    bool norm=false, float tol=0.0, bool print_eigen_vals = false);
  // #MENU #NULL_OK perform multidimensional scaling on the distance matrix (computed according to metric, norm, tol parameters) of patterns in pat_no across events in the graph log (NULL = new log)
  virtual void	DataSetPrjnPlot(DataSet* x_axis_event, DataSet* y_axis_event, int pat_no,
			      GraphLog* disp_log, float_RArray::DistMetric metric=float_RArray::INNER_PROD,
			      bool norm=false, float tol=0.0);
  // #MENU #NULL_OK #FROM_GROUP_1_events project all events according to their smiliarity to the two specified events using given distance metrics

  virtual void	EnvToGrid(GridLog* disp_log, int pat_no, int ev_x=-1, int ev_y=-1, int pt_x=-1, int pt_y=-1);
  // #MENU #MENU_SEP_BEFORE #NULL_OK send environment to grid log, with given layout
  //   (-1 = default, ev = event layout, pt = pattern layout) (NULL = new grid log) 

  virtual void	PatFreqGrid(GridLog* disp_log, float act_thresh = .5f, bool proportion = false);
  // #MENU #NULL_OK report frequency (proportion) of pattern values greater than act_thresh across events, to a grid log (NULL = make new log)
  virtual void	PatAggGrid(GridLog* disp_log, Aggregate& agg);
  // #MENU #NULL_OK aggregate patterns over events and plot aggregated results in a grid log (NULL = make new log)
#endif // end of gui-only analysis functions
  virtual DataSetSpec*	GetAnDataSetSpec();
  // returns either the default event spec if it exists, or makes one

//obs  TypeDef*	GetDefaultView()	{ return &TA_EnviroView; }
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Environment); }
#endif
  void	UpdateAfterEdit();

  void	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void 	Copy(const Environment& cp);
  TA_BASEFUNS(Environment); //

protected: //ISequencable i/f note: we hide this from direct access -- get at it through sequencer() prop
  override int		num_items() {return DataSetCount();} // #IGNORE 
  override bool		is_indexable() {return true;} // #IGNORE 
  override bool		is_sequential() {return true;} // #IGNORE
  override void		GoToItem(int index); // #IGNORE 
  override void		ResetItem(); // #IGNORE
  override bool		NextItem(); // #IGNORE

public: // IDataSource i/f
  override bool		can_sequence() const {return true;} // true if has a ISequencable interface
  override ISequencable* sequencer() {return this;} // sequencing interface, if sequencable
  override int		source_channel_count(); // number of source channels
  override SourceChannel* source_channel(int idx); // get a source channel
protected: 
//  override void 	DoProduceData(SourceChannel* ch, ptaMatrix_impl& data, bool& handled); // #IGNORE

protected: // support routines for added 4.0 interfaces
  virtual void		SetCurrentDataSet(DataSet* ev); // called by GoToItem and NextItem
};

// note: Environment_MGroup name is for compatiblity with v3.2 files
class Environment_MGroup : public taGroup<Environment> {
  // group of environments
public:
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Environment); }

  void  Initialize()            { SetBaseType(&TA_Environment); }
  void  Destroy()               { };
  TA_BASEFUNS(Environment_MGroup);
};

class Environment_List : public taList<Environment> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER a simple list of environments
public:
  void  Initialize()            { SetBaseType(&TA_Environment); }
  void  Destroy()               { };
  TA_BASEFUNS(Environment_List);
};

*/
#endif // NETDATA_H
