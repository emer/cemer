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

#ifndef SNrPrjnSpec_h
#define SNrPrjnSpec_h 1

// parent includes:
#include <GpCustomPrjnSpecBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(SNrPrjnSpec);

class E_API SNrPrjnSpec : public GpCustomPrjnSpecBase {
  // SNrThal projection -- automatically deals with the convergence and divergence of connectivity between gating-specific layers in either Matrix or PFC (INPUT, IN_MNT, OUTPUT, etc.) and the SNrThal which represents all gating types in one layer 
INHERITED(GpCustomPrjnSpecBase)
public:
  void	Connect_impl(Projection* prjn);

  TA_BASEFUNS_NOCOPY(SNrPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // SNrPrjnSpec_h
