/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// enviro_extra.cc


#include "enviro_extra.h"
#include "netstru.h"
#include "pdpshell.h"
#include "ta_filer.h"
//nn #include "tarandom.h"
#include "css_machine.h"


//////////////////////////
//       Script 	//
//////////////////////////

void ScriptEnv::InitLinks() {
  Environment::InitLinks();
  taBase::Own(s_args, this);
  if(script_file->fname.empty())	// initialize only on startup up, not transfer
    SetScript("");
}

void ScriptEnv::Copy_(const ScriptEnv& cp) {
  s_args = cp.s_args;
  *script_file = *(cp.script_file);
  script_string = cp.script_string;
}

void ScriptEnv::InitEvents() {
  Environment::InitEvents();
  RunScript(); // called at beginning of epoch
}

void ScriptEnv::UpdateAfterEdit() {
  Environment::UpdateAfterEdit();
  UpdateReCompile();
}

void ScriptEnv::Interact() {
  if(script == NULL)   return;
  cssMisc::next_shell = script;
}

void ScriptEnv::Compile() {
  LoadScript();
}

//////////////

void InteractiveScriptEnv::Initialize() {
  next_event = NULL;
}

void InteractiveScriptEnv::Copy_(const InteractiveScriptEnv& cp) {
  taBase::SetPointer((TAPtr*)&next_event, cp.next_event);
}

void InteractiveScriptEnv::InitEvents() {
  Environment::InitEvents();
}

Event* InteractiveScriptEnv::GetNextEvent() {
  static bool already_running = false;
  if(!HasScript())
    return Environment::GetNextEvent();
  if(already_running) return next_event; // this is needed in case the script triggers an UpdateAfterEdit that calls itself!
  already_running = true;
  RunScript();
  already_running = false;
  event_ctr++;			// always keep track of this
  return next_event;
}


//////////////////////////
//      Frequency 	//
//////////////////////////

void FreqEvent::Initialize() {
  frequency = 1.0f;
}

void FreqEvent::Copy_(const FreqEvent& cp) {
  frequency = cp.frequency;
}

void FreqEvent_Group::Initialize() {
  frequency = 1.0f;
  fenv = NULL;
}

void FreqEvent_Group::InitLinks() {
  Event_MGroup::InitLinks();
  taBase::Own(list, this);
  fenv = GET_MY_OWNER(FreqEnv);
}

void FreqEvent_Group::CutLinks() {
  fenv = NULL;
  Event_MGroup::CutLinks();
}

void FreqEvent_Group::Copy_(const FreqEvent_Group& cp) {
  frequency = cp.frequency;
  list = cp.list;
}

void FreqEvent_Group::InitEvents(Environment*) {
  list.Reset();
  if(fenv == NULL)	return;
  int cnt = 0;
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, this->, i) {
    if(ev->InheritsFrom(TA_FreqEvent)) {
      float freq = ((FreqEvent*)ev)->frequency;
      if(fenv->sample_type == FreqEnv::RANDOM) {
	int j;
	for(j=0; j<fenv->n_sample; j++) {
	  if(Random::ZeroOne() <= freq)
	    list.Add(cnt);
	}
      }
      else {			// permuted, add that many to the list..
	int num_ev = (int)(freq * (float)fenv->n_sample);
	int j;
	for(j=0; j<num_ev; j++)
	  list.Add(cnt);
      }
    }
    else {
      // otherwise frequency is assumed to be 1
      list.Add(cnt);
    }
    cnt++;
  }
}

int FreqEvent_Group::EventCount() {
  if((fenv == NULL) || (fenv->freq_level != FreqEnv::GROUP_EVENT))
    return Event_MGroup::EventCount();
  return list.size;
}

Event* FreqEvent_Group::GetEvent(int i) {
  if((fenv == NULL) || (fenv->freq_level != FreqEnv::GROUP_EVENT))
    return Event_MGroup::GetEvent(i);
  return (Event*)Leaf(list.SafeEl(i));
}

void FreqEnv::Initialize() {
  n_sample = 1;
  freq_level = GROUP_EVENT;
  sample_type = PERMUTED;
  events.SetBaseType(&TA_FreqEvent);
  events.gp.SetBaseType(&TA_FreqEvent_Group);
}

void FreqEnv::InitLinks() {
  Environment::InitLinks();
  taBase::Own(list, this);
  events.gp.SetBaseType(&TA_FreqEvent_Group);
}

