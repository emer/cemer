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

#include "T3ExaminerViewer.h"
#include <iT3ViewspaceWidget>
#include <T3QuarterWidget>
#include <iT3Panel>
#include <T3Panel>
#include <T3DataViewMain>
#include <iContextMenuButton>

#include <iThumbWheel>
#include <iMenuButton>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QMenu>
#include <QPrinter>
#include <QPrintDialog>
#include <QKeyEvent>
#include <QPainter>
#include <QApplication>

#include <taMisc>
#include <taiMisc>

#ifdef TA_QT3D
#include <T3Entity>

#include <Qt3DCore/QCamera>
#include <Qt3DCore/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QLookAtTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRenderer/QRenderAspect>
#include <Qt3DRenderer/QFrameGraph>
#include <Qt3DRenderer/QForwardRenderer>
#include <Qt3DRenderer/QPhongMaterial>

#include <QKeyEvent>
#include <QOpenGLContext>

T3RenderView::T3RenderView(QScreen *screen)
: QWindow(screen)
{
  setSurfaceType(QSurface::OpenGLSurface);

  // resize(1024, 768);

  QSurfaceFormat format;
  if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
    format.setVersion(4, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
  }
  format.setDepthBufferSize( 24 );
  format.setSamples( 4 );
  setFormat(format);
  create();
}

T3RenderView::~T3RenderView() {
}

void T3RenderView::keyPressEvent( QKeyEvent* e ) {
  // switch ( e->key() ) {
  // case Qt::Key_Escape:
  //   QGuiApplication::quit();
  //   break;

  // default:
  QWindow::keyPressEvent( e );
  // }
}

#else
#include <Inventor/SoEventManager.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#endif

#include "pick.xpm"
#include "view.xpm"
// #include "home.xpm"
// #include "set_home.xpm"
#include "seek.xpm"
#include "view_all.xpm"
#include "print.xpm"
#include "snapshot.xpm"


#define WHEEL_LENGTH 60         // long axis
#define WHEEL_WIDTH 20          // short axis
#define BUTTON_WIDTH 20
#define BUTTON_HEIGHT 20

// for thumb wheel -- max value and wraparound delta threshold detection
#define THUMB_MAX_VAL 1000
#define THUMB_INIT_VAL 500
#define THUMB_PAGE_STEP 10
#define THUMB_WRAP_THR 800

const int T3ExaminerViewer::n_views = 10;
const float fixed_pan_distance = .025;   // how much to move on each key press
const float fixed_rotate_distance = .025;   // how much to rotate on each key press
const float fixed_zoom_delta = .025;   // how much to rotate on each key press


bool T3ExaminerViewer::so_scrollbar_is_dragging = false;

static void t3ev_config_wheel(iThumbWheel* whl) {
  whl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  whl->setWrapsAround(true);
  whl->setLimitedDrag(false);
}

