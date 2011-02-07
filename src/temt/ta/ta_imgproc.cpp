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
  filter_width = 4;
  filter_size = filter_width * 2 + 1;
  on_sigma = 1.0f;
  off_sigma = 2.0f;
  spacing = 1;
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

bool taImageProc::GetBorderColor_float(float_Matrix& img_data, float& r, float& g, float& b) {
  if(img_data.dims() == 3) {	// an rgb guy
    return GetBorderColor_float_rgb(img_data, r, g, b);
  }
  else {
    bool rval = GetBorderColor_float_grey(img_data, r);
    g = r; b = r;
    return rval;
  }
}

bool taImageProc::GetBorderColor_float_rgb(float_Matrix& img_data, float& r, float& g, float& b) {
  if(img_data.dims() != 3) {
    taMisc::Error("taImageProc::GetBorderColor_float_rgb", "image must have 3 dims: x, y, color");
    return false; // err
  }
  float clrs[3];
  for(int i=0;i<3;i++) {
    float_Matrix* cmp = img_data.GetFrameSlice(i);
    taBase::Ref(cmp);
    GetBorderColor_float_grey(*cmp, clrs[i]);
    taBase::unRefDone(cmp);
  }
  r = clrs[0]; g = clrs[1]; b = clrs[2];
  return true;
}

