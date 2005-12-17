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



// event.h

#ifndef EVENT_H
#define EVENT_H

#include "pdpbase.h"
//#include "spec.h"
#include "ta_data.h"
//#include "datatable.h"
#include "tarandom.h"

// NOTE: see NOTES and TODOS in .cpp file

#ifdef TA_GUI
#include "fontspec.h"
#endif

//////////////////////////
//   NetConduit		//
//////////////////////////

#define FOR_ITR_PAT_SPEC(pT, pel, pgrp, pitr, sT, sel, sgrp, sitr) \
for(pel = (pT*) pgrp FirstEl(pitr), sel = (sT*) sgrp FirstEl(sitr); \
    pel && sel; \
    pel = (pT*) pgrp NextEl(pitr), sel = (sT*) sgrp NextEl(sitr))

// forwards declared this file:


/*obs
////////////////////////
//   Pattern/Event    //
////////////////////////

class Pattern : public taOBase {
  // ##SCOPE_Environment ##EXT_pat ##NO_TOKENS ##NO_UPDATE_AFTER Contains activation values to be applied to a network layer
public:
  float_RArray 	value;  	// Values of Pattern
  int_Array   	flag;  		// Flags of Pattern
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_NetConduit); }
#endif
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const Pattern& cp);
  COPY_FUNS(Pattern, taOBase);
  TA_BASEFUNS(Pattern);
};

BaseGroup_of(Pattern);


class Event : public taNBase {
  // ##SCOPE_Environment ##EXT_evt ##NO_TOKENS ##NO_UPDATE_AFTER Contains patterns of activation for different layers in the network specifying one event
public:
  int			index;		// #NO_SAVE #READ_ONLY Index of this event within group
  Pattern_Group 	patterns;  	// #NO_SAVE_PATH_R group of patterns
  NetConduit_SPtr	spec;		// determines the configuration of patterns and how they are presented to the network

  void 		ApplyPatterns(Network* net) { spec->ApplyPatterns(this, net); }

  virtual void	GetLocalSpec();	// get event spec that is local to enviro
  virtual void	SetSpec(NetConduit* es);	// set the spec to this spec, and update the event to fit this spec
  virtual void	UpdateFmSpec();	// #BUTTON update event configuration to fit current spec

  virtual String GetDisplayName(); // get a name for displaying event clearly

  virtual void	AddToView();		// add event to view(s)
  virtual void	RemoveFromView();	// remove event from view(s)

  virtual void	PresentEvent(TrialProcess* trial_proc, bool new_init=false);
  // #BUTTON present this event to the given trial process, which is ReInit (or new_init) and run

  virtual void	AutoNameEvent(float act_thresh = .5f, int max_pat_nm = 3, int max_val_nm = 3);
  // #BUTTON automatically name event based on the pattern names and value (unit) names for those units above act_thresh, e.g., Inp:vl1_vl2,Out:vl1_vl2
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_NetConduit); }
#endif
  void	UpdateAfterEdit();

  void	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void 	Copy(const Event& cp);
  TA_BASEFUNS(Event);
};

// environment presents the following model: either a flat list of events
// (interface = EventCount() && GetEvent()) or a set of event-groups (leaf groups)
// (interface = GroupCount() && GetGroup())
// when events are generated algorithmically, the results are put in a set of event
// structures, and the same interface can be used (see ProcEnvironment below)
// other models are definable, but the standard EpochProcess will not
// understand them.

class Event_MGroup : public taGroup<Event> {
  // ##SCOPE_Environment Group of events
protected:
  void	El_SetIndex_(void* base, int idx) { ((Event*)base)->index = idx; }
public:

  virtual void	InitEvents(Environment*)	{ };
  // initialize events at the level of each event group (can be called by enviro)

  virtual int	 EventCount()		{ return leaves; }
  // #MENU #MENU_ON_Actions #USE_RVAL number of events in environment
  virtual Event* GetEvent(int i)	{ return (Event*)Leaf_(i); }

  Event* 	New(int n_objs=0, TypeDef* typ = NULL);    // make using default spec..
  Event* 	NewEl(int n_els=0, TypeDef* typ = NULL);  // make using default spec..
  Event*	NewFmSpec(int n_objs, TypeDef* typ = NULL, NetConduit* es = NULL);
  // #MENU #MENU_ON_Object #TYPE_ON_el_base #UPDATE_MENUS make using given event spec
  virtual void	NewEl_impl(int old_sz, NetConduit* es=NULL);
  // #IGNORE implements event spec copying (if es is NULL, uses default)

  virtual String GetDisplayName(); // get a name for displaying event clearly
  virtual void	AddToView();
  virtual void	RemoveFromView();
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_NetConduit); }
#endif
  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(Event_MGroup);
};

////////////////////////
//   Environment      //
////////////////////////

class Environment : public taNBase {
  // ##EXT_env ##COMPRESS basic environment: contains events to present to the network, and can be used to hold data for analysis
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
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
  Event_MGroup 		events;		// the events, contain patterns that map onto layers of the network
  int			event_ctr; 	// #READ_ONLY #SHOW counter for interactive interface with environment: number of events processed since last InitEvents()

  virtual void	InitEvents()	{ event_ctr = 0; }
  // #MENU #MENU_ON_Actions #UPDATE_MENUS initialize events for an epoch (eg, if algorithmically created)
  virtual void	UnSetLayers();
  // reset layer pointers on patterns so they are automatically recomputed when events are presented

  virtual void	UpdateAllEvents();
  // #MENU #MENU_ON_Actions #CONFIRM update all events from their event specs
  virtual void	UpdateAllNetConduits();
  // #MENU #CONFIRM update all event specs based on the current configuration of the default network

  // Note: there are 3 interfaces to the environment:
  // 1. A flat list of events accessed by event index (supported by standard EpochProcess)
  // 2. Groups of events accessed first by group index, then by index of event within group
  // 	(supported by SequenceEpoch and SequenceProcess)
  // 3. An 'interactive' model that doesn't depend on indicies, just InitEvents() at start and GetNextEvent() until NULL
  //    (GetNextEvent is hook for generating new events based on current state; supported by InteractiveEpoch)

  // the flat event list model of the environment
  virtual int	EventCount()		{ return events.leaves; }
  // #MENU #MENU_ON_Actions #USE_RVAL #MENU_SEP_BEFORE number of events in environment
  virtual Event* GetEvent(int ev_index)	{ return (Event*)events.Leaf_(ev_index); }
  // get the event at given index in a flat list of all events in the environment

  // the leaf-group model of the environment
  virtual int	GroupCount();
  // #MENU #USE_RVAL number of event groups in environment
  virtual Event_MGroup* GetGroup(int gp_index);
  // get the event group (collection of events) at the specified index of all groups in the environment

  // the interactive model of the environment: just GetNextEvent() until it returns NULL
  virtual Event* GetNextEvent();
  // #MENU return the next event for processing (or NULL to end epoch): interface for the interactive environment model (hook for generating new event based on current state)

  virtual void  UnitNamesToNet(NetConduit* event_spec_with_names = NULL, Network* network = NULL);
  // #MENU #MENU_SEP_BEFORE #NULL_OK copy names from pattern spec value_names to corresponding units in the network.  uses default event spec and network if NULL
  virtual void  MakeNetIOLayers(NetConduit* event_spec, Network* network);
  // #MENU configure network input/output layers based on patterns in event_spec
  virtual void	AutoNameAllEvents(float act_thresh = .5f, int max_pat_nm = 3, int max_val_nm = 3);
  // #MENU automatically name all events based on the pattern names and value (unit) names for those units above act_thresh, e.g., Inp:vl1_vl2,Out:vl1_vl2

  virtual void 	ReadText(istream& strm, NetConduit* es, TextFmt fmt = NAME_FIRST);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_strm_pat #UPDATE_MENUS Read text-formatted event/pattern files, including old pdp files, (using given event spec, -1=default)
  virtual void	WriteText(ostream& strm, int pat_no = -1, TextFmt fmt = NAME_FIRST);
  // #MENU #EXT_strm_pat Save enviro in text format: pat_no of -1 gives all pats, fmt for name
  virtual void	ReadBinary(istream& strm, NetConduit* es);
  // #MENU #EXT_strm_pat read event pattern information in binary format (just the numbers only)
  virtual void	WriteBinary(ostream& strm);
  // #MENU #EXT_strm_pat write event pattern information in binary format (just the numbers only)

  // the following are simple pattern generation routines

  virtual void	ReplicateEvents(int n_replicas, bool make_groups=false);
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
  static void	PermutedBinaryPat(Pattern* pat, int n_on, float on_val=1.0f, float off_val=0.0f);
  // set pat values to permuted binary pattern of n_on on_vals and rest off_vals (pat_no: -1 = all pats)
  static void	FlipBitsPat(Pattern* pat, int n_off, int n_on);
  // flip n_off of the 1 bits into the 0 state, and n_on of the 0 bits to the 1 state
  static void	AddNoisePat(Pattern* pat, const Random& rnd_spec);
  // add random noise to given pattern
  virtual float LastMinDist(int n, int pat_no, float_RArray::DistMetric metric=float_RArray::HAMMING,
			    bool norm=false, float tol=0.0f);
  // returns minimum distance (or max correl) between last (n th) pattern and all previous
  virtual float LastMinMaxDist(int n, int pat_no, float& max_dist,
			       float_RArray::DistMetric metric=float_RArray::HAMMING,
			       bool norm=false, float tol=0.0f);
  // returns min and max distance between last (n th) pattern and all previous
  virtual float GpWithinMinMaxDist(Event_MGroup* gp, int n, int pat_no, float& max_dist,
				   float_RArray::DistMetric metric=float_RArray::HAMMING,
				   bool norm=false, float tol=0.0f);
  // returns min and max distance between last (n th) pattern and all previous within group
  virtual float GpLastMinMaxDist(int gp_no, Pattern* trg_pat, int pat_no, float& max_dist,
				 float_RArray::DistMetric metric=float_RArray::HAMMING,
				 bool norm=false, float tol=0.0f, int st_gp=0);
  // returns min and max distance between patterns in all groups up to gp_no for pattern pat
  virtual float GpMinMaxDist(Event_MGroup* gp, Pattern* trg_pat, int pat_no, float& max_dist,
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

  virtual void	CmpDistArrayPat(float_RArray& dist_ary, Pattern* trg_pat,
				int cmp_pat_no, float_RArray::DistMetric metric=float_RArray::HAMMING,
				bool norm=false, float tol=0.0f);
  // compute comparative distance array, one pattern against this environment
  virtual void	ValOverEventsArray(float_RArray& ary, int pat_no, int val_no);
  // extract an array containing values for given value index in pattern pat_no across events
  virtual void	CorrelMatrix(float_RArray& mat, int pat_no, int& dim);
  // generate a correlation matrix for all patterns in pat_no in the environment (e.g., correlation of unit 1 with all other units across patterns, etc); dim = dimensionality of correl matrix = no. of vals in pattern
  virtual void	PCAEigens(float_RArray& evecs, float_RArray& evals, int pat_no, int& dim);
  // get principal components analysis eigenvectors and eigenvalues of correlation matrix across events for pattern pat_no (dim = dimensionality of correl matrix = no. of vals in pattern)
  virtual void	ProjectPatterns(const float_RArray& prjn_vector, float_RArray& vals, int pat_no);
  // project patterns in pat_no onto prjn_vector (dot product), and store resulting array of vals in vals (length = no. of events)
  virtual void	PatFreqArray(float_RArray& freqs, int pat_no, float act_thresh = .5f, bool proportion = false);
  // get frequency (proportion) of pattern activations greater than act_thresh across events
  virtual void	PatFreqText(float act_thresh = .5f, bool proportion = false, ostream& strm = cerr);
  // #MENU #ARGC_2 report frequency (proportion) of pattern values greater than act_thresh across events, to a text output (most useful if pattern values are named in value_names)
  virtual void	PatAggArray(float_RArray& agg_vals, int pat_no, Aggregate& agg);
  // aggregate pattern pat_no values over events to given array object
  virtual void	PatAggText(Aggregate& agg, ostream& strm = cerr);
  // #MENU #ARGC_1 aggregate patterns over events and print aggregated results to a text output (most useful if pattern values are named in value_names)
  virtual void 	EventFreqText(bool proportion = false, ostream& strm = cerr);
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
  virtual void	EventPrjnPlot(Event* x_axis_event, Event* y_axis_event, int pat_no,
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
  virtual NetConduit*	GetAnNetConduit();
  // returns either the default event spec if it exists, or makes one

//obs  TypeDef*	GetDefaultView()	{ return &TA_EnviroView; }
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_NetConduit); }
#endif
  void	UpdateAfterEdit();

  void	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void 	Copy(const Environment& cp);
  TA_BASEFUNS(Environment);
};

*/

#endif // enviro_h
