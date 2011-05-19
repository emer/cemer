// Copyright, 1995-2011, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "BrainVolumeView.h"
#include "BrainView.h"
#include "T3BrainNode.h"

#include <Inventor/SoEventManager.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoTranslation.h>

Layer*
BrainVolumeView::layer() const {
  return (Layer*) data();
}

T3BrainNode*
BrainVolumeView::node_so() const {
  return (T3BrainNode*) inherited::node_so();
}

BrainView*
BrainVolumeView::bv() {
  if (!m_bv)
    m_bv = GET_MY_OWNER(BrainView);
  return m_bv;
}

void BrainVolumeView::Initialize() {
  data_base = &TA_Network;
  m_bv = NULL;
}

void BrainVolumeView::InitLinks() {
  inherited::InitLinks();
}

void BrainVolumeView::CutLinks() {
  m_bv = NULL;
  inherited::CutLinks();
}

void BrainVolumeView::Destroy() {
  Reset();
}

void BrainVolumeView::AllocUnitViewData() {
  //note: allocate based on geom, not current size, in case not built yet
  BrainView* bv = this->bv();
  Layer* lay = this->layer(); //cache
  if (!lay) return;
  int mbs_sz = MAX(bv->membs.size, 1);
  MatrixGeom nwgm1(3, lay->flat_geom.x, lay->flat_geom.y, mbs_sz);
  if (uvd_bases.geom != nwgm1) {
    uvd_bases.SetGeomN(nwgm1);
  }
}

void BrainVolumeView::BuildAll() {
  Reset(); // in case where we are rebuilding
  BrainView* bv = this->bv();
  UpdateUnitViewBases(bv->unit_src);
  if(bv->unit_disp_mode == BrainView::UDM_BLOCK) return; // optimized

  Layer* lay = this->layer(); //cache
  if(!lay) return;
  TwoDCoord coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      if (!unit) continue;

      UnitView* uv = new UnitView();
      uv->SetData(unit);
      children.Add(uv);
    }
  }
}

void BrainVolumeView::InitDisplay() {
  BrainView* bv = this->bv();
  UpdateUnitViewBases(bv->unit_src);
}

float BrainVolumeView::GetUnitDisplayVal(const TwoDCoord& co, void*& base) {
  BrainView* bv = this->bv();
  float val = bv->scale.zero;
  if(bv->unit_disp_idx < 0) return val;
  base = uvd_bases.SafeEl(co.x, co.y, bv->unit_disp_idx);
  if(!base) return val;

  switch (bv->unit_md_flags) {
    case BrainView::MD_FLOAT:
      val = *((float*)base); break;
    case BrainView::MD_DOUBLE:
      val = *((double*)base); break;
    case BrainView::MD_INT:
      val = *((int*)base); break;
    default:
      val = 0.0f; break;
  }
  return val;
}

