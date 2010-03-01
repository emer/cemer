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



// t3viewer.cc

#include "t3viewer.h"

#include "ta_qt.h"
#include "ta_qttype.h"
#include "ta_qtdata.h"
#include "ta_qtclipdata.h"
#include "ta_qtdialog.h"

#include "css_machine.h" // for trace flag

#include <qapplication.h>
#include <qclipboard.h>
#include <qevent.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qmime.h>
#include <QMenu>
#include <qscrollbar.h>
#include <QTabWidget>
#include <QTimer>
#include <QPushButton>
#include <QGLWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QKeyEvent>

#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoOutput.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/annex/HardCopy/SoVectorizePSAction.h>
#include <Inventor/events/SoButtonEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/misc/SoBase.h>
//#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/SoEventManager.h>
#include <Inventor/SoRenderManager.h>

#include "qtthumbwheel.h"


const float t3Misc::pts_per_geom(72.0f);
const float t3Misc::geoms_per_pt(1/pts_per_geom);
const float t3Misc::char_ht_to_wd_pts(1.8f); // with proportional spacing, this is more accurate on average..
const float t3Misc::char_base_fract(0.20f); //TODO: find correct val from coin src

using namespace Qt;

// from: http://doc.trolltech.com/4.3/opengl-samplebuffers-glwidget-cpp.html
#ifndef GL_MULTISAMPLE
# define GL_MULTISAMPLE  0x809D
#endif

#include "pick.xpm"
#include "view.xpm"
// #include "home.xpm"
// #include "set_home.xpm"
#include "seek.xpm"
#include "view_all.xpm"
#include "print.xpm"
#include "snapshot.xpm"

#define WHEEL_LENGTH 60		// long axis
#define WHEEL_WIDTH 20		// short axis
#define BUTTON_WIDTH 20
#define BUTTON_HEIGHT 20

void T3SavedView::Initialize() {
  view_saved = false;
  view_button = NULL;
  view_action = NULL;
  focal_dist = 0.0f;
  pos = 0.0f;
}

void T3SavedView::getCameraParams(SoCamera* cam) {
  SbVec3f sb_pos = cam->position.getValue();
  SbVec3f sb_axis;
  cam->orientation.getValue(sb_axis, orient.rot);
  focal_dist = cam->focalDistance.getValue();

  pos.x = sb_pos[0]; pos.y = sb_pos[1]; pos.z = sb_pos[2];
  orient.x = sb_axis[0]; orient.y = sb_axis[1]; orient.z = sb_axis[2];

  view_saved = true;
}

bool T3SavedView::setCameraParams(SoCamera* cam) {
  if(!view_saved) return false;
  cam->position.setValue(pos.x, pos.y, pos.z);
  cam->orientation.setValue(SbVec3f(orient.x, orient.y, orient.z), orient.rot);
  cam->focalDistance.setValue(focal_dist);
  return true;
}

void T3SavedView_List::Initialize() {
  SetBaseType(&TA_T3SavedView);
}

void T3SavedView_List::SetCameraPos(int view_no, float x, float y, float z) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "SetCameraPos", "view no out of range:", String(view_no), "n views:",
	       String(size))) return;
  sv->SetCameraPos(x,y,z);
}

void T3SavedView_List::SetCameraOrient(int view_no, float x, float y, float z, float r) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "SetCameraOrient", "view no out of range:", String(view_no), "n views:",
	       String(size))) return;
  sv->SetCameraOrient(x,y,z,r);
}

void T3SavedView_List::SetCameraFocDist(int view_no, float fd) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "SetCameraFocDist", "view no out of range:", String(view_no), "n views:",
	       String(size))) return;
  sv->SetCameraFocDist(fd);
}

void T3SavedView_List::GetCameraPos(int view_no, float& x, float& y, float& z) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "GetCameraPos", "view no out of range:", String(view_no), "n views:",
	       String(size))) return;
  sv->GetCameraPos(x,y,z);
}

void T3SavedView_List::GetCameraOrient(int view_no, float& x, float& y, float& z, float& r) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "GetCameraOrient", "view no out of range:", String(view_no), "n views:",
	       String(size))) return;
  sv->GetCameraOrient(x,y,z,r);
}

void T3SavedView_List::GetCameraFocDist(int view_no, float& fd) {
  T3SavedView* sv = SafeEl(view_no);
  if(TestError(!sv, "GetCameraFocDist", "view no out of range:", String(view_no), "n views:",
	       String(size))) return;
  sv->GetCameraFocDist(fd);
}

const int T3ExaminerViewer::n_views = 6;

static void t3ev_config_wheel(QtThumbWheel* whl) {
  whl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  whl->setWrapsAround(true);
  whl->setLimitedDrag(false);
}

T3ExaminerViewer::T3ExaminerViewer(iT3ViewspaceWidget* parent)
  : QWidget(parent)
{
  t3vw = parent;
  quarter = NULL;		// for startup events

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

  quarter = new QuarterWidget(this);
  // set any initial configs for quarter widget here (or somewhere else if you please)
  quarter->setInteractionModeEnabled(true);
  quarter->setTransparencyType(QuarterWidget::BLEND); // this is a good default
  quarter->setNavigationModeFile(QUrl("coin:///scxml/navigation/examiner.xml"));
  quarter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  main_hbox->addWidget(quarter, 1);


  rhs_vbox = new QVBoxLayout;
  rhs_vbox->setMargin(0); rhs_vbox->setSpacing(0);
  main_hbox->addLayout(rhs_vbox);

  /////	make wheels all together

  hrot_wheel = new QtThumbWheel(0, 1000, 10, 500, Qt::Horizontal, this);
  t3ev_config_wheel(hrot_wheel);
  hrot_start_val = 500;
  hrot_wheel->setMaximumSize(WHEEL_LENGTH, WHEEL_WIDTH);
  connect(hrot_wheel, SIGNAL(valueChanged(int)), this, SLOT(hrotwheelChanged(int)));

  vrot_wheel = new QtThumbWheel(0, 1000, 10, 500, Qt::Vertical, this);
  t3ev_config_wheel(vrot_wheel);
  vrot_start_val = 500;
  vrot_wheel->setMaximumSize(WHEEL_WIDTH, WHEEL_LENGTH);
  connect(vrot_wheel, SIGNAL(valueChanged(int)), this, SLOT(vrotwheelChanged(int)));

  zoom_wheel = new QtThumbWheel(0, 1000, 10, 500, Qt::Vertical, this);
  t3ev_config_wheel(zoom_wheel);
  zoom_start_val = 500;
  zoom_wheel->setMaximumSize(WHEEL_WIDTH, WHEEL_LENGTH);
  connect(zoom_wheel, SIGNAL(valueChanged(int)), this, SLOT(zoomwheelChanged(int)));

  hpan_wheel = new QtThumbWheel(0, 1000, 10, 500, Qt::Horizontal, this);
  t3ev_config_wheel(hpan_wheel);
  hpan_start_val = 500;
  hpan_wheel->setMaximumSize(WHEEL_LENGTH, WHEEL_WIDTH);
  connect(hpan_wheel, SIGNAL(valueChanged(int)), this, SLOT(hpanwheelChanged(int)));

  vpan_wheel = new QtThumbWheel(0, 1000, 10, 500, Qt::Vertical, this);
  t3ev_config_wheel(vpan_wheel);
  vpan_start_val = 500;
  vpan_wheel->setMaximumSize(WHEEL_WIDTH, WHEEL_LENGTH);
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
  zoom_lbl->setToolTip("Wheel above will Zoom the view camera in and out -- keyboard arrows and PgUp/PgDn also work here, as does the scroll wheel (hold down Shift or Ctrl to move in finer steps)");
  rhs_vbox->addWidget(zoom_lbl);

  rhs_vbox->addSpacing(4);
  rhs_vbox->addWidget(vpan_wheel);

  /////  bot_hbox

  vrot_lbl = new QLabel("V.Rot", this);
  vrot_lbl->setToolTip("Wheel above will rotate the view vertically -- keyboard arrows and PgUp/PgDn also work here, as does the scroll wheel (hold down Shift or Ctrl to move in finer steps)");
  bot_hbox->addWidget(vrot_lbl);

  bot_hbox->addSpacing(4);

  hrot_lbl = new QLabel("H.Rot", this);
  hrot_lbl->setToolTip("Wheel to the right will rotate the view horizontally -- keyboard arrows and PgUp/PgDn also work here, as does the scroll wheel (hold down Shift or Ctrl to move in finer steps)");
  bot_hbox->addWidget(hrot_lbl);

  bot_hbox->addWidget(hrot_wheel);

  bot_hbox->addStretch();

  bot_button_hbox = new QHBoxLayout;
  bot_button_hbox->setMargin(0); rhs_button_vbox->setSpacing(0);
  bot_hbox->addLayout(bot_button_hbox);

  bot_hbox->addStretch();

  bot_hbox->addWidget(hpan_wheel);
  hpan_lbl = new QLabel("H.Pan", this);
  hpan_lbl->setToolTip("Wheel to the left will pan (move) the view horizontally -- keyboard arrows and PgUp/PgDn also work here, as does the scroll wheel (hold down Shift or Ctrl to move in finer steps)");
  bot_hbox->addWidget(hpan_lbl);

  bot_hbox->addSpacing(4);

  vpan_lbl = new QLabel("V.Pan ", this);
  vpan_lbl->setToolTip("Wheel above will pan (move) the view vertically -- keyboard arrows and PgUp/PgDn also work here, as does the scroll wheel (hold down Shift or Ctrl to move in finer steps)");
  bot_hbox->addWidget(vpan_lbl);

  Constr_RHS_Buttons();
  Constr_LHS_Buttons();
  Constr_Bot_Buttons();
}

