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

#include "LayerView.h"
#include <T3LayerNode>
#include <Network>
#include <NetView>
#include <UnitView>
#include <LayerView>
#include <UnitGroupView>
#include <MemberDef>
#include <T3ExaminerViewer>
#include <T3Misc>
#include <taProject>
#include <T3Panel>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/SbLinear.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/nodes/SoTransform.h>

void LayerView::Initialize() {
  data_base = &TA_Layer;
  disp_mode = DISP_UNITS;
}

void LayerView::Destroy() {
  Reset();
}

void LayerView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateNetLayDispMode();
}

void LayerView::UpdateNetLayDispMode() {
  NetView* n = getNetView();
  Layer* lay = layer(); //cache
  if(n && lay) {
    n->SetLayDispMode(lay->name, disp_mode);
  }
}

void LayerView::BuildAll() {
  NetView* n = getNetView();
  Layer* lay = layer(); //cache

  int dspmd = n->GetLayDispMode(lay->name);
  if(dspmd >= 0) disp_mode = (LayerView::DispMode)dspmd;

  Reset(); //for when we are invoked after initial construction

  UnitGroupView* ugv = new UnitGroupView;       // always just one guy to run everything there
  ugv->SetData(lay);            // unitgroupview data is layer!
  ugv->SetLayerView(this);
  children.Add(ugv);
  ugv->BuildAll();
}

void LayerView::InitDisplay() {
  UnitGroupView* ugrv = (UnitGroupView*)children.SafeEl(0);
  if(ugrv)
    ugrv->InitDisplay();
}

void LayerView::UpdateUnitValues() { // *actually* only does unit value updating
  UnitGroupView* ugrv = (UnitGroupView*)children.SafeEl(0);
  if(ugrv)
    ugrv->UpdateUnitValues();
}

void LayerView::UpdateAutoScale(bool& updated) {
  UnitGroupView* ugrv = (UnitGroupView*)children.SafeEl(0);
  if(ugrv)
    ugrv->UpdateAutoScale(updated);
}

void LayerView::SigRecvUpdateAfterEdit_impl() {
  inherited::SigRecvUpdateAfterEdit_impl();
  // always update kids!!
  DoActionChildren_impl(RENDER_IMPL);

  NetView* nv = GET_MY_OWNER(NetView);
  if (!nv) return;
  nv->Layer_DataUAE(this);
}

taBase::DumpQueryResult LayerView::Dump_QuerySaveMember(MemberDef* md) {
  static String str_ch("children");
  // for this class only, we never save groups/units
  if (md->name == str_ch) return DQR_NO_SAVE;
  else return inherited::Dump_QuerySaveMember(md);
}

void LayerView::DoHighlightColor(bool apply) {
  T3LayerNode* nd = node_so();
  if (!nd) return;
  NetView* nv = getNetView();

  SoMaterial* mat = node_so()->material(); //cache
  if (apply) {
    mat->diffuseColor.setValue(m_hcolor);
    mat->transparency.setValue(0.0f);
  } else {
    Layer* lay = layer();
    if(lay && lay->lesioned())
      mat->diffuseColor.setValue(0.5f, 0.5f, 0.5f); // grey
    else
      mat->diffuseColor.setValue(0.0f, 0.5f, 0.5f); // aqua
    mat->transparency.setValue(nv->view_params.lay_trans);
  }
}

void LayerView::Render_pre() {
  bool show_drag = true;;
  T3ExaminerViewer* vw = GetViewer();
  if(vw)
    show_drag = vw->interactionModeOn();

  NetView* nv = getNetView();
  if(!nv->lay_mv) show_drag = false;

  T3LayerNode* node_so = new T3LayerNode(this, show_drag,
					 nv->lay_layout == NetView::TWO_D);
  setNode(node_so);
  DoHighlightColor(false);

  inherited::Render_pre();
}

