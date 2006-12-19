// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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
#include "ta_geometry.h"
#include "ta_qtclipdata.h"

#include "css_machine.h" // for trace flag
//#include "irenderarea.h"

#include <qapplication.h>
#include <qclipboard.h>
//#include <qcursor.h>
#include <Q3DragObject>
#include <qevent.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qmime.h>
#include <QMenu>
#include <qscrollbar.h>
#include <QTabWidget>

#include <Inventor/SoPath.h>
#include <Inventor/SoOutput.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/events/SoButtonEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/misc/SoBase.h>
//#include <Inventor/nodes/SoDirectionalLight.h>
//#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>
#include <Inventor/Qt/viewers/SoQtViewer.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>


const float t3Misc::pts_per_geom(72.0f);
const float t3Misc::geoms_per_pt(1/pts_per_geom);
const float t3Misc::char_ht_to_wd_pts(12.0f/8.0f);
const float t3Misc::char_base_fract(0.20f); //TODO: find correct val from coin src

//////////////////////////
//	T3DataView	//
//////////////////////////
#ifdef TA_PROFILE
int T3DataView_inst_cnt = 0;
#endif

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
      rval = (T3DataView*)t3node->dataView;
      break;
    }
    path->truncate(path->getLength() - 1);
  }
  path->unref();
  return rval;
}


void T3DataView::Initialize() {
  flags = 0; //TODO: provide way to init
  m_md = NULL; //TODO: set later
//  m_viewer = NULL;
  last_child_node = NULL;
  m_transform = NULL;
//TODO  setDragEnabled(flags & DNF_CAN_DRAG);
//TODO  setDropEnabled(!(flags & DNF_NO_CAN_DROP));
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

void T3DataView::CutLinks() {
  Reset();
  if (m_transform) {
    m_transform->CutLinks();
    delete m_transform;
    m_transform = NULL;
  }
  inherited::CutLinks();
}

void T3DataView::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  if (m_node_so.ptr())
    Render_impl();
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
  node_so()->dataView = NULL;

  // we remove top-most item first, which results in only one update to the scene graph
  if (hasParent()) {
    parent()->ChildClearing(this); // parent clears us
  }
}

void T3DataView::setNode(T3Node* node_) {
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

/*void T3DataView::DataChanged(int dcr, void* op1_, void* op2_) { //TODO
}  */

void T3DataView::DataStructUpdateEnd_impl() {
  Reset(); // note: should be superfluous, since we did one on start -- TODO: maybe just nuke
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
  if (m_node_so)
    m_node_so->touch();
}

void T3DataView::FillContextMenu_EditItems_impl(taiMenu* menu, int allowed) {
  //TODO
}

void T3DataView::FillContextMenu_impl(taiMenu* menu) {
  //TODO
}

void T3DataView::GetEditActionsS_impl_(int& allowed, int& forbidden) const {
  if (flags & DNF_IS_MEMBER) {
    forbidden |= (taiClipData::EA_CUT | taiClipData::EA_DELETE);
  }
  ISelectable::GetEditActionsS_impl_(allowed, forbidden);
}

ISelectableHost* T3DataView::host() const {
  T3DataViewRoot* root = ((T3DataView*)this)->root(); // safely cast away constness
  return (root) ? root->host : NULL;
}

bool T3DataView::isMapped() const {
  return (m_node_so);
}


taiDataLink* T3DataView::par_link() const {
  if (hasParent()) return parent()->link();
  else     return NULL;
}

MemberDef* T3DataView::par_md() const {
  if (hasParent()) return parent()->md();
  else     return NULL;
}

void T3DataView::OnWindowBind(iT3DataViewFrame* vw) {
  OnWindowBind_impl(vw);
}

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
  //TODO: maybe make more efficient
  T3DataViewRoot* rval = GET_MY_OWNER(T3DataViewRoot);
  return rval;
}
/*obs
void T3DataView::SetPos(const TDCoord& pos) {
  SetPos(pos.x, pos.y, pos.z);
}

void T3DataView::SetPos(int x, int y, int z) {
  FloatTransform* ft = transform(true);
//  ft->translate.SetXYZ(x, (z * pdpZScale) + 0.5f, -y);
  ft->translate.SetXYZ(x, (z * pdpZScale) + 0.5f, -y);
} */

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

