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

#include "UnitGroupView.h"
#include <T3UnitGroupNode>
#include <T3UnitNode>
#include <Network>
#include <NetView>
#include <UnitView>
#include <LayerView>
#include <T3Color>
#include <MemberDef>
#include <T3Panel>
#include <T3ExaminerViewer>
#include <taSvg>

#include <taMisc>

#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>

TA_BASEFUNS_CTORS_DEFN(UnitGroupView);

void UnitGroupView::Initialize() {
  data_base = &TA_Layer;
  m_lv = NULL;
}

void UnitGroupView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(uvd_hist, this);
  taBase::Own(uvd_hist_idx, this);
  uvd_hist_idx.matrix = &uvd_hist; // this is key, to link to hist
}

void UnitGroupView::CutLinks() {
  uvd_hist_idx.CutLinks();
  uvd_hist.CutLinks();
  inherited::CutLinks();
}

void UnitGroupView::Destroy() {
  Reset();
}

void UnitGroupView::AllocUnitViewData() {
  //note: allocate based on geom, not current size, in case not built yet
  NetView* nv = getNetView();
  Layer* lay = this->layer(); //cache
  if (!lay) return;
  int mbs_sz = MAX(nv->membs.size, 1);
  MatrixGeom nwgm1(3, lay->flat_geom.x, lay->flat_geom.y, mbs_sz);
  if (uvd_bases.geom != nwgm1) {
    uvd_bases.SetGeomN(nwgm1);
  }
  MatrixGeom nwgm2(4, lay->flat_geom.x, lay->flat_geom.y, mbs_sz, nv->hist_max);
  bool reset_idx = nv->hist_reset_req; // if requested somewhere, reset us!
  if (uvd_hist.geom != nwgm2) {
    if (!uvd_hist.SetGeomN(nwgm2)) {
      taMisc::Warning("Forcing nv->hist_max to 1");
      nv->hist_max = 1;
      nwgm2.Set(3, nv->hist_max);
      uvd_hist.SetGeomN(nwgm2); // still might fail, but it's the best we can do.
        }
    reset_idx = true;
    nv->hist_reset_req = true;  // tell main netview history guy to reset and reset everyone
  }
  if (reset_idx) {
    uvd_hist_idx.Reset();
  }
}

void UnitGroupView::BuildAll() {
  Reset(); // in case where we are rebuilding
  NetView* nv = getNetView();
  UpdateUnitViewBases(nv->unit_src);
  if(nv->unit_disp_mode == NetView::UDM_BLOCK) return; // optimized

  Layer* lay = this->layer(); //cache
  if(!lay) return;
  taVector2i coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      if (!unit) continue;
      if(unit->lesioned()) continue;

      UnitView* uv = new UnitView();
      uv->SetData(unit);
      children.Add(uv);
    }
  }
}

void UnitGroupView::InitDisplay() {
  NetView* nv = getNetView();
  UpdateUnitViewBases(nv->unit_src);
}

float UnitGroupView::GetUnitDisplayVal(const taVector2i& co, void*& base) {
  NetView* nv = getNetView();
  float val = nv->scale.zero;
  if(nv->unit_disp_idx < 0) return val;
  base = uvd_bases.SafeEl(co.x, co.y, nv->unit_disp_idx);
  if(!base) return val;

  if(nv->hist_idx > 0) {
    int cidx = (uvd_hist_idx.length - nv->hist_idx);
    int midx = uvd_hist_idx.CircIdx(cidx);
    if(uvd_hist.InRange(co.x, co.y, nv->unit_disp_idx, midx))
      val = uvd_hist.FastEl4d(co.x, co.y, nv->unit_disp_idx, midx);
    else {
      taMisc::Info("midx idx problem:", String(midx), "cidx:",
                   String(cidx), "mbs:", String(uvd_hist.dim(3)));
    }
  }
  else {
    switch (nv->unit_md_flags) {
    case NetView::MD_FLOAT:
      val = *((float*)base); break;
    case NetView::MD_DOUBLE:
      val = *((double*)base); break;
    case NetView::MD_INT:
      val = *((int*)base); break;
    default:
      val = 0.0f; break;
    }
  }
  return val;
}

float UnitGroupView::GetUnitDisplayVal_Idx(const taVector2i& co, int midx, void*& base) {
  NetView* nv = getNetView();
  float val = nv->scale.zero;
  base = uvd_bases.SafeEl(co.x, co.y, midx);
  MemberDef* md = nv->membs.SafeEl(midx);
  if(!base) {
    return val;
  }
  if(md) {
    if(md->type->InheritsFrom(&TA_float))
      val = *((float*)base);
    else if(md->type->InheritsFrom(&TA_double))
      val = *((double*)base);
    else if(md->type->InheritsFrom(&TA_int))
      val = *((int*)base);
  }
  return val;
}

void UnitGroupView::UpdateUnitViewBases(Unit* src_u) {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  NetView* nv = getNetView();
  AllocUnitViewData();
  for(int midx=0;midx<nv->membs.size;midx++) {
    MemberDef* disp_md = nv->membs[midx];
    String nm = disp_md->name.before(".");
    if(nm.empty()) { // direct unit member
      UpdateUnitViewBase_Unit_impl(midx, disp_md);
    }
    else if ((nm=="s") || (nm == "r")) {
      UpdateUnitViewBase_Con_impl(midx, (nm=="s"), disp_md->name.after('.'), src_u,
                                  nv->con_type);
    }
    else if (nm=="bias") {
      UpdateUnitViewBase_Bias_impl(midx, disp_md);
    }
    else { // sub-member of unit
      UpdateUnitViewBase_Sub_impl(midx, disp_md);
    }
  }
}

