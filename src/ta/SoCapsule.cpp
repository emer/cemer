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

// Note: the class documentation for the basic primitive shapes
// SoSphere, SoCapsule, SoCone and SoCube have many common, or at
// least close to common, paragraphs. If you make any changes, check
// those other shapes too, to see if your updates / fixes should be
// migrated. <mortene@sim.no>.
/*!
  \class SoCapsule SoCapsule.h Inventor/nodes/SoCapsule.h
  \brief The SoCapsule class is for rendering capsule shapes.
  \ingroup nodes

  Insert a capsule shape into the scenegraph. The capsule is
  rendered with the current material, texture and drawstyle settings
  (if any, otherwise the default settings are used).

  The SoCapsule node class is provided as a convenient abstraction
  for the application programmer to use "complex" shapes of this type
  without having to do the tessellation to polygons and other low-level
  programming herself.

  A capsule is visualized by the underlying rendering system by first
  tessellating the conceptual capsule into a set of polygons. To
  control the trade-off between an as much as possible correct visual
  appearance of the capsule versus fast rendering, use an
  SoComplexity node to influence the number of polygons generated from
  the tessellation process. (The higher the complexity value, the more
  polygons will be generated, the more \e rounded the sides of the
  capsule will look.) Set the SoComplexity::value field to what you
  believe would be a good trade-off between correctness and speed for
  your particular application.

  A nice trick for rendering a disc is to render an SoCapsule with
  SoCapsule::height set to zero:

  \code
  #Inventor V2.1 ascii
  
  ShapeHints { # to get two-sided lighting on the disc
     vertexOrdering COUNTERCLOCKWISE
     shapeType UNKNOWN_SHAPE_TYPE
  }
  
  Capsule {
     height 0
     parts TOP
  }
  \endcode

  <b>FILE FORMAT/DEFAULTS:</b>
  \code
    Capsule {
        radius 1
        height 2
        parts (SIDES | TOP | BOTTOM)
    }
  \endcode

  \sa SoCylinder, SoCone, SoSphere, SoCube
*/

#include "SoCapsule.h"
//#include <Inventor/nodes/SoCapsule.h>
//#include <Inventor/nodes/SoSubNodeP.h>

#include <Inventor/SbCylinder.h>
#include <Inventor/SbPlane.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetPrimitiveCountAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/details/SoCylinderDetail.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoGLTexture3EnabledElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoMultiTextureEnabledElement.h>
#include <Inventor/SoPrimitiveVertex.h>

#include <Inventor/C/glue/gl.h>

// #include <Inventor/misc/SoGL.h>
// #include <Inventor/misc/SoGenerate.h>
// #include <Inventor/misc/SoPick.h>
// #include <Inventor/misc/SoState.h>

// from SoGenerate.h
#define SOGEN_GENERATE_SIDE       0x01
#define SOGEN_GENERATE_TOP        0x02
#define SOGEN_GENERATE_BOTTOM     0x04
#define SOGEN_MATERIAL_PER_PART   0x08

// from SoGL.h
#define SOGL_RENDER_SIDE         0x01
#define SOGL_RENDER_TOP          0x02
#define SOGL_RENDER_BOTTOM       0x04
#define SOGL_MATERIAL_PER_PART   0x08
#define SOGL_NEED_NORMALS        0x10
#define SOGL_NEED_TEXCOORDS      0x20
#define SOGL_NEED_3DTEXCOORDS    0x40
#define SOGL_NEED_MULTITEXCOORDS 0x80 // internal

#include <math.h>
#include <iostream>

#define CYL_SIDE_NUMTRIS 30.0f
#define SPHERE_NUM_SLICES 30.0f
#define SPHERE_NUM_STACKS 30.0f

/*!
  \enum SoCapsule::Part
  The parts of a capsule shape.
*/


/*!
  \var SoSFFloat SoCapsule::radius
  Radius of capsule and its spherical ends. Default value 1.0.
*/
/*!
  \var SoSFFloat SoCapsule::height
  Height of capsule. Default is 3.0. This height includes the radius of the spherical ends, so that the minimum value is 2 * radius
*/
/*!
  \var SoSFBitMask SoCapsule::parts
  Which parts to use for rendering, picking, etc.  Defaults to
  SoCapsule::ALL.
*/


// *************************************************************************

SO_NODE_SOURCE(SoCapsule);

