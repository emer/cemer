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

#ifndef V1ComplexSpec_h
#define V1ComplexSpec_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API V1ComplexSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 complex cells, which integrate over v1 simple polarity invariant inputs to compute length sum and end stopping detectors
INHERITED(taOBase)
public:
  bool		sg4;		// #DEF_false #AKA_pre_gp4 use a 4x4 square grouping of v1s features prior to computing subsequent steps (length sum, end stop) -- this square grouping provides more isotropic coverage of the space, reduces the computational cost of subsequent steps, and also usefully makes it more robust to minor variations -- size must be even due to half-overlap for spacing requirement, so 4x4 is only size that makes sense
  bool		spc4;		// #DEF_true #CONDSHOW_ON_sg4 use 4x4 spacing for square grouping, instead of half-overlap 2x2 spacing -- this results in greater savings in computation, at some small cost in uniformity of coverage of the space
  int		len_sum_len;	// #DEF_1 length (in pre-grouping of v1s/b rf's) beyond rf center (aligned along orientation of the cell) to integrate length summing -- this is a half-width, such that overall length is 1 + 2 * len_sum_len
  float		es_thr;		// #DEF_0.2 threshold for end stopping activation -- there are typically many "ghost" end stops, so this filters those out

  int		sg_rf;		// #READ_ONLY size of sg-grouping -- always 4 for now, as it is the only thing that makes sense
  int		sg_half;	// #READ_ONLY sg_rf / 2
  int		sg_spacing;	// #READ_ONLY either 4 or 2 depending on spc4
  int		sg_border;	// #READ_ONLY border onto v1s filters -- automatically computed based on wrap mode and spacing setting

  int		len_sum_width;	// #READ_ONLY 1 + 2 * len_sum_len -- computed
  float		len_sum_norm;	// #READ_ONLY 1.0 / len_sum_width -- normalize sum

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1ComplexSpec);
protected:
  void 	UpdateAfterEdit_impl();
};

#endif // V1ComplexSpec_h