T3ExaminerViewer::T3ExaminerViewer(iT3ViewspaceWidget* parent)
: QWidget(parent)
{
  t3vw = parent;
#ifndef TA_QT3D
  quarter = NULL;               // for startup events
#endif
  viewer_mode = VIEW;
  
  // all the main layout code
  //  main_vbox: main_hbox: lhs_vbox quarter rhs_vbox
  //             bot_hbox
  
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
  main_vbox = new QVBoxLayout(this);
  main_vbox->setMargin(0); main_vbox->setSpacing(0);
  
  main_hbox = new QHBoxLayout;
  main_hbox->setMargin(0); main_hbox->setSpacing(0);
  main_vbox->addLayout(main_hbox);
  
  bot_hbox = new QHBoxLayout;
  bot_hbox->setMargin(0); main_hbox->setSpacing(0);
  main_vbox->addLayout(bot_hbox);
  
  lhs_vbox = new QVBoxLayout;
  lhs_vbox->setMargin(0); lhs_vbox->setSpacing(0);
  main_hbox->addLayout(lhs_vbox);


#ifdef TA_QT3D
  scene = NULL;
  bg_color = QColor::fromRgbF(0.0, 0.5, 1.0, 1.0);
  
  view3d = new T3RenderView();
  QWidget* container = QWidget::createWindowContainer(view3d);
  main_hbox->addWidget(container, 1);
  engine = new Qt3D::QAspectEngine;
  render = new Qt3D::QRenderAspect(); // Qt3D::QRenderAspect::Synchronous); // hangs
  engine->registerAspect(render);
  input = new Qt3D::QInputAspect;
  engine->registerAspect(input);
  engine->initialize();
  QVariantMap data;
  data.insert(QStringLiteral("surface"),
              QVariant::fromValue(static_cast<QSurface *>(view3d)));
  data.insert(QStringLiteral("eventSource"), QVariant::fromValue(view3d));
  engine->setData(data);

  root_entity = new Qt3D::QEntity();
  camera = new Qt3D::QCamera(root_entity);

  QSize sz = container->size();
  float aspect_ratio = (float)sz.width() / (float)sz.height();
  
  camera->lens()->setPerspectiveProjection(20.0f, aspect_ratio, 0.1f, 1000.0f);
  camera->setPosition(QVector3D(0.0f, 0.0f, 5.0f));
  camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
  camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
  input->setCamera(camera);

  framegraph = new Qt3D::QFrameGraph();
  forward_renderer = new Qt3D::QForwardRenderer();
  forward_renderer->setClearColor(bg_color);
  forward_renderer->setCamera(camera);
  framegraph->setActiveFrameGraph(forward_renderer);
  
  root_entity->addComponent(framegraph);
  engine->setRootEntity(root_entity);

#else
  // note: we're setting our format right at construction, instead of doing
  // it later in iT3ViewSpaceWidget, which we used to do..
  QGLFormat fmt;
#if (QT_VERSION >= 0x040700)
  // this is critical for Coin3d, which depends on legacy 1.x OpenGL functionality
  fmt.setProfile(QGLFormat::CompatibilityProfile);
#endif
  if(taMisc::antialiasing_level > 1) {
    fmt.setSampleBuffers(true);
    fmt.setSamples(taMisc::antialiasing_level);
  }
  quarter = new T3QuarterWidget(fmt, this);
  
  quarter->setUpdatesEnabled(false);
  // set any initial configs for quarter widget here (or somewhere else if you please)
  quarter->setInteractionModeEnabled(true);
  quarter->setTransparencyType(QuarterWidget::BLEND); // this is a good default
  quarter->setNavigationModeFile(QUrl("coin:///scxml/navigation/examiner.xml"));
  quarter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  quarter->installEventFilter(this);
  SoEventManager* emgr = quarter->getSoEventManager();
  emgr->setNavigationState(SoEventManager::JUST_NAVIGATION);
  main_hbox->addWidget(quarter, 1);
#endif
  
  rhs_vbox = new QVBoxLayout;
  rhs_vbox->setMargin(0); rhs_vbox->setSpacing(0);
  main_hbox->addLayout(rhs_vbox);
  
  ///// make wheels all together
  
  hrot_wheel = new iThumbWheel(0, THUMB_MAX_VAL, THUMB_PAGE_STEP, THUMB_INIT_VAL, Qt::Horizontal, this);
  t3ev_config_wheel(hrot_wheel);
  hrot_start_val = THUMB_INIT_VAL;
  hrot_wheel->setMaximumSize(WHEEL_LENGTH, WHEEL_WIDTH);
  hrot_wheel->setToolTip(taiMisc::ToolTipPreProcess("Key equivalent - left/right arrow keys"));
  connect(hrot_wheel, SIGNAL(valueChanged(int)), this, SLOT(hrotwheelChanged(int)));
  
  vrot_wheel = new iThumbWheel(0, THUMB_MAX_VAL, THUMB_PAGE_STEP, THUMB_INIT_VAL, Qt::Vertical, this);
  t3ev_config_wheel(vrot_wheel);
  vrot_start_val = THUMB_INIT_VAL;
  vrot_wheel->setMaximumSize(WHEEL_WIDTH, WHEEL_LENGTH);
  vrot_wheel->setToolTip(taiMisc::ToolTipPreProcess("Key equivalent - up/down arrow keys"));
  connect(vrot_wheel, SIGNAL(valueChanged(int)), this, SLOT(vrotwheelChanged(int)));
  
  zoom_wheel = new iThumbWheel(0, THUMB_MAX_VAL, THUMB_PAGE_STEP, THUMB_INIT_VAL, Qt::Vertical, this);
  t3ev_config_wheel(zoom_wheel);
  zoom_start_val = THUMB_INIT_VAL;
  zoom_wheel->setMaximumSize(WHEEL_WIDTH, WHEEL_LENGTH);
  zoom_wheel->setToolTip(taiMisc::ToolTipPreProcess("Key equivalent '+' key and '-' key"));
  connect(zoom_wheel, SIGNAL(valueChanged(int)), this, SLOT(zoomwheelChanged(int)));
  
  hpan_wheel = new iThumbWheel(0, THUMB_MAX_VAL, THUMB_PAGE_STEP, THUMB_INIT_VAL, Qt::Horizontal, this);
  t3ev_config_wheel(hpan_wheel);
  hpan_start_val = THUMB_INIT_VAL;
  hpan_wheel->setMaximumSize(WHEEL_LENGTH, WHEEL_WIDTH);
  hpan_wheel->setToolTip(taiMisc::ToolTipPreProcess("Key equivalent - shift + left/right arrow keys"));
  connect(hpan_wheel, SIGNAL(valueChanged(int)), this, SLOT(hpanwheelChanged(int)));
  
  vpan_wheel = new iThumbWheel(0, THUMB_MAX_VAL, THUMB_PAGE_STEP, THUMB_INIT_VAL, Qt::Vertical, this);
  t3ev_config_wheel(vpan_wheel);
  vpan_start_val = THUMB_INIT_VAL;
  vpan_wheel->setMaximumSize(WHEEL_WIDTH, WHEEL_LENGTH);
  vpan_wheel->setToolTip(taiMisc::ToolTipPreProcess("Key equivalent - shift + up/down arrow keys"));
  connect(vpan_wheel, SIGNAL(valueChanged(int)), this, SLOT(vpanwheelChanged(int)));
  
  /////  lhs_vbox
  
  lhs_button_vbox = new QVBoxLayout;
  lhs_button_vbox->setMargin(0); lhs_button_vbox->setSpacing(0);
  lhs_vbox->addLayout(lhs_button_vbox);
  
  lhs_vbox->addStretch();
  
  lhs_vbox->addWidget(vrot_wheel);
  
  /////  rhs_vbox
  
  rhs_button_vbox = new QVBoxLayout;
  rhs_button_vbox->setMargin(0); rhs_button_vbox->setSpacing(0);
  rhs_vbox->addLayout(rhs_button_vbox);
  
  rhs_vbox->addStretch();
  
  rhs_vbox->addWidget(zoom_wheel);
  zoom_lbl = new QLabel("Zoom", this);
  zoom_lbl->setFont(taiM->buttonFont(taiMisc::sizMedium));
  zoom_lbl->setToolTip(taiMisc::ToolTipPreProcess("Wheel above will Zoom the view camera in and out -- keyboard arrows and PgUp/PgDn also work here, as does the scroll wheel (hold down Shift or Ctrl to move in finer steps)"));
  rhs_vbox->addWidget(zoom_lbl);
  
  rhs_vbox->addSpacing(4);
  rhs_vbox->addWidget(vpan_wheel);
  
  /////  bot_hbox
  
  vrot_lbl = new QLabel("V.Rot", this);
  vrot_lbl->setToolTip(taiMisc::ToolTipPreProcess("Wheel above will rotate the view vertically -- keyboard arrows and PgUp/PgDn also work here, as does the scroll wheel (hold down Shift or Ctrl to move in finer steps)"));
  bot_hbox->addWidget(vrot_lbl);
  
  bot_hbox->addSpacing(4);
  
  hrot_lbl = new QLabel("H.Rot", this);
  hrot_lbl->setToolTip(taiMisc::ToolTipPreProcess("Wheel to the right will rotate the view horizontally -- keyboard arrows and PgUp/PgDn also work here, as does the scroll wheel (hold down Shift or Ctrl to move in finer steps)"));
  bot_hbox->addWidget(hrot_lbl);
  
  bot_hbox->addWidget(hrot_wheel);
  
  bot_hbox->addStretch();
  
  bot_button_hbox = new QHBoxLayout;
  bot_button_hbox->setMargin(0); rhs_button_vbox->setSpacing(0);
  bot_hbox->addLayout(bot_button_hbox);
  
  bot_hbox->addStretch();
  
  bot_hbox->addWidget(hpan_wheel);
  hpan_lbl = new QLabel("H.Pan", this);
  hpan_lbl->setToolTip(taiMisc::ToolTipPreProcess("Wheel to the left will pan (move) the view horizontally -- keyboard arrows and PgUp/PgDn also work here, as does the scroll wheel (hold down Shift or Ctrl to move in finer steps)"));
  bot_hbox->addWidget(hpan_lbl);
  
  bot_hbox->addSpacing(4);
  
  vpan_lbl = new QLabel("V.Pan ", this);
  vpan_lbl->setToolTip(taiMisc::ToolTipPreProcess("Wheel above will pan (move) the view vertically -- keyboard arrows and PgUp/PgDn also work here, as does the scroll wheel (hold down Shift or Ctrl to move in finer steps)"));
  bot_hbox->addWidget(vpan_lbl);
  
  Constr_RHS_Buttons();
  Constr_LHS_Buttons();
  Constr_Bot_Buttons();
  
  //   quarter->setInteractionModeOn(false);
  setInteractionModeOn(false);  // default start it off!
  setFocusPolicy(Qt::StrongFocus);
}

