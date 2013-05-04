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

#ifndef taINBase_h
#define taINBase_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(taINBase);

class TA_API taINBase : public taNBase {
  // #NO_TOKENS a named, owned base class of taBase, for simple classes where the name should be hidden when it appears inline in an editor
INHERITED(taNBase)
public:
#ifdef __MAKETA__
  String                name; // #CONDEDIT_OFF_base_flags:NAME_READONLY #CAT_taBase #HIDDEN_INLINE name of the object
#endif

  TA_BASEFUNS_NOCOPY(taINBase);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // taINBase_h