/*!
  Constructor.
*/
SoCapsule::SoCapsule(void)
{
  //  SO_NODE_INTERNAL_CONSTRUCTOR(SoCapsule);
  SO_NODE_CONSTRUCTOR(SoCapsule);
}

/*!
  Destructor.
*/
SoCapsule::~SoCapsule()
{
}

// Doc in parent.
void
SoCapsule::initClass(void)
{
  //  SO_NODE_INTERNAL_INIT_CLASS(SoCapsule, SO_FROM_INVENTOR_1|SoNode::VRML1);
  SO_NODE_INIT_CLASS(SoCapsule, SoCylinder, "SoCylinder");
}

//////////////////////////////////////////////////////////////////
//			GL

static const cc_glglue *
my_sogl_glue_instance(const SoState * state)
{
  SoGLRenderAction * action = (SoGLRenderAction *)state->getAction();
  // FIXME: disabled until we figure out why this doesn't work on some
  // Linux systems (gcc 3.2 systems, it seems). pederb, 2003-11-24
#if 0
  assert(action->isOfType(SoGLRenderAction::getClassTypeId()) &&
         "must have state from SoGLRenderAction to get hold of GL wrapper");
  return cc_glglue_instance(action->getCacheContext());
#else // disabled
  if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
    return cc_glglue_instance(action->getCacheContext());    
  }
  static int didwarn = 0;
  if (!didwarn) {
    didwarn = 1;
    std::cerr << "sogl_glue_instance: Wrong action type detected!" << std::endl;
  }
  // just return some cc_glglue instance. It usually doesn't matter
  // that much unless multiple contexts on multiple displays are used.
  return cc_glglue_instance(1); 
#endif // workaround version
}

// generate a 3d circle in the x-z plane
static void
my_sogl_generate_3d_circle(SbVec3f *coords, const int num, const float radius, const float y)
{
  float delta = 2.0f*float(M_PI)/float(num);
  float angle = 0.0f;
  for (int i = 0; i < num; i++) {
    coords[i][0] = -float(sin(angle)) * radius;
    coords[i][1] = y;
    coords[i][2] = -float(cos(angle)) * radius;
    angle += delta;
  }
}

// generate a 2d circle
static void
my_sogl_generate_2d_circle(SbVec2f *coords, const int num, const float radius)
{
  float delta = 2.0f*float(M_PI)/float(num);
  float angle = 0.0f;
  for (int i = 0; i < num; i++) {
    coords[i][0] = -float(sin(angle)) * radius;
    coords[i][1] = -float(cos(angle)) * radius;
    angle += delta;
  }
}

