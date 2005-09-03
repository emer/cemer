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

// stuff to implement environment view..

#include <pdp/enviro_iv.h>
#include <pdp/enviro.h>
#include <pdp/netstru.h>
#include <ta/ta_group_iv.h>

#include <iv_misc/lrScrollBox.h>
#include <iv_misc/tbScrollBox.h>
#include <iv_misc/dastepper.h>

#include <ta/enter_iv.h>
#include <InterViews/brush.h>
#include <InterViews/color.h>
#include <InterViews/tformsetter.h>
#include <InterViews/transformer.h>
#include <InterViews/action.h>
#include <InterViews/box.h>
#include <InterViews/layout.h>
#include <InterViews/window.h>
#include <InterViews/canvas.h>
#include <InterViews/session.h>
#include <InterViews/event.h>
#include <InterViews/style.h>
#include <InterViews/telltale.h>
#include <InterViews/geometry.h>
#include <InterViews/patch.h>
#include <InterViews/background.h>
#include <InterViews/cursor.h>
#include <InterViews/bitmap.h>
#include <InterViews/deck.h> // for hilit buttons
#include <InterViews/font.h>
#include <InterViews/polyglyph.h> // for height field neighbors
#include <IV-look/kit.h>
#include <IV-look/stepper.h>
#include <IV-look/field.h>
#include <IV-look/bevel.h>
#include <OS/file.h>
#include <OS/math.h>
#include <ta/leave_iv.h>

#include <unistd.h>

// EnviroView stuff
const float ENV_VIEW_INITIAL_X = 472.28f;
const float ENV_VIEW_INITIAL_Y = 216.97f;
const float EVENT_MARGIN = 12.0f;
const float ENV_VIEW_X_MARG = 8.0f;
const float ENV_VIEW_Y_MARG = 8.0f;

#define MAX_EVENTS 100000	// Maxiumum number of events for scroll event_name
                                // region of enviroview

#define DIST(x,y) sqrt((double) ((x * x) + (y*y)))

//////////////////////////////////
//	   Environment_G	//
//////////////////////////////////

Environment_G::Environment_G(Environment* n) {
  env = n;
  defbrush = new ivBrush(BRUSH_SOLID,DEFAULT_BRUSH_WIDTH);
  ivResource::ref(defbrush);

  border = new ivColor(0.0, 0.0, 0.0, 1.0);
  ivResource::ref(border);

  reinit_display = false;
  taBase::Own(selectgroup, env); // these are conceptually owned by the environment..
  taBase::Own(pickgroup, env);	  // ditto
  tx = NULL;
}


Environment_G::~Environment_G() {
  // Reset(); // do not call this since there is a flush in Reset();
  selectgroup.RemoveAll(); // just do these two instead
  pickgroup.RemoveAll(); 

  ivResource::unref(defbrush); defbrush = NULL;
  ivResource::unref(border);	border = NULL;
}

void Environment_G::Reset() {
  remove_all();
  selectgroup.RemoveAll();
  pickgroup.RemoveAll();
  ivResource::flush(); // clear our all defered unrefs.
}

void Environment_G::Build() {
  Reset();			// clear out exiting layers here

  ivAllotment& ax = _a.x_allotment();
  ivAllotment& ay = _a.y_allotment();
  span.x = ax.span();		// overall size of display
  span.y = ay.span();

  ivFontBoundingBox bbox;
  owner->owner->view_font.fnt->font_bbox(bbox);
  float fy=bbox.ascent() + bbox.descent();
  float eff_y_marg = (1.4f * fy) / span.y;

  max_size.x = max_size.y = 0;
  evt_geom.x = evt_geom.y = 0;
  TDCoord max_ev_size;		// max size of one event
  if(owner->edit_mode == EnvEditor::SPECS) {
    eff_y_marg *= 2.0f;		// allow for more space for text
    evt_geom.FitNinXY(owner->owner->specs_displayed.leaves);
    TDCoord xy, posn, sz;
    Event_G* esg;
    EventSpec* es;
    taLeafItr i;
    FOR_ITR_EL(EventSpec, es, owner->owner->specs_displayed., i) {
      sz.x = es->MaxX();
      sz.y = es->MaxY();
      max_ev_size.x = MAX(max_ev_size.x, sz.x);
      max_ev_size.y = MAX(max_ev_size.y, sz.y);
    }
    max_size.x = (float)evt_geom.x * (max_ev_size.x + 1); // always put one extra block in the x dim
    max_size.x -= 1.0f;
    
    // now, figure out exact max y size scaling to fit in display
    float marg_extra = evt_geom.y * eff_y_marg;
    int mxypos = evt_geom.y * max_ev_size.y;
    // 1 = mxyp / mxy + mg_ex,  mxyp/mxy = 1-mg_ex, mxyp = mxy(1-mg_ex), mxy = mxyp /(1-mg_ex)
    max_size.y = ((float)mxypos / (1.0f - marg_extra));
    max_size.y = MAX(4.0f, max_size.y);
    max_size.x = MAX(4.0f, max_size.x);
    posn.x = 0; posn.y = 0;
    xy.x = 0; xy.y = 0;
    FOR_ITR_EL(EventSpec, es, owner->owner->specs_displayed., i) {
      esg = new Event_G(es, NULL, this);
      append_(esg);
      esg->translate(xcscale((float)posn.x), ycscale((float)posn.y) + (float)xy.y * eff_y_marg);
      esg->Build();
      if(owner->owner->event_layout == EnviroView::HORIZONTAL) {
	xy.x++; posn.x += max_ev_size.x + 1;
	if(xy.x >= evt_geom.x) {
	  xy.y++; xy.x = 0; posn.x = 0; posn.y += max_ev_size.y;
	}
      }
      else {
	xy.y++; posn.y += max_ev_size.y;
	if(xy.y >= evt_geom.y) {
	  xy.x++; xy.y = 0; posn.y = 0; posn.x += max_ev_size.x + 1;
	}
      }
    }
  }
  else {
    if(owner->pathead_type != NULL) { // also displaying a pattern
      eff_y_marg *= 2.0f;		// double it up!
    }
    evt_geom.FitNinXY(owner->owner->events_displayed.leaves);
    TDCoord xy, posn, sz;
    Event_G* evg;
    Event* ev;
    taLeafItr i;
    FOR_ITR_EL(Event, ev, owner->owner->events_displayed., i) {
      EventSpec* es = (EventSpec*)ev->spec.spec;
      sz.x = es->MaxX();
      sz.y = es->MaxY();
      max_ev_size.x = MAX(max_ev_size.x, sz.x);
      max_ev_size.y = MAX(max_ev_size.y, sz.y);
    }
    max_size.x = (float)evt_geom.x * (max_ev_size.x + 1);
    max_size.x -= 1.0f;

    // now, figure out exact max y size scaling to fit in display
    float marg_extra = evt_geom.y * eff_y_marg;
    int mxypos = evt_geom.y * max_ev_size.y;
    // 1 = mxyp / mxy + mg_ex,  mxyp/mxy = 1-mg_ex, mxyp = mxy(1-mg_ex), mxy = mxyp /(1-mg_ex)
    max_size.y = ((float)mxypos / (1.0f - marg_extra));
    max_size.y = MAX(4.0f, max_size.y);
    max_size.x = MAX(4.0f, max_size.x);
    posn.x = 0; posn.y = 0;
    xy.x = 0; xy.y = 0;
    FOR_ITR_EL(Event, ev, owner->owner->events_displayed., i) {
      EventSpec* es = (EventSpec*)ev->spec.spec;
      evg = new Event_G(es, ev, this);
      append_(evg);
      evg->translate(xcscale((float)posn.x), ycscale((float)posn.y) + (float)xy.y * eff_y_marg);
      evg->Build();
      if(owner->owner->event_layout == EnviroView::HORIZONTAL) {
	xy.x++; posn.x += max_ev_size.x + 1;
	if(xy.x >= evt_geom.x) {
	  xy.y++; xy.x = 0; posn.x = 0; posn.y += max_ev_size.y;
	}
      }
      else {
	xy.y++; posn.y += max_ev_size.y;
	if(xy.y >= evt_geom.y) {
	  xy.x++; xy.y = 0; posn.y = 0; posn.x += max_ev_size.x + 1;
	}
      }
    }
  }
  ivExtension ext;
  if((owner!= NULL) && (owner->viewer != NULL) &&
     (owner->viewer->canvas() != NULL)) {
    allocate(owner->viewer->canvas(),_a,ext);
    safe_damage_me(owner->viewer->canvas());
  }
}

void Environment_G::ApplyValues() {
  Event_G* evg = NULL;
  int j;
  for(j=0;j<count_();j++){
    evg = (Event_G *) component_(j);
    if(evg->InheritsFrom(&TA_Event_G)) {
      evg->ApplyValues();
    }
  }
}

void Environment_G::RevertValues() {
  Event_G* evg = NULL;
  int j;
  for(j=0;j<count_();j++){
    evg = (Event_G *) component_(j);
    if(evg->InheritsFrom(&TA_Event_G)) {
      evg->RevertValues();
    }
  }
}

ivColor* Environment_G::GetLabelColor() {
  return owner->scale->background.contrastcolor;
}

void EnvironmentGSelectEffect(void* obj){
  ((Environment_G*)obj)->owner->FixEditorButtons();
}

ivColor*  EnvironmentGGetLabelColor(void* obj){
  return ((Environment_G *) obj)->GetLabelColor();
}

bool Environment_G::update_from_state(ivCanvas* c) {
  bool result = GraphicMaster::update_from_state(c);
  return result;
}

bool Environment_G::effect_select(bool set_select) {
  bool result = GraphicMaster::effect_select(set_select);
  if(reinit_display) {
    reinit_display = false;
    owner->InitDisplay();
  }
  return result;
}

bool Environment_G::select(const ivEvent& e, Tool& tool, bool b) {
  bool result = GraphicMaster::select(e,tool,b);
  if(reinit_display) {
    reinit_display = false;
    owner->InitDisplay();
  }
  else {
    owner->FixEditorButtons();
  }
  return result;
}

void Environment_G::ScaleCenter(const ivAllocation& a){
  ivAllocation b(a);
  ivAllotment& ax = _a.x_allotment();
  ivAllotment& ay = _a.y_allotment();
  ivAllotment& bx = b.x_allotment();
  ivAllotment& by = b.y_allotment();
  translate((bx.span()-ax.span())/2.0, (by.span()-ay.span())/2.0);
  translate(bx.begin()-ax.begin(), by.begin()-ay.begin());
  translate(0, 3);
}

void Environment_G::ReCenter(){
  // clear out old allocation so environment is rescaled/translated.
  ivAllocation a = _a; //temp for clearing
  ivAllocation b;
  _a = b; // zero's
  ScaleCenter(a);
  _a = a;
}

void Environment_G::allocate (ivCanvas* c, const ivAllocation& a, ivExtension& ext) {
  if(tx) {
    transformer(tx);
    ivResource::unref(tx);
    tx = NULL;
    _a = a;
  }
  else {
    if (!_a.equals(a, 0.001)) {
      if((owner!= NULL) && (owner->owner != NULL)){
	ScaleCenter(a);
      }
      _a = a;
    }
  }
  if (c != nil) {
    PolyGraphic::allocate(c, a, ext);
  }
}

//////////////////////////////////
//	 ObjNameEdit		//
//////////////////////////////////

void ObjNameEdit::Initialize(){
  obj = NULL;
}

void ObjNameEdit::SetObj(TAPtr ob){
  if(ob != NULL) {
    taBase::SetPointer((TAPtr*)&obj, (TAPtr)ob);
    name = obj->GetName();
  }
}

void ObjNameEdit::Destroy() {
  if(obj != NULL){
    taBase::DelPointer((TAPtr*)&obj);
  }
}

void ObjNameEdit::UpdateAfterEdit(){
  if(obj != NULL) {
    DMEM_GUI_RUN_IF {
      obj->SetName(name);
    }
    taivMisc::RecordScript(obj->GetPath() + ".name = \"" + name + "\";\n");
  }
}

//////////////////////////////////
//	 ObjValuedit		//
//////////////////////////////////

void ObjValueEdit::Initialize(){
  obj = NULL;
  mbr = NULL;
}

void ObjValueEdit::SetObj(TAPtr ob, MemberDef* md){
  taBase::SetPointer((TAPtr*)&(obj), ob);
  mbr = md;
  if(obj != NULL) {
    name = mbr->name;
    value = mbr->type->GetValStr(mbr->GetOff((void*)obj), NULL, mbr);
  }
}

void ObjValueEdit::Destroy() {
  if(obj != NULL){
    taBase::DelPointer((TAPtr*)&obj);
  }
}

void ObjValueEdit::UpdateAfterEdit(){
  if((obj != NULL) && (mbr != NULL)) {
    mbr->type->SetValStr(value, mbr->GetOff((void*)obj), NULL, mbr);
    taivMisc::SRIAssignment(obj, mbr);
    tabMisc::DelayedUpdateAfterEdit(obj);
  }
}

//////////////////////////////////
//	 PatSpecEdit		//
//////////////////////////////////

void PatSpecEdit::Initialize(){
  obj = NULL;
}

void PatSpecEdit::SetObj(PatternSpec* ob){
  if(ob != NULL) {
    taBase::SetPointer((TAPtr*)&(obj), ob);
    name = obj->GetName();
    type = obj->type;
    to_layer = obj->to_layer;
    layer_name = obj->layer_name;
    layer_num = obj->layer_num;
  }
}

void PatSpecEdit::Destroy() {
  if(obj != NULL){
    taBase::DelPointer((TAPtr*)&obj);   
  }
}

void PatSpecEdit::UpdateAfterEdit(){
  if((name.empty() || name.contains("PatternSpec")) && !layer_name.empty())
    name = layer_name;
  if(obj != NULL) {
    String opath = obj->GetPath();
    if(obj->name != name) {
      obj->name = name;
      taivMisc::RecordScript(opath + ".name = \"" + name + "\";\n");
    }
    if(obj->type != type) {
      obj->type = type;
      taivMisc::RecordScript(opath + ".type = " + (String)(int)type + ";\n");
    }
    if(obj->to_layer != to_layer) {
      obj->to_layer = to_layer;
      taivMisc::RecordScript(opath + ".to_layer = " + (String)(int)to_layer + ";\n");
    }
    if(obj->layer_name != layer_name) {
      obj->layer_name = layer_name;
      taivMisc::RecordScript(opath + ".layer_name = \"" + layer_name + "\";\n");
    }
    if(obj->layer_num != layer_num) {
      obj->layer_num = layer_num;
      taivMisc::RecordScript(opath + ".layer_num = " + (String)layer_num + ";\n");
    }
    // can't do value_names because it requires a separate edit dialog, which doesn't work!
//     int mxvl = MIN(obj->value_names.size, value_names.size);
//     for(int i=0;i<mxvl;i++) {
//       if(obj->value_names[i] != value_names[i]) {
// 	recordstring = opath + ".value_names[" + String(i) + "] = " + value_names[i] + ";\n";
// 	taivMisc::RecordScript(recordstring);
//       }
//     }
    tabMisc::DelayedUpdateAfterEdit(obj);
  }
}

//////////////////////////////////
//	   EditText_G		//
//////////////////////////////////

EditText_G::EditText_G(TAPtr o, GraphicMaster* m, const ivFont* f, const ivColor* c,
	   const char* ch, ivTransformer* t) :
  NoScale_Text_G(o, m, f, c, ch, t) {
  editable = true;
  get_color = EnvironmentGGetLabelColor; // use background color of environment_g
}

bool EditText_G::effect_select(bool set_select) {
  bool temp = Graphic::effect_select(set_select);
  Environment_G* envg = (Environment_G*)master;
  if(set_select && editb_used && (!envg->reinit_display) && (master->selectgroup.size == 1)) {
    obj->Edit(true);		// wait
    editb_used = false;		// don't do it again!
    envg->reinit_display = true;
  }
  if(set_select)
    master->selectgroup.LinkUnique(obj);
  else
    master->selectgroup.Remove(obj);
  return temp;
}


//////////////////////////////////
//	   Event_G		//
//////////////////////////////////

Event_G::Event_G(EventSpec* es, Event* ev, Environment_G* ng) {
  espec = es;
  evt = ev;
  envg = ng;
  name_edit = new ObjNameEdit;
  taBase::Ref(name_edit);
  value_edit = new ObjValueEdit;
  taBase::Ref(value_edit);
}

Event_G::~Event_G() {
  taBase::DelPointer((TAPtr*)&name_edit);
  taBase::DelPointer((TAPtr*)&value_edit);
}

ivGlyph* Event_G::clone() const {
  return new Event_G(espec, evt, envg); 
}

