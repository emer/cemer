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

#ifndef DoGRetinaProc_h
#define DoGRetinaProc_h 1

// parent includes:
#include <RetinaProc>

// member includes:

// declare all other types mentioned but not required to include:
class VisRegionSpecBase; // 


class TA_API DoGRetinaProc : public RetinaProc {
  // Difference-of-Gaussians version of retinal filtering -- takes raw input images, applies various transforms, and then runs through filtering -- first region is used for retina size and other basic params
INHERITED(RetinaProc)
public:
  virtual VisRegionSpecBase* AddRegion()
  { return (VisRegionSpecBase*)regions.New(1, &TA_DoGRegionSpec); }

  void 	Initialize();
  void	Destroy() { };
  TA_BASEFUNS_NOCOPY(DoGRetinaProc);
};

#endif // DoGRetinaProc_h
