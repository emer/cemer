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
//#include <qcursor.h>
#include <Q3DragObject>
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
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>
#include <Inventor/Qt/viewers/SoQtViewer.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

const float t3Misc::pts_per_geom(72.0f);
const float t3Misc::geoms_per_pt(1/pts_per_geom);
const float t3Misc::char_ht_to_wd_pts(1.8f); // with proportional spacing, this is more accurate on average..
const float t3Misc::char_base_fract(0.20f); //TODO: find correct val from coin src

using namespace Qt;

// from: http://doc.trolltech.com/4.3/opengl-samplebuffers-glwidget-cpp.html
#ifndef GL_MULTISAMPLE
# define GL_MULTISAMPLE  0x809D
#endif

//////////////////////////
//	T3ExaminerViewer
//////////////////////////

SOQT_OBJECT_SOURCE(T3ExaminerViewer);

T3ExaminerViewer::T3ExaminerViewer(iT3ViewspaceWidget *parent, const char *name, bool embed)
 :inherited(parent, name, embed, SoQtFullViewer::BUILD_ALL, SoQtViewer::BROWSER, FALSE) // no build
{
  t3vw = parent;
  interactbutton = NULL;
  viewbutton = NULL;
  setClassName("T3ExaminerViewer");
  QWidget * widget = buildWidget(getParentWidget()); // build now so that stuff is there
  setBaseWidget(widget);
}

void T3ExaminerViewer::processEvent(QEvent* ev_) {
  //NOTE: the base classes don't check if event is already handled, so we have to skip
  // calling inherited if we handle it ourselves

  if (!t3vw) goto do_inherited;

  if (ev_->type() == QEvent::MouseButtonPress) {
    QMouseEvent* ev = (QMouseEvent*)ev_;
    if (ev->button() == Qt::RightButton) {
      //TODO: maybe should check for item under mouse???
      //TODO: pos will need to be adjusted for parent offset of renderarea ???
      ISelectable* ci = t3vw->curItem();
      if (!ci) goto do_inherited;

      ev->accept();
      t3vw->ContextMenuRequested(ev->globalPos());
      return;
    }
  }

do_inherited:
  inherited::processEvent(ev_);
}

T3ExaminerViewer::~T3ExaminerViewer() {
  // anything?
  t3vw = NULL;
}

// pimpl is definitely a wart!  any attempt at re-use is really really awful!

// Button icons.
#include "pick.xpm"
#include "view.xpm"
#include "home.xpm"
#include "set_home.xpm"
#include "seek.xpm"
#include "view_all.xpm"
#include "print.xpm"
#include "snapshot.xpm"

// original buttons
enum {
  INTERACT_BUTTON = 0,
  EXAMINE_BUTTON,
  HOME_BUTTON,
  SET_HOME_BUTTON,
  VIEW_ALL_BUTTON,
  SEEK_BUTTON,
  SNAPSHOT_BUTTON,
  PRINT_BUTTON,
};

