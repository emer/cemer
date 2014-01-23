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

#ifndef GpOneToManyPrjnSpec_h
#define GpOneToManyPrjnSpec_h 1

// parent includes:
#include <OneToOnePrjnSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(GpOneToManyPrjnSpec);

class E_API GpOneToManyPrjnSpec : public OneToOnePrjnSpec {
  // unit_group based one-to-many connectivity (one sending gp to all recv units) -- only works with explicit unit groups, not virtual unit groups
INHERITED(OneToOnePrjnSpec)
public:
  enum NConGroups {		// number of connection groups for this projection
    RECV_SEND_PAIR,		// create separate con_groups for each recv_send pair
    SEND_ONLY,			// create separate recv con_groups for ea sending gp
    ONE_GROUP 			// make only one con_group
  };

  NConGroups	n_con_groups;	// number of con_groups to create

  virtual void 	GetNGroups(Projection* prjn, int& r_n_ugp, int& s_n_ugp);
  // get number of connection groups for the projection

  void	PreConnect(Projection* prjn) CPP11_OVERRIDE;
  void	Connect_impl(Projection* prjn) CPP11_OVERRIDE;

  TA_SIMPLE_BASEFUNS(GpOneToManyPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // GpOneToManyPrjnSpec_h
