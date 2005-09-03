/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
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

#include "enviro_qtso.h"

#include "ta_qtgroup.h"
#include "pdplog_qtso.h"

//////////////////////
//   EnviroView     //
//////////////////////

void EnviroView::Initialize() {
  event_layout = HORIZONTAL;
  auto_scale = false;
  val_disp_mode = COLOR;
  val_text = NAMES;
  no_border = false;
  colorspec = NULL;
//TODO  editor = NULL;
  scale_range.min = -1.0f;
  scale_range.max = 1.0f;
  event_header = "Event::name";
  pattern_header = "PatternSpec::layer_name";
  pattern_blocks = "Pattern::values";
}

void EnviroView::InitLinks(){
  taBase::Own(events_displayed, this);
  taBase::Own(ev_gps_displayed, this);
  taBase::Own(specs_displayed, this);
  taBase::Own(scale_range, this);
  taBase::Own(view_font,this);
  taBase::Own(value_font,this);
  PDPView::InitLinks();
}

void EnviroView::CutLinks(){
  events_displayed.CutLinks();
  ev_gps_displayed.CutLinks();
  scale_range.CutLinks();
  view_font.CutLinks();
  value_font.CutLinks();
  taBase::DelPointer((TAPtr*)&colorspec);
//TODO  if(editor != NULL) { delete editor; editor = NULL; }
  PDPView::CutLinks();
}

void EnviroView::Destroy(){
  CutLinks();
}

void EnviroView::Copy_(const EnviroView& cp) {
  event_layout = cp.event_layout;
  display_toggle = cp.display_toggle;
  val_disp_mode = cp.val_disp_mode;
  val_text = cp.val_text;
  no_border = cp.no_border;
  view_font = cp.view_font;
  value_font = cp.value_font;
  scale_range = cp.scale_range;
  taBase::SetPointer((TAPtr*)&colorspec, cp.colorspec);
}

void EnviroView::UpdateAfterEdit(){
  PDPView::UpdateAfterEdit();
  if(!taMisc::gui_active)    return;
/*TODO  if(editor == NULL) return; // loading
  if((colorspec != NULL) && (editor->scale->spec != colorspec)) {
    taBase::SetPointer((TAPtr*)&(editor->scale->spec), colorspec);
    editor->scale->MapColors();
    editor->cbar->SetColorScale(editor->scale);
    editor->envg->background(editor->scale->Get_Background());
  }
  InitDisplay(); */
}

Network* EnviroView::GetDefaultNetwork() {
  return pdpMisc::GetDefNetwork(GET_MY_OWNER(Project));
}

/*nuke: void EnviroView::UpdateMenus_impl() {
  PDPView::UpdateMenus_impl();
  InitDisplay();
}*/

void EnviroView::SetEventSpec(EventSpec* es){
  if(es == NULL) return;
  int i;
  for(i=0;i<events_displayed.size;i++){
    Event* ev = (Event *) events_displayed[i];
    ev->spec.SetSpec(es);
    es->UpdateEvent(ev);
  }
  InitDisplay();
}

void EnviroView::ChangeEventType(TypeDef* new_type) {
  if(new_type == NULL) return;
  int i;
  for(i=events_displayed.size-1;i>=0;i--){
    Event* ev = (Event *) events_displayed[i];
    ev->ChangeMyType(new_type);
  }
  taMisc::DelayedMenuUpdate(this);
}

void EnviroView::ChangeEventGpType(TypeDef* new_type) {
  if(new_type == NULL) return;
  int i;
  for(i=ev_gps_displayed.size-1;i>=0;i--){
    Event_MGroup* ev = (Event_MGroup*) ev_gps_displayed[i];
    ev->ChangeMyType(new_type);
  }
  taMisc::DelayedMenuUpdate(this);
}

void EnviroView::DuplicateEvents() {
  int i;
  for(i=0;i<events_displayed.size;i++){
    Event* ev = (Event *) events_displayed[i];
    ev->DuplicateMe();
  }
  taMisc::DelayedMenuUpdate(this);
}

void EnviroView::DuplicateEventGps() {
  int i;
  for(i=0;i<ev_gps_displayed.size;i++){
    Event_MGroup* ev = (Event_MGroup*) ev_gps_displayed[i];
    ev->DuplicateMe();
  }
  taMisc::DelayedMenuUpdate(this);
}