void UnitGroupView::UpdateUnitViewBase_Con_impl(int midx, bool is_send, String nm,
                                                Unit* src_u, int con_type) {
  if(!src_u) return;
  String prjn_starts_with;
  switch(con_type) {
  case NetView::STD_CON:
    prjn_starts_with = "Fm_";
    break;
  case NetView::CTXT_CON:
    prjn_starts_with = "Ctxt_";
    break;
  }
  bool check_prjn = (prjn_starts_with.nonempty());
    
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  Network* net = lay->own_net;

  taVector2i coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      uvd_bases.Set(NULL, coord.x, coord.y, midx);
      if (!unit) continue;  // rest will be null too, but we loop to null disp_base
      if(unit->lesioned()) continue;

      if (is_send) {
        for(int g=0;g<unit->recv.size;g++) {
          RecvCons* tcong = unit->recv.FastEl(g);
          if(check_prjn && tcong->prjn && !tcong->prjn->name.startsWith(prjn_starts_with))
            continue;
          MemberDef* act_md = tcong->con_type->members.FindName(nm);
          if (!act_md)  continue;
          int con = tcong->FindConFromIdx(src_u);
          if (con < 0) continue;
          // have to use safe b/c could be PtrCon and other side might be gone..
          uvd_bases.Set(&tcong->SafeFastCn(con, act_md->idx, net), coord.x, coord.y, midx);
          break;                // once you've got one, done!
        }
      }
      else {
        for(int g=0;g<unit->send.size;g++) {
          SendCons* tcong = unit->send.FastEl(g);
          if(check_prjn && tcong->prjn && !tcong->prjn->name.startsWith(prjn_starts_with))
            continue;
          MemberDef* act_md = tcong->con_type->members.FindName(nm);
          if (!act_md)  continue;
          int con = tcong->FindConFromIdx(src_u);
          if (con < 0) continue;
          // have to use safe b/c could be PtrCon and other side might be gone..
          uvd_bases.Set(&tcong->SafeFastCn(con, act_md->idx, net), coord.x, coord.y, midx);
          break;                // once you've got one, done!
        }
      }
    }
  }
}

void UnitGroupView::UpdateUnitViewBase_Bias_impl(int midx, MemberDef* disp_md) {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  Network* net = lay->own_net;
  taVector2i coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      uvd_bases.Set(NULL, coord.x, coord.y, midx);
      if (!unit) continue;  // rest will be null too, but we loop to null disp_base
      if(unit->lesioned()) continue;
      if(unit->bias.size == 0) continue;
      uvd_bases.Set(&(unit->bias.OwnCn(0, disp_md->idx)), coord.x, coord.y, midx);
    }
  }
}

void UnitGroupView::UpdateUnitViewBase_Unit_impl(int midx, MemberDef* disp_md) {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  taVector2i coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      uvd_bases.Set(NULL, coord.x, coord.y, midx);
      if (!unit) continue;  // rest will be null too, but we loop to null disp_base
      if(unit->lesioned()) continue;
      uvd_bases.Set(disp_md->GetOff(unit), coord.x, coord.y, midx);
    }
  }
}

void UnitGroupView::UpdateUnitViewBase_Sub_impl(int midx, MemberDef* disp_md) {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  TypeDef* own_td = lay->units.el_typ; // should be unit type
  ta_memb_ptr net_mbr_off = 0;
  int net_base_off = 0;
  MemberDef* smd = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off,
                                                 disp_md->name, false); // no warn
  taVector2i coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      uvd_bases.Set(NULL, coord.x, coord.y, midx);
      if(!unit || !smd) continue;  // rest will be null too, but we loop to null disp_base
      if(unit->lesioned()) continue;
      void* sbaddr = MemberDef::GetOff_static(unit, net_base_off, net_mbr_off);
      uvd_bases.Set(sbaddr, coord.x, coord.y, midx);
    }
  }
}

void UnitGroupView::UpdateAutoScale(bool& updated) {
  NetView* nv = getNetView();
  taVector2i co;
  void* base;
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  for (co.y = 0; co.y < lay->flat_geom.y; ++co.y) {
    for (co.x = 0; co.x < lay->flat_geom.x; ++co.x) {
      float val = GetUnitDisplayVal(co, base);
      if(base) {
        if(!updated) {
          nv->scale.SetMinMax(val, val);
          updated = true;
        }
        else {
          nv->scale.UpdateMinMax(val);
        }
      }
    }
  }
}

// this callback is registered in NetView::Render_pre, defined in NetView.cpp
// void UnitGroupView_MouseCB(void* userData, SoEventCallback* ecb) {

void UnitGroupView::Render_pre() {
  NetView* nv = getNetView();
  Layer* lay = this->layer(); //cache

  if(!lay || lay->Iconified()) {
    return;                     // don't render anything!
  }

  bool no_units = true;
  if(nv->unit_disp_mode != NetView::UDM_BLOCK)
    no_units = false;

  setNode(new T3UnitGroupNode(this, no_units));
  //NOTE: we create/adjust the units in the Render_impl routine
  T3UnitGroupNode* ugrp_so = node_so(); // cache

  ugrp_so->setGeom(lay->disp_geom.x, lay->disp_geom.y, nv->eff_max_size.x,
		   nv->eff_max_size.y, nv->eff_max_size.z, lay->disp_scale);

  inherited::Render_pre();
}

