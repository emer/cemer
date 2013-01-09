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

#include "T3DataView.h"

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
      rval = t3node->dataView();
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
    taMisc::DebugInfo("T3DataView_inst_cnt:", String(T3DataView_inst_cnt));
#endif
}

void T3DataView::Destroy() {
//should not be needed...  DestroyPanels();
  CutLinks();
#ifdef TA_PROFILE
  --T3DataView_inst_cnt;
  if (cssMisc::refcnt_trace)
    taMisc::DebugInfo("T3DataView_inst_cnt:", String(T3DataView_inst_cnt));
#endif
}

void T3DataView::Copy_(const T3DataView& cp) {
  flags = cp.flags;
  m_md = cp.m_md;
  Clear_impl(); // hope this works!
  last_child_node = NULL;
  taTransform* ft = cp.m_transform;
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
  if(m_transform) fixTransformAxis();
}

bool T3DataView::fixTransformAxis() {
  if(!m_transform) return false;
  if(m_transform->rotate.x == 0.0f && m_transform->rotate.y == 0.0f &&
     m_transform->rotate.z == 0.0f) {
    m_transform->rotate.z = 1.0f;       // axis must be defined, even if not used.
    return true;
  }
  return false;
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
  if (T3Node *ch_so = child->node_so()) {
    AddRemoveChildNode(ch_so, false); // remove node
    child->setNode(NULL);
  }
}

void T3DataView::ChildRendered(taDataView* child_) { // child is always a T3DataView
  if (!node_so()) return; // shouldn't happen
  if (T3DataView *child = dynamic_cast<T3DataView*>(child_)) {
    if (T3Node *ch_so = child->node_so()) {
      AddRemoveChildNode(ch_so, true); // add node
    }
  }
}

void T3DataView::ChildRemoving(taDataView* child_) {
  if (T3DataView *child = dynamic_cast<T3DataView*>(child_)) {
    // remove the visual rendering of child, if any
    if (SoNode *ch_so = child->node_so()) {
      AddRemoveChildNode(ch_so, false); // remove node
      child->setNode(NULL);
    }
  }
}

void T3DataView::Clear_impl() { // note: no absolute guarantee par will be T3DataView
  inherited::Clear_impl(); // does children
  // can't have any visual children, if we don't exist ourselves...
  // also, don't need to report to parent if we don't have any impl ourselves
  if (!m_node_so.ptr()) return;

  // we remove top-most item first, which results in only one update to the scene graph
  if (hasParent()) {
    parent()->ChildClearing(this); // parent clears us
  }
  else {
    setNode(NULL);
  }
}

taiDataLink* T3DataView::clipParLink(GuiContext sh_typ) const {
  return own_link(sh_typ);
}

void T3DataView::setNode(T3Node* node_) {
  if (m_node_so.ptr() == node_) return; // generally shouldn't happen
  if (m_node_so.ptr()) {
    if (T3DataViewFrame *dvf = GetFrame()) {
      if (iT3DataViewFrame *idvf = dvf->widget()) {
        idvf->NodeDeleting(m_node_so); // just desels all, for now
      }
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

T3DataViewFrame* T3DataView::GetFrame() const {
  T3DataViewFrame* frame = GET_MY_OWNER(T3DataViewFrame);
  return frame;
}

T3ExaminerViewer* T3DataView::GetViewer() const {
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
  if(!m_node_so) {
    // taMisc::Info("view:",name,"not mapped because no node_so");
    return false;
  }
  if(taMisc::gui_no_win) return true; // bypass these further checks for no_win case
  T3ExaminerViewer* vw = GetViewer();
  if(!vw) {
    // taMisc::Info("view:",name,"not mapped because no GetViewer");
    return false;
  }
  if(!vw->quarter) {
    // taMisc::Info("view:",name,"not mapped because no quarter");
    return false;
  }
  if(!vw->quarter->isValid()) {
    // taMisc::Info("view:",name,"not mapped because no quarter->isValid()");
    return false;
  }
  return true;
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
  if (T3Node *node = m_node_so.ptr()) {
    node->clear();
  }
}

void T3DataView::Render_impl() {
  if (T3ExaminerViewer *vw = GetViewer()) {
    // always make sure it is sync'd with what we think it should be
    vw->syncViewerMode();
  }

  T3Node* node = m_node_so.ptr();
  if (m_transform && node) {
    fixTransformAxis();         // make sure
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

taTransform* T3DataView::transform(bool auto_create) {
  if (!m_transform && auto_create) {
    m_transform = new taTransform();
    taBase::Own(m_transform, this);
    fixTransformAxis();
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