T3ExaminerViewer::~T3ExaminerViewer() {
#ifdef TA_QT3D
  // engine->shutdown();  // this causes crash at exit!
  // delete engine;
  // delete view3d;
#else
  quarter->setInteractionModeOn(false); // turn off interaction mode. before we die..
  // otherwise we can crash..
#endif
}

T3Panel* T3ExaminerViewer::GetPanel() {
  if(!t3vw) return NULL;
  iT3Panel* ipanl = t3vw->i_data_frame();
  if(!ipanl) return NULL;
  T3Panel* panl = ipanl->viewer();
  return panl;
}

void T3ExaminerViewer::Constr_RHS_Buttons() {
  interact_button = new QToolButton(this);
  interact_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  interact_button->setCheckable(true);
  interact_button->setChecked(false);
  interact_button->setIcon(QPixmap((const char **)pick_xpm));
  interact_button->setToolTip(taiMisc::ToolTipPreProcess("Interact (I key, or ESC to toggle): Allows you to select and manipulate objects in the display \n(ESC toggles between Interact and Camera View"));
  connect(interact_button, SIGNAL(clicked()), this, SLOT(interactbuttonClicked()));
  rhs_button_vbox->addWidget(interact_button);
  
  view_button = new QToolButton(this);
  view_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  view_button->setCheckable(true);
  view_button->setChecked(true);
  view_button->setIcon(QPixmap((const char **)view_xpm));
  view_button->setToolTip(taiMisc::ToolTipPreProcess("Camera View (V key, or ESC to toggle): Allows you to move the view around (click and drag to move; \nshift = move in the plane; ESC toggles between Camera View and Interact)"));
  connect(view_button, SIGNAL(clicked()), this, SLOT(viewbuttonClicked()));
  rhs_button_vbox->addWidget(view_button);
  
  view_all_button = new QToolButton(this);
  view_all_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  view_all_button->setIcon(QPixmap((const char **)view_all_xpm));
  view_all_button->setToolTip(taiMisc::ToolTipPreProcess("(A key) View All: repositions the camera view to the standard initial view with everything in view"));
  connect(view_all_button, SIGNAL(clicked()), this, SLOT(viewallbuttonClicked()));
  rhs_button_vbox->addWidget(view_all_button);
  
  seek_button = new QToolButton(this);
  seek_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  seek_button->setIcon(QPixmap((const char **)seek_xpm));
  seek_button->setToolTip(taiMisc::ToolTipPreProcess("Seek (S key): Click on objects (not text!) in the display and the camera will \nfocus in on the point where you click -- repeated clicks will zoom in further"));
  connect(seek_button, SIGNAL(clicked()), this, SLOT(seekbuttonClicked()));
  rhs_button_vbox->addWidget(seek_button);
  
  snapshot_button = new QToolButton(this);
  snapshot_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  snapshot_button->setIcon(QPixmap((const char **)snapshot_xpm));
  snapshot_button->setToolTip(taiMisc::ToolTipPreProcess("Snapshot: save the current viewer image to a file -- PNG format is recommended for most cases as it compresses well and is lossles.  Use EPS to produce a postscript format file that can be converted to PDF or other vector formats for editing."));
  connect(snapshot_button, SIGNAL(clicked()), this, SLOT(snapshotbuttonClicked()));
  rhs_button_vbox->addWidget(snapshot_button);
  
  print_button = new QToolButton(this);
  print_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  print_button->setIcon(QPixmap((const char **)print_xpm));
  print_button->setToolTip(taiMisc::ToolTipPreProcess("Print: print the current viewer image to a printer -- uses the bitmap of screen image\n make window as large as possible for better quality"));
  connect(print_button, SIGNAL(clicked()), this, SLOT(printbuttonClicked()));
  rhs_button_vbox->addWidget(print_button);
  
  annote_button = new iMenuButton(this);
  taiM->FormatButton(annote_button, "+", taiMisc::fonBig);
  annote_button->setToolTip(taiMisc::ToolTipPreProcess("Add annotations (lines, text, etc) to the view"));
  
  QMenu* anmenu = new QMenu();
  QAction* tmpact = new QAction("line", this);
  connect(tmpact, SIGNAL(triggered()), this, SLOT(annoteLineClicked()));
  anmenu->addAction(tmpact);
  
  tmpact = new QAction("start arrow", this);
  connect(tmpact, SIGNAL(triggered()), this, SLOT(annoteStartArrowClicked()));
  anmenu->addAction(tmpact);
  
  tmpact = new QAction("end arrow", this);
  connect(tmpact, SIGNAL(triggered()), this, SLOT(annoteEndArrowClicked()));
  anmenu->addAction(tmpact);
  
  tmpact = new QAction("double arrow", this);
  connect(tmpact, SIGNAL(triggered()), this, SLOT(annoteDoubleArrowClicked()));
  anmenu->addAction(tmpact);
  
  tmpact = new QAction("rectangle", this);
  connect(tmpact, SIGNAL(triggered()), this, SLOT(annoteRectangleClicked()));
  anmenu->addAction(tmpact);
  
  tmpact = new QAction("ellipse", this);
  connect(tmpact, SIGNAL(triggered()), this, SLOT(annoteEllipseClicked()));
  anmenu->addAction(tmpact);
  
  tmpact = new QAction("circle", this);
  connect(tmpact, SIGNAL(triggered()), this, SLOT(annoteCircleClicked()));
  anmenu->addAction(tmpact);
  
  tmpact = new QAction("text", this);
  connect(tmpact, SIGNAL(triggered()), this, SLOT(annoteTextClicked()));
  anmenu->addAction(tmpact);
  
  tmpact = new QAction("object", this);
  connect(tmpact, SIGNAL(triggered()), this, SLOT(annoteObjectClicked()));
  anmenu->addAction(tmpact);
  
  tmpact = new QAction("clear all", this);
  connect(tmpact, SIGNAL(triggered()), this, SLOT(annoteClearAllClicked()));
  anmenu->addAction(tmpact);
  
  tmpact = new QAction("edit all", this);
  connect(tmpact, SIGNAL(triggered()), this, SLOT(annoteEditAllClicked()));
  anmenu->addAction(tmpact);
  
  annote_button->setMenu(anmenu);
  rhs_button_vbox->addWidget(annote_button);
}