void UnitGroupView::Render_impl() {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  NetView* nv = getNetView();

  inherited::Render_impl();
}

void UnitGroupView::ValToDispText(float val, String& str) {
  float val_abs = fabsf(val);
  if ((val_abs < .0001) || (val_abs >= 10000))
    str.convert(val, "%7.1e"); //note: 7 chars
  if (val_abs < 1)
    str.convert(val, "%#6.4f");
  else if (val_abs < 10)
    str.convert(val, "%#6.3f");
  else if (val_abs < 100)
    str.convert(val, "%#6.2f");
  else if (val_abs < 1000)
    str.convert(val, "%#6.1f");
  else //must be: if (val_abs < 10000)
    str.convert(val, "6.0f");
}

void UnitGroupView::DoActionChildren_impl(DataViewAction acts) {
  if (acts & RENDER_IMPL) {
    acts = (DataViewAction)( acts & ~RENDER_IMPL); // note: only supposed to be one, but don't assume
    Render_impl_children();
    if (!acts) return;
  }
  inherited::DoActionChildren_impl(acts);
}

void UnitGroupView::Render_impl_children() {
  NetView* nv = getNetView(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay || !nv) return;

  if(lay->Iconified() || !lv() || (lv()->disp_mode == LayerView::DISP_FRAME)) {
    return;                     // don't render anything!
  }

  T3UnitGroupNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  UpdateUnitViewBases(nv->unit_src); // always make sure we're allocated properly

  if(lv()->disp_mode == LayerView::DISP_OUTPUT_NAME) {
    Render_impl_outnm();
    return;
  }

  Render_impl_snap_bord();

  if(nv->unit_disp_mode == NetView::UDM_BLOCK) {
    Render_impl_blocks();
    return;
  }

  // for efficiency we assume responsibility for the _impl of UnitViews
  T3UnitNode* unit_so;
  taVector2i co;
  float val;
  float sc_val;
  T3Color col;
  String val_str;
  // if displaying unit text, set up the viewing props in the unitgroup
  if (nv->unit_text_disp != NetView::UTD_NONE) {
    SoFont* font = node_so->unitCaptionFont(true);
    font->size.setValue(nv->font_sizes.unit);
  }

  float max_z = MIN(nv->eff_max_size.x, nv->eff_max_size.y); // smallest XY
  max_z = MAX(max_z, nv->eff_max_size.z); // make sure Z isn't bigger
  float font_y = .45f / nv->eff_max_size.y;
  float trans = nv->view_params.unit_trans;
  float font_z = nv->font_sizes.unit*2.0f;
  SbVec3f font_xform(0.0f, font_z, font_y);

  int ui = 0;
  taVector2i coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      if(!unit) continue;
      if(unit->lesioned()) continue;
      UnitView* uv = (UnitView*)children.FastEl(ui++);
      unit_so = uv->node_so();
      if(!unit_so) continue; // shouldn't happen
      nv->GetUnitDisplayVals(this, coord, val, col, sc_val);
      // Value
      unit_so->setAppearance(sc_val, col, max_z, trans);
      //TODO: we maybe shouldn't alter picked here, because that is expensive -- do only when select changes
      // except that might be complicated, ex. when Render() called,
      unit_so->setPicked(uv->picked);

      // Text (if any)
      SoAsciiText* at = unit_so->captionNode(false);
      if (nv->unit_text_disp == NetView::UTD_NONE) {
        if (at)
          at->string.setValue( "");
      }
      else { // text of some kind
        unit_so->transformCaption(font_xform);
        if (nv->unit_text_disp & NetView::UTD_VALUES) {
          ValToDispText(val, val_str);
        }
        if (!at) // force captionNode creation
          at = unit_so->captionNode(true);
        SoMFString* mfs = &(at->string);
        // fastest is to do each case in one operation
        switch (nv->unit_text_disp) {
        case NetView::UTD_VALUES:
          mfs->setValue(val_str.chars());
          break;
        case NetView::UTD_NAMES:
          mfs->setValue(unit->name.chars());
          break;
        case NetView::UTD_BOTH:
          const char* strs[2];
          strs[0] = val_str.chars();
          strs[1] = unit->name.chars();
          mfs->setValues(0, 2, strs);
          break;
        default: break; // compiler food, won't happen
        }
      }
    }
  }
}

