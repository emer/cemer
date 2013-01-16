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

#ifndef NetCounterInit_h
#define NetCounterInit_h 1

// parent includes:
#include <NetCounterBase>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(NetCounterInit);

class EMERGENT_API NetCounterInit: public NetCounterBase { 
  // initialize a network counter: program keeps a local version of the counter, and updates both this and the network's copy
INHERITED(NetCounterBase)
public:
  override String	GetDisplayName() const;
  override String	GetToolbarName() const { return "net ctr init"; }

  PROGEL_SIMPLE_BASEFUNS(NetCounterInit);
protected:
  override void		GenCssBody_impl(Program* prog);

private:
  void	Initialize() { };
  void	Destroy() { };
};

#endif // NetCounterInit_h