float BrainVolumeView::GetUnitDisplayVal_Idx(const TwoDCoord& co, int midx, void*& base) {
  BrainView* bv = this->bv();
  float val = bv->scale.zero;
  base = uvd_bases.SafeEl(co.x, co.y, midx);
  MemberDef* md = bv->membs.SafeEl(midx);
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

void BrainVolumeView::UpdateUnitViewBases(Unit* src_u) {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  BrainView* bv = this->bv();
  AllocUnitViewData();
  for(int midx=0;midx<bv->membs.size;midx++) {
    MemberDef* disp_md = bv->membs[midx];
    String nm = disp_md->name.before(".");
    if(nm.empty()) { // direct unit member
      UpdateUnitViewBase_Unit_impl(midx, disp_md);
    } else if ((nm=="s") || (nm == "r")) {
      UpdateUnitViewBase_Con_impl(midx, (nm=="s"), disp_md->name.after('.'), src_u);
    } else if (nm=="bias") {
      UpdateUnitViewBase_Bias_impl(midx, disp_md);
    } else { // sub-member of unit
      UpdateUnitViewBase_Sub_impl(midx, disp_md);
    }
  }
}

void BrainVolumeView::UpdateUnitViewBase_Con_impl(int midx, bool is_send, String nm, Unit* src_u) {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  TwoDCoord coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      uvd_bases.Set(NULL, coord.x, coord.y, midx);
      if (!unit) continue;  // rest will be null too, but we loop to null disp_base

      if (is_send) {
        for(int g=0;g<unit->recv.size;g++) {
          RecvCons* tcong = unit->recv.FastEl(g);
          MemberDef* act_md = tcong->con_type->members.FindName(nm);
          if (!act_md)  continue;
          Connection* con = tcong->FindConFrom(src_u);
          if (!con) continue;
          uvd_bases.Set(act_md->GetOff(con), coord.x, coord.y, midx);
          break;                // once you've got one, done!
        }
      }
      else {
        for(int g=0;g<unit->send.size;g++) {
          SendCons* tcong = unit->send.FastEl(g);
          MemberDef* act_md = tcong->con_type->members.FindName(nm);
          if (!act_md)  continue;
          Connection* con = tcong->FindConFrom(src_u);
          if (!con) continue;
          uvd_bases.Set(act_md->GetOff(con), coord.x, coord.y, midx);
          break;                // once you've got one, done!
        }
      }
    }
  }
}

void BrainVolumeView::UpdateUnitViewBase_Bias_impl(int midx, MemberDef* disp_md) {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  TwoDCoord coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      uvd_bases.Set(NULL, coord.x, coord.y, midx);
      if (!unit) continue;  // rest will be null too, but we loop to null disp_base
      if(unit->bias.size == 0) continue;
      Connection* con = unit->bias.Cn(0);
      uvd_bases.Set(disp_md->GetOff(con), coord.x, coord.y, midx);
    }
  }
}

void BrainVolumeView::UpdateUnitViewBase_Unit_impl(int midx, MemberDef* disp_md) {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  TwoDCoord coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      uvd_bases.Set(NULL, coord.x, coord.y, midx);
      if (!unit) continue;  // rest will be null too, but we loop to null disp_base
      uvd_bases.Set(disp_md->GetOff(unit), coord.x, coord.y, midx);
    }
  }
}

void BrainVolumeView::UpdateUnitViewBase_Sub_impl(int midx, MemberDef* disp_md) {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  TypeDef* own_td = lay->units.el_typ; // should be unit type
  ta_memb_ptr net_mbr_off = 0;
  int net_base_off = 0;
  MemberDef* smd = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off,
                                                 disp_md->name, false); // no warn
  TwoDCoord coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      uvd_bases.Set(NULL, coord.x, coord.y, midx);
      if(!unit || !smd) continue;  // rest will be null too, but we loop to null disp_base
      void* sbaddr = MemberDef::GetOff_static(unit, net_base_off, net_mbr_off);
      uvd_bases.Set(sbaddr, coord.x, coord.y, midx);
    }
  }
}

void BrainVolumeView::UpdateAutoScale(bool& updated) {
  BrainView* bv = this->bv();
  TwoDCoord co;
  void* base;
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  for (co.y = 0; co.y < lay->flat_geom.y; ++co.y) {
    for (co.x = 0; co.x < lay->flat_geom.x; ++co.x) {
      float val = GetUnitDisplayVal(co, base);
      if(base) {
        if(!updated) {
          bv->scale.SetMinMax(val, val);
          updated = true;
        }
        else
          bv->scale.UpdateMinMax(val);
      }
    }
  }
}

void BrainVolumeView::Render_pre() {
  BrainView* bv = this->bv();

  bool no_units = true;
  if(bv->unit_disp_mode != BrainView::UDM_BLOCK)
    no_units = false;

  setNode(new T3BrainNode(this, no_units));
  //NOTE: we create/adjust the units in the Render_impl routine
  T3BrainNode* ugrp_so = node_so(); // cache

//  ugrp_so->setGeom(lay->disp_geom.x, lay->disp_geom.y, bv->max_size.x,
//                   bv->max_size.y, bv->max_size.z, lay->disp_scale);

  inherited::Render_pre();
}