void FreqEnv::Copy_(const FreqEnv& cp) {
  n_sample = cp.n_sample;
  freq_level = cp.freq_level;
  sample_type = cp.sample_type;
  list = cp.list;
}

void FreqEnv::InitEvents() {
  Environment::InitEvents();
  list.Reset();
  if(freq_level == EVENT) {
    int cnt = 0;
    taLeafItr i;
    Event* ev;
    FOR_ITR_EL(Event, ev, events., i) {
      if(ev->InheritsFrom(TA_FreqEvent)) {
	float freq = ((FreqEvent*)ev)->frequency;
	if(sample_type == RANDOM) {
	  int j;
	  for(j=0; j<n_sample; j++) {
	    if(Random::ZeroOne() <= freq)
	      list.Add(cnt);
	  }
	}
	else {			// permuted, add that many to the list..
	  int num_ev = (int)(freq * (float)n_sample);
	  int j;
	  for(j=0; j<num_ev; j++)
	    list.Add(cnt);
	}
      }
      else {
	// otherwise frequency is assumed to be 1
	list.Add(cnt);
      }
      cnt++;
    }
  }
  else if((freq_level == GROUP) || (freq_level == GROUP_EVENT)) {
    int i;
    if(events.leaf_gp == NULL) events.InitLeafGp();
    for(i=0; i<events.leaf_gp->size; i++) {
      Event_MGroup* evg = (Event_MGroup*)events.leaf_gp->FastEl(i);
      if(evg->InheritsFrom(TA_FreqEvent_Group)) {
	FreqEvent_Group* fevg = (FreqEvent_Group*)evg;
	float freq = fevg->frequency;
	if(sample_type == RANDOM) {
	  int j;
	  for(j=0; j<n_sample; j++) {
	    if(Random::ZeroOne() <= freq)
	      list.Add(i);
	  }
	}
	else {			// permuted, add that many to the list..
	  int num_ev = (int)(freq * (float)n_sample);
	  int j;
	  for(j=0; j<num_ev; j++)
	    list.Add(i);
	}
	if(freq_level == GROUP_EVENT)
	  fevg->InitEvents(this);	// initialize the event listing within group
      }
      else {
	// otherwise frequency is assumed to be 1
	list.Add(i);
      }
    }
  }
}

int FreqEnv::EventCount() {
  if(freq_level == EVENT) {
    return list.size;
  }
  return Environment::EventCount();
}

Event* FreqEnv::GetEvent(int i) {
  if(freq_level == EVENT) {
    return (Event*)events.Leaf(list.SafeEl(i));
  }
  return Environment::GetEvent(i);
}

int FreqEnv::GroupCount() {
  if((freq_level == GROUP) || (freq_level == GROUP_EVENT)) {
    return list.size;
  }
  return Environment::GroupCount();
}

Event_MGroup* FreqEnv::GetGroup(int i) {
  if((freq_level == GROUP) || (freq_level == GROUP_EVENT)) {
    if(events.leaf_gp == NULL) events.InitLeafGp();
    return (Event_MGroup*)events.leaf_gp->SafeEl(list.SafeEl(i));
  }
  return Environment::GetGroup(i);
}

Event* FreqEnv::GetNextEvent() {
  if(freq_level == EVENT) {
    if(list.size > event_ctr)
      return (Event*)events.Leaf(list.SafeEl(event_ctr++));
    return NULL;
  }
  return Environment::GetNextEvent();
}

//////////////////////////
//         Time 	//
//////////////////////////

void TimeEvent::Initialize() {
  time = 0.0f;
}

void TimeEvent::Copy_(const TimeEvent& cp) {
  time = cp.time;
}

void TimeEvent_MGroup::Initialize() {
  end_time = 1.0f;
  interpolate = USE_ENVIRO;
  SetBaseType(&TA_TimeEvent);
}

void TimeEvent_MGroup::Copy_(const TimeEvent_MGroup& cp) {
  interpolate = cp.interpolate;
  end_time = cp.end_time;
}

void TimeEvent_MGroup::UpdateAfterEdit() {
  Event_MGroup::UpdateAfterEdit();
  if(leaves == 0)
    return;
  TimeEvent* ev = (TimeEvent*)Leaf(leaves-1);
  if(!ev->InheritsFrom(TA_TimeEvent))
    return;
  if(end_time < ev->time)	// make sure end_time is big enough
    end_time = ev->time;
}

