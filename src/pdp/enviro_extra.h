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



// enviro_extra.h
// extra enviro objects derived from basic ones that are included in standard
// pdp library


#ifndef enviro_extra_h
#define enviro_extra_h 1

#include "enviro.h"
#include "ta_script.h"

//////////////////////////
//       Script 	//
//////////////////////////

class ScriptEnv : public Environment, public ScriptBase {
  // For algorithmically generated events: Initialization of events is done by a script at the start of each epoch through the InitEvents() function
public:
  SArg_Array	s_args;		// string-valued arguments to pass to script

  void		InitEvents();	// resets event_ctr to 0 and then calls the script to generate events for the epoch

  TypeDef*	GetThisTypeDef()	{ return GetTypeDef(); }
  void*		GetThisPtr()		{ return (void*)this; }

  virtual void	Interact();
  // #BUTTON change to this shell in script (terminal) window to interact, debug etc script
  virtual void	Compile();
  // #BUTTON compile script from script file into internal runnable format

  void	UpdateAfterEdit();

  void 	Initialize()	{ };
  void 	Destroy()	{ };
  void	InitLinks();
  void	Copy_(const ScriptEnv& cp);
  COPY_FUNS(ScriptEnv, Environment);
  TA_BASEFUNS(ScriptEnv);
};

class InteractiveScriptEnv : public ScriptEnv {
  // For interactively-generated environments: Script is called for each event in GetNextEvent function (use with InteractiveEpoch)
public:
  Event*	next_event;	// #HIDDEN #NO_SAVE script must set this to point to the next event to present (or NULL to signal end of epoch)

  void		InitEvents();	// just resets event_ctr and does not call the script (event_ctr = 0 marks start of epoch)
  Event*	GetNextEvent();	// calls the script, and then returns next_event, auto increments event_ctr (if no script attached, just functions as a standard Environment)

  void 	Initialize();
  void 	Destroy()	{ };
  void	Copy_(const InteractiveScriptEnv& cp);
  COPY_FUNS(InteractiveScriptEnv, ScriptEnv);
  TA_BASEFUNS(InteractiveScriptEnv); //
  
};


//////////////////////////
//      Frequency 	//
//////////////////////////

class FreqEvent : public Event {
  // an event that has a frequency associated with it
public:
  float 	frequency;	// #ENVIROVIEW_freq frequency of occurance for this event

  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const FreqEvent& cp);
  COPY_FUNS(FreqEvent, Event);
  TA_BASEFUNS(FreqEvent);
};

class FreqEnv;

class FreqEvent_Group : public Event_MGroup {
  // an event group that has a frequency associated with it
public:
  FreqEnv*	fenv;		// #READ_ONLY #NO_SAVE parent frequency environment
  int_Array	list;		// #HIDDEN list of event indicies to present for GROUP_EVENT
  float		frequency;	// frequency of occurance for this group of events

  void		InitEvents(Environment* env);	// used for GROUP_EVENT frequencies
  int	 	EventCount();
  Event* 	GetEvent(int i);

  void	Initialize();
  void 	Destroy()		{ CutLinks();};
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const FreqEvent_Group& cp);
  COPY_FUNS(FreqEvent_Group, Event_MGroup);
  TA_BASEFUNS(FreqEvent_Group);
};


class FreqEnv : public Environment {
  // environment which has a frequency for each event
public:
  enum FreqLevel {
    NO_FREQ,			// don't use frequency
    EVENT,			// use frequency at the event level
    GROUP,			// use frequency at the group level
    GROUP_EVENT 		// frequency at both group and event levels
  };

  enum SampleType {		// type of frequency sampling to use
    RANDOM,			// random sampling (n_samples at freq probability)
    PERMUTED 			// permuted (n_sample * freq evts per epoch)
  };

