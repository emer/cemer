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

#ifndef taMatrix_Group_h
#define taMatrix_Group_h 1

// parent includes:
#include <taMatrix>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taMatrix_Group);

class TA_API taMatrix_Group: public taGroup<taMatrix> {
  // ##CAT_Data group that can hold matrix items -- typically used for dataset elements
INHERITED(taGroup<taMatrix>)
public:

  TA_BASEFUNS_NOCOPY(taMatrix_Group);
private:
  void          Initialize() {SetBaseType(&TA_taMatrix);}
  void          Destroy() {}
};

#endif // taMatrix_Group_h