String T3DataView::view_name() const {
  // if we have a name, use that
  // otherwise, if a member, use the member name,
  // otherwise, get from the data
  String rval = GetName();
  if (!rval.empty()) return rval;

  if ((flags & DNF_IS_MEMBER) && m_md)
    return m_md->name;

  //TODO: may need to delegate to link, and let link generate a name
  // for anonymous list items
  if (m_data)
    return ((T3DataView*)this)->data()->GetName(); // safe unconstify

  //TODO: should use centralized, base class default name
  return String("(no name)");
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
  m_node_so = new T3NodeParent;
}




//////////////////////////
//    iRenderArea	//
//////////////////////////

// this is an internal implementation class -- it can be subclassed from SoQtRenderArea, or
// one of its convenience/development subclasses

class iRenderArea: public SoQtExaminerViewer {
typedef SoQtExaminerViewer inherited;
public:

  iRenderArea(iT3ViewspaceWidget *parent, const char *name=NULL, SbBool embed=TRUE,
     SoQtFullViewer::BuildFlag flag=BUILD_ALL, SoQtViewer::Type type=BROWSER);

  iT3ViewspaceWidget*		t3vw;
protected:
  override void 		processEvent(QEvent* ev_);
//  override SbBool 		processSoEvent(const SoEvent* const ev);
};


iRenderArea::iRenderArea(iT3ViewspaceWidget *parent, const char *name, SbBool embed,
     SoQtFullViewer::BuildFlag flag, SoQtViewer::Type type)
:inherited(parent, name, embed, flag, type)
{
  t3vw = parent;
}


void iRenderArea::processEvent(QEvent* ev_) {
  //NOTE: the base classes don't check if event is already handled, so we have to skip
  // calling inherited if we handle it ourselves
  if (!t3vw) goto do_inherited;

  if (ev_->type() == QEvent::MouseButtonPress) {
    QMouseEvent* ev = (QMouseEvent*)ev_;
    if (ev->button() == Qt::RightButton) {
//cerr << "iRenderArea::processEvent: right mouse press handled\n";
      //TODO: maybe should check for item under mouse???
      //TODO: pos will need to be adjusted for parent offset of renderarea ???
      ev->accept();
      t3vw->ContextMenuRequested(ev->globalPos());
      return;
    }
  }

do_inherited:
  inherited::processEvent(ev_);
}

/*nn SbBool iRenderArea::processSoEvent(const SoEvent* const ev)
{
  if (!t3vw) goto do_inherited;

  if (ev->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
    SoMouseButtonEvent* const e = (SoMouseButtonEvent *) ev;
    if ((e->getButton() == SoMouseButtonEvent::BUTTON2)) {
      if (e->getState() == SoButtonEvent::DOWN) {
        iPoint pos(e->getPosition());
        t3vw->emit_contextMenuRequested(QPoint(pos));
        // Steal all RMB-events if the viewer uses the popup-menu.
        return TRUE;
      }
    }
  }

do_inherited:
  return inherited::processSoEvent(ev);
} */


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

iT3ViewspaceWidget::iT3ViewspaceWidget(QWidget* parent)
:QWidget(parent)
{
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
}

void iT3ViewspaceWidget::init() {
  m_renderArea = NULL;
  m_horScrollBar = NULL;
  m_verScrollBar = NULL;
  m_root_so = new SoSeparator(); // refs
//nn  m_raw = NULL;
  m_selMode = SM_NONE;
  m_scene = NULL;
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
//       rend_act->setNumPasses(2);    // 1 = no antialiasing; 2 = antialiasing
      // this does not work on the mac at least

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

QScrollBar* iT3ViewspaceWidget::verScrollBar(bool auto_create) {
  if (auto_create && !(m_verScrollBar))
    setHasVerScrollBar(true);
  return m_verScrollBar;
}

void iT3ViewspaceWidget::ContextMenuRequested(const QPoint& pos) {
  ISelectable* ci = curItem();
  //TODO: what to do if nothing selected, but user right clicks???
  //TODO: how to handle multi-select
  if (ci == NULL) return;
  T3DataView* dv = (T3DataView*)ci->This();

  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);
  //TODO: any for us first (ex. delete)

  dv->FillContextMenu(selItems(), menu); // also calls link menu filler

  FillContextMenu(menu);

  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
}

void iT3ViewspaceWidget::SoSelectionEvent(iSoSelectionEvent* ev) {
  T3DataView* t3node = T3DataView::GetViewFromPath(ev->path);
  if (!t3node) return;

  if (ev->is_selected) {
    AddSelectedItem(t3node);
  } else {
    RemoveSelectedItem(t3node);
  }
  // notify to our frame that we have grabbed focus
  Emit_GotFocusSignal();
}