void TimeEvent_MGroup::RegularlySpacedTimes(float start_time, float increment) {
  float tim = start_time;
  TimeEvent* ev;
  taLeafItr i;
  FOR_ITR_EL(TimeEvent, ev, this->, i) {
    ev->time = tim;
    tim += increment;
  }
}

TimeEvent* TimeEvent_MGroup::GetTimeEvent(float time) {
  Interpolate intrp = interpolate;
  if(interpolate == USE_ENVIRO) {
    TimeEnvironment* env = GET_MY_OWNER(TimeEnvironment);
    if(env != NULL)
      intrp = (Interpolate)env->interpolate;
  }
  TimeEvent* last_ev = NULL;
  TimeEvent* ev;
  taLeafItr i;
  FOR_ITR_EL(TimeEvent, ev, this->, i) {
    if(ev->time < time) {
      last_ev = ev;
      continue;
    }
    break;
  }
  if(intrp == PUNCTATE) {
    if((ev != NULL) && (fabsf(ev->time - time) < .00001f)) // allow for roundoff
      return ev;
    if((last_ev != NULL) && (fabsf(last_ev->time - time) < .00001f)) // allow for roundoff
      return last_ev;
    return NULL;
  }
  if(intrp == CONSTANT) {
    if((ev != NULL) && (fabsf(ev->time - time) < .00001f)) // allow for roundoff
      return ev;
    return last_ev;
  }
  if(intrp == LINEAR) {
    if(ev == NULL)					return last_ev;
    if(ev->time < 0.0f)					return NULL;
    if((last_ev == NULL) || (last_ev->time < 0.0f))     return ev;
    if(ev->time < time)					return ev;
    TimeEvent* rval = (TimeEvent*)FastEl(0); // store dummy event in 1st position
    if(rval->time >= 0.0f) {
      rval = (TimeEvent*)rval->Clone();	// make one of the same type..
      Insert(rval, 0);
    }
    ev->spec->NewEvent(rval);	// always set up for current spec
    rval->time = -1;		// make sure it doesn't get run..
    float span = ev->time - last_ev->time;
    float dist = time - last_ev->time;
    float ratio = dist / span;	// how far to go towards new value (ev) from old (last_ev)
    rval->name = last_ev->name + "+" + (String)ratio;
    int pn;
    for(pn = 0; pn < ev->patterns.size; pn++) {
      Pattern* ev_pat = (Pattern*)ev->patterns.Leaf(pn);
      Pattern* last_ev_pat = (Pattern*)last_ev->patterns.Leaf(pn);
      Pattern* nw_pat = (Pattern*)rval->patterns.Leaf(pn);

      int mx_val = MIN(ev_pat->value.size, nw_pat->value.size);
      mx_val = MIN(mx_val, last_ev_pat->value.size);
      int j;
      for(j=0; j<mx_val; j++) {
	nw_pat->value.FastEl(j) = last_ev_pat->value.FastEl(j) +
	  ratio * (ev_pat->value.FastEl(j) - last_ev_pat->value.FastEl(j));
      }
    }
    return rval;
  }
  return NULL;
}

void TimeEnvironment::Initialize() {
  events.SetBaseType(&TA_TimeEvent);
  events.gp.SetBaseType(&TA_TimeEvent_MGroup);
  interpolate = CONSTANT;
}

void TimeEnvironment::InitLinks() {
  Environment::InitLinks();
  events.gp.SetBaseType(&TA_TimeEvent_MGroup);
}

void TimeEnvironment::Copy_(const TimeEnvironment& cp) {
  interpolate = cp.interpolate;
}


//////////////////////////
//      FreqTime 	//
//////////////////////////

void FreqTimeEvent::Initialize() {
  frequency = 1.0f;
}

void FreqTimeEvent::Copy_(const FreqTimeEvent& cp) {
  frequency = cp.frequency;
}

void FreqTimeEvent_Group::Initialize() {
  frequency = 1.0f;
}

void FreqTimeEvent_Group::Copy_(const FreqTimeEvent_Group& cp) {
  frequency = cp.frequency;
}

void FreqTimeEnv::Initialize() {
  n_sample = 1;
  freq_level = GROUP;		// group is typically used with time
  sample_type = RANDOM;
  events.SetBaseType(&TA_FreqTimeEvent);
  events.gp.SetBaseType(&TA_FreqTimeEvent_Group);
}