void T3ExaminerViewer::Constr_LHS_Buttons() {
  
}

void T3ExaminerViewer::Constr_Bot_Buttons() {
  cur_view_no = -1;             // nothing set yet
  saved_views.SetSize(n_views);
  for(int i=0; i<n_views; i++) {
    T3SavedView* sv = saved_views[i];
    String nm = sv->name;
    if(nm.contains("T3SavedView")) {            // uninitialized
      nm = "Vw_" + String(i);
      sv->name = nm;
    }
    
    QToolButton* view_button = new iContextMenuButton(this);
    
    iAction* view_act = new iAction(i, nm, QKeySequence());
    view_act->connect(iAction::int_act, this,  SLOT(gotoviewbuttonClicked(int)));
    view_act->setParent(view_button);
    view_act->setCheckable(true);
    view_act->setChecked(sv->view_saved);
    
    view_button->setToolTip(taiMisc::ToolTipPreProcess("Go To View: Restores display to previously saved viewing configuration -- hold mouse down for menu to save view and set view name"));
    //    view_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    view_button->setDefaultAction(view_act);
    bot_button_hbox->addWidget(view_button);
    
    sv->view_action = view_act;
    sv->view_button = view_button;
    
    QMenu* view_menu = new QMenu(this);
    view_menu->setFont(taiM->menuFont(taiMisc::fonMedium));
    
    iAction* save_act = new iAction(i, "&Save View", QKeySequence());
    save_act->setStatusTip("Save the current view settings to this view button");
    save_act->connect(iAction::int_act, this,  SLOT(saveviewTriggered(int)));
    save_act->setParent(view_menu);
    view_menu->addAction(save_act);
    
    iAction* name_act = new iAction(i, "&Name View", QKeySequence());
    name_act->setStatusTip("Name this view button");
    name_act->connect(iAction::int_act, this,  SLOT(nameviewTriggered(int)));
    name_act->setParent(view_menu);
    view_menu->addAction(name_act);
    
    iAction* sname_act = new iAction(i, "&Save + Name", QKeySequence());
    sname_act->setStatusTip("Save the current view settings to this view button, and then name it");
    sname_act->connect(iAction::int_act, this,  SLOT(savenameviewTriggered(int)));
    sname_act->setParent(view_menu);
    view_menu->addAction(sname_act);
    
    view_button->setMenu(view_menu);
    view_button->setPopupMode(QToolButton::DelayedPopup);
  }
}

int T3ExaminerViewer::addDynButton(const String& label, const String& tooltip) {
  int but_no = dyn_buttons.FindName(label);
  if(but_no >= 0) return but_no;
  but_no = dyn_buttons.size;
  NameVar nv(label, but_no);
  dyn_buttons.Add(nv);
  
  QToolButton* dyn_button = new QToolButton(this);
  // make them all the same size
  QSizePolicy sp(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  dyn_button->setSizePolicy(sp);
  iAction* dyn_act = new iAction(but_no, label, QKeySequence());
  dyn_act->connect(iAction::int_act, this,  SLOT(dynbuttonClicked(int)));
  dyn_act->setParent(dyn_button);
  dyn_act->setToolTip(taiMisc::ToolTipPreProcess(tooltip));
  dyn_button->setDefaultAction(dyn_act);
  dyn_button->setFont(taiM->buttonFont(taiMisc::sizMedium));
  lhs_button_vbox->addWidget(dyn_button);
  dyn_actions.Add(dyn_act);
  return but_no;
}

iAction* T3ExaminerViewer::getDynButton(int but_no) {
  return dyn_actions.SafeEl(but_no);
}

iAction* T3ExaminerViewer::getDynButtonName(const String& label) {
  int but_no = dyn_buttons.FindName(label);
  if(but_no < 0) return NULL;
  return getDynButton(but_no);
}

void T3ExaminerViewer::setDynButtonChecked(int but_no, bool onoff, bool mutex) {
  if(but_no < 0 || but_no >= dyn_actions.size) return;
  if(mutex) {
    for(int i=0;i<dyn_actions.size; i++) {
      iAction* da = dyn_actions[i];
      da->setCheckable(true);
      da->setChecked(false);
    }
  }
  
  iAction* da = dyn_actions[but_no];
  da->setCheckable(true);
  da->setChecked(onoff);
}

void T3ExaminerViewer::removeAllDynButtons() {
  dyn_buttons.Reset();
  dyn_actions.Reset();
  for(int i=lhs_button_vbox->count()-1; i>=0; i--) {
    lhs_button_vbox->removeItem(lhs_button_vbox->itemAt(i));
  }
}

bool T3ExaminerViewer::removeDynButton(int but_no) {
  if(but_no < 0 || but_no >= dyn_buttons.size) return false;
  dyn_buttons.RemoveIdx(but_no);
  dyn_actions.RemoveIdx(but_no);
  lhs_button_vbox->removeItem(lhs_button_vbox->itemAt(but_no));
  return true;
}

bool T3ExaminerViewer::removeDynButtonName(const String& label) {
  int but_no = dyn_buttons.FindName(label);
  if(but_no < 0) return false;
  return removeDynButton(but_no);
}

///////////////////////////////////////////////////////////////
//              Main Button Actions

#ifdef TA_QT3D
#define ROT_DELTA_MULT  0.02f
#else
#define ROT_DELTA_MULT  0.002f
#endif
#define PAN_DELTA_MULT  0.002f
#define ZOOM_DELTA_MULT 0.002f

void T3ExaminerViewer::hrotwheelChanged(int value) {
  float delta = (float)(value - hrot_start_val);
  if(fabsf(delta) > THUMB_WRAP_THR) {  // first detect wraparound
    int new_value = value;
    if(hrot_start_val < THUMB_INIT_VAL) new_value -= THUMB_MAX_VAL;
    else                     new_value += THUMB_MAX_VAL;
    delta = (float)(new_value - hrot_start_val);
  }
  hrot_start_val = value;
  horizRotateView(ROT_DELTA_MULT * delta);
}

void T3ExaminerViewer::vrotwheelChanged(int value) {
  float delta = (float)(value - vrot_start_val);
  if(fabsf(delta) > THUMB_WRAP_THR) {  // first detect wraparound
    int new_value = value;
    if(vrot_start_val < THUMB_INIT_VAL) new_value -= THUMB_MAX_VAL;
    else                     new_value += THUMB_MAX_VAL;
    delta = (float)(new_value - vrot_start_val);
  }
  vrot_start_val = value;
  vertRotateView(ROT_DELTA_MULT * delta);
}

void T3ExaminerViewer::zoomwheelChanged(int value) {
  float delta = (float)(value - zoom_start_val);
  if(fabsf(delta) > THUMB_WRAP_THR) {  // first detect wraparound
    int new_value = value;
    if(zoom_start_val < THUMB_INIT_VAL) new_value -= THUMB_MAX_VAL;
    else                     new_value += THUMB_MAX_VAL;
    delta = (float)(new_value - zoom_start_val);
  }
  zoom_start_val = value;
  zoomView(-ZOOM_DELTA_MULT * delta); // direction is opposite
}

void T3ExaminerViewer::hpanwheelChanged(int value) {
  float delta = (float)(value - hpan_start_val);
  if(fabsf(delta) > THUMB_WRAP_THR) {  // first detect wraparound
    int new_value = value;
    if(hpan_start_val < THUMB_INIT_VAL) new_value -= THUMB_MAX_VAL;
    else                     new_value += THUMB_MAX_VAL;
    delta = (float)(new_value - hpan_start_val);
  }
  hpan_start_val = value;
  horizPanView(PAN_DELTA_MULT * delta);
}

void T3ExaminerViewer::vpanwheelChanged(int value) {
  float delta = (float)(value - vpan_start_val);
  if(fabsf(delta) > THUMB_WRAP_THR) {  // first detect wraparound
    int new_value = value;
    if(vpan_start_val < THUMB_INIT_VAL) new_value -= THUMB_MAX_VAL;
    else                     new_value += THUMB_MAX_VAL;
    delta = (float)(new_value - vpan_start_val);
  }
  vpan_start_val = value;
  vertPanView(PAN_DELTA_MULT * delta);
}

void T3ExaminerViewer::interactbuttonClicked() {
  setInteractionModeOn(true);
}

void T3ExaminerViewer::viewbuttonClicked() {
  setInteractionModeOn(false);
}

void T3ExaminerViewer::viewallbuttonClicked() {
  viewAll();
}

void T3ExaminerViewer::seekbuttonClicked() {
#ifndef TA_QT3D
  quarter->seek();
#endif
}

void T3ExaminerViewer::snapshotbuttonClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  panl->CallFun("SaveImageAs");
}

