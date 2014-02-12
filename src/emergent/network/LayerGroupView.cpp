// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "LayerGroupView.h"
#include <T3LayerGroupNode>
#include <T3LayerNode>
#include <Network>
#include <NetView>
#include <LayerView>
#include <MemberDef>
#include <T3ExaminerViewer>
#include <T3Misc>
#include <taProject>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>

TA_BASEFUNS_CTORS_DEFN(LayerGroupView);

void LayerGroupView::Initialize() {
  data_base = &TA_Layer_Group;
  root_laygp = false;           // set by NetView after init, during build
}

void LayerGroupView::Destroy() {
  Reset();
}

void LayerGroupView::BuildAll() {
  Reset(); //for when we are invoked after initial construction
//  String node_nm;

  NetView* nv = getNetView();
  Layer_Group* lgp = layer_group(); //cache
  for(int li=0;li<lgp->size;li++) {
    Layer* lay = lgp->FastEl(li);
    if(!nv->show_iconified && lay->Iconified()) continue;
    LayerView* lv = new LayerView();
    lv->SetData(lay);
    children.Add(lv);
    lv->BuildAll();
  }

  for(int gi=0;gi<lgp->gp.size;gi++) {
    Layer_Group* slgp = (Layer_Group*)lgp->gp.FastEl(gi);
    LayerGroupView* lv = new LayerGroupView();
    lv->SetData(slgp);
    children.Add(lv);
    lv->BuildAll();
  }
}

void LayerGroupView::UpdateUnitValues() { // *actually* only does unit value updating
  for(int i=0; i<children.size; i++) {
    T3DataView* chld = (T3DataView*)children.FastEl(i);
    if(chld->InheritsFrom(&TA_LayerView)) {
      ((LayerView*)chld)->UpdateUnitValues();
    }
    else if(chld->InheritsFrom(&TA_LayerGroupView)) {
      ((LayerGroupView*)chld)->UpdateUnitValues();
    }
  }
}

void LayerGroupView::InitDisplay() {
  for(int i=0; i<children.size; i++) {
    T3DataView* chld = (T3DataView*)children.FastEl(i);
    if(chld->InheritsFrom(&TA_LayerView)) {
      ((LayerView*)chld)->InitDisplay();
    }
    else if(chld->InheritsFrom(&TA_LayerGroupView)) {
      ((LayerGroupView*)chld)->InitDisplay();
    }
  }
}

void LayerGroupView::UpdateAutoScale(bool& updated) {
  for(int i=0; i<children.size; i++) {
    T3DataView* chld = (T3DataView*)children.FastEl(i);
    if(chld->InheritsFrom(&TA_LayerView)) {
      ((LayerView*)chld)->UpdateAutoScale(updated);
    }
    else if(chld->InheritsFrom(&TA_LayerGroupView)) {
      ((LayerGroupView*)chld)->UpdateAutoScale(updated);
    }
  }
}

void LayerGroupView::SetHighlightSpec(BaseSpec* spec) {
  for(int i=0; i<children.size; i++) {
    T3DataView* chld = (T3DataView*)children.FastEl(i);
    if(chld->InheritsFrom(&TA_LayerView)) {
      ((LayerView*)chld)->SetHighlightSpec(spec);
    }
    else if(chld->InheritsFrom(&TA_LayerGroupView)) {
      ((LayerGroupView*)chld)->SetHighlightSpec(spec);
    }
  }
}

void LayerGroupView::SigRecvUpdateAfterEdit_impl() {
  inherited::SigRecvUpdateAfterEdit_impl();
  // always update kids!!
//   DoActionChildren_impl(RENDER_IMPL);

//   NetView* nv = GET_MY_OWNER(NetView);
//   if (!nv) return;
//   nv->Layer_DataUAE(this);
}

taBase::DumpQueryResult LayerGroupView::Dump_QuerySaveMember(MemberDef* md) {
  static String str_ch("children");
  // no save -- won't happen anyway b/c network doesn't save us either..
  if (md->name == str_ch) return DQR_NO_SAVE;
  else return inherited::Dump_QuerySaveMember(md);
}

