// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "iT3Panel.h"

#include <iT3ViewspaceWidget>
#include <T3ExaminerViewer>
#include <iT3PanelViewer>
#include <T3Panel>
#include <iContextMenuButton>
#include <iViewPanelSet>
#include <iDialogChoice>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QFileDialog>

#ifdef TA_QT3D

#include <T3Node>

using namespace Qt3DCore;

#else // TA_QT3D
#include <Inventor/SoOutput.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoSelection.h>
#endif // TA_QT3D


iT3Panel::iT3Panel(T3Panel* viewer_, QWidget* parent)
:inherited(parent), IViewerWidget(viewer_)
{
  Init();
}

iT3Panel::~iT3Panel() {
  if (panel_set) {
    panel_set->ClosePanel();
  }
}

void iT3Panel::Constr_impl() {
  m_t3viewer->show();
}

void iT3Panel::Init() {
  QVBoxLayout* lay = new QVBoxLayout(this);
  lay->setSpacing(0);  lay->setMargin(0);
  //create the so viewer
  t3vs = new iT3ViewspaceWidget(this);
  lay->addWidget(t3vs);
  t3vs->setSelMode(iT3ViewspaceWidget::SM_MULTI); // default

  m_t3viewer = new T3ExaminerViewer(t3vs);
  t3vs->setT3viewer(m_t3viewer);
  connect(m_t3viewer, SIGNAL(viewSaved(int)), this, SLOT(viewSaved(int)) );
}

void iT3Panel::viewSaved(int view_no) {
  T3Panel* t3dvf = viewer();
  if(!t3dvf) return;            // shouldn't happen
  t3dvf->GetSavedView(view_no);
}

void iT3Panel::fileExportInventor() {
  static QFileDialog* fd = NULL;
#ifdef TA_QT3D

#else // TA_QT3D
  SoNode* scene = m_t3viewer->quarter->getSceneGraph();
  if (!scene) {
    iDialogChoice::ErrorDialog(this, "No scene exists yet.", "No scene", false);
    return;
  }

  if (!fd) {
    fd = new QFileDialog(this, "fd");
#if (QT_VERSION >= 0x050000)
    fd->setNameFilter( "Inventor files (*.iv)" );
#else
    fd->setFilter( "Inventor files (*.iv)" );
#endif
  }
  fd->setFileMode(QFileDialog::AnyFile);
  taMisc::in_eventproc++;       // this is an event proc!
  bool fdrv = fd->exec();
  taMisc::in_eventproc--;
  if (!fdrv) return;
  QString fileName;
  {QStringList files = fd->selectedFiles();
  QString selected;
  if (!files.isEmpty())
     fileName = files[0];}

  // check if exists, to warn user
  QFile f(fileName.toLatin1());
  if (f.exists()) {
    if (iDialogChoice::ChoiceDialog(this,
      "That file already exists, overwrite it?",
      "Confirm file overwrite",
      "&Ok" + iDialogChoice::delimiter + "&Cancel") != 0) return;
  }
  SoOutput out;
  if (!out.openFile(fileName.toLatin1())) {
    iDialogChoice::ErrorDialog(this, "Could not open file.", "File error", false);
    return;
  }
  SoWriteAction wa(&out);
  wa.apply(scene);

  out.closeFile();
#endif // TA_QT3D
}

void iT3Panel::NodeDeleting(T3Node* node) {
  if (t3vs->sel_so) {
    // deselect all the damn nodes because too complicated to try to figure out
    // how to deselect just one
#ifndef TA_QT3D
    t3vs->sel_so->deselectAll();
#endif
  }
}

void iT3Panel::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  Refresh();
}

void iT3Panel::Showing(bool showing) {
  if (panel_set) {
    // note: don't focus, because that results in hard-to-prevent side-effect
    // of spurious focusing when restoring windows, changing desktops, etc.
    panel_set->FrameShowing(showing, false);
  }
}

void iT3Panel::Render_pre() {
  //nothing
}

void iT3Panel::Render_impl() {
  //nothing
}

void iT3Panel::Render_post() {
}

void iT3Panel::Reset_impl() {
  setSceneTop(NULL);
}

void iT3Panel::Refresh_impl() {
  viewRefresh();
}

void iT3Panel::RegisterPanel(iViewPanel* pan) {
  if (panel_set) {
    panel_set->AddSubPanel(pan);
  }
#ifdef DEBUG
  else {
    taMisc::Warning("Attempt to RegisterPanel failed because it doesn't exist!");
  }
#endif
}

T3DataViewRoot* iT3Panel::root() {
  return (m_viewer) ? &(((T3Panel*)m_viewer)->root_view) : NULL;
}

#ifdef TA_QT3D
void iT3Panel::setSceneTop(QEntity* node) {
  t3vs->setSceneGraph(node);
}
#else
void iT3Panel::setSceneTop(SoNode* node) {
  t3vs->setSceneGraph(node);
}
#endif

void iT3Panel::T3DataViewClosing(T3DataView* node) {
}

iT3PanelViewer* iT3Panel::viewerWidget() const {
//note: this fun not called much, usually only once on constr, so not cached
  QWidget* par = const_cast<iT3Panel*>(this); // ok to cast away constness
  while ((par = par->parentWidget())) {
    iT3PanelViewer* rval = qobject_cast<iT3PanelViewer*>(par);
    if (rval) return rval;
  }
  return NULL;
}

void iT3Panel::viewRefresh() {
  if (viewer()) {
    viewer()->Render();
// #ifdef TA_QT3D
//     m_t3viewer->render->renderSynchronous(); // crashes
// #endif // TA_QT3D
  }
}

