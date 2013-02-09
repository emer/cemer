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

#ifndef T3BrainNode_h
#define T3BrainNode_h 1

// parent includes:
#include "network_def.h"
#include <T3NodeLeaf>

// member includes:

// declare all other types mentioned but not required to include:
class SoSeparator;      // #IGNORE
class SoIndexedFaceSet; // #IGNORE
class SoVertexProperty; // #IGNORE
class SoMaterial;       // #IGNORE
class SoTransform;      // #IGNORE

class E_API T3BrainNode: public T3NodeLeaf {
#ifndef __MAKETA__
  typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3BrainNode);
#endif
public:
  static void		initClass();

  SoSeparator*          brain_group; //#IGNORE
  SoMaterial**          brain_tex_mat_array; //#IGNORE
  SoVertexProperty**    atlas_vrtx_prop_array; //#IGNORE
  SoVertexProperty**    voxel_vrtx_prop_array; //#IGNORE
  SoIndexedFaceSet**    atlas_face_set_array; //#IGNORE
  SoIndexedFaceSet**    voxel_face_set_array; //#IGNORE
  SoTransform*          spacing_xform; //#IGNORE

  T3BrainNode(T3DataView* dataView_ = NULL);

protected:
  ~T3BrainNode();

};

#endif // T3BrainNode_h