void LayerView::Render_impl() {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  NetView* nv = getNetView();

  taTransform* ft = transform(true);
  T3LayerNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  if(nv->lay_layout == NetView::THREE_D) {
    ft->translate.SetXYZ((float)lay->pos.x / nv->eff_max_size.x,
			 (float)lay->pos.z / nv->eff_max_size.z,
			 (float)-lay->pos.y / nv->eff_max_size.y);
  }
  else {
    ft->translate.SetXYZ((float)lay->pos2d.x / nv->eff_max_size.x,
			 0.0f,
			 (float)-lay->pos2d.y / nv->eff_max_size.y);
    // ft->rotate.SetXYZR(1.0f, 0.0f, 0.0f, 1.5707963f);
  }

  if(lay->Iconified()) {
    node_so->setGeom(1, 1, nv->eff_max_size.x, nv->eff_max_size.y,
		     nv->eff_max_size.z, 1.0f);
  }
  else {
    node_so->setGeom(lay->disp_geom.x, lay->disp_geom.y,
		     nv->eff_max_size.x, nv->eff_max_size.y,
		     nv->eff_max_size.z, lay->disp_scale);
  }
  float max_xy = MAX(nv->eff_max_size.x, nv->eff_max_size.y);
  float fx = (float)lay->scaled_disp_geom.x / nv->eff_max_size.x;

  node_so->setCaption(data()->GetName().chars());

  float lay_wd = T3LayerNode::width / max_xy;
  lay_wd = MIN(lay_wd, T3LayerNode::max_width);

  // ensure that the layer label does not go beyond width of layer itself!
  float eff_lay_font_size = nv->font_sizes.layer;
  float lnm_wd = (eff_lay_font_size * lay->name.length()) / T3Misc::char_ht_to_wd_pts;
  if(lnm_wd > fx) {
    eff_lay_font_size = (fx / (float)lay->name.length()) * T3Misc::char_ht_to_wd_pts;
  }
  eff_lay_font_size = MAX(eff_lay_font_size, nv->font_sizes.layer_min);
  node_so->resizeCaption(eff_lay_font_size);

  if(nv->lay_layout == NetView::THREE_D) {
    SbVec3f tran(0.0f, -eff_lay_font_size, lay_wd);
    node_so->transformCaption(tran);
  }
  else {
    SbVec3f tran(0.0f, 0.5f * lay_wd, 1.1f * eff_lay_font_size);
    SbRotation rot(SbVec3f(1.0f, 0.0f, 0.0f), -1.5707963f);
    node_so->transformCaption(rot, tran);
  }
  inherited::Render_impl();
}

// callback for layer xy dragger
void T3LayerNode_XYDragFinishCB(void* userData, SoDragger* dragr) {
  SoTranslate2Dragger* dragger = (SoTranslate2Dragger*)dragr;
  T3LayerNode* laynd = (T3LayerNode*)userData;
  LayerView* lv = static_cast<LayerView*>(laynd->dataView());
  Layer* lay = lv->layer();
  NetView* nv = lv->getNetView();
  Network* net = nv->net();
  taProject* proj = GET_OWNER(net, taProject);

  float fx = (float)lay->disp_geom.x / nv->eff_max_size.x;
  float fy = (float)lay->disp_geom.y / nv->eff_max_size.y;
  float xfrac = .5f * fx;
  float yfrac = .5f * fy;

  const SbVec3f& trans = dragger->translation.getValue();
  float new_x = trans[0] * nv->eff_max_size.x;
  float new_y = trans[1] * nv->eff_max_size.y;
  if(new_x < 0.0f)      new_x -= .5f; // add an offset to effect rounding.
  else                  new_x += .5f;
  if(new_y < 0.0f)      new_y -= .5f;
  else                  new_y += .5f;

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Layer Move", net, false, NULL); // save at net
  }

  if(nv->lay_layout == NetView::THREE_D) {
    lay->pos.x += (int)new_x;
    lay->pos.y += (int)new_y;
  }
  else {
    lay->pos2d.x += (int)new_x;
    lay->pos2d.y += (int)new_y;
  }

  laynd->txfm_shape()->translation.setValue(xfrac, 0.0f, -yfrac); // reset!
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);

  lay->SigEmitUpdated();
  nv->net()->LayerPos_Cleanup(); // reposition everyone to avoid conflicts

  nv->UpdateDisplay();
}