void FreqTimeEnv::InitLinks() {
  TimeEnvironment::InitLinks();
  taBase::Own(list, this);
  events.gp.SetBaseType(&TA_FreqTimeEvent_Group);
}

void FreqTimeEnv::Copy_(const FreqTimeEnv& cp) {
  n_sample = cp.n_sample;
  freq_level = cp.freq_level;
  sample_type = cp.sample_type;
  list = cp.list;
}

void FreqTimeEnv::InitEvents() {
  Environment::InitEvents();
  list.Reset();
  if(freq_level == EVENT) {
    int cnt = 0;
    taLeafItr i;
    Event* ev;
    FOR_ITR_EL(Event, ev, events., i) {
      if(ev->InheritsFrom(TA_FreqTimeEvent)) {
	float freq = ((FreqTimeEvent*)ev)->frequency;
	if(sample_type == RANDOM) {
	  int j;
	  for(j=0; j<n_sample; j++) {
	    if(Random::ZeroOne() <= freq)
	      list.Add(cnt);
	  }
	}
	else {			// permuted, add that many to the list..
	  int num_ev = (int)(freq * (float)n_sample);
	  int j;
	  for(j=0; j<num_ev; j++)
	    list.Add(cnt);
	}
      }
      else {
	// otherwise frequency is assumed to be 1
	list.Add(cnt);
      }
      cnt++;
    }
  }
  else if(freq_level == GROUP) {
    int i;
    if(events.leaf_gp == NULL) events.InitLeafGp();
    for(i=0; i<events.leaf_gp->size; i++) {
      Event_MGroup* evg = (Event_MGroup*)events.leaf_gp->FastEl(i);
      if(evg->InheritsFrom(TA_FreqTimeEvent_Group)) {
	float freq = ((FreqTimeEvent_Group*)evg)->frequency;
	if(sample_type == RANDOM) {
	  int j;
	  for(j=0; j<n_sample; j++) {
	    if(Random::ZeroOne() <= freq)
	      list.Add(i);
	  }
	}
	else {			// permuted, add that many to the list..
	  int num_ev = (int)(freq * (float)n_sample);
	  int j;
	  for(j=0; j<num_ev; j++)
	    list.Add(i);
	}
      }
      else {
	// otherwise frequency is assumed to be 1
	list.Add(i);
      }
    }
  }
}

int FreqTimeEnv::EventCount() {
  if(freq_level == EVENT) {
    return list.size;
  }
  return TimeEnvironment::EventCount();
}

Event* FreqTimeEnv::GetEvent(int i) {
  if(freq_level == EVENT) {
    return (Event*)events.Leaf(list.SafeEl(i));
  }
  return TimeEnvironment::GetEvent(i);
}

int FreqTimeEnv::GroupCount() {
  if(freq_level == GROUP) {
    return list.size;
  }
  return TimeEnvironment::GroupCount();
}

Event_MGroup* FreqTimeEnv::GetGroup(int i) {
  if(freq_level == GROUP) {
    if(events.leaf_gp == NULL) events.InitLeafGp();
    return (Event_MGroup*)events.leaf_gp->SafeEl(list.SafeEl(i));
  }
  return TimeEnvironment::GetGroup(i);
}

Event* FreqTimeEnv::GetNextEvent() {
  if(freq_level == EVENT) {
    if(list.size > event_ctr)
      return (Event*)events.Leaf(list.SafeEl(event_ctr++));
    return NULL;
  }
  return Environment::GetNextEvent();
}

//////////////////////////
//     Probability 	//
//////////////////////////

void ProbPattern::Initialize() {
  prob = 1.0f;
  applied = false;
}

void ProbPattern::Copy_(const ProbPattern& cp) {
  prob = cp.prob;
}

void ProbPattern::InitLinks() {
  Pattern::InitLinks();
  Event* e = GET_MY_OWNER(Event);
  if((e != NULL) && (e->spec.spec != NULL) &&
     (e->spec.spec->InheritsFrom(TA_ProbEventSpec))) {
    prob = ((ProbEventSpec*)e->spec.spec)->default_prob;
  }
}

void ProbPatternSpec_Group::UpdateAfterEdit() {
  PatternSpec_Group::UpdateAfterEdit();

  last_pat = -1;
}

void ProbPatternSpec_Group::CutLinks() {
  PatternSpec_Group::CutLinks();

  last_pat = -1;
}

