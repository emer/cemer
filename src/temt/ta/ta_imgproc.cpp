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
//      taImage

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

bool taImage::ImageToMatrix_rgba(float_Matrix& rgba_data) {
  if(q_img.isNull()) {
    return false;
  }
  int ht = q_img.height();
  int wd = q_img.width();

  rgba_data.SetGeom(3, wd, ht, 4); // r,g,b,a

  for(int y=0; y<ht; y++) {
    for(int x=0; x< wd; x++) {
      QRgb pix = q_img.pixel(x, y);
      float rval = qRed(pix) / 255.0f;
      float gval = qGreen(pix) / 255.0f;
      float bval = qBlue(pix) / 255.0f;
      float aval = qAlpha(pix) / 255.0f;
      rgba_data.Set(rval, x, ht-1 -y, 0);
      rgba_data.Set(gval, x, ht-1 -y, 1);
      rgba_data.Set(bval, x, ht-1 -y, 2);
      rgba_data.Set(aval, x, ht-1 -y, 3);
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
      int gval = (int)(img_data.FastEl(x, y) * 255.0f);
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
      int rval = (int)(rgb_data.FastEl(x, y, 0) * 255.0f);
      int gval = (int)(rgb_data.FastEl(x, y, 1) * 255.0f);
      int bval = (int)(rgb_data.FastEl(x, y, 2) * 255.0f);
      QRgb pix = qRgb(rval, gval, bval);
      q_img.setPixel(x, ht-1-y, pix);
    }
  }
  return true;
}

