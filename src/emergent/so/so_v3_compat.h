// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef so_v3_compat_h
#define so_v3_compat_h

#include "so.h"
#include "v3_compat.h"

class SO_API SoTrial : public TrialProcess {
  // one self-organizing trial step
INHERITED(TrialProcess)
public:
  
  void	Initialize()		{ };
  void	Destroy()		{ };
  TA_BASEFUNS_NOCOPY(SoTrial);
};

class SO_API V3SoProject : public V3ProjectBase {
  // self-organizing learning project
INHERITED(V3ProjectBase)
public:

  override bool	ConvertToV4_impl(); 

  void	Initialize() {};
  void	Destroy() 	{ };
  TA_BASEFUNS(V3SoProject);
};

#endif // so_v3_compat_h