T3ExaminerViewer::~T3ExaminerViewer() {
  
}

T3DataViewFrame* T3ExaminerViewer::GetFrame() {
  if(!t3vw) return NULL;
  iT3DataViewFrame* idvf = t3vw->i_data_frame();
  if(!idvf) return NULL;
  T3DataViewFrame* dvf = idvf->viewer();
  return dvf;
}

void T3ExaminerViewer::Constr_RHS_Buttons() {
  interact_button = new QToolButton(this);
  interact_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  interact_button->setCheckable(true);
  interact_button->setIcon(QPixmap((const char **)pick_xpm));
  interact_button->setToolTip("Interact (I key, or ESC to toggle): Allows you to select and manipulate objects in the display \n(ESC toggles between Interact and Camera View");
  connect(interact_button, SIGNAL(clicked()), this, SLOT(interactbuttonClicked()));
  rhs_button_vbox->addWidget(interact_button);

  view_button = new QToolButton(this);
  view_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  view_button->setCheckable(true);
  view_button->setChecked(true);
  view_button->setIcon(QPixmap((const char **)view_xpm));
  view_button->setToolTip("Camera View (V key, or ESC to toggle): Allows you to move the view around (click and drag to move; \nshift = move in the plane; ESC toggles between Camera View and Interact)");
  connect(view_button, SIGNAL(clicked()), this, SLOT(viewbuttonClicked()));
  rhs_button_vbox->addWidget(view_button);

  view_all_button = new QToolButton(this);
  view_all_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  view_all_button->setIcon(QPixmap((const char **)view_all_xpm));
  view_all_button->setToolTip("(A key) View All: repositions the camera view to the standard initial view with everything in view");
  connect(view_all_button, SIGNAL(clicked()), this, SLOT(viewallbuttonClicked()));
  rhs_button_vbox->addWidget(view_all_button);

  seek_button = new QToolButton(this);
  seek_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  seek_button->setIcon(QPixmap((const char **)seek_xpm));
  seek_button->setToolTip("Seek (S key): Click on objects (not text!) in the display and the camera will \nfocus in on the point where you click -- repeated clicks will zoom in further");
  connect(seek_button, SIGNAL(clicked()), this, SLOT(seekbuttonClicked()));
  rhs_button_vbox->addWidget(seek_button);

  snapshot_button = new QToolButton(this);
  snapshot_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  snapshot_button->setIcon(QPixmap((const char **)snapshot_xpm));
  snapshot_button->setToolTip("Snapshot: save the current viewer image to a file -- PNG format is recommended for most cases as it compresses well and is lossles.  Use EPS to produce a postscript format file that can be converted to PDF or other vector formats for editing.");
  connect(snapshot_button, SIGNAL(clicked()), this, SLOT(snapshotbuttonClicked()));
  rhs_button_vbox->addWidget(snapshot_button);

  print_button = new QToolButton(this);
  print_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  print_button->setIcon(QPixmap((const char **)print_xpm));
  print_button->setToolTip("Print: print the current viewer image to a printer -- uses the bitmap of screen image\n make window as large as possible for better quality");
  connect(print_button, SIGNAL(clicked()), this, SLOT(printbuttonClicked()));
  rhs_button_vbox->addWidget(print_button);
}

void T3ExaminerViewer::Constr_LHS_Buttons() {
  
}

