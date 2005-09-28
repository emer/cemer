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



// enviro.cc


#include "enviro.h"
#include "netstru.h"
#include "sched_proc.h" // for trialproc check in enview::update_display
#include "pdpshell.h"

#ifdef TA_GUI
#include "ta_qtgroup.h"
#include "pdplog_qtso.h"
#endif


#include <math.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>


//////////////////////
//   PatternSpec    //
//////////////////////

void PatternSpec::Initialize() {
  type = INACTIVE;
  to_layer = LAY_NAME;
  layer_num = 0;
  layer = NULL;
  pattern_type = &TA_Pattern;
  layer_flags = DEFAULT;
  use_flags = USE_NO_FLAGS;
  n_vals = 0;
  geom.y = 1;
  initial_val = 0.0f;
  noise.type = Random::NONE;
  noise.mean = 0.0f;
  noise.var = 0.5f;
}

void PatternSpec::InitLinks(){
  BaseSubSpec::InitLinks();
  taBase::Own(pos, this);
  taBase::Own(geom, this);
  taBase::Own(noise,this);
  taBase::Own(value_names, this);
  taBase::Own(global_flags, this);
  EventSpec* evs = GET_MY_OWNER(EventSpec);
  if(evs == NULL)
    return;
  int index = evs->patterns.FindLeaf(this);
  if(index == 0){
    to_layer = FIRST;
    type = INPUT;
    pos.y = 0;
    UpdateAfterEdit();
  }
  else if((index == (evs->patterns.size -1)) && (index == 1)) {	// only first one!
    to_layer= LAST;
    type = TARGET;
    int ypos = evs->MaxY();
    pos.y = ypos + 1;		// by default start higher..
    UpdateAfterEdit();
  }
  else {
    int ypos = evs->MaxY();
    pos.y = ypos + 1;		// by default start higher..
  }
}

void PatternSpec::Destroy() {
  CutLinks();
}

void PatternSpec::CutLinks() {
  taBase::DelPointer((TAPtr*)&layer);
  BaseSubSpec::CutLinks();
}

void PatternSpec::Copy_(const PatternSpec& cp) {
  type=cp.type;
  to_layer=cp.to_layer;
  layer_name=cp.layer_name;
  layer_num = cp.layer_num;
  taBase::SetPointer((TAPtr*)&layer,cp.layer);
  pattern_type = cp.pattern_type;
  layer_flags = cp.layer_flags;
  use_flags = cp.use_flags;
  n_vals = cp.n_vals;
  geom = cp.geom;
  pos = cp.pos;
  initial_val = cp.initial_val;
  noise = cp.noise;
  value_names = cp.value_names;
  global_flags = cp.global_flags;
}

void PatternSpec::SetToLayName(const char* lay_nm) {
  to_layer = LAY_NAME;
  name = lay_nm;
  layer_name = lay_nm;
}

Network* PatternSpec::GetDefaultNetwork() {
  return pdpMisc::GetDefNetwork(GET_MY_OWNER(Project));
}

void PatternSpec::SetToLayer(Layer* lay) {
  if(lay == NULL) {
    Network* net = GetDefaultNetwork();
    if((net == NULL) || !SetLayer(net))
      return;
    lay = layer;
    UnSetLayer();
  }
  to_layer = LAY_NAME;
  layer_name = lay->name;
  name = layer_name;
  lay->GetActGeomNoSpc(geom);
  geom.z = 1;
  n_vals = MAX(lay->units.leaves,lay->n_units);
  pos.z=0;
  pos.x = lay->pos.x;
  pos.y = lay->pos.y;
  if(lay->pos.z > 0) {
    Network* net = lay->own_net;
    if(net != NULL) {
      int index = net->layers.FindLeaf(lay);
      pos.y = 0;
      int n;
      for(n=0;n<index;n++) {
	Layer* nlay = (Layer*)net->layers.Leaf(n);
	int y_val = nlay->pos.y + nlay->act_geom.y + 1;
	pos.y =  MAX(pos.y, y_val);
      }
    }
  }
  UpdateAfterEdit();
  UpdateAllEvents();
}

void PatternSpec::UpdateAfterEdit() {
  BaseSubSpec::UpdateAfterEdit();
  Network* net = GetDefaultNetwork();
  if((net != NULL) && SetLayer(net)) {
    if((geom.x * geom.y == 0) || (n_vals == 0))  {
      layer->GetActGeomNoSpc(geom);
      geom.z = 1;
      n_vals = MAX(layer->units.leaves,layer->n_units);
      if(net->lay_layout == Network::TWO_D){
	pos = layer->pos;
      }
      else{
	pos.z=0;
	pos.x = layer->pos.x;
	pos.y = layer->pos.y;
	if(layer->pos.z > 0) {
	  int index = net->layers.FindLeaf(layer);
	  pos.y = 0;
	  int n;
	  for(n=0;n<index;n++) {
	    Layer* lay = (Layer*)net->layers.Leaf(n);
	    int y_val = lay->pos.y + lay->act_geom.y + 1;
	    pos.y =  MAX(pos.y, y_val);
	  }
	}
      }
    }
    UnSetLayer();
  }

  if(n_vals == 0)
    n_vals = geom.x * geom.y;
  geom.FitNinXY(n_vals);

  if((name.empty() || name.contains(GetTypeDef()->name)) && !layer_name.empty())
    name = layer_name;

  value_names.EnforceSize(n_vals);
  global_flags.EnforceSize(n_vals);

  if(!taMisc::is_loading) {
    EventSpec* es = GET_MY_OWNER(EventSpec);
    if(es != NULL) {
      es->UnSetLayers();
      es->UpdateChildren();
    }
  }
}

void PatternSpec::UpdateAllEvents() {
  EventSpec* es = GET_MY_OWNER(EventSpec);
  if(es == NULL)
    return;
  es->UpdateAllEvents();
}

bool PatternSpec::SetLayer(Network* net) {
  switch(to_layer) {
  case FIRST:
    taBase::SetPointer((TAPtr*)&(layer), net->layers.Leaf(0));
    if(layer ==  NULL) {
      taMisc::Error("*** Cannot apply pattern:", GetName(),", no layers in network:", net->GetName());
      return false;
    }
    layer_num = 0;
    layer_name = layer->name;
    break;
  case LAST:
    if (net->layers.leaves > 0 ) {
      taBase::SetPointer((TAPtr*)&(layer), net->layers.Leaf(net->layers.leaves-1));
      layer_num = net->layers.leaves-1;
      layer_name = layer->name;
    }
    else {
      taMisc::Error("*** Cannot apply pattern:", GetName(),",no layers in network:", net->GetName());
      return false;
    }
    break;
  case LAY_NAME:
    taBase::SetPointer((TAPtr*)&(layer), net->layers.FindLeafName(layer_name,layer_num));
    if (layer == NULL) {
      taMisc::Error("*** Cannot apply pattern:", GetName(),
		     "no layer with name:", layer_name, "in network:", net->GetName());
      return false;
    }
    break;
  case LAY_NUM:
    if (layer_num >= net->layers.leaves) {
      taMisc::Error("*** Cannot apply pattern:", GetName(),
		     "to layer number:", String(layer_num),
		     ", only:", String((int)net->layers.leaves),
		     "layers in network:", net->GetName());
      return false;
    }
    taBase::SetPointer((TAPtr*)&(layer), net->layers.Leaf(layer_num));
    layer_name = layer->GetName();
    break;
  }
  return true;
}

void PatternSpec::UnSetLayer() {
  taBase::DelPointer((TAPtr*)&layer);
}

void PatternSpec::FlagLayer() {
  if((layer == NULL) || (layer_flags == NO_LAYER_FLAGS))
    return;

  if(layer_flags == DEFAULT) {
    switch(type) {
    case INACTIVE:
      break;
    case INPUT:
      layer->SetExtFlag(Unit::EXT);
      break;
    case TARGET:
      layer->SetExtFlag(Unit::TARG);
      break;
    case COMPARE:
      layer->SetExtFlag(Unit::COMP);
      break;
    }
    return;
  }
  layer->SetExtFlag(layer_flags); // the bits are the same..
}

float PatternSpec::Value(Pattern* pat, int index) {
  return pat->value.SafeEl(index);
}

int PatternSpec::Flag(PatUseFlags flag_type, Pattern* pat, int index) {
  static int noflags = NO_FLAGS;
  switch(flag_type) {
  case USE_NO_FLAGS:
    return noflags;
  case USE_PATTERN_FLAGS:
    return pat->flag.SafeEl(index);
  case USE_GLOBAL_FLAGS:
    return global_flags.SafeEl(index);
  case USE_PAT_THEN_GLOBAL_FLAGS: {
    int flags = pat->flag.SafeEl(index);
    if(flags == NO_FLAGS)
      return global_flags.SafeEl(index);
    return flags;
  }
  }
  return noflags;
}

void PatternSpec::ApplyValue(Pattern* pat, Unit* uni, int index) {
  float val = Value(pat, index) + noise.Gen();
  int flags = Flag(use_flags, pat, index);
  ApplyValue_impl(uni, val, flags);
}

void PatternSpec::ApplyValue_impl(Unit* uni, float val, int flags) {
  if(flags & NO_APPLY)
    return;
  if(flags != NO_FLAGS) {
    ApplyValueWithFlags(uni, val, flags);
    return;
  }
  switch(type) {
  case INACTIVE:
    break;
  case INPUT:
    uni->ext = val;
    uni->SetExtFlag(Unit::EXT);
    break;
  case TARGET:
    uni->targ = val;
    uni->SetExtFlag(Unit::TARG);
    break;
  case COMPARE:
    uni->targ = val;
    uni->SetExtFlag(Unit::COMP);
    break;
  }
}

void PatternSpec::ApplyValueWithFlags(Unit* uni, float val, int flags) {
  if(!(flags & NO_UNIT_FLAG)) {
    if(flags & COMP_TARG_EXT_FLAG) { // one of these guys was set
      uni->SetExtFlag((Unit::ExtType)(flags & COMP_TARG_EXT_FLAG));
    }
    else {
      switch(type) {
      case INACTIVE:
	break;
      case INPUT:
	uni->SetExtFlag(Unit::EXT);
	break;
      case TARGET:
	uni->SetExtFlag(Unit::TARG);
	break;
      case COMPARE:
	uni->SetExtFlag(Unit::COMP);
	break;
      }
    }
  }
  if(!(flags & NO_UNIT_VALUE)) {
    if(flags & TARG_EXT_VALUE) {
      if(flags & TARG_VALUE)
	uni->targ = val;
      if(flags & EXT_VALUE)
	uni->ext = val;
    }
    else {
      switch(type) {
      case INACTIVE:
	break;
      case INPUT:
	uni->ext = val;
	break;
      case TARGET:
	uni->targ = val;
	break;
      case COMPARE:
	uni->targ = val;
	break;
      }
    }
  }
}

void PatternSpec::ApplyPattern(Pattern* pat) {
  if(layer == NULL)
    return;
  FlagLayer();
  int v;     	// current value index;
  Unit* u; 	// current unit
  taLeafItr j;
  Unit_Group* ug = &(layer->units);
  int val_sz = pat->value.size;
  for(u = (Unit*)ug->FirstEl(j), v = 0; (u && (v < val_sz));
      u = (Unit*)ug->NextEl(j), v++)
  {
    ApplyValue(pat,u,v);
  }
}

void PatternSpec::ApplyNames() {
  if(layer == NULL)
    return;
  int v;     	// current value index;
  Unit* u; 	// current unit
  taLeafItr j;
  Unit_Group* ug = &(layer->units);
  int val_sz = value_names.size;
  for(u = (Unit*)ug->FirstEl(j), v = 0; (u && (v < val_sz));
      u = (Unit*)ug->NextEl(j), v++)
  {
    u->name = value_names[v];
  }
}

Pattern* PatternSpec::NewPattern(Event*, Pattern_Group* par) {
  Pattern* rval = (Pattern*)par->NewEl(1, pattern_type);
  rval->value.Insert(initial_val, 0, n_vals);
  if((use_flags == USE_PATTERN_FLAGS) || (use_flags == USE_PAT_THEN_GLOBAL_FLAGS))
    rval->flag.Insert(0, 0, n_vals);
  return rval;
}

void PatternSpec::UpdatePattern(Event*, Pattern* pat) {
  if(pat->value.size < n_vals)
    pat->value.Insert(initial_val, pat->value.size, n_vals - pat->value.size);
  if(pat->value.size > n_vals)
    pat->value.size = n_vals;

  if((use_flags == USE_PATTERN_FLAGS) || (use_flags == USE_PAT_THEN_GLOBAL_FLAGS)) {
    if(pat->flag.size < n_vals)
      pat->flag.Insert(0, pat->flag.size, n_vals - pat->flag.size);
    if(pat->flag.size > n_vals)
      pat->flag.size = n_vals;
  }
  else
    pat->flag.size = 0;
}


////////////////////////////
//   PatternSpec_Group    //
////////////////////////////

void PatternSpec_Group::Initialize() {
  pat_gp_type = &TA_Pattern_Group;
  SetBaseType(&TA_PatternSpec);
}