static void
my_sogl_render_cylinder(const float radius,
                     const float height,
                     const int numslices,
                     SoMaterialBundle * const material,
                     const unsigned int flagsin,
                     SoState * state)
{
  const SbBool * unitenabled = NULL;
  int maxunit = 0;
  const cc_glglue * glue = NULL;

  int flags = flagsin;

  if (state) {
    unitenabled = 
      SoMultiTextureEnabledElement::getEnabledUnits(state, maxunit);
    if (unitenabled) {
      glue = my_sogl_glue_instance(state);
      flags |= SOGL_NEED_MULTITEXCOORDS;
    }
    else maxunit = -1;
  }

  int i, u;
  int slices = numslices;
  if (slices > 128) slices = 128;
  if (slices < 4) slices = 4;

  float h2 = height * 0.5f;

  SbVec3f coords[129];
  SbVec3f normals[130];
  SbVec2f texcoords[129];

  my_sogl_generate_3d_circle(coords, slices, radius, -h2);
  coords[slices] = coords[0];
  if (flags & SOGL_NEED_3DTEXCOORDS ||
      (flags & SOGL_NEED_TEXCOORDS &&
       flags & (SOGL_RENDER_BOTTOM | SOGL_RENDER_TOP))) {
    my_sogl_generate_2d_circle(texcoords, slices, 0.5f);
    texcoords[slices] = texcoords[0];
  }

  if (flags & SOGL_NEED_NORMALS) {
    my_sogl_generate_3d_circle(normals, slices, 1.0f, 0.0f);
    normals[slices] = normals[0];
    normals[slices+1] = normals[1];
  }

  int matnr = 0;

  if (flags & SOGL_RENDER_SIDE) {
    glBegin(GL_QUAD_STRIP);
    i = 0;

    float t = 0.0;
    float inc = 1.0f / slices;

    while (i <= slices) {
      if (flags & SOGL_NEED_TEXCOORDS) {
        glTexCoord2f(t, 1.0f);
      }
      else if (flags & SOGL_NEED_3DTEXCOORDS) {
        glTexCoord3f(texcoords[i][0]+0.5f, 1.0f, 1.0f - texcoords[i][1]-0.5f);
      }
      if (flags & SOGL_NEED_NORMALS) {
        glNormal3fv((const GLfloat*)&normals[i]);
      }
      if (flags & SOGL_NEED_MULTITEXCOORDS) {
        for (u = 1; u <= maxunit; u++) {
          if (unitenabled[u]) {
            cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
                                        t, 1.0f);
          }
        }
      }

      SbVec3f c = coords[i];
      glVertex3f(c[0], h2, c[2]);
      if (flags & SOGL_NEED_TEXCOORDS) {
        glTexCoord2f(t, 0.0f);
      }
      else if (flags & SOGL_NEED_3DTEXCOORDS) {
        glTexCoord3f(texcoords[i][0]+0.5f, 0.0f, 1.0f - texcoords[i][1]-0.5f);
      }
      if (flags & SOGL_NEED_MULTITEXCOORDS) {
        for (u = 1; u <= maxunit; u++) {
          if (unitenabled[u]) {
            cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
                                        t, 0.0f);
          }
        }
      }
      glVertex3f(c[0], c[1], c[2]);
      i++;
      t += inc;
    }

    matnr++;
    glEnd();
  }

  if ((flags & (SOGL_NEED_TEXCOORDS|SOGL_NEED_3DTEXCOORDS|SOGL_NEED_MULTITEXCOORDS)) &&
      (flags & (SOGL_RENDER_BOTTOM | SOGL_RENDER_TOP))) {
    my_sogl_generate_2d_circle(texcoords, slices, 0.5f);
    texcoords[slices] = texcoords[0];
  }

  if (flags & SOGL_RENDER_TOP) {
    if (flags & SOGL_MATERIAL_PER_PART) {
      material->send(matnr, TRUE);
    }
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);

    for (i = 0; i < slices; i++) {
      if (flags & SOGL_NEED_TEXCOORDS) {
        glTexCoord2f(texcoords[i][0]+0.5f, 1.0f - texcoords[i][1]-0.5f);
      }
      else if (flags & SOGL_NEED_3DTEXCOORDS) {
        glTexCoord3f(texcoords[i][0]+0.5f, 1.0f, 1.0f - texcoords[i][1]-0.5f);
      }
      if (flags & SOGL_NEED_MULTITEXCOORDS) {
        for (u = 1; u <= maxunit; u++) {
          if (unitenabled[u]) {
            cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
                                       texcoords[i][0]+0.5f, 1.0f - texcoords[i][1]-0.5f);
          }
        }
      }
      const SbVec3f &c = coords[i];
      glVertex3f(c[0], h2, c[2]);
    }
    glEnd();
    matnr++;
  }
  if (flags & SOGL_RENDER_BOTTOM) {
    if (flags & SOGL_MATERIAL_PER_PART) {
      material->send(matnr, TRUE);
    }
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);

    for (i = slices-1; i >= 0; i--) {
      if (flags & SOGL_NEED_TEXCOORDS) {
        glTexCoord2f(texcoords[i][0]+0.5f, texcoords[i][1]+0.5f);
      }
      else if (flags & SOGL_NEED_3DTEXCOORDS) {
        glTexCoord3f(texcoords[i][0]+0.5f, 0.0f, 1.0f - texcoords[i][1]-0.5f);
      }
      if (flags & SOGL_NEED_MULTITEXCOORDS) {
        for (u = 1; u <= maxunit; u++) {
          if (unitenabled[u]) {
            cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
                                        texcoords[i][0]+0.5f, texcoords[i][1]+0.5f);
          }
        }
      }
      glVertex3fv((const GLfloat*)&coords[i]);
    }
    glEnd();
  }
  if (state && (SoComplexityTypeElement::get(state) ==
                SoComplexityTypeElement::OBJECT_SPACE)) {
    // encourage auto caching for object space
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DO_AUTO_CACHE);
  }
}

