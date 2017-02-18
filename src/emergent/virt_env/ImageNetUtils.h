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

#ifndef ImageNetUtils_h
#define ImageNetUtils_h 1

// parent includes:
#include "network_def.h"

// member includes:
#include <taString>
#include <taVector2i>
#include <taVector3i>

#include <QXmlStreamReader>

// declare all other types mentioned but not required to include:

class E_API ImageNetUtils {
  // methods to parse ImageNet annotation files that accompany ImageNet images

public:
  virtual void          GetSize(const String& filename, taVector3i& width_height_depth);
  virtual void          GetBoundingBox(const String& filename, taVector2i& top_left, taVector2i& bottom_right);

};

#endif // ImageNetUtils_h
