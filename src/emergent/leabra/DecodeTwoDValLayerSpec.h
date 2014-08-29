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

#ifndef DecodeTwoDValLayerSpec_h
#define DecodeTwoDValLayerSpec_h 1

// parent includes:
#include <TwoDValLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(DecodeTwoDValLayerSpec);

class E_API DecodeTwoDValLayerSpec : public TwoDValLayerSpec {
  // a two-d-value layer spec that copies its activations from one-to-one input prjns, to act as a decoder of another layer
INHERITED(TwoDValLayerSpec)
public:
  void ReadValue_ugp(TwoDValLeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
	                 LeabraNetwork* net) override;
  void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) override;

  // don't do any learning:
  bool	Compute_dWt_Test(LeabraLayer* lay, LeabraNetwork* net) override
  { return false; }

  TA_BASEFUNS_NOCOPY(DecodeTwoDValLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};

#endif // DecodeTwoDValLayerSpec_h
