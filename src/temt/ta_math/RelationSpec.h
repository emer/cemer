// Copyright 2015, Regents of the University of Colorado,
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

#ifndef RelationSpec_h
#define RelationSpec_h 1

// parent includes:
#include <Relation>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(RelationSpec);

class TA_API RelationSpec : public Relation {
  // counting criteria params -- use this for inline members (hides name field)
  INHERITED(Relation)
public:
#ifdef __MAKETA__
  String        name;           // #HIDDEN_INLINE name of the object
#endif
  void  Initialize()            { };
  void  Destroy()               { };
  TA_BASEFUNS_NOCOPY(RelationSpec);
};

#endif // RelationSpec_h