  int_Array	list;		// #HIDDEN list of event/group indicies to present
  FreqLevel	freq_level;	// level at which to use the frequency information
  int		n_sample;	// #CONDEDIT_OFF_freq_level:NO_FREQ number samples of the events to make per epoch
  SampleType	sample_type;	// #CONDEDIT_OFF_freq_level:NO_FREQ type of sampling (random with freq or permuted n_samples * freq)

  void		InitEvents();

  // the event model of the environment
  int	 	EventCount();
  Event* 	GetEvent(int i);

  // the group model of the environment
  int	 	GroupCount();		// number of groups in the environment
  Event_MGroup* GetGroup(int i); 	// return the ith event group

  Event*	GetNextEvent();

  void	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  void	Copy_(const FreqEnv& cp);
  COPY_FUNS(FreqEnv, Environment);
  TA_BASEFUNS(FreqEnv);
};


//////////////////////////
//         Time 	//
//////////////////////////

class TimeEvent : public Event {
  // an event which occurs at a specific time
public:
  float		time;		// #ENVIROVIEW time at which it should appear

  void 	Initialize();
  void	Destroy()		{ };
  void	Copy_(const TimeEvent& cp);
  COPY_FUNS(TimeEvent, Event);
  TA_BASEFUNS(TimeEvent);
};

class TimeEvent_MGroup : public Event_MGroup {
  // a group of time-based events
public:
  enum Interpolate {
    PUNCTATE,			// events appear for a single instant only
    CONSTANT,			// events persist constantly, change discretely
    LINEAR,			// linear interpolation is performed between events
    USE_ENVIRO 			// use interpolation specified in the environment
  };

  Interpolate	interpolate;	// if and how to interpolate between given event times
  float		end_time;	// time this sequence ends at

  virtual TimeEvent*	GetTimeEvent(float time);

  virtual void	RegularlySpacedTimes(float start_time, float increment);
  // #MENU fill in the time values of the events in the group with regularly spaced times

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void	Copy_(const TimeEvent_MGroup& cp);
  COPY_FUNS(TimeEvent_MGroup, Event_MGroup);
  TA_BASEFUNS(TimeEvent_MGroup);
};

class TimeEnvironment : public Environment {
  // an environment that manages time-based events
public:
  enum Interpolate {
    PUNCTATE,			// events appear for a single instant only
    CONSTANT,			// events persist constantly, change discretely
    LINEAR 			// linear interpolation is performed between events
  };

  Interpolate	interpolate;    // if and how to interpolate between given event times

  void 	Initialize();
  void	Destroy()		{ };
  void 	InitLinks();
  void	Copy_(const TimeEnvironment& cp);
  COPY_FUNS(TimeEnvironment, Environment);
  TA_BASEFUNS(TimeEnvironment);
};

//////////////////////////
//     FreqTime 	//
//////////////////////////

class FreqTimeEvent : public TimeEvent {
  // a time event that has a frequency associated with it
public:
  float 	frequency;	// #ENVIROVIEW_freq frequency of occurance for this event

  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const FreqTimeEvent& cp);
  COPY_FUNS(FreqTimeEvent, TimeEvent);
  TA_BASEFUNS(FreqTimeEvent);
};

class FreqTimeEvent_Group : public TimeEvent_MGroup {
  // a time event group that has a frequency associated with it
public:
  float		frequency;	// frequency of occurance for this group of events

  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const FreqTimeEvent_Group& cp);
  COPY_FUNS(FreqTimeEvent_Group, TimeEvent_MGroup);
  TA_BASEFUNS(FreqTimeEvent_Group);
};


class FreqTimeEnv : public TimeEnvironment {
  // a time environment which has a frequency for each event
public:
  enum FreqLevel {
    NO_FREQ,			// don't use frequency
    EVENT,			// use frequency at the event level
    GROUP 			// use frequency at the group level
  };

  enum SampleType {		// type of frequency sampling to use
    RANDOM,			// random sampling (n_samples at freq probability)
    PERMUTED 			// permuted (n_sample * freq evts per epoch)
  };

