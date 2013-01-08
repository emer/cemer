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

#ifndef ScriptBase_List_h
#define ScriptBase_List_h 1

// parent includes:
#include <AbstractScriptBase>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API ScriptBase_List : public taPtrList<AbstractScriptBase> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER list of script base objects (doesn't own anything)
public:
  ~ScriptBase_List()    { Reset(); }
};

#endif // ScriptBase_List_h