void LayerGroupView::DoHighlightColor(bool apply) {
  T3LayerGroupNode* nd = node_so();
  if (!nd) return;
//  NetView* nv = getNetView();

  SoMaterial* mat = node_so()->material(); //cache
  if (apply) {
    mat->diffuseColor.setValue(m_hcolor);
    mat->transparency.setValue(0.0f);
  } else {
    mat->diffuseColor.setValue(0.8f, 0.5f, 0.8f); // violet
    mat->transparency.setValue(0.3f);
  }
}

void LayerGroupView::Render_pre() {
  bool show_drag = true;;
  T3ExaminerViewer* vw = GetViewer();
  if(vw)
    show_drag = vw->interactionModeOn();

  NetView* nv = getNetView();
  if(!nv->lay_mv) show_drag = false;

  if(root_laygp)
    show_drag = false;          // never for root.

  bool hide_lines = !nv->view_params.show_laygp;
  if(root_laygp) hide_lines = true; // always true

  setNode(new T3LayerGroupNode(this, show_drag, hide_lines,
			       nv->lay_layout == NetView::TWO_D));
  DoHighlightColor(false);

  inherited::Render_pre();
}

void LayerGroupView::Render_impl() {
  Layer_Group* lgp = this->layer_group(); //cache
  NetView* nv = getNetView();

  taVector3i pos;

  T3LayerGroupNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  float net_margin = 0.05f;
  float gpz_2d = 0.01f;

  taTransform* ft = transform(true);
  if(nv->lay_layout == NetView::THREE_D) {
    lgp->GetAbsPos(pos);
    ft->translate.SetXYZ((float)pos.x / nv->eff_max_size.x,
			 ((float)pos.z) / nv->eff_max_size.z,
			 (float)-pos.y / nv->eff_max_size.y);

    node_so->setGeom(lgp->pos.x, lgp->pos.y, lgp->pos.z,
		     lgp->max_disp_size.x, lgp->max_disp_size.y, lgp->max_disp_size.z,
		     nv->eff_max_size.x, nv->eff_max_size.y, nv->eff_max_size.z);
  }
  else {
    lgp->GetAbsPos2d(pos);
    ft->translate.SetXYZ(((float)pos.x) / nv->eff_max_size.x,
			 0.0f,
			 -net_margin + ((float)-pos.y) / nv->eff_max_size.y);

    node_so->setGeom(lgp->pos2d.x, lgp->pos2d.y, 0,
		     lgp->max_disp_size2d.x, lgp->max_disp_size2d.y, 1.0,
		     nv->eff_max_size.x, nv->eff_max_size.y, 5.0f);
  }

  if(!node_so->hideLines()) {
    node_so->drawStyle()->lineWidth = nv->view_params.laygp_width;

    node_so->setCaption(data()->GetName().chars());
    float lay_wd_y = T3LayerNode::width / nv->eff_max_size.y;
    float lay_ht_z = T3LayerNode::height / nv->eff_max_size.z;
    float fx = (float)lgp->max_disp_size.x / nv->eff_max_size.x;
    lay_wd_y = MIN(lay_wd_y, T3LayerNode::max_width);

    // ensure that the layer label does not go beyond width of layer itself!
    float eff_lay_font_size = nv->font_sizes.layer;
    float lnm_wd = (eff_lay_font_size * lgp->name.length()) / T3Misc::char_ht_to_wd_pts;
    if(lnm_wd > fx) {
      eff_lay_font_size = (fx / (float)lgp->name.length()) * T3Misc::char_ht_to_wd_pts;
    }
    node_so->resizeCaption(eff_lay_font_size);

    if(nv->lay_layout == NetView::THREE_D) {
      SbVec3f tran(0.0f, -eff_lay_font_size - 2.0f * lay_ht_z, lay_wd_y);
      node_so->transformCaption(tran);
    }
    else {
      SbVec3f tran(0.0f, 0.5f * lay_wd_y, 1.5f * eff_lay_font_size);
      SbRotation rot(SbVec3f(1.0f, 0.0f, 0.0f), -1.5707963f);
      node_so->transformCaption(rot, tran);
    }
  }

  inherited::Render_impl();
}

