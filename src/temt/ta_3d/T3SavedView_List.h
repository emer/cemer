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

#ifndef T3SavedView_List_h
#define T3SavedView_List_h 1

// parent includes:
#include <taList>
#include <T3SavedView>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(T3SavedView_List);

class TA_API T3SavedView_List: public taList<T3SavedView> {
  // ##CAT_View list of saved views
  INHERITED(taList<T3SavedView>)
public:

  void  SetCameraPos(int view_no, float x, float y, float z);
  // #CAT_Display for given view number, set camera position
  void  GetCameraPos(int view_no, float& x, float& y, float& z);
  // #CAT_Display for given view number, set camera position

#ifdef TA_QT3D
  void  SetCameraLookAt(int view_no, float x, float y, float z);
  // #CAT_Display for given view number, set camera look at point
  void  GetCameraLookAt(int view_no, float& x, float& y, float& z);
  // #CAT_Display for given view number, set camera look at point
  void  SetCameraUp(int view_no, float x, float y, float z);
  // #CAT_Display for given view number, set camera look at point
  void  GetCameraUp(int view_no, float& x, float& y, float& z);
  // #CAT_Display for given view number, set camera look at point

#else // TA_QT3D
  void  SetCameraOrient(int view_no, float x, float y, float z, float r);
  // #CAT_Display for given view number, set camera orientation -- x,y,z axis and r rotation value
  void  SetCameraFocDist(int view_no, float fd);
  // #CAT_Display for given view number, set camera focal distance

  void  GetCameraOrient(int view_no, float& x, float& y, float& z, float& r);
  // #CAT_Display for given view number, set camera orientation -- x,y,z axis and r rotation value
  void  GetCameraFocDist(int view_no, float& fd);
  // #CAT_Display for given view number, set camera focal distance
#endif // TA_QT3D


  TA_BASEFUNS_NOCOPY(T3SavedView_List);
private:
  void  Initialize();
  void  Destroy() { };
};

#endif // T3SavedView_List_h
