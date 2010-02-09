
#include "quarter_examiner_viewer.h"
#include "qtthumbwheel.h"

#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QKeyEvent>

#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/SoEventManager.h>

#include "pick.xpm"
#include "view.xpm"
#include "home.xpm"
#include "set_home.xpm"
#include "seek.xpm"
#include "view_all.xpm"
#include "print.xpm"
#include "snapshot.xpm"

// just for debugging
// #include <iostream>
// using namespace std;

// overall GUI size parameters

#define WHEEL_LENGTH 80		// long axis
#define WHEEL_WIDTH 20		// short axis
#define BUTTON_WIDTH 20
#define BUTTON_HEIGHT 20

QEVSavedCamera::QEVSavedCamera() {
  view_saved = false;
}

QEVSavedCamera::~QEVSavedCamera() {
  view_saved = false;
}

void QEVSavedCamera::getCameraParams(SoCamera* cam) {
  pos = cam->position.getValue();
  cam->orientation.getValue(rot_axis, rot_angle);
  focal_dist = cam->focalDistance.getValue();
  view_saved = true;
}

bool QEVSavedCamera::setCameraParams(SoCamera* cam) {
  if(!view_saved) return false;
  cam->position.setValue(pos);
  cam->orientation.setValue(rot_axis, rot_angle);
  cam->focalDistance.setValue(focal_dist);
  return true;
}

QuarterExaminerViewer::QuarterExaminerViewer(QWidget* parent)
  : QWidget(parent)
{
  // all the main layout code
  //  main_vbox: main_hbox: lhs_vbox quarter rhs_vbox
  //             bot_hbox

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

  quarter = new QuarterWidget(QGLFormat(QGL::SampleBuffers), this);
  main_hbox->addWidget(quarter);

  // set any initial configs for quarter widget here (or somewhere else if you please)
  quarter->setInteractionModeEnabled(true);
  quarter->setTransparencyType(QuarterWidget::NONE); // this is a good default
  quarter->setNavigationModeFile(QUrl("coin:///scxml/navigation/examiner.xml"));

  rhs_vbox = new QVBoxLayout;
  rhs_vbox->setMargin(0); rhs_vbox->setSpacing(0);
  main_hbox->addLayout(rhs_vbox);

  /////	make wheels all together

  hrot_wheel = new QtThumbWheel(0, 1000, 10, 500, Qt::Horizontal, this);
  hrot_start_val = 500;
  hrot_wheel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  hrot_wheel->setMaximumSize(WHEEL_LENGTH, WHEEL_WIDTH);
  hrot_wheel->setWrapsAround(true);
  hrot_wheel->setLimitedDrag(false);
  QObject::connect(hrot_wheel, SIGNAL(valueChanged(int)), this, SLOT(hrotwheelChanged(int)));

  vrot_wheel = new QtThumbWheel(0, 1000, 10, 500, Qt::Vertical, this);
  vrot_start_val = 500;
  vrot_wheel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  vrot_wheel->setMaximumSize(WHEEL_WIDTH, WHEEL_LENGTH);
  vrot_wheel->setWrapsAround(true);
  vrot_wheel->setLimitedDrag(false);
  QObject::connect(vrot_wheel, SIGNAL(valueChanged(int)), this, SLOT(vrotwheelChanged(int)));

  zoom_wheel = new QtThumbWheel(0, 1000, 10, 500, Qt::Vertical, this);
  zoom_start_val = 500;
  zoom_wheel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  zoom_wheel->setMaximumSize(WHEEL_WIDTH, WHEEL_LENGTH);
  zoom_wheel->setWrapsAround(true);
  zoom_wheel->setLimitedDrag(false);
  QObject::connect(zoom_wheel, SIGNAL(valueChanged(int)), this, SLOT(zoomwheelChanged(int)));

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

  /////  bot_hbox

  vrot_lbl = new QLabel("V.Rot ", this);
  bot_hbox->addWidget(vrot_lbl);

  hrot_lbl = new QLabel("H.Rot", this);
  bot_hbox->addWidget(hrot_lbl);

  bot_hbox->addWidget(hrot_wheel);

  bot_hbox->addStretch();

  bot_button_hbox = new QHBoxLayout;
  bot_button_hbox->setMargin(0); rhs_button_vbox->setSpacing(0);
  bot_hbox->addLayout(bot_button_hbox);

  bot_hbox->addStretch();

  zoom_lbl = new QLabel("Zoom   ", this);
  bot_hbox->addWidget(zoom_lbl);

  Constr_RHS_Buttons();
  Constr_LHS_Buttons();
  Constr_Bot_Buttons();
}

QuarterExaminerViewer::~QuarterExaminerViewer() {
  
}