void PatternSpec_Group::Destroy() {
  CutLinks();
}

void PatternSpec_Group::Copy_(const PatternSpec_Group& cp) {
  pat_gp_type = cp.pat_gp_type;
}

void PatternSpec_Group::UpdateAfterEdit() {
  taBase_Group::UpdateAfterEdit();
  if(!taMisc::is_loading) {
    EventSpec* es = GET_MY_OWNER(EventSpec);
    if(es == NULL)
      return;
    es->UnSetLayers();
    if(es->DetectOverlap()) {
      int choice = taMisc::Choice("Overlap of PatternSpecs",
				  "Enforce Linear Layout", "Ignore");
      if(choice == 0) es->LinearLayout();
    }
  }
}

void PatternSpec_Group::LinearLayout() {
  EventSpec* es = GET_MY_OWNER(EventSpec);
  if(es == NULL) return;
  es->LinearLayout();
}

void PatternSpec_Group::CutLinks() {
  if((pat_gp_type == NULL) || (owner == NULL)) {
    taBase_Group::CutLinks();
    return;
  }
  pat_gp_type = NULL;		// don't do it more than once

  if(!owner->InheritsFrom(TA_taSubGroup)) { // only for created groups
    taBase_Group::CutLinks();
    return;
  }
  EventSpec* es = GET_MY_OWNER(EventSpec);
  if(es == NULL) {
    taBase_Group::CutLinks();
    return;
  }
  Environment* env = GET_OWNER(es,Environment);
  if(env == NULL) {
    taBase_Group::CutLinks();
    return;
  }
  String my_path = GetPath(NULL, es); // my path is same as corresp pat group
  Event* ev;
  taLeafItr e;
  FOR_ITR_EL(Event, ev, env->events., e) {
    if(ev->spec.spec != es)
      continue;			// only for events that use me!
    MemberDef* md;
    Pattern_Group* pg = (Pattern_Group*)ev->FindFromPath(my_path, md);
    if((pg == NULL) || !pg->InheritsFrom(TA_Pattern_Group) ||
       !pg->owner->InheritsFrom(TA_taSubGroup))
      continue;
    taSubGroup* og = (taSubGroup*)pg->owner;
    og->Remove(pg);
  }
  taBase_Group::CutLinks();
}

TAPtr PatternSpec_Group::NewEl(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
    if(taMisc::gui_active)
      return gpiGroupNew::New(this,typ);
#endif
    return NULL;
  }
  EventSpec* es = GET_MY_OWNER(EventSpec);
  if((es == NULL) || taMisc::is_loading) // don't do fancy stuff when loading
    return taBase_Group::NewEl(no,typ);
  Environment* env = GET_OWNER(es,Environment);
  if(env == NULL)
    return taBase_Group::NewEl(no,typ);
  String my_path = GetPath(NULL, es); // my path is same as corresp pat group
  int st_idx = size;
  TAPtr rval = taBase_Group::NewEl(no,typ);
  Event* ev;
  taLeafItr e;
  FOR_ITR_EL(Event, ev, env->events., e) {
    if(ev->spec.spec != es)
      continue;			// only for events that use me!
    MemberDef* md;
    Pattern_Group* pg = (Pattern_Group*)ev->FindFromPath(my_path, md);
    if((pg == NULL) || !pg->InheritsFrom(&TA_Pattern_Group))
      continue;
    int i;
    for(i=st_idx; i<size; i++)
      ((PatternSpec*)FastEl(i))->NewPattern(ev, pg);
  }
  es->UnSetLayers();
  es->UpdateChildren();
  return rval;
}

taGroup<taBase>* PatternSpec_Group::NewGp(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
    if(taMisc::gui_active)
      gpiGroupNew::New(this,typ);
#endif
    return NULL;
  }
  if((typ == NULL) || !(typ->InheritsFrom(GetTypeDef())))
    typ = GetTypeDef();
  EventSpec* es = GET_MY_OWNER(EventSpec);
  if((es == NULL) || taMisc::is_loading) // don't do fancy stuff when loading..
    return taBase_Group::NewGp(no,typ);
  Environment* env = GET_OWNER(es,Environment);
  if(env == NULL)
    return taBase_Group::NewGp(no,typ);
  String my_path = GetPath(NULL, es); // my path is same as corresp pat group
  int st_idx = gp.size;
  taGroup<taBase>* rval = taBase_Group::NewGp(no,typ);
  Event* ev;
  taLeafItr e;
  FOR_ITR_EL(Event, ev, env->events., e) {
    if(ev->spec.spec != es)
      continue;			// only for events that use me!
    MemberDef* md;
    Pattern_Group* pg = (Pattern_Group*)ev->FindFromPath(my_path, md);
    if((pg == NULL) || !pg->InheritsFrom(&TA_Pattern_Group))
      continue;
    int i;
    for(i=st_idx; i<gp.size; i++)
      ((PatternSpec_Group*)(gp.FastEl(i)))->NewPatternGroup(ev, pg);
  }
  es->UnSetLayers();
  es->UpdateChildren();
  return rval;
}


TAPtr PatternSpec_Group::New(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
    if(taMisc::gui_active)
      return gpiGroupNew::New(this,typ);
#endif
    return NULL;
  }
  if(typ == NULL) typ = el_typ;
  if(typ->InheritsFrom(TA_taGroup_impl))
    return (TAPtr)NewGp(no,typ);
  return NewEl(no,typ);
}

bool PatternSpec_Group::Remove(int i) {
  EventSpec* es = GET_MY_OWNER(EventSpec);
  if(es == NULL) return false;
  Environment* env = GET_OWNER(es,Environment);
  if(env == NULL) return false;
  String my_path = GetPath(NULL, es); // my path is same as corresp pat group
  Event* ev;
  taLeafItr e;
  FOR_ITR_EL(Event, ev, env->events., e) {
    if(ev->spec.spec != es)
      continue;			// only for events that use me!
    MemberDef* md;
    Pattern_Group* pg = (Pattern_Group*)ev->FindFromPath(my_path, md);
    if((pg == NULL) || !pg->InheritsFrom(&TA_Pattern_Group))
      continue;
    pg->Remove(i);
  }
  bool rval = taBase_Group::Remove(i);
  es->UnSetLayers();
  es->UpdateChildren();
  return rval;
}

Pattern_Group* PatternSpec_Group::NewPatternGroup(Event* ev, Pattern_Group* par) {
  Pattern_Group* rval = (Pattern_Group*)par->NewGp(1, pat_gp_type);

  PatternSpec* ps;
  int i;
  for(i=0; i<size; i++) {
    ps = (PatternSpec*)FastEl(i);
    ps->NewPattern(ev,rval);
  }

  PatternSpec_Group* pg;
  for(i=0; i<gp.size; i++) {
    pg = (PatternSpec_Group*)gp.FastEl(i);
    pg->NewPatternGroup(ev,rval);
  }
  return rval;
}

void PatternSpec_Group::UpdatePatternGroup(Event* ev, Pattern_Group* pg) {
  // enforce sizes to be the same..
  while(pg->size > size) pg->Remove(pg->size-1);
  PatternSpec* ps;
  Pattern* pat;
  int i;
  for(i=0; i<size; i++) {
    ps = (PatternSpec*)FastEl(i);
    if(pg->size <= i) {
      pat = (Pattern*)pg->NewEl(1,ps->pattern_type);
    }
    else {
      pat = (Pattern*)pg->FastEl(i);
      if(pat->GetTypeDef() != ps->pattern_type) {
	pat = (Pattern*)taBase::MakeToken(ps->pattern_type);
	if(pat != NULL)
	  pg->Replace(i, pat);
      }
    }
    ps->UpdatePattern(ev,pat);
  }

  while(pg->gp.size > gp.size) pg->gp.Remove(pg->gp.size-1);
  PatternSpec_Group* pgs;
  Pattern_Group* sgp;
  for(i=0; i<gp.size; i++) {
    pgs = (PatternSpec_Group*)gp.FastEl(i);
    if(pg->gp.size <= i) {
      sgp = (Pattern_Group*)pg->NewGp(1, pgs->pat_gp_type);
    }
    else {
      sgp = (Pattern_Group*)pg->gp.FastEl(i);
      if(sgp->GetTypeDef() != pgs->pat_gp_type) {
	sgp = (Pattern_Group*)taBase::MakeToken(pgs->pat_gp_type);
	if(sgp != NULL)
	  pg->gp.Replace(i, sgp);
      }
    }
    pgs->UpdatePatternGroup(ev,sgp);
  }
}


////////////////////////////
//        EventSpec       //
////////////////////////////

void EventSpec::Initialize() {
  last_net = NULL;
  pattern_layout = HORIZONTAL;
}

void EventSpec::InitLinks() {
  BaseSpec::InitLinks();
  taBase::Own(patterns, this);
  if(!taMisc::is_loading)
    UpdateAfterEdit();
  if(taMisc::gui_active)
    AddToView();
}

void EventSpec::CutLinks() {
  if(taMisc::gui_active)
    RemoveFromView();
  patterns.CutLinks();
  taBase::DelPointer((TAPtr*)&last_net);
  BaseSpec::CutLinks();
}

void EventSpec::Copy(const EventSpec& cp) {
  BaseSpec::Copy(cp);
  patterns = cp.patterns;
  pattern_layout = cp.pattern_layout;
}

void EventSpec::UpdateSubSpecs() {
  EventSpec* parent = (EventSpec*)FindParent();
  if(parent != NULL) {
    // match our sub-patterns with the parents
    patterns.EnforceSameStru(parent->patterns);
  }
  PatternSpec* ps;
  taLeafItr psi;
  FOR_ITR_EL(PatternSpec, ps, patterns., psi)
    ps->UpdateSpec();
}

void EventSpec::UpdateSpec() {
  BaseSpec::UpdateSpec();
  UpdateAllEvents();
}

void EventSpec::SetLayers(Network* net) {
  PatternSpec* ps;
  taLeafItr psi;
  FOR_ITR_EL(PatternSpec, ps, patterns., psi)
    ps->SetLayer(net);
  taBase::SetPointer((TAPtr*)&last_net, net);
}

void EventSpec::UnSetLayers() {
  PatternSpec* ps;
  taLeafItr psi;
  FOR_ITR_EL(PatternSpec, ps, patterns., psi)
    ps->UnSetLayer();
  taBase::DelPointer((TAPtr*)&last_net);
}

void EventSpec::ApplyPatterns(Event* ev, Network* net) {
  if(net == NULL) {
    taMisc::Error("Event:",ev->GetPath(), " cannot apply patterns since network is NULL.",
		   "Try reseting the network pointer in your processes");
    return;
  }

  if(net != last_net)
    SetLayers(net);

  Pattern* pat;
  PatternSpec* ps;
  taLeafItr pi, psi;
  FOR_ITR_PAT_SPEC(Pattern, pat, ev->patterns., pi, PatternSpec, ps, patterns., psi) {
    ps->ApplyPattern(pat);
  }
}

void EventSpec::ApplyNames(Network* net) {
  if(net == NULL)
    return;

  SetLayers(net);

  PatternSpec* ps;
  taLeafItr psi;
  FOR_ITR_EL(PatternSpec, ps, patterns., psi) {
    ps->ApplyNames();
  }

  UnSetLayers();
}

void EventSpec::NewEvent(Event* ev) {
  ev->spec.SetSpec(this);

  ev->patterns.Reset();
  PatternSpec* ps;
  int i;
  for(i=0; i<patterns.size; i++) {
    ps = (PatternSpec*)patterns.FastEl(i);
    ps->NewPattern(ev,&(ev->patterns));
  }

  PatternSpec_Group* pg;
  for(i=0; i<patterns.gp.size; i++) {
    pg = (PatternSpec_Group*)patterns.gp.FastEl(i);
    pg->NewPatternGroup(ev,&(ev->patterns));
  }
}

void EventSpec::UpdateEvent(Event* ev) {
  patterns.UpdatePatternGroup(ev, &(ev->patterns));
}

void EventSpec::UpdateAfterEdit() {
  UnSetLayers();
  if(!taMisc::is_loading && !taMisc::is_duplicating) {
    if((patterns.leaves == 0) && (patterns.gp.size == 0)) { // new eventspec..
      patterns.NewEl(2);
      if(taMisc::gui_active)
	taMisc::DelayedMenuUpdate(this);
    }
    if(DetectOverlap()) {
      int choice = taMisc::Choice("Overlap of PatternSpecs",
				  "Enforce Linear Layout", "Ignore");
      if(choice == 0) LinearLayout();
    }
    PatternSpec* ps;
    taLeafItr psi;
    FOR_ITR_EL(PatternSpec, ps, patterns., psi)
      ps->UpdateAfterEdit();
  }
  BaseSpec::UpdateAfterEdit();	// this calls UpdateSpec which calls UpdateAllEvents..
}

