
#include "qt3d_examiner_viewer.h"
#include "qtthumbwheel.h"
#include "window.h"

#include <Qt3DCore/QEntity>
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

#include <QWindow>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QKeyEvent>
#include <QQuaternion>
#include <cmath>

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

void QEVSavedCamera::getCameraParams(Qt3D::QCamera *cam) {
  eye = cam->position();
  up_vector = cam->upVector();
  center = cam->viewCenter();
  view_saved = true;
}

bool QEVSavedCamera::setCameraParams(Qt3D::QCamera *cam) {
  if (!view_saved) return false;
  cam->setViewCenter(center);
  cam->setPosition(eye);
  cam->setUpVector(up_vector);
  return true;
}

Qt3DExaminerViewer::Qt3DExaminerViewer(QWidget* parent)
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

  m_qview = new Window();       // this must be a special OpenGL window!!!
  QWidget *container = QWidget::createWindowContainer(m_qview);
  main_hbox->addWidget(container);

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

Qt3DExaminerViewer::~Qt3DExaminerViewer() {
  if (m_qview)
  {
    delete m_qview;
  }
}

void Qt3DExaminerViewer::Constr_RHS_Buttons() {
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

void Qt3DExaminerViewer::Constr_LHS_Buttons() {

}

void Qt3DExaminerViewer::Constr_Bot_Buttons() {

}

///////////////////////////////////////////////////////////////
//		Main Button Actions

#define ROT_DELTA_MULT  0.01f
#define ZOOM_DELTA_MULT 0.01f

void Qt3DExaminerViewer::hrotwheelChanged(int value) {
  // first detect wraparound
  if (hrot_start_val < 100 && value > 900) hrot_start_val += 1000;
  if (value < 100 && hrot_start_val > 900) hrot_start_val -= 1000;
  float delta = (float)(value - hrot_start_val);
  hrot_start_val = value;
  horizRotateView(ROT_DELTA_MULT * delta);
}

void Qt3DExaminerViewer::vrotwheelChanged(int value) {
  if (vrot_start_val < 100 && value > 900) vrot_start_val += 1000;
  if (value < 100 && vrot_start_val > 900) vrot_start_val -= 1000;
  float delta = (float)(value - vrot_start_val);
  vrot_start_val = value;
  vertRotateView(ROT_DELTA_MULT * delta);
}

void Qt3DExaminerViewer::zoomwheelChanged(int value) {
  if (zoom_start_val < 100 && value > 900) zoom_start_val += 1000;
  if (value < 100 && zoom_start_val > 900) zoom_start_val -= 1000;
  float delta = (float)(value - zoom_start_val);
  zoom_start_val = value;
  zoomView(-ZOOM_DELTA_MULT * delta); // direction is opposite
}

void Qt3DExaminerViewer::interactbuttonClicked() {
  setInteractionModeOn(true);
}

void Qt3DExaminerViewer::viewbuttonClicked() {
  setInteractionModeOn(false);
}

void Qt3DExaminerViewer::homebuttonClicked() {
  goHome();
}

void Qt3DExaminerViewer::sethomebuttonClicked() {
  saveHome();
}

void Qt3DExaminerViewer::viewallbuttonClicked() {
  viewAll();
}

void Qt3DExaminerViewer::seekbuttonClicked() {
  //  quarter->seek();
}

void Qt3DExaminerViewer::snapshotbuttonClicked() {
  //saveImage("quarter_viewer_snap.png");
}

void Qt3DExaminerViewer::printbuttonClicked() {
  //printImage();
}

void Qt3DExaminerViewer::keyPressEvent(QKeyEvent* e) {
  if (e->key() == Qt::Key_Escape) {
#pragma message ("Write interaction mode overrides. Need to understand the QWindow event manager a bit better.")
#if 0
    if(quarter->interactionModeOn()) {
      setInteractionModeOn(false);
    }
    else {
      setInteractionModeOn(true);
    }
    e->accept();
#endif // #if 0
    return;
  }
  if (e->key() == Qt::Key_I) {
    setInteractionModeOn(true);
    e->accept();
    return;
  }
  if (e->key() == Qt::Key_V) {
    setInteractionModeOn(false);
    e->accept();
    return;
  }
  if ((e->key() == Qt::Key_Home) || (e->key() == Qt::Key_H)) {
    goHome();
    e->accept();
    return;
  }
  if (e->key() == Qt::Key_A) {
    viewAll();
    e->accept();
    return;
  }
  QWidget::keyPressEvent(e);
}



///////////////////////////////////////////////////////////////
//		Actual functions

Qt3D::QCamera* Qt3DExaminerViewer::getViewerCamera() {
  return m_camera;
}

void Qt3DExaminerViewer::viewAll() {
#pragma message ("Write viewAll() code.")
#if 0
  quarter->viewAll();
#endif // #if 0
  zoomView(-.35f);		// zoom in !!
}

void Qt3DExaminerViewer::zoomView(const float diffvalue) {
  if (!m_camera) return; // can happen for empty scenegraph

  // This will be in the range of <0, ->>.
  float multiplicator = float(exp(diffvalue));

  if (m_camera->projectionType() == Qt3D::QCameraLens::OrthogonalProjection)
  {
    // Since there's no perspective, "zooming" in the original sense
    // of the word won't have any visible effect. So we just increase
    // or decrease the field-of-view values of the camera instead, to
    // "shrink" the projection size of the model / scene.
    float view_size = m_camera->aspectRatio();
    m_camera->setAspectRatio(view_size * multiplicator);
  }
  else // if (m_camera->projectionType() == Qt3D::QCameraLens::PerspectiveProjection)
  {
    float zoom = m_camera->fieldOfView();
    m_camera->setFieldOfView(zoom * multiplicator);
  }
}

void Qt3DExaminerViewer::horizRotateView(const float rot_value) {
  RotateView(QVector3D(0.0f, -1.0f, 0.0f), rot_value);
}

void Qt3DExaminerViewer::vertRotateView(const float rot_value) {
  RotateView(QVector3D(-1.0f, 0.0f, 0.0f), rot_value);
}

// copied from SoQtExaminerViewer.cpp -- hidden method on private P class

void Qt3DExaminerViewer::RotateView(const QVector3D& axis, const float ang) {
  if (!m_camera) return;

  QQuaternion rotation(ang, axis);
  m_camera->rotate(rotation);
}

void Qt3DExaminerViewer::setInteractionModeOn(bool onoff) {
#pragma message "Interaction mode is not coded yet."
}

void Qt3DExaminerViewer::saveHome() {
  if (!m_camera) return;
  saved_home.getCameraParams(m_camera);
  emit homeSaved();
}

void Qt3DExaminerViewer::goHome() {
  if (!m_camera) return;
  saved_home.setCameraParams(m_camera);
}

#if 0

QImage	Qt3DExaminerViewer::grabImage() {
  return quarter->grabFrameBuffer(true); // true = get alpha
}

void Qt3DExaminerViewer::saveImage(const QString& fname) {
  QImage img = grabImage();
  img.save(fname);
}

void Qt3DExaminerViewer::printImage() {
  QImage img = grabImage();
  QPrinter pr;
  QPrintDialog pd(&pr, this);
  if(pd.exec() == QDialog::Accepted) {
    QPainter p(&pr);
    p.drawImage(0, 0, img);
  }
}

#endif // #if 0