bool taImage::ImageToDataCell(DataTable* dt, const Variant& col, int row) {
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
  else {                        // must be > 2
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

bool taImage::ImageFromDataCell(DataTable* dt, const Variant& col, int row) {
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
          int gval = (int)(mat->FastElAsFloat(x, y) * 255.0f);
          QRgb pix = qRgb(gval, gval, gval);
          q_img.setPixel(x, ht-1-y, pix);
        }
        else {
          int gval = (int)(mat->FastElAsFloat(x, y));
          QRgb pix = qRgb(gval, gval, gval);
          q_img.setPixel(x, ht-1-y, pix);
        }
      }
    }
  }
  else {                        // must be > 2
    for(int y=0; y<ht; y++) {
      for(int x=0; x< wd; x++) {
        QRgb pix = q_img.pixel(x, y);
        if(isfloat) {
          int rval = (int)(mat->FastElAsFloat(x, y, 0) * 255.0f);
          int gval = (int)(mat->FastElAsFloat(x, y, 1) * 255.0f);
          int bval = (int)(mat->FastElAsFloat(x, y, 2) * 255.0f);
          QRgb pix = qRgb(rval, gval, bval);
          q_img.setPixel(x, ht-1-y, pix);
        }
        else {                  // assume int
          int rval = (int)mat->FastElAsFloat(x, y, 0);
          int gval = (int)mat->FastElAsFloat(x, y, 1);
          int bval = (int)mat->FastElAsFloat(x, y, 2);
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
//      taCanvas

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
//      Threading

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
    if(mx == nib_stop) break;           // we're the last guy
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
  InitAll();                    // make sure

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

    tasks[0]->run();            // run our own set..

    // finally, always need to sync at end to ensure that everyone is done!
    SyncThreads();
  }
}

///////////////////////////////////////////
//      DoG Filter

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
//      Gabor Filters!

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
  case CTR_X:   return ctr_x;
  case CTR_Y:   return ctr_y;
  case ANGLE:   return angle;
  case PHASE:   return phase;
  case FREQ:    return freq;
  case LENGTH:  return length;
  case WIDTH:   return width;
  case AMP:     return amp;
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

String& GaborFilter::PrintParams(String& strm) {
  strm << "ctr: " << ctr_x << ", " << ctr_y << ", angle: " << angle
       << ", phase: " << phase << ", freq: " << freq
       << ", length: " << length << ", width: " << width
       << ", amp: " << amp
       << "\n";
  return strm;
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

  spat_angle = 0;               // angle of sine wave in 2-d space (in radians)
  time_angle = 1;               // angle of sine wave in 2-d time (in radians)

  phase = 0;            // phase of sine wave wrt the center of the gaussian (radians)
  freq = 1;             // frequency of the sine wave
  freq_t = 1;           // frequency of the sine wave

  width = 4;            // width of the gaussian in the wave direction
  length = 8;           // width of the gaussian in the wave direction
  width_t = 6;          // width of the gaussian in the wave direction

  amp = 1;              // amplitude (maximum value)
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
  case CTR_X:   return ctr_x;
  case CTR_Y:   return ctr_y;
  case CTR_T:   return ctr_t;
  case TIME_ANGLE:      return spat_angle;
  case SPAT_ANGLE:      return time_angle;
  case PHASE:   return phase;
  case FREQ:    return freq;
  case FREQ_T:  return freq_t;
  case LENGTH:  return length;
  case WIDTH:   return width;
  case WIDTH_T: return width_t;
  case AMP:     return amp;
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

String& MotionGaborFilter::PrintParams(String& strm) {
  strm << "ctr: " << ctr_x << ", " << ctr_y << ", angle: " << spat_angle
       << ", phase: " << phase << ", freq: " << freq
       << ", length: " << length << ", width: " << width
       << ", amp: " << amp
       << "\n";
  return strm;
}


//////////////////////////////////////////////////////////
//      Gabor Fitter!


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
//   float      ctr_inc = (x_size - 2.0f * ctr_brd) / 3.0f;
//   float ang_inc = PI / 3.0f;
//   float phs_inc = PI;
//   float      frq_min = PI / x_size;
//   float frq_max = PI;
//   float frq_inc = (frq_max - frq_min) / 2.0f;
//   float sz_min = 1.0f;
//   float sz_max = .5f;
//   float sz_inc = ((x_size * sz_max) - sz_min) / 2.0f;

// //   float tot_comp = 3.0f * 3.0f * 4.0f * 2.0f * 2.0f * 2.0f * 2.0f;

//   float min_d = FLT_MAX;
//   GaborFitter min_params;
//   String my_nm = name;

//   for(ctr_x = ctr_brd; ctr_x <= x_size - ctr_brd; ctr_x += ctr_inc) {
//     for(ctr_y = ctr_brd; ctr_y <= y_size - ctr_brd; ctr_y += ctr_inc) {
//       for(angle = 0.0f; angle <= PI; angle += ang_inc) {
//      for(phase = 0.0f; phase <= 2.0f * PI; phase += phs_inc) {
//        for(freq = frq_min; freq <= frq_max; freq += frq_inc) {
//          for(length = sz_min; length <= y_size * sz_max; length += sz_inc) {
//            for(width = sz_min; width <= x_size * sz_max; width += sz_inc) {
//              float dist = SquaredDist(data_vals);
//              if(dist < min_d) {
//                min_d = dist;
//                min_params = *this;
//              }
//            }
//          }
//        }
//      }
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
// //     String strm;
// //     strm << "After 1st Pass\t min_d: " << min_d << "\t";
// //     PrintParams(strm);
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
// //   min_d = dist;
// //   min_amp = amp;
// //       }
// //     }
// //     amp = min_amp;

//     float cur_ctr_x = ctr_x;  float cur_ctr_y = ctr_y;
//     float min_ctr_x = ctr_x;  float min_ctr_y = ctr_y;
//     for(ctr_x = cur_ctr_x-ctr_rng; ctr_x <= cur_ctr_x + ctr_rng; ctr_x += ctr_inc) {
//       if((ctr_x < 1) || (ctr_x >= x_size-1)) continue;
//       for(ctr_y = cur_ctr_y-ctr_rng; ctr_y <= cur_ctr_y + ctr_rng; ctr_y += ctr_inc) {
//      if((ctr_y < 1) || (ctr_y >= y_size-1)) continue;
//      float dist = SquaredDist(data_vals);
//      if(dist < min_d) {
//        min_d = dist;
//        min_ctr_x = ctr_x; min_ctr_y = ctr_y;
//      }
//       }
//     }
//     ctr_x = min_ctr_x; ctr_y = min_ctr_y;

//     float cur_ang = angle;
//     float min_ang = angle;
//     for(angle = cur_ang - ang_rng; angle <= cur_ang + ang_rng; angle += ang_inc) {
//       float dist = SquaredDist(data_vals);
//       if(dist < min_d) {
//      min_d = dist;
//      min_ang = angle;
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
//      min_d = dist;
//      min_frq = freq;
//       }
//     }
//     freq = min_frq;

//     float cur_wid = width;
//     float min_wid = width;
//     for(width = cur_wid - sz_rng; width <= cur_wid + sz_rng; width += sz_inc) {
//       if(width < .75) continue;
//       float dist = SquaredDist(data_vals);
//       if(dist < min_d) {
//      min_d = dist;
//      min_wid = width;
//       }
//     }
//     width = min_wid;

//     float cur_phs = phase; cur_ctr_x = ctr_x;  cur_ctr_y = ctr_y; cur_wid = width;
//     float min_phs = phase; min_ctr_x = ctr_x;  min_ctr_y = ctr_y; min_wid = width;
//     for(phase = cur_phs - phs_rng; phase <= cur_phs + phs_rng; phase += phs_inc) {
//       for(ctr_x = cur_ctr_x-sub_ctr_rng; ctr_x <= cur_ctr_x + sub_ctr_rng; ctr_x += sub_ctr_inc) {
//      if((ctr_x < 1) || (ctr_x >= x_size-1)) continue;
//      for(ctr_y = cur_ctr_y-sub_ctr_rng; ctr_y <= cur_ctr_y + sub_ctr_rng; ctr_y += sub_ctr_inc) {
//        if((ctr_y < 1) || (ctr_y >= y_size-1)) continue;
//        for(width = cur_wid - sub_wd_rng; width <= cur_wid + sub_wd_rng; width += sub_wd_inc) {
//          if(width < .75) continue;
//          float dist = SquaredDist(data_vals);
//          if(dist < min_d) {
//            min_d = dist;
//            min_phs = phase; min_ctr_x = ctr_x; min_ctr_y = ctr_y; min_wid = width;
//          }
//        }
//      }
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
//      min_d = dist;
//      min_len = length;
//       }
//     }
//     length = min_len;

// //     strm << "Itr: " << itr << "\t min_d: " << min_d << "\t";
// //     PrintParams(strm);
//   }

// //   strm << "Final fit\t min_d: " << min_d << "\t";
// //   PrintParams(strm);
//   fit_dist = min_d;
//   return min_d;
// }

// float GaborFitter::TestFit() {
//   ctr_x = 2 + Random::IntZeroN(x_size - 4);
//   ctr_y = 2 + Random::IntZeroN(y_size - 4);
//   angle = PI * Random::ZeroOne();
//   phase = 2.0f * PI * Random::ZeroOne();
//   float      frq_min = PI / x_size;
//   float frq_max = PI;
//   float frq_inc = (frq_max - frq_min);
//   freq = frq_min + frq_inc * Random::ZeroOne();
//   length = .75 + ((.25 * y_size) - .5) * Random::ZeroOne();
//   width = .75 + ((.25 * x_size) - .5) * Random::ZeroOne();
//   amp = 1.0;
//   String strm;
//   strm << "\nBefore:\t";
//   PrintParams(strm);
//   GridFilter(NULL);
//   taivMisc::RunIVPending();
//   float_Matrix data(false);
//   RenderFilter(data);
//   float min_d = FitData(data, false);
//   strm << "Min Dist: " << min_d << "\n";
//   strm << "After:\t";
//   PrintParams(strm);
//   GridFilter(NULL);
//   return min_d;
// }

//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//              taImageProc

void taImageProc::Initialize() {
}

void taImageProc::Destroy() {
}

bool taImageProc::GetBorderColor_float(float_Matrix& img_data, float& r, float& g, float& b) {
  if(img_data.dims() == 3) {    // an rgb guy
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
  if(img_data.dims() == 3) {    // an rgb guy
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
  if(img_data.dims() == 3) {    // an rgb guy
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
  if(img_data.dims() == 3) {    // an rgb guy
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

  int   half_int = (int)floor(half_sc);
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

  taMath_float::vec_norm_sum(&sc_ary);          // make it add to 1

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

  int x,y;                      // coords in new image
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
  float_Matrix xlate_img(false);        taBase::Ref(xlate_img);
  float_Matrix rot_img(false);          taBase::Ref(rot_img);
  float_Matrix sc_img(false);           taBase::Ref(sc_img);

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
    xformed_img = *use_img;     // todo: this is somewhat inefficient
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

  int   half_int = (int)floor(half_sc);
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

  taMath_float::vec_norm_sum(&sc_ary);          // make it add to 1

  bool wrap = (edge == WRAP);

  int pcx[2];    int pcy[2];
  float pwx[2];  float pwy[2];
  float_Matrix rot_ary(false);  rot_ary.SetGeom(2, 2, 2);

  TwoDCoord wc;
  for(wc.y=0; wc.y<win_size.y; wc.y++) {
    for(wc.x=0; wc.x<win_size.x; wc.x++) {
      FloatTwoDCoord wcd = ((FloatTwoDCoord)wc) - win_ctr; // delta from ctr in window
      FloatTwoDCoord icd = wcd / scale;                    // scaled delta from ctr in img
      FloatTwoDCoord icr(icd.x * rot_cos + icd.y * rot_sin, // rotated
                         icd.y * rot_cos - icd.x * rot_sin);
      TwoDCoord icc((int)floor(.5f + icr.x + img_ctr.x), // img center coord
                    (int)floor(.5f + icr.y + img_ctr.y));

      float r_avg = 0.0f;
      float g_avg = 0.0f;
      float b_avg = 0.0f;

      TwoDCoord oc;             // offsets
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

bool taImageProc::BubbleMask(float_Matrix& img, int n_bubbles, float bubble_sig, float_Matrix* foreground, int_Matrix* bubble_coords) {
  // floor value for mask
  float floor_thr=pow(10.0f, -8.0f);

  // get the img size -- need for lots of stuff
  TwoDCoord img_size(img.dim(0), img.dim(1));

  // create the mask and temporary mask -- if bubble_coords is specified, init it for saving coords
  float_Matrix mask;
  mask.SetGeom(2, img_size.x, img_size.y);
  float_Matrix mask_tmp;
  mask_tmp.SetGeom(2, img_size.x, img_size.y);
  for(int yi=0; yi< img_size.y; yi++) {
    for(int xi=0; xi< img_size.x; xi++) {
      mask.FastEl(xi, yi) = 0.0f;
      mask_tmp.FastEl(xi, yi) = 0.0f;
    }
  }
 
 // for saving bubble coordinates
 if(bubble_coords != NULL) {
        bubble_coords->SetGeom(2, 2, n_bubbles);
  }

  // ndgrid from matlab
  float_Matrix ndgridx;
  ndgridx.SetGeom(2, img_size.x, img_size.y);
  float_Matrix ndgridy;
  ndgridy.SetGeom(2, img_size.x, img_size.y);
  for(int yi=0; yi< img_size.y; yi++) {
    for(int xi=0; xi< img_size.x; xi++) {
      ndgridx.FastEl(xi, yi) = (float)xi;
      ndgridy.FastEl(xi, yi) = (float)yi;
    }
  }

  // apply bubbles to the mask
  for(int bubble=0; bubble < n_bubbles; bubble++) {
    // random center
   int xc = Random::IntMinMax(0, img_size.x-1);
   int yc = Random::IntMinMax(0, img_size.y-1);

   // save the bubble coords
   if(bubble_coords != NULL) {
     bubble_coords->FastEl(0, bubble) = xc;
     bubble_coords->FastEl(1, bubble) = yc;
   }

    for(int yi=0; yi< img_size.y; yi++) {
      for(int xi=0; xi< img_size.x; xi++) {
            float &mask_iv = mask_tmp.FastEl(xi, yi);
            float ndgridx_val = ndgridx.FastEl(xi, yi);
            float ndgridy_val = ndgridy.FastEl(xi, yi);
            // key formula -- note that bubble_sig is in normalized image coords here, assumes image is square (uses img_size.x)
            mask_iv = expf(-(pow(ndgridx_val-xc,2.0f) + pow(ndgridy_val-yc,2.0f))/2.0f/pow(bubble_sig*float(img_size.x),2.0f));
          }
        }

        // normalize mask each time, and combine with all masks using max
        taMath_float::vec_norm_max(&mask_tmp);
        for(int yi=0; yi< img_size.y; yi++) {
          for(int xi=0; xi< img_size.x; xi++) {
                float &mask_iv = mask.FastEl(xi, yi);
                float mask_tmp_iv = mask_tmp.FastEl(xi, yi);
                mask_iv = max(mask_iv, mask_tmp_iv);
          }
        }
  }

  // floor anything in mask below floor thresh
  taMath_float::vec_threshold_low(&mask, floor_thr, 0.0f);

  // combine the image and foreground using the mask as a weighting matrix  
  // if foreground is null, just use a blank image with border color
  float_Matrix fg;
  if(foreground == NULL) {
    float brd_clr[3];
    GetBorderColor_float(img, brd_clr[0], brd_clr[1], brd_clr[2]);
	fg.SetGeom(2, img_size.x, img_size.y);
	  for(int yi=0; yi< img_size.y; yi++) {
   		 for(int xi=0; xi< img_size.x; xi++) {
		      fg.FastEl(xi, yi) = brd_clr[0];
    	 }
      }
  }
  for(int yi=0; yi< img_size.y; yi++) {
        for(int xi=0; xi< img_size.x; xi++) {
          float &img_iv = img.FastEl(xi, yi);
          float mask_iv = mask.FastEl(xi, yi);
          float foreground_iv;
          if(foreground == NULL) {  foreground_iv = fg.FastEl(xi, yi); }
          else { foreground_iv = foreground->FastEl(xi, yi); }
          img_iv = mask_iv*img_iv + (1.0f-mask_iv)*foreground_iv;
        }
  }
  
  return true;
}

bool taImageProc::AdjustContrast(float_Matrix& img, float new_contrast, int bg_color) {
  TwoDCoord img_size(img.dim(0), img.dim(1));

  // if bg color not specified, use upper left corner pix
  float brd_clr[3];
  if(bg_color > -1) {
  	brd_clr[0] = bg_color;
  	brd_clr[1] = bg_color;
  	brd_clr[2] = bg_color;
  }
  else {
    // get background color
    GetBorderColor_float(img, brd_clr[0], brd_clr[1], brd_clr[2]);
  }

  // different processing depending on whether image is rgb or gray
  if(img.dim(2) >= 3) { // rgb or rgba
  	for(int yi=0; yi< img_size.y; yi++) {
    	for(int xi=0; xi< img_size.x; xi++) {        	 
    		// red channel 	
    		float& rv = img.FastEl(xi, yi, 0);
    		rv = ((rv-brd_clr[0])*new_contrast)+brd_clr[0];
    		// green channel
    	    float& gv = img.FastEl(xi, yi, 1);
    	    gv = ((gv-brd_clr[1])*new_contrast)+brd_clr[1];
    	    // blue channel
    	    float& bv = img.FastEl(xi, yi, 2);          	  
    	    bv = ((bv-brd_clr[2])*new_contrast)+brd_clr[2];
      	  }
  	  }
  }

  else { // grayscale
  	  for(int yi=0; yi< img_size.y; yi++) {
      	  for(int xi=0; xi< img_size.x; xi++) {      	        	  	
          	  float& iv = img.FastEl(xi, yi);
          	  iv = ((iv-brd_clr[0])*new_contrast)+brd_clr[0]; // just use red channel
      	  }
  	  }
  }
  return true;
}

bool taImageProc::CompositeImages(float_Matrix& img1, float_Matrix& img2) {
  if(img1.dim(2) != 4) {
    taMisc::Error("img1 must be rgba format");
  	return false;
  }

  // assume both images are same size
  TwoDCoord img_size(img1.dim(0), img1.dim(1));

  for(int yi=0; yi< img_size.y; yi++) {
  	for(int xi=0; xi< img_size.x; xi++) {
    	float& i1r = img1.FastEl(xi, yi, 0);
    	float& i1g = img1.FastEl(xi, yi, 1);
    	float& i1b = img1.FastEl(xi, yi, 2);
    	
    	// red channel alpha blend
    	i1r = i1r*img1.FastEl(xi, yi, 3) + img2.FastEl(xi,yi,0)*(1.0f-img1.FastEl(xi, yi, 3));
    	// green channel alpha blend
    	i1g = i1g*img1.FastEl(xi, yi, 3) + img2.FastEl(xi,yi,1)*(1.0f-img1.FastEl(xi, yi, 3));
    	// blue channel alpha blend
    	i1b = i1b*img1.FastEl(xi, yi, 3) + img2.FastEl(xi,yi,2)*(1.0f-img1.FastEl(xi, yi, 3));
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//              Visual Region Filtering (Base)

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
//      DoGRegion       Filtering

bool VisRegionSpecBase::FilterImage(float_Matrix* right_eye_image, float_Matrix* left_eye_image,
                                    bool motion_only) {
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

  cur_mot_only = motion_only;
  cur_img_r = right_eye_image;

  IncrTime();
  bool rval = FilterImage_impl(motion_only);

  cur_img_r = NULL;
  cur_img_l = NULL;

  return rval;
}

void VisRegionSpecBase::IncrTime() {
  // note: override in derived classes..
}

bool VisRegionSpecBase::FilterImage_impl(bool motion_only) {
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

bool VisRegionSpecBase::RenormOutput(RenormMode mode, float_Matrix* mat) {
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


////////////////////////////////////////////////////////////////////
//      DoGRegion       Data Table Output

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
//              List

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
//              DoG Processing

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

  // 0 1 2 3 4 5 6 7 8  retina_size = 9
  // b b .   .   . b b  border = 2, spacing = 2: input_size = 5, output_size = 3

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

  dog_out_r.SetGeom(4, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    dog_out_l.SetGeom(4, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
  else
    dog_out_l.SetGeom(1,1);     // free memory

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
//      DoGRegion       Filtering

bool DoGRegionSpec::FilterImage_impl(bool motion_only) {
  inherited::FilterImage_impl(motion_only);

  bool rval = DoGFilterImage(cur_img_r, &dog_out_r);
  if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
    rval &= DoGFilterImage(cur_img_l, &dog_out_l);
  }

  if(!data_table || save_mode == NONE_SAVE) // bail now
    return rval;

  if(dog_save & SAVE_DATA && !(!taMisc::gui_active && dog_save & ONLY_GUI)) {
    DoGOutputToTable(data_table);
  }

  return rval;
}

bool DoGRegionSpec::DoGFilterImage(float_Matrix* image, float_Matrix* out) {
  cur_img = image;
  cur_out = out;
  rgb_img = (cur_img->dims() == 3);
  wrap = (region.edge_mode == VisRegionParams::WRAP);

  if(rgb_img) {
    ColorRGBtoCMYK(*cur_img);   // precompute!
  }

  int n_run = dog_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  ThreadImgProcCall ip_call((ThreadImgProcMethod)(DoGRegionMethod)&DoGRegionSpec::DoGFilterImage_thread);
  threads.Run(&ip_call, n_run);

  // renormalize -- todo: could thread this perhaps, but chunk size would have to be larger probably
  if(dog_renorm != NO_RENORM) {
    RenormOutput(dog_renorm, out);
  }

  return true;
}

void DoGRegionSpec::DoGFilterImage_thread(int dog_idx, int thread_no) {
  TwoDCoord dc;                 // dog coords
  dc.SetFmIndex(dog_idx, dog_img_geom.x);
  TwoDCoord icc = input_size.border + dog_specs.spacing * dc; // image coords center

  float_Matrix* dog_img = cur_img;

  // x = on/off, y = color channel
  TwoDCoord ic;         // image coord
  for(int chan = 0; chan < dog_feat_geom.y; chan++) {
    ColorChannel cchan = (ColorChannel)chan;
    if(rgb_img) {
      dog_img = GetImageForChan(cchan);
    }

    float cnv_sum = 0.0f;               // convolution sum
    if(chan == 0 || rgb_img) {          // only rgb images if chan > 0
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
      cur_out->FastEl(0, chan, dc.x, dc.y) = cnv_sum; // feat x = 0 = on
      cur_out->FastEl(1, chan, dc.x, dc.y) = 0.0f;      // feat x = 1 = off
    }
    else {
      cur_out->FastEl(0, chan, dc.x, dc.y) = 0.0f;      // feat x = 0 = on
      cur_out->FastEl(1, chan, dc.x, dc.y) = -cnv_sum; // feat x = 1 = off
    }
  }
}

////////////////////////////////////////////////////////////////////
//      DoGRegion       Data Table Output

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
  DoGOutputToTable_impl(dtab, &dog_out_r, "_r");
  if(region.ocularity == VisRegionParams::BINOCULAR)
    DoGOutputToTable_impl(dtab, &dog_out_l, "_l");
  return true;
}

bool DoGRegionSpec::DoGOutputToTable_impl(DataTable* dtab, float_Matrix* out, const String& col_sufx) {
  DataCol* col;
  int idx;
  if(dog_save & SEP_MATRIX) {
    TwoDCoord dc;               // dog coords
    TwoDCoord fc;               // feature coords
    for(int i=0;i<dog_feat_geom.n;i++) {
      fc.SetFmIndex(i, dog_feat_geom.x);
      String nm = name + "_" + GetDoGFiltName(i) + "_dog";
      col = data_table->FindMakeColName(nm+ col_sufx, idx, DataTable::VT_FLOAT, 2,
                                  dog_img_geom.x, dog_img_geom.y);
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(dc.y = 0; dc.y < dog_img_geom.y; dc.y++) {
        for(dc.x = 0; dc.x < dog_img_geom.x; dc.x++) {
          float val = out->FastEl(fc.x, fc.y, dc.x, dc.y);
          dout->FastEl(dc.x, dc.y) = val;
        }
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_dog" + col_sufx, idx, DataTable::VT_FLOAT, 4,
                                      dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(out);
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
        ic.WrapClip(true, input_size.retina_size);      mat->FastEl(ic.x,ic.y) = 1.0f;
      }
    }
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}



////////////////////////////////////////////////////////////////////
//              V1 Processing -- basic RF's

///////////////////////////////////////////////////////////
//              V1 KWTA

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
  nxx1_fun.res = .001f; // needs same fine res to get the noise transitions
  nxx1_fun.UpdateAfterEdit_NoGui();
  float ns_rng = 3.0f * nvar;   // range factor based on noise level -- 3 sd
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
//              Basic Specs

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
          if(val > 0.0f)        { pos_sum += val; }
          else if(val < 0.0f)   { neg_sum += val; }
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
        if(val > 0.0f)          { val *= pos_norm; }
        else if(val < 0.0f)     { val *= neg_norm; }
      }
    }
  }
}

void V1GaborSpec::GridFilters(float_Matrix& fltrs, DataTable* graph_data, bool reset) {
  RenderFilters(fltrs);         // just to make sure

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
  r_only = true;
  n_speeds = 1;
  speed_inc = 1;
  tuning_width = 1;
  gauss_sig = 0.8f;
  opt_thr = 0.01f;

  tot_width = 1 + 2 * tuning_width;
}

void V1MotionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_width = 1 + 2 * tuning_width;
}

void V1BinocularSpec::Initialize() {
  mot_in = false;
  n_disps = 1;
  disp_range_pct = 0.05f;
  gauss_sig = 0.7f;
  disp_spacing = 2.0f;
  end_extra = 2;
  fix_horiz = true;
  horiz_thr = 0.2f;

  tot_disps = 1 + 2 * n_disps;
  ambig_wt = 1.0f / (float)tot_disps; // ambiguous case weighting
  UpdateFmV1sSize(36);
}

void V1BinocularSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_disps = 1 + 2 * n_disps;
  ambig_wt = 1.0f / (float)tot_disps; // ambiguous case weighting
}

void V1ComplexSpec::Initialize() {
  sg4 = false;
  spc4 = true;
  len_sum_len = 1;
  es_thr = 0.2f;

  sg_rf = 4;
  sg_half = sg_rf / 2;
  if(spc4)
    sg_spacing = sg_rf;
  else
    sg_spacing = sg_half;

  len_sum_width = 1 + 2 * len_sum_len;
  len_sum_norm = 1.0f / (float)(len_sum_width);
}

void V1ComplexSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  sg_rf = 4;
  sg_half = sg_rf / 2;
  if(spc4)
    sg_spacing = sg_rf;
  else
    sg_spacing = sg_half;

  len_sum_width = 1 + 2 * len_sum_len;
  len_sum_norm = 1.0f / (float)(len_sum_width);
}

void V2BordOwnSpec::Initialize() {
  lat_itrs = 10;
  lat_dt = 0.5f;
  act_thr = 0.1f;
  ambig_gain = 0.2f;
  l_t_inhib_thr = 0.2f;
  depths_out = 1;
  depth_idx = -1;

  ffbo_gain = 1.0f - ambig_gain;
}

void V2BordOwnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  ffbo_gain = 1.0f - ambig_gain;
}

void V2BordOwnStencilSpec::Initialize() {
  gain = 4.0f;
  radius = 4;
  t_on = true;
  opp_on = false;
  ang_sig = 0.5f;
  dist_sig = 0.8f;
  weak_mag = 0.5f;
  con_thr = 0.2f;
}

void VisSpatIntegSpec::Initialize() {
  spat_rf = 6;
  gauss_sig = 0.8f;
  sum_rf = false;

  spat_half = spat_rf / 2;
  spat_spacing = spat_half;
  spat_border = 0;
}

void VisSpatIntegSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  spat_rf.SetGtEq(1);
  spat_half = spat_rf / 2;
  spat_spacing = spat_half;
  spat_spacing.SetGtEq(1);
}

// for thread function calling:
typedef void (V1RegionSpec::*V1RegionMethod)(int, int);

void V1RegionSpec::Initialize() {
  v1s_renorm = LIN_RENORM;
  v1s_save = (DataSave)(SAVE_DATA | ONLY_GUI);
  v1s_feat_geom.SetXYN(4, 2, 8);

  v1m_renorm = LIN_RENORM;

  v1b_filters = BF_DEFAULT;
  v1b_renorm = LIN_RENORM;
  v1b_save = SAVE_DATA;

  v1c_filters = CF_DEFAULT;
  v1c_renorm = LIN_RENORM;
  v1c_save = SAVE_DATA;

  v2_filters = V2_DEFAULT;
  v2_save = SAVE_DATA;

  spat_integ = SI_NONE;
  si_save = SAVE_DATA;

  opt_filters = OF_NONE;
  opt_save = SAVE_DATA;

  v1s_kwta.on = true;
  v1s_kwta.gp_k = 1;
  v1s_kwta.gp_g = 0.02f;

  v1ls_kwta.on = true;
  v1ls_kwta.gp_k = 1;
  v1ls_kwta.gp_g = 0.6f;

  v1ls_neigh_inhib.on = false;
  v1ls_neigh_inhib.inhib_g = 0.8f;

  si_renorm = LIN_RENORM;
  si_kwta.on = true;
  si_kwta.gp_k = 2;
  si_kwta.gp_g = 0.1f;

  n_colors = 1;
  n_polarities = 2;
  n_polclr = n_colors * n_polarities;

  cur_out_acts = NULL;
  cur_still = NULL;
  cur_maxout = NULL;
  cur_hist = NULL;
  cur_v1b_in_r = NULL;
  cur_v1b_in_l = NULL;
}

void V1RegionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  v1s_specs.UpdateAfterEdit_NoGui();
  v1s_kwta.UpdateAfterEdit_NoGui();
  v1s_neigh_inhib.UpdateAfterEdit_NoGui();
  v1s_motion.UpdateAfterEdit_NoGui();
  v1b_specs.UpdateAfterEdit_NoGui();
  v1c_specs.UpdateAfterEdit_NoGui();
  v1ls_kwta.UpdateAfterEdit_NoGui();
  v1ls_neigh_inhib.UpdateAfterEdit_NoGui();
  v2_specs.UpdateAfterEdit_NoGui();
  v2_ffbo.UpdateAfterEdit_NoGui();
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


static void geom_get_angles(float angf, float& cosx, float& siny,
                            float& cosx_raw, float& siny_raw) {
  cosx_raw = taMath_float::cos(angf);
  siny_raw = taMath_float::sin(angf);
  // always normalize by the largest value so that it is equal to 1
  if(fabsf(cosx_raw) > fabsf(siny_raw)) {
    siny = siny_raw / fabsf(cosx_raw);          // must come first!
    cosx = cosx_raw / fabsf(cosx_raw);
  }
  else {
    cosx = cosx_raw / fabsf(siny_raw);
    siny = siny_raw / fabsf(siny_raw);
  }
}

void V1RegionSpec::UpdateGeom() {
  static bool redo = false;
  inherited::UpdateGeom();

  ///////////////////////////////////////////////////////////////
  //                    V1 S

  if(v1b_specs.mot_in) {
    if(motion_frames > 1) {
      if(v1s_motion.r_only) {
        taMisc::Warning("V1RegionSpec:", name, " -- v1b_specs.mot_in cannot have v1s_motion.r_only -- turning off");
        v1s_motion.r_only = false;
      }
    }
    else {
      taMisc::Warning("V1RegionSpec:", name, " -- v1b_specs.mot_in only works when motion_frames > 1 -- turning off for now");
      v1b_specs.mot_in = false;
    }
  }

  n_polarities = 2;             // justin case
  if(region.color == VisRegionParams::COLOR) {
    n_colors = 4;
  }
  else {
    n_colors = 1;
  }
  n_polclr = n_colors * n_polarities;
  v1s_feat_geom.x = v1s_specs.n_angles;
  v1s_feat_geom.y = n_polclr;
  v1s_feat_geom.UpdateNfmXY();

  if(region.edge_mode == VisRegionParams::WRAP) {
    v1s_img_geom = input_size.input_size / v1s_specs.spacing;
  }
  else {
    v1s_img_geom = ((input_size.input_size - 1) / v1s_specs.spacing) + 1;
  }

  v1s_ang_slopes.SetGeom(3,2,2,v1s_specs.n_angles);
  v1s_ang_slopes_raw.SetGeom(3,2,2,v1s_specs.n_angles);
  float ang_inc = taMath_float::pi / (float)v1s_specs.n_angles;
  for(int ang=0; ang<v1s_specs.n_angles; ang++) {
    float cosx, siny;
    float cosx_raw, siny_raw;
    float angf = (float)ang * ang_inc;
    geom_get_angles(angf, cosx, siny, cosx_raw, siny_raw);
    v1s_ang_slopes.FastEl(X, LINE, ang) = cosx;
    v1s_ang_slopes.FastEl(Y, LINE, ang) = siny;
    v1s_ang_slopes_raw.FastEl(X, LINE, ang) = cosx_raw;
    v1s_ang_slopes_raw.FastEl(Y, LINE, ang) = siny_raw;

    geom_get_angles(angf + taMath_float::pi * .5f, cosx, siny, cosx_raw, siny_raw);
    v1s_ang_slopes.FastEl(X, ORTHO, ang) = cosx;
    v1s_ang_slopes.FastEl(Y, ORTHO, ang) = siny;
    v1s_ang_slopes_raw.FastEl(X, ORTHO, ang) = cosx_raw;
    v1s_ang_slopes_raw.FastEl(Y, ORTHO, ang) = siny_raw;
  }

  ///////////////////////////////////////////////////////////////
  //                    V1M Motion

  // all angles for each gabor
  v1m_in_polarities = 1;        // always using polinvar inputs
  v1m_feat_geom.x = v1s_specs.n_angles;
  v1m_feat_geom.y = 2 * v1m_in_polarities * v1s_motion.n_speeds; // 2 directions
  v1m_feat_geom.UpdateNfmXY();

  ///////////////////////////////////////////////////////////////
  //                    V1B Binocular

  v1b_feat_geom.x = v1s_feat_geom.x; // just the angles
  v1b_feat_geom.y = v1b_specs.tot_disps;
  v1b_feat_geom.UpdateNfmXY();

  v1b_specs.UpdateFmV1sSize(v1s_img_geom.x); // update based on size of v1s

  ///////////////////////////////////////////////////////////////
  //                    V1C Complex

  int n_cmplx = 1;              // assume len sum
  if(v1c_filters & END_STOP) {
    v1c_filters = (ComplexFilters) (v1c_filters | LEN_SUM);     // must be set
    n_cmplx = 3;                                                // 2 es dirs
  }

  v1c_feat_geom.x = v1s_specs.n_angles;
  v1c_feat_geom.y = n_cmplx;
  v1c_feat_geom.UpdateNfmXY();

  ///////       V1C spatial geom
  if(region.edge_mode == VisRegionParams::WRAP) {
    v1c_specs.sg_border = 0;
    if(v1c_specs.sg4) {
      v1sg_img_geom = v1s_img_geom / v1c_specs.sg_spacing;
      v1c_img_geom = v1sg_img_geom;
    }
    else {
      v1sg_img_geom = v1s_img_geom;
      v1c_img_geom = v1s_img_geom;
    }
  }
  else {
    if(v1c_specs.sg4) {
      v1c_specs.sg_border = v1c_specs.sg_spacing;
      v1sg_img_geom = (((v1s_img_geom - 2 * v1c_specs.sg_border)-1) / v1c_specs.sg_spacing) + 1;
      v1c_img_geom = v1sg_img_geom;
    }
    else {
      v1c_specs.sg_border = 0;
      v1sg_img_geom = v1s_img_geom;
      v1c_img_geom = v1s_img_geom;
    }

//      v1c_img_geom = (((v1sg_img_geom - 2 * v1c_specs.spat_border)-1) / v1c_specs.spat_spacing) + 1;
  }

//   if(v2_filters & V2_BO) {
//     v2_filters = (V2Filters) (v2_filters | V2_TL); // must be set
//   }
  if(v2_filters & V2_TL) {
    v1c_filters = (ComplexFilters) (v1c_filters | (LEN_SUM | END_STOP)); // must be set
  }

  ///////////////////////////////////////
  //  Spat Integ Geoms

  if(region.edge_mode == VisRegionParams::WRAP) {
    si_specs.spat_border = 0;
    si_v1s_geom = v1s_img_geom / si_specs.spat_spacing;
    si_v1sg_geom = v1sg_img_geom / si_specs.spat_spacing;
    si_v1c_geom = v1c_img_geom / si_specs.spat_spacing;
   }
  else {
    si_specs.spat_border = si_specs.spat_spacing;
    si_v1s_geom = (((v1s_img_geom - 2 * si_specs.spat_border)-1) / si_specs.spat_spacing) + 1;
    si_v1sg_geom = (((v1sg_img_geom - 2 * si_specs.spat_border)-1) / si_specs.spat_spacing) + 1;
    si_v1c_geom = (((v1c_img_geom - 2 * si_specs.spat_border)-1) / si_specs.spat_spacing) + 1;
  }

  ///////////////////////////////////////
  //  Double-check geom from top down

  return;                       // don't do this yet -- wait till spat invar done

//   if(redo) {                 // if doing a redo, stop here and bail
//     redo = false;
//     return;
//   }

//   TwoDCoord v1s_fm_v1c;
//   if(region.edge_mode == VisRegionParams::WRAP) {
//     v1s_fm_v1c = v1c_specs.net_spacing * v1c_img_geom;
//   }
//   else {
// //     cg = ((sg - 2b - 1) / sp) + 1;
// //     cg - 1 = ((sg - 2b - 1) / sp);
// //     sp (cg - 1) = (sg - 2b - 1);
// //     sp (cg - 1) + 2b + 1 = sg;
//     if(v1c_specs.sg4) {
//       TwoDCoord v1csg_fm_v1c;
// //       v1sg_fm_v1c = v1c_specs.spat_spacing * (v1c_img_geom - 1) + 2 * v1c_specs.spat_border + 1;
//       v1s_fm_v1c = v1c_specs.sg_spacing * (v1sg_fm_v1c - 1) + 2 * v1c_specs.sg_border + 1;
//     }
//     else {
//       v1s_fm_v1c = v1c_specs.net_spacing * (v1c_img_geom - 1) + 2 * v1c_specs.net_border + 1;
//     }
//   }

//   if(v1s_fm_v1c != v1s_img_geom) { // mismatch!
//     taMisc::Info("V1RegionSpec:", name,
//               "v1s_img_geom:", v1s_img_geom.GetStr(),
//               "is not an even multiple of v1c_specs.net_spacing:",
//               v1c_specs.net_spacing.GetStr(),
//               "this geometry is:", v1s_fm_v1c.GetStr(),
//               "Now recomputing image size to fit this -- you might want to increment by some multiple of spacing to get closer to desired input size");
//     v1s_img_geom = v1s_fm_v1c;
//     redo = true;             // recompute from here
//   }

//   TwoDCoord inp_fm_v1s;
//   if(region.edge_mode == VisRegionParams::WRAP) {
//     inp_fm_v1s = v1s_img_geom * v1s_specs.spacing;
//   }
//   else {
//     inp_fm_v1s = v1s_specs.spacing * (v1s_img_geom - 1) + 1;
//   }

//   if(inp_fm_v1s != input_size.input_size) { // mismatch!
//     if(!redo) {                         // only err if not already redoing
//       taMisc::Info("V1RegionSpec:", name,
//                 "input_size:", input_size.input_size.GetStr(),
//                 "is not an even multiple of v1s_specs.spacing:", String(v1s_specs.spacing),
//                 "this geometry is:", inp_fm_v1s.GetStr(),
//                 "Recomputing image size to fit this -- you might want to increment by some multiple of spacing to get closer to desired input size");
//     }
//     input_size.input_size = inp_fm_v1s;
//     input_size.retina_size = input_size.input_size + 2 * input_size.border;
//     redo = true;             // recompute from here
//   }
}

bool V1RegionSpec::InitFilters() {
  inherited::InitFilters();
  InitFilters_V1Simple();
  if(motion_frames > 1)
    InitFilters_V1Motion();
  if(region.ocularity == VisRegionParams::BINOCULAR)
    InitFilters_V1Binocular();
  InitFilters_V1Complex();
  InitFilters_V2();
  InitFilters_SpatInteg();
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

  if(motion_frames <= 1) {
    v1m_stencils.SetGeom(1,1);
    v1m_still_stencils.SetGeom(1,1);
  }

  return true;
}

bool V1RegionSpec::InitFilters_V1Motion() {
  v1m_stencils.SetGeom(6, 2, v1s_motion.tot_width, motion_frames, 2,
                       v1s_specs.n_angles, v1s_motion.n_speeds);

  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int dir = 0; dir < 2; dir++) { // directions
      float dirsign = (dir == 0) ? -1.0f : 1.0f; // direction sign for multiplying times slope values
      float dx = dirsign * v1s_ang_slopes.FastEl(X, ORTHO, ang);
      float dy = dirsign * v1s_ang_slopes.FastEl(Y, ORTHO, ang);
      for(int speed = 0; speed < v1s_motion.n_speeds; speed++) { // speed
        int spd_off = (speed+1) * v1s_motion.speed_inc;
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

  v1m_still_stencils.SetGeom(4, 2, v1s_motion.tot_width, motion_frames, v1s_specs.n_angles);
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float dx = v1s_ang_slopes.FastEl(X, ORTHO, ang);
    float dy = v1s_ang_slopes.FastEl(Y, ORTHO, ang);
    for(int mot = 0; mot < motion_frames; mot++) { // time steps back in time
      for(int ew = -v1s_motion.tuning_width; ew <= v1s_motion.tuning_width; ew++) {
        int ox = taMath_float::rint((float)ew * dx);
        int oy = taMath_float::rint((float)ew * dy);
        v1m_still_stencils.FastEl(X, v1s_motion.tuning_width+ew, mot, ang) = ox;
        v1m_still_stencils.FastEl(Y, v1s_motion.tuning_width+ew, mot, ang) = oy;
      }
    }
  }

  v1m_weights.SetGeom(1, v1s_motion.tot_width);
  if(v1s_motion.tuning_width > 0) {
    int idx = 0;
    for(int x=-v1s_motion.tuning_width; x<=v1s_motion.tuning_width; x++, idx++) {
      float fx = (float)x / (float)v1s_motion.tuning_width;
      v1m_weights.FastEl(idx) = taMath_float::gauss_den_sig(fx, v1s_motion.gauss_sig);
    }
  }
  taMath_float::vec_norm_max(&v1m_weights); // max norm to 1
  return true;
}

bool V1RegionSpec::InitFilters_V1Binocular() {
  v1b_widths.SetGeom(1, v1b_specs.tot_disps);
  v1b_weights.SetGeom(2, v1b_specs.max_width, v1b_specs.tot_disps);
  v1b_stencils.SetGeom(2, v1b_specs.max_width, v1b_specs.tot_disps);

  v1b_weights.InitVals(0.0f);   // could have some lurkers in there from other settings, which can affect normalization

  int twe = v1b_specs.disp_range + v1b_specs.end_ext;

  // everything is conditional on the disparity
  for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
    int didx = disp + v1b_specs.n_disps;
    int doff = disp * v1b_specs.disp_spc;
    if(disp == 0) {             // focal
      v1b_widths.FastEl(didx) = 1 + 2 * v1b_specs.disp_range;
      for(int tw=-v1b_specs.disp_range; tw<=v1b_specs.disp_range; tw++) {
        int twidx = tw + v1b_specs.disp_range;
        float fx = (float)tw / (float)v1b_specs.disp_range;
        v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
        v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else if(disp == -v1b_specs.n_disps) {
      v1b_widths.FastEl(didx) = 1 + 2 * v1b_specs.disp_range + v1b_specs.end_ext;
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
  // sg4 guys -- center points relative to lower-left corner of 4x4 group
  v1sg_stencils.SetGeom(3, 3, 10, 4);
  // lengths stored in position 2 of first point
  v1sg_stencils.FastEl(2,0,0) = 8;
  v1sg_stencils.FastEl(2,0,1) = 10;
  v1sg_stencils.FastEl(2,0,2) = 8;
  v1sg_stencils.FastEl(2,0,3) = 10;
  for(int lpdx=0; lpdx < 10; lpdx++) {
    // 0 = 0 deg
    v1sg_stencils.FastEl(X, lpdx, 0) = 1 + lpdx / 4;
    v1sg_stencils.FastEl(Y, lpdx, 0) = lpdx % 4;
    // 1 = 45 deg
    v1sg_stencils.FastEl(X, lpdx, 1) = 2 + lpdx/5 - (lpdx % 5)/2;
    v1sg_stencils.FastEl(Y, lpdx, 1) = lpdx/5 + ((lpdx%5)+1)/2;
    // 2 = 90 deg
    v1sg_stencils.FastEl(X, lpdx, 2) = lpdx % 4;
    v1sg_stencils.FastEl(Y, lpdx, 2) = 1 + lpdx / 4;
    // 3 = 135 deg
    v1sg_stencils.FastEl(X, lpdx, 3) = lpdx/5 + (lpdx % 5)/2;
    v1sg_stencils.FastEl(Y, lpdx, 3) = (1 - lpdx/5) + ((lpdx%5)+1)/2;
  }

  // config: x,y coords by points, by angles
  v1ls_stencils.SetGeom(3, 2, v1c_specs.len_sum_width, v1s_specs.n_angles);
  v1es_stencils.SetGeom(5, 2, 3, 2, 2, v1s_specs.n_angles);

  for(int ang=0; ang < v1s_specs.n_angles; ang++) {
    for(int lpt=-v1c_specs.len_sum_len; lpt <= v1c_specs.len_sum_len; lpt++) {
      int lpdx = lpt + v1c_specs.len_sum_len;
      v1ls_stencils.FastEl(X, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(X, LINE, ang));
      v1ls_stencils.FastEl(Y, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(Y, LINE, ang));
    }

    float ls_off = (float)v1c_specs.len_sum_len;
    // center of length sum guy, "left" direction
    v1es_stencils.FastEl(X, 0, ON, LEFT, ang) =
      taMath_float::rint(-ls_off * v1s_ang_slopes.FastEl(X, LINE, ang));
    v1es_stencils.FastEl(Y, 0, ON, LEFT, ang) =
      taMath_float::rint(-ls_off * v1s_ang_slopes.FastEl(Y, LINE, ang));
    // center of length sum guy, "right" direction
    v1es_stencils.FastEl(X, 0, ON, RIGHT, ang) =
      taMath_float::rint(ls_off * v1s_ang_slopes.FastEl(X, LINE, ang));
    v1es_stencils.FastEl(Y, 0, ON, RIGHT, ang) =
      taMath_float::rint(ls_off * v1s_ang_slopes.FastEl(Y, LINE, ang));

    for(int orthdx=0; orthdx < 3; orthdx++) {
      int ortho = orthdx - 1;
      if(ang % 2 == 0) {        // vert, horiz
        // off guy, "left" direction
        v1es_stencils.FastEl(X, orthdx, OFF, LEFT, ang) =
          taMath_float::rint(v1s_ang_slopes.FastEl(X, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl(X, ORTHO, ang));
        v1es_stencils.FastEl(Y, orthdx, OFF, LEFT, ang) =
          taMath_float::rint(v1s_ang_slopes.FastEl(Y, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl(Y, ORTHO, ang));
        // off guy, "right" direction
        v1es_stencils.FastEl(X, orthdx, OFF, RIGHT, ang) =
          taMath_float::rint(-v1s_ang_slopes.FastEl(X, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl(X, ORTHO, ang));
        v1es_stencils.FastEl(Y, orthdx, OFF, RIGHT, ang) =
          taMath_float::rint(-v1s_ang_slopes.FastEl(Y, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl(Y, ORTHO, ang));
      }
      else {
        // this is an arcane formula that produces the "arrowhead" shape
        // off guy, "left" direction
        int off = (ortho == 0 ? 0 : 1);
        if(ortho < 0) ortho = 0;
        v1es_stencils.FastEl(X, orthdx, OFF, LEFT, ang) = (ang == 1 ? 0 : +off) +
          taMath_float::rint(v1s_ang_slopes.FastEl(X, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl(X, ORTHO, ang));
        v1es_stencils.FastEl(Y, orthdx, OFF, LEFT, ang) = (ang == 1 ? -off : 0) +
          taMath_float::rint(v1s_ang_slopes.FastEl(Y, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl(Y, ORTHO, ang));
        // off guy, "right" direction
        v1es_stencils.FastEl(X, orthdx, OFF, RIGHT, ang) = (ang == 1 ? +off : 0) +
          taMath_float::rint(-v1s_ang_slopes.FastEl(X, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl(X, ORTHO, ang));
        v1es_stencils.FastEl(Y, orthdx, OFF, RIGHT, ang) = (ang == 1 ? 0 : +off) +
          taMath_float::rint(-v1s_ang_slopes.FastEl(Y, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl(Y, ORTHO, ang));
      }
    }
  }

  // config: x,y coords by tot_ni_len, by angles
  // todo: also include close-in down/up of diagonal!
  v1ls_ni_stencils.SetGeom(3, 2, v1ls_neigh_inhib.tot_ni_len, v1s_specs.n_angles);

  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int lpt=-v1ls_neigh_inhib.inhib_d; lpt <= v1ls_neigh_inhib.inhib_d; lpt++) {
      int lpdx = lpt + v1ls_neigh_inhib.inhib_d;
      v1ls_ni_stencils.FastEl(X, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(X, ORTHO, ang)); // ortho
      v1ls_ni_stencils.FastEl(Y, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(Y, ORTHO, ang));
    }
  }

  return true;
}

bool V1RegionSpec::InitFilters_V2() {
  v2tl_stencils.SetGeom(3, 3, 2, 4);

  // NOTE: first row in figures is LEFT, 2nd row is RIGHT

  // only specify the T, LEFT = L, RIGHT case -- just reverse the DIR for T, RIGHT and L, LEFT

  // T: ANG_0
  v2tl_stencils.FastEl(0,ANG,ANG_0) = ANG_90;
  v2tl_stencils.FastEl(0,DIR,ANG_0) = LEFT;
  v2tl_stencils.FastEl(1,ANG,ANG_0) = ANG_135;
  v2tl_stencils.FastEl(1,DIR,ANG_0) = LEFT;
  v2tl_stencils.FastEl(2,ANG,ANG_0) = ANG_45;
  v2tl_stencils.FastEl(2,DIR,ANG_0) = LEFT;

  // T: ANG_45
  v2tl_stencils.FastEl(0,ANG,ANG_45) = ANG_135;
  v2tl_stencils.FastEl(0,DIR,ANG_45) = LEFT;
  v2tl_stencils.FastEl(1,ANG,ANG_45) = ANG_0;
  v2tl_stencils.FastEl(1,DIR,ANG_45) = RIGHT;
  v2tl_stencils.FastEl(2,ANG,ANG_45) = ANG_90;
  v2tl_stencils.FastEl(2,DIR,ANG_45) = LEFT;

  // T: ANG_90
  v2tl_stencils.FastEl(0,ANG,ANG_90) = ANG_0;
  v2tl_stencils.FastEl(0,DIR,ANG_90) = RIGHT;
  v2tl_stencils.FastEl(1,ANG,ANG_90) = ANG_135;
  v2tl_stencils.FastEl(1,DIR,ANG_90) = LEFT;
  v2tl_stencils.FastEl(2,ANG,ANG_90) = ANG_45;
  v2tl_stencils.FastEl(2,DIR,ANG_90) = RIGHT;

  // T: ANG_135
  v2tl_stencils.FastEl(0,ANG,ANG_135) = ANG_45;
  v2tl_stencils.FastEl(0,DIR,ANG_135) = RIGHT;
  v2tl_stencils.FastEl(1,ANG,ANG_135) = ANG_0;
  v2tl_stencils.FastEl(1,DIR,ANG_135) = RIGHT;
  v2tl_stencils.FastEl(2,ANG,ANG_135) = ANG_90;
  v2tl_stencils.FastEl(2,DIR,ANG_135) = RIGHT;

  int max_cnt = 4 * v2_ffbo.radius * v2_ffbo.radius;
  v2ffbo_stencils.SetGeom(6, 2, max_cnt, 2, v1s_specs.n_angles, 2, v1s_specs.n_angles);
  v2ffbo_weights.SetGeom(5, max_cnt, 2, v1s_specs.n_angles, 2, v1s_specs.n_angles);
  v2ffbo_stencil_n.SetGeom(4, 2, v1s_specs.n_angles, 2, v1s_specs.n_angles);
  v2ffbo_norms.SetGeom(4, 2, v1s_specs.n_angles, 2, v1s_specs.n_angles);
  TwoDCoord suc;                        // send coords
  for(int rang_dx = 0; rang_dx < v1s_specs.n_angles; rang_dx++) {
    for(int rdir = 0; rdir < 2; rdir++) {
      for(int sang_dx = 0; sang_dx < v1s_specs.n_angles; sang_dx++) {
        for(int sdir = 0; sdir < 2; sdir++) { // integrate over sending directions
          int cnt = 0;
          for(suc.y = -v2_ffbo.radius; suc.y <= v2_ffbo.radius; suc.y++) {
            for(suc.x = -v2_ffbo.radius; suc.x <= v2_ffbo.radius; suc.x++) {
              float netwt = V2FFBoWt(suc, rang_dx, sang_dx, rdir, sdir);
              if(netwt < v2_ffbo.con_thr) continue;
              v2ffbo_stencils.FastEl(X, cnt, sdir, sang_dx, rdir, rang_dx) = suc.x;
              v2ffbo_stencils.FastEl(Y, cnt, sdir, sang_dx, rdir, rang_dx) = suc.y;
              v2ffbo_weights.FastEl(cnt, sdir, sang_dx, rdir, rang_dx) = netwt;
              cnt++;
              if(cnt >= max_cnt) {
                taMisc::Error("cnt >= max_cnt:", String(max_cnt),"in V2FFBo stencil alloc -- programmer error -- please submit bug report");
                return false;
              }
            }
          }
          v2ffbo_stencil_n.FastEl(sdir, sang_dx, rdir, rang_dx) = cnt;
          if(cnt > 0)
            v2ffbo_norms.FastEl(sdir, sang_dx, rdir, rang_dx) = 1.0f / (float)cnt;
        }
      }
    }
  }

  return true;
}

float V1RegionSpec::V2FFBoWt(TwoDCoord& suc, int rang_dx, int sang_dx, int rdir, int sdir) {
  float n_angles = v1s_specs.n_angles;

  // integer angles -- useful for precise conditionals..
  int rang_n = rang_dx + rdir * 4;
  int sang_n = sang_dx + sdir * 4;
  int dang_n;
  if(sang_n < rang_n)
    dang_n = (8 + sang_n) - rang_n;
  else
    dang_n = sang_n - rang_n;
  int dang_n_pi = dang_n;
  if(dang_n >= 4) dang_n_pi = 8 - dang_n;
  int abs_dang_n_pi = dang_n_pi < 0 ? -dang_n_pi : dang_n_pi;

//   if(dang_n == 0) return 0.0f;       // no self-line guys
  if(!v2_ffbo.opp_on && dang_n == 4) return 0.0f;       // no opposite angle cons

  TwoDCoord del = suc;          // assume 0,0 ruc
  float dst = del.Mag();
  if(dst > (float)v2_ffbo.radius) return 0.0f;
  if(dst == 0.0f) return 0.0f;  // no self con
  float nrmdst = dst / (float)v2_ffbo.radius;

  float gang = atan2f(del.y, del.x); // group angle -- 0..pi or -pi
  if(gang < 0.0f) gang += 2.0f * taMath_float::pi; // keep it positive

  // dir 0 = 0..pi, dir 1 = pi..2pi
  float rang = taMath_float::pi * ((float)rang_dx / n_angles) + taMath_float::pi * (float)rdir;
  float sang = taMath_float::pi * ((float)sang_dx / n_angles) + taMath_float::pi * (float)sdir;

  float dang;                   // delta-angle -- keep this positive too
  if(sang < rang)
    dang = (2.0f * taMath_float::pi + sang) - rang;
  else
    dang = sang - rang;
  float dang_pi = dang; // this determines type of projection -- equal fabs(dang_pi) are same type
  if(dang >= taMath_float::pi) dang_pi = (2.0f * taMath_float::pi) - dang;
  float abs_dang_pi = fabs(dang_pi);

  float op_mag = 0.0f;
  if(abs_dang_pi < 0.499f * taMath_float::pi)
    op_mag = ((0.5f * taMath_float::pi - abs_dang_pi) / (0.5f * taMath_float::pi)); // 1 for 0, .5 for 45

  float tang = rang + 0.5f * dang; // target angle

  float gtang = gang - tang;
  if(gtang > taMath_float::pi) gtang -= 2.0f * taMath_float::pi;
  if(gtang < -taMath_float::pi) gtang += 2.0f * taMath_float::pi;

  // make symmetric around half sphere
  bool op_side = false;
  if(gtang > taMath_float::pi * 0.5f) { gtang -= taMath_float::pi; op_side = true; }
  if(gtang < -taMath_float::pi * 0.5f){ gtang += taMath_float::pi; op_side = true; }

  float eff_mag = 1.0f;
  if(abs_dang_pi > 0.501f * taMath_float::pi) eff_mag = v2_ffbo.weak_mag;

  float netwt = eff_mag * taMath_float::gauss_den_nonorm(gtang, v2_ffbo.ang_sig) *
    taMath_float::gauss_den_nonorm(nrmdst, v2_ffbo.dist_sig);

  if(op_side)
    netwt *= op_mag;

  if(v2_ffbo.t_on && abs_dang_n_pi == 2 && dst <= 2.9f) {
    float grang = gang - rang;
    if(fabsf(grang - (1.5f * taMath_float::pi)) < .1f ||
       fabsf(grang - (-0.5f * taMath_float::pi)) < .1f) {
      netwt = 1.0f;
    }
  }

  return netwt;
}

bool V1RegionSpec::InitFilters_SpatInteg() {
  if(si_specs.spat_rf.MaxVal() > 1) {
    taMath_float::vec_kern2d_gauss(&si_weights, si_specs.spat_rf.x,
                                   si_specs.spat_rf.y, si_specs.gauss_sig,
                                   si_specs.gauss_sig);
    taMath_float::vec_norm_max(&si_weights, 1.0f); // max, not sum
  }
  else {
    si_weights.SetGeom(2, 1,1);
    si_weights.FastEl(0,0) = 1.0f;
  }
  return true;
}


bool V1RegionSpec::InitOutMatrix() {
  inherited::InitOutMatrix();

  ///////////////////  V1S Output ////////////////////////
  v1s_out_r.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    v1s_out_l.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
  else
    v1s_out_l.SetGeom(1,1);     // free memory
  v1s_out_r_raw.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    v1s_out_l_raw.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);

  v1pi_out_r.SetGeom(4, v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    v1pi_out_l.SetGeom(4, v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
  else
    v1pi_out_l.SetGeom(1,1);

  ///////////////////  V1M Output ////////////////////////
  if(motion_frames > 1) {
    v1m_out_r.SetGeom(4, v1m_feat_geom.x, v1m_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    if(!v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR)
      v1m_out_l.SetGeom(4, v1m_feat_geom.x, v1m_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    else
      v1m_out_l.SetGeom(1,1);   // free memory

    v1m_maxout_r.SetGeom(4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    if(!v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR)
      v1m_maxout_l.SetGeom(4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    else
      v1m_maxout_l.SetGeom(1,1);        // free memory

    // hist -- only saves on/off luminance
    v1m_hist_r.SetGeom(5, v1s_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y,
                       motion_frames);
    if(!v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR)
      v1m_hist_l.SetGeom(5, v1s_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y,
                         motion_frames);
    else
      v1m_hist_l.SetGeom(1,1);  // free memory

    // still filters on top of history
    v1m_still_r.SetGeom(4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    if(!v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR)
      v1m_still_l.SetGeom(4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    else
      v1m_still_l.SetGeom(1,1); // free memory
  }
  else {
    v1m_out_r.SetGeom(1,1);
    v1m_out_l.SetGeom(1,1);
    v1m_hist_r.SetGeom(1,1);
    v1m_hist_l.SetGeom(1,1);
    v1m_still_r.SetGeom(1,1);
    v1m_still_l.SetGeom(1,1);
  }
  v1m_circ_r.matrix = &v1m_hist_r;
  v1m_circ_l.matrix = &v1m_hist_l;
  v1m_circ_r.Reset();
  v1m_circ_l.Reset();

  ///////////////////  V1B Output ////////////////////////
  if(region.ocularity == VisRegionParams::BINOCULAR) {
    v1b_dsp_out.SetGeom(4, v1b_feat_geom.x, v1b_feat_geom.y,
                        v1s_img_geom.x, v1s_img_geom.y);
    v1b_dsp_out_tmp.SetGeom(4, v1b_feat_geom.x, v1b_feat_geom.y,
                            v1s_img_geom.x, v1s_img_geom.y);


    v1b_dsp_horiz.SetGeom(3, DHZ_N, v1s_img_geom.x, v1s_img_geom.y);
  }
  else {
    v1b_dsp_out.SetGeom(1,1);
    v1b_dsp_out_tmp.SetGeom(1,1);
    v1b_dsp_horiz.SetGeom(1,1);
  }

  ///////////////////  V1C Output ////////////////////////
  if(v1c_filters & LEN_SUM) {
    v1sg_out.SetGeom(4, v1s_feat_geom.x, 1, v1sg_img_geom.x, v1sg_img_geom.y);
    v1ls_out.SetGeom(4, v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
    v1ls_out_raw.SetGeomN(v1ls_out.geom);
    if(v1c_filters & END_STOP) {
      v1es_out.SetGeom(4, v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
    }
    else {
      v1es_out.SetGeom(1,1);
    }
  }
  else {
    v1sg_out.SetGeom(1,1);
    v1ls_out.SetGeom(1,1);
    v1ls_out_raw.SetGeom(1,1);
  }

  ///////////////////  V2 Output ////////////////////////
  if(v2_filters & V2_TL) {
    v2tl_out.SetGeom(4, v1c_feat_geom.x, 4, v1c_img_geom.x, v1c_img_geom.y);
    v2tl_max.SetGeom(2, v1c_img_geom.x, v1c_img_geom.y);
  }
  else {
    v2tl_out.SetGeom(1,1);
    v2tl_max.SetGeom(1,1);
  }

  if(v2_filters & V2_BO) {
    v2bo_out.SetGeom(4, v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
    v2bo_lat.SetGeom(4, v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
    if(v2_save & SAVE_DEBUG && taMisc::gui_active)
      v2bos_out.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y*2, v1s_img_geom.x, v1s_img_geom.y);
    else
      v2bos_out.SetGeom(1,1);
  }
  else {
    v2bo_out.SetGeom(1,1);
    v2bo_lat.SetGeom(1,1);
    v2bos_out.SetGeom(1,1);
  }

  ////////////  Spat Integ

  if(spat_integ & SI_V1S) {
    si_v1s_out.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, si_v1s_geom.x, si_v1s_geom.y);
    si_v1s_out_raw.SetGeomN(si_v1s_out.geom);
  }
  else {
    si_v1s_out.SetGeom(1,1);
    si_v1s_out_raw.SetGeom(1,1);
  }
  if(spat_integ & SI_V1PI) {
    si_v1pi_out.SetGeom(4, v1s_feat_geom.x, 1, si_v1s_geom.x, si_v1s_geom.y);
    si_v1pi_out_raw.SetGeomN(si_v1pi_out.geom);
  }
  else {
    si_v1pi_out.SetGeom(1,1);
    si_v1pi_out_raw.SetGeom(1,1);
  }

  if(spat_integ & SI_V1PI_SG) {
    si_v1pi_sg_out.SetGeom(4, v1s_feat_geom.x, 1, si_v1sg_geom.x, si_v1sg_geom.y);
    si_v1pi_sg_out_raw.SetGeomN(si_v1pi_sg_out.geom);
  }
  else {
    si_v1pi_sg_out.SetGeom(1,1);
    si_v1pi_sg_out_raw.SetGeom(1,1);
  }

  if(spat_integ & SI_V1S_SG) {
    v1s_sg_out.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1sg_img_geom.x, v1sg_img_geom.y);
    si_v1s_sg_out.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, si_v1sg_geom.x, si_v1sg_geom.y);
    si_v1s_sg_out_raw.SetGeomN(si_v1s_sg_out.geom);
  }
  else {
    v1s_sg_out.SetGeom(1,1);
    si_v1s_sg_out.SetGeom(1,1);
    si_v1s_sg_out_raw.SetGeom(1,1);
  }

  if(spat_integ & SI_V1C) {
    if(spat_integ & SI_V1S_SG) { // special case -- combine both
      si_v1c_out.SetGeom(4, v1c_feat_geom.x, v1s_feat_geom.y + v1c_feat_geom.y, si_v1c_geom.x, si_v1c_geom.y);
      si_v1c_out_raw.SetGeomN(si_v1c_out.geom);
    }
    else {
      si_v1c_out.SetGeom(4, v1c_feat_geom.x, v1c_feat_geom.y, si_v1c_geom.x, si_v1c_geom.y);
      si_v1c_out_raw.SetGeomN(si_v1c_out.geom);
    }
  }
  else {
    si_v1c_out.SetGeom(1,1);
    si_v1c_out_raw.SetGeom(1,1);
  }

  if(spat_integ & SI_V2BO) {
    si_v2bo_out.SetGeom(4, v1c_feat_geom.x, 2, si_v1c_geom.x, si_v1c_geom.y);
    si_v2bo_out_raw.SetGeomN(si_v2bo_out.geom);
  }
  else {
    si_v2bo_out.SetGeom(1,1);
    si_v2bo_out_raw.SetGeom(1,1);
  }

  ///////////////////  OPT Output ////////////////////////
  if(opt_filters & ENERGY) {
    energy_out.SetGeom(2, v1s_img_geom.x, v1s_img_geom.y);
  }
  else {
    energy_out.SetGeom(1,1);
  }

  v1b_avgsum_out = 0.0f;

  return true;
}

void V1RegionSpec::IncrTime() {
  inherited::IncrTime();

  if(motion_frames <= 1) {
    return;             // nop
  }
  else {
    v1m_circ_r.CircAddLimit(motion_frames);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      v1m_circ_l.CircAddLimit(motion_frames);
    }
  }
}

////////////////////////////////////////////////////////////////////
//      V1Region        Filtering

bool V1RegionSpec::FilterImage_impl(bool motion_only) {
  inherited::FilterImage_impl(motion_only); // do dogs first

  // todo: maybe check rval for fail and bail -- not currently used..

  wrap = (region.edge_mode == VisRegionParams::WRAP);

  bool rval = V1SimpleFilter();

  if(!motion_only) {
    if(rval && v1b_filters != BF_NONE && region.ocularity == VisRegionParams::BINOCULAR) {
      rval &= V1BinocularFilter();
    }

    if(rval && v1c_filters != CF_NONE) {
      rval &= V1ComplexFilter();
    }

    if(rval && v2_filters != V2_NONE) {
      rval &= V2Filter();
    }

    if(rval && spat_integ != SI_NONE) {
      rval &= SpatIntegFilter();
    }

    if(rval && opt_filters != OF_NONE) {
      rval &= V1OptionalFilter();
    }
  }

  if(!data_table || save_mode == NONE_SAVE) // bail now
    return rval;

  if(v1s_save & SAVE_DATA && !(!taMisc::gui_active && v1s_save & ONLY_GUI)) {
    V1SOutputToTable(data_table);
  }

  if(!motion_only) {
    if(v1b_save & SAVE_DATA && v1b_filters != BF_NONE &&
       !(taMisc::gui_active && v1b_save & ONLY_GUI) &&
       (region.ocularity == VisRegionParams::BINOCULAR)) {
      V1BOutputToTable(data_table);
    }
    if(v1c_filters != CF_NONE && v1c_save & SAVE_DATA &&
       !(!taMisc::gui_active && v1c_save & ONLY_GUI)) {
      V1COutputToTable(data_table);
    }
    if(v2_filters != V2_NONE && v2_save & SAVE_DATA &&
       !(!taMisc::gui_active && v2_save & ONLY_GUI)) {
      V2OutputToTable(data_table);
    }
    if(spat_integ != SI_NONE && si_save & SAVE_DATA &&
       !(!taMisc::gui_active && si_save & ONLY_GUI)) {
      SIOutputToTable(data_table);
    }
    if(opt_filters != OF_NONE && opt_save & SAVE_DATA &&
       !(!taMisc::gui_active && opt_save & ONLY_GUI)) {
      OptOutputToTable(data_table);
    }
  }

  return rval;
}

bool V1RegionSpec::V1SimpleFilter() {
  bool rval = V1SimpleFilter_Static(cur_img_r, &v1s_out_r_raw, &v1s_out_r);
  if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
    rval &= V1SimpleFilter_Static(cur_img_l, &v1s_out_l_raw, &v1s_out_l);
  }

  rval &= V1SimpleFilter_PolInvar(&v1s_out_r, &v1pi_out_r);
  if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
    rval &= V1SimpleFilter_PolInvar(&v1s_out_l, &v1pi_out_l);
  }

  if(motion_frames > 1) {
    rval &= V1SimpleFilter_Motion(&v1pi_out_r, &v1m_out_r, &v1m_maxout_r,
                                  &v1m_still_r, &v1m_hist_r, &v1m_circ_r);
    if(rval && !v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR) {
      rval &= V1SimpleFilter_Motion(&v1pi_out_l, &v1m_out_l, &v1m_maxout_l,
                                    &v1m_still_l, &v1m_hist_l, &v1m_circ_l);
    }
  }

  return rval;
}

bool V1RegionSpec::V1SimpleFilter_Static(float_Matrix* image, float_Matrix* out_raw,
                                         float_Matrix* out) {
  cur_img = image;
  rgb_img = (cur_img->dims() == 3);

  if(rgb_img) {
    ColorRGBtoCMYK(*cur_img);   // precompute!
  }

  if(v1s_kwta.on) {
    cur_out = out_raw;
    cur_out_acts = out;
  }
  else {
    cur_out = out;
  }

  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Static_thread);
  threads.Run(&ip_call, n_run);

  if(v1s_renorm != NO_RENORM) {            // always renorm prior to any kwta
    RenormOutput(v1s_renorm, cur_out);
  }

  if(v1s_kwta.on) {
//     v1s_kwta.Compute_Kwta(*out_raw, *out, v1s_gci);
    v1s_kwta.Compute_Inhib_IThr(*out_raw, v1s_gci, v1s_ithr);
    if(v1s_neigh_inhib.on) {
      ThreadImgProcCall ip_call_ni((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Static_neighinhib_thread);
      threads.Run(&ip_call_ni, n_run);
    }
    else {
      v1s_kwta.Compute_Act(*out_raw, *cur_out_acts, v1s_gci);
    }
  }

  return true;
}

void V1RegionSpec::V1SimpleFilter_Static_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);
  TwoDCoord icc = input_size.border + v1s_specs.spacing * sc; // image coords center
  float_Matrix* v1s_img = cur_img;

  int ctr_off;
  if(v1s_specs.filter_size % 2 == 0)
    ctr_off = v1s_specs.filter_size / 2;
  else
    ctr_off = (v1s_specs.filter_size-1) / 2;

  icc -= ctr_off;               // always offset

  TwoDCoord ic;         // image coord
  for(int chan = 0; chan < n_colors; chan++) {
    ColorChannel cchan = (ColorChannel)chan;
    if(rgb_img) {
      v1s_img = GetImageForChan(cchan);
    }

    int fcy = chan * n_polarities; // starting of y axis -- add 1 for off-polarity

    for(int ang = 0; ang < v1s_specs.n_angles; ang++) {
      float cnv_sum = 0.0f;             // convolution sum
      if(chan == 0 || rgb_img) {                // only rgb images if chan > 0
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
        cur_out->FastEl(ang, fcy, sc.x, sc.y) = cnv_sum; // on-polarity
        cur_out->FastEl(ang, fcy+1, sc.x, sc.y) = 0.0f;
      }
      else {
        cur_out->FastEl(ang, fcy, sc.x, sc.y) = 0.0f;
        cur_out->FastEl(ang, fcy+1, sc.x, sc.y) = -cnv_sum; // off-polarity
      }
    }
  }
}

void V1RegionSpec::V1SimpleFilter_Static_neighinhib_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  float gi = v1s_gci.FastEl(sc.x, sc.y);

  TwoDCoord oc;         // other coord
  for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      float raw = cur_out->FastEl(ang, polclr, sc.x, sc.y);
      float feat_inhib_max = 0.0f;
      for(int lpdx=0; lpdx < v1s_neigh_inhib.tot_ni_len; lpdx++) { // go out to neighs
        if(lpdx == v1s_neigh_inhib.inhib_d) continue;              // skip self
        int xp = v1s_ni_stencils.FastEl(X,lpdx,ang);
        int yp = v1s_ni_stencils.FastEl(Y,lpdx,ang);
        oc.x = sc.x + xp;
        oc.y = sc.y + yp;
        if(oc.WrapClip(wrap, v1s_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float oth_ithr = v1s_ithr.FastEl(ang, polclr, oc.x, oc.y); // other guy
        // weights already have gain factor built in
        float ogi = v1s_neigh_inhib.inhib_g * oth_ithr; // note: directly on ithr!
        feat_inhib_max = MAX(feat_inhib_max, ogi);
      }

      float ge = v1s_kwta.g_bar_e * raw;
      float gi_eff = MAX(gi, feat_inhib_max);
      float act = v1s_kwta.Compute_ActFmIn(ge, gi_eff);
      cur_out_acts->FastEl(ang, polclr, sc.x,  sc.y) = act;
    }
  }
}

bool V1RegionSpec::V1SimpleFilter_PolInvar(float_Matrix* v1s_out_in, float_Matrix* v1pi_out) {
  cur_in = v1s_out_in;          // using img for this..
  cur_out = v1pi_out;

  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_PolInvar_thread);
  threads.Run(&ip_call, n_run);

  return true;
}

void V1RegionSpec::V1SimpleFilter_PolInvar_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float max_pi = 0.0f;
    for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
      float val = cur_in->FastEl(ang, polclr, sc.x, sc.y);
      max_pi = MAX(max_pi, val);
    }
    cur_out->FastEl(ang, 0, sc.x, sc.y) = max_pi;
  }
}


////////////////////////////////////////////////////////
//              Motion Filters

bool V1RegionSpec::V1SimpleFilter_Motion(float_Matrix* in, float_Matrix* out, float_Matrix* maxout,
                 float_Matrix* still, float_Matrix* hist, CircMatrix* circ) {
  cur_in = in;
  cur_out = out;
  cur_maxout = maxout;
  cur_still = still;
  cur_hist = hist;
  cur_circ = circ;

  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  ThreadImgProcCall ip_cp_hist((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Motion_CpHist_thread);
  threads.Run(&ip_cp_hist, n_run);

  if(!cur_mot_only) {
    // if motion only, then really just load the history for later processing!
    ThreadImgProcCall ip_call_still((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Motion_Still_thread);
    threads.Run(&ip_call_still, n_run);

    ThreadImgProcCall ip_call_mot((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Motion_thread);
    threads.Run(&ip_call_mot, n_run);

    if(v1m_renorm != NO_RENORM) {
      RenormOutput(v1m_renorm, out);
      RenormOutput(v1m_renorm, maxout);
    }
  }
  return true;
}

void V1RegionSpec::V1SimpleFilter_Motion_CpHist_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int cur_mot_idx = cur_circ->CircIdx_Last();
  int mot_len = cur_circ->length;

  TwoDCoord mo;                 // motion offset
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float in_val = cur_in->FastEl(ang, 0, sc.x, sc.y);
    cur_hist->FastEl(ang, 0, sc.x, sc.y, cur_mot_idx) = in_val;
  }
}

void V1RegionSpec::V1SimpleFilter_Motion_Still_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int cur_mot_idx = cur_circ->CircIdx_Last(); // e.g. 2
  int mot_len = cur_circ->length;             // e.g. 3

  TwoDCoord mo;                 // motion offset
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float cur_val = cur_hist->FastEl(ang, 0, sc.x, sc.y, cur_mot_idx);
    float min_mot = cur_val;
    if(cur_val >= v1s_motion.opt_thr) { // save time
      int mx_mot = mot_len-1; // don't go up to last value -- e.g., 2
      for(int mot = 0; mot < mx_mot; mot++) { // time steps back in time -- e.g., 0, 1
        float t_val = 0.0f;
        for(int tw = -v1s_motion.tuning_width; tw <= v1s_motion.tuning_width; tw++) {
          int twidx = v1s_motion.tuning_width+tw;
          int xp = v1m_still_stencils.FastEl(X, twidx, mot, ang);
          int yp = v1m_still_stencils.FastEl(Y, twidx, mot, ang);
          mo.x = sc.x + xp;
          mo.y = sc.y + yp;
          if(mo.WrapClip(wrap, v1s_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
          }
          int midx = cur_circ->CircIdx(mx_mot-1 - mot); // e.g., 1-0 = 1; 1-1 = 0,
          float val = cur_hist->FastEl(ang, 0, mo.x, mo.y, midx);
          val *= v1m_weights.FastEl(twidx);
          t_val = MAX(t_val, val);
        }
        min_mot = MIN(min_mot, t_val); // MIN = fast product
      }
    }
    cur_still->FastEl(ang, 0, sc.x, sc.y) = min_mot;
  }
}

void V1RegionSpec::V1SimpleFilter_Motion_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int cur_mot_idx = cur_circ->CircIdx_Last(); // e.g., 2
  int mot_len = cur_circ->length;             // e.g., 3

  TwoDCoord mo;                 // motion offset
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float max_out = 0.0f;
    for(int speed = 0; speed < v1s_motion.n_speeds; speed++) { // speed
      for(int dir = 0; dir < 2; dir++) { // directions
        int moty = (speed * 2 + dir);

        float cur_val = cur_hist->FastEl(ang, 0, sc.x, sc.y, cur_mot_idx);
        float still_val = cur_still->FastEl(ang, 0, sc.x, sc.y);
        cur_val -= still_val;   // subtract out still bg
        cur_val = MAX(cur_val, 0.0f);
        float min_mot = cur_val;
        if(cur_val >= v1s_motion.opt_thr) { // save time
          int mx_mot = mot_len-1; // don't go up to last value -- e.g., 2
          for(int mot = 0; mot < mx_mot; mot++) { // time steps back in time -- e.g., 0, 1
            float t_val = 0.0f;
            for(int tw = -v1s_motion.tuning_width; tw <= v1s_motion.tuning_width; tw++) {
              int twidx = v1s_motion.tuning_width+tw;
              int xp = v1m_stencils.FastEl(X, twidx, mot, dir, ang, speed);
              int yp = v1m_stencils.FastEl(Y, twidx, mot, dir, ang, speed);

              mo.x = sc.x + xp;
              mo.y = sc.y + yp;
              if(mo.WrapClip(wrap, v1s_img_geom)) {
                if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
              }
              int midx = cur_circ->CircIdx(mx_mot-1 - mot); // e.g., 1-0 = 1; 1-1 = 0,
              float val = cur_hist->FastEl(ang, 0, mo.x, mo.y, midx);
              float still_val = cur_still->FastEl(ang, 0, mo.x, mo.y);
              val -= still_val; // subtract out still bg
              val = MAX(val, 0.0f);
              val *= v1m_weights.FastEl(twidx);
              t_val = MAX(t_val, val);
            }
            min_mot = MIN(min_mot, t_val); // MIN = fast product
          }
        }
        cur_out->FastEl(ang, moty, sc.x, sc.y) = min_mot;
        max_out = MAX(max_out, min_mot);
      }
    }
    cur_maxout->FastEl(ang, 0, sc.x, sc.y) = max_out;
  }
}


////////////////////////////////////////////////////////
//              Binocular Filters

bool V1RegionSpec::V1BinocularFilter() {
  int n_run_s = v1s_img_geom.Product();
  int n_run_pre = v1sg_img_geom.Product();
  int n_run_c = v1c_img_geom.Product();

  threads.n_threads = MIN(n_run_s, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  // basic disparity matching computation -- MIN(Left, Right)
  if(v1b_specs.mot_in && motion_frames > 1) {
    cur_v1b_in_r = &v1m_maxout_r;
    cur_v1b_in_l = &v1m_maxout_l;
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_MinLr_thread);
    threads.Run(&ip_call, n_run_s);
  }
  else {
    cur_v1b_in_r = &v1pi_out_r;
    cur_v1b_in_l = &v1pi_out_l;
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_MinLr_thread);
    threads.Run(&ip_call, n_run_s);
  }

  if(v1b_specs.fix_horiz) {
    // first tag horiz line elements in parallel
    v1b_dsp_horiz.InitVals(-1);
    ThreadImgProcCall ip_call_horiz((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_HorizTag_thread);
    threads.Run(&ip_call_horiz, n_run_s);

    V1BinocularFilter_HorizAgg();       // then aggregate and correct disparity
  }

  if(v1b_renorm != NO_RENORM) {
    RenormOutput(v1b_renorm, &v1b_dsp_out);
  }

  // optional outputs
  if(v1b_filters & V1B_AVGSUM) {
    V1BinocularFilter_AvgSum();
  }
  return true;
}

void V1RegionSpec::V1BinocularFilter_MinLr_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  TwoDCoord bo;
  bo.y = sc.y;

  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    int dwd = v1b_widths.FastEl(didx);
    for(int ang = 0; ang < v1b_feat_geom.x; ang++) {
      float rv = cur_v1b_in_r->FastEl(ang, 0, sc.x, sc.y);
      float lval = 0.0f;
      for(int twidx = 0; twidx < dwd; twidx++) {
        int off = v1b_stencils.FastEl(twidx, didx);
        bo.x = sc.x - off;
        if(bo.WrapClip(wrap, v1s_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float lv = cur_v1b_in_l->FastEl(ang, 0, bo.x, bo.y);
        float lvwt = lv * v1b_weights.FastEl(twidx, didx);
        lval = MAX(lvwt, lval);                  // agg as max
      }
      float min_rl = MIN(rv, lval); // min = simple version of product..
      v1b_dsp_out.FastEl(ang, didx, sc.x, sc.y) = min_rl;
    }
  }
}

void V1RegionSpec::V1BinocularFilter_HorizTag_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  float sum_dist = 0.0f;
  int n_sum = 0;
  // horiz value is always first in row..  get it
  float hv = v1pi_out_r.FastEl(0, 0, sc.x, sc.y); // note: sc
  if(hv < v1b_specs.horiz_thr) return;          // note: v1b_dsp_horiz init -1 at start
  float max_rest = 0.0f;
  for(int ang = 1; ang < v1s_feat_geom.x; ang++) {              // rest of orients
    // response is anchored at corresponding location on the right (dominant) eye
    float rv = v1pi_out_r.FastEl(ang, 0, sc.x, sc.y); // note: sc
    if(rv > max_rest) {
      max_rest = rv;
    }
  }
  if(hv >= max_rest) {
    v1b_dsp_horiz.FastEl(DHZ_LEN, sc.x, sc.y) = 1;
    v1b_dsp_horiz.FastEl(DHZ_START, sc.x, sc.y) = sc.x;
  }
}

void V1RegionSpec::V1BinocularFilter_HorizAgg() {
  TwoDCoord sc;                 // simple coords
  for(sc.y=0; sc.y<v1s_img_geom.y; sc.y++) {
    int cur_st = -1;
    int cur_len = 0;
    int cur_mode = 0;           // 0 = nothing, 1 = completing line
    bool backprop = false;
    for(sc.x=0; sc.x<v1s_img_geom.x; sc.x++) {
      int ptlen = v1b_dsp_horiz.FastEl(DHZ_LEN, sc.x, sc.y);
      switch(cur_mode) {
      case 0: {                 // nothing
        if(ptlen > 0) {         // got something
          cur_len = 1;
          cur_st = sc.x;
          cur_mode = 1;
        }
        break;
      }
      case 1: {                 // completing line
        if(ptlen > 0) {         // keep going
          cur_len++;
        }
        else {                  // ended
          backprop = true;      // done!
        }
        break;
      }
      }

      if(backprop || (cur_mode > 0 && sc.x == v1s_img_geom.x-1)) {
        // propagate back to all the points along the horizontal line -- this is the key routine
        // just look at start and end cases -- this is only place where there is good data
        if(cur_len > 3) {
          for(int didx = 0; didx <v1b_specs.tot_disps; didx++) {
            float avg_dsp = 0.0f;
            for(int bxi=0; bxi < cur_len; bxi += cur_len-1) {
              int bx = cur_st + bxi;
              float dval = v1b_dsp_out.FastEl(0, didx, bx, sc.y); // 0 = horiz
              avg_dsp += dval;
            }
            avg_dsp *= 0.5f;    // 2 points, divide by 2

            // then copy out to the whole line
            for(int bxi=0; bxi < cur_len; bxi++) {
              int bx = cur_st + bxi;
              int& bptlen = v1b_dsp_horiz.FastEl(DHZ_LEN, bx, sc.y);
              int& bptst = v1b_dsp_horiz.FastEl(DHZ_START, bx, sc.y);
              bptlen = cur_len;
              bptst = cur_st;

              float& dval = v1b_dsp_out.FastEl(0, didx, bx, sc.y); // 0 = horiz
              dval = MIN(dval, avg_dsp);
            }
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

void V1RegionSpec::V1BinocularFilter_AvgSum() {
  float norm_val = 0.0f;
  float* sums = new float[v1b_specs.tot_disps];
  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    sums[didx] = 0.0f;
  }

  TwoDCoord bc;         // binocular coords
  for(bc.y = 0; bc.y < v1s_img_geom.y; bc.y++) {
    for(bc.x = 0; bc.x < v1s_img_geom.x; bc.x++) {
      for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
        float dmax = 0.0f;
        for(int ang=0; ang < v1b_feat_geom.x; ang++) {
          float dval = v1b_dsp_out.FastEl(ang, didx, bc.x, bc.y);
          dmax = MAX(dval, dmax);
        }
        sums[didx] += dmax;
        norm_val += dmax;
      }
    }
  }
  // todo: could read out per didx for more info
  float dwt = 0.0f;
  if(norm_val > 0.0f) {
    for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
      int disp = didx - v1b_specs.n_disps;
      sums[didx] /= norm_val;
      dwt += (float)disp * sums[didx];
    }
  }
  v1b_avgsum_out = dwt;
}


void V1RegionSpec::V1bDspCrossResMin(float extra_width, int max_extra,
                                        float pct_to_min) {
  // todo: make core routine threaded..
  RetinaProc* own = (RetinaProc*)GetOwner(&TA_RetinaProc);
  if(!own) return;
  for(int i=0; i<own->regions.size; i++) {
    V1RegionSpec* rsa = (V1RegionSpec*)own->regions.FastEl(i);
    for(int j=i+1; j<own->regions.size; j++) {
      V1RegionSpec* rsb = (V1RegionSpec*)own->regions.FastEl(j);
      V1RegionSpec* rs_sm;      // smaller
      V1RegionSpec* rs_lg;      // larger
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

      TwoDCoord lc;             // large coords
      TwoDCoord sc;             // small coords
      TwoDCoord xc;             // extra coords
      TwoDCoord alc;            // actual large coord
      for(sc.y = 0; sc.y < rs_sm->v1s_img_geom.y; sc.y++) {
        for(sc.x = 0; sc.x < rs_sm->v1s_img_geom.x; sc.x++) {
          lc = sc * sm_to_lg;
          for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
            for(int ang = 0; ang < v1b_feat_geom.x; ang++) {
              float smval = rs_sm->v1b_dsp_out.FastEl(ang, didx, sc.x, sc.y);
              float lmax = 0.0f;
              for(xc.y=-extra.y; xc.y<tot_wd.y; xc.y++) {
                for(xc.x=-extra.x; xc.x<tot_wd.x; xc.x++) {
                  alc = lc + xc;
                  if(alc.WrapClip(wrap, rs_lg->v1s_img_geom)) {
                    if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
                  }
                  float lval = rs_lg->v1b_dsp_out.FastEl(ang, didx, alc.x, alc.y);
                  lmax = MAX(lmax, lval);
                }
              }
              // soft-min function:
              float nw_val;
              if(lmax < smval) {
                nw_val = smval + pct_to_min * (lmax - smval);
              }
              else {
                nw_val = smval;
              }

//            if(sc == sm_half || sc == sm_0)
//              mn = 2.0f;      // test
              rs_sm->v1b_dsp_out_tmp.FastEl(ang, didx, sc.x, sc.y) = nw_val;
              // apply this result ONLY to the "core" large guys, not the extras..
              for(xc.y=0; xc.y<sm_to_lg.y; xc.y++) {
                for(xc.x=0; xc.x<sm_to_lg.x; xc.x++) {
                  alc = lc + xc;
                  float lval = rs_lg->v1b_dsp_out.FastEl(ang, didx, alc.x, alc.y);
                  // soft-min function:
                  float nw_val;
                  if(smval < lval) {
                    nw_val = lval + pct_to_min * (smval - lval);
                  }
                  else {
                    nw_val = lval;
                  }
                  rs_lg->v1b_dsp_out_tmp.FastEl(ang, didx, alc.x, alc.y) = nw_val;
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
    rsa->v1b_dsp_out.CopyFrom(&rsa->v1b_dsp_out_tmp); // get tmp vals after all over
    // re-output v1b_dsp_out
    if(!rsa->data_table || rsa->save_mode == NONE_SAVE) // bail now
      continue;
    rsa->V1BOutputToTable(rsa->data_table);
  }
}


//////////////////////////////////////////////////////////////////////
//              Complex Filters

bool V1RegionSpec::V1ComplexFilter() {
  int n_run = v1c_img_geom.Product();
  int n_run_sg = v1sg_img_geom.Product();
  int n_run_v1s = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  // first, square-group to optimize computation
  if(v1c_specs.sg4) {
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_SqGp4_thread);
    threads.Run(&ip_call, n_run_sg);
    cur_in = &v1sg_out;
  }
  else {
    cur_in = &v1pi_out_r;
  }

  if(v1c_filters & LEN_SUM) {
    if(v1ls_kwta.on)
      cur_out = &v1ls_out_raw;
    else
      cur_out = &v1ls_out;

    ThreadImgProcCall ip_call_ls((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_LenSum_thread);
    threads.Run(&ip_call_ls, n_run);

    // always renorm *prior* to any kwta
    if(v1c_renorm != NO_RENORM) {
      RenormOutput(v1c_renorm, cur_out);
    }

    if(v1ls_kwta.on) {
//       v1ls_kwta.Compute_Kwta(v1ls_out_raw, v1ls_out, v1ls_gci);
      v1ls_kwta.Compute_Inhib_IThr(v1ls_out_raw, v1ls_gci, v1ls_ithr);
      if(v1ls_neigh_inhib.on) {
        ThreadImgProcCall ip_call_ni((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_LenSum_neighinhib_thread);
        threads.Run(&ip_call_ni, n_run);
      }
      else {
        v1ls_kwta.Compute_Act(v1ls_out_raw, v1ls_out, v1ls_gci);
      }
    }

    if(v1c_filters & END_STOP) {
      ThreadImgProcCall ip_call_es((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_EndStop_thread);
      threads.Run(&ip_call_es, n_run);
    }
  }
  return true;
}

void V1RegionSpec::V1ComplexFilter_SqGp4_thread(int v1sg_idx, int thread_no) {
  TwoDCoord pc;                 // pre coords
  pc.SetFmIndex(v1sg_idx, v1sg_img_geom.x);
  TwoDCoord scs = v1c_specs.sg_spacing * pc; // v1s coords start
  scs += v1c_specs.sg_border;
  scs -= v1c_specs.sg_half; // convert to lower-left starting position, not center

  TwoDCoord sc;                 // simple coord
  TwoDCoord scc;                // simple coord, center
  for(int ang=0; ang<v1s_specs.n_angles; ang++) {
    float max_rf = 0.0f;   // max over spatial rfield
    int nctrs = v1sg_stencils.FastEl(2, 0, ang);         // length stored here
    for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
      int xp = v1sg_stencils.FastEl(X, ctrdx, ang);
      int yp = v1sg_stencils.FastEl(Y, ctrdx, ang);
      sc.y = scs.y + yp;
      sc.x = scs.x + xp;
      scc = sc; // center
      if(scc.WrapClip(wrap, v1s_img_geom)) {
        if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
      }
      float ctr_val = v1pi_out_r.FastEl(ang, 0, scc.x, scc.y); // gets from polinvar
      max_rf = MAX(max_rf, ctr_val);
    }
    v1sg_out.FastEl(ang, 0, pc.x, pc.y) = max_rf;
  }
}

void V1RegionSpec::V1ComplexFilter_LenSum_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  TwoDCoord lc;         // line coord
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float line_sum = 0.0f;
    for(int lpdx=0; lpdx < v1c_specs.len_sum_width; lpdx++) {
      lc.x = cc.x + v1ls_stencils.FastEl(X,lpdx,ang);
      lc.y = cc.y + v1ls_stencils.FastEl(Y,lpdx,ang);
      if(lc.WrapClip(wrap, v1c_img_geom)) {
        if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
      }
      float lval = cur_in->FastEl(ang, 0, lc.x, lc.y);
      line_sum += lval;
    }
    line_sum *= v1c_specs.len_sum_norm;
    cur_out->FastEl(ang, 0, cc.x, cc.y) = line_sum;
  }
}

void V1RegionSpec::V1ComplexFilter_LenSum_neighinhib_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  float gi = v1ls_gci.FastEl(cc.x, cc.y);

  TwoDCoord oc;         // other coord
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float raw = cur_out->FastEl(ang, 0, cc.x, cc.y);
    float feat_inhib_max = 0.0f;
    for(int lpdx=0; lpdx < v1ls_neigh_inhib.tot_ni_len; lpdx++) { // go out to neighs
      if(lpdx == v1ls_neigh_inhib.inhib_d) continue;               // skip self
      int xp = v1ls_ni_stencils.FastEl(X,lpdx,ang);
      int yp = v1ls_ni_stencils.FastEl(Y,lpdx,ang);
      oc.x = cc.x + xp;
      oc.y = cc.y + yp;
      if(oc.WrapClip(wrap, v1c_img_geom)) {
        if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
      }
      float oth_ithr = v1ls_ithr.FastEl(ang, 0, oc.x, oc.y); // other guy
      // weights already have gain factor built in
      float ogi = v1ls_neigh_inhib.inhib_g * oth_ithr; // note: directly on ithr!
      feat_inhib_max = MAX(feat_inhib_max, ogi);
    }

    float ge = v1ls_kwta.g_bar_e * raw;
    float gi_eff = MAX(gi, feat_inhib_max);
    float act = v1ls_kwta.Compute_ActFmIn(ge, gi_eff);
    v1ls_out.FastEl(ang, 0, cc.x,  cc.y) = act;
  }
}

void V1RegionSpec::V1ComplexFilter_EndStop_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  TwoDCoord lc;         // line coord
  TwoDCoord oc;         // off coord
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int dir=0; dir < 2; dir++) {                  // direction
      // len sum point
      lc.x = cc.x + v1es_stencils.FastEl(X,0,ON,dir,ang);
      lc.y = cc.y + v1es_stencils.FastEl(Y,0,ON,dir,ang);
      if(lc.WrapClip(wrap, v1c_img_geom)) {
        if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
      }
      float lsval = v1ls_out.FastEl(ang, 0, lc.x, lc.y); // len sum

      // off point
      float max_off = 0.0f;
      for(int orthdx=0; orthdx < 3; orthdx++) {
        oc.x = cc.x + v1es_stencils.FastEl(X,orthdx,OFF,dir,ang);
        oc.y = cc.y + v1es_stencils.FastEl(Y,orthdx,OFF,dir,ang);
        if(oc.WrapClip(wrap, v1c_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float offval = cur_in->FastEl(ang, 0, oc.x, oc.y); // single oriented line
        max_off = MAX(offval, max_off);
      }
      float esval = lsval - max_off;
      if(esval < v1c_specs.es_thr) esval = 0.0f; // keep it real
      v1es_out.FastEl(ang, dir, cc.x, cc.y) = esval;
    }
  }
}


//////////////////////////////////////////////////////////////////////
//              V2 Filters

bool V1RegionSpec::V2Filter() {
  int n_run = v1c_img_geom.Product();

//   if(v2_kwta.on)
//     cur_out = &v2tl_out_raw;
//   else
//     cur_out = &v2tl_out;

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  if(v2_filters & V2_TL) {
    ThreadImgProcCall ip_call_v2tl((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V2Filter_TL_thread);
    threads.Run(&ip_call_v2tl, n_run);
  }

  if(v2_filters & V2_BO) {
    if(v2_save & SAVE_DEBUG && taMisc::gui_active)
      v2bos_out.InitVals(0.0f);
    ThreadImgProcCall ip_call_v2ffbo((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V2Filter_FFBO_thread);
    threads.Run(&ip_call_v2ffbo, n_run);
    ThreadImgProcCall ip_call_v2latbo((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V2Filter_LatBO_thread);
    ThreadImgProcCall ip_call_v2latbointeg((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V2Filter_LatBOinteg_thread);
    for(int li=0; li < v2_specs.lat_itrs; li++) {
      threads.Run(&ip_call_v2latbo, n_run);
      threads.Run(&ip_call_v2latbointeg, n_run);
    }
    ThreadImgProcCall ip_call_v2bofinal((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V2Filter_BOfinal_thread);
    threads.Run(&ip_call_v2bofinal, n_run);
  }

//   if(v2_kwta.on) {
//     v2_kwta.Compute_Kwta(v2tl_out_raw, v2tl_out, v2tl_gci);
//   }

  return true;
}

void V1RegionSpec::V2Filter_TL_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  TwoDCoord lc;         // line coord
  float max_lval = 0.0f;
  float max_tval = 0.0f;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float lsedge = v1ls_out.FastEl(ang, 0, cc.x, cc.y);

    // first decrement lsedge for any end stopping along it
    float lsesmax = 0.0f;
    for(int dir=0; dir < 2; dir++) {                  // direction
      for(int lpdx=0; lpdx < v1c_specs.len_sum_width; lpdx++) {
        lc.x = cc.x + v1ls_stencils.FastEl(X,lpdx,ang);
        lc.y = cc.y + v1ls_stencils.FastEl(Y,lpdx,ang);
        if(lc.WrapClip(wrap, v1c_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float esv = v1es_out.FastEl(ang, dir, lc.x, lc.y);
        lsesmax = MAX(lsesmax, esv);
      }
    }
    lsedge -= lsesmax;
    if(lsedge < 0.0f) lsedge = 0.0f;

    for(int dir=0; dir < 2; dir++) {                  // direction

      float ang_es = v1es_out.FastEl(ang, dir, cc.x, cc.y);
      float op_ang_es = v1es_out.FastEl(ang, 1-dir, cc.x, cc.y);
      float max_ang_es = MAX(ang_es, op_ang_es);

      float op_t_max = 0.0f;
      float op_l_max = 0.0f;
      for(int opdx = 0; opdx < 3; opdx++) {
        int op_ang = v2tl_stencils.FastEl(opdx, ANG, ang);
        int op_dir = v2tl_stencils.FastEl(opdx, DIR, ang);

        // stencil is for T, LEFT -- opposite dirs for T, RIGHT
        float op_t = v1es_out.FastEl(op_ang, (dir == 0 ? op_dir : 1-op_dir), cc.x, cc.y);
        // stencil is for L, RIGHT -- opposite dirs for L, LEFT
        float op_l = v1es_out.FastEl(op_ang, (dir == 1 ? op_dir : 1-op_dir), cc.x, cc.y);

        op_t_max = MAX(op_t_max, op_t);
        op_l_max = MAX(op_l_max, op_l);
      }
      float tval = MIN(lsedge, op_t_max); // MIN = multiply
      v2tl_out.FastEl(ang, dir, cc.x, cc.y) = tval;
      float lval = MIN(max_ang_es, op_l_max); // MIN = multiply
      v2tl_out.FastEl(ang, 2+dir, cc.x, cc.y) = lval;
      max_lval = MAX(max_lval, lval);
      max_tval = MAX(max_tval, tval);
    }
  }
  if(max_lval > v2_specs.l_t_inhib_thr) {
    max_tval = 0.0f;
    // inhibit T's in same group!
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      for(int dir=0; dir < 2; dir++) {                // direction
        float& tval = v2tl_out.FastEl(ang, dir, cc.x, cc.y);
        tval -= max_lval;
        if(tval < 0.0f) tval = 0.0f;
        max_tval = MAX(max_tval, tval);
      }
    }
  }
  v2tl_max.FastEl(cc.x, cc.y) = MAX(max_lval, max_tval);
}

// todo: seems like it could do more nonlinear MIN/MAX kind of stuff for integrating?
// * diff radii for straight vs. non-straight cases!
// * multiple integration for ffstep???
void V1RegionSpec::V2Filter_FFBO_thread(int v1s_idx, int thread_no) {
  TwoDCoord cc;                 // complex coords
  cc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  TwoDCoord lc;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float lsedge = v1pi_out_r.FastEl(ang, 0, cc.x, cc.y);
    float dirmax = 0.0f;
    int maxdir = -1;
    for(int dir=0; dir < 2; dir++) {                  // direction
      if(lsedge < v2_specs.act_thr) {
        v2bo_out.FastEl(ang, dir, cc.x, cc.y) = lsedge;
        continue;
      }
      float dirval = 0.0f;
      for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
        float sedge = v1s_out_r.FastEl(ang, polclr, cc.x, cc.y);
        int pol = polclr % 2;          // polarity
        if(sedge < v2_specs.act_thr) {
          if(v2_save & SAVE_DEBUG && taMisc::gui_active)
            v2bos_out.FastEl(ang, polclr*2 + dir, cc.x, cc.y) = sedge;
          continue;
        }
        // compute netinput from ffbo stencils
        float netin = 0.0f;
        for(int sang = 0; sang < v1s_specs.n_angles; sang++) { // sending angles
          for(int sdir = 0; sdir < 2; sdir++) { // sending dir
            int poloff = 0;
            if(sdir != dir)
              poloff = (pol == 1) ? -1 : 1;
            int spolclr = polclr + poloff;
            if(sang == ang && spolclr == polclr) continue;       // no info from same guy
            int cnt = v2ffbo_stencil_n.FastEl(sdir, sang, dir, ang);
            float snetin = 0.0f;
            for(int i=0; i<cnt; i++) {
              lc.x = cc.x + v2ffbo_stencils.FastEl(X, i, sdir, sang, dir, ang);
              lc.y = cc.y + v2ffbo_stencils.FastEl(Y, i, sdir, sang, dir, ang);
              if(lc.WrapClip(wrap, v1s_img_geom)) {
                if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
              }
              float lsv = v1s_out_r.FastEl(sang, spolclr, lc.x, lc.y);
              snetin += lsv * v2ffbo_weights.FastEl(i, sdir, sang, dir, ang);
            }
            snetin *= v2_ffbo.gain * v2ffbo_norms.FastEl(sdir, sang, dir, ang);
            netin += snetin;
          }
        }
        float net_gain = v2_specs.ambig_gain + v2_specs.ffbo_gain * netin;
        float boval = net_gain * sedge;
        if(boval > sedge) boval = sedge;
        if(v2_save & SAVE_DEBUG && taMisc::gui_active)
          v2bos_out.FastEl(ang, polclr*2 + dir, cc.x, cc.y) = boval;
        dirval = MAX(dirval, boval);
      }
      if(dirval > dirmax) {
        maxdir = dir;
        dirmax = dirval;
      }
    }
    if(lsedge < v2_specs.act_thr)
      continue;
    for(int dir=0; dir < 2; dir++) {                  // direction
      if(dir == maxdir) {
        float bo = dirmax;
        if(bo > lsedge) bo = lsedge;
        v2bo_out.FastEl(ang, dir, cc.x, cc.y) = bo;
      }
      else {
        v2bo_out.FastEl(ang, dir, cc.x, cc.y) = v2_specs.ambig_gain * lsedge;
      }
    }
  }
}

void V1RegionSpec::V2Filter_LatBO_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  TwoDCoord lc;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float lsedge = v1pi_out_r.FastEl(ang, 0, cc.x, cc.y);
    if(lsedge < v2_specs.act_thr) {
      continue;
    }
    for(int dir=0; dir < 2; dir++) {                  // direction
      float netin = 0.0f;
      int cnt = v2ffbo_stencil_n.FastEl(dir, ang, dir, ang);
      float snetin = 0.0f;
      for(int i=0; i<cnt; i++) {
        lc.x = cc.x + v2ffbo_stencils.FastEl(X, i, dir, ang, dir, ang);
        lc.y = cc.y + v2ffbo_stencils.FastEl(Y, i, dir, ang, dir, ang);
        if(lc.WrapClip(wrap, v1c_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float obo = v2bo_out.FastEl(ang, dir, lc.x, lc.y);
        if(obo > 2.0f * v2_specs.ambig_gain) // non-ambig!
          snetin += obo * v2ffbo_weights.FastEl(i, dir, ang, dir, ang);
      }
      snetin *= v2_ffbo.gain * v2ffbo_norms.FastEl(dir, ang, dir, ang);
      v2bo_lat.FastEl(ang, dir, cc.x, cc.y) = snetin;
    }
  }
}

void V1RegionSpec::V2Filter_LatBOinteg_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  TwoDCoord lc;
  float dirvals[2];
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float lsedge = v1pi_out_r.FastEl(ang, 0, cc.x, cc.y);
    if(lsedge < v2_specs.act_thr) {
      continue;
    }
    float dirmax = 0.0f;
    int maxdir = -1;
    for(int dir=0; dir < 2; dir++) {                  // direction
      float dirval = v2bo_lat.FastEl(ang, dir, cc.x, cc.y);
      dirvals[dir] = dirval;
      if(dirval > dirmax) {
        maxdir = dir;
        dirmax = dirval;
      }
    }
    if(maxdir >= 0) {
      int othdir = (maxdir == 0) ? 1 : 0;
      if(dirmax - dirvals[othdir] < v2_specs.ambig_gain) {// close
        float inc = v2_specs.lat_dt * dirvals[othdir];
        float& bo = v2bo_out.FastEl(ang, othdir, cc.x, cc.y);
        bo += inc;
        if(bo > lsedge) bo = lsedge;
      }
      float inc = v2_specs.lat_dt * dirmax;
      float& bo = v2bo_out.FastEl(ang, maxdir, cc.x, cc.y);
      bo += inc;
      if(bo > lsedge) bo = lsedge;
    }
  }
}

void V1RegionSpec::V2Filter_BOfinal_thread(int v1c_idx, int thread_no) {
  TwoDCoord cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  TwoDCoord lc;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float lsedge = v1pi_out_r.FastEl(ang, 0, cc.x, cc.y);
    if(lsedge < v2_specs.act_thr) {
      continue;
    }
    float dirsum = 0.0f;
    for(int dir=0; dir < 2; dir++) {                  // direction
      float dirval = v2bo_out.FastEl(ang, dir, cc.x, cc.y);
      dirsum += dirval;
    }
    if(dirsum > lsedge) {
      // ensure that the sum never exceeds the raw val
      float dirnorm = lsedge / dirsum;
      for(int dir=0; dir < 2; dir++) {                // direction
        float& bo = v2bo_out.FastEl(ang, dir, cc.x, cc.y);
        bo *= dirnorm;
      }
    }
  }
}


/////////////////////////////////////////////////////////////////////
//              Spatial Integration

bool V1RegionSpec::SpatIntegFilter() {
  int n_run_s = si_v1s_geom.Product();
  int n_run_sg = si_v1sg_geom.Product();
  int n_run_c = si_v1c_geom.Product();

  threads.n_threads = MIN(n_run_c, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  if(spat_integ & SI_V1S) {
    if(si_kwta.on) cur_out = &si_v1s_out_raw;
    else           cur_out = &si_v1s_out;
    ThreadImgProcCall ip_call_v1s((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1S_thread);
    threads.Run(&ip_call_v1s, n_run_s);
    if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
    if(si_kwta.on) si_kwta.Compute_Kwta(si_v1s_out_raw, si_v1s_out, si_gci);
  }

  if(spat_integ & SI_V1PI) {
    if(si_kwta.on) cur_out = &si_v1pi_out_raw;
    else           cur_out = &si_v1pi_out;
    ThreadImgProcCall ip_call_v1pi((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1PI_thread);
    threads.Run(&ip_call_v1pi, n_run_s);
    if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
    if(si_kwta.on) si_kwta.Compute_Kwta(si_v1pi_out_raw, si_v1pi_out, si_gci);
  }

  if(spat_integ & SI_V1PI_SG) {
    if(si_kwta.on) cur_out = &si_v1pi_sg_out_raw;
    else           cur_out = &si_v1pi_sg_out;
    ThreadImgProcCall ip_call_v1sg((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1PI_SG_thread);
    threads.Run(&ip_call_v1sg, n_run_sg);
    if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
    if(si_kwta.on) si_kwta.Compute_Kwta(si_v1pi_sg_out_raw, si_v1pi_sg_out, si_gci);
  }

  if(spat_integ & SI_V1S_SG) {
    int n_run_sg_gp4 = v1sg_img_geom.Product();

    ThreadImgProcCall ip_call_v1ssg_pre((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1S_SqGp4_thread);
    threads.Run(&ip_call_v1ssg_pre, n_run_sg_gp4);

    if(si_kwta.on && !(spat_integ & SI_V1C)) cur_out = &si_v1s_sg_out_raw;
    else           cur_out = &si_v1s_sg_out;
    ThreadImgProcCall ip_call_v1sg((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1S_SG_thread);
    threads.Run(&ip_call_v1sg, n_run_sg);

    if(!(spat_integ & SI_V1C)) {
      if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
      if(si_kwta.on) si_kwta.Compute_Kwta(si_v1s_sg_out_raw, si_v1s_sg_out, si_gci);
    }
  }

  if(spat_integ & SI_V1C) {
    if(si_kwta.on) cur_out = &si_v1c_out_raw;
    else           cur_out = &si_v1c_out;
    ThreadImgProcCall ip_call_v1c((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1C_thread);
    threads.Run(&ip_call_v1c, n_run_c);

    if(spat_integ & SI_V1S_SG) { // both are on -- combine output into same table prior to kwta
      TwoDCoord cc;
      for(cc.y = 0; cc.y < si_v1c_geom.y; cc.y++) {
        for(cc.x = 0; cc.x < si_v1c_geom.x; cc.x++) {
          for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
            for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
              float lval = si_v1s_sg_out.FastEl(ang, polclr, cc.x, cc.y);
              cur_out->FastEl(ang, v1c_feat_geom.y + polclr, cc.x, cc.y) = lval;
            }
          }
        }
      }
    }

    if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
    if(si_kwta.on) si_kwta.Compute_Kwta(si_v1c_out_raw, si_v1c_out, si_gci);
  }

  if(spat_integ & SI_V2BO) {
    if(si_kwta.on) cur_out = &si_v2bo_out_raw;
    else           cur_out = &si_v2bo_out;
    ThreadImgProcCall ip_call_v2bo((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V2BO_thread);
    threads.Run(&ip_call_v2bo, n_run_c);
    if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
    if(si_kwta.on) si_kwta.Compute_Kwta(si_v2bo_out_raw, si_v2bo_out, si_gci);
  }

  return true;
}

void V1RegionSpec::SpatIntegFilter_V1S_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;
  sc.SetFmIndex(v1s_idx, si_v1s_geom.x);
  TwoDCoord ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  TwoDCoord ic;                 // input coord
  TwoDCoord icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
        ic.y = ics.y + ys;
        for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
          ic.x = ics.x + xs;
          icc = ic;     // center
          if(icc.WrapClip(wrap, v1s_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
          }
          float val = v1s_out_r.FastEl(ang, polclr, icc.x, icc.y);
          val *= si_weights.FastEl(xs, ys); // spatial rf weighting
          max_rf = MAX(max_rf, val);
        }
      }
      cur_out->FastEl(ang, polclr, sc.x, sc.y) = max_rf;
    } // for polclr
  }  // for ang
}

void V1RegionSpec::SpatIntegFilter_V1PI_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;
  sc.SetFmIndex(v1s_idx, si_v1s_geom.x);
  TwoDCoord ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  TwoDCoord ic;                 // input coord
  TwoDCoord icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float max_rf = 0.0f;   // max over spatial rfield
    for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
      ic.y = ics.y + ys;
      for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
        ic.x = ics.x + xs;
        icc = ic;       // center
        if(icc.WrapClip(wrap, v1s_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float val = v1pi_out_r.FastEl(ang, 0, icc.x, icc.y);
        val *= si_weights.FastEl(xs, ys); // spatial rf weighting
        max_rf = MAX(max_rf, val);
      }
    }
    cur_out->FastEl(ang, 0, sc.x, sc.y) = max_rf;
  } // for ang
}

void V1RegionSpec::SpatIntegFilter_V1PI_SG_thread(int v1sg_idx, int thread_no) {
  TwoDCoord sc;
  sc.SetFmIndex(v1sg_idx, si_v1sg_geom.x);
  TwoDCoord ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  TwoDCoord ic;                 // input coord
  TwoDCoord icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float max_rf = 0.0f;   // max over spatial rfield
    for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
      ic.y = ics.y + ys;
      for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
        ic.x = ics.x + xs;
        icc = ic;       // center
        if(icc.WrapClip(wrap, v1sg_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float val = v1sg_out.FastEl(ang, 0, icc.x, icc.y);
        val *= si_weights.FastEl(xs, ys); // spatial rf weighting
        max_rf = MAX(max_rf, val);
      }
    }
    cur_out->FastEl(ang, 0, sc.x, sc.y) = max_rf;
  } // for ang
}

void V1RegionSpec::SpatIntegFilter_V1S_SqGp4_thread(int v1sg_idx, int thread_no) {
  TwoDCoord pc;                 // pre coords
  pc.SetFmIndex(v1sg_idx, v1sg_img_geom.x);
  TwoDCoord scs = v1c_specs.sg_spacing * pc; // v1s coords start
  scs += v1c_specs.sg_border;
  scs -= v1c_specs.sg_half; // convert to lower-left starting position, not center

  TwoDCoord sc;                 // simple coord
  TwoDCoord scc;                // simple coord, center
  for(int ang=0; ang<v1s_specs.n_angles; ang++) {
    for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
      float max_rf = 0.0f;   // max over spatial rfield
      int nctrs = v1sg_stencils.FastEl(2, 0, ang);       // length stored here
      for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
        int xp = v1sg_stencils.FastEl(X, ctrdx, ang);
        int yp = v1sg_stencils.FastEl(Y, ctrdx, ang);
        sc.y = scs.y + yp;
        sc.x = scs.x + xp;
        scc = sc;       // center
        if(scc.WrapClip(wrap, v1s_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float ctr_val = v1s_out_r.FastEl(ang, polclr, scc.x, scc.y);
        max_rf = MAX(max_rf, ctr_val);
      }
      v1s_sg_out.FastEl(ang, polclr, pc.x, pc.y) = max_rf;
    }
  }
}

void V1RegionSpec::SpatIntegFilter_V1S_SG_thread(int v1sg_idx, int thread_no) {
  TwoDCoord sc;
  sc.SetFmIndex(v1sg_idx, si_v1sg_geom.x);
  TwoDCoord ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  TwoDCoord ic;                 // input coord
  TwoDCoord icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
        ic.y = ics.y + ys;
        for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
          ic.x = ics.x + xs;
          icc = ic;     // center
          if(icc.WrapClip(wrap, v1sg_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
          }
          float val = v1s_sg_out.FastEl(ang, polclr, icc.x, icc.y);
          val *= si_weights.FastEl(xs, ys); // spatial rf weighting
          max_rf = MAX(max_rf, val);
        }
      }
      cur_out->FastEl(ang, polclr, sc.x, sc.y) = max_rf;
    } // for ang
  }
}

void V1RegionSpec::SpatIntegFilter_V1C_thread(int v1c_idx, int thread_no) {
  TwoDCoord sc;
  sc.SetFmIndex(v1c_idx, si_v1c_geom.x);
  TwoDCoord ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  TwoDCoord ic;                 // input coord
  TwoDCoord icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int cfdx = 0; cfdx < v1c_feat_geom.y; cfdx++) { // cfdx features
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
        ic.y = ics.y + ys;
        for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
          ic.x = ics.x + xs;
          icc = ic;     // center
          if(icc.WrapClip(wrap, v1c_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
          }
          float val;
          if(cfdx == 0)         // length-sum = 0
            val = v1ls_out.FastEl(ang, 0, icc.x, icc.y);
          else
            val = v1es_out.FastEl(ang, cfdx-1, icc.x, icc.y);
          val *= si_weights.FastEl(xs, ys); // spatial rf weighting
          max_rf = MAX(max_rf, val);
        }
      }
      cur_out->FastEl(ang, cfdx, sc.x, sc.y) = max_rf;
    } // for cfdx
  }  // for ang
}


void V1RegionSpec::SpatIntegFilter_V2BO_thread(int v1c_idx, int thread_no) {
  TwoDCoord sc;
  sc.SetFmIndex(v1c_idx, si_v1c_geom.x);
  TwoDCoord ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  TwoDCoord ic;                 // input coord
  TwoDCoord icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int dir = 0; dir < 2; dir++) { // dir
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
        ic.y = ics.y + ys;
        for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
          ic.x = ics.x + xs;
          icc = ic;     // center
          if(icc.WrapClip(wrap, v1c_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
          }
          float val = v2bo_out.FastEl(ang, dir, icc.x, icc.y);
          val *= si_weights.FastEl(xs, ys); // spatial rf weighting
          max_rf = MAX(max_rf, val);
        }
      }
      cur_out->FastEl(ang, dir, sc.x, sc.y) = max_rf;
    } // for dir
  }  // for ang
}

// void V1RegionSpec::V1ComplexFilter_Blob_thread(int v1c_idx, int thread_no) {
//   TwoDCoord cc;                      // complex coords
//   cc.SetFmIndex(v1c_idx, v1c_img_geom.x);
//   TwoDCoord pcs = v1c_specs.spat_spacing * cc; // v1sg_out coords start
//   pcs += v1c_specs.spat_border;
//   pcs -= v1c_specs.spat_half; // convert to lower-left starting position, not center

//   TwoDCoord pc;                      // pre coord
//   TwoDCoord pcc;             // pre coord, center
//   TwoDCoord sfc;             // v1s feature coords
//   TwoDCoord fc;                      // v1c feature coords
//   for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features -- includes b/w on/off
//     sfc.y = polclr;
//     fc.y = v1c_feat_blob_y + polclr / v1c_feat_geom.x;
//     fc.x = polclr % v1c_feat_geom.x;
//     float max_rf = 0.0f;   // max over spatial rfield
//     for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // yspat
//       pc.y = pcs.y + ys;
//       for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xspat
//      pc.x = pcs.x + xs;
//      pcc = pc;       // center
//      if(pcc.WrapClip(wrap, v1sg_img_geom)) {
//        if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
//      }
//      // todo: could pre-compute this as a blob_raw guy in pre coords..
//      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // just max over angles -- blobify!
//        sfc.x = ang;
//        float ctr_val = cur_v1sg_out->FastEl(sfc.x, sfc.y, pcc.x, pcc.y);
//        ctr_val *= v1c_weights.FastEl(xs, ys); // spatial rf weighting
//        max_rf = MAX(max_rf, ctr_val);
//      }
//       }
//     }
//     cur_out->FastEl(fc.x, fc.y, cc.x, cc.y) = max_rf;
//   }
// }

/////////////////////////////////////////////////////////////////
//              Optional Filters

bool V1RegionSpec::V1OptionalFilter() {
  int n_run = v1s_img_geom.Product();
  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  if(opt_filters & ENERGY) {
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1OptionalFilter_Energy_thread);
    threads.Run(&ip_call, n_run);
  }

  return true;
}

void V1RegionSpec::V1OptionalFilter_Energy_thread(int v1s_idx, int thread_no) {
  TwoDCoord sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  float max_feat = 0.0f;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // just max over angles -- blobify!
    float v1sval = v1pi_out_r.FastEl(ang, 0, sc.x, sc.y);
    max_feat = MAX(max_feat, v1sval);
  }

  energy_out.FastEl(sc.x, sc.y) = max_feat;
}


////////////////////////////////////////////////////////////////////
//      V1Region        Data Table Output

bool V1RegionSpec::InitDataTable() {
  inherited::InitDataTable();
  if(!data_table) {
    return false;
  }

  bool fmt_only = true;
  if(v1s_save & SAVE_DATA && !(!taMisc::gui_active && v1s_save & ONLY_GUI)) {
    V1SOutputToTable(data_table, fmt_only);
  }

  if(v1b_save & SAVE_DATA && v1b_filters != BF_NONE &&
     !(taMisc::gui_active && v1b_save & ONLY_GUI)
     && (region.ocularity == VisRegionParams::BINOCULAR)) {
    V1BOutputToTable(data_table, fmt_only);
  }
  if(v1c_filters != CF_NONE && v1c_save & SAVE_DATA &&
     !(!taMisc::gui_active && v1c_save & ONLY_GUI)) {
    V1COutputToTable(data_table, fmt_only);
  }
  if(v2_filters != V2_NONE && v2_save & SAVE_DATA &&
     !(!taMisc::gui_active && v2_save & ONLY_GUI)) {
    V2OutputToTable(data_table, fmt_only);
  }
  if(spat_integ != SI_NONE && si_save & SAVE_DATA &&
     !(!taMisc::gui_active && si_save & ONLY_GUI)) {
    SIOutputToTable(data_table, fmt_only);
  }
  if(opt_filters != OF_NONE && opt_save & SAVE_DATA &&
     !(!taMisc::gui_active && opt_save & ONLY_GUI)) {
    OptOutputToTable(data_table, fmt_only);
  }

  return true;
}

bool V1RegionSpec::V1SOutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  int idx;
  V1SOutputToTable_impl(dtab, &v1s_out_r, "_r", fmt_only);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    V1SOutputToTable_impl(dtab, &v1s_out_l, "_l", fmt_only);

  { // polarinvar
    col = data_table->FindMakeColName(name + "_v1pi_r", idx, DataTable::VT_FLOAT, 4,
                                      v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&v1pi_out_r);
    }
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      col = data_table->FindMakeColName(name + "_v1pi_l", idx, DataTable::VT_FLOAT, 4,
                                        v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr doutl; doutl = (float_Matrix*)col->GetValAsMatrix(-1);
        doutl->CopyFrom(&v1pi_out_l);
      }
    }
  }

  if(motion_frames > 1) {
    V1MOutputToTable_impl(dtab, &v1m_out_r, &v1m_maxout_r, &v1m_still_r, &v1m_hist_r, &v1m_circ_r, "_r", fmt_only);
    if(!v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR)
      V1MOutputToTable_impl(dtab, &v1m_out_l, &v1m_maxout_l, &v1m_still_l, &v1m_hist_l, &v1m_circ_l, "_l", fmt_only);
  }

  return true;
}

