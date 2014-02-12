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

#include "T3PanelViewer.h"
#include <T3Panel>
#include <T3Panel_List>
#include <iT3PanelViewer>
#include <taProject>
#include <MainWindowViewer>
#include <iMainWindowViewer>
#include <taiWidgetMenu>
#include <iTabBarBase>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(T3PanelViewer);


iT3PanelViewer* T3PanelViewer::widget() {
  return (iT3PanelViewer*)inherited::widget();
}

iTabBarBase* T3PanelViewer::tabBar() {
  if(!widget()) return NULL; return widget()->tabBar();
}

T3Panel* T3PanelViewer::GetBlankOrNewT3Panel(taBase* obj) {
  if (!obj) return NULL;
  T3Panel* fr = NULL;
  taProject* proj = (taProject*)obj->GetOwner(&TA_taProject);
  MainWindowViewer* vw = MainWindowViewer::GetDefaultProjectViewer(proj);
  if (!vw) return NULL; // shouldn't happen
  int idx;
  T3PanelViewer* t3vw = (T3PanelViewer*)vw->FindFrameByType(&TA_T3PanelViewer, idx);
  if (!t3vw) return NULL; // shouldn't happen
  // make in default, if default is empty
  fr = t3vw->FirstEmptyT3Panel();
  if(!fr || fr->root_view.children.size == 0) {
    if(!fr)
      fr = t3vw->NewT3Panel();
    fr->SetName(obj->GetDisplayName()); // tis better to have one good name.. can always
    fr->UpdateAfterEdit();              // show name
  }
  return fr;
}

void T3PanelViewer::Initialize() {
//  link_type = &TA_T3SigLink;
}

void T3PanelViewer::Destroy() {
  Reset();
  CutLinks();
}

void T3PanelViewer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(panels, this);
  // add a default panel, if none yet
  if(!taMisc::is_loading && panels.size == 0) {
    //T3Panel* fv = (T3Panel*)
    panels.New(1);
    //nuke fv->SetName("DefaultPanel");
  }
}

void T3PanelViewer::CutLinks() {
  panels.CutLinks();
  inherited::CutLinks();
}

void T3PanelViewer::Copy_(const T3PanelViewer& cp) {
  panels = cp.panels;
}

IViewerWidget* T3PanelViewer::ConstrWidget_impl(QWidget* gui_parent) {
  return new iT3PanelViewer(this, gui_parent);
}

void T3PanelViewer::Constr_impl(QWidget* gui_parent) {
  inherited::Constr_impl(gui_parent); // prob just creates the widget
  if (!dvwidget()) return; // shouldn't happen

  ConstrPanels_impl();
}

void T3PanelViewer::ConstrPanels_impl() {
  iT3PanelViewer* idv = widget(); //cache
  T3Panel* fv = NULL;
  for (int i = 0; i < panels.size; ++i) {
    fv = panels.FastEl(i);
    if (!fv) continue; // shouldn't happen
    // note: don't parent the panel, since we use the api to add it
    ((taViewer*)fv)->Constr_impl(NULL);
    idv->AddT3Panel(fv->widget());
  }
  MainWindowViewer* mwv = mainWindowViewer();
  if(mwv && mwv->widget()) {
    idv->installEventFilter(mwv->widget());
    idv->tabBar()->installEventFilter(mwv->widget());
  }
}

void T3PanelViewer::DoActionChildren_impl(DataViewAction act) {
  // note: only ever called with one action
  if (act & CONSTR_MASK) {
    inherited::DoActionChildren_impl(act);
    panels.DoAction(act);
  }
  else { // DESTR_MASK
    panels.DoAction(act);
    inherited::DoActionChildren_impl(act);
  }
}


T3DataView* T3PanelViewer::FindRootViewOfData(taBase* data) {
  if (!data) return NULL;
  for (int i = 0; i < panels.size; ++i) {
    T3Panel* f = panels.FastEl(i);
    T3DataView* dv = f->FindRootViewOfData(data);
    if (dv) return dv;
  }
  return NULL;
}

T3Panel* T3PanelViewer::FirstEmptyT3Panel() {
  for (int i = 0; i < panels.size; ++i) {
    T3Panel* fv = panels.FastEl(i);
    if (fv->root_view.children.size == 0)
      return fv;
  }
  return NULL;
}

void T3PanelViewer::PanelChanged(T3Panel* panel) {
  // just update all the tab names, in case that is what changed
  if (isMapped()) {
    widget()->UpdateTabNames();
  }
}

void T3PanelViewer::GetWinState_impl() {
  inherited::GetWinState_impl();
//  iT3PanelViewer* w = widget();
//  int view_panel_selected = w->tw->currentIndex();
  SetUserData("view_panel_selected", widget()->isVisible());
  SigEmitUpdated();
}

void T3PanelViewer::SetWinState_impl() {
  inherited::SetWinState_impl();
  iT3PanelViewer* w = widget();
  int view_panel_selected =
    GetUserDataDef("view_panel_selected", 0).toInt();
  if (view_panel_selected < w->tw->count())
    w->tw->setCurrentIndex(view_panel_selected);
}

T3Panel* T3PanelViewer::NewT3Panel() {
  T3Panel* fv = (T3Panel*)panels.New(1);
  iT3PanelViewer* idv = widget(); //cache
  if (idv) {
    // note: don't parent the panel, since we use the api to add it
    fv->Constr_impl(NULL);
    idv->AddT3Panel(fv->widget());
    fv->Constr_post(); // have to do this manually once mapped
  }
  return fv;
}


void T3PanelViewer::Reset_impl() {
  panels.Reset();
  inherited::Reset_impl();
}


