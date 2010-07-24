// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "ta_imgproc.h"
#include "ta_project.h"

#include <QMatrix>
#include <QImage>
#include <QPainter>
#include <QPainterPath>

///////////////////////////////////////////
//	taImage

void taImage::Initialize() {
}

void taImage::Copy_(const taImage& cp) {
  q_img = cp.q_img;
}

bool taImage::LoadImage(const String& fname) {
  String fnm = fname;
  if (fname.empty()) {
    taFiler* flr = GetLoadFiler(fname);
    fnm = flr->FileName();
    flr->Close();
    taRefN::unRefDone(flr);
  }
  if(name.empty()) {
    name = fnm;
    if(name.contains(".")) {
      name = name.before(".",-1);
    }
    name = taMisc::StringCVar(name); // make names C legal names -- just much safer
  }
  QString fn = (const char*)fnm;
  if(!q_img.load(fn)) {
    taMisc::Error("LoadImage: could not read image file:", fnm);
    return false;
  }
  return true;
}

bool taImage::SaveImage(const String& fname) {
  String fnm = fname;
  if (fname.empty()) {
    taFiler* flr = GetLoadFiler(fname);
    fnm = flr->FileName();
    flr->Close();
    taRefN::unRefDone(flr);
  }
  if(name.empty()) {
    name = fnm;
    if(name.contains(".")) {
      name = name.before(".",-1);
    }
    name = taMisc::StringCVar(name); // make names C legal names -- just much safer
  }
  QString fn = (const char*)fnm;
  if(!q_img.save(fn)) {
    int ht, wd;
    GetImageSize(ht, wd);
    taMisc::Error("SaveImage: could not save image file:", fnm);
    return false;
  }
  return true;
}

float taImage::GetPixelGrey_float(int x, int y) {
  if(q_img.isNull()) {
    return -1.0f;
  }
  QRgb pix = q_img.pixel(x, y);
  float gval = qGray(pix) / 255.0f;
  return gval;
}

bool taImage::GetPixelRGB_float(int x, int y, float& r, float& g, float& b) {
  if(q_img.isNull()) {
    return false;
  }
  QRgb pix = q_img.pixel(x, y);
  r = qRed(pix) / 255.0f;
  g = qGreen(pix) / 255.0f;
  b = qBlue(pix) / 255.0f;
  return true;
}

bool taImage::ImageToMatrix_grey(float_Matrix& img_data) {
  if(q_img.isNull()) {
    return false;
  }
  int ht = q_img.height();
  int wd = q_img.width();

  img_data.SetGeom(2, wd, ht);

  for(int y=0; y<ht; y++) {
    for(int x=0; x< wd; x++) {
      QRgb pix = q_img.pixel(x, y);
      float gval = qGray(pix) / 255.0f;
      img_data.Set(gval, x, ht-1 - y);
    }
  }
  return true;
}

bool taImage::ImageToMatrix_rgb(float_Matrix& rgb_data) {
  if(q_img.isNull()) {
    return false;
  }
  int ht = q_img.height();
  int wd = q_img.width();

  rgb_data.SetGeom(3, wd, ht, 3); // r,g,b = 3rd dim

  for(int y=0; y<ht; y++) {
    for(int x=0; x< wd; x++) {
      QRgb pix = q_img.pixel(x, y);
      float rval = qRed(pix) / 255.0f;
      float gval = qGreen(pix) / 255.0f;
      float bval = qBlue(pix) / 255.0f;
      rgb_data.Set(rval, x, ht-1 -y, 0);
      rgb_data.Set(gval, x, ht-1 -y, 1);
      rgb_data.Set(bval, x, ht-1 -y, 2);
    }
  }
  return true;
}

bool taImage::ImageFromMatrix_grey(const float_Matrix& img_data) {
  if(TestError((img_data.dims() < 2), "IMageFromMatrix_grey", "img data does not have at least 2 dimensions"))
    return false;
  int wd = img_data.dim(0);
  int ht = img_data.dim(1);

  q_img = QImage(wd, ht, QImage::Format_RGB32);
  
  for(int y=0; y<ht; y++) {
    for(int x=0; x< wd; x++) {
      float gval = img_data.FastEl(x, y) * 255.0f;
      QRgb pix = qRgb(gval, gval, gval);
      q_img.setPixel(x, ht-1-y, pix);
    }
  }
  return true;
}

bool taImage::ImageFromMatrix_rgb(const float_Matrix& rgb_data) {
  if(TestError((rgb_data.dims() < 3), "IMageFromMatrix_rgb", "img data does not have at least 3 dimensions"))
    return false;
  int wd = rgb_data.dim(0);
  int ht = rgb_data.dim(1);

  q_img = QImage(wd, ht, QImage::Format_RGB32);

  for(int y=0; y<ht; y++) {
    for(int x=0; x< wd; x++) {
      float rval = rgb_data.FastEl(x, y, 0) * 255.0f;
      float gval = rgb_data.FastEl(x, y, 1) * 255.0f;
      float bval = rgb_data.FastEl(x, y, 2) * 255.0f;
      QRgb pix = qRgb(rval, gval, bval);
      q_img.setPixel(x, ht-1-y, pix);
    }
  }
  return true;
}

bool taImage::ImageToDataCell(DataTable* dt, Variant col, int row) {
  if(TestError(q_img.isNull(), "ImageToDataCell", "Null image")) return false;
  if(TestError(!dt, "ImageToDataCell", "Null data table")) return false;

  int ht = q_img.height();
  int wd = q_img.width();

  DataCol* da = dt->GetColData(col);
  if(!da) return false;
  bool isfloat = da->isFloat();

  if(TestError(da->cell_dims() < 2, "ImageToDataCell", "cell dimensions less than 2 -- must have at least 2 dimensions for greyscale, 3 for color")) return false;

  wd = MIN(wd, da->GetCellGeom(0));
  ht = MIN(ht, da->GetCellGeom(1));

  taMatrixPtr mat; mat = da->GetValAsMatrix(row);
  if(!mat) return false;

  bool rval = true;

  DataUpdate(true);
  if(mat->dims() == 2) {
    for(int y=0; y<ht; y++) {
      for(int x=0; x< wd; x++) {
	QRgb pix = q_img.pixel(x, y);
	if(isfloat) {
	  float gval = qGray(pix) / 255.0f;
	  mat->SetFmVar(gval, x, ht-1 - y);
	}
	else {
	  mat->SetFmVar(qGray(pix), x, ht-1 - y);
	}
      }
    }
  }
  else {			// must be > 2
    for(int y=0; y<ht; y++) {
      for(int x=0; x< wd; x++) {
	QRgb pix = q_img.pixel(x, y);
	if(isfloat) {
	  float rval = qRed(pix) / 255.0f;
	  float gval = qGreen(pix) / 255.0f;
	  float bval = qBlue(pix) / 255.0f;
	  mat->SetFmVar(rval, x, ht-1 - y, 0);
	  mat->SetFmVar(gval, x, ht-1 - y, 1);
	  mat->SetFmVar(bval, x, ht-1 - y, 2);
	}
	else {
	  mat->SetFmVar(qRed(pix), x, ht-1 - y, 0);
	  mat->SetFmVar(qGreen(pix), x, ht-1 - y, 1);
	  mat->SetFmVar(qBlue(pix), x, ht-1 - y, 2);
	}
      }
    }
  }

  DataUpdate(false); 

  return rval;
}

bool taImage::ImageFromDataCell(DataTable* dt, Variant col, int row) {
  if(TestError(!dt, "ImageToDataCell", "Null data table")) return false;

  int ht = q_img.height();
  int wd = q_img.width();

  DataCol* da = dt->GetColData(col);
  if(!da) return false;
  bool isfloat = da->isFloat();

  if(TestError(da->cell_dims() < 2, "ImageFromDataCell", "cell dimensions less than 2 -- must have at least 2 dimensions for greyscale, 3 for color")) return false;

  wd = da->GetCellGeom(0);
  ht = da->GetCellGeom(1);

  q_img = QImage(wd, ht, QImage::Format_RGB32);
 
  taMatrixPtr mat; mat = da->GetValAsMatrix(row);
  if(!mat) return false;

  bool rval = true;

  DataUpdate(true);
  if(mat->dims() == 2) {
    for(int y=0; y<ht; y++) {
      for(int x=0; x< wd; x++) {
	if(isfloat) {
	  float gval = mat->FastElAsFloat(x, y) * 255.0f;
	  QRgb pix = qRgb(gval, gval, gval);
	  q_img.setPixel(x, ht-1-y, pix);
	}
	else {
	  float gval = mat->FastElAsFloat(x, y);
	  QRgb pix = qRgb(gval, gval, gval);
	  q_img.setPixel(x, ht-1-y, pix);
	}
      }
    }
  }
  else {			// must be > 2
    for(int y=0; y<ht; y++) {
      for(int x=0; x< wd; x++) {
	QRgb pix = q_img.pixel(x, y);
	if(isfloat) {
	  float rval = mat->FastElAsFloat(x, y, 0) * 255.0f;
	  float gval = mat->FastElAsFloat(x, y, 1) * 255.0f;
	  float bval = mat->FastElAsFloat(x, y, 2) * 255.0f;
	  QRgb pix = qRgb(rval, gval, bval);
	  q_img.setPixel(x, ht-1-y, pix);
	}
	else {			// assume int
	  float rval = mat->FastElAsFloat(x, y, 0);
	  float gval = mat->FastElAsFloat(x, y, 1);
	  float bval = mat->FastElAsFloat(x, y, 2);
	  QRgb pix = qRgb(rval, gval, bval);
	  q_img.setPixel(x, ht-1-y, pix);
	}
      }
    }
  }

  DataUpdate(false); 

  return rval;
}

bool taImage::ImageToDataCellName(DataTable* dt, const String& col_nm, int row) {
  return ImageToDataCell(dt, col_nm, row);
}

bool taImage::ConfigDataColName(DataTable* dt, const String& col_nm, ValType val_type,
				bool rgb) {
  if(q_img.isNull() || !dt) {
    return false;
  }
  int ht = q_img.height();
  int wd = q_img.width();

  if(rgb)
    dt->FindMakeColMatrix(col_nm, val_type, 3, wd, ht, 3);
  else
    dt->FindMakeColMatrix(col_nm, val_type, 2, wd, ht);

  dt->SetColUserData("IMAGE", true, col_nm);

  return true;
}

bool taImage::ScaleImage(float sx, float sy, bool smooth) {
  if(q_img.isNull()) {
    return false;
  }
  int wd = q_img.width();
  int ht = q_img.height();
  int nw_wd = (int)(sx * (float)wd);
  int nw_ht = (int)(sy * (float)ht);
  if(smooth)
    q_img = q_img.scaled(nw_wd, nw_ht, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  else
    q_img = q_img.scaled(nw_wd, nw_ht); // default is fast
  return true;
}

bool taImage::RotateImage(float norm_deg, bool smooth) {
  if(q_img.isNull()) {
    return false;
  }
  float deg = norm_deg * 360.0f;
  QMatrix mat;
  mat.rotate(deg);
  if(smooth)
    q_img = q_img.transformed(mat, Qt::SmoothTransformation);
  else
    q_img = q_img.transformed(mat); // default is fast
  return true;
}

bool taImage::TranslateImage(float move_x, float move_y, bool smooth) {
  if(q_img.isNull()) {
    return false;
  }
  int wd = q_img.width();
  int ht = q_img.height();
  int nw_move_x= (int)(move_x * (float)wd);
  int nw_move_y = (int)(move_y * (float)ht);
  QMatrix mat;
  mat.translate(nw_move_x, nw_move_y);
  if(smooth)
    q_img = q_img.transformed(mat, Qt::SmoothTransformation);
  else
    q_img = q_img.transformed(mat); // default is fast
  return true;
}

bool taImage::GetImageSize(int& width, int& height) {
  width = q_img.width();
  height = q_img.height();
  if(q_img.isNull()) {
    return false;
  }
  return true;
}

bool taImage::SetImageSize(int width, int height) {
  int cur_wd, cur_ht;
  GetImageSize(cur_wd, cur_ht);
  if(width == cur_wd && height == cur_ht) return false;
  q_img = QImage(QSize(width,height), QImage::Format_ARGB32);
  return true;
}


///////////////////////////////////////////
//	taCanvas

void taCanvas::Initialize() {
  coord_type = PIXELS;
  cur_path = NULL;
  m_init = false;
}
void taCanvas::Destroy() {
  DeletePath();
}

void taCanvas::Copy_(const taCanvas& cp) {
  coord_type = cp.coord_type;
}

void taCanvas::InitCanvas() {
  if(q_img.isNull()) {
    SetImageSize(256,256);
  }
  if(q_painter.isActive())
    q_painter.end();
  q_painter.begin(&q_img);
  DeletePath();
  q_painter.setBackgroundMode(Qt::OpaqueMode);
  q_painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

  fill_brush.setStyle(Qt::SolidPattern);
  
  int cur_wd, cur_ht;
  GetImageSize(cur_wd, cur_ht);
  if(coord_type == PIXELS) {
    //    q_painter.setWorldTransform(QTransform(1.0, 0.0, 0.0, -1.0, 0.0, cur_ht), false);
    q_painter.translate(0, cur_ht);
    q_painter.scale(1.0, -1.0);
  }
  else {
    q_painter.translate(0, cur_ht); // could be 1.0 instead..
    q_painter.scale(cur_wd, -cur_ht);
  }
  m_init = true;
}

void taCanvas::EraseRGBA(float r, float g, float b, float a) {
  if(!CheckInit()) return;
  QColor clr;
  clr.setRgbF(r, g, b, a);
  q_painter.setBackground(QBrush(clr));

  if(coord_type == PIXELS) {
    int cur_wd, cur_ht;
    GetImageSize(cur_wd, cur_ht);
    q_painter.eraseRect(QRectF(0.0f,0.0f, cur_wd,cur_ht));
  }
  else {
    q_painter.eraseRect(QRectF(0.0f,0.0f, 1.0f, 1.0f));
  }
}

void taCanvas::EraseName(const String& name) {
  if(!CheckInit()) return;
  QColor clr((const char*)name);
  q_painter.setBackground(QBrush(clr));

  if(coord_type == PIXELS) {
    int cur_wd, cur_ht;
    GetImageSize(cur_wd, cur_ht);
    q_painter.eraseRect(QRectF(0.0f,0.0f, cur_wd,cur_ht));
  }
  else {
    q_painter.eraseRect(QRectF(0.0f,0.0f, 1.0f, 1.0f));
  }
}

void taCanvas::Point(float x1, float y1) {
  if(!CheckInit()) return;
  q_painter.drawPoint(QPointF(x1, y1));
}

void taCanvas::Line(float x1, float y1, float x2, float y2) {
  if(!CheckInit()) return;
  q_painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
}

void taCanvas::Rect(float l, float b, float r, float t) {
  if(!CheckInit()) return;
  q_painter.drawRect(QRectF(QPointF(l, b), QSizeF(r-l, t-b)));
}
void taCanvas::Circle(float x, float y, float r) {
  if(!CheckInit()) return;
  q_painter.drawEllipse(QRectF(QPointF(x-.5f*r, y-.5f*r), QSizeF(r, r)));
}
void taCanvas::Ellipse(float x, float y, float rx, float ry) {
  if(!CheckInit()) return;
  q_painter.drawEllipse(QRectF(QPointF(x - .5f*rx, y - .5f*ry), QSizeF(rx, ry)));
}
void taCanvas::FillRect(float l, float b, float r, float t) {
  if(!CheckInit()) return;
  q_painter.fillRect(QRectF(QPointF(l, b), QSizeF(r-l, t-b)), fill_brush);
}
void taCanvas::FillCircle(float x, float y, float r) {
  if(!CheckInit()) return;
  QPainterPath pp;
  pp.addEllipse(QRectF(QPointF(x-.5f*r, y-.5f*r), QSizeF(r, r)));
  q_painter.fillPath(pp, fill_brush);
}
void taCanvas::FillEllipse(float x, float y, float rx, float ry) {
  if(!CheckInit()) return;
  QPainterPath pp;
  pp.addEllipse(QRectF(QPointF(x - .5f*rx, y - .5f*ry), QSizeF(rx, ry)));
  q_painter.fillPath(pp, fill_brush);
}

void taCanvas::NewPath() {
  if(!CheckInit()) return;
  if(TestError(cur_path, "NewPath", "New path called before previous one was closed!")) return;
  cur_path = new QPainterPath;
}

void taCanvas::MoveTo(float x, float y) {
  if(!CheckInit()) return;
  if(!cur_path) NewPath();
  cur_path->moveTo(x,y);
}
void taCanvas::LineTo(float x, float y) {
  if(!CheckInit()) return;
  if(TestError(!cur_path, "LineTo", "No current path -- must call NewPath first!")) return;
  cur_path->lineTo(x,y);
}
void taCanvas::CurveTo(float x, float y, float x1, float y1, float x2, float y2) {
  if(!CheckInit()) return;
  if(TestError(!cur_path, "CurveTo", "No current path -- must call NewPath first!")) return;
  cur_path->cubicTo(x1,y1, x2,y2, x,y);
}
void taCanvas::DrawPath() {
  if(!CheckInit()) return;
  if(TestError(!cur_path, "DrawPath", "No current path -- must call NewPath first!")) return;
  q_painter.drawPath(*cur_path);
  DeletePath();
}
void taCanvas::FillPath() {
  if(!CheckInit()) return;
  if(TestError(!cur_path, "FillPath", "No current path -- must call NewPath first!")) return;
  q_painter.fillPath(*cur_path, fill_brush);
  DeletePath();
}

void taCanvas::DrawFillPath() {
  if(!CheckInit()) return;
  if(TestError(!cur_path, "DrawFillPath", "No current path -- must call NewPath first!")) return;
  q_painter.fillPath(*cur_path, fill_brush);
  q_painter.drawPath(*cur_path);
  DeletePath();
}

void taCanvas::DeletePath() {
  if(!cur_path) return;
  delete cur_path;
  cur_path = NULL;
}

void taCanvas::TextLeft(const String& txt, float x, float y) {
  if(!CheckInit()) return;
  q_painter.save();
  q_painter.resetMatrix();
  int cur_wd, cur_ht;
  GetImageSize(cur_wd, cur_ht);
  float xp, yp;
  if(coord_type == PIXELS) {
    xp = x; yp = cur_ht - y;
  }
  else {
    xp = x * (float)cur_wd; yp = cur_ht - (y * (float)cur_ht);
  }
  q_painter.drawText(QPointF(xp,yp), txt);
  q_painter.restore();
}
void taCanvas::TextCenter(const String& txt, float x, float y) {
  if(!CheckInit()) return;
  // this is harder.. need a rectangle..
//   q_painter.drawText(QPointF(x,y), Qt::AlignCenter, txt);
}
void taCanvas::TextRight(const String& txt, float x, float y) {
  if(!CheckInit()) return;
  // this is harder.. need a rectangle..
  //  q_painter.drawText(QPointF(x,y), Qt::AlignRight, txt);
}

void taCanvas::ClipRect(float l, float b, float r, float t) {
  if(!CheckInit()) return;
}

void taCanvas::PenColorRGBA(float r, float g, float b, float a) {
  if(!CheckInit()) return;
  QColor clr;
  clr.setRgbF(r, g, b, a);
  QPen pen = q_painter.pen();
  pen.setColor(clr);
  q_painter.setPen(pen);
}
void taCanvas::PenColorHSVA(float h, float s, float v, float a) {
  if(!CheckInit()) return;
  QColor clr;
  clr.setHsvF(h/360.0f, s, v, a);
  QPen pen = q_painter.pen();
  pen.setColor(clr);
  q_painter.setPen(pen);
}
void taCanvas::PenColorCMYKA(float c, float m, float y, float k, float a) {
  if(!CheckInit()) return;
  QColor clr;
  clr.setCmykF(c, m, y, k, a);
  QPen pen = q_painter.pen();
  pen.setColor(clr);
  q_painter.setPen(pen);
}
void taCanvas::PenColorName(const String& name) {
  if(!CheckInit()) return;
  QColor clr((const char*)name);
  QPen pen = q_painter.pen();
  pen.setColor(clr);
  q_painter.setPen(pen);
}
void taCanvas::PenWidth(float width) {
  if(!CheckInit()) return;
  QPen pen = q_painter.pen();
  pen.setWidthF(width);
  q_painter.setPen(pen);
}
void taCanvas::FillColorRGBA(float r, float g, float b, float a) {
  if(!CheckInit()) return;
  QColor clr;
  clr.setRgbF(r, g, b, a);
  fill_brush.setColor(clr);
}
void taCanvas::FillColorHSVA(float h, float s, float v, float a) {
  if(!CheckInit()) return;
  QColor clr;
  clr.setHsvF(h/360.0f, s, v, a);
  fill_brush.setColor(clr);
}
void taCanvas::FillColorCMYKA(float c, float m, float y, float k, float a) {
  if(!CheckInit()) return;
  QColor clr;
  clr.setCmykF(c, m, y, k, a);
  fill_brush.setColor(clr);
}
void taCanvas::FillColorName(const String& name) {
  if(!CheckInit()) return;
  QColor clr((const char*)name);
  fill_brush.setColor(clr);
}
void taCanvas::SetFont(const String& font_name, int point_size, int weight, bool italic) {
  if(!CheckInit()) return;
  q_painter.setFont(QFont(font_name, point_size, weight, italic));
}

////////////////////////////////////////////////////////
//	Threading

void ImgProcCallTask::Initialize() {
  img_proc_call = NULL;
}

void ImgProcCallTask::Destroy() {
  img_proc_call = NULL;
}

void ImgProcCallTask::run() {
  ImgProcCallThreadMgr* mg = mgr();
  ImgProcThreadBase* base = mg->img_proc();

  // all nibbling all the time
  const int nib_chnk = mg->nibble_chunk;
  const int nib_stop = mg->n_cmp_units;

  while(true) {
    int nxt_uidx = mg->nibble_i.fetchAndAddOrdered(nib_chnk);
    if(nxt_uidx >= nib_stop) break;
    const int mx = MIN(nib_stop, nxt_uidx + nib_chnk);
    for(int i=nxt_uidx; i <mx; i++) {
      img_proc_call->call(base, i, task_id); // task id indicates threading, and which thread
    }
    if(mx == nib_stop) break;		// we're the last guy
  }
}

void ImgProcCallThreadMgr::Initialize() {
  min_units = taMisc::thread_defaults.min_units;
  nibble_chunk = taMisc::thread_defaults.nibble_chunk;
  task_type = &TA_ImgProcCallTask;
}

void ImgProcCallThreadMgr::Destroy() {
}

void ImgProcCallThreadMgr::Run(ThreadImgProcCall* img_proc_call, int n_cmp_un) {
  InitAll();			// make sure

  n_cmp_units = n_cmp_un;

  ImgProcThreadBase* base = img_proc();
  if(n_threads == 1 || n_cmp_units < min_units || n_cmp_units < tasks.size) {
    for(int i=0;i<n_cmp_units;i++) {
      img_proc_call->call(base, i, -1); // -1 indicates no threading
    }
  }
  else {
    // everything is done with nibbling -- so much more effective at load balancing
    nibble_i = 0;

    // set the call
    for(int i=0;i<tasks.size;i++) {
      ImgProcCallTask* uct = (ImgProcCallTask*)tasks[i];
      uct->img_proc_call = img_proc_call;
    }

    // then run the subsidiary guys
    RunThreads();

    tasks[0]->run();		// run our own set..

    // finally, always need to sync at end to ensure that everyone is done!
    SyncThreads();
  }
}

///////////////////////////////////////////
//	DoG Filter

void DoGFilter::Initialize() {
  filter_width = 8;
  filter_size = filter_width * 2 + 1;
  on_sigma = 2.0f;
  off_sigma = 4.0f;
  circle_edge = true;
  on_filter.SetGeom(2, filter_size, filter_size);
  off_filter.SetGeom(2, filter_size, filter_size);
  net_filter.SetGeom(2, filter_size, filter_size);
}

void DoGFilter::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  filter_size = filter_width * 2 + 1;
  UpdateFilter();
}

float DoGFilter::FilterPoint_rgb(int x, int y, ColorChannel color,
				 float r_val, float g_val, float b_val) {
  float flt = net_filter.FastEl(x+filter_width, y+filter_width);
  switch(color) {
  case LUMINANCE: {
    float grey = 0.33333f * (r_val + g_val + b_val);
    return grey * flt;
  }
  case RED_CYAN: {
    float r_v_c = r_val - 0.5f * (g_val + b_val);
    return r_v_c * flt;
  }
  case GREEN_MAGENTA: {
    float g_v_m = g_val - 0.5f * (r_val + b_val);
    return g_v_m * flt;
  }
  case BLUE_YELLOW: {
    float b_v_y = b_val - 0.5f * (r_val + g_val);
    return b_v_y * flt;
  }
  }
  return 0.0f;
}

void DoGFilter::InvertFilterPoint_rgb(int x, int y, float act, ColorChannel color,
				       float& r_val, float& g_val, float& b_val) {
  float flt = net_filter.FastEl(x+filter_width, y+filter_width);
  switch(color) {
  case LUMINANCE: {
    float grey = (0.5f * act * flt) + 0.5f;
    r_val = grey; g_val = grey; b_val = grey;
    break;
  }
  }
//   case RED_CYAN: {
//     float r_v_c = act * filt;
//     r_v_c = r_val - 0.5f * (g_val + b_val);
//     return r_v_c * flt;
//   }
//   case GREEN_MAGENTA: {
//     float g_v_m = g_val - 0.5f * (r_val + b_val);
//     return g_v_m * flt;
//   }
//   case BLUE_YELLOW: {
//     float b_v_y = b_val - 0.5f * (r_val + g_val);
//     return b_v_y * flt;
//   }
//   }
//   return 0.0f;
}

void DoGFilter::RenderFilter(float_Matrix& on_flt, float_Matrix& off_flt,
				 float_Matrix& net_flt) {
  on_flt.SetGeom(2, filter_size, filter_size);
  off_flt.SetGeom(2, filter_size, filter_size);
  net_flt.SetGeom(2, filter_size, filter_size);
  int x,y;
  for(y=-filter_width; y<=filter_width; y++) {
    for(x=-filter_width; x<=filter_width; x++) {
      float dist = taMath_float::hypot(x, y);
      float ong = 0.0f;
      float offg = 0.0f;
      if(!circle_edge || (dist <= filter_width)) { // only set values inside of filter radius
	ong = taMath_float::gauss_den_sig(dist, on_sigma);
	offg = taMath_float::gauss_den_sig(dist, off_sigma);
      }
      on_flt.Set(ong, x+filter_width, y+filter_width);
      off_flt.Set(offg, x+filter_width, y+filter_width);
    }
  }

  taMath_float::vec_norm_sum(&on_flt); // make sure sums to 1.0
  taMath_float::vec_norm_sum(&off_flt); // make sure sums to 1.0

  for(int i=0;i<on_flt.size;i++) {
    float net = on_flt.FastEl_Flat(i) - off_flt.FastEl_Flat(i);
    net_flt.FastEl_Flat(i) = net;
  }
  //  taMath_float::vec_norm_abs_max(&net_flt); // max norm = 1
}

void DoGFilter::UpdateFilter() {
  RenderFilter(on_filter, off_filter, net_filter);
}