bool V1RegionSpec::V1SOutputToTable_impl(DataTable* dtab, float_Matrix* out,
                                         const String& col_sufx, bool fmt_only) {
  TwoDCoord sc;         // simple coords
  DataCol* col;
  int idx;
  if(v1s_save & SEP_MATRIX) {
    { // basic luminance b/w filters
      col = data_table->FindMakeColName(name + "_v1s_bw" + col_sufx, idx, DataTable::VT_FLOAT, 4,
                                        v1s_feat_geom.x, 2, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
          for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
            for(int polclr = 0; polclr < 2; polclr++) { // polclr features -- just first 2
              for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
                float val = out->FastEl(ang, polclr, sc.x, sc.y);
                dout->FastEl(ang, polclr, sc.x, sc.y) = val;
              }
            }
          }
        }
      }
    }
    if(region.color == VisRegionParams::COLOR) {
      col = data_table->FindMakeColName(name + "_v1s_clr" + col_sufx, idx, DataTable::VT_FLOAT, 4,
                          v1s_feat_geom.x, 4, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
          for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
            for(int polclr = 2; polclr < n_polclr; polclr++) { // polclr features -- just color
              for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
                float val = out->FastEl(ang, polclr, sc.x, sc.y);
                dout->FastEl(ang, polclr-2, sc.x, sc.y) = val;
              }
            }
          }
        }
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_v1s" + col_sufx, idx, DataTable::VT_FLOAT, 4,
              v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(out);
    }
  }

  return true;
}