void T3ExaminerViewer::printbuttonClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  panl->PrintImage();
}

void T3ExaminerViewer::annoteLineClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  T3DataViewMain* dvm = panl->FirstChild();
  if(!dvm) return;
  dvm->CallFun("AnnoteLine");
}

void T3ExaminerViewer::annoteStartArrowClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  T3DataViewMain* dvm = panl->FirstChild();
  if(!dvm) return;
  dvm->CallFun("AnnoteStartArrow");
}

void T3ExaminerViewer::annoteEndArrowClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  T3DataViewMain* dvm = panl->FirstChild();
  if(!dvm) return;
  dvm->CallFun("AnnoteEndArrow");
}

void T3ExaminerViewer::annoteDoubleArrowClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  T3DataViewMain* dvm = panl->FirstChild();
  if(!dvm) return;
  dvm->CallFun("AnnoteDoubleArrow");
}

void T3ExaminerViewer::annoteRectangleClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  T3DataViewMain* dvm = panl->FirstChild();
  if(!dvm) return;
  dvm->CallFun("AnnoteRectangle");
}

void T3ExaminerViewer::annoteEllipseClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  T3DataViewMain* dvm = panl->FirstChild();
  if(!dvm) return;
  dvm->CallFun("AnnoteEllipse");
}

void T3ExaminerViewer::annoteCircleClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  T3DataViewMain* dvm = panl->FirstChild();
  if(!dvm) return;
  dvm->CallFun("AnnoteCircle");
}

void T3ExaminerViewer::annoteTextClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  T3DataViewMain* dvm = panl->FirstChild();
  if(!dvm) return;
  dvm->CallFun("AnnoteText");
}

void T3ExaminerViewer::annoteObjectClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  T3DataViewMain* dvm = panl->FirstChild();
  if(!dvm) return;
  dvm->CallFun("AnnoteObject");
}

void T3ExaminerViewer::annoteClearAllClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  T3DataViewMain* dvm = panl->FirstChild();
  if(!dvm) return;
  dvm->CallFun("AnnoteClearAll"); // gives auto undo..
}

void T3ExaminerViewer::annoteEditAllClicked() {
  T3Panel* panl = GetPanel();
  if(!panl) return;
  T3DataViewMain* dvm = panl->FirstChild();
  if(!dvm) return;
  dvm->annotations.BrowserSelectMe();
}

void T3ExaminerViewer::gotoviewbuttonClicked(int view_no) {
  T3SavedView* sv = saved_views.SafeEl(view_no);
  if(!sv) return;
  if(sv->view_action)
    sv->view_action->setChecked(sv->view_saved); // always update with this state!
  gotoView(view_no);
}

void T3ExaminerViewer::saveviewTriggered(int view_no) {
  saveView(view_no);
}

void T3ExaminerViewer::nameviewTriggered(int view_no) {
  T3SavedView* sv = saved_views.SafeEl(view_no);
  if(!sv) return;
  if(sv->OpenInWindow(true))              // modal!
    nameView(view_no, sv->name);
}

void T3ExaminerViewer::savenameviewTriggered(int view_no) {
  saveView(view_no);
  T3SavedView* sv = saved_views.SafeEl(view_no);
  if(!sv) return;
  if(sv->OpenInWindow(true))              // modal!
    nameView(view_no, sv->name);
}

void T3ExaminerViewer::dynbuttonClicked(int but_no) {
  emit dynbuttonActivated(but_no);
}

