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

#ifndef V1MotionSpec_h
#define V1MotionSpec_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API V1MotionSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 motion coding by simple cells
INHERITED(taOBase)
public:
  bool		r_only;		// #DEF_true perform motion computation only on right eye signals -- this is the dominant eye and often all that is needed
  int		n_speeds;	// #DEF_1 for motion coding, number of speeds in each direction to encode separately -- only applicable if motion_frames > 1
  int		speed_inc;	// #DEF_1 how much to increase speed for each speed value -- how fast is the slowest speed basically
  int		tuning_width;	// #DEF_1 additional width of encoding around the trajectory for the target speed -- allows for some fuzziness in encoding -- effective value is multiplied by speed, so it gets fuzzier as speed gets higher
  float		gauss_sig;	// #DEF_0.8 gaussian sigma for weighting the contribution of extra width guys -- normalized by effective tuning_width
  float		opt_thr;	// #DEF_0.01 optimization threshold -- skip if current value is below this value

  int		tot_width;	// #READ_ONLY total width = 1 + 2 * tuning_width

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1MotionSpec);
protected:
  void 	UpdateAfterEdit_impl();
};

#endif // V1MotionSpec_h
