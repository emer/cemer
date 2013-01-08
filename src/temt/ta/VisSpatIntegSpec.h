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

#ifndef VisSpatIntegSpec_h
#define VisSpatIntegSpec_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <taVector2i>

// declare all other types mentioned but not required to include:


class TA_API VisSpatIntegSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image spatial integration parameters for visual signals -- happens as last step after all other feature detection operations -- performs a MAX or AVG over rfields
INHERITED(taOBase)
public:
  taVector2i	spat_rf;	// integrate over this many spatial locations (uses MAX operator over gaussian weighted filter matches at each location) in computing the response of the v1c cells -- produces a larger receptive field -- always uses 1/2 overlap spacing
  float		gauss_sig;	// #DEF_0.8 gaussian sigma for spatial rf -- weights the contribution of more distant locations more weakly
  bool		sum_rf;		// #DEF_false sum over the receptive field instead of computing the max (actually computes the average instead of sum)

  taVector2i	spat_half;	// #READ_ONLY half rf
  taVector2i	spat_spacing;	// #READ_ONLY 1/2 overlap spacing with spat_rf
  taVector2i	spat_border;	// #READ_ONLY border onto v1s filters -- automatically computed based on wrap mode and spacing setting

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(VisSpatIntegSpec);
protected:
  void 	UpdateAfterEdit_impl();
};

#endif // VisSpatIntegSpec_h