void EnviroView::UpdateDisplay(TAPtr updtr) {
/*TODO  if(!IsMapped() || !display_toggle || editor == NULL) return;
  if(updtr == NULL) { editor->UpdateDisplay(); return;}
  Event* ev = NULL;
  if(updtr->InheritsFrom(&TA_TrialProcess)){ // fast check for common case
    ev = ((TrialProcess *) updtr)->cur_event;
  }
  else {
    MemberDef* mbr;
    if(((mbr = updtr->FindMember(&TA_Event)) != NULL) && // an event member
       (mbr->type->ptr == 1)) { // that is a pointer to an event
      ev = (Event *) mbr->GetOff(updtr);
    }
  }
  if (ev==NULL) return;
  editor->UnDisplayAllEvents();
  editor->DisplayEvent(ev);
  if(anim.capture) CaptureAnimImg(); */
}

void EnviroView::InitDisplay() {
  if(!taMisc::gui_active)
    return;
/*TODO  if(editor != NULL)
    editor->InitDisplay(); */
}

void EnviroView::SetColorSpec(ColorScaleSpec* colors) {
  taBase::SetPointer((TAPtr*)&colorspec, colors);
  UpdateAfterEdit();
}

void EnviroView::SetLayout(EventLayout layout) {
  event_layout = layout;
  UpdateAfterEdit();
}

void EnviroView::SetBorder(bool border) {
  no_border = !border;
  UpdateAfterEdit();
}

void EnviroView::SetViewFontSize(int point_size) {
  view_font.SetFontSize(point_size);
  UpdateAfterEdit();
}

void EnviroView::SetValueFontSize(int point_size) {
  value_font.SetFontSize(point_size);
  UpdateAfterEdit();
}

void EnviroView::SelectEvents(int start, int n_events) {
/*TODO  Environment* env = (Environment*)mgr;
  if(env == NULL)  return;
  if(n_events == -1)
    n_events = env->events.leaves;
  int i;
  for(i=0; i<n_events; i++) {
    Event* ev = (Event*)env->events.Leaf(start + i);
    if(ev == NULL) break;
    events_displayed.LinkUnique(ev);
  }
  InitDisplay();*/
}

void EnviroView::DeselectEvents(int start, int n_events) {
/*TODO  Environment* env = (Environment*)mgr;
  if(env == NULL)  return;
  if(n_events == -1)
    n_events = env->events.leaves;
  int i;
  for(i=0; i<n_events; i++) {
    Event* ev = (Event*)env->events.Leaf(start + i);
    if(ev == NULL) break;
    events_displayed.Remove(ev);
  }
  InitDisplay(); */
}

void EnviroView::SelectEvent(Event* ev) {
  events_displayed.LinkUnique(ev);
  InitDisplay();
}

void EnviroView::DeselectEvent(Event* ev) {
  events_displayed.Remove(ev);
  InitDisplay();
}

void EnviroView::SelectEventGp(Event_MGroup* eg) {
  ev_gps_displayed.LinkUnique(eg);
  InitDisplay();
}

void EnviroView::DeselectEventGp(Event_MGroup* eg) {
  ev_gps_displayed.Remove(eg);
  Event* ev;
  taLeafItr ei;
  FOR_ITR_EL(Event, ev, eg->, ei) {
    events_displayed.Remove(ev);
  }
  InitDisplay();
}

void EnviroView::SelectGpEvents(Event_MGroup* eg) {
  ev_gps_displayed.Remove(eg);
  Event* ev;
  taLeafItr ei;
  FOR_ITR_EL(Event, ev, eg->, ei) {
    events_displayed.Link(ev);
  }
  InitDisplay();
}

void EnviroView::DeselectAllEvents() {
  events_displayed.Reset();
  ev_gps_displayed.Reset();
  InitDisplay();
}

void EnviroView::SetPatLabel(const char* pat_lbl) {
  pattern_header = pat_lbl;
//TODO  editor->SetPatHeadStr(pat_lbl);
  InitDisplay();
}

void EnviroView::SetEventLabel(const char* evt_lbl) {
  event_header = evt_lbl;
//TODO  editor->SetEventHeadStr(evt_lbl);
  InitDisplay();
}


void EnviroView::GetBodyRep() {
  if(!taMisc::gui_active) return;
/*TODO  if(body != NULL) return;
  Environment* env = (Environment*)mgr;
  editor = new EnvEditor(env, this, window);
  editor->Init();
  body = editor->GetLook(); */
}

QWidget* EnviroView::GetPrintData(){
return NULL;
//TODO  return editor->print_patch;
}

