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

#include "T3SavedView_List.h"

TA_BASEFUNS_CTORS_DEFN(T3SavedView_List);

void T3SavedView_List::Initialize() {
  SetBaseType(&TA_T3SavedView);
}

void T3SavedView_List::SetCameraPos(int view_no, float x, float y, float z) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "SetCameraPos", "view no out of range:", String(view_no), "n views:",
               String(size))) return;
  sv->SetCameraPos(x,y,z);
}

void T3SavedView_List::SetCameraOrient(int view_no, float x, float y, float z, float r) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "SetCameraOrient", "view no out of range:", String(view_no), "n views:",
               String(size))) return;
  sv->SetCameraOrient(x,y,z,r);
}

void T3SavedView_List::SetCameraFocDist(int view_no, float fd) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "SetCameraFocDist", "view no out of range:", String(view_no), "n views:",
               String(size))) return;
  sv->SetCameraFocDist(fd);
}

void T3SavedView_List::GetCameraPos(int view_no, float& x, float& y, float& z) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "GetCameraPos", "view no out of range:", String(view_no), "n views:",
               String(size))) return;
  sv->GetCameraPos(x,y,z);
}

void T3SavedView_List::GetCameraOrient(int view_no, float& x, float& y, float& z, float& r) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "GetCameraOrient", "view no out of range:", String(view_no), "n views:",
               String(size))) return;
  sv->GetCameraOrient(x,y,z,r);
}

void T3SavedView_List::GetCameraFocDist(int view_no, float& fd) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "GetCameraFocDist", "view no out of range:", String(view_no), "n views:",
               String(size))) return;
  sv->GetCameraFocDist(fd);
}