void
T3ExaminerViewer::createViewerButtons(QWidget * parent, SbPList * buttonlist)
{

  // note that the parent of this guy has a gridlayout that is causing icons
  // to overlap in mac mode, most likely due to a mac bug in 4.3.1, that is due to
  // be fixed in 4.4.0
  for (int i=0; i <= PRINT_BUTTON; i++) {
    QPushButton * p = new QPushButton(parent);
    // Button focus doesn't really make sense in the way we're using
    // the pushbuttons.
    p->setFocusPolicy(Qt::NoFocus);
    // In some GUI styles in Qt4, a default button is drawn with an
    // extra frame around it, up to 3 pixels or more. This causes
    // pixmaps on buttons to look tiny, which is not what we want.
    p->setIconSize(QSize(24, 24));

    switch (i) {
    case INTERACT_BUTTON:
      interactbutton = p;
      p->setCheckable(TRUE);
      p->setIcon(QPixmap((const char **)pick_xpm));
      p->setChecked(this->isViewing() ? FALSE : TRUE);
      p->setToolTip("Interact (ESC key): Allows you to select and manipulate objects in the display \n(ESC toggles between Interact and Camera View");
      connect(p, SIGNAL(clicked()),
	      this, SLOT(interactbuttonClicked()));
      break;
    case EXAMINE_BUTTON:
      viewbutton = p;
      p->setCheckable(TRUE);
      p->setIcon(QPixmap((const char **)view_xpm));
      p->setChecked(this->isViewing());
      p->setToolTip("Camera View (ESC key): Allows you to move the view around (click and drag to move; \nshift = move in the plane; ESC toggles between Camera View and Interact)");
      QObject::connect(p, SIGNAL(clicked()),
		       this, SLOT(viewbuttonClicked()));
      break;
    case HOME_BUTTON:
      QObject::connect(p, SIGNAL(clicked()), this, SLOT(homebuttonClicked()));
      p->setToolTip("Home View (Home key): Restores display to the 'home' viewing configuration\n(set by next button down, saved with the project)");
      p->setIcon(QPixmap((const char **)home_xpm));
      break;
    case SET_HOME_BUTTON:
      QObject::connect(p, SIGNAL(clicked()),
		       this, SLOT(sethomebuttonClicked()));
      p->setToolTip("Save Home: Saves the current 'home' viewing configuration \n(click button above to go back to this view) -- saved with the project");
      p->setIcon(QPixmap((const char **)set_home_xpm));
      break;
    case VIEW_ALL_BUTTON:
      QObject::connect(p, SIGNAL(clicked()),
		       this, SLOT(viewallbuttonClicked()));
      p->setToolTip("View All: repositions the camera view to the standard initial view with everything in view");
      p->setIcon(QPixmap((const char **)view_all_xpm));
      break;
    case SEEK_BUTTON:
      seekbutton = p;
      p->setCheckable(TRUE);
      p->setChecked(isSeekMode());
      QObject::connect(p, SIGNAL(clicked()), this, SLOT(seekbuttonClicked()));
      p->setToolTip("Seek: Click on objects (not text!) in the display and the camera will \nfocus in on the point where you click -- repeated clicks will zoom in further");
      p->setIcon(QPixmap((const char **)seek_xpm));
      break;
    case SNAPSHOT_BUTTON:
      QObject::connect(p, SIGNAL(clicked()),
		       this, SLOT(snapshotbuttonClicked()));
      p->setToolTip("Snapshot: save the current image to a file\nEPS format gives best resolution but transparency etc not captured\n -- use EPS primarily for graphs and grids\n PNG is best for lossless compression \n JPEG is best for lossy compression (see jpeg_qualty in preferences)");
      p->setIcon(QPixmap((const char **)snapshot_xpm));
      break;
    case PRINT_BUTTON:
      QObject::connect(p, SIGNAL(clicked()),
		       this, SLOT(printbuttonClicked()));
      p->setToolTip("Print: print the current image to a printer -- uses the bitmap of screen image\n make window as large as possible for better quality");
      p->setIcon(QPixmap((const char **)print_xpm));
      break;
    default:
      assert(0);
      break;
    }

    p->adjustSize();
    buttonlist->append(p);
  }
}

// this is copied directly from SoQtFullViewer.cpp, which defines it as a static
// method on SoGuiFullViewerP -- again, pimpl = no code reuse!  crazy.  what are 
// you trying to hide anyway!!