bool taImageProc::GetBorderColor_float_grey(float_Matrix& img_data, float& grey) {
  if(img_data.dims() != 2) {
    taMisc::Error("taImageProc::GetBorderColor_float_grey", "image must have 2 dims: x, y");
    return false; // err
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

  grey = .25 * (tavg + bavg + lavg + ravg);
  return true;
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
  float grey;
  GetBorderColor_float_grey(img_data, grey);

  TwoDCoord img_size(img_data.dim(0), img_data.dim(1));

  for(int x=0;x<img_size.x;x++) {
    img_data.FastEl(x, img_size.y-1) = grey;
    img_data.FastEl(x, 0) = grey;
  }
  for(int y=1;y<img_size.y-1;y++) {
    img_data.FastEl(img_size.x-1, y) = grey;
    img_data.FastEl(0, y) = grey;
  }
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


bool taImageProc::BlobBlurOcclude(float_Matrix& img, float pct_occlude,
				  float circ_radius, float gauss_sig, 
				  EdgeMode edge, bool use_border_clr) {
  TwoDCoord img_size(img.dim(0), img.dim(1));

  float gauss_eff = gauss_sig * (float)img_size.x;
  float radius_eff = circ_radius * (float)img_size.x;
  int filt_half = (int)radius_eff + (int)(gauss_eff * 2);
  int filt_wd = filt_half * 2;
  TwoDCoord ntot = (img_size / filt_half) + 1;
  int totblob = ntot.Product();
  int nblob = (int) (2.5f * pct_occlude * (float)totblob + 0.5f);
  if(pct_occlude == 1.0f) nblob *= 2; // really nuke it for sure!

  float_Matrix filt_wt;
  filt_wt.SetGeom(2, filt_wd, filt_wd);
  float ctr = (float)(filt_wd-1) * .5f;
  for(int yi=0; yi < filt_wd; yi++) {
    float y = (float)yi - ctr;
    for(int xi=0; xi < filt_wd; xi++) {
      float x = (float)xi - ctr;
      float d = taMath_float::hypot(x,y);
      float fv = 1.0f;
      if(d > radius_eff) {
	float gd = (d-radius_eff) / gauss_eff;
	fv = expf(-(gd * gd)/2.0f);
      }
      filt_wt.FastEl(xi, yi) = fv;
    }
  }

  float_Matrix filt_cnv;
  filt_cnv.CopyFrom(&filt_wt);
  taMath_float::vec_norm_sum(&filt_cnv, 1.0f); // conv = sum norm
  taMath_float::vec_norm_max(&filt_wt, 1.0f);  // weights = max norm

  bool rgb_img = false;
  if(img.dims() == 3) { // rgb
    rgb_img = true;
  }

  bool wrap = (edge == WRAP);

  float brd_clr[3];

  if(use_border_clr) {
    GetBorderColor_float(img, brd_clr[0], brd_clr[1], brd_clr[2]);
  }

  TwoDCoord ic;
  TwoDCoord icw;
  for(int blob=0; blob < nblob; blob++) {
    ic.x = Random::IntMinMax(0, img_size.x);
    ic.y = Random::IntMinMax(0, img_size.y);

    if(rgb_img) {
      for(int rgb=0; rgb<3; rgb++) {
	float clr = 0.0f;
	if(use_border_clr) {
	  clr = brd_clr[rgb];
	}
	else {
	  for(int yi=0; yi< filt_wd; yi++) {
	    for(int xi=0; xi< filt_wd; xi++) {
	      icw.x = ic.x + xi - filt_half;
	      icw.y = ic.y + yi - filt_half;
	      icw.WrapClip(wrap, img_size); // use edges if clipping
	      float iv = img.FastEl(icw.x, icw.y, rgb);
	      clr += filt_cnv.FastEl(xi, yi) * iv;
	    }
	  }
	}

	for(int yi=0; yi< filt_wd; yi++) {
	  for(int xi=0; xi< filt_wd; xi++) {
	    icw.x = ic.x + xi - filt_half;
	    icw.y = ic.y + yi - filt_half;
	    if(icw.WrapClip(wrap, img_size)) {
	      if(!wrap) continue;
	    }
	    float& iv = img.FastEl(icw.x, icw.y, rgb);
	    float wt = filt_wt.FastEl(xi, yi);
	    float nw_iv = (1.0f - wt) * iv + wt * clr;
	    iv = nw_iv;
	  }
	}
      }
    }
    else {
      float clr = 0.0f;
      if(use_border_clr) {
	clr = brd_clr[0];
      }
      else {
	for(int yi=0; yi< filt_wd; yi++) {
	  for(int xi=0; xi< filt_wd; xi++) {
	    icw.x = ic.x + xi - filt_half;
	    icw.y = ic.y + yi - filt_half;
	    icw.WrapClip(wrap, img_size); // use edges if clipping
	    float iv = img.FastEl(icw.x, icw.y);
	    clr += filt_cnv.FastEl(xi, yi) * iv;
	  }
	}
      }

      for(int yi=0; yi< filt_wd; yi++) {
	for(int xi=0; xi< filt_wd; xi++) {
	  icw.x = ic.x + xi - filt_half;
	  icw.y = ic.y + yi - filt_half;
	  if(icw.WrapClip(wrap, img_size)) {
	    if(!wrap) continue;
	  }
	  float& iv = img.FastEl(icw.x, icw.y);
	  float wt = filt_wt.FastEl(xi, yi);
	  float nw_iv = (1.0f - wt) * iv + wt * clr;
	  iv = nw_iv;
	}
      }
    }
  }
  return true;
}

bool taImageProc::AdjustContrast(float_Matrix& img, float new_contrast) {  
  TwoDCoord img_size(img.dim(0), img.dim(1));

  // get background color
  float brd_clr[3];
  GetBorderColor_float(img, brd_clr[0], brd_clr[1], brd_clr[2]);
  
  // check for rgb img
  bool rgb_img = false;
  if(img.dims() == 3) { // rgb
    rgb_img = true;
  }  
  
  // no support for rgb yet
  if(rgb_img) {  
  }
  
  else {
  // just use first dim for gray
  float clr = 0.0f;
  clr = brd_clr[0];
  for(int yi=0; yi< img_size.y; yi++) {
      for(int xi=0; xi< img_size.x; xi++) {    
	  	  float& iv = img.FastEl(xi, yi);
	  	  float nw_iv = ((iv-clr)*new_contrast)+clr;
	  	  iv = nw_iv;
	  }
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//		Visual Region Filtering (Base)

// for thread function calling:
typedef void (VisRegionSpecBase::*VisRegionMethod)(int, int);

void VisRegionParams::Initialize() {
  ocularity = MONOCULAR;
  region = FOVEA;
  res = HI_RES;
  color = MONOCHROME;
  edge_mode= WRAP;
  renorm_thr = 1.0e-5f;
}

void VisRegionSizes::Initialize() {
  retina_size = 144;
  border = 0;
  input_size = 144;
}

void VisRegionSizes::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  input_size = (retina_size - 2 * border);
}

void VisRegionSpecBase::Initialize() {
  save_mode = FIRST_ROW;
  image_save = (DataSave)(SAVE_DATA | ONLY_GUI);
  motion_frames = 0;
  
  cur_img_r = NULL;
  cur_img_l = NULL;
  cur_img = NULL;
  cur_out = NULL;
  cur_circ = NULL;
  rgb_img = false;
  wrap = false;
}

void VisRegionSpecBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  input_size.UpdateAfterEdit_NoGui();
  motion_frames = MAX(motion_frames, 0);
  UpdateGeom();
}

void VisRegionSpecBase::UpdateGeom() {
  // note: override in derived classes..
}

bool VisRegionSpecBase::Init() {
  bool rval = InitFilters();
  rval &= InitOutMatrix();
  rval &= InitDataTable();
  return rval;
}

bool VisRegionSpecBase::NeedsInit() {
  // no way to know in base -- override!
  return false;
}

bool VisRegionSpecBase::InitFilters() {
  // note: override in derived classes..
  return true;
}

bool VisRegionSpecBase::InitOutMatrix() {
  // note: override in derived classes..
  return true;
}

////////////////////////////////////////////////////////////////////
//	DoGRegion 	Filtering

bool VisRegionSpecBase::FilterImage(float_Matrix* right_eye_image, float_Matrix* left_eye_image) {
  // this is not typically overwritten -- just all the checks -- see _impl
  if(TestWarning(NeedsInit(),
		 "FilterImage", "not properly initialized to current geom -- running Init now")) {
    Init();
  }  

  if(TestError(!right_eye_image, "FilterIMage", "right_eye_image is NULL -- must pass image"))
    return false;

  if(TestError((right_eye_image->dim(0) != input_size.retina_size.x) ||
	       (right_eye_image->dim(1) != input_size.retina_size.y),
	       "FilterImage", "right_eye_image is not appropriate size -- must be same as retina_size!"))
    return false;

  if(region.ocularity == VisRegionParams::BINOCULAR) {
    if(TestError(!left_eye_image, "FilterIMage", "left_eye_image is NULL -- must pass image"))
      return false;

    if(TestError((left_eye_image->dim(0) != input_size.retina_size.x) ||
		 (left_eye_image->dim(1) != input_size.retina_size.y),
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

void VisRegionSpecBase::IncrTime() {
  // note: override in derived classes..
}

bool VisRegionSpecBase::FilterImage_impl() {
  if(!data_table || save_mode == NONE_SAVE) // bail now
    return false;

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
  return true;
}

bool VisRegionSpecBase::ColorRGBtoCMYK(float_Matrix& img) {
  TwoDCoord img_size(img.dim(0), img.dim(1));

  cur_img_grey.SetGeom(2, img_size.x, img_size.y);
  cur_img_rc.SetGeom(2, img_size.x, img_size.y);
  cur_img_gm.SetGeom(2, img_size.x, img_size.y);
  cur_img_by.SetGeom(2, img_size.x, img_size.y);

  for(int yi = 0; yi < img_size.y; yi++) {
    for(int xi = 0; xi < img_size.y; xi++) {
      float r_val = img.FastEl(xi, yi, 0);
      float g_val = img.FastEl(xi, yi, 1);
      float b_val = img.FastEl(xi, yi, 2);

      float grey = 0.33333f * (r_val + g_val + b_val);
      float r_v_c = r_val - 0.5f * (g_val + b_val);
      float g_v_m = g_val - 0.5f * (r_val + b_val);
      float b_v_y = b_val - 0.5f * (r_val + g_val);

      cur_img_grey.FastEl(xi, yi) = grey;
      cur_img_rc.FastEl(xi, yi) = r_v_c;
      cur_img_gm.FastEl(xi, yi) = g_v_m;
      cur_img_by.FastEl(xi, yi) = b_v_y;
    }
  }
  return true;
}

float_Matrix* VisRegionSpecBase::GetImageForChan(ColorChannel cchan) {
  switch(cchan) {
  case LUMINANCE:
    return &cur_img_grey;
  case RED_CYAN:
    return &cur_img_rc;
  case GREEN_MAGENTA:
    return &cur_img_gm;
  case BLUE_YELLOW:
    return &cur_img_by;
  }
  return NULL;
}

bool VisRegionSpecBase::RenormOutput_Frames(RenormMode mode, float_Matrix* out, CircMatrix* circ) {
  bool rval = false;
  float_Matrix* mat = out;
  if(motion_frames > 1) {
    mat = (float_Matrix*)out->GetFrameSlice(circ->CircIdx_Last());
    taBase::Ref(mat);
  }
  int idx;
  float max_val = taMath_float::vec_max(mat, idx);
  if(max_val > region.renorm_thr) {
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

bool VisRegionSpecBase::RenormOutput_NoFrames(RenormMode mode, float_Matrix* mat) {
  bool rval = false;
  int idx;
  float max_val = taMath_float::vec_max(mat, idx);
  if(max_val > region.renorm_thr) {
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

bool VisRegionSpecBase::InvertFilters() {
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

  if(TestWarning(NeedsInit(),
		 "InvertFilters", "not properly initialized to current geom -- running Init now")) {
    Init();
  }  

  bool rval = InvertFilters_impl();
  return rval;
}

bool VisRegionSpecBase::InvertFilters_impl() {
  if(save_mode == FIRST_ROW) {
    data_table->EnforceRows(1);
    data_table->WriteItem(0);
    data_table->ReadItem(0);
  }
  else {
    data_table->AddBlankRow();
  }

  return true;
}


////////////////////////////////////////////////////////////////////
//	DoGRegion 	Data Table Output

bool VisRegionSpecBase::InitDataTable() {
  if(!data_table) {
    return false;
  }
  int idx;
  if(image_save & SAVE_DATA) {
    DataCol* col;
    String sufx = "_r";

    if(region.color == VisRegionParams::COLOR)
      col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 3,
					input_size.retina_size.x, input_size.retina_size.y, 3);
    else
      col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 2,
					input_size.retina_size.x, input_size.retina_size.y);
    col->SetUserData("IMAGE", true);

    if(region.ocularity == VisRegionParams::BINOCULAR) {
      sufx = "_l";
      if(region.color == VisRegionParams::COLOR)
	col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 3,
					  input_size.retina_size.x, input_size.retina_size.y, 3);
      else
	col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 2,
					  input_size.retina_size.x, input_size.retina_size.y);
      col->SetUserData("IMAGE", true);
    }
  }

  return true;
}

bool VisRegionSpecBase::ImageToTable(DataTable* dtab, float_Matrix* right_eye_image,
				 float_Matrix* left_eye_image) {
  ImageToTable_impl(dtab, right_eye_image, "_r");
  if(region.ocularity == VisRegionParams::BINOCULAR) 
    ImageToTable_impl(dtab, left_eye_image, "_l");
  return true;
}

bool VisRegionSpecBase::ImageToTable_impl(DataTable* dtab, float_Matrix* img,
				      const String& col_sufx) {
  DataCol* col;
  int idx;
  if(region.color == VisRegionParams::COLOR)
    col = data_table->FindMakeColName(name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 3,
				      input_size.retina_size.x, input_size.retina_size.y, 3);
  else
    col = data_table->FindMakeColName(name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 2,
				      input_size.retina_size.x, input_size.retina_size.y);

  float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);
  ret_img->CopyFrom(img);
  return true;
}

/////////////////////////////////
//		List

VisRegionSpecBase* VisRegionSpecBaseList::FindRetinalRegion(VisRegionParams::Region reg) {
  for(int i=0;i<size;i++) {
    VisRegionSpecBase* fs = (VisRegionSpecBase*)FastEl(i);
    if(fs->region.region == reg)
      return fs;
  }
  return NULL;
}

VisRegionSpecBase* VisRegionSpecBaseList::FindRetinalRes(VisRegionParams::Resolution res) {
  for(int i=0;i<size;i++) {
    VisRegionSpecBase* fs = (VisRegionSpecBase*)FastEl(i);
    if(fs->region.res == res)
      return fs;
  }
  return NULL;
}

VisRegionSpecBase* VisRegionSpecBaseList::FindRetinalRegionRes(VisRegionParams::Region reg,
						       VisRegionParams::Resolution res) {
  for(int i=0;i<size;i++) {
    VisRegionSpecBase* fs = (VisRegionSpecBase*)FastEl(i);
    if((fs->region.region == reg) && (fs->region.res == res))
      return fs;
  }
  VisRegionSpecBase* rval = FindRetinalRes(res);
  if(rval) return rval;
  rval = FindRetinalRegion(reg);
  if(rval) return rval;
  return NULL;
}



////////////////////////////////////////////////////////////////////
//		DoG Processing 

// for thread function calling:
typedef void (DoGRegionSpec::*DoGRegionMethod)(int, int);

void DoGRegionSpec::Initialize() {
  dog_specs.filter_width = 4;
  dog_specs.on_sigma = 1;
  dog_specs.off_sigma = 2;
  dog_specs.circle_edge = true;
  dog_renorm = LOG_RENORM;
  dog_save = SAVE_DATA;
  dog_feat_geom.x = 2;
  dog_feat_geom.y = 3;
  dog_feat_geom.n = 6;
  dog_img_geom = 144;
}

void DoGRegionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dog_specs.name = name;
  dog_specs.UpdateAfterEdit_NoGui();
}

void DoGRegionSpec::UpdateGeom() {
  inherited::UpdateGeom();

  // 0 1 2 3 4 5 6 7 8 	retina_size = 9
  // b b .   .   . b b 	border = 2, spacing = 2: input_size = 5, output_size = 3

  // wrap mode:
  // 0 1 2 3 4 5 6 7  retina_size = 8
  // .   .   .   .    border = 0, spacing = 2; input_size = 8, output_size = 4

  if(region.edge_mode == VisRegionParams::WRAP) {
    dog_img_geom = input_size.input_size / dog_specs.spacing;
  }
  else {
    dog_img_geom = ((input_size.input_size - 1) / dog_specs.spacing) + 1;
  }
  if(region.color == VisRegionParams::COLOR) {
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

bool DoGRegionSpec::InitFilters() {
  inherited::InitFilters();
  UpdateGeom();
  dog_specs.UpdateFilter();
  return true;
}

bool DoGRegionSpec::InitOutMatrix() {
  inherited::InitOutMatrix();

  dog_circ_r.matrix = &dog_out_r;
  dog_circ_l.matrix = &dog_out_l;

  if(motion_frames <= 1) {
    dog_out_r.SetGeom(4, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
    if(region.ocularity == VisRegionParams::BINOCULAR)
      dog_out_l.SetGeom(4, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
    else
      dog_out_l.SetGeom(1,1);	// free memory
  }
  else {
    dog_out_r.SetGeom(5, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y,
		      motion_frames);
    if(region.ocularity == VisRegionParams::BINOCULAR)
      dog_out_l.SetGeom(5, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y,
			motion_frames);
    else
      dog_out_l.SetGeom(1,1);	// free memory
  }
  dog_circ_r.Reset();
  dog_circ_l.Reset();

  return true;
}

bool DoGRegionSpec::NeedsInit() {
  if((dog_out_r.dims() < 4) ||
     (dog_out_r.dim(0) * dog_out_r.dim(1) != dog_feat_geom.n) ||
     (dog_out_r.dim(2) != dog_img_geom.x) ||
     (dog_out_r.dim(3) != dog_img_geom.y))
    return true;

  if(region.ocularity == VisRegionParams::BINOCULAR) {
    if((dog_out_l.dims() < 4) ||
       (dog_out_l.dim(0) * dog_out_l.dim(1) != dog_feat_geom.n) ||
       (dog_out_l.dim(2) != dog_img_geom.x) ||
       (dog_out_l.dim(3) != dog_img_geom.y))
      return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////////
//	DoGRegion 	Filtering

void DoGRegionSpec::IncrTime() {
  inherited::IncrTime();

  if(motion_frames <= 1) {
    return;
  }
  else {
    dog_circ_r.CircAddLimit(motion_frames);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      dog_circ_l.CircAddLimit(motion_frames);
    }
  }
}

bool DoGRegionSpec::FilterImage_impl() {
  inherited::FilterImage_impl();

  IncrTime();
  bool rval = DoGFilterImage(cur_img_r, &dog_out_r, &dog_circ_r);
  if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
    rval &= DoGFilterImage(cur_img_l, &dog_out_l, &dog_circ_l);
  }

  if(!data_table || save_mode == NONE_SAVE) // bail now
    return rval;

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
  wrap = (region.edge_mode == VisRegionParams::WRAP);

  if(rgb_img) {
    ColorRGBtoCMYK(*cur_img);	// precompute!
  }

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
  TwoDCoord icc = input_size.border + dog_specs.spacing * dc; // image coords center

  float_Matrix* dog_img = cur_img;
  int mot_idx = cur_circ->CircIdx_Last(); // always write to last position

  // x = on/off, y = color channel
  TwoDCoord ic;		// image coord
  for(int chan = 0; chan < dog_feat_geom.y; chan++) { 
    ColorChannel cchan = (ColorChannel)chan;
    if(rgb_img) {
      dog_img = GetImageForChan(cchan);
    }

    float cnv_sum = 0.0f;		// convolution sum
    if(chan == 0 || rgb_img) {		// only rgb images if chan > 0
      for(int yf = -dog_specs.filter_width; yf <= dog_specs.filter_width; yf++) {
	for(int xf = -dog_specs.filter_width; xf <= dog_specs.filter_width; xf++) {
	  ic.y = icc.y + yf;
	  ic.x = icc.x + xf;
	  if(ic.WrapClip(wrap, input_size.retina_size)) {
	    if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	  }
	  cnv_sum += dog_specs.FilterPoint(xf, yf, dog_img->FastEl(ic.x, ic.y));
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
  if(max_val > region.renorm_thr) {
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
  if(max_val > region.renorm_thr) {
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

bool DoGRegionSpec::InvertFilters_impl() {
  inherited::InvertFilters_impl();

  bool rval = DoGInvertFilter(&dog_out_r, &dog_circ_r, "_r");
  if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
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
  if(region.color == VisRegionParams::COLOR)
    col = data_table->FindMakeColName(name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 3,
				      input_size.retina_size.x, input_size.retina_size.y, 3);
  else
    col = data_table->FindMakeColName(name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 2,
				      input_size.retina_size.x, input_size.retina_size.y);

  float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);

  // note: this does not deal with rgb images, nor does it work very well at all anyway

  ret_img->InitVals(0.0f);

  int mot_idx = circ->CircIdx_Last(); // always write to last position
  TwoDCoord dc;			// dog coords
  for(dc.y=0; dc.y < dog_img_geom.y; dc.y++) {
    for(dc.x=0; dc.x < dog_img_geom.x; dc.x++) {
      TwoDCoord icc = input_size.border + dog_specs.spacing * dc; // image coords center
      // x = on/off, y = color channel
      TwoDCoord ic;		// image coord
      for(int chan = 0; chan < dog_feat_geom.y; chan++) { 
	float on_act = MatMotEl(out, 0, chan, dc.x, dc.y, mot_idx); // feat x = 0 = on
	float off_act = MatMotEl(out, 1, chan, dc.x, dc.y, mot_idx);
	float net_act = on_act - off_act;

	if(chan == 0 || rgb_img) {		// only rgb images if chan > 0
	  for(int yf = -dog_specs.filter_width; yf <= dog_specs.filter_width; yf++) {
	    for(int xf = -dog_specs.filter_width; xf <= dog_specs.filter_width; xf++) {
	      ic.y = icc.y + yf;
	      ic.x = icc.x + xf;
	      if(ic.WrapClip(wrap, input_size.retina_size)) {
		if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	      }
	      float grey = dog_specs.InvertFilterPoint(xf, yf, net_act);
	      ret_img->FastEl(ic.x, ic.y) += grey;
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
  inherited::InitDataTable();

  int idx;
  if(dog_save & SAVE_DATA) {
    if(dog_save & SEP_MATRIX) {
      for(int i=0;i<dog_feat_geom.n;i++) {
	String nm = name + "_" + GetDoGFiltName(i) + "_dog";
	data_table->FindMakeColName(nm+ "_r", idx, DataTable::VT_FLOAT, 2,
				    dog_img_geom.x, dog_img_geom.y);
	if(region.ocularity == VisRegionParams::BINOCULAR) {
	  data_table->FindMakeColName(nm+ "_l", idx, DataTable::VT_FLOAT, 2,
				      dog_img_geom.x, dog_img_geom.y);
	}
      }
    }
    else {
      data_table->FindMakeColName(name + "_dog_r", idx, DataTable::VT_FLOAT, 4,
				  dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
      if(region.ocularity == VisRegionParams::BINOCULAR) {
	data_table->FindMakeColName(name + "_dog_l", idx, DataTable::VT_FLOAT, 4,
				    dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
      }
    }
  }
  return true;
}

bool DoGRegionSpec::DoGOutputToTable(DataTable* dtab) {
  DoGOutputToTable_impl(dtab, &dog_out_r, &dog_circ_r, "_r");
  if(region.ocularity == VisRegionParams::BINOCULAR) 
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
					      input_size.retina_size.x, input_size.retina_size.y);
  graph_data->SetUserData("N_ROWS", 1);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);
  graph_data->SetUserData("BLOCK_SPACE", 20.0f);
  graph_data->SetUserData("WIDTH", 1.0f + (float)input_size.retina_size.x / (float)input_size.retina_size.y);

  graph_data->AddBlankRow();
  nmda->SetValAsString("DoG", -1);
  float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
  if(mat) {
    TwoDCoord ic;
    int x,y;
    for(y=input_size.border.y; y<= input_size.retina_size.y-input_size.border.y; y+= dog_specs.spacing) {
      for(x=input_size.border.x; x<= input_size.retina_size.x-input_size.border.x; x+=dog_specs.spacing) {
	ic.y = y; ic.x = x;
	ic.WrapClip(true, input_size.retina_size);	mat->FastEl(ic.x,ic.y) = 1.0f;
      }
    }
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}



////////////////////////////////////////////////////////////////////
//		V1 Processing -- basic RF's

///////////////////////////////////////////////////////////
// 		V1 KWTA

void V1KwtaSpec::Initialize() {
  on = false;
  gp_k = 1;
  gp_g = 0.1f;
  kwta_pt = 0.5f;
  gain = 40.0f;
  nvar = 0.01f;
  g_bar_l = 0.1f;

  // gelin defaults:
  g_bar_e = 0.5f;
  e_rev_e = 1.0f;
  e_rev_l = 0.3f;
  thr = 0.5f;

  noise_conv.x_range.min = -.05f;
  noise_conv.x_range.max = .05f;
  noise_conv.res = .001f;
  noise_conv.UpdateAfterEdit_NoGui();

  nxx1_fun.x_range.min = -.03f;
  nxx1_fun.x_range.max = 1.0f;
  nxx1_fun.res = .001f;
  nxx1_fun.UpdateAfterEdit_NoGui();

  gber_l = g_bar_l * e_rev_l;
  e_rev_sub_thr_e = e_rev_e - thr;
  e_rev_sub_thr_i = e_rev_l - thr;
  gbl_e_rev_sub_thr_l = g_bar_l * (e_rev_l - thr);
  thr_sub_e_rev_i = thr - e_rev_l;
  thr_sub_e_rev_e = thr - e_rev_e;

  CreateNXX1Fun();
}

void V1KwtaSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  // these are all gelin defaults
  g_bar_e = 0.5f;
  e_rev_e = 1.0f;
  e_rev_l = 0.3f;
  thr = 0.5f;

  gber_l = g_bar_l * e_rev_l;
  e_rev_sub_thr_e = e_rev_e - thr;
  e_rev_sub_thr_i = e_rev_l - thr;
  gbl_e_rev_sub_thr_l = g_bar_l * (e_rev_l - thr);
  thr_sub_e_rev_i = thr - e_rev_l;
  thr_sub_e_rev_e = thr - e_rev_e;

  CreateNXX1Fun();
}

void V1KwtaSpec::CreateNXX1Fun() {
  // first create the gaussian noise convolver
  nxx1_fun.x_range.max = 1.0f;
  nxx1_fun.res = .001f;	// needs same fine res to get the noise transitions
  nxx1_fun.UpdateAfterEdit_NoGui();
  float ns_rng = 3.0f * nvar;	// range factor based on noise level -- 3 sd 
  ns_rng = MAX(ns_rng, nxx1_fun.res);
  nxx1_fun.x_range.min = -ns_rng; 

  noise_conv.x_range.min = -ns_rng;
  noise_conv.x_range.max = ns_rng;
  noise_conv.res = nxx1_fun.res;
  noise_conv.UpdateAfterEdit_NoGui();

  noise_conv.AllocForRange();
  int i;
  float eff_nvar = MAX(nvar, 1.0e-6f); // just too lazy to do proper conditional for 0..
  float var = eff_nvar * eff_nvar;
  for(i=0; i < noise_conv.size; i++) {
    float x = noise_conv.Xval(i);
    noise_conv[i] = expf(-((x * x) / var)); // shouldn't there be a factor of 1/2 here..?
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

  nxx1_fun.Convolve(fun, noise_conv); // does alloc
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

bool V1KwtaSpec::Compute_Kwta(float_Matrix& inputs, float_Matrix& outputs,
			      float_Matrix& gc_i_mat) {
  if(TestError(inputs.dims() != 4, "Compute_Kwta",
	       "input matrix must have 4 dimensions: gp x,y, outer (image) x,y"))
    return false;

  Compute_Inhib(inputs, gc_i_mat);
  Compute_Act(inputs, outputs, gc_i_mat);
  return true;
}

void V1KwtaSpec::Compute_Act(float_Matrix& inputs, float_Matrix& outputs,
			      float_Matrix& gc_i_mat) {
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
	  outputs.FastEl(gx, gy, ix, iy) = act; 
	}
      }
    }
  }
}

void V1KwtaSpec::Compute_Inhib_IThr(float_Matrix& inputs, float_Matrix& gc_i_mat,
				    float_Matrix& ithrs) {
  Compute_All_IThr(inputs, ithrs);
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
	  gpmat.FastEl(gx, gy) = ithrs.FastEl(gx, gy, ix, iy);
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

void V1KwtaSpec::Compute_All_IThr(float_Matrix& inputs, float_Matrix& ithrs) {
  int gxs = inputs.dim(0);
  int gys = inputs.dim(1);
  int ixs = inputs.dim(2);
  int iys = inputs.dim(3);
  ithrs.SetGeom(4, gxs, gys, ixs, iys);
  for(int iy=0; iy < iys; iy++) {
    for(int ix=0; ix < ixs; ix++) {
      for(int gy=0; gy < gys; gy++) {
	for(int gx=0; gx < gxs; gx++) {
	  ithrs.FastEl(gx, gy, ix, iy) = Compute_IThresh(g_bar_e * inputs.FastEl(gx, gy, ix, iy));
	}
      }
    }
  }
}

///////////////////////////////////////////////////////////
// 		Basic Specs

void V1GaborSpec::Initialize() {
  gain = 2.0f;
  n_angles = 4;
  filter_size = 6;
  spacing = 1;
  wvlen = 6.0f;
  gauss_sig_len = 0.3f;
  gauss_sig_wd = 0.2f;
  phase_off = 0.0f;
  circle_edge = true;
}

void V1GaborSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void V1GaborSpec::RenderFilters(float_Matrix& fltrs) {
  fltrs.SetGeom(3, filter_size, filter_size, n_angles);

  float ctr = (float)(filter_size-1) / 2.0f;
  float ang_inc = taMath_float::pi / (float)n_angles;

  float circ_radius = (float)(filter_size) / 2.0f;

  float gs_len_eff = gauss_sig_len * (float)filter_size;
  float gs_wd_eff = gauss_sig_wd * (float)filter_size;

  float len_norm = 1.0f / (2.0f * gs_len_eff * gs_len_eff);
  float wd_norm = 1.0f / (2.0f * gs_wd_eff * gs_wd_eff);

  float twopinorm = (2.0f * taMath_float::pi) / wvlen;
    
  for(int ang = 0; ang < n_angles; ang++) {
    float angf = -(float)ang * ang_inc;
    
    float pos_sum = 0.0f;
    float neg_sum = 0.0f;
    for(int x = 0; x < filter_size; x++) {
      for(int y = 0; y < filter_size; y++) {
        float xf = (float)x - ctr;
        float yf = (float)y - ctr;
        
        float dist = taMath_float::hypot(xf, yf);
	float val = 0.0f;
	if(!(circle_edge && (dist > circ_radius))) {
	  float nx = xf * cosf(angf) - yf * sinf(angf);
	  float ny = yf * cosf(angf) + xf * sinf(angf);
	  float gauss = expf(-(len_norm * (nx * nx) + wd_norm * (ny * ny)));
	  float sin_val = sinf(twopinorm * ny + phase_off);
	  val = gauss * sin_val;
	  if(val > 0.0f) 	{ pos_sum += val; }
	  else if(val < 0.0f) 	{ neg_sum += val; }
	}
	fltrs.FastEl(x, y, ang) = val;
      }
    }
    // renorm each half
    float pos_norm = 1.0f / pos_sum;
    float neg_norm = -1.0f / neg_sum;
    for(int x = 0; x < filter_size; x++) {
      for(int y = 0; y < filter_size; y++) {
	float& val = fltrs.FastEl(x, y, ang);
	if(val > 0.0f) 		{ val *= pos_norm; }
	else if(val < 0.0f) 	{ val *= neg_norm; }
      }
    }
  }
}

void V1GaborSpec::GridFilters(float_Matrix& fltrs, DataTable* graph_data, bool reset) {
  RenderFilters(fltrs);		// just to make sure

  String name;
  if(owner) name = owner->GetName();

  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_V1Gabor_GridFilters", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->ResetData();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
//   nmda->SetUserData("WIDTH", 10);
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, filter_size, filter_size);

  float maxv = taMath_float::vec_abs_max(&fltrs, idx);

  graph_data->SetUserData("N_ROWS", 4);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);

  int ang_inc = 180 / n_angles;

  for(int ang=0; ang<n_angles; ang++) {
    graph_data->AddBlankRow();
    float_MatrixPtr frm; frm = (float_Matrix*)fltrs.GetFrameSlice(ang);
    matda->SetValAsMatrix(frm, -1);
    nmda->SetValAsString("Angle: " + String(ang * ang_inc), -1);
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

void V1sNeighInhib::Initialize() {
  on = true;
  inhib_d = 1;
  inhib_g = 0.8f;

  tot_ni_len = 2 * inhib_d + 1;
}

void V1sNeighInhib::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_ni_len = 2 * inhib_d + 1;
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
  dsp_ang = false;
  dsp_v1c_thr = 0.1f;
  n_disps = 1;
  disp_range_pct = 0.05f;
  gauss_sig = 0.7f;
  disp_spacing = 2.0f;
  end_extra = 2;
  dsp_gain = 0.2f;
  edge_pct = 0.05f;

  tot_disps = 1 + 2 * n_disps;
  ambig_wt = 1.0f / (float)tot_disps; // ambiguous case weighting
  UpdateFmV1sSize(36);
}

void V1BinocularSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_disps = 1 + 2 * n_disps;
  ambig_wt = 1.0f / (float)tot_disps; // ambiguous case weighting
}

void V1DisparitySpec::Initialize() {
  ambig_off = true;
  n_matches = 7;
  opt_thr = 0.1f;
  good_thr = 0.8f;
  win_half_sz = 1;
  win_thr = 0.1f;
  min_hz_len = 3;
  hz_win_sz = 6;

  thr_gain = 1.0f / good_thr;
  win_sz = 1 + 2 * win_half_sz;
  win_area = win_sz * win_sz;
}

void V1DisparitySpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  thr_gain = 1.0f / good_thr;
  win_sz = 1 + 2 * win_half_sz;
  win_area = win_sz * win_sz;
}

void V1DisparityStats::Initialize() {
  InitStats();
  pct_ambig = 0.0f;
  sel_mean = 0.0f;
}

void V1ComplexSpec::Initialize() {
  pre_gp4 = true;
  spat_rf = 12;
  border = 0;
  gauss_sig = 0.8f;
  len_sum_len = 1;
  end_stop_dist = 2;
  es_adjang_wt = 0.2f;
  es_gain = 1.2f;
  nonfocal_wt = 0.8f;

  pre_rf = 4;
  pre_half = pre_rf / 2;
  pre_spacing = pre_half;
  pre_border = 0;

  spat_half = 6;
  spat_spacing = 6;
  spat_border = 0;

  net_rf = spat_rf * pre_rf;
  net_half = spat_half * pre_half;
  net_spacing = spat_spacing * pre_spacing;
  net_border = spat_border * pre_border + border;

  len_sum_width = 1 + 2 * len_sum_len;
  len_sum_norm = 1.0f / (float)(len_sum_width);
}

void V1ComplexSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  pre_rf = 4;
  pre_half = pre_rf / 2;
  pre_spacing = pre_half;

  spat_rf.SetGtEq(1);
  spat_half = spat_rf / 2;
  spat_spacing = spat_half;
  spat_spacing.SetGtEq(1);

  if(pre_gp4) {
    net_rf = spat_rf * pre_rf;
    net_half = spat_spacing * pre_half; // use spacing to avoid 0 for 1,1 case
    net_spacing = spat_spacing * pre_spacing;
    net_border = spat_border * pre_border + border;
  }
  else {
    net_rf = spat_rf;
    net_half = spat_half;
    net_spacing = spat_spacing; 
    net_border = spat_border + border;
  }

  len_sum_width = 1 + 2 * len_sum_len;
  len_sum_norm = 1.0f / (float)(len_sum_width);
}


// for thread function calling:
typedef void (V1RegionSpec::*V1RegionMethod)(int, int);

void V1RegionSpec::Initialize() {
  v1s_filters = ALL_POLS;
  v1s_renorm = LIN_RENORM;
  v1m_renorm = NO_RENORM;
  v1s_save = SAVE_DATA;
  v1s_feat_geom.SetXYN(4, 6, 24);

  v1c_filters = CF_DEFAULT;
  v1c_renorm = LIN_RENORM;
  v1c_sep_renorm = CF_NONE;
  v1c_save = SAVE_DATA;
  v1c_feat_geom.SetXYN(4, 2, 8);
  v1c_polinv_geom.SetXYN(4, 1, 4);

  v1b_filters = BF_DEFAULT;
  v1b_renorm = NO_RENORM;
  v1b_save = (DataSave)(SAVE_DATA | SEP_MATRIX);

  opt_filters = OF_NONE;
  opt_save = SAVE_DATA;

  v1s_kwta.on = true;
  v1s_kwta.gp_k = 1;
  v1s_kwta.gp_g = 0.02f;
  v1c_kwta.on = true;
  v1c_kwta.gp_k = 2;
  v1c_kwta.gp_g = 0.1f;

  n_colors = 1;
  n_polarities = 2;
  n_polclr = n_colors * n_polarities;

  cur_out_acts = NULL;
  v1s_feat_mot_y = 0;
}

void V1RegionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  v1s_specs.UpdateAfterEdit_NoGui();
  v1s_kwta.UpdateAfterEdit_NoGui();
  v1s_neigh_inhib.UpdateAfterEdit_NoGui();
  v1s_motion.UpdateAfterEdit_NoGui();
  v1b_specs.UpdateAfterEdit_NoGui();
  v1b_dsp_specs.UpdateAfterEdit_NoGui();
  v1c_specs.UpdateAfterEdit_NoGui();
  v1c_kwta.UpdateAfterEdit_NoGui();
  // UpdateGeom is called in parent..
}

bool V1RegionSpec::NeedsInit() {
  if((v1s_out_r.dims() < 4) ||
     (v1s_out_r.dim(0) * v1s_out_r.dim(1) != v1s_feat_geom.n) ||
     (v1s_out_r.dim(2) != v1s_img_geom.x) ||
     (v1s_out_r.dim(3) != v1s_img_geom.y))
    return true;

  if(region.ocularity == VisRegionParams::BINOCULAR) {
    if((v1s_out_l.dims() < 4) ||
       (v1s_out_l.dim(0) * v1s_out_l.dim(1) != v1s_feat_geom.n) ||
       (v1s_out_l.dim(2) != v1s_img_geom.x) ||
       (v1s_out_l.dim(3) != v1s_img_geom.y))
      return true;
  }
  return false;
}


static void geom_get_angles(float angf, float& cosx, float& siny) {
  cosx = taMath_float::cos(angf);
  siny = taMath_float::sin(angf);
  // always normalize by the largest value so that it is equal to 1
  if(fabsf(cosx) > fabsf(siny)) {
    siny = siny / fabsf(cosx);			// must come first!
    cosx = cosx / fabsf(cosx);
  }
  else {
    cosx = cosx / fabsf(siny);
    siny = siny / fabsf(siny);
  }
}

void V1RegionSpec::UpdateGeom() {
  static bool redo = false;
  inherited::UpdateGeom();

  ///////////////////////////////////////////////////////////////
  //			V1 S

  if(region.ocularity == VisRegionParams::BINOCULAR && v1b_specs.dsp_ang) {
    v1s_filters = (SimpleFilters) (v1s_filters | MAX_POLS);
    v1b_filters = (BinocularFilters) (v1b_filters & ~V1B_S); // no can do right now
  } 

  n_polarities = 2;		// justin case
  if(region.color == VisRegionParams::COLOR) {
    n_colors = 4;
  }
  else {
    n_colors = 1;
  }
  n_polclr = n_colors * n_polarities;

  v1s_ang_slopes.SetGeom(3,2,2,v1s_specs.n_angles);
  float ang_inc = taMath_float::pi / (float)v1s_specs.n_angles;
  for(int ang=0; ang<v1s_specs.n_angles; ang++) {
    float cosx, siny;
    float angf = (float)ang * ang_inc;
    geom_get_angles(angf, cosx, siny);
    v1s_ang_slopes.FastEl(X, LINE, ang) = cosx;
    v1s_ang_slopes.FastEl(Y, LINE, ang) = siny;
    
    geom_get_angles(angf + taMath_float::pi * .5f, cosx, siny);
    v1s_ang_slopes.FastEl(X, ORTHO, ang) = cosx;
    v1s_ang_slopes.FastEl(Y, ORTHO, ang) = siny;
  }

  // all angles for each dog
  int n_static = v1s_specs.n_angles * n_polarities * n_colors; // 2 = polarities
  v1s_feat_mot_y = n_polarities * n_colors;		// just the y axis!
  int n_motion = 0;
  if(motion_frames > 1) {
    n_motion = 2 * n_polarities * v1s_specs.n_angles * v1s_motion.n_speeds; // 2 directions
  }
  v1s_feat_geom.x = v1s_specs.n_angles;
  v1s_feat_geom.n = n_static + n_motion;
  v1s_feat_geom.y = v1s_feat_geom.n / v1s_feat_geom.x;

  if(region.edge_mode == VisRegionParams::WRAP) {
    v1s_img_geom = input_size.input_size / v1s_specs.spacing;
  }
  else {
    v1s_img_geom = ((input_size.input_size - 1) / v1s_specs.spacing) + 1;
  }

  ///////////////////////////////////////////////////////////////
  //			V1 C

//   if(v1c_specs.pre_gp4 && v1s_specs.n_angles != 4) {
//     taMisc::Warning("V1RegionSpec:", name, " -- v1c_specs.pre_gp4 only works with v1s_specs.n_angles = 4 -- turning pre_gp4 off because n_angles=", String(v1s_specs.n_angles));
//     v1c_specs.pre_gp4 = false;
//   }
//   if(v1c_specs.pre_gp4 && !(v1s_specs.line_len == 4 || v1s_specs.line_len == 5)) {
//     taMisc::Warning("V1RegionSpec:", name, " -- v1c_specs.pre_gp4 only works with v1s_specs.line_len = 4 or 5 -- turning pre_gp4 off because line_len=", String(v1s_specs.line_len));
//     v1c_specs.pre_gp4 = false;
//   }

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
    n_cmplx += n_polarities * n_colors;
    v1c_feat_blob_y = cmplx_y;
    if(n_colors == 1)
      cmplx_y++;
    else
      cmplx_y+=2;		// color = 2 full rows
  }
  else {
    v1c_feat_blob_y = -1;
  }
  if(cmplx_y == 0) {
    cmplx_y = 1;
    n_cmplx = v1s_specs.n_angles;
  }

  v1c_feat_geom.x = v1s_specs.n_angles;
  v1c_feat_geom.n = n_cmplx;
  v1c_feat_geom.y = cmplx_y;
  v1c_feat_geom.UpdateFlag();

  v1c_polinv_geom.x = v1s_specs.n_angles;
  v1c_polinv_geom.y = 1;
  v1c_polinv_geom.n = v1s_specs.n_angles;
  v1c_polinv_geom.UpdateFlag();

  ///////	V1C spatial geom

  TwoDCoord v1s_img_geom_sub_b;
  v1s_img_geom_sub_b = v1s_img_geom - 2 * v1c_specs.border;
  if(region.edge_mode == VisRegionParams::WRAP) {
    v1c_specs.pre_border = 0;
    if(v1c_specs.pre_gp4) {
      v1c_pre_geom = v1s_img_geom_sub_b / v1c_specs.pre_spacing;
    }
    else {
      v1c_pre_geom = v1s_img_geom_sub_b;
    }

    v1c_specs.spat_border = 0;
    v1c_img_geom = v1c_pre_geom / v1c_specs.spat_spacing;
  }
  else {
    if(v1c_specs.pre_gp4) {
      v1c_specs.pre_border = v1c_specs.pre_spacing;
      v1c_pre_geom = (((v1s_img_geom_sub_b - 2 * v1c_specs.pre_border)-1) / v1c_specs.pre_spacing) + 1;
    }
    else {
      v1c_specs.pre_border = 0;
      v1c_pre_geom = v1s_img_geom_sub_b;
    }

    v1c_specs.spat_border = v1c_specs.spat_spacing;
    v1c_img_geom = (((v1c_pre_geom - 2 * v1c_specs.spat_border)-1) / v1c_specs.spat_spacing) + 1;
  }

  ///////////////////////////////////////////////////////////////
  //			V1 B

  v1b_dsp_feat_geom.x = v1b_specs.tot_disps;
  v1b_dsp_feat_geom.y = 1;

  v1b_s_feat_geom.x = v1s_feat_geom.x * v1b_specs.tot_disps;
  v1b_s_feat_geom.y = v1s_feat_geom.y;

  v1b_specs.UpdateFmV1sSize(v1s_img_geom.x); // update based on size of v1s

  ///////////////////////////////////////
  //  Double-check geom from top down 

  if(redo) {			// if doing a redo, stop here and bail
    redo = false;
    return;
  }

  TwoDCoord v1s_fm_v1c;
  if(region.edge_mode == VisRegionParams::WRAP) {
    v1s_fm_v1c = v1c_specs.net_spacing * v1c_img_geom;
  }
  else {
//     cg = ((sg - 2b - 1) / sp) + 1;
//     cg - 1 = ((sg - 2b - 1) / sp);
//     sp (cg - 1) = (sg - 2b - 1);
//     sp (cg - 1) + 2b + 1 = sg;
    if(v1c_specs.pre_gp4) {
      TwoDCoord v1cpre_fm_v1c;
      v1cpre_fm_v1c = v1c_specs.spat_spacing * (v1c_img_geom - 1) + 2 * v1c_specs.spat_border + 1;
      v1s_fm_v1c = v1c_specs.pre_spacing * (v1cpre_fm_v1c - 1) + 2 * v1c_specs.pre_border + 1;
    }
    else {
      v1s_fm_v1c = v1c_specs.net_spacing * (v1c_img_geom - 1) + 2 * v1c_specs.net_border + 1;
    }
  }
  v1s_fm_v1c += 2 * v1c_specs.border;

  if(v1s_fm_v1c != v1s_img_geom) { // mismatch!
    taMisc::Info("V1RegionSpec:", name,
		 "v1s_img_geom:", v1s_img_geom.GetStr(),
		 "is not an even multiple of v1c_specs.net_spacing:",
		 v1c_specs.net_spacing.GetStr(),
		 "this geometry is:", v1s_fm_v1c.GetStr(),
		 "Now recomputing image size to fit this -- you might want to increment by some multiple of spacing to get closer to desired input size");
    v1s_img_geom = v1s_fm_v1c;
    redo = true;		// recompute from here
  }

  TwoDCoord inp_fm_v1s;
  if(region.edge_mode == VisRegionParams::WRAP) {
    inp_fm_v1s = v1s_img_geom * v1s_specs.spacing;
  }
  else {
    inp_fm_v1s = v1s_specs.spacing * (v1s_img_geom - 1) + 1;
  }

  if(inp_fm_v1s != input_size.input_size) { // mismatch!
    if(!redo) {			   // only err if not already redoing
      taMisc::Info("V1RegionSpec:", name,
		   "input_size:", input_size.input_size.GetStr(),
		   "is not an even multiple of v1s_specs.spacing:", String(v1s_specs.spacing),
		   "this geometry is:", inp_fm_v1s.GetStr(),
		   "Recomputing image size to fit this -- you might want to increment by some multiple of spacing to get closer to desired input size");
    }
    input_size.input_size = inp_fm_v1s;
    input_size.retina_size = input_size.input_size + 2 * input_size.border;
    redo = true;		// recompute from here
  }
}

bool V1RegionSpec::InitFilters() {
  inherited::InitFilters();
  InitFilters_V1Simple();
  if(motion_frames > 1)
    InitFilters_V1Motion();
  if(region.ocularity == VisRegionParams::BINOCULAR)
    InitFilters_V1Binocular();
  InitFilters_V1Complex();
  return true;
}

bool V1RegionSpec::InitFilters_V1Simple() {

  v1s_specs.RenderFilters(v1s_gabor_filters);

  // config: x,y coords by tot_ni_len, by angles
  v1s_ni_stencils.SetGeom(3, 2, v1s_neigh_inhib.tot_ni_len, v1s_specs.n_angles);

  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int lpt=-v1s_neigh_inhib.inhib_d; lpt <= v1s_neigh_inhib.inhib_d; lpt++) {
      int lpdx = lpt + v1s_neigh_inhib.inhib_d;
      v1s_ni_stencils.FastEl(X, lpdx, ang) = 
	taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(X, ORTHO, ang)); // ortho
      v1s_ni_stencils.FastEl(Y, lpdx, ang) = 
	taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(Y, ORTHO, ang));
    }
  }

  if(motion_frames > 1) {
    v1m_stencils.SetGeom(6, 2, v1s_motion.tot_width, motion_frames, 2, 
			 v1s_specs.n_angles, v1s_motion.n_speeds);

    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      for(int dir = 0; dir < 2; dir++) { // directions
	float dirsign = (dir == 0) ? 1.0f : -1.0f; // direction sign for multiplying times slope values
	float dx = dirsign * v1s_ang_slopes.FastEl(X, ORTHO, ang);
	float dy = dirsign * v1s_ang_slopes.FastEl(Y, ORTHO, ang);
	for(int speed = 0; speed < v1s_motion.n_speeds; speed++) { // speed
	  int spd_off = 1 << speed;
	  for(int mot = 0; mot < motion_frames; mot++) { // time steps back in time
	    for(int ew = -v1s_motion.tuning_width; ew <= v1s_motion.tuning_width; ew++) {
	      int ox = taMath_float::rint((float)(spd_off*mot + ew) * dx);
	      int oy = taMath_float::rint((float)(spd_off*mot + ew) * dy);
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

  int twe = v1b_specs.disp_range + v1b_specs.end_ext;

  // everything is conditional on the disparity
  for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
    int didx = disp + v1b_specs.n_disps;
    int doff = disp * v1b_specs.disp_spc;
    if(disp == 0) {		// focal
      v1b_widths.FastEl(didx) = 1 + 2 * v1b_specs.disp_range;
      v1bc_weights.FastEl(didx) = 1.0f;
      for(int tw=-v1b_specs.disp_range; tw<=v1b_specs.disp_range; tw++) {
	int twidx = tw + v1b_specs.disp_range;
	float fx = (float)tw / (float)v1b_specs.disp_range;
	v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
	v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else if(disp == -v1b_specs.n_disps) {
      v1b_widths.FastEl(didx) = 1 + 2 * v1b_specs.disp_range + v1b_specs.end_ext;
      v1bc_weights.FastEl(didx) = v1c_specs.nonfocal_wt;
      for(int tw=-twe; tw<=v1b_specs.disp_range; tw++) {
	int twidx = tw + twe;
	if(tw < 0)
	  v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(0.0f, v1b_specs.gauss_sig);
	else {
	  float fx = (float)tw / (float)v1b_specs.disp_range;
	  v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
	}
	v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else if(disp == v1b_specs.n_disps) {
      v1b_widths.FastEl(didx) = 1 + 2 * v1b_specs.disp_range + v1b_specs.end_ext;
      v1bc_weights.FastEl(didx) = v1c_specs.nonfocal_wt;
      for(int tw=-v1b_specs.disp_range; tw<=twe; tw++) {
	int twidx = tw + v1b_specs.disp_range;
	if(tw > 0)
	  v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(0.0f, v1b_specs.gauss_sig);
	else {
	  float fx = (float)tw / (float)v1b_specs.disp_range;
	  v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
	}
	v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else {
      v1b_widths.FastEl(didx) = 1 + 2 * v1b_specs.disp_range;
      v1bc_weights.FastEl(didx) = v1c_specs.nonfocal_wt;
      for(int tw=-v1b_specs.disp_range; tw<=v1b_specs.disp_range; tw++) {
	int twidx = tw + v1b_specs.disp_range;
	float fx = (float)tw / (float)v1b_specs.disp_range;
	v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
	v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
  }
      
  taMath_float::vec_norm_max(&v1b_weights); // max norm to 1

  return true;
}

bool V1RegionSpec::InitFilters_V1Complex() {
  if(v1c_specs.spat_rf.MaxVal() > 1) {
    taMath_float::vec_kern2d_gauss(&v1c_weights, v1c_specs.spat_rf.x,
				   v1c_specs.spat_rf.y, v1c_specs.gauss_sig,
				   v1c_specs.gauss_sig);
    taMath_float::vec_norm_max(&v1c_weights, 1.0f); // max, not sum
  }
  else {
    v1c_weights.SetGeom(2, 1,1);
    v1c_weights.FastEl(0,0) = 1.0f;
  }

  // config: x,y coords by points, by angles
  v1c_es_stencils.SetGeom(4, 2, 2, 2, v1s_specs.n_angles);
  v1c_ls_stencils.SetGeom(3, 2, v1c_specs.len_sum_width, v1s_specs.n_angles);

  for(int ang=0; ang < v1s_specs.n_angles; ang++) {
    for(int sidx=0; sidx < 2; sidx++) {
      int side = (sidx == 0) ? -1 : 1;

      int sx = v1c_specs.end_stop_dist * 
	taMath_float::rint((float)side * v1s_ang_slopes.FastEl(X, LINE, ang));
      int sy = v1c_specs.end_stop_dist * 
	taMath_float::rint((float)side * v1s_ang_slopes.FastEl(Y, LINE, ang));
      for(int lpdx=0; lpdx < 2; lpdx++) {
	int lpt = (lpdx == 0) ? -1 : 1;
	v1c_es_stencils.FastEl(X, lpdx, sidx, ang) = sx + v1c_specs.end_stop_dist *
	  taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(X, ORTHO, ang));
	v1c_es_stencils.FastEl(Y, lpdx, sidx, ang) = sy + v1c_specs.end_stop_dist * 
	  taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(Y, ORTHO, ang));
      }
    }
    for(int lpt=-v1c_specs.len_sum_len; lpt <= v1c_specs.len_sum_len; lpt++) {
      int lpdx = lpt + v1c_specs.len_sum_len;
      v1c_ls_stencils.FastEl(X, lpdx, ang) = 
 	taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(X, LINE, ang));
     v1c_ls_stencils.FastEl(Y, lpdx, ang) = 
	taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(Y, LINE, ang));
    }
  }

  v1c_es_angwts.SetGeom(2, v1s_specs.n_angles, v1s_specs.n_angles);
  for(int ang1=0; ang1 < v1s_specs.n_angles; ang1++) {
    for(int ang2=0; ang2 < v1s_specs.n_angles; ang2++) {
      int ang_dst = ABS(ang1-ang2);
      {
	int wrap_dst = ABS(ang1 - (ang2 + v1s_specs.n_angles));
	if(wrap_dst < ang_dst) ang_dst = wrap_dst;
      }
      {
	int wrap_dst = ABS(ang1 - (ang2 - v1s_specs.n_angles));
	if(wrap_dst < ang_dst) ang_dst = wrap_dst;
      }
      float wt;
      if(ang_dst == 0)
	wt = 0.0f;
      else if(ang_dst == 1)
	wt = v1c_specs.es_adjang_wt;
      else
	wt = 1.0f;
      v1c_es_angwts.FastEl(ang1, ang2) = wt;
    }
  }

  // pre_gp4 guys -- center points relative to lower-left corner of 4x4 group
  v1c_gp4_stencils.SetGeom(3, 3, 10, 4);
  // lengths stored in position 2 of first point
  v1c_gp4_stencils.FastEl(2,0,0) = 8;
  v1c_gp4_stencils.FastEl(2,0,1) = 10;
  v1c_gp4_stencils.FastEl(2,0,2) = 8;
  v1c_gp4_stencils.FastEl(2,0,3) = 10;
  for(int lpdx=0; lpdx < 10; lpdx++) {
    // 0 = 0 deg
    v1c_gp4_stencils.FastEl(X, lpdx, 0) = 1 + lpdx / 4;
    v1c_gp4_stencils.FastEl(Y, lpdx, 0) = lpdx % 4;
    // 1 = 45 deg
    v1c_gp4_stencils.FastEl(X, lpdx, 1) = 2 + lpdx/5 - (lpdx % 5)/2;
    v1c_gp4_stencils.FastEl(Y, lpdx, 1) = lpdx/5 + ((lpdx%5)+1)/2;
    // 2 = 90 deg
    v1c_gp4_stencils.FastEl(X, lpdx, 2) = lpdx % 4;
    v1c_gp4_stencils.FastEl(Y, lpdx, 2) = 1 + lpdx / 4;
    // 3 = 135 deg
    v1c_gp4_stencils.FastEl(X, lpdx, 3) = lpdx/5 + (lpdx % 5)/2;
    v1c_gp4_stencils.FastEl(Y, lpdx, 3) = (1 - lpdx/5) + ((lpdx%5)+1)/2;
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
    if(region.ocularity == VisRegionParams::BINOCULAR)
      v1s_out_l.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    else
      v1s_out_l.SetGeom(1,1);	// free memory
    v1s_out_r_max.SetGeom(2, v1s_img_geom.x, v1s_img_geom.y);
  }
  else {
    v1s_out_r.SetGeom(5, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y,
		      motion_frames);
    if(region.ocularity == VisRegionParams::BINOCULAR)
      v1s_out_l.SetGeom(5, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y,
			motion_frames);
    else
      v1s_out_l.SetGeom(1,1);	// free memory
    v1s_out_r_max.SetGeom(3, v1s_img_geom.x, v1s_img_geom.y, motion_frames);
  }
  v1s_circ_r.Reset();
  v1s_circ_l.Reset();
  v1s_out_r_raw.SetGeomN(v1s_out_r.geom);
  v1s_out_l_raw.SetGeomN(v1s_out_l.geom);

  if(v1s_filters & MAX_POLS) {
    v1s_maxpols_out_r.SetGeom(4, v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
    if(region.ocularity == VisRegionParams::BINOCULAR)
      v1s_maxpols_out_l.SetGeom(4, v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
    else
      v1s_maxpols_out_l.SetGeom(1,1);
  }
  else {
    v1s_maxpols_out_r.SetGeom(1,1);
    v1s_maxpols_out_l.SetGeom(1,1);
  }

  ///////////////////  V1C Output ////////////////////////
  v1c_pre.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1c_pre_geom.x, v1c_pre_geom.y);
  v1c_pre_polinv.SetGeom(4, v1c_polinv_geom.x, v1c_polinv_geom.y, v1c_pre_geom.x, v1c_pre_geom.y);
  v1c_esls_raw.SetGeom(4, v1c_feat_geom.x, 2, v1c_pre_geom.x, v1c_pre_geom.y);
  v1c_out.SetGeom(4, v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);
  v1c_out_raw.SetGeomN(v1c_out.geom);

  ///////////////////  V1B Output ////////////////////////
  if(v1b_filters & REQ_V1B_C) {
    v1b_v1c_pre.SetGeom(5, v1s_feat_geom.x, v1s_feat_geom.y, v1c_pre_geom.x, v1c_pre_geom.y,
			v1b_specs.tot_disps);
    v1b_v1c_pre_polinv.SetGeom(5, v1s_feat_geom.x, 1, v1c_pre_geom.x, v1c_pre_geom.y,
			       v1b_specs.tot_disps);
    v1b_v1c_out.SetGeom(5, v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y,
			v1b_specs.tot_disps);
    v1b_v1c_out_raw.SetGeomN(v1b_v1c_out.geom);
    
    if(v1b_filters & V1B_C_FM_IN) {
      v1b_dsp_in.SetGeom(4, v1b_specs.tot_disps, 1, v1c_pre_geom.x, v1c_pre_geom.y);
      v1b_dsp_ang_in.SetGeom(4, v1s_specs.n_angles, v1b_specs.tot_disps, v1c_pre_geom.x, v1c_pre_geom.y);
    }
  }
  else {
    v1b_v1c_pre.SetGeom(1,1);
    v1b_v1c_pre_polinv.SetGeom(1,1);
    v1b_v1c_out.SetGeom(1,1);
    v1b_v1c_out_raw.SetGeom(1,1);
  }

  if(region.ocularity == VisRegionParams::BINOCULAR) {
    v1b_dsp_horiz.SetGeom(3, DHZ_N, v1s_img_geom.x, v1s_img_geom.y);

    if(v1b_specs.dsp_ang) {
      v1b_dsp_ang_out.SetGeom(4, v1s_specs.n_angles, v1b_specs.tot_disps,
			      v1s_img_geom.x, v1s_img_geom.y);
      v1b_dsp_ang_out_tmp.SetGeom(4, v1s_specs.n_angles, v1b_specs.tot_disps,
				  v1s_img_geom.x, v1s_img_geom.y);


      v1b_dsp_nmatch.SetGeom(1,1);
      v1b_dsp_flags.SetGeom(1,1);
      v1b_dsp_match.SetGeom(1,1);
      v1b_dsp_win.SetGeom(1,1);

      v1b_dsp_out.SetGeom(1,1);
      v1b_dsp_wts.SetGeom(1,1);

      v1b_dsp_out_pre.SetGeom(1,1);
      if(v1b_filters & V1B_C) {
	v1b_dsp_ang_out_pre.SetGeom(4, v1s_specs.n_angles, v1b_specs.tot_disps,
				    v1c_pre_geom.x, v1c_pre_geom.y);
      }
      else {
	v1b_dsp_ang_out_pre.SetGeom(1,1);
      }
    }
    else {
      v1b_dsp_nmatch.SetGeom(2, v1s_img_geom.x, v1s_img_geom.y);
      v1b_dsp_flags.SetGeom(2, v1s_img_geom.x, v1s_img_geom.y);
      v1b_dsp_match.SetGeom(4, 2, v1b_dsp_specs.n_matches, v1s_img_geom.x, v1s_img_geom.y);
      v1b_dsp_win.SetGeom(3, DSP_N, v1s_img_geom.x, v1s_img_geom.y);
      v1b_dsp_horiz.SetGeom(3, DHZ_N, v1s_img_geom.x, v1s_img_geom.y);

      v1b_dsp_out.SetGeom(4, v1b_specs.tot_disps, 1, v1s_img_geom.x, v1s_img_geom.y);
      v1b_dsp_wts.SetGeom(4, v1b_specs.tot_disps, 1, v1s_img_geom.x, v1s_img_geom.y);

      v1b_dsp_ang_out.SetGeom(1,1);
      v1b_dsp_ang_out_tmp.SetGeom(1,1);
      v1b_dsp_ang_out_pre.SetGeom(1,1);

      if(v1b_filters & V1B_C) {
	v1b_dsp_out_pre.SetGeom(4, v1b_specs.tot_disps, 1, v1c_pre_geom.x, v1c_pre_geom.y);
      }
      else {
	v1b_dsp_out_pre.SetGeom(1,1);
      }
    }

    if(v1b_filters & REQ_V1B_S) {
      v1b_s_out.SetGeom(4, v1b_s_feat_geom.x, v1b_s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    }
    else {
      v1b_s_out.SetGeom(1,1);
    }
  }
  else {
    v1b_dsp_nmatch.SetGeom(1,1);
    v1b_dsp_flags.SetGeom(1,1);
    v1b_dsp_match.SetGeom(1,1);
    v1b_dsp_win.SetGeom(1,1);
    v1b_dsp_horiz.SetGeom(1,1);
    v1b_dsp_out.SetGeom(1,1);
    v1b_dsp_wts.SetGeom(1,1);
    //    v1b_dsp_in.SetGeom(1,1);  // let this one be..
    v1b_s_out.SetGeom(1,1);
  }

  ///////////////////  OPT Output ////////////////////////

  v1b_avgsum_out = 0.0f;

  return true;
}

void V1RegionSpec::IncrTime() {
  inherited::IncrTime();

  if(motion_frames <= 1) {
    return;		// nop
  }
  else {
    v1s_circ_r.CircAddLimit(motion_frames);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      v1s_circ_l.CircAddLimit(motion_frames);
    }
  }
}

////////////////////////////////////////////////////////////////////
//	V1Region 	Filtering

bool V1RegionSpec::FilterImage_impl() {
  inherited::FilterImage_impl(); // do dogs first

  // todo: maybe check rval for fail and bail -- not currently used..

  wrap = (region.edge_mode == VisRegionParams::WRAP);

  bool rval = V1SimpleFilter();

  if(rval && v1c_filters != CF_NONE) {
    rval &= V1ComplexFilter();
  }

  if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
    rval &= V1BinocularFilter();
  }

  if(rval) {
    rval &= V1OptionalFilter();
  }

  if(!data_table || save_mode == NONE_SAVE) // bail now
    return rval;

  if(v1s_save & SAVE_DATA && !(!taMisc::gui_active && v1s_save & ONLY_GUI)) {
    V1SOutputToTable(data_table);
  }
  if(v1c_save & SAVE_DATA && !(!taMisc::gui_active && v1c_save & ONLY_GUI)) {
    V1COutputToTable(data_table);
  }
  if(v1b_save & SAVE_DATA && !(taMisc::gui_active && v1b_save & ONLY_GUI)
     && (region.ocularity == VisRegionParams::BINOCULAR || v1b_filters & REQ_V1B_C)) {
    V1BOutputToTable(data_table);
  }
  if(opt_save & SAVE_DATA && !(!taMisc::gui_active && opt_save & ONLY_GUI)) {
    OptOutputToTable(data_table);
  }

  return rval;
}


bool V1RegionSpec::V1SimpleFilter() {
  bool rval = V1SimpleFilter_Static(cur_img_r, &v1s_out_r_raw, &v1s_out_r, &v1s_circ_r);
  if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
    rval &= V1SimpleFilter_Static(cur_img_l, &v1s_out_l_raw, &v1s_out_l, &v1s_circ_l);
  }

  if(motion_frames > 1) {
    rval &= V1SimpleFilter_Motion(&v1s_out_r, &v1s_circ_r);
    if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
      rval &= V1SimpleFilter_Motion(&v1s_out_l, &v1s_circ_l);
    }
  }

  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_OutMax_thread);
  threads.Run(&ip_call, n_run);

  if(v1s_filters & MAX_POLS) {
    rval &= V1SimpleFilter_MaxPols(&v1s_out_r, &v1s_maxpols_out_r, &v1s_circ_r);
    if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
      rval &= V1SimpleFilter_MaxPols(&v1s_out_l, &v1s_maxpols_out_l, &v1s_circ_l);
    }
  }

  return rval;
}

bool V1RegionSpec::V1SimpleFilter_Static(float_Matrix* image, float_Matrix* out_raw,
					 float_Matrix* out, CircMatrix* circ) {
  cur_img = image;
  cur_circ = circ;
  rgb_img = (cur_img->dims() == 3);

  if(rgb_img) {
    ColorRGBtoCMYK(*cur_img);	// precompute!
  }

  if(v1s_kwta.on) {
    cur_out = out_raw;
    cur_out_acts = out;
  }
  else {
    cur_out = out;
  }
  cur_circ = circ;

  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Static_thread);
  threads.Run(&ip_call, n_run);

  if(v1s_renorm != NO_RENORM) {		   // always renorm prior to any kwta
    V1SRenormOutput_Static(cur_out, circ); // only do static, separate from motion
  }

  if(v1s_kwta.on) {
//     v1s_kwta.Compute_Kwta(*out_raw, *out, v1s_gci);
    v1s_kwta.Compute_Inhib_IThr(*out_raw, v1s_gci, v1s_ithr);
    if(v1s_neigh_inhib.on) {
      ThreadImgProcCall ip_call_ni((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Static_neighinhib_thread);
      threads.Run(&ip_call_ni, n_run);
    }
    else {
      v1s_kwta.Compute_Act(*out_raw, *out, v1s_gci);
    }
  }

  return true;
}

void V1RegionSpec::V1SimpleFilter_Static_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);
  TwoDCoord icc = input_size.border + v1s_specs.spacing * sc; // image coords center

  int mot_idx = cur_circ->CircIdx_Last(); // always write to last position

  float_Matrix* v1s_img = cur_img;

  int ctr_off;
  if(v1s_specs.filter_size % 2 == 0)
    ctr_off = v1s_specs.filter_size / 2;
  else
    ctr_off = (v1s_specs.filter_size-1) / 2;

  icc -= ctr_off;		// always offset

  TwoDCoord ic;		// image coord
  for(int chan = 0; chan < n_colors; chan++) { 
    ColorChannel cchan = (ColorChannel)chan;
    if(rgb_img) {
      v1s_img = GetImageForChan(cchan);
    }

    int fcy = chan * n_polarities; // starting of y axis -- add 1 for off-polarity

    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { 
      float cnv_sum = 0.0f;		// convolution sum
      if(chan == 0 || rgb_img) {		// only rgb images if chan > 0
	for(int yf = 0; yf < v1s_specs.filter_size; yf++) {
	  for(int xf = 0; xf < v1s_specs.filter_size; xf++) {
	    ic.y = icc.y + yf;
	    ic.x = icc.x + xf;
	    if(ic.WrapClip(wrap, input_size.retina_size)) {
	      if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	    }
	    cnv_sum += v1s_gabor_filters.FastEl(xf, yf, ang) * v1s_img->FastEl(ic.x, ic.y);
	  }
	}
      }
      cnv_sum *= v1s_specs.gain;
      if(cnv_sum >= 0.0f) {
	MatMotEl(cur_out, ang, fcy, sc.x, sc.y, mot_idx) = cnv_sum; // on-polarity
	MatMotEl(cur_out, ang, fcy+1, sc.x, sc.y, mot_idx) = 0.0f; 
      }
      else {
	MatMotEl(cur_out, ang, fcy, sc.x, sc.y, mot_idx) = 0.0f; 	
	MatMotEl(cur_out, ang, fcy+1, sc.x, sc.y, mot_idx) = -cnv_sum; // off-polarity
      }
    }
  }
}

void V1RegionSpec::V1SimpleFilter_Static_neighinhib_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  float gi = v1s_gci.FastEl(sc.x, sc.y);

  TwoDCoord fc;		// v1s feature coords
  TwoDCoord oc;		// other coord
  for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
    fc.y = polclr;
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      fc.x = ang;
      float raw = cur_out->FastEl(fc.x, fc.y, sc.x, sc.y);
      float feat_inhib_max = 0.0f;
      for(int lpdx=0; lpdx < v1s_neigh_inhib.tot_ni_len; lpdx++) { // go out to neighs
	if(lpdx == v1s_neigh_inhib.inhib_d) continue;		   // skip self
	int xp = v1s_ni_stencils.FastEl(X,lpdx,ang);
	int yp = v1s_ni_stencils.FastEl(Y,lpdx,ang);
	oc.x = sc.x + xp;
	oc.y = sc.y + yp;
	if(oc.WrapClip(wrap, v1s_img_geom)) {
	  if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	}
	float oth_ithr = v1s_ithr.FastEl(fc.x, fc.y, oc.x, oc.y); // other guy
	// weights already have gain factor built in
	float ogi = v1s_neigh_inhib.inhib_g * oth_ithr; // note: directly on ithr!
	feat_inhib_max = MAX(feat_inhib_max, ogi);
      }

      float ge = v1s_kwta.g_bar_e * raw;
      float gi_eff = MAX(gi, feat_inhib_max);
      float act = v1s_kwta.Compute_ActFmIn(ge, gi_eff);
      cur_out_acts->FastEl(fc.x, fc.y, sc.x,  sc.y) = act; 
    }
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
      for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
	fc.y = polclr;
	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	  fc.x = ang;
	  float val = mat->FastEl(fc.x, fc.y, sc.x, sc.y);
	  max_val = MAX(val, max_val);
	}
      }
    }
  }
  if(max_val > region.renorm_thr) {
    rval = true;
    if(v1s_renorm == LIN_RENORM) {
      float rescale = 1.0f / max_val;
      for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	  for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
	    fc.y = polclr;
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
	  for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
	    fc.y = polclr;
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

	  float cur_val = cur_out->FastEl(sfc.x, sfc.y, mo.x, mo.y, cur_mot_idx);
	  float sum_mot = 0.0f;
	  
	  int mx_mot = mot_len-1; // don't go up to last value
	  for(int mot = 0; mot < mx_mot; mot++) { // time steps back in time
	    for(int tw = -v1s_motion.tuning_width; tw <= v1s_motion.tuning_width; tw++) {
	      int twidx = v1s_motion.tuning_width+tw;
	      int xp = v1m_stencils.FastEl(X, twidx, mot, dir, ang, speed);
	      int yp = v1m_stencils.FastEl(Y, twidx, mot, dir, ang, speed);

	      mo.x = sc.x + xp;
	      mo.y = sc.y + yp;
	      if(mo.WrapClip(wrap, v1s_img_geom)) {
		if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	      }

	      float val = cur_out->FastEl(sfc.x, sfc.y, mo.x, mo.y,
					  cur_circ->CircIdx(mot_len-1 - mot));
	      sum_mot += val * v1m_weights.FastEl(twidx);
	    }
	  }
	  cur_out->FastEl(fc.x, fc.y, sc.x, sc.y, cur_mot_idx) = cur_val * sum_mot;
	  // product rule for matching!
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
  if(max_val > region.renorm_thr) {
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


void V1RegionSpec::V1SimpleFilter_OutMax_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int cur_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord fc;		// v1s feature coords
  float max_feat = 0.0f;
  for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features -- includes b/w on/off
    fc.y = polclr;
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // just max over angles -- blobify!
      fc.x = ang;
      float v1sval = MatMotEl(&v1s_out_r, fc.x, fc.y, sc.x, sc.y, cur_mot_idx);
      max_feat = MAX(max_feat, v1sval);
    }
  }

  v1s_out_r_max.FastEl(sc.x, sc.y) = max_feat;
}

bool V1RegionSpec::V1SimpleFilter_MaxPols(float_Matrix* v1s_out_in,
					 float_Matrix* maxpols_out, CircMatrix* circ) {
  cur_img = v1s_out_in;		// using img for this..
  cur_circ = circ;
  cur_out = maxpols_out;

  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_MaxPols_thread);
  threads.Run(&ip_call, n_run);

  return true;
}

void V1RegionSpec::V1SimpleFilter_MaxPols_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int cur_mot_idx = cur_circ->CircIdx_Last();

  TwoDCoord fc;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    fc.x = ang;
    float max_pi = 0.0f;
    for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
      fc.y = polclr;
      float val = MatMotEl(cur_img, fc.x, fc.y, sc.x, sc.y, cur_mot_idx);
      max_pi = MAX(max_pi, val);      
    }
    cur_out->FastEl(fc.x, 0, sc.x, sc.y) = max_pi;
  }
}

bool V1RegionSpec::V1ComplexFilter() {
  if(v1c_kwta.on) {
    cur_out = &v1c_out_raw;
    cur_v1c_kwta_out = &v1c_out;
  }
  else {
    cur_out = &v1c_out;
  }

  int n_run = v1c_img_geom.Product();
  int n_run_pre = v1c_pre_geom.Product();
  int n_run_v1s = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  // first, pre-group to optimize computation
  if(v1c_specs.pre_gp4) {
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_Pre_thread);
    threads.Run(&ip_call, n_run_pre);
  }
  else {
    if(v1c_specs.border != 0) {
      // need to aggregate v1b_dsp_out into v1b_dsp_out_pre to remove border
      ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_PreBord_thread);
      threads.Run(&ip_call, n_run_pre);
    }
    else {
      v1c_pre.CopyFrom(&v1s_out_r); // just a copy
    }
  }

  if(v1c_filters & CF_ESLS) {
    if(v1c_filters & END_STOP) {
      ThreadImgProcCall ip_call_polinv((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_Pre_Polinv_thread);
      threads.Run(&ip_call_polinv, n_run_pre);
    }
  }

  cur_v1c_pre = &v1c_pre;
  cur_v1c_pre_polinv = &v1c_pre_polinv;

  bool rval = V1ComplexFilter_impl(); // process everything based on current pre inputs
  return rval;
}

bool V1RegionSpec::V1ComplexFilter_impl() {
  // cur_out, cur_v1c_pre and cur_v1c_pre_polinv must all be set prior to calling this!

  int n_run = v1c_img_geom.Product();
  int n_run_pre = v1c_pre_geom.Product();
  int n_run_v1s = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  if(v1c_filters & CF_ESLS) {
    ThreadImgProcCall ip_call_raw((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_EsLs_Raw_thread);
    threads.Run(&ip_call_raw, n_run_pre);
    ThreadImgProcCall ip_call_integ((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_EsLs_Integ_thread);
    threads.Run(&ip_call_integ, n_run);
  }
  if(v1c_filters & V1S_MAX) {
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_V1SMax_thread);
    threads.Run(&ip_call, n_run);
  }
  if(v1c_filters & BLOB) {
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_Blob_thread);
    threads.Run(&ip_call, n_run);
  }

  // always renorm *prior* to any kwta
  if(v1c_renorm != NO_RENORM) {
    V1CRenormOutput_EsLsBlob(cur_out);
    //    RenormOutput_NoFrames(v1c_renorm, cur_out);  // this renorms everything together, which is not as good as separately renorming the blob vs. the rest of the guys
  }

  if(v1c_kwta.on) {
    v1c_kwta.Compute_Kwta(*cur_out, *cur_v1c_kwta_out, v1c_gci);
  }

  return true;
}


void V1RegionSpec::V1ComplexFilter_Pre_thread(int v1c_pre_idx, int thread_no) {
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);
  TwoDCoord scs = v1c_specs.pre_spacing * pc; // v1s coords start
  scs += v1c_specs.pre_border;
  scs += v1c_specs.border;
  scs -= v1c_specs.pre_half; // convert to lower-left starting position, not center

  int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord sc;			// simple coord
  TwoDCoord scc;		// simple coord, center
  TwoDCoord sfc;		// v1s feature coords
  for(int sfi = 0; sfi < v1s_feat_geom.n; sfi++) { // full scale integration
    sfc.SetFmIndex(sfi, v1s_feat_geom.x);
    float max_rf = 0.0f;   // max over spatial rfield
    int ang = sfc.x;
    int nctrs = v1c_gp4_stencils.FastEl(2, 0, ang);	 // length stored here
    for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
      int xp = v1c_gp4_stencils.FastEl(X, ctrdx, ang);
      int yp = v1c_gp4_stencils.FastEl(Y, ctrdx, ang);
      sc.y = scs.y + yp;
      sc.x = scs.x + xp;
      scc = sc;	// center
      if(scc.WrapClip(wrap, v1s_img_geom)) {
	if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
      }
      float ctr_val = MatMotEl(&v1s_out_r, sfc.x, sfc.y, scc.x, scc.y, v1s_mot_idx);
      max_rf = MAX(max_rf, ctr_val);
    }
    v1c_pre.FastEl(sfc.x, sfc.y, pc.x, pc.y) = max_rf;
  }
}

