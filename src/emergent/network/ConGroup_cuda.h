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

#ifndef ConGroup_cuda_h
#define ConGroup_cuda_h 1


// parent includes:
#include <ConGroup_core>

// member includes:

// declare all other types mentioned but not required to include:

class  ConGroup_cuda : public taNBase {
  // <describe here in full detail in one extended line comment>
INHERITED(taNBase)
public:

  TA_SIMPLE_BASEFUNS(ConGroup_cuda);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // ConGroup_cuda_h
