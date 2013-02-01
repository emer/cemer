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

#include "iT3ViewspaceWidget.h"
#include <iSoSelectionEvent>
#include <T3ExaminerViewer>
#include <iT3DataViewFrame>
#include <iVec2i>
#include <T3DataView>
#include <taiSigLink>

#include <taMisc>
#include <taiMisc>
#include <tabMisc>
#include <taRootBase>


#include <QGLWidget>
#include <QScrollBar>

#include <Inventor/nodes/SoSelection.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>


void iT3ViewspaceWidget::SoSelectionCallback(void* inst, SoPath* path) {
  iSoSelectionEvent ev(true, path);
  iT3ViewspaceWidget* t3vw = (iT3ViewspaceWidget*)inst;
  if(!t3vw->t3viewer()->interactionModeOn()) return;
  t3vw->SoSelectionEvent(&ev);
  t3vw->sel_so->touch(); // to redraw
}

void iT3ViewspaceWidget::SoDeselectionCallback(void* inst, SoPath* path) {
  iSoSelectionEvent ev(false, path);
  iT3ViewspaceWidget* t3dv = (iT3ViewspaceWidget*)inst;
  if(!t3dv->t3viewer()->interactionModeOn()) return;
  t3dv->SoSelectionEvent(&ev);
  t3dv->sel_so->touch(); // to redraw
}

iT3ViewspaceWidget::iT3ViewspaceWidget(iT3DataViewFrame* parent)
:QWidget(parent)
{
  m_i_data_frame = parent;
  init();
}

iT3ViewspaceWidget::iT3ViewspaceWidget(QWidget* parent)
:QWidget(parent)
{
  m_i_data_frame = NULL;
  init();
}

iT3ViewspaceWidget::~iT3ViewspaceWidget() {
//  setViewspace(NULL);
  sel_so = NULL;
  m_scene = NULL;
  m_root_so = NULL; // unref's/deletes
  setT3viewer(NULL);
  m_horScrollBar = NULL;
  m_verScrollBar = NULL;
  m_i_data_frame = NULL;
}

void iT3ViewspaceWidget::init() {
  m_t3viewer = NULL;
  m_horScrollBar = NULL;
  m_verScrollBar = NULL;
  m_root_so = new SoSeparator(); // refs
  m_selMode = SM_NONE;
  m_scene = NULL;
  m_last_vis = 0;
//TEST
  setMinimumSize(320, 320);
}

void iT3ViewspaceWidget::deleteScene() {
  if (m_t3viewer) {
    // remove the nodes
    m_t3viewer->quarter->setSceneGraph(NULL);
  }
}

QScrollBar* iT3ViewspaceWidget::horScrollBar(bool auto_create) {
  if (auto_create && !(m_horScrollBar))
    setHasHorScrollBar(true);
  return m_horScrollBar;
}

void iT3ViewspaceWidget::LayoutComponents() {
return;//TEMP
  QSize sz = size(); // already valid
  int ra_wd = (m_verScrollBar) ? sz.width() - m_verScrollBar->width() : sz.width();
  int ra_ht = (m_horScrollBar) ? sz.height() - m_horScrollBar->height() : sz.height();
  if (m_t3viewer) {
    //NOTE: presumably has 0,0 origin, and could change by changing baseWidget()
    // todo: do this in SoEentManager guy
    m_t3viewer->setMinimumSize(ra_wd, ra_ht);
  }
  if (m_horScrollBar) {
    m_horScrollBar->setGeometry(0, ra_ht, ra_wd, m_horScrollBar->height());
  }
  if (m_verScrollBar) {
    m_verScrollBar->setGeometry(ra_wd, 0, m_verScrollBar->width(), ra_ht);
  }
}

void iT3ViewspaceWidget::resizeEvent(QResizeEvent* ev) {
  inherited::resizeEvent(ev);
//TEMP  LayoutComponents();
//TEMP:
  QSize sz = size(); // already valid
  if (m_t3viewer) {
    //NOTE: presumably has 0,0 origin, and could change by changing baseWidget()
    // todo: do this in SoEentManager guy
    m_t3viewer->resize(sz);
  }
}