void Event_G::Build() {
  remove_all();
  TAPtr obj = espec;
  if(evt != NULL) obj = evt;

  EventBox_G* eb = new EventBox_G(this, envg); // first element
  append_(eb);
  ivCoord x = espec->MaxX();
  ivCoord y = espec->MaxY();
  eb->SetPoints(x, y);

  ivFontBoundingBox bbox;
  envg->owner->owner->view_font.fnt->font_bbox(bbox);
  float fy=bbox.ascent() + bbox.descent();

  if(evt != NULL) {
    String mbr_val = "n/a";
    value_edit->SetObj(NULL, NULL); // clear out
    TypeDef* td = envg->owner->eventhead_type;
    MemberDef* md = envg->owner->event_header;
    if(td != NULL) {
      if(td->InheritsFrom(&TA_Event)) {
	if(evt->InheritsFrom(td)) {
	  mbr_val = md->type->GetValStr(md->GetOff((void*)evt), NULL, md);
	  if(md->name != "name")
	    mbr_val = md->name + ":" + mbr_val;
	  value_edit->SetObj(evt, md);
	}
      }
      else {
	if(espec->InheritsFrom(td)) {
	  mbr_val = md->type->GetValStr(md->GetOff((void*)espec), NULL, md);
	  if(md->name != "name")
	    mbr_val = md->name + ":" + mbr_val;
	  value_edit->SetObj(espec, md);
	}
      }
      EditText_G* et = new EditText_G(value_edit, envg, envg->owner->owner->view_font.fnt,
				      envg->border, (const char*)mbr_val, NULL);
      append_(et);
      et->translate(0, -fy);
    }
  }
  else {
    name_edit->SetObj(obj);
    EditText_G* et = new EditText_G(name_edit, envg, envg->owner->owner->view_font.fnt,
				    envg->border, (const char*)name_edit->name, NULL);
    append_(et);
    et->translate(0, -fy);
  }

  Pattern_G* pg;
  PatternSpec* ps;
  if(evt != NULL) {
    Pattern* pat;
    taLeafItr pi, psi;
    FOR_ITR_PAT_SPEC(Pattern, pat, evt->patterns., pi, PatternSpec, ps, espec->patterns., psi) {
      pg = new Pattern_G(ps, pat, envg);
      append_(pg);
      pg->Build();
    }
  }
  else {
    taLeafItr psi;
    FOR_ITR_EL(PatternSpec, ps, espec->patterns., psi) {
      pg = new Pattern_G(ps, NULL, envg);
      append_(pg);
      pg->Build();
    }
  }
}

void Event_G::ApplyValues() {
  Pattern_G* pg = NULL;
  int j;
  for(j=0;j<count_();j++){
    pg = (Pattern_G *) component_(j);
    if(pg->InheritsFrom(&TA_Pattern_G)) {
      pg->ApplyValues();
    }
  }
}

void Event_G::RevertValues() {
  Pattern_G* pg = NULL;
  int j;
  for(j=0;j<count_();j++){
    pg = (Pattern_G *) component_(j);
    if(pg->InheritsFrom(&TA_Pattern_G)) {
      pg->RevertValues();
    }
  }
}

bool Event_G::selectable() {
  // if editing, cut to the text!
  if((envg->owner->viewer->last_button == ivEvent::right) ||
     (envg->owner->one_espec_sel != NULL)) // if just one spec, deal with it!
    return false;
  return true;
}

bool Event_G::select(const ivEvent& e, Tool& tool, bool unselect) {
  // this code makes the event box be selected upon first click
  // and then subsequent things can be selected later
  long i;
  if(unselect)
    _selected = false;
  else
    _selected = !_selected;

  if(count_() > 0) { // there are subobjs
    EventBox_G* lbg = (EventBox_G*) component_(0); // first item
    if(!unselect) {
      if(lbg->is_selected()) {
	lbg->select(e,tool,false);
      }
      else {
	lbg->select(e,tool,false);
	for(i=1; i < count_(); i++) {
	  component_(i)->select(e, tool, true);
	}
      }
    }
    else {
      if(lbg->is_selected()){
	lbg->select(e,tool,true);
      }
      else {
	for(i=1; i < count_(); i++) {
	  component_(i)->select(e, tool, unselect);
	}
      }
    }
  }
  return true;
}

bool Event_G::effect_select(bool set_select) {
  bool temp = PolyGraphic::effect_select(set_select);
  TAPtr obj = espec;
  if(evt != NULL) obj = evt;
  if(set_select) {
    envg->selectgroup.Reset();	// get rid of anything else
    envg->selectgroup.LinkUnique(obj);
  }
  else {
    envg->selectgroup.Remove(obj);
  }
  return temp;
}

//////////////////////////////////
//	   EventBox_G		//
//////////////////////////////////

static ivCoord pts[4];

EventBox_G::EventBox_G(Event_G* eg, Environment_G* eng) :
Polygon(eng->defbrush, eng->border, NULL, pts, pts, 4, NULL) {
  evtg = eg;
  envg = eng;
}

void EventBox_G::SetPoints(ivCoord x, ivCoord y) {
  float xmarg = EVENT_MARGIN / envg->span.x;
  float ymarg = EVENT_MARGIN / envg->span.y;

  _x[0] = envg->xcscale(- xmarg);
  _x[1] = envg->xcscale(x + xmarg);
  _x[2] = envg->xcscale(x + xmarg);
  _x[3] = envg->xcscale(- xmarg);
  _y[0] = envg->ycscale(- ymarg);
  _y[1] = envg->ycscale(- ymarg);
  _y[2] = envg->ycscale(y + ymarg);
  _y[3] = envg->ycscale(y + ymarg);
  recompute_shape();
}

bool EventBox_G::selectable() {
  // if editing, cut to the text!
  if((envg->owner->viewer->last_button == ivEvent::right) ||
     (envg->owner->one_espec_sel != NULL)) // if just one spec, deal with it!
    return false;
  return true;
}

bool EventBox_G::select(const ivEvent& e, Tool& tool, bool unselect) {
  if(evtg->evt != NULL) return false; // never select
  return Polygon::select(e, tool, unselect);
}

bool EventBox_G::effect_select(bool set_select) {
  bool temp = Polygon::effect_select(set_select);
  TAPtr obj = evtg->espec;
  if(evtg->evt != NULL) obj = evtg->evt;
  if(set_select)
    envg->selectgroup.LinkUnique(obj);
  else
    envg->selectgroup.Remove(obj);
  return temp;
}


//////////////////////////////////
//	   Pattern_G		//
//////////////////////////////////

Pattern_G::Pattern_G(PatternSpec* ps, Pattern* pt, Environment_G* ng) {
  patspec = ps;
  pat = pt;
  envg = ng;
  Position();
  ps_edit = NULL;
  if(pat == NULL) {
    ps_edit = new PatSpecEdit;
    taBase::Ref(ps_edit);
  }
  name_edit = new ObjNameEdit;
  taBase::Ref(name_edit);
  value_edit = new ObjValueEdit;
  taBase::Ref(value_edit);
  taBase::Ref(val_buffer);	// just in case..
}

Pattern_G::~Pattern_G() {
  taBase::DelPointer((TAPtr*)&ps_edit);
  taBase::DelPointer((TAPtr*)&name_edit);
  taBase::DelPointer((TAPtr*)&value_edit);
}

ivGlyph* Pattern_G::clone() const {
  return new Pattern_G(patspec, pat, envg); 
}

void Pattern_G::Position() {
  translate(envg->xcscale((float)patspec->pos.x), envg->ycscale((float)patspec->pos.y));
}

void Pattern_G::Build() {
  remove_all();
  move_pos.Initialize();

  PatternBox_G* pbox = new PatternBox_G(patspec, pat, envg);
  append_(pbox);
  pbox->Build();
  pbox->Position();

  if(pat != NULL) {
    String mbr_val = "n/a";
    value_edit->SetObj(NULL, NULL); // clear out
    TypeDef* td = envg->owner->pathead_type;
    MemberDef* md = envg->owner->pattern_header;
    if(td != NULL) {
      if(td->InheritsFrom(&TA_Pattern)) {
	if(pat->InheritsFrom(td)) {
	  mbr_val = md->type->GetValStr(md->GetOff((void*)pat), NULL, md);
	  if(md->name != "name")
	    mbr_val = md->name + ":" + mbr_val;
	  value_edit->SetObj(pat, md);
	}
      }
      else {
	if(patspec->InheritsFrom(td)) {
	  mbr_val = md->type->GetValStr(md->GetOff((void*)patspec), NULL, md);
	  if(md->name != "name")
	    mbr_val = md->name + ":" + mbr_val;
	  value_edit->SetObj(patspec, md);
	}
      }
      int idx = ((Pattern_Group*)pat->owner)->FindEl(pat);
      String txt = (String)idx + ": " + mbr_val;
      EditText_G* et = new EditText_G(value_edit, envg, envg->owner->owner->view_font.fnt,
				      envg->border, (const char*)txt, NULL);
      append_(et);
      et->translate(0, envg->ytscale(patspec->geom.y) + 3);
    }
  }
  else {
    name_edit->SetObj(patspec);
    int idx = ((PatternSpec_Group*)patspec->owner)->FindEl(patspec);
    String txt = (String)idx + ": " + patspec->name;
    EditText_G* psnm = new EditText_G(name_edit, envg, envg->owner->owner->view_font.fnt,
				      envg->border, (const char*)txt, NULL);
    append_(psnm);
    psnm->translate(0, envg->ytscale(patspec->geom.y) + 3);
  }
  if(pat == NULL) {
    if(patspec->n_vals > 0) {
      NValBox_G* nvb = new NValBox_G(this, envg);
      append_(nvb);
      nvb->Build();
      nvb->Position();
    }
    ps_edit->SetObj(patspec);
    int stmbridx = TA_PatternSpec.members.Find("type");
    ivCoord totht = (envg->ytscale(1) - envg->ytscale(0)) * (float)patspec->geom.y;
    ivFontBoundingBox bbox;
    envg->owner->owner->value_font.fnt->font_bbox(bbox);
    float fy=bbox.ascent() + bbox.descent();
    int nlines = (int)(totht / fy);
    nlines = MAX(nlines, 1);
    nlines = MIN(nlines, 4);
    totht = MIN((fy * (float)nlines) + 3, totht);
    int i;
    for(i=0; i<nlines; i++) {
      MemberDef* md = TA_PatternSpec.members[stmbridx + i];
      String val = md->type->GetValStr(md->GetOff((void*)patspec), NULL, md);
      val = md->name + ": " + val;
      EditText_G* et = new EditText_G(ps_edit, envg, envg->owner->owner->value_font.fnt,
					      envg->border, (const char*)val, NULL);
      append_(et);
      et->translate(3, totht - ((float)(i+1) * fy));
    }
    if(patspec->n_vals > 0) {
      // special n_vals text marker
      String val = "n_vals: " + (String)patspec->n_vals;
      TDCoord loc;
      loc.x = (patspec->n_vals-1) % patspec->geom.x;
      loc.y = (patspec->n_vals-1) / patspec->geom.x;
      EditText_G* et = new EditText_G(ps_edit, envg, envg->owner->owner->value_font.fnt,
					      envg->border, (const char*)val, NULL);
      append_(et);
      et->translate(envg->xtscale(loc.x) + 3, envg->ytscale(loc.y + 1) - 15);
    }
  }
  else {
    RevertValues();
  }
}

void Pattern_G::RevertValues() {
  if(pat == NULL) return;
  val_buffer.EnforceSize(pat->value.size);
  EnvEditor* ed = envg->owner;
  if(ed->patflags == PatternSpec::NO_FLAGS) {
    val_buffer.CopyFrom(&pat->value); // get the values
  }
  else {
    bool use_spflag = ed->specflags;
    if(!use_spflag && (patspec->use_flags != PatternSpec::USE_PATTERN_FLAGS) && 
       (patspec->use_flags != PatternSpec::USE_PAT_THEN_GLOBAL_FLAGS)) {
      taMisc::Error("*** Can't view/edit pattern-level flags if use_flags are not set to pattern-level!");
      use_spflag = true;
    }
    int i;
    for(i=0;i<pat->value.size;i++) {
      int flag;
      if(use_spflag)
	flag = patspec->Flag(PatternSpec::USE_GLOBAL_FLAGS, pat, i);
      else
	flag = patspec->Flag(PatternSpec::USE_PATTERN_FLAGS, pat, i);
      float val = ((flag & ed->patflags) == 0) ? 0.0f : 1.0f;
      val_buffer[i] = val;
    }
  }
}

void Pattern_G::ApplyValues() {
  if(pat == NULL) return;
  EnvEditor* ed = envg->owner;
  if(ed->patflags == PatternSpec::NO_FLAGS) {
    pat->value.CopyFrom(&val_buffer); // get the values
  }
  else {
    bool use_spflag = ed->specflags;
    if(!use_spflag && (patspec->use_flags != PatternSpec::USE_PATTERN_FLAGS) && 
       (patspec->use_flags != PatternSpec::USE_PAT_THEN_GLOBAL_FLAGS)) {
      taMisc::Error("*** Can't view/edit pattern-level flags if use_flags are not set to pattern-level!");
      use_spflag = true;
    }
    int i;
    for(i=0;i<pat->value.size;i++) {
      if(use_spflag) {
	int& flag = patspec->Flag(PatternSpec::USE_GLOBAL_FLAGS, pat, i);
	flag = (val_buffer[i] == 0 ) ? (flag & ~ed->patflags) : (flag | ed->patflags);
      }
      else {
	int& flag = patspec->Flag(PatternSpec::USE_PATTERN_FLAGS, pat, i);
	flag = (val_buffer[i] == 0 ) ? (flag & ~ed->patflags) : (flag | ed->patflags);
      }
    }
  }

  if(taivMisc::record_script != NULL) {
    int i;
    ostream* strm = taivMisc::record_script;
    if(ed->patflags == PatternSpec::NO_FLAGS) {
      *strm << "{ float_RArray* ths = " << pat->GetPath() << "->value;\n" << "  ths = \"{";
      for(i=0; i<pat->value.size; i++)
	*strm << pat->value[i] << ",";
      *strm << "}\";\n" << "}" << endl;
    }
    else {
      bool use_spflag = ed->specflags;
      if(!use_spflag && (patspec->use_flags != PatternSpec::USE_PATTERN_FLAGS) && 
	 (patspec->use_flags != PatternSpec::USE_PAT_THEN_GLOBAL_FLAGS)) {
	use_spflag = true;
      }
      if(use_spflag)  {
	*strm << "{ float_RArray* ths = " << patspec->GetPath() << "->global_flags;\n" << "  ths = \"{";
	for(i=0; i<patspec->global_flags.size; i++)
	  *strm << patspec->global_flags[i] << ",";
	*strm << "}\";\n" << "}\n";
      }
      else  {
	*strm << "{ float_RArray* ths = " << pat->GetPath() << "->flag;\n" << "  ths = \"{";
	for(i=0; i<pat->flag.size; i++)
	  *strm << pat->flag[i] << ",";
	*strm << "}\";\n" << "}\n";
      }
      strm->flush();
    }
  }
}

bool Pattern_G::selectable() {
  // if editing, cut to the text!
  if(envg->owner->viewer->last_button == ivEvent::right)
    return false;
  return true;
}

bool Pattern_G::select(const ivEvent& e, Tool& tool, bool unselect) {
  if(pat == NULL) {
    long i;
    if(unselect)
      _selected = false;
    else
      _selected = !_selected;
    // this makes the first click activate the patternbox, instead of
    // activating everything
    if(count_() > 0) { // there are subobjs
      PatternBox_G* lbg = (PatternBox_G*) component_(0); // first item
      if(unselect == false) {
	if(lbg->is_selected()){
	  lbg->select(e,tool,false);
	}
	else {
	  lbg->select(e,tool,false);
	  for(i=1; i < count_(); i++) {
	    component_(i)->select(e, tool, true);
	  }
	}
      }
      else {
	if(lbg->is_selected()){
	  lbg->select(e,tool,true);
	}
	else {
	  for(i=1; i < count_(); i++) {
	    component_(i)->select(e, tool, unselect);
	  }
	}
      }
    }
    return true;
  }

  if(taivM->GetButton(e) == ivEvent::right) // don't do anything on rmb!
    return false;
  ivCoord x = e.pointer_x();
  ivCoord y = e.pointer_y();
  ivCoord nx = x;
  ivCoord ny = y;
  Graphic gs;
  total_gs(gs);
  ivTransformer* tx = gs.transformer();
  if(tx == NULL) return false;
  tx->inverse_transform(nx, ny);
  TDCoord np;
  np.x = (int)floor(nx * (float)envg->max_size.x);
  np.y = (int)floor(ny * (float)envg->max_size.y);
  if((np.x >= 0) && (np.y >= 0) && (np.x < patspec->geom.x) &&
     (np.y < patspec->geom.y)) {
    int idx = np.y * patspec->geom.x + np.x;
    if(idx < val_buffer.size) {
      float nwval;
      if(taivM->GetButton(e) == ivEvent::middle)
	//	nwval = pat->value.El(idx); // revert value
	nwval = 0.0f;		// middle always = set to zero!
      else
	nwval = envg->owner->cbar->GetSelectedVal();
      val_buffer[idx] = nwval;
      update_from_state(envg->owner->viewer->canvas());
      envg->owner->UpdatePosivPatch(np.x, np.y, 0);
      envg->owner->HighlightApply();
    }
  }
  return false;
}

