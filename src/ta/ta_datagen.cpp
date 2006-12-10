// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include "ta_datagen.h"
#include "css_machine.h"
#include "ta_geometry.h"
#include "ta_project.h"		// for debugging

#include <QDir>

bool taDataGen::CheckDims(float_Matrix* mat, int dims) {
  if(mat->dims() != dims) {
    taMisc::Warning("taDataGen: matrix:", mat->name, "path:", mat->GetPath(),
		    "is not of dimension:", String(dims), "it is:", String(mat->dims()));
    return false;
  }
  return true;
}

bool taDataGen::WritePoint(float_Matrix* mat, int x, int y, float color, bool wrap) {
  if(!CheckDims(mat, 2)) return false;
  bool clipped = (TwoDCoord::WrapClipOne(wrap, x, mat->dim(0)) ||
		  TwoDCoord::WrapClipOne(wrap, y, mat->dim(1)));
  if(!wrap && clipped)
    return false;
  mat->FastEl(x,y) = color;
  return true;
}

bool taDataGen::RenderLine(float_Matrix* mat, int xs, int ys, int xe, int ye,
			   float color, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int xd = xe - xs;
  int yd = ye - ys;
  int x,y;
  if((xd == 0) && (yd == 0)) {
    WritePoint(mat, xs, ys, color, wrap);
    return true;
  }
  if(xd == 0) {
    if(yd > 0) { for(y=ys;y<=ye;y++) WritePoint(mat, xs, y, color, wrap); }
    else       { for(y=ys;y>=ye;y--) WritePoint(mat, xs, y, color, wrap); }
    return true;
  }
  if(yd == 0) {
    if(xd > 0)	{ for(x=xs;x<=xe;x++) WritePoint(mat, x, ys, color, wrap); }
    else	{ for(x=xs;x>=xe;x--) WritePoint(mat, x, ys, color, wrap); }
    return true;
  }
  if(abs(xd) > abs(yd)) {
    if(yd > 0) yd++; else yd--;
    if(xd > 0) {
      for(x=xs; x<=xe; x++) {
	y = ys + (yd * (x - xs)) / abs(xd);
	if(yd > 0) y = MIN(ye, y);
	else y = MAX(ye, y);
	WritePoint(mat, x, y, color, wrap);
      }
    }
    else {
      for(x=xs; x>=xe; x--) {
	y = ys + (yd * (xs - x)) / abs(xd);
	if(yd > 0) y = MIN(ye, y);
	else y = MAX(ye, y);
	WritePoint(mat, x, y, color, wrap);
      }
    }
  }
  else {
    if(xd > 0) xd++; else xd--;
    if(yd > 0) {
      for(y=ys; y<=ye; y++) {
	x = xs + (xd * (y - ys)) / abs(yd);
	if(xd > 0) x = MIN(xe, x);
	else x = MAX(xe, x);
	WritePoint(mat, x, y, color, wrap);
      }
    }
    else {
      for(y=ys; y>=ye; y--) {
	x = xs + (xd * (ys - y)) / abs(yd);
	if(xd > 0) x = MIN(xe, x);
	else x = MAX(xe, x);
	WritePoint(mat, x, y, color, wrap);
      }
    }
  }
  return true;
}

bool taDataGen::WriteXPoints(float_Matrix* mat, int x, int y,
			     const float_Matrix& color, int wdth, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int del = (wdth - 1) / 2;
  int i;
  for(i=0; i<wdth; i++)
    WritePoint(mat, x, y+i-del, color.FastEl(i), wrap);
  return true;
}

bool taDataGen::WriteYPoints(float_Matrix* mat, int x, int y,
			     const float_Matrix& color, int wdth, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int del = (wdth - 1) / 2;
  int i;
  for(i=0; i<wdth; i++)
    WritePoint(mat, x+i-del, y, color.FastEl(i), wrap);
  return true;
}

bool taDataGen::RenderWideLine(float_Matrix* mat, int xs, int ys, int xe, int ye,
			       const float_Matrix& color, int wdth, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int xd = xe - xs;
  int yd = ye - ys;
  int x,y;
  if((xd == 0) && (yd == 0)) {
    WriteXPoints(mat, xs, ys, color, wdth, wrap);
    WriteYPoints(mat, xs, ys, color, wdth, wrap);
    return true;
  }
  if(xd == 0) {
    if(yd > 0) { for(y=ys;y<=ye;y++) WriteXPoints(mat, xs, y, color, wdth, wrap); }
    else       { for(y=ys;y>=ye;y--) WriteXPoints(mat, xs, y, color, wdth, wrap); }
    return true;
  }
  if(yd == 0) {
    if(xd > 0)	{ for(x=xs;x<=xe;x++) WriteYPoints(mat, x, ys, color, wdth, wrap); }
    else	{ for(x=xs;x>=xe;x--) WriteYPoints(mat, x, ys, color, wdth, wrap); }
    return true;
  }
  if(abs(xd) > abs(yd)) {
    if(xd > 0) {
      for(x=xs; x<=xe; x++) {
	y = ys + (yd * (x - xs)) / abs(xd);
	WriteXPoints(mat, x, y, color, wdth, wrap);
      }
    }
    else {
      for(x=xs; x>=xe; x--) {
	y = ys + (yd * (xs - x)) / abs(xd);
	WriteXPoints(mat, x, y, color, wdth, wrap);
      }
    }
  }
  else {
    if(yd > 0) {
      for(y=ys; y<=ye; y++) {
	x = xs + (xd * (y - ys)) / abs(yd);
	WriteYPoints(mat, x, y, color, wdth, wrap);
      }
    }
    else {
      for(y=ys; y>=ye; y--) {
	x = xs + (xd * (ys - y)) / abs(yd);
	WriteYPoints(mat, x, y, color, wdth, wrap);
      }
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// misc data functions

bool taDataGen::GetDirFiles(DataTable* dest, const String& dir_path, 
			    const String& filter, bool recursive,
			    const String& fname_col_nm,
			    const String& path_col_nm) {
  if(!dest) return false;
  dest->StructUpdate(true);

  if(recursive) {
    taMisc::Warning("Warning: GetDirFiles does not yet support the recursive flag!");
  }

  int fname_idx = -1;
  if(!fname_col_nm.empty())
    dest->FindMakeColName(fname_col_nm, fname_idx, DataTable::VT_STRING, 0);

  int path_idx = -1;
  if(!path_col_nm.empty())
    dest->FindMakeColName(path_col_nm, path_idx, DataTable::VT_STRING, 0);

  bool found_some = false;
  QDir dir(dir_path);
  QStringList files = dir.entryList();
  if(files.size() == 0) return false;
  for(int i=0;i<files.size();i++) {
    String fl = files[i];
    if(filter.empty() || fl.contains(filter)) {
      dest->AddBlankRow();
      found_some = true;
      if(fname_idx >= 0) {
	dest->SetValAsString(fl, fname_idx, -1);
      }
      if(path_idx >= 0) {
	dest->SetValAsString(dir_path + "/" + fl, path_idx, -1);
      }
    }
    // todo: deal with recursive flag
  }
  
  return found_some;
}

/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

void DataGenCall::Initialize() {
  min_type = &TA_taDataGen;
  object_type = &TA_taDataGen;
}