void V1RegionSpec::V1ComplexFilter_PreBord_thread(int v1c_pre_idx, int thread_no) {
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);
  TwoDCoord scs = pc; // v1s coords start
  scs += v1c_specs.border;

  int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord sc;			// simple coord
  TwoDCoord sfc;		// v1s feature coords
  for(int sfi = 0; sfi < v1s_feat_geom.n; sfi++) { // full scale integration
    sfc.SetFmIndex(sfi, v1s_feat_geom.x);
    float v1s_val = MatMotEl(&v1s_out_r, sfc.x, sfc.y, scs.x, scs.y, v1s_mot_idx);
    v1c_pre.FastEl(sfc.x, sfc.y, pc.x, pc.y) = v1s_val;
  }
}

void V1RegionSpec::V1ComplexFilter_Pre_Polinv_thread(int v1c_pre_idx, int thread_no) {
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);

  TwoDCoord fc;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    fc.x = ang;
    float max_pi = 0.0f;
    for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
      fc.y = polclr;
      float val = v1c_pre.FastEl(fc.x, fc.y, pc.x, pc.y);
      max_pi = MAX(max_pi, val);
    }
    v1c_pre_polinv.FastEl(fc.x, 0, pc.x, pc.y) = max_pi;
  }
}

void V1RegionSpec::V1ComplexFilter_EsLs_Raw_thread(int v1c_pre_idx, int thread_no) {
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);

  TwoDCoord fc;			// for storing in v1c_esls_raw
  TwoDCoord pce;		// pre coord, ends
  TwoDCoord sfc_ctr;		// simple feature coords for the central point
  TwoDCoord sfc_end;		// simple feature coords for the end point

  sfc_ctr.y = 0;
  sfc_end.y = 0;

  // first do end stop
  if(v1c_filters & END_STOP) {
    fc.y = 0;
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      sfc_ctr.x = ang;
      sfc_end.x = ang;
      fc.x = ang;

      // first get central value -- always the same
      float ctr_val = cur_v1c_pre_polinv->FastEl(sfc_ctr.x, sfc_ctr.y, pc.x, pc.y);
      
      float es_max = 0.0f; // es response is max of activity in either end of the symmetric end stop "opposite polarity" rf
      for(int sidx=0; sidx < 2; sidx++) {
	float sidesum = 0.0f;
	for(int lpdx=0; lpdx < 2; lpdx++) {
	  int xp = v1c_es_stencils.FastEl(X,lpdx,sidx,ang);
	  int yp = v1c_es_stencils.FastEl(Y,lpdx,sidx,ang);
	  pce.x = pc.x + xp;
	  pce.y = pc.y + yp;

	  if(pce.WrapClip(wrap, v1c_pre_geom)) {
	    if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	  }

	  float end_val = 0.0f;
	  // compute max over other angles -- just not us..
	  for(int opang=0; opang<v1s_specs.n_angles; opang++) {
	    float angwt = v1c_es_angwts.FastEl(ang, opang);
	    if(angwt == 0.0f) continue;
	    float ev = angwt * cur_v1c_pre_polinv->FastEl(opang, sfc_end.y, pce.x, pce.y);
	    end_val = MAX(end_val, ev);
	  }
	  sidesum += end_val;
	}
	sidesum *= 0.5f;	       // norm
	es_max = MAX(es_max, sidesum);
      }
      es_max = MIN(es_max, ctr_val); // cannot exceed central value -- this is the primary
      // 	  es_max *= ctr_val; 	// product rule to take into account central value -- too punative once the kwta dynamics kick in...
      v1c_esls_raw.FastEl(fc.x, fc.y, pc.x, pc.y) = v1c_specs.es_gain * es_max;
    }  // for ang
  }

  // then do length sum
  if(v1c_filters & LEN_SUM) {
    fc.y = 1;
    
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      fc.x = ang;
      float max_sf = 0.0f;	// max over simple features
      for(int v1sf = 0; v1sf < v1s_feat_geom.y; v1sf++) {
	// v1 simple features -- for end-stop, designates the sign of the center..
	int v1sf_onst = 2 * (v1sf / 2); // even numbers = start of on-center
	sfc_ctr.x = ang;
	sfc_ctr.y = v1sf;
	sfc_end.x = ang;
	sfc_end.y = v1sf;

	// first get central value -- always the same
	float ctr_val = cur_v1c_pre->FastEl(sfc_ctr.x, sfc_ctr.y, pc.x, pc.y);
	float line_sum = 0.0f;
	for(int lpt=-v1c_specs.len_sum_len; lpt <= v1c_specs.len_sum_len; lpt++) {
	  if(lpt == 0) continue; // skip center
	  int lpdx = lpt + v1c_specs.len_sum_len;
	  int xp = v1c_ls_stencils.FastEl(X,lpdx,ang);
	  int yp = v1c_ls_stencils.FastEl(Y,lpdx,ang);
	  pce.x = pc.x + xp;
	  pce.y = pc.y + yp;

	  if(pce.WrapClip(wrap, v1c_pre_geom)) {
	    if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	  }

	  float end_val = cur_v1c_pre->FastEl(sfc_end.x, sfc_end.y, pce.x, pce.y);
	  line_sum += end_val;
	}
	line_sum = (ctr_val + line_sum) * v1c_specs.len_sum_norm;
	// 	  line_sum *= v1c_specs.len_sum_norm * ctr_val;
	// also use product rule for central value -- makes it more analogous to end stop, with similar overall activation levels too -- too punative 
	line_sum = MIN(line_sum, ctr_val); // use the min just like end stop..
	max_sf = MAX(line_sum, max_sf);
      }	// for v1sf
      v1c_esls_raw.FastEl(fc.x, fc.y, pc.x, pc.y) = max_sf;
    }  // for ang
  }
}

