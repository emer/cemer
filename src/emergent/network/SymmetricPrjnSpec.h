// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef SymmetricPrjnSpec_h
#define SymmetricPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(SymmetricPrjnSpec);

class E_API SymmetricPrjnSpec : public ProjectionSpec {
  // connects units with receiving connection where sending one already exists
INHERITED(ProjectionSpec)
public:
  void Connect_impl(Projection* prjn) override;

  TA_BASEFUNS_NOCOPY(SymmetricPrjnSpec);
private:
  void	Initialize()	{ };
  void	Destroy()	{ };
};

#endif // SymmetricPrjnSpec_h