bool V1RegionSpec::V1MOutputToTable_impl(DataTable* dtab, float_Matrix* out,
     float_Matrix* maxout, float_Matrix* still, float_Matrix* hist, CircMatrix* circ,
     const String& col_sufx, bool fmt_only) {
  DataCol* col;
  int idx;

  {
    col = data_table->FindMakeColName(name + "_v1m" + col_sufx, idx, DataTable::VT_FLOAT, 4,
              v1m_feat_geom.x, v1m_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(out);
    }
  }

  {
    col = data_table->FindMakeColName(name + "_v1m_max" + col_sufx, idx, DataTable::VT_FLOAT, 4,
              v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(maxout);
    }
  }

  if(v1s_save & SAVE_DEBUG) {
    col = data_table->FindMakeColName(name + "_v1m_still" + col_sufx, idx, DataTable::VT_FLOAT,
      4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(still);
    }

    int mmax = MIN(motion_frames, circ->length);
    for(int midx=0; midx < mmax; midx++) {
      col = data_table->FindMakeColName(name + "_v1m_hist" + col_sufx + "_m" + String(midx),
                        idx, DataTable::VT_FLOAT, 4,
                        v1s_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        float_MatrixPtr lstfrm; lstfrm = (float_Matrix*)hist->GetFrameSlice(circ->CircIdx(midx));
        dout->CopyFrom(lstfrm);
      }
    }
  }
  return true;
}