bool Pattern_G::effect_select(bool set_select) {
  bool temp = PolyGraphic::effect_select(set_select);
  TAPtr obj = patspec;
  if(pat != NULL)
    obj = pat;
  if(set_select)
    envg->selectgroup.LinkUnique(obj);
  else
    envg->selectgroup.Remove(obj);
  return temp;
}

bool Pattern_G::grasp_move(const ivEvent&,Tool&,ivCoord, ivCoord) {
  move_pos.Initialize();
  envg->owner->UpdatePosivPatch(patspec->pos);
  envg->owner->FixEditorButtons();
  return true;
}

bool Pattern_G::manip_move(const ivEvent& ,Tool& ,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly, ivCoord cx, ivCoord cy) {
  ivCoord dx = cx - lx;
  ivCoord dy = cy - ly;

  if((dx == 0) && (dy == 0))
    return true;

  dx = cx - ix;
  dy = cy - iy;

  TDCoord ng, dg;
  ng.x = (int) (dx * (float)envg->max_size.x);
  ng.y = (int) (dy * (float)envg->max_size.y);

  dg = ng;
  dg -= move_pos;

  if((dg.x != 0) || (dg.y != 0)) {
    TDCoord updtpos = patspec->pos + dg;
    if((updtpos.x <0) || (updtpos.y <0)) {
      return true;
    }
    move_pos = ng;
    patspec->pos.Invert();
    Position();
    patspec->pos.Invert();
    PosTDCoord og = patspec->pos;
    patspec->pos += dg;
    Position();			// put into the new one
    envg->owner->UpdatePosivPatch(patspec->pos);
  }
  return true;
}

bool Pattern_G::effect_move(const ivEvent&,Tool&,ivCoord,
			     ivCoord,ivCoord, ivCoord) {
  if((move_pos.x == 0) && (move_pos.y == 0)) return true;
  envg->reinit_display = true;
  move_pos.Initialize();
  // can't do this:
  //  patspec->UpdateAfterEdit();
  tabMisc::DelayedUpdateAfterEdit(envg->env);
  tabMisc::NotifyEdits(patspec);
  envg->owner->ClearPosivPatch();
  taivMisc::SRIAssignment(patspec,patspec->FindMember("pos"));
  return true;
}

bool Pattern_G::grasp_stretch(const ivEvent& , Tool& ,ivCoord, ivCoord) {
  move_pos.Initialize();
  envg->owner->UpdatePosivPatch(patspec->geom);
  envg->owner->FixEditorButtons();
  return true;
}

bool Pattern_G::manip_stretch(const ivEvent& ,Tool& ,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly, ivCoord cx, ivCoord cy) {
  ivCoord dx = cx - lx;
  ivCoord dy = cy - ly;
  if((dx == 0) && (dy == 0))
    return true;

  dx = (cx - ix);
  dy = (cy - iy);

  TDCoord ng, dg;
  ng.x = (int) (dx * (float)envg->max_size.x);
  ng.y = (int) (dy * (float)envg->max_size.y);

  dg = ng;
  dg -= move_pos;

  if((dg.x != 0) || (dg.y != 0)) {
    TDCoord updtgeom = patspec->geom + dg;
    if((updtgeom.x <0) || (updtgeom.y <1)) {
      return true;
    }
    move_pos = ng;
    PosTDCoord og = patspec->geom;
    patspec->geom += dg;
    PatternBox_G* pbox = (PatternBox_G*)component_(0);	// always first
    pbox->Build();
    envg->owner->UpdatePosivPatch(patspec->geom);
  }
  return true;
}

bool Pattern_G::effect_stretch(const ivEvent&, Tool&,ivCoord,
			       ivCoord, ivCoord, ivCoord)
{
  if((move_pos.x == 0) && (move_pos.y == 0)) return true;
  envg->reinit_display = true;
  move_pos.Initialize();
  int gsz = patspec->geom.x * patspec->geom.y;
  if((patspec->n_vals == 0) || (patspec->n_vals > gsz))
    patspec->n_vals = gsz;
  patspec->value_names.EnforceSize(patspec->n_vals);
  // can't update cuz it redraws display!
  // patspec->UpdateAfterEdit();
  tabMisc::DelayedUpdateAfterEdit(envg->env);
  //  envg->env->UpdateAllEvents();
  tabMisc::NotifyEdits(patspec);
  envg->owner->ClearPosivPatch();
  taivMisc::SRIAssignment(patspec,patspec->FindMember("geom"));
  return true;
}

//////////////////////////////////
//	   PatternBox_G		//
//////////////////////////////////

PatternBox_G::PatternBox_G(PatternSpec* ps, Pattern* pt, Environment_G* ng) :
Polygon(ng->defbrush, ng->border, NULL, pts, pts, 4, NULL) {
  patspec = ps;
  pat = pt;
  envg = ng;
  Position();
  ScaleBar* cbar = envg->owner->cbar;
  ivColor* fl;
  ivColor* txt;
  cbar->GetColor(0.0,&fl,&txt);
  fill(fl);			// so we're solid!
}

PatternBox_G::~PatternBox_G() {
}

ivGlyph* PatternBox_G::clone() const {
  return new PatternBox_G(patspec, pat, envg); 
}

void PatternBox_G::Position() {
  translate(0.0f, 0.0f);
}

void PatternBox_G::Build() {
  ivCoord px[4], py[4];
  px[0] = py[0] = py[1] = px[3] = 0;
  px[1] = px[2] = envg->xcscale((float)patspec->geom.x);
  py[2] = py[3] = envg->ycscale((float)patspec->geom.y);
  SetPoints(px[0],py[0],px[1],py[1],px[2],py[2],px[3],py[3]);
}

void PatternBox_G::SetPoints(ivCoord x0,ivCoord y0, ivCoord x1, ivCoord y1,
			ivCoord x2,ivCoord y2, ivCoord x3, ivCoord y3) {
  _x[0] = x0; _x[1] = x1; _x[2] = x2; _x[3] = x3;
  _y[0] = y0; _y[1] = y1; _y[2] = y2; _y[3] = y3;
  recompute_shape();
}

bool PatternBox_G::update_from_state(ivCanvas* c) {
  if(pat == NULL) return false;
  damage_me(c);
  return true;
}

void PatternBox_G::render_text(ivCanvas* c, ScaleBar* cbar, float val, String& str,
			       FloatTwoDCoord& ll, FloatTwoDCoord& ur, bool from_top)
{
  ivColor* fl;
  ivColor* txt;
  cbar->GetColor(val, &fl, &txt);

  ivFontBoundingBox bbox;
  envg->owner->owner->value_font.fnt->font_bbox(bbox);
  float fy=bbox.ascent() + bbox.descent();

  float nx,ny;
  float ex,ey;
  c->transformer().transform(ll.x, ll.y,nx,ny);
  c->transformer().transform(ur.x, ur.y,ex,ey);
  c->push_transform(); // store the line*net transform;
  ivTransformer nt = c->transformer();
  nt.invert();			// invert current transformer
  c->transform(nt);		// put inverse in there -- effectively null xform
  int len = str.length();
  nx += 1.0f;
  if(from_top) {
    ny = ey - fy;
  }
  else {
    ny += 1.0f;
  }
  float cx = nx;
  int j;
  for(j=0;j<len;j++){	// draw each character
    envg->owner->owner->value_font.fnt->char_bbox(str[j], bbox);
    float nxtx = cx + bbox.width();
    if(nxtx > ex) break;
    c->character((const ivFont*)(envg->owner->owner->value_font.fnt),str[j],8,txt,cx,ny);
    cx = nxtx;
  }

  c->pop_transform(); // remove null transform
}

void PatternBox_G::render_color(ivCanvas* c, ScaleBar* cbar, float val,
				FloatTwoDCoord& ll, FloatTwoDCoord& ur)
{
  ivColor* fl;
  ivColor* txt;
  cbar->GetColor(val, &fl, &txt);
  c->fill_rect(ll.x, ll.y, ur.x, ur.y, fl);
}

void PatternBox_G::render_area(ivCanvas* c, ScaleBar* cbar, float val,
			       FloatTwoDCoord& ll, FloatTwoDCoord& ur)
{
  ivColor* bg_color;
  ivColor* fg_color;
  ivColor* delete_me = NULL;
  bg_color = cbar->bar->scale->GetColor(((int) (.5f * (float)(cbar->bar->scale->chunks-1))));
  if(val > cbar->max)
    fg_color = cbar->bar->scale->maxout.color;
  else if(val < cbar->min)
    fg_color = cbar->bar->scale->minout.color;
  else {
    if(val >= cbar->zero)
      fg_color = cbar->bar->scale->GetColor(cbar->bar->scale->colors.size-1);
    else 
      fg_color = cbar->bar->scale->GetColor(0);

    if(fg_color->alpha() < 0.5f) {// ??
      ivColorIntensity r,g,b;
      cbar->bar->scale->background.color->intensities(r,g,b);
      delete_me = fg_color = new ivColor(r,g,b,1.0-fg_color->alpha());
    }
  }
  //first draw the background
  c->fill_rect(ll.x, ll.y, ur.x, ur.y, bg_color);

  // then compute the foreground
  ivCoord cleft,cright,cbottom,ctop;
  c->transformer().transform(ll.x, ll.y, cleft,cbottom);
  c->transformer().transform(ur.x, ur.y, cright,ctop);

  ivPixelCoord left = c->to_pixels(cleft);
  ivPixelCoord right = c->to_pixels(cright);
  ivPixelCoord top = c->to_pixels(ctop);
  ivPixelCoord bottom = c->to_pixels(cbottom);
  
  int width = right-left;
  int height = top-bottom;
  int max_area = width * height;
  float area = ((float) max_area) * cbar->GetAbsPercent(val);
  if(area > max_area) area = max_area;

  ivPixelCoord max_size = (ivPixelCoord) sqrt(fabs((double) max_area));
  ivPixelCoord box_size = (ivPixelCoord) sqrt(fabs((double) area));

  int x_size = (int) ((((float) width)/(float) max_size) * (float) box_size);
  int y_size = (int) (((float) (height)/(float) max_size) * (float) box_size);

  ivPixelCoord xboxoff = (width - x_size) / 2;
  ivPixelCoord yboxoff = (height - y_size) / 2;

  c->push_transform(); // save old xform
  ivTransformer tt(c->transformer()); // create null xform
  tt.invert();
  c->transform(tt); // set null xform

  c->fill_rect(cleft + c->to_coord(xboxoff),
	       cbottom + c->to_coord(yboxoff),
	       cleft + c->to_coord(xboxoff + x_size),
	       cbottom + c->to_coord(yboxoff + y_size),fg_color);

  if(delete_me != NULL)
    delete delete_me;
  c->pop_transform(); // remove null xform
}

void PatternBox_G::render_linear(ivCanvas* c, ScaleBar* cbar, float val,
				FloatTwoDCoord& ll, FloatTwoDCoord& ur)
{
  ivColor* bg_color;
  ivColor* fg_color;
  ivColor* delete_me = NULL;
  bg_color = cbar->bar->scale->GetColor(((int) (.5f * (float)(cbar->bar->scale->chunks-1))));
  if(val > cbar->max)
    fg_color = cbar->bar->scale->maxout.color;
  else if(val < cbar->min)
    fg_color = cbar->bar->scale->minout.color;
  else {
    if(val >= cbar->zero)
      fg_color = cbar->bar->scale->GetColor(cbar->bar->scale->colors.size-1);
    else 
      fg_color = cbar->bar->scale->GetColor(0);

    if(fg_color->alpha() < 0.5f) {// ??
      ivColorIntensity r,g,b;
      cbar->bar->scale->background.color->intensities(r,g,b);
      delete_me = fg_color = new ivColor(r,g,b,1.0-fg_color->alpha());
    }
  }
  //first draw the background
  c->fill_rect(ll.x, ll.y, ur.x, ur.y, bg_color);

  // then compute the foreground
  ivCoord cleft,cright,cbottom,ctop;
  c->transformer().transform(ll.x, ll.y, cleft,cbottom);
  c->transformer().transform(ur.x, ur.y, cright,ctop);

  ivPixelCoord left = c->to_pixels(cleft);
  ivPixelCoord right = c->to_pixels(cright);
  ivPixelCoord top = c->to_pixels(ctop);
  ivPixelCoord bottom = c->to_pixels(cbottom);
  
  int width = right-left;
  int height = top-bottom;

  int x_size = (int) (((float) width) * cbar->GetAbsPercent(val));
  int y_size = (int) (((float) height) * cbar->GetAbsPercent(val));

  if(y_size > height) y_size = height;
  if(x_size > width) x_size = width;

  ivPixelCoord xboxoff = (width - x_size) / 2;
  ivPixelCoord yboxoff = (height - y_size) / 2;

  c->push_transform(); // save old xform
  ivTransformer tt(c->transformer()); // create null xform
  tt.invert();
  c->transform(tt); // set null xform

  c->fill_rect(cleft + c->to_coord(xboxoff),
	       cbottom + c->to_coord(yboxoff),
	       cleft + c->to_coord(xboxoff + x_size),
	       cbottom + c->to_coord(yboxoff + y_size),fg_color);

  if(delete_me != NULL)
    delete delete_me;
  c->pop_transform(); // remove null xform
}

void PatternBox_G::draw_gs (ivCanvas* c, Graphic* gs) {
  if(_ctrlpts <= 0)
    return;
  const ivBrush* brush = gs->brush();
  const ivColor* stroke = gs->stroke();
  if(brush == nil || stroke == nil) return;

  ivTransformer* tx = gs->transformer();
  if (tx != nil) {
    c->push_transform();
    c->transform(*tx);
  }
  FloatTwoDCoord pos;
  if(pat == NULL) {
    FloatTwoDCoord ll;		// lower left coords
    FloatTwoDCoord ur;		// upper right
    ScaleBar* cbar = envg->owner->cbar;
    int vl;
    for(vl=0;vl<patspec->n_vals;vl++) {
      ll.x = envg->xcscale(pos.x);
      ll.y = envg->ycscale(pos.y);
      ur.x = envg->xcscale(pos.x+1.0f);
      ur.y = envg->ycscale(pos.y+1.0f);
      c->rect(ll.x, ll.y, ur.x, ur.y, stroke, brush);
      if(vl < patspec->value_names.size) {
	String str = patspec->value_names[vl];
	if(!str.empty())
	  render_text(c, cbar, 0.0f, str, ll, ur, true); // from top
      }
      pos.x++;
      if(pos.x >= patspec->geom.x) {
	pos.x = 0;
	pos.y++;
      }
    }
  }
  else {
    Pattern_G* patg = (Pattern_G*)parent();
    FloatTwoDCoord ll;		// lower left coords
    FloatTwoDCoord ur;		// upper right
    ScaleBar* cbar = envg->owner->cbar;
    EnviroView::ValTextMode vt = envg->owner->owner->val_text;
    int vl;
    for(vl=0;vl<patg->val_buffer.size;vl++) {
      float val = patg->val_buffer[vl];
      ll.x = envg->xcscale(pos.x);
      ll.y = envg->ycscale(pos.y);
      ur.x = envg->xcscale(pos.x+1.0f);
      ur.y = envg->ycscale(pos.y+1.0f);
      switch(envg->owner->owner->val_disp_mode) {
      case EnviroView::COLOR:
	render_color(c, cbar, val, ll, ur);
	break;
      case EnviroView::AREA:
	render_area(c, cbar, val, ll, ur);
	break;
      case EnviroView::LINEAR:
	render_linear(c, cbar, val, ll, ur);
	break;
      }
      if(vt != EnviroView::NONE) {
	if((vt == EnviroView::VALUES) || (vt == EnviroView::BOTH)) {
	  String val_str = (String)patg->val_buffer[vl];
	  if(val_str.contains("e-")) val_str = "0.0";
	  render_text(c, cbar, val, val_str, ll, ur);
	}
	if((vt == EnviroView::NAMES) || (vt == EnviroView::BOTH)) {
	  if(vl < patspec->value_names.size) {
	    String str = patspec->value_names[vl];
	    if(!str.empty())
	      render_text(c, cbar, val, str, ll, ur, true); // from top
	  }
	}
      }
      if(!envg->owner->owner->no_border) {
	c->rect(ll.x, ll.y, ur.x, ur.y, stroke, brush);
      }
      pos.x++;
      if(pos.x >= patspec->geom.x) {
	pos.x = 0;
	pos.y++;
      }
    }
  }
  c->new_path();
  c->move_to(_x[0], _y[0]);
  for (int i = 1; i < _ctrlpts; ++i) {
    c->line_to(_x[i], _y[i]);
  }
  if (_closed) {
    c->close_path();
  }
  c->stroke(stroke, brush);
  if (tx != nil) {
    c->pop_transform();
  }
}

