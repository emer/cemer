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

#ifndef ImgProcThreadBase_h
#define ImgProcThreadBase_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ImgProcCallThreadMgr>

// declare all other types mentioned but not required to include:


class TA_API ImgProcThreadBase : public taNBase {
  // #VIRT_BASE ##CAT_Image base class for image-processing code that uses threading -- defines a basic interface for thread calls to deploy filtering or other intensive computations
  INHERITED(taNBase)
public:
  ImgProcCallThreadMgr threads; // #CAT_Threads parallel threading of image processing computation

  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(ImgProcThreadBase);
};

#endif // ImgProcThreadBase_h
