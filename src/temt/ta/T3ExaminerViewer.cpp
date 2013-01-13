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

#include <QtThumbWheel>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>

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
  quarter = NULL;               // for startup events

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

  quarter = new T3QuarterWidget(this);
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

  rhs_vbox = new QVBoxLayout;
  rhs_vbox->setMargin(0); rhs_vbox->setSpacing(0);
  main_hbox->addLayout(rhs_vbox);

  ///// make wheels all together

  hrot_wheel = new QtThumbWheel(0, THUMB_MAX_VAL, THUMB_PAGE_STEP, THUMB_INIT_VAL, Qt::Horizontal, this);
  t3ev_config_wheel(hrot_wheel);
  hrot_start_val = THUMB_INIT_VAL;
  hrot_wheel->setMaximumSize(WHEEL_LENGTH, WHEEL_WIDTH);
  connect(hrot_wheel, SIGNAL(valueChanged(int)), this, SLOT(hrotwheelChanged(int)));

  vrot_wheel = new QtThumbWheel(0, THUMB_MAX_VAL, THUMB_PAGE_STEP, THUMB_INIT_VAL, Qt::Vertical, this);
  t3ev_config_wheel(vrot_wheel);
  vrot_start_val = THUMB_INIT_VAL;
  vrot_wheel->setMaximumSize(WHEEL_WIDTH, WHEEL_LENGTH);
  connect(vrot_wheel, SIGNAL(valueChanged(int)), this, SLOT(vrotwheelChanged(int)));

  zoom_wheel = new QtThumbWheel(0, THUMB_MAX_VAL, THUMB_PAGE_STEP, THUMB_INIT_VAL, Qt::Vertical, this);
  t3ev_config_wheel(zoom_wheel);
  zoom_start_val = THUMB_INIT_VAL;
  zoom_wheel->setMaximumSize(WHEEL_WIDTH, WHEEL_LENGTH);
  connect(zoom_wheel, SIGNAL(valueChanged(int)), this, SLOT(zoomwheelChanged(int)));

  hpan_wheel = new QtThumbWheel(0, THUMB_MAX_VAL, THUMB_PAGE_STEP, THUMB_INIT_VAL, Qt::Horizontal, this);
  t3ev_config_wheel(hpan_wheel);
  hpan_start_val = THUMB_INIT_VAL;
  hpan_wheel->setMaximumSize(WHEEL_LENGTH, WHEEL_WIDTH);
  connect(hpan_wheel, SIGNAL(valueChanged(int)), this, SLOT(hpanwheelChanged(int)));

  vpan_wheel = new QtThumbWheel(0, THUMB_MAX_VAL, THUMB_PAGE_STEP, THUMB_INIT_VAL, Qt::Vertical, this);
  t3ev_config_wheel(vpan_wheel);
  vpan_start_val = THUMB_INIT_VAL;
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

//   quarter->setInteractionModeOn(false);
  setInteractionModeOn(false);  // default start it off!
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
  interact_button->setChecked(false);
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
  cur_view_no = -1;             // nothing set yet
  saved_views.SetSize(n_views);
  for(int i=0; i<n_views; i++) {
    T3SavedView* sv = saved_views[i];
    String nm = sv->name;
    if(nm.contains("T3SavedView")) {            // uninitialized
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
//              Main Button Actions

#define ROT_DELTA_MULT  0.002f
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
  if(sv->EditDialog(true))              // modal!
    nameView(view_no, sv->name);
}

void T3ExaminerViewer::savenameviewTriggered(int view_no) {
  saveView(view_no);
  T3SavedView* sv = saved_views.SafeEl(view_no);
  if(!sv) return;
  if(sv->EditDialog(true))              // modal!
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
    gotoView(0);                        // 0 is base guy
    e->accept();
    return;
  }
  if(e->key() == Qt::Key_A) {
    viewAll();
    e->accept();
    return;
  }
  if(e->key() == Qt::Key_S) {   // seek
    quarter->seek();
    e->accept();
    return;
  }
  emit unTrappedKeyPressEvent(e);
  QWidget::keyPressEvent(e);
}



///////////////////////////////////////////////////////////////
//              Actual functions

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
  zoomView(-.35f);              // zoom in !!
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
  syncViewerMode();             // keep it sync'd -- this tends to throw it off
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


void T3ExaminerViewer::syncViewerMode() {
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
}

bool T3ExaminerViewer::syncCursor() {
  if(!quarter) return false;
  if(viewer_mode == INTERACT) {
    setCursor(quarter->stateCursor("interact"));
  }
  else {
    setCursor(quarter->cursor());
  }
  return true;
}

void T3ExaminerViewer::setInteractionModeOn(bool onoff, bool re_render) {
  viewer_mode = (ViewerMode)onoff; // enum matches
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
}

bool T3ExaminerViewer::eventFilter(QObject* obj, QEvent* ev_) {
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
}