void EventSpec::UpdateAllEvents() {
  if(!taMisc::is_loading)
    StructUpdate(true);
  UnSetLayers();
  UpdateChildren();
  Environment* env = GET_MY_OWNER(Environment);
  if(env == NULL) return;
  Event* ev;
  taLeafItr e;
  FOR_ITR_EL(Event, ev, env->events., e) {
    ev->GetLocalSpec();
    if(ev->spec.spec != this)
      continue;			// only for events that use me!
    UpdateEvent(ev);
  }
  if(!taMisc::is_loading)
    StructUpdate(false);
//TODO: need to hook env to specs updating
//    env->InitAllViews();
}

void EventSpec::UpdateFromLayers() {
  PatternSpec* ps;
  taLeafItr psi;
  FOR_ITR_EL(PatternSpec, ps, patterns., psi)
    ps->SetToLayer();
}

bool EventSpec::DetectOverlap() {
  // check for overlap
  int maxx = MaxX()+1; int maxy = MaxY()+1;
  bool* grid = new bool[maxx * maxy];
  int x;
  for(x=0;x<maxx;x++) {
    int y;
    for(y=0;y<maxy;y++)
      grid[(y*maxx) + x] = false;
  }
  PatternSpec* pat;
  taLeafItr i;
  FOR_ITR_EL(PatternSpec, pat, patterns., i) {
    int px;
    for(px=0; px<pat->geom.x; px++) {
      int py;
      for(py=0;py<pat->geom.y;py++) {
	int acty = MIN(maxy, (py + pat->pos.y));
	int actx = MIN(maxx, (px + pat->pos.x));
	int index = (acty * maxx) + actx;
	if(grid[index] != false) { // overlap
	  delete [] grid;
	  return true;
	}
	grid[index] = true;
      }
    }
  }
  delete [] grid;
  return false;
}

void EventSpec::LinearLayout(PatternLayout pat_layout) {
  if(pat_layout == DEFAULT)
    pat_layout = pattern_layout;
  else
    pattern_layout = pat_layout;
  int del_x = 1;  int del_y = 0;
  if(pat_layout == VERTICAL) {
    del_x = 1;	del_y = 0;
  }

  int xp = 0;
  int yp = 0;
  PatternSpec* pat;
  taLeafItr i;
  FOR_ITR_EL(PatternSpec, pat, patterns., i) {
    pat->pos.x = xp;
    pat->pos.y = yp;
    xp += del_x * (pat->geom.x + 1);
    yp += del_y * (pat->geom.y + 1);
    tabMisc::NotifyEdits(pat);
  }
  UpdateChildren();		// apply changes to sub specs
  UpdateAllEvents();
}

void EventSpec::AutoNameEvent(Event* ev, float act_thresh, int max_pat_nm, int max_val_nm) {
  String nm;
  Pattern* pat;
  PatternSpec* ps;
  taLeafItr pi, psi;
  FOR_ITR_PAT_SPEC(Pattern, pat, ev->patterns., pi, PatternSpec, ps, patterns., psi) {
    if(!nm.empty()) nm += ",";
    nm += taMisc::StringMaxLen(ps->name, max_pat_nm) + ":";
    for(int i=0;i<pat->value.size;i++) {
      if(pat->value[i] > act_thresh) {
	if(nm.lastchar() != ':') nm += "_";
	nm += taMisc::StringMaxLen(ps->value_names.SafeEl(i), max_val_nm);
      }
    }
  }
  ev->name = nm;
}

void EventSpec::AutoNameAllEvents(float act_thresh, int max_pat_nm, int max_val_nm) {
  Environment* env = GET_MY_OWNER(Environment);
  if(env == NULL) return;
  StructUpdate(true);
  Event* ev;
  taLeafItr e;
  FOR_ITR_EL(Event, ev, env->events., e) {
    if(ev->spec.spec != this)
      continue;			// only for events that use me!
    ev->AutoNameEvent(act_thresh, max_pat_nm, max_val_nm);
  }
  StructUpdate(false);
  //TODO: spec change must trigger env update
//  env->InitAllViews();
}

int EventSpec::MaxX() {
  int mx = 0;
  PatternSpec* pat;
  taLeafItr i;
  FOR_ITR_EL(PatternSpec, pat, patterns., i){
    int patx = pat->pos.x + pat->geom.x;
    mx = MAX(mx,patx);
  }
  return mx;
}

int EventSpec::MaxY() {
  int mx = 0;
  PatternSpec* pat;
  taLeafItr i;
  FOR_ITR_EL(PatternSpec, pat, patterns., i){
    int paty = pat->pos.y + pat->geom.y;
    mx = MAX(mx,paty);
  }
  return mx;
}

void EventSpec::AddToView() {
  if(!taMisc::gui_active || taMisc::is_loading) return;
  Environment* env = GET_MY_OWNER(Environment);
  if(env == NULL) return;
/*TODO
  int leaf_idx = -2;

  EnviroView* evv;
  taLeafItr j;
  FOR_ITR_EL(EnviroView,evv,env->views.,j) {
    if(evv->editor == NULL) continue;
    evv->editor->AddSpec(this, leaf_idx);	// -2 is signal to update after, and find leaf idx
  } */
}

void EventSpec::RemoveFromView() {
  if(!taMisc::gui_active) return;
  Environment* env = GET_MY_OWNER(Environment);
  if(env == NULL) return;
/*TODO
  EnviroView* evv;
  taLeafItr j;
  FOR_ITR_EL(EnviroView,evv,env->views.,j) {
    if(evv->editor == NULL) continue;
    evv->editor->RemoveSpec(this);
  } */
}

BaseSpec_MGroup* EventSpec_SPtr::GetSpecGroup() {
  Environment* env = GET_OWNER(owner,Environment);
  if(env == NULL)
    return NULL;
  return &(env->event_specs);
}


////////////////////////////
//        Pattern         //
////////////////////////////

void Pattern::Initialize() {
}

void Pattern::InitLinks() {
  taBase::Own(value, this);
  taBase::Own(flag, this);
  taOBase::InitLinks();
}

void Pattern::Copy_(const Pattern& cp) {
  value = cp.value;
  flag = cp.flag;
}

void Pattern::CutLinks() {
  value.Reset();
  flag.Reset();
  taOBase::CutLinks();
}

//////////////////////
//      Event       //
//////////////////////


void Event::Initialize() {
  index = -1;
  patterns.SetBaseType(&TA_Pattern);
}

void Event::Destroy() {
  CutLinks();
}

void Event::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(patterns, this);
  spec.SetDefaultSpec(this);
  if(taMisc::gui_active)
    AddToView();
}

void Event::CutLinks() {
  if(taMisc::gui_active)
    RemoveFromView();
  index = -1;
  patterns.Reset();
  spec.CutLinks();
  taNBase::CutLinks();
}

void Event::Copy(const Event& cp) {
  taNBase::Copy(cp);
  spec = cp.spec;
  patterns = cp.patterns;
  GetLocalSpec();
}

void Event::UpdateAfterEdit() {
  taNBase::UpdateAfterEdit();
  GetLocalSpec();
  if(spec.spec == NULL) return ;// no spec found (this Event might be a default object)
  if(patterns.leaves != spec->patterns.leaves) {
    int choice = taMisc::Choice("Warning:: The Event does not have the same number of patterns as its EventSpec",
				"Enforce EventSpec","Create New EventSpec");
    if(choice == 0) { // enforce
      patterns.EnforceLeaves(spec->patterns.leaves);
    }
    else { // new eventspec
      Environment* env = GET_OWNER(owner,Environment);
      env->event_specs.DuplicateEl(spec.spec);
      EventSpec* sp = (EventSpec *) env->event_specs.Peek();
      sp->name +="(copy)";
      sp->patterns.EnforceLeaves(patterns.leaves);
      spec.SetSpec(sp);
      taMisc::DelayedMenuUpdate(sp);
    }
  }
  spec->UpdateEvent(this);
  if(taMisc::gui_active)
    AddToView();
}

void Event::SetSpec(EventSpec* es) {
  if(es == NULL) return;
  spec.SetSpec(es);
  UpdateFmSpec();
}

void Event::UpdateFmSpec() {
  GetLocalSpec();
  patterns.EnforceLeaves(spec->patterns.leaves);
  spec->UpdateEvent(this);
}

void Event::GetLocalSpec() {
  Environment* env = GET_MY_OWNER(Environment);
  if(env == NULL)
    return;
  if(spec.spec == NULL) {
    spec.SetSpec((EventSpec*)env->event_specs.DefaultEl());
    return;
  }
  if(GET_OWNER(spec.spec,Environment) != env) {
    EventSpec* es = (EventSpec*)env->event_specs.FindName(spec->name);
    if(es != NULL)
      spec.SetSpec(es);
    else if(spec.spec != NULL) {
      es = (EventSpec*)spec.spec->Clone(); // make one of these..
      es->name = spec.spec->name;
      env->event_specs.Add(es);
      spec.SetSpec(es);
      taMisc::DelayedMenuUpdate(es);
    }
  }
}

String Event::GetDisplayName() {
  Event_MGroup* ownr = (Event_MGroup*)owner;
  if(ownr == NULL) return name;
  String rval = String("[") + String(index) + "] " + name;
  if(ownr->taList_impl::name.empty())
    return rval;
  Environment* env = GET_MY_OWNER(Environment);
  if((env == NULL) || (owner == &(env->events)))
    return rval;
  int min_len = MIN(ownr->taList_impl::name.length(), 4);
  return ownr->taList_impl::name.before(min_len) + rval;
}

void Event::AddToView() {
  if(!taMisc::gui_active || taMisc::is_loading) return;
  Environment* env = GET_MY_OWNER(Environment);
  if(env == NULL) return;
/*TODO
  EnviroView* evv;
  taLeafItr j;
  FOR_ITR_EL(EnviroView,evv,env->views.,j) {
    if(evv->editor == NULL) continue;
    evv->editor->AddEvent(this, -2);	// -2 is signal to update after, and find button idx
  } */
}

void Event::RemoveFromView() {
  if(!taMisc::gui_active) return;
  Environment* env = GET_MY_OWNER(Environment);
  if(env == NULL) return;
/*TODO
  EnviroView* evv;
  taLeafItr j;
  FOR_ITR_EL(EnviroView,evv,env->views.,j) {
    if(evv->editor == NULL) continue;
    evv->editor->RemoveEvent(this);
  }*/
}

void Event::PresentEvent(TrialProcess* trial_proc, bool new_init) {
  if(trial_proc == NULL) return;
  if(trial_proc->epoch_proc != NULL)
    taBase::SetPointer((TAPtr*)&(trial_proc->epoch_proc->cur_event), this);
  else
    taBase::SetPointer((TAPtr*)&(trial_proc->cur_event), this);
  if(new_init)
    trial_proc->NewInit();
  else
    trial_proc->ReInit();
  trial_proc->Run();
}

void Event::AutoNameEvent(float act_thresh, int max_pat_nm, int max_val_nm) {
  if(spec.spec == NULL) return;
  spec.spec->AutoNameEvent(this, act_thresh, max_pat_nm, max_val_nm);
}


//////////////////////
//   Event_MGroup    //
//////////////////////

void Event_MGroup::Initialize() {
  SetBaseType(&TA_Event);
}

void Event_MGroup::InitLinks() {
  taGroup<Event>::InitLinks();
  if(taMisc::gui_active)
    AddToView();
}

void Event_MGroup::CutLinks() {
  RemoveFromView();
  taGroup<Event>::CutLinks();
}

void Event_MGroup::UpdateAfterEdit() {
  taGroup<Event>::UpdateAfterEdit();
  if(taMisc::gui_active)
    AddToView();
}

String Event_MGroup::GetDisplayName() {
  String rval = String(">> ") + taGroup<Event>::name;
  if(!taGroup<Event>::name.empty()) return rval;
   if(super_gp != NULL) {
    int idx = super_gp->gp.Find(this);
    rval += String("Gp[") + String(idx) + "]";
    return rval;
  }
  return rval;		// this shouldn't happen
}

void Event_MGroup::AddToView() {
  if(!taMisc::gui_active || taMisc::is_loading) return;
  Environment* env = GET_MY_OWNER(Environment);
  if(env == NULL) return;
/*TODO
  EnviroView* evv;
  taLeafItr j;
  FOR_ITR_EL(EnviroView,evv,env->views.,j) {
    if(evv->editor == NULL) continue;
    evv->editor->AddEventGp(this, -2);	// -2 is signal to update after, and find button idx
  } */
}

void Event_MGroup::RemoveFromView() {
  if(!taMisc::gui_active) return;
  Environment* env = GET_MY_OWNER(Environment);
  if(env == NULL) return;
/*TODO
  EnviroView* evv;
  taLeafItr j;
  FOR_ITR_EL(EnviroView,evv,env->views.,j) {
    if(evv->editor == NULL) continue;
    evv->editor->RemoveEventGp(this);
  } */
}

Event* Event_MGroup::New(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
    if(taMisc::gui_active)
      return (Event*)gpiGroupNew::New(this,typ);
#endif
    return NULL;
  }
  int old_sz = size;
  Event* rval = taGroup<Event>::NewEl(no, typ);
  // actually created elements (not groups)
  if(((typ == NULL) || !typ->InheritsFrom(&TA_taGroup_impl)) && !taMisc::is_loading)
    NewEl_impl(old_sz);
  return rval;
}