  int_Array	list;		// #HIDDEN list of event/group indicies to present
  FreqLevel	freq_level;	// level at which to use the frequency information
  int		n_sample;	// #CONDEDIT_OFF_freq_level:NO_FREQ number samples of the events to make per epoch
  SampleType	sample_type;	// #CONDEDIT_OFF_freq_level:NO_FREQ type of sampling (random with freq or permuted n_samples * freq)

  void		InitEvents();

  // the event model of the environment
  int	 	EventCount();
  Event* 	GetEvent(int i);

  // the group model of the environment
  int	 	GroupCount();		// number of groups in the environment
  Event_MGroup* GetGroup(int i); 	// return the ith event group

  Event*	GetNextEvent();

  void	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  void	Copy_(const FreqTimeEnv& cp);
  COPY_FUNS(FreqTimeEnv, TimeEnvironment);
  TA_BASEFUNS(FreqTimeEnv);
};


//////////////////////////
//     Probability 	//
//////////////////////////

class ProbPattern : public Pattern {
  // pattern is chosen from group of patterns with given probability
public:
  float    	prob;		// #ENVIROVIEW probability of showing this pattern
  bool		applied;	// #READ_ONLY #NO_SAVE whether it was applied

  void 	Initialize();
  void	Destroy()	{ };
  void	InitLinks();
  void 	Copy_(const ProbPattern& cp);
  COPY_FUNS(ProbPattern, Pattern);
  TA_BASEFUNS(ProbPattern);
};

class ProbPatternSpec_Group : public PatternSpec_Group {
  // defines a group of patterns that are chosen according to their probabilities
public:

  int 	last_pat; 	 // #HIDDEN #NO_SAVE last pattern chosen

  void UpdateAfterEdit();
  void CutLinks();
  void Initialize();
  void Destroy()	{ };
  TA_BASEFUNS(ProbPatternSpec_Group);
};

class ProbEventSpec : public EventSpec {
  // events have probabalistically-chosen patterns contained in ProbPatternSpec_Groups
public:
  float		default_prob;	// default probability

  void 		ApplyPatterns(Event* ev, Network* net);
  void		ApplySamePats(Event* ev, Network* net);

  void	Initialize();
  void	Destroy()	{ };
  TA_BASEFUNS(ProbEventSpec);
};

//////////////////////////
//     XY Offset	//
//////////////////////////

class XYPatternSpec : public PatternSpec {
  // for patterns that are positioned at a particular x,y offset location
public:
  bool		wrap;
  // whether to wrap around target layer if pattern extends beyond coords
  bool		apply_background;
  // whether to give all units not in the pattern a background value
  float		background_value;
  // value to apply to all other units in the layer (if applied)

  void	ApplyPattern(Pattern* pat);

  int	WrapClip(int coord, int max_coord);  // implements wrap-around (or not)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(XYPatternSpec);
  COPY_FUNS(XYPatternSpec, PatternSpec);
  TA_BASEFUNS(XYPatternSpec);
};

class XYPattern : public Pattern {
  // specifies the x,y offset location of the pattern in the layer
public:
  TwoDCoord	offset;		// #ENVIROVIEW offset within network layer for pattern

  void	Initialize()	{ };
  void	Destroy()	{ };
  SIMPLE_COPY(XYPattern);
  COPY_FUNS(XYPattern, Pattern);
  TA_BASEFUNS(XYPattern);
};


//////////////////////////
//     XY Subset	//
//////////////////////////

class XYSubPatternSpec : public PatternSpec {
  // presents rectagular subsets (size of layer) of large patterns at x,y offset
public:
  bool		wrap;
  // whether to wrap around pattern if layer extends beyond coords

  void	ApplyPattern(Pattern* pat);