void T3ExaminerViewer::keyPressEvent(QKeyEvent* key_event) {
  taiMisc::UpdateUiOnCtrlPressed(this, key_event);
  
  taiMisc::BoundAction action = taiMisc::GetActionFromKeyEvent(taiMisc::GRAPHICS_CONTEXT, key_event);
  
  switch (action) {
    case taiMisc::GRAPHICS_INTERACTION_MODE_TOGGLE:
      if(interactionModeOn()) { 
        setInteractionModeOn(false);
      }
      else {
        setInteractionModeOn(true);
      }
      key_event->accept();
      return;
    case taiMisc::GRAPHICS_INTERACTION_MODE_OFF:
      setInteractionModeOn(false);
      key_event->accept();
      return;
    case taiMisc::GRAPHICS_INTERACTION_MODE_ON:
      setInteractionModeOn(true);
      key_event->accept();
      return;
    case taiMisc::GRAPHICS_RESET_VIEW:
      gotoView(0);                        // 0 is base guy
      key_event->accept();
      return;
    case taiMisc::GRAPHICS_VIEW_ALL:
      viewAll();
      key_event->accept();
      return;
    case taiMisc::GRAPHICS_SEEK:
#ifndef TA_QT3D
      quarter->seek();
#endif
      key_event->accept();
      return;
    case taiMisc::GRAPHICS_PAN_LEFT: // arrow keys do double duty - depends on mode ;break and drop through if "interaction" mode
      if (!interactionModeOn()) {
        horizPanView(-fixed_pan_distance);
        key_event->accept();
        return;
      }
      break;
    case taiMisc::GRAPHICS_PAN_RIGHT:
      if (!interactionModeOn()) {
        horizPanView(fixed_pan_distance);
        key_event->accept();
        return;
      }
      break;
    case taiMisc::GRAPHICS_PAN_UP:
      if (!interactionModeOn()) {
        vertPanView(-fixed_pan_distance);
        key_event->accept();
        return;
      }
      break;
    case taiMisc::GRAPHICS_PAN_DOWN:
      if (!interactionModeOn()) {
        vertPanView(fixed_pan_distance);
        key_event->accept();
        return;
      }
      break;
    case taiMisc::GRAPHICS_ROTATE_LEFT:
      if (!interactionModeOn()) {
        horizRotateView(-fixed_rotate_distance);
        key_event->accept();
        return;
      }
      break;
    case taiMisc::GRAPHICS_ROTATE_RIGHT:
      if (!interactionModeOn()) {
        horizRotateView(fixed_rotate_distance);
        key_event->accept();
        return;
      }
      break;
    case taiMisc::GRAPHICS_ROTATE_UP:
      if (!interactionModeOn()) {
        vertRotateView(-fixed_rotate_distance);
        key_event->accept();
        return;
      }
      break;
    case taiMisc::GRAPHICS_ROTATE_DOWN:
      if (!interactionModeOn()) {
        vertRotateView(fixed_rotate_distance);
        key_event->accept();
        return;
      }
      break;
    case taiMisc::GRAPHICS_ZOOM_IN:
    case taiMisc::GRAPHICS_ZOOM_IN_II:
      zoomView(-fixed_zoom_delta);
      key_event->accept();
      return;
    case taiMisc::GRAPHICS_ZOOM_OUT:
    case taiMisc::GRAPHICS_ZOOM_OUT_II:
      zoomView(fixed_zoom_delta);
      key_event->accept();
      return;
    case taiMisc::GRAPHICS_VIEW_LEFT:
      previousView();
      key_event->accept();
      return;
    case taiMisc::GRAPHICS_VIEW_RIGHT:
      nextView();
      key_event->accept();
      return;
    default:
      emit unTrappedKeyPressEvent(key_event);
      QWidget::keyPressEvent(key_event);
  }
  emit unTrappedKeyPressEvent(key_event);
  QWidget::keyPressEvent(key_event);
}

///////////////////////////////////////////////////////////////
//              Actual functions

#ifdef TA_QT3D

void T3ExaminerViewer::setSceneGraph(Qt3D::QEntity* root) {
  if(scene) {
    scene->setParent((Qt3DNode*)NULL);
  }
  scene = root;
  scene->setParent(root_entity);
}

void T3ExaminerViewer::setBackgroundColor(const QColor & color) {
  bg_color = color;
  forward_renderer->setClearColor(bg_color);
}

#else

SoCamera* T3ExaminerViewer::getViewerCamera() const {
  SoEventManager* mgr = quarter->getSoEventManager();
  if(!mgr) return NULL;
  return mgr->getCamera();
}

const SbViewportRegion& T3ExaminerViewer::getViewportRegion() const {
  SoEventManager* mgr = quarter->getSoEventManager();
  return mgr->getViewportRegion(); // hope it works!
}
#endif


void T3ExaminerViewer::viewAll() {
#ifdef TA_QT3D
  camera->setUpVector(QVector3D(0, 1.0f, 0));
  camera->setPosition(QVector3D(0.0f, 0.0f, 5.0f));
  camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
#else
  SoCamera* cam = getViewerCamera();
  if(!cam) return; // can happen for empty scenegraph
  // restore camera position to head-on
  quarter->seek();  // stop the rotation
  cam->orientation.setValue(SbVec3f(-1.0f, 0.0f, 0.0f), 0.0f);
  quarter->viewAll();
  zoomView(-.35f);              // zoom in !!
#endif
}

// this is copied directly from SoQtFullViewer.cpp, which defines it as a static
// method on SoGuiFullViewerP