// Set up vertices, normals, and coordinate indices for a collection of
// triangle strips that represent 5 sides of each unit block.  The heights
// of all units are set to defaults in this function and later updated by
// UpdateUnitValues_blocks() based on the value of each unit.
void UnitGroupView::Render_impl_blocks() {
  NetView* nv = getNetView(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  T3UnitGroupNode* node_so = this->node_so(); // cache
  SoIndexedTriangleStripSet* sits = node_so->shape();
  SoVertexProperty* vtx_prop = node_so->vtxProp();
  if(!sits || !vtx_prop) return; // something wrong..

  SoMFVec3f& vertex = vtx_prop->vertex;
  SoMFVec3f& normal = vtx_prop->normal;
  SoMFUInt32& color = vtx_prop->orderedRGBA;

  vtx_prop->normalBinding.setValue(SoNormalBinding::PER_FACE_INDEXED);
  vtx_prop->materialBinding.setValue(SoMaterialBinding::PER_PART_INDEXED);

  normal.setNum(5);
  SbVec3f* normal_dat = normal.startEditing();
  int idx=0;
  normal_dat[idx++].setValue(0.0f, 0.0f, -1.0f); // back = 0
  normal_dat[idx++].setValue(1.0f, 0.0f, 0.0f); // right = 1
  normal_dat[idx++].setValue(-1.0f, 0.0f, 0.0f); // left = 2
  normal_dat[idx++].setValue(0.0f, 0.0f, 1.0f); // front = 3
  normal_dat[idx++].setValue(0.0f, 1.0f, 0.0f); // top = 4
  normal.finishEditing();

  const float disp_scale = lay->disp_scale;

  int n_geom = lay->flat_geom.Product();
  int n_per_vtx = 8;
  int tot_vtx =  n_geom * n_per_vtx;
  vertex.setNum(tot_vtx);
  color.setNum(n_geom);

  float spacing = nv->view_params.unit_spacing;
  float max_z = MIN(nv->eff_max_size.x, nv->eff_max_size.y); // smallest XY
  max_z = MAX(max_z, nv->eff_max_size.z); // make sure Z isn't bigger
  float max_xy = MAX(nv->eff_max_size.x, nv->eff_max_size.y);

  if(nv->snap_bord_disp) {
    spacing += max_xy * 0.0005f * nv->snap_bord_width; // todo: play with this constant
    if(spacing > 0.25f) spacing = 0.25f;
  }

  if(nv->render_svg) {
    nv->svg_str << taSvg::Group();
  }

  bool build_text = false;
  SoSeparator* un_txt = NULL;
  float ufontsz = nv->font_sizes.unit;
  if(nv->unit_text_disp == NetView::UTD_NONE) {
    un_txt = node_so->unitText();
    if(un_txt) {
      node_so->removeUnitText();
    }
  }
  else {
    un_txt = node_so->unitText();
    if(!un_txt) {
      un_txt = node_so->getUnitText();
      build_text = true;
      node_so->addChild(un_txt);
      T3Panel* fr = GetFrame();
      iColor txtcolr = fr->GetTextColor();
      SoBaseColor* bc = new SoBaseColor;
      bc->rgb.setValue(txtcolr.redf(), txtcolr.greenf(), txtcolr.bluef());
      un_txt->addChild(bc);
      // doesn't seem to make much diff:
      SoComplexity* cplx = new SoComplexity;
      cplx->value.setValue(taMisc::text_complexity);
      un_txt->addChild(cplx);
      SoFont* fnt = new SoFont();
      fnt->name = (const char*)taMisc::t3d_font_name;
      un_txt->addChild(fnt);
    }
    SoFont* fnt = (SoFont*)un_txt->getChild(2);
    fnt->size.setValue(ufontsz);
  }

  // SVG render stuff:
  taVector3i svg_pos;
  taVector3f svg_posn;
  T3Color col;
  float trans = nv->view_params.unit_trans;
  if(nv->render_svg) {
    lay->GetAbsPos(svg_pos);
    svg_posn = nv->LayerPosToCoin3D(svg_pos);
  }

  SbVec3f* vertex_dat = vertex.startEditing();

  String val_str = "0.0";       // initial default
  String unit_name;
  taVector2i pos;
  taVector2i upos;
  int v_idx = 0;
  int t_idx = 3;                // base color + complexity + font
  // these go in normal order; indexes are backwards
  for(pos.y=0; pos.y<lay->flat_geom.y; pos.y++) {
    for(pos.x=0; pos.x<lay->flat_geom.x; pos.x++) { // right to left
      Unit* unit = lay->UnitAtCoord(pos);
      if(unit)
        lay->UnitDispPos(unit, upos);
      float xp = disp_scale * (((float)upos.x + spacing) / nv->eff_max_size.x);
      float yp = -disp_scale * (((float)upos.y + spacing) / nv->eff_max_size.y);
      float xp1 = disp_scale * (((float)upos.x+1 - spacing) / nv->eff_max_size.x);
      float yp1 = -disp_scale * (((float)upos.y+1 - spacing) / nv->eff_max_size.y);
      float zp = .5f / max_z;

      vertex_dat[v_idx++].setValue(xp, 0.0f, yp); // 00_0 = 0
      vertex_dat[v_idx++].setValue(xp1, 0.0f, yp); // 10_0 = 0
      vertex_dat[v_idx++].setValue(xp, 0.0f, yp1); // 01_0 = 0
      vertex_dat[v_idx++].setValue(xp1, 0.0f, yp1); // 11_0 = 0

      // zp will be updated later!
      vertex_dat[v_idx++].setValue(xp, zp, yp); // 00_v = 1
      vertex_dat[v_idx++].setValue(xp1, zp, yp); // 10_v = 2
      vertex_dat[v_idx++].setValue(xp, zp, yp1); // 01_v = 3
      vertex_dat[v_idx++].setValue(xp1, zp, yp1); // 11_v = 4

      if(nv->unit_text_disp != NetView::UTD_NONE) {
        if(build_text || un_txt->getNumChildren() <= t_idx) {
          SoSeparator* tsep = new SoSeparator;
          un_txt->addChild(tsep);
          SoTranslation* tr = new SoTranslation;
          tsep->addChild(tr);
          SoAsciiText* txt = new SoAsciiText();
          txt->justification = SoAsciiText::CENTER;
          tsep->addChild(txt);
        }
        // todo: could figure out how to do this without a separator
        // but it just isn't clear that it is that big a deal..  very convenient
        SoSeparator* tsep = (SoSeparator*)un_txt->getChild(t_idx);
        SoTranslation* tr = (SoTranslation*)tsep->getChild(0);
        float xfp = .5f * (xp + xp1);
        tr->translation.setValue(xfp, MAX(zp,0.0f) + .01f, yp);
        SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
//      if(nv->unit_text_disp & NetView::UTD_VALUES) {
          //      ValToDispText(0.0f, val_str); // just use default
//      }
        if(nv->unit_text_disp & NetView::UTD_NAMES) {
          if(unit)
            unit_name = unit->name;
        }
        SoMFString* mfs = &(txt->string);
        // fastest is to do each case in one operation
        switch (nv->unit_text_disp) {
        case NetView::UTD_VALUES:
          mfs->setValue(val_str.chars());
          break;
        case NetView::UTD_NAMES:
          mfs->setValue(unit_name.chars());
          break;
        case NetView::UTD_BOTH:
          const char* strs[2];
          strs[0] = val_str.chars();
          strs[1] = unit_name.chars();
          mfs->setValues(0, 2, strs);
          break;
        default: break; // compiler food, won't happen
        }
        t_idx++;
      }
    }
  }
  vertex.finishEditing();

  // todo: could cleanup un_txt child list if extras, but not clear if needed

  SoMFInt32& coords = sits->coordIndex;
  SoMFInt32& norms = sits->normalIndex;
  SoMFInt32& mats = sits->materialIndex;
  int nc_per_idx = 19;          // number of coords per index
  int nn_per_idx = 10;          // number of norms per index
  int nm_per_idx = 3;           // number of mats per index
  coords.setNum(n_geom * nc_per_idx);
  norms.setNum(n_geom * nn_per_idx);
  mats.setNum(n_geom * nm_per_idx);

  int nx = lay->flat_geom.x;

  // values of the cubes xy_[0,v]
  //     01_v   11_v
  //   01_0   11_0
  //     00_v   10_v
  //   00_0   10_0

  // triangle strip order is 0 1 2, 2 1 3, 2 3 4

  int32_t* coords_dat = coords.startEditing();
  int32_t* norms_dat = norms.startEditing();
  int32_t* mats_dat = mats.startEditing();
  int cidx = 0;
  int nidx = 0;
  int midx = 0;
  for(pos.y=lay->flat_geom.y-1; pos.y>=0; pos.y--) { // go back to front
    for(pos.x=0; pos.x<lay->flat_geom.x; pos.x++) { // right to left
      int c00_0 = (pos.y * nx + pos.x) * n_per_vtx;
      int c10_0 = c00_0 + 1;
      int c01_0 = c00_0 + 2;
      int c11_0 = c00_0 + 3;
      int c00_v = c00_0 + 4;
      int c10_v = c00_0 + 5;
      int c01_v = c00_0 + 6;
      int c11_v = c00_0 + 7;

      int mat_idx = (pos.y * nx + pos.x);

      // back - right     think this:     not this:
      //     1    3          x----x         x----x
      //   0    2          x | top|       x----x |
      //     x    5        | x----x       |frnt| x
      //   x    4          x----x         x----x

      coords_dat[cidx++] = (c01_0); // 0
      coords_dat[cidx++] = (c01_v); // 1
      coords_dat[cidx++] = (c11_0); // 2
      coords_dat[cidx++] = (c11_v); // 3
      coords_dat[cidx++] = (c10_0); // 4
      coords_dat[cidx++] = (c10_v); // 5
      coords_dat[cidx++] = (-1); // -1  -- 7 total

      norms_dat[nidx++] = (0); // back
      norms_dat[nidx++] = (0); // back
      norms_dat[nidx++] = (1); // right
      norms_dat[nidx++] = (1); // right -- 4 total

      mats_dat[midx++] = (mat_idx);

      // left - front
      //     1    x
      //   0    x
      //     3    5
      //   2    4

      coords_dat[cidx++] = (c01_0); // 0
      coords_dat[cidx++] = (c01_v); // 1
      coords_dat[cidx++] = (c00_0); // 2
      coords_dat[cidx++] = (c00_v); // 3
      coords_dat[cidx++] = (c10_0); // 4
      coords_dat[cidx++] = (c10_v); // 5
      coords_dat[cidx++] = (-1); // -1 -- 7 total

      norms_dat[nidx++] = (2); // left
      norms_dat[nidx++] = (2); // left
      norms_dat[nidx++] = (3); // front
      norms_dat[nidx++] = (3); // front -- 4 total

      mats_dat[midx++] = (mat_idx);

      // triangle strip order is 0 1 2, 2 1 3, 2 3 4
      // top
      //     0    1
      //   x    x
      //     2    3
      //   x    x

      coords_dat[cidx++] = (c01_v); // 0
      coords_dat[cidx++] = (c11_v); // 1
      coords_dat[cidx++] = (c00_v); // 2
      coords_dat[cidx++] = (c10_v); // 3
      coords_dat[cidx++] = (-1); // -1 -- 5 total

      norms_dat[nidx++] = (4); // top
      norms_dat[nidx++] = (4); // top -- 2 total

      mats_dat[midx++] = (mat_idx);

      // total coords = 7 + 7 + 5 = 19
      // total norms = 4 + 4 + 2 = 10
      // total mats = 3

      if(nv->render_svg) {
        // this has to be here because of the backward ordering
        Unit* unit = lay->UnitAtCoord(pos);
        if(unit)
          lay->UnitDispPos(unit, upos);
        float xp = disp_scale * (((float)upos.x + spacing) / nv->eff_max_size.x);
        float yp = -disp_scale * (((float)upos.y + spacing) / nv->eff_max_size.y);
        float xp1 = disp_scale * (((float)upos.x+1 - spacing) / nv->eff_max_size.x);
        float yp1 = -disp_scale * (((float)upos.y+1 - spacing) / nv->eff_max_size.y);

        float val;
        float sc_val;
        // this is from UpdateUnitValues_blocks()
        nv->GetUnitDisplayVals(this, pos, val, col, sc_val);
        if(nv->unit_con_md && (unit == nv->unit_src.ptr())) {
          col.r = 0.0f; col.g = 1.0f; col.b = 0.0f;
        }
        if(unit && unit->lesioned()) {
          sc_val = 0.0f;
        }
        float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
        if(unit && unit->lesioned()) {
          alpha = 0.0f;
        }

        iColor icol = (iColor)col;
        icol.a = iColor::fc2ic(alpha);

        float zp = .5f * sc_val / max_z;

        nv->svg_str
          << taSvg::Path(icol, 1.0f, true, icol) // fill
          // left side wall: note: the order matters for self-intersection
          << "M " << taSvg::Coords(svg_posn.x + xp, svg_posn.y + 0.0f, svg_posn.z + yp)
          << "L " << taSvg::Coords(svg_posn.x + xp, svg_posn.y + 0.0f, svg_posn.z + yp1)
          << "L " << taSvg::Coords(svg_posn.x + xp, svg_posn.y + zp, svg_posn.z + yp1)
          << "L " << taSvg::Coords(svg_posn.x + xp, svg_posn.y + zp, svg_posn.z + yp)
          // right side wall:
          << "M " << taSvg::Coords(svg_posn.x + xp1, svg_posn.y + 0.0f, svg_posn.z + yp)
          << "L " << taSvg::Coords(svg_posn.x + xp1, svg_posn.y + zp, svg_posn.z + yp)
          << "L " << taSvg::Coords(svg_posn.x + xp1, svg_posn.y + zp, svg_posn.z + yp1)
          << "L " << taSvg::Coords(svg_posn.x + xp1, svg_posn.y + 0.0f, svg_posn.z + yp1)
          // front wall:
          << "M " << taSvg::Coords(svg_posn.x + xp, svg_posn.y + 0.0f, svg_posn.z + yp)
          << "L " << taSvg::Coords(svg_posn.x + xp1, svg_posn.y + 0.0f, svg_posn.z + yp)
          << "L " << taSvg::Coords(svg_posn.x + xp1, svg_posn.y + zp, svg_posn.z + yp)
          << "L " << taSvg::Coords(svg_posn.x + xp, svg_posn.y + zp, svg_posn.z + yp)
          // top wall:
          << "M " << taSvg::Coords(svg_posn.x + xp, svg_posn.y + zp, svg_posn.z + yp)
          << "L " << taSvg::Coords(svg_posn.x + xp1, svg_posn.y + zp, svg_posn.z + yp)
          << "L " << taSvg::Coords(svg_posn.x + xp1, svg_posn.y + zp, svg_posn.z + yp1)
          << "L " << taSvg::Coords(svg_posn.x + xp, svg_posn.y + zp, svg_posn.z + yp1)
          << taSvg::PathEnd();
      }
    }
  }
  coords.finishEditing();
  norms.finishEditing();
  mats.finishEditing();

  UpdateUnitValues_blocks(); // hand off to next guy to adjust block heights

  if(nv->render_svg) {
    nv->svg_str << taSvg::GroupEnd();
  }
}

// Update the height of each unit block based on the unit's value as
// returned by nv->GetUnitDisplayVals().  The blocks have already been
// drawn and positioned by Render_impl_blocks(), so this should be fast.
void UnitGroupView::UpdateUnitValues_blocks() {
  NetView* nv = getNetView(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  T3UnitGroupNode* node_so = this->node_so(); // cache
  if(!node_so) return;
  SoIndexedTriangleStripSet* sits = node_so->shape();
  SoVertexProperty* vtx_prop = node_so->vtxProp();
  if(!sits || !vtx_prop) return; // something wrong..

  SoMFVec3f& vertex = vtx_prop->vertex;
  SoMFUInt32& color = vtx_prop->orderedRGBA;

  SbVec3f* vertex_dat = vertex.startEditing();
  uint32_t* color_dat = color.startEditing();

  float trans = nv->view_params.unit_trans;
  float max_z = MIN(nv->eff_max_size.x, nv->eff_max_size.y); // smallest XY
  max_z = MAX(max_z, nv->eff_max_size.z); // make sure Z isn't bigger

  SoSeparator* un_txt = node_so->unitText();

  String val_str;
  String unit_name;
  float val;
  float sc_val;
  T3Color col;
  taVector2i pos;
  int v_idx = 0;
  int c_idx = 0;
  int t_idx = 3;                // base color + font
  // these go in normal order; indexes are backwards
  for(pos.y=0; pos.y<lay->flat_geom.y; pos.y++) {
    for(pos.x=0; pos.x<lay->flat_geom.x; pos.x++) { // right to left
      nv->GetUnitDisplayVals(this, pos, val, col, sc_val);
      Unit* unit = lay->UnitAtCoord(pos);
      if(nv->unit_con_md && (unit == nv->unit_src.ptr())) {
        col.r = 0.0f; col.g = 1.0f; col.b = 0.0f;
      }
      if(unit && unit->lesioned()) {
        sc_val = 0.0f;
      }

      float zp = .5f * sc_val / max_z;
      v_idx+=4;                 // skip the _0 cases

      vertex_dat[v_idx++][1] = zp; // 00_v = 1
      vertex_dat[v_idx++][1] = zp; // 10_v = 2
      vertex_dat[v_idx++][1] = zp; // 01_v = 3
      vertex_dat[v_idx++][1] = zp; // 11_v = 4

      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      if(unit && unit->lesioned()) {
        alpha = 0.0f;
      }
      color_dat[c_idx++] = T3Color::makePackedRGBA(col.r, col.g, col.b, alpha);

      if(nv->unit_text_disp & NetView::UTD_VALUES) {
        SoSeparator* tsep = (SoSeparator*)un_txt->getChild(t_idx);
        SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
        SoMFString* mfs = &(txt->string);
        ValToDispText(val, val_str);
        switch (nv->unit_text_disp) {
        case NetView::UTD_VALUES:
          mfs->setValue(val_str.chars());
          break;
        case NetView::UTD_NAMES:
          break;
        case NetView::UTD_BOTH:
          mfs->set1Value(0, val_str.chars());
          break;
        default: break; // compiler food, won't happen
        }
        t_idx++;
      }
    }
  }
  vertex.finishEditing();
  color.finishEditing();
}

void UnitGroupView::UpdateUnitValues() {
  NetView* nv = getNetView(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  if(lay->Iconified() || !lv() || lv()->disp_mode == LayerView::DISP_FRAME) {
    return;                     // don't render anything!
  }
  if(nv->hist_save)
    SaveHist();
  if(nv->snap_bord_disp)
    UpdateUnitValues_snap_bord();
  if(lv()->disp_mode == LayerView::DISP_OUTPUT_NAME) {
    UpdateUnitValues_outnm();
    return;
  }
  if(nv->unit_disp_mode == NetView::UDM_BLOCK) {
    UpdateUnitValues_blocks();
    return;
  }
  Render_impl_children();
}

void UnitGroupView::SaveHist() {
  NetView* nv = getNetView(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay) return;

  // bump up the frame circ idx..
  int circ_idx = uvd_hist_idx.CircAddLimit(nv->hist_max);
  int eff_hist_idx = uvd_hist_idx.CircIdx(circ_idx);

  taVector2i coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      for(int midx=0; midx < nv->membs.size; midx++) {
        void* base;
        float val = GetUnitDisplayVal_Idx(coord, midx, base);
        uvd_hist.Set(val, coord.x, coord.y, midx, eff_hist_idx);
      }
    }
  }
}

void UnitGroupView::Reset_impl() {
  inherited::Reset_impl();
  uvd_bases.Reset();
  uvd_hist.Reset();
  uvd_hist_idx.Reset();
}

void UnitGroupView::Render_impl_outnm() {
  // this function just does all the memory allocation and static configuration
  // that doesn't depend on unit values, then it calls UpdateUnitValues
  // which sets all the values!

  NetView* nv = getNetView(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  T3UnitGroupNode* node_so = this->node_so(); // cache

  SoSeparator* un_txt = node_so->unitText();
  if(!un_txt) {
    un_txt = node_so->getUnitText();
    node_so->addChild(un_txt);
    T3Panel* fr = GetFrame();
    iColor txtcolr = fr->GetTextColor();
    SoBaseColor* bc = new SoBaseColor;
    bc->rgb.setValue(txtcolr.redf(), txtcolr.greenf(), txtcolr.bluef());
    un_txt->addChild(bc);
    // doesn't seem to make much diff:
    SoComplexity* cplx = new SoComplexity;
    cplx->value.setValue(taMisc::text_complexity);
    un_txt->addChild(cplx);
    SoFont* fnt = new SoFont();
    fnt->name = (const char*)taMisc::t3d_font_name;
    un_txt->addChild(fnt);
    SoTranslation* tr = new SoTranslation;
    un_txt->addChild(tr);
    SoAsciiText* txt = new SoAsciiText();
    txt->justification = SoAsciiText::CENTER;
    un_txt->addChild(txt);
    SoMFString* mfs = &(txt->string);
    mfs->setValue(lay->output_name.chars());
  }

  float szx = (float)lay->flat_geom.x / nv->eff_max_size.x;
  float szy = (float)lay->flat_geom.y / nv->eff_max_size.y;

  float cx = .5f * szx;
  float cy = .5f * szy;

  SoTranslation* tr = (SoTranslation*)un_txt->getChild(3);
  tr->translation.setValue(cx, 0.0f, -cy);

  szx = 1.5f * szx / (float)nv->font_sizes.un_nm_len;

  float ufontsz = MIN(szx, szy);

  SoFont* fnt = (SoFont*)un_txt->getChild(2);
  fnt->size.setValue(ufontsz);

  UpdateUnitValues_outnm();             // hand off to next guy..
}

void UnitGroupView::UpdateUnitValues_outnm() {
  //  NetView* nv = getNetView(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  T3UnitGroupNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  SoSeparator* un_txt = node_so->unitText();

  SoAsciiText* txt = (SoAsciiText*)un_txt->getChild(4);
  SoMFString* mfs = &(txt->string);
  // todo: need a string array in layer!
//   if(lay->unit_groups)
//     mfs->setValue(ugrp->output_name.chars());
//   else
    mfs->setValue(lay->output_name.chars());
}

void UnitGroupView::Render_impl_snap_bord() {
  NetView* nv = getNetView(); //cache
  T3UnitGroupNode* node_so = this->node_so(); //cache
  if(!node_so) return;

  Layer* lay = this->layer(); //cache
  if(!lay) return;

  bool do_lines = nv->snap_bord_disp;
  SoIndexedLineSet* ils = node_so->snapBordSet();
  SoDrawStyle* drw = node_so->snapBordDraw();
  SoVertexProperty* vtx_prop = node_so->snapBordVtxProp();

  SoMFVec3f& vertex = vtx_prop->vertex;
  SoMFUInt32& color = vtx_prop->orderedRGBA;
  SoMFInt32& coords = ils->coordIndex;
  SoMFInt32& mats = ils->materialIndex;

  if(!do_lines) {
    vertex.setNum(0);
    color.setNum(0);
    coords.setNum(0);
    mats.setNum(0);
    return;
  }

  drw->style = SoDrawStyleElement::LINES;
  drw->lineWidth = nv->snap_bord_width;
  // FACE = polyline, PART = line segment!!
  vtx_prop->materialBinding.setValue(SoMaterialBinding::PER_FACE_INDEXED);

  float disp_scale = lay->disp_scale;

  int n_geom = lay->flat_geom.Product();

  vertex.setNum(n_geom * 4);    // 4 virtex per
  coords.setNum(n_geom * 6);    // 6 coords per
  color.setNum(n_geom);
  mats.setNum(n_geom);

  SbVec3f* vertex_dat = vertex.startEditing();
  int32_t* coords_dat = coords.startEditing();
  int32_t* mats_dat = mats.startEditing();
  int v_idx = 0;
  int cidx = 0;
  int midx = 0;

  float max_xy = MAX(nv->eff_max_size.x, nv->eff_max_size.y);
  float max_z = MAX(max_xy, nv->eff_max_size.z); // make sure Z isn't bigger

  float spacing = 0.5f * max_xy * 0.0005f * nv->snap_bord_width; // todo: play with this constant
  if(spacing > .1f) spacing = .1f;
  float zp = (spacing * 2.0f) / max_z;

  taVector2i pos;
  taVector2i upos;
  for(pos.y=0; pos.y<lay->flat_geom.y; pos.y++) {
    for(pos.x=0; pos.x<lay->flat_geom.x; pos.x++) { // right to left
      Unit* unit = lay->UnitAtCoord(pos);
      if(unit)
        lay->UnitDispPos(unit, upos);
      float xp = disp_scale * (((float)upos.x + spacing) / nv->eff_max_size.x);
      float yp = -disp_scale * (((float)upos.y + spacing) / nv->eff_max_size.y);
      float xp1 = disp_scale * (((float)upos.x+1 - spacing) / nv->eff_max_size.x);
      float yp1 = -disp_scale * (((float)upos.y+1 - spacing) / nv->eff_max_size.y);

      coords_dat[cidx++] = v_idx; coords_dat[cidx++] = v_idx+1;
      coords_dat[cidx++] = v_idx+2; coords_dat[cidx++] = v_idx+3;
      coords_dat[cidx++] = v_idx; coords_dat[cidx++] = -1;

      vertex_dat[v_idx++].setValue(xp, zp, yp); // 00_0 = 0
      vertex_dat[v_idx++].setValue(xp1, zp, yp); // 10_0 = 0
      vertex_dat[v_idx++].setValue(xp1, zp, yp1); // 11_0 = 0
      vertex_dat[v_idx++].setValue(xp, zp, yp1); // 01_0 = 0

      mats_dat[midx] = midx;  midx++;
    }
  }

  vertex.finishEditing();
  coords.finishEditing();
  mats.finishEditing();

  UpdateUnitValues_snap_bord();         // hand off to next guy..
}

void UnitGroupView::UpdateUnitValues_snap_bord() {
  NetView* nv = getNetView(); //cache
  if(!nv->snap_bord_disp) return;

  Layer* lay = this->layer(); //cache
  if(!lay) return;
  T3UnitGroupNode* node_so = this->node_so(); // cache

  SoVertexProperty* vtx_prop = node_so->snapBordVtxProp();
  SoMFUInt32& color = vtx_prop->orderedRGBA;

  uint32_t* color_dat = color.startEditing();
  if(!color_dat) return;

  int cur_disp_idx = nv->unit_disp_idx;

  MemberDef* md = (MemberDef*)nv->membs.FindName("snap");
  if(!md) return;               // shouldn't happen
  nv->unit_disp_idx = nv->membs.FindEl(md);

  float trans = nv->view_params.unit_trans;
  float val;
  float sc_val;
  iColor tc;
  T3Color col;
  taVector2i pos;
  int c_idx = 0;
  for(pos.y=0; pos.y<lay->flat_geom.y; pos.y++) {
    for(pos.x=0; pos.x<lay->flat_geom.x; pos.x++) { // right to left
      val = 0.0f;
      Unit* unit = lay->UnitAtCoord(pos);
      if(unit) {
        void* base;
        val = GetUnitDisplayVal(pos, base);
      }
      nv->GetUnitColor(val, tc, sc_val);
      col.setValue(tc.redf(), tc.greenf(), tc.bluef());

      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      color_dat[c_idx++] = T3Color::makePackedRGBA(col.r, col.g, col.b, alpha);
    }
  }

  nv->unit_disp_idx = cur_disp_idx; // restore!

  color.finishEditing();
}
