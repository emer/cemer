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

#include "T3SavedView.h"

#ifdef TA_QT3D

#else // TA_QT3D

#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>

#endif // TA_QT3D

TA_BASEFUNS_CTORS_DEFN(T3SavedView);

void T3SavedView::Initialize() {
  view_saved = false;
  view_button = NULL;
  view_action = NULL;
  pos = 0.0f;
#ifdef TA_QT3D
  pos.z = -5.0f;
  up = 0.0f; up.y = 1.0f;
  look_at = 0.0f;
#else // TA_QT3D
  focal_dist = 0.0f;
#endif // TA_QT3D
}

#ifdef TA_QT3D

void T3SavedView::getCameraParams(Qt3D::QCamera* cam) {
  pos = cam->position();
  look_at = cam->viewCenter();
  up = cam->upVector();
  view_saved = true;
}

bool T3SavedView::setCameraParams(Qt3D::QCamera* cam) {
  if(!view_saved) return false;
  cam->setPosition(pos);
  cam->setViewCenter(look_at);
  cam->setUpVector(up);
  return true;
}

#else // TA_QT3D

void T3SavedView::getCameraParams(SoCamera* cam) {
  SbVec3f sb_pos = cam->position.getValue();
  SbVec3f sb_axis;
  cam->orientation.getValue(sb_axis, orient.rot);
  focal_dist = cam->focalDistance.getValue();

  pos.x = sb_pos[0]; pos.y = sb_pos[1]; pos.z = sb_pos[2];
  orient.x = sb_axis[0]; orient.y = sb_axis[1]; orient.z = sb_axis[2];

  view_saved = true;
}

bool T3SavedView::setCameraParams(SoCamera* cam) {
  if(!view_saved) return false;
  cam->position.setValue(pos.x, pos.y, pos.z);
  cam->orientation.setValue(SbVec3f(orient.x, orient.y, orient.z), orient.rot);
  cam->focalDistance.setValue(focal_dist);
  return true;
}

#endif // TA_QT3D