bool PatternBox_G::selectable() {
  // if editing, cut to the text!
  if(envg->owner->viewer->last_button == ivEvent::right)
    return false;
  return true;
}

bool PatternBox_G::select(const ivEvent& e, Tool& tool, bool unselect){
  if(pat != NULL) {
    ToolState& ts = tool.toolstate();
    envg->owner->UpdatePosivPatch((int)ts._l, (int)ts._b, (int)ts._r);
  }
  return Graphic::select(e,tool,unselect);
}

bool PatternBox_G::effect_select(bool set_select) {
  bool temp = Polygon::effect_select(set_select);
  TAPtr obj = patspec;
  if(pat != NULL)
    obj = pat;
  if(set_select)
    envg->selectgroup.LinkUnique(obj);
  else
    envg->selectgroup.Remove(obj);
  return temp;
}

//////////////////////////////////
//	   NValBox_G		//
//////////////////////////////////

NValBox_G::NValBox_G(Pattern_G* eg, Environment_G* eng) :
    Polygon(eng->defbrush, eng->border, NULL, pts, pts, 4, NULL) {
  patg = eg;
  envg = eng;
  ScaleBar* cbar = envg->owner->cbar;
  ivColor* fl;
  ivColor* txt;
  cbar->GetColor(0.0,&fl,&txt);
  fill(fl);
}

void NValBox_G::Build() {
  _x[0] = 0;
  _x[1] = envg->xcscale(1);
  _x[2] = envg->xcscale(1);
  _x[3] = 0;
  _y[0] = 0;
  _y[1] = 0;
  _y[2] = envg->ycscale(1);
  _y[3] = envg->ycscale(1);
  recompute_shape();
}

void NValBox_G::Position() {
  nval_pos.x = (patg->patspec->n_vals-1) % patg->patspec->geom.x;
  nval_pos.y = (patg->patspec->n_vals-1) / patg->patspec->geom.x;
  translate(envg->xcscale((float)nval_pos.x), envg->ycscale((float)nval_pos.y));
}

bool NValBox_G::selectable() {
  // if editing, cut to the text!
  if(envg->owner->viewer->last_button == ivEvent::right)
    return false;
  return true;
}

bool NValBox_G::select(const ivEvent& e, Tool& tool, bool unselect) {
  return Polygon::select(e, tool, unselect);
}

bool NValBox_G::effect_select(bool set_select) {
  bool temp = Polygon::effect_select(set_select);
  return temp;
}

void NValBox_G::draw_gs (ivCanvas* c, Graphic* gs) {
  Polygon::draw_gs(c, gs);
}

bool NValBox_G::grasp_move(const ivEvent&,Tool&,ivCoord, ivCoord) {
  move_pos.Initialize();
  envg->owner->UpdatePosivPatch(patg->patspec->n_vals);
  envg->owner->FixEditorButtons();
  return true;
}

bool NValBox_G::manip_move(const ivEvent& ,Tool& ,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly, ivCoord cx, ivCoord cy) {
  ivCoord dx = cx - lx;
  ivCoord dy = cy - ly;
  if((dx == 0) && (dy == 0))
    return true;

  dx = cx - ix;
  dy = cy - iy;

  TDCoord ng, dg;
  ng.x = (int) (dx * (float)envg->max_size.x);
  ng.y = (int) (dy * (float)envg->max_size.y);

  dg = ng;
  dg -= move_pos;

  if((dg.x != 0) || (dg.y != 0)) {
    TDCoord updtpos = nval_pos + dg;
    if((updtpos.x <0) || (updtpos.y <0) ||
       (updtpos.x >= patg->patspec->geom.x) || (updtpos.y >= patg->patspec->geom.y)) {
      return true;
    }
    move_pos = ng;
    nval_pos.Invert();
    translate(envg->xcscale((float)nval_pos.x), envg->ycscale((float)nval_pos.y));
    nval_pos.Invert();
    TDCoord og = nval_pos;
    nval_pos = updtpos;
    patg->patspec->n_vals = (nval_pos.y * patg->patspec->geom.x) + nval_pos.x + 1;
    patg->patspec->value_names.EnforceSize(patg->patspec->n_vals);
    Position();			// put into the new one
    envg->owner->UpdatePosivPatch(patg->patspec->n_vals);
  }
  return true;
}

bool NValBox_G::effect_move(const ivEvent&,Tool&,ivCoord,
			     ivCoord,ivCoord, ivCoord) {
  if((move_pos.x == 0) && (move_pos.y == 0)) return true;
  move_pos.Initialize();
  // can't do this!
  //  patg->patspec->UpdateAfterEdit();
  patg->patspec->value_names.EnforceSize(patg->patspec->n_vals);
  tabMisc::DelayedUpdateAfterEdit(envg->env);
  //  envg->env->UpdateAllEvents();
  tabMisc::NotifyEdits(patg->patspec);
  envg->reinit_display = true;
  envg->owner->ClearPosivPatch();
  taivMisc::SRIAssignment(patg->patspec,patg->patspec->FindMember("n_vals"));
  return true;
}


//////////////////////////////////
//	  EnvViewer		//
//////////////////////////////////

//  static ivCursor* picker_cursor = nil;
static ivCursor* mywindow_cursor = nil;

EnvViewer::EnvViewer(EnviroView* n, float w, float h, const ivColor* bg)
: GlyphViewer(w,h,bg) {
  envv = n;
};

void EnvViewer::allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext) {
  GlyphViewer::allocate(c,a,ext);
  ((Environment_G*)_root)->owner->InitDisplay(); // initialize upon resize!
}

//////////////////////////////////
//	   EventButton		//
//////////////////////////////////

class EventButton : public ivButton {
public:
  Event* 	event;		// 
  Event_MGroup*	ev_gp;		// event group (event = null, one or the other)
  EnvEditor*	view;
  ivGlyph*	rep;
  String	text;
  bool		gp_events_sel;	// the events within the gp are selected
  int		press_button;	// button actually pressed

  void		press(const ivEvent& e); // process mouse press
  void		release(const ivEvent& e); // process mouse release..
  bool		IsChosen();	// is this button selected

  static EventButton*	GetButton(Event* ev, EnvEditor* enview);
  static EventButton*	GetButton(Event_MGroup* eg, EnvEditor* enview);

  EventButton(ivGlyph* g, ivStyle* s, ivTelltaleState* t,
	       Event* ev, EnvEditor* enview);
  EventButton(ivGlyph* g, ivStyle* s, ivTelltaleState* t,
	       Event_MGroup* eg, EnvEditor* enview);
};

EventButton::EventButton(ivGlyph* g, ivStyle* s, ivTelltaleState* t,
			   Event* ev, EnvEditor* enview)
: ivButton(g,s,t,NULL) {
  rep = g;
  event = ev;
  ev_gp = NULL;
  view = enview;
  press_button = 0;
  text = event->GetDisplayName();
  state()->set(ivTelltaleState::is_choosable | 
	       ivTelltaleState::is_toggle |
	       ivTelltaleState::is_enabled, true);
  state()->set(ivTelltaleState::is_chosen, false);
  gp_events_sel = false;
}

EventButton::EventButton(ivGlyph* g, ivStyle* s, ivTelltaleState* t,
			 Event_MGroup* eg, EnvEditor* enview)
: ivButton(g,s,t,NULL) {

  rep = g;
  event = NULL;
  ev_gp = eg;
  view = enview;
  press_button = 0;
  text = eg->GetDisplayName();
  state()->set(ivTelltaleState::is_choosable | 
	       ivTelltaleState::is_toggle |
	       ivTelltaleState::is_enabled, true);
  state()->set(ivTelltaleState::is_chosen, false);
  gp_events_sel = false;
}

EventButton* EventButton::GetButton(Event* ev, EnvEditor* enview) {
  ivWidgetKit* wkit = ivWidgetKit::instance();

  ivTelltaleState* ts = new ivTelltaleState;
  String nm = ev->GetDisplayName();
  ivGlyph* look = wkit->palette_button_look(wkit->label((char *)nm), ts);
  return new EventButton(look, new ivStyle(wkit->style()), ts, ev, enview);
}

EventButton* EventButton::GetButton(Event_MGroup* eg, EnvEditor* enview) {
  ivWidgetKit* wkit = ivWidgetKit::instance();
  ivTelltaleState* ts = new ivTelltaleState;
  String nm = eg->GetDisplayName();
  ivGlyph* look = wkit->palette_button_look(wkit->label((char *)nm), ts);
  return new EventButton(look, new ivStyle(wkit->style()), ts, eg, enview);
}

bool EventButton::IsChosen() {
  return state()->test(ivTelltaleState::is_chosen);
}

void EventButton::press(const ivEvent& e) {
  press_button = taivM->GetButton(e);
  if(press_button != ivEvent::right) {
    ivButton::press(e);
  }
}

void EventButton::release(const ivEvent&) {
  EnvEditor* myview = view; // since this* might be destroyed during apply
  Event* myevent = event;    // when event_buttons are fixed because an apply
  Event_MGroup* myev_gp = ev_gp;    // when event_buttons are fixed because an apply

  if(press_button == ivEvent::right) {
    if(event != NULL)
      event->Edit(false);
    else if(ev_gp != NULL)
      ev_gp->Edit(false);
  }
  else if(press_button == ivEvent::middle) {
    if(event != NULL) {
      if(view->owner->events_displayed.Find(event) >= 0) {
	view->Apply();
	myview->UnDisplayEvent(myevent);
      }
      else {
	view->DisplayEvent(event);
      }
    }
    else if(ev_gp != NULL) {
      if(view->owner->ev_gps_displayed.Find(ev_gp) >= 0) {
	if(gp_events_sel) {
	  view->Apply();
	  myview->UnDisplayEventGp(myev_gp);
	  gp_events_sel = false;
	}
	else {
	  view->DisplayGpEvents(ev_gp);
	  gp_events_sel = true;
	}
      }
      else {
	view->DisplayEventGp(ev_gp);
      }
    }
  }
  else if(press_button == ivEvent::left) {
    EnvEditor* myview = view; // since this* might be destroyed during apply
    if(event != NULL) {
      // event name was changed
      view->Apply();
      myview->UnDisplayAllEvents();
      myview->DisplayEvent(myevent);
    }
    else {
      if(myview->owner->ev_gps_displayed.Find(ev_gp) >= 0) {
	if(gp_events_sel) {
	  view->Apply();
	  myview->UnDisplayAllEvents();
	  myview->DisplayEventGp(myev_gp);
	  gp_events_sel = false;
	}
	else {
	  view->DisplayGpEvents(ev_gp);
	  gp_events_sel = true;
	}
      }
      else {
	view->Apply();
	myview->UnDisplayAllEvents();
	myview->DisplayEventGp(myev_gp);
	gp_events_sel = false;
      }
    }
  }
}


//////////////////////////////////
//	   SpecButton		//
//////////////////////////////////

class SpecButton : public ivButton {
public:
  EventSpec* 	event;
  EnvEditor*	view;
  ivGlyph*	rep;
  String	text;
  int		press_button;	// button actually pressed

  void		press(const ivEvent& e); // process mouse press
  void		release(const ivEvent& e); // process mouse release..
  bool		IsChosen();	// is this button selected

  static SpecButton*	GetButton(EventSpec* ev, EnvEditor* enview);
  SpecButton(ivGlyph* g, ivStyle* s, ivTelltaleState* t,
	       EventSpec* ev, EnvEditor* enview);
};

SpecButton::SpecButton(ivGlyph* g, ivStyle* s, ivTelltaleState* t,
			   EventSpec* ev, EnvEditor* enview)
: ivButton(g,s,t,NULL) {

  rep = g;
  event = ev;
  view = enview;
  text = event->name;
  press_button = 0;
  state()->set(ivTelltaleState::is_choosable | 
	       ivTelltaleState::is_toggle |
	       ivTelltaleState::is_enabled, true);
  state()->set(ivTelltaleState::is_chosen, false);
}

SpecButton* SpecButton::GetButton(EventSpec* ev, EnvEditor* enview) {
  ivWidgetKit* wkit = ivWidgetKit::instance();
  ivTelltaleState* ts = new ivTelltaleState;
  String nm = ev->name;
  ivGlyph* look = wkit->palette_button_look(wkit->label((char *)nm), ts);
  return new SpecButton(look, new ivStyle(wkit->style()), ts, ev, enview);
}

bool SpecButton::IsChosen() {
  return state()->test(ivTelltaleState::is_chosen);
}

void SpecButton::press(const ivEvent& e) {
  press_button = taivM->GetButton(e);
  if(press_button != ivEvent::right) {
    ivButton::press(e);
  }
}

void SpecButton::release(const ivEvent&) {
  if(press_button == ivEvent::right) {
    event->Edit(false);
  }
  else if(press_button == ivEvent::middle) {
    if(view->owner->specs_displayed.Find(event) >= 0) {
      view->UnDisplaySpec(event);
    }
    else {
      view->DisplaySpec(event);
    }
  }
  else if (press_button == ivEvent::left) {
    EnvEditor* myview = view; // since this* might be destroyed during apply
    EventSpec* myevent = event;    // when event_buttons are fixed because an
    myview->UnDisplayAllSpecs();
    myview->DisplaySpec(myevent);
    return;
  }
}

#include <ta/enter_iv.h>
declareActionCallback(EnvEditor)
implementActionCallback(EnvEditor)
declare_taivMenuCallback(EnvEditor)
implement_taivMenuCallback(EnvEditor)
#include <ta/leave_iv.h>

//////////////////////////////////
//	   EnvEditor		//
//////////////////////////////////

EnvEditor::EnvEditor(Environment* en, EnviroView* ev, ivWindow* w) {
  edit_mode = EVENTS;
  owner = ev;
  env = en;
  viewer = NULL;
  envg = NULL;
  win = w;

  specflags = false;
  patflags = 0;

  event_header = NULL;
  eventhead_type = NULL;
  pattern_header = NULL;
  pathead_type = NULL;

  body = NULL;
  eddeck_patch = NULL;
  edit_deck = NULL;

  pospatch = NULL;
  disp_ckbox = NULL;
  as_ckbox = NULL;

  vtextmenu=NULL;
  dispmdmenu=NULL;
  eventheadmenu = NULL;
  patheadmenu = NULL;
  patflagsmenu = NULL;

  new_event_button = NULL;
  apply_button = NULL;
  apply_button_patch = NULL;

  event_names = NULL;
  evnm_patch = NULL;
  spec_names = NULL;
  spnm_patch = NULL;

  print_patch = NULL;

  scale = NULL;
  cbar = NULL;

  one_espec_sel = NULL;

  trans_dupe_action = NO_TRANS_DUPE;
  spec_type_action = NO_SPEC_TYPE;
  new_action = NEW_SPEC;
  remove_action = NO_REMOVE;
  edit_action = NO_EDIT;
  new2_action = NO_NEW2;
  misc_action = NO_MISC;

  tool_gp = NULL;
  ev_butpatch = NULL;
  sp_butpatch = NULL;
  new_but = NULL;
  remove_but = NULL;
  edit_but = NULL;
  new2_but = NULL;
  misc_but = NULL;
}

EnvEditor::~EnvEditor() {
  ivResource::unref(envg); 	envg = NULL;
  ivResource::unref(viewer); 	viewer = NULL;
  ivResource::unref(evnm_patch); evnm_patch = NULL;
  ivResource::unref(spnm_patch); spnm_patch = NULL;
  ivResource::unref(cbar); 	cbar = NULL;
  ivResource::unref(body); 	body = NULL;
  if (scale != NULL) { taBase::unRefDone(scale); scale = NULL; }

  ivResource::unref(pospatch); pospatch = NULL;
  ivResource::unref(tool_gp); tool_gp = NULL;
  ivResource::unref(ev_butpatch); ev_butpatch = NULL;
  ivResource::unref(sp_butpatch); sp_butpatch = NULL;

  one_espec_sel = NULL;

  if(vtextmenu != NULL)     { delete vtextmenu; vtextmenu = NULL; }
  if(dispmdmenu != NULL)    { delete dispmdmenu; dispmdmenu = NULL; }
  if(eventheadmenu != NULL) { delete eventheadmenu; eventheadmenu = NULL; }
  if(patheadmenu != NULL)   { delete patheadmenu; patheadmenu = NULL; }
  if(patflagsmenu != NULL)  { delete patflagsmenu; patflagsmenu = NULL; }
}