void iT3ViewspaceWidget::UpdateSelectedItems_impl() {
  //TODO
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
//nn  Reset_impl();
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


  m_ra = new iRenderArea(t3vs);
  m_ra->setBackgroundColor(SbColor(0.5f, 0.5f, 0.5f));
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
    QMessageBox::information(this, "No scene", "No scene exists yet.", "Ok");
    return;
  }

  if (!fd) {
    fd = new QFileDialog(this, "fd");
    fd->setFilter( "Inventor files (*.iv)" );
  }
  fd->setMode(QFileDialog::AnyFile);
  if (!fd->exec()) return;
  QString fileName = fd->selectedFile();
  // check if exists, to warn user
  QFile f(fileName);
  if (f.exists()) {
    if (QMessageBox::question(this, "Confirm file overwrite", "That file already exists, overwrite it?",
        QString::null, "&Ok", "&Cancel", 1, 1) != 0) return;
  }
  SoOutput out;
  if (!out.openFile(fileName)) {
    QMessageBox::warning(this, "File error", "Could not open file.", "Ok");
    return;
  }
  SoWriteAction wa(&out);
  wa.apply(scene);

  out.closeFile();
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
//TODO  m_sel_items.Reset();
  setSceneTop(NULL);
}

void iT3DataViewFrame::Refresh_impl() {
  T3DataViewRoot* rt = root();
  if (rt) 
    rt->Refresh();
}

T3DataViewRoot* iT3DataViewFrame::root() {
  return (m_viewer) ? &(((T3DataViewFrame*)m_viewer)->root_view) : NULL;
}

void iT3DataViewFrame::setSceneTop(SoNode* node) {
  t3vs->setSceneGraph(node);
}

void iT3DataViewFrame::T3DataViewClosing(T3DataView* node) {
//TODO  RemoveSelectedItem(node);
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
}

void T3DataViewFrame::Destroy() {
  Reset();
  CutLinks();
}

void T3DataViewFrame::InitLinks() {
  inherited::InitLinks();
  taBase::Own(root_view, this);
}

void T3DataViewFrame::CutLinks() {
  root_view.CutLinks();
  inherited::CutLinks();
}

void T3DataViewFrame::Copy_(const T3DataViewFrame& cp) {
  root_view = cp.root_view;
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
  root_view.host = widget()->t3vs; // TODO: prob should encapsulate this better
}

void T3DataViewFrame::Constr_post() {
  inherited::Constr_post();
  root_view.OnWindowBind(widget());
}

IDataViewWidget* T3DataViewFrame::ConstrWidget_impl(QWidget* gui_parent) {
  return new iT3DataViewFrame(this, gui_parent);
}

void T3DataViewFrame::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if (dcr == DCR_ITEM_UPDATED) {
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

void T3DataViewFrame::Render_pre() {
  inherited::Render_pre();
  widget()->Render_pre();
  root_view.Render_pre();
}

void T3DataViewFrame::Render_impl() {
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
  widget()->ra()->viewAll();
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
  tw = new QTabWidget(this); //top, standard tabs
  lay->addWidget(tw);
}

void iT3DataViewer::AddT3DataViewFrame(iT3DataViewFrame* idvf, int idx) {
  T3DataViewFrame* dvf = idvf->viewer();
  String tab_label = dvf->GetName();
  if (idx < 0)
    tw->addTab(idvf, tab_label);
  else
    tw->insertTab(idx, idvf, tab_label);
  idvf->t3vs->Connect_SelectableHostNotifySignal(this, 
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
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

//////////////////////////
//	T3DataViewer	//
//////////////////////////

T3DataViewFrame* T3DataViewer::GetBlankOrNewT3DataViewFrame(taBase* obj) {
  if (!obj) return NULL;
  T3DataViewFrame* fr = NULL;
  taProject* proj = (taProject*)obj->GetOwner(&TA_taProject);
  //TODO: this would be more intuitive if done in current or top-most proj browser
  MainWindowViewer* vw = MainWindowViewer::GetDefaultProjectBrowser(proj);
  if (!vw) return NULL; // shouldn't happen
  T3DataViewer* t3vw = (T3DataViewer*)vw->FindFrameByType(&TA_T3DataViewer);
  if (!t3vw) return NULL; // shouldn't happen
  // make in default, if default is empty
  fr = t3vw->FirstEmptyT3DataViewFrame();
  if (!fr)
    fr = t3vw->NewT3DataViewFrame();
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
    T3DataViewFrame* fv = (T3DataViewFrame*)frames.New(1);
    fv->SetName("DefaultFrame");
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
  for (int i = 0; i < frames.size; ++i) {
    T3DataViewFrame* fv = frames.FastEl(i);
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