void QuarterExaminerViewer::Constr_RHS_Buttons() {
  interact_button = new QToolButton(this);
  interact_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  interact_button->setCheckable(true);
  interact_button->setIcon(QPixmap((const char **)pick_xpm));
  interact_button->setToolTip("Interact (I key, or ESC to toggle): Allows you to select and manipulate objects in the display \n(ESC toggles between Interact and Camera View");
  QObject::connect(interact_button, SIGNAL(clicked()),
		   this, SLOT(interactbuttonClicked()));
  rhs_button_vbox->addWidget(interact_button);

  view_button = new QToolButton(this);
  view_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  view_button->setCheckable(true);
  view_button->setChecked(true);
  view_button->setIcon(QPixmap((const char **)view_xpm));
  view_button->setToolTip("Camera View (V key, or ESC to toggle): Allows you to move the view around (click and drag to move; \nshift = move in the plane; ESC toggles between Camera View and Interact)");
  QObject::connect(view_button, SIGNAL(clicked()),
		   this, SLOT(viewbuttonClicked()));
  rhs_button_vbox->addWidget(view_button);

  home_button = new QToolButton(this);
  home_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  home_button->setIcon(QPixmap((const char **)home_xpm));
  home_button->setToolTip("Home View (H or Home key): Restores display to the 'home' viewing configuration\n(set by next button down, saved with the project)");
  QObject::connect(home_button, SIGNAL(clicked()),
		   this, SLOT(homebuttonClicked()));
  rhs_button_vbox->addWidget(home_button);

  set_home_button = new QToolButton(this);
  set_home_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  set_home_button->setIcon(QPixmap((const char **)set_home_xpm));
  set_home_button->setToolTip("Save Home: Saves the current 'home' viewing configuration \n(click button above to go back to this view) -- saved with the project");
  QObject::connect(set_home_button, SIGNAL(clicked()),
		   this, SLOT(sethomebuttonClicked()));
  rhs_button_vbox->addWidget(set_home_button);

  view_all_button = new QToolButton(this);
  view_all_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  view_all_button->setIcon(QPixmap((const char **)view_all_xpm));
  view_all_button->setToolTip("(A key) View All: repositions the camera view to the standard initial view with everything in view");
  QObject::connect(view_all_button, SIGNAL(clicked()),
		   this, SLOT(viewallbuttonClicked()));
  rhs_button_vbox->addWidget(view_all_button);

  seek_button = new QToolButton(this);
  seek_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  seek_button->setIcon(QPixmap((const char **)seek_xpm));
  seek_button->setToolTip("Seek (S key): Click on objects (not text!) in the display and the camera will \nfocus in on the point where you click -- repeated clicks will zoom in further");
  QObject::connect(seek_button, SIGNAL(clicked()),
		   this, SLOT(seekbuttonClicked()));
  rhs_button_vbox->addWidget(seek_button);

  snapshot_button = new QToolButton(this);
  snapshot_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  snapshot_button->setIcon(QPixmap((const char **)snapshot_xpm));
  snapshot_button->setToolTip("Snapshot: save the current viewer image to quarter_image_snap.png file");
  QObject::connect(snapshot_button, SIGNAL(clicked()),
		   this, SLOT(snapshotbuttonClicked()));
  rhs_button_vbox->addWidget(snapshot_button);

  print_button = new QToolButton(this);
  print_button->setIconSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
  print_button->setIcon(QPixmap((const char **)print_xpm));
  print_button->setToolTip("Print: print the current viewer image to a printer -- uses the bitmap of screen image\n make window as large as possible for better quality");
  QObject::connect(print_button, SIGNAL(clicked()),
		   this, SLOT(printbuttonClicked()));
  rhs_button_vbox->addWidget(print_button);
}

void QuarterExaminerViewer::Constr_LHS_Buttons() {
  
}

void QuarterExaminerViewer::Constr_Bot_Buttons() {
  
}

///////////////////////////////////////////////////////////////
//		Main Button Actions

#define ROT_DELTA_MULT  0.01f
#define ZOOM_DELTA_MULT 0.01f

void QuarterExaminerViewer::hrotwheelChanged(int value) {
  // first detect wraparound
  if(hrot_start_val < 100 && value > 900) hrot_start_val += 1000;
  if(value < 100 && hrot_start_val > 900) hrot_start_val -= 1000;
  float delta = (float)(value - hrot_start_val);
  hrot_start_val = value;
  horizRotateView(ROT_DELTA_MULT * delta);
}