void T3ExaminerViewer::Constr_Bot_Buttons() {
  cur_view_no = -1;		// nothing set yet
  saved_views.SetSize(n_views);
  for(int i=0; i<n_views; i++) {
    T3SavedView* sv = saved_views[i];
    String nm = sv->name;
    if(nm.contains("T3SavedView")) {		// uninitialized
      nm = "View " + String(i);
      sv->name = nm;
    }

    QToolButton* view_button = new iContextMenuButton(this);

    taiAction* view_act = new taiAction(i, nm, QKeySequence());
    view_act->connect(taiAction::int_act, this,  SLOT(gotoviewbuttonClicked(int))); 
    view_act->setParent(view_button);
    view_act->setCheckable(true);
    view_act->setChecked(sv->view_saved);

    view_button->setToolTip("Go To View: Restores display to previously saved viewing configuration -- hold mouse down for menu to save view and set view name");
    //    view_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    view_button->setDefaultAction(view_act);
    bot_button_hbox->addWidget(view_button);

    sv->view_action = view_act;
    sv->view_button = view_button;

    QMenu* view_menu = new QMenu(this);
    view_menu->setFont(taiM->menuFont(taiMisc::fonMedium));

    taiAction* save_act = new taiAction(i, "&Save View", QKeySequence());
    save_act->setStatusTip("Save the current view settings to this view button");
    save_act->connect(taiAction::int_act, this,  SLOT(saveviewTriggered(int))); 
    save_act->setParent(view_menu);
    view_menu->addAction(save_act);

    taiAction* name_act = new taiAction(i, "&Name View", QKeySequence());
    name_act->setStatusTip("Name this view button");
    name_act->connect(taiAction::int_act, this,  SLOT(nameviewTriggered(int))); 
    name_act->setParent(view_menu);
    view_menu->addAction(name_act);

    taiAction* sname_act = new taiAction(i, "&Save + Name", QKeySequence());
    sname_act->setStatusTip("Save the current view settings to this view button, and then name it");
    sname_act->connect(taiAction::int_act, this,  SLOT(savenameviewTriggered(int))); 
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
  taiAction* dyn_act = new taiAction(but_no, label, QKeySequence());
  dyn_act->connect(taiAction::int_act, this,  SLOT(dynbuttonClicked(int))); 
  dyn_act->setParent(dyn_button);
//   dyn_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  dyn_button->setDefaultAction(dyn_act);
  dyn_button->setToolTip(tooltip);
  dyn_button->setFont(taiM->buttonFont(taiMisc::sizMedium));
  lhs_button_vbox->addWidget(dyn_button);
  dyn_actions.Add(dyn_act);
  return but_no;
}

taiAction* T3ExaminerViewer::getDynButton(int but_no) {
  return dyn_actions.SafeEl(but_no);
}

taiAction* T3ExaminerViewer::getDynButtonName(const String& label) {
  int but_no = dyn_buttons.FindName(label);
  if(but_no < 0) return NULL;
  return getDynButton(but_no);
}

void T3ExaminerViewer::setDynButtonChecked(int but_no, bool onoff, bool mutex) {
  if(but_no < 0 || but_no >= dyn_actions.size) return;
  if(mutex) {
    for(int i=0;i<dyn_actions.size; i++) {
      taiAction* da = dyn_actions[i];
      da->setCheckable(true);
      da->setChecked(false);
    }
  }

  taiAction* da = dyn_actions[but_no];
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
//		Main Button Actions

#define ROT_DELTA_MULT  0.002f
#define PAN_DELTA_MULT  0.002f
#define ZOOM_DELTA_MULT 0.002f

void T3ExaminerViewer::hrotwheelChanged(int value) {
  // first detect wraparound
  if(hrot_start_val < 100 && value > 900) hrot_start_val += 1000;
  if(value < 100 && hrot_start_val > 900) hrot_start_val -= 1000;
  float delta = (float)(value - hrot_start_val);
  hrot_start_val = value;
  horizRotateView(ROT_DELTA_MULT * delta);
}

void T3ExaminerViewer::vrotwheelChanged(int value) {
  if(vrot_start_val < 100 && value > 900) vrot_start_val += 1000;
  if(value < 100 && vrot_start_val > 900) vrot_start_val -= 1000;
  float delta = (float)(value - vrot_start_val);
  vrot_start_val = value;
  vertRotateView(ROT_DELTA_MULT * delta);
}

void T3ExaminerViewer::zoomwheelChanged(int value) {
  if(zoom_start_val < 100 && value > 900) zoom_start_val += 1000;
  if(value < 100 && zoom_start_val > 900) zoom_start_val -= 1000;
  float delta = (float)(value - zoom_start_val);
  zoom_start_val = value;
  zoomView(-ZOOM_DELTA_MULT * delta); // direction is opposite
}

void T3ExaminerViewer::hpanwheelChanged(int value) {
  // first detect wraparound
  if(hpan_start_val < 100 && value > 900) hpan_start_val += 1000;
  if(value < 100 && hpan_start_val > 900) hpan_start_val -= 1000;
  float delta = (float)(value - hpan_start_val);
  hpan_start_val = value;
  horizPanView(PAN_DELTA_MULT * delta);
}

void T3ExaminerViewer::vpanwheelChanged(int value) {
  if(vpan_start_val < 100 && value > 900) vpan_start_val += 1000;
  if(value < 100 && vpan_start_val > 900) vpan_start_val -= 1000;
  float delta = (float)(value - vpan_start_val);
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
  quarter->seek();
}

void T3ExaminerViewer::snapshotbuttonClicked() {
  T3DataViewFrame* dvf = GetFrame();
  if(!dvf) return;
  dvf->CallFun("SaveImageAs");
}

void T3ExaminerViewer::printbuttonClicked() {
  T3DataViewFrame* dvf = GetFrame();
  if(!dvf) return;
  dvf->PrintImage();
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
  if(sv->EditDialog(true))		// modal!
    nameView(view_no, sv->name);
}

void T3ExaminerViewer::savenameviewTriggered(int view_no) {
  saveView(view_no);
  T3SavedView* sv = saved_views.SafeEl(view_no);
  if(!sv) return;
  if(sv->EditDialog(true))		// modal!
    nameView(view_no, sv->name);
}

void T3ExaminerViewer::dynbuttonClicked(int but_no) {
  emit dynbuttonActivated(but_no);
}

void T3ExaminerViewer::keyPressEvent(QKeyEvent* e) {
  if(e->key() == Qt::Key_Escape) {
    if(interactionModeOn()) {
      setInteractionModeOn(false);
    }
    else {
      setInteractionModeOn(true);
    }
    e->accept();
    return;
  }
  if(e->key() == Qt::Key_I) {
    setInteractionModeOn(true);
    e->accept();
    return;
  }
  if(e->key() == Qt::Key_V) {
    setInteractionModeOn(false);
    e->accept();
    return;
  }
  if((e->key() == Qt::Key_Home) || (e->key() == Qt::Key_H)) {
    gotoView(0);			// 0 is base guy
    e->accept();
    return;
  }
  if(e->key() == Qt::Key_A) {
    viewAll();
    e->accept();
    return;
  }
  if(e->key() == Qt::Key_S) {	// seek
    quarter->seek();
    e->accept();
    return;
  }
  QWidget::keyPressEvent(e);
}



///////////////////////////////////////////////////////////////
//		Actual functions

SoCamera* T3ExaminerViewer::getViewerCamera() const {
  SoEventManager* mgr = quarter->getSoEventManager();
  if(!mgr) return NULL;
  return mgr->getCamera();
}

const SbViewportRegion& T3ExaminerViewer::getViewportRegion() const {
  SoEventManager* mgr = quarter->getSoEventManager();
  return mgr->getViewportRegion(); // hope it works!
}

void T3ExaminerViewer::viewAll() {
  SoCamera* cam = getViewerCamera();
  if(!cam) return; // can happen for empty scenegraph
  // restore camera position to head-on
  cam->orientation.setValue(SbVec3f(-1.0f, 0.0f, 0.0f), 0.0f);
  quarter->viewAll();
  zoomView(-.35f);		// zoom in !!
}

// this is copied directly from SoQtFullViewer.cpp, which defines it as a static
// method on SoGuiFullViewerP

void T3ExaminerViewer::zoomView(const float diffvalue) {
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
  syncViewerMode();		// keep it sync'd -- this tends to throw it off
}

void T3ExaminerViewer::horizRotateView(const float rot_value) {
  RotateView(SbVec3f(0.0f, -1.0f, 0.0f), rot_value);
}

void T3ExaminerViewer::vertRotateView(const float rot_value) {
  RotateView(SbVec3f(-1.0f, 0.0f, 0.0f), rot_value);
}

void T3ExaminerViewer::horizPanView(const float pan_value) {
  PanView(SbVec3f(-1.0f, 0.0f, 0.0f), pan_value);
}

void T3ExaminerViewer::vertPanView(const float pan_value) {
  PanView(SbVec3f(0.0f, 1.0f, 0.0f), pan_value);
}

// copied from SoQtExaminerViewer.cpp -- hidden method on private P class

void T3ExaminerViewer::RotateView(const SbVec3f& axis, const float ang) {
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
  syncViewerMode();		// keep it sync'd -- this tends to throw it off
}

void T3ExaminerViewer::PanView(const SbVec3f& dir, const float dist) {
  SoCamera* cam = getViewerCamera();
  if(!cam) return;

  SbVec3f mvec = dir * dist;

  SbVec3f newpos = cam->position.getValue() + mvec;
  cam->position = newpos;
  syncViewerMode();		// keep it sync'd -- this tends to throw it off
}


void T3ExaminerViewer::syncViewerMode() {
  if(!quarter) return;
  bool int_onoff = (bool)viewer_mode;
  if(quarter->interactionModeOn() != int_onoff) {
    quarter->setInteractionModeOn(int_onoff);
  }
}

void T3ExaminerViewer::setInteractionModeOn(bool onoff, bool re_render) {
  viewer_mode = (ViewerMode)onoff; // enum matches
  if(quarter->interactionModeOn() != onoff) {
    quarter->setInteractionModeOn(onoff);
    if(re_render) {
      T3DataViewFrame* dvf = GetFrame();
      if(dvf) {
	dvf->Render();
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
}

void T3ExaminerViewer::saveView(int view_no) {
  if(view_no < 0 || view_no >= n_views) return;
  SoCamera* cam = getViewerCamera();
  if(!cam) return;
  T3SavedView* sv = saved_views[view_no];
  sv->getCameraParams(cam);
  if(sv->view_action)
    sv->view_action->setChecked(true);
  emit viewSaved(view_no);
}

void T3ExaminerViewer::gotoView(int view_no) {
  if(view_no < 0 || view_no >= n_views) return;
  SoCamera* cam = getViewerCamera();
  if(!cam) return;
  saved_views[view_no]->setCameraParams(cam);
  cur_view_no = view_no;
  syncViewerMode();
  emit viewSelected(view_no);
}

bool T3ExaminerViewer::nameView(int view_no, const String& label) {
  if(view_no < 0 || view_no >= n_views) return false;
  T3SavedView* sv = saved_views[view_no];
  sv->name = label;
  if(sv->view_action)
    sv->view_action->setText(label);
  emit viewSaved(view_no);	// view updated -- propagate
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

QImage	T3ExaminerViewer::grabImage() {
  return quarter->grabFrameBuffer(true); // true = get alpha
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

bool so_scrollbar_is_dragging = false; // referenced in t3node_so.cpp

bool T3ExaminerViewer::event(QEvent* ev_) {
  static bool inside_event_loop = false;

  // this is probably not necc -- too much!
//   syncViewerMode();		// keep it sync'd

  //NOTE: the base classes don't check if event is already handled, so we have to skip
  // calling inherited if we handle it ourselves

  if (!t3vw) goto do_inherited;

  if (ev_->type() == QEvent::MouseButtonPress) {
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
}

//////////////////////////
//	T3DataView	//
//////////////////////////

#ifdef TA_PROFILE
int T3DataView_inst_cnt = 0;
#endif


T3DataView* T3DataView_List::FindData(taBase* dat, int& idx) {
  for(idx=0; idx<size; idx++) {
    T3DataView* dv = FastEl(idx);
    if(dv->data() == dat) {
      return dv;
    }
  }
  idx = -1;
  return NULL;
}

T3DataView* T3DataView::GetViewFromPath(const SoPath* path_) {
  SoPath* path = path_->copy();
  path->ref();
  T3DataView* rval = NULL;
  // keep looking backwards from current tail, until we find a T3Node
  while (!rval && (path->getLength() > 0)) {
    SoNode* node = path->getTail();
    // T3Nodes
    if (node->getTypeId().isDerivedFrom(T3Node::getClassTypeId())) {
      T3Node* t3node = (T3Node*)node;
      rval = (T3DataView*)t3node->dataView();
      break;
    } 
    path->truncate(path->getLength() - 1);
  }
  path->unref();
  return rval;
}


void T3DataView::Initialize() {
  flags = 0; 
  m_md = NULL; // set later
//  m_viewer = NULL;
  last_child_node = NULL;
  m_transform = NULL;
//TODO: if dnd done, put these where flags are set:
//  setDragEnabled(flags & DNF_CAN_DRAG);
//  setDropEnabled(!(flags & DNF_NO_CAN_DROP));
#ifdef TA_PROFILE
  ++T3DataView_inst_cnt;
  if (cssMisc::refcnt_trace)
    cerr << "T3DataView_inst_cnt: " << T3DataView_inst_cnt << '\n';
#endif
}

void T3DataView::Destroy() {
//should not be needed...  DestroyPanels();
  CutLinks();
#ifdef TA_PROFILE
  --T3DataView_inst_cnt;
  if (cssMisc::refcnt_trace)
    cerr << "T3DataView_inst_cnt: " << T3DataView_inst_cnt << '\n';
#endif
}

void T3DataView::Copy_(const T3DataView& cp) {
  flags = cp.flags;
  m_md = cp.m_md;
  Clear_impl(); // hope this works!
  last_child_node = NULL;
  FloatTransform* ft = cp.m_transform; 
  if (ft)
    transform(true)->Copy(*ft);
}


void T3DataView::CutLinks() {
  Reset();
  if (m_transform) {
    m_transform->CutLinks();
    delete m_transform;
    m_transform = NULL;
  }
  inherited::CutLinks();
}

void T3DataView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(m_transform) {
    if(m_transform->rotate.x == 0.0f && m_transform->rotate.y == 0.0f && m_transform->rotate.z == 0.0f) {
      m_transform->rotate.z = 1.0f;	// axis must be defined, even if not used.
    }
  }
}

void T3DataView::AddRemoveChildNode(SoNode* node, bool adding) {
  if (m_node_so.ptr())
    AddRemoveChildNode_impl(node, adding);
#ifdef DEBUG
  else {
    taMisc::Warning("T3DataView::AddRemoveChildNode_impl",
      "Attempt to access NULL node_so");
  }
#endif
}

void T3DataView::AddRemoveChildNode_impl(SoNode* node, bool adding) {
  node_so()->addRemoveChildNode(node, adding);
}

void T3DataView::ChildClearing(taDataView* child_) { // child is always a T3DataView
  T3DataView* child = (T3DataView*)child_;
  // remove the visual rendering of child, if any
  T3Node* ch_so;
  if (!(ch_so = child->node_so())) return;
  AddRemoveChildNode(ch_so, false); // remove node
  child->setNode(NULL);
}

void T3DataView::ChildRendered(taDataView* child_) { // child is always a T3DataView
  if (!node_so()) return; // shouldn't happen
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if (!child) return;
  T3Node* ch_so;
  if (!(ch_so = child->node_so())) return; // shouldn't happen
  AddRemoveChildNode(ch_so, true); // add node
}

void T3DataView::ChildRemoving(taDataView* child_) {
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if (!child) return;
  // remove the visual rendering of child, if any
  SoNode* ch_so;
  if (!(ch_so = child->node_so())) return;
  AddRemoveChildNode(ch_so, false); // remove node
  child->setNode(NULL);
}

void T3DataView::Clear_impl() { // note: no absolute guarantee par will be T3DataView
  inherited::Clear_impl(); // does children
  // can't have any visual children, if we don't exist ourselves...
  // also, don't need to report to parent if we don't have any impl ourselves
  if (!m_node_so.ptr()) return;

  // we remove top-most item first, which results in only one update to the scene graph
  if (hasParent()) {
    parent()->ChildClearing(this); // parent clears us
  } else {
    setNode(NULL);
  }
}

taiDataLink* T3DataView::clipParLink(GuiContext sh_typ) const {
  return own_link(sh_typ);
}

void T3DataView::setNode(T3Node* node_) {
  if (m_node_so.ptr() == node_) return; // generally shouldn't happen
  if (m_node_so.ptr()) {
    T3DataViewFrame* dvf = GetFrame();
    iT3DataViewFrame* idvf;
    if (dvf && (idvf = dvf->widget())) {
      idvf->NodeDeleting(m_node_so); // just desels all, for now
    }
  }
  m_node_so = node_;
}


void T3DataView::Close() {
  if (m_parent) {
    m_parent->CloseChild(this);
  }
  //NOTE: we may delete at this point -- do not put any more code
}

void T3DataView::DataDestroying() {
  Close();
  //NOTE: we may delete at this point -- do not put any more code
}

void T3DataView::DataStructUpdateEnd_impl() {
  Reset(); // note: should be superfluous, since we did one on start
  BuildAll();
  Render();
}

void T3DataView::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
  inherited::DataDataChanged(dl, dcr, op1, op2);
  // if we have started a batch op, then Reset, since we will for sure eventually
  if ((dcr == DCR_STRUCT_UPDATE_BEGIN) && (dbuCnt() == 1))
    Reset();
}

void T3DataView::DataUpdateAfterEdit_impl() {
  inherited::DataUpdateAfterEdit_impl();
  DoActions(RENDER_IMPL);
  // this should be superfluous:
//   if (m_node_so)
//     m_node_so->touch();
}

T3DataViewFrame* T3DataView::GetFrame() {
  T3DataViewFrame* frame = GET_MY_OWNER(T3DataViewFrame);
  return frame;
}

T3ExaminerViewer* T3DataView::GetViewer() {
  T3DataViewFrame* frame = GetFrame();
  if(!frame || !frame->widget()) return NULL;
  T3ExaminerViewer* viewer = frame->widget()->t3viewer();
  return viewer;
}

void T3DataView::QueryEditActionsS_impl_(int& allowed, int& forbidden,
  GuiContext sh_typ) const 
{
  if ((sh_typ == GC_DUAL_DEF_DATA) && (flags & DNF_IS_MEMBER)) {
    forbidden |= (taiClipData::EA_CUT | taiClipData::EA_DELETE);
  }
  IObjectSelectable::QueryEditActionsS_impl_(allowed, forbidden, sh_typ);
}

ISelectableHost* T3DataView::host() const {
  T3DataViewRoot* root = ((T3DataView*)this)->root(); // safely cast away constness
  return (root) ? root->host : NULL;
}

bool T3DataView::isMapped() const {
  return (m_node_so);
}


void T3DataView::OnWindowBind(iT3DataViewFrame* vw) {
  OnWindowBind_impl(vw);
}

ISelectable* T3DataView::par() const {
  T3DataView* rval = parent();
  if (rval) return rval;
  else return NULL;
}

/* taiDataLink* T3DataView::par_link() const {
  if (hasParent()) return parent()->link();
  else     return NULL;
}*/

/* MemberDef* T3DataView::par_md() const {
  if (hasParent()) return parent()->md();
  else     return NULL;
} */


void T3DataView::ReInit() {
  ReInit_impl();
}

void T3DataView::ReInit_impl() {
  T3Node* node;
  if ((node = m_node_so.ptr()) != NULL) {
    node->clear();
  }
}

void T3DataView::Render_impl() {
  T3ExaminerViewer* vw = GetViewer();
  if(vw) vw->syncViewerMode();	// always make sure it is sync'd with what we think it should be

  T3Node* node = m_node_so.ptr();
  if (m_transform && node) {
    m_transform->CopyTo(node->transform());
  }
  inherited::Render_impl();
}

void T3DataView::Render_pre() {
  Constr_Node_impl();
  inherited::Render_pre(); // does children
  // we remove top-most item first, which results in only one update to the scene graph
  if (hasParent()) {
    parent()->ChildRendered(this);
  }
}

T3DataViewRoot* T3DataView::root() {
  T3DataViewRoot* rval = GET_MY_OWNER(T3DataViewRoot);
  return rval;
}

FloatTransform* T3DataView::transform(bool auto_create) {
  if (!m_transform && auto_create) {
    m_transform = new FloatTransform();
    taBase::Own(m_transform, this);
  }
  return m_transform;
}

void T3DataView::UpdateChildNames(T3DataView*) {
  //nothing
}

taiDataLink* T3DataView::viewLink() const {
  return (taiDataLink*)const_cast<T3DataView*>(this)->GetDataLink();
}  


void T3DataView::ViewProperties() {
  EditDialog();
}


//////////////////////////
//    T3DataViewPar	//
//////////////////////////

void T3DataViewPar::InitLinks() {
  inherited::InitLinks();
  taBase::Own(children, this);
}

void T3DataViewPar::CutLinks() {
  Reset();
  children.CutLinks();
  inherited::CutLinks();
}

void T3DataViewPar::Copy_(const T3DataViewPar& cp) {
  Clear_impl(); // hope this works!
  children = cp.children; 
}
void T3DataViewPar::CloseChild(taDataView* child) {
  child->Reset();
  children.RemoveEl(child);
}

void T3DataViewPar::DoActionChildren_impl(DataViewAction acts) {
  if (acts & CONSTR_MASK) {
    inherited::DoActionChildren_impl(acts);
    children.DoAction(acts);
  } else {
    children.DoAction(acts);
    inherited::DoActionChildren_impl(acts);
  }
}


/*nn??
void T3DataViewPar::InsertItem(T3DataView* item, T3DataView* after) {
  int where = 0;
  // the Group list
  if (after) {
    where = children.Find(after) + 1; //note: we want it to actually go after
  }
  children.Insert(item, where);
} */

void T3DataViewPar::OnWindowBind(iT3DataViewFrame* vw) {
  inherited::OnWindowBind(vw);
  for (int i = 0; i < children.size; ++i) {
    T3DataView* item = children.FastEl(i);
    item->OnWindowBind(vw);
  }
}

void T3DataViewPar::ReInit() {
  for (int i = children.size - 1; i >= 0; --i) {
    T3DataView* item = children.FastEl(i);
    item->ReInit();
  }
  ReInit_impl();
}

//////////////////////////
//    T3DataViewRoot	//
//////////////////////////

void T3DataViewRoot::Initialize() {
  host = NULL;
  children.SetBaseType(&TA_T3DataViewMain);
}

void T3DataViewRoot::Constr_Node_impl() {
  setNode(new T3NodeParent);
}

//////////////////////////
//    T3DataViewMain	//
//////////////////////////

void T3DataViewMain::InitLinks() {
  inherited::InitLinks();
  taBase::Own(main_xform, this);
}

void T3DataViewMain::Copy_(const T3DataViewMain& cp) {
  main_xform = cp.main_xform;
}


//////////////////////////
//   iSoSelectionEvent	//
//////////////////////////

iSoSelectionEvent::iSoSelectionEvent(bool is_selected_, const SoPath* path_)
: is_selected(is_selected_), path(path_)
{
}

//////////////////////////////////
//    iT3ViewspaceWidget	//
//////////////////////////////////

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
  int extNameLen;

  extNameLen = strlen(extName);
  end = p + strlen(p);
    
  while (p < end) {
    int n = strcspn(p, " ");
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
    qglw->setFormat(fmt);		// obs: this is supposedly deprecated..
    qglw->makeCurrent();
    glEnable(GL_MULTISAMPLE);
  }
  else {
    fmt.setSampleBuffers(false);
    qglw->setFormat(fmt);		// obs: this is supposedly deprecated..
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
  // 	if (this->data) glEnable(GL_TEXTURE_3D);
  // 	else glDisable(GL_TEXTURE_3D);
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
    rman->setAntialiasing(true, 1); // low-cost line smoothing
    m_t3viewer->quarter->setTransparencyType(QuarterWidget::BLEND);
    // make sure it has the transparency set for new guy
  }
  LayoutComponents();
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
    cerr << "Warning: iT3ViewspaceWidget::setSelMode not allowed to change when scene graph active\n";
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
  }
}

void iT3ViewspaceWidget::hideEvent(QHideEvent* ev) {
  if ((bool)m_top_view && (m_last_vis != -1)) {
    m_last_vis = -1;
    m_top_view->SetVisible(false);
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
    taiDataLink* link = t3node->effLink(ISelectable::GC_DEFAULT);
    if(link) {
      taBase* obj = (taBase*)link->data();
      if(obj)
	tabMisc::DelayedFunCall_gui(obj, "BrowserSelectMe");
    }
  }
  else {
    RemoveSelectedItem(t3node);
  }
  // notify to our frame that we have grabbed focus
  Emit_GotFocusSignal();
}


void iT3ViewspaceWidget::UpdateSelectedItems_impl() {
  // note: prolly not needed
}

//////////////////////////
//   iT3DataViewFrame	//
//////////////////////////

iT3DataViewFrame::iT3DataViewFrame(T3DataViewFrame* viewer_, QWidget* parent)
:inherited(parent), IDataViewWidget(viewer_)
{
  Init();
}

iT3DataViewFrame::~iT3DataViewFrame() {
  if (panel_set) {
    panel_set->ClosePanel();
  }
}

void iT3DataViewFrame::Constr_impl() {
  m_t3viewer->show();
}  

void iT3DataViewFrame::Init() {
  QVBoxLayout* lay = new QVBoxLayout(this);
  lay->setSpacing(0);  lay->setMargin(0);
  //create the so viewer
  t3vs = new iT3ViewspaceWidget(this);
  lay->addWidget(t3vs);
  t3vs->setSelMode(iT3ViewspaceWidget::SM_MULTI); // default

  m_t3viewer = new T3ExaminerViewer(t3vs);
  t3vs->setT3viewer(m_t3viewer);
  connect(m_t3viewer, SIGNAL(viewSaved(int)), this, SLOT(viewSaved(int)) );
}

void iT3DataViewFrame::viewSaved(int view_no) {
  T3DataViewFrame* t3dvf = viewer();
  if(!t3dvf) return;		// shouldn't happen
  t3dvf->GetSavedView(view_no);
}

void iT3DataViewFrame::fileExportInventor() {
  static QFileDialog* fd = NULL;
  SoNode* scene = m_t3viewer->quarter->getSceneGraph();
  if (!scene) {
    taiChoiceDialog::ErrorDialog(this, "No scene exists yet.", "No scene", false);
    return;
  }

  if (!fd) {
    fd = new QFileDialog(this, "fd");
    fd->setFilter( "Inventor files (*.iv)" );
  }
  fd->setFileMode(QFileDialog::AnyFile);
  if (!fd->exec()) return;
  QString fileName;
  {QStringList files = fd->selectedFiles();
  QString selected;
  if (!files.isEmpty())
     fileName = files[0];}

  // check if exists, to warn user
  QFile f(fileName.toLatin1());
  if (f.exists()) {
    if (taiChoiceDialog::ChoiceDialog(this, 
      "That file already exists, overwrite it?",
      "Confirm file overwrite",
      "&Ok" + taiChoiceDialog::delimiter + "&Cancel") != 0) return;
  }
  SoOutput out;
  if (!out.openFile(fileName.toLatin1())) {
    taiChoiceDialog::ErrorDialog(this, "Could not open file.", "File error", false);
    return;
  }
  SoWriteAction wa(&out);
  wa.apply(scene);

  out.closeFile();
}

void iT3DataViewFrame::NodeDeleting(T3Node* node) {
  if (t3vs->sel_so) {
    // deselect all the damn nodes because too complicated to try to figure out
    // how to deselect just one
    t3vs->sel_so->deselectAll();
  }
}

/*void iT3DataViewFrame::hideEvent(QHideEvent* ev) {
  inherited::hideEvent(ev);
  Showing(false);
}*/

void iT3DataViewFrame::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  Refresh();
/*// #ifdef TA_OS_MAC
  // this was needed on Mac as of 4.0.19 Qt 4.4.1+ to prevent
  // the occasional "white screen of death" that was occurring
//   taiMiscCore::ProcessEvents();
// #endif
  Showing(true);*/
}

void iT3DataViewFrame::Showing(bool showing) {
  if (panel_set) {
    // note: don't focus, because that results in hard-to-prevent side-effect
    // of spurious focusing when restoring windows, changing desktops, etc.
    panel_set->FrameShowing(showing, false);
  }
}

void iT3DataViewFrame::Render_pre() {
  //nothing
}

void iT3DataViewFrame::Render_impl() {
  //nothing
}

void iT3DataViewFrame::Render_post() {
//  nothing
}

void iT3DataViewFrame::Reset_impl() {
  setSceneTop(NULL);
}

void iT3DataViewFrame::Refresh_impl() {
  viewRefresh();
}

void iT3DataViewFrame::RegisterPanel(iViewPanelFrame* pan) {
    if (panel_set) {
      panel_set->AddSubPanel(pan);
    } 
#ifdef DEBUG
    else {
      taMisc::Warning("Attempt to RegisterPanel failed because it doesn't exist!");
    }
#endif
}

T3DataViewRoot* iT3DataViewFrame::root() {
  return (m_viewer) ? &(((T3DataViewFrame*)m_viewer)->root_view) : NULL;
}

void iT3DataViewFrame::setSceneTop(SoNode* node) {
  t3vs->setSceneGraph(node);
}

void iT3DataViewFrame::T3DataViewClosing(T3DataView* node) {
}

iT3DataViewer* iT3DataViewFrame::viewerWidget() const {
//note: this fun not called much, usually only once on constr, so not cached
  QWidget* par = const_cast<iT3DataViewFrame*>(this); // ok to cast away constness
  while ((par = par->parentWidget())) {
    iT3DataViewer* rval = qobject_cast<iT3DataViewer*>(par);
    if (rval) return rval;
  }
  return NULL;
}

void iT3DataViewFrame::viewRefresh() {
  if (viewer())
    viewer()->Render();
}


//////////////////////////
//	T3DataViewFrame	//
//////////////////////////

void T3DataViewFrame::Initialize() {
//  link_type = &TA_T3DataLink;
  bg_color.setColorName(taMisc::t3d_bg_color);
  text_color.setColorName(taMisc::t3d_text_color);
  headlight_on = true;
  stereo_view = STEREO_NONE;
}

void T3DataViewFrame::Destroy() {
  Reset();
  CutLinks();
}

void T3DataViewFrame::InitLinks() {
  inherited::InitLinks();
  taBase::Own(root_view, this);
  taBase::Own(bg_color, this);
  taBase::Own(text_color, this);
  taBase::Own(saved_views, this);
}

void T3DataViewFrame::CutLinks() {
  bg_color.CutLinks();
  text_color.CutLinks();
  root_view.CutLinks();
  inherited::CutLinks();
}

void T3DataViewFrame::Copy_(const T3DataViewFrame& cp) {
  root_view = cp.root_view;
  bg_color = cp.bg_color;
  text_color = cp.text_color;
}


void T3DataViewFrame::AddView(T3DataView* view) {
  root_view.children.Add(view);
  if (dvwidget())
    view->OnWindowBind(widget());
}

// note: dispatchers for these _impl always check for the widget
void T3DataViewFrame::Clear_impl() {
  root_view.Clear_impl();
  widget()->Reset_impl();
  inherited::Clear_impl();
}

void T3DataViewFrame::Constr_impl(QWidget* gui_parent) {
  inherited::Constr_impl(gui_parent);
  root_view.host = widget()->t3vs; 
  // note: set top view to the root, not us, because we don't pass doactions down
  widget()->t3vs->setTopView(&root_view);
}

void T3DataViewFrame::Constr_post() {
  inherited::Constr_post();
  root_view.OnWindowBind(widget());
//   SetCameraPosOrient();
}

IDataViewWidget* T3DataViewFrame::ConstrWidget_impl(QWidget* gui_parent) {
  iT3DataViewFrame* rval = new iT3DataViewFrame(this, gui_parent);
  // make the corresponding viewpanelset
  MainWindowViewer* mwv = GET_MY_OWNER(MainWindowViewer);
  int idx;
  PanelViewer* pv = (PanelViewer*)mwv->FindFrameByType(&TA_PanelViewer, idx);
  iTabViewer* itv = pv->widget();
  taiDataLink* dl = (taiDataLink*)GetDataLink();
  iViewPanelSet* ivps = new iViewPanelSet(dl);
  rval->panel_set = ivps;
  itv->AddPanelNewTab(ivps);
  return rval;
}

void T3DataViewFrame::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if (dcr <= DCR_ITEM_UPDATED_ND) {
    T3DataViewer* par = GET_MY_OWNER(T3DataViewer);
    if (par) par->FrameChanged(this);
  }
}

T3DataView* T3DataViewFrame::FindRootViewOfData(taBase* data) {
  if (!data) return NULL;
  for (int i = 0; i < root_view.children.size; ++i) {
    T3DataView* dv;
    if (!(dv = dynamic_cast<T3DataView*>(root_view.children[i]))) continue;
    if (dv->data() == data) return dv;
  }
  return NULL;
}

T3DataView* T3DataViewFrame::singleChild() const {
  if (root_view.children.size != 1) return NULL;
  T3DataView* rval = dynamic_cast<T3DataView*>(root_view.children[0]);
  return rval;
}


const iColor T3DataViewFrame::GetBgColor() const {
  bool sm = singleMode();
  iColor rval;
  if (sm) {
    T3DataView* sng = singleChild();
    if (sng) {
      bool ok = false;
      rval = sng->bgColor(ok);
      if (ok) {
	if(bg_color.r != 0.8f || bg_color.g != 0.8f || bg_color.b != 0.8f) 
	  rval = bg_color;	// when frame is set to something different, it overrides regardless
	return rval;
      }
    }
  }
  rval = bg_color;
  return rval;
}

const iColor T3DataViewFrame::GetTextColor() const {
  return text_color;  // no actual logic required it seems
}

void T3DataViewFrame::Render_pre() {
  inherited::Render_pre();
  widget()->Render_pre();
  root_view.Render_pre();

  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(viewer) {
    if(viewer->cur_view_no < 0) {
      SetAllSavedViews();		// init from us
      viewer->gotoView(0);		// goto first saved view as default
    }
  }
}

void T3DataViewFrame::Render_impl() {
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(viewer) {
    QColor bg = (QColor)GetBgColor();
    if(viewer->quarter->backgroundColor() != bg)
      viewer->quarter->setBackgroundColor(bg);
    if(viewer->quarter->stereoMode() != (QuarterWidget::StereoMode)stereo_view)
      viewer->quarter->setStereoMode((QuarterWidget::StereoMode)stereo_view);
    if(viewer->quarter->headlightEnabled() != headlight_on)
      viewer->quarter->setHeadlightEnabled(headlight_on);
    viewer->syncViewerMode();	// keep it in sync
  }
  inherited::Render_impl();
  root_view.Render_impl();
  widget()->Render_impl();
}

void T3DataViewFrame::Render_post() {
  inherited::Render_post();
  root_view.Render_post();
  widget()->setSceneTop(root_view.node_so());
  widget()->Render_post();
  // on first opening, do a viewall to center all geometry in viewer
  if(saved_views[0]->pos == 0.0f && saved_views[0]->focal_dist == 0.0f) {
    ViewAll();
    SaveCurView(0);		// save to 0 view
  }
}

void T3DataViewFrame::Reset_impl() {
  root_view.Reset();
  inherited::Reset_impl();
}

void T3DataViewFrame::WindowClosing(CancelOp& cancel_op) {
  inherited::WindowClosing(cancel_op);
  if (cancel_op != CO_CANCEL) {
    root_view.DoActions(CLEAR_IMPL);
    root_view.host = NULL;
  }
}

void T3DataViewFrame::ViewAll() {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(viewer)
    viewer->viewAll();
}

void T3DataViewFrame::GetSavedView(int view_no) {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer || view_no < 0 || view_no >= viewer->n_views) return;
  T3SavedView* oursc = saved_views.SafeEl(view_no);
  T3SavedView* sc = viewer->saved_views.SafeEl(view_no);
  if(!sc || !oursc) return;
  oursc->CopyFrom(sc);	// initialize from them
  oursc->name = sc->name;	// names not usu copied
}

void T3DataViewFrame::SetSavedView(int view_no) {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer || view_no < 0 || view_no >= viewer->n_views) return;
  T3SavedView* oursc = saved_views.SafeEl(view_no);
  T3SavedView* sc = viewer->saved_views.SafeEl(view_no);
  if(!sc || !oursc) return;
  sc->CopyFrom(oursc);	// initialize from us
  sc->name = oursc->name;	// names not usu copied
  viewer->updtViewName(view_no); // trigger update of label
}

