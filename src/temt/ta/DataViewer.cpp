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

#include "DataViewer.h"
#include <IDataViewWidget>
#include <taFiler>
#include <taMisc>

#include <QPixmap>
#include <QWidget>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <MainWindowViewer>


String_Array DataViewer::image_exts;

bool DataViewer::InitImageExts() {
  if(image_exts.size == IV+1) return false;
  image_exts.Reset();
  image_exts.Add("eps");
  image_exts.Add("jpg");
  image_exts.Add("png");
  image_exts.Add("ppm");
  image_exts.Add("iv");
  return true;
}

void DataViewer::GetFileProps(TypeDef* td, String& fltr, bool& cmprs) {
  int opt = td->opts.FindContains("EXT_");
  if (opt >= 0) {
    fltr = td->opts.FastEl(opt).after("EXT_");
    fltr = "*." + fltr + "*";
  }
  else {
    fltr = "*";
  }
  cmprs = td->HasOption("COMPRESS");
}

void DataViewer::Initialize() {
  m_dvwidget = NULL;
  visible = true; // default for most types
}

void DataViewer::Destroy() {
  CutLinks();
}

void DataViewer::InitLinks() {
  inherited::InitLinks();
  InitImageExts();
}

void DataViewer::CutLinks() {
  if (m_dvwidget) {
    m_dvwidget->Close(); // destructive close
    m_dvwidget = NULL; //
  }
  inherited::CutLinks();
}

void DataViewer::Copy_(const DataViewer& cp) {
  visible = cp.visible; //note: not the same as mapped
}

void DataViewer::CloseWindow_impl() { // only called if mapped
  m_dvwidget->Close(); // typically is destructive close, and calls us back + resets instance
  m_dvwidget = NULL; // for safety
  //note: don't call inherited, because we don't want to call children
  // gui destroy at this level should always destroy all nested gui items
}

void DataViewer::Constr(QWidget* gui_parent) {
  Constr_impl(gui_parent);
  Constr_post();
}

void DataViewer::Constr_impl(QWidget* gui_parent) {
  m_dvwidget = ConstrWidget_impl(gui_parent);
  m_dvwidget->Constr(); // virtual guy
}

void DataViewer::Constr_post() {
  m_dvwidget->Constr_post();
  // call inherited last, for a top-down type of call sequence
  inherited::Constr_post(); // does children
  // now, finally restore the state
  SetWinState();
}

void DataViewer::Dump_Save_pre() {
  inherited::Dump_Save_pre();
//no: we only save explicitly in v4
//  GetWinState(); // prior to saving
}

bool DataViewer::GetWinState() {
  if (!isMapped()) return false;
  GetWinState_impl();
  return true;
}

void DataViewer::Hide() {
  if (!isMapped()) return;
  Hide_impl();
  visible = false;
}

void DataViewer::Hide_impl() {
  widget()->hide();
}

bool DataViewer::isVisible() const {
  return visible;
}

void DataViewer::ResolveChanges(CancelOp& cancel_op) {
  if (!isMapped()) return;
  ResolveChanges_impl(cancel_op);
}

void DataViewer::ResolveChanges_impl(CancelOp& cancel_op) {
  dvwidget()->ResolveChanges(cancel_op);
}

void DataViewer::Show() {
  if (!isMapped()) return;
  Show_impl();
  visible = true;
}

void DataViewer::Show_impl() {
  widget()->show();
  //  widget()->raise();
}

void DataViewer::setVisible(bool value, bool update_view) {
  if (visible == value) return;
  visible = value;
  if (update_view) {
    if (visible) Show(); else Hide();
  }
}

bool DataViewer::SetWinState() {
  if (!isMapped()) return false;
  SetWinState_impl();
  return true;
}


QPixmap DataViewer::GrabImage(bool& got_image) {
  if(!widget()) {
    got_image = false;
    return QPixmap();
  }
  got_image = true;
  return QPixmap::grabWidget(widget());
  //  return QPixmap::grabWindow(widget()->winId());
}

bool DataViewer::SaveImageAs(const String& fname, ImageFormat img_fmt) {
  if(TestError(img_fmt == EPS || img_fmt == IV, "SaveImageAs",
               "EPS (encapsulated postscript) or IV (Open Inventor) not supported for this type of view"))
    return false;
  bool rval = false;
  String ext = image_exts.SafeEl(img_fmt);
  String fext = String(".") + ext; // filer needs .
  taFiler* flr = GetSaveFiler(fname, fext);
  if(flr->ostrm) {
    QPixmap pix = GrabImage(rval);
    if(rval) {
      flr->Close();
      int quality = taMisc::jpeg_quality;
      // for png, quality should be 0, which uses maximum deflate compression
      if (img_fmt == PNG) {
        quality = 0;
      }
      pix.save(flr->FileName(), ext, quality);
      String msg;
      msg << "Saving image of size: " << pix.width() << " x " << pix.height() << " depth: " << pix.depth() << " to: " << flr->FileName();
      taMisc::Info(msg);
    }
  }
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

bool DataViewer::PrintImage() {
  bool rval = false;
  QPixmap pix = GrabImage(rval);
  if(rval) {
    QPrinter pr;
    QPrintDialog pd(&pr, widget());
    if(pd.exec() == QDialog::Accepted) {
      QPainter p(&pr);
      p.drawPixmap(0, 0, pix);
    }
  }
  return rval;
}

bool DataViewer::isMapped() const {
//NOTE: do NOT put gui_active into this!!!
  return (m_dvwidget);
}

MainWindowViewer* DataViewer::parent() const {
  if (!m_parent) {
    m_parent = (taDataView*)GetOwner(parentType()); // NULL if no owner, or no compatible type
  }
  return dynamic_cast<MainWindowViewer*>(m_parent); // dyn for safety
}

iMainWindowViewer* DataViewer::viewerWindow() const {
  if (m_dvwidget) return m_dvwidget->viewerWindow();
  else return NULL;
}


void DataViewer::WidgetDeleting() {
  WidgetDeleting_impl();
  if (deleteOnWinClose()) {
    // do a deferred delete
    if(!isDestroying())
      CloseLater();
  }
}

void DataViewer::WidgetDeleting_impl() {
  m_dvwidget = NULL;
}

QWidget* DataViewer::widget() {
  return (m_dvwidget) ? m_dvwidget->widget() : NULL;
}