static void
sogl_render_half_sphere(const float radius,
		       const float offset,
		       const int numstacks,
		       const int numslices,
		       SoMaterialBundle * const /* material */,
		       const unsigned int flagsin,
		       SoState * state)
{
  const SbBool * unitenabled = NULL;
  int maxunit = 0;
  const cc_glglue * glue = NULL;

  unsigned int flags = flagsin;

  if (state && (flags & SOGL_NEED_TEXCOORDS)) {
    unitenabled = 
      SoMultiTextureEnabledElement::getEnabledUnits(state, maxunit);
    if (unitenabled) {
      glue = my_sogl_glue_instance(state);
      flags |= SOGL_NEED_MULTITEXCOORDS;
    }
    else maxunit = -1;
  }
  
  int stacks = numstacks / 2;
  int slices = numslices;

  if (stacks < 3) stacks = 3;
  if (slices < 4) slices = 4;

  if (slices > 128) slices = 128;

  // used to cache last stack's data
  SbVec3f coords[129];
  SbVec3f normals[129];
  float S[129];
  SbVec3f texcoords[129];

  int i, j, u;
  float rho;
  float drho;
  float theta;
  float dtheta;
  float tc, ts;
  SbVec3f tmp;

  drho = .5f * float(M_PI) / (float) (stacks-1); // full sphere is w/out .5
  dtheta = 2.0f * float(M_PI) / (float) slices;

  float currs = 0.0f;
  float incs = 1.0f / (float)slices;
  if(flags & SOGL_RENDER_TOP)
    rho = drho;
  else
    rho = float(M_PI) - drho;
  theta = 0.0f;
  tc = (float) cos(rho);
  ts = - (float) sin(rho);
  tmp.setValue(0.0f,
               tc,
               ts);
  normals[0] = tmp;
  texcoords[0] = tmp/2 + SbVec3f(0.5f,0.5f,0.5f);
  tmp *= radius;
  tmp[1] += offset;
  coords[0] = tmp;
  S[0] = currs;
  float dT = 1.0f / (float) (stacks-1);
  float T = 1.0f - dT;

  glBegin(GL_TRIANGLES);

  for (j = 1; j <= slices; j++) {
    glNormal3f(0.0f, 1.0f, 0.0f);
    if (flags & SOGL_NEED_TEXCOORDS) {
      glTexCoord2f(currs + 0.5f * incs, 1.0f);
    }
    else if (flags & SOGL_NEED_3DTEXCOORDS) {
      glTexCoord3f(0.5f, 1.0f, 0.5f);
    }
    if (flags & SOGL_NEED_MULTITEXCOORDS) {
      for (u = 1; u <= maxunit; u++) {
        if (unitenabled[u]) {
          cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
                                      currs + 0.5f * incs, 1.0f);
        }
      }
    }
    if(flags & SOGL_RENDER_TOP)
      glVertex3f(0.0f, radius + offset, 0.0f);
    else
      glVertex3f(0.0f, -radius + offset, 0.0f);

    glNormal3fv((const GLfloat*) &normals[j-1]);
    if (flags & SOGL_NEED_TEXCOORDS) {
      glTexCoord2f(currs, T);
    }
    else if (flags & SOGL_NEED_3DTEXCOORDS) {
      glTexCoord3fv((const GLfloat*) &texcoords[j-1]);
    }
    if (flags & SOGL_NEED_MULTITEXCOORDS) {
      for (u = 1; u <= maxunit; u++) {
        if (unitenabled[u]) {
          cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
                                      currs, T);
        }
      }
    }

    glVertex3fv((const GLfloat*) &coords[j-1]);

    currs += incs;
    theta += dtheta;
    tmp.setValue(float(sin(theta))*ts,
                 tc,
                 float(cos(theta))*ts);

    normals[j] = tmp;
    glNormal3fv((const GLfloat*)&normals[j]);
    if (flags & SOGL_NEED_TEXCOORDS) {
      S[j] = currs;
      glTexCoord2f(currs, T);
    }
    else if (flags & SOGL_NEED_3DTEXCOORDS) {
      texcoords[j] = tmp/2 + SbVec3f(0.5f,0.5f,0.5f);
      glTexCoord3fv((const GLfloat*) &texcoords[j]);
    }
    if (flags & SOGL_NEED_MULTITEXCOORDS) {
      for (u = 1; u <= maxunit; u++) {
        if (unitenabled[u]) {
          cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
                                      currs, T);
        }
      }
    }
    tmp *= radius;
    tmp[1] += offset;
    coords[j] = tmp;
    glVertex3fv((const GLfloat*)&coords[j]);
  }
  glEnd(); // GL_TRIANGLES

  if(flags & SOGL_RENDER_TOP)
    rho += drho;
  else
    rho -= drho;

  for (i = 1; i < stacks-1; i++) {
    tc = (float)cos(rho);
    ts = - (float) sin(rho);
    glBegin(GL_QUAD_STRIP);
    theta = 0.0f;
    for (j = 0; j <= slices; j++) {
      if (flags & SOGL_NEED_TEXCOORDS) {
        glTexCoord2f(S[j], T);
      }
      else if (flags & SOGL_NEED_3DTEXCOORDS) {
        glTexCoord3fv((const GLfloat*) &texcoords[j]);
      }
      if (flags & SOGL_NEED_MULTITEXCOORDS) {
        for (u = 1; u <= maxunit; u++) {
          if (unitenabled[u]) {
            cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
                                        S[j], T);
          }
        }
      }
      glNormal3fv((const GLfloat*)&normals[j]);
      glVertex3fv((const GLfloat*)&coords[j]);

      tmp.setValue(float(sin(theta))*ts,
                   tc,
                   float(cos(theta))*ts);
      if (flags & SOGL_NEED_TEXCOORDS) {
        glTexCoord2f(S[j], T - dT);
      }
      else if (flags & SOGL_NEED_3DTEXCOORDS) {
        texcoords[j] = tmp/2 + SbVec3f(0.5f,0.5f,0.5f);
        glTexCoord3fv((const GLfloat*) &texcoords[j]);
      }
      if (flags & SOGL_NEED_MULTITEXCOORDS) {
        for (u = 1; u <= maxunit; u++) {
          if (unitenabled[u]) {
            cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
                                        S[j], T - dT);
          }
        }
      }
      normals[j] = tmp;
      glNormal3f(tmp[0], tmp[1], tmp[2]);
      tmp *= radius;
      tmp[1] += offset;
      glVertex3f(tmp[0], tmp[1], tmp[2]);
      coords[j] = tmp;
      theta += dtheta;
    }
    glEnd(); // GL_QUAD_STRIP
    if(flags & SOGL_RENDER_TOP)
      rho += drho;
    else
      rho -= drho;
    T -= dT;
  }

  if(flags & SOGL_RENDER_SIDE) { // consider the flat bottom of the dome to be the "side"
    glBegin(GL_TRIANGLES);
    for (j = 0; j < slices; j++) {
      if (flags & SOGL_NEED_TEXCOORDS) {
	glTexCoord2f(S[j], T);
      }
      else if (flags & SOGL_NEED_3DTEXCOORDS) {
	glTexCoord3fv((const GLfloat*) &texcoords[j]);
      }
      if (flags & SOGL_NEED_MULTITEXCOORDS) {
	for (u = 1; u <= maxunit; u++) {
	  if (unitenabled[u]) {
	    cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
					S[j], T);
	  }
	}
      }
      glNormal3fv((const GLfloat*)&normals[j]);
      glVertex3fv((const GLfloat*)&coords[j]);

      if (flags & SOGL_NEED_TEXCOORDS) {
	glTexCoord2f(S[j]+incs*0.5f, 0.0f);
      }
      else if (flags & SOGL_NEED_3DTEXCOORDS) {
	glTexCoord3f(0.5f, 0.0f, 0.5f);
      }
      if (flags & SOGL_NEED_MULTITEXCOORDS) {
	for (u = 1; u <= maxunit; u++) {
	  if (unitenabled[u]) {
	    cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
					S[j]+incs*0.5f, 0.0f);
	  }
	}
      }
      glNormal3f(0.0f, -1.0f, 0.0f);
      glVertex3f(0.0f, offset, 0.0f);

      if (flags & SOGL_NEED_TEXCOORDS) {
	glTexCoord2f(S[j+1], T);
      }
      else if (flags & SOGL_NEED_3DTEXCOORDS) {
	glTexCoord3fv((const GLfloat*) &texcoords[j+1]);
      }
      if (flags & SOGL_NEED_MULTITEXCOORDS) {
	for (u = 1; u <= maxunit; u++) {
	  if (unitenabled[u]) {
	    cc_glglue_glMultiTexCoord2f(glue, (GLenum) (GL_TEXTURE0 + u),
					S[j+1], T);
	  }
	}
      }
      glNormal3fv((const GLfloat*)&normals[j+1]);
      glVertex3fv((const GLfloat*)&coords[j+1]);
    }
    glEnd(); // GL_TRIANGLES
  }

  if (state && (SoComplexityTypeElement::get(state) ==
                SoComplexityTypeElement::OBJECT_SPACE)) {
    // encourage auto caching for object space
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DO_AUTO_CACHE);
  }
}

