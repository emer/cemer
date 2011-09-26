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

Network*
BrainVolumeView::net() const {
  return (Network*) data();
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
  int mbs_sz = MAX(bv->membs.size, 1);
//   MatrixGeom nwgm1(3, lay->flat_geom.x, lay->flat_geom.y, mbs_sz);
//   if (uvd_bases.geom != nwgm1) {
//     uvd_bases.SetGeomN(nwgm1);
//   }
}

void BrainVolumeView::BuildAll() {
  Reset(); // in case where we are rebuilding
  BrainView* bv = this->bv();
  UpdateUnitViewBases(bv->unit_src);
  return;
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
  TwoDCoord coord;
//   for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
//     for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
//       Unit* unit = lay->UnitAtCoord(coord);
//       uvd_bases.Set(NULL, coord.x, coord.y, midx);
//       if (!unit) continue;  // rest will be null too, but we loop to null disp_base

//       if (is_send) {
//         for(int g=0;g<unit->recv.size;g++) {
//           RecvCons* tcong = unit->recv.FastEl(g);
//           MemberDef* act_md = tcong->con_type->members.FindName(nm);
//           if (!act_md)  continue;
//           Connection* con = tcong->FindConFrom(src_u);
//           if (!con) continue;
//           uvd_bases.Set(act_md->GetOff(con), coord.x, coord.y, midx);
//           break;                // once you've got one, done!
//         }
//       }
//       else {
//         for(int g=0;g<unit->send.size;g++) {
//           SendCons* tcong = unit->send.FastEl(g);
//           MemberDef* act_md = tcong->con_type->members.FindName(nm);
//           if (!act_md)  continue;
//           Connection* con = tcong->FindConFrom(src_u);
//           if (!con) continue;
//           uvd_bases.Set(act_md->GetOff(con), coord.x, coord.y, midx);
//           break;                // once you've got one, done!
//         }
//       }
//     }
//   }
}

void BrainVolumeView::UpdateUnitViewBase_Bias_impl(int midx, MemberDef* disp_md) {
//   TwoDCoord coord;
//   for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
//     for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
//       Unit* unit = lay->UnitAtCoord(coord);
//       uvd_bases.Set(NULL, coord.x, coord.y, midx);
//       if (!unit) continue;  // rest will be null too, but we loop to null disp_base
//       if(unit->bias.size == 0) continue;
//       Connection* con = unit->bias.Cn(0);
//       uvd_bases.Set(disp_md->GetOff(con), coord.x, coord.y, midx);
//     }
//   }
}

void BrainVolumeView::UpdateUnitViewBase_Unit_impl(int midx, MemberDef* disp_md) {
//   TwoDCoord coord;
//   for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
//     for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
//       Unit* unit = lay->UnitAtCoord(coord);
//       uvd_bases.Set(NULL, coord.x, coord.y, midx);
//       if (!unit) continue;  // rest will be null too, but we loop to null disp_base
//       uvd_bases.Set(disp_md->GetOff(unit), coord.x, coord.y, midx);
//     }
//   }
}

void BrainVolumeView::UpdateUnitViewBase_Sub_impl(int midx, MemberDef* disp_md) {
//   TypeDef* own_td = lay->units.el_typ; // should be unit type
//   ta_memb_ptr net_mbr_off = 0;
//   int net_base_off = 0;
//   MemberDef* smd = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off,
//                                                  disp_md->name, false); // no warn
//   TwoDCoord coord;
//   for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
//     for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
//       Unit* unit = lay->UnitAtCoord(coord);
//       uvd_bases.Set(NULL, coord.x, coord.y, midx);
//       if(!unit || !smd) continue;  // rest will be null too, but we loop to null disp_base
//       void* sbaddr = MemberDef::GetOff_static(unit, net_base_off, net_mbr_off);
//       uvd_bases.Set(sbaddr, coord.x, coord.y, midx);
//     }
//   }
}

void BrainVolumeView::UpdateAutoScale(bool& updated) {
//   BrainView* bv = this->bv();
//   TwoDCoord co;
//   void* base;
//   for (co.y = 0; co.y < lay->flat_geom.y; ++co.y) {
//     for (co.x = 0; co.x < lay->flat_geom.x; ++co.x) {
//       float val = GetUnitDisplayVal(co, base);
//       if(base) {
//         if(!updated) {
//           bv->scale.SetMinMax(val, val);
//           updated = true;
//         }
//         else
//           bv->scale.UpdateMinMax(val);
//       }
//     }
//   }
}

