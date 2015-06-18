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

#ifndef taVector2i_List_h
#define taVector2i_List_h 1

// parent includes:
#include <taList>
#include <taVector2i>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(taVector2i_List);

class TA_API taVector2i_List : public taList<taVector2i> {
  // ##NO_TOKENS #NO_UPDATE_AFTER 
INHERITED(taList<taVector2i>)
public:

  TA_BASEFUNS_NOCOPY(taVector2i_List);
private:
void Initialize()  { SetBaseType(&TA_taVector2i);};
void Destroy()     { };
};

#endif // taVector2i_List_h