void ProbPatternSpec_Group::Initialize() {
  last_pat = -1;
}

void ProbEventSpec::Initialize() {
  patterns.gp.el_typ = &TA_ProbPatternSpec_Group;
  default_prob = .25f;
}

// distribution group = group of patterns in a ProbPatternSpec_Group
// that, by virtue of their probabilities, form a distribution of patterns
// that could be seen

/* this function loops through the groups of patterns in an event, applying
   each of them to the layer specified in the pattern's corresponding spec.
   Whenever a ProbPatternSpec_Group is encountered, however, it only applies
   _one_ of the patterns, based on that pattern's assigned probability.
   Also, for any pattern not in a distribution group but which still has
   an associated probability, it is presented or not based on that single
   probability.  Also, it is assumed that the group of patterns on the event
   match with the group of patternspecs on the eventspec. */

void ProbEventSpec::ApplyPatterns(Event* ev, Network* net) {
  if(net == NULL) {
    taMisc::Error("Event:",ev->GetPath(), " cannot apply patterns since network is NULL.",
		   "Try reseting the network pointer in your process.");
    return;
  }

  if(net != last_net)
    SetLayers(net);

  PatternSpec_Group* psgroup;
  Pattern_Group* pgroup;
  int g,h;
  // this loops through both the pattern groups and the pattern spec groups
  // at once.  groups are handled depending on whether they are distributions or not.
  for(pgroup =  (Pattern_Group*)ev->patterns.FirstGp(g),
       psgroup = (PatternSpec_Group*)patterns.FirstGp(h);
       pgroup && psgroup;
      pgroup = (Pattern_Group*)ev->patterns.NextGp(g),
      psgroup = (PatternSpec_Group*)patterns.NextGp(h))
  {
    if(psgroup->InheritsFrom(&TA_ProbPatternSpec_Group)) { //is it a dist?
      ProbPatternSpec_Group* dpsgroup = (ProbPatternSpec_Group*)psgroup;
      float rand_prob = Random::ZeroOne();  //the random number
      float total_prob = 0;                 //the running total for the group

      // cycle through the patterns in the group:  each pattern's prob
      // is treated as a range starting from the previous pattern's prob sum.
      // this loop picks a single pattern to be applied.
      int i;
      for(i = 0; (i < psgroup->size) && (i < pgroup->size); i++) {
	ProbPattern* pat = (ProbPattern*) pgroup->FastEl(i);
	PatternSpec* ps = (PatternSpec*) psgroup->FastEl(i);
	if(rand_prob < (total_prob + pat->prob)) {
	  if(ps->layer == NULL)
	    continue;

	  ps->ApplyPattern(pat);

	  dpsgroup->last_pat = i;
	  break;            //stop after first pat qualifies
	}
	else {
	  total_prob += pat->prob;
	}
      }
    }
    else {
      // if the group isn't a distribution group, then apply each pattern, and
      // if the pattern has a prob, treat it accordingly.
      int i;
      for(i = 0; (i < psgroup->size) && (i < pgroup->size); i++) {
	Pattern* pat = (Pattern*)pgroup->FastEl(i);
	PatternSpec* ps = (PatternSpec*)psgroup->FastEl(i);
	bool is_prob_pat = pat->InheritsFrom(&TA_ProbPattern);
	ProbPattern* prob_pat = NULL;
	if(is_prob_pat)
	  prob_pat = (ProbPattern*)pat;

	float rand_prob = Random::ZeroOne();

	if(!prob_pat || (rand_prob < prob_pat->prob))	{
	  if(ps->layer == NULL)
	    continue;

	  ps->ApplyPattern(pat);

	  if(is_prob_pat)
	    prob_pat->applied = true;
	}
	else {
	  if(is_prob_pat)
	    prob_pat->applied = false;
	}
      }
    }
  }
}

