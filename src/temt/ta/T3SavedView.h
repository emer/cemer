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

#ifndef T3SavedView_h
#define T3SavedView_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <taVector3f>
#include <taAxisAngle>

// declare all other types mentioned but not required to include:
class SoCamera; // #IGNORE
class taiAction; //
class QToolButton; // #IGNORE


class TA_API T3SavedView : public taNBase {
  // ##CAT_View Saves camera parameters for the Quarter Examiner Viewer -- name of view is name of object -- can store other arbitrary info in UserData for this guy
INHERITED(taNBase)
public:
  bool          view_saved;     // #READ_ONLY #SHOW whether a view is currently saved or not
  taVector3f    pos;            // #READ_ONLY #EXPERT position of camera in view
  taAxisAngle   orient;         // #READ_ONLY #EXPERT orientation of camera in view
  float         focal_dist;     // #READ_ONLY #EXPERT focal distance

  void          getCameraParams(SoCamera* cam);
  // get the camera parameters into our saved values
  bool          setCameraParams(SoCamera* cam);
  // set the camera parmeters from our saved values (returns false if no view saved)

  void          SetCameraPos(float x, float y, float z) { pos.SetXYZ(x,y,z); }
  // #CAT_Display set camera position
  void          SetCameraOrient(float x, float y, float z, float r) { orient.SetXYZR(x,y,z,r); }
  // #CAT_Display set camera orientation -- x,y,z axis and r rotation value
  void          SetCameraFocDist(float fd) { focal_dist = fd; }
  // #CAT_Display set camera focal distance

  void          GetCameraPos(float& x, float& y, float& z) { pos.GetXYZ(x,y,z); }
  // #CAT_Display set camera position
  void          GetCameraOrient(float& x, float& y, float& z, float& r) { orient.GetXYZR(x,y,z,r); }
  // #CAT_Display set camera orientation -- x,y,z axis and r rotation value
  void          GetCameraFocDist(float& fd) { fd = focal_dist; }
  // #CAT_Display set camera focal distance

  QToolButton*  view_button;    // #IGNORE view button for this view
  taiAction*    view_action;    // #IGNORE action for the gotoview function

  TA_SIMPLE_BASEFUNS(T3SavedView);
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // T3SavedView_h
