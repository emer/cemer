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

#ifndef OneToOnePrjnSpec_h
#define OneToOnePrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(OneToOnePrjnSpec);

class E_API OneToOnePrjnSpec : public ProjectionSpec {
  // one-to-one connectivity (1st unit to 1st unit, etc)
INHERITED(ProjectionSpec)
public:
  int	n_conns;		// number of connections to make (-1 for size of layer)
  int	recv_start;		// starting unit index for recv connections
  int 	send_start;		// starting unit index for sending connections
  bool  use_gp;                 // if unit groups are present in the recv or sender layer, and the other layer fits within one unit group, then connectivity will be replicated for across groups in that layer

  void	Connect_impl(Projection* prjn) override;
  virtual void	ConnectRecvGp_impl(Projection* prjn); // recv is using groups
  virtual void	ConnectSendGp_impl(Projection* prjn); // send is using groups

  TA_SIMPLE_BASEFUNS(OneToOnePrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // OneToOnePrjnSpec_h