// this function applies the same patterns as was most recently applied
void ProbEventSpec::ApplySamePats(Event* ev, Network* net) {
  if(net == NULL) {
    taMisc::Error("Event:",ev->GetPath(), " cannot apply patterns since network is NULL.",
		   "Try reseting the network pointer in your process.");
    return;
  }

  if(net != last_net)
    SetLayers(net);

  PatternSpec_Group* psgroup;
  Pattern_Group* pgroup;
  int g,h;
// this loops through both the pattern groups and the pattern spec groups
// at once.  groups are handled depending on whether they are distributions or not.
  for(pgroup =  (Pattern_Group*)ev->patterns.FirstGp(g),
      psgroup = (PatternSpec_Group*)patterns.FirstGp(h);
      pgroup && psgroup;
      pgroup = (Pattern_Group*)ev->patterns.NextGp(g),
      psgroup = (PatternSpec_Group*)patterns.NextGp(h))
  {
    if(psgroup->InheritsFrom(&TA_ProbPatternSpec_Group)) { //is it a dist?
      ProbPatternSpec_Group* dpsgroup = (ProbPatternSpec_Group*)psgroup;
      if(dpsgroup->last_pat < 0) continue;

      PatternSpec* ps = (PatternSpec *) dpsgroup->SafeEl(dpsgroup->last_pat);
      Pattern* pat = (Pattern *) pgroup->SafeEl(dpsgroup->last_pat);

      if((ps == NULL) || (pat == NULL)) {
	taMisc::Error("last_pat in pattern spec group out of range");
	return;
      }
      ps->ApplyPattern(pat);
    }
    else {
      // if the group isn't a distribution group, then apply each pattern
      // according to whether it was applied before

      for (int i = 0; (i < psgroup->size) && (i < pgroup->size); i++) {
	Pattern* pat = (Pattern *) pgroup->FastEl(i);
	PatternSpec* ps = (PatternSpec *) psgroup->FastEl(i);

	if(!pat->InheritsFrom(&TA_ProbPattern) || ((ProbPattern*)pat)->applied) {
	  ps->ApplyPattern(pat);
	}
      }
    }
  }
}

//////////////////////////
// 	XY Patterns	//
//////////////////////////

void XYPatternSpec::Initialize() {
  pattern_type = &TA_XYPattern;
  wrap = false;
  apply_background = false;
  background_value = 0.0f;
}

int XYPatternSpec::WrapClip(int coord, int max_coord) {
  if(coord >= max_coord) {
    if(wrap)
      coord = coord % max_coord;
    else
      coord = -1;
  }
  else if(coord < 0) {
    if(wrap)
      coord = max_coord + (coord % max_coord);
    else
      coord = -1;
  }
  return coord;
}


void XYPatternSpec::ApplyPattern(Pattern* pat) {
  if(layer == NULL)
    return;
  if(!pat->InheritsFrom(TA_XYPattern)) {
    PatternSpec::ApplyPattern(pat);
    return;
  }
  FlagLayer();
  XYPattern* xypat = (XYPattern*)pat;

  if(apply_background) {
    float old_val = pat->value[0]; // use the zero value for background..
    pat->value[0] = background_value;
    Unit* u;
    taLeafItr j;
    FOR_ITR_EL(Unit, u, layer->units., j)
      ApplyValue(pat,u,0);
    pat->value[0] = old_val;
  }

  int v = 0;
  int py, px;
  for(py = 0; py<geom.y; py++) {
    for(px = 0; px<geom.x; px++, v++) {
      if(v >= pat->value.size)
	return;			// bail..
      int su_x = WrapClip(xypat->offset.x + px, layer->geom.x);
      int su_y = WrapClip(xypat->offset.y + py, layer->geom.y);

      if((su_x < 0) || (su_y < 0))
	continue;

      int su_idx = su_y * layer->geom.x + su_x;
      Unit* u = layer->units.Leaf(su_idx);
      if(u == NULL)
	continue;
      ApplyValue(pat,u,v);
    }
  }
}

//////////////////////////
// 	XY Subset	//
//////////////////////////

void XYSubPatternSpec::Initialize() {
  pattern_type = &TA_XYSubPattern;
  wrap = false;
}

int XYSubPatternSpec::WrapClip(int coord, int max_coord) {
  if(coord >= max_coord) {
    if(wrap)
      coord = coord % max_coord;
    else
      coord = -1;
  }
  else if(coord < 0) {
    if(wrap)
      coord = max_coord + (coord % max_coord);
    else
      coord = -1;
  }
  return coord;
}

