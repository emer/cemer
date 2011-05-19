// Copyright, 1995-2011, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#ifndef T3_BRAIN_NODE_H
#define T3_BRAIN_NODE_H

#include "netstru_so.h" // overkill #include

class SoIndexedFaceSet; // #IGNORE

class EMERGENT_API T3BrainNode: public T3NodeLeaf {
#ifndef __MAKETA__
  typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3BrainNode);
#endif

public:
  static void		initClass();

  SoIndexedFaceSet*     shape();
  SoVertexProperty* 	vtxProp() {return vtx_prop_;}

  T3BrainNode(T3DataView* dataView_ = NULL);

protected:
  ~T3BrainNode();

protected:
  SoIndexedFaceSet*     shape_; //#IGNORE
  SoVertexProperty*	vtx_prop_;

};

#endif // T3_BRAIN_NODE_H