void DoGFilter::GraphFilter(DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_DoG_GraphFilter", true);
  }
  graph_data->StructUpdate(true);
  graph_data->Reset();
  int idx;
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
  DataCol* zda = graph_data->FindMakeColName("Z", idx, VT_FLOAT);
  DataCol* valda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);

  xda->SetUserData("X_AXIS", true);
  zda->SetUserData("Z_AXIS", true);
  valda->SetUserData("PLOT_1", true);

  float_Matrix* mat = &net_filter;
  int x,z;
  for(z=-filter_width; z<=filter_width; z++) {
    for(x=-filter_width; x<=filter_width; x++) {
      float val = mat->FastEl(x+filter_width, z+filter_width);
      graph_data->AddBlankRow();
      xda->SetValAsFloat(x, -1);
      zda->SetValAsFloat(z, -1);
      valda->SetValAsFloat(val, -1);
    }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void DoGFilter::GridFilter(DataTable* graph_data, bool reset) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_DoG_GridFilter", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->Reset();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, filter_size, filter_size);

  float maxv = taMath_float::vec_max(&on_filter, idx);

  graph_data->SetUserData("N_ROWS", 3);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);

  for(int i=0;i<3;i++) {
    float_Matrix* mat;
    graph_data->AddBlankRow();
    if(i==0) {
      nmda->SetValAsString("On", -1);
      mat = &on_filter;
    }
    else if(i==1) {
      nmda->SetValAsString("Off", -1);
      mat = &off_filter;
    }
    else {
      nmda->SetValAsString("Net", -1);
      mat = &net_filter;
    }
    matda->SetValAsMatrix(mat, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}


//////////////////////////////////////////////////////////
//  	Gabor Filters!

void GaborFilter::Initialize() {
  x_size = 2;
  y_size = 2;
  ctr_x = .5f * x_size;
  ctr_y = .5f * y_size;
  angle = 0;
  phase = 0;
  freq = 1;
  length = 8;
  width = 4;
  amp = 1;
  filter.SetGeom(2, x_size, y_size);
  // for some reason this initialization fails in Windows
}

float GaborFilter::Eval(float x, float y) {
  // normalize into normal coords, where sin goes along x axis 
  float cx = x - ctr_x;
  float cy = y - ctr_y;

  float r = sqrtf(cx*cx + cy*cy);
  float thet = atan2(cy, cx);
  float totang = thet - angle;
  float n_x = r * cos(totang);
  float n_y = r * sin(totang);

  float rval = amp * cos(phase + freq * n_y) * 
    exp(-.5f * ((n_y * n_y) / (width * width) + (n_x * n_x) / (length * length)));

  return rval;
}

void GaborFilter::RenderFilter(float_Matrix& flt) {
  flt.SetGeom(2, x_size, y_size);
  for(int y=0;y<y_size;y++) {
    for(int x=0;x<x_size;x++) {
      flt.Set(Eval(x, y), x, y);
    }
  }
}

void GaborFilter::UpdateFilter() {
  RenderFilter(filter);
}

float GaborFilter::GetParam(GaborParam param) {
  switch(param) {
  case CTR_X: 	return ctr_x;
  case CTR_Y: 	return ctr_y;
  case ANGLE:	return angle;
  case PHASE:	return phase;
  case FREQ:	return freq;
  case LENGTH:	return length;
  case WIDTH:	return width;
  case AMP:	return amp;
  }	
  return 0.0f;
}


void GaborFilter::GraphFilter(DataTable* graph_data) {
  UpdateFilter();
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_Gabor_GraphFilter", true);
  }
  graph_data->StructUpdate(true);
  graph_data->Reset();
  int idx;
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
  DataCol* zda = graph_data->FindMakeColName("Z", idx, VT_FLOAT);
  DataCol* valda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);

  xda->SetUserData("X_AXIS", true);
  zda->SetUserData("Z_AXIS", true);
  valda->SetUserData("PLOT_1", true);

  int x,z;
  for(z=0; z<y_size; z++) {
    for(x=0; x<x_size; x++) {
      float val = filter.FastEl(x,z);
      graph_data->AddBlankRow();
      xda->SetValAsFloat(x, -1);
      zda->SetValAsFloat(z, -1);
      valda->SetValAsFloat(val, -1);
    }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void GaborFilter::GridFilter(DataTable* graph_data, bool reset) {
  UpdateFilter();
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_Gabor_GridFilter", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->ResetData();
  int idx;
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, x_size, y_size);

  float maxv = taMath_float::vec_abs_max(&filter, idx);

  graph_data->SetUserData("N_ROWS", 1);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);

  graph_data->AddBlankRow();
  matda->SetValAsMatrix(&filter, -1);

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

void GaborFilter::OutputParams(ostream& strm) {
  strm << "ctr: " << ctr_x << ", " << ctr_y << ", angle: " << angle
       << ", phase: " << phase << ", freq: " << freq
       << ", length: " << length << ", width: " << width
       << ", amp: " << amp
       << endl;
}


void MotionGaborFilter::Initialize() {
/*
  x_size = 2;
  y_size = 2;
  ctr_x = .5f * x_size;
  ctr_y = .5f * y_size;
  angle = 0;
  phase = 0;
  freq = 1;
  length = 8;
  width = 4;
  amp = 1;
  filter.SetGeom(2, x_size, y_size);
*/
  x_size = 4;		
  y_size = 4;		
  t_size = 3;		
  ctr_x = .5f * x_size;		
  ctr_y = .5f * y_size;		
  ctr_t = .5f * t_size;		

  spat_angle = 0;		// angle of sine wave in 2-d space (in radians)
  time_angle = 1;		// angle of sine wave in 2-d time (in radians)

  phase = 0;		// phase of sine wave wrt the center of the gaussian (radians)
  freq = 1;		// frequency of the sine wave
  freq_t = 1;		// frequency of the sine wave

  width = 4;		// width of the gaussian in the wave direction
  length = 8;		// width of the gaussian in the wave direction
  width_t = 6;		// width of the gaussian in the wave direction

  amp = 1;		// amplitude (maximum value)
  filter.SetGeom(3, x_size, y_size, t_size);
}

float MotionGaborFilter::Eval(float x, float y, float t) {
  // normalize into normal coords, where sin goes along x axis 
  float cx = x - ctr_x;
  float cy = y - ctr_y;
  float ct = time_angle*(t - ctr_t);
	
	
  float r = sqrtf(cx*cx + cy*cy);
  float thet = atan2(cy, cx);
  float totang = thet - spat_angle;
  float n_x = r * cos(totang);
  float n_y = r * sin(totang);
	
  float rval = 0;
  if(use_3d_gabors) {
    rval = amp * cos(phase + freq * n_y + freq_t*ct) * 
      exp(-.5f * ((n_y * n_y) / (width * width) + (n_x * n_x) / (length * length) + (ct*ct)/(width_t*width_t) ));
  }
  else {
    n_y += (t - ctr_t) * freq_t;
    rval = amp * cos(phase + freq * n_y) * 
      exp(-.5f * ((n_y * n_y) / (width * width) + (n_x * n_x) / (length * length)));
  }
	
  return rval;
}

void MotionGaborFilter::RenderFilter(float_Matrix& flt) {
  flt.SetGeom(3, x_size, y_size, t_size);
  for(int y=0;y<y_size;y++) {
    for(int x=0;x<x_size;x++) {
       for(int t = 0; t < t_size; t++) {
		   float val = Eval(x, y, t);
         flt.Set(val, x, y, t);
       }
    }
  }
}

void MotionGaborFilter::UpdateFilter() {
  RenderFilter(filter);
}

float MotionGaborFilter::GetParam(MotionGaborParam param) {
  switch(param) {
  case CTR_X: 	return ctr_x;
  case CTR_Y: 	return ctr_y;
  case CTR_T: 	return ctr_t;
  case TIME_ANGLE:	return spat_angle;
  case SPAT_ANGLE:	return time_angle;
  case PHASE:	return phase;
  case FREQ:	return freq;
  case FREQ_T:	return freq_t;
  case LENGTH:	return length;
  case WIDTH:	return width;
  case WIDTH_T:	return width_t;
  case AMP:	return amp;
  }	
  return 0.0f;
}


void MotionGaborFilter::GraphFilter(DataTable* graph_data) {
  UpdateFilter();
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_MotionGabor_GraphFilter", true);
  }
  graph_data->StructUpdate(true);
  graph_data->Reset();
  int idx;
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
  DataCol* zda = graph_data->FindMakeColName("Z", idx, VT_FLOAT);
  DataCol* valda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);

  xda->SetUserData("X_AXIS", true);
  zda->SetUserData("Z_AXIS", true);
  valda->SetUserData("PLOT_1", true);

  int x,z,t;
  for(t=0; t<t_size; t++) {
    for(z=0; z<y_size; z++) {
      for(x=0; x<x_size; x++) {
        float val = filter.FastEl(x,z,t);
        graph_data->AddBlankRow();
        xda->SetValAsFloat(x, -1);
        zda->SetValAsFloat(z, -1);
        valda->SetValAsFloat(val, -1);
      } 
    }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void MotionGaborFilter::GridFilter(DataTable* graph_data, bool reset) {
  UpdateFilter();
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_MotionGabor_GridFilter", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->Reset();
  int idx;
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 3, x_size, y_size, t_size);

  float maxv = taMath_float::vec_abs_max(&filter, idx);

  graph_data->SetUserData("N_ROWS", 1);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);

  graph_data->AddBlankRow();
  matda->SetValAsMatrix(&filter, -1);

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

void MotionGaborFilter::OutputParams(ostream& strm) {
  strm << "ctr: " << ctr_x << ", " << ctr_y << ", angle: " << spat_angle
       << ", phase: " << phase << ", freq: " << freq
       << ", length: " << length << ", width: " << width
       << ", amp: " << amp
       << endl;
}


//////////////////////////////////////////////////////////
//  	Gabor Fitter!


void GaborFitter::Initialize() {
  fit_dist = 0.0f;
}

static inline float gfs_sqdist(const float f1, const float f2) {
  float d = f1-f2;
  return d * d;
}

float GaborFitter::ParamDist(const GaborFilter& oth) {
  return sqrtf(gfs_sqdist(ctr_x, oth.ctr_x) + gfs_sqdist(ctr_y, oth.ctr_y)
	       + gfs_sqdist(angle, oth.angle) + gfs_sqdist(phase, oth.phase) 
	       + gfs_sqdist(freq, oth.freq)  + gfs_sqdist(length, oth.length) 
	       + gfs_sqdist(width, oth.width) + gfs_sqdist(amp, oth.amp));
}

// float GaborFitter::SquaredDist(float_Matrix& data_vals) {
//   float rval = 0.0f;
//   int x,y;
//   int ctr = 0;
//   for(y=0;y<y_size;y++) {
//     for(x=0;x<x_size;x++, ctr++) {
//       float d = Eval(x, y) - data_vals[ctr];
//       rval += d * d;
//     }
//   }
//   return rval;
// }

// float GaborFitter::FitData_firstpass(float_Matrix& data_vals) {
//   float ctr_brd = 1.0;
//   float	ctr_inc = (x_size - 2.0f * ctr_brd) / 3.0f;
//   float ang_inc = PI / 3.0f;
//   float phs_inc = PI;
//   float	frq_min = PI / x_size;
//   float frq_max = PI;
//   float frq_inc = (frq_max - frq_min) / 2.0f;
//   float sz_min = 1.0f;
//   float sz_max = .5f;
//   float sz_inc = ((x_size * sz_max) - sz_min) / 2.0f;

// //   float tot_comp = 3.0f * 3.0f * 4.0f * 2.0f * 2.0f * 2.0f * 2.0f;
// //   cerr << "total cycles: " << tot_comp << endl;

//   float min_d = FLT_MAX;
//   GaborFitter min_params;
//   String my_nm = name;

//   for(ctr_x = ctr_brd; ctr_x <= x_size - ctr_brd; ctr_x += ctr_inc) {
//     for(ctr_y = ctr_brd; ctr_y <= y_size - ctr_brd; ctr_y += ctr_inc) {
//       for(angle = 0.0f; angle <= PI; angle += ang_inc) {
// 	for(phase = 0.0f; phase <= 2.0f * PI; phase += phs_inc) {
// 	  for(freq = frq_min; freq <= frq_max; freq += frq_inc) {
// 	    for(length = sz_min; length <= y_size * sz_max; length += sz_inc) {
// 	      for(width = sz_min; width <= x_size * sz_max; width += sz_inc) {
// 		float dist = SquaredDist(data_vals);
// 		if(dist < min_d) {
// 		  min_d = dist;
// 		  min_params = *this;
// 		}
// 	      }
// 	    }
// 	  }
// 	}
//       }
//     }
//   }
//   *this = min_params;
//   name = my_nm;
//   return min_d;
// }

// float GaborFitter::FitData(float_Matrix& data_vals, bool use_cur_vals) {
//   float min_d = FLT_MAX;
//   if(!use_cur_vals) {
//     min_d = FitData_firstpass(data_vals);
// //     cerr << "After 1st Pass\t min_d: " << min_d << "\t";
// //     OutputParams();
//   }

//   int n_itr = 3;
//   int itr;
//   for(itr = 0; itr<n_itr; itr++) {
//     float rng_div = (2.0f * float(itr) + 1.0f);
//     float inc_div = 6.0f;

// //     float amp_rng = .4 / rng_div;
// //     float amp_inc = amp_rng / inc_div;
//     float ctr_rng = 8.0 / rng_div;
//     float ctr_inc = ctr_rng / inc_div;
//     float ang_rng = (PI / 2.0) / rng_div;
//     float ang_inc = ang_rng / inc_div;
//     float phs_rng = PI / rng_div;
//     float phs_inc = phs_rng / inc_div;
//     float sub_ctr_rng = 3.0 / rng_div;
//     float sub_ctr_inc = ctr_rng / 3.0f;
//     float frq_rng = (PI / 2.0) / rng_div;
//     float frq_inc = frq_rng / inc_div;
//     float sz_rng = 4.0 / rng_div;
//     float sz_inc = sz_rng / inc_div;
//     float sub_wd_rng = 2.0 / rng_div;
//     float sub_wd_inc = sz_rng / 4.0;

//     amp = 1.0f;
// //     float cur_amp = amp;
// //     float min_amp = amp;
// //     for(amp = cur_amp - amp_rng; amp <= cur_amp + amp_rng; amp += amp_inc) {
// //       if(amp < .75) continue;
// //       float dist = SquaredDist(data_vals);
// //       if(dist < min_d) {
// // 	min_d = dist;
// // 	min_amp = amp;
// //       }
// //     }
// //     amp = min_amp;

//     float cur_ctr_x = ctr_x;  float cur_ctr_y = ctr_y;
//     float min_ctr_x = ctr_x;  float min_ctr_y = ctr_y;
//     for(ctr_x = cur_ctr_x-ctr_rng; ctr_x <= cur_ctr_x + ctr_rng; ctr_x += ctr_inc) {
//       if((ctr_x < 1) || (ctr_x >= x_size-1)) continue;
//       for(ctr_y = cur_ctr_y-ctr_rng; ctr_y <= cur_ctr_y + ctr_rng; ctr_y += ctr_inc) {
// 	if((ctr_y < 1) || (ctr_y >= y_size-1)) continue;
// 	float dist = SquaredDist(data_vals);
// 	if(dist < min_d) {
// 	  min_d = dist;
// 	  min_ctr_x = ctr_x; min_ctr_y = ctr_y;
// 	}
//       }
//     }
//     ctr_x = min_ctr_x; ctr_y = min_ctr_y;

//     float cur_ang = angle;
//     float min_ang = angle;
//     for(angle = cur_ang - ang_rng; angle <= cur_ang + ang_rng; angle += ang_inc) {
//       float dist = SquaredDist(data_vals);
//       if(dist < min_d) {
// 	min_d = dist;
// 	min_ang = angle;
//       }
//     }
//     angle = min_ang;
//     if(angle < 0.0f) angle += PI;
//     if(angle > PI) angle -= PI;

//     float min_frq = freq;
//     float cur_frq = freq;
//     for(freq = cur_frq - frq_rng; freq <= cur_frq + frq_rng; freq += frq_inc) {
//       if(freq < .1) continue;
//       float dist = SquaredDist(data_vals);
//       if(dist < min_d) {
// 	min_d = dist;
// 	min_frq = freq;
//       }
//     }
//     freq = min_frq;

//     float cur_wid = width;
//     float min_wid = width;
//     for(width = cur_wid - sz_rng; width <= cur_wid + sz_rng; width += sz_inc) {
//       if(width < .75) continue;
//       float dist = SquaredDist(data_vals);
//       if(dist < min_d) {
// 	min_d = dist;
// 	min_wid = width;
//       }
//     }
//     width = min_wid;

//     float cur_phs = phase; cur_ctr_x = ctr_x;  cur_ctr_y = ctr_y; cur_wid = width;
//     float min_phs = phase; min_ctr_x = ctr_x;  min_ctr_y = ctr_y; min_wid = width;
//     for(phase = cur_phs - phs_rng; phase <= cur_phs + phs_rng; phase += phs_inc) {
//       for(ctr_x = cur_ctr_x-sub_ctr_rng; ctr_x <= cur_ctr_x + sub_ctr_rng; ctr_x += sub_ctr_inc) {
// 	if((ctr_x < 1) || (ctr_x >= x_size-1)) continue;
// 	for(ctr_y = cur_ctr_y-sub_ctr_rng; ctr_y <= cur_ctr_y + sub_ctr_rng; ctr_y += sub_ctr_inc) {
// 	  if((ctr_y < 1) || (ctr_y >= y_size-1)) continue;
// 	  for(width = cur_wid - sub_wd_rng; width <= cur_wid + sub_wd_rng; width += sub_wd_inc) {
// 	    if(width < .75) continue;
// 	    float dist = SquaredDist(data_vals);
// 	    if(dist < min_d) {
// 	      min_d = dist;
// 	      min_phs = phase; min_ctr_x = ctr_x; min_ctr_y = ctr_y; min_wid = width;
// 	    }
// 	  }
// 	}
//       }
//     }
//     phase = min_phs;
//     if(phase < 0.0f) phase += 2.0f * PI;
//     if(phase > 2.0f * PI) phase -= 2.0f * PI;
//     ctr_x = min_ctr_x; ctr_y = min_ctr_y;
//     width = min_wid;

//     float cur_len = length;
//     float min_len = length;
//     for(length = cur_len - sz_rng; length <= cur_len + sz_rng; length += sz_inc) {
//       if(length < .75) continue;
//       float dist = SquaredDist(data_vals);
//       if(dist < min_d) {
// 	min_d = dist;
// 	min_len = length;
//       }
//     }
//     length = min_len; 

// //     cerr << "Itr: " << itr << "\t min_d: " << min_d << "\t";
// //     OutputParams();
//   }

// //   cerr << "Final fit\t min_d: " << min_d << "\t";
// //   OutputParams();
//   fit_dist = min_d;
//   return min_d;
// }

// float GaborFitter::TestFit() {
//   ctr_x = 2 + Random::IntZeroN(x_size - 4);
//   ctr_y = 2 + Random::IntZeroN(y_size - 4);
//   angle = PI * Random::ZeroOne();
//   phase = 2.0f * PI * Random::ZeroOne();
//   float	frq_min = PI / x_size;
//   float frq_max = PI;
//   float frq_inc = (frq_max - frq_min);
//   freq = frq_min + frq_inc * Random::ZeroOne();
//   length = .75 + ((.25 * y_size) - .5) * Random::ZeroOne();
//   width = .75 + ((.25 * x_size) - .5) * Random::ZeroOne();
//   amp = 1.0;
//   cerr << "\nBefore:\t";
//   OutputParams();
//   GridFilter(NULL);
//   taivMisc::RunIVPending();
//   float_Matrix data(false);
//   RenderFilter(data);
//   float min_d = FitData(data, false);
//   cerr << "Min Dist: " << min_d << endl;
//   cerr << "After:\t";
//   OutputParams();
//   GridFilter(NULL);
//   return min_d;
// }

//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// 		taImageProc

void taImageProc::Initialize() {
}

void taImageProc::Destroy() {
}

bool taImageProc::RenderBorder_float(float_Matrix& img_data) {
  if(img_data.dims() == 3) {	// an rgb guy
    for(int i=0;i<3;i++) {
      float_Matrix* cmp = img_data.GetFrameSlice(i);
      taBase::Ref(cmp);
      RenderBorder_float(*cmp);
      taBase::unRefDone(cmp);
    }
    return true;
  }
  TwoDCoord img_size(img_data.dim(0), img_data.dim(1));
  float tavg = 0.0f;
  float bavg = 0.0f;
  for(int x=0;x<img_size.x;x++) {
    tavg += img_data.FastEl(x, img_size.y-1);
    bavg += img_data.FastEl(x, 0);
  }
  tavg /= (float)(img_size.x);
  bavg /= (float)(img_size.x);

  float lavg = 0.0f;
  float ravg = 0.0f;
  for(int y=0;y<img_size.y;y++) {
    ravg += img_data.FastEl(img_size.x-1, y);
    lavg += img_data.FastEl(0, y);
  }
  lavg /= (float)(img_size.y);
  ravg /= (float)(img_size.y);

  float oavg = .25 * (tavg + bavg + lavg + ravg);

  for(int x=0;x<img_size.x;x++) {
    img_data.FastEl(x, img_size.y-1) = oavg;
    img_data.FastEl(x, 0) = oavg;
  }
  for(int y=1;y<img_size.y-1;y++) {
    img_data.FastEl(img_size.x-1, y) = oavg;
    img_data.FastEl(0, y) = oavg;
  }
//   cerr << "border avgs: t: " << tavg << ", b: " << bavg
//        << ", l: " << lavg << ", r: " << ravg << endl;
  return true;
}

bool taImageProc::FadeEdgesToBorder_float(float_Matrix& img_data, int fade_width) {
  if(img_data.dims() == 3) {	// an rgb guy
    for(int i=0;i<3;i++) {
      float_Matrix* cmp = img_data.GetFrameSlice(i);
      taBase::Ref(cmp);
      FadeEdgesToBorder_float(*cmp, fade_width);
      taBase::unRefDone(cmp);
    }
    return true;
  }
  TwoDCoord img_size(img_data.dim(0), img_data.dim(1));
  float oavg = img_data.FastEl(0,0); // assuming already has renderborder called
  for(int wd=1; wd<=fade_width;wd++) {
    float pct = (float)wd / (float)(fade_width+1);
    float pct_c = 1.0f - pct;
    float oavgadd = pct_c * oavg;
    for(int x=wd;x<img_size.x-wd;x++) {
      float& tv = img_data.FastEl(x, img_size.y-1-wd);
      float& bv = img_data.FastEl(x, wd);
      tv = oavgadd + pct * tv;
      bv = oavgadd + pct * bv;
    }
    for(int y=wd+1;y<img_size.y-wd-1;y++) {
      float& rv = img_data.FastEl(img_size.x-1-wd, y);
      float& lv = img_data.FastEl(wd, y);
      rv = oavgadd + pct * rv;
      lv = oavgadd + pct * lv;
    }
  }
  return true;
}

bool taImageProc::RenderOccluderBorderColor_float(float_Matrix& img_data, 
						  float llx, float lly, float urx, float ury) {
  if(img_data.dims() == 3) {	// an rgb guy
    for(int i=0;i<3;i++) {
      float_Matrix* cmp = img_data.GetFrameSlice(i);
      taBase::Ref(cmp);
      RenderOccluderBorderColor_float(*cmp, llx, lly, urx, ury);
      taBase::unRefDone(cmp);
    }
    return true;
  }
  TwoDCoord img_size(img_data.dim(0), img_data.dim(1));
  TwoDCoord ll;
  ll.x = (int)(img_size.x * llx);  ll.y = (int)(img_size.y * lly);
  TwoDCoord ur;
  ur.x = (int)(img_size.x * urx);  ur.y = (int)(img_size.y * ury);
  float oavg = img_data.FastEl(0,0); // assuming already has renderborder called
  for(int y = ll.y; y < ur.y; y++) {
    for(int x = ll.x; x < ur.x; x++) {
      img_data.FastEl(x, y) = oavg;
    }
  }
  return true;
}

bool taImageProc::TranslateImagePix_float(float_Matrix& xlated_img, float_Matrix& orig_img,
					  int move_x, int move_y, EdgeMode edge) {

  TwoDCoord img_size(orig_img.dim(0), orig_img.dim(1)); 
  FloatTwoDCoord img_ctr = FloatTwoDCoord(img_size) / 2.0f;
  TwoDCoord img_off = TwoDCoord(move_x, move_y);

  bool rgb_img = false;
  if(orig_img.dims() == 3) { // rgb
    xlated_img.SetGeom(3, img_size.x, img_size.y, 3);
    rgb_img = true;
  }
  else
    xlated_img.SetGeom(2, img_size.x, img_size.y);

  bool wrap = (edge == WRAP);

  TwoDCoord ic;
  for(int ny = 0; ny < img_size.y; ny++) {
    ic.y = ny - img_off.y;
    for(int nx = 0; nx < img_size.x; nx++) {
      ic.x = nx - img_off.x;
      if(ic.WrapClip(wrap, img_size)) {
	if(edge == CLIP) continue; // bail on clipping only
      }
      if(rgb_img) {
	for(int i=0;i<3;i++)
	  xlated_img.FastEl(nx, ny, i) = orig_img.FastEl(ic.x, ic.y, i);
      }
      else {
	xlated_img.FastEl(nx, ny) = orig_img.FastEl(ic.x, ic.y);
      }
    }
  }
  return true;
}

bool taImageProc::TranslateImage_float(float_Matrix& xlated_img, float_Matrix& orig_img,
				       float move_x, float move_y, EdgeMode edge) {

  FloatTwoDCoord deltas(move_x, move_y);
  TwoDCoord img_size(orig_img.dim(0), orig_img.dim(1)); 
  FloatTwoDCoord img_ctr = FloatTwoDCoord(img_size) / 2.0f;
  TwoDCoord img_off = TwoDCoord(deltas * img_ctr);

  return TranslateImagePix_float(xlated_img, orig_img, img_off.x, img_off.y, edge);
}

bool taImageProc::ScaleImage_float(float_Matrix& scaled_img, float_Matrix& orig_img,
				   float scale, EdgeMode edge) {
  if(scale < .01f) {
    taMisc::Error("Can't scale below .01.");
    return false;
  }
  if(scale > 100.0f) {
    taMisc::Error("Can't scale above 100.");
    return false;
  }
  TwoDCoord img_size(orig_img.dim(0), orig_img.dim(1)); 
  TwoDCoord scaled_size; 
  scaled_size.x = 2 + (int)(scale * (img_size.x-2)); // keep border in there
  scaled_size.y = 2 + (int)(scale * (img_size.y-2));

  bool rgb_img = false;
  if(orig_img.dims() == 3) { // rgb
    scaled_img.SetGeom(3, scaled_size.x, scaled_size.y, 3);
    rgb_img = true;
  }
  else
    scaled_img.SetGeom(2, scaled_size.x, scaled_size.y);
  float half_sc;
  if(scale < 1.0f)
    half_sc = .5f / scale; // how much on each side of the pixel to include in avg
  else
    half_sc = .5f * scale;

  // scale > 1
  //   o o o    3
  //  /|X|X|\   connections
  // s s s s s  5

  int	half_int = (int)floor(half_sc);
  float extra = half_sc - (float)half_int;
  int n_orig_pix = half_int + 1; // number of pixels to get from original image for each scaled pixel
  //    int tot_org_pix = 2 * n_orig_pix + 1;
  float_Matrix sc_ary(false);
  sc_ary.SetGeom(2, n_orig_pix*2 + 1, n_orig_pix*2 + 1);
  int x, y;
  for(y=-n_orig_pix; y<= n_orig_pix; y++) {
    float ysc = extra;
    if((y >= -half_int) && (y <= half_int))
      ysc = 1.0f;
    for(x=-n_orig_pix; x<= n_orig_pix; x++) {
      float xsc = extra;
      if((x >= -half_int) && (x <= half_int))
	xsc = 1.0f;
      sc_ary.Set(ysc * xsc, x + n_orig_pix, y + n_orig_pix);
    }
  }

  taMath_float::vec_norm_sum(&sc_ary);		// make it add to 1

  bool wrap = (edge == WRAP);

  TwoDCoord ic;
  for(y=0; y<scaled_size.y; y++) {
    int oyc = (int)floor(.5f + ((float)y / scale));
    for(x=0; x<scaled_size.x; x++) {
      int oxc = (int)floor(.5f + ((float)x / scale));
      float r_avg = 0.0f;
      float g_avg = 0.0f;
      float b_avg = 0.0f;
      int oxi, oyi;
      for(oyi=-n_orig_pix; oyi<=n_orig_pix; oyi++) {
	ic.y = oyc + oyi;
	for(oxi=-n_orig_pix;oxi<=n_orig_pix;oxi++) {
	  ic.x = oxc + oxi;
	  if(ic.WrapClip(wrap, img_size)) {
	    if(edge == CLIP) continue; // bail on clipping only
	  }
	  float sc = sc_ary.FastEl(oxi + n_orig_pix, oyi + n_orig_pix);
	  if(rgb_img) {
	    r_avg += sc * orig_img.FastEl(ic.x, ic.y, 0);
	    g_avg += sc * orig_img.FastEl(ic.x, ic.y, 1);
	    b_avg += sc * orig_img.FastEl(ic.x, ic.y, 2);
	  }
	  else {
	    r_avg += sc * orig_img.FastEl(ic.x, ic.y);
	  }
	}
      }
      if(rgb_img) {
	scaled_img.FastEl(x, y, 0) = r_avg;
	scaled_img.FastEl(x, y, 1) = g_avg;
	scaled_img.FastEl(x, y, 2) = b_avg;
      }
      else {
	scaled_img.FastEl(x, y) = r_avg;
      }	
    }
  }
  return true;
}

// get pixel coordinates (pc1, 2) with norm weights (pw1, 2) for given floating coordinate coord
void taImageProc::GetWeightedPixels_float(float coord, int size, int* pc, float* pw) {
  //   |  .|   |  get from two closest pixels..
  pc[0] = (int)floor(coord);
  float xfrac = coord - floor(coord);
  if(xfrac < .5f) {
    pw[0] = xfrac;
    pc[1] = pc[0]-1; pw[1] = 1.0f - xfrac;
  }
  else if(xfrac > .5f) {
    pw[0] = 1.0f - xfrac;
    pc[1] = pc[0]+1; pw[1] = xfrac;
  }
  else {
    pw[0] = pw[1] = .5f;
    pc[1] = pc[0];
  }
  if(pc[0] < 0) pc[0] = 0; if(pc[0] >= size) pc[0] = size-1;      
  if(pc[1] < 0) pc[1] = 0; if(pc[1] >= size) pc[1] = size-1;      
}