  int	WrapClip(int coord, int max_coord);  // implements wrap-around (or not)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(XYSubPatternSpec);
  COPY_FUNS(XYSubPatternSpec, PatternSpec);
  TA_BASEFUNS(XYSubPatternSpec);
};

class XYSubPattern : public Pattern {
  // specifies the x,y offset location of the layer within the pattern
public:
  TwoDCoord	offset;		// #ENVIROVIEW offset within pattern for network layer

  void	Initialize()	{ };
  void	Destroy()	{ };
  SIMPLE_COPY(XYSubPattern);
  COPY_FUNS(XYSubPattern, Pattern);
  TA_BASEFUNS(XYSubPattern);
};


//////////////////////////
//     GroupPattern	//
//////////////////////////

class GroupPatternSpec : public PatternSpec {
  // organizes pattern values into sub-groups for viewing and/or sending to network
public:
  PosTDCoord	sub_geom;
  // geometry of the individual sub-groups: must evenly divide into overall geom in both x & y
  PosTDCoord	gp_geom;
  // #READ_ONLY geometry of the groups within overall geom (just geom / sub_geom)
  bool		trans_apply;
  // translate apply of values to network (only if units are flat, not grouped!)

  int	FlatToValueIdx(int index);
  // translate given index from a flat view into the value index taking into account groups
  int	CoordToValueIdx(const TwoDCoord& gp_coord, const TwoDCoord& sub_coord);
  // get index into actual values from given coordinates of group and sub-group
  int	ValueToFlatIdx(int index);
  // translate given index of a value into a flat view taking into account groups
  int	CoordToFlatIdx(const TwoDCoord& gp_coord, const TwoDCoord& sub_coord);
  // get index into flat structure from given coordinates of group and sub-group

  float Value(Pattern* pat, int index);
  int	Flag(PatUseFlags flag_type, Pattern* pat, int index);
  void 	ApplyValue(Pattern* pat, Unit* uni, int index);

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(GroupPatternSpec);
  COPY_FUNS(GroupPatternSpec, PatternSpec);
  TA_BASEFUNS(GroupPatternSpec);
};

//////////////////////////////////////////
// 	Duration  Events		//
//////////////////////////////////////////

class DurEvent : public Event {
  // an event which lasts for a particular amount of time
public:
  float		duration;	// #ENVIROVIEW length of time (cycles) event should be presented

  void 	Initialize();
  void	Destroy()		{ };
  void	Copy_(const DurEvent& cp);
  COPY_FUNS(DurEvent, Event);
  TA_BASEFUNS(DurEvent);
};

//////////////////////////////////////////
// 	Read from File  		//
//////////////////////////////////////////

class FromFileEnv : public Environment {
  // Environment that reads events incrementally from a file into events.
public:
  enum 	ReadMode {
    ONE_EPOCH,			// read one epoch at a time, using InitEvents interface
    ONE_EVENT			// read one event at a time, using GetNextEvent interface (requires InteractiveEpoch process)
  };

  ReadMode	read_mode;	// how to read in events: either one epoch or one event at a time (one event requires InteractiveEpoch process)
  taFiler*	event_file;	// file to read events from
  TextFmt	text_file_fmt;	// format of text file
  bool		binary;		// file is binary (written by WriteBinary). Otherwise, its Text as readable by ReadText
  int		events_per_epc;	// how many events to present per epoch
  int		file_pos;	// #READ_ONLY #SHOW #NO_SAVE position (in terms of events) within the file

  virtual void	ReadEvent(Event* ev);
  // read from file into one event (assumes file is open, etc)

  void		InitEvents();
  Event* 	GetEvent(int ev_index);
  Event* 	GetNextEvent();

  void 	Initialize();
  void	Destroy();
  void 	InitLinks();
  void	Copy_(const FromFileEnv& cp);
  COPY_FUNS(FromFileEnv, Environment);
  TA_BASEFUNS(FromFileEnv);
};


#endif // enviro_extra_h