void QuarterExaminerViewer::vrotwheelChanged(int value) {
  if(vrot_start_val < 100 && value > 900) vrot_start_val += 1000;
  if(value < 100 && vrot_start_val > 900) vrot_start_val -= 1000;
  float delta = (float)(value - vrot_start_val);
  vrot_start_val = value;
  vertRotateView(ROT_DELTA_MULT * delta);
}

void QuarterExaminerViewer::zoomwheelChanged(int value) {
  if(zoom_start_val < 100 && value > 900) zoom_start_val += 1000;
  if(value < 100 && zoom_start_val > 900) zoom_start_val -= 1000;
  float delta = (float)(value - zoom_start_val);
  zoom_start_val = value;
  zoomView(-ZOOM_DELTA_MULT * delta); // direction is opposite
}

void QuarterExaminerViewer::interactbuttonClicked() {
  setInteractionModeOn(true);
}

void QuarterExaminerViewer::viewbuttonClicked() {
  setInteractionModeOn(false);
}

void QuarterExaminerViewer::homebuttonClicked() {
  goHome();
}

void QuarterExaminerViewer::sethomebuttonClicked() {
  saveHome();
}

void QuarterExaminerViewer::viewallbuttonClicked() {
  viewAll();
}

void QuarterExaminerViewer::seekbuttonClicked() {
  quarter->seek();
}

void QuarterExaminerViewer::snapshotbuttonClicked() {
  saveImage("quarter_viewer_snap.png");
}

void QuarterExaminerViewer::printbuttonClicked() {
  printImage();
}

void QuarterExaminerViewer::keyPressEvent(QKeyEvent* e) {
  if(e->key() == Qt::Key_Escape) {
    if(quarter->interactionModeOn()) {
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
    goHome();
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

SoCamera* QuarterExaminerViewer::getViewerCamera() {
  SoEventManager* mgr = quarter->getSoEventManager();
  if(!mgr) return NULL;
  return mgr->getCamera();
}

void QuarterExaminerViewer::viewAll() {
  quarter->viewAll();
  zoomView(-.35f);		// zoom in !!
}

// this is copied directly from SoQtFullViewer.cpp, which defines it as a static
// method on SoGuiFullViewerP

void QuarterExaminerViewer::zoomView(const float diffvalue) {
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
        SoDebugError::postWarning("SoGuiFullViewerP::zoom",
                                  "Unknown camera type, "
                                  "will zoom by moving position, but this might not be correct.");
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
    if (distorigo > 1.0e+19) {
#if SOQT_DEBUG && 0 // debug
      SoDebugError::postWarning("SoGuiFullViewerP::zoom",
                                "zoomed too far (distance to origo==%f (%e))",
                                distorigo, distorigo);
#endif // debug
    }
    else {
      cam->position = newpos;
      cam->focalDistance = newfocaldist;
    }
  }
}

void QuarterExaminerViewer::horizRotateView(const float rot_value) {
  RotateView(SbVec3f(0.0f, -1.0f, 0.0f), rot_value);
}

void QuarterExaminerViewer::vertRotateView(const float rot_value) {
  RotateView(SbVec3f(-1.0f, 0.0f, 0.0f), rot_value);
}

// copied from SoQtExaminerViewer.cpp -- hidden method on private P class

void QuarterExaminerViewer::RotateView(const SbVec3f& axis, const float ang) {
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
}

void QuarterExaminerViewer::setInteractionModeOn(bool onoff) {
  if(quarter->interactionModeOn() != onoff)
    quarter->setInteractionModeOn(onoff);
  if(quarter->interactionModeOn()) {
    interact_button->setChecked(true);
    view_button->setChecked(false);
  }
  else {
    interact_button->setChecked(false);
    view_button->setChecked(true);
  }
}

void QuarterExaminerViewer::saveHome() {
  SoCamera* cam = getViewerCamera();
  if(!cam) return;
  saved_home.getCameraParams(cam);
  emit homeSaved();
}

void QuarterExaminerViewer::goHome() {
  SoCamera* cam = getViewerCamera();
  if(!cam) return;
  saved_home.setCameraParams(cam);
}

QImage	QuarterExaminerViewer::grabImage() {
//   return QPixmap::grabWidget(this); // this only shows the frame, not contents!
  //  return QPixmap::grabWidget(quarter); 
  // oops! this is blank
  return quarter->grabFrameBuffer(true); // true = get alpha
  // also no version of this works either!
}

void QuarterExaminerViewer::saveImage(const QString& fname) {
  QImage img = grabImage();
  img.save(fname);
}

void QuarterExaminerViewer::printImage() {
  QImage img = grabImage();
  QPrinter pr;
  QPrintDialog pd(&pr, this);
  if(pd.exec() == QDialog::Accepted) {
    QPainter p(&pr);
    p.drawImage(0, 0, img);
  }
}