// callback for layer z dragger
void T3LayerNode_ZDragFinishCB(void* userData, SoDragger* dragr) {
  SoTranslate1Dragger* dragger = (SoTranslate1Dragger*)dragr;
  T3LayerNode* laynd = (T3LayerNode*)userData;
  LayerView* lv = static_cast<LayerView*>(laynd->dataView());
  Layer* lay = lv->layer();
  NetView* nv = lv->getNetView();
  Network* net = nv->net();
  taProject* proj = GET_OWNER(net, taProject);

  const SbVec3f& trans = dragger->translation.getValue();
  float new_z = trans[0] * nv->eff_max_size.z;
  if(new_z < 0.0f)      new_z -= .5f;
  else                  new_z += .5f;

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Layer Move", net, false, NULL); // save at net
  }

  lay->pos.z += (int)new_z;
//   if(lay->pos.z < 0) lay->pos.z = 0;

  const SbVec3f& shptrans = laynd->txfm_shape()->translation.getValue();
  laynd->txfm_shape()->translation.setValue(shptrans[0], 0.0f, shptrans[2]); // reset!
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);

  lay->SigEmitUpdated();
  nv->net()->LayerPos_Cleanup(); // reposition everyone to avoid conflicts

  nv->UpdateDisplay();
}

void LayerView::DispUnits() {
  disp_mode = DISP_UNITS;
  UpdateNetLayDispMode();
  if (getNetView())
    getNetView()->Render();
}

void LayerView::DispOutputName() {
  disp_mode = DISP_OUTPUT_NAME;
  UpdateNetLayDispMode();
  if (getNetView())
    getNetView()->Render();
}

void LayerView::UseViewer(T3DataViewMain* viewer) {
  disp_mode = DISP_FRAME;
  UpdateNetLayDispMode();

  if(!viewer) return;
  NetView* nv = getNetView();
  Layer* lay = this->layer(); //cache
  if(!nv || !lay) return;

  viewer->main_xform = nv->main_xform; // first get the network

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->main_xform.rotate.x, nv->main_xform.rotate.y,
                           nv->main_xform.rotate.z), nv->main_xform.rotate.rot);
  // translate to layer offset + indent into layer
  SbVec3f trans;
  float szx = ((float)lay->scaled_disp_geom.x / nv->eff_max_size.x);
  float szy = ((float)lay->scaled_disp_geom.y / nv->eff_max_size.y);
  taVector3i pos;
  if(nv->lay_layout == NetView::THREE_D) {
    lay->GetAbsPos(pos);
  }
  else {
    lay->GetAbsPos2d(pos);
    pos.z = 0.0f;
  }

  trans[0] = nv->main_xform.scale.x * (((float)pos.x / nv->eff_max_size.x) + .05f * szx);
  trans[1] = nv->main_xform.scale.y * ((((float)pos.z + 0.5f) / nv->eff_max_size.z));
  trans[2] = nv->main_xform.scale.z * (((float)-pos.y / nv->eff_max_size.y) - .05f * szy);

  cur_rot.multVec(trans, trans); // rotate the translation by current rotation
  viewer->main_xform.translate.x += trans[0];
  viewer->main_xform.translate.y += trans[1];
  viewer->main_xform.translate.z += trans[2];

  // scale to size of layer
  taVector3f sc;
  sc.x = .8f * szx;
  sc.y = .8f * szy;
  sc.z = 1.0f;
  viewer->main_xform.scale *= sc;

  // rotate down in the plane
  SbRotation rot;
  rot.setValue(SbVec3f(1.0f, 0.0f, 0.0f), -1.5708f);
  SbRotation nw_rot = rot * cur_rot;
  SbVec3f axis;
  float angle;
  nw_rot.getValue(axis, angle);
  viewer->main_xform.rotate.SetXYZR(axis[0], axis[1], axis[2], angle);

  T3Panel* fr = GetFrame();
  if(fr) fr->Render();
}

void LayerView::SetHighlightSpec(BaseSpec* spec) {
  Layer* lay = layer();
  if(lay && NetView::UsesSpec(lay, spec)) {
    bool ok;
    iColor hc = spec->GetEditColorInherit(ok);
    if (ok) {
      setHighlightColor(T3Color(hc));
    }
  }
  else {
    setDefaultColor();
  }
}

