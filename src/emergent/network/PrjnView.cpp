// Copyright 2017, Regents of the University of Colorado,
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

#include "PrjnView.h"
#include <T3PrjnNode>
#include <T3LayerNode>
#include <NetView>
#include <Projection>
#include <Layer>
#include <taVector3i>

#ifdef TA_QT3D

#include <T3TwoDText>

#else // TA_QT3D

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>

#endif // TA_QT3D

TA_BASEFUNS_CTORS_DEFN(PrjnView);

void PrjnView::Initialize() {
  data_base = &TA_Projection;
}

void PrjnView::Destroy() {
  Reset();
}

void PrjnView::DoHighlightColor(bool apply) {
  T3PrjnNode* nd = node_so();
  if (!nd) return;
  NetView* nv = getNetView();
  Projection* prjn = this->prjn(); // cache
  float prjn_trans = nv->view_params.prjn_trans;
  if(!nv || !nv->net() || !nv->net()->IsBuiltIntact()) {
    prjn_trans = .8f;
  }

#ifdef TA_QT3D
  nd->line->setColor(prjn->prjn_clr.color());
#else // TA_QT3D
  SoMaterial* mat = node_so()->material(); //cache
  if (apply) {
    mat->diffuseColor.setValue(m_hcolor);
    mat->transparency.setValue(0.0f);
  } else {
    // Color and transparency are assigned from a constant and from NetView, respectively
    // Width is also assigned from NetView, just not here. 
    // Projection objects contain other parameters/attributes, like display on/off
    mat->diffuseColor.setValue(SbColor(prjn->prjn_clr.r, prjn->prjn_clr.g, prjn->prjn_clr.b));
    mat->transparency.setValue(prjn_trans);
  }
  nd->setArrowColor(SbColor(prjn->prjn_clr.r, prjn->prjn_clr.g, prjn->prjn_clr.b), prjn_trans);

#endif // TA_QT3D

}

void PrjnView::Render_pre() {
  NetView* nv = getNetView();
  Projection* prjn = this->prjn(); // cache
  book projected = true;
  if(!nv || !nv->net() || !nv->net()->IsBuiltIntact()) {
    projected = false;
  }
#ifdef TA_QT3D
  setNode(new T3PrjnNode(NULL, this, projected, nv->view_params.prjn_width,
                         nv->lay_layout == NetView::TWO_D));
#else // TA_QT3D
  setNode(new T3PrjnNode(this, projected, nv->view_params.prjn_width));
#endif // TA_QT3D
  DoHighlightColor(false);
  inherited::Render_pre();
}

