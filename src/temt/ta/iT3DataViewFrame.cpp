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

#include "iT3DataViewFrame.h"

#include <iT3ViewspaceWidget>
#include <T3ExaminerViewer>
#include <iT3DataViewer>
#include <T3DataViewFrame>

#include <taiChoiceDialog>


iT3DataViewFrame::iT3DataViewFrame(T3DataViewFrame* viewer_, QWidget* parent)
:inherited(parent), IDataViewWidget(viewer_)
{
  Init();
}

iT3DataViewFrame::~iT3DataViewFrame() {
  if (panel_set) {
    panel_set->ClosePanel();
  }
}

void iT3DataViewFrame::Constr_impl() {
  m_t3viewer->show();
}

void iT3DataViewFrame::Init() {
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

void iT3DataViewFrame::viewSaved(int view_no) {
  T3DataViewFrame* t3dvf = viewer();
  if(!t3dvf) return;            // shouldn't happen
  t3dvf->GetSavedView(view_no);
}

void iT3DataViewFrame::fileExportInventor() {
  static QFileDialog* fd = NULL;
  SoNode* scene = m_t3viewer->quarter->getSceneGraph();
  if (!scene) {
    taiChoiceDialog::ErrorDialog(this, "No scene exists yet.", "No scene", false);
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
  if (!fd->exec()) return;
  QString fileName;
  {QStringList files = fd->selectedFiles();
  QString selected;
  if (!files.isEmpty())
     fileName = files[0];}

  // check if exists, to warn user
  QFile f(fileName.toLatin1());
  if (f.exists()) {
    if (taiChoiceDialog::ChoiceDialog(this,
      "That file already exists, overwrite it?",
      "Confirm file overwrite",
      "&Ok" + taiChoiceDialog::delimiter + "&Cancel") != 0) return;
  }
  SoOutput out;
  if (!out.openFile(fileName.toLatin1())) {
    taiChoiceDialog::ErrorDialog(this, "Could not open file.", "File error", false);
    return;
  }
  SoWriteAction wa(&out);
  wa.apply(scene);

  out.closeFile();
}

void iT3DataViewFrame::NodeDeleting(T3Node* node) {
  if (t3vs->sel_so) {
    // deselect all the damn nodes because too complicated to try to figure out
    // how to deselect just one
    t3vs->sel_so->deselectAll();
  }
}

/*void iT3DataViewFrame::hideEvent(QHideEvent* ev) {
  inherited::hideEvent(ev);
  Showing(false);
}*/

void iT3DataViewFrame::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  Refresh();
/*// #ifdef TA_OS_MAC
  // this was needed on Mac as of 4.0.19 Qt 4.4.1+ to prevent
  // the occasional "white screen of death" that was occurring
//   taiMiscCore::ProcessEvents();
// #endif
  Showing(true);*/
}

void iT3DataViewFrame::Showing(bool showing) {
  if (panel_set) {
    // note: don't focus, because that results in hard-to-prevent side-effect
    // of spurious focusing when restoring windows, changing desktops, etc.
    panel_set->FrameShowing(showing, false);
  }
}

void iT3DataViewFrame::Render_pre() {
  //nothing
}

void iT3DataViewFrame::Render_impl() {
  //nothing
}

void iT3DataViewFrame::Render_post() {
//  nothing
}

void iT3DataViewFrame::Reset_impl() {
  setSceneTop(NULL);
}

void iT3DataViewFrame::Refresh_impl() {
  viewRefresh();
}

void iT3DataViewFrame::RegisterPanel(iViewPanelFrame* pan) {
    if (panel_set) {
      panel_set->AddSubPanel(pan);
    }
#ifdef DEBUG
    else {
      taMisc::Warning("Attempt to RegisterPanel failed because it doesn't exist!");
    }
#endif
}

T3DataViewRoot* iT3DataViewFrame::root() {
  return (m_viewer) ? &(((T3DataViewFrame*)m_viewer)->root_view) : NULL;
}

void iT3DataViewFrame::setSceneTop(SoNode* node) {
  t3vs->setSceneGraph(node);
}

void iT3DataViewFrame::T3DataViewClosing(T3DataView* node) {
}

iT3DataViewer* iT3DataViewFrame::viewerWidget() const {
//note: this fun not called much, usually only once on constr, so not cached
  QWidget* par = const_cast<iT3DataViewFrame*>(this); // ok to cast away constness
  while ((par = par->parentWidget())) {
    iT3DataViewer* rval = qobject_cast<iT3DataViewer*>(par);
    if (rval) return rval;
  }
  return NULL;
}

void iT3DataViewFrame::viewRefresh() {
  if (viewer())
    viewer()->Render();
}