void T3DataViewFrame::SetAllSavedViews() {
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer) return;
  saved_views.SetSize(viewer->n_views); // make sure
  for(int i=0;i<saved_views.size;i++) {
    T3SavedView* sv = saved_views[i];
    if(sv->name.contains("T3SavedView")) {		// uninitialized
      sv->name = "View " + String(i);
    }
    SetSavedView(i);
  }
}

void T3DataViewFrame::SaveCurView(int view_no) {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(view_no < 0 || view_no >= viewer->n_views) return;
  viewer->saveView(view_no);  // this automatically calls signal to update us
}

void T3DataViewFrame::GoToSavedView(int view_no) {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(view_no < 0 || view_no >= viewer->n_views) return;
  SetSavedView(view_no);
  viewer->gotoView(view_no);
}

QPixmap T3DataViewFrame::GrabImage(bool& got_image) {
  got_image = false;
  if(!widget()) {
    return QPixmap();
  }
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(TestError(!viewer, "GrabImage", "viewer is NULL!")) return QPixmap();
  QImage img = viewer->grabImage();
  if(TestError(img.isNull(), "GrabImage", "got a null image from T3ExaminerViewer"))
    return QPixmap();
  got_image = true;
//   cerr << "Grabbed image of size: " << img.width() << " x " << img.height() << " depth: " << img.depth() << endl;
  return QPixmap::fromImage(img);     // more costly but works!
}