void PrjnView::Render_impl() {
  T3PrjnNode* node_so = this->node_so(); // cache
  NetView* nv = getNetView();
  if(!node_so) return;

  // find the total receive num, and our ordinal
  Projection* prjn = this->prjn(); // cache
  Layer* lay_fr = prjn->from;
  Layer* lay_to = prjn->layer;

  if(lay_fr == NULL) lay_fr = lay_to;

  float net_margin = 0.05f;

  taVector3f src;             // source and dest coords
  taVector3f dst;
  taVector3i lay_fr_pos;
  taVector3i lay_to_pos;

  if(nv->lay_layout == NetView::THREE_D) {
    lay_fr->GetAbsPos(lay_fr_pos);
    lay_to->GetAbsPos(lay_to_pos);
  }
  else {
    lay_fr->GetAbsPos2d(lay_fr_pos);
    lay_to->GetAbsPos2d(lay_to_pos);
  }

  float max_xy = MAX(nv->eff_max_size.x, nv->eff_max_size.y);
  if(nv->lay_layout == NetView::THREE_D) {
    // y = network z coords -- same for all cases
    src.y = ((float)lay_fr_pos.z) / nv->eff_max_size.z;
    dst.y = ((float)lay_to_pos.z) / nv->eff_max_size.z;
  }
  else {
    // y = network z coords -- same for all cases
    src.y = 0.0f;
    dst.y = 0.0f;
  }

  float lay_ht = T3LayerNode::height / max_xy;
  float lay_wd = T3LayerNode::width / max_xy;
  lay_wd = MIN(lay_wd, T3LayerNode::max_width);

  // move above/below layer plane
  if(src.y < dst.y) {
    src.y += lay_ht; dst.y -= lay_ht;
  }
  else if(src.y > dst.y) {
    src.y -= lay_ht; dst.y += lay_ht;
  }
  else {
    src.y += lay_ht; dst.y += lay_ht;
  }

  // todo: with new Qt3D, we can draw better projection lines!
  
  if(nv->view_params.prjn_disp == NetViewParams::B_F) {
    // origin is *back* center
    src.x = ((float)lay_fr_pos.x + .5f * (float)lay_fr->scaled_disp_geom.x) /
      nv->eff_max_size.x;
    src.z = -((float)(lay_fr_pos.y + lay_fr->scaled_disp_geom.y) / nv->eff_max_size.y) -
      lay_wd;

    // dest is *front* *center*
    dst.x = ((float)lay_to_pos.x + .5f * (float)lay_to->scaled_disp_geom.x) /
      nv->eff_max_size.x;
    dst.z = -((float)lay_to_pos.y / nv->eff_max_size.y) + lay_wd;
  }
  else if(nv->view_params.prjn_disp == NetViewParams::L_R_F) { // easier to see
    // origin is *front* left
    src.x = ((float)lay_fr_pos.x) / nv->eff_max_size.x + lay_wd;
    src.z = -((float)(lay_fr_pos.y) / nv->eff_max_size.y) + lay_wd;

    // dest is *front* right
    dst.x = ((float)lay_to_pos.x + (float)lay_to->scaled_disp_geom.x) / nv->eff_max_size.x
      - lay_wd;
    dst.z = -((float)lay_to_pos.y / nv->eff_max_size.y) + lay_wd;
  }
  else if(nv->view_params.prjn_disp == NetViewParams::L_R_B) { // out of the way
    // origin is *back* left
    src.x = ((float)lay_fr_pos.x) / nv->eff_max_size.x + lay_wd;
    src.z = -((float)(lay_fr_pos.y + lay_fr->scaled_disp_geom.y) / nv->eff_max_size.y) -
      lay_wd;

    // dest is *back* right
    dst.x = ((float)lay_to_pos.x + (float)lay_to->scaled_disp_geom.x) / nv->eff_max_size.x
      - lay_wd;
    dst.z = -((float)(lay_to_pos.y  + lay_to->scaled_disp_geom.y) / nv->eff_max_size.y) -
      lay_wd;
  }

  if(dst.y == src.y && dst.x == src.x && dst.z == src.z) {
    dst.x += lay_wd;            // give it some minimal something
  }

  if(nv->lay_layout == NetView::TWO_D) {
    src.y -= .01f;		// slightly back in depth
    dst.y -= .01f;
    src.z -= net_margin;	// rise up just a bit per similar on layer
    dst.z -= net_margin;
    if(lay_fr->InLayerSubGroup())
      src.z -= net_margin; // extra offset
    if(lay_to->InLayerSubGroup())
      dst.z -= net_margin; // extra offset
  }

  transform(true)->translate.SetXYZ(src.x, src.y, src.z);
#ifdef TA_QT3D
  node_so->SetEndPoint(dst.x - src.x, dst.y - src.y, dst.z - src.z);
#else // TA_QT3D
  node_so->setEndPoint(SbVec3f(dst.x - src.x, dst.y - src.y, dst.z - src.z));
#endif // TA_QT3D

  // caption location is half way
  if(nv->view_params.prjn_name) {
    taVector3f cap((dst.x - src.x) / 2.0f - .05f, (dst.y - src.y) / 2.0f, (dst.z - src.z) / 2.0f);
    node_so->setCaption(prjn->name.chars());
#ifdef TA_QT3D
    node_so->caption->Translate(cap);
#else // TA_QT3D
    node_so->transformCaption(cap);
#endif // TA_QT3D
    node_so->resizeCaption(nv->font_sizes.prjn);
  }

  inherited::Render_impl();
}

void PrjnView::Reset_impl() {
  inherited::Reset_impl();
}

void PrjnView::SetHighlightSpec(BaseSpec* spec) {
  Projection* prjn = this->prjn();
  if(prjn && NetView::UsesSpec(prjn, spec)) {
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