void XYSubPatternSpec::ApplyPattern(Pattern* pat) {
  if(layer == NULL)
    return;
  if(!pat->InheritsFrom(TA_XYSubPattern)) {
    PatternSpec::ApplyPattern(pat);
    return;
  }
  FlagLayer();
  XYSubPattern* xysp = (XYSubPattern*)pat;

  int v=0;
  int py, px;
  for(py = 0; py<layer->geom.y; py++) {
    for(px = 0; px<layer->geom.x; px++, v++) {
      int su_x = WrapClip(xysp->offset.x + px, geom.x);
      int su_y = WrapClip(xysp->offset.y + py, geom.y);

      if((su_x < 0) || (su_y < 0))
	continue;

      int su_idx = su_y * geom.x + su_x;
      Unit* u = layer->units.Leaf(v);
      if(u == NULL)
	continue;
      ApplyValue(pat,u,su_idx);
    }
  }
}

//////////////////////////
//     GroupPattern	//
//////////////////////////

void GroupPatternSpec::UpdateAfterEdit() {
  PatternSpec::UpdateAfterEdit();
  sub_geom.SetGtEq(1);		// no dividing by zero!
  TDCoord& gg = gp_geom;
  gg = geom / sub_geom;
}

void GroupPatternSpec::Initialize() {
  sub_geom.SetGtEq(1);
  TDCoord& gg = gp_geom;
  gg = geom / sub_geom;
  trans_apply = false;
}

int GroupPatternSpec::FlatToValueIdx(int index) {
  int mx = gp_geom.x * sub_geom.x;
  int y = index / mx;
  int x = index % mx;
  TwoDCoord gpc, sbc;
  gpc.y = y / sub_geom.y;
  sbc.y = y % sub_geom.y;
  gpc.x = x / sub_geom.x;
  sbc.x = x % sub_geom.x;
  return CoordToValueIdx(gpc, sbc);
}

int GroupPatternSpec::CoordToValueIdx(const TwoDCoord& gpc, const TwoDCoord& sbc) {
  int gpno = gpc.y * gp_geom.x + gpc.x;
  int rval = gpno * (sub_geom.x * sub_geom.y);
  rval += sbc.y * sub_geom.x + sbc.x;
  return rval;
}

int GroupPatternSpec::ValueToFlatIdx(int index) {
  int mx = sub_geom.x * sub_geom.y;
  int gpno = index / mx;
  int sbno = index % mx;
  TwoDCoord gpc, sbc;
  gpc.y = gpno / gp_geom.x;
  gpc.x = gpno % gp_geom.x;
  sbc.y = sbno / sub_geom.x;
  sbc.x = sbno % sub_geom.x;
  return CoordToFlatIdx(gpc, sbc);
}

int GroupPatternSpec::CoordToFlatIdx(const TwoDCoord& gpc, const TwoDCoord& sbc) {
  int x = gpc.x * sub_geom.x + sbc.x;
  int y = gpc.y * sub_geom.y + sbc.y;
  return (y * gp_geom.x * sub_geom.x) + x;
}

float GroupPatternSpec::Value(Pattern* pat, int index) {
  return pat->value.FastEl(FlatToValueIdx(index));
}

int GroupPatternSpec::Flag(PatUseFlags flag_type, Pattern* pat, int index) {
  int idx = FlatToValueIdx(index);
  return PatternSpec::Flag(flag_type, pat, idx);
}

void GroupPatternSpec::ApplyValue(Pattern* pat, Unit* uni, int index) {
  if(!trans_apply) {
    float val = PatternSpec::Value(pat, index) + noise.Gen();
    int flags = PatternSpec::Flag(use_flags, pat, index);
    ApplyValue_impl(uni, val, flags);
    return;
  }
  PatternSpec::ApplyValue(pat, uni, index);
}

//////////////////////////////////
// 	Duration Event		//
//////////////////////////////////

void DurEvent::Initialize() {
  duration = 50.0f;
}

void DurEvent::Copy_(const DurEvent& cp) {
  duration = cp.duration;
}

//////////////////////////////////////////
// 	Read from File  		//
//////////////////////////////////////////


void FromFileEnv::Initialize() {
  if (!event_file) {
    event_file = taFiler_CreateInstance(".","*pat*",false);
    taRefN::Ref(event_file);
    event_file->select_only = true;	// just selecting a file name here
    event_file->mode = taFiler::NO_AUTO;
  }
  read_mode = ONE_EPOCH;
  text_file_fmt = NAME_FIRST;
  binary = false;
  events_per_epc = 100;
  file_pos = 0;
}

void FromFileEnv::Destroy() {
  if (event_file) taRefN::unRefDone(event_file);
  event_file = NULL;
}


void FromFileEnv::InitLinks() {
  Environment::InitLinks();
}

