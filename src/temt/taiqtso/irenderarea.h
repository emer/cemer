// Copyright, 1995-2007, Regents of the University of Colorado,
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

// irenderarea.h -- SoOffscreenRendererQt

#ifndef IRENDERAREA_H
#define IRENDERAREA_H

#include "taiqtso_def.h"

#ifdef TA_USE_INVENTOR

//#include <QGLPixelBuffer>
#include <QGLFramebufferObject>

#include <Inventor/SbColor.h>
#include <Inventor/actions/SoGLRenderAction.h>

class SbViewportRegion; // #IGNORE
class SoCamera; // #IGNORE

class TAIQTSO_API SoOffscreenRendererQt {
  // ##NO_CSS ##NO_INSTANCE ##NO_TOKENS offscreen renderer that uses a QGLPixelBuffer as the underlying offscreen render buffer -- this then provides direct support for multisampling antialiasing, which is enabled by default
public:
  SoOffscreenRendererQt(const SbViewportRegion & viewportregion);
  SoOffscreenRendererQt(SoGLRenderAction * action);
  virtual ~SoOffscreenRendererQt();

  virtual void setViewportRegion(const SbViewportRegion & region);
  virtual const SbViewportRegion & getViewportRegion(void) const;

  virtual void setBackgroundColor(const SbColor & color);
  virtual const SbColor & getBackgroundColor(void) const;

  virtual void setGLRenderAction(SoGLRenderAction * action);
  virtual SoGLRenderAction * getGLRenderAction(void) const;

  virtual SbBool render(SoNode * scene);
  virtual SbBool render(SoPath * scene);

  virtual void		makeBuffer(int width, int height, const QGLFramebufferObjectFormat& fmt);
  // create the pixel buffer of given size and format -- this should be called prior to render, otherwise a default will be constructed
  virtual void		makeMultisampleBuffer(int width, int height, int samples = -1);
  // create the pixel buffer of given size, setting the gl format information to use multisample antialiasing -- a -1 means use default value (4), otherwise use what is specified

  QGLFramebufferObject*	getBuffer() { return pbuff; }
  // returns the pixel buffer that has the image in it
  QImage		getImage()  { return pbuff->toImage(); }
  // use the QImage for all file IO stuff etc

protected:
  // NOTE: just putting all the pimpl stuff right here for simplicity, since its not much

  virtual void	Constr(const SbViewportRegion & vpr, SoGLRenderAction * glrenderaction = NULL);
  virtual SbBool renderFromBase(SoBase * base);

  QGLFramebufferObject*	pbuff; // the offscreen rendering supported by qt

  SbViewportRegion viewport;
  SbColor backgroundcolor;
  SoGLRenderAction * renderaction;
  SbBool didallocation;
};

#endif // TA_USE_INVENTOR
#endif // IRENDERAREA_H
