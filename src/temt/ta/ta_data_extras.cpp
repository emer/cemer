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



#include "ta_data_extras.h"

#include <QDir>

//////////////////////////
//  DirectoryCatalog    //
//////////////////////////

void DirectoryCatalog::Initialize() {
  m_dir = NULL;
  options = CO_Files;
  directory = "./";
}

void DirectoryCatalog::Destroy() {
  ReadClose();
}

void DirectoryCatalog::Copy_(const DirectoryCatalog& cp) {
  ReadClose();
  directory = cp.directory;
}

QDir& DirectoryCatalog::dir() {
  if (!m_dir)
    m_dir = new QDir();
  return *m_dir;
}

const Variant DirectoryCatalog::GetData_impl(int chan) {
  switch (chan) {
  case 0: return dir()[rd_itr]; // blank if out of range
  default: return _nilVariant;
  }
}

int DirectoryCatalog::ItemCount() const {
  return m_dir->count();
}

void DirectoryCatalog::ReadOpen_impl(bool& ok) {
  inherited::ReadOpen_impl(ok);
  if (!ok) return;
  QDir& dir = this->dir(); // cache
  dir.setPath(directory);
  QDir::Filters flags = QDir::NoDotAndDotDot;
  if (options & CO_Files) flags |= QDir::Files;
  if (options & CO_Dirs) flags |= QDir::AllDirs;
  dir.setFilter(flags);
  if (filters.nonempty()) {
    QStringList sl((filters.toQString()).split(";"));
    dir.setNameFilters(sl);
  }
  ReadItrInit();
}

void DirectoryCatalog::ReadClose_impl() {
  if (m_dir) {
    delete m_dir;
    m_dir = NULL;
  }
  inherited::ReadClose_impl();
}

const String DirectoryCatalog::SourceChannelName(int chan) const {
  static String chan_files("files");
  switch (chan) {
  case 0: return chan_files;
  default: return _nilString;
  }
}


//////////////////////////
//  ImageReader         //
//////////////////////////

void ImageReader::Initialize() {
  color_mode = CM_RGB;
  img_format = IF_AUTO;
  cur_img_format = img_format;
  m_mat = NULL;
  m_rgb_mat = NULL;
}

void ImageReader::Destroy() {
  SetMat(NULL);
}

void ImageReader::InitLinks() {
  inherited::InitLinks();
  taBase::Own(img_size, this);
}

void ImageReader::CutLinks() {
  img_size.CutLinks();
  inherited::CutLinks();
}

void ImageReader::Copy_(const ImageReader& cp) {
  color_mode = cp.color_mode;
  img_format = cp.img_format;
  //dynamics get cleared
  cur_img_format = IF_AUTO;
  fname = _nilString;
  SetMat(NULL);
}

taMatrix* ImageReader::GetDataMatrix_impl(int chan) {
  if (!m_mat) return NULL;

  if (chan == chan_img)
    return m_mat;

  //TODO: rgb_Matrix channel
/*obs  switch (color_mode) {
  case CM_GRAYSCALE:
    switch (chan) {
    case chan_bw: return m_mat->GetFrameSlice_(0);
    default: return NULL;
    }
  case CM_RGB:
    switch (chan) {
    case chan_r: return m_mat->GetFrameSlice_(0);
    case chan_g: return m_mat->GetFrameSlice_(1);
    case chan_b: return m_mat->GetFrameSlice_(2);
    default: return NULL;
    }
  }*/
  return NULL;
}

void ImageReader::ReadClose_impl() {
  if (m_mat) {
    taBase::DelPointer((taBase**)&m_mat);
  }
  inherited::ReadClose_impl();
}

bool ImageReader::ReadImage_Jpeg() {
  FILE *infile = fopen(fname, "rb");
  if (infile == NULL) {
    taMisc::Warning("ImageReader::ReadImage_Jpeg: can't open file: ", fname);
    return false;
  }

  // todo: use QtImage or taImage for this!  not worth the libjpeg dependency!!

  return true;
}


bool ImageReader::ReadItem_impl() {
  switch (cur_img_format) {
  case IF_JPEG: return ReadImage_Jpeg();
  default: return false;
  }
}

void ImageReader::ReadOpen_impl(bool& ok) {
  inherited::ReadOpen_impl(ok);
  if (!ok) return;

  // if in auto mode, try to guess based on file name
  if (img_format == IF_AUTO) {
    String tmp = upcase(fname);
    if (tmp.contains(".JPG") || tmp.contains(".JPEG"))
      cur_img_format = IF_JPEG;
/*    else if (tmp.contains(".PNG"))
      cur_img_format = IF_PNG;
    else if (tmp.contains(".TIF") || tmp.contains(".TIFF"))
      cur_img_format = IF_TIF; */
  } else {
    cur_img_format = img_format;
  }
}

void ImageReader::SetMat(taMatrix* new_mat) {
  taBase::SetPointer((taBase**)&m_mat, new_mat);
}

int ImageReader::sourceChannelCount() const {
  int rval = 1; // base
  switch (color_mode) {
  case CM_GRAYSCALE: rval += 1; break;
  case CM_RGB: rval += 3; break;
  }
  return rval;
}

const String ImageReader::sourceChannelName(int chan) const {
//note: it is safe to not care what current channel count is, because access by name checks
  static String nm_img("img");

  if (chan == chan_img)
    return nm_img;
  else return _nilString;
  //TODO: rgb format

/*obs  switch (color_mode) {
  case CM_GRAYSCALE:
    switch (chan) {
    case chan_bw: return nm_bw;
    default: return _nilString;
    }
  case CM_RGB:
    switch (chan) {
    case chan_r: return nm_r;
    case chan_g: return nm_g;
    case chan_b: return nm_b;
    default: return _nilString;
    }
  }*/
}