void V1RegionSpec::V1ComplexFilter_EsLs_Integ_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;			// complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);
  TwoDCoord pcs = v1c_specs.spat_spacing * cc; // v1c_pre coords start
  pcs += v1c_specs.spat_border;
  pcs -= v1c_specs.spat_half; // convert to lower-left starting position, not center

  TwoDCoord pc;			// pre coord
  TwoDCoord pcc;		// pre coord, center
  TwoDCoord fc;			// v1c feature coords
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
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // yspat
	pc.y = pcs.y + ys;
	for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xspat
	  pc.x = pcs.x + xs;
	  pcc = pc;	// center
	  if(pcc.WrapClip(wrap, v1c_pre_geom)) {
	    if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	  }
	  float max_sf = v1c_esls_raw.FastEl(fc.x, cfeat, pcc.x, pcc.y); // use cfeat -- raw stored that way
	  max_sf *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
	  max_rf = MAX(max_rf, max_sf);
	}
      }
      cur_out->FastEl(fc.x, fc.y, cc.x, cc.y) = max_rf;
    }  // for ang
  }
}

void V1RegionSpec::V1ComplexFilter_V1SMax_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;			// complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);
  TwoDCoord pcs = v1c_specs.spat_spacing * cc; // v1s coords start
  pcs += v1c_specs.spat_border;
  pcs -= v1c_specs.spat_half; // convert to lower-left starting position, not center

  TwoDCoord pc;			// pre coord
  TwoDCoord pcc;		// pre coord, center
  TwoDCoord sfc;		// v1s feature coords
  TwoDCoord fc;			// v1c feature coords
  for(int polclr = 0; polclr < 2; polclr++) { // only first monochrome on/off guys
    sfc.y = polclr;
    fc.y = v1c_feat_smax_y + polclr;
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      sfc.x = ang;
      fc.x = ang;
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // yspat
	pc.y = pcs.y + ys;
	for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xspat
	  pc.x = pcs.x + xs;
	  pcc = pc;	// center
	  if(pcc.WrapClip(wrap, v1c_pre_geom)) {
	    if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	  }
	  float ctr_val = cur_v1c_pre->FastEl(sfc.x, sfc.y, pcc.x, pcc.y);
	  ctr_val *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
	  max_rf = MAX(max_rf, ctr_val);
	}
      }
      cur_out->FastEl(fc.x, fc.y, cc.x, cc.y) = max_rf;
    } // for ang
  }  // for polclr
}