void
SoCapsule::GLRender(SoGLRenderAction * action)
{
  if (!shouldGLRender(action)) return;

  SoState * state = action->getState();

  SoCylinder::Part p = (SoCylinder::Part) this->parts.getValue();
  SoMaterialBundle mb(action);
  mb.sendFirst();

  SbBool sendNormals = !mb.isColorOnly() || 
    (SoTextureCoordinateElement::getType(state) == SoTextureCoordinateElement::FUNCTION);

  unsigned int flags = 0;
  if (sendNormals)
    flags |= SOGL_NEED_NORMALS;
  if (SoGLTextureEnabledElement::get(state))
    flags |= SOGL_NEED_TEXCOORDS;
  else if (SoGLTexture3EnabledElement::get(state))
    flags |= SOGL_NEED_3DTEXCOORDS;
  
  SoMaterialBindingElement::Binding bind =
    SoMaterialBindingElement::get(state);
  if (bind == SoMaterialBindingElement::PER_PART ||
      bind == SoMaterialBindingElement::PER_PART_INDEXED)
    flags |= SOGL_MATERIAL_PER_PART;

  float complexity = this->getComplexityValue(action);

  float rad = this->radius.getValue();
  float ht = this->height.getValue();
  if(ht < 2.0f * rad) ht = 2.0f * rad;
  float cyl_ht = ht - 2.0f * rad;
  float offset = .5f * cyl_ht;

  if(p & TOP) {
    sogl_render_half_sphere(rad,
			   offset,
			   (int)(SPHERE_NUM_SLICES * complexity),
			   (int)(SPHERE_NUM_STACKS * complexity),
			   &mb,
			   flags | SOGL_RENDER_TOP, state);
  }

  if(p & BOTTOM) {
    sogl_render_half_sphere(rad,
			    -offset,
			    (int)(SPHERE_NUM_SLICES * complexity),
			    (int)(SPHERE_NUM_STACKS * complexity),
			    &mb,
			    flags | SOGL_RENDER_BOTTOM, state);
  }

  if(p & SIDES) {
    my_sogl_render_cylinder(rad,
			    cyl_ht,
			    (int)(CYL_SIDE_NUMTRIS * complexity),
			    &mb,
			    flags | SOGL_RENDER_SIDE, state);
  }
}