void FromFileEnv::Copy_(const FromFileEnv& cp) {
  if (event_file) *event_file = *(cp.event_file);
  text_file_fmt = cp.text_file_fmt;
  binary = cp.binary;
  events_per_epc = cp.events_per_epc;
}

void FromFileEnv::InitEvents() {
  if(read_mode == ONE_EVENT) {
    Environment::InitEvents();
    return;
  }
  // we probably just did this so we're not going to do this again..
  if(event_file->IsOpen() && (event_ctr <= 1) && (events.size == events_per_epc))
    return;

  Environment::InitEvents();

  if(!event_file->IsOpen()) {
    istream* strm = event_file->Open(event_file->fname, true);
    if(strm == NULL) return;
    file_pos = 0;
  }

  if(events.size != events_per_epc)
    events.EnforceSize(events_per_epc);
  int i;
  for(i=0;i<events_per_epc;i++) {
    Event* ev = (Event*)events[i];
    ReadEvent(ev);
  }
}

Event* FromFileEnv::GetEvent(int ev_index) {
  if(read_mode == ONE_EVENT) {
    taMisc::Error("FromFileEnv: read_mode = ONE_EVENT, requires use of InteractiveEpoch process, not standard Epoch!");
  }
  event_ctr++;
  return Environment::GetEvent(ev_index);
}

Event* FromFileEnv::GetNextEvent() {
  if(read_mode == ONE_EPOCH)
    return Environment::GetNextEvent();
  if(event_ctr >= events_per_epc) {
    event_ctr = 0;
    return NULL;
  }
  if(!event_file->IsOpen()) {
    istream* strm = event_file->Open(event_file->fname, true);
    if(strm == NULL) return NULL;
    file_pos = 0;
  }
  if(events.size == 0)
    events.EnforceSize(1);
  Event* ev = (Event*)events[0];
  ReadEvent(ev);
  event_ctr++;
  return ev;
}

void FromFileEnv::ReadEvent(Event* ev) {
  istream& strm = *(event_file->istrm);
  while(true) {
    if(binary) {
      Pattern* pat;
      taLeafItr pi;
      FOR_ITR_EL(Pattern, pat, ev->patterns., pi) {
	int j;
	for(j=0; j<pat->value.size; j++) {
	  float cur_val;
	  strm.read((char*)&cur_val, sizeof(float));
	  if(strm.eof() || strm.bad()) break;
	  pat->value.Set(j, cur_val);
	}
	if(strm.eof() || strm.bad()) break;
      }
      if(!(strm.eof() || strm.bad())) {
	ev->name = "event_" + String(file_pos);
      }
    }
    else {
      Pattern* pat;
      taLeafItr pi;
      bool first = true;
      FOR_ITR_EL(Pattern, pat, ev->patterns., pi) {
	int j;
	for(j=0; j<pat->value.size; j++) {
	  while(true) {
	    int c = taMisc::read_alnum(strm);
	    if(c == EOF) break;
	    int fc = taMisc::LexBuf.firstchar();
	    // skip comments
	    if((fc == '#') || (taMisc::LexBuf.before(2) == "//")) {
	      if(c != '\n')
		taMisc::read_till_eol(strm);
	      continue; // just a comment
	    }
	    if(first) {
	      if(text_file_fmt == NAME_FIRST) {
		ev->name = taMisc::LexBuf;
		first = false;
		continue;
	      }
	      first = false;
	    }
	    break;
	  }
	  if(strm.eof() || strm.bad()) break;
	  float val = (float)taMisc::LexBuf;
	  pat->value.Set(j, val);
	}
      }
      if(!(strm.eof() || strm.bad())) {
	if(text_file_fmt == NAME_LAST) {
	  while(true) {
	    int c = taMisc::read_alnum(strm);
	    if(c == EOF) break;
	    int fc = taMisc::LexBuf.firstchar();
	    // skip comments
	    if((fc == '#') || (taMisc::LexBuf.before(2) == "//")) {
	      if(c != '\n')
		taMisc::read_till_eol(strm);
	      continue; // just a comment
	    }
	    ev->name = taMisc::LexBuf;
	  }
	}
      }
    }
    if(strm.eof() || strm.bad()) { // start over!
      strm.seekg(0, ios::beg);
      strm.clear();
      file_pos = 0;
    }
    else {
      file_pos++;
      break;
    }
  }
}