bool V1RegionSpec::V1BOutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  int idx;

  if(region.ocularity == VisRegionParams::BINOCULAR) {
    if(v1b_filters & V1B_DSP) {
      col = data_table->FindMakeColName(name + "_v1b_dsp", idx, DataTable::VT_FLOAT, 4,
                                        v1b_feat_geom.x, v1b_specs.tot_disps, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v1b_dsp_out);
      }
    }
  }

  if(v1b_filters & V1B_AVGSUM) {
    col = data_table->FindMakeColName(name + "_v1b_avgsum", idx, DataTable::VT_FLOAT, 2,
                                      1, 1);
    if(!fmt_only) {
      col->SetMatrixVal(v1b_avgsum_out, -1, 0, 0);
    }
  }
  return true;
}

bool V1RegionSpec::V1COutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  TwoDCoord cc;         // complex coords
  int idx;

  if(v1c_save & SEP_MATRIX || !(v1c_filters & END_STOP)) {
    if(v1c_filters & LEN_SUM) {
      col = data_table->FindMakeColName(name + "_v1ls", idx, DataTable::VT_FLOAT, 4,
                                        v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v1ls_out);
      }
    }

    if(v1c_filters & END_STOP) {
      col = data_table->FindMakeColName(name + "_v1es", idx, DataTable::VT_FLOAT, 4,
                                        v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v1es_out);
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_v1c", idx, DataTable::VT_FLOAT, 4,
                      v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
        for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
          for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
            float lsval = v1ls_out.FastEl(ang, 0, cc.x, cc.y); // len sum
            dout->FastEl(ang, 0, cc.x, cc.y) = lsval;
            for(int dir=0; dir < 2; dir++) {                  // direction
              float esval = v1es_out.FastEl(ang, dir, cc.x, cc.y);
              dout->FastEl(ang, 1+dir, cc.x, cc.y) = esval;
            }
          }
        }
      }
    }
  }

  if(v1c_save & SAVE_DEBUG) {
    {
      col = data_table->FindMakeColName(name + "_v1sg_out", idx, DataTable::VT_FLOAT, 4,
                                        v1s_feat_geom.x, 1, v1sg_img_geom.x, v1sg_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v1sg_out);
      }
    }
  }
  return true;
}