bool taImageProc::RotateImage_float(float_Matrix& rotated_img, float_Matrix& orig_img,
				    float rotate, EdgeMode edge) {
  TwoDCoord img_size(orig_img.dim(0), orig_img.dim(1));

  bool rgb_img = false;
  if(orig_img.dims() == 3) { // rgb
    rotated_img.SetGeom(3, img_size.x, img_size.y, 3);
    rgb_img = true;
  }
  else
    rotated_img.SetGeom(2, img_size.x, img_size.y);
  FloatTwoDCoord ctr = ((FloatTwoDCoord) img_size) / 2.0f;

  rotate *= 2.0f * taMath_float::pi; // convert to radians
  float rot_sin = sin(rotate);
  float rot_cos = cos(rotate);

  bool wrap = (edge == WRAP);

  float_Matrix sc_ary(false);  sc_ary.SetGeom(2, 2, 2);

  int x,y;			// coords in new image
  for(y=0;y<img_size.y;y++) {
    for(x=0;x<img_size.x;x++) {
      float cx = ((float) x) - ctr.x;
      float cy = ((float) y) - ctr.y;

      float org_x = cx * rot_cos + cy * rot_sin + ctr.x;
      float org_y = cy * rot_cos - cx * rot_sin + ctr.y;

      int pcx[2];
      int pcy[2];
      float pwx[2];
      float pwy[2];

      GetWeightedPixels_float(org_x, img_size.x, pcx, pwx);
      GetWeightedPixels_float(org_y, img_size.y, pcy, pwy);

      int oxi, oyi;
      for(oyi=0;oyi<2;oyi++) {
	for(oxi=0;oxi<2;oxi++) {
	  sc_ary.FastEl(oxi, oyi) = pwx[oxi] + pwy[oyi];
	}
      }
      taMath_float::vec_norm_sum(&sc_ary);

      float r_avg = 0.0f;
      float g_avg = 0.0f;
      float b_avg = 0.0f;
      TwoDCoord ic;
      for(oyi=0;oyi<2;oyi++) {
	ic.y = pcy[oyi];
	for(oxi=0;oxi<2;oxi++) {
	  ic.x = pcx[oxi];
	  if(ic.WrapClip(wrap, img_size)) {
	    if(edge == CLIP) continue; // bail on clipping only
	  }
	  if(rgb_img) {
	    r_avg += sc_ary.FastEl(oxi, oyi) * orig_img.FastEl(ic.x, ic.y, 0); 
	    g_avg += sc_ary.FastEl(oxi, oyi) * orig_img.FastEl(ic.x, ic.y, 1);
	    b_avg += sc_ary.FastEl(oxi, oyi) * orig_img.FastEl(ic.x, ic.y, 2);
	  }
	  else {
	    r_avg += sc_ary.FastEl(oxi, oyi) * orig_img.SafeEl(ic.x, ic.y);
	  }
	}
      }
      if(rgb_img) {
	rotated_img.FastEl(x,y,0) = r_avg;
	rotated_img.FastEl(x,y,1) = g_avg;
	rotated_img.FastEl(x,y,2) = b_avg;
      }
      else {
	rotated_img.FastEl(x,y) = r_avg;
      }
    }
  }
  return true;
}

bool taImageProc::CropImage_float(float_Matrix& crop_img, float_Matrix& orig_img, 
				  int crop_width, int crop_height, EdgeMode edge) {
  TwoDCoord img_size(orig_img.dim(0), orig_img.dim(1)); 
  TwoDCoord crop_size(crop_width, crop_height);
  if(crop_size.x < 0) crop_size.x = img_size.x;
  if(crop_size.y < 0) crop_size.y = img_size.y;

  TwoDCoord img_ctr = img_size / 2;
  TwoDCoord crop_ctr = crop_size / 2;
  TwoDCoord img_off = img_ctr - crop_ctr; // offset for 0,0 pixel of cropped image, in orig_img
  
  bool rgb_img = false;
  if(orig_img.dims() == 3) { // rgb
    crop_img.SetGeom(3, crop_size.x, crop_size.y, 3);
    rgb_img = true;
  }
  else
    crop_img.SetGeom(2, crop_size.x, crop_size.y);

  TwoDCoord ic;
  for(int ny = 0; ny < crop_size.y; ny++) {
    ic.y = img_off.y + ny;
    for(int nx = 0; nx < crop_size.x; nx++) {
      ic.x = img_off.x + nx;

      if(ic.WrapClip(false, img_size)) { // always clip!
	if(edge == CLIP) continue; // bail on clipping only
      }

      if(rgb_img) {
	for(int i=0;i<3;i++)
	  crop_img.FastEl(nx, ny, i) = orig_img.FastEl(ic.x, ic.y, i);
      }
      else {
	crop_img.FastEl(nx, ny) = orig_img.FastEl(ic.x, ic.y);
      }
    }
  }
  return true;
}

bool taImageProc::TransformImage_float(float_Matrix& xformed_img, float_Matrix& orig_img,
				       float move_x, float move_y, float rotate,
				       float scale, int crop_width, int crop_height,
				       EdgeMode edge)
{
  float_Matrix* use_img = &orig_img;
  float_Matrix xlate_img(false);     	taBase::Ref(xlate_img);
  float_Matrix rot_img(false);		taBase::Ref(rot_img);
  float_Matrix sc_img(false);		taBase::Ref(sc_img);

  // render border after each xform to keep edges clean..
  if(edge == BORDER) taImageProc::RenderBorder_float(*use_img);
  if((move_x != 0.0f) || (move_y != 0.0f)) {
    taImageProc::TranslateImage_float(xlate_img, *use_img, move_x, move_y, edge);
    use_img = &xlate_img;
    if(edge == BORDER) taImageProc::RenderBorder_float(*use_img);
  }
  if(rotate != 0.0f) {
    taImageProc::RotateImage_float(rot_img, *use_img, rotate, edge);
    use_img = &rot_img;
    if(edge == BORDER) taImageProc::RenderBorder_float(*use_img);
  }
  if(scale != 1.0f) {
    taImageProc::RenderBorder_float(*use_img); // always borderize prior to scaling to avoid probs
    taImageProc::ScaleImage_float(sc_img, *use_img, scale, BORDER); // border!
    use_img = &sc_img;
    if(edge == BORDER) taImageProc::RenderBorder_float(*use_img);
  }
  if(crop_width < 0 && crop_height < 0) {
    xformed_img = *use_img;	// todo: this is somewhat inefficient
  }
  else {
    taImageProc::CropImage_float(xformed_img, *use_img, crop_width, crop_height, edge);
  }
  return true;
}

bool taImageProc::SampleImageWindow_float(float_Matrix& out_img, float_Matrix& in_img,
					  int win_width, int win_height,
					  float ctr_x, float ctr_y,
					  float rotate, float scale,
					  EdgeMode edge) {
  TwoDCoord img_size(in_img.dim(0), in_img.dim(1));
  TwoDCoord win_size(win_width, win_height);

  FloatTwoDCoord img_ctr(img_size.x * ctr_x, img_size.y * ctr_y);
  FloatTwoDCoord win_ctr = ((FloatTwoDCoord) win_size) / 2.0f;

  if(edge == BORDER) taImageProc::RenderBorder_float(in_img);

  rotate *= 2.0f * taMath_float::pi; // convert to radians
  float rot_sin = sin(rotate);
  float rot_cos = cos(rotate);

  bool rgb_img = false;
  if(in_img.dims() == 3) { // rgb
    out_img.SetGeom(3, win_size.x, win_size.y, 3);
    rgb_img = true;
  }
  else {
    out_img.SetGeom(2, win_size.x, win_size.y);
  }

  float half_sc;
  if(scale < 1.0f)
    half_sc = .5f / scale; // how much on each side of the pixel to include in avg
  else
    half_sc = .5f * scale;

  // scale > 1
  //   o o o    3
  //  /|X|X|\   connections
  // s s s s s  5

  int	half_int = (int)floor(half_sc);
  float extra = half_sc - (float)half_int;
  int n_orig_pix = half_int + 1; // number of pixels to get from original image for each scaled pixel
  //    int tot_org_pix = 2 * n_orig_pix + 1;
  float_Matrix sc_ary(false);
  sc_ary.SetGeom(2, n_orig_pix*2 + 1, n_orig_pix*2 + 1);
  int x, y;
  for(y=-n_orig_pix; y<= n_orig_pix; y++) {
    float ysc = extra;
    if((y >= -half_int) && (y <= half_int))
      ysc = 1.0f;
    for(x=-n_orig_pix; x<= n_orig_pix; x++) {
      float xsc = extra;
      if((x >= -half_int) && (x <= half_int))
	xsc = 1.0f;
      sc_ary.Set(ysc * xsc, x + n_orig_pix, y + n_orig_pix);
    }
  }

  taMath_float::vec_norm_sum(&sc_ary);		// make it add to 1

  bool wrap = (edge == WRAP);

  int pcx[2];    int pcy[2];
  float pwx[2];  float pwy[2];
  float_Matrix rot_ary(false);  rot_ary.SetGeom(2, 2, 2);

  TwoDCoord wc;
  for(wc.y=0; wc.y<win_size.y; wc.y++) {
    for(wc.x=0; wc.x<win_size.x; wc.x++) {
      FloatTwoDCoord wcd = ((FloatTwoDCoord)wc) - win_ctr; // delta from ctr in window
      FloatTwoDCoord icd = wcd / scale;			   // scaled delta from ctr in img
      FloatTwoDCoord icr(icd.x * rot_cos + icd.y * rot_sin, // rotated
			 icd.y * rot_cos - icd.x * rot_sin);
      TwoDCoord icc((int)floor(.5f + icr.x + img_ctr.x), // img center coord
		    (int)floor(.5f + icr.y + img_ctr.y));

      float r_avg = 0.0f;
      float g_avg = 0.0f;
      float b_avg = 0.0f;
      
      TwoDCoord oc;		// offsets
      for(oc.y=-n_orig_pix; oc.y<=n_orig_pix; oc.y++) {
	for(oc.x=-n_orig_pix;oc.x<=n_orig_pix;oc.x++) {
	  TwoDCoord ic = icc + oc;
	  if(ic.WrapClip(wrap, img_size)) {
	    if(edge == CLIP) continue; // bail on clipping only
	  }
	  float sc = sc_ary.FastEl(oc.x + n_orig_pix, oc.y + n_orig_pix);

	  if(rotate != 0.0f) {
	    // now, for each scaling fuzzy-sampled point, correct for rotational aliasing..
	    GetWeightedPixels_float(ic.x, img_size.x, pcx, pwx);
	    GetWeightedPixels_float(ic.y, img_size.y, pcy, pwy);

	    int oxi, oyi;
	    for(oyi=0;oyi<2;oyi++) {
	      for(oxi=0;oxi<2;oxi++) {
		rot_ary.FastEl(oxi, oyi) = pwx[oxi] + pwy[oyi];
	      }
	    }
	    taMath_float::vec_norm_sum(&rot_ary);

	    float r_r_avg = 0.0f;
	    float r_g_avg = 0.0f;
	    float r_b_avg = 0.0f;
	    TwoDCoord ric;
	    for(oyi=0;oyi<2;oyi++) {
	      ric.y = pcy[oyi];
	      for(oxi=0;oxi<2;oxi++) {
		ric.x = pcx[oxi];
		if(ric.WrapClip(wrap, img_size)) {
		  if(edge == CLIP) continue; // bail on clipping only
		}

		float rsc = rot_ary.FastEl(oxi, oyi);

		if(rgb_img) {
		  r_r_avg += rsc * in_img.FastEl(ric.x, ric.y, 0); 
		  r_g_avg += rsc * in_img.FastEl(ric.x, ric.y, 1);
		  r_b_avg += rsc * in_img.FastEl(ric.x, ric.y, 2);
		}
		else {
		  r_r_avg += rsc * in_img.SafeEl(ric.x, ric.y);
		}
	      }
	    }
	    if(rgb_img) {
	      r_avg += sc * r_r_avg;
	      g_avg += sc * r_g_avg;
	      b_avg += sc * r_b_avg;
	    }
	    else {
	      r_avg += sc * r_r_avg;
	    }
	  }
	  else {
	    if(rgb_img) {
	      r_avg += sc * in_img.FastEl(ic.x, ic.y, 0);
	      g_avg += sc * in_img.FastEl(ic.x, ic.y, 1);
	      b_avg += sc * in_img.FastEl(ic.x, ic.y, 2);
	    }
	    else {
	      r_avg += sc * in_img.FastEl(ic.x, ic.y);
	    }
	  }
	}
      }
      if(rgb_img) {
	out_img.FastEl(wc.x, wc.y, 0) = r_avg;
	out_img.FastEl(wc.x, wc.y, 1) = g_avg;
	out_img.FastEl(wc.x, wc.y, 2) = b_avg;
      }
      else {
	out_img.FastEl(wc.x, wc.y) = r_avg;
      }	
    }
  }
  return true;
}


bool taImageProc::AttentionFilter(float_Matrix& mat, float radius_pct) {
  TwoDCoord img_size(mat.dim(0), mat.dim(1));
  TwoDCoord img_ctr = img_size / 2;

  float max_radius = taMath_float::max(img_ctr.x, img_ctr.y);
  float scale_x = max_radius / (float)img_ctr.x;
  float scale_y = max_radius / (float)img_ctr.y;
  float radius = radius_pct * max_radius;
  float r_sq = radius * radius;

  for (int y = 0; y < img_size.y; y++) {
    for (int x = 0; x < img_size.x; x++) {
      float dist_x = scale_x * float(x - img_ctr.x);
      float dist_y = scale_y * float(y - img_ctr.y);
      float dist_sq = dist_x * dist_x + dist_y * dist_y;
      if (dist_sq > r_sq) {
	float mult = (float) r_sq / (float) dist_sq;
	mat.FastEl(x,y) *= mult;
      }
    }
  }
  int idx;
  float max_v = taMath_float::vec_max(&mat, idx);
  if(max_v > .01f)
    taMath_float::vec_norm_max(&mat);
  return true;
}


bool taImageProc::BlobBlurOcclude(float_Matrix& img, float gauss_sig, float pct_occlude) {
  TwoDCoord img_size(img.dim(0), img.dim(1));

  float gauss_eff = gauss_sig * (float)img_size.x;
  int gauss_half = (int)gauss_eff * 4;
  int gauss_wd = gauss_half * 2;
  TwoDCoord ntot = (img_size / gauss_half) + 1;
  int totblob = ntot.Product();
  int nblob = 2 * (int)(pct_occlude * (float)totblob + 0.5f);

  float_Matrix gauss_wt;
  gauss_wt.SetGeom(2, gauss_wd, gauss_wd);
  float ctr = (float)(gauss_wd-1) * .5f;
  float ctr_y = (float)(gauss_wd-1) * .5f;
  for(int yi=0; yi< gauss_wd; yi++) {
    float y = ((float)yi - ctr_y) / gauss_eff;
    for(int xi=0; xi< gauss_wd; xi++) {
      float x = ((float)xi - ctr_y) / gauss_eff;
      float gv = expf(-(x*x + y*y)/2.0f);
      gauss_wt.FastEl(xi, yi) = gv;
    }
  }
  float_Matrix gauss_cnv;
  gauss_cnv.CopyFrom(&gauss_wt);
  taMath_float::vec_norm_max(&gauss_wt, 2.71828f); // max to e
  taMath_float::vec_threshold_high(&gauss_wt, 1.0f, 1.0f); // flatten out top

  taMath_float::vec_norm_sum(&gauss_cnv, 1.0f);

  bool rgb_img = false;
  if(img.dims() == 3) { // rgb
    rgb_img = true;
  }

  TwoDCoord ic_min(gauss_half, gauss_half);
  TwoDCoord ic_max(img_size.x - gauss_half, img_size.y - gauss_half);

  TwoDCoord ic;
  for(int blob=0; blob < nblob; blob++) {
    ic.x = Random::IntMinMax(ic_min.x, ic_max.x);
    ic.y = Random::IntMinMax(ic_min.y, ic_max.y);

    if(rgb_img) {
    }
    else {
      float avg = 0.0f;
      for(int yi=0; yi< gauss_wd; yi++) {
	for(int xi=0; xi< gauss_wd; xi++) {
	  float iv = img.FastEl(ic.x + xi - gauss_half, ic.y + yi - gauss_half);
	  avg += gauss_cnv.FastEl(xi, yi) * iv;
	}
      }

      for(int yi=0; yi< gauss_wd; yi++) {
	for(int xi=0; xi< gauss_wd; xi++) {
	  float& iv = img.FastEl(ic.x + xi - gauss_half, ic.y + yi - gauss_half);
	  float wt = gauss_wt.FastEl(xi, yi);
	  float nw_iv = (1.0f - wt) * iv + wt * avg;
	  iv = nw_iv;
	}
      }
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//		Retinal Processing (DoG model)

// for thread function calling:
typedef void (DoGRegionSpec::*DoGRegionMethod)(int, int);

void DoGRegionSpec::Initialize() {
  save_mode = FIRST_ROW;
  image_save = (DataSave)(SAVE_DATA | ONLY_GUI);
  ocularity = MONOCULAR;
  region = FOVEA;
  res = HI_RES;
  color = COLOR;
  motion_frames = 0;
  retina_size = 144;
  border = 0;
  input_size = 144;
  edge_mode= WRAP;
  dog_specs.filter_width = 4;
  dog_specs.on_sigma = 1;
  dog_specs.off_sigma = 2;
  dog_specs.circle_edge = 2;
  dog_spacing = 1;
  dog_renorm = LOG_RENORM;
  renorm_thr = 1.0e-5f;
  dog_save = SAVE_DATA;
  dog_feat_geom.x = 2;
  dog_feat_geom.y = 3;
  dog_feat_geom.n = 6;
  dog_img_geom = 320;
  
  cur_img_r = NULL;
  cur_img_l = NULL;
  cur_img = NULL;
  cur_out = NULL;
  cur_circ = NULL;
  rgb_img = false;
  wrap = false;
}

void DoGRegionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dog_specs.name = name;
  dog_specs.UpdateAfterEdit_NoGui();
  dog_spacing.x = MAX(dog_spacing.x, 1);
  dog_spacing.y = MAX(dog_spacing.y, 1);
  motion_frames = MAX(motion_frames, 0);
  UpdateGeom();
}

void DoGRegionSpec::UpdateGeom() {
  // 0 1 2 3 4 5 6 7 8 	retina_size = 9
  // b b .   .   . b b 	border = 2, spacing = 2: input_size = 5, output_size = 3

  // wrap mode:
  // 0 1 2 3 4 5 6 7  retina_size = 8
  // .   .   .   .    border = 0, spacing = 2; input_size = 8, output_size = 4

  input_size = (retina_size - 2 * border);
  if(edge_mode == WRAP) {
    dog_img_geom = input_size / dog_spacing;
  }
  else {
    dog_img_geom = ((input_size - 1) / dog_spacing) + 1;
  }
  if(color == COLOR) {
    dog_feat_geom.SetXYN(2,4,8);
  }
  else {
    dog_feat_geom.SetXYN(2,1,2);
  }
}

String DoGRegionSpec::GetDoGFiltName(int flt_no) {
  String nm;
  switch(flt_no) {
  case 0: nm = "on"; break;
  case 1: nm = "off"; break;
  case 2: nm = "rvc"; break;
  case 3: nm = "cvr"; break;
  case 4: nm = "gvm"; break;
  case 5: nm = "mvg"; break;
  case 6: nm = "bvy"; break;
  case 7: nm = "yvb"; break;
  }
  return nm;
}

bool DoGRegionSpec::Init() {
  bool rval = InitFilters();
  rval &= InitOutMatrix();
  rval &= InitDataTable();
  return rval;
}

bool DoGRegionSpec::InitFilters() {
  UpdateGeom();
  dog_specs.UpdateFilter();
  return true;
}

bool DoGRegionSpec::InitOutMatrix() {
  dog_circ_r.matrix = &dog_out_r;
  dog_circ_l.matrix = &dog_out_l;

  if(motion_frames <= 1) {
    dog_out_r.SetGeom(4, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
    if(ocularity == BINOCULAR)
      dog_out_l.SetGeom(4, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
    else
      dog_out_l.SetGeom(1,1);	// free memory
  }
  else {
    dog_out_r.SetGeom(5, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y,
		      motion_frames);
    if(ocularity == BINOCULAR)
      dog_out_l.SetGeom(5, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y,
			motion_frames);
    else
      dog_out_l.SetGeom(1,1);	// free memory
  }
  dog_circ_r.Reset();
  dog_circ_l.Reset();

  return true;
}

////////////////////////////////////////////////////////////////////
//	DoGRegion 	Filtering

bool DoGRegionSpec::FilterImage(float_Matrix* right_eye_image, float_Matrix* left_eye_image) {
  // this is not typically overwritten -- just all the checks -- see _impl
  if(TestWarning((dog_out_r.dims() < 4) ||
		 (dog_out_r.dim(0) * dog_out_r.dim(1) != dog_feat_geom.n) ||
		 (dog_out_r.dim(2) != dog_img_geom.x) ||
		 (dog_out_r.dim(3) != dog_img_geom.y),
		 "FilterImage", "not properly initialized to current geom -- running Init now")) {
    Init();
  }  

  if(TestError(!right_eye_image, "FilterIMage", "right_eye_image is NULL -- must pass image"))
    return false;

  if(TestError((right_eye_image->dim(0) != retina_size.x) ||
	       (right_eye_image->dim(1) != retina_size.y),
	       "FilterImage", "right_eye_image is not appropriate size -- must be same as retina_size!"))
    return false;

  if(ocularity == BINOCULAR) {
    if(TestWarning((dog_out_l.dims() < 4) ||
		   (dog_out_l.dim(0) * dog_out_l.dim(1) != dog_feat_geom.n) ||
		   (dog_out_l.dim(2) != dog_img_geom.x) ||
		   (dog_out_l.dim(3) != dog_img_geom.y),
		   "FilterImage", "not properly initialized to current geom -- running Init now")) {
      Init();
    }  

    if(TestError(!left_eye_image, "FilterIMage", "left_eye_image is NULL -- must pass image"))
      return false;

    if(TestError((left_eye_image->dim(0) != retina_size.x) ||
		 (left_eye_image->dim(1) != retina_size.y),
		 "FilterImage", "left_eye_image is not appropriate size -- must be same as retina_size!"))
      return false;

    cur_img_l = left_eye_image;
  }

  cur_img_r = right_eye_image;

  bool rval = FilterImage_impl();

  cur_img_r = NULL;
  cur_img_l = NULL;

  return rval;
}

void DoGRegionSpec::IncrTime() {
  if(motion_frames <= 1) {
    return;
  }
  else {
    dog_circ_r.CircAddLimit(motion_frames);
    if(ocularity == BINOCULAR) {
      dog_circ_l.CircAddLimit(motion_frames);
    }
  }
}

bool DoGRegionSpec::FilterImage_impl() {
  IncrTime();
  bool rval = DoGFilterImage(cur_img_r, &dog_out_r, &dog_circ_r);
  if(rval && ocularity == BINOCULAR) {
    rval &= DoGFilterImage(cur_img_l, &dog_out_l, &dog_circ_l);
  }

  if(!data_table || save_mode == NONE_SAVE) // bail now
    return rval;

  if(save_mode == FIRST_ROW) {
    data_table->EnforceRows(1);
    data_table->WriteItem(0);
    data_table->ReadItem(0);
  }
  else {
    data_table->AddBlankRow();
  }

  if(image_save & SAVE_DATA && !(!taMisc::gui_active && image_save & ONLY_GUI)) {
    ImageToTable(data_table, cur_img_r, cur_img_l);
  }
  if(dog_save & SAVE_DATA && !(!taMisc::gui_active && dog_save & ONLY_GUI)) {
    DoGOutputToTable(data_table);
  }

  return rval;
}

bool DoGRegionSpec::DoGFilterImage(float_Matrix* image, float_Matrix* out, CircMatrix* circ) {
  cur_img = image;
  cur_out = out;
  cur_circ = circ;
  rgb_img = (cur_img->dims() == 3);
  wrap = (edge_mode == WRAP);

  int n_run = dog_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  ThreadImgProcCall ip_call((ThreadImgProcMethod)(DoGRegionMethod)&DoGRegionSpec::DoGFilterImage_thread);
  threads.Run(&ip_call, n_run);

  // renormalize -- todo: could thread this perhaps, but chunk size would have to be larger probably
  if(dog_renorm != NO_RENORM) {
    RenormOutput_Frames(dog_renorm, out, circ);
  }

  return true;
}

void DoGRegionSpec::DoGFilterImage_thread(int dog_idx, int thread_no) {
  TwoDCoord dc;			// dog coords
  dc.SetFmIndex(dog_idx, dog_img_geom.x);
  TwoDCoord icc = border + dog_spacing * dc; // image coords center

  int mot_idx = cur_circ->CircIdx_Last(); // always write to last position

  // x = on/off, y = color channel
  TwoDCoord ic;		// image coord
  for(int chan = 0; chan < dog_feat_geom.y; chan++) { 
    DoGFilter::ColorChannel cchan = (DoGFilter::ColorChannel)chan;

    float cnv_sum = 0.0f;		// convolution sum
    if(chan == 0 || rgb_img) {		// only rgb images if chan > 0
      for(int yf = -dog_specs.filter_width; yf <= dog_specs.filter_width; yf++) {
	for(int xf = -dog_specs.filter_width; xf <= dog_specs.filter_width; xf++) {
	  ic.y = icc.y + yf;
	  ic.x = icc.x + xf;
	  if(ic.WrapClip(wrap, retina_size)) {
	    if(edge_mode == CLIP) continue; // bail on clipping only
	  }
	  if(rgb_img) {
	    cnv_sum += dog_specs.FilterPoint_rgb(xf, yf, cchan, cur_img->FastEl(ic.x, ic.y, 0),
						 cur_img->FastEl(ic.x, ic.y, 1),
						 cur_img->FastEl(ic.x, ic.y, 2));
	  }
	  else {
	    cnv_sum += dog_specs.FilterPoint_grey(xf, yf, cur_img->FastEl(ic.x, ic.y));
	  }
	}
      }
    }
    if(cnv_sum >= 0.0f) {
      MatMotEl(cur_out, 0, chan, dc.x, dc.y, mot_idx) = cnv_sum; // feat x = 0 = on
      MatMotEl(cur_out, 1, chan, dc.x, dc.y, mot_idx) = 0.0f; 	// feat x = 1 = off
    }
    else {
      MatMotEl(cur_out, 0, chan, dc.x, dc.y, mot_idx) = 0.0f; 	// feat x = 0 = on
      MatMotEl(cur_out, 1, chan, dc.x, dc.y, mot_idx) = -cnv_sum; // feat x = 1 = off
    }
  }
}

bool DoGRegionSpec::RenormOutput_Frames(RenormMode mode, float_Matrix* out, CircMatrix* circ) {
  bool rval = false;
  float_Matrix* mat = out;
  if(motion_frames > 1) {
    mat = (float_Matrix*)out->GetFrameSlice(circ->CircIdx_Last());
    taBase::Ref(mat);
  }
  int idx;
  float max_val = taMath_float::vec_max(mat, idx);
  if(max_val > renorm_thr) {
    rval = true;
    if(mode == LIN_RENORM) {
      taMath_float::vec_mult_scalar(mat, 1.0f / max_val);
    }
    else if(mode == LOG_RENORM) {
      float rescale = 1.0f / logf(1.0f + max_val);
      for(int j=0;j<mat->size;j++) {
	float& vl = mat->FastEl_Flat(j);
	vl = logf(1.0f + vl) * rescale;
      }
    }
  }
  if(motion_frames > 1) {
    taBase::unRefDone(mat);
  }
  return rval;
}

bool DoGRegionSpec::RenormOutput_NoFrames(RenormMode mode, float_Matrix* mat) {
  bool rval = false;
  int idx;
  float max_val = taMath_float::vec_max(mat, idx);
  if(max_val > renorm_thr) {
    rval = true;
    if(mode == LIN_RENORM) {
      taMath_float::vec_mult_scalar(mat, 1.0f / max_val);
    }
    else if(mode == LOG_RENORM) {
      float rescale = 1.0f / logf(1.0f + max_val);
      for(int j=0;j<mat->size;j++) {
	float& vl = mat->FastEl_Flat(j);
	vl = logf(1.0f + vl) * rescale;
      }
    }
  }
  return rval;
}

bool DoGRegionSpec::InvertFilters() {
  // this is not typically overwritten -- just all the checks -- see _impl
  if(TestWarning((!data_table || save_mode == NONE_SAVE),
		 "InvertFilters", "must have a data table and save mode != NONE_SAVE")) {
    return false;
  }  

  if(TestWarning(!(image_save & SAVE_DATA),
		 "InvertFilters", "must have image_save == SAVE_DATA -- uses saved images")) {
    image_save = (DataSave)(image_save | SAVE_DATA);
    Init();			// init!
  }  

  if(TestWarning((dog_out_r.dims() < 4) ||
		 (dog_out_r.dim(0) * dog_out_r.dim(1) != dog_feat_geom.n) ||
		 (dog_out_r.dim(2) != dog_img_geom.x) ||
		 (dog_out_r.dim(3) != dog_img_geom.y),
		 "InvertFilters", "not properly initialized to current geom -- running Init now")) {
    Init();
  }  

  if(ocularity == BINOCULAR) {
    if(TestWarning((dog_out_l.dims() < 4) ||
		   (dog_out_l.dim(0) * dog_out_l.dim(1) != dog_feat_geom.n) ||
		   (dog_out_l.dim(2) != dog_img_geom.x) ||
		   (dog_out_l.dim(3) != dog_img_geom.y),
		   "InvertFilters", "not properly initialized to current geom -- running Init now")) {
      Init();
    }  
  }
  
  bool rval = InvertFilters_impl();
  return rval;
}

bool DoGRegionSpec::InvertFilters_impl() {
  if(save_mode == FIRST_ROW) {
    data_table->EnforceRows(1);
    data_table->WriteItem(0);
    data_table->ReadItem(0);
  }
  else {
    data_table->AddBlankRow();
  }

  bool rval = DoGInvertFilter(&dog_out_r, &dog_circ_r, "_r");
  if(rval && ocularity == BINOCULAR) {
    rval &= DoGInvertFilter(&dog_out_l, &dog_circ_l, "_l");
  }

  if(dog_save & SAVE_DATA && !(!taMisc::gui_active && dog_save & ONLY_GUI)) {
    DoGOutputToTable(data_table);
  }

  return rval;
}

bool DoGRegionSpec::DoGInvertFilter(float_Matrix* out, CircMatrix* circ, const String& col_sufx) {
  DataCol* col;
  int idx;
  if(color == COLOR)
    col = data_table->FindMakeColName(name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 3,
				      retina_size.x, retina_size.y, 3);
  else
    col = data_table->FindMakeColName(name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 2,
				      retina_size.x, retina_size.y);

  float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);

  ret_img->InitVals(0.0f);

  int mot_idx = circ->CircIdx_Last(); // always write to last position
  TwoDCoord dc;			// dog coords
  for(dc.y=0; dc.y < dog_img_geom.y; dc.y++) {
    for(dc.x=0; dc.x < dog_img_geom.x; dc.x++) {
      TwoDCoord icc = border + dog_spacing * dc; // image coords center
      // x = on/off, y = color channel
      TwoDCoord ic;		// image coord
      for(int chan = 0; chan < dog_feat_geom.y; chan++) { 
	DoGFilter::ColorChannel cchan = (DoGFilter::ColorChannel)chan;

	float on_act = MatMotEl(out, 0, chan, dc.x, dc.y, mot_idx); // feat x = 0 = on
	float off_act = MatMotEl(out, 1, chan, dc.x, dc.y, mot_idx);
	float net_act = on_act - off_act;

	if(chan == 0 || rgb_img) {		// only rgb images if chan > 0
	  for(int yf = -dog_specs.filter_width; yf <= dog_specs.filter_width; yf++) {
	    for(int xf = -dog_specs.filter_width; xf <= dog_specs.filter_width; xf++) {
	      ic.y = icc.y + yf;
	      ic.x = icc.x + xf;
	      if(ic.WrapClip(wrap, retina_size)) {
		if(edge_mode == CLIP) continue; // bail on clipping only
	      }
	      if(rgb_img) {
		float r,g,b;
		dog_specs.InvertFilterPoint_rgb(xf, yf, net_act, cchan, r,g,b);
		cur_img->FastEl(ic.x, ic.y, 0) += r;
		cur_img->FastEl(ic.x, ic.y, 1) += g;
		cur_img->FastEl(ic.x, ic.y, 2) += b;
	      }
	      else {
		float grey = dog_specs.InvertFilterPoint_grey(xf, yf, net_act);
		ret_img->FastEl(ic.x, ic.y) += grey;
	      }
	    }
	  }
	}
      }
    }
  }

  taMath_float::vec_norm_max(ret_img);

  return true;
}


////////////////////////////////////////////////////////////////////
//	DoGRegion 	Data Table Output

bool DoGRegionSpec::InitDataTable() {
  if(!data_table) {
    if(!(image_save & SAVE_DATA) && !(dog_save & SAVE_DATA)) return true;
    return false;
  }
  int idx;
  if(image_save & SAVE_DATA) {
    DataCol* col;
    String sufx = "_r";

    if(color == COLOR)
      col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 3,
					retina_size.x, retina_size.y, 3);
    else
      col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 2,
					retina_size.x, retina_size.y);
    col->SetUserData("IMAGE", true);

    if(ocularity == BINOCULAR) {
      sufx = "_l";
      if(color == COLOR)
	col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 3,
					  retina_size.x, retina_size.y, 3);
      else
	col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 2,
					  retina_size.x, retina_size.y);
      col->SetUserData("IMAGE", true);
    }
  }

  if(dog_save & SAVE_DATA) {
    if(dog_save & SEP_MATRIX) {
      for(int i=0;i<dog_feat_geom.n;i++) {
	String nm = name + "_" + GetDoGFiltName(i) + "_dog";
	data_table->FindMakeColName(nm+ "_r", idx, DataTable::VT_FLOAT, 2,
				    dog_img_geom.x, dog_img_geom.y);
	if(ocularity == BINOCULAR) {
	  data_table->FindMakeColName(nm+ "_l", idx, DataTable::VT_FLOAT, 2,
				      dog_img_geom.x, dog_img_geom.y);
	}
      }
    }
    else {
      data_table->FindMakeColName(name + "_dog_r", idx, DataTable::VT_FLOAT, 4,
				  dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
      if(ocularity == BINOCULAR) {
	data_table->FindMakeColName(name + "_dog_l", idx, DataTable::VT_FLOAT, 4,
				    dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
      }
    }
  }
  return true;
}

bool DoGRegionSpec::ImageToTable(DataTable* dtab, float_Matrix* right_eye_image,
				 float_Matrix* left_eye_image) {
  ImageToTable_impl(dtab, right_eye_image, "_r");
  if(ocularity == BINOCULAR) 
    ImageToTable_impl(dtab, left_eye_image, "_l");
  return true;
}

bool DoGRegionSpec::ImageToTable_impl(DataTable* dtab, float_Matrix* img,
				      const String& col_sufx) {
  DataCol* col;
  int idx;
  if(color == COLOR)
    col = data_table->FindMakeColName(name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 3,
				      retina_size.x, retina_size.y, 3);
  else
    col = data_table->FindMakeColName(name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 2,
				      retina_size.x, retina_size.y);

  float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);
  ret_img->CopyFrom(img);
  return true;
}

