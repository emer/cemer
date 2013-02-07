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

#include "T3Panel.h"
#include <iT3Panel>
#include <iT3ViewspaceWidget>
#include <T3PanelViewer>
#include <T3DataViewMain>
#include <iT3PanelViewer>
#include <T3ExaminerViewer>
#include <T3DataView>
#include <PanelViewer>
#include <iPanelViewer>
#include <iViewPanelSet>
#include <iMainWindowViewer>
#include <T3Node>
#include <taFiler>

#include <SigLinkSignal>
#include <taMisc>

#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/annex/HardCopy/SoVectorizePSAction.h>


void T3Panel::Initialize() {
//  link_type = &TA_T3SigLink;
  bg_color.setColorName(taMisc::t3d_bg_color);
  text_color.setColorName(taMisc::t3d_text_color);
  headlight_on = true;
  stereo_view = STEREO_NONE;
}

void T3Panel::Destroy() {
  Reset();
  CutLinks();
}

void T3Panel::InitLinks() {
  inherited::InitLinks();
  taBase::Own(root_view, this);
  taBase::Own(bg_color, this);
  taBase::Own(text_color, this);
  taBase::Own(saved_views, this);
}

void T3Panel::CutLinks() {
  bg_color.CutLinks();
  text_color.CutLinks();
  root_view.CutLinks();
  inherited::CutLinks();
}

void T3Panel::Copy_(const T3Panel& cp) {
//   root_view = cp.root_view;
  bg_color = cp.bg_color;
  text_color = cp.text_color;
  headlight_on = cp.headlight_on;
  stereo_view = cp.stereo_view;
  saved_views = cp.saved_views;
}

void T3Panel::CopyFromViewPanel(T3Panel* cp) {
  Copy_(*cp);
  SetAllSavedViews();
  GoToSavedView(0);
}

void T3Panel::AddView(T3DataView* view) {
  root_view.children.Add(view);
  if (dvwidget())
    view->OnWindowBind(widget());
}

// note: dispatchers for these _impl always check for the widget
void T3Panel::Clear_impl() {
  root_view.Clear_impl();
  widget()->Reset_impl();
  inherited::Clear_impl();
}

void T3Panel::Constr_impl(QWidget* gui_parent) {
  inherited::Constr_impl(gui_parent);
  if(widget() && widget()->t3vs) {
    root_view.host = widget()->t3vs;
    // note: set top view to the root, not us, because we don't pass doactions down
    widget()->t3vs->setTopView(&root_view);
  }
}

void T3Panel::Constr_post() {
  inherited::Constr_post();
  root_view.OnWindowBind(widget());
//   SetCameraPosOrient();
}

IViewerWidget* T3Panel::ConstrWidget_impl(QWidget* gui_parent) {
  iT3Panel* rval = new iT3Panel(this, gui_parent);
  // make the corresponding viewpanelset
  MainWindowViewer* mwv = GET_MY_OWNER(MainWindowViewer);
  int idx;
  PanelViewer* pv = (PanelViewer*)mwv->FindFrameByType(&TA_PanelViewer, idx);
  iPanelViewer* itv = pv->widget();
  taiSigLink* dl = (taiSigLink*)GetSigLink();
  iViewPanelSet* ivps = new iViewPanelSet(dl);
  rval->panel_set = ivps;
  itv->AddPanelNewTab(ivps);
  return rval;
}

void T3Panel::SigEmit(int sls, void* op1, void* op2) {
  inherited::SigEmit(sls, op1, op2);
  if (sls <= SLS_ITEM_UPDATED_ND) {
    T3PanelViewer* par = GET_MY_OWNER(T3PanelViewer);
    if (par) par->PanelChanged(this);
  }
}

T3DataView* T3Panel::FindRootViewOfData(taBase* data) {
  if (!data) return NULL;
  for (int i = 0; i < root_view.children.size; ++i) {
    if (T3DataView *dv = dynamic_cast<T3DataView*>(root_view.children[i])) {
      if (dv->data() == data) {
        return dv;
      }
    }
  }
  return NULL;
}

T3DataView* T3Panel::singleChild() const {
  if (root_view.children.size != 1) return NULL;
  T3DataView* rval = dynamic_cast<T3DataView*>(root_view.children[0]);
  return rval;
}


const iColor T3Panel::GetBgColor() const {
  iColor rval;
  if (singleMode()) {
    T3DataView* sng = singleChild();
    if (sng) {
      bool ok = false;
      rval = sng->bgColor(ok);
      if (ok) {
        if(bg_color.r != 0.8f || bg_color.g != 0.8f || bg_color.b != 0.8f)
          rval = bg_color;      // when panel is set to something different, it overrides regardless
        return rval;
      }
    }
  }
  rval = bg_color;
  return rval;
}