void V1RegionSpec::V1ComplexFilter_Blob_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;			// complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);
  TwoDCoord pcs = v1c_specs.spat_spacing * cc; // v1c_pre coords start
  pcs += v1c_specs.spat_border;
  pcs -= v1c_specs.spat_half; // convert to lower-left starting position, not center

  TwoDCoord pc;			// pre coord
  TwoDCoord pcc;		// pre coord, center
  TwoDCoord sfc;		// v1s feature coords
  TwoDCoord fc;			// v1c feature coords
  for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features -- includes b/w on/off
    sfc.y = polclr;
    fc.y = v1c_feat_blob_y + polclr / v1c_feat_geom.x;
    fc.x = polclr % v1c_feat_geom.x;
    float max_rf = 0.0f;   // max over spatial rfield
    for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // yspat
      pc.y = pcs.y + ys;
      for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xspat
	pc.x = pcs.x + xs;
	pcc = pc;	// center
	if(pcc.WrapClip(wrap, v1c_pre_geom)) {
	  if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	}
	// todo: could pre-compute this as a blob_raw guy in pre coords..
	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // just max over angles -- blobify!
	  sfc.x = ang;
	  float ctr_val = cur_v1c_pre->FastEl(sfc.x, sfc.y, pcc.x, pcc.y);
	  ctr_val *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
	  max_rf = MAX(max_rf, ctr_val);
	}
      }
    }
    cur_out->FastEl(fc.x, fc.y, cc.x, cc.y) = max_rf;
  }
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
	for(int polclr = 0; polclr < 2; polclr++) {
	  fc.y=v1c_feat_smax_y + polclr;
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
	if(region.color == VisRegionParams::COLOR) {
	  for(int pol = 2; pol < 8; pol++) { // polarities
	    fc.x = pol % v1c_feat_geom.x;
	    float val = out->FastEl(fc.x, fc.y + pol / v1c_feat_geom.x, cc.x, cc.y);
	    blob_c_max_val = MAX(val, blob_c_max_val);
	  }
	}
      }
    }
  }

  float group_max = 0.0f;
  if(!(v1c_sep_renorm & END_STOP))
    group_max = MAX(group_max, es_max_val);
  if(!(v1c_sep_renorm & LEN_SUM))
    group_max = MAX(group_max, ls_max_val);
  if(!(v1c_sep_renorm & V1S_MAX))
    group_max = MAX(group_max, smax_max_val);

  if(!(v1c_sep_renorm & END_STOP))
    es_max_val = group_max;
  if(!(v1c_sep_renorm & LEN_SUM))
    ls_max_val = group_max;
  if(!(v1c_sep_renorm & V1S_MAX))
    smax_max_val = group_max;

  if(es_max_val > region.renorm_thr) {	// nonblank
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
	    for(int polclr = 0; polclr < 2; polclr++) {
	      fc.y=v1c_feat_smax_y + polclr;
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
	    if(region.color == VisRegionParams::COLOR) {
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
	    for(int polclr = 0; polclr < 2; polclr++) {
	      fc.y=v1c_feat_smax_y + polclr;
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
	    if(region.color == VisRegionParams::COLOR) {
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


////////////////////////////////////////////////////////
//		Binocular Filter

bool V1RegionSpec::V1BinocularFilter() {
  int n_run_s = v1s_img_geom.Product();
  int n_run_pre = v1c_pre_geom.Product();
  int n_run_c = v1c_img_geom.Product();

  threads.n_threads = MIN(n_run_s, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  if(v1b_specs.dsp_ang) {
    ThreadImgProcCall ip_call_dspang((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_DspAng_thread);
    threads.Run(&ip_call_dspang, n_run_s);
  }
  else {
    // the following sequence is all the initial required processing for any subsequent step

    v1b_dsp_nmatch.InitVals(0);
    v1b_dsp_flags.InitVals(0);

    dsp_stats.InitStats();

    ThreadImgProcCall ip_call_match((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_Match_thread);
    threads.Run(&ip_call_match, n_run_s);

    // first tag horiz line elements in parallel
    v1b_dsp_horiz.InitVals(-1);
    ThreadImgProcCall ip_call_horiz((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_HorizTag_thread);
    threads.Run(&ip_call_horiz, n_run_s);

    V1BinocularFilter_HorizAgg();	// then aggregate and correct disparity

    // then generate net and final output
    ThreadImgProcCall ip_call_dspout((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_DspOut_thread);
    threads.Run(&ip_call_dspout, n_run_s);

    dsp_stats.ComputeStats();

    // finally aggregate based on output!
    ThreadImgProcCall ip_call_winagg((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_WinAgg_thread);
    threads.Run(&ip_call_winagg, n_run_s);
  }

  // now comes the optional outputs
  bool rval = V1BinocularFilter_Optionals();
  return rval;
}

bool V1RegionSpec::V1BinocularFilter_Optionals() {
  int n_run_s = v1s_img_geom.Product();
  int n_run_pre = v1c_pre_geom.Product();
  int n_run_c = v1c_img_geom.Product();

  threads.n_threads = MIN(n_run_s, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  // now comes the optional outputs
  if(!v1b_specs.dsp_ang && v1b_filters & REQ_V1B_S) {	// compute if required..
    cur_v1b_dsp = &v1b_dsp_out;	// use output values just computed
    ThreadImgProcCall ip_call_sout((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_S_Out_thread);
    threads.Run(&ip_call_sout, n_run_s);
    if(v1b_renorm != NO_RENORM) {
      RenormOutput_NoFrames(v1b_renorm, &v1b_s_out);
    }
  }

  if(v1b_filters & V1B_C) {
    if(v1c_specs.pre_gp4) {
      // need to aggregate v1b_dsp_out into v1b_dsp_out_pre to get into right format
      if(v1b_specs.dsp_ang) {
	ThreadImgProcCall ip_call_dspoutpre((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_DspAngOutPre_thread);
	threads.Run(&ip_call_dspoutpre, n_run_pre);
	cur_v1b_dsp = &v1b_dsp_ang_out_pre;	// use output values just computed
      }
      else {
	ThreadImgProcCall ip_call_dspoutpre((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_DspOutPre_thread);
	threads.Run(&ip_call_dspoutpre, n_run_pre);
	cur_v1b_dsp = &v1b_dsp_out_pre;	// use output values just computed
      }
    }
    else {
      if(v1c_specs.border != 0) {
	if(v1b_specs.dsp_ang) {
	  // need to aggregate v1b_dsp_out into v1b_dsp_out_pre to remove border
	  ThreadImgProcCall ip_call_dspoutpre((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_DspAngOutPreBord_thread);
	  threads.Run(&ip_call_dspoutpre, n_run_pre);
	  cur_v1b_dsp = &v1b_dsp_ang_out_pre;	// use output values just computed
	}
	else {
	  // need to aggregate v1b_dsp_out into v1b_dsp_out_pre to remove border
	  ThreadImgProcCall ip_call_dspoutpre((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_DspOutPreBord_thread);
	  threads.Run(&ip_call_dspoutpre, n_run_pre);
	  cur_v1b_dsp = &v1b_dsp_out_pre;	// use output values just computed
	}
      }
      else {
	if(v1b_specs.dsp_ang) {
	  cur_v1b_dsp = &v1b_dsp_ang_out;	// use v1s scale values
	}
	else {
	  cur_v1b_dsp = &v1b_dsp_out;	// use v1s scale values
	}
      }
    }

    if(v1b_specs.dsp_ang) {
      V1BinocularFilter_Complex_Pre_DspAng(); // pre-process by disparity
    }
    else {
      V1BinocularFilter_Complex_Pre(); // pre-process by disparity
    }
    V1BinocularFilter_Complex(); // then do all the complex processing
  }

  if(v1b_filters & V1B_AVGSUM) {
    V1BinocularFilter_AvgSum();
  }

  return true;
}


void V1RegionSpec::V1BinocularFilter_DspAng_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

//   int cur_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord bo;
  bo.y = sc.y;

  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    int dwd = v1b_widths.FastEl(didx);
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) {
      float rv = v1s_maxpols_out_r.FastEl(ang, 0, sc.x, sc.y);
      if(rv < v1b_dsp_specs.opt_thr) {
	v1b_dsp_ang_out.FastEl(ang, didx, sc.x, sc.y) = 0.0f;
	continue;
      }
      float lval = 0.0f;
      for(int twidx = 0; twidx < dwd; twidx++) {
	int off = v1b_stencils.FastEl(twidx, didx);
	bo.x = sc.x - off;
	if(bo.WrapClip(wrap, v1s_img_geom)) {
	  if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	}
	float lv = v1s_maxpols_out_l.FastEl(ang, 0, bo.x, bo.y);
	float lvwt = lv * v1b_weights.FastEl(twidx, didx);
	lval = MAX(lvwt, lval);			 // agg as max
      }
      float min_rl = MIN(rv, lval); // min = simple version of product..
      v1b_dsp_ang_out.FastEl(ang, didx, sc.x, sc.y) = min_rl;
    }
  }
}

void V1RegionSpec::V1BinocularFilter_Match_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int& flag = v1b_dsp_flags.FastEl(sc.x, sc.y);

  int cur_mot_idx = v1s_circ_r.CircIdx_Last();

  float maxfv = MatMotEl2D(&v1s_out_r_max, sc.x, sc.y, cur_mot_idx);
  if(maxfv < v1b_dsp_specs.opt_thr) {
    flag = DSP_NO_ACT;
    return;
  }

  TwoDCoord sfc;		// v1s feature coords -- source
  TwoDCoord bo;			// binoc offset
  bo.y = sc.y;			// this will never be wrapped or clipped so can be set once..

  int n_match = 0;		// total number of top-level matches recorded
  int n_good = 0;		// total number that exceed the basic goodness threshold
  bool has_rv = false;		// is there actually an above-threshold feature in the right visual input
  int max_idx = -1;		// index of max guy on top-matches list
  float max_dist = 100.0f;	// max guy on the current top-matches list
  for(int off = -v1b_specs.max_off; off <= v1b_specs.max_off; off++) {
    // collect average feature dist at this offset
    float sum_dist = 0.0f;
    int n_sum = 0;
    for(int sfi = 0; sfi < v1s_feat_geom.n; sfi++) { // simple feature index
      sfc.SetFmIndex(sfi, v1s_feat_geom.x);

      // response is anchored at corresponding location on the right (dominant) eye
      float rv = MatMotEl(&v1s_out_r, sfc.x, sfc.y, sc.x, sc.y, cur_mot_idx); // note: sc
      if(rv < v1b_dsp_specs.opt_thr) {
	continue;
      }

      bo.x = sc.x - off;
      if(bo.WrapClip(wrap, v1s_img_geom)) {
	if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
      }
      float lv = MatMotEl(&v1s_out_l, sfc.x, sfc.y, bo.x, bo.y, cur_mot_idx);
      float dist = fabsf(lv - rv) / MAX(lv, rv); // max-norm absolute difference of values
      sum_dist += dist;
      n_sum++;
    }
    if(n_sum == 0) continue;	// all empty -- shouldn't happen..
    has_rv = true;

    float avg_dist = sum_dist / (float)n_sum;
    if(avg_dist >= v1b_dsp_specs.good_thr) continue; // not good enough to consider
    n_good++;  // if too many good fits, then we're in trouble..

    // use the replace-the worst case strategy to maintain a list of top matches
    if(n_match == v1b_dsp_specs.n_matches) { // already saturated the list
      if(avg_dist > max_dist) continue; // worse than current worst case, bail
      // replace the guy that was the current max value on the list
      v1b_dsp_match.FastEl(DSP_DIST, max_idx, sc.x, sc.y) = avg_dist;
      v1b_dsp_match.FastEl(DSP_OFF, max_idx, sc.x, sc.y) = off;
      // now find the new min and max guy
      max_dist = 0.0f;
      for(int m=0; m<n_match; m++) {
	float mdst = v1b_dsp_match.FastEl(DSP_DIST, m, sc.x, sc.y);
	if(mdst > max_dist) {
	  max_dist = mdst;
	  max_idx = m;
	}
      }
    }
    else {			// otherwise add given that it is good
      if(n_match == 0 || avg_dist > max_dist) {
	max_dist = avg_dist;	// new threshold
	max_idx = n_match;
      }
      v1b_dsp_match.FastEl(DSP_DIST, n_match, sc.x, sc.y) = avg_dist;
      v1b_dsp_match.FastEl(DSP_OFF, n_match, sc.x, sc.y) = off;
      n_match++;
      v1b_dsp_nmatch.FastEl(sc.x, sc.y) = n_match;
    }
  }
  
//   if(n_good > v1b_dsp_specs.n_matches) {	// we matched more than we recorded -- mark as ambig
//     flag = DSP_AMBIG_N;
//   }

  if(has_rv && n_match == 0) {	// couldn't find any good matches, but has an rv
    flag = DSP_AMBIG_THR;
    return;			// nothing more to do..
  }

  // now pick the top match and call it good, and compute relative selectivity
  float max_wt = 0.0f;
  float sum_wt = 0.0f;
  int max_oidx = -1;
  for(int m=0; m<n_match; m++) {
    float mdst = v1b_dsp_match.FastEl(DSP_DIST, m, sc.x, sc.y);
    float wt = (1.0f - v1b_dsp_specs.thr_gain * mdst); // weight for this location
    sum_wt += wt;
    if(wt > max_wt) {
      max_wt = wt;
      int off = (int)v1b_dsp_match.FastEl(DSP_OFF, m, sc.x, sc.y);
      max_oidx = v1b_specs.max_off + off;
    }
  }

  float sel = max_wt / sum_wt;	// selectivity

  v1b_dsp_win.FastEl(DSP_DIST, sc.x, sc.y) = sel;
  v1b_dsp_win.FastEl(DSP_OFF, sc.x, sc.y) = max_oidx - v1b_specs.max_off;
}

void V1RegionSpec::V1BinocularFilter_HorizTag_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int& flag = v1b_dsp_flags.FastEl(sc.x, sc.y);
  if(flag == DSP_NO_ACT) return; // nothing to do here

  int cur_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord sfc;		// v1s feature coords -- source

  float sum_dist = 0.0f;
  int n_sum = 0;
  for(sfc.y = 0; sfc.y < v1s_feat_geom.y; sfc.y++) { // simple feature index
    // horiz value is always first in row..  get it
    float hv = MatMotEl(&v1s_out_r, 0, sfc.y, sc.x, sc.y, cur_mot_idx); // note: sc
    if(hv < v1b_dsp_specs.opt_thr) {
      continue;			// no chance
    }
    float max_rest = 0.0f;
    for(sfc.x = 1; sfc.x < v1s_feat_geom.x; sfc.x++) {		// rest of orients
      // response is anchored at corresponding location on the right (dominant) eye
      float rv = MatMotEl(&v1s_out_r, sfc.x, sfc.y, sc.x, sc.y, cur_mot_idx); // note: sc
      if(rv > max_rest) {
	max_rest = rv;
      }
    }

    if(hv >= max_rest) {
      v1b_dsp_horiz.FastEl(DHZ_LEN, sc.x, sc.y) = 1;
      v1b_dsp_horiz.FastEl(DHZ_START, sc.x, sc.y) = sc.x;
      return;			// done!  not gonna get any better..
    }
  }
}

void V1RegionSpec::V1BinocularFilter_HorizAgg() {
  TwoDCoord sc;			// simple coords
  for(sc.y=0; sc.y<v1s_img_geom.y; sc.y++) {
    int cur_st = -1;
    int cur_len = 0;
    int cur_mode = 0;		// 0 = nothing, 1 = completing line
    bool backprop = false;
    for(sc.x=0; sc.x<v1s_img_geom.x; sc.x++) {
      int ptlen = v1b_dsp_horiz.FastEl(DHZ_LEN, sc.x, sc.y);
      switch(cur_mode) {
      case 0: {			// nothing
	if(ptlen > 0) {		// got something
	  cur_len = 1;
	  cur_st = sc.x;
	  cur_mode = 1;
	}
	break;
      }
      case 1: {			// completing line
	if(ptlen > 0) {		// keep going
	  cur_len++;
	}
	else {			// ended
	  backprop = true;	// done!
	}
	break;
      }
      }

      if(backprop || (cur_mode > 0 && sc.x == v1s_img_geom.x-1)) {
	// propagate back to all the points along the horizontal line -- this is the key routine

	// first get aggregated votes along the line
	float* win_wt = new float[v1b_specs.tot_offs]; // sum of weights integrated over window
  
	for(int i=0; i<v1b_specs.tot_offs; i++) {
	  win_wt[i] = 0.0f;
	}

	int left_limit = v1b_dsp_specs.hz_win_sz;
	int rt_limit = cur_len - v1b_dsp_specs.hz_win_sz;

	for(int bxi=0; bxi < cur_len; bxi++) {
	  if(bxi >= left_limit && bxi < rt_limit) continue; // only the ends have any good data

	  int bx = cur_st + bxi;
	  if(bx < v1b_specs.edge_off || bx >= (v1s_img_geom.x - v1b_specs.edge_off))
	    continue;		// don't trust the edges!

 	  int nflag = v1b_dsp_flags.FastEl(bx, sc.y);
	  if(nflag == DSP_AMBIG_THR) continue; // has no relevant data

	  // use the final already-voted values here
	  float woff = v1b_dsp_win.FastEl(DSP_OFF, bx, sc.y);
	  float woffwt = v1b_dsp_win.FastEl(DSP_DIST, bx, sc.y);
	  int oidx = v1b_specs.max_off + woff;
	  win_wt[oidx] += woffwt;
	}

	// then find the max case
	float max_wt = 0.0f;
	float max_oidx = -1;
	for(int i=0; i<v1b_specs.tot_offs; i++) {
	  float wt_sum = win_wt[i];
	  if(wt_sum > max_wt) {
	    max_wt = wt_sum;
	    max_oidx = i;
	  }
	}

	delete win_wt;

	int max_off = max_oidx - v1b_specs.max_off;

	// then copy out to the whole line
	for(int bxi=0; bxi < cur_len; bxi++) {
	  int bx = cur_st + bxi;
	  int& bptlen = v1b_dsp_horiz.FastEl(DHZ_LEN, bx, sc.y);
	  int& bptst = v1b_dsp_horiz.FastEl(DHZ_START, bx, sc.y);
	  int& bflag = v1b_dsp_flags.FastEl(bx, sc.y);
	  bptlen = cur_len;
	  bptst = cur_st;

	  if(bflag != DSP_NO_ACT && cur_len >= v1b_dsp_specs.min_hz_len) {
	    float& woff = v1b_dsp_win.FastEl(DSP_OFF, bx, sc.y);
	    v1b_dsp_horiz.FastEl(DHZ_ORIG_OFF, bx, sc.y) = woff;
	    woff = max_off;
	    //	    v1b_dsp_win.FastEl(DSP_DIST, bx, sc.y) = max_wt;
	    // no need to update weight -- more useful as original vals
	    bflag = DSP_NONE;	// clear any other flags
	  }
	}
	// start over
	cur_st = -1;
	cur_len = 0;
	cur_mode = 0;
	backprop = false;
      }
    }
  }
}

void V1RegionSpec::V1BinocularFilter_DspOut_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int cur_mot_idx = v1s_circ_r.CircIdx_Last();

  float maxfv = MatMotEl2D(&v1s_out_r_max, sc.x, sc.y, cur_mot_idx);
  int flag = v1b_dsp_flags.FastEl(sc.x, sc.y);
  if(flag == DSP_NO_ACT) {
    for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
      v1b_dsp_wts.FastEl(didx, 0, sc.x, sc.y) = 0.0f;
      v1b_dsp_out.FastEl(didx, 0, sc.x, sc.y) = 0.0f;
    }
    return;
  }

  dsp_stats.tot_ambig_all += maxfv;

  if(flag != DSP_NONE) {	// some kind of ambiguous -- just bail on it
    float wt;
    if(v1b_dsp_specs.ambig_off)
      wt = 0.0f;
    else
      wt = v1b_specs.ambig_wt;
    float netval = wt * maxfv;
    for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
      v1b_dsp_wts.FastEl(didx, 0, sc.x, sc.y) = wt;
      v1b_dsp_out.FastEl(didx, 0, sc.x, sc.y) = netval;
    }
    dsp_stats.tot_ambig += maxfv;
    return;
  }

  dsp_stats.tot_sel_all += maxfv; // sel does not include ambiguous cases, so we don't double count those

  TwoDCoord fc;			// v1b feature coords -- destination
  TwoDCoord sfc;		// v1s feature coords -- source

  int min_off = (int)v1b_dsp_win.FastEl(DSP_OFF, sc.x, sc.y); // this is our final disparity offset

  float max_dval = 0.0f;
  float sum_dval = 0.0f;

  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    int dwd = v1b_widths.FastEl(didx);
    float dval = 0.0f;
    for(int twidx = 0; twidx < dwd; twidx++) {
      int off = v1b_stencils.FastEl(twidx, didx);
      if(off == min_off) {
	dval = v1b_weights.FastEl(twidx, didx);
	break;
      }
    }
    sum_dval += dval;
    if(dval > max_dval) max_dval = dval;
    v1b_dsp_out.FastEl(didx, 0, sc.x, sc.y) = maxfv * dval;
    v1b_dsp_wts.FastEl(didx, 0, sc.x, sc.y) = dval;
  }

  // do stats -- happens in the outer part
  if(sum_dval > 0.0f) {		// should definitely be true, right??
    float sel = max_dval / sum_dval;
    dsp_stats.tot_sel += maxfv * sel;
  }
}

void V1RegionSpec::V1BinocularFilter_WinAgg_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int& flag = v1b_dsp_flags.FastEl(sc.x, sc.y);
  if(flag == DSP_NO_ACT) return; // nothing to do here

  // important: this operates on v1b_dsp_wts output!!

  TwoDCoord bn;			// binoc neighbor

  int my_n_match = v1b_dsp_nmatch.FastEl(sc.x, sc.y);
  if(my_n_match == 0) return;	// shouldn't happen

  float* win_wt = new float[v1b_specs.tot_disps]; // sum of weights integrated over window
  for(int i=0; i<v1b_specs.tot_disps; i++) {
    win_wt[i] = 0.0f;
  }
  
  // accumulate weights by offsets across window 
  for(int wy = -v1b_dsp_specs.win_half_sz; wy <= v1b_dsp_specs.win_half_sz; wy++) {
    for(int wx = -v1b_dsp_specs.win_half_sz; wx <= v1b_dsp_specs.win_half_sz; wx++) {
      if(wy == 0 && wy == 0) continue; // skip self

      bn.y = sc.y + wy;
      bn.x = sc.x + wx;
      if(bn.WrapClip(wrap, v1s_img_geom)) {
	if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
      }

      int n_match = v1b_dsp_nmatch.FastEl(bn.x, bn.y);
      if(n_match == 0) continue;
      int nflag = v1b_dsp_flags.FastEl(bn.x, bn.y);
      if(nflag != DSP_NONE) continue; // avoid anything with a flag (can only be DSP_AMBIG_N)

      for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
	win_wt[didx] += v1b_dsp_wts.FastEl(didx, 0, bn.x, bn.y);
      }
    }
  }

  // find max's
  float max_wt = 0.0f;
  int max_didx = -1;
  float my_max_wt = 0.0f;
  int	my_max_didx = -1;
  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    if(win_wt[didx] > max_wt) {
      max_wt = win_wt[didx];
      max_didx = didx;
    }
    float mywt = v1b_dsp_wts.FastEl(didx, 0, sc.x, sc.y);
    if(mywt > my_max_wt) {
      my_max_wt = mywt;
      my_max_didx = didx;
    }
  }

  // logic is: if window around my max weight is below threshold, or I'm ambiguous, then
  // fill me in with surrounding window
  if((win_wt[my_max_didx] < v1b_dsp_specs.win_thr) || (flag != DSP_NONE)) {
    int cur_mot_idx = v1s_circ_r.CircIdx_Last();
    float maxfv = MatMotEl2D(&v1s_out_r_max, sc.x, sc.y, cur_mot_idx);
    for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
      float wt = 0.0f;
      if(didx == max_didx) wt = 1.0f;
      v1b_dsp_wts.FastEl(didx, 0, sc.x, sc.y) = wt;
      v1b_dsp_out.FastEl(didx, 0, sc.x, sc.y) = maxfv * wt;
    }
    int dwd = v1b_widths.FastEl(max_didx);
    int off = v1b_stencils.FastEl(dwd / 2, max_didx);

    v1b_dsp_win.FastEl(DSP_OFF, sc.x, sc.y) = off; // update offset to new val
    flag = DSP_NONE;				   // update flag
  }

  delete win_wt;
}

void V1RegionSpec::V1BinocularFilter_S_Out_thread(int v1b_idx, int thread_no) {
  TwoDCoord sc;			// simple coords
  sc.SetFmIndex(v1b_idx, v1s_img_geom.x);

  int cur_mot_idx = v1s_circ_r.CircIdx_Last();

  TwoDCoord fc;			// v1b feature coords -- destination
  TwoDCoord sfc;		// v1s feature coords -- source

  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    float dval = v1b_dsp_wts.FastEl(didx, 0, sc.x, sc.y);
    for(int sfi = 0; sfi < v1s_feat_geom.n; sfi++) { // simple feature index
      sfc.SetFmIndex(sfi, v1s_feat_geom.x);
      fc.y = sfc.y;

      float rv = MatMotEl(&v1s_out_r, sfc.x, sfc.y, sc.x, sc.y, cur_mot_idx);
      fc.x = sfc.x + didx * v1s_feat_geom.x;
      v1b_s_out.FastEl(fc.x, fc.y, sc.x, sc.y) = rv * dval;
    }
  }
}

bool V1RegionSpec::V1BinocularFilter_Complex_Pre() {
  int n_run_pre = v1c_pre_geom.Product();

  threads.n_threads = MIN(n_run_pre, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  // weight the v1b_pre outputs by disparity values -- uses cur_v1b_dsp -- must be same fmt as pre
  ThreadImgProcCall ip_call_pre((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_V1C_Pre_thread);
  threads.Run(&ip_call_pre, n_run_pre);

  if(v1c_filters & CF_ESLS) {
    if(v1c_filters & END_STOP) {
      ThreadImgProcCall ip_call_polinv((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_V1C_Pre_Polinv_thread);
      threads.Run(&ip_call_polinv, n_run_pre);
    }
  }
  return true;
}

bool V1RegionSpec::V1BinocularFilter_Complex_Pre_DspAng() {
  int n_run_pre = v1c_pre_geom.Product();

  threads.n_threads = MIN(n_run_pre, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;	// small chunks

  // weight the v1b_pre outputs by disparity values -- uses cur_v1b_dsp -- must be same fmt as pre
  ThreadImgProcCall ip_call_pre((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_V1C_Pre_DspAng_thread);
  threads.Run(&ip_call_pre, n_run_pre);

  if(v1c_filters & CF_ESLS) {
    if(v1c_filters & END_STOP) {
      ThreadImgProcCall ip_call_polinv((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_V1C_Pre_DspAng_Polinv_thread);
      threads.Run(&ip_call_polinv, n_run_pre);
    }
  }
  return true;
}

bool V1RegionSpec::V1BinocularFilter_Complex() {
  // iterate piecewise over each disparity level and recompute complex from that
  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    if(v1c_kwta.on) {
      cur_out = (float_Matrix*)v1b_v1c_out_raw.GetFrameSlice(didx);
      taBase::Ref(cur_out);
      cur_v1c_kwta_out = (float_Matrix*)v1b_v1c_out.GetFrameSlice(didx);
      taBase::Ref(cur_v1c_kwta_out);
    }
    else {
      cur_out = (float_Matrix*)v1b_v1c_out.GetFrameSlice(didx);
      taBase::Ref(cur_out);
    }
    cur_v1c_pre = (float_Matrix*)v1b_v1c_pre.GetFrameSlice(didx);
    taBase::Ref(cur_v1c_pre);
    cur_v1c_pre_polinv = (float_Matrix*)v1b_v1c_pre_polinv.GetFrameSlice(didx);
    taBase::Ref(cur_v1c_pre_polinv);

    bool rval = V1ComplexFilter_impl(); // process everything based on current pre inputs

    // then deref everything
    if(v1c_kwta.on) {
      taBase::unRefDone(cur_out);
      taBase::unRefDone(cur_v1c_kwta_out);
    }
    else {
      taBase::unRefDone(cur_out);
    }
    taBase::unRefDone(cur_v1c_pre);
    taBase::unRefDone(cur_v1c_pre_polinv);
  }
  
  return true;
}

void V1RegionSpec::V1BinocularFilter_DspOutPre_thread(int v1c_pre_idx, int thread_no) {
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);

  TwoDCoord scs = v1c_specs.pre_spacing * pc; // v1s coords start
  scs += v1c_specs.pre_border;
  scs += v1c_specs.border;
  scs -= v1c_specs.pre_half; // convert to lower-left starting position, not center

  TwoDCoord sc;
  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    float max_dval = 0.0f;
    for(int rfy = 0; rfy < v1c_specs.pre_rf; rfy++) {
      for(int rfx = 0; rfx < v1c_specs.pre_rf; rfx++) {
	sc.x = scs.x + rfx;
	sc.y = scs.y + rfy;
	if(sc.WrapClip(wrap, v1s_img_geom)) {
	  if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	}
	float dval = v1b_dsp_out.FastEl(didx, 0, sc.x, sc.y);
	max_dval = MAX(max_dval, dval);
      }
    }
    v1b_dsp_out_pre.FastEl(didx, 0, pc.x, pc.y) = max_dval;
  }
}

void V1RegionSpec::V1BinocularFilter_DspOutPreBord_thread(int v1c_pre_idx, int thread_no) {
  // only needed if the v1c_border is non-zero
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);

  TwoDCoord scs = pc; // v1s coords start
  scs += v1c_specs.border;

  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    float dval = v1b_dsp_out.FastEl(didx, 0, scs.x, scs.y);
    v1b_dsp_out_pre.FastEl(didx, 0, pc.x, pc.y) = dval;
  }
}


void V1RegionSpec::V1BinocularFilter_DspAngOutPre_thread(int v1c_pre_idx, int thread_no) {
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);

  TwoDCoord scs = v1c_specs.pre_spacing * pc; // v1s coords start
  scs += v1c_specs.pre_border;
  scs += v1c_specs.border;
  scs -= v1c_specs.pre_half; // convert to lower-left starting position, not center

  TwoDCoord sc;
  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    for(int ang=0; ang < v1s_specs.n_angles; ang++) {
      float max_dval = 0.0f;
      for(int rfy = 0; rfy < v1c_specs.pre_rf; rfy++) {
	for(int rfx = 0; rfx < v1c_specs.pre_rf; rfx++) {
	  sc.x = scs.x + rfx;
	  sc.y = scs.y + rfy;
	  if(sc.WrapClip(wrap, v1s_img_geom)) {
	    if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	  }
	  float dval = v1b_dsp_ang_out.FastEl(ang, didx, sc.x, sc.y);
	  max_dval = MAX(max_dval, dval);
	}
      }
      v1b_dsp_out_pre.FastEl(didx, 0, pc.x, pc.y) = max_dval;
    }
  }
}

void V1RegionSpec::V1BinocularFilter_DspAngOutPreBord_thread(int v1c_pre_idx, int thread_no) {
  // only needed if the v1c_border is non-zero
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);

  TwoDCoord scs = pc; // v1s coords start
  scs += v1c_specs.border;

  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    for(int ang=0; ang < v1s_specs.n_angles; ang++) {
      float dval = v1b_dsp_ang_out.FastEl(ang, didx, scs.x, scs.y);
      v1b_dsp_ang_out_pre.FastEl(ang, didx, pc.x, pc.y) = dval;
    }
  }
}


void V1RegionSpec::V1BinocularFilter_V1C_Pre_thread(int v1c_pre_idx, int thread_no) {
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);

  TwoDCoord fc;			// v1b feature coords -- destination

  // IMPORTANT: any code changed here must also be changed in subsequent Polinv function

  // todo: if this is useful, then precompute dmax as otherwise is computed 2x
  float dmax = 0.0f;
  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    float dval = cur_v1b_dsp->FastEl(didx, 0, pc.x, pc.y);
    if(dval > dmax) dmax = dval;
  }

  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    float dval = cur_v1b_dsp->FastEl(didx, 0, pc.x, pc.y);
    // penalize non-maximal values by extent to which non-maximal -- anything at max is a pass-through
    float wt = 1.0f - v1b_specs.dsp_gain * (dmax - dval);
    if(wt < 0.0f) wt = 0.0f;
    for(int sfi = 0; sfi < v1s_feat_geom.n; sfi++) { // simple feature index
      fc.SetFmIndex(sfi, v1s_feat_geom.x);
      float rv = v1c_pre.FastEl(fc.x, fc.y, pc.x, pc.y);
      v1b_v1c_pre.FastEl(fc.x, fc.y, pc.x, pc.y, didx) = rv * wt;
    }
  }
}

void V1RegionSpec::V1BinocularFilter_V1C_Pre_Polinv_thread(int v1c_pre_idx, int thread_no) {
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);

  TwoDCoord fc;			// v1b feature coords -- destination

  float dmax = 0.0f;
  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    float dval = cur_v1b_dsp->FastEl(didx, 0, pc.x, pc.y);
    if(dval > dmax) dmax = dval;
  }

  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    float dval = cur_v1b_dsp->FastEl(didx, 0, pc.x, pc.y);
    // penalize non-maximal values by extent to which non-maximal -- anything at max is a pass-through
    float wt = 1.0f - v1b_specs.dsp_gain * (dmax - dval);
    if(wt < 0.0f) wt = 0.0f;
    for(int sfi = 0; sfi < v1c_polinv_geom.n; sfi++) { // simple feature index
      fc.SetFmIndex(sfi, v1c_polinv_geom.x);
      float rv = v1c_pre_polinv.FastEl(fc.x, fc.y, pc.x, pc.y);
      v1b_v1c_pre_polinv.FastEl(fc.x, fc.y, pc.x, pc.y, didx) = rv * wt;
    }
  }
}