bool V1RegionSpec::V2OutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  TwoDCoord cc;         // complex coords
  int idx;

  if(v2_filters & V2_TL) {
    if(v2_save & SEP_MATRIX) {
      {
        col = data_table->FindMakeColName(name + "_v2t", idx, DataTable::VT_FLOAT, 4,
                                          v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
        if(!fmt_only) {
          float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
          for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
            for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
              for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
                for(int dir=0; dir < 2; dir++) {                      // direction
                  float tval = v2tl_out.FastEl(ang, dir, cc.x, cc.y);
                  dout->FastEl(ang, dir, cc.x, cc.y) = tval;
                }
              }
            }
          }
        }
      }
      {
        col = data_table->FindMakeColName(name + "_v2l", idx, DataTable::VT_FLOAT, 4,
                                          v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
        if(!fmt_only) {
          float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
          for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
            for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
              for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
                for(int dir=0; dir < 2; dir++) {                      // direction
                  float lval = v2tl_out.FastEl(ang, 2+dir, cc.x, cc.y);
                  dout->FastEl(ang, dir, cc.x, cc.y) = lval;
                }
              }
            }
          }
        }
      }
    }
    else {
      col = data_table->FindMakeColName(name + "_v2tl", idx, DataTable::VT_FLOAT, 4,
                                        v1c_feat_geom.x, 4, v1c_img_geom.x, v1c_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v2tl_out);
      }
    }
  }

  if(v2_filters & V2_BO) {
    col = data_table->FindMakeColName(name + "_v2bo", idx, DataTable::VT_FLOAT, 4,
                      v1c_feat_geom.x, v2_specs.depths_out * 2, v1c_img_geom.x, v1c_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      if(v2_specs.depths_out == 1) {
        dout->CopyFrom(&v2bo_out);
      }
      else {
        if(v2_specs.depth_idx >= 0 && v2_specs.depth_idx < v2_specs.depths_out) {
          dout->InitVals(0.0f); // got to clear it b/c not setting everything
        }
        for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
          for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
            for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
              for(int dir=0; dir < 2; dir++) {                // direction
                float tval = v2bo_out.FastEl(ang, dir, cc.x, cc.y);
                if(v2_specs.depth_idx >= 0 && v2_specs.depth_idx < v2_specs.depths_out) {
                  dout->FastEl(ang, v2_specs.depth_idx * 2 + dir, cc.x, cc.y) = tval;
                }
                else {          // all
                  for(int depth=0; depth < v2_specs.depths_out; depth++) {
                    dout->FastEl(ang, depth * 2 + dir, cc.x, cc.y) = tval;
                  }
                }
              }
            }
          }
        }
      }
    }
    if(v2_save & SAVE_DEBUG && taMisc::gui_active) { // save v2bos
      col = data_table->FindMakeColName(name + "_v2bos", idx, DataTable::VT_FLOAT, 4,
                v1s_feat_geom.x, v1s_feat_geom.y*2, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v2bos_out);
      }
    }
  }

  return true;
}

