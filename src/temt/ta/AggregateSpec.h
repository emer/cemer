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

#ifndef AggregateSpec_h
#define AggregateSpec_h 1

// parent includes:
#include <Aggregate>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API AggregateSpec : public Aggregate {
  //  Basic aggregation operations -- use this for inline members (hides name field)
  INHERITED(Aggregate)
public:
#ifdef __MAKETA__
  String        name;           // #HIDDEN_INLINE name of the object
#endif
  void  Initialize()            { };
  void  Destroy()               { };
  TA_BASEFUNS_NOCOPY(AggregateSpec);
};

#endif // AggregateSpec_h