void V1RegionSpec::V1BinocularFilter_V1C_Pre_DspAng_thread(int v1c_pre_idx, int thread_no) {
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);

  TwoDCoord fc;			// v1b feature coords -- destination

  // IMPORTANT: any code changed here must also be changed in subsequent Polinv function

  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    for(int sfi = 0; sfi < v1s_feat_geom.n; sfi++) { // simple feature index
      fc.SetFmIndex(sfi, v1s_feat_geom.x);
      float dval = cur_v1b_dsp->FastEl(fc.x, didx, pc.x, pc.y);
      if(v1b_specs.dsp_v1c_thr > 0.0f) {
	if(dval > v1b_specs.dsp_v1c_thr) dval = 1.0f; // binarize
	else dval = 0.0f;
      }
      // just access by angle and disparity -- straight-up multiplies result (for now)
      float rv = v1c_pre.FastEl(fc.x, fc.y, pc.x, pc.y);
      v1b_v1c_pre.FastEl(fc.x, fc.y, pc.x, pc.y, didx) = rv * dval;
    }
  }
}

void V1RegionSpec::V1BinocularFilter_V1C_Pre_DspAng_Polinv_thread(int v1c_pre_idx, int thread_no) {
  TwoDCoord pc;			// pre coords
  pc.SetFmIndex(v1c_pre_idx, v1c_pre_geom.x);

  TwoDCoord fc;			// v1b feature coords -- destination

  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    for(int sfi = 0; sfi < v1c_polinv_geom.n; sfi++) { // simple feature index
      fc.SetFmIndex(sfi, v1c_polinv_geom.x);
      float dval = cur_v1b_dsp->FastEl(fc.x, didx, pc.x, pc.y);
      if(v1b_specs.dsp_v1c_thr > 0.0f) {
	if(dval > v1b_specs.dsp_v1c_thr) dval = 1.0f; // binarize
	else dval = 0.0f;
      }
      // just access by angle and disparity -- straight-up multiplies result (for now)
      float rv = v1c_pre_polinv.FastEl(fc.x, fc.y, pc.x, pc.y);
      v1b_v1c_pre_polinv.FastEl(fc.x, fc.y, pc.x, pc.y, didx) = rv * dval;
    }
  }
}

void V1RegionSpec::V1BinocularFilter_AvgSum() {
  float sum_val = 0.0f;
  float	norm_val = 0.0f;
  TwoDCoord bc;		// binocular coords
  TwoDCoord bfc;	// v1b feature coords
  TwoDCoord sfc;	// v1s feature coords
  for(bc.y = 0; bc.y < v1s_img_geom.y; bc.y++) {
    for(bc.x = 0; bc.x < v1s_img_geom.x; bc.x++) {
      int flag = v1b_dsp_flags.FastEl(bc.x, bc.y);
      if(flag != DSP_NONE) continue;
      float min_off = v1b_dsp_win.FastEl(DSP_OFF, bc.x, bc.y); // this is our final offset
      float norm_dsp = min_off / (float)v1b_specs.max_off;
      sum_val += norm_dsp;
      norm_val += 1.0f;		// just give equal weighting..
    }
  }
  if(norm_val > 0.0f)
    v1b_avgsum_out = sum_val / norm_val;
  else
    v1b_avgsum_out = 0.0f;
}


void V1RegionSpec::V1bDspAngCrossResMin(float extra_width, int max_extra) {
  // todo: make core routine threaded..
  RetinaProc* own = (RetinaProc*)GetOwner(&TA_RetinaProc);
  if(!own) return;
  for(int i=0; i<own->regions.size; i++) {
    V1RegionSpec* rsa = (V1RegionSpec*)own->regions.FastEl(i);
    for(int j=i+1; j<own->regions.size; j++) {
      V1RegionSpec* rsb = (V1RegionSpec*)own->regions.FastEl(j);
      V1RegionSpec* rs_sm;	// smaller
      V1RegionSpec* rs_lg;	// larger
      if(rsa->v1s_img_geom.x >= rsb->v1s_img_geom.x) {
	rs_sm = rsb; rs_lg = rsa;
      }
      else {
	rs_sm = rsa; rs_lg = rsb;
      }

      // testing
//       TwoDCoord sm_half = rs_sm->v1s_img_geom / 2;
//       TwoDCoord sm_0 = 0;

      TwoDCoord sm_to_lg;
      sm_to_lg = rs_lg->v1s_img_geom / rs_sm->v1s_img_geom;
      TwoDCoord extra;
      extra.x = (int)((float)sm_to_lg.x * extra_width + 0.5f);
      extra.y = (int)((float)sm_to_lg.y * extra_width + 0.5f);
      extra.x = MIN(max_extra, extra.x);
      extra.y = MIN(max_extra, extra.y);
      
      TwoDCoord tot_wd = sm_to_lg + extra;

      TwoDCoord lc;		// large coords
      TwoDCoord sc;		// small coords
      TwoDCoord xc;		// extra coords
      TwoDCoord alc;		// actual large coord
      for(sc.y = 0; sc.y < rs_sm->v1s_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < rs_sm->v1s_img_geom.x; sc.x++) {
	  lc = sc * sm_to_lg;
	  for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) {
	      float smval = rs_sm->v1b_dsp_ang_out.FastEl(ang, didx, sc.x, sc.y);
	      float lmax = 0.0f;
	      for(xc.y=-extra.y; xc.y<tot_wd.y; xc.y++) {
		for(xc.x=-extra.x; xc.x<tot_wd.x; xc.x++) {
		  alc = lc + xc;
		  if(alc.WrapClip(wrap, rs_lg->v1s_img_geom)) {
		    if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
		  }
		  float lval = rs_lg->v1b_dsp_ang_out.FastEl(ang, didx, alc.x, alc.y);
		  lmax = MAX(lmax, lval);
		}
	      }
	      float mn = MIN(smval, lmax); // quick product..
// 	      if(sc == sm_half || sc == sm_0)
// 		mn = 2.0f;	// test
	      rs_sm->v1b_dsp_ang_out_tmp.FastEl(ang, didx, sc.x, sc.y) = mn;
	      // apply this result ONLY to the "core" large guys, not the extras..
	      for(xc.y=0; xc.y<sm_to_lg.y; xc.y++) {
		for(xc.x=0; xc.x<sm_to_lg.x; xc.x++) {
		  alc = lc + xc;
		  float lval = rs_lg->v1b_dsp_ang_out.FastEl(ang, didx, alc.x, alc.y);
		  rs_lg->v1b_dsp_ang_out_tmp.FastEl(ang, didx, alc.x, alc.y) = MIN(mn, lval);
		  // *never* create new feature activation beyond what is present in large guys
		}
	      }
	    }
	  }
	}
      }
    }
  }

  for(int i=0; i<own->regions.size; i++) {
    V1RegionSpec* rsa = (V1RegionSpec*)own->regions.FastEl(i);
    rsa->v1b_dsp_ang_out.CopyFrom(&rsa->v1b_dsp_ang_out_tmp); // get tmp vals after all over
    // re-output v1b_dsp_ang_out
    if(!rsa->data_table || rsa->save_mode == NONE_SAVE) // bail now
      continue;
    rsa->V1BOutputToTable(rsa->data_table);
  }
}



bool V1RegionSpec::V1bDspInFmDataTable(DataTable* data_table, Variant col, int row,
				       float diff_thr, int integ_sz) {
  if(TestError(!data_table, "V1bDspInFmDataTable", "data table is null"))
    return false;

  float_MatrixPtr dacell; dacell = (float_Matrix*)data_table->GetValAsMatrix(col, row);
  if(!dacell) return false;	// err msg should have already happened?
  bool rval = V1bDspInFmMatrix(dacell, diff_thr, integ_sz); 
  return rval;
}

bool V1RegionSpec::V1bDspInFmMatrix(float_Matrix* dacell, float diff_thr, int integ_sz) {
  if(TestError(dacell->dims() != 4, "V1bDspInFmMatrix",
	       "dsp_in data column must be 4 dimensional --", dacell->name, "has:",
	       String(dacell->dims())))
    return false;
  if(TestError(dacell->dim(0) != v1b_specs.tot_disps, "V1bDspInFmMatrix",
	       "dsp_in data column first dimension must be = v1b_specs.tot_disps:", 
	       String(v1b_specs.tot_disps), dacell->name, "has:",
	       String(dacell->dim(0))))
    return false;
  if(TestError(dacell->dim(1) != 1, "V1bDspInFmMatrix",
	       "dsp_in data column second dimension must be = 1:", 
	       dacell->name, "has:", String(dacell->dim(1))))
    return false;

  TwoDCoord ingm;
  ingm.x = dacell->dim(2);
  ingm.y = dacell->dim(3);
  
  TwoDCoord prjrat;
  TwoDCoord recon;
  bool in_larger = false;
  prjrat = v1c_pre_geom / ingm;
  if(prjrat.x == 0 || prjrat.y == 0) {
    prjrat = ingm / v1c_pre_geom;
    in_larger = true;
    recon = ingm / prjrat;	// reconstruct
  }
  else {
    recon = ingm * prjrat;	// reconstruct
  }

  if(TestError(recon != v1c_pre_geom, "V1bDspInFmMatrix",
	       "dsp_in data column outer dimensions must be even multiple of v1c_pre_geom:", 
	       v1c_pre_geom.GetStr(), "instead,", dacell->name, "has dimension:",
	       ingm.GetStr()))
    return false;

  if(in_larger) {
    if(TestError(in_larger, "V1bDspInFmMatrix",
		 "dsp_in data column outer dimensions are larger than v1c_pre_geom -- not currently supported:", 
		 v1c_pre_geom.GetStr(), "instead,", dacell->name, "has dimension:",
		 ingm.GetStr()))
      return false;
  }
  else {
    TwoDCoord pc;
    TwoDCoord subc;
    TwoDCoord ic;
    TwoDCoord sic;
    for(pc.y = 0; pc.y < v1c_pre_geom.y; pc.y++) {
      for(pc.x = 0; pc.x < v1c_pre_geom.x; pc.x++) {
	ic = pc / prjrat;
	int max_dx = -1;
	float max_dx_val = 0.0f;
	for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
	  float dav = dacell->SafeEl(didx, 0 , ic.x, ic.y);
	  if(dav > max_dx_val) {
	    max_dx = didx;
	    max_dx_val = dav;
	  }
	}
	if(max_dx_val < 0.2f) {	// not very confident max
	  // integrate over subregion from input and take max as local value to apply
	  for(subc.y = -integ_sz; subc.y <= integ_sz; subc.y++) {
	    for(subc.x = -integ_sz; subc.x <= integ_sz; subc.x++) {
	      sic = ic + subc;
	      if(sic.WrapClip(wrap, ingm)) {
		if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	      }
	      for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
		float dav = dacell->SafeEl(didx, 0 , sic.x, sic.y);
		if(dav > max_dx_val) {
		  max_dx = didx;
		  max_dx_val = dav;
		}
	      }
	    }
	  }
	}
	for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
	  if(didx == max_dx)
	    v1b_dsp_in.FastEl(didx, 0, pc.x, pc.y) = max_dx_val;
	  else
	    v1b_dsp_in.FastEl(didx, 0, pc.x, pc.y) = 0.0f;
	}
      }
    }
  }

  if(diff_thr > 0.0f) {
    if(v1b_dsp_in_prv.geom.Equal(v1b_dsp_in.geom)) {
      float dst = taMath_float::vec_euclid_dist(&v1b_dsp_in, &v1b_dsp_in_prv, true, 0.0f);
      if(dst < diff_thr) return false;
    }
  }
  v1b_dsp_in_prv.CopyFrom(&v1b_dsp_in);

  return true;
}


void V1RegionSpec::UpdateV1cFromV1bDspIn() {
  if(TestError(!(v1b_filters & V1B_C_FM_IN), "UpdateV1cFromV1bDspIn",
	       "must have the v1b_filters set to include V1B_C_FM_IN -- this is required for prior formatting of data structures"))
    return;
  cur_v1b_dsp = &v1b_dsp_in;
  V1BinocularFilter_Complex_Pre();
  V1BinocularFilter_Complex();
  if(!data_table || save_mode == NONE_SAVE) // bail now
    return;
  V1BOutputToTable(data_table);
}


bool V1RegionSpec::V1bDspAngInFmDataTable(DataTable* data_table, Variant col, int row,
					  float diff_thr, int integ_sz, bool use_ang) {
  if(TestError(!data_table, "V1bDspAngInFmDataTable", "data table is null"))
    return false;

  float_MatrixPtr dacell; dacell = (float_Matrix*)data_table->GetValAsMatrix(col, row);
  if(!dacell) return false;	// err msg should have already happened?
  bool rval = V1bDspAngInFmMatrix(dacell, diff_thr, integ_sz, use_ang);
  return rval;
}

bool V1RegionSpec::V1bDspAngInFmMatrix(float_Matrix* dacell, float diff_thr, int integ_sz,
				       bool use_ang) {
  if(TestError(dacell->dims() != 4, "V1bDspAngInFmMatrix",
	       "dsp_in data column must be 4 dimensional --", dacell->name, "has:",
	       String(dacell->dims())))
    return false;
  if(TestError(dacell->dim(0) != v1s_specs.n_angles, "V1bDspAngInFmMatrix",
	       "dsp_in data column first dimension must be = v1s_specs.n_angles:", 
	       String(v1s_specs.n_angles), dacell->name, "has:",
	       String(dacell->dim(0))))
    return false;
  if(TestError(dacell->dim(1) != 1, "V1bDspAngInFmMatrix",
	       "dsp_in data column second dimension must be = 1:", 
	       dacell->name, "has:", String(dacell->dim(1))))
    return false;

  TwoDCoord ingm;
  ingm.x = dacell->dim(2);
  ingm.y = dacell->dim(3);
  
  TwoDCoord prjrat;
  TwoDCoord recon;
  bool in_larger = false;
  prjrat = v1c_pre_geom / ingm;
  if(prjrat.x == 0 || prjrat.y == 0) {
    prjrat = ingm / v1c_pre_geom;
    in_larger = true;
    recon = ingm / prjrat;	// reconstruct
  }
  else {
    recon = ingm * prjrat;	// reconstruct
  }

  if(TestError(recon != v1c_pre_geom, "V1bDspAngInFmMatrix",
	       "dsp_in data column outer dimensions must be even multiple of v1c_pre_geom:", 
	       v1c_pre_geom.GetStr(), "instead,", dacell->name, "has dimension:",
	       ingm.GetStr()))
    return false;

  if(in_larger) {
    if(TestError(in_larger, "V1bDspAngInFmMatrix",
		 "dsp_in data column outer dimensions are larger than v1c_pre_geom -- not currently supported:", 
		 v1c_pre_geom.GetStr(), "instead,", dacell->name, "has dimension:",
		 ingm.GetStr()))
      return false;
  }
  else {
    TwoDCoord pc;
    TwoDCoord subc;
    TwoDCoord ic;
    TwoDCoord sic;
    for(pc.y = 0; pc.y < v1c_pre_geom.y; pc.y++) {
      for(pc.x = 0; pc.x < v1c_pre_geom.x; pc.x++) {
	ic = pc / prjrat;
	int max_dx = -1;
	int max_ang = -1;
	float max_dx_val = 0.0f;
	bool set_max = false;
	for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
	  for(int ang = 0; ang < v1s_specs.n_angles; ang++) {
	    float dav = dacell->SafeEl(ang, didx, ic.x, ic.y);
	    if(dav > max_dx_val) {
	      max_dx = didx;
	      max_ang = ang;
	      max_dx_val = dav;
	    }
	  }
	}
	if(max_dx_val < 0.2f) {	// not very confident max
	  set_max = true;
	  // integrate over subregion from input and take max as local value to apply
	  for(subc.y = -integ_sz; subc.y <= integ_sz; subc.y++) {
	    for(subc.x = -integ_sz; subc.x <= integ_sz; subc.x++) {
	      sic = ic + subc;
	      if(sic.WrapClip(wrap, ingm)) {
		if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
	      }
	      for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
		for(int ang = 0; ang < v1s_specs.n_angles; ang++) {
		  float dav = dacell->SafeEl(ang, didx, sic.x, sic.y);
		  if(dav > max_dx_val) {
		    max_dx = didx;
		    max_ang = ang;
		    max_dx_val = dav;
		  }
		}
	      }
	    }
	  }
	}
	for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
	  for(int ang = 0; ang < v1s_specs.n_angles; ang++) {
	    if(use_ang) {
	      float dav = dacell->SafeEl(ang, didx , ic.x, ic.y);
	      v1b_dsp_ang_in.FastEl(ang, didx, pc.x, pc.y) = dav;
	    }
	    else {
	      if(didx == max_dx)
		v1b_dsp_ang_in.FastEl(ang, didx, pc.x, pc.y) = max_dx_val;
	      else
		v1b_dsp_ang_in.FastEl(ang, didx, pc.x, pc.y) = 0.0f;
	    }
	  }
	}
	if(set_max && use_ang) {
	  v1b_dsp_ang_in.FastEl(max_ang, max_dx, pc.x, pc.y) = max_dx_val; // set max
	}
      }
    }
  }

  if(diff_thr > 0.0f) {
    if(v1b_dsp_ang_in_prv.geom.Equal(v1b_dsp_ang_in.geom)) {
      float dst = taMath_float::vec_euclid_dist(&v1b_dsp_ang_in, &v1b_dsp_ang_in_prv, true, 0.0f);
      if(dst < diff_thr) return false;
    }
  }
  v1b_dsp_ang_in_prv.CopyFrom(&v1b_dsp_ang_in);

  return true;
}

void V1RegionSpec::UpdateV1cFromV1bDspAngIn() {
  if(TestError(!(v1b_filters & V1B_C_FM_IN), "UpdateV1cFromV1bDspAngIn",
	       "must have the v1b_filters set to include V1B_C_FM_IN -- this is required for prior formatting of data structures"))
    return;
  cur_v1b_dsp = &v1b_dsp_ang_in;
  V1BinocularFilter_Complex_Pre_DspAng(); // dsp ang version!
  V1BinocularFilter_Complex();
  if(!data_table || save_mode == NONE_SAVE) // bail now
    return;
  V1BOutputToTable(data_table);
}

void V1bDspInOutStats::Initialize() {
  InitStats();
}

bool V1RegionSpec::V1bDspInVsOutStats(float_Matrix* v1b_in, float_Matrix* v1b_out,
				      V1bDspInOutStats* stats, int disp) {
  if(TestError(!v1b_in, "V1bDspInVsOutStats", "v1b_in is NULL"))
    return false;
  if(TestError(!v1b_out, "V1bDspInVsOutStats", "v1b_out is NULL"))
    return false;
  if(TestError(!stats, "V1bDspInVsOutStats", "stats is NULL"))
    return false;
  if(TestError(!v1b_in->geom.Equal(v1b_out->geom), "V1bDspInVsOutStats",
	       "v1b_in geom is not same as v1b_out geom -- must be"))
    return false;
  stats->InitStats();
  
  TwoDCoord imgm;
  imgm.x = v1b_in->dim(2);
  imgm.y = v1b_in->dim(3);
  TwoDCoord dspgm;
  dspgm.x = v1b_in->dim(0);
  dspgm.y = v1b_in->dim(1);	// should always be 1

  TwoDCoord pc;
  TwoDCoord subc;
  TwoDCoord ic;
  TwoDCoord sic;
  for(pc.y = 0; pc.y < imgm.y; pc.y++) {
    for(pc.x = 0; pc.x < imgm.x; pc.x++) {
      int mxd_in = -1;
      int mxd_out = -1;
      float mxd_in_val = 0.0f;
      float mxd_out_val = 0.0f;
      for(int didx = 0; didx < dspgm.x; didx++) {
	float ival = v1b_in->FastEl(didx, 0, pc.x, pc.y);
	float oval = v1b_out->FastEl(didx, 0, pc.x, pc.y);
	if(ival > mxd_in_val) {
	  mxd_in_val = ival;
	  mxd_in = didx;
	}
	if(oval > mxd_out_val) {
	  mxd_out_val = oval;
	  mxd_out = didx;
	}
      }

      if(mxd_in_val < v1b_dsp_specs.opt_thr && mxd_out_val < v1b_dsp_specs.opt_thr)
	continue;

      if(disp < 0 || mxd_out == disp) {
	float ival = v1b_in->FastEl(mxd_out, 0, pc.x, pc.y);
	stats->p_in_g_out += ival; // increment
	stats->p_in_g_out_denom += MAX(ival, mxd_out_val);

	stats->p_in_g_out_max += (mxd_in == mxd_out) ? mxd_out_val : 0.0f;
	stats->p_in_g_out_max_denom += mxd_out_val;
      }

      if(disp < 0 || mxd_in == disp) {
	float oval = v1b_out->FastEl(mxd_in, 0, pc.x, pc.y);
	stats->p_out_g_in += oval; // increment
	stats->p_out_g_in_denom += MAX(oval, mxd_in_val);

	stats->p_out_g_in_max += (mxd_in == mxd_out) ? mxd_in_val : 0.0f;
	stats->p_out_g_in_max_denom += mxd_in_val;
      }

      if(disp >= 0 && mxd_in != disp) continue;
      
      int mxd2_in = -1;
      float mxd2_in_val = 0.0f;
      for(int didx = 0; didx < dspgm.x; didx++) {
	if(didx == mxd_in) continue; // skip max
	float ival = v1b_in->FastEl(didx, 0, pc.x, pc.y);
	if(ival > mxd2_in_val) {
	  mxd2_in_val = ival;
	  mxd2_in = didx;
	}
      }
      
      stats->snr += mxd_in_val - mxd2_in_val;
      stats->snr_denom += mxd_in_val;
    }
  }
  stats->ComputeStats();
  return true;
}


bool V1RegionSpec::V1bDspInVsOutDiffs(float_Matrix* v1b_in, float_Matrix* v1b_out,
				      DataTable* data_out) {
  if(TestError(!v1b_in, "V1bDspInVsOutDiffs", "v1b_in is NULL"))
    return false;
  if(TestError(!v1b_out, "V1bDspInVsOutDiffs", "v1b_out is NULL"))
    return false;
  if(TestError(!data_out, "V1bDspInVsOutDiffs", "data_out is NULL"))
    return false;
  if(TestError(!v1b_in->geom.Equal(v1b_out->geom), "V1bDspInVsOutDiffs",
	       "v1b_in geom is not same as v1b_out geom -- must be"))
    return false;

  TwoDCoord imgm;
  imgm.x = v1b_in->dim(2);
  imgm.y = v1b_in->dim(3);
  TwoDCoord dspgm;
  dspgm.x = v1b_in->dim(0);
  dspgm.y = v1b_in->dim(1);	// should always be 1

  data_out->StructUpdate(true);

  int idx;
  for(int didx=0; didx < dspgm.x; didx++) {
    data_out->FindMakeColName(name + "_v1b_in_out_in_d" + String(didx),
				      idx, DataTable::VT_FLOAT, 2, imgm.x, imgm.y);
    data_out->FindMakeColName(name + "_v1b_in_out_out_d" + String(didx),
				      idx, DataTable::VT_FLOAT, 2, imgm.x, imgm.y);
    data_out->FindMakeColName(name + "_v1b_in_out_diff_d" + String(didx),
				      idx, DataTable::VT_FLOAT, 2, imgm.x, imgm.y);
  }

  if(data_out->rows == 0)
    data_out->AddBlankRow();

  TwoDCoord pc;
  TwoDCoord subc;
  TwoDCoord ic;
  TwoDCoord sic;
  for(int didx = 0; didx < dspgm.x; didx++) {
    DataCol* col_in = data_out->FindMakeColName(name + "_v1b_in_out_in_d" + String(didx),
					     idx, DataTable::VT_FLOAT, 2, imgm.x, imgm.y);
    float_MatrixPtr dout_in; dout_in = (float_Matrix*)col_in->GetValAsMatrix(-1);
    DataCol* col_out = data_out->FindMakeColName(name + "_v1b_in_out_out_d" + String(didx),
					     idx, DataTable::VT_FLOAT, 2, imgm.x, imgm.y);
    float_MatrixPtr dout_out; dout_out = (float_Matrix*)col_out->GetValAsMatrix(-1);
    DataCol* col_dif = data_out->FindMakeColName(name + "_v1b_in_out_diff_d" + String(didx),
					     idx, DataTable::VT_FLOAT, 2, imgm.x, imgm.y);
    float_MatrixPtr dout_dif; dout_dif = (float_Matrix*)col_dif->GetValAsMatrix(-1);
    for(pc.y = 0; pc.y < imgm.y; pc.y++) {
      for(pc.x = 0; pc.x < imgm.x; pc.x++) {
	float ival = v1b_in->FastEl(didx, 0, pc.x, pc.y);
	float oval = v1b_out->FastEl(didx, 0, pc.x, pc.y);
	dout_in->FastEl(pc.x, pc.y) = ival;
	dout_out->FastEl(pc.x, pc.y) = oval;
	dout_dif->FastEl(pc.x, pc.y) = ival - oval; // straight diff -- sign is consistent with nature of the error..
      }
    }
  }
  data_out->StructUpdate(false);
  data_out->WriteClose();
  return true;
}

