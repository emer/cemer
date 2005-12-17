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



// event.cpp


#include "event.h"
#include "netstru.h"
//#include "sched_proc.h" // for trialproc check in enview::update_display
#include "pdpshell.h"

#ifdef TA_GUI
#include "ta_qtgroup.h"
#include "pdplog_qtso.h"
#endif


#include <math.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>

/* NOTES:




*/
/* obs
////////////////////////////
//   LayerWriter_Group    //
////////////////////////////

void LayerWriter_Group::Initialize() {
  pat_gp_type = &TA_Pattern_Group;
  SetBaseType(&TA_LayerWriter);
}

void LayerWriter_Group::Destroy() {
  CutLinks();
}

void LayerWriter_Group::Copy_(const LayerWriter_Group& cp) {
  pat_gp_type = cp.pat_gp_type;
}

void LayerWriter_Group::UpdateAfterEdit() {
  taBase_Group::UpdateAfterEdit();
  if(!taMisc::is_loading) {
    NetConduit* es = GET_MY_OWNER(NetConduit);
    if(es == NULL)
      return;
    es->UnSetLayers();
    if(es->DetectOverlap()) {
      int choice = taMisc::Choice("Overlap of LayerWriters",
				  "Enforce Linear Layout", "Ignore");
      if(choice == 0) es->LinearLayout();
    }
  }
}

void LayerWriter_Group::LinearLayout() {
  NetConduit* es = GET_MY_OWNER(NetConduit);
  if(es == NULL) return;
  es->LinearLayout();
}

void LayerWriter_Group::CutLinks() {
  if((pat_gp_type == NULL) || (owner == NULL)) {
    taBase_Group::CutLinks();
    return;
  }
  pat_gp_type = NULL;		// don't do it more than once

  if(!owner->InheritsFrom(TA_taSubGroup)) { // only for created groups
    taBase_Group::CutLinks();
    return;
  }
  NetConduit* es = GET_MY_OWNER(NetConduit);
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

TAPtr LayerWriter_Group::NewEl(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
    if(taMisc::gui_active)
      return gpiGroupNew::New(this,typ);
#endif
    return NULL;
  }
  NetConduit* es = GET_MY_OWNER(NetConduit);
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
      ((LayerWriter*)FastEl(i))->NewPattern(ev, pg);
  }
  es->UnSetLayers();
  es->UpdateChildren();
  return rval;
}

taGroup<taBase>* LayerWriter_Group::NewGp(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
    if(taMisc::gui_active)
      gpiGroupNew::New(this,typ);
#endif
    return NULL;
  }
  if((typ == NULL) || !(typ->InheritsFrom(GetTypeDef())))
    typ = GetTypeDef();
  NetConduit* es = GET_MY_OWNER(NetConduit);
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
      ((LayerWriter_Group*)(gp.FastEl(i)))->NewPatternGroup(ev, pg);
  }
  es->UnSetLayers();
  es->UpdateChildren();
  return rval;
}


TAPtr LayerWriter_Group::New(int no, TypeDef* typ) {
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

bool LayerWriter_Group::Remove(int i) {
  NetConduit* es = GET_MY_OWNER(NetConduit);
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

Pattern_Group* LayerWriter_Group::NewPatternGroup(Event* ev, Pattern_Group* par) {
  Pattern_Group* rval = (Pattern_Group*)par->NewGp(1, pat_gp_type);

  LayerWriter* ps;
  int i;
  for(i=0; i<size; i++) {
    ps = (LayerWriter*)FastEl(i);
    ps->NewPattern(ev,rval);
  }

  LayerWriter_Group* pg;
  for(i=0; i<gp.size; i++) {
    pg = (LayerWriter_Group*)gp.FastEl(i);
    pg->NewPatternGroup(ev,rval);
  }
  return rval;
}

void LayerWriter_Group::UpdatePatternGroup(Event* ev, Pattern_Group* pg) {
  // enforce sizes to be the same..
  while(pg->size > size) pg->Remove(pg->size-1);
  LayerWriter* ps;
  Pattern* pat;
  int i;
  for(i=0; i<size; i++) {
    ps = (LayerWriter*)FastEl(i);
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
  LayerWriter_Group* pgs;
  Pattern_Group* sgp;
  for(i=0; i<gp.size; i++) {
    pgs = (LayerWriter_Group*)gp.FastEl(i);
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
*/