void T3ExaminerViewer::zoomView(const float diffvalue) {
#ifdef TA_QT3D
  //  float multiplicator = float(exp(diffvalue));
  float multiplicator = -diffvalue;

  if (camera->projectionType() == Qt3D::QCameraLens::OrthogonalProjection) {
    // Since there's no perspective, "zooming" in the original sense
    // of the word won't have any visible effect. So we just increase
    // or decrease the field-of-view values of the camera instead, to
    // "shrink" the projection size of the model / scene.
    float view_size = camera->aspectRatio();
    camera->setAspectRatio(view_size * multiplicator);
  }
  else if (camera->projectionType() == Qt3D::QCameraLens::PerspectiveProjection) {
    QVector3D direction(0.0f, 0.0f, -1.0f); // just move back in z for now
    const QVector3D oldpos = camera->position();
    const QVector3D newpos = oldpos + multiplicator * direction;
    camera->setPosition(newpos);

    // this kinda works but then doesn't after a while..
    
    // Qt3D::QTransform* trns = camera->transform();
    // QVector3D direction = trns->matrix().mapVector(QVector3D(0.0f, 0.0f, -1.0f));
    
    // const QVector3D oldpos = camera->position();
    // const QVector3D newpos = oldpos + multiplicator * direction;
    
    // const float distorigo = newpos.length();
    // // sqrt(FLT_MAX) == ~ 1e+19, which should be both safe for further
    // // calculations and ok for the end-user and app-programmer.
    // if (distorigo < 1.0e+19) {
    //   camera->setPosition(newpos);
    //   // camera->focalDistance = newfocaldist;
    // }
    // float zoom = camera->fieldOfView();
    // camera->setFieldOfView(zoom * multiplicator);
  }
#else
  SoCamera* cam = getViewerCamera();
  if(!cam) return; // can happen for empty scenegraph
  
  SoType t = cam->getTypeId();
  SbName tname = t.getName();
  
  // This will be in the range of <0, ->>.
  float multiplicator = float(exp(diffvalue));
  
  if (t.isDerivedFrom(SoOrthographicCamera::getClassTypeId())) {
    // Since there's no perspective, "zooming" in the original sense
    // of the word won't have any visible effect. So we just increase
    // or decrease the field-of-view values of the camera instead, to
    // "shrink" the projection size of the model / scene.
    SoOrthographicCamera * oc = (SoOrthographicCamera *)cam;
    oc->height = oc->height.getValue() * multiplicator;
  }
  else {
    // FrustumCamera can be found in the SmallChange CVS module (it's
    // a camera that lets you specify (for instance) an off-center
    // frustum (similar to glFrustum())
    if (!t.isDerivedFrom(SoPerspectiveCamera::getClassTypeId()) &&
        tname != "FrustumCamera") {
      static SbBool first = TRUE;
      if (first) {
        /*SoDebugError prolly doesn't exist in non-debug builds of Coin...
         SoDebugError::postWarning("SoGuiFullViewerP::zoom",
         "Unknown camera type, "
         "will zoom by moving position, but this might not be correct.");*/
        taMisc::Warning("SoGuiFullViewerP::zoom",
                        "Unknown camera type, will zoom by moving position, but this might not be correct.");
        first = FALSE;
      }
    }
    
    const float oldfocaldist = cam->focalDistance.getValue();
    const float newfocaldist = oldfocaldist * multiplicator;
    
    SbVec3f direction;
    cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
    
    const SbVec3f oldpos = cam->position.getValue();
    const SbVec3f newpos = oldpos + (newfocaldist - oldfocaldist) * -direction;
    
    // This catches a rather common user interface "buglet": if the
    // user zooms the camera out to a distance from origo larger than
    // what we still can safely do floating point calculations on
    // (i.e. without getting NaN or Inf values), the faulty floating
    // point values will propagate until we start to get debug error
    // messages and eventually an assert failure from core Coin code.
    //
    // With the below bounds check, this problem is avoided.
    //
    // (But note that we depend on the input argument ''diffvalue'' to
    // be small enough that zooming happens gradually. Ideally, we
    // should also check distorigo with isinf() and isnan() (or
    // inversely; isinfite()), but those only became standardized with
    // C99.)
    const float distorigo = newpos.length();
    // sqrt(FLT_MAX) == ~ 1e+19, which should be both safe for further
    // calculations and ok for the end-user and app-programmer.
    if (distorigo < 1.0e+19) {
      cam->position = newpos;
      cam->focalDistance = newfocaldist;
    }
  }
#endif
  syncViewerMode();             // keep it sync'd -- this tends to throw it off
}

void T3ExaminerViewer::horizRotateView(const float rot_value) {
#ifdef TA_QT3D
  camera->panAboutViewCenter(-rot_value);
#else
  RotateView(SbVec3f(0.0f, -1.0f, 0.0f), rot_value);
#endif
}

void T3ExaminerViewer::vertRotateView(const float rot_value) {
#ifdef TA_QT3D
  camera->tiltAboutViewCenter(rot_value);
#else
  RotateView(SbVec3f(-1.0f, 0.0f, 0.0f), rot_value);
#endif
}

void T3ExaminerViewer::horizPanView(const float pan_value) {
#ifdef TA_QT3D
  // camera->pan(-pan_value);
  PanView(QVector3D(-1.0f, 0.0f, 0.0f), pan_value);
#else
  PanView(SbVec3f(-1.0f, 0.0f, 0.0f), pan_value);
#endif
}

void T3ExaminerViewer::vertPanView(const float pan_value) {
#ifdef TA_QT3D
  // camera->tilt(pan_value);
  PanView(QVector3D(0.0f, 1.0f, 0.0f), pan_value);
#else
  PanView(SbVec3f(0.0f, 1.0f, 0.0f), pan_value);
#endif
}

#ifdef TA_QT3D

void T3ExaminerViewer::RotateView(const QVector3D& axis, const float ang) {
  QQuaternion rotation(ang, axis);
  camera->rotateAboutViewCenter(rotation);
  syncViewerMode();             // keep it sync'd -- this tends to throw it off
}

void T3ExaminerViewer::PanView(const QVector3D& dir, const float dist) {
  QVector3D mvec = dir * dist;
  QVector3D newpos = camera->position() + mvec;
  camera->setPosition(newpos);
  QVector3D newview = camera->viewCenter() + mvec;
  camera->setViewCenter(newview);
  syncViewerMode();             // keep it sync'd -- this tends to throw it off
}

#else

void T3ExaminerViewer::RotateView(const SbVec3f& axis, const float ang) {
  // copied from SoQtExaminerViewer.cpp -- hidden method on private P class
  SoCamera* cam = getViewerCamera();
  if(!cam) return;
  
  const SbVec3f DEFAULTDIRECTION(0, 0, -1);
  const SbRotation currentorientation = cam->orientation.getValue();
  
  SbVec3f currentdir;
  currentorientation.multVec(DEFAULTDIRECTION, currentdir);
  
  const SbVec3f focalpoint = cam->position.getValue() +
  cam->focalDistance.getValue() * currentdir;
  
  // set new orientation
  cam->orientation = SbRotation(axis, ang) * currentorientation;
  
  SbVec3f newdir;
  cam->orientation.getValue().multVec(DEFAULTDIRECTION, newdir);
  cam->position = focalpoint - cam->focalDistance.getValue() * newdir;
  syncViewerMode();             // keep it sync'd -- this tends to throw it off
}

void T3ExaminerViewer::PanView(const SbVec3f& dir, const float dist) {
  SoCamera* cam = getViewerCamera();
  if(!cam) return;
  
  SbVec3f mvec = dir * dist;
  
  SbVec3f newpos = cam->position.getValue() + mvec;
  cam->position = newpos;
  syncViewerMode();             // keep it sync'd -- this tends to throw it off
}

#endif

void T3ExaminerViewer::syncViewerMode() {
#ifndef TA_QT3D
  if(!quarter) return;
  bool int_onoff = (bool)viewer_mode;
  if(quarter->interactionModeOn() != int_onoff) {
    SoEventManager* emgr = quarter->getSoEventManager();
    if(int_onoff) {
      emgr->setNavigationState(SoEventManager::NO_NAVIGATION);
    }
    quarter->setInteractionModeOn(int_onoff);
    if(!int_onoff) {
      emgr->setNavigationState(SoEventManager::JUST_NAVIGATION);
    }
    // #ifdef DEBUG
    //     taMisc::Info("sync interact to:", String(int_onoff));
    // #endif
  }
#endif
}

