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

#ifdef _WINDOWS
# define _USE_MATH_DEFINES
#endif
#ifndef __MAKETA__
#include <Inventor/nodes/SoCylinder.h>
#endif

#include "ta_def.h"
#include "ta_TA_type_WRAPPER.h"

class SoCylinder; // #IGNORE
class SoGetPrimitiveCountAction; // #IGNORE
class SoAction;  // #IGNORE
class SoGLRenderAction; // #IGNORE

class TA_API SoCapsule : public SoCylinder {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS a cylinder with spheres at the ends
#ifndef __MAKETA__
  typedef SoCylinder inherited;
  SO_NODE_HEADER(SoCapsule);
#endif

public:
  static void initClass();
  SoCapsule();

  virtual void getPrimitiveCount(SoGetPrimitiveCountAction * action);
  virtual void GLRender(SoGLRenderAction * action);

protected:
  virtual ~SoCapsule();

  virtual void generatePrimitives(SoAction * action);

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