/*TODO
void NetConduit::UpdateSubSpecs() {
  NetConduit* parent = (NetConduit*)FindParent();
  if(parent != NULL) {
    // match our sub-patterns with the parents
    patterns.EnforceSameStru(parent->patterns);
  }
  LayerWriter* ps;
  taLeafItr psi;
  FOR_ITR_EL(LayerWriter, ps, patterns., psi)
    ps->UpdateSpec();
}

void NetConduit::UpdateSpec() {
  BaseSpec::UpdateSpec();
  UpdateAllEvents();
}

void NetConduit::SetLayers(Network* net) {
  LayerWriter* ps;
  taLeafItr psi;
  FOR_ITR_EL(LayerWriter, ps, patterns., psi)
    ps->SetLayer(net);
  taBase::SetPointer((TAPtr*)&last_net, net);
}

void NetConduit::UnSetLayers() {
  LayerWriter* ps;
  taLeafItr psi;
  FOR_ITR_EL(LayerWriter, ps, patterns., psi)
    ps->UnSetLayer();
  taBase::DelPointer((TAPtr*)&last_net);
}

void NetConduit::ApplyPatterns(Event* ev, Network* net) {
  if(net == NULL) {
    taMisc::Error("Event:",ev->GetPath(), " cannot apply patterns since network is NULL.",
		   "Try reseting the network pointer in your processes");
    return;
  }

  if(net != last_net)
    SetLayers(net);

  Pattern* pat;
  LayerWriter* ps;
  taLeafItr pi, psi;
  FOR_ITR_PAT_SPEC(Pattern, pat, ev->patterns., pi, LayerWriter, ps, patterns., psi) {
    ps->ApplyPattern(pat);
  }
}

void NetConduit::ApplyNames(Network* net) {
  if(net == NULL)
    return;

  SetLayers(net);

  LayerWriter* ps;
  taLeafItr psi;
  FOR_ITR_EL(LayerWriter, ps, patterns., psi) {
    ps->ApplyNames();
  }

  UnSetLayers();
}

void NetConduit::NewEvent(Event* ev) {
  ev->spec.SetSpec(this);

  ev->patterns.Reset();
  LayerWriter* ps;
  int i;
  for(i=0; i<patterns.size; i++) {
    ps = (LayerWriter*)patterns.FastEl(i);
    ps->NewPattern(ev,&(ev->patterns));
  }

  LayerWriter_Group* pg;
  for(i=0; i<patterns.gp.size; i++) {
    pg = (LayerWriter_Group*)patterns.gp.FastEl(i);
    pg->NewPatternGroup(ev,&(ev->patterns));
  }
}

void NetConduit::UpdateEvent(Event* ev) {
  patterns.UpdatePatternGroup(ev, &(ev->patterns));
}

void NetConduit::UpdateAllEvents() {
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

void NetConduit::UpdateFromLayers() {
  LayerWriter* ps;
  taLeafItr psi;
  FOR_ITR_EL(LayerWriter, ps, patterns., psi)
    ps->SetToLayer();
}

bool NetConduit::DetectOverlap() {
  // check for overlap
  int maxx = MaxX()+1; int maxy = MaxY()+1;
  bool* grid = new bool[maxx * maxy];
  int x;
  for(x=0;x<maxx;x++) {
    int y;
    for(y=0;y<maxy;y++)
      grid[(y*maxx) + x] = false;
  }
  LayerWriter* pat;
  taLeafItr i;
  FOR_ITR_EL(LayerWriter, pat, patterns., i) {
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

void NetConduit::LinearLayout(PatternLayout pat_layout) {
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
  LayerWriter* pat;
  taLeafItr i;
  FOR_ITR_EL(LayerWriter, pat, patterns., i) {
    pat->pos.x = xp;
    pat->pos.y = yp;
    xp += del_x * (pat->geom.x + 1);
    yp += del_y * (pat->geom.y + 1);
    tabMisc::NotifyEdits(pat);
  }
  UpdateChildren();		// apply changes to sub specs
  UpdateAllEvents();
}

void NetConduit::AutoNameEvent(Event* ev, float act_thresh, int max_pat_nm, int max_val_nm) {
  String nm;
  Pattern* pat;
  LayerWriter* ps;
  taLeafItr pi, psi;
  FOR_ITR_PAT_SPEC(Pattern, pat, ev->patterns., pi, LayerWriter, ps, patterns., psi) {
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

void NetConduit::AutoNameAllEvents(float act_thresh, int max_pat_nm, int max_val_nm) {
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

int NetConduit::MaxX() {
  int mx = 0;
  LayerWriter* pat;
  taLeafItr i;
  FOR_ITR_EL(LayerWriter, pat, patterns., i){
    int patx = pat->pos.x + pat->geom.x;
    mx = MAX(mx,patx);
  }
  return mx;
}

int NetConduit::MaxY() {
  int mx = 0;
  LayerWriter* pat;
  taLeafItr i;
  FOR_ITR_EL(LayerWriter, pat, patterns., i){
    int paty = pat->pos.y + pat->geom.y;
    mx = MAX(mx,paty);
  }
  return mx;
}

void NetConduit::AddToView() {
  if(!taMisc::gui_active || taMisc::is_loading) return;
  Environment* env = GET_MY_OWNER(Environment);
  if(env == NULL) return;
//TODO
//  int leaf_idx = -2;

//  EnviroView* evv;
//  taLeafItr j;
//  FOR_ITR_EL(EnviroView,evv,env->views.,j) {
//    if(evv->editor == NULL) continue;
//    evv->editor->AddSpec(this, leaf_idx);	// -2 is signal to update after, and find leaf idx
//  } 
}

void NetConduit::RemoveFromView() {
  if(!taMisc::gui_active) return;
  Environment* env = GET_MY_OWNER(Environment);
  if(env == NULL) return;
//TODO
//  EnviroView* evv;
//  taLeafItr j;
//  FOR_ITR_EL(EnviroView,evv,env->views.,j) {
//    if(evv->editor == NULL) continue;
//    evv->editor->RemoveSpec(this);
//  } 
}

*/