// #include <GL/gl.h>
static bool CheckExtension(const char *extName ) {
  /*
  ** Search for extName in the extensions string.  Use of strstr()
  ** is not sufficient because extension names can be prefixes of
  ** other extension names.  Could use strtok() but the constant
  ** string returned by glGetString can be in read-only memory.
  */
  const char *exts = (char *) glGetString(GL_EXTENSIONS);
  const char *p = exts;

  const char *end;
  std::size_t extNameLen = strlen(extName);
  end = p + strlen(p);

  while (p < end) {
    std::size_t n = strcspn(p, " ");
    if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
      return true;
    }
    p += (n + 1);
  }
  taMisc::Error("This display does NOT have OpenGL support for the extension:",
                extName, "which is required -- your system will likely crash soon.",
                "Please read the emergent manual for required 3D graphics driver information."
                "Here is a list of your extensions:",
                exts);
  return false;
}

void iT3ViewspaceWidget::setT3viewer(T3ExaminerViewer* value) {
  if (m_t3viewer == value) return;
  if (value && (value->t3vw != this))
      taMisc::Error("iT3ViewspaceWidget::setT3viewer",
      "The RenderArea must be owned by ViewspaceWidget being assigned.");
  if (m_t3viewer) {
    delete m_t3viewer;
  }
  m_t3viewer = value;
  if(!m_t3viewer) return;

  // this is the new Multisampling method -- much better!

  QGLWidget* qglw = (QGLWidget*)m_t3viewer->quarter; // it is this guy
  QGLFormat fmt = qglw->format();

  // note: can move this to an inherited Quarter widget if we need to, as it is a QGLWidget
  if(taMisc::antialiasing_level > 1) {
    fmt.setSampleBuffers(true);
    fmt.setSamples(taMisc::antialiasing_level);
    qglw->setFormat(fmt);               // obs: this is supposedly deprecated..
    qglw->makeCurrent();
    glEnable(GL_MULTISAMPLE);
  }
  else {
    fmt.setSampleBuffers(false);
    qglw->setFormat(fmt);               // obs: this is supposedly deprecated..
    qglw->makeCurrent();
    glDisable(GL_MULTISAMPLE);
  }

#ifdef DEBUG
  // as of 6/28/09 -- this stuff no longer seems to be the problem -- just crashes
  // in low-level Qt gl code around direct rendering.
  // todo: try fmt->setDirectRendering(false) to test for remote viewing

  // apparently the key problem e.g., with remote X into mac X server
  // is this code, GL_TEXTURE_3D:
  //     void
  //       SoGLTexture3EnabledElement::updategl(void)
  //     {
  //       const cc_glglue * glw = sogl_glue_instance(this->state);

  //       if (SoGLDriverDatabase::isSupported(glw, SO_GL_3D_TEXTURES)) {
  //    if (this->data) glEnable(GL_TEXTURE_3D);
  //    else glDisable(GL_TEXTURE_3D);
  //       }
  //     }

  // but the glxinfo suggests that it should be supported, and doing it
  // directly here does NOT cause a problem

  // this extension is also used quite a bit, but apparently is not the problem:
  //    CheckExtension("GL_EXT_texture_rectangle");

  // this will tell you what version is running for debugging purposes:
  //     String gl_vers = (int)QGLFormat::openGLVersionFlags();
  //     taMisc::Error("GL version:", gl_vers);
#endif

  if (m_selMode == SM_NONE)
    m_t3viewer->quarter->setSceneGraph(m_root_so);
  else {
    sel_so = new SoSelection();
    switch (m_selMode) {
    case SM_SINGLE: sel_so->policy = SoSelection::SINGLE; break;
    case SM_MULTI: sel_so->policy = SoSelection::SHIFT; break;
    default: break; // compiler food
    }
    sel_so->addSelectionCallback(SoSelectionCallback, (void*)this);
    sel_so->addDeselectionCallback(SoDeselectionCallback, (void*)this);
    sel_so->addChild(m_root_so);
    m_t3viewer->quarter->setSceneGraph(sel_so);

    SoBoxHighlightRenderAction* rend_act = new SoBoxHighlightRenderAction;
    SoRenderManager* rman = m_t3viewer->quarter->getSoRenderManager();
    rman->setGLRenderAction(rend_act);
    // following may be important for smoothing in offscreen rendering!
    //    rman->setAntialiasing(true, MAX(taMisc::antialiasing_level, 1));
    rman->setAntialiasing(true, 1);
    m_t3viewer->quarter->setTransparencyType(QuarterWidget::BLEND);
    // make sure it has the transparency set for new guy
  }
  LayoutComponents();
  // m_t3viewer->quarter->setUpdatesEnabled(true);
}

