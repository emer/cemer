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

#ifndef MarkerGpOneToOnePrjnSpec_h
#define MarkerGpOneToOnePrjnSpec_h 1

// parent includes:
#include <OneToOnePrjnSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(MarkerGpOneToOnePrjnSpec);

class E_API MarkerGpOneToOnePrjnSpec : public OneToOnePrjnSpec {
  // unit_group based one-to-one connectivity for marking a projection -- all of the recv units receive from just the first unit in the sending unit group, thus providing a marker for where to receive information from the sending group
INHERITED(OneToOnePrjnSpec)
public:
  void	Connect_impl(Projection* prjn) override;

  TA_SIMPLE_BASEFUNS(MarkerGpOneToOnePrjnSpec);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // MarkerGpOneToOnePrjnSpec_h