bool DoGRegionSpec::DoGOutputToTable(DataTable* dtab) {
  DoGOutputToTable_impl(dtab, &dog_out_r, &dog_circ_r, "_r");
  if(ocularity == BINOCULAR) 
    DoGOutputToTable_impl(dtab, &dog_out_l, &dog_circ_l, "_l");
  return true;
}

bool DoGRegionSpec::DoGOutputToTable_impl(DataTable* dtab, float_Matrix* out, CircMatrix* circ,
					  const String& col_sufx) {
  DataCol* col;
  int idx;
  if(dog_save & SEP_MATRIX) {
    TwoDCoord dc;		// dog coords
    TwoDCoord fc;		// feature coords
    for(int i=0;i<dog_feat_geom.n;i++) {
      fc.SetFmIndex(i, dog_feat_geom.x);
      String nm = name + "_" + GetDoGFiltName(i) + "_dog";
      col = data_table->FindMakeColName(nm+ col_sufx, idx, DataTable::VT_FLOAT, 2,
				  dog_img_geom.x, dog_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      int mot_idx = circ->CircIdx_Last();
      for(dc.y = 0; dc.y < dog_img_geom.y; dc.y++) {
	for(dc.x = 0; dc.x < dog_img_geom.x; dc.x++) {
	  float val = MatMotEl(out, fc.x, fc.y, dc.x, dc.y, mot_idx);
	  dout->FastEl(dc.x, dc.y) = val;
	}
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_dog" + col_sufx, idx, DataTable::VT_FLOAT, 4,
				      dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    if(motion_frames <= 1) {
      dout->CopyFrom(out);
    }
    else {
      float_MatrixPtr lstfrm; lstfrm = (float_Matrix*)out->GetFrameSlice(circ->CircIdx_Last());
      dout->CopyFrom(lstfrm);
    }
  }
  return true;
}

void DoGRegionSpec::GraphDoGFilter(DataTable* graph_data) {
  dog_specs.GraphFilter(graph_data);
}

void DoGRegionSpec::GridDoGFilter(DataTable* graph_data) {
  dog_specs.GridFilter(graph_data);
}

void DoGRegionSpec::PlotSpacing(DataTable* graph_data, bool reset) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_DoG_PlotSpacing", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->Reset();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
  DataCol* matda = graph_data->FindMakeColName("Spacing", idx, VT_FLOAT, 2,
					      retina_size.x, retina_size.y);
  graph_data->SetUserData("N_ROWS", 1);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);
  graph_data->SetUserData("BLOCK_SPACE", 20.0f);
  graph_data->SetUserData("WIDTH", 1.0f + (float)retina_size.x / (float)retina_size.y);

  graph_data->AddBlankRow();
  nmda->SetValAsString("DoG", -1);
  float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
  if(mat) {
    TwoDCoord ic;
    int x,y;
    for(y=border.y; y<= retina_size.y-border.y; y+= dog_spacing.y) {
      for(x=border.x; x<= retina_size.x-border.x; x+=dog_spacing.x) {
	ic.y = y; ic.x = x;
	ic.WrapClip(true, retina_size);	mat->FastEl(ic.x,ic.y) = 1.0f;
      }
    }
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

DoGRegionSpec* DoGRegionSpecList::FindRetinalRegion(DoGRegionSpec::Region reg) {
  for(int i=0;i<size;i++) {
    DoGRegionSpec* fs = (DoGRegionSpec*)FastEl(i);
    if(fs->region == reg)
      return fs;
  }
  return NULL;
}

DoGRegionSpec* DoGRegionSpecList::FindRetinalRes(DoGRegionSpec::Resolution res) {
  for(int i=0;i<size;i++) {
    DoGRegionSpec* fs = (DoGRegionSpec*)FastEl(i);
    if(fs->res == res)
      return fs;
  }
  return NULL;
}

DoGRegionSpec* DoGRegionSpecList::FindRetinalRegionRes(DoGRegionSpec::Region reg,
						       DoGRegionSpec::Resolution res) {
  for(int i=0;i<size;i++) {
    DoGRegionSpec* fs = (DoGRegionSpec*)FastEl(i);
    if((fs->region == reg) && (fs->res == res))
      return fs;
  }
  DoGRegionSpec* rval = FindRetinalRes(res);
  if(rval) return rval;
  rval = FindRetinalRegion(reg);
  if(rval) return rval;
  return NULL;
}

////////////////////////////////////////////////////////////////////
//		V1 Processing -- basic RF's

///////////////////////////////////////////////////////////
// 		V1 KWTA

void V1KwtaSpec::Initialize() {
  on = false;
  raw_pct = 1.0f;
  gp_k = 1;
  gp_g = 0.1f;
  loc_g = 0.5f;
  loc_sz = 4;
  kwta_pt = 0.6f;
  gain = 600.0f;
  nvar = 0.01f;
  g_bar_e = 0.5f;
  g_bar_l = 0.1f;
  e_rev_e = 1.0f;
  e_rev_l = 0.15f;
  thr = 0.25f;

  noise_conv.x_range.min = -.05f;
  noise_conv.x_range.max = .05f;
  noise_conv.res = .001f;
  noise_conv.UpdateAfterEdit_NoGui();

  nxx1_fun.x_range.min = -.03f;
  nxx1_fun.x_range.max = .20f;
  nxx1_fun.res = .001f;
  nxx1_fun.UpdateAfterEdit_NoGui();

  raw_pct_c = 1.0f - raw_pct;
  gber_l = g_bar_l * e_rev_l;
  e_rev_sub_thr_e = e_rev_e - thr;
  gbl_e_rev_sub_thr_l = g_bar_l * (e_rev_l - thr);
  thr_sub_e_rev_i = thr - e_rev_l;
  loc_half_sz = loc_sz / 2;

  CreateNXX1Fun();
}

void V1KwtaSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  raw_pct_c = 1.0f - raw_pct;
  gber_l = g_bar_l * e_rev_l;
  e_rev_sub_thr_e = e_rev_e - thr;
  gbl_e_rev_sub_thr_l = g_bar_l * (e_rev_l - thr);
  thr_sub_e_rev_i = thr - e_rev_l;
  loc_half_sz = loc_sz / 2;

  CreateNXX1Fun();
}

void V1KwtaSpec::CreateNXX1Fun() {
  // first create the gaussian noise convolver
  noise_conv.AllocForRange();
  int i;
  float var = nvar * nvar;
  for(i=0; i < noise_conv.size; i++) {
    float x = noise_conv.Xval(i);
    noise_conv[i] = expf(-((x * x) / var));
  }

  // normalize it
  float sum = 0.0f;
  for(i=0; i < noise_conv.size; i++)
    sum += noise_conv[i];
  for(i=0; i < noise_conv.size; i++)
    noise_conv[i] /= sum;

  // then create the initial function
  FunLookup fun;
  fun.x_range.min = nxx1_fun.x_range.min + noise_conv.x_range.min;
  fun.x_range.max = nxx1_fun.x_range.max + noise_conv.x_range.max;
  fun.res = nxx1_fun.res;
  fun.UpdateAfterEdit_NoGui();
  fun.AllocForRange();

  for(i=0; i<fun.size; i++) {
    float x = fun.Xval(i);
    float val = 0.0f;
    if(x > 0.0f)
      val = (gain * x) / ((gain * x) + 1.0f);
    fun[i] = val;
  }

  nxx1_fun.Convolve(fun, noise_conv);
}

void V1KwtaSpec::Compute_Inhib(float_Matrix& inputs, float_Matrix& gc_i_mat) {
  int gxs = inputs.dim(0);
  int gys = inputs.dim(1);
  int ixs = inputs.dim(2);
  int iys = inputs.dim(3);
  float_Matrix gpmat;
  gpmat.SetGeom(2, gxs, gys);
  gc_i_mat.SetGeom(2, ixs, iys);
  float max_gi = 0.0f;
  for(int iy=0; iy < iys; iy++) {
    for(int ix=0; ix < ixs; ix++) {
      for(int gy=0; gy < gys; gy++) {
	for(int gx=0; gx < gxs; gx++) {
	  gpmat.FastEl(gx, gy) = Compute_IThresh(g_bar_e * inputs.FastEl(gx, gy, ix, iy));
	}
      }
      float top_k_avg, bot_k_avg;
      taMath_float::vec_kwta_avg(top_k_avg, bot_k_avg, &gpmat, gp_k, true);
      float nw_gi = bot_k_avg + kwta_pt * (top_k_avg - bot_k_avg);
      gc_i_mat.FastEl(ix, iy) = nw_gi;
      max_gi = MAX(max_gi, nw_gi);
    }
  }
  if(gp_g > 0.0f) {
    float gpg_eff = gp_g * max_gi;
    for(int iy=0; iy < iys; iy++) {
      for(int ix=0; ix < ixs; ix++) {
	float gi = gc_i_mat.FastEl(ix, iy);
	gi = MAX(gi, gpg_eff);
	gc_i_mat.FastEl(ix, iy) = gi;
      }
    }
  }
}

void V1KwtaSpec::Compute_Inhib_Loc(float_Matrix& gc_i_mat, float_Matrix& gc_i_tmp_mat,
				   bool wrap) {
  TwoDCoord is(gc_i_mat.dim(0), gc_i_mat.dim(1));

  int bord;
  TwoDCoord ns;
  if(wrap) {
    bord = 0;
    ns = is / loc_half_sz;
  }
  else {
    bord = loc_half_sz; 
    ns = (((is - 2 * bord)-1) / loc_half_sz) + 1;
  }

  gc_i_tmp_mat.SetGeom(2, ns.x, ns.y);

  TwoDCoord nc;
  TwoDCoord ic;
  TwoDCoord ics;
  TwoDCoord gc;
  TwoDCoord icc;
  for(nc.y=0; nc.y < ns.y; nc.y++) {
    for(nc.x=0; nc.x < ns.x; nc.x++) {
      ic = nc * loc_half_sz + bord;
      ics = ic - loc_half_sz;	// starting point
      float max_i = 0.0f;
      for(gc.y=0; gc.y < loc_sz; gc.y++) {
	for(gc.x=0; gc.x < loc_sz; gc.x++) {
	  icc = ics + gc;
	  if(icc.WrapClip(wrap, is)) {
	    if(!wrap) continue; // bail on clipping only -- shouldn't happen!
	  }
	  float gi = gc_i_mat.FastEl(icc.x, icc.y);
	  max_i = MAX(max_i, gi);
	}
      }
      gc_i_tmp_mat.FastEl(nc.x, nc.y) = max_i;
    }
  }

  // because of the half-overlap, this must be done in 2 stages..
  for(nc.y=0; nc.y < ns.y; nc.y++) {
    for(nc.x=0; nc.x < ns.x; nc.x++) {
      ic = nc * loc_half_sz + bord;
      ics = ic - loc_half_sz;	// starting point
      float max_i = gc_i_tmp_mat.FastEl(nc.x, nc.y);
      float loci = loc_g * max_i;
      for(gc.y=0; gc.y < loc_sz; gc.y++) {
	for(gc.x=0; gc.x < loc_sz; gc.x++) {
	  icc = ics + gc;
	  if(icc.WrapClip(wrap, is)) {
	    if(!wrap) continue; // bail on clipping only -- shouldn't happen!
	  }
	  float& gi = gc_i_mat.FastEl(icc.x, icc.y);
	  gi = MAX(loci, gi);
	}
      }
    }
  }
}

bool V1KwtaSpec::Compute_Kwta(float_Matrix& inputs, float_Matrix& outputs,
			      float_Matrix& gc_i_mat, float_Matrix& gc_i_tmp_mat, bool wrap) {
  if(TestError(inputs.dims() != 4, "Compute_Kwta",
	       "input matrix must have 4 dimensions: gp x,y, outer (image) x,y"))
    return false;

  Compute_Inhib(inputs, gc_i_mat);

  if(loc_g > 0.0f)
    Compute_Inhib_Loc(gc_i_mat, gc_i_tmp_mat, wrap);

  int gxs = inputs.dim(0);
  int gys = inputs.dim(1);
  int ixs = inputs.dim(2);
  int iys = inputs.dim(3);

  for(int iy=0; iy < iys; iy++) {
    for(int ix=0; ix < ixs; ix++) {
      float gi = gc_i_mat.FastEl(ix, iy);
      for(int gy=0; gy < gys; gy++) {
	for(int gx=0; gx < gxs; gx++) {
	  float raw = inputs.FastEl(gx, gy, ix, iy);
	  float ge = g_bar_e * raw;
	  float act = Compute_ActFmIn(ge, gi);
	  if(act > raw)
	    act = raw_pct * raw + raw_pct_c * act; // blendy
	  outputs.FastEl(gx, gy, ix, iy) = act; 
	}
      }
    }
  }
  return true;
}


///////////////////////////////////////////////////////////
// 		Basic Specs

void V1SimpleSpec::Initialize() {
  n_angles = 4;
  rf_size = 4;
  rf_half = rf_size / 2;
  spacing = rf_half;
  border = 2;
  rf_norm = 1.0f / rf_size;
}

void V1SGaborSpec::Initialize() {
  use = false;
  freq = 1.5f;
  length = 2.0f;
  width = 2.0f;
  amp = 0.2f;
}

void V1SimpleSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  n_angles = 4;			// clamped!
  rf_size = 4;
  rf_half = rf_size / 2;
  if(spacing != 4 && spacing != 2) // must be one of these two
    spacing = 2;
  rf_norm = 1.0f / rf_size;
}

void V1MotionSpec::Initialize() {
  n_speeds = 1;
  tuning_width = 1;
  gauss_sig = 0.8f;
  
  tot_width = 1 + 2 * tuning_width;
}

void V1MotionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_width = 1 + 2 * tuning_width;
}

void V1BinocularSpec::Initialize() {
  n_disps = 1;
  disp_off = 4;
  tuning_width = 1;
  end_width = 4;
  gauss_sig = 0.7f;
  opt_thr = 0.01f;

  tot_disps = 1 + 2 * n_disps;
  max_width = 1 + 2 * tuning_width + end_width;
}

void V1BinocularSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_disps = 1 + 2 * n_disps;
  max_width = 1 + 2 * tuning_width + end_width;
}

void V1DispGroupSpec::Initialize() {
  dgp_iters = 10;
  disp_gain = 0.2f;
  v1b_mix = 0.5f;
  dgp_rf_sz.SetXY(10,4);
  gauss_sig = 1.0f;

  dgp_rf_half_sz = dgp_rf_sz / 2;
  dgp_border = 0;
  dgp_rf_norm = 1.0f;
}

void V1DispGroupSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dgp_rf_half_sz = dgp_rf_sz / 2;
}

void V1ComplexSpec::Initialize() {
  end_stop_len = 2;
  len_sum_len = 1;
  gauss_sig = 0.8f;
  nonfocal_wt = 0.5f;
  spat_rf = 6;
  spat_rf_half = 3;
  spacing = 3;
  border = 0;

  end_stop_width = 1 + 2 * end_stop_len;
  len_sum_width = 1 + 2 * len_sum_len;
  len_sum_norm = 1.0f / (float)len_sum_width;
}

void V1ComplexSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  spat_rf_half = spat_rf / 2;

  end_stop_width = 1 + 2 * end_stop_len;
  len_sum_width = 1 + 2 * len_sum_len;
  len_sum_norm = 1.0f / (float)len_sum_width;
}


// for thread function calling:
typedef void (V1RegionSpec::*V1RegionMethod)(int, int);

void V1RegionSpec::Initialize() {
  gabor_filters.SetBaseType(&TA_GaborFilter);

  v1s_renorm = NO_RENORM;
  v1m_renorm = NO_RENORM;
  v1s_save = SAVE_DATA;
  v1s_feat_geom.SetXYN(4, 6, 24);
  v1b_renorm = LIN_RENORM;
  v1b_save = SAVE_DATA;
  v1c_filters = CF_DEFAULT;
  v1c_renorm = LOG_RENORM;
  v1c_save = SAVE_DATA;
  v1c_feat_geom.SetXYN(4, 2, 8);

  v1s_kwta.on = true;
  v1s_kwta.gp_k = 1;
  v1s_kwta.gp_g = 0.02f;
  v1s_kwta.loc_g = 0.0f;     // 0.8f; -- this is not good for learning in highvis
  v1b_dgp_kwta.on = true;
  v1b_dgp_kwta.gp_k = 1;
  v1b_dgp_kwta.loc_g = 0.0f;
  v1b_dgp_kwta.raw_pct = 0.0f;
  v1b_dgp_kwta.nvar = 0.02f;
  v1c_kwta.on = true;
  v1c_kwta.gp_k = 2;
  v1c_kwta.gp_g = 0.1f;
  v1c_kwta.loc_g = 0.0f;

  cur_dog = NULL;
  cur_dog_circ = NULL;
  v1s_feat_mot_y = 0;
}

void V1RegionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  v1s_specs.UpdateAfterEdit_NoGui();
  v1s_kwta.UpdateAfterEdit_NoGui();
  v1s_motion.UpdateAfterEdit_NoGui();
  v1b_specs.UpdateAfterEdit_NoGui();
  v1b_dgp_specs.UpdateAfterEdit_NoGui();
  v1b_dgp_kwta.UpdateAfterEdit_NoGui();
  v1c_specs.UpdateAfterEdit_NoGui();
  v1c_kwta.UpdateAfterEdit_NoGui();
  // UpdateGeom is called in parent..
}