bool V1RegionSpec::V1bDspUpdtFmHint(float_Matrix* v1b_hint_in, int hint_dsp, float hint_thr,
				    bool mutex, int mutex_dsp) {
  if(TestError(!v1b_hint_in, "V1bDspUpdtFmHint", "v1b_hint_in is NULL"))
    return false;
  if(TestError(v1b_hint_in->geom.dim(0) != v1b_dsp_out.dim(2) ||
	       v1b_hint_in->geom.dim(1) != v1b_dsp_out.dim(3),
	       "V1bDspUpdtFmHint",
	       "v1b_in geom is not same as v1b_dsp_out geom -- must be"))
    return false;

  int cur_mot_idx = v1s_circ_r.CircIdx_Last();

  int hint_didx = v1b_specs.n_disps + hint_dsp;
  int mutex_didx = v1b_specs.n_disps + mutex_dsp;

  TwoDCoord sc;			// simple coords
  for(sc.y=0; sc.y < v1s_img_geom.y; sc.y++) {
    for(sc.x=0; sc.x < v1s_img_geom.x; sc.x++) {
      int flag = v1b_dsp_flags.FastEl(sc.x, sc.y);
      if(flag == DSP_NO_ACT) continue;
      float maxfv = MatMotEl2D(&v1s_out_r_max, sc.x, sc.y, cur_mot_idx);

      float hint = v1b_hint_in->FastEl(sc.x, sc.y);
      if(hint > hint_thr) {
	for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
	  float wt = 0.0f;
	  if(didx == hint_didx)
	    wt = 1.0f;
	  v1b_dsp_out.FastEl(didx, 0, sc.x, sc.y) = maxfv * wt;
	  v1b_dsp_wts.FastEl(didx, 0, sc.x, sc.y) = wt;
	}
      }
      else if(mutex) {
	float& htwt = v1b_dsp_wts.FastEl(hint_didx, 0, sc.x, sc.y);
	if(htwt > 0.0f) {
	  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
	    float wt = 0.0f;
	    if(didx == mutex_didx)
	      wt = 1.0f;
	    v1b_dsp_out.FastEl(didx, 0, sc.x, sc.y) = maxfv * wt;
	    v1b_dsp_wts.FastEl(didx, 0, sc.x, sc.y) = wt;
	  }
	}
      }
    }
  }

  bool rval = V1BinocularFilter_Optionals(); // finish up from here

  // re-write v1b stuff out to datatable
  if(v1b_save & SAVE_DATA && !(taMisc::gui_active && v1b_save & ONLY_GUI)
     && (region.ocularity == VisRegionParams::BINOCULAR || v1b_filters & REQ_V1B_C)) {
    V1BOutputToTable(data_table);
  }

  return rval;
}

/////////////////////////////////////////////////////////////////
//		Optional Filters

bool V1RegionSpec::V1OptionalFilter() {
//   int n_run = v1c_img_geom.Product();
//   int n_run_pre = v1c_pre_geom.Product();
//   int n_run_v1s = v1s_img_geom.Product();

  // energy is already computed as v1s_out_r_max so we don't need anything here yet..

//   if(opt_filters & ENERGY) {
//     ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_Energy_thread);
//     threads.Run(&ip_call, n_run_v1s);
//   }

  return true;
}

////////////////////////////////////////////////////////////////////
//	V1Region 	Data Table Output

bool V1RegionSpec::InitDataTable() {
  inherited::InitDataTable();
  if(!data_table) {
    return false;
  }

  int idx;
  DataCol* col;
  // SIMPLE
  if(v1s_save & SAVE_DATA) {
    if(v1s_filters & ALL_POLS) {
      if(v1s_save & SEP_MATRIX) {
	// break out into sub-structure: bw, color, motion
	data_table->FindMakeColName(name + "_v1s_bw_r", idx, DataTable::VT_FLOAT, 4,
				    v1s_feat_geom.x, 2, v1s_img_geom.x, v1s_img_geom.y);
	if(region.ocularity == VisRegionParams::BINOCULAR)
	  data_table->FindMakeColName(name + "_v1s_bw_l", idx, DataTable::VT_FLOAT, 4,
				      v1s_feat_geom.x, 2, v1s_img_geom.x, v1s_img_geom.y);
	if(region.color == VisRegionParams::COLOR) {
	  data_table->FindMakeColName(name + "_v1s_clr_r", idx, DataTable::VT_FLOAT, 4,
				      v1s_feat_geom.x, 4, v1s_img_geom.x, v1s_img_geom.y);
	  if(region.ocularity == VisRegionParams::BINOCULAR)
	    data_table->FindMakeColName(name + "_v1s_clr_l", idx, DataTable::VT_FLOAT, 4,
					v1s_feat_geom.x, 4, v1s_img_geom.x, v1s_img_geom.y);
	}
	if(motion_frames > 1) {
	  data_table->FindMakeColName(name + "_v1s_mot_r", idx, DataTable::VT_FLOAT, 4,
	      v1s_feat_geom.x, 4 * v1s_motion.n_speeds, v1s_img_geom.x, v1s_img_geom.y);
	  if(region.ocularity == VisRegionParams::BINOCULAR)
	    data_table->FindMakeColName(name + "_v1s_mot_l", idx, DataTable::VT_FLOAT, 4,
		v1s_feat_geom.x, 4 * v1s_motion.n_speeds, v1s_img_geom.x, v1s_img_geom.y);
	}
      }
      else {
	col = data_table->FindMakeColName(name + "_v1s_r", idx, DataTable::VT_FLOAT, 4,
		  v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
	if(region.ocularity == VisRegionParams::BINOCULAR)
	  col = data_table->FindMakeColName(name + "_v1s_l", idx, DataTable::VT_FLOAT, 4,
		    v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
      }
    }

    if(v1s_filters & MAX_POLS) {
      col = data_table->FindMakeColName(name + "_v1s_maxpols_r", idx, DataTable::VT_FLOAT, 4,
		v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
      if(region.ocularity == VisRegionParams::BINOCULAR)
	col = data_table->FindMakeColName(name + "_v1s_maxpols_l", idx, DataTable::VT_FLOAT, 4,
		v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
    }
  }

  // COMPLEX
  if(v1c_save & SAVE_DATA) {
    if(v1c_save & SEP_MATRIX) {
      // break out into sub-structure: end-stop/len-sum, other edges -- assumes both of each present
      if(v1c_filters & END_STOP) {
	data_table->FindMakeColName(name + "_v1c_es", idx, DataTable::VT_FLOAT, 4,
			    v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
      }
      if(v1c_filters & LEN_SUM) {
	data_table->FindMakeColName(name + "_v1c_ls", idx, DataTable::VT_FLOAT, 4,
			    v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
      }
      if(v1c_filters & V1S_MAX) {
	data_table->FindMakeColName(name + "_v1c_smax", idx, DataTable::VT_FLOAT, 4,
			    v1c_feat_geom.x, n_polclr, v1c_img_geom.x, v1c_img_geom.y);
      }
      if(v1c_filters & BLOB) {
	data_table->FindMakeColName(name + "_v1c_blob", idx, DataTable::VT_FLOAT, 4,
			    v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
      }
    }
    else {
      col = data_table->FindMakeColName(name + "_v1c", idx, DataTable::VT_FLOAT, 4,
		v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);
    }
    if(v1c_save & SAVE_DEBUG) {
      col = data_table->FindMakeColName(name + "_v1c_pre", idx, DataTable::VT_FLOAT, 4,
		v1s_feat_geom.x, v1s_feat_geom.y, v1c_pre_geom.x, v1c_pre_geom.y);
      col = data_table->FindMakeColName(name + "_v1c_esls_raw", idx, DataTable::VT_FLOAT, 4,
		v1s_feat_geom.x, 2, v1c_pre_geom.x, v1c_pre_geom.y);
    }
  }

  // VisRegionParams::BINOCULAR
  if(v1b_save & SAVE_DATA) {
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      if(v1b_filters & V1B_DSP) {
	if(v1b_specs.dsp_ang) {
	  col = data_table->FindMakeColName(name + "_v1b_dsp", idx, DataTable::VT_FLOAT, 4,
		    v1s_specs.n_angles, v1b_specs.tot_disps, v1s_img_geom.x, v1s_img_geom.y);
	}
	else {
	  col = data_table->FindMakeColName(name + "_v1b_dsp", idx, DataTable::VT_FLOAT, 4,
		    v1b_specs.tot_disps, 1, v1s_img_geom.x, v1s_img_geom.y);
	}
      }

      if(v1b_filters & V1B_S) {
	col = data_table->FindMakeColName(name + "_v1b_s", idx, DataTable::VT_FLOAT, 4,
		  v1b_s_feat_geom.x, v1b_s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
      }
    }

    if(v1b_filters & REQ_V1B_C) {
      for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
	col = data_table->FindMakeColName(name + "_v1b_v1c_d" + String(didx),
					  idx, DataTable::VT_FLOAT, 4,
		  v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);
      }
    }

    if(region.ocularity == VisRegionParams::BINOCULAR) {
      if(v1b_filters & V1B_AVGSUM) {
	col = data_table->FindMakeColName(name + "_v1b_avgsum", idx, DataTable::VT_FLOAT, 2,
					  1, 1);
      }
    }

    if(v1b_save & SAVE_DEBUG) {
      if(region.ocularity == VisRegionParams::BINOCULAR) {
	col = data_table->FindMakeColName(name + "_v1b_dsp_off",
			  idx, DataTable::VT_FLOAT, 2, v1s_img_geom.x, v1s_img_geom.y);
	col = data_table->FindMakeColName(name + "_v1b_dsp_offwt",
			  idx, DataTable::VT_FLOAT, 2, v1s_img_geom.x, v1s_img_geom.y);
	col = data_table->FindMakeColName(name + "_v1b_dsp_horiz_orig",
			  idx, DataTable::VT_FLOAT, 2, v1s_img_geom.x, v1s_img_geom.y);
// 	col = data_table->FindMakeColName(name + "_v1b_dsp_horiz_st",
// 			  idx, DataTable::VT_FLOAT, 2, v1s_img_geom.x, v1s_img_geom.y);
	col = data_table->FindMakeColName(name + "_v1b_dsp_wts", idx, DataTable::VT_FLOAT, 4,
			  v1b_specs.tot_disps, 1, v1s_img_geom.x, v1s_img_geom.y);
      }

      if(v1b_filters & V1B_C) {
	col = data_table->FindMakeColName(name + "_v1b_dsp_pre", idx, DataTable::VT_FLOAT, 4,
				  v1b_specs.tot_disps, 1, v1c_pre_geom.x, v1c_pre_geom.y);
      }

      if(v1b_filters & REQ_V1B_C) {
	for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
	  col = data_table->FindMakeColName(name + "_v1b_v1c_pre_d" + String(didx),
					    idx, DataTable::VT_FLOAT, 4,
			    v1s_feat_geom.x, v1s_feat_geom.y, v1c_pre_geom.x, v1c_pre_geom.y);
	}
      }

      if(v1b_filters & V1B_C_FM_IN) {
	if(v1b_specs.dsp_ang) {
	  col = data_table->FindMakeColName(name + "_v1b_dsp_ang_in", idx, DataTable::VT_FLOAT, 4,
	    v1s_specs.n_angles, v1b_specs.tot_disps, v1c_pre_geom.x, v1c_pre_geom.y);
	}
	else {
	  col = data_table->FindMakeColName(name + "_v1b_dsp_in", idx, DataTable::VT_FLOAT, 4,
					    v1b_specs.tot_disps, 1, v1c_pre_geom.x, v1c_pre_geom.y);
	}
      }
    }
  }

  if(opt_save & SAVE_DATA) {
    if(opt_filters & ENERGY) {
      col = data_table->FindMakeColName(name + "_energy", idx, DataTable::VT_FLOAT, 2,
					v1s_img_geom.x, v1s_img_geom.y);
    }
  }
  return true;
}

bool V1RegionSpec::V1SOutputToTable(DataTable* dtab) {
  DataCol* col;
  int idx;
  if(v1s_filters & ALL_POLS) {
    V1SOutputToTable_impl(dtab, &v1s_out_r, &v1s_circ_r, "_r");
    if(region.ocularity == VisRegionParams::BINOCULAR) 
      V1SOutputToTable_impl(dtab, &v1s_out_l, &v1s_circ_l, "_l");
  }

  if(v1s_filters & MAX_POLS) {
    col = data_table->FindMakeColName(name + "_v1s_maxpols_r", idx, DataTable::VT_FLOAT, 4,
				      v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(&v1s_maxpols_out_r);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      col = data_table->FindMakeColName(name + "_v1s_maxpols_l", idx, DataTable::VT_FLOAT, 4,
					v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
      float_MatrixPtr doutl; doutl = (float_Matrix*)col->GetValAsMatrix(-1);
      doutl->CopyFrom(&v1s_maxpols_out_l);
    }
  }

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
	  for(int polclr = 0; polclr < 2; polclr++) { // polclr features -- just first 2
	    fc.y = polclr;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float val = MatMotEl(out, fc.x, fc.y, sc.x, sc.y, mot_idx);
	      dout->FastEl(fc.x, fc.y, sc.x, sc.y) = val;
	    }
	  }
	}
      }
    }
    if(region.color == VisRegionParams::COLOR) {
      col = data_table->FindMakeColName(name + "_v1s_clr" + col_sufx, idx, DataTable::VT_FLOAT, 4,
			  v1s_feat_geom.x, 4, v1s_img_geom.x, v1s_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	  for(int polclr = 2; polclr < n_polclr; polclr++) { // polclr features -- just color
	    fc.y = polclr;
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


bool V1RegionSpec::V1COutputToTable(DataTable* dtab) {
  DataCol* col;
  TwoDCoord cc;		// complex coords
  TwoDCoord fc;		// v1c feature coords
  int idx;
  if(v1c_save & SEP_MATRIX) {
    // todo: write this
    // break out into sub-structure: end-stop/len-sum, other edges -- assumes both of each present
    if(v1c_filters & END_STOP) {
      col = data_table->FindMakeColName(name + "_v1c_es", idx, DataTable::VT_FLOAT, 4,
					v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
      
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
	for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
	  fc.y=v1c_feat_es_y;
	  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	    fc.x = ang;
	    float val = v1c_out.FastEl(fc.x, fc.y, cc.x, cc.y);
	    dout->FastEl(fc.x, 0, cc.x, cc.y) = val;
	  }
	}
      }
    }
    if(v1c_filters & LEN_SUM) {
      col = data_table->FindMakeColName(name + "_v1c_ls", idx, DataTable::VT_FLOAT, 4,
					v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
      
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
	for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
	  fc.y=v1c_feat_ls_y;
	  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	    fc.x = ang;
	    float val = v1c_out.FastEl(fc.x, fc.y, cc.x, cc.y);
	    dout->FastEl(fc.x, 0, cc.x, cc.y) = val;
	  }
	}
      }
    }
    if(v1c_filters & V1S_MAX) {
      col = data_table->FindMakeColName(name + "_v1c_smax", idx, DataTable::VT_FLOAT, 4,
		  v1c_feat_geom.x, n_polclr, v1c_img_geom.x, v1c_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
	for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
	  for(int polclr = 0; polclr < 2; polclr++) {
	    fc.y=v1c_feat_smax_y + polclr;
	    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	      fc.x = ang;
	      float val = v1c_out.FastEl(fc.x, fc.y, cc.x, cc.y);
	      dout->FastEl(fc.x, polclr, cc.x, cc.y) = val;
	    }
	  }
	}
      }
    }
    if(v1c_filters & BLOB) {
      col = data_table->FindMakeColName(name + "_v1c_blob", idx, DataTable::VT_FLOAT, 4,
			v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);

      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
	for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
	  fc.y=v1c_feat_blob_y;
	  for(int pol = 0; pol < 2; pol++) { // polarities
	    fc.x = pol;
	    float val = v1c_out.FastEl(fc.x, fc.y, cc.x, cc.y);
	    dout->FastEl(fc.x, 0, cc.x, cc.y) = val;
	  }
	  if(region.color == VisRegionParams::COLOR) {
	    for(int pol = 2; pol < 8; pol++) { // polarities
	      fc.x = pol % v1c_feat_geom.x;
	      int fy = pol / v1c_feat_geom.x;
	      float val = v1c_out.FastEl(fc.x, fc.y + fy, cc.x, cc.y);
	      dout->FastEl(fc.x, fy, cc.x, cc.y) = val;
	    }
	  }
	}
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_v1c", idx, DataTable::VT_FLOAT, 4,
		      v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(&v1c_out);
  }

  if(v1c_save & SAVE_DEBUG) {
    {
      col = data_table->FindMakeColName(name + "_v1c_pre", idx, DataTable::VT_FLOAT, 4,
					v1s_feat_geom.x, v1s_feat_geom.y, v1c_pre_geom.x, v1c_pre_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&v1c_pre);
    }
    {
      col = data_table->FindMakeColName(name + "_v1c_esls_raw", idx, DataTable::VT_FLOAT, 4,
					v1s_feat_geom.x, 2, v1c_pre_geom.x, v1c_pre_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&v1c_esls_raw);
    }
  }
  return true;
}

bool V1RegionSpec::V1BOutputToTable(DataTable* dtab) {
  DataCol* col;
  int idx;
  TwoDCoord sc;		// simple coords
  TwoDCoord fc;		// v1s feature coords

  if(region.ocularity == VisRegionParams::BINOCULAR) {
    if(v1b_filters & V1B_DSP) {
      if(v1b_specs.dsp_ang) {
	col = data_table->FindMakeColName(name + "_v1b_dsp", idx, DataTable::VT_FLOAT, 4,
					  v1s_specs.n_angles, v1b_specs.tot_disps, v1s_img_geom.x, v1s_img_geom.y);
	float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
	dout->CopyFrom(&v1b_dsp_ang_out);
      }
      else {
	col = data_table->FindMakeColName(name + "_v1b_dsp", idx, DataTable::VT_FLOAT, 4,
					  v1b_specs.tot_disps, 1, v1s_img_geom.x, v1s_img_geom.y);
	float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
	dout->CopyFrom(&v1b_dsp_out);
      }
    }

    if(v1b_filters & V1B_S) {
      col = data_table->FindMakeColName(name + "_v1b_s", idx, DataTable::VT_FLOAT, 4,
					v1b_s_feat_geom.x, v1b_s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&v1b_s_out);
    }
  }

  if(v1b_filters & REQ_V1B_C) {
    for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
      col = data_table->FindMakeColName(name + "_v1b_v1c_d" + String(didx),
					idx, DataTable::VT_FLOAT, 4,
		  v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      float_MatrixPtr vcout; vcout = (float_Matrix*)v1b_v1c_out.GetFrameSlice(didx);
      dout->CopyFrom(vcout);
    }
  }

  if(v1b_filters & V1B_AVGSUM) {
    col = data_table->FindMakeColName(name + "_v1b_avgsum", idx, DataTable::VT_FLOAT, 2,
				      1, 1);
    col->SetMatrixVal(v1b_avgsum_out, -1, 0, 0);
  }

  if(v1b_save & SAVE_DEBUG) {
    if(!v1b_specs.dsp_ang) {
      {
	col = data_table->FindMakeColName(name + "_v1b_dsp_off",
					  idx, DataTable::VT_FLOAT, 2, v1s_img_geom.x, v1s_img_geom.y);
	float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
	for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	  for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	    float val;
	    int flag = v1b_dsp_flags.FastEl(sc.x, sc.y);
	    if(flag == DSP_NONE) {
	      val = v1b_dsp_win.FastEl(DSP_OFF, sc.x, sc.y) / (float)v1b_specs.max_off;
	      // normalized disparity
	    }
	    else if(flag == DSP_NO_ACT) {
	      val = -1.1f;
	    }
	    else if(flag == DSP_AMBIG_N) {
	      val = -1.2f;
	    }
	    else if(flag == DSP_AMBIG_THR) {
	      val = -1.3f;
	    }
	    dout->FastEl(sc.x, sc.y) = val;
	  }
	}
      }
      {
	col = data_table->FindMakeColName(name + "_v1b_dsp_offwt",
					  idx, DataTable::VT_FLOAT, 2, v1s_img_geom.x, v1s_img_geom.y);
	float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
	for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	  for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	    float val;
	    int flag = v1b_dsp_flags.FastEl(sc.x, sc.y);
	    if(flag == DSP_NONE) {
	      val = v1b_dsp_win.FastEl(DSP_DIST, sc.x, sc.y); // this is our offset weight
	    }
	    else if(flag == DSP_NO_ACT) {
	      val = -0.7f;
	    }
	    else if(flag == DSP_AMBIG_N) {
	      val = -0.8f;
	    }
	    else if(flag == DSP_AMBIG_THR) {
	      val = -0.9f;
	    }
	    dout->FastEl(sc.x, sc.y) = val;
	  }
	}
      }

      {
	col = data_table->FindMakeColName(name + "_v1b_dsp_horiz_orig",
					  idx, DataTable::VT_FLOAT, 2, v1s_img_geom.x, v1s_img_geom.y);
	float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
	for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
	  for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
	    float val = (float)v1b_dsp_horiz.FastEl(DHZ_ORIG_OFF, sc.x, sc.y)
	      / (float)v1b_specs.max_off;
	    dout->FastEl(sc.x, sc.y) = val;
	  }
	}
      }

      //     {
      //       col = data_table->FindMakeColName(name + "_v1b_dsp_horiz_len",
      // 	idx, DataTable::VT_FLOAT, 2, v1s_img_geom.x, v1s_img_geom.y);
      //       float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      //       for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
      // 	for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
      // 	  float val;
      // 	  int flag = v1b_dsp_flags.FastEl(sc.x, sc.y);
      // 	  if(flag == DSP_NONE || flag == DSP_NO_ACT) {
      // 	    val = v1b_dsp_horiz.FastEl(DHZ_LEN, sc.x, sc.y);
      // 	  }
      // 	  else if(flag == DSP_AMBIG_N) {
      // 	    val = -1.2f;
      // 	  }
      // 	  else if(flag == DSP_AMBIG_THR) {
      // 	    val = -1.3f;
      // 	  }
      // 	  dout->FastEl(sc.x, sc.y) = val;
      // 	}
      //       }
      //     }

      //     {
      //       col = data_table->FindMakeColName(name + "_v1b_dsp_horiz_st",
      // 	idx, DataTable::VT_FLOAT, 2, v1s_img_geom.x, v1s_img_geom.y);
      //       float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      //       for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
      // 	for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
      // 	  float val;
      // 	  int flag = v1b_dsp_flags.FastEl(sc.x, sc.y);
      // 	  if(flag == DSP_NONE || flag == DSP_NO_ACT) {
      // 	    val = v1b_dsp_horiz.FastEl(DHZ_START, sc.x, sc.y);
      // 	  }
      // 	  else if(flag == DSP_AMBIG_N) {
      // 	    val = -1.2f;
      // 	  }
      // 	  else if(flag == DSP_AMBIG_THR) {
      // 	    val = -1.3f;
      // 	  }
      // 	  dout->FastEl(sc.x, sc.y) = val;
      // 	}
      //       }
      //     }

      {
	col = data_table->FindMakeColName(name + "_v1b_dsp_wts", idx, DataTable::VT_FLOAT, 4,
					  v1b_specs.tot_disps, 1, v1s_img_geom.x, v1s_img_geom.y);
	float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
	dout->CopyFrom(&v1b_dsp_wts);
      }
    }

    if(v1b_filters & V1B_C) {
      if(v1b_specs.dsp_ang) {
	col = data_table->FindMakeColName(name + "_v1b_dsp_ang_pre", idx, DataTable::VT_FLOAT, 4,
		  v1s_specs.n_angles, v1b_specs.tot_disps, v1c_pre_geom.x, v1c_pre_geom.y);
	float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
	dout->CopyFrom(&v1b_dsp_ang_out_pre);
      }
      else {
	col = data_table->FindMakeColName(name + "_v1b_dsp_pre", idx, DataTable::VT_FLOAT, 4,
				  v1b_specs.tot_disps, 1, v1c_pre_geom.x, v1c_pre_geom.y);
	float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
	dout->CopyFrom(&v1b_dsp_out_pre);
      }
    }

    if(v1b_filters & REQ_V1B_C) {
      for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
	col = data_table->FindMakeColName(name + "_v1b_v1c_pre_d" + String(didx),
			  idx, DataTable::VT_FLOAT, 4,
			  v1s_feat_geom.x, v1s_feat_geom.y, v1c_pre_geom.x, v1c_pre_geom.y);
	float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
	float_MatrixPtr vcout; vcout = (float_Matrix*)v1b_v1c_pre.GetFrameSlice(didx);
	dout->CopyFrom(vcout);
      }
    }

    if(v1b_filters & V1B_C_FM_IN) {
      if(v1b_specs.dsp_ang) {
	col = data_table->FindMakeColName(name + "_v1b_dsp_ang_in", idx, DataTable::VT_FLOAT, 4,
		  v1s_specs.n_angles, v1b_specs.tot_disps, v1c_pre_geom.x, v1c_pre_geom.y);
	float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
	dout->CopyFrom(&v1b_dsp_ang_in);
      }
      else {
	col = data_table->FindMakeColName(name + "_v1b_dsp_in", idx, DataTable::VT_FLOAT, 4,
			  v1b_specs.tot_disps, 1, v1c_pre_geom.x, v1c_pre_geom.y);
	float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
	dout->CopyFrom(&v1b_dsp_in);
      }
    }
  }
  return true;
}