const iColor T3Panel::GetTextColor() const {
  return text_color;  // no actual logic required it seems
}

void T3Panel::Render_pre() {
  inherited::Render_pre();
  widget()->Render_pre();
  root_view.Render_pre();

  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(viewer) {
    if(viewer->cur_view_no < 0) {
      SetAllSavedViews();               // init from us
      viewer->gotoView(0);              // goto first saved view as default
    }
  }
}

void T3Panel::Render_impl() {
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(viewer) {
    QColor bg = (QColor)GetBgColor();
    if(viewer->quarter->backgroundColor() != bg)
      viewer->quarter->setBackgroundColor(bg);
    if(viewer->quarter->stereoMode() != (QuarterWidget::StereoMode)stereo_view)
      viewer->quarter->setStereoMode((QuarterWidget::StereoMode)stereo_view);
    if(viewer->quarter->headlightEnabled() != headlight_on)
      viewer->quarter->setHeadlightEnabled(headlight_on);
    viewer->syncViewerMode();   // keep it in sync
  }
  inherited::Render_impl();
  root_view.Render_impl();
  widget()->Render_impl();
}

void T3Panel::Render_post() {
  inherited::Render_post();
  root_view.Render_post();
  widget()->setSceneTop(root_view.node_so());
  widget()->Render_post();
  // on first opening, do a viewall to center all geometry in viewer
  if(saved_views[0]->pos == 0.0f && saved_views[0]->focal_dist == 0.0f) {
    ViewAll();
    SaveCurView(0);             // save to 0 view
  }
}

void T3Panel::Reset_impl() {
  root_view.Reset();
  inherited::Reset_impl();
}

void T3Panel::WindowClosing(CancelOp& cancel_op) {
  inherited::WindowClosing(cancel_op);
  if (cancel_op != CO_CANCEL) {
    root_view.DoActions(CLEAR_IMPL);
    root_view.host = NULL;
  }
}

void T3Panel::ViewAll() {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(viewer)
    viewer->viewAll();
}

void T3Panel::GetSavedView(int view_no) {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer || view_no < 0 || view_no >= viewer->n_views) return;
  T3SavedView* oursc = saved_views.SafeEl(view_no);
  T3SavedView* sc = viewer->saved_views.SafeEl(view_no);
  if(!sc || !oursc) return;
  oursc->CopyFrom(sc);  // initialize from them
  oursc->name = sc->name;       // names not usu copied
}

void T3Panel::SetSavedView(int view_no) {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer || view_no < 0 || view_no >= viewer->n_views) return;
  T3SavedView* oursc = saved_views.SafeEl(view_no);
  T3SavedView* sc = viewer->saved_views.SafeEl(view_no);
  if(!sc || !oursc) return;
  sc->CopyFrom(oursc);  // initialize from us
  sc->name = oursc->name;       // names not usu copied
  viewer->updtViewName(view_no); // trigger update of label
}

void T3Panel::SetAllSavedViews() {
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer) return;
  saved_views.SetSize(viewer->n_views); // make sure
  for(int i=0;i<saved_views.size;i++) {
    T3SavedView* sv = saved_views[i];
    if(sv->name.contains("T3SavedView")) {              // uninitialized
      sv->name = "View " + String(i);
    }
    SetSavedView(i);
  }
}

void T3Panel::SaveCurView(int view_no) {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(view_no < 0 || view_no >= viewer->n_views) return;
  viewer->saveView(view_no);  // this automatically calls signal to update us
}

void T3Panel::GoToSavedView(int view_no) {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(view_no < 0 || view_no >= viewer->n_views) return;
  SetSavedView(view_no);
  viewer->gotoView(view_no);
}

QPixmap T3Panel::GrabImage(bool& got_image) {
  got_image = false;
  if(!widget()) {
    return QPixmap();
  }
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(TestError(!viewer, "GrabImage", "viewer is NULL!")) return QPixmap();
  QImage img = viewer->grabImage();
  if(TestError(img.isNull(), "GrabImage", "got a null image from T3ExaminerViewer"))
    return QPixmap();
  got_image = true;
  return QPixmap::fromImage(img);     // more costly but works!
}

bool T3Panel::SaveImageAs(const String& fname, ImageFormat img_fmt) {
  if(!widget()) return false;
  if(img_fmt == EPS)
    return SaveImageEPS(fname);
  if(img_fmt == IV)
    return SaveImageIV(fname);

  return inherited::SaveImageAs(fname, img_fmt);
}