void V1RegionSpec::UpdateGeom() {
  static bool redo = false;
  inherited::UpdateGeom();

  ///////////////////////////////////////////////////////////////
  //			V1 S

  // angle ordering = 0 = horiz, 1 = 45, 2 = vert, 3 = 135
  v1s_ang_slopes.SetGeom(3,2,2,v1s_specs.n_angles);
  // 0
  int ang = 0;
  v1s_ang_slopes.FastEl(X, LINE, ang) = 1;  v1s_ang_slopes.FastEl(Y, LINE, ang) = 0;
  v1s_ang_slopes.FastEl(X, ORTHO, ang) = 0;  v1s_ang_slopes.FastEl(Y, ORTHO, ang) = 1;
  // 45
  ang++;
  v1s_ang_slopes.FastEl(X, LINE, ang) = 1;  v1s_ang_slopes.FastEl(Y, LINE, ang) = 1;
  v1s_ang_slopes.FastEl(X, ORTHO, ang) = -1;  v1s_ang_slopes.FastEl(Y, ORTHO, ang) = 1;
  // 90
  ang++;
  v1s_ang_slopes.FastEl(X, LINE, ang) = 0;  v1s_ang_slopes.FastEl(Y, LINE, ang) = 1;
  v1s_ang_slopes.FastEl(X, ORTHO, ang) = 1;  v1s_ang_slopes.FastEl(Y, ORTHO, ang) = 0;
  // 135
  ang++;
  v1s_ang_slopes.FastEl(X, LINE, ang) = -1;  v1s_ang_slopes.FastEl(Y, LINE, ang) = 1;
  v1s_ang_slopes.FastEl(X, ORTHO, ang) = 1;  v1s_ang_slopes.FastEl(Y, ORTHO, ang) = 1;

  // all angles for each dog
  int n_static = v1s_specs.n_angles * dog_feat_geom.n;
  v1s_feat_mot_y = dog_feat_geom.n;		// just the y axis!
  int n_motion = 0;
  if(motion_frames > 1) {
    n_motion = 2 * 2 * v1s_specs.n_angles * v1s_motion.n_speeds; // 2 polarity, 2 direction
  }
  v1s_feat_geom.x = v1s_specs.n_angles;
  v1s_feat_geom.n = n_static + n_motion;
  v1s_feat_geom.y = v1s_feat_geom.n / v1s_feat_geom.x;

  // 0 1 2 3 4 5 6 7 8 	retina_size = 9
  // b b .   .   . b b 	border = spacing = 2: input_size = 5, output_size = 3

  // wrap mode, spacing = 1/2 rf_size
  // 0 1 2 3 4 5 6 7  retina_size = 8
  // .   .   .   .    border = 0, spacing = 2; input_size = 8, output_size = 4

  // spacing = rf_size = special case regardless of wrap
  // 0 1 2 3 4 5 6 7   retina_size = 8
  //     .  |    .  |  border = 0, spacing = 2; input_size = 8, output_size = 4

  if(v1s_specs.spacing == v1s_specs.rf_size) {
    v1s_specs.border = v1s_specs.rf_size / 2;
    v1s_img_geom = dog_img_geom / v1s_specs.rf_size;
  }
  else {
    if(edge_mode == WRAP) {
      v1s_specs.border = 0;
      v1s_img_geom = dog_img_geom / v1s_specs.spacing;
    }
    else {
      v1s_specs.border = v1s_specs.rf_size / 2; // always
      v1s_img_geom = (((dog_img_geom - 2 * v1s_specs.border)-1) / v1s_specs.spacing) + 1;
    }
  }

  ///////////////////////////////////////////////////////////////
  //			V1 B

  v1b_img_geom = v1s_img_geom;
  v1b_feat_geom.x = v1s_feat_geom.x;
  v1b_feat_geom.y = v1s_feat_geom.y * (1 + 2 * v1b_specs.n_disps);

  if(edge_mode == WRAP) {
    v1b_dgp_specs.dgp_border = 0;
    v1b_dgp_geom = v1s_img_geom / v1b_dgp_specs.dgp_rf_half_sz;
  }
  else {
    v1b_dgp_specs.dgp_border = v1b_dgp_specs.dgp_rf_half_sz;
    v1b_dgp_geom = (((v1s_img_geom - 2 * v1b_dgp_specs.dgp_border)-1) / v1b_dgp_specs.dgp_rf_half_sz) + 1;
  }

  ///////////////////////////////////////////////////////////////
  //			V1 C

  int n_cmplx = 0;
  int cmplx_y = 0;
  if(v1c_filters & END_STOP) {
    n_cmplx += v1s_specs.n_angles;
    v1c_feat_es_y = cmplx_y++;
  }
  else {
    v1c_feat_es_y = -1;
  }
  if(v1c_filters & LEN_SUM) {
    n_cmplx += v1s_specs.n_angles;
    v1c_feat_ls_y = cmplx_y++;
  }
  else {
    v1c_feat_ls_y = -1;
  }
  if(v1c_filters & V1S_MAX) {
    n_cmplx += v1s_specs.n_angles * 2; // just b/w filters for v1smax
    v1c_feat_smax_y = cmplx_y;
    cmplx_y += 2;
  }
  else {
    v1c_feat_smax_y = -1;
  }
  if(v1c_filters & BLOB) {
    n_cmplx += dog_feat_geom.n;	// one for each unit in dog
    v1c_feat_blob_y = cmplx_y;
    if(dog_feat_geom.n == 2)	// monochrome
      cmplx_y++;
    else
      cmplx_y+=2;		// color = 2 full rows
  }
  else {
    v1c_feat_blob_y = -1;
  }
  if(v1c_filters & CF_EDGES) {
    v1c_feat_edge_y = cmplx_y++;
    if(v1c_filters & DISP_EDGE)
      n_cmplx += 2;		// on/off
    if(v1c_filters & MOTION_EDGE)
      n_cmplx += 2;		// on/off
  }
  else {
    v1c_feat_edge_y = -1;
  }

  v1c_feat_geom.x = v1s_specs.n_angles;
  v1c_feat_geom.n = n_cmplx;
  v1c_feat_geom.y = cmplx_y;
  v1c_feat_geom.UpdateFlag();

  if(edge_mode == WRAP) {
    v1c_specs.border = 0;
    v1c_img_geom = v1s_img_geom / v1c_specs.spacing;
  }
  else {
    v1c_specs.border = v1c_specs.spacing;
    v1c_img_geom = (((v1s_img_geom - 2 * v1c_specs.border)-1) / v1c_specs.spacing) + 1;
  }

  ///////////////////////////////////////
  //  Double-check geom from top down 

  if(redo) {			// if doing a redo, stop here and bail
    redo = false;
    return;
  }

  TwoDCoord v1s_fm_v1c;
  if(edge_mode == WRAP) {
    v1s_fm_v1c = v1c_specs.spacing * v1c_img_geom;
  }
  else {
//     cg = ((sg - 2b - 1) / sp) + 1;
//     cg - 1 = ((sg - 2b - 1) / sp);
//     sp (cg - 1) = (sg - 2b - 1);
//     sp (cg - 1) + 2b + 1 = sg;
    v1s_fm_v1c = v1c_specs.spacing * (v1c_img_geom - 1) + 2 * v1c_specs.border + 1;
  }

  if(v1s_fm_v1c != v1s_img_geom) { // mismatch!
    taMisc::Info("V1RegionSpec:", name,
		 "v1s_img_geom:", v1s_img_geom.GetStr(),
		 "is not an even multiple of v1c_specs.spacing:", v1c_specs.spacing.GetStr(),
		 "this geometry is:", v1s_fm_v1c.GetStr(),
		 "Now recomputing image size to fit this -- you might want to increment by some multiple of spacing to get closer to desired input size");
    v1s_img_geom = v1s_fm_v1c;
    redo = true;		// recompute from here
  }

  TwoDCoord dog_fm_v1s;
  if(v1s_specs.spacing == v1s_specs.rf_size) {
    dog_fm_v1s = v1s_img_geom * v1s_specs.rf_size;
  }
  else {
    if(edge_mode == WRAP) {
      dog_fm_v1s = v1s_img_geom * v1s_specs.spacing;
    }
    else {
      dog_fm_v1s = v1s_specs.spacing * (v1s_img_geom - 1) + 2 * v1s_specs.border + 1;
    }
  } 

  if(dog_fm_v1s != dog_img_geom) { // mismatch!
    if(!redo) {			   // only err if not already redoing
      taMisc::Info("V1RegionSpec:", name,
		   "dog_img_geom:", dog_img_geom.GetStr(),
		   "is not an even multiple of v1s_specs.spacing:", v1c_specs.spacing.GetStr(),
		   "this geometry is:", dog_fm_v1s.GetStr(),
		   "Now recomputing image size to fit this -- you might want to increment by some multiple of spacing to get closer to desired input size");
    }
    dog_img_geom = dog_fm_v1s;
    redo = true;		// recompute from here
  }

  TwoDCoord inp_fm_dog;
  if(edge_mode == WRAP) {
    inp_fm_dog = dog_img_geom * dog_spacing;
  }
  else {
    inp_fm_dog = dog_spacing * (dog_img_geom - 1) + 1;
  }

  if(inp_fm_dog != input_size) { // mismatch!
    if(!redo) {			   // only err if not already redoing
      taMisc::Info("V1RegionSpec:", name,
		   "input_size:", input_size.GetStr(),
		   "is not an even multiple of dog_spacing:", dog_spacing.GetStr(),
		   "this geometry is:", inp_fm_dog.GetStr(),
		   "Recomputing image size to fit this -- you might want to increment by some multiple of spacing to get closer to desired input size");
    }
    input_size = inp_fm_dog;
    retina_size = input_size + 2 * border;
    redo = true;		// recompute from here
  }
}

bool V1RegionSpec::InitFilters() {
  inherited::InitFilters();
  InitFilters_V1Simple();
  if(motion_frames > 1)
    InitFilters_V1Motion();
  if(ocularity == BINOCULAR)
    InitFilters_V1Binocular();
  InitFilters_V1Complex();
  return true;
}

bool V1RegionSpec::InitFilters_V1Simple() {
  // 0,0 = lower left of rf
  // config: x,y coords by rf_size = 1 line, by rf_size = averages over lines, by angles
  v1s_stencils.SetGeom(4, 2, v1s_specs.rf_size, v1s_specs.rf_size, v1s_specs.n_angles);

  // horizontal lines
  for(int pos=0; pos<v1s_specs.rf_size; pos++) { // starting position of line
    for(int lpt=0; lpt<v1s_specs.rf_size; lpt++) { // points in line
      v1s_stencils.FastEl(X, lpt, pos, 0) = lpt;
      v1s_stencils.FastEl(Y, lpt, pos, 0) = pos;
    }
  }
  // vertical lines
  for(int pos=0; pos<v1s_specs.rf_size; pos++) {
    for(int lpt=0; lpt<v1s_specs.rf_size; lpt++) {
      v1s_stencils.FastEl(X, lpt, pos, 2) = pos;
      v1s_stencils.FastEl(Y, lpt, pos, 2) = lpt;
    }
  }
  // diagonal right
  for(int pos=0; pos<v1s_specs.rf_size; pos++) {
    int xs = 1 - pos / 2;	// move up-left, left every other even
    int ys = -1 + (pos+1) / 2;	// move up-left, up every other odd
    for(int lpt=0; lpt<v1s_specs.rf_size; lpt++) {
      v1s_stencils.FastEl(X, lpt, pos, 1) = xs + lpt;
      v1s_stencils.FastEl(Y, lpt, pos, 1) = ys + lpt;
    }
  }
  // diagonal left
  for(int pos=0; pos<v1s_specs.rf_size; pos++) {
    int xs = 2 + pos / 2;	// move up-left, left every other even
    int ys = -1 + (pos+1) / 2;	// move up-left, up every other odd
    for(int lpt=0; lpt<v1s_specs.rf_size; lpt++) {
      v1s_stencils.FastEl(X, lpt, pos, 3) = xs - lpt;
      v1s_stencils.FastEl(Y, lpt, pos, 3) = ys + lpt;
    }
  }

  if(motion_frames > 1) {
    v1m_stencils.SetGeom(6, 2, v1s_motion.tot_width, motion_frames, 2, 
			 v1s_specs.n_angles, v1s_motion.n_speeds);

    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      for(int dir = 0; dir < 2; dir++) { // directions
	int dirsign = (dir == 0) ? 1 : -1; // direction sign for multiplying times slope values
	int dx = dirsign * v1s_ang_slopes.FastEl(X, ORTHO, ang);
	int dy = dirsign * v1s_ang_slopes.FastEl(Y, ORTHO, ang);
	for(int speed = 0; speed < v1s_motion.n_speeds; speed++) { // speed
	  int spd_off = 1 << speed;
	  for(int mot = 0; mot < motion_frames; mot++) { // time steps back in time
	    for(int ew = -v1s_motion.tuning_width; ew <= v1s_motion.tuning_width; ew++) {
	      int ox = (spd_off*mot + ew) * dx;
	      int oy = (spd_off*mot + ew) * dy;
	      v1m_stencils.FastEl(X, v1s_motion.tuning_width+ew, mot, dir, ang, speed) = ox;
	      v1m_stencils.FastEl(Y, v1s_motion.tuning_width+ew, mot, dir, ang, speed) = oy;
	    }
	  }
	}
      }
    }
  }
  else {
    v1m_stencils.SetGeom(1,1);
  }

  // Gabor filters for V1s
  if(v1s_gabors.use) {
    int n_filters = v1s_specs.n_angles * 2;
    gabor_filters.SetSize(n_filters);
    for(int i=0;i<n_filters;i++) {
      float angle_dx = (float)(i % v1s_specs.n_angles); // angle is inner dim
      float phase_dx = (float)((i / v1s_specs.n_angles) % 2); // then phase
		
      GaborFilter* gf = (GaborFilter*)gabor_filters[i];
		
      gf->x_size = v1s_specs.rf_size;
      gf->y_size = v1s_specs.rf_size;
      gf->ctr_x = (float)(gf->x_size - 1.0f) / 2.0f;
      gf->ctr_y = (float)(gf->y_size - 1.0f) / 2.0f;
      gf->angle = taMath_float::pi * angle_dx / (float)v1s_specs.n_angles;
      gf->phase = taMath_float::pi * phase_dx;
      gf->freq = v1s_gabors.freq;
      gf->length = v1s_gabors.length;
      gf->width = v1s_gabors.width;
      gf->amp = v1s_gabors.amp;
      gf->UpdateFilter();
    }
  }

  return true;
}

bool V1RegionSpec::InitFilters_V1Motion() {
  v1m_weights.SetGeom(1, v1s_motion.tot_width);
  if(v1s_motion.tuning_width > 0) {
    int idx = 0;
    for(int x=-v1s_motion.tuning_width; x<=v1s_motion.tuning_width; x++, idx++) {
      float fx = (float)x / (float)v1s_motion.tuning_width;
      v1m_weights.FastEl(idx) = taMath_float::gauss_den_sig(fx, v1s_motion.gauss_sig);
    }
  }
  taMath_float::vec_norm_sum(&v1m_weights); // make sure sums to 1.0
  return true;
}

bool V1RegionSpec::InitFilters_V1Binocular() {
  v1b_widths.SetGeom(1, v1b_specs.tot_disps);
  v1b_weights.SetGeom(2, v1b_specs.max_width, v1b_specs.tot_disps);
  v1b_stencils.SetGeom(2, v1b_specs.max_width, v1b_specs.tot_disps);

  v1b_weights.InitVals(0.0f);	// could have some lurkers in there from other settings, which can affect normalization

  v1bc_weights.SetGeom(1, v1b_specs.tot_disps);

  int twe = v1b_specs.tuning_width + v1b_specs.end_width;

  // everything is conditional on the disparity
  for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
    int didx = disp + v1b_specs.n_disps;
    int doff = disp * v1b_specs.disp_off;
    if(disp == 0) {		// focal
      v1b_widths.FastEl(didx) = 1 + 2 * v1b_specs.tuning_width;
      v1bc_weights.FastEl(didx) = 1.0f;
      for(int tw=-v1b_specs.tuning_width; tw<=v1b_specs.tuning_width; tw++) {
	int twidx = tw + v1b_specs.tuning_width;
	float fx = (float)tw / (float)v1b_specs.tuning_width;
	v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
	v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else if(disp == -v1b_specs.n_disps) {
      v1b_widths.FastEl(didx) = 1 + 2 * v1b_specs.tuning_width + v1b_specs.end_width;
      v1bc_weights.FastEl(didx) = v1c_specs.nonfocal_wt;
      for(int tw=-twe; tw<=v1b_specs.tuning_width; tw++) {
	int twidx = tw + twe;
	if(tw < 0)
	  v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(0.0f, v1b_specs.gauss_sig);
	else {
	  float fx = (float)tw / (float)v1b_specs.tuning_width;
	  v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
	}
	v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else if(disp == v1b_specs.n_disps) {
      v1b_widths.FastEl(didx) = 1 + 2 * v1b_specs.tuning_width + v1b_specs.end_width;
      v1bc_weights.FastEl(didx) = v1c_specs.nonfocal_wt;
      for(int tw=-v1b_specs.tuning_width; tw<=twe; tw++) {
	int twidx = tw + v1b_specs.tuning_width;
	if(tw > 0)
	  v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(0.0f, v1b_specs.gauss_sig);
	else {
	  float fx = (float)tw / (float)v1b_specs.tuning_width;
	  v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
	}
	v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else {
      v1b_widths.FastEl(didx) = 1 + 2 * v1b_specs.tuning_width;
      v1bc_weights.FastEl(didx) = v1c_specs.nonfocal_wt;
      for(int tw=-v1b_specs.tuning_width; tw<=v1b_specs.tuning_width; tw++) {
	int twidx = tw + v1b_specs.tuning_width;
	float fx = (float)tw / (float)v1b_specs.tuning_width;
	v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
	v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
  }
      
  taMath_float::vec_norm_max(&v1b_weights); // max norm to 1


  v1b_dgp_weights.SetGeom(2, v1b_dgp_specs.dgp_rf_sz.x, v1b_dgp_specs.dgp_rf_sz.y);
  float ctr_x = (float)(v1b_dgp_specs.dgp_rf_sz.x-1) * .5f;
  float ctr_y = (float)(v1b_dgp_specs.dgp_rf_sz.y-1) * .5f;
  float eff_sig_x = v1b_dgp_specs.gauss_sig * ctr_x;
  float eff_sig_y = v1b_dgp_specs.gauss_sig * ctr_y;
  for(int yi=0; yi < v1b_dgp_specs.dgp_rf_sz.y; yi++) {
    float y = ((float)yi - ctr_y) / eff_sig_y;
    for(int xi=0; xi < v1b_dgp_specs.dgp_rf_sz.x; xi++) {
      float x = ((float)xi - ctr_x) / eff_sig_x;
      float gv = expf(-(x*x + y*y)/2.0f);
      v1b_dgp_weights.FastEl(xi, yi) = gv;
    }
  }
  taMath_float::vec_norm_max(&v1b_dgp_weights, 1.0f);
  
  v1b_dgp_specs.dgp_rf_norm = 1.0f / taMath_float::vec_sum(&v1b_dgp_weights); // norm sum

  return true;
}

bool V1RegionSpec::InitFilters_V1Complex() {
  v1c_weights.SetGeom(2, v1c_specs.spat_rf.x, v1c_specs.spat_rf.y);
  float ctr_x = (float)(v1c_specs.spat_rf.x-1) * .5f;
  float ctr_y = (float)(v1c_specs.spat_rf.y-1) * .5f;
  float eff_sig_x = v1c_specs.gauss_sig * ctr_x;
  float eff_sig_y = v1c_specs.gauss_sig * ctr_y;
  for(int yi=0; yi< v1c_specs.spat_rf.y; yi++) {
    float y = ((float)yi - ctr_y) / eff_sig_y;
    for(int xi=0; xi< v1c_specs.spat_rf.x; xi++) {
      float x = ((float)xi - ctr_y) / eff_sig_x;
      float gv = expf(-(x*x + y*y)/2.0f);
      v1c_weights.FastEl(xi, yi) = gv;
    }
  }
  taMath_float::vec_norm_max(&v1c_weights, 1.0f);

  // config: x,y coords by points, by angles
  v1c_es_stencils.SetGeom(3, 2, v1c_specs.end_stop_width, v1s_specs.n_angles);
  v1c_ls_stencils.SetGeom(3, 2, v1c_specs.len_sum_width, v1s_specs.n_angles);

  // angle ordering = 0 = horiz, 1 = 45, 2 = vert, 3 = 135
  for(int ang=0; ang < v1s_specs.n_angles; ang++) {
    for(int lpt=-v1c_specs.end_stop_len; lpt <= v1c_specs.end_stop_len; lpt++) {
      int lpidx = lpt + v1c_specs.end_stop_len;
      v1c_es_stencils.FastEl(X, lpidx, ang) = lpt * v1s_ang_slopes.FastEl(X, LINE, ang);
      v1c_es_stencils.FastEl(Y, lpidx, ang) = lpt * v1s_ang_slopes.FastEl(Y, LINE, ang);
    }
    for(int lpt=-v1c_specs.len_sum_len; lpt <= v1c_specs.len_sum_len; lpt++) {
      int lpidx = lpt + v1c_specs.len_sum_len;
      v1c_ls_stencils.FastEl(X, lpidx, ang) = lpt * v1s_ang_slopes.FastEl(X, LINE, ang);
      v1c_ls_stencils.FastEl(Y, lpidx, ang) = lpt * v1s_ang_slopes.FastEl(Y, LINE, ang);
    }
  }
  return true;
}

bool V1RegionSpec::InitOutMatrix() {
  inherited::InitOutMatrix();

  ///////////////////  V1S Output ////////////////////////
  v1s_circ_r.matrix = &v1s_out_r;
  v1s_circ_l.matrix = &v1s_out_l;

  if(motion_frames <= 1) {
    v1s_out_r.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    if(ocularity == BINOCULAR)
      v1s_out_l.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    else
      v1s_out_l.SetGeom(1,1);	// free memory
  }
  else {
    v1s_out_r.SetGeom(5, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y,
		      motion_frames);
    if(ocularity == BINOCULAR)
      v1s_out_l.SetGeom(5, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y,
			motion_frames);
    else
      v1s_out_l.SetGeom(1,1);	// free memory
  }
  v1s_circ_r.Reset();
  v1s_circ_l.Reset();

  ///////////////////  V1B Output ////////////////////////
  if(ocularity == BINOCULAR) {
    v1b_out.SetGeom(4, v1b_feat_geom.x, v1b_feat_geom.y, v1b_img_geom.x, v1b_img_geom.y);
    v1b_disp_out.SetGeom(4, v1b_specs.tot_disps, 1, v1b_img_geom.x, v1b_img_geom.y);
    v1b_dgp_out.SetGeom(4, v1b_specs.tot_disps, 1, v1b_dgp_geom.x, v1b_dgp_geom.y);
  }
  else {
    v1b_out.SetGeom(1,1);	// free memory
    v1b_dgp_out.SetGeom(1,1);
  }

  ///////////////////  V1C Output ////////////////////////
  v1c_out.SetGeom(4, v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);

  // set all the raw ones based on other guys
  v1s_out_r_raw.SetGeomN(v1s_out_r.geom);
  v1s_out_l_raw.SetGeomN(v1s_out_l.geom);
  v1b_out_raw.SetGeomN(v1b_out.geom);
  v1b_disp_raw.SetGeomN(v1b_disp_out.geom);
  v1b_disp_dgraw.SetGeomN(v1b_disp_out.geom);
  v1b_dgp_raw.SetGeomN(v1b_dgp_out.geom);
  v1c_out_raw.SetGeomN(v1c_out.geom);

  return true;
}

void V1RegionSpec::IncrTime() {
  inherited::IncrTime();

  if(motion_frames <= 1) {
    return;		// nop
  }
  else {
    v1s_circ_r.CircAddLimit(motion_frames);
    if(ocularity == BINOCULAR) {
      v1s_circ_l.CircAddLimit(motion_frames);
    }
  }
}

////////////////////////////////////////////////////////////////////
//	V1Region 	Filtering

bool V1RegionSpec::FilterImage_impl() {
  inherited::FilterImage_impl(); // do dogs first

  // todo: maybe check rval for fail and bail -- not currently used..

  wrap = (edge_mode == WRAP);

  bool rval = V1SimpleFilter_Static(&dog_out_r, &dog_circ_r, &v1s_out_r_raw, &v1s_out_r, 
				    &v1s_circ_r);
  if(rval && ocularity == BINOCULAR) {
    rval &= V1SimpleFilter_Static(&dog_out_l, &dog_circ_l, &v1s_out_l_raw, &v1s_out_l,
				  &v1s_circ_l);
  }

  if(motion_frames > 1) {
    rval &= V1SimpleFilter_Motion(&v1s_out_r, &v1s_circ_r);
    if(rval && ocularity == BINOCULAR) {
      rval &= V1SimpleFilter_Motion(&v1s_out_l, &v1s_circ_l);
    }
  }

  if(rval && ocularity == BINOCULAR) {
    rval &= V1BinocularFilter();
  }

  if(rval) {
    rval &= V1ComplexFilter();
  }

  if(!data_table || save_mode == NONE_SAVE) // bail now
    return rval;

  if(v1s_save & SAVE_DATA && !(!taMisc::gui_active && v1s_save & ONLY_GUI)) {
    V1SOutputToTable(data_table);
  }
  if(ocularity == BINOCULAR && v1b_save & SAVE_DATA && !(taMisc::gui_active && v1b_save & ONLY_GUI)) {
    V1BOutputToTable(data_table);
  }
  if(v1c_save & SAVE_DATA && !(!taMisc::gui_active && v1c_save & ONLY_GUI)) {
    V1COutputToTable(data_table);
  }

  return rval;
}

bool V1RegionSpec::V1SimpleFilter_Static(float_Matrix* dog, CircMatrix* dog_circ,
					 float_Matrix* out_raw, float_Matrix* out,
					 CircMatrix* circ) {
  cur_dog = dog;
  cur_dog_circ = dog_circ;
  if(v1s_kwta.on)
    cur_out = out_raw;
  else
    cur_out = out;
  cur_circ = circ;

  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  if(v1s_gabors.use) {
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Static_Gabor_thread);
    threads.Run(&ip_call, n_run);
  }
  else {
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Static_thread);
    threads.Run(&ip_call, n_run);
  }

  if(v1s_renorm != NO_RENORM) {		   // always renorm prior to any kwta
    V1SRenormOutput_Static(cur_out, circ); // only do static, separate from motion
  }

  if(v1s_kwta.on) {
    v1s_kwta.Compute_Kwta(*out_raw, *out, v1s_gci, v1s_gci_tmp, wrap);
  }

  return true;
}

void V1RegionSpec::V1SimpleFilter_Static_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);
  TwoDCoord dcs = v1s_specs.spacing * sc; // dog coords start
  dcs += v1s_specs.border;
  dcs -= v1s_specs.rf_half;	// convert to lower-left starting position, not center

  int dog_mot_idx = cur_dog_circ->CircIdx_Last(); // always write to last position
  int mot_idx = cur_circ->CircIdx_Last(); // always write to last position

  TwoDCoord dc;			// dog coord
  TwoDCoord fc;			// v1s feature coords
  TwoDCoord dfc;		// dog feature coords
  for(int dog = 0; dog < dog_feat_geom.n; dog++) { // dog features
    dfc.SetFmIndex(dog, dog_feat_geom.x);
    fc.y = dog;
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      fc.x = ang;
      float max_line = 0.0f;
      for(int lpos = 0; lpos < v1s_specs.rf_size; lpos++) { // line starting positions
	float line_sum = 0.0f;
	for(int lpt = 0; lpt < v1s_specs.rf_size; lpt++) { // line points
	  int xp = v1s_stencils.FastEl(X,lpt,lpos,ang);
	  int yp = v1s_stencils.FastEl(Y,lpt,lpos,ang);
	  dc.x = dcs.x + xp;
	  dc.y = dcs.y + yp;

	  if(dc.WrapClip(wrap, dog_img_geom)) {
	    if(edge_mode == CLIP) continue; // bail on clipping only
	  }

	  float dogval = MatMotEl(cur_dog, dfc.x, dfc.y, dc.x, dc.y, dog_mot_idx);
	  line_sum += dogval;
	}
	line_sum *= v1s_specs.rf_norm;
	max_line = MAX(max_line, line_sum);
      }
      MatMotEl(cur_out, fc.x, fc.y, sc.x, sc.y, mot_idx) = max_line;
    }
  }
}

void V1RegionSpec::V1SimpleFilter_Static_Gabor_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);
  TwoDCoord dcs = v1s_specs.spacing * sc; // dog coords start
  dcs += v1s_specs.border;
  dcs -= v1s_specs.rf_half;	// convert to lower-left starting position, not center

  int dog_mot_idx = cur_dog_circ->CircIdx_Last(); // always write to last position
  int mot_idx = cur_circ->CircIdx_Last(); // always write to last position

  TwoDCoord gc;					     // gabor coords
  TwoDCoord dc;					     // dog coords
  for(int gfi = 0; gfi < gabor_filters.size; gfi++) { // gabor filters
    GaborFilter* gf = (GaborFilter*)gabor_filters[gfi];
    int ang = gfi % v1s_specs.n_angles;
    int pol = gfi / v1s_specs.n_angles;
    
    float flt_sum = 0.0f;
    for(gc.y=0; gc.y < v1s_specs.rf_size; gc.y++) {
      for(gc.x=0; gc.x < v1s_specs.rf_size; gc.x++) {
	dc = dcs + gc;
	if(dc.WrapClip(wrap, dog_img_geom)) {
	  if(edge_mode == CLIP) continue; // bail on clipping only
	}
	float fval = gf->filter.FastEl(gc.x, gc.y);
	float oval;
	if(fval > 0.0f) oval = fval * MatMotEl(cur_dog, 0, 0, dc.x, dc.y, dog_mot_idx); // 0=on
	else	        oval = -fval * MatMotEl(cur_dog, 1, 0, dc.x, dc.y, dog_mot_idx); // 1=off
	flt_sum += oval;
      }
    }
    MatMotEl(cur_out, ang, pol, sc.x, sc.y, mot_idx) = flt_sum;
  }
}

bool V1RegionSpec::V1SRenormOutput_Static(float_Matrix* out, CircMatrix* circ) {
  bool rval = false;
  float_Matrix* mat = out;
  if(motion_frames > 1) {
    mat = (float_Matrix*)out->GetFrameSlice(circ->CircIdx_Last());
    taBase::Ref(mat);
  }
  float max_val = 0.0f;
  TwoDCoord sc;		// simple coords
  TwoDCoord fc;		// v1s feature coords
  for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
    for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
      for(int dog = 0; dog < dog_feat_geom.n; dog++) { // dog features
	fc.y = dog;
	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	  fc.x = ang;
	  float val = mat->FastEl(fc.x, fc.y, sc.x, sc.y);
	  max_val = MAX(val, max_val);
	}
      }
    }
  }
  if(max_val > renorm_thr) {
    rval = true;
    if(v1s_renorm == LIN_RENORM) {
      float rescale = 1.0f / max_val;
      for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	  for(int dog = 0; dog < dog_feat_geom.n; dog++) { // dog features
	    fc.y = dog;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float& val = mat->FastEl(fc.x, fc.y, sc.x, sc.y);
	      val *= rescale;
	    }
	  }
	}
      }
    }
    else if(v1s_renorm == LOG_RENORM) {
      float rescale = 1.0f / logf(1.0f + max_val);
      for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	  for(int dog = 0; dog < dog_feat_geom.n; dog++) { // dog features
	    fc.y = dog;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float& val = mat->FastEl(fc.x, fc.y, sc.x, sc.y);
	      val = logf(1.0f + val) * rescale;
	    }
	  }
	}
      }
    }
  }
  if(motion_frames > 1) {
    taBase::unRefDone(mat);
  }
  return rval;
}

bool V1RegionSpec::V1SimpleFilter_Motion(float_Matrix* out, CircMatrix* circ) {
  cur_out = out;	
  cur_circ = circ;

  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Motion_thread);
  threads.Run(&ip_call, n_run);
  return true;

  if(v1m_renorm != NO_RENORM) {
    V1SRenormOutput_Motion(out, circ); // only do static, separate from motion
  }
}

void V1RegionSpec::V1SimpleFilter_Motion_thread(int v1s_idx, int thread_no) {
  // reads off of the v1s static filters through time history..
  TwoDCoord sc;			// complex coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int cur_mot_idx = cur_circ->CircIdx_Last();
  int mot_len = cur_circ->length;

  TwoDCoord fc;			// v1s feature coords -- destination
  TwoDCoord sfc;		// v1s feature coords -- source
  TwoDCoord mo;			// motion offset
  for(int speed = 0; speed < v1s_motion.n_speeds; speed++) { // speed
    for(int dir = 0; dir < 2; dir++) { // directions
      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	for(int pol = 0; pol < 2; pol++) { // polarities that we care about
	  fc.x = ang;
	  fc.y = v1s_feat_mot_y + speed * 4 + dir * 2 + pol;
	  sfc.x = ang;
	  sfc.y = pol;		// polarity

	  float sum_mot = 0.0f;
	  for(int mot = 0; mot < motion_frames; mot++) { // time steps back in time
	    if(mot >= mot_len) continue;	 // off end
	    for(int tw = -v1s_motion.tuning_width; tw <= v1s_motion.tuning_width; tw++) {
	      int twidx = v1s_motion.tuning_width+tw;
	      int xp = v1m_stencils.FastEl(X, twidx, mot, dir, ang, speed);
	      int yp = v1m_stencils.FastEl(Y, twidx, mot, dir, ang, speed);

	      mo.x = sc.x + xp;
	      mo.y = sc.y + yp;
	      if(mo.WrapClip(wrap, v1s_img_geom)) {
		if(edge_mode == CLIP) continue; // bail on clipping only
	      }

	      float val = cur_out->FastEl(sfc.x, sfc.y, mo.x, mo.y,
					  cur_circ->CircIdx(mot_len-1 - mot));
	      sum_mot += val * v1m_weights.FastEl(twidx);
	    }
	  }
	  cur_out->FastEl(fc.x, fc.y, sc.x, sc.y, cur_mot_idx) = sum_mot;
	}
      }
    }
  }
}

