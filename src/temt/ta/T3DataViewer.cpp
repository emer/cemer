// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "T3DataViewer.h"
#include <T3DataViewFrame>
#include <T3DataViewFrame_List>
#include <iT3DataViewer>
#include <taProject>
#include <MainWindowViewer>
#include <iMainWindowViewer>
#include <taiMenu>
#include <iTabBarBase>

#include <taMisc>


iT3DataViewer* T3DataViewer::widget() {
  return (iT3DataViewer*)inherited::widget();
}

iTabBarBase* T3DataViewer::tabBar() {
  if(!widget()) return NULL; return widget()->tabBar();
}

T3DataViewFrame* T3DataViewer::GetBlankOrNewT3DataViewFrame(taBase* obj) {
  if (!obj) return NULL;
  T3DataViewFrame* fr = NULL;
  taProject* proj = (taProject*)obj->GetOwner(&TA_taProject);
  MainWindowViewer* vw = MainWindowViewer::GetDefaultProjectViewer(proj);
  if (!vw) return NULL; // shouldn't happen
  int idx;
  T3DataViewer* t3vw = (T3DataViewer*)vw->FindFrameByType(&TA_T3DataViewer, idx);
  if (!t3vw) return NULL; // shouldn't happen
  // make in default, if default is empty
  fr = t3vw->FirstEmptyT3DataViewFrame();
  if(!fr || fr->root_view.children.size == 0) {
    if(!fr)
      fr = t3vw->NewT3DataViewFrame();
    fr->SetName(obj->GetDisplayName()); // tis better to have one good name.. can always
    fr->UpdateAfterEdit();              // show name
  }
  return fr;
}

void T3DataViewer::Initialize() {
//  link_type = &TA_T3SigLink;
}

void T3DataViewer::Destroy() {
  Reset();
  CutLinks();
}

void T3DataViewer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(frames, this);
  // add a default frame, if none yet
  if(!taMisc::is_loading && frames.size == 0) {
    //T3DataViewFrame* fv = (T3DataViewFrame*)
    frames.New(1);
    //nuke fv->SetName("DefaultFrame");
  }
}

void T3DataViewer::CutLinks() {
  frames.CutLinks();
  inherited::CutLinks();
}

void T3DataViewer::Copy_(const T3DataViewer& cp) {
  frames = cp.frames;
}

IDataViewWidget* T3DataViewer::ConstrWidget_impl(QWidget* gui_parent) {
  return new iT3DataViewer(this, gui_parent);
}

void T3DataViewer::Constr_impl(QWidget* gui_parent) {
  inherited::Constr_impl(gui_parent); // prob just creates the widget
  if (!dvwidget()) return; // shouldn't happen

  ConstrFrames_impl();
}

void T3DataViewer::ConstrFrames_impl() {
  iT3DataViewer* idv = widget(); //cache
  T3DataViewFrame* fv = NULL;
  for (int i = 0; i < frames.size; ++i) {
    fv = frames.FastEl(i);
    if (!fv) continue; // shouldn't happen
    // note: don't parent the frame, since we use the api to add it
    ((DataViewer*)fv)->Constr_impl(NULL);
    idv->AddT3DataViewFrame(fv->widget());
  }
  MainWindowViewer* mwv = mainWindowViewer();
  if(mwv && mwv->widget()) {
    idv->installEventFilter(mwv->widget());
    idv->tabBar()->installEventFilter(mwv->widget());
  }
}

void T3DataViewer::SigEmit_Child(taBase* child, int dcr, void* op1, void* op2) {
  if (child == &frames) {
    // if reorder, then do a gui reorder
    //TODO:if new addition when mapped, then add gui
  }
}

void T3DataViewer::DoActionChildren_impl(DataViewAction act) {
// note: only ever called with one action
  if (act & CONSTR_MASK) {
    inherited::DoActionChildren_impl(act);
    frames.DoAction(act);
  } else { // DESTR_MASK
    frames.DoAction(act);
    inherited::DoActionChildren_impl(act);
  }
}


T3DataView* T3DataViewer::FindRootViewOfData(taBase* data) {
  if (!data) return NULL;
  for (int i = 0; i < frames.size; ++i) {
    T3DataViewFrame* f = frames.FastEl(i);
    T3DataView* dv = f->FindRootViewOfData(data);
    if (dv) return dv;
  }
  return NULL;
}

T3DataViewFrame* T3DataViewer::FirstEmptyT3DataViewFrame() {
  for (int i = 0; i < frames.size; ++i) {
    T3DataViewFrame* fv = frames.FastEl(i);
    if (fv->root_view.children.size == 0)
      return fv;
  }
  return NULL;
}

void T3DataViewer::FrameChanged(T3DataViewFrame* frame) {
  // just update all the tab names, in case that is what changed
  if (isMapped()) {
    widget()->UpdateTabNames();
  }
}

void T3DataViewer::GetWinState_impl() {
  inherited::GetWinState_impl();
//  iT3DataViewer* w = widget();
//  int view_frame_selected = w->tw->currentIndex();
  SetUserData("view_frame_selected", widget()->isVisible());
  SigEmitUpdated();
}

void T3DataViewer::SetWinState_impl() {
  inherited::SetWinState_impl();
  iT3DataViewer* w = widget();
  int view_frame_selected =
    GetUserDataDef("view_frame_selected", 0).toInt();
  if (view_frame_selected < w->tw->count())
    w->tw->setCurrentIndex(view_frame_selected);
}

T3DataViewFrame* T3DataViewer::NewT3DataViewFrame() {
  T3DataViewFrame* fv = (T3DataViewFrame*)frames.New(1);
  iT3DataViewer* idv = widget(); //cache
  if (idv) {
    // note: don't parent the frame, since we use the api to add it
    fv->Constr_impl(NULL);
    idv->AddT3DataViewFrame(fv->widget());
    fv->Constr_post(); // have to do this manually once mapped
  }
  return fv;
}


void T3DataViewer::Reset_impl() {
  frames.Reset();
  inherited::Reset_impl();
}


