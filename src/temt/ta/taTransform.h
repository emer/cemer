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

#ifndef taTransform_h
#define taTransform_h 1

// parent includes:
#include <taBase>

// member includes:
#include <taVector3f>
#include <taAxisAngle>

// declare all other types mentioned but not required to include:
class SoTransform; // #IGNORE

class TA_API taTransform: public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math 3-d transformation data; applied in order: s, r, t
  INHERITED(taBase)
public:
  taVector3f          scale; // scale factors, in x, y, and z
  taAxisAngle         rotate; // rotation
  taVector3f          translate; // translate, in x, y, and z

  TA_BASEFUNS_LITE(taTransform);
#ifdef TA_USE_INVENTOR
  void                  CopyTo(SoTransform* txfm); // #IGNORE txfers values to an inventor txfm -- note, does a transfer, not an accumulate
#endif
private:
  void                  Copy_(const taTransform& cp)
    {scale.Copy(cp.scale); rotate.Copy(cp.rotate); translate.Copy(cp.translate);}
  void                  Initialize() {scale = 1.0f;}
  void                  Destroy() {}
};

typedef taTransform FloatTransform;	// #INSTANCE obsolete version of taTransform

#endif // taTransform_h