Event* Event_MGroup::NewEl(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
    if(taMisc::gui_active)
      return (Event*)gpiGroupNew::New(this,typ);
#endif
    return NULL;
  }
  int old_sz = size;
  Event* rval = taGroup<Event>::NewEl(no, typ);
  NewEl_impl(old_sz);
  return rval;
}

Event* Event_MGroup::NewFmSpec(int no, TypeDef* typ, EventSpec* es) {
  int old_sz = size;
  Event* rval = taGroup<Event>::NewEl(no, typ);
  NewEl_impl(old_sz, es);
  return (Event*)rval;
}

void Event_MGroup::NewEl_impl(int old_sz, EventSpec* es) {
  if(es == NULL) {
    Environment* env = GET_MY_OWNER(Environment);
    if(env == NULL) return;
    es = (EventSpec*)env->event_specs.DefaultEl();
    if(es == NULL) return;
  }
  // number with the index implicit..
  int i;
  for(i=old_sz; i<size; i++) {
    Event* new_ev = (Event*)FastEl(i);
    es->NewEvent(new_ev);
    new_ev->name = new_ev->GetTypeDef()->name + "_" + String(i);
  }
}


//////////////////////
//   Environment    //
//////////////////////

void Environment::Initialize() {
  events.SetBaseType(&TA_Event);
  event_specs.SetBaseType(&TA_EventSpec);
//TODO  views.SetBaseType(&TA_EnviroView);
  event_ctr = 0;
}

void Environment::InitLinks() {
  taBase::Own(events, this);
  taBase::Own(event_specs, this);
  inherited::InitLinks();
}

void Environment::Destroy(){
  CutLinks();
}

void Environment::Copy(const Environment& cp) {
  inherited::Copy(cp);
  event_specs = cp.event_specs;
  events = cp.events;
  event_ctr = cp.event_ctr;
}

void Environment::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  UpdateAllEvents();
}

EventSpec* Environment::GetAnEventSpec() {
  if(event_specs.size == 0)
    return (EventSpec*)event_specs.NewEl(1);
  return (EventSpec*)event_specs.DefaultEl();
}

void Environment::CutLinks() {
  static bool in_repl = false;
  if(in_repl || (owner == NULL)) return; // already replacing or already dead
  Project* prj = (Project *) GET_MY_OWNER(Project);
  if((prj != NULL) && !prj->deleting) {
    Environment* replenv = NULL;
    int nl;
    for(nl=0;nl<prj->environments.leaves;nl++) {
      Environment* nt = (Environment*)prj->environments.Leaf(nl);
      if(nt != this) {
	replenv = nt;
	break;
      }
    }
    // replace any actual process.environment ptrs with another one -- could be useful
    prj->processes.ReplaceEnvPtrs(this, replenv);
    // set any misc environment pointers to NULL!
    in_repl = true;
    taMisc::ReplaceAllPtrs(GetTypeDef(), (void*)this, NULL);
    in_repl = false;
  }
  inherited::CutLinks();	// winmgrs cut views first..
  event_specs.CutLinks();
  events.CutLinks();
}

#ifdef TA_GUI
void Environment::DistMatrixGrid(GridLog* disp_log, int pat_no, float_RArray::DistMetric metric,
			     bool norm, float tol)
{
  if(events.leaves == 0)
    return;

  if(disp_log == NULL) {
    disp_log = (GridLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GridLog);
    if(disp_log == NULL) return;
  }
  else {
/*TODO:    LogView* lv = (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return; */
  }

  disp_log->SetName( "DistMatrixGrid: " + GetName() + " Pattern: " + String(pat_no));
  DataTable* dt = &(disp_log->data);

  dt->Reset();
  dt->NewColString("Event");
  DataTable* dtgp = dt->NewGroupFloat("dists", events.leaves);
  bool first = true;
  int cnt = 0;
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    String nm = ev->name;
    if(first) {
      nm = String("<") + String(events.leaves) + ">" + nm;
      first = false;
      String geom = "GEOM_X=" + String(events.leaves);
      dtgp->AddColDispOpt(geom, cnt);
      dtgp->AddColDispOpt("GEOM_Y=1", cnt);
      dtgp->AddColDispOpt("USE_EL_NAMES", cnt); // each column has separate element names in gp
    }
    dtgp->SetColName(nm, cnt++);
  }
  dt->ResetData();

  dt->RowAdding();
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }
    dt->AddStringVal_deprecated(ev->name, 0); // col 0, subgp -1
    cnt = 0;			// column number counter
    taLeafItr j;
    Event* oev;
    FOR_ITR_EL(Event, oev, events., j) {
      Pattern* pat = (Pattern*)oev->patterns.Leaf(pat_no);
      if(pat == NULL) {
	taMisc::Error("*** Pattern number:", String(pat_no), "not found");
	return;
      }
      float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
      dt->AddFloatVal_deprecated(dst, cnt, 0); // col cnt, subgp 0
      cnt++;
    }
  }
  dt->RowAdded();

  disp_log->ViewAllData();
}

void Environment::CmpDistMatrixGrid(GridLog* disp_log, int pat_no, Environment* cmp_env,
				    int cmp_pat_no, float_RArray::DistMetric metric,
				    bool norm, float tol)
{
  if((events.leaves == 0) || (cmp_env == NULL) || (cmp_env->events.leaves == 0))
    return;

  if(disp_log == NULL) {
    disp_log = (GridLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GridLog);
    if(disp_log == NULL) return;
  }
  else {
/*TODO:    LogView* lv = (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return; */
  }

  disp_log->SetName("CmpDistMatrixGrid: " + GetName() + " (pat: " + String(pat_no) + ") vs "
    + cmp_env->GetName() + " (pat: " + String(cmp_pat_no) + ")");
  DataTable* dt = &(disp_log->data);

  dt->Reset();
  dt->NewColString("Event");
  DataTable* dtgp = dt->NewGroupFloat("dists", cmp_env->events.leaves);
  bool first = true;
  int cnt = 0;
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, cmp_env->events., i) {
    String nm = ev->name;
    if(first) {
      nm = String("<") + String(cmp_env->events.leaves) + ">" + nm;
      first = false;
      String geom = "GEOM_X=" + String(cmp_env->events.leaves);
      dtgp->AddColDispOpt(geom, cnt);
      dtgp->AddColDispOpt("GEOM_Y=1", cnt);
      dtgp->AddColDispOpt("USE_EL_NAMES", cnt); // each column has separate element names in gp
    }
    dtgp->SetColName(nm, cnt++);
  }
  dt->ResetData();
  dt->RowAdding();
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }
    dt->AddStringVal_deprecated(ev->name, 0); // col 0, subgp -1
    cnt = 0;			// column number counter
    taLeafItr j;
    Event* oev;
    FOR_ITR_EL(Event, oev, cmp_env->events., j) {
      Pattern* pat = (Pattern*)oev->patterns.Leaf(pat_no);
      if(pat == NULL) {
	taMisc::Error("*** Pattern number:", String(pat_no), "not found");
	return;
      }
      float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
      dt->AddFloatVal_deprecated(dst, cnt, 0); // col cnt, subgp 0
      cnt++;
    }
  }
  dt->RowAdded();

  disp_log->ViewAllData();
}

void Environment::ClusterPlot(GraphLog* disp_log, int pat_no,
			      float_RArray::DistMetric metric,
			      bool norm, float tol)
{
  if(disp_log == NULL) {
    disp_log = (GraphLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
    if(disp_log == NULL) return;
  }
  else {
/*TODO:    LogView* lv = (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return;*/
  }

  // initialize with leaves
  ClustNode root;
  taLeafItr ei;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., ei) {
    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }
    ClustNode* nd = new ClustNode;
    nd->name = ev->name;
    nd->SetPat(&(trg_pat->value));
    root.AddChild(nd);
  }

  root.Cluster(metric, norm, tol);

  disp_log->SetName("ClusterPlot: " + GetName() + " Pattern: " + String(pat_no));
  root.GraphData(&(disp_log->data));
  disp_log->ViewAllData();
//TODO:must be updated  disp_log->InitAllViews();
}

void Environment::CorrelMatrixGrid(GridLog* disp_log, int pat_no) {
  if(events.leaves == 0)
    return;

  if(disp_log == NULL) {
    disp_log = (GridLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GridLog);
    if(disp_log == NULL) return;
  }
  else {
/*TODO:    LogView* lv = (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return; */
  }

  disp_log->SetName("CorrelMatrixGrid: " + GetName() + " Pattern: " + String(pat_no));
  DataTable* dt = &(disp_log->data);

  int dim;
  float_RArray correl_mat;
  CorrelMatrix(correl_mat, pat_no, dim);

  dt->Reset();
  dt->NewColString("val");
  DataTable* dtgp = dt->NewGroupFloat("correls", dim);

  bool first = true;
  for(int i=0;i<dim;i++) {
    String nm = String("v") + String(i);
    if(first) {
      nm = String("<") + String(dim) + ">" + nm;
      first = false;
      String geom = "GEOM_X=" + String(dim);
      dtgp->AddColDispOpt(geom, i);
      dtgp->AddColDispOpt("GEOM_Y=1", i);
      dtgp->AddColDispOpt("USE_EL_NAMES", i); // each column has separate element names in gp
    }
    dtgp->SetColName(nm, i);
  }
  dt->ResetData();
  dt->RowAdding();
  for(int i=0;i<dim;i++) {
    String nm = String("v") + String(i);
    dt->AddStringVal_deprecated(nm, 0);
    for(int j=0;j<dim;j++) {
      float val = correl_mat.FastTriMatEl(dim, i, j);
      dt->AddFloatVal_deprecated(val, i, 0); // col cnt, subgp 0
    }
  }
  dt->RowAdded();

  disp_log->ViewAllData();
}

void Environment::PCAEigenGrid(GridLog* disp_log, int pat_no, bool print_eigens) {
  if(event_specs.size == 0) return;
  PatternSpec* ps = (PatternSpec*)((EventSpec*)event_specs[0])->patterns[pat_no];
  if(ps == NULL) return;

  if(disp_log == NULL) {
    disp_log = (GridLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GridLog);
    if(disp_log == NULL) return;
  }
  else {
    disp_log->Clear();
/*TODO:    LogView* lv = (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return;*/
  }

  disp_log->SetName((String)"PCAEigenGrid: " + GetName()  + " Pattern: " + String(pat_no));

  DataTable* md = &(disp_log->data);
  md->RemoveAll();

  md->NewColString("row");

  int ptx = ps->geom.x;
  int pty = ps->geom.y;

  int dim;			// dimensionality
  float_RArray evecs;		// eigen vectors
  float_RArray evals;		// eigen values
  PCAEigens(evecs, evals, pat_no, dim);

  if(print_eigens) {
    cerr << "full list of eigen values (sorted lowest-to-highest, reverse of component indicies): " << endl;
    evals.List(cerr);		// these are sorted in ascending order
    cerr << endl;
  }

  TwoDCoord outerg;		// outer-grid geom
  outerg.FitN(dim);		// just find best fit
  int evx = outerg.x; int evy = outerg.y; // evx,y from EnvToGrid

  // these are the columns
  int i;
  for(i=0; i<evx; i++) {
    md->NewGroupFloat((String)"c" + (String)i, dim);
    md->AddColDispOpt(String("GEOM_X=") + String(ptx), 0, i); // column 0, subgp i
    md->AddColDispOpt(String("GEOM_Y=") + String(pty), 0, i); // column 0, subgp i
  }

  md->ResetData();

  for(i=0;i<evy;i++) {
    md->AddBlankRow();		// add the rows
    md->SetStringVal(String(evy - i -1), 0, i);
  }

  float_RArray evec;

  int uni = 0;
  int ux, uy;
  for(uy=0;uy<evy; uy++) {
    if(uni >= dim)
      break;
    for(ux=0; ux<evx; ux++, uni++) {
      if(uni >= dim)
	break;

      evecs.GetMatCol(dim, evec, dim-uni-1); // get eigen vector = column of correl_matrix
      DataTable* dt = (DataTable*)md->gp.FastEl(ux);
      dt->PutArrayToRow(evec, evy-uy-1);
    }
  }

  disp_log->ViewAllData();
}