bool V1RegionSpec::OptOutputToTable(DataTable* dtab) {
  DataCol* col;
  TwoDCoord cc;		// complex coords
  TwoDCoord fc;		// v1c feature coords
  int idx;

  if(opt_filters & ENERGY) {
    col = data_table->FindMakeColName(name + "_energy", idx, DataTable::VT_FLOAT, 2,
		      v1s_img_geom.x, v1s_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(&v1s_out_r_max);
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

//   if(TestWarning(region.ocularity == VisRegionParams::BINOCULAR, "InvertFilters",
// 	 "binocular inversion not currently supported -- only monocular will be rendered"));

  V1ComplexInvertFilter();

//   V1SimpleInvertFilter_Static(&dog_out_r, &dog_circ_r, &v1s_out_r_raw, &v1s_out_r, 
// 			      &v1s_circ_r);
//   if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
//     rval &= V1SimpleFilter_Static(&dog_out_l, &dog_circ_l, &v1s_out_l_raw, &v1s_out_l,
// 				  &v1s_circ_l);
//   }


//   DoGInvertFilter(&dog_out_r, &dog_circ_r, "_r");
//   if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
//     rval &= DoGInvertFilter(&dog_out_l, &dog_circ_l, "_l");
//   }

//   if(dog_save & SAVE_DATA && !(!taMisc::gui_active && dog_save & ONLY_GUI)) {
//     DoGOutputToTable(data_table);
//   }
  if(v1s_save & SAVE_DATA && !(!taMisc::gui_active && v1s_save & ONLY_GUI)) {
    V1SOutputToTable(data_table);
  }
  if(region.ocularity == VisRegionParams::BINOCULAR && v1b_save & SAVE_DATA && !(taMisc::gui_active && v1b_save & ONLY_GUI)) {
    V1BOutputToTable(data_table);
  }
  if(v1c_save & SAVE_DATA && !(!taMisc::gui_active && v1c_save & ONLY_GUI)) {
    V1COutputToTable(data_table);
  }
  if(opt_save & SAVE_DATA && !(!taMisc::gui_active && opt_save & ONLY_GUI)) {
    OptOutputToTable(data_table);
  }

  return rval;
}

bool V1RegionSpec::V1ComplexInvertFilter() {
  v1s_out_r.InitVals(0.0f);

  if(region.ocularity == VisRegionParams::BINOCULAR) {
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

//   int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

//   TwoDCoord cc;			// complex coords
//   TwoDCoord sc;			// simple coord
//   TwoDCoord sce;		// simple coord, ends
//   TwoDCoord scc;		// simple coord, center
//   TwoDCoord fc;			// v1c feature coords
//   TwoDCoord sfc_ctr;		// simple feature coords for the central point
//   TwoDCoord sfc_end;		// simple feature coords for the end point
  
//   for(cc.y=0; cc.y < v1c_img_geom.y; cc.y++) {
//     for(cc.x=0; cc.x < v1c_img_geom.x; cc.x++) {
//       TwoDCoord scs = v1c_specs.spat_spacing * cc; // v1s coords start
//       scs += v1c_specs.spat_border;
//       scs -= v1c_specs.spat_half; // convert to lower-left starting position, not center

//       int cfeat = 1;		// 
//       fc.y = v1c_feat_ls_y;

//       for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
// 	fc.x = ang;
// 	float v1c_act = v1c_out.FastEl(fc.x, fc.y, cc.x, cc.y);
// 	int v1sf = 0;		// just choose first feature -- on center -- monochrome only!
// 	if(v1c_filters & V1S_MAX) { // use v1smax to figure it out
// 	  float v1c_smax_on = v1c_out.FastEl(fc.x, v1c_feat_smax_y, cc.x, cc.y);
// 	  float v1c_smax_off = v1c_out.FastEl(fc.x, v1c_feat_smax_y+1, cc.x, cc.y);
// 	  if(v1c_smax_off > v1c_smax_on) v1sf = 1; // switch, else keep with 0
// 	}
// 	sfc_ctr.x = ang;
// 	sfc_ctr.y = v1sf;
// 	sfc_end.x = ang;
// 	if(cfeat == 0)	{	// end stop
// 	  sfc_end.y = 1 - v1sf;  // opposite polarity -- flip bit in odd/even within same set of 2
// 	}
// 	else {
// 	  sfc_end.y = v1sf;
// 	}

// 	int ys = v1c_specs.spat_half.y;
// 	sc.y = scs.y + ys;
// 	int xs = v1c_specs.spat_half.x;
// 	sc.x = scs.x + xs;
// 	scc = sc;	// center
// 	if(scc.WrapClip(wrap, v1s_img_geom)) {
// 	  if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
// 	}

// 	MatMotEl(&v1s_out_r, sfc_ctr.x, sfc_ctr.y, scc.x, scc.y, v1s_mot_idx) = v1c_act;

// 	for(int lpt=-v1c_specs.len_sum_len; lpt <= v1c_specs.len_sum_len; lpt++) {
// 	  if(lpt == 0) continue; // skip center
// 	  int lpdx = lpt + v1c_specs.len_sum_len;
// 	  int xp = v1c_ls_stencils.FastEl(X,lpdx,ang);
// 	  int yp = v1c_ls_stencils.FastEl(Y,lpdx,ang);
// 	  sce.x = sc.x + xp;
// 	  sce.y = sc.y + yp;

// 	  if(sce.WrapClip(wrap, v1s_img_geom)) {
// 	    if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
// 	  }

// 	  MatMotEl(&v1s_out_r, sfc_end.x, sfc_end.y, sce.x, sce.y, v1s_mot_idx) = v1c_act;
// 	}
//       }
//     }
//   }
  return true;
}

bool V1RegionSpec::V1ComplexInvertFilter_V1SMax_Monocular() {
//   int v1s_mot_idx = v1s_circ_r.CircIdx_Last();

//   TwoDCoord cc;			// complex coords
//   TwoDCoord sc;			// simple coord
//   TwoDCoord scc;		// simple coord, center
//   TwoDCoord sfc;		// v1s feature coords
//   TwoDCoord fc;			// v1c feature coords
//   for(cc.y=0; cc.y < v1c_img_geom.y; cc.y++) {
//     for(cc.x=0; cc.x < v1c_img_geom.x; cc.x++) {
//       TwoDCoord scs = v1c_specs.spat_spacing * cc; // v1s coords start
//       scs += v1c_specs.spat_border;
//       scs -= v1c_specs.spat_half; // convert to lower-left starting position, not center

//       for(int polclr = 0; polclr < 2; polclr++) { // only first monochrome on/off guys
// 	sfc.y = polclr;
// 	fc.y = v1c_feat_smax_y + polclr;
// 	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
// 	  sfc.x = ang;
// 	  fc.x = ang;

// 	  float v1c_act = cur_out->FastEl(fc.x, fc.y, cc.x, cc.y);

// 	  // just pick central point
// 	  int ys = v1c_specs.spat_half.y;
// 	  sc.y = scs.y + ys;
// 	  int xs = v1c_specs.spat_half.x;
// 	  sc.x = scs.x + xs;
// 	  scc = sc;	// center
// 	  if(scc.WrapClip(wrap, v1s_img_geom)) {
// 	    if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
// 	  }

// 	  MatMotEl(&v1s_out_r, sfc.x, sfc.y, scc.x, scc.y, v1s_mot_idx) = v1c_act;
// 	} // for ang
//       }  // for polclr
//     }
//   }
  return true;
}

bool V1RegionSpec::V1SimpleInvertFilter_Static(float_Matrix* dog, CircMatrix* dog_circ,
					 float_Matrix* out_raw, float_Matrix* out,
					 CircMatrix* circ) {
//   dog->InitVals(0.0f);

//   int dog_mot_idx = dog_circ->CircIdx_Last(); // always write to last position
//   int mot_idx = circ->CircIdx_Last(); // always write to last position

//   TwoDCoord dc;			// dog coord
//   TwoDCoord fc;			// v1s feature coords
//   TwoDCoord dfc;		// dog feature coords
//   TwoDCoord sc;			// simple coords
//   for(sc.y=0; sc.y < v1s_img_geom.y; sc.y++) {
//     for(sc.x=0; sc.x < v1s_img_geom.x; sc.x++) {
//       TwoDCoord dcs = v1s_specs.spacing * sc; // dog coords start
//       dcs += v1s_specs.border;
//       dcs -= v1s_specs.rf_half;	// convert to lower-left starting position, not center

//       for(int dogdx = 0; dogdx < dog_feat_geom.n; dogdx++) { // dog features
// 	dfc.SetFmIndex(dogdx, dog_feat_geom.x);
// 	fc.y = dogdx;
// 	for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
// 	  fc.x = ang;

// 	  float v1s_act = MatMotEl(out, fc.x, fc.y, sc.x, sc.y, mot_idx);
// 	  int lpos = v1s_specs.rf_half;		     // choose half-way point
// 	  for(int lpt = 0; lpt < v1s_specs.rf_size; lpt++) { // line points
// 	    int xp = v1s_stencils.FastEl(X,lpt,lpos,ang);
// 	    int yp = v1s_stencils.FastEl(Y,lpt,lpos,ang);
// 	    dc.x = dcs.x + xp;
// 	    dc.y = dcs.y + yp;

// 	    if(dc.WrapClip(wrap, dog_img_geom)) {
// 	      if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
// 	    }

// 	    MatMotEl(dog, dfc.x, dfc.y, dc.x, dc.y, dog_mot_idx) = v1s_act;
// 	  }
// 	}
//       }
//     }
//   }
  return true;
}

/////////////////////////////////////////////////////
//			Graphing

int  V1RegionSpec::AngleDeg(int ang_no) {
  int ang_inc = 180 / v1s_specs.n_angles;
  return ang_no * ang_inc;
}

void V1RegionSpec::GridGaborFilters(DataTable* graph_data) {
  v1s_specs.GridFilters(v1s_gabor_filters, graph_data);
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
  //  graph_data->SetUserData("WIDTH", .5f + (float)input_size.retina_size.x / (float)input_size.retina_size.y);

  TwoDCoord max_sz(v1s_specs.filter_size, v1s_specs.filter_size);
  max_sz.Max(v1c_specs.spat_rf);

  int bin_rf_max = 5;;
  if(region.ocularity == VisRegionParams::BINOCULAR) {
    TwoDCoord bin_max(v1b_specs.tot_offs, v1b_specs.tot_disps + 2);
    max_sz.Max(bin_max);
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

  if(region.ocularity == VisRegionParams::BINOCULAR) { // v1b
    { // basic stencils
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1b Binoc", -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      TwoDCoord ic;
      TwoDCoord dc;
      for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
	int didx = disp + v1b_specs.n_disps;
	int dwd = v1b_widths.FastEl(didx);
	ic.y = half_sz.y + disp;
	ic.x = half_sz.x;
	    
	if(ic.WrapClip(true, max_sz)) continue;

	for(int twidx = 0; twidx < dwd; twidx++) {
	  int off = v1b_stencils.FastEl(twidx, didx);
	  dc = ic;
	  dc.x += off;
	  if(dc.WrapClip(true, max_sz)) continue;
	  mat->FastEl(dc.x,dc.y) = v1b_weights.FastEl(twidx, didx);
	}
      }
      mat->FastEl(half_sz.x,half_sz.y-v1b_specs.n_disps-1) = -0.5f;
    }
  }

  if(motion_frames > 1) { // v1simple, motion
    for(int speed = 0; speed < v1s_motion.n_speeds; speed++) { // speed
      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
	for(int dir = 0; dir < 2; dir++) { // directions
	  float dirsign = (dir == 0) ? 1.0f : -1.0f; // direction sign for multiplying times slope values
	  graph_data->AddBlankRow();
	  nmda->SetValAsString("V1m sp:" + String(speed) + " ang:" + String(AngleDeg(ang)) +
			       " dir:" + String(dir == 0 ? "-" : "+"), -1);
	  float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
	  TwoDCoord ic;
	  for(int mot = 0; mot < motion_frames; mot++) { // time steps back in time
	    ic.y = half_sz.y;
	    ic.x = brd.x + mot * mot_rf_max;
	    
	    // offset along line to prevent overwrite
	    ic.x += taMath_float::rint(dirsign * v1s_ang_slopes.FastEl(X, LINE, ang));
	    ic.y += taMath_float::rint(dirsign * v1s_ang_slopes.FastEl(Y, LINE, ang));

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

  if(v1c_specs.pre_gp4) { // v1complex, pre_gp4
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1C PreGp 4x4 Ctrs: " + String(AngleDeg(ang)), -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      TwoDCoord ic;
      // first draw a bounding box
      for(int ys = -1; ys <= 4; ys++) {
	ic.x = brd.x-1; ic.y = brd.y + ys;
	if(ic.WrapClip(true, max_sz)) continue;
	mat->FastEl(ic.x,ic.y) = -.5;
	ic.x = brd.x+4; ic.y = brd.y + ys;
	if(ic.WrapClip(true, max_sz)) continue;
	mat->FastEl(ic.x,ic.y) = -.5;
      }
      for(int xs = -1; xs <= 4; xs++) {
	ic.x = brd.x+xs; ic.y = brd.y -1;
	if(ic.WrapClip(true, max_sz)) continue;
	mat->FastEl(ic.x,ic.y) = -.5;
	ic.x = brd.x+xs; ic.y = brd.y + 4;
	if(ic.WrapClip(true, max_sz)) continue;
	mat->FastEl(ic.x,ic.y) = -.5;
      }
      int nctrs = v1c_gp4_stencils.FastEl(2, 0, ang);	 // length stored here
      for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
	int xp = v1c_gp4_stencils.FastEl(X, ctrdx, ang);
	int yp = v1c_gp4_stencils.FastEl(Y, ctrdx, ang);
	ic.x = brd.x + xp;
	ic.y = brd.y + yp;

	if(ic.WrapClip(true, max_sz)) continue;
	mat->FastEl(ic.x,ic.y) = (ctrdx % 2 == 0) ? 1.0f: -1.0f;
      }
    }
  }
  { // v1complex, spatial
    if(v1c_specs.spat_rf.MaxVal() > 1) {
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
  }

  { // v1complex, es, ls
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1C End Stop Ang: " + String(AngleDeg(ang)), -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      TwoDCoord ic;
      for(int sidx=0; sidx < 2; sidx++) {
	for(int lpdx=0; lpdx < 2; lpdx++) {
	  int xp = v1c_es_stencils.FastEl(X,lpdx,sidx,ang);
	  int yp = v1c_es_stencils.FastEl(Y,lpdx,sidx,ang);
	  ic.x = brd.x + xp;
	  ic.y = brd.y + yp;

	  if(ic.WrapClip(true, max_sz)) continue;
	  mat->FastEl(ic.x,ic.y) = (sidx == 0) ? -1.0f: 1.0f;
	}
      }
    }
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1C Len Sum Ang: " + String(AngleDeg(ang)), -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      TwoDCoord ic;
      for(int lpt=-v1c_specs.len_sum_len; lpt <= v1c_specs.len_sum_len; lpt++) {
	//	if(lpt == 0) continue; // skip center
	int lpdx = lpt + v1c_specs.len_sum_len;
	int xp = v1c_ls_stencils.FastEl(X,lpdx,ang);
	int yp = v1c_ls_stencils.FastEl(Y,lpdx,ang);
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
					      input_size.retina_size.x, input_size.retina_size.y);
  graph_data->SetUserData("N_ROWS", 1);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);
  graph_data->SetUserData("BLOCK_SPACE", 20.0f);
  graph_data->SetUserData("WIDTH", .5f + (float)input_size.retina_size.x / (float)input_size.retina_size.y);

//   TwoDCoord ic;
//   int x,y;
//   { // first do dogs
//     graph_data->AddBlankRow();
//     nmda->SetValAsString("DoG", -1);
//     float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
//     for(y=input_size.border.y; y<= input_size.retina_size.y-input_size.border.y; y+= dog_specs.spacing.y) {
//       for(x=input_size.border.x; x<= input_size.retina_size.x-input_size.border.x; x+=dog_specs.spacing.x) {
// 	ic.y = y; ic.x = x;
// 	ic.WrapClip(true, input_size.retina_size);	mat->FastEl(ic.x,ic.y) = 1.0f;
//       }
//     }
//   }
  
//   { // then v1 simple
//     graph_data->AddBlankRow();
//     nmda->SetValAsString("V1_Simple", -1);
//     float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
//     TwoDCoord brd(input_size.border.x+v1s_specs.border*dog_specs.spacing.x,
// 		  input_size.border.y+v1s_specs.border*dog_specs.spacing.y);
//     TwoDCoord spc(dog_specs.spacing.x * v1s_specs.spacing, dog_specs.spacing.y * v1s_specs.spacing);
//     // first render borders of RF's, every other
//     for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= 2*spc.y) {
//       for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+= 2*spc.x) {
// 	ic.y = y; ic.x = x;
// 	ic -= v1s_specs.rf_half*dog_specs.spacing; // lower left
// 	TwoDCoord ec;
// 	int ex,ey;
// 	for(ey=0; ey < v1s_specs.rf_size; ey++) {
// 	  ec.y = ic.y + ey*dog_specs.spacing.y;  ec.x = ic.x;
// 	  ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
// 	  ec.y = ic.y + ey*dog_specs.spacing.y;  ec.x = ic.x + dog_specs.spacing.x * (v1s_specs.rf_size-1);
// 	  ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
// 	}
// 	for(ex=0; ex < v1s_specs.rf_size; ex++) {
// 	  ec.y = ic.y;	  ec.x = ic.x + ex*dog_specs.spacing.x;
// 	  ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
// 	  ec.y = ic.y + dog_specs.spacing.y * (v1s_specs.rf_size-1); ec.x = ic.x + ex*dog_specs.spacing.x;
// 	  ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
// 	}
//       }
//     }
//     // then centers
//     for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= spc.y) {
//       for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+=spc.x) {
// 	ic.y = y; ic.x = x;
// 	ic.WrapClip(true, input_size.retina_size);	mat->FastEl(ic.x,ic.y) = 1.0f;
//       }
//     }
//   }

  { // then v1 complex
    TwoDCoord ic;
    int x,y;
    graph_data->AddBlankRow();
    nmda->SetValAsString("V1_Complex", -1);
    float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
    TwoDCoord brd(input_size.border.x+v1s_specs.spacing*v1c_specs.net_border.x,
		  input_size.border.y+v1s_specs.spacing*v1c_specs.net_border.y);
    TwoDCoord spc(v1s_specs.spacing * v1c_specs.net_spacing.x,
		  v1s_specs.spacing * v1c_specs.net_spacing.y);
    TwoDCoord spcb(v1s_specs.spacing, v1s_specs.spacing);
    // first render borders of RF's, every other
    for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= 2*spc.y) {
      for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+= 2*spc.x) {
	ic.y = y; ic.x = x;
	ic -= v1c_specs.net_half*spcb; // lower left
	TwoDCoord ec;
	int ex,ey;
	for(ey=0; ey < v1c_specs.spat_rf.y; ey++) {
	  ec.y = ic.y + ey*spcb.y;  ec.x = ic.x;
	  ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	  ec.y = ic.y + ey*spcb.y;  ec.x = ic.x + spcb.x * (v1c_specs.spat_rf.x-1);
	  ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	}
	for(ex=0; ex < v1c_specs.spat_rf.x; ex++) {
	  ec.y = ic.y;	  ec.x = ic.x + ex*spcb.x;
	  ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	  ec.y = ic.y + spcb.y * (v1c_specs.spat_rf.y-1); ec.x = ic.x + ex*spcb.x;
	  ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
	}
      }
    }
    for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= spc.y) {
      for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+=spc.x) {
	ic.y = y; ic.x = x;
	ic.WrapClip(true, input_size.retina_size);	mat->FastEl(ic.x,ic.y) = 1.0f;
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
    VisRegionSpecBase* reg = regions[ri];
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
  VisRegionSpecBase* reg = regions[0]; // take params from first

  float ctr_x = .5f + .5 * move_x;
  float ctr_y = .5f + .5 * move_y;

  taImageProc::SampleImageWindow_float(xform_image, eye_image, reg->input_size.retina_size.x,
				       reg->input_size.retina_size.y, 
				       ctr_x, ctr_y, rotate, scale, edge_mode);
  if(edge_mode == taImageProc::BORDER) taImageProc::RenderBorder_float(xform_image);
  if(edge_mode == taImageProc::BORDER && fade_width > 0) {
    taImageProc::FadeEdgesToBorder_float(xform_image, fade_width); 
  }
  return true;
}

bool RetinaProc::LookAtImageData_impl(float_Matrix& eye_image,
				      float_Matrix& xform_image,
				      VisRegionParams::Region region,
				      float box_ll_x, float box_ll_y,
				      float box_ur_x, float box_ur_y,
				      float move_x, float move_y,
				      float scale, float rotate) 
{
  // todo: add error messages on all these..
  if(regions.size == 0) return false;
  VisRegionSpecBase* trg_reg = regions.FindRetinalRegion(region);
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
  float sc_x = (float)trg_reg->input_size.input_size.x / pix_x;
  float sc_y = (float)trg_reg->input_size.input_size.y / pix_y;

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
    VisRegionSpecBase* reg = regions[ri];
    reg->FilterImage(&xform_image_r, &xform_image_l);
  }
  return true;
}

bool RetinaProc::InvertFilter() {
  if(regions.size == 0) return false;
  for(int ri=0; ri < regions.size; ri++) {
    VisRegionSpecBase* reg = regions[ri];
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
				 VisRegionParams::Region region,
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
				      VisRegionParams::Color color) {
  if(color == VisRegionParams::COLOR) {
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
  VisRegionSpecBase* reg = regions[0]; // take params from first
  if(right_eye_image) {
    ConvertImageToMatrix(raw_image_r, right_eye_image, reg->region.color);
    TransformImageData_impl(raw_image_r, xform_image_r, move_x, move_y, scale, rotate);
  }
  if(left_eye_image) {
    ConvertImageToMatrix(raw_image_l, left_eye_image, reg->region.color);
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
			     VisRegionParams::Region region,
			     float box_ll_x, float box_ll_y,
			     float box_ur_x, float box_ur_y,
			     float move_x, float move_y,
			     float scale, float rotate)
{
  if(regions.size == 0) return false;
  VisRegionSpecBase* reg = regions[0]; // take params from first
  if(right_eye_image) {
    ConvertImageToMatrix(raw_image_r, right_eye_image, reg->region.color);
    LookAtImageData_impl(raw_image_r, xform_image_r, region, box_ll_x, box_ll_y,
			 box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  if(left_eye_image) {
    ConvertImageToMatrix(raw_image_l, left_eye_image, reg->region.color);
    LookAtImageData_impl(raw_image_l, xform_image_l, region, box_ll_x, box_ll_y,
			 box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  return true;
}

bool RetinaProc::LookAtImageName(const String& right_eye_img_fname,
				 const String& left_eye_img_fname,
				 VisRegionParams::Region region,
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

bool RetinaProc::AttendRegion(DataTable* dt, VisRegionParams::Region region) {
//   VisRegionSpecBase* fov_spec = regions.FindRetinalRegion(region);
//   if(!fov_spec) return false;

//   float fov_x_pct = (float)fov_spec->spacing.input_size.x / (float)input_size.retina_size.x;
//   float fov_y_pct = (float)fov_spec->spacing.input_size.y / (float)input_size.retina_size.y;
//   float fov_pct = taMath_float::max(fov_x_pct, fov_y_pct);

//   int idx;
//   for(int i=0;i<regions.size;i++) {
//     VisRegionSpecBase* sp = regions[i];
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

void DoGRetinaProc::Initialize() {
  regions.SetDefaultElType(&TA_DoGRegionSpec);
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

