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

#ifndef ActrBuffer_List_h
#define ActrBuffer_List_h 1

// parent includes:
#include <ActrBuffer>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrBuffer_List);

class E_API ActrBuffer_List : public taList<ActrBuffer> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_ActR list of ActR buffers
INHERITED(taList<ActrBuffer>)
public:

  override String 	GetTypeDecoKey() const { return "DataTable"; }

  TA_BASEFUNS_NOCOPY(ActrBuffer_List);
private:
  void Initialize();
  void Destroy()     { Reset(); }
};

#endif // ActrBuffer_List_h
