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

#include "taImage.h"
#include <taFiler>
#include <float_Matrix>
#include <DataTable>

#include <QByteArray>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(taImage);

void taImage::Initialize() {
}

void taImage::Copy_(const taImage& cp) {
  ImageChanging();
  q_img = cp.q_img;
  ImageChanged();
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
  ImageChanging();
  if(!q_img.load(fn)) {
    taMisc::Error("LoadImage: could not read image file:", fnm);
    ImageChanged();
    return false;
  }
  ImageChanged();
  return true;
}

bool taImage::LoadImageFromBase64(const QByteArray &data) {
  bool rval = true;
  ImageChanging();
  QByteArray by = QByteArray::fromBase64(data);
  if (!q_img.loadFromData(by)) {
    rval = false;
  }
  ImageChanged();
  return rval;
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
  
  ImageChanging();
  q_img = QImage(wd, ht, QImage::Format_RGB32);
  
  for(int y=0; y<ht; y++) {
    for(int x=0; x< wd; x++) {
      int gval = (int)(img_data.FastEl2d(x, y) * 255.0f);
      QRgb pix = qRgb(gval, gval, gval);
      q_img.setPixel(x, ht-1-y, pix);
    }
  }
  ImageChanged();
  return true;
}

bool taImage::ImageFromMatrix_rgb(const float_Matrix& rgb_data) {
  if(TestError((rgb_data.dims() < 3), "IMageFromMatrix_rgb", "img data does not have at least 3 dimensions"))
    return false;
  int wd = rgb_data.dim(0);
  int ht = rgb_data.dim(1);
  
  ImageChanging();
  q_img = QImage(wd, ht, QImage::Format_RGB32);
  
  for(int y=0; y<ht; y++) {
    for(int x=0; x< wd; x++) {
      int rval = (int)(rgb_data.FastEl3d(x, y, 0) * 255.0f);
      int gval = (int)(rgb_data.FastEl3d(x, y, 1) * 255.0f);
      int bval = (int)(rgb_data.FastEl3d(x, y, 2) * 255.0f);
      QRgb pix = qRgb(rval, gval, bval);
      q_img.setPixel(x, ht-1-y, pix);
    }
  }
  ImageChanged();
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
  
  ImageChanging();
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
  
  ImageChanged();
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
  ImageChanging();
  int wd = q_img.width();
  int ht = q_img.height();
  int nw_wd = (int)(sx * (float)wd);
  int nw_ht = (int)(sy * (float)ht);
  if(smooth)
    q_img = q_img.scaled(nw_wd, nw_ht, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  else
    q_img = q_img.scaled(nw_wd, nw_ht); // default is fast
  ImageChanged();
  return true;
}

bool taImage::RotateImage(float norm_deg, bool smooth) {
  if(q_img.isNull()) {
    return false;
  }
  ImageChanging();
  float deg = norm_deg * 360.0f;
  QMatrix mat;
  mat.rotate(deg);
  if(smooth)
    q_img = q_img.transformed(mat, Qt::SmoothTransformation);
  else
    q_img = q_img.transformed(mat); // default is fast
  ImageChanged();
  return true;
}

bool taImage::TranslateImage(float move_x, float move_y, bool smooth) {
  if(q_img.isNull()) {
    return false;
  }
  ImageChanging();
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
  ImageChanged();
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
  ImageChanging();
  q_img = QImage(QSize(width,height), QImage::Format_ARGB32);
  ImageChanged();
  return true;
}