bool V1RegionSpec::V1SRenormOutput_Motion(float_Matrix* out, CircMatrix* circ) {
  bool rval = false;
  float_Matrix* mat = out;
  if(motion_frames > 1) {
    mat = (float_Matrix*)out->GetFrameSlice(circ->CircIdx_Last());
    taBase::Ref(mat);
  }
  float max_val = 0.0f;
  TwoDCoord sc;		// simple coords
  TwoDCoord fc;		// v1s feature coords
  for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
    for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
      for(int mot = v1s_feat_mot_y; mot < v1s_feat_geom.y; mot++) { // motion
	fc.y = mot;
	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	  fc.x = ang;
	  float val = mat->FastEl(fc.x, fc.y, sc.x, sc.y);
	  max_val = MAX(val, max_val);
	}
      }
    }
  }
  if(max_val > renorm_thr) {
    rval = true;
    if(v1m_renorm == LIN_RENORM) {
      float rescale = 1.0f / max_val;
      for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	  for(int mot = v1s_feat_mot_y; mot < v1s_feat_geom.y; mot++) { // motion
	    fc.y = mot;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float& val = mat->FastEl(fc.x, fc.y, sc.x, sc.y);
	      val *= rescale;
	    }
	  }
	}
      }
    }
    else if(v1m_renorm == LOG_RENORM) {
      float rescale = 1.0f / logf(1.0f + max_val);
      for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	  for(int mot = v1s_feat_mot_y; mot < v1s_feat_geom.y; mot++) { // motion
	    fc.y = mot;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float& val = mat->FastEl(fc.x, fc.y, sc.x, sc.y);
	      val = logf(1.0f + val) * rescale;
	    }
	  }
	}
      }
    }
  }
  if(motion_frames > 1) {
    taBase::unRefDone(mat);
  }
  return rval;
}

bool V1RegionSpec::V1BinocularFilter() {
  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_thread);
  threads.Run(&ip_call, n_run);

  if(v1b_renorm != NO_RENORM) {
    RenormOutput_NoFrames(v1b_renorm, cur_out);
  }

  // then do disparity grouping
  V1BinocularFilter_DispGp();

  return true;
}

void V1RegionSpec::V1BinocularFilter_thread(int v1b_idx, int thread_no) {
  TwoDCoord bc;			// complex coords
  bc.SetFmIndex(v1b_idx, v1b_img_geom.x);

  // note: binoc order = near..focus..far

  int cur_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord fc;			// v1b feature coords -- destination
  TwoDCoord sfc;		// v1s feature coords -- source
  TwoDCoord bo;			// binoc offset
  bo.y = bc.y;			// this will never be wrapped or clipped so can be set once..

  float_Matrix disp_wts;
  disp_wts.SetGeom(1, v1b_specs.tot_disps);

  for(int sfi = 0; sfi < v1s_feat_geom.n; sfi++) { // simple feature index
    sfc.SetFmIndex(sfi, v1s_feat_geom.x);
    fc.x = sfc.x;

    // response is anchored at corresponding location on the right (dominant) eye --
    // this determines the max response at this point in the visual field
    float rv = MatMotEl(&v1s_out_r, sfc.x, sfc.y, bc.x, bc.y, cur_mot_idx); // note: bc

    if(rv < v1b_specs.opt_thr) {
      for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
	int didx = disp + v1b_specs.n_disps;
	fc.y = sfc.y + didx * v1s_feat_geom.y;
	v1b_out_raw.FastEl(fc.x, fc.y, bc.x, bc.y) = 0.0f;
      }
      continue;
    }

    disp_wts.InitVals(0.0f);
    // find max left eye response across each disparity
    for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
      int didx = disp + v1b_specs.n_disps;
      int dwd = v1b_widths.FastEl(didx);
      float max_lv = 0.0f;
      for(int twidx = 0; twidx < dwd; twidx++) {
	int off = v1b_stencils.FastEl(twidx, didx);
	
	// left eye is opposite
	bo.x = bc.x - off;
	if(bo.WrapClip(wrap, v1s_img_geom)) {
	  if(edge_mode == CLIP) continue; // bail on clipping only
	}
	float lv = MatMotEl(&v1s_out_l, sfc.x, sfc.y, bo.x, bo.y, cur_mot_idx);
	lv *= v1b_weights.FastEl(twidx, didx);
	max_lv = MAX(max_lv, lv);
      }
      disp_wts.FastEl(didx) = max_lv;
    }

    taMath_float::vec_norm_max(&disp_wts); // normalize to max = 1
    for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
      int didx = disp + v1b_specs.n_disps;
      fc.y = sfc.y + didx * v1s_feat_geom.y;
      v1b_out_raw.FastEl(fc.x, fc.y, bc.x, bc.y) = rv * disp_wts.FastEl(didx); // apportion by norm
    }
  }

  // now integrate result into pure disparity weighting for this location
  float tot_disp = 0.0f;
  for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
    int didx = disp + v1b_specs.n_disps;
    float disp_sum = 0.0f;
    for(int sfi = 0; sfi < v1s_feat_geom.n; sfi++) { // simple feature index
      sfc.SetFmIndex(sfi, v1s_feat_geom.x);
      fc.x = sfc.x;
      fc.y = sfc.y + didx * v1s_feat_geom.y;
      float dv = v1b_out_raw.FastEl(fc.x, fc.y, bc.x, bc.y);
      disp_sum += dv;
    }
    v1b_disp_raw.FastEl(didx, 0, bc.x, bc.y) = disp_sum;
    tot_disp += disp_sum;
  }
  if(tot_disp > 0.0f) {
    for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
      int didx = disp + v1b_specs.n_disps;
      v1b_disp_raw.FastEl(didx, 0, bc.x, bc.y) /= tot_disp; // normalize
    }
  }
  else {
    for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
      int didx = disp + v1b_specs.n_disps;
      v1b_disp_raw.FastEl(didx, 0, bc.x, bc.y) = 0.0f;
    }
  }
}

void V1RegionSpec::V1BinocularFilter_DispGp() {
  TwoDCoord dgpc;
  TwoDCoord bc;
  TwoDCoord bcs;
  TwoDCoord gc;
  TwoDCoord bcc;
  TwoDCoord fc;			// v1b feature coords -- destination
  TwoDCoord sfc;		// v1s feature coords -- source

  v1b_disp_out.CopyFrom(&v1b_disp_raw); // first step is raw copy

  for(int iter = 0; iter < v1b_dgp_specs.dgp_iters; iter++) {
    // first compute current sums
    for(dgpc.y=0; dgpc.y < v1b_dgp_geom.y; dgpc.y++) {
      for(dgpc.x=0; dgpc.x < v1b_dgp_geom.x; dgpc.x++) {
	bc = dgpc * v1b_dgp_specs.dgp_rf_half_sz + v1b_dgp_specs.dgp_border;
	bcs = bc - v1b_dgp_specs.dgp_rf_half_sz;	// starting point
	for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
	  float sum_disp = 0.0f;
	  for(gc.y=0; gc.y < v1b_dgp_specs.dgp_rf_sz.y; gc.y++) {
	    for(gc.x=0; gc.x < v1b_dgp_specs.dgp_rf_sz.x; gc.x++) {
	      bcc = bcs + gc;
	      if(bcc.WrapClip(wrap, v1b_img_geom)) {
		if(!wrap) continue; // bail on clipping only -- shouldn't happen!
	      }
	      float dval = v1b_disp_out.FastEl(didx, 0, bcc.x, bcc.y);
	      sum_disp += v1b_dgp_weights.FastEl(gc.x, gc.y) * dval;
	    }
	  }
	  sum_disp *= v1b_dgp_specs.dgp_rf_norm;
	  v1b_dgp_raw.FastEl(didx, 0, dgpc.x, dgpc.y) = sum_disp;
	}
      }
    }

    v1b_dgp_kwta.Compute_Kwta(v1b_dgp_raw, v1b_dgp_out, v1b_dgp_gci, v1b_dgp_gci_tmp, wrap);
    // kwta on output

    // then integrate weightings from overlapping groups into an overall weighting score
    v1b_disp_dgraw.InitVals(0.0f);
    for(dgpc.y=0; dgpc.y < v1b_dgp_geom.y; dgpc.y++) {
      for(dgpc.x=0; dgpc.x < v1b_dgp_geom.x; dgpc.x++) {
	bc = dgpc * v1b_dgp_specs.dgp_rf_half_sz + v1b_dgp_specs.dgp_border;
	bcs = bc - v1b_dgp_specs.dgp_rf_half_sz;	// starting point
	for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
	  float dgp_val = v1b_dgp_out.FastEl(didx, 0, dgpc.x, dgpc.y);
	  dgp_val *= v1b_dgp_specs.disp_gain;
	  for(gc.y=0; gc.y < v1b_dgp_specs.dgp_rf_sz.y; gc.y++) {
	    for(gc.x=0; gc.x < v1b_dgp_specs.dgp_rf_sz.x; gc.x++) {
	      bcc = bcs + gc;
	      if(bcc.WrapClip(wrap, v1b_img_geom)) {
		if(!wrap) continue; // bail on clipping only -- shouldn't happen!
	      }
	      float dgp_wt = v1b_dgp_weights.FastEl(gc.x, gc.y) * dgp_val;
	      v1b_disp_dgraw.FastEl(didx, 0, bcc.x, bcc.y) += dgp_wt; // integrate weights
	    }
	  }
	}
      }
    }
    // then propagate out to influence disparity values
    for(bc.y=0; bc.y < v1b_img_geom.y; bc.y++) {
      for(bc.x=0; bc.x < v1b_img_geom.x; bc.x++) {
	float dgp_wt_sum = 0.0f;
	for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
	  float dgp_wt = v1b_disp_dgraw.FastEl(didx, 0, bc.x, bc.y);
	  dgp_wt_sum += dgp_wt;
	}
	float dgp_wt_avg = dgp_wt_sum / (float)v1b_specs.tot_disps;
	for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
	  float& dgrw = v1b_disp_dgraw.FastEl(didx, 0, bc.x, bc.y);
	  float dgp_wt = dgrw - dgp_wt_avg; // sub avg
	  float rawv = v1b_disp_raw.FastEl(didx, 0, bc.x, bc.y);
	  if(rawv > 0.0f) {
	    float nwval = rawv + dgp_wt; // add to raw
	    if(nwval < 0) nwval = 0.0f;
	    if(nwval > 1.0f) nwval = 1.0f;
	    dgrw = nwval;
	  }
	  else {
	    dgrw = 0.0f;	// keep it null
	  }
	}
      }
    }

    v1b_dgp_kwta.Compute_Kwta(v1b_disp_dgraw, v1b_disp_out, v1b_disp_gci, v1b_disp_gci_tmp, wrap);
    // kwta on final output
  }

  float v1b_mix_c = 1.0f - v1b_dgp_specs.v1b_mix;

  // then propagate out to influence specific feature reps
  for(bc.y=0; bc.y < v1b_img_geom.y; bc.y++) {
    for(bc.x=0; bc.x < v1b_img_geom.x; bc.x++) {
      for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
	float dwt = v1b_disp_out.FastEl(didx, 0, bc.x, bc.y);
	for(int sfi = 0; sfi < v1s_feat_geom.n; sfi++) { // simple feature index
	  sfc.SetFmIndex(sfi, v1s_feat_geom.x);
	  fc.x = sfc.x;
	  fc.y = sfc.y + didx * v1s_feat_geom.y;
	  float rawv = v1b_out_raw.FastEl(fc.x, fc.y, bc.x, bc.y);
	  v1b_out.FastEl(fc.x, fc.y, bc.x, bc.y) = v1b_mix_c * rawv + 
	    v1b_dgp_specs.v1b_mix * rawv * dwt; // just reweight
	}
      }
    }
  }
}

bool V1RegionSpec::V1ComplexFilter() {
  if(v1c_kwta.on)
    cur_out = &v1c_out_raw;
  else
    cur_out = &v1c_out;

  int n_run = v1c_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  if(ocularity == BINOCULAR) {
    if(v1c_filters & CF_ESLS) {
      ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_EsLs_Binocular_thread);
      threads.Run(&ip_call, n_run);
    }
    if(v1c_filters & V1S_MAX) {
      ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_V1SMax_Binocular_thread);
      threads.Run(&ip_call, n_run);
    }
    if(v1c_filters & BLOB) {
      ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_Blob_Binocular_thread);
      threads.Run(&ip_call, n_run);
    }
    if(v1c_filters & DISP_EDGE) {
      ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_DispEdge_thread);
      threads.Run(&ip_call, n_run);
    }
    if(motion_frames > 1 && v1c_filters & MOTION_EDGE) {
      ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_MotionEdge_Binocular_thread);
      threads.Run(&ip_call, n_run);
    }
  }
  else {
    if(v1c_filters & CF_ESLS) {
      ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_EsLs_Monocular_thread);
      threads.Run(&ip_call, n_run);
    }
    if(v1c_filters & V1S_MAX) {
      ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_V1SMax_Monocular_thread);
      threads.Run(&ip_call, n_run);
    }
    if(v1c_filters & BLOB) {
      ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_Blob_Monocular_thread);
      threads.Run(&ip_call, n_run);
    }
    if(motion_frames > 1 && v1c_filters & MOTION_EDGE) {
      ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_MotionEdge_Monocular_thread);
      threads.Run(&ip_call, n_run);
    }
  }

  // always renorm *prior* to any kwta
  if(v1c_renorm != NO_RENORM) {
    V1CRenormOutput_EsLsBlob(cur_out);
    //    RenormOutput_NoFrames(v1c_renorm, cur_out);  // this renorms everything together, which is not as good as separately renorming the blob vs. the rest of the guys
  }

  if(v1c_kwta.on) {
    v1c_kwta.Compute_Kwta(v1c_out_raw, v1c_out, v1c_gci, v1c_gci_tmp, wrap);
  }

  return true;
}

void V1RegionSpec::V1ComplexFilter_EsLs_Monocular_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;			// complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);
  TwoDCoord scs = v1c_specs.spacing * cc; // v1s coords start
  scs += v1c_specs.border;
  scs -= v1c_specs.spat_rf_half; // convert to lower-left starting position, not center

  int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord sc;			// simple coord
  TwoDCoord sce;		// simple coord, ends
  TwoDCoord scc;		// simple coord, center
  TwoDCoord fc;			// v1c feature coords
  TwoDCoord sfc_ctr;		// simple feature coords for the central point
  TwoDCoord sfc_end;		// simple feature coords for the end point
  for(int cfeat = 0; cfeat < 2; cfeat++) { // end-stop, length-sum
    if(cfeat == 0) {
      if(!(v1c_filters & END_STOP)) continue;
      fc.y = v1c_feat_es_y;
    }
    else {
      if(!(v1c_filters & LEN_SUM)) continue;
      fc.y = v1c_feat_ls_y;
    }
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      fc.x = ang;
      float max_sf = 0.0f;	// max over simple features
      for(int v1sf = 0; v1sf < v1s_feat_geom.y; v1sf++) {
	// v1 simple features -- for end-stop, designates the sign of the center..
	int v1sf_onst = 2 * (v1sf / 2); // even numbers = start of on-center
	sfc_ctr.x = ang;
	sfc_ctr.y = v1sf;
	sfc_end.x = ang;
	if(cfeat == 0)	{	// end stop
	  sfc_end.y = v1sf_onst + (1 - (v1sf - v1sf_onst)); // opposite polarity -- flip bit in odd/even within same set of 2
	}
	else {
	  sfc_end.y = v1sf;
	}

	float max_rf = 0.0f;   // max over spatial rfield
	for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // ysimple
	  sc.y = scs.y + ys;
	  for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xsimple
	    sc.x = scs.x + xs;
	    scc = sc;	// center
	    if(scc.WrapClip(wrap, v1s_img_geom)) {
	      if(edge_mode == CLIP) continue; // bail on clipping only
	    }

	    // first get central value -- always the same
	    float ctr_val = MatMotEl(&v1s_out_r, sfc_ctr.x, sfc_ctr.y, scc.x, scc.y, v1s_mot_idx);
	    if(cfeat == 0) {	// then do end stop
	      float es_max = 0.0f; // es response is max of activity in either end of the symmetric end stop "opposite polarity" rf
	      for(int lpt=-v1c_specs.end_stop_len; lpt <= v1c_specs.end_stop_len; lpt++) {
		if(lpt == 0) continue; // skip center
		int lpidx = lpt + v1c_specs.end_stop_len;
		int xp = v1c_es_stencils.FastEl(X,lpidx,ang);
		int yp = v1c_es_stencils.FastEl(Y,lpidx,ang);
		sce.x = sc.x + xp;
		sce.y = sc.y + yp;

		if(sce.WrapClip(wrap, v1s_img_geom)) {
		  if(edge_mode == CLIP) continue; // bail on clipping only
		}

		float end_val = 0.0f;
		// compute max over other angles -- any opposite polarity angle will do
		for(int opang=0; opang<v1s_specs.n_angles; opang++) {
		  float ev = MatMotEl(&v1s_out_r, opang, sfc_end.y, sce.x, sce.y, v1s_mot_idx);
		  float opev = MatMotEl(&v1s_out_r, opang, sfc_ctr.y, sce.x, sce.y, v1s_mot_idx); // opposite polarity guy
		  ev -= opev;	// count only relative strength compared to same angle of opposite contrast -- sometimes wide lines get double-counted..
		  // ev *= v1bc_weights.FastEl(didx);
		  // note: not doing weighting on end-stopping guys!! better for learning..
		  // todo: should have separate params for es and ls
		  end_val = MAX(end_val, ev);
		}
		es_max = MAX(es_max, end_val);
	      }
	      es_max = MIN(es_max, ctr_val); // cannot exceed central value -- this is the primary
	      // driver of the cell so ends cannot get it more excited than center --
	      // otherwise you can get a 2/3 activity of bright ends with a weak ctr
	      es_max *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
	      max_rf = MAX(max_rf, es_max);
	    }
	    else if(cfeat == 1) {	// length sum
	      float line_sum = ctr_val; // just a straight-up sum over the line segment
	      for(int lpt=-v1c_specs.len_sum_len; lpt <= v1c_specs.len_sum_len; lpt++) {
		if(lpt == 0) continue; // skip center
		int lpidx = lpt + v1c_specs.len_sum_len;
		int xp = v1c_ls_stencils.FastEl(X,lpidx,ang);
		int yp = v1c_ls_stencils.FastEl(Y,lpidx,ang);
		sce.x = sc.x + xp;
		sce.y = sc.y + yp;

		if(sce.WrapClip(wrap, v1s_img_geom)) {
		  if(edge_mode == CLIP) continue; // bail on clipping only
		}

		float end_val = MatMotEl(&v1s_out_r, sfc_end.x, sfc_end.y, sce.x, sce.y, v1s_mot_idx);
		line_sum += end_val;
	      }
	      line_sum *= v1c_specs.len_sum_norm;
	      line_sum *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
	      max_rf = MAX(max_rf, line_sum);
	    }
	  }
	}
	max_sf = MAX(max_sf, max_rf); // max over all simple features -- very general
      } // for v1sf
      cur_out->FastEl(fc.x, fc.y, cc.x, cc.y) = max_sf;
    }  // for ang
  }
}

void V1RegionSpec::V1ComplexFilter_EsLs_Binocular_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;			// complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);
  TwoDCoord scs = v1c_specs.spacing * cc; // v1s coords start
  scs += v1c_specs.border;
  scs -= v1c_specs.spat_rf_half; // convert to lower-left starting position, not center

  int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord sc;			// simple coord
  TwoDCoord sce;		// simple coord, ends
  TwoDCoord scc;		// simple coord, center
  TwoDCoord fc;			// v1c feature coords
  TwoDCoord sfc_ctr;		// simple feature coords for the central point
  TwoDCoord sfc_end;		// simple feature coords for the end point
  for(int cfeat = 0; cfeat < 2; cfeat++) { // end-stop, length-sum
    if(cfeat == 0) {
      if(!(v1c_filters & END_STOP)) continue;
      fc.y = v1c_feat_es_y;
    }
    else {
      if(!(v1c_filters & LEN_SUM)) continue;
      fc.y = v1c_feat_ls_y;
    }
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      fc.x = ang;
      float max_sf = 0.0f;	// max over simple features
      for(int v1sf = 0; v1sf < v1s_feat_geom.y; v1sf++) {
	// v1 simple features -- for end-stop, designates the sign of the center..
	int v1sf_onst = 2 * (v1sf / 2); // even numbers = start of on-center
	sfc_ctr.x = ang;
	sfc_ctr.y = v1sf;
	sfc_end.x = ang;
	if(cfeat == 0)	{	// end stop
	  sfc_end.y = v1sf_onst + (1 - (v1sf - v1sf_onst)); // opposite polarity -- flip bit in odd/even within same set of 2
	}
	else {
	  sfc_end.y = v1sf;
	}

	float max_rf = 0.0f;   // max over spatial rfield
	for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // ysimple
	  sc.y = scs.y + ys;
	  for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xsimple
	    sc.x = scs.x + xs;
	    scc = sc;	// center
	    if(scc.WrapClip(wrap, v1b_img_geom)) {
	      if(edge_mode == CLIP) continue; // bail on clipping only
	    }

	    // first get central value -- always the same
	    float ctr_val = 0.0f;
	    for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
	      int didx = disp + v1b_specs.n_disps;
	      float cv = MatMotEl(&v1b_out, sfc_ctr.x, sfc_ctr.y + didx * v1s_feat_geom.y,
				  scc.x, scc.y, v1s_mot_idx) * v1bc_weights.FastEl(didx);
	      ctr_val = MAX(ctr_val, cv);
	    }

	    if(cfeat == 0) {	// then do end stop
	      float es_max = 0.0f; // es response is max of activity in either end of the symmetric end stop "opposite polarity" rf
	      for(int lpt=-v1c_specs.end_stop_len; lpt <= v1c_specs.end_stop_len; lpt++) {
		if(lpt == 0) continue; // skip center
		int lpidx = lpt + v1c_specs.end_stop_len;
		int xp = v1c_es_stencils.FastEl(X,lpidx,ang);
		int yp = v1c_es_stencils.FastEl(Y,lpidx,ang);
		sce.x = sc.x + xp;
		sce.y = sc.y + yp;

		if(sce.WrapClip(wrap, v1b_img_geom)) {
		  if(edge_mode == CLIP) continue; // bail on clipping only
		}

		float end_val = 0.0f;
		// compute max over other angles -- any opposite polarity angle will do
		for(int opang=0; opang<v1s_specs.n_angles; opang++) {
		  for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
		    int didx = disp + v1b_specs.n_disps;
		    float ev = v1b_out.FastEl(opang, sfc_end.y + didx * v1s_feat_geom.y,
					      sce.x, sce.y);
		    float opev = v1b_out.FastEl(opang, sfc_ctr.y + didx * v1s_feat_geom.y,
					      sce.x, sce.y);
		    ev -= opev;	// count only relative strength compared to same angle of opposite contrast -- sometimes wide lines get double-counted..
		    // ev *= v1bc_weights.FastEl(didx);
		    // note: not doing weighting on end-stopping guys!! better for learning..
		    // todo: should have separate params for es and ls
		    end_val = MAX(end_val, ev);
		  }
		}
		es_max = MAX(es_max, end_val);
	      }
	      es_max = MIN(es_max, ctr_val); // cannot exceed central value -- this is the primary
	      // driver of the cell so ends cannot get it more excited than center --
	      // otherwise you can get a 2/3 activity of bright ends with a weak ctr
	      es_max *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
	      max_rf = MAX(max_rf, es_max);
	    }
	    else if(cfeat == 1) {	// length sum
	      float line_sum = ctr_val; // just a straight-up sum over the line segment
	      for(int lpt=-v1c_specs.len_sum_len; lpt <= v1c_specs.len_sum_len; lpt++) {
		if(lpt == 0) continue; // skip center
		int lpidx = lpt + v1c_specs.len_sum_len;
		int xp = v1c_ls_stencils.FastEl(X,lpidx,ang);
		int yp = v1c_ls_stencils.FastEl(Y,lpidx,ang);
		sce.x = sc.x + xp;
		sce.y = sc.y + yp;

		if(sce.WrapClip(wrap, v1b_img_geom)) {
		  if(edge_mode == CLIP) continue; // bail on clipping only
		}

		float end_val = 0.0f;
		for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
		  int didx = disp + v1b_specs.n_disps;
		  float ev = v1b_out.FastEl(sfc_end.x, sfc_end.y + didx * v1s_feat_geom.y,
					    sce.x, sce.y);
		  ev *=  v1bc_weights.FastEl(didx);
		  end_val = MAX(end_val, ev);
		}
		line_sum += end_val;
	      }
	      line_sum *= v1c_specs.len_sum_norm;
	      line_sum *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
	      max_rf = MAX(max_rf, line_sum);
	    }
	  }
	}
	max_sf = MAX(max_sf, max_rf); // max over all simple features -- very general
      } // for v1sf
      cur_out->FastEl(fc.x, fc.y, cc.x, cc.y) = max_sf;
    }  // for ang
  }
}

void V1RegionSpec::V1ComplexFilter_V1SMax_Monocular_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;			// complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);
  TwoDCoord scs = v1c_specs.spacing * cc; // v1s coords start
  scs += v1c_specs.border;
  scs -= v1c_specs.spat_rf_half; // convert to lower-left starting position, not center

  int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord sc;			// simple coord
  TwoDCoord scc;		// simple coord, center
  TwoDCoord sfc;		// v1s feature coords
  TwoDCoord fc;			// v1c feature coords
  for(int dog = 0; dog < 2; dog++) { // only first monochrome on/off guys
    sfc.y = dog;
    fc.y = v1c_feat_smax_y + dog;
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      sfc.x = ang;
      fc.x = ang;
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // ysimple
	sc.y = scs.y + ys;
	for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xsimple
	  sc.x = scs.x + xs;
	  scc = sc;	// center
	  if(scc.WrapClip(wrap, v1s_img_geom)) {
	    if(edge_mode == CLIP) continue; // bail on clipping only
	  }
	  float ctr_val = MatMotEl(&v1s_out_r, sfc.x, sfc.y, scc.x, scc.y, v1s_mot_idx);
	  ctr_val *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
	  max_rf = MAX(max_rf, ctr_val);
	}
      }
      cur_out->FastEl(fc.x, fc.y, cc.x, cc.y) = max_rf;
    } // for ang
  }  // for dog
}

void V1RegionSpec::V1ComplexFilter_V1SMax_Binocular_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;			// complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);
  TwoDCoord scs = v1c_specs.spacing * cc; // v1s coords start
  scs += v1c_specs.border;
  scs -= v1c_specs.spat_rf_half; // convert to lower-left starting position, not center

  int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord sc;			// simple coord
  TwoDCoord scc;		// simple coord, center
  TwoDCoord sfc;		// v1s feature coords
  TwoDCoord fc;			// v1c feature coords
  for(int dog = 0; dog < 2; dog++) { // only first monochrome on/off guys
    sfc.y = dog;
    fc.y = v1c_feat_smax_y + dog;
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      sfc.x = ang;
      fc.x = ang;
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // ysimple
	sc.y = scs.y + ys;
	for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xsimple
	  sc.x = scs.x + xs;
	  scc = sc;	// center
	  if(scc.WrapClip(wrap, v1b_img_geom)) {
	    if(edge_mode == CLIP) continue; // bail on clipping only
	  }
	  float ctr_val = 0.0f;
	  for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
	    int didx = disp + v1b_specs.n_disps;
	    float cv = MatMotEl(&v1b_out, sfc.x, sfc.y + didx * v1s_feat_geom.y,
				scc.x, scc.y, v1s_mot_idx)* v1bc_weights.FastEl(didx);
	    ctr_val = MAX(ctr_val, cv);
	  }
	  ctr_val *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
	  max_rf = MAX(max_rf, ctr_val);
	}
      }
      cur_out->FastEl(fc.x, fc.y, cc.x, cc.y) = max_rf;
    } // for ang
  }  // for dog
}

