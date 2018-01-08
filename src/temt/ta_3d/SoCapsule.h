// Copyright 2007-2018, Regents of the University of Colorado,
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

#ifndef COIN_SOCAPSULE_H
#define COIN_SOCAPSULE_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2007 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See http://www.coin3d.org/ for more information.
 *
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  http://www.sim.no/  sales@sim.no  coin-support@coin3d.org
 *
\**************************************************************************/


// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <Inventor/nodes/SoCylinder.h>
#endif

#include <T3Node>

// member includes:

// declare all other types mentioned but not required to include:

class SoCylinder; // #IGNORE
class SoGetPrimitiveCountAction; // #IGNORE
class SoAction;  // #IGNORE
class SoGLRenderAction; // #IGNORE


taTypeDef_Of(SoCapsule);

class TA_API SoCapsule : public SoCylinder {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS a cylinder with spheres at the ends
#ifndef __MAKETA__
  typedef SoCylinder inherited;
  TA_SO_NODE_HEADER(SoCapsule);
#endif

public:
  static void initClass();
  SoCapsule();

  virtual void getPrimitiveCount(SoGetPrimitiveCountAction * action) override;  // #IGNORE 
  virtual void GLRender(SoGLRenderAction * action) override; // #IGNORE 

protected:
  virtual ~SoCapsule();

  virtual void generatePrimitives(SoAction * action) override;

  void generate_cylinder(const float radius,
                            const float height,
                            const int numslices,
                            const unsigned int flags,
                            SoAction * action);

  void generate_half_sphere(const float radius,
                            const float offset,
                            const int numstacks,
                            const int numslices,
                            const unsigned int flags,
                            SoAction * action);
};

#endif // !COIN_SOCAPSULE_H