bool T3Panel::SaveImageEPS(const String& fname) {
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer) return false;

  String ext = String(".") + image_exts.SafeEl(EPS);
  taFiler* flr = GetSaveFiler(fname, ext);
  if(!flr->ostrm) {
    flr->Close();
    taRefN::unRefDone(flr);
    return false;
  }
  flr->Close();

  SoVectorizePSAction * ps = new SoVectorizePSAction;
  SoVectorOutput * out = ps->getOutput();

  if (!out->openFile(flr->FileName())) {
    return false; // unable to open output file
  }

  // to enable gouraud shading. 0.1 is a nice epsilon value
  // ps->setGouraudThreshold(0.1f);

  // clear to white background. Not really necessary if you
  // want a white background
  ps->setBackgroundColor(TRUE, SbColor(1.0f, 1.0f, 1.0f));

  // select LANDSCAPE or PORTRAIT orientation
  //  ps->setOrientation(SoVectorizeAction::LANDSCAPE);
  ps->setOrientation(SoVectorizeAction::PORTRAIT);

  // compute size based on actual image..  190.0f max width/height (= 7.5in)
  float wd = widget()->width();
  float ht = widget()->height();
  float pwd, pht;
  if(wd > ht) {
    pwd = 190.0f; pht = (ht/wd) * 190.0f;
  }
  else {
    pht = 190.0f; pwd = (wd/ht) * 190.0f;
  }

  // start creating a new page (based on actual size)
  ps->beginPage(SbVec2f(0.0f, 0.0f), SbVec2f(pwd, pht));

  // There are also enums for A0-A10. Example:
  //   ps->beginStandardPage(SoVectorizeAction::A4, 30.0f);

  // calibrate so that text, lines, points and images will have the
  // same size in the postscript file as on the monitor.
  ps->calibrate(viewer->getViewportRegion());

  // apply action on the viewer scenegraph. Remember to use
  // SoSceneManager's scene graph so that the camera is included.
  ps->apply(viewer->quarter->getSoRenderManager()->getSceneGraph());

  // this will create the postscript file
  ps->endPage();

  // close file
  out->closeFile();

  delete ps;
  taRefN::unRefDone(flr);
  return true;
}

bool T3Panel::SaveImageIV(const String& fname) {
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer) return false;

  String ext = String(".") + image_exts.SafeEl(IV);
  taFiler* flr = GetSaveFiler(fname, ext);
  if(!flr->ostrm) {
    flr->Close();
    taRefN::unRefDone(flr);
    return false;
  }
  flr->Close();

  SoOutput out;
  if(!out.openFile(flr->FileName())) return false;
  SoWriteAction wa(&out);

  wa.apply(root_view.node_so()); // just the data, not the whole camera
  //  wa.apply(viewer->quarter->getSceneGraph());
  out.closeFile();

  taRefN::unRefDone(flr);
  return true;
}

void T3Panel::SetImageSize(int width, int height) {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer || !viewer->quarter) return;
  // note: these may not be the same on all platforms!! works for me on my mac.. :)
  viewer->quarter->resize(width, height);
}

void T3Panel::SetTextBgColor(const String &new_text_color, const String &new_bg_color) {
  text_color.setColorName(new_text_color);
  bg_color.setColorName(new_bg_color);
}

void T3Panel::SetColorScheme(ColorScheme color_scheme) {
  switch (color_scheme) {
    case BLACK_ON_GREY:  SetTextBgColor("black", "grey");  break;
    case BLACK_ON_WHITE: SetTextBgColor("black", "white"); break;
    case WHITE_ON_BLACK: SetTextBgColor("white", "black"); break;
    case RED_ON_BLACK:   SetTextBgColor("red",   "black"); break;
    case GREEN_ON_BLACK: SetTextBgColor("green", "black"); break;
    case BLUE_ON_BLACK:  SetTextBgColor("blue",  "black"); break;
  }
  UpdateAfterEdit();
}

void T3Panel::GridLayout(int n_horiz, float horiz_sp, float vert_sp, bool save_views) {
  if (n_horiz < 1) n_horiz = 1;
  int idx = 0;
  for (int i = 0; i < root_view.children.size; ++i) {
    T3DataViewMain* dv = dynamic_cast<T3DataViewMain*>(root_view.children[i]);
    if (!dv) continue;
    int cur_x = idx % n_horiz;
    int cur_y = idx / n_horiz;
    float xp = (float)cur_x * (1.0 + horiz_sp);
    float yp = (float)cur_y * (1.0 + vert_sp);
    dv->main_xform.translate.x = xp;
    dv->main_xform.translate.y = yp;
    if (save_views && idx + 1 < saved_views.size) {
      // todo: this is not functional: need to do appropriate projection per quarter viewAll calculation involving bounding boxes etc...
      SetCameraPos(idx + 1, xp + .5F * (1.0F + horiz_sp), yp + .5F * (1.0F + vert_sp), 1.6F);
      SetCameraFocDist(idx + 1, 1.6F);
    }
    idx++;
  }
  UpdateAfterEdit();
}