void V1RegionSpec::V1ComplexFilter_Blob_Monocular_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;			// complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);
  TwoDCoord scs = v1c_specs.spacing * cc; // v1s coords start
  scs += v1c_specs.border;
  scs -= v1c_specs.spat_rf_half; // convert to lower-left starting position, not center

  int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord sc;			// simple coord
  TwoDCoord scc;		// simple coord, center
  TwoDCoord sfc;		// v1s feature coords
  TwoDCoord fc;			// v1c feature coords
  for(int dog = 0; dog < dog_feat_geom.n; dog++) { // dog features -- includes b/w on/off
    sfc.y = dog;
    fc.y = v1c_feat_blob_y + dog / v1c_feat_geom.x;
    fc.x = dog % v1c_feat_geom.x;
    float max_rf = 0.0f;   // max over spatial rfield
    for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // ysimple
      sc.y = scs.y + ys;
      for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xsimple
	sc.x = scs.x + xs;
	scc = sc;	// center
	if(scc.WrapClip(wrap, v1s_img_geom)) {
	  if(edge_mode == CLIP) continue; // bail on clipping only
	}
	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // just max over angles -- blobify!
	  sfc.x = ang;
	  float ctr_val = MatMotEl(&v1s_out_r, sfc.x, sfc.y, scc.x, scc.y, v1s_mot_idx);
	  ctr_val *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
	  max_rf = MAX(max_rf, ctr_val);
	}
      }
    }
    cur_out->FastEl(fc.x, fc.y, cc.x, cc.y) = max_rf;
  }
}

void V1RegionSpec::V1ComplexFilter_Blob_Binocular_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;			// complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);
  TwoDCoord scs = v1c_specs.spacing * cc; // v1s coords start
  scs += v1c_specs.border;
  scs -= v1c_specs.spat_rf_half; // convert to lower-left starting position, not center

  int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord sc;			// simple coord
  TwoDCoord scc;		// simple coord, center
  TwoDCoord sfc;		// v1s feature coords
  TwoDCoord fc;			// v1c feature coords
  for(int dog = 0; dog < dog_feat_geom.n; dog++) { // dog features -- includes b/w on/off
    sfc.y = dog;
    fc.y = v1c_feat_blob_y + dog / v1c_feat_geom.x;
    fc.x = dog % v1c_feat_geom.x;
    float max_rf = 0.0f;   // max over spatial rfield
    for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // ysimple
      sc.y = scs.y + ys;
      for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xsimple
	sc.x = scs.x + xs;
	scc = sc;	// center
	if(scc.WrapClip(wrap, v1b_img_geom)) {
	  if(edge_mode == CLIP) continue; // bail on clipping only
	}
	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // just max over angles -- blobify!
	  sfc.x = ang;
	  float ctr_val = 0.0f;
	  for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
	    int didx = disp + v1b_specs.n_disps;
	    float cv = MatMotEl(&v1b_out, sfc.x, sfc.y + didx * v1s_feat_geom.y,
				scc.x, scc.y, v1s_mot_idx)* v1bc_weights.FastEl(didx);
	    ctr_val = MAX(ctr_val, cv);
	  }
	  ctr_val *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
	  max_rf = MAX(max_rf, ctr_val);
	}
      }
    }
    cur_out->FastEl(fc.x, fc.y, cc.x, cc.y) = max_rf;
  }
}

void V1RegionSpec::V1ComplexFilter_DispEdge_thread(int v1c_idx, int thread_no) {
}

void V1RegionSpec::V1ComplexFilter_MotionEdge_Monocular_thread(int v1c_idx, int thread_no) {
}

void V1RegionSpec::V1ComplexFilter_MotionEdge_Binocular_thread(int v1c_idx, int thread_no) {
}

bool V1RegionSpec::V1CRenormOutput_EsLsBlob(float_Matrix* out) {
  // logic here is to compute max's separately and renorm separately for each 
  // type of filter, and then combine the max's in the middle to achieve desired
  // groupings of max's.  results overall indicate that only breaking out blob vs
  // others is best

  bool rval = false;
  float es_max_val = 0.0f;
  float ls_max_val = 0.0f;
  float smax_max_val = 0.0f;
  float blob_m_max_val = 0.0f;
  float blob_c_max_val = 0.0f;
  TwoDCoord cc;		// complex coords
  TwoDCoord fc;		// v1s feature coords
  for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
    for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
      if(v1c_filters & END_STOP) {
	fc.y=v1c_feat_es_y;
	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	  fc.x = ang;
	  float val = out->FastEl(fc.x, fc.y, cc.x, cc.y);
	  es_max_val = MAX(val, es_max_val);
	}
      }
      if(v1c_filters & LEN_SUM) {
	fc.y=v1c_feat_ls_y;
	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	  fc.x = ang;
	  float val = out->FastEl(fc.x, fc.y, cc.x, cc.y); // long
	  ls_max_val = MAX(val, ls_max_val);
	}
      }
      if(v1c_filters & V1S_MAX) {
	for(int dog = 0; dog < 2; dog++) {
	  fc.y=v1c_feat_smax_y + dog;
	  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	    fc.x = ang;
	    float val = out->FastEl(fc.x, fc.y, cc.x, cc.y); // long
	    smax_max_val = MAX(val, smax_max_val);
	  }
	}
      }
      if(v1c_filters & BLOB) {
	fc.y=v1c_feat_blob_y;
	for(int pol = 0; pol < 2; pol++) { // polarities
	  fc.x = pol;
	  float val = out->FastEl(fc.x, fc.y, cc.x, cc.y);
	  blob_m_max_val = MAX(val, blob_m_max_val);
	}
	if(color == COLOR) {
	  for(int pol = 2; pol < 8; pol++) { // polarities
	    fc.x = pol % v1c_feat_geom.x;
	    float val = out->FastEl(fc.x, fc.y + pol / v1c_feat_geom.x, cc.x, cc.y);
	    blob_c_max_val = MAX(val, blob_c_max_val);
	  }
	}
      }
    }
  }

  // all the line guys are pooled
  es_max_val = MAX(es_max_val, ls_max_val);
  es_max_val = MAX(es_max_val, smax_max_val);
  ls_max_val = es_max_val;
  smax_max_val = es_max_val;

  if(es_max_val > renorm_thr) {	// nonblank
    rval = true;
    if(v1c_renorm == LIN_RENORM) {
      float es_rescale = 1.0f / (es_max_val > 0.0f ? es_max_val : 1.0f);
      float ls_rescale = 1.0f / (ls_max_val > 0.0f ? ls_max_val : 1.0f);
      float smax_rescale = 1.0f/ (smax_max_val > 0.0f ? smax_max_val : 1.0f);
      float blob_m_rescale = 1.0f / (blob_m_max_val > 0.0f ? blob_m_max_val : 1.0f);
      float blob_c_rescale = 1.0f / (blob_c_max_val > 0.0f ? blob_c_max_val : 1.0f);
      for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
	for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
	  if(v1c_filters & END_STOP) {
	    fc.y=v1c_feat_es_y;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float& val = out->FastEl(fc.x, fc.y, cc.x, cc.y);
	      val *= es_rescale;
	    }
	  }
	  if(v1c_filters & LEN_SUM) {
	    fc.y=v1c_feat_ls_y;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float& val = out->FastEl(fc.x, fc.y, cc.x, cc.y);
	      val *= ls_rescale;
	    }
	  }
	  if(v1c_filters & V1S_MAX) {
	    for(int dog = 0; dog < 2; dog++) {
	      fc.y=v1c_feat_smax_y + dog;
	      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
		fc.x = ang;
		float& val = out->FastEl(fc.x, fc.y, cc.x, cc.y);
		val *= smax_rescale;
	      }
	    }
	  }
	  if(v1c_filters & BLOB) {
	    fc.y=v1c_feat_blob_y;
	    for(int pol = 0; pol < 2; pol++) { // polarities
	      fc.x = pol;
	      float& val = out->FastEl(fc.x, fc.y, cc.x, cc.y);
	      val *= blob_m_rescale;
	    }
	    if(color == COLOR) {
	      for(int pol = 2; pol < 8; pol++) { // polarities
		fc.x = pol % v1c_feat_geom.x;
		float& val = out->FastEl(fc.x, fc.y + pol / v1c_feat_geom.x, cc.x, cc.y);
		val *= blob_c_rescale;
	      }
	    }
	  }
	}
      }
    }
    else if(v1c_renorm == LOG_RENORM) {
      float es_rescale = 1.0f / logf(1.0f + (es_max_val > 0.0f ? es_max_val : 1.0f));
      float ls_rescale = 1.0f / logf(1.0f + (ls_max_val > 0.0f ? ls_max_val : 1.0f));
      float smax_rescale = 1.0f / logf(1.0f + (smax_max_val > 0.0f ? smax_max_val : 1.0f));
      float blob_m_rescale = 1.0f / logf(1.0f + (blob_m_max_val > 0.0f ? blob_m_max_val : 1.0f));
      float blob_c_rescale = 1.0f / logf(1.0f + (blob_c_max_val > 0.0f ? blob_c_max_val : 1.0f));
      for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
	for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
	  if(v1c_filters & END_STOP) {
	    fc.y=v1c_feat_es_y;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float& val = out->FastEl(fc.x, fc.y, cc.x, cc.y);
	      val = logf(1.0f + val) * es_rescale;
	    }
	  }
	  if(v1c_filters & LEN_SUM) {
	    fc.y=v1c_feat_ls_y;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float& val = out->FastEl(fc.x, fc.y, cc.x, cc.y);
	      val = logf(1.0f + val) * ls_rescale;
	    }
	  }
	  if(v1c_filters & V1S_MAX) {
	    for(int dog = 0; dog < 2; dog++) {
	      fc.y=v1c_feat_smax_y + dog;
	      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
		fc.x = ang;
		float& val = out->FastEl(fc.x, fc.y, cc.x, cc.y);
		val = logf(1.0f + val) * smax_rescale;
	      }
	    }
	  }
	  if(v1c_filters & BLOB) {
	    fc.y=v1c_feat_blob_y;
	    for(int pol = 0; pol < 2; pol++) { // polarities
	      fc.x = pol;
	      float& val = out->FastEl(fc.x, fc.y, cc.x, cc.y);
	      val = logf(1.0f + val) * blob_m_rescale;
	    }
	    if(color == COLOR) {
	      for(int pol = 2; pol < 8; pol++) { // polarities
		fc.x = pol % v1c_feat_geom.x;
		float& val = out->FastEl(fc.x, fc.y + pol / v1c_feat_geom.x, cc.x, cc.y);
		val = logf(1.0f + val) * blob_c_rescale;
	      }
	    }
	  }
	}
      }
    }
  }
  return rval;
}


////////////////////////////////////////////////////////////////////
//	V1Region 	Data Table Output

bool V1RegionSpec::InitDataTable() {
  inherited::InitDataTable();
  if(!data_table) {
    if(!(image_save & SAVE_DATA) && !(dog_save & SAVE_DATA)) return true;
    return false;
  }

  int idx;
  DataCol* col;
  // SIMPLE
  if(v1s_save & SAVE_DATA) {
    if(v1s_save & SEP_MATRIX) {
      // break out into sub-structure: bw, color, motion
      data_table->FindMakeColName(name + "_v1s_bw_r", idx, DataTable::VT_FLOAT, 4,
			    v1s_feat_geom.x, 2, v1s_img_geom.x, v1s_img_geom.y);
      if(ocularity == BINOCULAR)
	data_table->FindMakeColName(name + "_v1s_bw_l", idx, DataTable::VT_FLOAT, 4,
			    v1s_feat_geom.x, 2, v1s_img_geom.x, v1s_img_geom.y);
      if(color == COLOR) {
	data_table->FindMakeColName(name + "_v1s_clr_r", idx, DataTable::VT_FLOAT, 4,
				    v1s_feat_geom.x, 4, v1s_img_geom.x, v1s_img_geom.y);
	if(ocularity == BINOCULAR)
	  data_table->FindMakeColName(name + "_v1s_clr_l", idx, DataTable::VT_FLOAT, 4,
				      v1s_feat_geom.x, 4, v1s_img_geom.x, v1s_img_geom.y);
      }
      if(motion_frames > 1) {
	data_table->FindMakeColName(name + "_v1s_mot_r", idx, DataTable::VT_FLOAT, 4,
		    v1s_feat_geom.x, 4 * v1s_motion.n_speeds, v1s_img_geom.x, v1s_img_geom.y);
	if(ocularity == BINOCULAR)
	  data_table->FindMakeColName(name + "_v1s_mot_l", idx, DataTable::VT_FLOAT, 4,
		      v1s_feat_geom.x, 4 * v1s_motion.n_speeds, v1s_img_geom.x, v1s_img_geom.y);
      }
    }
    else {
      col = data_table->FindMakeColName(name + "_v1s_r", idx, DataTable::VT_FLOAT, 4,
		v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
      if(ocularity == BINOCULAR)
	col = data_table->FindMakeColName(name + "_v1s_l", idx, DataTable::VT_FLOAT, 4,
		v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    }
  }

  // BINOCULAR
  if(ocularity == BINOCULAR && v1b_save & SAVE_DATA) {
    if(v1b_save & SEP_MATRIX) {
      // break out into sub-structure: bw, color, motion
      data_table->FindMakeColName(name + "_v1b_bw", idx, DataTable::VT_FLOAT, 4,
			    v1b_feat_geom.x, v1b_specs.tot_disps * 2, v1b_img_geom.x, v1b_img_geom.y);
      if(color == COLOR) {
	data_table->FindMakeColName(name + "_v1b_clr", idx, DataTable::VT_FLOAT, 4,
				    v1b_feat_geom.x, v1b_specs.tot_disps * 4, v1b_img_geom.x, v1b_img_geom.y);
      }
      if(motion_frames > 1) {
	data_table->FindMakeColName(name + "_v1b_mot", idx, DataTable::VT_FLOAT, 4,
		    v1b_feat_geom.x, v1b_specs.tot_disps * 4 * v1s_motion.n_speeds, v1b_img_geom.x, v1b_img_geom.y);
      }
    }
    else {
      col = data_table->FindMakeColName(name + "_v1b", idx, DataTable::VT_FLOAT, 4,
		v1b_feat_geom.x, v1b_feat_geom.y, v1b_img_geom.x, v1b_img_geom.y);
    }
    col = data_table->FindMakeColName(name + "_v1b_disp_raw", idx, DataTable::VT_FLOAT, 4,
				      v1b_specs.tot_disps, 1, v1b_img_geom.x, v1b_img_geom.y);
    col = data_table->FindMakeColName(name + "_v1b_disp", idx, DataTable::VT_FLOAT, 4,
				      v1b_specs.tot_disps, 1, v1b_img_geom.x, v1b_img_geom.y);
    col = data_table->FindMakeColName(name + "_v1b_dgp", idx, DataTable::VT_FLOAT, 4,
				      v1b_specs.tot_disps, 1, v1b_dgp_geom.x, v1b_dgp_geom.y);
  }

  // COMPLEX
  if(v1c_save & SAVE_DATA) {
    if(v1c_save & SEP_MATRIX) {
      // break out into sub-structure: end-stop/len-sum, other edges -- assumes both of each present
      if(v1c_filters & CF_ESLS) {
	data_table->FindMakeColName(name + "_v1c_esls", idx, DataTable::VT_FLOAT, 4,
				    v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
      }
      if(v1c_filters & BLOB) {
	data_table->FindMakeColName(name + "_v1c_blob", idx, DataTable::VT_FLOAT, 4,
				    v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
      }
      if(v1c_filters & V1S_MAX) {
	data_table->FindMakeColName(name + "_v1c_smax", idx, DataTable::VT_FLOAT, 4,
				    v1c_feat_geom.x, dog_feat_geom.n, v1c_img_geom.x, v1c_img_geom.y);
      }
      if(v1c_filters & CF_EDGES) {
	data_table->FindMakeColName(name + "_v1c_edge", idx, DataTable::VT_FLOAT, 4,
				    v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
      }
    }
    else {
      col = data_table->FindMakeColName(name + "_v1c", idx, DataTable::VT_FLOAT, 4,
		v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);
    }
  }
  return true;
}

bool V1RegionSpec::V1SOutputToTable(DataTable* dtab) {
  V1SOutputToTable_impl(dtab, &v1s_out_r, &v1s_circ_r, "_r");
  if(ocularity == BINOCULAR) 
    V1SOutputToTable_impl(dtab, &v1s_out_l, &v1s_circ_l, "_l");
  return true;
}

bool V1RegionSpec::V1SOutputToTable_impl(DataTable* dtab, float_Matrix* out, CircMatrix* circ,
					 const String& col_sufx) {
  int mot_idx = circ->CircIdx_Last();
  TwoDCoord sc;		// simple coords
  TwoDCoord fc;		// v1s feature coords
  DataCol* col;
  int idx;
  if(v1s_save & SEP_MATRIX) {
    { // basic luminance b/w filters
      col = data_table->FindMakeColName(name + "_v1s_bw" + col_sufx, idx, DataTable::VT_FLOAT, 4,
					v1s_feat_geom.x, 2, v1s_img_geom.x, v1s_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	  for(int dog = 0; dog < 2; dog++) { // dog features -- just first 2
	    fc.y = dog;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float val = MatMotEl(out, fc.x, fc.y, sc.x, sc.y, mot_idx);
	      dout->FastEl(fc.x, fc.y, sc.x, sc.y) = val;
	    }
	  }
	}
      }
    }
    if(color == COLOR) {
      col = data_table->FindMakeColName(name + "_v1s_clr" + col_sufx, idx, DataTable::VT_FLOAT, 4,
			  v1s_feat_geom.x, 4, v1s_img_geom.x, v1s_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	  for(int dog = 2; dog < dog_feat_geom.n; dog++) { // dog features -- just color
	    fc.y = dog;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float val = MatMotEl(out, fc.x, fc.y, sc.x, sc.y, mot_idx);
	      dout->FastEl(fc.x, fc.y-2, sc.x, sc.y) = val;
	    }
	  }
	}
      }
    }
    if(motion_frames > 1) {
      col = data_table->FindMakeColName(name + "_v1s_mot" + col_sufx, idx, DataTable::VT_FLOAT, 4,
		  v1s_feat_geom.x, 4 * v1s_motion.n_speeds, v1s_img_geom.x, v1s_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	  for(int mot = v1s_feat_mot_y; mot < v1s_feat_geom.y; mot++) {
	    fc.y = mot;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float val = MatMotEl(out, fc.x, fc.y, sc.x, sc.y, mot_idx);
	      dout->FastEl(fc.x, fc.y-v1s_feat_mot_y, sc.x, sc.y) = val;
	    }
	  }
	}
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_v1s" + col_sufx, idx, DataTable::VT_FLOAT, 4,
	      v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    if(motion_frames <= 1) {
      dout->CopyFrom(out);
    }
    else {
      float_MatrixPtr lstfrm; lstfrm = (float_Matrix*)out->GetFrameSlice(circ->CircIdx_Last());
      dout->CopyFrom(lstfrm);
    }
  }
  return true;
}

bool V1RegionSpec::V1BOutputToTable(DataTable* dtab) {
  DataCol* col;
  int idx;
  TwoDCoord sc;		// simple coords
  TwoDCoord fc;		// v1s feature coords
  if(v1b_save & SEP_MATRIX) {
    { // basic luminance b/w filters
      col = data_table->FindMakeColName(name + "_v1b_bw" , idx, DataTable::VT_FLOAT, 4,
		v1b_feat_geom.x, v1b_specs.tot_disps * 2, v1b_img_geom.x, v1b_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(sc.y = 0; sc.y < v1b_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1b_img_geom.x; sc.x++) {
	  for(int dog = 0; dog < 2; dog++) { // dog features -- just first 2
	    fc.y = dog;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
		float val = v1b_out.FastEl(fc.x, fc.y + didx * v1s_feat_geom.y, sc.x, sc.y);
		dout->FastEl(fc.x, fc.y + didx * 2, sc.x, sc.y) = val;
	      }
	    }
	  }
	}
      }
    }
    if(color == COLOR) {
      col = data_table->FindMakeColName(name + "_v1b_clr" , idx, DataTable::VT_FLOAT, 4,
			  v1b_feat_geom.x, v1b_specs.tot_disps * 4, v1b_img_geom.x, v1b_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(sc.y = 0; sc.y < v1b_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1b_img_geom.x; sc.x++) {
	  for(int dog = 2; dog < dog_feat_geom.n; dog++) { // dog features -- just color
	    fc.y = dog;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
		float val = v1b_out.FastEl(fc.x, fc.y + didx * v1s_feat_geom.y, sc.x, sc.y);
		dout->FastEl(fc.x, fc.y-2 + didx * 4, sc.x, sc.y) = val;
	      }
	    }
	  }
	}
      }
    }
    if(motion_frames > 1) {
      col = data_table->FindMakeColName(name + "_v1b_mot" , idx, DataTable::VT_FLOAT, 4,
				v1b_feat_geom.x, v1b_specs.tot_disps * 4 * v1s_motion.n_speeds,
					v1b_img_geom.x, v1b_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(sc.y = 0; sc.y < v1b_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1b_img_geom.x; sc.x++) {
	  for(int mot = v1s_feat_mot_y; mot < v1b_feat_geom.y; mot++) {
	    fc.y = mot;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
		float val = v1b_out.FastEl(fc.x, fc.y + didx * v1s_feat_geom.y, sc.x, sc.y);
		dout->FastEl(fc.x, fc.y-v1s_feat_mot_y + didx * 4 * v1s_motion.n_speeds,
			     sc.x, sc.y) = val;
	      }
	    }
	  }
	}
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_v1b", idx, DataTable::VT_FLOAT, 4,
		      v1b_feat_geom.x, v1b_feat_geom.y, v1b_img_geom.x, v1b_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(&v1b_out);
  }
  {
    col = data_table->FindMakeColName(name + "_v1b_disp_raw", idx, DataTable::VT_FLOAT, 4,
				      v1b_specs.tot_disps, 1, v1b_img_geom.x, v1b_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(&v1b_disp_raw);
  }
  {
    col = data_table->FindMakeColName(name + "_v1b_disp", idx, DataTable::VT_FLOAT, 4,
				      v1b_specs.tot_disps, 1, v1b_img_geom.x, v1b_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(&v1b_disp_out);
  }
  {
    col = data_table->FindMakeColName(name + "_v1b_dgp", idx, DataTable::VT_FLOAT, 4,
				      v1b_specs.tot_disps, 1, v1b_dgp_geom.x, v1b_dgp_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(&v1b_dgp_out);
  }
  return true;
}

bool V1RegionSpec::V1COutputToTable(DataTable* dtab) {
  DataCol* col;
  int idx;
  if(v1c_save & SEP_MATRIX) {
    // todo: write this
    // break out into sub-structure: end-stop/len-sum, other edges -- assumes both of each present
//       if(v1c_filters & CF_ESLS) {
// 	data_table->FindMakeColName(name + "_v1c_esls", idx, DataTable::VT_FLOAT, 4,
// 				    v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
//       }
//       if(v1c_filters & BLOB) {
// 	data_table->FindMakeColName(name + "_v1c_blob", idx, DataTable::VT_FLOAT, 4,
// 				    v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
//       }
//       if(v1c_filters & V1S_MAX) {
// 	data_table->FindMakeColName(name + "_v1c_smax", idx, DataTable::VT_FLOAT, 4,
// 				    v1c_feat_geom.x, dog_feat_geom.n, v1c_img_geom.x, v1c_img_geom.y);
//       }
//       if(v1c_filters & CF_EDGES) {
// 	data_table->FindMakeColName(name + "_v1c_edge", idx, DataTable::VT_FLOAT, 4,
// 				    v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
//       }
  }
  else {
    col = data_table->FindMakeColName(name + "_v1c", idx, DataTable::VT_FLOAT, 4,
		      v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(&v1c_out);
  }
  return true;
}

/////////////////////////////////////////////////////
//			Inverting

bool V1RegionSpec::InvertFilters_impl() {
  bool rval = true;

  if(save_mode == FIRST_ROW) {
    data_table->EnforceRows(1);
    data_table->WriteItem(0);
    data_table->ReadItem(0);
  }
  else {
    data_table->AddBlankRow();
  }

//   if(TestWarning(ocularity == BINOCULAR, "InvertFilters",
// 	 "binocular inversion not currently supported -- only monocular will be rendered"));

  V1ComplexInvertFilter();

  V1SimpleInvertFilter_Static(&dog_out_r, &dog_circ_r, &v1s_out_r_raw, &v1s_out_r, 
			      &v1s_circ_r);
//   if(rval && ocularity == BINOCULAR) {
//     rval &= V1SimpleFilter_Static(&dog_out_l, &dog_circ_l, &v1s_out_l_raw, &v1s_out_l,
// 				  &v1s_circ_l);
//   }


  DoGInvertFilter(&dog_out_r, &dog_circ_r, "_r");
//   if(rval && ocularity == BINOCULAR) {
//     rval &= DoGInvertFilter(&dog_out_l, &dog_circ_l, "_l");
//   }

  if(dog_save & SAVE_DATA && !(!taMisc::gui_active && dog_save & ONLY_GUI)) {
    DoGOutputToTable(data_table);
  }
  if(v1s_save & SAVE_DATA && !(!taMisc::gui_active && v1s_save & ONLY_GUI)) {
    V1SOutputToTable(data_table);
  }
  if(ocularity == BINOCULAR && v1b_save & SAVE_DATA && !(taMisc::gui_active && v1b_save & ONLY_GUI)) {
    V1BOutputToTable(data_table);
  }
  if(v1c_save & SAVE_DATA && !(!taMisc::gui_active && v1c_save & ONLY_GUI)) {
    V1COutputToTable(data_table);
  }

  return rval;
}

bool V1RegionSpec::V1ComplexInvertFilter() {
  v1s_out_r.InitVals(0.0f);

  if(ocularity == BINOCULAR) {
//     if(v1c_filters & CF_ESLS) {
//       V1ComplexFilter_EsLs_Binocular();
//     }
//     if(v1c_filters & V1S_MAX) {
//       V1ComplexFilter_V1SMax_Binocular();
//     }
//     if(v1c_filters & BLOB) {
//       V1ComplexFilter_Blob_Binocular();
//     }
//     if(v1c_filters & DISP_EDGE) {
//       V1ComplexFilter_DispEdge();
//     }
//     if(motion_frames > 1 && v1c_filters & MOTION_EDGE) {
//       V1ComplexFilter_MotionEdge_Binocular();
//     }
  }
  else {
//     if(v1c_filters & CF_ESLS) {
//       V1ComplexInvertFilter_EsLs_Monocular();
//     }
    if(v1c_filters & V1S_MAX) {
      V1ComplexInvertFilter_V1SMax_Monocular();
    }
//     if(v1c_filters & BLOB) {
//       V1ComplexInvertFilter_Blob_Monocular();
//     }
//     if(motion_frames > 1 && v1c_filters & MOTION_EDGE) {
//       V1ComplexInvertFilter_MotionEdge_Monocular();
//     }
  }
  return true;
}

bool V1RegionSpec::V1ComplexInvertFilter_EsLs_Monocular() {
  if(!(v1c_filters & LEN_SUM)) return false; // only does len sum - end stop not worth inverting

  int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord cc;			// complex coords
  TwoDCoord sc;			// simple coord
  TwoDCoord sce;		// simple coord, ends
  TwoDCoord scc;		// simple coord, center
  TwoDCoord fc;			// v1c feature coords
  TwoDCoord sfc_ctr;		// simple feature coords for the central point
  TwoDCoord sfc_end;		// simple feature coords for the end point
  
  for(cc.y=0; cc.y < v1c_img_geom.y; cc.y++) {
    for(cc.x=0; cc.x < v1c_img_geom.x; cc.x++) {
      TwoDCoord scs = v1c_specs.spacing * cc; // v1s coords start
      scs += v1c_specs.border;
      scs -= v1c_specs.spat_rf_half; // convert to lower-left starting position, not center

      int cfeat = 1;		// 
      fc.y = v1c_feat_ls_y;

      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	fc.x = ang;
	float v1c_act = v1c_out.FastEl(fc.x, fc.y, cc.x, cc.y);
	int v1sf = 0;		// just choose first feature -- on center -- monochrome only!
	if(v1c_filters & V1S_MAX) { // use v1smax to figure it out
	  float v1c_smax_on = v1c_out.FastEl(fc.x, v1c_feat_smax_y, cc.x, cc.y);
	  float v1c_smax_off = v1c_out.FastEl(fc.x, v1c_feat_smax_y+1, cc.x, cc.y);
	  if(v1c_smax_off > v1c_smax_on) v1sf = 1; // switch, else keep with 0
	}
	sfc_ctr.x = ang;
	sfc_ctr.y = v1sf;
	sfc_end.x = ang;
	if(cfeat == 0)	{	// end stop
	  sfc_end.y = 1 - v1sf;  // opposite polarity -- flip bit in odd/even within same set of 2
	}
	else {
	  sfc_end.y = v1sf;
	}

	int ys = v1c_specs.spat_rf_half.y;
	sc.y = scs.y + ys;
	int xs = v1c_specs.spat_rf_half.x;
	sc.x = scs.x + xs;
	scc = sc;	// center
	if(scc.WrapClip(wrap, v1s_img_geom)) {
	  if(edge_mode == CLIP) continue; // bail on clipping only
	}

	MatMotEl(&v1s_out_r, sfc_ctr.x, sfc_ctr.y, scc.x, scc.y, v1s_mot_idx) = v1c_act;

	for(int lpt=-v1c_specs.len_sum_len; lpt <= v1c_specs.len_sum_len; lpt++) {
	  if(lpt == 0) continue; // skip center
	  int lpidx = lpt + v1c_specs.len_sum_len;
	  int xp = v1c_ls_stencils.FastEl(X,lpidx,ang);
	  int yp = v1c_ls_stencils.FastEl(Y,lpidx,ang);
	  sce.x = sc.x + xp;
	  sce.y = sc.y + yp;

	  if(sce.WrapClip(wrap, v1s_img_geom)) {
	    if(edge_mode == CLIP) continue; // bail on clipping only
	  }

	  MatMotEl(&v1s_out_r, sfc_end.x, sfc_end.y, sce.x, sce.y, v1s_mot_idx) = v1c_act;
	}
      }
    }
  }
  return true;
}

bool V1RegionSpec::V1ComplexInvertFilter_V1SMax_Monocular() {
  int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord cc;			// complex coords
  TwoDCoord sc;			// simple coord
  TwoDCoord scc;		// simple coord, center
  TwoDCoord sfc;		// v1s feature coords
  TwoDCoord fc;			// v1c feature coords
  for(cc.y=0; cc.y < v1c_img_geom.y; cc.y++) {
    for(cc.x=0; cc.x < v1c_img_geom.x; cc.x++) {
      TwoDCoord scs = v1c_specs.spacing * cc; // v1s coords start
      scs += v1c_specs.border;
      scs -= v1c_specs.spat_rf_half; // convert to lower-left starting position, not center

      for(int dog = 0; dog < 2; dog++) { // only first monochrome on/off guys
	sfc.y = dog;
	fc.y = v1c_feat_smax_y + dog;
	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	  sfc.x = ang;
	  fc.x = ang;

	  float v1c_act = cur_out->FastEl(fc.x, fc.y, cc.x, cc.y);

	  // just pick central point
	  int ys = v1c_specs.spat_rf_half.y;
	  sc.y = scs.y + ys;
	  int xs = v1c_specs.spat_rf_half.x;
	  sc.x = scs.x + xs;
	  scc = sc;	// center
	  if(scc.WrapClip(wrap, v1s_img_geom)) {
	    if(edge_mode == CLIP) continue; // bail on clipping only
	  }

	  MatMotEl(&v1s_out_r, sfc.x, sfc.y, scc.x, scc.y, v1s_mot_idx) = v1c_act;
	} // for ang
      }  // for dog
    }
  }
  return true;
}

bool V1RegionSpec::V1SimpleInvertFilter_Static(float_Matrix* dog, CircMatrix* dog_circ,
					 float_Matrix* out_raw, float_Matrix* out,
					 CircMatrix* circ) {
  dog->InitVals(0.0f);

  int dog_mot_idx = dog_circ->CircIdx_Last(); // always write to last position
  int mot_idx = circ->CircIdx_Last(); // always write to last position

  TwoDCoord dc;			// dog coord
  TwoDCoord fc;			// v1s feature coords
  TwoDCoord dfc;		// dog feature coords
  TwoDCoord sc;			// simple coords
  for(sc.y=0; sc.y < v1s_img_geom.y; sc.y++) {
    for(sc.x=0; sc.x < v1s_img_geom.x; sc.x++) {
      TwoDCoord dcs = v1s_specs.spacing * sc; // dog coords start
      dcs += v1s_specs.border;
      dcs -= v1s_specs.rf_half;	// convert to lower-left starting position, not center

      for(int dogdx = 0; dogdx < dog_feat_geom.n; dogdx++) { // dog features
	dfc.SetFmIndex(dogdx, dog_feat_geom.x);
	fc.y = dogdx;
	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	  fc.x = ang;

	  float v1s_act = MatMotEl(out, fc.x, fc.y, sc.x, sc.y, mot_idx);
	  int lpos = v1s_specs.rf_half;		     // choose half-way point
	  for(int lpt = 0; lpt < v1s_specs.rf_size; lpt++) { // line points
	    int xp = v1s_stencils.FastEl(X,lpt,lpos,ang);
	    int yp = v1s_stencils.FastEl(Y,lpt,lpos,ang);
	    dc.x = dcs.x + xp;
	    dc.y = dcs.y + yp;

	    if(dc.WrapClip(wrap, dog_img_geom)) {
	      if(edge_mode == CLIP) continue; // bail on clipping only
	    }

	    MatMotEl(dog, dfc.x, dfc.y, dc.x, dc.y, dog_mot_idx) = v1s_act;
	  }
	}
      }
    }
  }
  return true;
}

