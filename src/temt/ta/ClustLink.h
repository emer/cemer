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

#ifndef ClustLink_h
#define ClustLink_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:
class ClustNode; // 


class TA_API ClustLink : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Math a link in the cluster tree with distance
INHERITED(taBase)
public:
  float		dist;		// distance to this node from parent
  ClustNode*	node;		// cluster node

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	CutLinks();
  void	Copy_(const ClustLink& cp);
  TA_BASEFUNS_LITE(ClustLink);
};

#endif // ClustLink_h