bool T3DataViewFrame::SaveImageAs(const String& fname, ImageFormat img_fmt) {
  if(!widget()) return false;
  if(img_fmt == EPS)
    return SaveImageEPS(fname);
  if(img_fmt == IV)
    return SaveImageIV(fname);

  return inherited::SaveImageAs(fname, img_fmt);
}      

bool T3DataViewFrame::SaveImageEPS(const String& fname) {
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer) return false;

  String ext = String(".") + image_exts.SafeEl(EPS);
  taFiler* flr = GetSaveFiler(fname, ext);
  if(!flr->ostrm) {
    flr->Close();
    taRefN::unRefDone(flr);
    return false;
  }
  flr->Close();

  SoVectorizePSAction * ps = new SoVectorizePSAction;
  SoVectorOutput * out = ps->getOutput();

  if (!out->openFile(flr->FileName())) {
    return false; // unable to open output file
  }

  // to enable gouraud shading. 0.1 is a nice epsilon value
  // ps->setGouraudThreshold(0.1f);

  // clear to white background. Not really necessary if you
  // want a white background
  ps->setBackgroundColor(TRUE, SbColor(1.0f, 1.0f, 1.0f));

  // select LANDSCAPE or PORTRAIT orientation
  //  ps->setOrientation(SoVectorizeAction::LANDSCAPE);
  ps->setOrientation(SoVectorizeAction::PORTRAIT);

  // compute size based on actual image..  190.0f max width/height (= 7.5in)
  float wd = widget()->width();
  float ht = widget()->height();
  float pwd, pht;
  if(wd > ht) {
    pwd = 190.0f; pht = (ht/wd) * 190.0f;
  }
  else {
    pht = 190.0f; pwd = (wd/ht) * 190.0f;
  }

  // start creating a new page (based on actual size)
  ps->beginPage(SbVec2f(0.0f, 0.0f), SbVec2f(pwd, pht));

  // There are also enums for A0-A10. Example:
  //   ps->beginStandardPage(SoVectorizeAction::A4, 30.0f);

  // calibrate so that text, lines, points and images will have the
  // same size in the postscript file as on the monitor.
  ps->calibrate(viewer->getViewportRegion());

  // apply action on the viewer scenegraph. Remember to use
  // SoSceneManager's scene graph so that the camera is included.
  ps->apply(viewer->quarter->getSoRenderManager()->getSceneGraph());

  // this will create the postscript file
  ps->endPage();

  // close file
  out->closeFile();

  delete ps;
  taRefN::unRefDone(flr);
  return true;
}