void
T3ExaminerViewer::zoom(SoCamera* cam, const float diffvalue) {
  if (cam == NULL) return; // can happen for empty scenegraph
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

// make another button that does the angle = .35 thing??

void T3ExaminerViewer::viewAll() {
  SoCamera* cam = getCamera();
  SoNode* sg = getSceneGraph();
  if (cam && sg) {
    SbVec3f axis;
    axis[0]=-1.0; axis[1]=0.0f; axis[2]=0.0f;
//     float angle = .35;
    float angle = 0.0f;
    cam->orientation.setValue(axis, angle);
    cam->viewAll(sg, getViewportRegion());
    zoom(cam, -.35f);		// zoom in !!
  }
}

// Qt slot.
void
T3ExaminerViewer::interactbuttonClicked(void)
{
  if(isSeekMode()) setSeekMode_doit(FALSE);	// get out of seek mode
  if (interactbutton)
    interactbutton->setChecked(TRUE);
  if (viewbutton)
    viewbutton->setChecked(FALSE);
  if (isViewing())
    setViewing(FALSE); // other guys assume buttons!
  T3DataViewFrame* dvf = GetFrame();
  if(!dvf) return;
  // rebuilds to update manipulators
  dvf->Render();
}

// *************************************************************************

// Qt slot.
void
T3ExaminerViewer::viewbuttonClicked(void)
{
  if(isSeekMode()) setSeekMode_doit(FALSE);	// get out of seek mode
  if (interactbutton)
    interactbutton->setChecked(FALSE);
  if (viewbutton)
    viewbutton->setChecked(TRUE);
  if (!isViewing())
    setViewing(TRUE); // other guys assume buttons!
  T3DataViewFrame* dvf = GetFrame();
  if(!dvf) return;
  // rebuilds to update manipulators
  dvf->Render();
}

void
T3ExaminerViewer::viewallbuttonClicked()
{
  if(isSeekMode()) setSeekMode_doit(FALSE);	// get out of seek mode
  viewAll();
}

void
T3ExaminerViewer::homebuttonClicked()
{
  if(isSeekMode()) setSeekMode_doit(FALSE);	// get out of seek mode
  resetToHomePosition();
}

void
T3ExaminerViewer::sethomebuttonClicked()
{
  saveHomePosition();
}

void
T3ExaminerViewer::seekbuttonClicked()
{
  setSeekMode_doit(isSeekMode() ? FALSE : TRUE);
}

void
T3ExaminerViewer::setSeekMode(SbBool enable)
{
  // do nothing -- the thing is always trying to turn off seek mode too much!
}

void
T3ExaminerViewer::setSeekMode_doit(SbBool enable)
{
  if (seekbutton)
    seekbutton->setChecked(enable);
  inherited::setSeekMode(enable); // actually do it!
}

void
T3ExaminerViewer::snapshotbuttonClicked()
{
  T3DataViewFrame* dvf = GetFrame();
  if(!dvf) return;
  dvf->CallFun("SaveImageAs");
}

void
T3ExaminerViewer::printbuttonClicked()
{
  T3DataViewFrame* dvf = GetFrame();
  if(!dvf) return;
  dvf->PrintImage();
}


T3DataViewFrame* T3ExaminerViewer::GetFrame() {
  if(!t3vw) return NULL;
  iT3DataViewFrame* idvf = t3vw->i_data_frame();
  if(!idvf) return NULL;
  T3DataViewFrame* dvf = idvf->viewer();
  return dvf;
}

void
T3ExaminerViewer::saveHomePosition() {
  T3DataViewFrame* dvf = GetFrame();
  if(!dvf) return;
  dvf->GetCameraPosOrient();
}

void
T3ExaminerViewer::resetToHomePosition(void)
{
  T3DataViewFrame* dvf = GetFrame();
  if(!dvf) return;
  dvf->SetCameraPosOrient();
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

void T3DataView::FillContextMenu_impl(taiActions* menu) {
  TypeDef* typ = GetTypeDef();
  // put view props first, if any
  if (hasViewProperties()) {
    for(int i=0;i<typ->methods.size;i++) {
      MethodDef* md = typ->methods[i];
      if(md->im && md->HasOption("VIEWMENU")) {
	taiMethodData* mth_rep = md->im->GetMethodRep(this, NULL, NULL, NULL);
	if (mth_rep) {;
	  mth_rep->AddToMenu(menu);
	}
      }
    }
  }
  menu->AddSep();
  IObjectSelectable::FillContextMenu_impl(menu);
} 

T3DataViewFrame* T3DataView::GetFrame() {
  T3DataViewFrame* frame = GET_MY_OWNER(T3DataViewFrame);
  return frame;
}

SoQtViewer* T3DataView::GetViewer() {
  T3DataViewFrame* frame = GetFrame();
  if(!frame || !frame->widget()) return NULL;
  SoQtViewer* viewer = frame->widget()->ra();
  return viewer;
}

void T3DataView::QueryEditActionsS_impl_(int& allowed, int& forbidden) const {
  if (flags & DNF_IS_MEMBER) {
    forbidden |= (taiClipData::EA_CUT | taiClipData::EA_DELETE);
  }
  IObjectSelectable::QueryEditActionsS_impl_(allowed, forbidden);
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

void T3DataViewRoot::Constr_Node_impl() {
  setNode(new T3NodeParent);
}

void T3DataViewRoot::ChildRemoving(taDataView* child) {
  inherited::ChildRemoving(child);
/* Special Rules:
  1. in a non-default frame, delete if deleting last item
*/
  if (!isDestroying() && (children.size == 0)) {
    T3DataViewFrame* dvf = GET_MY_OWNER(T3DataViewFrame);
    if (dvf && (dvf->GetIndex() > 0)) {
      dvf->CloseLater();
    }
  }
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
  t3vw->SoSelectionEvent(&ev);
  t3vw->sel_so->touch(); // to redraw

}

void iT3ViewspaceWidget::SoDeselectionCallback(void* inst, SoPath* path) {
  iSoSelectionEvent ev(false, path);
  iT3ViewspaceWidget* t3dv = (iT3ViewspaceWidget*)inst;
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
  setRenderArea(NULL);
  m_horScrollBar = NULL;
  m_verScrollBar = NULL;
  m_i_data_frame = NULL;
}

void iT3ViewspaceWidget::init() {
  m_renderArea = NULL;
  m_horScrollBar = NULL;
  m_verScrollBar = NULL;
  m_root_so = new SoSeparator(); // refs
//nn  m_raw = NULL;
  m_selMode = SM_NONE;
  m_scene = NULL;
  m_last_vis = 0;
//TEST
  setMinimumSize(320, 320);
}

void iT3ViewspaceWidget::deleteScene() {
  if (m_renderArea) {
    // remove the nodes
    m_renderArea->setSceneGraph(NULL);
  }
}

QScrollBar* iT3ViewspaceWidget::horScrollBar(bool auto_create) {
  if (auto_create && !(m_horScrollBar))
    setHasHorScrollBar(true);
  return m_horScrollBar;
}

void iT3ViewspaceWidget::LayoutComponents() {
  QSize sz = size(); // already valid
  int ra_wd = (m_verScrollBar) ? sz.width() - m_verScrollBar->width() : sz.width();
  int ra_ht = (m_horScrollBar) ? sz.height() - m_horScrollBar->height() : sz.height();
  if (m_renderArea) {
    //NOTE: presumably has 0,0 origin, and could change by changing baseWidget()
    m_renderArea->setSize(SbVec2s(ra_wd, ra_ht));
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
  LayoutComponents();
}

void iT3ViewspaceWidget::setRenderArea(SoQtRenderArea* value) {
  if (m_renderArea == value) return;
  if (value && (value->getParentWidget() != this))
      taMisc::Error("iT3ViewspaceWidget::setRenderArea",
      "The RenderArea must be owned by ViewspaceWidget being assigned.");
  if (m_renderArea) {
    delete m_renderArea;
  }
  m_renderArea = value;

  if (m_renderArea) {

    // NOTE: the following is old-style and is VERY slow
//     m_renderArea->setAccumulationBuffer(true);
//     bool accum_buf = m_renderArea->getAccumulationBuffer();
//     if(accum_buf)
//       m_renderArea->setAntialiasing(true, taMisc::antialiasing_passes);
//     else {
//       m_renderArea->setAntialiasing(true, 1);
//       taMisc::Warning("Note: was not able to establish an accumulation buffer so rendering will not be anti-aliased.  Sorry.");
//     }

    // this is the new Multisampling method -- much better!

    QGLWidget* qglw = (QGLWidget*)m_renderArea->getGLWidget();
    QGLFormat fmt = qglw->format();
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

    if (m_selMode == SM_NONE)
      m_renderArea->setSceneGraph(m_root_so);
    else {
      sel_so = new SoSelection();
      switch (m_selMode) {
      case SM_SINGLE: sel_so->policy = SoSelection::SINGLE; break;
      case SM_MULTI: sel_so->policy = SoSelection::SHIFT; break;
      default: break; // compiler food
      }
      sel_so->addChild(m_root_so);
      sel_so->addSelectionCallback(SoSelectionCallback, (void*)this);
      sel_so->addDeselectionCallback(SoDeselectionCallback, (void*)this);
      m_renderArea->setSceneGraph(sel_so);
      SoBoxHighlightRenderAction* rend_act = new SoBoxHighlightRenderAction;
//       rend_act->setTransparencyType(SoGLRenderAction::DELAYED_BLEND);
      rend_act->setTransparencyType(SoGLRenderAction::BLEND);
      rend_act->setSmoothing(true); // low-cost line smoothing

      // old-style accummulation buffer antialiasing:
      //#ifndef TA_OS_MAC		    // temp until bug is fixed!
      // bug is now fixed with patch to soqt 1.4.1
//       if(accum_buf)
// 	rend_act->setNumPasses(taMisc::antialiasing_passes);    // 1 = no antialiasing; 2 = antialiasing
      //#endif
      m_renderArea->setGLRenderAction(rend_act);
    }
    LayoutComponents();
  }
}

void iT3ViewspaceWidget::setHasHorScrollBar(bool value) {
  if ((m_horScrollBar != NULL) == value) return;
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
  if ((m_verScrollBar != NULL) == value) return;
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
  if (!m_renderArea) return; //not supposed to happen
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

void iT3ViewspaceWidget::EditAction_Delete() {
  ISelectableHost::EditAction_Delete();
}

void iT3ViewspaceWidget::SoSelectionEvent(iSoSelectionEvent* ev) {
  T3DataView* t3node = T3DataView::GetViewFromPath(ev->path);
  if (!t3node) return;

  if (ev->is_selected) {
//     if (t3node->selectEditMe()) {
//       if (taMisc::click_style == taMisc::CS_SINGLE) {
//         t3node->ViewProperties();
//         // don't also select or grab focus in this mode
//         return;
//       }
//     }
    AddSelectedItem(t3node);
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
  m_ra->show();
}  

void iT3DataViewFrame::Init() {
  QVBoxLayout* lay = new QVBoxLayout(this);
  lay->setSpacing(0);  lay->setMargin(0);
  //create the so viewer
  t3vs = new iT3ViewspaceWidget(this);
  lay->addWidget(t3vs);
/*obs  int mw = (taiM->scrn_s.width() * 3) / 20; // 15% min for 3d viewer
  t3vs->resize(mw, t3vs->height());
  t3vs->setMinimumWidth(mw);*/
  t3vs->setSelMode(iT3ViewspaceWidget::SM_MULTI); // default

  m_ra = new T3ExaminerViewer(t3vs);
  t3vs->setRenderArea(m_ra);
}

/*void iT3DataViewFrame::Constr_Menu_impl() {
  inherited::Constr_Menu_impl();
  taiAction* act = AddAction(new taiAction("Inventor", QKeySequence(), "fileExportInventor" ));
  act->AddTo(fileExportMenu);
  act->connect(taiAction::action, this, SLOT( fileExportInventor() ) );
} */


void iT3DataViewFrame::fileExportInventor() {
  static QFileDialog* fd = NULL;
  SoNode* scene = m_ra->getSceneGraph();
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

void iT3DataViewFrame::hideEvent(QHideEvent* ev) {
  inherited::hideEvent(ev);
  Showing(false);
}

void iT3DataViewFrame::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  Refresh();
  Showing(true);
}

void iT3DataViewFrame::Showing(bool showing) {
  if (panel_set) {
    panel_set->FrameShowing(showing, showing); // focus it if showing
  }
}

void iT3DataViewFrame::Render_pre() {
  //nothing
}

void iT3DataViewFrame::Render_impl() {
  //nothing
}

void iT3DataViewFrame::Render_post() {
//  m_ra->show();
}

void iT3DataViewFrame::Reset_impl() {
  setSceneTop(NULL);
}

void iT3DataViewFrame::Refresh_impl() {
/*obs  T3DataViewRoot* rt = root();
  if (rt) 
    rt->Refresh(); */
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
  camera_focdist = 0.0f;
  bg_color.no_a = true; 
  bg_color.r = 0.8f;
  bg_color.g = 0.8f;
  bg_color.b = 0.8f;
}

void T3DataViewFrame::Destroy() {
  Reset();
  CutLinks();
}

void T3DataViewFrame::InitLinks() {
  inherited::InitLinks();
  taBase::Own(root_view, this);
  taBase::Own(camera_pos, this);
  taBase::Own(camera_orient, this);
  taBase::Own(bg_color, this);
}

void T3DataViewFrame::CutLinks() {
  bg_color.CutLinks();
  root_view.CutLinks();
  inherited::CutLinks();
}

void T3DataViewFrame::Copy_(const T3DataViewFrame& cp) {
  root_view = cp.root_view;
  bg_color = cp.bg_color;
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
  SetCameraPosOrient();
}

IDataViewWidget* T3DataViewFrame::ConstrWidget_impl(QWidget* gui_parent) {
  iT3DataViewFrame* rval = new iT3DataViewFrame(this, gui_parent);
  // make the corresponding viewpanelset
  MainWindowViewer* mwv = GET_MY_OWNER(MainWindowViewer);
  PanelViewer* pv = (PanelViewer*)mwv->FindFrameByType(&TA_PanelViewer);
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

T3DataView* T3DataViewFrame::FindRootViewOfData(TAPtr data) {
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
      if (ok) return rval;
    }
  }
  rval = bg_color;
  return rval;
}

void T3DataViewFrame::Render_pre() {
  inherited::Render_pre();
  widget()->Render_pre();
  root_view.Render_pre();
}

void T3DataViewFrame::Render_impl() {
  inherited::Render_impl();
  root_view.Render_impl();
  SoQtViewer* viewer = widget()->ra();
  iColor bg = GetBgColor();
  viewer->setBackgroundColor(SbColor(bg.redf(), bg.greenf(), bg.bluef()));
  widget()->Render_impl();
}

void T3DataViewFrame::Render_post() {
  inherited::Render_post();
  root_view.Render_post();
  widget()->setSceneTop(root_view.node_so());
  widget()->Render_post();
  // on first opening, do a viewall to center all geometry in viewer
  if(camera_pos == 0.0f && camera_focdist == 0.0f) {
    ViewAll();
    GetCameraPosOrient();
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
  SoQtViewer* viewer = widget()->ra();
  viewer->viewAll();
}

void T3DataViewFrame::GetCameraPosOrient() {
  if(!widget()) return;
  SoQtViewer* viewer = widget()->ra();
  SoCamera* cam = viewer->getCamera();
  SbVec3f pos = cam->position.getValue();
  camera_pos.x = pos[0]; camera_pos.y = pos[1]; camera_pos.z = pos[2];
  SbVec3f axis;
  float angle;
  cam->orientation.getValue(axis, angle);
  camera_orient.x = axis[0]; camera_orient.y = axis[1]; camera_orient.z = axis[2];
  camera_orient.rot = angle;
  camera_focdist = cam->focalDistance.getValue();
}

void T3DataViewFrame::SetCameraPosOrient() {
  if(!widget()) return;
  SoQtViewer* viewer = widget()->ra();
  if(camera_pos == 0.0f && camera_focdist == 0.0f) {
    viewer->viewAll();
    return;
  }
  SoCamera* cam = viewer->getCamera();
  cam->position.setValue(camera_pos.x, camera_pos.y, camera_pos.z);
  SbVec3f axis;
  axis[0]=camera_orient.x; axis[1]=camera_orient.y; axis[2]=camera_orient.z;
  float angle = camera_orient.rot;
  cam->orientation.setValue(axis, angle);
  cam->focalDistance.setValue(camera_focdist);
}

void T3DataViewFrame::Dump_Save_pre() {
  inherited::Dump_Save_pre();
  // note: not doing this now: it is up to the viewer to do this now
//   GetCameraPosOrient();
}

QPixmap T3DataViewFrame::GrabImage(bool& got_image) {
  got_image = false;
  if(!widget()) {
    return QPixmap();
  }
  // this doesn't get the gl parts:
  //  return QPixmap::grabWidget(widget());
  // this doesn't work either: no gl
//   QPoint widgpos = widget()->mapToGlobal(QPoint(0,0));
//   QWidget* winwidg = widget()->window();
//   QPoint winpos = winwidg->mapToGlobal(QPoint(0,0));
//   QPoint off = widgpos - winpos;
//   return QPixmap::grabWindow(winwidg->winId(), (int)fabsf(off.x()), (int)fabsf(off.y()),
// 			     (int)widget()->width(), (int)widget()->height());
  SoQtViewer* viewer = widget()->ra();
  if(TestError(!viewer, "GrabImage", "viewer is NULL!")) return QPixmap();
  QGLWidget* qglw = (QGLWidget*)viewer->getGLWidget();
  // renderPixmap did not work -- returned a black screen -- something about InitGL..
  QImage img = qglw->grabFrameBuffer(false); // todo: try true?
  if(TestError(img.isNull(), "GrabImage", "got a null image from grabFrameBuffer call!"))
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
  SoQtViewer* viewer = widget()->ra();
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

  if (!out->openFile(flr->fileName())) {
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
  ps->apply(viewer->getSceneManager()->getSceneGraph());

  // this will create the postscript file
  ps->endPage();

  // close file
  out->closeFile();

  delete ps;
  taRefN::unRefDone(flr);
  return true;
}

bool T3DataViewFrame::SaveImageIV(const String& fname) {
  SoQtViewer* viewer = widget()->ra();
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
  if(!out.openFile(flr->fileName())) return false;
  SoWriteAction wa(&out);

  wa.apply(root_view.node_so()); // just the data, not the whole camera
  //  wa.apply(viewer->getSceneManager()->getSceneGraph());
  out.closeFile();

  taRefN::unRefDone(flr);
  return true;
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
  setTabBar(new iTabBarEx(this));
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
  if (tw->count() > 0)
    tw->setCurrentIndex(0);

}

void iT3DataViewer::tw_customContextMenuRequested2(const QPoint& pos, int tab_idx) {
  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);
  FillContextMenu_impl(menu, tab_idx);
  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
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

//////////////////////////
//	T3DataViewer	//
//////////////////////////

T3DataViewFrame* T3DataViewer::GetBlankOrNewT3DataViewFrame(taBase* obj) {
  if (!obj) return NULL;
  T3DataViewFrame* fr = NULL;
  taProject* proj = (taProject*)obj->GetOwner(&TA_taProject);
  MainWindowViewer* vw = MainWindowViewer::GetDefaultProjectViewer(proj);
  if (!vw) return NULL; // shouldn't happen
  T3DataViewer* t3vw = (T3DataViewer*)vw->FindFrameByType(&TA_T3DataViewer);
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
}

void T3DataViewer::DataChanged_Child(TAPtr child, int dcr, void* op1, void* op2) {
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


T3DataView* T3DataViewer::FindRootViewOfData(TAPtr data) {
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