void EnvEditor::Init() {
  scale = new ColorScale();
  taBase::Own(scale,env);

  envg = new Environment_G(env);
  envg->owner = this;
  ivResource::ref(envg);
  envg->Build();

  viewer = new EnvViewer((EnviroView *) owner,ENV_VIEW_INITIAL_X,
			 ENV_VIEW_INITIAL_Y, scale->Get_Background());
  ivResource::ref(viewer);
  viewer->root(envg);
  //  envg->SetMax(envg->env->max_size);
  if(envg->transformer()) {
    ivTransformer* t = envg->transformer();
    ivTransformer idnty;
    *t = idnty;			// return to identity
    envg->scale(ENV_VIEW_INITIAL_X - 2.0f * ENV_VIEW_X_MARG,
		ENV_VIEW_INITIAL_Y - 2.0f * ENV_VIEW_Y_MARG);
    envg->translate(-(.5f * (float)ENV_VIEW_INITIAL_X),
		    -(.5f * (float)ENV_VIEW_INITIAL_Y));
  }
  else {
    envg->scale(ENV_VIEW_INITIAL_X - 2.0f * ENV_VIEW_X_MARG,
		ENV_VIEW_INITIAL_Y - 2.0f * ENV_VIEW_Y_MARG,
		0.5,0.5);
  }
}

ivGlyph* EnvEditor::GetLook() {
  if(!taMisc::iv_active) return NULL;

  ivLayoutKit* layout = ivLayoutKit::instance();
  ivWidgetKit* wkit = ivWidgetKit::instance();

  env = (Environment*)owner->mgr;

  print_patch = new ivPatch(viewer);

  // organized in a top-down fashion, left of the display
  // checkboxes at the top
  disp_ckbox = wkit->check_box("Disp",new ActionCallback
			  (EnvEditor)(this,&EnvEditor::MenuToggleDisplay));
  disp_ckbox->state()->set(ivTelltaleState::is_chosen,owner->display_toggle);

  as_ckbox = wkit->check_box("AutoSc",new ActionCallback
			  (EnvEditor)(this,&EnvEditor::MenuToggleAutoScale));
  as_ckbox->state()->set(ivTelltaleState::is_chosen,owner->auto_scale);

  ClearPosivPatch();

  //////////////////////////////////////
  // now for the event editing functions
  //////////////////////////////////////

  BuildVTextMenu();
  BuildDispMdMenu();
  BuildEventHeadMenu();
  BuildPatHeadMenu();
  BuildPatFlagsMenu();

  new_event_button =
    new HiLightButton
    ("New Evt/Gp",new ActionCallback(EnvEditor)
     (this,&EnvEditor::NewEvent));
  new_event_button->flip_to(1);
  apply_button = 
    new HiLightButton
    ("Apply",new ActionCallback(EnvEditor)(this,&EnvEditor::Apply));
  apply_button->flip_to(0);
  apply_button_patch = new ivPatch(apply_button);

  trans_dupe_but = new ivDeck(TRANS_DUPE_COUNT);
  trans_dupe_but->append
    (wkit->push_button("Xfer/Dupe", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::TransDupeEvents)));
  ((ivButton*) trans_dupe_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  trans_dupe_but->append
    (wkit->push_button("Xfer Event", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::TransDupeEvents)));
  trans_dupe_but->append
    (wkit->push_button("Dupe Event", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::TransDupeEvents)));
  trans_dupe_but->append
    (wkit->push_button("Dupe Group", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::TransDupeEvents)));

  spec_type_but = new ivDeck(SPEC_TYPE_COUNT);
  spec_type_but->append
    (wkit->push_button("Set Spec/Type", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::SpecTypeEvents)));
  ((ivButton*) spec_type_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  spec_type_but->append
    (wkit->push_button("Set Spec/Type", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::SpecTypeEvents)));
  spec_type_but->append
    (wkit->push_button("Change Type", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::SpecTypeEvents)));
  spec_type_but->append
    (wkit->push_button("Select Events", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::SpecTypeEvents)));

  trans_dupe_but->flip_to(0);
  spec_type_but->flip_to(0);

  ev_butpatch = new ivPatch
    (layout->vbox
     (layout->hflexible
      (layout->hbox
       (layout->hflexible(new_event_button),
	layout->hflexible
	(wkit->push_button("Delete",new ActionCallback(EnvEditor)
			   (this,&EnvEditor::DeleteEvents))))),
      layout->hbox
      (layout->hflexible(trans_dupe_but),
       layout->hflexible(spec_type_but)),

      layout->hbox
      (layout->vcenter(layout->hflexible(apply_button_patch),0),
       layout->vcenter
       (layout->hflexible
	(new ScriptButton("Init/Revert",new ActionCallback(EnvEditor)
			  (this,&EnvEditor::InitDisplay),
			  owner->GetPath() + ".InitDisplay();\n")),0)),
      wkit->menu_item_separator_look()));

  ivResource::ref(ev_butpatch);

  event_names = new tbScrollBox();
  event_names->naturalnum = 4;
  event_names->default_height = 24;
  evnm_patch = new ivPatch(event_names);
  ivResource::ref(evnm_patch);

  GetEventLabels();
    
  ivPolyGlyph* event_scrollbox = layout->vbox
    (layout->hcenter
     (wkit->inset_frame
      (layout->hbox
       (layout->hglue(),
	layout->vcenter(wkit->chiseled_label("Events"),0),
	layout->hglue())),0),
     layout->hcenter
     (layout->hbox
      (wkit->vscroll_bar(event_names),
       layout->hnatural(evnm_patch,100)),0));
  
  ivGlyph* event_ctrl = 
    layout->vbox
    (layout->hcenter
     (layout->hflexible
      (wkit->push_button("Edit Specs",new ActionCallback(EnvEditor)
			 (this,&EnvEditor::EditSpecs))),0),
     layout->hcenter
     (layout->hbox
      (wkit->fancy_label("Val Txt:"),
       layout->hfixed(vtextmenu->GetRep(),55),
       wkit->fancy_label("Disp Md:"),
       layout->hfixed(dispmdmenu->GetRep(),50)),0),
     layout->hcenter
     (layout->hbox
      (wkit->fancy_label("Evt Label:"),
       layout->hfixed(eventheadmenu->GetRep(),120)),0),
     layout->hcenter
     (layout->hbox
      (wkit->fancy_label("Pat Label:"),
       layout->hfixed(patheadmenu->GetRep(),120)),0),
     layout->hcenter
     (layout->hbox
      (wkit->fancy_label("Pat Block:"),
       layout->hfixed(patflagsmenu->GetRep(),120)),0),
     wkit->menu_item_separator_look(),
     layout->hcenter(ev_butpatch,0),
     layout->hcenter(event_scrollbox,0));


  //////////////////////////////////////
  // now for the spec editing functions
  //////////////////////////////////////
  tool_gp = new ivTelltaleGroup;
  ivResource::ref(tool_gp);

  int i;
  for(i=0;i<9;i++){
    actbutton[i]=NULL;
  }
  actbutton[Tool::select] = MEButton
    (tool_gp,"Select",
     new ActionCallback(EnvEditor)(this,&EnvEditor::SetSelect));
  ((NoUnSelectButton *) actbutton[Tool::select])->chooseme();

  actbutton[Tool::move] =  MEButton
    (tool_gp,"Move",
     new ActionCallback(EnvEditor)(this,&EnvEditor::SetMove));

  actbutton[Tool::stretch] =  MEButton
    (tool_gp,"ReShape",
     new ActionCallback(EnvEditor)(this,&EnvEditor::SetReShape));

  actbutton[Tool::scale] =  MEButton
    (tool_gp,"ReScale",
     new ActionCallback(EnvEditor)(this,&EnvEditor::SetReScale));


  new_but = new ivDeck(NEW_COUNT);
  new_but->append
    (wkit->push_button("New Spec",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::NewObjects)));
  new_but->append
    (wkit->push_button("New Pattern",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::NewObjects)));
  new_but->append
    (wkit->push_button("Set To Layer",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::NewObjects)));
  // start of "HOT" buttons
  wkit->push_style(taivM->apply_button_style);
  new_but->append
    (wkit->push_button("New Spec",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::NewObjects)));
  wkit->pop_style();

  remove_but = new ivDeck(REMOVE_COUNT);
  remove_but->append
    (wkit->push_button("Remove", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::RemoveSelections)));
  ((ivButton*) remove_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  remove_but->append
    (wkit->push_button("Rmv Spec(s)", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::RemoveSelections)));
  remove_but->append
    (wkit->push_button("Rmv Pat(s)", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::RemoveSelections)));
  remove_but->append
    (wkit->push_button("Rmv Unit(s)", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::RemoveSelections)));

  edit_but = new ivDeck(EDIT_COUNT);
  edit_but->append
    (wkit->push_button("Edit", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::EditSelections)));
  ((ivButton*) edit_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  edit_but->append
    (wkit->push_button("Edit Spec(s)", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::EditSelections)));
  edit_but->append
    (wkit->push_button("Edit Pat(s)", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::EditSelections)));
  edit_but->append
    (wkit->push_button("Edit Name(s)", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::EditSelections)));
  edit_but->append
    (wkit->push_button("Edit Val(s)", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::EditSelections)));
  edit_but->append
    (wkit->push_button("Edit Obj(s)", new ActionCallback(EnvEditor)
		       (this, &EnvEditor::EditSelections)));

  new2_but = new ivDeck(NEW2_COUNT);
  new2_but->append
    (wkit->push_button("New Child",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::New2Objects)));
  ((ivButton*)new2_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  new2_but->append
    (wkit->push_button("New Child",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::New2Objects)));
  new2_but->append
    (wkit->push_button("Edit Names",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::EditValueNames)));

  misc_but = new ivDeck(MISC_COUNT);
  misc_but->append
    (wkit->push_button("Layout/Updt",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::MiscObjects)));
  ((ivButton*)misc_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  misc_but->append
    (wkit->push_button("Layout/Updt",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::MiscObjects)));

  copy_but = new ivDeck(COPY_COUNT);
  copy_but->append
    (wkit->push_button("Copy From",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::CopyObjects)));
  ((ivButton*)copy_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  copy_but->append
    (wkit->push_button("Copy Fm Spec",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::CopyObjects)));
  copy_but->append
    (wkit->push_button("Copy Fm Pat",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::CopyObjects)));

  dupe_but = new ivDeck(DUPE_COUNT);
  dupe_but->append
    (wkit->push_button("Duplicate",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::DupeObjects)));
  ((ivButton*)dupe_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  dupe_but->append
    (wkit->push_button("Dupe Spec",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::DupeObjects)));
  dupe_but->append
    (wkit->push_button("Dupe Pat",new ActionCallback(EnvEditor)
		      (this, &EnvEditor::DupeObjects)));

  new_but->flip_to(0);
  remove_but->flip_to(0);
  edit_but->flip_to(0);
  new2_but->flip_to(0);
  misc_but->flip_to(0);
  copy_but->flip_to(0);
  dupe_but->flip_to(0);

  sp_butpatch = 
    new ivPatch(layout->vbox
		(layout->hbox(taivM->medium_button(new_but),
			      layout->hglue(),
			      taivM->medium_button(remove_but)),
		 layout->hbox(taivM->medium_button(new2_but),
			      layout->hglue(),
			      taivM->medium_button(misc_but)),
		 layout->hbox(taivM->medium_button(copy_but),
			      layout->hglue(),
			      taivM->medium_button(dupe_but)),
		 layout->hbox(taivM->medium_button(edit_but),
			      layout->hglue(),
			      taivM->medium_button
			      (new ScriptButton("Init",new ActionCallback(EnvEditor)
						(this,&EnvEditor::InitDisplay),
						owner->GetPath() + ".InitDisplay();\n"))
			      )));
  ivResource::ref(sp_butpatch);

  spec_names = new tbScrollBox();
  spec_names->naturalnum = 4;
  spec_names->default_height = 24;
  spnm_patch = new ivPatch(spec_names);
  ivResource::ref(spnm_patch);

  GetSpecLabels();
    
  ivPolyGlyph* spec_scrollbox = layout->vbox
    (layout->hcenter
     (wkit->inset_frame
      (layout->hbox
       (layout->hglue(),
	layout->vcenter(wkit->chiseled_label("Event Specs"),0),
	layout->hglue())),0),
     layout->hcenter
     (layout->hbox
      (wkit->vscroll_bar(spec_names),
       layout->hnatural(spnm_patch,100)),0));

  
  ivGlyph* spec_ctrl = 
    layout->vbox
    (layout->hcenter
     (layout->hflexible
      (wkit->push_button("Edit Events",new ActionCallback(EnvEditor)
			 (this,&EnvEditor::EditEvents))),0),
     layout->hbox
     (taivM->medium_button(actbutton[Tool::select]),
      layout->hglue(),	   
      taivM->medium_button(actbutton[Tool::move])),
     layout->hbox	   
     (taivM->medium_button(actbutton[Tool::stretch]),
      layout->hglue(),	   
      taivM->medium_button(actbutton[Tool::scale])),
     sp_butpatch,
     wkit->menu_item_separator_look(),
     layout->hcenter(spec_scrollbox,0));

  edit_deck = new ivDeck(2);
  edit_deck->append(event_ctrl);
  edit_deck->append(spec_ctrl);
  edit_deck->flip_to(0);

  eddeck_patch = new ivPatch(edit_deck);
  
  cbar = new HPScaleBar(owner->scale_range.min,owner->scale_range.max,true,true,scale,16,14,18);
  ivResource::ref(cbar);
  cbar->SetAdjustNotify(new ActionCallback(EnvEditor)
			(this,&EnvEditor::CBarAdjustNotify));

  const ivColor* bg_color = env->GetEditColor();
  if(bg_color == NULL) bg_color = wkit->background();

  body = wkit->inset_frame
    (layout->hbox
     (layout->vcenter
      (wkit->outset_frame
       (new ivBackground
	(layout->vbox
	 (layout->hbox(layout->vcenter(disp_ckbox,0),layout->hglue(),
		       layout->vcenter(as_ckbox,0),layout->hglue(),
		       layout->vcenter(wkit->inset_frame(pospatch),0),
		       layout->hglue()), 
	  wkit->menu_item_separator_look(),
	  eddeck_patch), bg_color)),0),
      layout->vcenter
      (layout->vbox
       (layout->hcenter(wkit->inset_frame(layout->flexible(print_patch)),0),
	layout->hcenter(cbar->GetLook(),0)),0)));

  ivResource::ref(body);		// gotta ref the body..

  if(owner->auto_scale == true)  cbar->UpdateScaleValues();
  InitDisplay();

  return body;
}

void EnvEditor::UpdateDisplay(){ // ignores display_toggle
  envg->RevertValues();
  UpdateMinMaxScale();
  viewer->update_from_state();
  // hilight new_event_button if there are no events
  if(new_event_button != NULL)
    new_event_button->flip_to(env->events.leaves == 0);
  if((win->is_mapped()) && (win->bound())) win->repair();
}

void EnvEditor::update_from_state() {
  if(owner->display_toggle) UpdateDisplay();
}

void EnvEditor::InitDisplay(){
  if(new_event_button != NULL)
    new_event_button->flip_to(env->events.leaves == 0);
  if(apply_button != NULL) apply_button->flip_to(0); // turn off highlight
  apply_button_patch->redraw();
  FixEditorButtons();
  GetEventLabels();
  GetSpecLabels();
  if((edit_mode == SPECS) || owner->display_toggle) {
    UpdateMinMaxScale();
    envg->Build();
  }
  else {
    envg->Reset();
  }
  envg->transformer(NULL);
  envg->no_text_extent = true;
  viewer->init_graphic();
  envg->no_text_extent = false;
  envg->scale(viewer->viewallocation().x_allotment().span() - 2.0f * ENV_VIEW_X_MARG,
	      viewer->viewallocation().y_allotment().span() - 2.0f * ENV_VIEW_Y_MARG);
  envg->ReCenter();
  viewer->Update_All();
}

void EnvEditor::EditSpecs() {
  edit_mode = SPECS;
  edit_deck->flip_to(1);
  eddeck_patch->reallocate();
  eddeck_patch->redraw();
  SelectActButton(Tool::select);
  InitDisplay();
}