void Environment::PCAPrjnPlot(GraphLog* disp_log, int pat_no, int x_axis_c, int y_axis_c, bool print_eigens) {
  if(disp_log == NULL) {
    disp_log = (GraphLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
    if(disp_log == NULL) return;
  }
  else {
/*TODO:    LogView* lv = (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return;*/
  }

  int dim;			// dimensionality
  float_RArray evecs;		// eigen vectors
  float_RArray evals;		// eigen values
  PCAEigens(evecs, evals, pat_no, dim);

  if((x_axis_c < 0) || (x_axis_c >= dim)) {
    taMisc::Error("*** PCA: x_axis component must be between 0 and",String(dim-1));
    return;
  }
  if((y_axis_c < 0) || (y_axis_c >= dim)) {
    taMisc::Error("*** PCA: y_axis component must be between 0 and",String(dim-1));
    return;
  }

  disp_log->SetName("PCA: " + GetName() + " Pattern: " + String(pat_no)
    + " X=" + String(x_axis_c) + ", Y=" + String(y_axis_c));

  if(print_eigens) {
    cerr << "full list of eigen values (sorted lowest-to-highest, reverse of component indicies): " << endl;
    evals.List(cerr);		// these are sorted in ascending order
    cerr << endl;
  }

  int x_axis_rev = dim - 1 - x_axis_c; // reverse-order indicies, for accessing data structs
  int y_axis_rev = dim - 1 - y_axis_c;

  float_RArray xevec;		// x eigen vector
  evecs.GetMatCol(dim, xevec, x_axis_rev); // get eigen vector = column of correl_matrix
  if(print_eigens) {
    cerr << "Component no: " << x_axis_c << " has eigenvalue: " << evals[x_axis_rev] << endl;
  }

  float_RArray yevec;		// x eigen vector
  evecs.GetMatCol(dim, yevec, y_axis_rev); // get eigen vector = column of correl_matrix
  if(print_eigens) {
    cerr << "Component no: " << y_axis_c << " has eigenvalue: " << evals[y_axis_rev] << endl;
  }

  float_RArray xprjn;
  float_RArray yprjn;

  ProjectPatterns(xevec, xprjn, pat_no);
  ProjectPatterns(yevec, yprjn, pat_no);

  DataTable* dt = &(disp_log->data);
  dt->StructUpdate(true);
  dt->Reset();
  dt->NewColFloat(String("X = ") + String(x_axis_c));
  dt->NewColFloat(String("Y = ") + String(y_axis_c));
  dt->AddColDispOpt("NEGATIVE_DRAW",1);
  dt->NewColString("Event");
  dt->AddColDispOpt("DISP_STRING", 2);
  dt->AddColDispOpt("AXIS=1", 2); // labels use same axis as y values
  dt->StructUpdate(false);

  dt->DataUpdate(true);
  dt->PutArrayToCol(xprjn, 0);
  dt->PutArrayToCol(yprjn, 1);

  taLeafItr i;
  Event* ev;
  dt->RowAdding();
  FOR_ITR_EL(Event, ev, events., i) {
    dt->AddStringVal_deprecated(ev->name, 2);
  }
  dt->RowAdded();
  dt->DataUpdate(false);
  disp_log->ViewAllData();
//TODO: must get updated somehow  disp_log->InitAllViews();
}

void Environment::MDSPrjnPlot(GraphLog* disp_log, int pat_no, int x_axis_c, int y_axis_c,
			      float_RArray::DistMetric metric, bool norm, float tol,
			      bool print_eigens)
{
  if(disp_log == NULL) {
    disp_log = (GraphLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
    if(disp_log == NULL) return;
  }
  else {
/*TODO:    LogView* lv = (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return;*/
  }

  int dim = events.leaves;

  if((x_axis_c < 0) || (x_axis_c >= dim)) {
    taMisc::Error("*** MDSPrjnPlot: x_axis component must be between 0 and",String(dim-1));
    return;
  }
  if((y_axis_c < 0) || (y_axis_c >= dim)) {
    taMisc::Error("*** MDSPrjnPlot: y_axis component must be between 0 and",String(dim-1));
    return;
  }

  disp_log->SetName("MDS: " + GetName() + " Pattern: " + String(pat_no)
    + " X=" + String(x_axis_c) + ", Y=" + String(y_axis_c));

  float_RArray dist_ary;	// distance array (tri mat format)
  DistArray(dist_ary, pat_no, metric, norm, tol);
  float_RArray dist_mat;
  dist_mat.CopyFmTriMat(dim, dist_ary);

  float_RArray xcoords;
  float_RArray ycoords;
  dist_mat.MDS(dim, xcoords, ycoords, x_axis_c, y_axis_c, print_eigens);

  DataTable* dt = &(disp_log->data);
  dt->Reset();
  dt->NewColFloat(String("X = ") + String(x_axis_c));
  dt->NewColFloat(String("Y = ") + String(y_axis_c));
  dt->AddColDispOpt("NEGATIVE_DRAW",1);
  dt->NewColString("Event");
  dt->AddColDispOpt("DISP_STRING", 2);
  dt->AddColDispOpt("AXIS=1", 2); // labels use same axis as y values
  dt->PutArrayToCol(xcoords, 0);
  dt->PutArrayToCol(ycoords, 1);

  taLeafItr i;
  Event* ev;
  dt->RowAdding();
  FOR_ITR_EL(Event, ev, events., i) {
    dt->AddStringVal_deprecated(ev->name, 2);
  }
  dt->RowAdded();
  disp_log->ViewAllData();
//TODO:must get updated  disp_log->InitAllViews();
}

void Environment::EventPrjnPlot(Event* x_axis_e, Event* y_axis_e, int pat_no, GraphLog* disp_log,
				float_RArray::DistMetric metric, bool norm, float tol)
{
  if(disp_log == NULL) {
    disp_log = (GraphLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
    if(disp_log == NULL) return;
  }
  else {
/*TODO:    LogView* lv = (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return;*/
  }

  if((x_axis_e == NULL) || (y_axis_e == NULL)) return;
  if(x_axis_e->InheritsFrom(TA_Event_MGroup))
    x_axis_e = (Event*)((Event_MGroup*)x_axis_e)->SafeEl(0);
  if(y_axis_e->InheritsFrom(TA_Event_MGroup))
    y_axis_e = (Event*)((Event_MGroup*)y_axis_e)->SafeEl(0);
  if((x_axis_e == NULL) || (y_axis_e == NULL)) return;
  if(!x_axis_e->InheritsFrom(TA_Event) || !y_axis_e->InheritsFrom(TA_Event)) return;

  Pattern* x_axis_pat = (Pattern*)x_axis_e->patterns.Leaf(pat_no);
  if(x_axis_pat == NULL) {
    taMisc::Error("*** Pattern number:", String(pat_no), "not found");
    return;
  }
  Pattern* y_axis_pat = (Pattern*)y_axis_e->patterns.Leaf(pat_no);
  if(y_axis_pat == NULL) {
    taMisc::Error("*** Pattern number:", String(pat_no), "not found");
    return;
  }

  disp_log->SetName("EventPrjn: " + GetName() + " Pattern: " + String(pat_no)
    + " X=" + x_axis_e->GetName() + ", Y=" + y_axis_e->GetName());

  DataTable* dt = &(disp_log->data);
  dt->Reset();
  dt->NewColFloat(String("X = ") + x_axis_e->name);
  dt->NewColFloat(String("Y = ") + y_axis_e->name);
  dt->AddColDispOpt("NEGATIVE_DRAW",1);
  dt->NewColString("Event");
  dt->AddColDispOpt("DISP_STRING", 2);
  dt->AddColDispOpt("AXIS=1", 2); // labels use same axis as y values

  taLeafItr i;
  Event* ev;
  dt->RowAdding();
  FOR_ITR_EL(Event, ev, events., i) {
    dt->AddStringVal_deprecated(ev->name, 2);

    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }

    float xc = trg_pat->value.Dist(x_axis_pat->value, metric, norm, tol);
    float yc = trg_pat->value.Dist(y_axis_pat->value, metric, norm, tol);

    dt->AddFloatVal_deprecated(xc, 0);
    dt->AddFloatVal_deprecated(yc, 1);
  }
  dt->RowAdded();
  disp_log->ViewAllData();
//TODO:must get updated  disp_log->InitAllViews();
}

void Environment::EnvToGrid(GridLog* disp_log, int pat_no, int ev_x, int ev_y, int pt_x, int pt_y) {
  if(event_specs.size == 0) return;
  PatternSpec* ps = (PatternSpec*)((EventSpec*)event_specs.DefaultEl())->patterns[pat_no];
  if(ps == NULL) return;

  if(disp_log == NULL) {
    disp_log = (GridLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GridLog);
    if(disp_log == NULL) return;
  }
  else {
    disp_log->Clear();
/*TODO:    LogView* lv = (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return;*/
  }

  disp_log->SetName((String)"EnvToGrid: " + GetName()  + " Pattern: " + String(pat_no));

  DataTable* md = &(disp_log->data);
  md->RemoveAll();

  md->NewColString("row");

  int evx = ev_x;
  int evy = ev_y;
  if(evx < 0) {
    evx = (int)sqrtf((float)events.leaves);
  }
  if((evy < 0) || (evx * evy < events.leaves)) {
    evy = events.leaves / evx;
    while((evx * evy) < events.leaves)
      evy++;
  }

  int ptx = (pt_x < 0) ? ps->geom.x : pt_x;
  int pty = (pt_y < 0) ? ps->geom.y : pt_y;
  int totn = ptx * pty;

  // these are the columns
  int i;
  for(i=0; i<evx; i++) {
    md->NewGroupFloat((String)"c" + (String)i, totn);
    md->AddColDispOpt(String("GEOM_X=") + String(ptx), 0, i); // column 0, subgp i
    md->AddColDispOpt(String("GEOM_Y=") + String(pty), 0, i); // column 0, subgp i
  }

  md->ResetData();

  for(i=0;i<evy;i++) {
    md->AddBlankRow();		// add the rows
    md->SetStringVal(String(evy - i -1), 0, i);
  }

  int uni = 0;
  int ux, uy;
  for(uy=0;uy<evy; uy++) {
    if(uni >= events.leaves)
      break;
    for(ux=0; ux<evx; ux++, uni++) {
      if(uni >= events.leaves)
	break;
      Pattern* pat = (Pattern*)((Event*)events.Leaf(uni))->patterns[pat_no];
      DataTable* dt = (DataTable*)md->gp.FastEl(ux);
      dt->PutArrayToRow(pat->value, evy-uy-1);
    }
  }

  disp_log->ViewAllData();
}

void Environment::PatFreqGrid(GridLog* disp_log, float act_thresh, bool prop) {
  if(events.leaves == 0)
    return;

  if(event_specs.size == 0) return;

  if(disp_log == NULL) {
    disp_log = (GridLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GridLog);
    if(disp_log == NULL) return;
  }
  else {
/*TODO:    LogView* lv = (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return;*/
  }

  disp_log->SetName( "PatFreqGrid: " + GetName());
  DataTable* dt = &(disp_log->data);
  dt->Reset();

  EventSpec* es = (EventSpec*)event_specs.DefaultEl();
  if(es == NULL) return;
  float_RArray freqs;
  int pat_no;
  for(pat_no = 0; pat_no < es->patterns.size; pat_no++) {
    PatternSpec* ps = (PatternSpec*)es->patterns[pat_no];
    PatFreqArray(freqs, pat_no, act_thresh, prop);

    DataTable* dtgp = dt->NewGroupFloat(String("pattern_") + String(pat_no), freqs.size);

    bool first = true;
    for(int i=0;i<freqs.size;i++) {
      String nm;
      if(!ps->value_names[i].empty())
	nm = ps->value_names[i];
      else
	nm = String("v") + String(i);
      if(first) {
	nm = String("<") + String(ps->geom.x) + ">" + nm;
	first = false;
	String xgeom = "GEOM_X=" + String(ps->geom.x);
	dtgp->AddColDispOpt(xgeom, i);
	String ygeom = "GEOM_Y=" + String(ps->geom.y);
	dtgp->AddColDispOpt(ygeom, i);
	dtgp->AddColDispOpt("USE_EL_NAMES", i); // each column has separate element names in gp
      }
      dtgp->SetColName(nm, i);
    }
    dtgp->RowAdding();
    for(int i=0;i<freqs.size;i++) {
      dtgp->AddFloatVal_deprecated(freqs[i], i); // col cnt, subgp 0
    }
    dtgp->RowAdded();
  }

  disp_log->ViewAllData();
  GridLogView* glv = NULL; //TODO:(GridLogView*)disp_log->views()->SafeEl(0);
  if(glv == NULL) return;
  glv->auto_scale = true;
  glv->AllBlockTextOn();
  glv->SetBlockSizes(20,1);
  glv->UpdateGridLayout();
}

void Environment::PatAggGrid(GridLog* disp_log, Aggregate& agg) {
  if(events.leaves == 0)
    return;

  if(event_specs.size == 0) return;

  if(disp_log == NULL) {
    disp_log = (GridLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GridLog);
    if(disp_log == NULL) return;
  }
  else {
    LogView* lv = NULL; //TODO: (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return;
  }

  disp_log->SetName("PatAggGrid: " + GetName());
  DataTable* dt = &(disp_log->data);
  dt->Reset();

  EventSpec* es = (EventSpec*)event_specs.DefaultEl();
  if(es == NULL) return;
  float_RArray freqs;
  int pat_no;
  for(pat_no = 0; pat_no < es->patterns.size; pat_no++) {
    PatternSpec* ps = (PatternSpec*)es->patterns[pat_no];
    PatAggArray(freqs, pat_no, agg);

    DataTable* dtgp = dt->NewGroupFloat(String("pattern_") + String(pat_no), freqs.size);

    bool first = true;
    for(int i=0;i<freqs.size;i++) {
      String nm;
      if(!ps->value_names[i].empty())
	nm = ps->value_names[i];
      else
	nm = String("v") + String(i);
      if(first) {
	nm = String("<") + String(ps->geom.x) + ">" + nm;
	first = false;
	String xgeom = "GEOM_X=" + String(ps->geom.x);
	dtgp->AddColDispOpt(xgeom, i);
	String ygeom = "GEOM_Y=" + String(ps->geom.y);
	dtgp->AddColDispOpt(ygeom, i);
	dtgp->AddColDispOpt("USE_EL_NAMES", i); // each column has separate element names in gp
      }
      dtgp->SetColName(nm, i);
    }
    dtgp->RowAdding();
    for(int i=0;i<freqs.size;i++) {
      dtgp->AddFloatVal_deprecated(freqs[i], i); // col cnt, subgp 0
    }
    dtgp->RowAdded();
  }

  disp_log->ViewAllData();
  GridLogView* glv = NULL; //TODO:(GridLogView*)disp_log->views()->SafeEl(0);
  if(glv == NULL) return;
  glv->auto_scale = true;
  glv->AllBlockTextOn();
  glv->SetBlockSizes(20,1);
  glv->UpdateGridLayout();
}
#endif // TA_GUI
void Environment::UpdateAllEvents() {
  EventSpec* es;
  taLeafItr i;
  FOR_ITR_EL(EventSpec, es, event_specs., i)
    es->UpdateAllEvents();
}

void Environment::UpdateAllEventSpecs() {
  EventSpec* es;
  taLeafItr i;
  FOR_ITR_EL(EventSpec, es, event_specs., i)
    es->UpdateFromLayers();
}

void Environment::UnSetLayers() {
  EventSpec* es;
  taLeafItr i;
  FOR_ITR_EL(EventSpec, es, event_specs., i)
    es->UnSetLayers();
}

int Environment::GroupCount() {
  if(events.gp.size == 0)
    return 0;
  if(events.leaf_gp == NULL) events.InitLeafGp();
  return events.leaf_gp->size;
}

Event_MGroup* Environment::GetGroup(int i) {
  if(events.gp.size == 0)
    return NULL;
  if(events.leaf_gp == NULL) events.InitLeafGp();
  return (Event_MGroup*)events.leaf_gp->SafeEl(i);
}

Event* Environment::GetNextEvent() {
  if(events.leaves > event_ctr)
    return (Event*)events.Leaf(event_ctr++);
  return NULL;
}

void Environment::UnitNamesToNet(EventSpec* es, Network* net) {
  if(es == NULL) es = (EventSpec*)event_specs.DefaultEl();
  if(es == NULL) return;
  if(net == NULL) net = pdpMisc::GetDefNetwork(GET_MY_OWNER(Project));
  if(net == NULL) return;

  es->ApplyNames(net);
}

void Environment::MakeNetIOLayers(EventSpec* es, Network* net) {
  net->RemoveUnits();
  int i;
  for(i=0;i<es->patterns.size;i++) {
    PatternSpec* ps = (PatternSpec*)es->patterns[i];
    Layer* lay = net->FindMakeLayer(ps->layer_name);
    lay->geom = ps->geom;
    lay->n_units = ps->n_vals;
    lay->UpdateAfterEdit();
  }
  taMisc::DelayedMenuUpdate(net);
}

void Environment::AutoNameAllEvents(float act_thresh, int max_pat_nm, int max_val_nm) {
  Event* ev;
  taLeafItr e;
  StructUpdate(true);
  FOR_ITR_EL(Event, ev, events., e) {
    ev->AutoNameEvent(act_thresh, max_pat_nm, max_val_nm);
  }
  StructUpdate(false);
}

void Environment::WriteText(ostream& strm, int pat_no, TextFmt fmt) {
  Event_MGroup* lst_gp = NULL;
  Event* ev;
  taLeafItr i;
  FOR_ITR_EL(Event, ev, events., i) {
    if(events.size != events.leaves) { // has sub-groups
      Event_MGroup* own = (Event_MGroup*)ev->owner;
      if((own != &events) && (own != lst_gp)) {
	strm << "# startgroup\n";
	lst_gp = own;
      }
    }
    if(fmt == NAME_FIRST)
      strm << ev->name << "\t";
    Pattern* pat;
    if((pat_no >= 0) || (ev->patterns.leaves == 1)) {
      if(ev->patterns.leaves == 1)
	pat_no = 0;
      pat = (Pattern*)ev->patterns.Leaf(pat_no);
      int j;
      for(j=0; j<pat->value.size-1; j++)
	strm << pat->value.FastEl(j) << "\t";
      strm << pat->value.FastEl(j);
      if(fmt == NAME_LAST)
	strm << "\t" << ev->name;
      strm << "\n";
    }
    else {
      taLeafItr pi;
      FOR_ITR_EL(Pattern, pat, ev->patterns., pi) {
	int j;
	for(j=0; j<pat->value.size-1; j++)
	  strm << pat->value.FastEl(j) << "\t";
	strm << pat->value.FastEl(j) << "\n";
      }
      if(fmt == NAME_LAST)
	strm << ev->name << "\n";
    }
  }
}

// this is the reader for Old pdp format and other text formats
void Environment::ReadText(istream& strm, EventSpec* es, TextFmt fmt) {
  if((es == NULL) || (es->patterns.leaves == 0))  {
    taMisc::Error("ReadText: EventSpec was NULL or has no patterns");
    return;
  }

  Pattern* cur_pat = NULL;
  Event_MGroup* cur_gp = &events;
  Event* cur_ev = NULL;
  taLeafItr pat_itr;
  int cur_val_indx = 0;

  StructUpdate(true);
  strm.seekg(0);
  while(strm.good() && !strm.eof()) {
    int c = taMisc::read_alnum(strm);
    if((c == EOF) || taMisc::LexBuf.empty())
      continue;
    int fc = taMisc::LexBuf.firstchar();
    // skip comments
    if((fc == '#') || (taMisc::LexBuf.before(2) == "//")) {
      if(c != '\n')
	taMisc::read_till_eol(strm);
      taMisc::LexBuf.downcase();
      if(taMisc::LexBuf.contains("startgroup")) {
	cur_gp = (Event_MGroup*)events.NewGp(1);
      }
      continue; // just a comment
    }
    if(cur_pat == NULL) {	// start of an event
      bool cont = false;
      if((cur_ev != NULL) && (fmt == NAME_LAST)) {
	cur_ev->name = taMisc::LexBuf;
	cont = true;
      }
      cur_ev = (Event*) cur_gp->NewFmSpec(1, NULL, es);
      cur_pat = (Pattern*)cur_ev->patterns.FirstEl(pat_itr);
      cur_val_indx = 0;
      if(fmt == NAME_FIRST) {
	cur_ev->name = taMisc::LexBuf;
	continue;		// done reading, get next
      }
      if(cont) continue;	// read in a name, so continue
    }
    if(cur_val_indx >= cur_pat->value.size) {
      cur_pat = (Pattern*)cur_ev->patterns.NextEl(pat_itr);
      cur_val_indx = 0;
      if(cur_pat == NULL) {
	if(fmt == NAME_LAST) {
	  cur_ev->name = taMisc::LexBuf;
	}
	cur_ev = (Event*) cur_gp->NewFmSpec(1, NULL, es);
	cur_pat = (Pattern*)cur_ev->patterns.FirstEl(pat_itr);
	if(fmt == NAME_FIRST) {
	  cur_ev->name = taMisc::LexBuf;
	  continue;		// done reading, get next
	}
	if(fmt == NAME_LAST) continue; // read it in and need to continue
      }
    }
    if(!(isdigit(fc) || (fc == '.') || (fc == '-'))) {
      taMisc::Error("*** ReadText Warning: expecting number and didn't get it:", taMisc::LexBuf,"possible format errors");
    }
    float val = (float)taMisc::LexBuf;
    cur_pat->value.Set(cur_val_indx++, val);
  }
  StructUpdate(true);
}

void Environment::ReadBinary(istream& strm, EventSpec* es) {
  if((es == NULL) || (es->patterns.leaves == 0))  {
    taMisc::Error("ReadBinary: EventSpec was NULL or has no patterns");
    return;
  }
  Pattern* cur_pat = NULL;
  Event* cur_ev = NULL;
  taLeafItr pat_itr;
  int cur_val_indx = 0;

  StructUpdate(true);
  strm.seekg(0);
  while(strm.good() && !strm.eof()) {
    if(cur_pat == NULL) {
      cur_ev = (Event*)events.NewFmSpec(1, NULL, es);
      cur_pat = (Pattern*)cur_ev->patterns.FirstEl(pat_itr);
      cur_val_indx = 0;
    }
    if(cur_val_indx >= cur_pat->value.size) {
      cur_pat = (Pattern*)cur_ev->patterns.NextEl(pat_itr);
      cur_val_indx = 0;
      if(cur_pat == NULL)
	continue;		// we're done with this pattern, get new event..
    }
    // just read and plug!
    float cur_val;
    strm.read((char*)&cur_val, sizeof(float));
    cur_pat->value.Set(cur_val_indx++, cur_val);
  }
  StructUpdate(false);
}

void Environment::WriteBinary(ostream& strm) {
  Event* ev;
  taLeafItr i;
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* pat;
    taLeafItr pi;
    FOR_ITR_EL(Pattern, pat, ev->patterns., pi) {
      int j;
      for(j=0; j<pat->value.size; j++) {
	float cur_val = pat->value.FastEl(j);
	strm.write((char*)&cur_val, sizeof(float));
      }
    }
  }
}

///////////////////////////////
//   Generation Routines     //
///////////////////////////////

void Environment::ReplicateEvents(int n_replicas, bool make_groups) {
  StructUpdate(true);
  if(make_groups) {
    int i;
    for(i=0; i<events.size; i++) {
      Event* evt = (Event*)events.FastEl(i);
      String base_nm = evt->name;
      Event_MGroup* gp = (Event_MGroup*)events.NewGp(1);
      gp->taList_impl::name = evt->name + "_r";
      gp->Transfer(evt);
      evt->name += "_r0";
      int j;
      for(j=0; j<n_replicas; j++) {
	Event* nw_ev = (Event*)evt->Clone();
	nw_ev->name = base_nm + "_r" + String(j+1);
	gp->Add(nw_ev);
      }
      i--;
    }
  }
  else {
    int i;
    for(i=0; i<events.size; i++) {
      Event* evt = (Event*)events.FastEl(i);
      String base_nm = evt->name;
      evt->name += "_r0";
      int j;
      for(j=0; j<n_replicas; j++) {
	Event* nw_ev = (Event*)evt->Clone();
	nw_ev->name = base_nm + "_r" + String(j+1);
	events.Insert(nw_ev, ++i);
      }
    }
  }
  StructUpdate(false);
}

void Environment::PermutedBinary(int pat_no, int n_on) {
  DataUpdate(true);
  if(pat_no < 0) {
    EventSpec* es = (EventSpec*)event_specs.DefaultEl();
    if(es == NULL) {DataUpdate(false); return;}
    for(int pn = 0; pn < es->patterns.size; pn++) {
      PermutedBinary(pn, n_on);
    }
    {DataUpdate(false); return;}
  }
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      {DataUpdate(false); return;}
    }
    PermutedBinaryPat(pat, n_on);
  }

  DataUpdate(false);
}

