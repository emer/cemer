// Copyright 2017, Regents of the University of Colorado,
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

#include "taTransform.h"

TA_BASEFUNS_CTORS_LITE_DEFN(taTransform);

#ifdef TA_QT3D

#include <T3Entity>

void taTransform::CopyTo(T3Entity* ent) {
  if (!ent) return;
  ent->Translate(translate);
  ent->RotateRad(rotate, rotate.rot);
  ent->Scale3D(scale);
}

#else // TA_QT3D

#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoTransform.h>

void taTransform::CopyTo(SoTransform* txfm) {
  if (!txfm) return;
  txfm->translation.setValue(SbVec3f(translate.x, translate.y, translate.z));
  txfm->rotation.setValue(SbVec3f(rotate.x, rotate.y, rotate.z), rotate.rot);
  txfm->scaleFactor.setValue(SbVec3f(scale.x, scale.y, scale.z));
}

#endif // TA_QT3D