void EnvEditor::EditEvents() {
  edit_mode = EVENTS;
  edit_deck->flip_to(0);
  eddeck_patch->reallocate();
  eddeck_patch->redraw();
  if(viewer) 
    viewer->cur_tool(Tool::select);
  InitDisplay();
}

void EnvEditor::BuildVTextMenu(){
  vtextmenu = new taivMenu(taivMenu::menubar,taivMenu::normal_update,
			    taivMenu::skinny_small);
  TypeDef* flagenm = TA_EnviroView.sub_types.FindName("ValTextMode");
  int i;
  for(i=0;i<flagenm->enum_vals.size;i++) {
    vtextmenu->AddItem(flagenm->enum_vals[i]->name,NULL,taivMenu::use_default,
		       new taivMenuCallback(EnvEditor)
		       (this,&EnvEditor::SetVText));
  }

  vtextmenu->GetImage(owner->val_text);
  if(vtextmenu->items.FastEl(owner->val_text)->rep) {
    vtextmenu->items.FastEl(owner->val_text)
      ->rep->state()->set(ivTelltaleState::is_chosen,false);
  }
}

void EnvEditor::BuildDispMdMenu(){
  dispmdmenu = new taivMenu(taivMenu::menubar,taivMenu::normal_update,
			    taivMenu::skinny_small);
  TypeDef* flagenm = TA_EnviroView.sub_types.FindName("ValDispMode");
  int i;
  for(i=0;i<flagenm->enum_vals.size;i++) {
    dispmdmenu->AddItem(flagenm->enum_vals[i]->name,NULL,taivMenu::use_default,
		       new taivMenuCallback(EnvEditor)
		       (this,&EnvEditor::SetDispMd));
  }
  dispmdmenu->GetImage(owner->val_disp_mode);
  if(dispmdmenu->items.FastEl(owner->val_disp_mode)->rep) {
    dispmdmenu->items.FastEl(owner->val_disp_mode)
      ->rep->state()->set(ivTelltaleState::is_chosen,false);
  }
}

void EnvEditor::BuildPatFlagsMenu(){
  patflagsmenu = new taivHierMenu(taivMenu::menubar,taivMenu::normal_update,
			      taivMenu::small);
  
  patflagsmenu->AddItem("Pattern Values",NULL,taivMenu::use_default,
			new taivMenuCallback(EnvEditor)
			(this,&EnvEditor::SetPatFlags));

  int cur_no = patflagsmenu->cur_sno;
  patflagsmenu->AddSubMenu("Pattern Flags");
  TypeDef* flagenm = TA_PatternSpec.sub_types.FindName("PatFlags");
  int i;
  for(i=1;i<flagenm->enum_vals.size;i++) {
    String nm = flagenm->enum_vals[i]->name;
    if((nm == "TARG_EXT_FLAG") || (nm == "COMP_TARG_FLAG") || (nm == "COMP_EXT_FLAG") ||
       (nm == "COMP_TARG_EXT_FLAG") || (nm == "TARG_EXT_VALUE")) continue;
    patflagsmenu->AddItem(nm,NULL,taivMenu::use_default,
			  new taivMenuCallback(EnvEditor)
			  (this,&EnvEditor::SetPatFlags));
  }
  patflagsmenu->SetSub(cur_no);
  patflagsmenu->AddSubMenu("Global PatternSpec Flags");

  for(i=1;i<flagenm->enum_vals.size;i++) {
    String nm = flagenm->enum_vals[i]->name;
    if((nm == "TARG_EXT_FLAG") || (nm == "COMP_TARG_FLAG") || (nm == "COMP_EXT_FLAG") ||
       (nm == "COMP_TARG_EXT_FLAG") || (nm == "TARG_EXT_VALUE")) continue;
    patflagsmenu->AddItem(nm,NULL,taivMenu::use_default,
			  new taivMenuCallback(EnvEditor)
			  (this,&EnvEditor::SetPatFlags));
  }

  patflagsmenu->SetSub(cur_no);
  patflags = PatternSpec::NO_FLAGS;
  specflags = false;

  patflagsmenu->SetMLabel("Pattern Values");
}

// AddTypes links mememberdefs in TypeDef td
// which are tagged with the EnvEditor comment directive
// to the Memberspace ms

void EnvEditor::AddTypes(MemberSpace* ms, TypeSpace* ts, TypeDef* td){
  if((ms == NULL) || (ts == NULL) || (td == NULL)) return;
  bool c = false;
  int i;
  MemberDef* tempdef;
  for(i=0;i<td->members.size;i++){
    tempdef = (MemberDef *) td->members[i];
    if((tempdef->opts.FindContains("ENVIROVIEW") != -1) ||
	(tempdef->name == "name")) {
      for(int j=0;j<ms->size;j++){ // check to make sure md is not already on list
	MemberDef* ckdef  = ms->FastEl(j);
	if((ckdef == tempdef) && (td->InheritsFrom(ts->FastEl(j)))) {
	  c = true;  break; 
	}
      }
      if(c == true) { c = false; continue;}
      ms->Link(tempdef);
      ts->Link(td);
    }
  }
  for(i=0;i<td->children.size;i++){
    AddTypes(ms,ts,td->children[i]);
  }
}

void EnvEditor::BuildEventHeadMenu(){
  if(event_membspace.size == 0) {
    AddTypes(&event_membspace,&event_typespace,&TA_Event);
    AddTypes(&event_membspace,&event_typespace,&TA_EventSpec);
  }
  eventheadmenu = new taivMenu(taivMenu::menubar,taivMenu::normal_update,
			    taivMenu::skinny_small);

  eventheadmenu->AddItem("NONE",NULL,taivMenu::use_default,
		       new taivMenuCallback(EnvEditor)
		       (this,&EnvEditor::SetEventHead));

  for(int i=0;i<event_membspace.size;i++){
    MemberDef* md = (MemberDef *) event_membspace[i];
    String menuname =  md->OptionAfter("EnvEditor_");
    if(menuname == "") menuname = md->name;
    menuname = event_typespace[i]->name + "::" + menuname;
    eventheadmenu->AddItem(menuname,NULL,taivMenu::use_default,
			 new taivMenuCallback(EnvEditor)
			 (this,&EnvEditor::SetEventHead));
  }
  event_header =  (MemberDef*) event_membspace[0];
  eventhead_type = (TypeDef*) event_typespace[0];
  if(!owner->event_header.empty())
    SetEventHeadStr(owner->event_header);
  else {
    eventheadmenu->GetImage(1);
    if(eventheadmenu->items.FastEl(1)->rep){
      eventheadmenu->items.FastEl(1)->rep->state()->set(ivTelltaleState::is_chosen,false);
    }
  }
}

void EnvEditor::BuildPatHeadMenu(){
  if(pat_membspace.size == 0) {
    AddTypes(&pat_membspace,&pat_typespace,&TA_Pattern);
    AddTypes(&pat_membspace,&pat_typespace,&TA_PatternSpec);
  }
  patheadmenu = new taivMenu(taivMenu::menubar,taivMenu::normal_update,
			    taivMenu::skinny_small);

  patheadmenu->AddItem("NONE",NULL,taivMenu::use_default,
		       new taivMenuCallback(EnvEditor)
		       (this,&EnvEditor::SetPatHead));

  for(int i=0;i<pat_membspace.size;i++){
    MemberDef* md = (MemberDef *) pat_membspace[i];
    String menuname =  md->OptionAfter("EnvEditor");
    if(menuname == "") menuname = md->name;
    menuname = pat_typespace[i]->name + "::" + menuname;
    patheadmenu->AddItem(menuname,NULL,taivMenu::use_default,
			 new taivMenuCallback(EnvEditor)
			 (this,&EnvEditor::SetPatHead));
  }
  if(!owner->pattern_header.empty())
    SetPatHeadStr(owner->pattern_header);
  else {
    patheadmenu->GetImage(0);
    if(patheadmenu->items.FastEl(0)->rep){
      patheadmenu->items.FastEl(0)->rep->state()->set(ivTelltaleState::is_chosen,false);
    }
  }
}

void EnvEditor::SetVText(taivMenuEl* sel) {
  owner->val_text = (EnviroView::ValTextMode)sel->itm_no;
  sel->rep->state()->set(ivTelltaleState::is_chosen,false);
  InitDisplay();
  tabMisc::NotifyEdits(owner);
}

void EnvEditor::SetDispMd(taivMenuEl* sel) {
  owner->val_disp_mode = (EnviroView::ValDispMode)sel->itm_no;
  sel->rep->state()->set(ivTelltaleState::is_chosen,false);
  InitDisplay();
  tabMisc::NotifyEdits(owner);
}

void EnvEditor::SetPatFlags(taivMenuEl* sel) { // set patflags from menu
  Apply(); // apply current settings
  patflags = PatternSpec::NO_FLAGS; specflags = false;
  int submnu = ((taivHierEl *) sel)->sub_no;
  if(submnu > 0) {		// submnu 1 = pattern flags, submnu 2 = global (spec) flags
    if(submnu == 2)
      specflags = true;		// global (spec) flags
    int flag_sel = sel->itm_no;	// which flag item selected (TARG_FLAG (=0) - NO_APPLY (=12))
    // can't just bit-shift because some combinations are represented and others are pure flags
    // (i.e., TARG_EXT_FLAG is OR of TARG_FLAG and EXT_FLAG, but there aren't such combinations
    // for TARG_VALUE and TARG_FLAG, for example..
    switch (flag_sel) {
    case 0: patflags = PatternSpec::TARG_FLAG; break;
    case 1: patflags = PatternSpec::EXT_FLAG; break;
    case 2: patflags = PatternSpec::COMP_FLAG; break;
    case 3: patflags = PatternSpec::TARG_VALUE; break;
    case 4: patflags = PatternSpec::EXT_VALUE; break;
    case 5: patflags = PatternSpec::NO_UNIT_FLAG; break;
    case 6: patflags = PatternSpec::NO_UNIT_VALUE; break;
    case 7: patflags = PatternSpec::NO_APPLY; break;
    }
  }
  if((patflags != PatternSpec::NO_FLAGS) &&
     (owner->auto_scale == true) && (cbar != NULL))
    cbar->SetMinMax(-1,1);
  UpdateDisplay();
}

void EnvEditor::SetEventHead(taivMenuEl* sel){ // sets EventHeader from menu
  if(sel->itm_no == 0) {
    event_header = NULL;
    eventhead_type = NULL;
    owner->event_header = "NONE";
  }
  else {
    event_header =  (MemberDef*) event_membspace[sel->itm_no -1];
    eventhead_type = (TypeDef*) event_typespace[sel->itm_no -1];
    owner->event_header = eventhead_type->name + "::" + event_header->name;
  }
  sel->rep->state()->set(ivTelltaleState::is_chosen,false);
  tabMisc::NotifyEdits(owner);
  InitDisplay();
}

void EnvEditor::SetPatHead(taivMenuEl* sel){ // sets PatternHeader from menu
  if(sel->itm_no == 0) { 
    pattern_header = NULL;
    pathead_type = NULL;
    owner->pattern_header = "NONE";
  }
  else {
    pattern_header =  (MemberDef*) pat_membspace[sel->itm_no -1];
    pathead_type = (TypeDef *) pat_typespace[sel->itm_no -1];
    owner->pattern_header = pathead_type->name + "::" + pattern_header->name;
  }
  sel->rep->state()->set(ivTelltaleState::is_chosen,false);
  tabMisc::NotifyEdits(owner);
  InitDisplay();
}

void EnvEditor::SetPatHeadStr(const char* nm){ // sets PatternHeader from string
  String typ = nm;
  if(typ == "NONE") {
    patheadmenu->GetImage(0);
    pattern_header = NULL;
    pathead_type = NULL;
    return;
  }
  String mbr = typ.after("::");
  typ = typ.before("::");
  TypeDef* td = taMisc::types.FindName(typ);
  if(td == NULL) {
    taMisc::Error("SetPatHead: Couldn't find type:", typ, "for pattern display");
    return;
  }
  MemberDef* md = td->members.FindName(mbr);
  if(md == NULL) {
    taMisc::Error("SetPatHead: Couldn't find member", mbr, "in type:", typ, "for pattern display");
    return;
  }
  pattern_header = md;
  pathead_type = td;
  int i;
  for(i=0;i<pat_membspace.size && i< pat_typespace.size; i++) {
    if((md == pat_membspace[i]) && (td == pat_typespace[i])) {
      patheadmenu->GetImage(i+1);
      if(patheadmenu->items.FastEl(i+1)->rep) {
	patheadmenu->items.FastEl(i+1)->rep->state()->set(ivTelltaleState::is_chosen,false);
      }
      break;
    }
  }
}

void EnvEditor::SetEventHeadStr(const char* nm){ // sets PatternHeader from string
  String typ = nm;
  if(typ == "NONE") {
    eventheadmenu->GetImage(0);
    event_header = NULL;
    eventhead_type = NULL;
    return;
  }
  String mbr = typ.after("::");
  typ = typ.before("::");
  TypeDef* td = taMisc::types.FindName(typ);
  if(td == NULL) {
    taMisc::Error("SetEventHead: Couldn't find type:", typ, "for event display");
    return;
  }
  MemberDef* md = td->members.FindName(mbr);
  if(md == NULL) {
    taMisc::Error("SetEventHead: Couldn't find member", mbr, "in type:", typ, "for event display");
    return;
  }

  event_header = md;
  eventhead_type = td;
  int i;
  for(i=0;i<event_membspace.size && i< event_typespace.size; i++) {
    if((md == event_membspace[i]) && (td == event_typespace[i])) {
      eventheadmenu->GetImage(i+1);
      if(eventheadmenu->items.FastEl(i+1)->rep) {
	eventheadmenu->items.FastEl(i+1)->rep->state()->set(ivTelltaleState::is_chosen,false);
      }
      break;
    }
  }
}

ivButton* EnvEditor::MEButton(ivTelltaleGroup* gp, char* txt, ivAction* a) {
  ivWidgetKit* wkit = ivWidgetKit::instance();
  ivTelltaleState* t = new Teller((TelltaleFlags)0, (ivAction*)NULL);
  ivButton* rval = new NoUnSelectButton
    (wkit->palette_button_look(wkit->label(txt), t), wkit->style(),t,a);
  rval->state()->set(ivTelltaleState::is_choosable | ivTelltaleState::is_toggle |
	ivTelltaleState::is_enabled, true);
  rval->state()->join(gp);
  return rval;
}

void EnvEditor::GetEventLabels() {
  int tot_buts = env->events.leaves + env->events.gp.size; // total group + events buttons
  while(event_names->count() > tot_buts) {
    event_names->remove(event_names->count() -1);
  }
  // first do the top-level events
  int butidx = 0;
  Event* ev;
  int i;
  for(i=0;i<env->events.size;i++) {
    ev = (Event*)env->events.FastEl(i);
    AddEvent(ev, butidx++);
  }
  // then the sub-groups
  Event_MGroup* eg;
  for(i=0;i<env->events.gp.size;i++) {
    eg = (Event_MGroup*)env->events.gp.FastEl(i);
    AddEventGp(eg, butidx);
    butidx += 1 + eg->leaves;
  }
  ivResource::flush();
  evnm_patch->reallocate();
  evnm_patch->redraw();
}

void EnvEditor::DisplayEvent(Event* ev) {
#ifdef DMEM_COMPILE
  if(taMisc::dmem_nprocs > 1) {
    taivMisc::RecordScript(owner->GetPath() + ".SelectEvent(" + ev->GetPath() + ");\n");
    return;
  }
#endif
  owner->events_displayed.Link(ev);
  int butidx = GetEventButIdx(ev);
  if((butidx >= 0) && (butidx < (int)event_names->count())) {
    EventButton* eb = (EventButton*)event_names->component(butidx);
    ivTelltaleState* s = eb->state();
    s->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, true);
  }
  InitDisplay();
}

void EnvEditor::UnDisplayEvent(Event* ev) {
#ifdef DMEM_COMPILE
  if(taMisc::dmem_nprocs > 1) {
    taivMisc::RecordScript(owner->GetPath() + ".DeselectEvent(" + ev->GetPath() + ");\n");
    return;
  }
#endif
  UnDisplayEvent_impl(ev);
  owner->events_displayed.Remove(ev);
  InitDisplay();
}

void EnvEditor::UnDisplayEvent_impl(Event* ev) {
  int butidx = GetEventButIdx(ev);
  if((butidx >= 0) && (butidx < (int)event_names->count())) {
    EventButton* eb = (EventButton*)event_names->component(butidx);
    ivTelltaleState* s = eb->state();
    s->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, false);
  }
}