void Environment::PermutedBinary_MinDist(int pat_no, int n_on, float dist,
					 float_RArray::DistMetric metric,
					 bool norm, float tol)
{
  bool larger_further;
  int bogus_count = 0;
  int ev_ctr = 0;
  Pattern* pat;
  int cnt;   // 100 plus 10 more for every new stim
  bool ok;
  float min_d;
  DataUpdate(true);
  if(pat_no < 0) {
    EventSpec* es = (EventSpec*)event_specs.DefaultEl();
    if(es == NULL) {DataUpdate(false); return;}
    for(int pn = 0; pn < es->patterns.size; pn++) {
      PermutedBinary_MinDist(pn, n_on, dist, metric, norm, tol);
    }
    {DataUpdate(false); return;}
  }
  larger_further = float_RArray::LargerFurther(metric);
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      {DataUpdate(false); return;}
    }
    cnt = 100 + (10 * (ev_ctr + 1));   // 100 plus 10 more for every new stim
    ok = false;
    do {
      PermutedBinaryPat(pat, n_on);
      min_d = LastMinDist(ev_ctr, pat_no, metric, norm, tol);
      cnt--;
      if(larger_further)
	ok = (min_d >= dist);
      else
	ok = (min_d <= dist);
    } while(!ok && (cnt > 0));

    if(cnt == 0) {
      taMisc::Error("*** Event:", ev->name, "dist of:", (String)min_d,
		     "under dist limit:", (String)dist);
      bogus_count++;
    }
    if(bogus_count > 5) {
      taMisc::Error("Giving up after 5 stimuli under the limit, set limits lower");
      {DataUpdate(false); return;}
    }
    ev_ctr++;
  }

  DataUpdate(false);
}