bool T3ExaminerViewer::syncCursor() {
#ifndef TA_QT3D
  if(!quarter) return false;
  if(viewer_mode == INTERACT) {
    setCursor(quarter->stateCursor("interact"));
  }
  else {
    setCursor(quarter->cursor());
  }
#endif
  return true;
}

void T3ExaminerViewer::setInteractionModeOn(bool onoff, bool re_render) {
  viewer_mode = (ViewerMode)onoff; // enum matches
#ifndef TA_QT3D
  if(quarter->interactionModeOn() != onoff) {
    SoEventManager* emgr = quarter->getSoEventManager();
    if(onoff) {
      emgr->setNavigationState(SoEventManager::NO_NAVIGATION);
    }
    quarter->setInteractionModeOn(onoff);
    if(!onoff) {
      emgr->setNavigationState(SoEventManager::JUST_NAVIGATION); // after is best here
    }
    // #ifdef DEBUG
    //     taMisc::Info("set interact to:", String(onoff));
    // #endif
    if(re_render) {
      T3Panel* panl = GetPanel();
      if(panl) {
        panl->Render();
      }
    }
  }
  if(quarter->interactionModeOn()) {
    interact_button->setChecked(true);
    view_button->setChecked(false);
  }
  else {
    interact_button->setChecked(false);
    view_button->setChecked(true);
  }
#endif
}

void T3ExaminerViewer::saveView(int view_no) {
  if(view_no < 0 || view_no >= n_views) return;
  T3SavedView* sv = saved_views[view_no];
#ifdef TA_QT3D
  sv->getCameraParams(camera);
  if(sv->view_action)
    sv->view_action->setChecked(true);
#else // TA_QT3D
  SoCamera* cam = getViewerCamera();
  if(!cam) return;
  sv->getCameraParams(cam);
  if(sv->view_action)
    sv->view_action->setChecked(true);
#endif
  emit viewSaved(view_no);
}

void T3ExaminerViewer::gotoView(int view_no) {
  if(view_no < 0 || view_no >= n_views) return;
  T3SavedView* sv = saved_views[view_no];
#ifdef TA_QT3D
  sv->setCameraParams(camera);
#else // TA_QT3D
  SoCamera* cam = getViewerCamera();
  if(!cam) return;
  sv->setCameraParams(cam);
#endif
  cur_view_no = view_no;
  syncViewerMode();
  emit viewSelected(view_no);
}

void T3ExaminerViewer::nextView() {
  int next_view_no = cur_view_no + 1;
  if (next_view_no == saved_views.size) {
    next_view_no = 0;
  }
  gotoviewbuttonClicked(next_view_no);
}

void T3ExaminerViewer::previousView() {
  int previous_view_no = cur_view_no - 1;
  if (previous_view_no == -1) {
    previous_view_no = saved_views.size - 1;
  }
  gotoviewbuttonClicked(previous_view_no);
}

bool T3ExaminerViewer::nameView(int view_no, const String& label) {
  if(view_no < 0 || view_no >= n_views) return false;
  T3SavedView* sv = saved_views[view_no];
  sv->name = label;
  if(sv->view_action)
    sv->view_action->setText(label);
  emit viewSaved(view_no);      // view updated -- propagate
  return true;
}

void T3ExaminerViewer::updtViewName(int view_no) {
  if(view_no < 0 || view_no >= n_views) return;
  T3SavedView* sv = saved_views[view_no];
  if(sv->view_action) {
    sv->view_action->setText(sv->name);
    sv->view_action->setChecked(sv->view_saved);
  }
}

QImage  T3ExaminerViewer::grabImage() {
#ifdef TA_QT3D
  return QImage();
#else
  return quarter->grabFrameBuffer(true); // true = get alpha
#endif
}

void T3ExaminerViewer::saveImage(const QString& fname) {
  QImage img = grabImage();
  img.save(fname);
}

void T3ExaminerViewer::printImage() {
  QImage img = grabImage();
  QPrinter pr;
  QPrintDialog pd(&pr, this);
  if(pd.exec() == QDialog::Accepted) {
    QPainter p(&pr);
    p.drawImage(0, 0, img);
  }
}

bool T3ExaminerViewer::event(QEvent* ev_) {
#ifdef TA_QT3D
  return inherited::event(ev_);
#else
  static bool inside_event_loop = false;
  
  //NOTE: the base classes don't check if event is already handled, so we have to skip
  // calling inherited if we handle it ourselves
  
  bool rval = inherited::event(ev_);
  
  if(so_scrollbar_is_dragging) {
    if(!inside_event_loop) {
      inside_event_loop = true;
      while(so_scrollbar_is_dragging) { // remain inside local scroll event loop until end!
        taiMiscCore::ProcessEvents();
      }
      inside_event_loop = false;
    }
  }
  return rval;
#endif
}

bool T3ExaminerViewer::eventFilter(QObject* obj, QEvent* ev_) {
#ifdef TA_QT3D
  return inherited::eventFilter(obj, ev_);
#else
  if(!t3vw || (obj != this && obj != quarter)) goto do_inherited;
  
  if((ev_->type() == QEvent::MouseMove) || (ev_->type() == QEvent::MouseButtonPress) ||
     (ev_->type() == QEvent::Enter) || (ev_->type() == QEvent::FocusIn)) {
    syncViewerMode();
  }
  
  if(ev_->type() == QEvent::MouseButtonPress) {
    QMouseEvent* ev = (QMouseEvent*)ev_;
    if (ev->button() == Qt::RightButton) {
      ISelectable* ci = t3vw->curItem();
      if (!ci) goto do_inherited;
      ev->accept();
      t3vw->ContextMenuRequested(ev->globalPos());
      return true;
    }
  }
  
do_inherited:
  static bool inside_event_loop = false;
  
  if(so_scrollbar_is_dragging) {
    if(!inside_event_loop) {
      inside_event_loop = true;
      while(so_scrollbar_is_dragging) { // remain inside local scroll event loop until end!
        taiMiscCore::ProcessEvents();
      }
      inside_event_loop = false;
    }
  }
  
  return inherited::eventFilter(obj, ev_);
#endif
}

void T3ExaminerViewer::resizeEvent(QResizeEvent* ev) {
  inherited::resizeEvent(ev);
#ifdef TA_QT3D
  QSize sz = view3d->size();
  float aspect_ratio = (float)sz.width() / (float)sz.height();
  camera->lens()->setAspectRatio(aspect_ratio);
#endif // TA_QT3D
}

