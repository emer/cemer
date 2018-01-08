// Co2018ght 2017-2017, Regents of the University of Colorado,
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

#ifndef LeabraExtraUnitSpecs_cuda_h
#define LeabraExtraUnitSpecs_cuda_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraExtraUnitSpecs_cuda);

class E_API LeabraExtraUnitSpecs_cuda : public taNBase {
  // <describe here in full detail in one extended line comment>
INHERITED(taNBase)
public:

  TA_SIMPLE_BASEFUNS(LeabraExtraUnitSpecs_cuda);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // LeabraExtraUnitSpecs_cuda_h
