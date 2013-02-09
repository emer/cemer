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

#ifndef RandomSpec_h
#define RandomSpec_h 1

// parent includes:
#include <Random>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(RandomSpec);

class TA_API RandomSpec : public Random {
  // Random Number Generation -- use this for inline members (hides name field)
INHERITED(Random)
public:
#ifdef __MAKETA__
  String        name;           // #HIDDEN_INLINE name of the object
#endif
  void  Initialize()            { };
  void  Destroy()               { };
  TA_BASEFUNS_NOCOPY(RandomSpec);
};

#endif // RandomSpec_h
