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
class iAction; //
class QToolButton; // #IGNORE

#ifdef TA_QT3D
#ifndef __MAKETA__
#include <Qt3DRender/QCamera>
#endif
#else // TA_QT3D
class SoCamera; // #IGNORE
#endif // TA_QT3D


taTypeDef_Of(T3SavedView);

class TA_API T3SavedView : public taNBase {
  // ##CAT_View ##NO_TOKEN_CHOOSER Saves camera parameters for the Quarter Examiner Viewer -- name of view is name of object -- can store other arbitrary info in UserData for this guy
INHERITED(taNBase)
public:
  bool          view_saved;     // #READ_ONLY #SHOW whether a view is currently saved or not
  taVector3f    pos;            // #READ_ONLY #EXPERT position of camera in view
#ifdef TA_QT3D
  taVector3f    look_at;        // #READ_ONLY #EXPERT where camera is looking
  taVector3f    up;             // #READ_ONLY #EXPERT up vector for camera
#else // TA_QT3D
  taAxisAngle   orient;         // #READ_ONLY #EXPERT orientation of camera in view
  float         focal_dist;     // #READ_ONLY #EXPERT focal distance
#endif // TA_QT3D

#ifdef TA_QT3D
#ifndef __MAKETA__
  void          getCameraParams(Qt3DRender::QCamera* cam);
  // #IGNORE get the camera parameters into our saved values
  bool          setCameraParams(Qt3DRender::QCamera* cam);
  // #IGNORE set the camera parmeters from our saved values (returns false if no view saved)
#endif
#else // TA_QT3D
  void          getCameraParams(SoCamera* cam);
  // #IGNORE get the camera parameters into our saved values
  bool          setCameraParams(SoCamera* cam);
  // #IGNORE set the camera parmeters from our saved values (returns false if no view saved)
#endif // TA_QT3D

  void          SetCameraPos(float x, float y, float z) { pos.SetXYZ(x,y,z); }
  // #CAT_Display set camera position
  void          GetCameraPos(float& x, float& y, float& z) { pos.GetXYZ(x,y,z); }
  // #CAT_Display set camera position

#ifdef TA_QT3D
  void          SetCameraLookAt(float x, float y, float z) { look_at.SetXYZ(x,y,z); }
  // #CAT_Display set where camera is looking
  void          GetCameraLookAt(float& x, float& y, float& z) { look_at.GetXYZ(x,y,z); }
  // #CAT_Display set where camera is looking
  void          SetCameraUp(float x, float y, float z) { up.SetXYZ(x,y,z); }
  // #CAT_Display set where camera is looking
  void          GetCameraUp(float& x, float& y, float& z) { up.GetXYZ(x,y,z); }
  // #CAT_Display set where camera is looking
#else // TA_QT3D
  void          SetCameraOrient(float x, float y, float z, float r) { orient.SetXYZR(x,y,z,r); }
  // #CAT_Display set camera orientation -- x,y,z axis and r rotation value
  void          SetCameraFocDist(float fd) { focal_dist = fd; }
  // #CAT_Display set camera focal distance

  void          GetCameraOrient(float& x, float& y, float& z, float& r) { orient.GetXYZR(x,y,z,r); }
  // #CAT_Display set camera orientation -- x,y,z axis and r rotation value
  void          GetCameraFocDist(float& fd) { fd = focal_dist; }
  // #CAT_Display set camera focal distance
#endif // TA_QT3D

  QToolButton*  view_button;    // #IGNORE view button for this view
  iAction*    view_action;    // #IGNORE action for the gotoview function

  TA_SIMPLE_BASEFUNS(T3SavedView);
protected:
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // T3SavedView_h