// callback for layer xy dragger
void T3LayerGroupNode_XYDragFinishCB(void* userData, SoDragger* dragr) {
  SoTranslate2Dragger* dragger = (SoTranslate2Dragger*)dragr;
  T3LayerGroupNode* laynd = (T3LayerGroupNode*)userData;
  LayerGroupView* lv = static_cast<LayerGroupView*>(laynd->dataView());
  Layer_Group* lgp = lv->layer_group();
  NetView* nv = lv->getNetView();
  Network* net = nv->net();
  taProject* proj = GET_OWNER(net, taProject);

  float fx = ((float)lgp->max_disp_size.x + 2.0f * T3LayerNode::width) / nv->eff_max_size.x;
  float fy = ((float)lgp->max_disp_size.y + 2.0f * T3LayerNode::width) / nv->eff_max_size.y;
  float fz = ((float)(lgp->max_disp_size.z-1) + 4.0f * T3LayerNode::height) / nv->eff_max_size.z;
  float xfrac = (.5f * fx) - (T3LayerNode::width / nv->eff_max_size.x);
  float yfrac = (.5f * fy) - (T3LayerNode::width / nv->eff_max_size.y);
  float zfrac = (.5f * fz) - 2.0f * (T3LayerNode::height / nv->eff_max_size.z);

  const SbVec3f& trans = dragger->translation.getValue();
  float new_x = trans[0] * nv->eff_max_size.x;
  float new_y = trans[1] * nv->eff_max_size.y;
  if(new_x < 0.0f)      new_x -= .5f; // add an offset to effect rounding.
  else                  new_x += .5f;
  if(new_y < 0.0f)      new_y -= .5f;
  else                  new_y += .5f;

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Layer Group Move", net, false, NULL); // save at net
  }

  if(nv->lay_layout == NetView::THREE_D) {
    lgp->pos.x += (int)new_x;
    if(lgp->pos.x < 0) lgp->pos.x = 0;
    lgp->pos.y += (int)new_y;
    if(lgp->pos.y < 0) lgp->pos.y = 0;
  }
  else {
    lgp->pos2d.x += (int)new_x;
    if(lgp->pos2d.x < 0) lgp->pos2d.x = 0;
    lgp->pos2d.y += (int)new_y;
    if(lgp->pos2d.y < 0) lgp->pos2d.y = 0;
  }

  laynd->txfm_shape()->translation.setValue(xfrac, zfrac, -yfrac); // reset!
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);

  lgp->SigEmitUpdated();
  nv->net()->LayerPos_Cleanup(); // reposition everyone to avoid conflicts

  nv->UpdateDisplay();
}

// callback for layer z dragger
void T3LayerGroupNode_ZDragFinishCB(void* userData, SoDragger* dragr) {
  SoTranslate1Dragger* dragger = (SoTranslate1Dragger*)dragr;
  T3LayerGroupNode* laynd = (T3LayerGroupNode*)userData;
  LayerGroupView* lv = static_cast<LayerGroupView*>(laynd->dataView());
  Layer_Group* lgp = lv->layer_group();
  NetView* nv = lv->getNetView();
  Network* net = nv->net();
  taProject* proj = GET_OWNER(net, taProject);

  float fz = (float)lgp->max_disp_size.z / nv->eff_max_size.z;
  float zfrac = .5f * fz;

  const SbVec3f& trans = dragger->translation.getValue();
  float new_z = trans[0] * nv->eff_max_size.z;
  if(new_z < 0.0f)      new_z -= .5f;
  else                  new_z += .5f;

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Layer Group Move", net, false, NULL); // save at net
  }

  if(nv->lay_layout == NetView::THREE_D) {
    lgp->pos.z += (int)new_z;
    if(lgp->pos.z < 0) lgp->pos.z = 0;
  }

  const SbVec3f& shptrans = laynd->txfm_shape()->translation.getValue();
  laynd->txfm_shape()->translation.setValue(shptrans[0], zfrac, shptrans[2]); // reset!
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);

  lgp->SigEmitUpdated();
  nv->net()->LayerPos_Cleanup(); // reposition everyone to avoid conflicts

  nv->UpdateDisplay();
}