void BrainVolumeView::Render_pre() {
  BrainView* bv = this->bv();

  setNode(new T3BrainNode(this));
  //NOTE: we create/adjust the units in the Render_impl routine
  T3BrainNode* ugrp_so = node_so(); // cache

//  ugrp_so->setGeom(lay->disp_geom.x, lay->disp_geom.y, bv->max_size.x,
//                   bv->max_size.y, bv->max_size.z, lay->disp_scale);

  inherited::Render_pre();
}

void BrainVolumeView::Render_impl() {
  inherited::Render_impl();
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
  if(!bv) return;

  T3BrainNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  UpdateUnitViewBases(bv->unit_src); // always make sure we're allocated properly

  Render_impl_blocks();
}

void BrainVolumeView::Render_impl_blocks() {
  // this function just does all the memory allocation and static configuration
  // that doesn't depend on unit values, then it calls UpdateUnitValues
  // which sets all the values!

  BrainView* bv = this->bv(); //cache
//   Network* net = this->net(); //cache
//   if(!net) return;
  T3BrainNode* node_so = this->node_so(); // cache
  SoIndexedFaceSet* ifs = node_so->shape();
  SoVertexProperty* vtx_prop = node_so->vtxProp();
  if (!ifs || !vtx_prop) return; // something wrong..

  SoMFVec3f& vertex = vtx_prop->vertex;
  SoMFVec3f& normal = vtx_prop->normal;
  SoMFUInt32& color = vtx_prop->orderedRGBA;

  vtx_prop->normalBinding.setValue(SoNormalBinding::OVERALL);
  vtx_prop->materialBinding.setValue(SoMaterialBinding::PER_FACE_INDEXED);

  normal.setNum(1);
  SbVec3f* normal_dat = normal.startEditing();
  int idx=0;
  normal_dat[idx++].setValue(0.0f, 1.0f, 0.0f); // top
  normal.finishEditing();

  TDCoord brain_geom;
  brain_geom.SetXYZ(10,10,10);

  float disp_scale = 0.1f;	// todo: this is fiction..
  
  int n_geom = brain_geom.x * brain_geom.y * brain_geom.z;
  int n_per_vtx = 4;
  int tot_vtx =  n_geom * n_per_vtx;

  vertex.setNum(tot_vtx);
  color.setNum(n_geom);

  SbVec3f* vertex_dat = vertex.startEditing();

  T3Color col;
  TDCoord pos;
  int v_idx = 0;
  int t_idx = 3;                // base color + complexity + font
  // these go in normal order; indexes are backwards
  for(pos.z=0; pos.z<brain_geom.y; pos.z++) {
    for(pos.y=0; pos.y<brain_geom.y; pos.y++) {
      for(pos.x=0; pos.x<brain_geom.x; pos.x++) { // right to left
	float xp = disp_scale * (((float)pos.x) / (float)brain_geom.x);
	float yp = -disp_scale * (((float)pos.y) / (float)brain_geom.y);
	float zp = disp_scale * (((float)pos.z) / (float)brain_geom.z);

	float xp1 = disp_scale * (((float)pos.x+1) / (float)brain_geom.x);
	float yp1 = -disp_scale * (((float)pos.y+1) / (float)brain_geom.y);
// 	float zp1 = disp_scale * (((float)pos.z+1) / (float)brain_geom.z);

	vertex_dat[v_idx++].setValue(xp, yp, zp); // 00_0 = 0
	vertex_dat[v_idx++].setValue(xp1, yp, zp); // 10_0 = 0
	vertex_dat[v_idx++].setValue(xp1, yp1, zp); // 01_0 = 0
	vertex_dat[v_idx++].setValue(xp, yp1, zp); // 11_0 = 0
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

  int nx = brain_geom.x;

  // values of the cubes xy_[0,v]
  //     01_v   11_v   
  //   01_0   11_0     
  //     00_v   10_v    
  //   00_0   10_0     

  // triangle strip order is 0 1 2, 2 1 3, 2 3 4

//   int32_t* coords_dat = coords.startEditing();
//   int32_t* norms_dat = norms.startEditing();
//   int32_t* mats_dat = mats.startEditing();
//   int cidx = 0;
//   int nidx = 0;
//   int midx = 0;
//   for(pos.y=brain_geom.y-1; pos.y>=0; pos.y--) { // go back to front
//     for(pos.x=0; pos.x<brain_geom.x; pos.x++) { // right to left
//       int c00_0 = (pos.y * nx + pos.x) * n_per_vtx;
//       int c10_0 = c00_0 + 1;
//       int c01_0 = c00_0 + 2;
//       int c11_0 = c00_0 + 3;
//       int c00_v = c00_0 + 4;
//       int c10_v = c00_0 + 5;
//       int c01_v = c00_0 + 6;
//       int c11_v = c00_0 + 7;

//       int mat_idx = (pos.y * nx + pos.x);

//       // back - right
//       //     1    3
//       //   0    2     
//       //     x    5  
//       //   x    4   

//       coords_dat[cidx++] = (c01_0); // 0
//       coords_dat[cidx++] = (c01_v); // 1
//       coords_dat[cidx++] = (c11_0); // 2
//       coords_dat[cidx++] = (c11_v); // 3
//       coords_dat[cidx++] = (c10_0); // 4
//       coords_dat[cidx++] = (c10_v); // 5
//       coords_dat[cidx++] = (-1); // -1  -- 7 total

//       norms_dat[nidx++] = (0); // back
//       norms_dat[nidx++] = (0); // back
//       norms_dat[nidx++] = (1); // right
//       norms_dat[nidx++] = (1); // right -- 4 total

//       mats_dat[midx++] = (mat_idx);

//       // left - front
//       //     1    x 
//       //   0    x   
//       //     3    5
//       //   2    4   

//       coords_dat[cidx++] = (c01_0); // 0
//       coords_dat[cidx++] = (c01_v); // 1
//       coords_dat[cidx++] = (c00_0); // 2
//       coords_dat[cidx++] = (c00_v); // 3
//       coords_dat[cidx++] = (c10_0); // 4
//       coords_dat[cidx++] = (c10_v); // 5
//       coords_dat[cidx++] = (-1); // -1 -- 7 total

//       norms_dat[nidx++] = (2); // left
//       norms_dat[nidx++] = (2); // left
//       norms_dat[nidx++] = (3); // front
//       norms_dat[nidx++] = (3); // front -- 4 total

//       mats_dat[midx++] = (mat_idx);

//       // triangle strip order is 0 1 2, 2 1 3, 2 3 4
//       // top
//       //     0    1
//       //   x    x  
//       //     2    3
//       //   x    x  

//       coords_dat[cidx++] = (c01_v); // 0
//       coords_dat[cidx++] = (c11_v); // 1
//       coords_dat[cidx++] = (c00_v); // 2
//       coords_dat[cidx++] = (c10_v); // 3
//       coords_dat[cidx++] = (-1); // -1 -- 5 total

//       norms_dat[nidx++] = (4); // top
//       norms_dat[nidx++] = (4); // top -- 2 total

//       mats_dat[midx++] = (mat_idx);

//       // total coords = 7 + 7 + 5 = 19
//       // total norms = 4 + 4 + 2 = 10
//       // total mats = 3
//     }
//   }
//   coords.finishEditing();
//   norms.finishEditing();
//   mats.finishEditing();

  UpdateUnitValues_blocks();            // hand off to next guy..
}

void BrainVolumeView::UpdateUnitValues_blocks() {
  BrainView* bv = this->bv(); //cache
  T3BrainNode* node_so = this->node_so(); // cache
  if(!node_so) return;
  SoIndexedFaceSet* ifs = node_so->shape();
  SoVertexProperty* vtx_prop = node_so->vtxProp();
  if (!ifs || !vtx_prop) return; // something wrong..

  SoMFUInt32& color = vtx_prop->orderedRGBA;

  uint32_t* color_dat = color.startEditing();

  float trans = bv->view_params.unit_trans;
//   float max_z = MIN(bv->max_size.x, bv->max_size.y); // smallest XY
//   max_z = MAX(max_z, bv->max_size.z); // make sure Z isn't bigger

  // todo: use cur_disp_vals to get values to plot here -- need extra function to turn unit values into disp vals that does
  // many to one and one to many issues etc...

  float val;
  float sc_val;
  T3Color col;
  TwoDCoord pos;
  int c_idx = 0;
  // these go in normal order; indexes are backwards
//   for(pos.y=0; pos.y<brain_geom.y; pos.y++) {
//     for(pos.x=0; pos.x<brain_geom.x; pos.x++) { // right to left
//       bv->GetUnitDisplayVals(this, pos, val, col, sc_val);
//       Unit* unit = lay->UnitAtCoord(pos);
//       if(bv->unit_con_md && (unit == bv->unit_src.ptr())) {
//         col.r = 0.0f; col.g = 1.0f; col.b = 0.0f;
//       }
//       float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
//       color_dat[c_idx++] = T3Color::makePackedRGBA(col.r, col.g, col.b, alpha);
//     }
//   }
  color.finishEditing();
}

void BrainVolumeView::UpdateUnitValues() {
  BrainView* bv = this->bv(); //cache
  UpdateUnitValues_blocks();
  Render_impl_children();
}

void BrainVolumeView::Reset_impl() {
  inherited::Reset_impl();
  uvd_bases.Reset();
}