float Environment::LastMinDist(int n, int pat_no, float_RArray::DistMetric metric,
			       bool norm, float tol)
{
  bool larger_further = float_RArray::LargerFurther(metric);
  float rval;
  if(larger_further)
    rval = FLT_MAX;
  else
    rval  = -FLT_MAX;
  if(n == 0) return rval;

  n = MIN(events.leaves-1, n);
  Event* trg_ev = (Event*)events.Leaf(n);
  Pattern* trg_pat = (Pattern*)trg_ev->patterns.Leaf(pat_no);
  if(trg_pat == NULL) {
    taMisc::Error("*** Pattern number:", String(pat_no), "not found");
    return rval;
  }

  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    if(ev == trg_ev) break;
    Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return rval;
    }
    float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
    if(larger_further)
      rval = MIN(dst, rval);
    else
      rval = MAX(dst, rval);
  }
  return rval;
}

void Environment::FlipBits(int pat_no, int n_off, int n_on) {
  DataUpdate(true);
  if(pat_no < 0) {
    EventSpec* es = (EventSpec*)event_specs.DefaultEl();
    if(es == NULL) {DataUpdate(false); return;}
    for(int pn = 0; pn < es->patterns.size; pn++) {
      FlipBits(pn, n_off, n_on);
    }
    {DataUpdate(false); return;}
  }
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      {DataUpdate(false); return;}
    }
    FlipBitsPat(pat, n_off, n_on);
  }

  DataUpdate(false);
}

void Environment::FlipBits_MinMax(int pat_no, int n_off, int n_on, float min_dist, float max_dist,
				  float_RArray::DistMetric metric, bool norm, float tol)
{
  DataUpdate(true);
  if(pat_no < 0) {
    EventSpec* es = (EventSpec*)event_specs.DefaultEl();
    if(es == NULL) {DataUpdate(false); return;}
    for(int pn = 0; pn < es->patterns.size; pn++) {
      FlipBits_MinMax(pn, n_off, n_on, min_dist, max_dist, metric, norm, tol);
    }
    {DataUpdate(false); return;}
  }
  float_RArray orig_pat;
  int bogus_count = 0;
  int ev_ctr = 0;
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      {DataUpdate(false); return;}
    }
    int cnt = 100 + (10 * (ev_ctr + 1));   // 100 plus 10 more for every new stim
    bool ok = false;
    float min_d, max_d;
    orig_pat.Reset();
    orig_pat = pat->value;
    do {
      FlipBitsPat(pat, n_off, n_on);
      min_d = LastMinMaxDist(ev_ctr, pat_no, max_d, metric, norm, tol);
      cnt--;
      ok = ((min_d >= min_dist) && (max_d <= max_dist));
      if(!ok)			// restore original pattern if not ok..
	pat->value = orig_pat;
    } while(!ok && (cnt > 0));

    if(cnt == 0) {
      taMisc::Error("*** Event:", ev->name, "min/max dist of:", String(min_d),
		    String(max_d), "not within dist limits:", String(min_dist),
		    String(max_dist));
      bogus_count++;
    }
    if(bogus_count > 5) {
      taMisc::Error("Giving up after 5 stimuli under the limit, set limits lower");
      {DataUpdate(false); return;}
    }
    ev_ctr++;
  }

  DataUpdate(false);
}

float Environment::LastMinMaxDist(int n, int pat_no, float& max_dist,
				  float_RArray::DistMetric metric, bool norm, float tol)
{
  float rval = FLT_MAX;
  max_dist = 0;
  if(n == 0) return rval;

  n = MIN(events.leaves-1, n);
  Event* trg_ev = (Event*)events.Leaf(n);
  Pattern* trg_pat = (Pattern*)trg_ev->patterns.Leaf(pat_no);
  if(trg_pat == NULL) {
    taMisc::Error("*** Pattern number:", String(pat_no), "not found");
    return rval;
  }

  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    if(ev == trg_ev) break;
    Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return rval;
    }
    float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
    rval = MIN(dst, rval);
    max_dist = MAX(dst, max_dist);
  }
  return rval;
}

void Environment::FlipBits_GpMinMax(int pat_no, int n_off, int n_on,
				    float within_min_dist, float within_max_dist,
				    float between_dist, float_RArray::DistMetric metric,
				    bool norm, float tol, int st_gp, int ed_gp)
{
  DataUpdate(true);
  if(pat_no < 0) {
    EventSpec* es = (EventSpec*)event_specs.DefaultEl();
    if(es == NULL) {DataUpdate(false); return;}
    for(int pn = 0; pn < es->patterns.size; pn++) {
      FlipBits_GpMinMax(pn, n_off, n_on, within_min_dist, within_max_dist, between_dist,
			metric, norm, tol, st_gp, ed_gp);
    }
    {DataUpdate(false); return;}
  }
  bool larger_further = float_RArray::LargerFurther(metric);
  float_RArray orig_pat;
  int bogus_count = 0;
  int g;
  int mx_gp = events.gp.size;
  if(ed_gp >= 0)
    mx_gp = MIN(ed_gp, mx_gp);
  for(g = st_gp; g < mx_gp; g++) {
    Event_MGroup* gp = (Event_MGroup*)events.gp.FastEl(g);

    int ev_ctr = 0;
    taLeafItr i;
    Event* ev;
    FOR_ITR_EL(Event, ev, gp->, i) {
      Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
      if(pat == NULL) {
	taMisc::Error("*** Pattern number:", String(pat_no), "not found");
	{DataUpdate(false); return;}
      }
      int cnt = 100 + (10 * (ev_ctr + 1));   // 100 plus 10 more for every new stim
      bool ok = false;
      float min_d, max_d, min_w, max_w;
      min_w = FLT_MAX;
      orig_pat.Reset();
      orig_pat = pat->value;
      do {
	FlipBitsPat(pat, n_off, n_on);
	min_d = GpWithinMinMaxDist(gp, ev_ctr, pat_no, max_d, metric, norm, tol);
	cnt--;
	ok = ((min_d >= within_min_dist) && (max_d <= within_max_dist));
	if(!ok)			// restore original pattern if not ok..
	  pat->value = orig_pat;
	else {
	  min_w = GpLastMinMaxDist(g, pat, pat_no, max_w, metric, norm, tol);
	  if(larger_further)
	    ok = (min_w >= between_dist);
	  else
	    ok = (max_w <= between_dist); // todo: check this!
	  if(!ok)
	    pat->value = orig_pat;
	}
      } while(!ok && (cnt > 0));

      if(cnt == 0) {
	taMisc::Error("*** Event:", ev->name, "within min/max dist of:", String(min_d),
		      String(max_d), "not within limits:", String(within_min_dist),
		      String(within_max_dist), "or between:", String(min_w),
		      "over:",String(between_dist));
	bogus_count++;
      }
      if(bogus_count > 5) {
	taMisc::Error("Giving up after 5 stimuli under the limit, set limits lower");
	{DataUpdate(false); return;}
      }
      ev_ctr++;
    }
  }

  DataUpdate(false);
}

float Environment::GpWithinMinMaxDist(Event_MGroup* gp, int n, int pat_no, float& max_dist,
				      float_RArray::DistMetric metric, bool norm, float tol)
{
  float rval = FLT_MAX;
  max_dist = 0;
  if(n == 0) return rval;

  n = MIN(gp->leaves-1, n);
  Event* trg_ev = (Event*)gp->Leaf(n);
  Pattern* trg_pat = (Pattern*)trg_ev->patterns.Leaf(pat_no);
  if(trg_pat == NULL) {
    taMisc::Error("*** Pattern number:", String(pat_no), "not found");
    return rval;
  }

  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, gp->, i) {
    if(ev == trg_ev) break;
    Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return rval;
    }
    float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
    rval = MIN(dst, rval);
    max_dist = MAX(dst, max_dist);
  }
  return rval;
}

float Environment::GpMinMaxDist(Event_MGroup* gp, Pattern* trg_pat, int pat_no, float& max_dist,
				float_RArray::DistMetric metric, bool norm, float tol)
{
  float rval = FLT_MAX;
  max_dist = 0;
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, gp->, i) {
    Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return rval;
    }
    float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
    rval = MIN(dst, rval);
    max_dist = MAX(dst, max_dist);
  }
  return rval;
}

float Environment::GpLastMinMaxDist(int gp_no, Pattern* trg_pat, int pat_no, float& max_dist,
				    float_RArray::DistMetric metric, bool norm, float tol, int st_gp)
{
  float rval = FLT_MAX;
  max_dist = 0;
  int mx_gp = MIN(gp_no, events.gp.size);
  int g;
  for(g=st_gp; g<mx_gp; g++) {
    Event_MGroup* gp = (Event_MGroup*)events.gp.FastEl(g);
    float mx_dst;
    float dst = GpMinMaxDist(gp, trg_pat, pat_no, mx_dst, metric, norm, tol);
    rval = MIN(dst, rval);
    max_dist = MAX(mx_dst, max_dist);
  }
  return rval;
}

void Environment::Clear(int pat_no, float val) {
  DataUpdate(true);
  if(pat_no < 0) {
    EventSpec* es = (EventSpec*)event_specs.DefaultEl();
    if(es == NULL) {DataUpdate(false); return;}
    for(int pn = 0; pn < es->patterns.size; pn++) {
      Clear(pn, val);
    }
    {DataUpdate(false); return;}
  }
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      {DataUpdate(false); return;}
    }
    pat->value.InitVals(val);
  }

  DataUpdate(false);
}

void Environment::AddNoise(int pat_no, const Random& rnd_spec) {
  DataUpdate(true);
  if(pat_no < 0) {
    EventSpec* es = (EventSpec*)event_specs.DefaultEl();
    if(es == NULL) {DataUpdate(false); return;}
    for(int pn = 0; pn < es->patterns.size; pn++) {
      AddNoise(pn, rnd_spec);
    }
    {DataUpdate(false); return;}
  }
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      {DataUpdate(false); return;}
    }
    AddNoisePat(pat, rnd_spec);
  }

  DataUpdate(false);
}

void Environment::TransformPats(int pat_no, const SimpleMathSpec& trans) {
  DataUpdate(true);
  if(pat_no < 0) {
    EventSpec* es = (EventSpec*)event_specs.DefaultEl();
    if(es == NULL) {DataUpdate(false); return;}
    for(int pn = 0; pn < es->patterns.size; pn++) {
      TransformPats(pn, trans);
    }
    {DataUpdate(false); return;}
  }
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      {DataUpdate(false); return;}
    }
    pat->value.SimpleMath(trans);
  }
  DataUpdate(false);
}

// DistMatrixGrid is in pdpshell.cc

void Environment::DistMatrix(ostream& strm, int pat_no, float_RArray::DistMetric metric,
			     bool norm, float tol, DistMatFmt format, int precision)
{
  if(events.leaves == 0)
    return;
  if(precision < 0) precision = 3;
  if(format == GRID_LOG)
    strm << "_H:\t";
  if(format != PRINT_NO_LABELS) {
    String nm = "$Event";
    PDPLog::LogColumn(strm, nm, 2);
  }

  int dist_wdth = 2;		// width of output
  if(metric != float_RArray::HAMMING)
    dist_wdth = 7;
  else
    precision = 0;

  bool first = true;
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    String nm = ev->name;
    if((format == GRID_LOG) && first) {
      nm = String("<") + String(events.leaves) + ">" + nm;
      first = false;
    }
    if((int)nm.length() > dist_wdth)
      nm = nm.before(dist_wdth);
    if(dist_wdth == 2) {
      strm << nm << " ";
      if(nm.length() == 1)
	strm << " ";
    }
    else
      PDPLog::LogColumn(strm, nm, 1);
  }
  strm << "\n";

  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }
    if(format == GRID_LOG)
      strm << "_D:\t";
    if(format != PRINT_NO_LABELS)
      PDPLog::LogColumn(strm, ev->name, 2);
    taLeafItr j;
    Event* oev;
    FOR_ITR_EL(Event, oev, events., j) {
      if((format != GRID_LOG) && (oev == ev)) break;
      Pattern* pat = (Pattern*)oev->patterns.Leaf(pat_no);
      if(pat == NULL) {
	taMisc::Error("*** Pattern number:", String(pat_no), "not found");
	return;
      }
      float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
      if(dist_wdth == 2) {
	String val = taMisc::LeadingZeros((int)dst, dist_wdth);
	strm << val << " ";
	if(val.length() == 1)
	  strm << " ";
      }
      else {
	String val = taMisc::FormatValue(dst, dist_wdth, precision);
	PDPLog::LogColumn(strm, val, 1);
      }
    }
    strm << "\n";
  }
}

void Environment::DistArray(float_RArray& dist_ary, int pat_no,
			    float_RArray::DistMetric metric, bool norm, float tol)
{
  dist_ary.Reset();
  if(events.leaves == 0)
    return;
  for(int i=0;i<events.leaves;i++) {
    Event* ev = (Event*)events.Leaf(i);
    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }
    for(int j=i;j<events.leaves;j++) {
      Event* oev = (Event*)events.Leaf(j);
      Pattern* pat = (Pattern*)oev->patterns.Leaf(pat_no);
      if(pat == NULL) {
	taMisc::Error("*** Pattern number:", String(pat_no), "not found");
	return;
      }
      float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
      dist_ary.Add(dst);
    }
  }
}

