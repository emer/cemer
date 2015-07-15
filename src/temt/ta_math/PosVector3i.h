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

#ifndef PosVector3i_h
#define PosVector3i_h 1

// parent includes:
#include <taVector3i>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(PosVector3i);

class TA_API PosVector3i : public taVector3i {
  // #NO_UPDATE_AFTER #INLINE #INLINE_DUMP positive-only value in 3D coordinate space
  INHERITED(taVector3i)
public:
  TA_BASEFUNS_LITE(PosVector3i);

  inline PosVector3i& operator=(const taVector3i& cp) { x = cp.x; y = cp.y; z = cp.z; UpdateAfterEdit(); return *this;}
  inline PosVector3i& operator=(int cp)  { x = cp; y = cp; z = cp; return *this;}
  inline PosVector3i& operator=(float cp) { x = (int)cp; y = (int)cp; z = (int)cp; return *this;}
  inline PosVector3i& operator=(double cp) { x = (int)cp; y = (int)cp; z = (int)cp; return *this;}
protected:
  void  UpdateAfterEdit_impl();
private:
  NOCOPY(PosVector3i)
  void  Initialize()    { }
  void  Destroy()       { };
};

typedef PosVector3i PosTDCoord;		// #INSTANCE obsolete version of PosVector3i

#endif // PosVector3i_h
