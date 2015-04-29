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

#ifndef BgPfcPrjnSpec_h
#define BgPfcPrjnSpec_h 1

// parent includes:
#include <GpCustomPrjnSpecBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(BgPfcPrjnSpec);

class E_API BgPfcPrjnSpec : public GpCustomPrjnSpecBase {
  // for connecting BG and PFC layers, where there are separate PFCmnt and PFCout layers that interconnect with a single BG layer (Matrix, GPi, etc), allowing competition within the BG -- uses layer names to automatically do the right thing -- looks for 'pfc'|'acc'|'ofc' and 'mnt' and 'out' strings to identify which side is the pfc, and assumes that mnt comprises the left-most set of unit groups, while out is on the right 
INHERITED(GpCustomPrjnSpecBase)
public:
  bool  cross_mnt_out;          // if true, then route maint to out or vice-versa
  
  void	Connect_impl(Projection* prjn, bool make_cons) override;

  virtual bool CheckLayerGeoms(Layer* bg_layer, Layer* pfc_layer);
  // check the layer geometries

  TA_SIMPLE_BASEFUNS(BgPfcPrjnSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // BgPfcPrjnSpec_h