void EnvEditor::UnDisplayAllEvents() {
#ifdef DMEM_COMPILE
  if(taMisc::dmem_nprocs > 1) {
    taivMisc::RecordScript(owner->GetPath() + ".DeselectAllEvents();\n");
    return;
  }
#endif
  UnDisplayAllEvents_impl();
  owner->events_displayed.RemoveAll();
  owner->ev_gps_displayed.RemoveAll();
}

void EnvEditor::UnDisplayAllEvents_impl() {
  int i;
  for(i = (int)event_names->count()-1; i >= 0; i--) {
    EventButton* eb = (EventButton *) event_names->component(i);
    ivTelltaleState* s = eb->state();
    s->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, false);
  }
}

void EnvEditor::DisplayEventGp(Event_MGroup* eg) {
#ifdef DMEM_COMPILE
  if(taMisc::dmem_nprocs > 1) {
    taivMisc::RecordScript(owner->GetPath() + ".SelectEventGp(" + eg->GetPath() + ");\n");
    return;
  }
#endif
  owner->ev_gps_displayed.Link(eg);
  int butidx = GetEventGpButIdx(eg);
  if((butidx >= 0) && (butidx < (int)event_names->count())) {
    EventButton* eb = (EventButton*)event_names->component(butidx);
    ivTelltaleState* s = eb->state();
    s->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, true);
  }
  InitDisplay();
}

void EnvEditor::DisplayGpEvents(Event_MGroup* eg) {
#ifdef DMEM_COMPILE
  if(taMisc::dmem_nprocs > 1) {
    taivMisc::RecordScript(owner->GetPath() + ".SelectGpEvents(" + eg->GetPath() + ");\n");
    return;
  }
#endif
  owner->ev_gps_displayed.Remove(eg);
  int butidx = GetEventGpButIdx(eg);
  if((butidx >= 0) && (butidx < (int)event_names->count())) {
    EventButton* eb = (EventButton*)event_names->component(butidx);
    ivTelltaleState* s = eb->state();
    s->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, false);
    butidx++;

    Event* ev;
    taLeafItr ei;
    FOR_ITR_EL(Event, ev, eg->, ei) {
      if(butidx < (int)event_names->count()) {
	EventButton* eb = (EventButton *) event_names->component(butidx++);
	ivTelltaleState* s = eb->state();
	s->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, true);
      }
      owner->events_displayed.Link(ev);
    }
  }
  InitDisplay();
}

void EnvEditor::UnDisplayEventGp(Event_MGroup* eg) {
#ifdef DMEM_COMPILE
  if(taMisc::dmem_nprocs > 1) {
    taivMisc::RecordScript(owner->GetPath() + ".DeselectEventGp(" + eg->GetPath() + ");\n");
    return;
  }
#endif
  int butidx = GetEventGpButIdx(eg);
  if((butidx >= 0) && (butidx < (int)event_names->count())) {
    EventButton* eb = (EventButton*)event_names->component(butidx);
    ivTelltaleState* s = eb->state();
    s->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, false);
    butidx++;
    
    owner->ev_gps_displayed.Remove(eg);
  
    Event* ev;
    taLeafItr ei;
    FOR_ITR_EL(Event, ev, eg->, ei) {
      if(butidx < (int)event_names->count()) {
	EventButton* eb = (EventButton *) event_names->component(butidx++);
	ivTelltaleState* s = eb->state();
	s->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, false);
      }
      owner->events_displayed.Remove(ev);
    }
  }
  InitDisplay();
}

void EnvEditor::RemoveEvent(Event* ev) {
  if(owner->window == NULL) return;
  if(owner->events_displayed.Remove(ev)) {
    if(edit_mode == EVENTS) {
      envg->safe_damage_me(viewer->canvas());
      envg->Reset();		// removing a displayed event: reset display..
      envg->safe_damage_me(viewer->canvas());
    }
  }
  int butidx = GetEventButIdx(ev);
  if((butidx >= 0) && (butidx < (int)event_names->count())) {
    event_names->remove(butidx);
  }
  evnm_patch->reallocate();
  evnm_patch->redraw();
  winbMisc::DelayedMenuUpdate(owner);
}

void EnvEditor::RemoveEventGp(Event_MGroup* eg) {
  if(owner->window == NULL) return;
  if(owner->ev_gps_displayed.Remove(eg)) {
    if(edit_mode == EVENTS) {
      envg->safe_damage_me(viewer->canvas());
      envg->Reset();		// removing a displayed event: reset display..
      envg->safe_damage_me(viewer->canvas());
    }
  }
  int butidx = GetEventGpButIdx(eg);
  if((butidx >= 0) && (butidx < (int)event_names->count())) {
    event_names->remove(butidx);
  }
  ivResource::flush();
  evnm_patch->reallocate();
  evnm_patch->redraw();
  winbMisc::DelayedMenuUpdate(owner);
}

void EnvEditor::NewEvent() {
  if(owner->ev_gps_displayed.size == 1)
    gpivGroupNew::New((TAGPtr) owner->ev_gps_displayed.FastEl(0), NULL, NULL, owner->window);
  else
    gpivGroupNew::New((TAGPtr) &(env->events), NULL, NULL, owner->window);
  if(new_event_button != NULL) new_event_button->flip_to(0); // turn off highlight
  InitDisplay();
  taivMisc::RecordScript(owner->GetPath() + ".InitDisplay();\n");
  winbMisc::DelayedMenuUpdate(&(env->events));
  return;
}

void EnvEditor::DeleteEvents() {
  int okdel = taMisc::Choice("Are you sure you want to delete these Events/Groups?","Yes - Delete","No - Cancel");
  if(okdel == 1) return;
  int i;
  for(i=owner->events_displayed.size-1;i>=0;i--) {
    Event* ev = (Event*)owner->events_displayed[i];
    taivMisc::RecordScript(env->events.GetPath() + ".RemoveLeafEl(" + ev->GetPath() + ");\n");
    DMEM_GUI_RUN_IF {
      env->events.RemoveLeaf(ev);
    }
  }
  for(i=owner->ev_gps_displayed.size-1;i>=0;i--) {
    Event_MGroup* emg = (Event_MGroup*) owner->ev_gps_displayed[i];
    taivMisc::RecordScript(env->events.gp.GetPath() + ".RemoveEl(" + emg->GetPath() + ");\n");
    DMEM_GUI_RUN_IF {
      env->events.gp.Remove(emg);
    }
  }
  winbMisc::DelayedMenuUpdate(&(env->events));
  winbMisc::DamageWindow(owner->window);
//  if(window != NULL) window->repair();
}

int EnvEditor::GetEventButIdx(Event* ev) {
  int butidx = -2;
  if(ev->owner == &(env->events)) {
    butidx = ev->index;
  }
  else {
    int nidx = env->events.size; // first add the events
    int g;
    for(g=0;g<env->events.gp.size;g++) {
      Event_MGroup* tg = (Event_MGroup*)env->events.gp.FastEl(g);
      int i = tg->FindLeaf(ev);
      if(i >= 0) {
	butidx = nidx + i;
	break;
      }
      nidx += tg->leaves;
    }
  }
  return butidx;
}

void EnvEditor::AddEvent(Event* ev, int butidx) {
  if(!taMisc::iv_active || (owner->window == NULL) || (event_names == NULL))
    return;
  bool refresh = false;
  if(butidx == -2) {		// clue to look up index
    butidx = GetEventButIdx(ev);
    if(butidx < 0) return;	// something bad happened
    refresh = true;		// refresh display -- not going to happen later
  }
  
  int enc = (int)event_names->count();
  if(enc > MAX_EVENTS) {
    taMisc::Error("*** Maxium number of events for view (", (String)MAX_EVENTS , ") exceeded",
		  "Close the EnvEditor to prevent seeing this message again!");
    ivResource::flush();
    return;
  }

  if(enc > butidx) {
    EventButton* but = (EventButton *)(event_names->component(butidx));
    if((but->event != ev) || (but->text != ev->GetDisplayName())) {
      event_names->replace(butidx,EventButton::GetButton(ev,this));
    }
  }
  else {
    event_names->append(EventButton::GetButton(ev,this));
  }
  if(butidx >= event_names->count()) { // somehow out of whack with count..
    ivResource::flush();
    return;
  }
  EventButton* but = (EventButton *) event_names->component(butidx);
  if(owner->events_displayed.Find(ev) == -1) {
    but->state()->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, false);
  }
  else {
    but->state()->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, true);
  }
  if(refresh) {
    evnm_patch->reallocate();
    evnm_patch->redraw();
    ivResource::flush();
  }
}

int EnvEditor::GetEventGpButIdx(Event_MGroup* eg) {
  int butidx = -2;
  int nidx = env->events.size; // first add the events
  int g;
  for(g=0;g<env->events.gp.size;g++) {
    Event_MGroup* tg = (Event_MGroup*)env->events.gp.FastEl(g);
    if(tg == eg) {
      butidx = nidx;
      break;
    }
    nidx += tg->leaves;
  }
  return butidx;
}

void EnvEditor::AddEventGp(Event_MGroup* eg, int butidx) {
  if(!taMisc::iv_active || (owner->window == NULL) || (event_names == NULL))
    return;
  bool refresh = false;
  if(butidx == -2) {		// clue to look up index
    butidx = GetEventGpButIdx(eg);
    if(butidx < 0) return;	// something bad happened
    refresh = true;		// refresh display -- not going to happen later
  }
  
  int enc = (int)event_names->count();
  if(enc > MAX_EVENTS) {
    taMisc::Error("*** Maxium number of events for view (", (String)MAX_EVENTS , ") exceeded",
		  "Close the EnvEditor to prevent seeing this message again!");
    ivResource::flush();
    return;
  }

  if(enc > butidx) {
    EventButton* but = (EventButton *)(event_names->component(butidx));
    if((but->ev_gp != eg) || (but->text != eg->GetDisplayName())) {
      event_names->replace(butidx,EventButton::GetButton(eg,this));
    }
  }
  else {
    event_names->append(EventButton::GetButton(eg,this));
  }
  if(butidx >= event_names->count()) { // somehow out of whack with count..
    ivResource::flush();
    return;
  }
  EventButton* but = (EventButton *) event_names->component(butidx);
  if(owner->ev_gps_displayed.Find(eg) == -1) {
    but->state()->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, false);
  }
  else {
    but->state()->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, true);
  }

  // now add all the sub-events in this group
  butidx++;			// add one to skip past group
  Event* ev;
  taLeafItr ei;
  FOR_ITR_EL(Event, ev, eg->, ei) {
    AddEvent(ev, butidx++);
  }

  if(refresh) {
    evnm_patch->reallocate();
    evnm_patch->redraw();
    ivResource::flush();
  }
}

void EnvEditor::HighlightApply() {
  if(apply_button != NULL) {
    apply_button->flip_to(1); // turn on highlight
    apply_button_patch->redraw();
    win->repair();
  }
}

void EnvEditor::Apply() {
  envg->ApplyValues();
  if(apply_button != NULL) {
    apply_button->flip_to(0); // turn off highlight
    apply_button_patch->redraw();
  }
}

void EnvEditor::AddSpec(EventSpec* ev, int& leaf_idx) {
  if(!taMisc::iv_active || (owner->window == NULL) || (spec_names == NULL))
    return;
  bool refresh = false;
  if(leaf_idx == -2) {		// clue to look up index
    leaf_idx = env->event_specs.FindLeaf(ev);
    if(leaf_idx < 0) return;	// something bad happened.
    refresh = true;		// refresh display -- not going to happen later
  }
  
  int enc = (int)spec_names->count();
  if(enc > leaf_idx) {
    SpecButton* but = (SpecButton *)(spec_names->component(leaf_idx));
    if((but->event != ev) || (but->text != ev->name)) {
      spec_names->replace(leaf_idx,SpecButton::GetButton(ev,this));
    }
  }
  else {
    spec_names->append(SpecButton::GetButton(ev,this));
  }
  if(leaf_idx >= spec_names->count()) { // somehow out of whack with count..
    ivResource::flush();
    return;
  }
  SpecButton* but = (SpecButton *) spec_names->component(leaf_idx);
  if(owner->specs_displayed.Find(ev) == -1) {
    but->state()->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, false);
  }
  else {
    but->state()->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, true);
  }
  if(refresh) {
    spnm_patch->reallocate();
    spnm_patch->redraw();
  }
  ivResource::flush();

  // iterative -- do the kids!
  EventSpec* evc;
  taLeafItr ic;
  FOR_ITR_EL(EventSpec, evc, ev->children., ic) {
    AddSpec(evc, ++leaf_idx);
  }
}

void EnvEditor::GetSpecLabels(){
  while(spec_names->count() > env->event_specs.leaves){
    spec_names->remove(spec_names->count() -1);
  }
  int leaf_idx = 0;
  EventSpec* ev;
  taLeafItr i;
  FOR_ITR_EL(EventSpec, ev, env->event_specs., i) {
    AddSpec(ev, leaf_idx);
    leaf_idx++;
  }
  spnm_patch->reallocate();
  spnm_patch->redraw();
  if((leaf_idx == 1) && (env->event_specs.size == 1)) { // only one spec, automatically display it!
    ev = (EventSpec*)env->event_specs.FastEl(0);
    if(owner->specs_displayed.Find(ev) < 0) {
      DisplaySpec(ev);
    }
  }
}

void EnvEditor::DisplaySpec(EventSpec* ev) {
  owner->specs_displayed.Link(ev);
  int i;
  for(i = (int)spec_names->count()-1; i >= 0; i--) {
    SpecButton* eb = (SpecButton *) spec_names->component(i);
    if(eb->event == ev) {
      ivTelltaleState* s = eb->state();
      s->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, true);
      break;
    }
  }
  InitDisplay();
}

void EnvEditor::UnDisplaySpec(EventSpec* ev) {
  UnDisplaySpec_impl(ev);
  owner->specs_displayed.Remove(ev);
  InitDisplay();
}

void EnvEditor::UnDisplaySpec_impl(EventSpec* ev) {
  int i;
  for(i = (int)spec_names->count()-1; i >= 0; i--) {
    SpecButton* eb = (SpecButton *) spec_names->component(i);
    if(eb->event == ev) {
      ivTelltaleState* s = eb->state();
      s->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, false);
      break;
    }
  }
}

void EnvEditor::UnDisplayAllSpecs() {
  UnDisplayAllSpecs_impl();
  owner->specs_displayed.RemoveAll();
}

void EnvEditor::UnDisplayAllSpecs_impl() {
  int i;
  for(i = (int)spec_names->count()-1; i >= 0; i--) {
    SpecButton* eb = (SpecButton *) spec_names->component(i);
    ivTelltaleState* s = eb->state();
    s->set(ivTelltaleState::is_chosen | ivTelltaleState::is_active, false);
  }
}

void EnvEditor::RemoveSpec(EventSpec* ev) {
  if(owner->window == NULL) return;
  UnDisplaySpec(ev);
  int i;
  for(i = (int)spec_names->count()-1; i >= 0; i--) {
    SpecButton* eb = (SpecButton *) spec_names->component(i);
    if(eb->event == ev) {
      spec_names->remove(i); break;
    }
  }
  ivResource::flush();
  spnm_patch->reallocate();
  spnm_patch->redraw();
}

void EnvEditor::ToggleDisplay(bool update){
  owner->display_toggle = !owner->display_toggle;
  disp_ckbox->state()->set(ivTelltaleState::is_chosen,owner->display_toggle);
  if(update == true) {
    UpdateDisplay();
    tabMisc::NotifyEdits(owner);
  }
}

void EnvEditor::CBarAdjustNotify(){
  if(owner->auto_scale == true) ToggleAutoScale();
  owner->scale_range.min = cbar->min;
  owner->scale_range.max = cbar->max;
  UpdateDisplay();
}

void EnvEditor::UpdateMinMaxScale() {
  if(!owner->auto_scale) return;
  owner->scale_range.max = 1.0f;
  owner->scale_range.min = -1.0f;
  int first_one = true;
  int k;
  Event* ev;
  taLeafItr i;
  FOR_ITR_EL(Event, ev, owner->events_displayed., i) {
    Pattern* ps;
    taLeafItr psi;
    FOR_ITR_EL(Pattern, ps, ev->patterns., psi) {
      for(k=0;k<ps->value.size;k++){
	float val = ps->value.FastEl(k);
	if(first_one) {
	  owner->scale_range.max= val;
	  owner->scale_range.min= val;
	  first_one = false;
	}
	else 
	  owner->scale_range.UpdateRange(val);
      }
    }
  }
  if((owner->scale_range.min == 0.0f) && (owner->scale_range.max == 0.0f)) {
    owner->scale_range.min = -1.0f;
    owner->scale_range.max = 1.0f;
  }
  if(cbar != NULL)
    cbar->SetMinMax(owner->scale_range.min, owner->scale_range.max);
}