bool V1RegionSpec::SIOutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  TwoDCoord cc;         // complex coords
  int idx;

  if(spat_integ & SI_V1S) {
    col = data_table->FindMakeColName(name + "_v1s_si", idx, DataTable::VT_FLOAT, 4,
                      v1s_feat_geom.x, v1s_feat_geom.y, si_v1s_geom.x, si_v1s_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v1s_out);
    }
  }
  if(spat_integ & SI_V1PI) {
    col = data_table->FindMakeColName(name + "_v1pi_si", idx, DataTable::VT_FLOAT, 4,
                      v1s_feat_geom.x, 1, si_v1s_geom.x, si_v1s_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v1pi_out);
    }
  }
  if(spat_integ & SI_V1PI_SG) {
    col = data_table->FindMakeColName(name + "_v1pi_sg_si", idx, DataTable::VT_FLOAT, 4,
                      v1s_feat_geom.x, 1, si_v1sg_geom.x, si_v1sg_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v1pi_sg_out);
    }
  }

  if(spat_integ & SI_V1S_SG && !(spat_integ & SI_V1C)) {
    // if both are on, then they are combined
    col = data_table->FindMakeColName(name + "_v1s_sg_si", idx, DataTable::VT_FLOAT, 4,
                                      v1s_feat_geom.x, v1s_feat_geom.y, si_v1sg_geom.x, si_v1sg_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v1s_sg_out);
    }
  }
  if(spat_integ & SI_V1C) {
    col = data_table->FindMakeColName(name + "_v1c_si", idx, DataTable::VT_FLOAT, 4,
                      v1c_feat_geom.x, si_v1c_out.dim(1), si_v1c_geom.x, si_v1c_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v1c_out);
    }
  }
  if(spat_integ & SI_V2BO) {
    col = data_table->FindMakeColName(name + "_v2bo_si", idx, DataTable::VT_FLOAT, 4,
                                      v1c_feat_geom.x, 2, si_v1c_geom.x, si_v1c_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v2bo_out);
    }
  }

  return true;
}