bool T3DataViewFrame::SaveImageIV(const String& fname) {
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer) return false;

  String ext = String(".") + image_exts.SafeEl(IV);
  taFiler* flr = GetSaveFiler(fname, ext);
  if(!flr->ostrm) {
    flr->Close();
    taRefN::unRefDone(flr);
    return false;
  }
  flr->Close();

  SoOutput out;
  if(!out.openFile(flr->FileName())) return false;
  SoWriteAction wa(&out);

  wa.apply(root_view.node_so()); // just the data, not the whole camera
  //  wa.apply(viewer->quarter->getSceneGraph());
  out.closeFile();

  taRefN::unRefDone(flr);
  return true;
}

void T3DataViewFrame::SetImageSize(int width, int height) {
  if(!widget()) return;
  T3ExaminerViewer* viewer = widget()->t3viewer();
  if(!viewer || !viewer->quarter) return;
  // note: these may not be the same on all platforms!! works for me on my mac.. :)
  viewer->quarter->resize(width, height);
}

//////////////////////////
//   iTabBarEx		//
//////////////////////////

iTabBarEx::iTabBarEx(iTabWidget* parent)
:inherited(parent)
{
  m_tab_widget = parent;
}

void iTabBarEx::contextMenuEvent(QContextMenuEvent * e) {
  // find the tab being clicked, or -1 if none
  int idx = count() - 1;
  while (idx >= 0) {
    if (tabRect(idx).contains(e->pos())) break;
    --idx;
  }
  QPoint gpos = mapToGlobal(e->pos());
  if (m_tab_widget) m_tab_widget->emit_customContextMenuRequested2(gpos, idx);
}

