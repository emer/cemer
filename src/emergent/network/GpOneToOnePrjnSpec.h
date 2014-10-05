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

#ifndef GpOneToOnePrjnSpec_h
#define GpOneToOnePrjnSpec_h 1

// parent includes:
#include <OneToOnePrjnSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(GpOneToOnePrjnSpec);

class E_API GpOneToOnePrjnSpec : public OneToOnePrjnSpec {
  // unit_group based one-to-one connectivity, with full connectivity within unit groups (all in 1st group to all in 1st group, etc) -- if one layer has same number of units as the other does unit groups, then each unit connects to entire unit group
INHERITED(OneToOnePrjnSpec)
public:
#ifdef __MAKETA__
  int	n_conns;		// number of unit groups to connect (-1 for size of layer)
  int	recv_start;		// starting unit group index for recv layer connections -- start making connections in this group -- index goes through x dimension first (inner) then y dimension (outer)
  int	send_start;		// starting unit group index for send layer connections -- start making connections in this group -- index goes through x dimension first (inner) then y dimension (outer)
  bool  use_gp;                 // #HIDDEN this is not used for GpOneToOnePrjnSpec
#endif


  void	Connect_impl(Projection* prjn, bool make_cons) override;

  virtual void	Connect_RecvUnitsSendGps(Projection* prjn, bool make_cons);
  // #IGNORE
  virtual void	Connect_SendUnitsRecvGps(Projection* prjn, bool make_cons);
  // #IGNORE 

  TA_BASEFUNS_NOCOPY(GpOneToOnePrjnSpec);
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
};

#endif // GpOneToOnePrjnSpec_h