/////////////////////////////////////////////////////
//			Graphing

void V1RegionSpec::GridGaborFilters(DataTable* graph_data) {
  Init();			// need to init filters here

  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_Gabor_GridFilter", true);
  }
  graph_data->StructUpdate(true);
  graph_data->Reset();

  for(int i=0;i<gabor_filters.size;i++) {
    GaborFilter* gf = (GaborFilter*)gabor_filters[i];
    gf->GridFilter(graph_data, false); // don't reset!
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

void V1RegionSpec::GridV1Stencils(DataTable* graph_data) {
  Init();			// need to init stencils for sure!

  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GridV1Stencils", true);
  }
  graph_data->StructUpdate(true);
  graph_data->Reset();

  graph_data->SetUserData("N_ROWS", 4);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);
  graph_data->SetUserData("BLOCK_SPACE", 4.0f);
  //  graph_data->SetUserData("WIDTH", .5f + (float)retina_size.x / (float)retina_size.y);

  TwoDCoord max_sz(v1s_specs.rf_size, v1s_specs.rf_size);
  max_sz.Max(v1c_specs.spat_rf);

  int bin_rf_max = 5;;
  if(ocularity == BINOCULAR) {
    bin_rf_max = v1b_specs.n_disps * v1b_specs.disp_off + v1b_specs.tuning_width;
    TwoDCoord bin_max(v1b_specs.tot_disps * bin_rf_max + 2*v1b_specs.end_width, 2);
    max_sz.Max(bin_max);
    TwoDCoord rf_max(v1b_dgp_specs.dgp_rf_sz.x, v1b_dgp_specs.dgp_rf_sz.y);
    max_sz.Max(rf_max);
  }
  
  int mot_rf_max = 5;
  if(motion_frames > 1) {
    mot_rf_max = motion_frames * (1 << v1s_motion.n_speeds) + v1s_motion.tuning_width;
    TwoDCoord mot_max(motion_frames * mot_rf_max, motion_frames * mot_rf_max);
    max_sz.Max(mot_max);
  }

  TwoDCoord brd(5,5);		// border
  max_sz += brd * 2;

  TwoDCoord half_sz = max_sz / 2;

  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
  nmda->SetUserData("WIDTH", 24);
  DataCol* matda = graph_data->FindMakeColName("Stencil", idx, VT_FLOAT, 2,
					      max_sz.x, max_sz.y);

  { // v1simple, static
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1S " + String(ang * 45), -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      TwoDCoord ic;
      for(int lpos = 0; lpos < v1s_specs.rf_size; lpos++) { // line starting positions
	for(int lpt = 0; lpt < v1s_specs.rf_size; lpt++) { // line pixels
	  int xp = v1s_stencils.FastEl(X,lpt,lpos,ang);
	  int yp = v1s_stencils.FastEl(Y,lpt,lpos,ang);
	  ic.x = brd.x + xp;
	  ic.y = brd.y + yp;

	  if(ic.WrapClip(true, max_sz)) continue;
	  mat->FastEl(ic.x,ic.y) = (lpos % 2 == 0) ? 1.0f: 0.5f;
	}
      }
    }
  }

  if(ocularity == BINOCULAR) { // v1b
    { // basic stencils
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1b Binoc", -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      TwoDCoord ic;
      TwoDCoord dc;
      for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
	int didx = disp + v1b_specs.n_disps;
	int dwd = v1b_widths.FastEl(didx);
	ic.y = half_sz.y;
	ic.x = brd.x + v1b_specs.end_width + v1b_specs.disp_off*v1b_specs.n_disps + didx * bin_rf_max;
	    
	if(ic.WrapClip(true, max_sz)) continue;
	mat->FastEl(ic.x,ic.y-1) = -0.5f;

	for(int twidx = 0; twidx < dwd; twidx++) {
	  int off = v1b_stencils.FastEl(twidx, didx);
	  dc = ic;
	  dc.x += off;
	  if(dc.WrapClip(true, max_sz)) continue;
	  mat->FastEl(dc.x,dc.y) = v1b_weights.FastEl(twidx, didx);
	}
      }
    }
    { // v1b_dgp weights
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1 Binoc dGp Wts", -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      TwoDCoord sc;
      for(int ys = 0; ys < v1b_dgp_specs.dgp_rf_sz.y; ys++) { // ysimple
	sc.y = brd.y + ys;
	for(int xs = 0; xs < v1b_dgp_specs.dgp_rf_sz.x; xs++) { // xsimple
	  sc.x = brd.x + xs;
	  if(sc.WrapClip(true, max_sz)) continue;
	  mat->FastEl(sc.x,sc.y) = v1b_dgp_weights.FastEl(xs, ys);
	}
      }
    }
  }

  if(motion_frames > 1) { // v1simple, motion
    for(int speed = 0; speed < v1s_motion.n_speeds; speed++) { // speed
      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	for(int dir = 0; dir < 2; dir++) { // directions
	  int dirsign = (dir == 0) ? 1 : -1; // direction sign for multiplying times slope values
	  graph_data->AddBlankRow();
	  nmda->SetValAsString("V1m sp:" + String(speed) + " ang:" + String(ang * 45) +
			       " dir:" + String(dir == 0 ? "-" : "+"), -1);
	  float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
	  TwoDCoord ic;
	  for(int mot = 0; mot < motion_frames; mot++) { // time steps back in time
	    ic.y = half_sz.y;
	    ic.x = brd.x + mot * mot_rf_max;
	    
	    // offset along line to prevent overwrite
	    ic.x += dirsign * v1s_ang_slopes.FastEl(X, LINE, ang);
	    ic.y += dirsign * v1s_ang_slopes.FastEl(Y, LINE, ang);

	    if(ic.WrapClip(true, max_sz)) continue;
	    mat->FastEl(ic.x,ic.y) = -0.5f;

	    for(int tw = -v1s_motion.tuning_width; tw <= v1s_motion.tuning_width; tw++) {
	      int twidx = v1s_motion.tuning_width+tw;
	      int xp = v1m_stencils.FastEl(X, twidx, mot, dir, ang, speed);
	      int yp = v1m_stencils.FastEl(Y, twidx, mot, dir, ang, speed);
	      ic.x = brd.x + xp + (motion_frames-1 -mot) * mot_rf_max;
	      ic.y = half_sz.y + yp;
	      if(ic.WrapClip(true, max_sz)) continue;
	      float mot_val = 1.0f; // color coding not necc: - (float)mot * (1.0f / (float)(motion_frames+2));
	      mat->FastEl(ic.x,ic.y) = mot_val * v1m_weights.FastEl(twidx);
	    }
	  }
	}
      }
    }
  }

  { // v1complex, spatial
    graph_data->AddBlankRow();
    nmda->SetValAsString("V1 Complex Spat RF", -1);
    float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
    TwoDCoord sc;
    for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // ysimple
      sc.y = brd.y + ys;
      for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xsimple
	sc.x = brd.x + xs;

	if(sc.WrapClip(true, max_sz)) continue;
	mat->FastEl(sc.x,sc.y) = v1c_weights.FastEl(xs, ys);
      }
    }
  }

  { // v1complex, es, ls
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1C End Stop Ang: " + String(ang * 45), -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      TwoDCoord ic;
      for(int lpt=-v1c_specs.end_stop_len; lpt <= v1c_specs.end_stop_len; lpt++) {
	//	if(lpt == 0) continue; // skip center
	int lpidx = lpt + v1c_specs.end_stop_len;
	int xp = v1c_es_stencils.FastEl(X,lpidx,ang);
	int yp = v1c_es_stencils.FastEl(Y,lpidx,ang);
	ic.x = brd.x + xp;
	ic.y = brd.y + yp;

	if(ic.WrapClip(true, max_sz)) continue;
	mat->FastEl(ic.x,ic.y) = (lpt == 0) ? 1.0f: -1.0f;
      }
    }
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1C Len Sum Ang: " + String(ang * 45), -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      TwoDCoord ic;
      for(int lpt=-v1c_specs.len_sum_len; lpt <= v1c_specs.len_sum_len; lpt++) {
	//	if(lpt == 0) continue; // skip center
	int lpidx = lpt + v1c_specs.len_sum_len;
	int xp = v1c_ls_stencils.FastEl(X,lpidx,ang);
	int yp = v1c_ls_stencils.FastEl(Y,lpidx,ang);
	ic.x = brd.x + xp;
	ic.y = brd.y + yp;

	if(ic.WrapClip(true, max_sz)) continue;
	mat->FastEl(ic.x,ic.y) = 1.0f;
      }
    }
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}


void V1RegionSpec::PlotSpacing(DataTable* graph_data, bool reset) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_V1_PlotSpacing", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->Reset();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
  nmda->SetUserData("WIDTH", 8);
  DataCol* matda = graph_data->FindMakeColName("Spacing", idx, VT_FLOAT, 2,
					      retina_size.x, retina_size.y);
  graph_data->SetUserData("N_ROWS", 1);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);
  graph_data->SetUserData("BLOCK_SPACE", 20.0f);
  graph_data->SetUserData("WIDTH", .5f + (float)retina_size.x / (float)retina_size.y);

  TwoDCoord ic;
  int x,y;
  { // first do dogs
    graph_data->AddBlankRow();
    nmda->SetValAsString("DoG", -1);
    float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
    for(y=border.y; y<= retina_size.y-border.y; y+= dog_spacing.y) {
      for(x=border.x; x<= retina_size.x-border.x; x+=dog_spacing.x) {
	ic.y = y; ic.x = x;
	ic.WrapClip(true, retina_size);	mat->FastEl(ic.x,ic.y) = 1.0f;
      }
    }
  }
  
  { // then v1 simple
    graph_data->AddBlankRow();
    nmda->SetValAsString("V1_Simple", -1);
    float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
    TwoDCoord brd(border.x+v1s_specs.border*dog_spacing.x,
		  border.y+v1s_specs.border*dog_spacing.y);
    TwoDCoord spc(dog_spacing.x * v1s_specs.spacing, dog_spacing.y * v1s_specs.spacing);
    // first render borders of RF's, every other
    for(y=brd.y; y<= retina_size.y-brd.y; y+= 2*spc.y) {
      for(x=brd.x; x<= retina_size.x-brd.x; x+= 2*spc.x) {
	ic.y = y; ic.x = x;
	ic -= v1s_specs.rf_half*dog_spacing; // lower left
	TwoDCoord ec;
	int ex,ey;
	for(ey=0; ey < v1s_specs.rf_size; ey++) {
	  ec.y = ic.y + ey*dog_spacing.y;  ec.x = ic.x;
	  ec.WrapClip(true, retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	  ec.y = ic.y + ey*dog_spacing.y;  ec.x = ic.x + dog_spacing.x * (v1s_specs.rf_size-1);
	  ec.WrapClip(true, retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	}
	for(ex=0; ex < v1s_specs.rf_size; ex++) {
	  ec.y = ic.y;	  ec.x = ic.x + ex*dog_spacing.x;
	  ec.WrapClip(true, retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	  ec.y = ic.y + dog_spacing.y * (v1s_specs.rf_size-1); ec.x = ic.x + ex*dog_spacing.x;
	  ec.WrapClip(true, retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	}
      }
    }
    // then centers
    for(y=brd.y; y<= retina_size.y-brd.y; y+= spc.y) {
      for(x=brd.x; x<= retina_size.x-brd.x; x+=spc.x) {
	ic.y = y; ic.x = x;
	ic.WrapClip(true, retina_size);	mat->FastEl(ic.x,ic.y) = 1.0f;
      }
    }
  }

  { // then v1 complex
    graph_data->AddBlankRow();
    nmda->SetValAsString("V1_Complex", -1);
    float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
    TwoDCoord brd(border.x+v1s_specs.border*dog_spacing.x+v1s_specs.border*dog_spacing.x*v1c_specs.border.x,
		  border.y+v1s_specs.border*dog_spacing.y+v1s_specs.border*dog_spacing.y*v1c_specs.border.y);
    TwoDCoord spc(dog_spacing.x * v1s_specs.spacing * v1c_specs.spacing.x,
		  dog_spacing.y * v1s_specs.spacing * v1c_specs.spacing.y);
    TwoDCoord spcb(dog_spacing.x * v1s_specs.spacing, dog_spacing.y * v1s_specs.spacing);
    // first render borders of RF's, every other
    for(y=brd.y; y<= retina_size.y-brd.y; y+= 2*spc.y) {
      for(x=brd.x; x<= retina_size.x-brd.x; x+= 2*spc.x) {
	ic.y = y; ic.x = x;
	ic -= v1c_specs.spat_rf_half*spcb; // lower left
	TwoDCoord ec;
	int ex,ey;
	for(ey=0; ey < v1c_specs.spat_rf.y; ey++) {
	  ec.y = ic.y + ey*spcb.y;  ec.x = ic.x;
	  ec.WrapClip(true, retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	  ec.y = ic.y + ey*spcb.y;  ec.x = ic.x + spcb.x * (v1c_specs.spat_rf.x-1);
	  ec.WrapClip(true, retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	}
	for(ex=0; ex < v1c_specs.spat_rf.x; ex++) {
	  ec.y = ic.y;	  ec.x = ic.x + ex*spcb.x;
	  ec.WrapClip(true, retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	  ec.y = ic.y + spcb.y * (v1c_specs.spat_rf.y-1); ec.x = ic.x + ex*spcb.x;
	  ec.WrapClip(true, retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	}
      }
    }
    for(y=brd.y; y<= retina_size.y-brd.y; y+= spc.y) {
      for(x=brd.x; x<= retina_size.x-brd.x; x+=spc.x) {
	ic.y = y; ic.x = x;
	ic.WrapClip(true, retina_size);	mat->FastEl(ic.x,ic.y) = 1.0f;
      }
    }
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}


//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// 		Full Retinal Spec

void RetinaProc::Initialize() {
  edge_mode = taImageProc::WRAP;
  fade_width = -1;
}

void RetinaProc::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void RetinaProc::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  regions.CheckConfig(quiet, rval);
}

///////////////////////////////////////////////////////////////////////
// 		Basic impl routines

bool RetinaProc::Init() {
  if(regions.size == 0) return false;
  for(int ri=0; ri < regions.size; ri++) {
    DoGRegionSpec* reg = regions[ri];
    reg->Init();
  }
  return true;
}

bool RetinaProc::TransformImageData_impl(float_Matrix& eye_image,
					 float_Matrix& xform_image,
					 float move_x, float move_y,
					 float scale, float rotate)
{
  if(regions.size == 0) return false;
  DoGRegionSpec* reg = regions[0]; // take params from first

  float ctr_x = .5f + .5 * move_x;
  float ctr_y = .5f + .5 * move_y;

  taImageProc::SampleImageWindow_float(xform_image, eye_image, reg->retina_size.x,
				       reg->retina_size.y, 
				       ctr_x, ctr_y, rotate, scale, edge_mode);
  if(edge_mode == taImageProc::BORDER) taImageProc::RenderBorder_float(xform_image);
  if(edge_mode == taImageProc::BORDER && fade_width > 0) {
    taImageProc::FadeEdgesToBorder_float(xform_image, fade_width); 
  }
  return true;
}

bool RetinaProc::LookAtImageData_impl(float_Matrix& eye_image,
				      float_Matrix& xform_image,
				      DoGRegionSpec::Region region,
				      float box_ll_x, float box_ll_y,
				      float box_ur_x, float box_ur_y,
				      float move_x, float move_y,
				      float scale, float rotate) 
{
  // todo: add error messages on all these..
  if(regions.size == 0) return false;
  DoGRegionSpec* trg_reg = regions.FindRetinalRegion(region);
  if(!trg_reg) return false;

  // translation: find the middle of the box
  FloatTwoDCoord obj_ctr((float) (0.5 * (float) (box_ll_x + box_ur_x)),
                       (float) (0.5 * (float) (box_ll_y + box_ur_y)));
  // convert into center-relative coords:
  FloatTwoDCoord obj_ctr_off = 2.0f * (obj_ctr - 0.5f);

  move_x -= obj_ctr_off.x;
  move_y -= obj_ctr_off.y;
  
  // now, scale the thing to fit in trg_reg->input_size
  TwoDCoord img_size(eye_image.dim(0), eye_image.dim(1));

  // height and width in pixels of box:
  float pix_x = (box_ur_x - box_ll_x) * img_size.x;
  float pix_y = (box_ur_y - box_ll_y) * img_size.y;

  // scale to fit within input size of filter or retina
  float sc_x = (float)trg_reg->input_size.x / pix_x;
  float sc_y = (float)trg_reg->input_size.y / pix_y;

  float fov_sc = MIN(sc_x, sc_y);
  scale *= fov_sc;
  if(scale > 100.0f)
    scale = 100.0f;
  if(scale < .01f)
    scale = .01f;

  bool rval = TransformImageData_impl(eye_image, xform_image, move_x, move_y, scale, rotate);
  return rval;
}

bool RetinaProc::FilterImageData() {
  if(regions.size == 0) return false;
  for(int ri=0; ri < regions.size; ri++) {
    DoGRegionSpec* reg = regions[ri];
    reg->FilterImage(&xform_image_r, &xform_image_l);
  }
  return true;
}

bool RetinaProc::InvertFilter() {
  if(regions.size == 0) return false;
  for(int ri=0; ri < regions.size; ri++) {
    DoGRegionSpec* reg = regions[ri];
    reg->InvertFilters();
  }
  return true;
}

//////////////////////////////////////////////////////
// 		Various front-ends

bool RetinaProc::TransformImageData(float_Matrix* right_eye_image,
				    float_Matrix* left_eye_image,
				    float move_x, float move_y,
				    float scale, float rotate)
{
  if(regions.size == 0) return false;
  if(right_eye_image) {
    raw_image_r.CopyFrom(right_eye_image);
    TransformImageData_impl(raw_image_r, xform_image_r, move_x, move_y, scale, rotate);
  }
  if(left_eye_image) {
    raw_image_l.CopyFrom(left_eye_image);
    TransformImageData_impl(raw_image_l, xform_image_l, move_x, move_y, scale, rotate);
  }
  return true;
}

bool RetinaProc::LookAtImageData(float_Matrix* right_eye_image,
				 float_Matrix* left_eye_image,
				 DoGRegionSpec::Region region,
				 float box_ll_x, float box_ll_y,
				 float box_ur_x, float box_ur_y,
				 float move_x, float move_y,
				 float scale, float rotate) 
{
  if(regions.size == 0) return false;
  if(right_eye_image) {
    raw_image_r.CopyFrom(right_eye_image);
    LookAtImageData_impl(raw_image_r, xform_image_r, region, box_ll_x, box_ll_y,
			 box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  if(left_eye_image) {
    raw_image_l.CopyFrom(left_eye_image);
    LookAtImageData_impl(raw_image_l, xform_image_l, region, box_ll_x, box_ll_y,
			 box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  return true;
}

bool RetinaProc::ConvertImageToMatrix(float_Matrix& img_data, taImage* img, 
				      DoGRegionSpec::Color color) {
  if(color == DoGRegionSpec::COLOR) {
    img->ImageToMatrix_rgb(img_data);
  }
  else {
    img->ImageToMatrix_grey(img_data);
  }
  return true;
}

bool RetinaProc::TransformImage(taImage* right_eye_image, taImage* left_eye_image,
				float move_x, float move_y, float scale, float rotate)
{
  if(regions.size == 0) return false;
  DoGRegionSpec* reg = regions[0]; // take params from first
  if(right_eye_image) {
    ConvertImageToMatrix(raw_image_r, right_eye_image, reg->color);
    TransformImageData_impl(raw_image_r, xform_image_r, move_x, move_y, scale, rotate);
  }
  if(left_eye_image) {
    ConvertImageToMatrix(raw_image_l, left_eye_image, reg->color);
    TransformImageData_impl(raw_image_l, xform_image_l, move_x, move_y, scale, rotate);
  }
  return true;
}

bool RetinaProc::TransformImageName(const String& right_eye_img_fname,
				    const String& left_eye_img_fname,
				    float move_x, float move_y,
				    float scale, float rotate)
{
  if(regions.size == 0) return false;
  taImage img_r;
  if(!img_r.LoadImage(right_eye_img_fname)) return false;
  img_r.name = right_eye_img_fname;		// explicitly name it
  if(left_eye_img_fname.nonempty()) {
    taImage img_l;
    if(!img_l.LoadImage(left_eye_img_fname)) return false;
    img_l.name = left_eye_img_fname;		// explicitly name it
    return TransformImage(&img_r, &img_l, move_x, move_y, scale, rotate);
  }
  else {
    return TransformImage(&img_r, NULL, move_x, move_y, scale, rotate);
  }
}

///////////// Look At

bool RetinaProc::LookAtImage(taImage* right_eye_image,
			     taImage* left_eye_image,
			     DoGRegionSpec::Region region,
			     float box_ll_x, float box_ll_y,
			     float box_ur_x, float box_ur_y,
			     float move_x, float move_y,
			     float scale, float rotate)
{
  if(regions.size == 0) return false;
  DoGRegionSpec* reg = regions[0]; // take params from first
  if(right_eye_image) {
    ConvertImageToMatrix(raw_image_r, right_eye_image, reg->color);
    LookAtImageData_impl(raw_image_r, xform_image_r, region, box_ll_x, box_ll_y,
			 box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  if(left_eye_image) {
    ConvertImageToMatrix(raw_image_l, left_eye_image, reg->color);
    LookAtImageData_impl(raw_image_l, xform_image_l, region, box_ll_x, box_ll_y,
			 box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  return true;
}

bool RetinaProc::LookAtImageName(const String& right_eye_img_fname,
				 const String& left_eye_img_fname,
				 DoGRegionSpec::Region region,
				 float box_ll_x, float box_ll_y,
				 float box_ur_x, float box_ur_y,
				 float move_x, float move_y,
				 float scale, float rotate)
{
  if(regions.size == 0) return false;
  taImage img_r;
  if(!img_r.LoadImage(right_eye_img_fname)) return false;
  img_r.name = right_eye_img_fname;		// explicitly name it
  if(left_eye_img_fname.nonempty()) {
    taImage img_l;
    if(!img_l.LoadImage(left_eye_img_fname)) return false;
    img_l.name = left_eye_img_fname;		// explicitly name it
    return LookAtImage(&img_r, &img_l, region, box_ll_x, box_ll_y,
			 box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  else {
    return LookAtImage(&img_r, NULL, region, box_ll_x, box_ll_y,
			 box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
}

///////////////////////////////////////////////////////////////////////
// Misc other processing operations

bool RetinaProc::AttendRegion(DataTable* dt, DoGRegionSpec::Region region) {
//   DoGRegionSpec* fov_spec = regions.FindRetinalRegion(region);
//   if(!fov_spec) return false;

//   float fov_x_pct = (float)fov_spec->spacing.input_size.x / (float)retina_size.x;
//   float fov_y_pct = (float)fov_spec->spacing.input_size.y / (float)retina_size.y;
//   float fov_pct = taMath_float::max(fov_x_pct, fov_y_pct);

//   int idx;
//   for(int i=0;i<regions.size;i++) {
//     DoGRegionSpec* sp = regions[i];
//     if(sp->spacing.region <= region) continue; // don't filter this region -- only ones above it!
//     DataCol* da_on = dt->FindMakeColName(sp->name + "_on", idx, DataTable::VT_FLOAT, 2,
// 						sp->spacing.output_size.x, sp->spacing.output_size.y);
//     DataCol* da_off = dt->FindMakeColName(sp->name + "_off", idx, DataTable::VT_FLOAT,
// 						 2, sp->spacing.output_size.x, sp->spacing.output_size.y);

//     float_MatrixPtr on_mat; on_mat = (float_Matrix*)da_on->GetValAsMatrix(-1);
//     float_MatrixPtr off_mat; off_mat = (float_Matrix*)da_off->GetValAsMatrix(-1);
//     taImageProc::AttentionFilter(*on_mat, fov_pct);
//     taImageProc::AttentionFilter(*off_mat, fov_pct);
//   }
  return true;
}

void V1RetinaProc::Initialize() {
  regions.SetDefaultElType(&TA_V1RegionSpec);
}

//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// 		program stuff

void ImageProcCall::Initialize() {
  min_type = &TA_taImageProc;
  object_type = &TA_taImageProc;
}