//////////////////////////////////////////////////////////////////////////

// Doc in parent.
void
SoCapsule::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  if (!this->shouldPrimitiveCount(action)) return;

  float complexity = this->getComplexityValue(action);
  int numtris = (int)(complexity * CYL_SIDE_NUMTRIS);

  if (this->parts.getValue() & SoCapsule::BOTTOM) {
    action->addNumTriangles((int)(complexity*SPHERE_NUM_SLICES*(SPHERE_NUM_STACKS-1)));
  }
  if (this->parts.getValue() & SoCapsule::TOP) {
    action->addNumTriangles((int)(complexity*SPHERE_NUM_SLICES*(SPHERE_NUM_STACKS-1)));
  }
  if (this->parts.getValue() & SoCapsule::SIDES) {
    action->addNumTriangles(numtris * 2);
  }
}

void SoCapsule::generate_cylinder(const float radius,
				  const float height,
				  const int numslices,
				  const unsigned int flags,
				  SoAction * action) {
  int i;
  int slices = numslices;
  if (slices > 128) slices = 128;
  if (slices < 4) slices = 4;

  float h2 = height * 0.5f;

  SbVec3f coords[129];
  SbVec3f normals[130];
  SbVec2f texcoords[129];

  my_sogl_generate_3d_circle(coords, slices, radius, -h2);
  coords[slices] = coords[0];

  my_sogl_generate_3d_circle(normals, slices, 1.0f, 0.0f);
  normals[slices] = normals[0];
  normals[slices+1] = normals[1];

  int matnr = 0;

  SoPrimitiveVertex vertex;
  SoCylinderDetail sideDetail;
  SoCylinderDetail bottomDetail;
  SoCylinderDetail topDetail;
  sideDetail.setPart(SoCylinder::SIDES);
  bottomDetail.setPart(SoCylinder::BOTTOM);
  topDetail.setPart(SoCylinder::TOP);

  if (flags & SOGEN_GENERATE_SIDE) {
    this->beginShape(action, SoShape::QUAD_STRIP);
    vertex.setDetail(&sideDetail);
    vertex.setMaterialIndex(matnr);
    i = 0;

    float t = 0.0;
    float inc = 1.0f / slices;

    while (i <= slices) {
      vertex.setTextureCoords(SbVec2f(t, 1.0f));
      vertex.setNormal(normals[i]);
      SbVec3f c = coords[i];
      vertex.setPoint(SbVec3f(c[0], h2, c[2]));
      this->shapeVertex(&vertex);

      vertex.setTextureCoords(SbVec2f(t, 0.0f));
      vertex.setPoint(c);
      this->shapeVertex(&vertex);
      i++;
      t += inc;
    }
    if (flags & SOGEN_MATERIAL_PER_PART) matnr++;
    this->endShape();
  }

  if (flags & (SOGEN_GENERATE_BOTTOM | SOGEN_GENERATE_TOP)) {
    my_sogl_generate_2d_circle(texcoords, slices, 0.5f);
    texcoords[slices] = texcoords[0];
  }

  if(flags & SOGEN_GENERATE_TOP) {
    vertex.setMaterialIndex(matnr);
    vertex.setDetail(&topDetail);
    vertex.setNormal(SbVec3f(0.0f, 1.0f, 0.0f));
    this->beginShape(action, SoShape::TRIANGLE_FAN);

    for (i = 0; i < slices; i++) {
      vertex.setTextureCoords(SbVec2f(texcoords[i][0] + 0.5f, 1.0f - texcoords[i][1] - 0.5f));
      const SbVec3f &c = coords[i];
      vertex.setPoint(SbVec3f(c[0], h2, c[2]));
      this->shapeVertex(&vertex);
    }
    this->endShape();
    if (flags & SOGEN_MATERIAL_PER_PART) matnr++;
  }
  if (flags & SOGEN_GENERATE_BOTTOM) {
    vertex.setMaterialIndex(matnr);
    vertex.setDetail(&bottomDetail);
    this->beginShape(action, SoShape::TRIANGLE_FAN);
    vertex.setNormal(SbVec3f(0.0f, -1.0f, 0.0f));

    for (i = slices-1; i >= 0; i--) {
      vertex.setTextureCoords(texcoords[i] + SbVec2f(0.5f, 0.5f));
      vertex.setPoint(coords[i]);
      this->shapeVertex(&vertex);
    }
    this->endShape();
  }
}