void BrainVolumeView::Render_impl() {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  BrainView* bv = this->bv();
  inherited::Render_impl();
}

void BrainVolumeView::ValToDispText(float val, String& str) {
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

void BrainVolumeView::DoActionChildren_impl(DataViewAction acts) {
  if (acts & RENDER_IMPL) {
    acts = (DataViewAction)( acts & ~RENDER_IMPL); // note: only supposed to be one, but don't assume
    Render_impl_children();
    if (!acts) return;
  } 
  inherited::DoActionChildren_impl(acts);
}

void BrainVolumeView::Render_impl_children() {
  BrainView* bv = this->bv(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay || !bv) return;

  if(lay->Iconified()) {
    return;                     // don't render anything!
  }

  T3BrainNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  UpdateUnitViewBases(bv->unit_src); // always make sure we're allocated properly

  if (false) {
    Render_impl_outnm();
    return;
  }

  Render_impl_snap_bord();

  if(bv->unit_disp_mode == BrainView::UDM_BLOCK) {
    Render_impl_blocks();
    return;
  }

  // for efficiency we assume responsibility for the _impl of UnitViews
  T3UnitNode* unit_so;
  TwoDCoord co;
  float val;
  float sc_val;
  T3Color col;
  String val_str;

  float max_z = MIN(bv->max_size.x, bv->max_size.y); // smallest XY
  max_z = MAX(max_z, bv->max_size.z); // make sure Z isn't bigger
  float trans = bv->view_params.unit_trans;
  float font_z = bv->font_sizes.unit*2.0f;
  float font_y = .45f / bv->max_size.y;
  SbVec3f font_xform(0.0f, font_z, font_y);

  int ui = 0;
  TwoDCoord coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      if(!unit) continue;
      UnitView* uv = (UnitView*)children.FastEl(ui++);
      unit_so = uv->node_so();
      if(!unit_so) continue; // shouldn't happen
      bv->GetUnitDisplayVals(this, coord, val, col, sc_val);
      // Value
      unit_so->setAppearance(sc_val, col, max_z, trans);
      //TODO: we maybe shouldn't alter picked here, because that is expensive -- do only when select changes
      // except that might be complicated, ex. when Render() called,
      unit_so->setPicked(uv->picked);

      // Text (if any)
      SoAsciiText* at = unit_so->captionNode(false);
      if (bv->unit_text_disp == BrainView::UTD_NONE) {
        if (at)
          at->string.setValue( "");
      } else { // text of some kind
        unit_so->transformCaption(font_xform);
        if (bv->unit_text_disp & BrainView::UTD_VALUES) {
          ValToDispText(val, val_str);
        }
        if (!at) // force captionNode creation
          at = unit_so->captionNode(true);
        SoMFString* mfs = &(at->string);
        // fastest is to do each case in one operation
        switch (bv->unit_text_disp) {
        case BrainView::UTD_VALUES:
          mfs->setValue(val_str.chars());
          break;
        case BrainView::UTD_NAMES:
          mfs->setValue(unit->name.chars());
          break;
        case BrainView::UTD_BOTH:
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

void BrainVolumeView::Render_impl_blocks() {
  // this function just does all the memory allocation and static configuration
  // that doesn't depend on unit values, then it calls UpdateUnitValues
  // which sets all the values!

  BrainView* bv = this->bv(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  T3BrainNode* node_so = this->node_so(); // cache
  SoIndexedFaceSet* ifs = node_so->shape();
  if (!ifs) return; // something wrong..

  SoMFVec3f vertex;
  SoMFVec3f normal;
  SoMFUInt32 color;

  normal.setNum(5);
  SbVec3f* normal_dat = normal.startEditing();
  int idx=0;
  normal_dat[idx++].setValue(0.0f, 0.0f, -1.0f); // back = 0
  normal_dat[idx++].setValue(1.0f, 0.0f, 0.0f); // right = 1
  normal_dat[idx++].setValue(-1.0f, 0.0f, 0.0f); // left = 2
  normal_dat[idx++].setValue(0.0f, 0.0f, 1.0f); // front = 3
  normal_dat[idx++].setValue(0.0f, 1.0f, 0.0f); // top = 4
  normal.finishEditing();

  float disp_scale = lay->disp_scale;

  int n_geom = lay->flat_geom.Product();
  int n_per_vtx = 8;
  int tot_vtx =  n_geom * n_per_vtx;
  vertex.setNum(tot_vtx);
  color.setNum(n_geom);

  float spacing = bv->view_params.unit_spacing;
  float max_z = MIN(bv->max_size.x, bv->max_size.y); // smallest XY
  max_z = MAX(max_z, bv->max_size.z); // make sure Z isn't bigger

  float max_xy = MAX(bv->max_size.x, bv->max_size.y);
  if(bv->snap_bord_disp) {
    spacing += max_xy * 0.0005f * bv->snap_bord_width; // todo: play with this constant
    if(spacing > 0.25f) spacing = 0.25f;
  }

  bool build_text = false;
  SoSeparator* un_txt = NULL;
  float ufontsz = bv->font_sizes.unit;
  un_txt = node_so->netText();
  SoFont* fnt = (SoFont*) un_txt->getChild(2);
  fnt->size.setValue(ufontsz);

  SbVec3f* vertex_dat = vertex.startEditing();

  String val_str = "0.0";       // initial default
  String unit_name;
  T3Color col;
  TwoDCoord pos;
  TwoDCoord upos;
  int v_idx = 0;
  int t_idx = 3;                // base color + complexity + font
  // these go in normal order; indexes are backwards
  for(pos.y=0; pos.y<lay->flat_geom.y; pos.y++) {
    for(pos.x=0; pos.x<lay->flat_geom.x; pos.x++) { // right to left
      Unit* unit = lay->UnitAtCoord(pos);
      if(unit)
        lay->UnitDispPos(unit, upos);
      float xp = disp_scale * (((float)upos.x + spacing) / bv->max_size.x);
      float yp = -disp_scale * (((float)upos.y + spacing) / bv->max_size.y);
      float xp1 = disp_scale * (((float)upos.x+1 - spacing) / bv->max_size.x);
      float yp1 = -disp_scale * (((float)upos.y+1 - spacing) / bv->max_size.y);
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

      if(bv->unit_text_disp != BrainView::UTD_NONE) {
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
//      if(bv->unit_text_disp & BrainView::UTD_VALUES) {
          //      ValToDispText(0.0f, val_str); // just use default
//      }
        if(bv->unit_text_disp & BrainView::UTD_NAMES) {
          if(unit)
            unit_name = unit->name;
        }
        SoMFString* mfs = &(txt->string);
        // fastest is to do each case in one operation
        switch (bv->unit_text_disp) {
        case BrainView::UTD_VALUES:
          mfs->setValue(val_str.chars());
          break;
        case BrainView::UTD_NAMES:
          mfs->setValue(unit_name.chars());
          break;
        case BrainView::UTD_BOTH:
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

  SoMFInt32& coords = ifs->coordIndex;
  SoMFInt32& norms = ifs->normalIndex;
  SoMFInt32& mats = ifs->materialIndex;
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

      // back - right
      //     1    3
      //   0    2     
      //     x    5  
      //   x    4   

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
    }
  }
  coords.finishEditing();
  norms.finishEditing();
  mats.finishEditing();

  UpdateUnitValues_blocks();            // hand off to next guy..
}

void BrainVolumeView::UpdateUnitValues_blocks() {
  BrainView* bv = this->bv(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  T3BrainNode* node_so = this->node_so(); // cache
  if(!node_so) return;
  SoIndexedFaceSet* ifs = node_so->shape();
  if(!ifs) return; // something wrong..

  SoMFVec3f vertex;
  SoMFUInt32 color;

  SbVec3f* vertex_dat = vertex.startEditing();
  uint32_t* color_dat = color.startEditing();

  float trans = bv->view_params.unit_trans;
  float max_z = MIN(bv->max_size.x, bv->max_size.y); // smallest XY
  max_z = MAX(max_z, bv->max_size.z); // make sure Z isn't bigger

  SoSeparator* un_txt = 0; //DPF TBD: node_so->unitText();

  String val_str;
  String unit_name;
  float val;
  float sc_val;
  T3Color col;
  TwoDCoord pos;
  int v_idx = 0;
  int c_idx = 0;
  int t_idx = 3;                // base color + font
  // these go in normal order; indexes are backwards
  for(pos.y=0; pos.y<lay->flat_geom.y; pos.y++) {
    for(pos.x=0; pos.x<lay->flat_geom.x; pos.x++) { // right to left
      bv->GetUnitDisplayVals(this, pos, val, col, sc_val);
      Unit* unit = lay->UnitAtCoord(pos);
      if(bv->unit_con_md && (unit == bv->unit_src.ptr())) {
        col.r = 0.0f; col.g = 1.0f; col.b = 0.0f;
      }
      float zp = .5f * sc_val / max_z;
      v_idx+=4;                 // skip the _0 cases

      vertex_dat[v_idx++][1] = zp; // 00_v = 1
      vertex_dat[v_idx++][1] = zp; // 10_v = 2
      vertex_dat[v_idx++][1] = zp; // 01_v = 3
      vertex_dat[v_idx++][1] = zp; // 11_v = 4

      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      color_dat[c_idx++] = T3Color::makePackedRGBA(col.r, col.g, col.b, alpha);

      if(bv->unit_text_disp & BrainView::UTD_VALUES) {
        SoSeparator* tsep = (SoSeparator*)un_txt->getChild(t_idx);
        SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
        SoMFString* mfs = &(txt->string);
        ValToDispText(val, val_str);
        switch (bv->unit_text_disp) {
        case BrainView::UTD_VALUES:
          mfs->setValue(val_str.chars());
          break;
        case BrainView::UTD_NAMES:
          break;
        case BrainView::UTD_BOTH:
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

void BrainVolumeView::UpdateUnitValues() {
  BrainView* bv = this->bv(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  if(lay->Iconified()) {
    return;                     // don't render anything!
  }
  if(bv->snap_bord_disp)
    UpdateUnitValues_snap_bord();
  if(false) {
    UpdateUnitValues_outnm();
    return;
  }
  if(bv->unit_disp_mode == BrainView::UDM_BLOCK) {
    UpdateUnitValues_blocks();
    return;
  }
  Render_impl_children();
}

void BrainVolumeView::Reset_impl() {
  inherited::Reset_impl();
  uvd_bases.Reset();
}

void BrainVolumeView::Render_impl_outnm() {
  // this function just does all the memory allocation and static configuration
  // that doesn't depend on unit values, then it calls UpdateUnitValues
  // which sets all the values!

  BrainView* bv = this->bv(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  T3BrainNode* node_so = this->node_so(); // cache

  SoSeparator* un_txt = 0; //DPF TBD: node_so->unitText();
  if(!un_txt) {
    un_txt = 0; //DPF TBD: node_so->getUnitText();
    node_so->addChild(un_txt);
    T3DataViewFrame* fr = GetFrame();
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

  float szx = (float)lay->flat_geom.x / bv->max_size.x;
  float szy = (float)lay->flat_geom.y / bv->max_size.y;

  float cx = .5f * szx;
  float cy = .5f * szy;

  SoTranslation* tr = (SoTranslation*)un_txt->getChild(3);
  tr->translation.setValue(cx, 0.0f, -cy);

  szx = 1.5f * szx / (float)bv->font_sizes.un_nm_len;

  float ufontsz = MIN(szx, szy);

  SoFont* fnt = (SoFont*)un_txt->getChild(2);
  fnt->size.setValue(ufontsz);

  UpdateUnitValues_outnm();             // hand off to next guy..
}

void BrainVolumeView::UpdateUnitValues_outnm() {
  //  BrainView* bv = this->bv(); //cache
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  T3BrainNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  SoSeparator* un_txt = 0; //DPF TBD: node_so->unitText();

  SoAsciiText* txt = (SoAsciiText*)un_txt->getChild(4);
  SoMFString* mfs = &(txt->string);
  mfs->setValue(lay->output_name.chars());
}

void BrainVolumeView::Render_impl_snap_bord() {
  BrainView* bv = this->bv(); //cache
  T3BrainNode* node_so = this->node_so(); //cache
  if(!node_so) return;

  Layer* lay = this->layer(); //cache
  if(!lay) return;

  bool do_lines = bv->snap_bord_disp;
  SoIndexedLineSet* ils = 0; //DPF TBD: node_so->snapBordSet();
  SoDrawStyle* drw = 0; //DPF TBD: node_so->snapBordDraw();

  SoMFVec3f vertex;
  SoMFUInt32 color;
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
  drw->lineWidth = bv->snap_bord_width;
  // FACE = polyline, PART = line segment!!

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

  float max_xy = MAX(bv->max_size.x, bv->max_size.y);
  float spacing = 0.5f * max_xy * 0.0005f * bv->snap_bord_width; // todo: play with this constant
  if(spacing > .1f) spacing = .1f;
  float max_z = MAX(max_xy, bv->max_size.z); // make sure Z isn't bigger
  float zp = (spacing * 2.0f) / max_z;

  TwoDCoord pos;
  TwoDCoord upos;
  for(pos.y=0; pos.y<lay->flat_geom.y; pos.y++) {
    for(pos.x=0; pos.x<lay->flat_geom.x; pos.x++) { // right to left
      Unit* unit = lay->UnitAtCoord(pos);
      if(unit)
        lay->UnitDispPos(unit, upos);
      float xp = disp_scale * (((float)upos.x + spacing) / bv->max_size.x);
      float yp = -disp_scale * (((float)upos.y + spacing) / bv->max_size.y);
      float xp1 = disp_scale * (((float)upos.x+1 - spacing) / bv->max_size.x);
      float yp1 = -disp_scale * (((float)upos.y+1 - spacing) / bv->max_size.y);
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

void BrainVolumeView::UpdateUnitValues_snap_bord() {
  BrainView* bv = this->bv(); //cache
  if(!bv->snap_bord_disp) return;

  Layer* lay = this->layer(); //cache
  if(!lay) return;
  T3BrainNode* node_so = this->node_so(); // cache

  SoMFUInt32 color;

  uint32_t* color_dat = color.startEditing();
  if(!color_dat) return;

  int cur_disp_idx = bv->unit_disp_idx;

  MemberDef* md = (MemberDef*)bv->membs.FindName("snap");
  if(!md) return;               // shouldn't happen
  bv->unit_disp_idx = bv->membs.FindEl(md);

  float trans = bv->view_params.unit_trans;
  float val;
  float sc_val;
  iColor tc;
  T3Color col;
  TwoDCoord pos;
  int c_idx = 0;
  for(pos.y=0; pos.y<lay->flat_geom.y; pos.y++) {
    for(pos.x=0; pos.x<lay->flat_geom.x; pos.x++) { // right to left
      val = 0.0f;
      Unit* unit = lay->UnitAtCoord(pos);
      if(unit) {
        void* base;
        val = GetUnitDisplayVal(pos, base);
      }
      bv->GetUnitColor(val, tc, sc_val);
      col.setValue(tc.redf(), tc.greenf(), tc.bluef());

      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      color_dat[c_idx++] = T3Color::makePackedRGBA(col.r, col.g, col.b, alpha);
    }
  }

  bv->unit_disp_idx = cur_disp_idx; // restore!

  color.finishEditing();
}