iTabWidget::iTabWidget(QWidget* parent)
:inherited(parent)
{
  iTabBarEx* itbex = new iTabBarEx(this);
  setTabBar(itbex);
  //  itbex->setFocusPolicy(Qt::NoFocus); // do not focus on this guy -- nothing useful here
}

iTabBarBase* iTabWidget::GetTabBar() {
  return (iTabBarBase*)tabBar();
}

void iTabWidget::emit_customContextMenuRequested2(const QPoint& pos,
     int tab_idx)
{
  emit customContextMenuRequested2(pos, tab_idx);
}

void iTabWidget::contextMenuEvent(QContextMenuEvent* e) {
  // NOTE: this is not a good thing because the viewer has its own menu that
  // does everything it needs, and this conflicts
//   QPoint gpos = mapToGlobal(e->pos());
//   emit_customContextMenuRequested2(gpos, -1);
}

//////////////////////////
//   iT3DataViewer	//
//////////////////////////

iT3DataViewer::iT3DataViewer(T3DataViewer* viewer_, QWidget* parent)
:inherited(viewer_, parent)
{
  Init();
}

iT3DataViewer::~iT3DataViewer() {
}

void iT3DataViewer::Init() {
  last_idx = -1;
  QVBoxLayout* lay = new QVBoxLayout(this);
  lay->setSpacing(0);  lay->setMargin(0);
  tw = new iTabWidget(this); //top, standard tabs
#if (QT_VERSION >= 0x040200)
  tw->setUsesScrollButtons(true);
  tw->setElideMode(Qt::ElideMiddle/*Qt::ElideNone*/); // fixed now..
  //  tw->setElideMode(Qt::ElideNone); // don't elide, because it does it even when enough room, and it is ugly and confusing
#endif
  lay->addWidget(tw);
  connect(tw, SIGNAL(customContextMenuRequested2(const QPoint&, int)),
	  this, SLOT(tw_customContextMenuRequested2(const QPoint&, int)) );
  connect(tw, SIGNAL(currentChanged(int)),
	  this, SLOT(tw_currentChanged(int)) );
  // punt, and just connect a timer to focus first tab
  // if any ProcessEvents get called (should not!) may have to make non-zero time
  QTimer::singleShot(0, this, SLOT(FocusFirstTab()) );
}

void iT3DataViewer::AddT3DataViewFrame(iT3DataViewFrame* idvf, int idx) {
  T3DataViewFrame* dvf = idvf->viewer();
  String tab_label = dvf->GetName();
  if (idx < 0)
    idx = tw->addTab(idvf, tab_label);
  else
    tw->insertTab(idx, idvf, tab_label);
  idvf->t3vs->Connect_SelectableHostNotifySignal(this, 
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
  tw->setCurrentIndex(idx); // not selected automatically
}

void iT3DataViewer::AddFrame() {
// T3DataViewFrame* fr  = 
 viewer()->NewT3DataViewFrame();
}

void iT3DataViewer::DeleteFrame(int tab_idx) {
  T3DataViewFrame* fr = viewFrame(tab_idx);
  if (!fr) return;
  fr->Close();
  //NOTE: do not place any code here -- we are deleted!
}

void iT3DataViewer::FrameProperties(int tab_idx) {
  T3DataViewFrame* fr = viewFrame(tab_idx);
  if (!fr) return;
  fr->EditDialog(true);
}

void iT3DataViewer::FillContextMenu_impl(taiMenu* menu, int tab_idx) {
  taiAction*
  act = menu->AddItem("&Add Frame", taiAction::action,
    this, SLOT(AddFrame()),_nilVariant);
  
  if (tab_idx >= 0) {
    act = menu->AddItem("&Delete Frame", taiAction::int_act,
      this, SLOT(DeleteFrame(int)), tab_idx);
    
    menu->AddSep();
    // should always be at bottom:
    act = menu->AddItem("Frame &Properties...", taiAction::int_act,
      this, SLOT(FrameProperties(int)), tab_idx);

  }
}

void iT3DataViewer::FocusFirstTab() {
  if (tw->count() > 0) {
    if (tw->currentIndex() == 0) {
      tw_currentChanged(0);
    } else {
      tw->setCurrentIndex(0);
    }
  }

}

void iT3DataViewer::tw_customContextMenuRequested2(const QPoint& pos, int tab_idx) {
  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);
  FillContextMenu_impl(menu, tab_idx);
  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
}