void Environment::GpDistArray(float_RArray& within_dist_ary, float_RArray& between_dist_ary,
			      int pat_no, float_RArray::DistMetric metric, bool norm, float tol)
{
  between_dist_ary.Reset();
  within_dist_ary.Reset();
  if(events.leaves == 0)
    return;

  for(int g=0; g < events.gp.size; g++) {
    Event_MGroup* gp = (Event_MGroup*)events.gp[g];
    for(int i=0; i<gp->size; i++) {
      Event* ev = (Event*)gp->FastEl(i);
      Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
      if(trg_pat == NULL) {
	taMisc::Error("*** Pattern number:", String(pat_no), "not found");
	return;
      }
      // the next loop computes within-group distance
      for(int j=i; j < gp->size; j++) {	// only does upper-triangle on within
	Event* oev = (Event*)gp->FastEl(j);
	Pattern* pat = (Pattern*)oev->patterns.Leaf(pat_no);
	if(pat == NULL) {
	  taMisc::Error("*** Pattern number:", String(pat_no), "not found");
	  return;
	}
	float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
	within_dist_ary.Add(dst);
      }

      // the next loop computes between-group distance
      for(int k=g; k < events.gp.size; k++) {
	Event_MGroup* ogp = (Event_MGroup*)events.gp[g];
	for(int l=0; l < ogp->size; l++) {
	  Event* oev = (Event*)ogp->FastEl(l);
	  Pattern* pat = (Pattern*)oev->patterns.Leaf(pat_no);
	  if(pat == NULL) {
	    taMisc::Error("*** Pattern number:", String(pat_no), "not found");
	    return;
	  }
	  float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
	  between_dist_ary.Add(dst);
	}
      }
    }
  }
}

void Environment::CmpDistMatrix(ostream& strm, int pat_no, Environment* cmp_env, int cmp_pat_no,
				float_RArray::DistMetric metric, bool norm, float tol, DistMatFmt format)
{
  if((events.leaves == 0) || (cmp_env == NULL) || (cmp_env->events.leaves == 0))
    return;

  int precision = 3;
  if(format == GRID_LOG)
    strm << "_H:\t";
  if(format != PRINT_NO_LABELS) {
    String nm = "$Event";
    PDPLog::LogColumn(strm, nm, 2);
  }

  int dist_wdth = 2;
  if(metric != float_RArray::HAMMING)
    dist_wdth = 7;
  else
    precision = 0;

  bool first = true;
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, cmp_env->events., i) {
    String nm = ev->name;
    if((format == GRID_LOG) && first) {
      nm = String("<") + String(cmp_env->events.leaves) + ">" + nm;
      first = false;
    }
    if((int)nm.length() > dist_wdth)
      nm = nm.before(dist_wdth);
    if(dist_wdth == 2) {
      strm << nm << " ";
      if(nm.length() == 1)
	strm << " ";
    }
    else
      PDPLog::LogColumn(strm, nm, 1);
  }
  strm << "\n";

  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }
    if(format == GRID_LOG)
      strm << "_D:\t";
    if(format != PRINT_NO_LABELS)
      PDPLog::LogColumn(strm, ev->name, 2);
    taLeafItr j;
    Event* oev;
    FOR_ITR_EL(Event, oev, cmp_env->events., j) {
      Pattern* pat = (Pattern*)oev->patterns.Leaf(pat_no);
      if(pat == NULL) {
	taMisc::Error("*** Pattern number:", String(cmp_pat_no), "not found");
	return;
      }
      float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
      if(dist_wdth == 2) {
	String val = taMisc::LeadingZeros((int)dst, dist_wdth);
	strm << val << " ";
	if(val.length() == 1)
	  strm << " ";
      }
      else {
	String val = taMisc::FormatValue(dst, dist_wdth, precision);
	PDPLog::LogColumn(strm, val, 1);
      }
    }
    strm << "\n";
  }
}

void Environment::CmpDistArray(float_RArray& dist_ary, int pat_no, Environment* cmp_env, int cmp_pat_no,
			       float_RArray::DistMetric metric, bool norm, float tol)
{
  dist_ary.Reset();
  if(events.leaves == 0)
    return;
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }
    cmp_env->CmpDistArrayPat(dist_ary, trg_pat, cmp_pat_no, metric, norm, tol);
  }
}

void Environment::CmpDistArrayPat(float_RArray& dist_ary, Pattern* trg_pat,
				  int cmp_pat_no, float_RArray::DistMetric metric,
				  bool norm, float tol)
{
  taLeafItr j;
  Event* oev;
  FOR_ITR_EL(Event, oev, events., j) {
    Pattern* pat = (Pattern*)oev->patterns.Leaf(cmp_pat_no);
    if(pat == NULL) {
      taMisc::Error("*** Pattern number:", String(cmp_pat_no), "not found");
      return;
    }
    float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
    dist_ary.Add(dst);
  }
}

void Environment::PermutedBinaryPat(Pattern* pat, int n_on, float on_val, float off_val) {
  int n_max = MIN(pat->value.size, n_on);
  int i;
  for(i=0; i<n_max; i++)
    pat->value.Set(i, on_val);
  for(;i<pat->value.size;i++)
    pat->value.Set(i, off_val);
  pat->value.Permute();
}

void Environment::FlipBitsPat(Pattern* pat, int n_off, int n_on) {
  int_Array on_ary, off_ary;
  int i;
  for(i=0; i<pat->value.size; i++) {
    if(pat->value.FastEl(i) == 1.0)
      on_ary.Add(i);
    else
      off_ary.Add(i);
  }
  on_ary.Permute();
  off_ary.Permute();
  int n_max = MIN(on_ary.size, n_off);
  for(i=0; i<n_max; i++)
    pat->value.Set(on_ary.FastEl(i), 0.0f);
  n_max = MIN(off_ary.size, n_on);
  for(i=0; i<n_max; i++)
    pat->value.Set(off_ary.FastEl(i), 1.0f);
}

void Environment::AddNoisePat(Pattern* pat, const Random& rnd_spec) {
  int i;
  for(i=0; i<pat->value.size; i++)
    pat->value.Set(i, pat->value.FastEl(i) + rnd_spec.Gen());
}

// ClusterPlot is in pdpshell.cc

void Environment::ValOverEventsArray(float_RArray& ary, int pat_no, int val_no) {
  ary.Reset();
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }
    ary.Add(trg_pat->value.SafeEl(val_no));
  }
}


void Environment::CorrelMatrix(float_RArray& mat, int pat_no, int& dim) {
  if(events.leaves <= 0) return;

  Event* ev = (Event*)events.Leaf(0);
  Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
  if(trg_pat == NULL) {
    taMisc::Error("*** Pattern number:", String(pat_no), "not found");
    return;
  }

  dim = trg_pat->value.size;
  mat.AllocTriMatSize(dim); // upper-triangular matrix, incl diagonal

  float_RArray p1vals;
  float_RArray p2vals;

  for(int i=0;i<dim;i++) {
    ValOverEventsArray(p1vals, pat_no, i);
    for(int j=i;j<dim;j++) {
      ValOverEventsArray(p2vals, pat_no, j);
      mat.FastTriMatEl(dim, i, j) = p1vals.Correl(p2vals);
    }
  }
}

void Environment::PCAEigens(float_RArray& evecs, float_RArray& evals, int pat_no, int& dim) {
  float_RArray correl_mat;
  CorrelMatrix(correl_mat, pat_no, dim);
  evecs.CopyFmTriMat(dim, correl_mat);
  evecs.Eigens(dim, evals);
}

void Environment::ProjectPatterns(const float_RArray& prjn_vector, float_RArray& vals, int pat_no) {
  if(events.leaves <= 0) return;
  vals.Reset();
  taLeafItr i;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., i) {
    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }
    vals.Add(trg_pat->value.InnerProd(prjn_vector));
  }
}

void Environment::PatFreqArray(float_RArray& freqs, int pat_no, float act_thresh, bool prop) {
  freqs.Reset();
  float_RArray cnts;
  taLeafItr ei;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., ei) {
    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }
    for(int i=0;i<trg_pat->value.size;i++) {
      if(freqs.size < trg_pat->value.size) {
	freqs.EnforceSize(trg_pat->value.size);
	cnts.EnforceSize(trg_pat->value.size);
      }
      float val = (trg_pat->value[i] > act_thresh) ? 1.0f : 0.0f;
      freqs[i] += val;
      cnts[i] += 1.0f;
    }
  }
  if(prop) {
    for(int i=0;i<freqs.size;i++) {
      if(cnts[i] > 0.0f) freqs[i] /= cnts[i];
    }
  }
}

void Environment::PatFreqText(float act_thresh, bool prop, ostream& strm) {
  EventSpec* es = (EventSpec*)event_specs.DefaultEl();
  if(es == NULL) return;
  float_RArray freqs;
  int pat_no;
  for(pat_no = 0; pat_no < es->patterns.size; pat_no++) {
    PatternSpec* ps = (PatternSpec*)es->patterns[pat_no];
    PatFreqArray(freqs, pat_no, act_thresh, prop);
    int col_cnt = 0;
    strm << "Pattern: " << pat_no << " " << ps->name << endl;
    for(int i=0; i<freqs.size; i++) {
      col_cnt += 8;
      if(col_cnt > taMisc::display_width) {
	strm << endl;
	col_cnt = 0;
      }
      strm << taMisc::StringMaxLen(ps->value_names.SafeEl(i), 7) << "\t";
    }
    strm << endl;
    col_cnt = 0;
    for(int i=0; i<freqs.size; i++) {
      col_cnt += 8;
      if(col_cnt > taMisc::display_width) {
	strm << endl;
	col_cnt = 0;
      }
      strm << taMisc::FormatValue(freqs[i], 7, 4) << "\t";
    }
    strm << endl;
  }
}

void Environment::PatAggArray(float_RArray& agg_vals, int pat_no, Aggregate& agg) {
  agg_vals.Reset();
  EventSpec* es = (EventSpec*)event_specs.DefaultEl();
  if(es == NULL) return;
  PatternSpec* ps = (PatternSpec*)es->patterns.Leaf(pat_no);
  if(ps == NULL) {
    taMisc::Error("*** Pattern number:", String(pat_no), "not found");
    return;
  }
  agg.Init();
  agg_vals.EnforceSize(ps->n_vals);
  agg_vals.InitVals(agg.InitAggVal());

  taLeafItr ei;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., ei) {
    Pattern* trg_pat = (Pattern*)ev->patterns.Leaf(pat_no);
    if(trg_pat == NULL) {
      taMisc::Error("*** Pattern number:", String(pat_no), "not found");
      return;
    }
    agg_vals.AggToArray(trg_pat->value, agg);
    agg.IncUpdt();
  }
}

void Environment::PatAggText(Aggregate& agg, ostream& strm) {
  EventSpec* es = (EventSpec*)event_specs.DefaultEl();
  if(es == NULL) return;
  float_RArray freqs;
  int pat_no;
  for(pat_no = 0; pat_no < es->patterns.size; pat_no++) {
    PatternSpec* ps = (PatternSpec*)es->patterns[pat_no];
    PatAggArray(freqs, pat_no, agg);
    int col_cnt = 0;
    strm << "Pattern: " << pat_no << " " << ps->name << endl;
    for(int i=0; i<freqs.size; i++) {
      col_cnt += 8;
      if(col_cnt > taMisc::display_width) {
	strm << endl;
	col_cnt = 0;
      }
      strm << taMisc::StringMaxLen(ps->value_names.SafeEl(i), 7) << "\t";
    }
    strm << endl;
    col_cnt = 0;
    for(int i=0; i<freqs.size; i++) {
      col_cnt += 8;
      if(col_cnt > taMisc::display_width) {
	strm << endl;
	col_cnt = 0;
      }
      strm << taMisc::FormatValue(freqs[i], 7, 4) << "\t";
    }
    strm << endl;
  }
}

void Environment::EventFreqText(bool prop, ostream& strm) {
  String tname = GetName();
  strm << "\nEvent Frequencies for Environment: " << tname << endl;
  float_RArray freqs;
  String_Array nms;
  taLeafItr ei;
  Event* ev;
  FOR_ITR_EL(Event, ev, events., ei) {
    int nmi = nms.Find(ev->name);
    if(nmi < 0) {
      nms.Add(ev->name);
      freqs.Add(1.0f);
    }
    else {
      freqs[nmi]+= 1.0f;
    }
  }
  if(nms.size > 1000) {
    int chs = taMisc::Choice("Over 1,000 events to be displayed (" + String(nms.size)
			     + ") are you sure you want to proceed?", "Ok", "Cancel");
    if(chs == 1) return;
  }
  for(int i=0;i<nms.size;i++) {
    strm << taMisc::StringEnforceLen(nms[i], 63) << "\t";
    if(prop)
      strm << taMisc::FormatValue(freqs[i] / (float)events.leaves, 7, 4);
    else
      strm << freqs[i];
    strm << endl;
  }
}

