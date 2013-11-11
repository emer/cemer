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

#include "SoOffscreenRendererQt.h"

#include <Inventor/C/glue/gl.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoCallback.h>

#include <taMisc>

////////////////////////////////////////////////////////
//		Offscreen Renderer

// note: not using pimpl so no worries
#define PRIVATE(p) p
#define PUBLIC(p) p

void SoOffscreenRendererQt::Constr(const SbViewportRegion & vpr,
				   SoGLRenderAction * glrenderaction) {
  this->backgroundcolor.setValue(0,0,0);
	
  if (glrenderaction) {
    this->renderaction = glrenderaction;
  }
  else {
    this->renderaction = new SoGLRenderAction(vpr);
    this->renderaction->setCacheContext(SoGLCacheContextElement::getUniqueCacheContext());
    this->renderaction->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
  }

  this->didallocation = glrenderaction ? FALSE : TRUE;
  this->viewport = vpr;

  this->pbuff = NULL;		// constructed later
#if (QT_VERSION >= 0x050000)
  this->gl_ctxt = NULL;
  this->gl_widg = NULL;
#endif
  this->cache_context = 0;
}

/*!
  Constructor. Argument is the \a viewportregion we should use when
  rendering. An internal SoGLRenderAction will be constructed.
*/
SoOffscreenRendererQt::SoOffscreenRendererQt(const SbViewportRegion & viewportregion)
{
  Constr(viewportregion);
}

/*!
  Constructor. Argument is the \a action we should apply to the
  scene graph when rendering the scene. Information about the
  viewport is extracted from the \a action.
*/
SoOffscreenRendererQt::SoOffscreenRendererQt(SoGLRenderAction * action)
{
  Constr(action->getViewportRegion(), action);
}

/*!
  Destructor.
*/
SoOffscreenRendererQt::~SoOffscreenRendererQt()
{
  if(pbuff) delete pbuff;
#if (QT_VERSION >= 0x050000)
  if(gl_widg) delete gl_widg;
#endif
  if (this->didallocation) { delete this->renderaction; }
}

/*!
  Sets the viewport region.

  This will invalidate the current buffer, if any. The buffer will not
  contain valid data until another call to
  SoOffscreenRendererQt::render() happens.
*/
void
SoOffscreenRendererQt::setViewportRegion(const SbViewportRegion & region)
{
  PRIVATE(this)->viewport = region;
}

/*!
  Returns the viewerport region.
*/
const SbViewportRegion &
SoOffscreenRendererQt::getViewportRegion(void) const
{
  return PRIVATE(this)->viewport;
}

/*!
  Sets the background color. The buffer is cleared to this color
  before rendering.
*/
void
SoOffscreenRendererQt::setBackgroundColor(const SbColor & color)
{
  PRIVATE(this)->backgroundcolor = color;
}

/*!
  Returns the background color.
*/
const SbColor &
SoOffscreenRendererQt::getBackgroundColor(void) const
{
  return PRIVATE(this)->backgroundcolor;
}

/*!
  Sets the render action. Use this if you have special rendering needs.
*/
void
SoOffscreenRendererQt::setGLRenderAction(SoGLRenderAction * action)
{
  if (action == PRIVATE(this)->renderaction) { return; }

  if (PRIVATE(this)->didallocation) { delete PRIVATE(this)->renderaction; }
  PRIVATE(this)->renderaction = action;
  PRIVATE(this)->didallocation = FALSE;
}

/*!
  Returns the rendering action currently used.
*/
SoGLRenderAction *
SoOffscreenRendererQt::getGLRenderAction(void) const
{
  return PRIVATE(this)->renderaction;
}

// *************************************************************************

static void
pre_render_cb(void * userdata, SoGLRenderAction * action)
{
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  action->setRenderingIsRemote(FALSE);
}

