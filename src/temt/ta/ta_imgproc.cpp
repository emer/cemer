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

bool taImage::ImageToGrey_float(float_Matrix& img_data) {
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

bool taImage::ImageToRGB_float(float_Matrix& rgb_data) {
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

bool taImage::ImageToDataCell(DataTable* dt, int col, int row) {
  if(q_img.isNull() || !dt) {
    return false;
  }
  int ht = q_img.height();
  int wd = q_img.width();

  taMatrixPtr mat; mat = dt->GetValAsMatrix(col, row);
  if(!mat) return false;

  bool rval = true;

  DataUpdate(true);
  if(mat->dims() == 2) {
    for(int y=0; y<ht; y++) {
      for(int x=0; x< wd; x++) {
	QRgb pix = q_img.pixel(x, y);
	float gval = qGray(pix) / 255.0f;
	mat->SetFmVar(gval, x, ht-1 - y);
      }
    }
  }
  else if(mat->dims() == 3) {
    for(int y=0; y<ht; y++) {
      for(int x=0; x< wd; x++) {
	QRgb pix = q_img.pixel(x, y);
	float rval = qRed(pix) / 255.0f;
	float gval = qGreen(pix) / 255.0f;
	float bval = qBlue(pix) / 255.0f;
	mat->SetFmVar(rval, x, ht-1 - y, 0);
	mat->SetFmVar(gval, x, ht-1 - y, 1);
	mat->SetFmVar(bval, x, ht-1 - y, 2);
      }
    }
  }
  else {
    TestError(true, "ImageToDataCell", "cell matrix is not either 2 or 3 dimensional");
    rval = false;
  }

  DataUpdate(false); 

  return rval;
}

bool taImage::ImageToDataCellName(DataTable* dt, const String& col_nm, int row) {
  if(q_img.isNull() || !dt) {
    return false;
  }
  int col= dt->FindColNameIdx(col_nm, true);
  if(col < 0) return false;
  DataCol* da = dt->data.FastEl(col);
  return ImageToDataCell(dt, col, row);
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
  uidx_st = -1;
  uidx_ed = -1;
  uidx_inc = -1;
  img_proc_call = NULL;
}

void ImgProcCallTask::Destroy() {
  img_proc_call = NULL;
}

void ImgProcCallTask::run() {
  ImgProcCallThreadMgr* mg = mgr();
  ImgProcThreadBase* base = mg->img_proc();

  for(int i=uidx_st; i<uidx_ed; i+=uidx_inc) {
    img_proc_call->call(base, i, task_id); // task id indicates threading, and which thread
  }
}

void ImgProcCallThreadMgr::Initialize() {
  min_units = taMisc::thread_defaults.min_units;
}

void ImgProcCallThreadMgr::Destroy() {
}

void ImgProcCallThreadMgr::InitAll() {
  InitThreads();
  CreateTasks(&TA_ImgProcCallTask);
  SetTasksToThreads();
}

void ImgProcCallThreadMgr::Run(ThreadImgProcCall* img_proc_call, int n_cmp_units) {
  ImgProcThreadBase* base = img_proc();
  if(n_threads == 1 || n_cmp_units < min_units || n_cmp_units < tasks.size) {
    for(int i=0;i<n_cmp_units;i++) {
      img_proc_call->call(base, i, -1); // -1 indicates no threading
    }
  }
  else {
    const int nt = tasks.size;
    int chnk_size = n_cmp_units / nt;
    int n_chunked = chnk_size * nt;
    int last_chnk = chnk_size + (n_cmp_units - n_chunked);

    int chk_dx = 0;

    for(int i=0;i<nt;i++) {
      ImgProcCallTask* uct = (ImgProcCallTask*)tasks[i];
      uct->img_proc_call = img_proc_call;
      uct->uidx_inc = 1;
      uct->uidx_st = chk_dx;
      if(i == nt-1)
	uct->uidx_ed = chk_dx + last_chnk;
      else
	uct->uidx_ed = chk_dx + chnk_size;
      chk_dx += chnk_size;
    }

    // then run the subsidiary guys
    for(int i=0;i<threads.size;i++) {
      threads[i]->runTask();
    }
    tasks[0]->run();		// run our own set..

    // finally, always need to sync at end to ensure that everyone is done!
    for(int i=0;i<threads.size;i++) {
      threads[i]->sync();
    }
  }
}

///////////////////////////////////////////
//	DoG Filter

void DoGFilterSpec::Initialize() {
  color_chan = BLACK_WHITE;
  filter_width = 8;
  filter_size = filter_width * 2 + 1;
  on_sigma = 2.0f;
  off_sigma = 4.0f;
  circle_edge = true;
//   on_filter.SetGeom(2, filter_size, filter_size);
//   off_filter.SetGeom(2, filter_size, filter_size);
//   net_filter.SetGeom(2, filter_size, filter_size);
}

void DoGFilterSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  filter_size = filter_width * 2 + 1;
  UpdateFilter();
}

float DoGFilterSpec::FilterPoint(int x, int y, float r_val, float g_val, float b_val) {
  if(color_chan == BLACK_WHITE) {
    float grey = r_val + g_val + b_val;
    return grey * net_filter.FastEl(x+filter_width, y+filter_width);
  }
  else if(color_chan == RED_GREEN) {
    return (on_filter.FastEl(x+filter_width, y+filter_width) * r_val - 
	    off_filter.FastEl(x+filter_width, y+filter_width) * g_val);
  }
  else if(color_chan == BLUE_YELLOW) {
    float y_val = .5f * (r_val + g_val);
    return (on_filter.FastEl(x+filter_width, y+filter_width) * b_val - 
	    off_filter.FastEl(x+filter_width, y+filter_width) * y_val);
  }
  return 0.0f;
}

void DoGFilterSpec::RenderFilter(float_Matrix& on_flt, float_Matrix& off_flt,
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

void DoGFilterSpec::UpdateFilter() {
  RenderFilter(on_filter, off_filter, net_filter);
}

void DoGFilterSpec::GraphFilter(DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GraphFilter", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
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

void DoGFilterSpec::GridFilter(DataTable* graph_data, bool reset) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GridFilter", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->ResetData();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, filter_size, filter_size);

  float maxv = taMath_float::vec_max(&on_filter, idx);

  graph_data->SetUserData("N_ROWS", 3);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 2.0f);

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

void GaborFilterSpec::Initialize() {
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
}

float GaborFilterSpec::Eval(float x, float y) {
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

void GaborFilterSpec::RenderFilter(float_Matrix& flt) {
  flt.SetGeom(2, x_size, y_size);
  for(int y=0;y<y_size;y++) {
    for(int x=0;x<x_size;x++) {
      flt.Set(Eval(x, y), x, y);
    }
  }
}

void GaborFilterSpec::UpdateFilter() {
  RenderFilter(filter);
}

float GaborFilterSpec::GetParam(GaborParam param) {
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


void GaborFilterSpec::GraphFilter(DataTable* graph_data) {
  UpdateFilter();
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GraphFilter", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
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

void GaborFilterSpec::GridFilter(DataTable* graph_data, bool reset) {
  UpdateFilter();
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GridFilter", true);
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
  graph_data->SetUserData("BLOCK_HEIGHT", 2.0f);

  graph_data->AddBlankRow();
  matda->SetValAsMatrix(&filter, -1);

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

void GaborFilterSpec::OutputParams(ostream& strm) {
  strm << "ctr: " << ctr_x << ", " << ctr_y << ", angle: " << angle
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

float GaborFitter::ParamDist(const GaborFilterSpec& oth) {
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

////////////////////////////////////////////////////////////////////
//		Retinal Processing (DoG model)


///////////////////////////////////////////////////////////
// 		Retinal Spacing

void RetinalSpacingSpec::Initialize() {
  region = PARAFOVEA;
  res = MED_RES;
  retina_size.x = 321;
  retina_size.y = 241;
  output_units = 0;
}

void RetinalSpacingSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  spacing.x = MAX(spacing.x, 1);
  spacing.y = MAX(spacing.y, 1);
  UpdateSizes();
}

void RetinalSpacingSpec::UpdateSizes() {
  // 0 1 2 3 4 5 6 7 8 	retina_size = 9
  // b b .   .   . b b 	border = 2, spacing = 2: input_size = 5, output_size = 3
  //     sc        ec

  input_size = (retina_size - 2 * border);
  output_size = ((input_size - 1) / spacing) + 1;
  output_units = output_size.x * output_size.y;

//   if((input_size.x - 1) % spacing.x != 0) {
//     taMisc::Warning("Filter:",name,"x spacing:",String(spacing.x),
// 		  "is not even multiple of input size:",String(input_size.x));
//   }
//   if((input_size.y - 1) % spacing.y != 0) {
//     taMisc::Warning("Filter:",name,"y spacing:",String(spacing.y),
// 		  "is not even multiple of input size:",String(input_size.y));
//   }
}

void RetinalSpacingSpec::PlotSpacing(DataTable* graph_data, float val) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_PlotSpacing", true);
  }
  graph_data->StructUpdate(true);
  int idx;
  DataCol* matda = graph_data->FindMakeColName("Spacing", idx, VT_FLOAT, 2,
						      retina_size.x,
						      retina_size.y);
  graph_data->SetUserData("N_ROWS", 1);
  graph_data->SetUserData("BLOCK_HEIGHT", 2.0f);
  graph_data->SetUserData("WIDTH", retina_size.x / retina_size.y);

  if(graph_data->rows < 1)
    graph_data->AddBlankRow();

  float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
  if(mat) {
    TwoDCoord ic;
    int x,y;
    for(y=border.y; y<= retina_size.y-border.y; y+= spacing.y) {
      for(x=border.x; x<= retina_size.x-border.x; x+=spacing.x) {
	ic.y = y; ic.x = x;
	ic.WrapClip(true, retina_size);
	mat->FastEl(x,y) += val;
      }
    }
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

///////////////////////////////////////////////////////////
// 		DoG + Retinal Spacing

void DoGRetinaSpec::Initialize() {
  dog.name = name;
  spacing.name = name;
}

void DoGRetinaSpec::UpdateAfterEdit_impl() {
  dog.name = name;
  spacing.name = name;
  dog.UpdateAfterEdit();
  spacing.UpdateAfterEdit();
}

void DoGRetinaSpec::GraphFilter(DataTable* graph_data) {
  dog.GraphFilter(graph_data);
}

void DoGRetinaSpec::GridFilter(DataTable* graph_data) {
  dog.GridFilter(graph_data);
}

void DoGRetinaSpec::PlotSpacing(DataTable* graph_data, float val) {
  spacing.PlotSpacing(graph_data, val);
}

bool DoGRetinaSpec::FilterRetina(float_Matrix& on_output, float_Matrix& off_output,
				 float_Matrix& retina_img, bool superimpose,
				 EdgeMode edge) {

  TwoDCoord img_size = spacing.retina_size;

  if((retina_img.dim(0) != img_size.x) || (retina_img.dim(1) != img_size.y)) {
    taMisc::Error("DoGFilterImage: retina_img is not appropriate size!");
    return false;
  }

  if((on_output.dim(0) != spacing.output_size.x) || 
     (on_output.dim(1) != spacing.output_size.y)) {
    taMisc::Error("DoGFilterImage: on_output is not appropriate size: spacing.output_size!");
    return false;
  }
  if((off_output.dim(0) != spacing.output_size.x) || 
     (off_output.dim(1) != spacing.output_size.y)) {
    taMisc::Error("DoGFilterImage: off_output is not appropriate size: spacing.output_size!");
    return false;
  }

  float_Matrix* on_out = &on_output;
  float_Matrix* off_out = &off_output;
  float_Matrix tmp_on_out(false);  taBase::Ref(tmp_on_out);
  float_Matrix tmp_off_out(false); taBase::Ref(tmp_off_out);

  if(superimpose) {
    tmp_on_out.SetGeom(2, spacing.output_size.x, spacing.output_size.y);
    tmp_off_out.SetGeom(2, spacing.output_size.x, spacing.output_size.y);
    tmp_on_out.InitVals(0.0f);
    tmp_off_out.InitVals(0.0f);
    on_out = &tmp_on_out;
    off_out = &tmp_off_out;
  }
  else {
    on_output.InitVals(0.0f);
    off_output.InitVals(0.0f);
  }

  bool rgb_img = false;
  if(retina_img.dims() == 3) rgb_img = true;

  TwoDCoord st = spacing.border;

  bool wrap = (edge == WRAP);

  TwoDCoord icc;
  TwoDCoord ic;
  for(int yo = 0; yo < spacing.output_size.y; yo++) {
    icc.y = st.y + yo * spacing.spacing.y; 
    for(int xo = 0; xo < spacing.output_size.x; xo++) {
      icc.x = st.x + xo * spacing.spacing.x; 
      // now convolve with dog filter
      float cnvl = 0.0f;
      for(int yf = -dog.filter_width; yf <= dog.filter_width; yf++) {
	ic.y = icc.y + yf;
	for(int xf = -dog.filter_width; xf <= dog.filter_width; xf++) {
	  ic.x = icc.x + xf;
	  if(ic.WrapClip(wrap, img_size)) {
	    if(edge == CLIP) continue; // bail on clipping only
	  }
	  if(rgb_img) {
	    cnvl += dog.FilterPoint(xf, yf, retina_img.FastEl(ic.x, ic.y,0),
					 retina_img.FastEl(ic.x, ic.y, 1),
					 retina_img.FastEl(ic.x, ic.y, 2));
	  }
	  else {
	    float gval = retina_img.FastEl(ic.x, ic.y);
	    cnvl += dog.FilterPoint(xf, yf, gval, gval, gval);
	  }
	}
      }
      if(cnvl > 0.0f)
	on_out->FastEl(xo, yo) = cnvl;
      else 
	off_out->FastEl(xo, yo) = -cnvl;
    }
  }

  // note: used to do separate normalization of on/off channels here..
  // this is important for superimpose, but it is not good for full
  // color filtering, because things get artificially strong..
  // to do superimpose, only renorm after the final superimpose!!
  
  if(superimpose) {			// add them back in!
    taMath_float::vec_add(&on_output, on_out);
    taMath_float::vec_add(&off_output, off_out);
  }
  return true;
}

DoGRetinaSpec* DoGRetinaSpecList::FindRetinalRegion(RetinalSpacingSpec::Region reg) {
  for(int i=0;i<size;i++) {
    DoGRetinaSpec* fs = (DoGRetinaSpec*)FastEl(i);
    if(fs->spacing.region == reg)
      return fs;
  }
  return NULL;
}

DoGRetinaSpec* DoGRetinaSpecList::FindRetinalRes(RetinalSpacingSpec::Resolution res) {
  for(int i=0;i<size;i++) {
    DoGRetinaSpec* fs = (DoGRetinaSpec*)FastEl(i);
    if(fs->spacing.res == res)
      return fs;
  }
  return NULL;
}

DoGRetinaSpec* DoGRetinaSpecList::FindRetinalRegionRes(RetinalSpacingSpec::Region reg,
						       RetinalSpacingSpec::Resolution res) {
  for(int i=0;i<size;i++) {
    DoGRetinaSpec* fs = (DoGRetinaSpec*)FastEl(i);
    if((fs->spacing.region == reg) && (fs->spacing.res == res))
      return fs;
  }
  DoGRetinaSpec* rval = FindRetinalRes(res);
  if(rval) return rval;
  rval = FindRetinalRegion(reg);
  if(rval) return rval;
  return NULL;
}

///////////////////////////////////////////////////////////
// 		GaborV1SpecBase

void GaborRFSpec::Initialize() {
  n_angles = 4;
  freq = 1.5f;
  width = 2.0f;
  length = 2.0f;
  amp = .9f;
}

void BlobRFSpec::Initialize() {
  n_sizes = 1;
  width_st = 1.0;
  width_inc = 2.0;
}

void GaborV1SpecBase::Initialize() {
  gabor_specs.SetBaseType(&TA_GaborFilterSpec);
  blob_specs.SetBaseType(&TA_DoGFilterSpec);
  rf_ovlp = 2;
  rf_width = rf_ovlp * 2;
  filter_type = GABOR;
  n_filters = gabor_rf.n_angles * 2;
}

void GaborV1SpecBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(filter_type != COPY) {
    rf_ovlp = rf_width / 2;
    rf_width = rf_ovlp * 2;	// ensure even
  }
  rf_ovlp.SetGtEq(1);		// min 1.. if rf_width = 1, then just moves along
  InitFilters();
}

bool GaborV1SpecBase::InitFilters() {
  if(filter_type == GABOR)
    return InitFilters_Gabor();
  else if(filter_type == BLOB)
    return InitFilters_Blob();
  else // COPY
    return InitFilters_Copy();
}

bool GaborV1SpecBase::InitFilters_Gabor() {
  n_filters = gabor_rf.n_angles * 2;
  gabor_specs.SetSize(n_filters);

  for(int i=0;i<n_filters;i++) {
    float angle_dx = (float)(i % gabor_rf.n_angles); // angle is inner dim
    float phase_dx = (float)((i / gabor_rf.n_angles) % 2); // then phase

    GaborFilterSpec* gf = (GaborFilterSpec*)gabor_specs[i];

    gf->x_size = rf_width.x;
    gf->y_size = rf_width.y;
    gf->ctr_x = (float)(gf->x_size - 1.0f) / 2.0f;
    gf->ctr_y = (float)(gf->y_size - 1.0f) / 2.0f;
    gf->angle = taMath_float::pi * angle_dx / (float)gabor_rf.n_angles;
    gf->phase = taMath_float::pi * phase_dx;
    gf->freq = gabor_rf.freq;
    gf->length = gabor_rf.length;
    gf->width = gabor_rf.width;
    gf->amp = gabor_rf.amp;
    gf->UpdateFilter();
  }
  return true;
}

bool GaborV1SpecBase::InitFilters_Blob() {
  // 2 for phase and 2 for color channel -- blobs are only relevant for color channels!
  n_filters = blob_rf.n_sizes * 4;
  blob_specs.SetSize(n_filters);

  for(int i=0;i<n_filters;i++) {
    float sz_dx = (float)(i % blob_rf.n_sizes); // size is inner dim
    int phase_dx = ((i / blob_rf.n_sizes) % 2); // then phase
    int clr_dx = (i / (blob_rf.n_sizes * 2) % 2); // then color

    DoGFilterSpec* df = (DoGFilterSpec*)blob_specs[i];

    df->filter_width = (rf_width.x) / 2; // assume x-y symmetric!
    df->filter_size = df->filter_width * 2 + 1;
    df->circle_edge = true;
    df->color_chan = (DoGFilterSpec::ColorChannel)(DoGFilterSpec::RED_GREEN + clr_dx);
    if(phase_dx == 0) {
      df->on_sigma = blob_rf.width_st + blob_rf.width_inc * sz_dx;
      df->off_sigma = 2.0f * df->on_sigma;
      df->UpdateFilter();
      float sc_fact = taMath_float::vec_norm_max(&df->on_filter); // on is most active
      for(int i=0;i<df->off_filter.size;i++)
	df->off_filter.FastEl_Flat(i) *= sc_fact;
    }
    else {
      df->off_sigma = blob_rf.width_st + blob_rf.width_inc * sz_dx;
      df->on_sigma = 2.0f * df->off_sigma;
      df->UpdateFilter();
      float sc_fact = taMath_float::vec_norm_max(&df->off_filter); // off is most active
      for(int i=0;i<df->on_filter.size;i++)
	df->on_filter.FastEl_Flat(i) *= sc_fact;
    }
  }
  return true;
}

bool GaborV1SpecBase::InitFilters_Copy() {
  return true;			// nothing to do
}

void GaborV1SpecBase::GraphFilter(DataTable* graph_data, int unit_no) {
  if(filter_type == GABOR) {
    GaborFilterSpec* gf = (GaborFilterSpec*)gabor_specs.SafeEl(unit_no);
    if(gf)
      gf->GraphFilter(graph_data);
  }
  else if(filter_type == BLOB) {
    DoGFilterSpec* df = (DoGFilterSpec*)blob_specs.SafeEl(unit_no);
    if(df)
      df->GraphFilter(graph_data);
  }
  else { // COPY
//     DoGFilterSpec* df = (DoGFilterSpec*)blob_specs.SafeEl(unit_no);
//     if(df)
//       df->GraphFilter(graph_data);
  }
}

void GaborV1SpecBase::GridFilter(DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GridFilter", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();

  if(filter_type == GABOR) {
    for(int i=0;i<gabor_specs.size;i++) {
      GaborFilterSpec* gf = (GaborFilterSpec*)gabor_specs.SafeEl(i);
      gf->GridFilter(graph_data, false); // don't reset!
    }
  }
  else if(filter_type == BLOB) {
    for(int i=0;i<blob_specs.size;i++) {
      DoGFilterSpec* df = (DoGFilterSpec*)blob_specs.SafeEl(i);
      df->GridFilter(graph_data, false); // don't reset!
    }
  }
  else {			// COPY
//     for(int i=0;i<blob_specs.size;i++) {
//       DoGFilterSpec* df = (DoGFilterSpec*)blob_specs.SafeEl(i);
//       df->GridFilter(graph_data, false); // don't reset!
//     }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

///////////////////////////////////////////////////////////
// 		GaborV1Spec

void GaborV1Spec::Initialize() {
  region = RetinalSpacingSpec::FOVEA;
  res = RetinalSpacingSpec::MED_RES;
  un_geom.SetXY(4,4);
  gp_geom.SetXY(8,8);
  wrap = true;
  n_filter_gps.SetXY(3,3);
  n_filters_per_gp = 3;
  gp_gauss_sigma = 0.5f;
  threads.min_units = 1;
  UpdateGeoms();
}

void GaborV1Spec::UpdateGeoms() {
  if(filter_type == COPY) {
    rf_ovlp.SetGtEq(1);		// min 1.. if rf_width = 1, then just moves along
    if(wrap)
      trg_input_size = gp_geom * input_ovlp;
    else
      trg_input_size = (gp_geom -1)* input_ovlp;
    n_filters = 0;
    n_filter_gps = 1;
    n_filters_per_gp = 1;
    tot_filter_gps = 1;
    filter_gp_ovlp = 0;
    input_ovlp = rf_ovlp;
    if(wrap)
      trg_input_size = un_geom * input_ovlp;
    else
      trg_input_size = (un_geom -1)* input_ovlp;
  }
  else {
    rf_ovlp = rf_width / 2;
    rf_ovlp.SetGtEq(1);		// min 1.. if rf_width = 1, then just moves along
    rf_width = rf_ovlp * 2;	// ensure even
    tot_filter_gps = n_filter_gps * n_filters_per_gp;
    filter_gp_ovlp = tot_filter_gps / 2;
    filter_gp_ovlp.SetGtEq(1);
    input_ovlp = filter_gp_ovlp * rf_ovlp;
    if(wrap)
      trg_input_size = gp_geom * input_ovlp;
    else
      trg_input_size = (gp_geom -1)* input_ovlp;
  }

  rf_ovlp.UpdateAfterEdit();
  rf_width.UpdateAfterEdit();
  tot_filter_gps.UpdateAfterEdit();
  filter_gp_ovlp.UpdateAfterEdit();
  input_ovlp.UpdateAfterEdit();
  trg_input_size.UpdateAfterEdit();

  if(filter_type != COPY) {
    gp_gauss_mat.SetGeom(2, tot_filter_gps.x, tot_filter_gps.y);

    if(tot_filter_gps.n > 1) {
      float ctr_x = (float)tot_filter_gps.x * .5f;;
      float ctr_y = (float)tot_filter_gps.y * .5f;;
      float eff_sig_x = gp_gauss_sigma * ctr_x;
      float eff_sig_y = gp_gauss_sigma * ctr_y;
      for(int yi=0; yi< tot_filter_gps.y; yi++) {
	float y = ((float)yi - ctr_y) / eff_sig_y;
	for(int xi=0; xi< tot_filter_gps.x; xi++) {
	  float x = ((float)xi - ctr_y) / eff_sig_x;
	  float gv = expf(-(x*x + y*y)/2.0f);
	  gp_gauss_mat.FastEl(xi, yi) = gv;
	}
      }
    }
    else {
      gp_gauss_mat.FastEl(0,0) = 1.0f;
    }
  }
}

void GaborV1Spec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  un_geom.UpdateAfterEdit();
  gp_geom.UpdateAfterEdit();
  UpdateGeoms();
  threads.InitAll();
}

bool GaborV1Spec::SetGpGeomFmInputSize(TwoDCoord& input_size) {
  UpdateGeoms();
  if(filter_type == COPY) {
    if(wrap) {
      un_geom = (input_size / input_ovlp);
    }
    else {
      un_geom = (input_size / input_ovlp) + 1;
    }
  }
  else {
    if(wrap) {
      gp_geom = (input_size / input_ovlp);
    }
    else {
      gp_geom = (input_size / input_ovlp) + 1;
    }
  }
  UpdateGeoms();
  return input_size == trg_input_size;
}

bool GaborV1Spec::SetGpGeomFmRetSpec(DoGRetinaSpecList& dogs) {
  DoGRetinaSpec* rs = dogs.FindRetinalRegionRes(region, res);
  if(!rs) return false;
  return SetGpGeomFmInputSize(rs->spacing.output_size);
}

bool GaborV1Spec::InitFilters_Gabor() {
  int n_filters = gabor_rf.n_angles * 2 * n_filters_per_gp;
  un_geom.FitN(n_filters);
  return inherited::InitFilters_Gabor();
}

bool GaborV1Spec::InitFilters_Blob() {
  // 2 for phase and 2 for color channel -- blobs are only relevant for color channels!
  int n_filters = blob_rf.n_sizes * 4 * n_filters_per_gp;
  un_geom.FitN(n_filters);
  return inherited::InitFilters_Gabor();
}

bool GaborV1Spec::FilterInput(float_Matrix& v1_output, DoGFilterSpec::ColorChannel c_chan,
			      float_Matrix& on_input, float_Matrix& off_input,
			      bool superimpose) {
  TwoDCoord input_size;
  input_size.x = on_input.dim(0);
  input_size.y = on_input.dim(1);

  if(input_size != trg_input_size) {
    taMisc::Error("GaborV1Spec: input size",input_size.GetStr(),
		  "is not correct size, should be:", trg_input_size.GetStr());
    return false;
  }

  if(filter_type == COPY) {
    v1_output.SetGeom(2, un_geom.x, un_geom.y);
  }
  else {
    v1_output.SetGeom(4, un_geom.x, un_geom.y, gp_geom.x, gp_geom.y);
  }
  if(!superimpose)
    v1_output.InitVals();		// reset all vals to 0

  cur_v1_output = &v1_output;
  cur_c_chan = c_chan;
  cur_on_input = &on_input;
  cur_off_input = &off_input;
  cur_superimpose = cur_superimpose;

  threads.InitAll();
  threads.min_units = 1;
  ThreadImgProcCall ip_call(&ImgProcThreadBase::Filter_Thread);
  threads.Run(&ip_call, un_geom.n);
  return true;
}

void GaborV1Spec::Filter_Thread(int cmp_idx, int thread_no) {
  if(filter_type == GABOR) {
    FilterInput_Gabor(cmp_idx);
  }
  else if(filter_type == BLOB) {
    FilterInput_Blob(cmp_idx);
  }
  else { // COPY
    FilterInput_Copy(cmp_idx);
  }
}

bool GaborV1Spec::FilterInput_Gabor(int cmp_idx) {
  TwoDCoord un;			// units within group
  TwoDCoord ugp;		// unit groups
  TwoDCoord fgp;		// filter groups
  TwoDCoord ugpof;		// offset from ugps
  TwoDCoord fgpof;		// offset from fgps
  TwoDCoord fc;			// filter coords
  TwoDCoord in;			// input coords
  int uidx = cmp_idx;
  un.y = uidx / un_geom.x;
  un.x = uidx % un_geom.x;
  int fidx = uidx / n_filters_per_gp;
  int fgpdx = uidx % n_filters_per_gp;
  GaborFilterSpec* gf = (GaborFilterSpec*)gabor_specs.SafeEl(fidx);
  if(!gf) return false;			     // shouldn't happen
  // for each unit, process entire input:
  for(ugp.y=0;ugp.y<gp_geom.y;ugp.y++) {
    for(ugp.x=0;ugp.x<gp_geom.x;ugp.x++) {
      if(wrap)
	ugpof = (ugp-1) * input_ovlp;
      else
	ugpof = ugp * input_ovlp;
      float max_val = 0.0f;	// output is max val
      // filter groups
      for(fgp.y=0;fgp.y<tot_filter_gps.y;fgp.y++) {
	int ymod = fgp.y % n_filters_per_gp;
	for(fgp.x=0;fgp.x<tot_filter_gps.x;fgp.x++) {
	  int xmod = (fgp.x + ymod) % n_filters_per_gp;
	  if(xmod != fgpdx) continue; // not our spot
	  float gmult = gp_gauss_mat.FastEl(fgp.x, fgp.y);
	  fgpof = ugpof + (fgp * rf_ovlp);
	  // now actually apply the filter itself
	  float flt_sum = 0.0f;
	  for(fc.y=0;fc.y<rf_width.y;fc.y++) {
	    for(fc.x=0;fc.x<rf_width.x;fc.x++) {
	      in = fgpof + fc;
	      if(in.WrapClip(wrap, trg_input_size)) continue;
	      float fval = gf->filter.FastEl(fc.x, fc.y);
	      float oval;
	      if(fval > 0.0f) oval = fval * cur_on_input->FastEl(in.x, in.y);
	      else	      oval = -fval * cur_off_input->FastEl(in.x, in.y);
	      flt_sum += oval;
	    }
	  }
	  flt_sum *= gmult;
	  max_val = MAX(max_val, flt_sum);
	}
      }
      if(cur_superimpose)
	cur_v1_output->FastEl(un.x, un.y, ugp.x, ugp.y) += max_val;
      else
	cur_v1_output->FastEl(un.x, un.y, ugp.x, ugp.y) = max_val;
    }
  }
  return true;
}

bool GaborV1Spec::FilterInput_Blob(int cmp_idx) {
  TwoDCoord un;			// units within group
  TwoDCoord ugp;		// unit groups
  TwoDCoord fgp;		// filter groups
  TwoDCoord ugpof;		// offset from ugps
  TwoDCoord fgpof;		// offset from fgps
  TwoDCoord fc;			// filter coords
  TwoDCoord in;			// input coords
  int uidx = cmp_idx;
  un.y = uidx / un_geom.x;
  un.x = uidx % un_geom.x;
  int fidx = uidx / n_filters_per_gp;
  int fgpdx = uidx % n_filters_per_gp;
  DoGFilterSpec* gf = (DoGFilterSpec*)blob_specs.SafeEl(fidx);
  if(!gf) return false;			     // shouldn't happen
  if(gf->color_chan != cur_c_chan) return false; // doesn't match
  // for each unit, process entire input:
  for(ugp.y=0;ugp.y<gp_geom.y;ugp.y++) {
    for(ugp.x=0;ugp.x<gp_geom.x;ugp.x++) {
      if(wrap)
	ugpof = (ugp-1) * input_ovlp;
      else
	ugpof = ugp * input_ovlp;
      float max_val = 0.0f;	// result is max over locs
      // filter groups
      for(fgp.y=0;fgp.y<tot_filter_gps.y;fgp.y++) {
	int ymod = fgp.y % n_filters_per_gp;
	for(fgp.x=0;fgp.x<tot_filter_gps.x;fgp.x++) {
	  int xmod = (fgp.x + ymod) % n_filters_per_gp;
	  if(xmod != fgpdx) continue; // not our spot
	  float gmult = gp_gauss_mat.FastEl(fgp.x, fgp.y);
	  fgpof = ugpof + (fgp * rf_ovlp);
	  // now actually apply the filter itself
	  float flt_sum = 0.0f;
	  for(fc.y=0;fc.y<rf_width.y;fc.y++) {
	    for(fc.x=0;fc.x<rf_width.x;fc.x++) {
	      in = fgpof + fc;
	      if(in.WrapClip(wrap, trg_input_size)) continue;
	      float oval = (gf->on_filter.FastEl(fc.x, fc.y) * cur_on_input->FastEl(in.x, in.y) - 
			    gf->off_filter.FastEl(fc.x, fc.y) * cur_off_input->FastEl(in.x, in.y));
	      flt_sum += oval;
	    }
	  }
	  flt_sum *= gmult;
	  max_val = MAX(max_val, flt_sum);
	}
      }
      if(cur_superimpose)
	cur_v1_output->FastEl(un.x, un.y, ugp.x, ugp.y) += max_val;
      else
	cur_v1_output->FastEl(un.x, un.y, ugp.x, ugp.y) = max_val;
    }
  }
  return true;
}

bool GaborV1Spec::FilterInput_Copy(int cmp_idx) {
  TwoDCoord un;			// units within group
  TwoDCoord inof;		// offset to input layer from units
  TwoDCoord in;			// input coords
  TwoDCoord fc;			// "filter" coords
  int uidx = cmp_idx;
  un.y = uidx / un_geom.x;
  un.x = uidx % un_geom.x;
  if(wrap && rf_width.x > 1)	// only if has some width does this make sense
    inof = (un-1) * input_ovlp;
  else
    inof = un * input_ovlp;

  float flt_sum = 0.0f;
  for(fc.y=0;fc.y<rf_width.y;fc.y++) {
    for(fc.x=0;fc.x<rf_width.x;fc.x++) {
      in = inof + fc;
      if(in.WrapClip(wrap, trg_input_size)) continue;
      float oval = cur_on_input->FastEl(in.x, in.y) + cur_off_input->FastEl(in.x, in.y);
      flt_sum += oval;
    }
  }

  if(cur_superimpose)
    cur_v1_output->FastEl(un.x, un.y) += flt_sum;
  else
    cur_v1_output->FastEl(un.x, un.y) = flt_sum;

  return true;
}

void GaborV1Spec::GridFilterInput(DataTable* graph_data, int unit_no, int gp_skip, bool ctrs_only) {
  if(filter_type == COPY) return; // not supported

  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GridFilterInput", true);
  }
  graph_data->StructUpdate(true);
  int idx;
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2,
					       trg_input_size.x, trg_input_size.y);
  graph_data->EnforceRows(1);
  float_Matrix* gmat = (float_Matrix*)matda->GetValAsMatrix(0);
  taBase::Ref(gmat);
  gmat->InitVals();

  graph_data->SetUserData("N_ROWS", 1);
//   graph_data->SetUserData("SCALE_MIN", -maxv);
//   graph_data->SetUserData("SCALE_MAX", maxv);
//   graph_data->SetUserData("BLOCK_HEIGHT", 2.0f);

  TwoDCoord ugp;		// unit groups
  TwoDCoord fgp;		// filter groups
  TwoDCoord ugpof;		// offset from ugps
  TwoDCoord fgpof;		// offset from fgps
  TwoDCoord fc;			// filter coords
  TwoDCoord in;			// input coords

  int uidx = unit_no;
  int fidx = uidx / n_filters_per_gp;
  int fgpdx = uidx % n_filters_per_gp;
  DoGFilterSpec* dgf = NULL;
  GaborFilterSpec* ggf = NULL;
  if(filter_type == GABOR)
    ggf = (GaborFilterSpec*)gabor_specs.SafeEl(fidx);
  else if(filter_type == BLOB)
    dgf = (DoGFilterSpec*)blob_specs.SafeEl(fidx);
  // nothing for COPY
  // for each unit, process entire input:
  for(ugp.y=0;ugp.y<gp_geom.y;ugp.y+= gp_skip) {
    for(ugp.x=0;ugp.x<gp_geom.x;ugp.x+= gp_skip) {
      if(wrap)
	ugpof = (ugp-1) * input_ovlp;
      else
	ugpof = ugp * input_ovlp;
      // filter groups
      for(fgp.y=0;fgp.y<tot_filter_gps.y;fgp.y++) {
	int ymod = fgp.y % n_filters_per_gp;
	for(fgp.x=0;fgp.x<tot_filter_gps.x;fgp.x++) {
	  int xmod = (fgp.x + ymod) % n_filters_per_gp;
	  if(xmod != fgpdx) continue; // not our spot
	  float gmult = gp_gauss_mat.FastEl(fgp.x, fgp.y);
	  fgpof = ugpof + (fgp * rf_ovlp);
	  // now actually apply the filter itself
	  for(fc.y=0;fc.y<rf_width.y;fc.y++) {
	    for(fc.x=0;fc.x<rf_width.x;fc.x++) {
	      in = fgpof + fc;
	      if(in.WrapClip(wrap, trg_input_size)) continue;
	      float fval;
	      if(filter_type == GABOR) {
		fval = ggf->filter.FastEl(fc.x, fc.y);
	      }
	      else if(filter_type == BLOB) {
		fval = (dgf->on_filter.FastEl(fc.x, fc.y) - 
			dgf->off_filter.FastEl(fc.x, fc.y));
	      }
	      else {		// COPY
		fval = 1.0f;
	      }
	      if(ctrs_only) {
		if(fc == rf_ovlp) fval = 1.0f;
		else fval = 0.0f;
	      }
	      gmat->FastEl(in.x, in.y) += gmult * fval;
	    }
	  }
	}
      }
    }
  }
  taBase::unRefDone(gmat);
  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

bool GaborV1SpecList::UpdateSizesFmRetina(DoGRetinaSpecList& dogs) {
  bool rval = true;
  for(int i=0;i<size;i++) {
    GaborV1Spec* sp = FastEl(i);
    bool rv = sp->SetGpGeomFmRetSpec(dogs);
    if(!rv)
      rval = false;
  }
  return rval;
}

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


bool taImageProc::DoGFilterRetina(float_Matrix& on_output, float_Matrix& off_output,
				  float_Matrix& retina_img, DoGRetinaSpec& spec,
				  bool superimpose, EdgeMode edge) {
  return spec.FilterRetina(on_output, off_output, retina_img, superimpose,
			   (DoGRetinaSpec::EdgeMode)edge);
}

bool taImageProc::GaborFilterV1(float_Matrix& v1_output, DoGFilterSpec::ColorChannel c_chan,
				float_Matrix& on_input, float_Matrix& off_input,
				GaborV1Spec& spec, bool superimpose, EdgeMode edge) {
  return spec.FilterInput(v1_output, c_chan, on_input, off_input, superimpose);
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

//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// 		Full Retinal Spec

void RetinaSpec::Initialize() {
  color_type = MONOCHROME;
  edge_mode = taImageProc::BORDER;
  fade_width = -1;
  renorm_thr = 0.00001f;
  retina_size.x = 321; retina_size.y = 241;
  threads.min_units = 1;
}

void RetinaSpec::UpdateRetinaSize() {
  for(int i=0;i<dogs.size; i++) {
    DoGRetinaSpec* sp = dogs[i];
    sp->spacing.retina_size = retina_size;
    sp->spacing.UpdateAfterEdit();
  }
}

void RetinaSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateRetinaSize();
  threads.InitAll();
}

void RetinaSpec::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  dogs.CheckConfig(quiet, rval);
}

void RetinaSpec::DefaultFilters() {
  StructUpdate(false);
  if(color_type == COLOR)
    dogs.SetSize(5);
  else
    dogs.SetSize(3);
  UpdateRetinaSize();
  DoGRetinaSpec* sp;
  int cnt = 0;
  if(color_type == MONOCHROME) {
    sp = dogs[cnt++];
    sp->name = "high_freq_bw";
    sp->dog.color_chan = DoGFilterSpec::BLACK_WHITE;
    sp->dog.filter_width = 8;
    sp->dog.on_sigma = 2;
    sp->dog.off_sigma = 4;
    sp->spacing.region = RetinalSpacingSpec::FOVEA;
    sp->spacing.res = RetinalSpacingSpec::HI_RES;
    sp->spacing.border.x = 0; sp->spacing.border.y = 0;
    sp->spacing.spacing.x = 2; sp->spacing.spacing.y = 2;
    sp->UpdateAfterEdit();

    sp = dogs[cnt++];
    sp->name = "med_freq_bw";
    sp->dog.color_chan = DoGFilterSpec::BLACK_WHITE;
    sp->dog.filter_width = 16;
    sp->dog.on_sigma = 4;
    sp->dog.off_sigma = 8;
    sp->spacing.region = RetinalSpacingSpec::FOVEA;
    sp->spacing.res = RetinalSpacingSpec::MED_RES;
    sp->spacing.border.x = 6; sp->spacing.border.y = 14;
    sp->spacing.spacing.x = 4; sp->spacing.spacing.y = 4;
    sp->UpdateAfterEdit();

    sp = dogs[cnt++];
    sp->name = "low_freq_bw";
    sp->dog.color_chan = DoGFilterSpec::BLACK_WHITE;
    sp->dog.filter_width = 32;
    sp->dog.on_sigma = 8;
    sp->dog.off_sigma = 16;
    sp->spacing.region = RetinalSpacingSpec::PARAFOVEA;
    sp->spacing.res = RetinalSpacingSpec::LOW_RES;
    sp->spacing.border.x = 8; sp->spacing.border.y = 16;
    sp->spacing.spacing.x = 8; sp->spacing.spacing.y = 8;
    sp->UpdateAfterEdit();
  }
  else if(color_type == COLOR) {
    sp = dogs[cnt++];
    sp->name = "high_freq_rg";
    sp->dog.color_chan = DoGFilterSpec::RED_GREEN;
    sp->dog.filter_width = 8;
    sp->dog.on_sigma = 2;
    sp->dog.off_sigma = 4;
    sp->spacing.region = RetinalSpacingSpec::FOVEA;
    sp->spacing.res = RetinalSpacingSpec::HI_RES;
    sp->spacing.border.x = 109; sp->spacing.border.y = 85;
    sp->spacing.spacing.x = 2; sp->spacing.spacing.y = 2;
    sp->UpdateAfterEdit();

    sp = dogs[cnt++];
    sp->name = "high_freq_by";
    sp->dog.color_chan = DoGFilterSpec::BLUE_YELLOW;
    sp->dog.filter_width = 8;
    sp->dog.on_sigma = 2;
    sp->dog.off_sigma = 4;
    sp->spacing.region = RetinalSpacingSpec::FOVEA;
    sp->spacing.res = RetinalSpacingSpec::HI_RES;
    sp->spacing.border.x = 109; sp->spacing.border.y = 85;
    sp->spacing.spacing.x = 2; sp->spacing.spacing.y = 2;
    sp->UpdateAfterEdit();

    sp = dogs[cnt++];
    sp->name = "med_freq_rg";
    sp->dog.color_chan = DoGFilterSpec::RED_GREEN;
    sp->dog.filter_width = 16;
    sp->dog.on_sigma = 4;
    sp->dog.off_sigma = 8;
    sp->spacing.region = RetinalSpacingSpec::PARAFOVEA;
    sp->spacing.res = RetinalSpacingSpec::MED_RES;
    sp->spacing.border.x = 6; sp->spacing.border.y = 14;
    sp->spacing.spacing.x = 4; sp->spacing.spacing.y = 4;
    sp->UpdateAfterEdit();

    sp = dogs[cnt++];
    sp->name = "med_freq_by";
    sp->dog.color_chan = DoGFilterSpec::BLUE_YELLOW;
    sp->dog.filter_width = 16;
    sp->dog.on_sigma = 4;
    sp->dog.off_sigma = 8;
    sp->spacing.region = RetinalSpacingSpec::PARAFOVEA;
    sp->spacing.res = RetinalSpacingSpec::MED_RES;
    sp->spacing.border.x = 6; sp->spacing.border.y = 14;
    sp->spacing.spacing.x = 4; sp->spacing.spacing.y = 4;
    sp->UpdateAfterEdit();

    sp = dogs[cnt++];
    sp->name = "low_freq_bw";
    sp->dog.color_chan = DoGFilterSpec::BLACK_WHITE;
    sp->dog.filter_width = 32;
    sp->dog.on_sigma = 8;
    sp->dog.off_sigma = 16;
    sp->spacing.region = RetinalSpacingSpec::PARAFOVEA;
    sp->spacing.res = RetinalSpacingSpec::LOW_RES;
    sp->spacing.border.x = 8; sp->spacing.border.y = 16;
    sp->spacing.spacing.x = 8; sp->spacing.spacing.y = 8;
    sp->UpdateAfterEdit();
  }
  StructUpdate(true);
}

void RetinaSpec::ConfigDataTable(DataTable* dt, bool reset_cols) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!dt) {
    DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("InputData");
    dt = dgp->NewEl(1, &TA_DataTable);
    if(!name.empty())
      dt->name = name + "_InputData";
    else
      dt->name = "RetinaSpec_InputData";
  }
  dt->ResetData();
  if(reset_cols) dt->Reset();
  dt->StructUpdate(true);
  int idx =0;
  dt->FindMakeColName("Name", idx, DataTable::VT_STRING, 0);
  dt->FindMakeColName("LookBox", idx, DataTable::VT_FLOAT, 1, 4);
  dt->FindMakeColName("ImageSize", idx, DataTable::VT_FLOAT, 1, 2);
  dt->FindMakeColName("Move", idx, DataTable::VT_FLOAT, 1, 2);
  dt->FindMakeColName("Scale", idx, DataTable::VT_FLOAT, 0);
  dt->FindMakeColName("Rotate", idx, DataTable::VT_FLOAT, 0);
  DataCol* col;
  if(color_type == COLOR)
    col = dt->FindMakeColName("RetinaImage", idx, DataTable::VT_FLOAT, 3,
			retina_size.x, retina_size.y, 3);
  else
    col = dt->FindMakeColName("RetinaImage", idx, DataTable::VT_FLOAT, 2,
			retina_size.x, retina_size.y);
  col->SetUserData("IMAGE", true);
  for(int i=0;i<dogs.size; i++) {
    DoGRetinaSpec* sp = dogs[i];
    dt->FindMakeColName(sp->name + "_on", idx, DataTable::VT_FLOAT, 2,
				sp->spacing.output_size.x, sp->spacing.output_size.y);
    dt->FindMakeColName(sp->name + "_off", idx, DataTable::VT_FLOAT, 2,
				sp->spacing.output_size.x, sp->spacing.output_size.y);
  }
  dt->StructUpdate(false);
}

void RetinaSpec::PlotSpacing(DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_PlotSpacing", true);
  }

  graph_data->StructUpdate(true);
  for(int i=0;i<dogs.size; i++) {
    DoGRetinaSpec* sp = dogs[i];
    float val = (float)i / (float)(dogs.size * 2);
    sp->PlotSpacing(graph_data, val);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

///////////////////////////////////////////////////////////////////////
// Basic functions operating on float image data: transform image, apply dog filters


int RetinaSpec::EffFadeWidth() {
  if(fade_width >= 0) return fade_width;
  int max_off_width = 4;	// use for fading edges
  for(int i=0;i<dogs.size;i++) {
    DoGRetinaSpec* sp = dogs[i];
    max_off_width = MAX(max_off_width, (int)sp->dog.off_sigma);
  }
  return max_off_width;
}

DataCol* RetinaSpec::GetRetImageColumn(DataTable* dt) {
  int idx;
  DataCol* da_ret;
  if(color_type == COLOR)
    da_ret = dt->FindMakeColName("RetinaImage", idx, DataTable::VT_FLOAT, 3,
				 retina_size.x, retina_size.y, 3);
  else
    da_ret = dt->FindMakeColName("RetinaImage", idx, DataTable::VT_FLOAT, 2,
				 retina_size.x, retina_size.y);
  return da_ret;
}

bool RetinaSpec::TransformImageData(float_Matrix& img_data, DataTable* dt,
				    float move_x, float move_y,
				    float scale, float rotate,
				    bool superimpose)
{
  if(dogs.size == 0) return false;
  if (!dt) return false;
  dt->EnforceRows(1);
  dt->WriteItem(0);
  dt->ReadItem(0);

  int eff_fd_wd = 0;
  if(edge_mode == taImageProc::BORDER) eff_fd_wd = EffFadeWidth();

  TwoDCoord img_size(img_data.dim(0), img_data.dim(1));

  float ctr_x = .5f + .5 * move_x;
  float ctr_y = .5f + .5 * move_y;

  DataCol* da_ret = GetRetImageColumn(dt);
  da_ret->SetUserData("IMAGE", true); // the one place we set this!
  float_MatrixPtr ret_img; ret_img = (float_Matrix*)da_ret->GetValAsMatrix(-1);

  taImageProc::SampleImageWindow_float(*ret_img, img_data, retina_size.x, retina_size.y, 
				       ctr_x, ctr_y, rotate, scale, edge_mode);
  if(edge_mode == taImageProc::BORDER) taImageProc::RenderBorder_float(*ret_img);

  if(edge_mode == taImageProc::BORDER && eff_fd_wd > 0) {
    taImageProc::FadeEdgesToBorder_float(*ret_img, eff_fd_wd); 
  }

  int idx;
  float_MatrixPtr isz_mat; isz_mat = (float_Matrix*)dt->FindMakeColName("ImageSize", idx, DataTable::VT_FLOAT, 1, 2)->GetValAsMatrix(-1);
  isz_mat->FastEl(0) = img_size.x; isz_mat->FastEl(1) = img_size.y;
  
  float_MatrixPtr mv_mat; mv_mat = (float_Matrix*)dt->FindMakeColName("Move", idx, DataTable::VT_FLOAT, 1, 2)->GetValAsMatrix(-1);
  mv_mat->FastEl(0) = move_x; mv_mat->FastEl(1) = move_y;

  dt->FindMakeColName("Scale", idx, DataTable::VT_FLOAT, 0)->SetValAsFloat(scale, -1);
  dt->FindMakeColName("Rotate", idx, DataTable::VT_FLOAT, 0)->SetValAsFloat(rotate, -1);
  return true;
}

bool RetinaSpec::LookAtImageData(float_Matrix& img_data, DataTable* dt,
				 RetinalSpacingSpec::Region region,
				 float box_ll_x, float box_ll_y,
				 float box_ur_x, float box_ur_y,
				 float move_x, float move_y,
				 float scale, float rotate, 
				 bool superimpose) {
  if(dogs.size == 0) return false;

  // find the fovea filter: one with smallest input_size
  DoGRetinaSpec* fov_spec = dogs.FindRetinalRegion(region);
  if(!fov_spec) return false;

  // translation: find the middle of the box
  FloatTwoDCoord obj_ctr((float) (0.5 * (float) (box_ll_x + box_ur_x)),
                       (float) (0.5 * (float) (box_ll_y + box_ur_y)));
  // convert into center-relative coords:
  FloatTwoDCoord obj_ctr_off = 2.0f * (obj_ctr - 0.5f);

  move_x -= obj_ctr_off.x;
  move_y -= obj_ctr_off.y;
  
  // now, scale the thing to fit in fov_spec->input_size
  TwoDCoord img_size(img_data.dim(0), img_data.dim(1));

  // height and width in pixels of box:
  float pix_x = (box_ur_x - box_ll_x) * img_size.x;
  float pix_y = (box_ur_y - box_ll_y) * img_size.y;

  // scale to fit within input size of filter or retina
  float sc_x = (float)fov_spec->spacing.input_size.x / pix_x;
  float sc_y = (float)fov_spec->spacing.input_size.y / pix_y;

  float fov_sc = MIN(sc_x, sc_y);
  scale *= fov_sc;
  if(scale > 100.0f)
    scale = 100.0f;
  if(scale < .01f)
    scale = .01f;

  bool rval = TransformImageData(img_data, dt, move_x, move_y, scale, rotate,
				 superimpose);

  if(rval) {
    int idx;
    float_MatrixPtr box_mat; box_mat = (float_Matrix*)dt->FindMakeColName("LookBox", idx, DataTable::VT_FLOAT, 1, 4)->GetValAsMatrix(-1);
    box_mat->FastEl(0) = box_ll_x; box_mat->FastEl(1) = box_ll_y;
    box_mat->FastEl(2) = box_ur_x; box_mat->FastEl(1) = box_ur_y;
  }
  return rval;
}

bool RetinaSpec::FilterImageData(DataTable* dt, bool superimpose, int renorm) {
  if (!dt) return false;
  if(dogs.size == 0) return false;
  if(superimpose) {
    if(dt->rows <= 0) superimpose = false; // can't do it!
  }

  cur_dt = dt;
  cur_superimpose = superimpose;
  cur_renorm = renorm;

  max_vals.SetGeom(1, dogs.size);
  max_vals.InitVals();

  // this is shared across threads so needs to be setup in advance
  DataCol* da_ret = GetRetImageColumn(cur_dt);
  cur_ret_img = (float_Matrix*)da_ret->GetValAsMatrix(-1);
  taBase::Ref(cur_ret_img);

  threads.InitAll();
  threads.min_units = 1;
  ThreadImgProcCall ip_call(&ImgProcThreadBase::Filter_Thread);

  cur_phase = 0;
  threads.Run(&ip_call, dogs.size);
  if(renorm > 0) {
    int idx;
    float max_val = taMath_float::vec_max(&max_vals, idx);
    if(max_val > renorm_thr) {
      cur_rescale = 1.0f;
      cur_renorm_factor = (float)(renorm - 1);
      if(renorm >= 2)
	cur_rescale = 1.0f / logf(1.0f + cur_renorm_factor * max_val);
      else
	cur_rescale = 1.0f / max_val;
      cur_phase = 1;
      threads.Run(&ip_call, dogs.size);
    }
  }

  taBase::unRefDone(cur_ret_img);
  return true;
}

void RetinaSpec::Filter_Thread(int cmp_idx, int thread_no) {
  if(cur_phase == 0) {
    DoGRetinaSpec* sp = dogs[cmp_idx];
    DataCol* da_on = cur_dt->FindColName(sp->name + "_on");
    DataCol* da_off = cur_dt->FindColName(sp->name + "_off");
    float_Matrix* on_mat = (float_Matrix*)da_on->GetValAsMatrix(-1);
    float_Matrix* off_mat = (float_Matrix*)da_off->GetValAsMatrix(-1);
    taBase::Ref(on_mat);    taBase::Ref(off_mat);
    taImageProc::DoGFilterRetina(*on_mat, *off_mat, *cur_ret_img, *sp, cur_superimpose, edge_mode);
    if(cur_renorm > 0) {
      int idx;
      float on_max = taMath_float::vec_max(on_mat, idx);
      float off_max = taMath_float::vec_max(off_mat, idx);
      on_max = MAX(on_max, off_max);
      max_vals.FastEl(cmp_idx) = on_max;
    }
    taBase::unRefDone(on_mat);    taBase::unRefDone(off_mat);
  }
  else {
    // normalize with single max for all channels, so they are all on a comparable scale
    DoGRetinaSpec* sp = dogs[cmp_idx];
    DataCol* da_on = cur_dt->FindColName(sp->name + "_on");
    DataCol* da_off = cur_dt->FindColName(sp->name + "_off");
    float_Matrix* on_mat = (float_Matrix*)da_on->GetValAsMatrix(-1);
    float_Matrix* off_mat = (float_Matrix*)da_off->GetValAsMatrix(-1);
    taBase::Ref(on_mat);    taBase::Ref(off_mat);
    if(cur_renorm >= 2) {
      for(int j=0;j<on_mat->size;j++)
	on_mat->FastEl_Flat(j) = logf(1.0f + cur_renorm_factor * on_mat->FastEl_Flat(j)) * cur_rescale;
      for(int j=0;j<off_mat->size;j++)
	off_mat->FastEl_Flat(j) = logf(1.0f + cur_renorm_factor * off_mat->FastEl_Flat(j)) * cur_rescale;
    }
    else {
      taMath_float::vec_mult_scalar(on_mat, cur_rescale);
      taMath_float::vec_mult_scalar(off_mat, cur_rescale);
    }
    taBase::unRefDone(on_mat);    taBase::unRefDone(off_mat);
  }
}

///////////////////////////////////////////////////////////////////////
// Transform Routines taking different sources for image input data

bool RetinaSpec::ConvertImageToMatrix(taImage& img, float_Matrix& img_data) {
  if(color_type == COLOR) {
    img.ImageToRGB_float(img_data);
  }
  else {
    img.ImageToGrey_float(img_data);
  }
  return true;
}

bool RetinaSpec::RecordImageName(taImage& img, DataTable* dt) {
  int idx;
  String imgnm = img.name;
  int n_slash = imgnm.freq('/');
  if(n_slash > 2) {
    for(int i=0;i<n_slash-2;i++)
      imgnm = imgnm.after('/'); // get rid of all but last 2
  }
  dt->FindMakeColName("Name", idx, DataTable::VT_STRING, 0)->SetValAsString(imgnm, -1);
  return true;
}

bool RetinaSpec::TransformImage(taImage& img, DataTable* dt,
				float move_x, float move_y,
				float scale, float rotate,
				bool superimpose)
{
  if (!dt) return false;
  float_Matrix img_data(false);
  taBase::Ref(img_data);	// make sure it isn't killed by some other ops..
  ConvertImageToMatrix(img, img_data);

  bool rval = TransformImageData(img_data, dt, move_x, move_y, scale, rotate,
				 superimpose);
  if(rval) {
    RecordImageName(img, dt);
  }
  return rval;
}

bool RetinaSpec::TransformImageName(const String& img_fname, DataTable* dt,
				    float move_x, float move_y,
				    float scale, float rotate,
				    bool superimpose)
{
  if (!dt) return false;
  taImage img;
  if(!img.LoadImage(img_fname)) return false;
  img.name = img_fname;		// explicitly name it
  return TransformImage(img, dt, move_x, move_y, scale, rotate,
			superimpose);
}

///////////// Look At

bool RetinaSpec::LookAtImage(taImage& img, DataTable* dt,
			     RetinalSpacingSpec::Region region,
			     float box_ll_x, float box_ll_y,
			     float box_ur_x, float box_ur_y,
			     float move_x, float move_y,
			     float scale, float rotate, 
			     bool superimpose) {
  float_Matrix img_data(false);
  taBase::Ref(img_data);	// make sure it isn't killed by some other ops..
  ConvertImageToMatrix(img, img_data);

  bool rval = LookAtImageData(img_data, dt, region,
			      box_ll_x, box_ll_y, box_ur_x, box_ur_y,
			      move_x, move_y, scale, rotate,
			      superimpose);
  if(rval) {
    RecordImageName(img, dt);
  }
  return rval;
}

bool RetinaSpec::LookAtImageName(const String& img_fname, DataTable* dt,
				 RetinalSpacingSpec::Region region,
				 float box_ll_x, float box_ll_y,
				 float box_ur_x, float box_ur_y,
				 float move_x, float move_y,
				 float scale, float rotate, 
				 bool superimpose) {
  taImage img;
  if(!img.LoadImage(img_fname)) return false;
  img.name = img_fname;		// explicitly name it
  return LookAtImage(img, dt, region, box_ll_x, box_ll_y, box_ur_x, box_ur_y,
		     move_x, move_y, scale, rotate,
		     superimpose);
}


///////////////////////////////////////////////////////////////////////
// Full end-to-end functionality of routines

bool RetinaSpec::XFormFilterImageData(float_Matrix& img_data, DataTable* dt,
				      float move_x, float move_y,
				      float scale, float rotate,
				      bool superimpose, int renorm)
{
  bool rval = TransformImageData(img_data, dt, move_x, move_y, scale, rotate,
				 superimpose);
  if(rval)
    rval = FilterImageData(dt, superimpose, renorm);
  if(rval)
    dt->WriteClose();
  return rval;
}

bool RetinaSpec::XFormFilterImage(taImage& img, DataTable* dt,
				  float move_x, float move_y,
				  float scale, float rotate,
				  bool superimpose, int renorm)
{
  bool rval = TransformImage(img, dt, move_x, move_y, scale, rotate,
			     superimpose);
  if(rval)
    rval = FilterImageData(dt, superimpose, renorm);
  if(rval)
    dt->WriteClose();
  return rval;
}

bool RetinaSpec::XFormFilterImageName(const String& img_fname, DataTable* dt,
				      float move_x, float move_y,
				      float scale, float rotate,
				      bool superimpose, int renorm)
{
  bool rval = TransformImageName(img_fname, dt, move_x, move_y, scale, rotate,
				 superimpose);
  if(rval)
    rval = FilterImageData(dt, superimpose, renorm);
  if(rval)
    dt->WriteClose();
  return rval;
}

bool RetinaSpec::LookAtFilterImageData(float_Matrix& img_data, DataTable* dt,
				       RetinalSpacingSpec::Region region,
				       float box_ll_x, float box_ll_y,
				       float box_ur_x, float box_ur_y,
				       float move_x, float move_y,
				       float scale, float rotate, 
				       bool superimpose, int renorm) {
  bool rval = LookAtImageData(img_data, dt, region,
			      box_ll_x, box_ll_y, box_ur_x, box_ur_y,
			      move_x, move_y, scale, rotate, 
			      superimpose);
  if(rval)
    rval = FilterImageData(dt, superimpose, renorm);
  if(rval)
    dt->WriteClose();
  return rval;
}

bool RetinaSpec::LookAtFilterImage(taImage& img, DataTable* dt,
				   RetinalSpacingSpec::Region region,
				   float box_ll_x, float box_ll_y,
				   float box_ur_x, float box_ur_y,
				   float move_x, float move_y,
				   float scale, float rotate, 
				   bool superimpose, int renorm) {
  bool rval = LookAtImage(img, dt, region,
			  box_ll_x, box_ll_y, box_ur_x, box_ur_y,
			  move_x, move_y, scale, rotate,
			  superimpose);
  if(rval)
    rval = FilterImageData(dt, superimpose, renorm);
  if(rval)
    dt->WriteClose();
  return rval;
}

bool RetinaSpec::LookAtFilterImageName(const String& img_fname, DataTable* dt,
				       RetinalSpacingSpec::Region region,
				       float box_ll_x, float box_ll_y,
				       float box_ur_x, float box_ur_y,
				       float move_x, float move_y,
				       float scale, float rotate, 
				       bool superimpose, int renorm) {
  bool rval = LookAtImageName(img_fname, dt, region,
			      box_ll_x, box_ll_y, box_ur_x, box_ur_y,
			      move_x, move_y, scale, rotate,
			      superimpose);
  if(rval)
    rval = FilterImageData(dt, superimpose, renorm);
  if(rval)
    dt->WriteClose();
  return rval;
}

///////////////////////////////////////////////////////////////////////
// Misc other processing operations

bool RetinaSpec::AttendRegion(DataTable* dt, RetinalSpacingSpec::Region region) {
  DoGRetinaSpec* fov_spec = dogs.FindRetinalRegion(region);
  if(!fov_spec) return false;

  float fov_x_pct = (float)fov_spec->spacing.input_size.x / (float)retina_size.x;
  float fov_y_pct = (float)fov_spec->spacing.input_size.y / (float)retina_size.y;
  float fov_pct = taMath_float::max(fov_x_pct, fov_y_pct);

  int idx;
  for(int i=0;i<dogs.size;i++) {
    DoGRetinaSpec* sp = dogs[i];
    if(sp->spacing.region <= region) continue; // don't filter this region -- only ones above it!
    DataCol* da_on = dt->FindMakeColName(sp->name + "_on", idx, DataTable::VT_FLOAT, 2,
						sp->spacing.output_size.x, sp->spacing.output_size.y);
    DataCol* da_off = dt->FindMakeColName(sp->name + "_off", idx, DataTable::VT_FLOAT,
						 2, sp->spacing.output_size.x, sp->spacing.output_size.y);

    float_MatrixPtr on_mat; on_mat = (float_Matrix*)da_on->GetValAsMatrix(-1);
    float_MatrixPtr off_mat; off_mat = (float_Matrix*)da_off->GetValAsMatrix(-1);
    taImageProc::AttentionFilter(*on_mat, fov_pct);
    taImageProc::AttentionFilter(*off_mat, fov_pct);
  }
  return true;
}

bool RetinaSpec::RenderOccluder(DataTable* dt, float llx, float lly, float urx, float ury) {
  if(!dt) return false;
  DataCol* da_ret = GetRetImageColumn(dt);
  float_MatrixPtr ret_img; ret_img = (float_Matrix*)da_ret->GetValAsMatrix(-1);

  taImageProc::RenderOccluderBorderColor_float(*ret_img, llx, lly, urx, ury);
  
  return true;
}



//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// 		Full V1 Spec

void V1GaborSpec::Initialize() {
  norm_max = .95f;
  norm_thr = 0.01f;
}

void V1GaborSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // todo: anything here??
}

void V1GaborSpec::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  gabors.CheckConfig(quiet, rval);
}

void V1GaborSpec::DefaultFilters() {
  if(!retina) {
    taMisc::Error("V1GaborSpec::DefaultFilters -- requires the retina spec pointer to be set to get needed information!");
    return;
  }
  StructUpdate(false);
  if(retina->color_type == RetinaSpec::COLOR)
    gabors.SetSize(5);
  else
    gabors.SetSize(3);
  GaborV1Spec* sp;
  int cnt = 0;
  sp = gabors[cnt++];
  sp->name = "V1_high";
  sp->filter_type = GaborV1Spec::GABOR;
  sp->region = RetinalSpacingSpec::FOVEA;
  sp->res = RetinalSpacingSpec::HI_RES;
  sp->rf_width = 4;
  sp->gabor_rf.n_angles = 4;
  sp->gabor_rf.freq = 1.5f;
  sp->gabor_rf.length = 2.0f;
  sp->gabor_rf.width = 2.0f;
  sp->gabor_rf.amp = .9f;
  sp->UpdateAfterEdit();

  sp = gabors[cnt++];
  sp->name = "V1_med";
  sp->filter_type = GaborV1Spec::GABOR;
  sp->region = RetinalSpacingSpec::FOVEA;
  sp->res = RetinalSpacingSpec::MED_RES;
  sp->rf_width = 4;
  sp->gabor_rf.n_angles = 4;
  sp->gabor_rf.freq = 1.5f;
  sp->gabor_rf.length = 2.0f;
  sp->gabor_rf.width = 2.0f;
  sp->gabor_rf.amp = .9f;
  sp->UpdateAfterEdit();

  sp = gabors[cnt++];
  sp->name = "V1_low";
  sp->filter_type = GaborV1Spec::GABOR;
  sp->region = RetinalSpacingSpec::FOVEA;
  sp->res = RetinalSpacingSpec::LOW_RES;
  sp->rf_width = 4;
  sp->gabor_rf.n_angles = 4;
  sp->gabor_rf.freq = 1.5f;
  sp->gabor_rf.length = 2.0f;
  sp->gabor_rf.width = 2.0f;
  sp->gabor_rf.amp = .9f;
  sp->UpdateAfterEdit();

  if(retina->color_type == RetinaSpec::COLOR) {
    sp = gabors[cnt++];
    sp->name = "V1_hblob";
    sp->filter_type = GaborV1Spec::BLOB;
    sp->region = RetinalSpacingSpec::FOVEA;
    sp->res = RetinalSpacingSpec::HI_RES;
    sp->rf_width = 4;
    sp->blob_rf.n_sizes = 1;
    sp->blob_rf.width_st = 1.0f;
    sp->blob_rf.width_inc = 2.0f;
    sp->UpdateAfterEdit();

    sp = gabors[cnt++];
    sp->name = "V1_mblob";
    sp->filter_type = GaborV1Spec::BLOB;
    sp->region = RetinalSpacingSpec::FOVEA;
    sp->res = RetinalSpacingSpec::MED_RES;
    sp->rf_width = 4;
    sp->blob_rf.n_sizes = 1;
    sp->blob_rf.width_st = 1.0f;
    sp->blob_rf.width_inc = 2.0f;
    sp->UpdateAfterEdit();
  }
  StructUpdate(true);
}

bool V1GaborSpec::UpdateSizesFmRetina() {
  if(!retina) {
    taMisc::Error("V1GaborSpec::UpdateSizesFmRetina -- requires the retina spec pointer to be set to get needed information!");
    return false;
  }
  bool rval = gabors.UpdateSizesFmRetina(retina->dogs);
  if(!rval) {
    taMisc::Error("V1GaborSpec::UpdateSizesFmRetina -- did not get all clean sizes!");
    return false;
  }
  return true;
}

void V1GaborSpec::ConfigDataTable(DataTable* dt, bool reset_cols) {
  if(!UpdateSizesFmRetina()) return;
  taProject* proj = GET_MY_OWNER(taProject);
  if(!dt) {
    DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("InputData");
    dt = dgp->NewEl(1, &TA_DataTable);
    if(!name.empty())
      dt->name = name + "_InputData";
    else
      dt->name = "V1GaborSpec_InputData";
  }
  dt->ResetData();
  if(reset_cols) dt->Reset();
  dt->StructUpdate(true);
  int idx =0;
  dt->FindMakeColName("Name", idx, DataTable::VT_STRING, 0);
  for(int i=0;i<gabors.size; i++) {
    GaborV1Spec* sp = gabors[i];
    if(sp->filter_type == GaborV1Spec::COPY) {
      dt->FindMakeColName(sp->name, idx, DataTable::VT_FLOAT, 2,
			  sp->un_geom.x, sp->un_geom.y);
    }
    else {
      dt->FindMakeColName(sp->name, idx, DataTable::VT_FLOAT, 4,
			  sp->un_geom.x, sp->un_geom.y, sp->gp_geom.x, sp->gp_geom.y);
    }
  }
  dt->StructUpdate(false);
}

bool V1GaborSpec::FilterRetinaData(DataTable* v1_out_dt, DataTable* ret_in_dt) {
  if(!retina) {
    taMisc::Error("V1GaborSpec::FilterRetinaData -- requires the retina spec pointer to be set to get needed information!");
    return false;
  }
  if(!v1_out_dt || !ret_in_dt) {
    taMisc::Error("V1GaborSpec::FilterRetinaData -- requires non-null input and output data tables");
    return false;
  }
  v1_out_dt->StructUpdate(true);
  v1_out_dt->EnforceRows(1);
  v1_out_dt->WriteItem(0);
  v1_out_dt->ReadItem(0);
  for(int i=0;i<gabors.size; i++) {
    GaborV1Spec* sp = gabors[i];
    float_Matrix* out_mat = (float_Matrix*)v1_out_dt->GetSinkMatrixByName(sp->name);

    bool first = true;
    for(int j=0;j<retina->dogs.size; j++) {
      DoGRetinaSpec* dog = retina->dogs[j];
      if(dog->spacing.region != sp->region || dog->spacing.res != sp->res) continue;
      float_Matrix* on_mat = (float_Matrix*)ret_in_dt->GetMatrixDataByName(dog->name + "_on");
      if(!on_mat) continue;
      taBase::Ref(on_mat);
      float_Matrix* off_mat = (float_Matrix*)ret_in_dt->GetMatrixDataByName(dog->name + "_off");
      if(!off_mat) continue;
      taBase::Ref(off_mat);

      sp->FilterInput(*out_mat, dog->dog.color_chan, *on_mat, *off_mat, !first);

      taBase::unRefDone(on_mat);
      taBase::unRefDone(off_mat);
      first = false;
    }

    if(norm_max > 0.0f) {
      int idx;
      float max_val = taMath_float::vec_max(out_mat, idx);
      if(max_val > norm_thr) {
	float rescale = norm_max / max_val;
	taMath_float::vec_mult_scalar(out_mat, rescale);
      }
    }
    taBase::unRefDone(out_mat);
  }
  v1_out_dt->SetDataByName(ret_in_dt->GetDataByName("Name"), "Name"); // transfer over the name
  v1_out_dt->StructUpdate(false);
  return true;
}


///////////////////////////////////////////////////////////
// 		program stuff

void ImageProcCall::Initialize() {
  min_type = &TA_taImageProc;
  object_type = &TA_taImageProc;
}

