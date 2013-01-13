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

#ifndef T3Misc_h
#define T3Misc_h 1

// parent includes:
#include "ta_def.h"

// member includes:

// declare all other types mentioned but not required to include:

class TA_API T3Misc {
// global params and funcs for T3 viewing system
public:
  static const float    pts_per_geom; // #DEF_72 chars*pt size / so unit
  static const float    geoms_per_pt; // number of geoms per point (1/72)
  static const float    char_ht_to_wd_pts; // ratio of ht to wd in metrics: 12/8
  static const float    char_base_fract; // fraction of total ht below baseline
};

#endif // T3Misc_h