bool V1RegionSpec::OptOutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  int idx;

  if(opt_filters & ENERGY) {
    col = data_table->FindMakeColName(name + "_energy", idx, DataTable::VT_FLOAT, 2,
                      v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&energy_out);
    }
  }

  return true;
}

void V1RegionSpec::V2BoDepthFmFg(V1RetinaProc* all_flat, float fg_thr) {
  DataCol* col;
  TwoDCoord cc;         // complex coords
  int idx;

  // todo: could thread this..
  RetinaProc* own = (RetinaProc*)GetOwner(&TA_RetinaProc);
  if(!own || !all_flat) return;
  int mxn = MIN(own->regions.size, all_flat->regions.size);
  for(int i=0; i<mxn; i++) {
    V1RegionSpec* fgrs = (V1RegionSpec*)own->regions.FastEl(i);
    V1RegionSpec* flatrs = (V1RegionSpec*)all_flat->regions.FastEl(i);
    col = fgrs->data_table->FindMakeColName(fgrs->name + "_v2bo_fgbg", idx, DataTable::VT_FLOAT,
            4, fgrs->v1c_feat_geom.x, fgrs->v2_specs.depths_out * 2,
            fgrs->v1c_img_geom.x, fgrs->v1c_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->InitVals(0.0f);       // got to clear it b/c not setting everything
    for(cc.y = 0; cc.y < fgrs->v1c_img_geom.y; cc.y++) {
      for(cc.x = 0; cc.x < fgrs->v1c_img_geom.x; cc.x++) {
        float fg_max = 0.0f;
        for(int ang = 0; ang < fgrs->v1s_specs.n_angles; ang++) { // angles
          for(int dir=0; dir < 2; dir++) {                    // direction
            float fgval = fgrs->v2bo_out.FastEl(ang, dir, cc.x, cc.y);
            fg_max = MAX(fgval, fg_max);
          }
        }
        int off = 2;                                              // bg
        if(fg_max >= fg_thr) off = 0;                             // fg
        for(int ang = 0; ang < fgrs->v1s_specs.n_angles; ang++) { // angles
          for(int dir=0; dir < 2; dir++) {                    // direction
            float flatval = flatrs->v2bo_out.FastEl(ang, dir, cc.x, cc.y);
            dout->FastEl(ang, off + dir, cc.x, cc.y) = flatval; // fg
          }
        }
      }
    }
  }
}

void V1RegionSpec::V2BoDepthFmImgMask(DataTable* img_mask, V1RetinaProc* all_flat, float fg_thr) {
  DataCol* col;
  TwoDCoord cc;         // complex coords
  int idx;

  // todo: could thread this..
  RetinaProc* own = (RetinaProc*)GetOwner(&TA_RetinaProc);
  if(!own || !all_flat || !img_mask) return;
  int mxn = MIN(own->regions.size, all_flat->regions.size);
  mxn = MIN(mxn, img_mask->cols());
  for(int i=0; i<mxn; i++) {
    V1RegionSpec* fgrs = (V1RegionSpec*)own->regions.FastEl(i);
    V1RegionSpec* flatrs = (V1RegionSpec*)all_flat->regions.FastEl(i);
    DataCol* mask = img_mask->data.FastEl(i);
    float_MatrixPtr mmat; mmat = (float_Matrix*)mask->GetValAsMatrix(-1);
    col = fgrs->data_table->FindMakeColName(fgrs->name + "_v2bo_fgbg", idx, DataTable::VT_FLOAT,
            4, fgrs->v1c_feat_geom.x, fgrs->v2_specs.depths_out * 2,
            fgrs->v1c_img_geom.x, fgrs->v1c_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->InitVals(0.0f);       // got to clear it b/c not setting everything
    for(cc.y = 0; cc.y < fgrs->v1c_img_geom.y; cc.y++) {
      for(cc.x = 0; cc.x < fgrs->v1c_img_geom.x; cc.x++) {
        float mask_val = mmat->FastEl(cc.x, cc.y);
        float fg_max = mask_val;
        if(fg_max < fg_thr) {
          for(int ang = 0; ang < fgrs->v1s_specs.n_angles; ang++) { // angles
            for(int dir=0; dir < 2; dir++) {                  // direction
              float fgval = fgrs->v2bo_out.FastEl(ang, dir, cc.x, cc.y);
              fg_max = MAX(fgval, fg_max);
            }
          }
        }
        int off = 2;                                              // bg
        if(fg_max >= fg_thr) off = 0;                             // fg
        for(int ang = 0; ang < fgrs->v1s_specs.n_angles; ang++) { // angles
          for(int dir=0; dir < 2; dir++) {                    // direction
            float flatval = flatrs->v2bo_out.FastEl(ang, dir, cc.x, cc.y);
            dout->FastEl(ang, off + dir, cc.x, cc.y) = flatval; // fg
          }
        }
      }
    }
  }
}


/////////////////////////////////////////////////////
//                      Graphing

int  V1RegionSpec::AngleDeg(int ang_no) {
  int ang_inc = 180 / v1s_specs.n_angles;
  return ang_no * ang_inc;
}

void V1RegionSpec::GridGaborFilters(DataTable* graph_data) {
  v1s_specs.GridFilters(v1s_gabor_filters, graph_data);
}

void V1RegionSpec::GridV1Stencils(DataTable* graph_data) {
  Init();                       // need to init stencils for sure!

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
  max_sz.Max(si_specs.spat_rf);

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

  TwoDCoord brd(5,5);           // border
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

  if(v1c_specs.sg4) { // v1complex, sg4
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
      int nctrs = v1sg_stencils.FastEl(2, 0, ang);       // length stored here
      for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
        int xp = v1sg_stencils.FastEl(X, ctrdx, ang);
        int yp = v1sg_stencils.FastEl(Y, ctrdx, ang);
        ic.x = brd.x + xp;
        ic.y = brd.y + yp;

        if(ic.WrapClip(true, max_sz)) continue;
        mat->FastEl(ic.x,ic.y) = (ctrdx % 2 == 0) ? 1.0f: -1.0f;
      }
    }
  }
  { // spatial integ
    if(si_specs.spat_rf.MaxVal() > 1) {
      graph_data->AddBlankRow();
      nmda->SetValAsString("Spat Integ RF", -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      TwoDCoord sc;
      for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // ysimple
        sc.y = brd.y + ys;
        for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xsimple
          sc.x = brd.x + xs;
          if(sc.WrapClip(true, max_sz)) continue;
          mat->FastEl(sc.x,sc.y) = si_weights.FastEl(xs, ys);
        }
      }
    }
  }

  { // v1complex, ls, es
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1C Len Sum Ang: " + String(AngleDeg(ang)), -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      TwoDCoord ic;
      for(int lpdx=0; lpdx < v1c_specs.len_sum_width; lpdx++) {
        ic.x = brd.x + v1ls_stencils.FastEl(X,lpdx,ang);
        ic.y = brd.y + v1ls_stencils.FastEl(Y,lpdx,ang);
        if(ic.WrapClip(true, max_sz)) continue;
        mat->FastEl(ic.x,ic.y) = 1.0f;
      }
    }
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      for(int dir=0; dir < 2; dir++) {                // direction
        graph_data->AddBlankRow();
        nmda->SetValAsString("V1C End Stop Ang: " + String(AngleDeg(ang))
                             + " Dir: " + String(dir), -1);
        float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
        mat->FastEl(brd.x,brd.y) = 0.5f;
        TwoDCoord ic;
        ic.x = brd.x + v1es_stencils.FastEl(X,0,ON,dir,ang);
        ic.y = brd.y + v1es_stencils.FastEl(Y,0,ON,dir,ang);
        if(ic.WrapClip(true, max_sz)) continue;
        mat->FastEl(ic.x,ic.y) = 1.0f;

        for(int orthdx=0; orthdx < 3; orthdx++) {
          ic.x = brd.x + v1es_stencils.FastEl(X,orthdx,OFF,dir,ang);
          ic.y = brd.y + v1es_stencils.FastEl(Y,orthdx,OFF,dir,ang);
          if(ic.WrapClip(true, max_sz)) continue;
          mat->FastEl(ic.x,ic.y) = -1.0f;
        }
      }
    }
  }

  if(v2_filters & V2_BO) {
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      for(int dir=0; dir < 2; dir++) {                // direction
        for(int sang = 0; sang < v1s_specs.n_angles; sang++) { // angles
          for(int sdir=0; sdir < 2; sdir++) {                 // direction
            graph_data->AddBlankRow();
            nmda->SetValAsString("V2BO Ang: " + String(AngleDeg(ang))
                                 + " Dir: " + String(dir)
                                 + " SAng: " + String(AngleDeg(sang))
                                 + " SDir: " + String(sdir), -1);
            float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
            mat->FastEl(brd.x,brd.y) = -0.5f;
            TwoDCoord ic;
            int cnt = v2ffbo_stencil_n.FastEl(sdir, sang, dir, ang);
            for(int i=0; i<cnt; i++) {
              ic.x = brd.x + v2ffbo_stencils.FastEl(X, i, sdir, sang, dir, ang);
              ic.y = brd.y + v2ffbo_stencils.FastEl(Y, i, sdir, sang, dir, ang);
              if(ic.WrapClip(true, max_sz)) continue;
              mat->FastEl(ic.x,ic.y) = v2ffbo_weights.FastEl(i, sdir, sang, dir, ang);
            }
          }
        }
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
//      ic.y = y; ic.x = x;
//      ic.WrapClip(true, input_size.retina_size);      mat->FastEl(ic.x,ic.y) = 1.0f;
//       }
//     }
//   }

//   { // then v1 simple
//     graph_data->AddBlankRow();
//     nmda->SetValAsString("V1_Simple", -1);
//     float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
//     TwoDCoord brd(input_size.border.x+v1s_specs.border*dog_specs.spacing.x,
//                input_size.border.y+v1s_specs.border*dog_specs.spacing.y);
//     TwoDCoord spc(dog_specs.spacing.x * v1s_specs.spacing, dog_specs.spacing.y * v1s_specs.spacing);
//     // first render borders of RF's, every other
//     for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= 2*spc.y) {
//       for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+= 2*spc.x) {
//      ic.y = y; ic.x = x;
//      ic -= v1s_specs.rf_half*dog_specs.spacing; // lower left
//      TwoDCoord ec;
//      int ex,ey;
//      for(ey=0; ey < v1s_specs.rf_size; ey++) {
//        ec.y = ic.y + ey*dog_specs.spacing.y;  ec.x = ic.x;
//        ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
//        ec.y = ic.y + ey*dog_specs.spacing.y;  ec.x = ic.x + dog_specs.spacing.x * (v1s_specs.rf_size-1);
//        ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
//      }
//      for(ex=0; ex < v1s_specs.rf_size; ex++) {
//        ec.y = ic.y;    ec.x = ic.x + ex*dog_specs.spacing.x;
//        ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
//        ec.y = ic.y + dog_specs.spacing.y * (v1s_specs.rf_size-1); ec.x = ic.x + ex*dog_specs.spacing.x;
//        ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
//      }
//       }
//     }
//     // then centers
//     for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= spc.y) {
//       for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+=spc.x) {
//      ic.y = y; ic.x = x;
//      ic.WrapClip(true, input_size.retina_size);      mat->FastEl(ic.x,ic.y) = 1.0f;
//       }
//     }
//   }

//   { // then v1 complex
//     TwoDCoord ic;
//     int x,y;
//     graph_data->AddBlankRow();
//     nmda->SetValAsString("V1_Complex", -1);
//     float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
//     TwoDCoord brd(input_size.border.x+v1s_specs.spacing*v1c_specs.net_border.x,
//                input_size.border.y+v1s_specs.spacing*v1c_specs.net_border.y);
//     TwoDCoord spc(v1s_specs.spacing * v1c_specs.net_spacing.x,
//                v1s_specs.spacing * v1c_specs.net_spacing.y);
//     TwoDCoord spcb(v1s_specs.spacing, v1s_specs.spacing);
//     // first render borders of RF's, every other
//     for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= 2*spc.y) {
//       for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+= 2*spc.x) {
//      ic.y = y; ic.x = x;
//      ic -= v1c_specs.net_half*spcb; // lower left
//      TwoDCoord ec;
//      int ex,ey;
//      for(ey=0; ey < v1c_specs.spat_rf.y; ey++) {
//        ec.y = ic.y + ey*spcb.y;  ec.x = ic.x;
//        ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
//        ec.y = ic.y + ey*spcb.y;  ec.x = ic.x + spcb.x * (v1c_specs.spat_rf.x-1);
//        ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
//      }
//      for(ex=0; ex < v1c_specs.spat_rf.x; ex++) {
//        ec.y = ic.y;    ec.x = ic.x + ex*spcb.x;
//        ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
//        ec.y = ic.y + spcb.y * (v1c_specs.spat_rf.y-1); ec.x = ic.x + ex*spcb.x;
//        ec.WrapClip(true, input_size.retina_size); mat->FastEl(ec.x,ec.y) = 0.2f;
//      }
//       }
//     }
//     for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= spc.y) {
//       for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+=spc.x) {
//      ic.y = y; ic.x = x;
//      ic.WrapClip(true, input_size.retina_size);      mat->FastEl(ic.x,ic.y) = 1.0f;
//       }
//     }
//   }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}


//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//              Full Retinal Spec

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
//              Basic impl routines

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
  if(TestError(scale == 0.0f, "TransformImageData_impl",
               "scale is 0 -- indicates bad parameters probably"))
    return false;

  VisRegionSpecBase* reg = regions[0]; // take params from first

  float ctr_x = .5f + .5 * move_x;
  float ctr_y = .5f + .5 * move_y;

  if(TestError(eye_image.dims() < 2, "TransformImageData_impl",
               "eye input image input must be at least 2 dimensional"))
    return false;

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

  if(TestError(scale == 0.0f, "LookAtImageData_impl",
               "scale is 0 -- indicates bad parameters probably"))
    return false;

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

bool RetinaProc::FilterImageData(bool motion_only) {
  if(regions.size == 0) return false;
  for(int ri=0; ri < regions.size; ri++) {
    VisRegionSpecBase* reg = regions[ri];
    reg->FilterImage(&xform_image_r, &xform_image_l, motion_only);
  }
  return true;
}

//////////////////////////////////////////////////////
//              Various front-ends

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
  img_r.name = right_eye_img_fname;             // explicitly name it
  if(left_eye_img_fname.nonempty()) {
    taImage img_l;
    if(!img_l.LoadImage(left_eye_img_fname)) return false;
    img_l.name = left_eye_img_fname;            // explicitly name it
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
  img_r.name = right_eye_img_fname;             // explicitly name it
  if(left_eye_img_fname.nonempty()) {
    taImage img_l;
    if(!img_l.LoadImage(left_eye_img_fname)) return false;
    img_l.name = left_eye_img_fname;            // explicitly name it
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
//                                              sp->spacing.output_size.x, sp->spacing.output_size.y);
//     DataCol* da_off = dt->FindMakeColName(sp->name + "_off", idx, DataTable::VT_FLOAT,
//                                               2, sp->spacing.output_size.x, sp->spacing.output_size.y);

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
//              program stuff

void ImageProcCall::Initialize() {
  min_type = &TA_taImageProc;
  object_type = &TA_taImageProc;
}

bool ImageProcCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains("::")) return false;
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  String objnm = mthobj.before("::");
  TypeDef* td = taMisc::types.FindName(objnm);
  if(!td) return false;
  if(objnm == "taImageProc") return true;
  return false;
}