void SoCapsule::generate_half_sphere(const float radius,
				     const float offset,
				     const int numstacks,
				     const int numslices,
				     const unsigned int flags,
				     SoAction * action) {
  int stacks = numstacks;
  int slices = numslices;

  if (stacks < 3) stacks = 3;
  if (slices < 4) slices = 4;

  if (slices > 128) slices = 128;

  // used to cache last stack's data
  SbVec3f coords[129];
  SbVec3f normals[129];
  float S[129];

  int i, j;
  float rho;
  float drho;
  float theta;
  float dtheta;
  float tc, ts;
  SbVec3f tmp;

  drho = .5f * float(M_PI) / (float) (stacks-1); // full sphere is w/out .5
  dtheta = 2.0f * float(M_PI) / (float) slices;

  float currs = 0.0f;
  float incs = 1.0f / (float)slices;
  if(flags & SOGEN_GENERATE_TOP)
    rho = drho;
  else
    rho = float(M_PI) - drho;
  theta = 0.0f;
  tc = (float) cos(rho);
  ts = - (float) sin(rho);
  tmp.setValue(0.0f,
	       tc,
	       ts);
  normals[0] = tmp;
  tmp *= radius;
  tmp[1] += offset;
  coords[0] = tmp;
  S[0] = currs;
  float dT = 1.0f / (float) (stacks-1);
  float T = 1.0f - dT;

  SoPrimitiveVertex vertex;
  this->beginShape(action, SoShape::TRIANGLES);

  for (j = 1; j <= slices; j++) {
    vertex.setNormal(SbVec3f(0.0f, 1.0f, 0.0f));
    vertex.setTextureCoords(SbVec2f(currs + 0.5f * incs, 1.0f));

    if(flags & SOGEN_GENERATE_TOP)
      vertex.setPoint(SbVec3f(0.0f, radius + offset, 0.0f));
    else
      vertex.setPoint(SbVec3f(0.0f, -radius + offset, 0.0f));
    this->shapeVertex(&vertex);

    vertex.setNormal(normals[j-1]);
    vertex.setTextureCoords(SbVec2f(currs, T));
    vertex.setPoint(coords[j-1]);
    this->shapeVertex(&vertex);

    currs += incs;
    theta += dtheta;
    S[j] = currs;
    tmp.setValue(float(sin(theta))*ts,
		 tc,
		 float(cos(theta))*ts);

    normals[j] = tmp;
    tmp *= radius;
    tmp[1] += offset;
    coords[j] = tmp;

    vertex.setNormal(normals[j]);
    vertex.setTextureCoords(SbVec2f(currs, T));
    vertex.setPoint(coords[j]);
    this->shapeVertex(&vertex);

  }
  this->endShape();

  if(flags & SOGEN_GENERATE_TOP)
    rho += drho;
  else
    rho -= drho;

  for (i = 1; i < stacks-1; i++) {
    tc = (float)cos(rho);
    ts = - (float) sin(rho);
    this->beginShape(action, SoShape::QUAD_STRIP);
    theta = 0.0f;
    for (j = 0; j <= slices; j++) {
      vertex.setTextureCoords(SbVec2f(S[j], T));
      vertex.setNormal(normals[j]);
      vertex.setPoint(coords[j]);
      this->shapeVertex(&vertex);

      vertex.setTextureCoords(SbVec2f(S[j], T-dT));
      tmp.setValue(float(sin(theta))*ts,
		   tc,
		   float(cos(theta))*ts);
      normals[j] = tmp;
      vertex.setNormal(tmp);
      tmp *= radius;
      tmp[1] += offset;
      coords[j] = tmp;
      theta += dtheta;
      vertex.setPoint(tmp);
      this->shapeVertex(&vertex);
    }
    this->endShape();
    if(flags & SOGEN_GENERATE_TOP)
      rho += drho;
    else
      rho -= drho;
    T -= dT;
  }

  if(flags & SOGEN_GENERATE_SIDE) {
    this->beginShape(action, SoShape::TRIANGLES);
    for (j = 0; j < slices; j++) {
      vertex.setTextureCoords(SbVec2f(S[j], T));
      vertex.setNormal(normals[j]);
      vertex.setPoint(coords[j]);
      this->shapeVertex(&vertex);

      vertex.setTextureCoords(SbVec2f(S[j]+incs*0.5f, 0.0f));
      vertex.setNormal(SbVec3f(0.0f, -1.0f, 0.0f));
      vertex.setPoint(SbVec3f(0.0f, 0.0f, 0.0f));
      this->shapeVertex(&vertex);

      vertex.setTextureCoords(SbVec2f(S[j+1], T));
      vertex.setNormal(normals[j+1]);
      vertex.setPoint(coords[j+1]);
      this->shapeVertex(&vertex);
    }
    this->endShape();
  }
}