void iT3DataViewer::tw_currentChanged(int tab_idx) {
//note: the backwards order below fulfills two competing requirements:
// 1) have a hide/show insures we don't get multiple tabs
// 2) but show/hide order prevents panel tab switching away
// TODO: known bug: when you delete a frame, it switches from CtrlPanel tab
// different logic required when tab numbers are the same!
  iT3DataViewFrame* idvf;
  if(tab_idx == last_idx) {
    idvf = iViewFrame(tab_idx);
    idvf->Showing(true);
  }
  else {
    idvf = iViewFrame(tab_idx);
    if (idvf) { // should exist
      idvf->Showing(true);
      idvf = iViewFrame(last_idx);
      if (idvf) { 
	idvf->Showing(false);
      }
    }
  }
  last_idx = tab_idx;
}

iT3DataViewFrame* iT3DataViewer::iViewFrame(int idx) const {
  iT3DataViewFrame* rval = NULL;
  if ((idx >= 0) && (idx < tw->count())) {
    rval = qobject_cast<iT3DataViewFrame*>(tw->widget(idx));
  }
  return rval;
}

T3DataViewFrame* iT3DataViewer::viewFrame(int idx) const {
  iT3DataViewFrame* idvf = iViewFrame(idx);
  if (idvf) return idvf->viewer();
  else return NULL;
}


void iT3DataViewer::Refresh_impl() {
  for (int i = 0; i < viewer()->frames.size; ++i) {
    T3DataViewFrame* dvf = viewer()->frames.FastEl(i);
    iT3DataViewFrame* idvf = dvf->widget();
    if (!idvf) continue;
    idvf->Refresh();
  }
  UpdateTabNames();
  inherited::Refresh_impl(); // prob nothing
}

void iT3DataViewer::UpdateTabNames() {
  for (int i = 0; i < viewer()->frames.size; ++i) {
    T3DataViewFrame* dvf = viewer()->frames.FastEl(i);
    iT3DataViewFrame* idvf = dvf->widget();
    if (!idvf) continue;
    int idx = tw->indexOf(idvf);
    if (idx >= 0)
      tw->setTabText(idx, dvf->GetName());
  }
}

int iT3DataViewer::TabIndexByName(const String& nm) const {
  for (int i = 0; i < tw->count(); ++i) {
    if(tw->tabText(i) == nm) return i;
  }
  return -1;
}

bool iT3DataViewer::SetCurrentTab(int tab_idx) {
  if(tab_idx < 0 || tab_idx >= tw->count()) return false;
  tw->setCurrentIndex(tab_idx);
  return true;
}

bool iT3DataViewer::SetCurrentTabName(const String& tab_nm) {
  int tab_idx = TabIndexByName(tab_nm);
  if(tab_idx >= 0) {
    return SetCurrentTab(tab_idx);
  }
  return false;
}

void iT3DataViewer::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev);
  MainWindowViewer* mwv = viewer()->mainWindowViewer();
  if(mwv && mwv->widget()) {
    mwv->widget()->FocusIsRightViewer();
  }
}


//////////////////////////
//	T3DataViewer	//
//////////////////////////

T3DataViewFrame* T3DataViewer::GetBlankOrNewT3DataViewFrame(taBase* obj) {
  if (!obj) return NULL;
  T3DataViewFrame* fr = NULL;
  taProject* proj = (taProject*)obj->GetOwner(&TA_taProject);
  MainWindowViewer* vw = MainWindowViewer::GetDefaultProjectViewer(proj);
  if (!vw) return NULL; // shouldn't happen
  int idx;
  T3DataViewer* t3vw = (T3DataViewer*)vw->FindFrameByType(&TA_T3DataViewer, idx);
  if (!t3vw) return NULL; // shouldn't happen
  // make in default, if default is empty
  fr = t3vw->FirstEmptyT3DataViewFrame();
  if(!fr || fr->root_view.children.size == 0) {
    if(!fr)
      fr = t3vw->NewT3DataViewFrame();
    fr->SetName(obj->GetDisplayName()); // tis better to have one good name.. can always
    fr->UpdateAfterEdit();		// show name
  }
  return fr;
}

void T3DataViewer::Initialize() {
//  link_type = &TA_T3DataLink;
}

void T3DataViewer::Destroy() {
  Reset();
  CutLinks();
}

void T3DataViewer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(frames, this);
  // add a default frame, if none yet
  if (frames.size == 0) {
    //T3DataViewFrame* fv = 
    (T3DataViewFrame*)frames.New(1);
    //nuke fv->SetName("DefaultFrame");
  }
}

void T3DataViewer::CutLinks() {
  frames.CutLinks();
  inherited::CutLinks();
}

void T3DataViewer::Copy_(const T3DataViewer& cp) {
  frames = cp.frames;
}

IDataViewWidget* T3DataViewer::ConstrWidget_impl(QWidget* gui_parent) {
  return new iT3DataViewer(this, gui_parent);
}

void T3DataViewer::Constr_impl(QWidget* gui_parent) {
  inherited::Constr_impl(gui_parent); // prob just creates the widget
  if (!dvwidget()) return; // shouldn't happen
  
  ConstrFrames_impl();
}

void T3DataViewer::ConstrFrames_impl() {
  iT3DataViewer* idv = widget(); //cache
  T3DataViewFrame* fv = NULL;
  for (int i = 0; i < frames.size; ++i) {
    fv = frames.FastEl(i);
    if (!fv) continue; // shouldn't happen
    // note: don't parent the frame, since we use the api to add it
    ((DataViewer*)fv)->Constr_impl(NULL);
    idv->AddT3DataViewFrame(fv->widget());
  }
  MainWindowViewer* mwv = mainWindowViewer();
  if(mwv && mwv->widget()) {
    idv->installEventFilter(mwv->widget());
    idv->tabBar()->installEventFilter(mwv->widget());
  }
}

void T3DataViewer::DataChanged_Child(taBase* child, int dcr, void* op1, void* op2) {
  if (child == &frames) {
    // if reorder, then do a gui reorder
    //TODO:if new addition when mapped, then add gui
  }
}

void T3DataViewer::DoActionChildren_impl(DataViewAction act) {
// note: only ever called with one action
  if (act & CONSTR_MASK) {
    inherited::DoActionChildren_impl(act);
    frames.DoAction(act);
  } else { // DESTR_MASK
    frames.DoAction(act);
    inherited::DoActionChildren_impl(act);
  }
}


T3DataView* T3DataViewer::FindRootViewOfData(taBase* data) {
  if (!data) return NULL;
  for (int i = 0; i < frames.size; ++i) {
    T3DataViewFrame* f = frames.FastEl(i);
    T3DataView* dv = f->FindRootViewOfData(data);
    if (dv) return dv;
  }
  return NULL;
}

T3DataViewFrame* T3DataViewer::FirstEmptyT3DataViewFrame() {
  for (int i = 0; i < frames.size; ++i) {
    T3DataViewFrame* fv = frames.FastEl(i);
    if (fv->root_view.children.size == 0) 
      return fv;
  }
  return NULL;
}

void T3DataViewer::FrameChanged(T3DataViewFrame* frame) {
  // just update all the tab names, in case that is what changed
  if (isMapped()) {
    widget()->UpdateTabNames();
  }
}

void T3DataViewer::GetWinState_impl() {
  inherited::GetWinState_impl();
//  iT3DataViewer* w = widget();
//  int view_frame_selected = w->tw->currentIndex();
  SetUserData("view_frame_selected", widget()->isVisible());
  DataChanged(DCR_ITEM_UPDATED);
}

void T3DataViewer::SetWinState_impl() {
  inherited::SetWinState_impl();
  iT3DataViewer* w = widget();
  int view_frame_selected = 
    GetUserDataDef("view_frame_selected", 0).toInt();
  if (view_frame_selected < w->tw->count())
    w->tw->setCurrentIndex(view_frame_selected);
}

T3DataViewFrame* T3DataViewer::NewT3DataViewFrame() {
  T3DataViewFrame* fv = (T3DataViewFrame*)frames.New(1);
  iT3DataViewer* idv = widget(); //cache
  if (idv) {
    // note: don't parent the frame, since we use the api to add it
    fv->Constr_impl(NULL);
    idv->AddT3DataViewFrame(fv->widget());
    fv->Constr_post(); // have to do this manually once mapped
  }
  return fv;
}


void T3DataViewer::Reset_impl() {
  frames.Reset();
  inherited::Reset_impl();
}



