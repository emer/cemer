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

#ifndef SoOffscreenRendererQt_h
#define SoOffscreenRendererQt_h 1

#include "ta_def.h"

#include <Inventor/SbColor.h>
#include <Inventor/actions/SoGLRenderAction.h>

#include <QImage>         // need to define QT_VERSION in first place..
#if (QT_VERSION >= 0x050000)
#include <QGLContext>
#include <QGLWidget>
#include <QOpenGLFramebufferObject>
#else
#include <QGLPixelBuffer>
#endif

class SbViewportRegion; // #IGNORE
class SoCamera; // #IGNORE

class TA_API SoOffscreenRendererQt {
  // ##NO_CSS ##NO_INSTANCE ##NO_TOKENS offscreen renderer that uses a Qt OpenGl frame buffer as the underlying offscreen render buffer -- this then provides direct support for multisampling antialiasing, which is enabled by default
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

#if (QT_VERSION >= 0x050000)
  virtual void		makeBuffer(int width, int height, const QOpenGLFramebufferObjectFormat& fmt);
#else
  virtual void		makeBuffer(int width, int height, const QGLFormat& fmt);
#endif
  // create the pixel buffer of given size and format -- this should be called prior to render, otherwise a default will be constructed
  virtual void		makeMultisampleBuffer(int width, int height, int samples = -1);
  // create the pixel buffer of given size, setting the gl format information to use multisample antialiasing -- a -1 means use default value (4), otherwise use what is specified

#if (QT_VERSION >= 0x050000)
  QOpenGLFramebufferObject* getBuffer() { return pbuff; }
#else
  QGLPixelBuffer*	getBuffer() { return pbuff; }
#endif
  // returns the pixel buffer that has the image in it
  QImage		getImage()  { return pbuff->toImage(); }
  // use the QImage for all file IO stuff etc

protected:
  // NOTE: just putting all the pimpl stuff right here for simplicity, since its not much

  virtual void	Constr(const SbViewportRegion & vpr, SoGLRenderAction * glrenderaction = NULL);
  virtual SbBool renderFromBase(SoBase * base);

#if (QT_VERSION >= 0x050000)
  QOpenGLFramebufferObject* pbuff;
  QGLContext*               gl_ctxt; // this is the gl context active when pbuff was made -- always set this to be active again when using pbuff
  QGLWidget*                gl_widg; // our own gl widget if no active context
#else
  QGLPixelBuffer*	pbuff;
#endif
  // the offscreen rendering supported by qt
  uint32_t		cache_context; // our unique context id

  SbViewportRegion viewport;
  SbColor backgroundcolor;
  SoGLRenderAction * renderaction;
  SbBool didallocation;
};

#endif // SoOffscreenRendererQt_h