// Doc in parent.
void
SoCapsule::generatePrimitives(SoAction * action)
{
  //  SoCapsule::Part p = (SoCapsule::Part) this->parts.getValue();
  unsigned int flags = 0;

  SoMaterialBindingElement::Binding bind =
    SoMaterialBindingElement::get(action->getState());
  if (bind == SoMaterialBindingElement::PER_PART ||
      bind == SoMaterialBindingElement::PER_PART_INDEXED)
    flags |= SOGL_MATERIAL_PER_PART;

  float complexity = this->getComplexityValue(action);

  float rad = this->radius.getValue();
  float ht = this->height.getValue();
  if(ht < 2.0f * rad) ht = 2.0f * rad;
  float cyl_ht = ht - 2.0f * rad;
  float offset = .5f * cyl_ht;

  generate_half_sphere(rad,
		       offset,
		       (int)(SPHERE_NUM_SLICES * complexity),
		       (int)(SPHERE_NUM_STACKS * complexity),
		       flags | SOGEN_GENERATE_TOP,
		       action);

  generate_half_sphere(rad,
		       -offset,
		       (int)(SPHERE_NUM_SLICES * complexity),
		       (int)(SPHERE_NUM_STACKS * complexity),
		       flags | SOGEN_GENERATE_BOTTOM,
		       action);

  generate_cylinder(rad,
		       cyl_ht,
		       (int)(CYL_SIDE_NUMTRIS * complexity),
		       flags | SOGEN_GENERATE_SIDE,
		       action);
}