void EnvEditor::ToggleAutoScale(bool) {
  owner->auto_scale = !owner->auto_scale;
  as_ckbox->state()->set(ivTelltaleState::is_chosen,owner->auto_scale);
  UpdateMinMaxScale();
}

void EnvEditor::UpdatePosivPatch(float val) {
  ivLayoutKit* layout = ivLayoutKit::instance();
  ivWidgetKit* wkit = ivWidgetKit::instance();
  if(pospatch == NULL) {
     pospatch = new ivPatch(NULL);
     ivResource::ref(pospatch);
  }
  String valstr(val,"%5.3g");
  pospatch->body(layout->hbox(wkit->fancy_label("Value: "),wkit->raised_label(valstr)));
  pospatch->reallocate();
  pospatch->redraw();
  win->repair();
}


void EnvEditor::UpdatePosivPatch(int x, int y, int z){
  ivLayoutKit* layout = ivLayoutKit::instance();
  ivWidgetKit* wkit = ivWidgetKit::instance();
  if(pospatch == NULL) {
     pospatch = new ivPatch(NULL);
     ivResource::ref(pospatch);
  }

  String xstr(x,"%3d ");
  String ystr(y,"%3d ");
  String zstr(z,"%3d ");

  ivGlyph* xrep = layout->hbox
    (wkit->fancy_label("X: "),wkit->raised_label(xstr));
  ivGlyph* yrep = layout->hbox
    (wkit->fancy_label("Y: "),wkit->raised_label(ystr));
//    ivGlyph* zrep = layout->hbox
//      (wkit->fancy_label("Z: "),wkit->raised_label(zstr));

  pospatch->body
    (layout->hbox(xrep,yrep));
  pospatch->reallocate();
  pospatch->redraw();
  win->repair();
}

void EnvEditor::ClearPosivPatch() {
  ivWidgetKit* wkit = ivWidgetKit::instance();
  if(pospatch == NULL) {
     pospatch = new ivPatch(NULL);
     ivResource::ref(pospatch);
  }
  pospatch->body(wkit->fancy_label("X: --- Y: ---"));
  pospatch->reallocate();
  pospatch->redraw();
  win->repair();
}

void EnvEditor::SelectActButton(int toolnumber){
  if(viewer == NULL)
    return;
  viewer->cur_tool(toolnumber);
  if(actbutton[toolnumber] != NULL)
    ((NoUnSelectButton *) actbutton[toolnumber])->chooseme();
}

void EnvEditor::SetSelect(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::select);
}
  
void EnvEditor::SetMove(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::move);
}
  
void EnvEditor::SetReScale(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::scale);
}

void EnvEditor::SetReShape(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::stretch);
}

void EnvEditor::SetRotate(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::rotate);
}

void EnvEditor::SetAlter(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::alter);
}

void EnvEditor::FixEditorButtons(){
  if(sp_butpatch == NULL) return;
  trans_dupe_action = NO_TRANS_DUPE;
  spec_type_action = SELECT_EVENTS;
  new_action = NEW_SPEC;
  remove_action = NO_REMOVE;
  edit_action = NO_EDIT;
  new2_action = NO_NEW2;
  misc_action = NO_MISC;
  copy_action = NO_COPY;
  dupe_action = NO_DUPE;
  
  one_espec_sel = NULL;
  if(owner->specs_displayed.size == 1)
    one_espec_sel = (EventSpec*)owner->specs_displayed.FastEl(0);

  int eventcount = 0; int speccount =0; int patcount=0;
  int patspeccount = 0; int namecount=0; int valcount=0;
  TAPtr o;  int i;
  for(i=0; i<envg->selectgroup.size; i++) {
    o = envg->selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_Event)) { eventcount++; continue; }
    if(o->InheritsFrom(&TA_EventSpec)) { speccount++; continue; }
    if(o->InheritsFrom(&TA_Pattern)) { patcount++; continue; }
    if(o->InheritsFrom(&TA_PatternSpec)) { patspeccount++; continue; }
    if(o->InheritsFrom(&TA_ObjNameEdit)) { namecount++; continue; }
    if(o->InheritsFrom(&TA_ObjValueEdit)) { valcount++; continue; }
    if(o->InheritsFrom(&TA_PatSpecEdit)) { valcount++; continue; }
  }

  // for the event editor
  if((owner->events_displayed.size >= 1) && (owner->ev_gps_displayed.size == 1)) {
    trans_dupe_action = TRANS_EVENT;
  }
  else if(owner->ev_gps_displayed.size >= 1) {
    trans_dupe_action = DUPE_EVGP;
    spec_type_action = CHANGE_TYPE;
  }
  else if(owner->events_displayed.size >= 1) {
    trans_dupe_action = DUPE_EVENT;
    spec_type_action = SET_SPEC_TYPE;
  }

  // for the spec editor
  if(((one_espec_sel != NULL) || (speccount == 1))
	  && (patspeccount == 0) && (namecount == 0) && (valcount == 0)) {
    new_action = NEW_PATTERNS;
    edit_action = EDIT_SPECS;
    remove_action = REMOVE_SPECS;
    new2_action = NEW_CHILD;
    misc_action = LAYOUT_UPDT;
    copy_action = COPY_SPEC;
    dupe_action = DUPE_SPEC;
  }
  else if((speccount == 0) && (patspeccount == 0) && (namecount == 0) && (valcount == 0)) {
    if(env->event_specs.leaves == 0) new_action = HOT_NEW_SPEC;
    else new_action = NEW_SPEC;
  }
  else if((speccount == 0) && (patspeccount == 1) && (namecount == 0) && (valcount == 0)) {
    new_action = NEW_TO_LAYER;
    new2_action = EDIT_VALUE_NAMES;
    edit_action = EDIT_PATTERNS;
    remove_action = REMOVE_PATTERNS;
    copy_action = COPY_PAT;
    dupe_action = DUPE_PAT;
  }
  else if((speccount == 0) && (patspeccount == 0) && (namecount > 0) && (valcount == 0)) {
    edit_action = EDIT_NAMES;
  }
  else if((speccount == 0) && (patspeccount == 0) && (namecount == 0) && (valcount > 0)) {
    edit_action = EDIT_VALS;
  }
  else if((speccount > 1) && (patspeccount == 0) && (namecount == 0) && (valcount == 0)) {
    edit_action = EDIT_SPECS;
    remove_action = REMOVE_SPECS;
  }
  else if((speccount == 0) && (patspeccount > 1) && (namecount == 0) && (valcount == 0)) {
    edit_action = EDIT_PATTERNS;
    remove_action = REMOVE_PATTERNS;
  }
  else if((speccount > 0) || (patspeccount > 0) || (namecount > 0) || (valcount > 0)) {
    edit_action = EDIT_OBJS;
  }

  trans_dupe_but->flip_to(trans_dupe_action);
  spec_type_but->flip_to(spec_type_action);

  new_but->flip_to(new_action);
  remove_but->flip_to(remove_action);
  edit_but->flip_to(edit_action);
  new2_but->flip_to(new2_action);
  misc_but->flip_to(misc_action);
  copy_but->flip_to(copy_action);
  dupe_but->flip_to(dupe_action);

  sp_butpatch->redraw();
  if((win->is_mapped()) && (win->bound())) win->repair();
}

void EnvEditor::NewObjects() {
  taBase_List& selectgroup = envg->selectgroup;
  BaseSpec_MGroup esg;
  PatternSpec_Group pg;
  TAPtr o = NULL;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_EventSpec)) { esg.Link((EventSpec*)o); continue; }
    if(o->InheritsFrom(&TA_PatternSpec)) { pg.Link((PatternSpec*)o); continue; }
  }
  if(one_espec_sel != NULL)
    esg.LinkUnique(one_espec_sel);
  int oldsize;
  if(pg.size == 1) {
    PatternSpec* ps = (PatternSpec*)pg.FastEl(0);
    ps->CallFun("SetToLayer");
  }
  else if(esg.size == 1) {
    EventSpec* es = (EventSpec*)esg.FastEl(0);
    oldsize = es->patterns.leaves;
    gpivGroupNew::New(&(es->patterns));
    if(es->patterns.leaves > oldsize) {
      winbMisc::DelayedMenuUpdate(&(es->patterns));
      //      owner->SelectActButton(Tool::stretch);
      
    }
  }
  else {			// new specs
    oldsize = env->event_specs.leaves;
    gpivGroupNew::New(&(env->event_specs));
    if(env->event_specs.leaves > oldsize) {
      winbMisc::DelayedMenuUpdate(&(env->event_specs));
    }
  }
  selectgroup.Reset();
  InitDisplay();
}

void EnvEditor::RemoveSelections() {
  int okdel = taMisc::Choice("Are you sure you want to remove these objects?","Yes - Delete","No - Cancel");
  if(okdel == 1) return;
  envg->safe_damage_me(viewer->canvas());
  taBase_List& selectgroup = envg->selectgroup;
  bool removed_something = false;
  int i;
  for(i=selectgroup.size-1;i>=0;i--){
    removed_something = true;
    TAPtr o = selectgroup[i];
    selectgroup.Remove(i);
    taList_impl* og = (taList_impl*)(o)->GetOwner(&TA_taList_impl);
    if(og != NULL) {
      taivMisc::RecordScript(og->GetPath() + ".RemoveEl(" + o->GetPath() + ");\n");
      DMEM_GUI_RUN_IF {
	og->Remove(o);
      }
      winbMisc::DelayedMenuUpdate(og);
    }
  }
  if(!removed_something && (one_espec_sel != NULL)) {
    taList_impl* og = (taList_impl*)(one_espec_sel)->GetOwner(&TA_taList_impl);
    if(og != NULL) {
      taivMisc::RecordScript(og->GetPath() + ".RemoveEl(" + one_espec_sel->GetPath() + ");\n");
      DMEM_GUI_RUN_IF {
	og->Remove(one_espec_sel);
      }
      one_espec_sel = NULL;
      winbMisc::DelayedMenuUpdate(og);
    }
  }
  envg->safe_damage_me(viewer->canvas());
  InitDisplay();
}

void EnvEditor::EditSelections() {
  taBase_List& selectgroup = envg->selectgroup;
  if(selectgroup.size > 1){
    selectgroup.Edit();
  }
  else if (selectgroup.size == 1) {
    if(selectgroup[0]->InheritsFrom(TA_PatSpecEdit) || 
       selectgroup[0]->InheritsFrom(TA_ObjValueEdit) ||
       selectgroup[0]->InheritsFrom(TA_ObjNameEdit)) {
      selectgroup[0]->Edit(true);
      InitDisplay();
    }
    else {
      selectgroup[0]->Edit();
    }
  }
  else if(one_espec_sel != NULL) {
    one_espec_sel->Edit();
  }
}   

void EnvEditor::New2Objects() {
  taBase_List& selectgroup = envg->selectgroup;
  BaseSpec_MGroup esg;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_EventSpec)) { esg.Link((EventSpec*)o); continue; }
  }
  if(one_espec_sel != NULL)
    esg.LinkUnique(one_espec_sel);
  int oldsize;
  if(esg.size == 1) {
    EventSpec* es = (EventSpec*)esg.FastEl(0);
    oldsize = es->children.leaves;
    gpivGroupNew::New(&(es->children));
    if(es->children.leaves > oldsize) {
      winbMisc::DelayedMenuUpdate(&(es->children));
      //      owner->SelectActButton(Tool::stretch);
    }
  }
  selectgroup.Reset();
  InitDisplay();
}

void EnvEditor::EditValueNames() {
  taBase_List& selectgroup = envg->selectgroup;
  BaseSpec_MGroup esg;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_PatternSpec)) { esg.Link((PatternSpec*)o); continue; }
  }
  for(i=0;i<esg.size;i++) {
    PatternSpec* ps = (PatternSpec*)esg.FastEl(i);
    ps->value_names.Edit();
  }
  selectgroup.Reset();
}

void EnvEditor::MiscObjects() {
  taBase_List& selectgroup = envg->selectgroup;
  BaseSpec_MGroup esg;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_EventSpec)) { esg.Link((EventSpec*)o); continue; }
  }
  if(one_espec_sel != NULL)
    esg.LinkUnique(one_espec_sel);
  if(esg.size == 1) {
    EventSpec* es = (EventSpec*)esg.FastEl(0);
    int actn = taMisc::Choice("Layout/Updt action to perform: Linear Layout arranges patterns in a line, Updt Fm Layers updates patterns from corresponding layers","Linear Layout","Updt Fm Layers","Cancel");
    if(actn == 0) {
      DMEM_GUI_RUN_IF {
	es->LinearLayout(); 
      }
      taivMisc::RecordScript(es->GetPath() + "->LinearLayout();\n");
    }
    else if(actn == 1) {
      DMEM_GUI_RUN_IF {
	es->UpdateFromLayers();
      }
      taivMisc::RecordScript(es->GetPath() + "->UpdateFromLayers();\n");
    }
  }
  selectgroup.Reset();
  InitDisplay();
}

void EnvEditor::CopyObjects() {
  taBase_List& selectgroup = envg->selectgroup;
  BaseSpec_MGroup esg;
  PatternSpec_Group pg;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_EventSpec)) { esg.Link((EventSpec*)o); continue; }
    if(o->InheritsFrom(&TA_PatternSpec)) { pg.Link((PatternSpec*)o); continue; }
  }
  if(one_espec_sel != NULL)
    esg.LinkUnique(one_espec_sel);
  if(pg.size == 1) {
    PatternSpec* ps = (PatternSpec*)pg.FastEl(0);
    ps->CallFun("CopyFrom");
  }
  else if(esg.size == 1) {
    EventSpec* ps = (EventSpec*)esg.FastEl(0);
    ps->CallFun("CopyFrom");
  }
  selectgroup.Reset();
  InitDisplay();
}

void EnvEditor::DupeObjects() {
  taBase_List& selectgroup = envg->selectgroup;
  BaseSpec_MGroup esg;
  PatternSpec_Group pg;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_EventSpec)) { esg.Link((EventSpec*)o); continue; }
    if(o->InheritsFrom(&TA_PatternSpec)) { pg.Link((PatternSpec*)o); continue; }
  }
  if(one_espec_sel != NULL)
    esg.LinkUnique(one_espec_sel);
  if(pg.size == 1) {
    PatternSpec* ps = (PatternSpec*)pg.FastEl(0);
    taivMisc::RecordScript(ps->GetPath() + "->DuplicateMe();\n");
    DMEM_GUI_RUN_IF {
      ps->DuplicateMe();
    }
  }
  else if(esg.size == 1) {
    EventSpec* es = (EventSpec*)esg.FastEl(0);
    taivMisc::RecordScript(es->GetPath() + "->DuplicateMe();\n");
    DMEM_GUI_RUN_IF {
      es->DuplicateMe();
    }
  }
  selectgroup.Reset();
  InitDisplay();
}

void EnvEditor::TransDupeEvents() {
  if((owner->events_displayed.size >= 1) && (owner->ev_gps_displayed.size == 1)) {
    // transfer
    Event_MGroup* eg = (Event_MGroup*)owner->ev_gps_displayed.FastEl(0);
    int i;
    for(i=owner->events_displayed.size-1;i>=0;i--) {
      Event* ev = (Event*)(owner->events_displayed.FastEl(i));
      taivMisc::RecordScript(eg->GetPath() + "->Transfer(" + ev->GetPath() + ");\n");
      DMEM_GUI_RUN_IF {
	eg->Transfer(ev);
      }
    }
    winbMisc::DelayedMenuUpdate(owner);
    InitDisplay();
  }
  else if(owner->ev_gps_displayed.size >= 1) {
    owner->CallFun("DuplicateEventGps");
    winbMisc::DelayedMenuUpdate(owner);
  }
  else if(owner->events_displayed.size >= 1) {
    owner->CallFun("DuplicateEvents");
    winbMisc::DelayedMenuUpdate(owner);
  }
}

void EnvEditor::SpecTypeEvents() {
  // if groups are selected, only option is to change type
  if(owner->ev_gps_displayed.size >= 1) {
    owner->CallFun("ChangeEventGpType");
  }
  else if(owner->events_displayed.size >= 1) {
    int actn = taMisc::Choice("Action to perform on selected events: Set Event Spec or Change Event Type","Set Event Spec","Change Type", "Cancel");
    if(actn == 0) {
      owner->CallFun("SetEventSpec");
    }
    else if(actn == 1) {
      owner->CallFun("ChangeEventType");
    }
  }
  else {
    owner->CallFun("SelectEvents");
  }    
}