void iT3ViewspaceWidget::setHasHorScrollBar(bool value) {
return;//TEMP  if ((m_horScrollBar != NULL) == value) return;
  if (m_horScrollBar) {
    m_horScrollBar->deleteLater();
    m_horScrollBar = NULL;
  } else {
    m_horScrollBar = new QScrollBar(Qt::Horizontal, this);
    m_horScrollBar->show();
    emit initScrollBar(m_horScrollBar);
  }
  LayoutComponents();
}

void iT3ViewspaceWidget::setHasVerScrollBar(bool value) {
return;//TEMP  if ((m_verScrollBar != NULL) == value) return;
  if (m_verScrollBar) {
    m_verScrollBar->deleteLater();
    m_verScrollBar = NULL;
  } else {
    m_verScrollBar = new QScrollBar(Qt::Vertical, this);
    m_verScrollBar->show();
    emit initScrollBar(m_verScrollBar);
  }
  LayoutComponents();
}

void iT3ViewspaceWidget::setSceneGraph(SoNode* sg) {
  if (!m_t3viewer) return; //not supposed to happen
  if (m_scene == sg) return;
  if (m_scene) { //had to have already been initialized before
    m_root_so->removeChild(m_scene);
  }
  m_scene = sg;
  if (m_scene) { //had to have already been initialized before
    m_root_so->addChild(m_scene);
  }
}

void iT3ViewspaceWidget::setSelMode(SelectionMode value) {
  if (m_scene) {
    taMisc::Warning("iT3ViewspaceWidget::setSelMode not allowed to change when scene graph active");
    return;
  }

  if (m_selMode == value) return;
    m_selMode = value;
}

void iT3ViewspaceWidget::setTopView(taDataView* tv) {
  if (m_top_view.ptr() == tv) return;
  m_top_view = tv;
  if (tv) {
    if (isVisible()) {
      m_last_vis = 1;
      tv->SetVisible(true);
    } else {
      m_last_vis = -1;
    }
  }
}

void iT3ViewspaceWidget::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  if ((bool)m_top_view && (m_last_vis != 1)) {
    m_last_vis = 1;
    m_top_view->SetVisible(true);
    if(m_t3viewer && m_t3viewer->quarter)
      m_t3viewer->quarter->setUpdatesEnabled(true);
  }
}

void iT3ViewspaceWidget::hideEvent(QHideEvent* ev) {
  if ((bool)m_top_view && (m_last_vis != -1)) {
    m_last_vis = -1;
    m_top_view->SetVisible(false);
    if(m_t3viewer && m_t3viewer->quarter)
      m_t3viewer->quarter->setUpdatesEnabled(false);
  }
  inherited::hideEvent(ev);
}

QScrollBar* iT3ViewspaceWidget::verScrollBar(bool auto_create) {
  if (auto_create && !(m_verScrollBar))
    setHasVerScrollBar(true);
  return m_verScrollBar;
}

void iT3ViewspaceWidget::ContextMenuRequested(const QPoint& pos) {
  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);

  FillContextMenu(menu);

  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
}

/*void iT3ViewspaceWidget::EditAction_Delete(ISelectable::GuiContext gc_typ) {
  ISelectableHost::EditAction_Delete();
}*/

void iT3ViewspaceWidget::SoSelectionEvent(iSoSelectionEvent* ev) {
  T3DataView* t3node = T3DataView::GetViewFromPath(ev->path);
  if (!t3node) return;

  if (ev->is_selected) {
    AddSelectedItem(t3node);
    taiSigLink* link = t3node->effLink(ISelectable::GC_DEFAULT);
    if(link) {
      taBase* obj = (taBase*)link->data();
      if(obj)
        tabMisc::DelayedFunCall_gui(obj, "BrowserSelectMe");
    }
  }
  else {
    RemoveSelectedItem(t3node);
  }

  t3viewer()->syncViewerMode();

  // notify to our frame that we have grabbed focus
  Emit_GotFocusSignal();
}


void iT3ViewspaceWidget::UpdateSelectedItems_impl() {
  // note: prolly not needed
}