#if (QT_VERSION >= 0x050000)
void SoOffscreenRendererQt::makeBuffer(int width, int height, const QOpenGLFramebufferObjectFormat& fmt) {
#else
void SoOffscreenRendererQt::makeBuffer(int width, int height, const QGLFormat& fmt) {
#endif
  viewport.setWindowSize(width, height);
  cache_context = SoGLCacheContextElement::getUniqueCacheContext();
#if (QT_VERSION >= 0x050000)
  gl_ctxt = (QGLContext*)QGLContext::currentContext(); // save current context
  if(!gl_ctxt) {
    QGLFormat glf;
    glf.setSampleBuffers(true);
    glf.setSamples(4);
    gl_widg = new QGLWidget(glf);
    gl_widg->show();
    gl_ctxt = gl_widg->context();
  }
  pbuff = new QOpenGLFramebufferObject(width, height, fmt);
#else
  pbuff = new QGLPixelBuffer(width, height, fmt);
#endif
}

void SoOffscreenRendererQt::makeMultisampleBuffer(int width, int height, int samples) {
  if(samples < 0) samples = 4;
#if (QT_VERSION >= 0x050000)
  QOpenGLFramebufferObjectFormat fmt;
  if(samples > 0) {
    fmt.setSamples(samples);
  }
  fmt.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
  makeBuffer(width, height, fmt);
#else
  QGLFormat fmt;
  if(samples > 0) {
    fmt.setSampleBuffers(true);
    fmt.setSamples(samples);
  }
  else {
    fmt.setSampleBuffers(false);
  }
  makeBuffer(width, height, fmt);
#endif
}


// Collects common code from the two render() functions.
SbBool
SoOffscreenRendererQt::renderFromBase(SoBase * base)
{
  const SbVec2s fullsize = this->viewport.getViewportSizePixels();

  // oldcontext is used to restore the previous context id, in case
  // the render action is not allocated by us.
  const uint32_t oldcontext = this->renderaction->getCacheContext();

  if(!pbuff) {
    makeMultisampleBuffer(fullsize[0], fullsize[1]); // use default
  }
  else if(pbuff->width() != fullsize[0] || pbuff->height() != fullsize[1]) {
    taMisc::DebugInfo("making new pbuff of right size");
    // get the size right!
    makeMultisampleBuffer(fullsize[0], fullsize[1]); // use default
  }

  this->renderaction->setCacheContext(cache_context);

#if (QT_VERSION >= 0x050000)
  gl_ctxt->makeCurrent();       // always reinstate our original context
  pbuff->bind();

  bool bad = false;
  if(!pbuff->isValid()) {
    taMisc::DebugInfo("SoOffscreenRendererQt: pbuff is not valid, rendering aborted");
    bad = true;
  }
  if(!pbuff->isBound()) {
    taMisc::DebugInfo("SoOffscreenRendererQt: pbuff is not bound, rendering aborted");
    bad = true;
  }
  if(bad) {
    pbuff->release();
    delete pbuff;               // clear out old one -- will get new one next time..
    pbuff = NULL;
    this->renderaction->setCacheContext(oldcontext); // restore old
    return FALSE;
  }
#else
  pbuff->makeCurrent();		// activate us!
#endif

  glEnable(GL_DEPTH_TEST);
  glClearColor(this->backgroundcolor[0],
               this->backgroundcolor[1],
               this->backgroundcolor[2],
               0.0f);

  // needed to clear viewport after glViewport() is called from
  // SoGLRenderAction
  this->renderaction->addPreRenderCallback(pre_render_cb, NULL);
  this->renderaction->setViewportRegion(this->viewport);

  if (base->isOfType(SoNode::getClassTypeId()))
    this->renderaction->apply((SoNode *)base);
  else if (base->isOfType(SoPath::getClassTypeId()))
    this->renderaction->apply((SoPath *)base);
  else  {
    assert(FALSE && "Cannot apply to anything else than an SoNode or an SoPath");
  }

  this->renderaction->removePreRenderCallback(pre_render_cb, NULL);

//   this->glcanvas.deactivateGLContext();
#if (QT_VERSION >= 0x050000)
  pbuff->release();
#else
  pbuff->doneCurrent();
#endif

  this->renderaction->setCacheContext(oldcontext); // restore old

  return TRUE;
}

/*!
  Render the scenegraph rooted at \a scene into our internal pixel
  buffer.


  Important note: make sure you pass in a \a scene node pointer which
  has both a camera and at least one lightsource below it -- otherwise
  you are likely to end up with just a blank or black image buffer.

  This mistake is easily made if you use an SoOffscreenRendererQt on a
  scenegraph from one of the standard viewer components, as you will
  often just leave the addition of a camera and a headlight
  lightsource to the viewer to set up. This camera and lightsource are
  then part of the viewer's private "super-graph" outside of the scope
  of the scenegraph passed in by the application programmer. To make
  sure the complete scenegraph (including the viewer's "private parts"
  (*snicker*)) are passed to this method, you can get the scenegraph
  root from the viewer's internal SoSceneManager instance instead of
  from the viewer's own getSceneGraph() method, like this:

  \code
  SoOffscreenRendererQt * myRenderer = new SoOffscreenRendererQt(vpregion);
  SoNode * root = myViewer->getSceneManager()->getSceneGraph();
  SbBool ok = myRenderer->render(root);
  // [then use image buffer in a texture, or write it to file, or whatever]
  \endcode

  If you do this and still get a blank buffer, another common problem
  is to have a camera which is not actually pointing at the scene
  geometry you want a snapshot of. If you suspect that could be the
  cause of problems on your end, take a look at SoCamera::pointAt()
  and SoCamera::viewAll() to see how you can make a camera node
  guaranteed to be directed at the scene geometry.

  Yet another common mistake when setting up the camera is to specify
  values for the SoCamera::nearDistance and SoCamera::farDistance
  fields which doesn't not enclose the full scene. This will result in
  either just the background color, or that parts at the front or the
  back of the scene will not be visible in the rendering.

  \sa writeToRGB()
*/
SbBool
SoOffscreenRendererQt::render(SoNode * scene)
{
  return PRIVATE(this)->renderFromBase(scene);
}

/*!
  Render the \a scene path into our internal memory buffer.
*/
SbBool
SoOffscreenRendererQt::render(SoPath * scene)
{
  return PRIVATE(this)->renderFromBase(scene);
}

#undef PRIVATE
#undef PUBLIC

