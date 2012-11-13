// Copyright, 1995-2007, Regents of the University of Colorado,
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

// stuff to implement unit view..

#include "netstru_qtso.h"

#include "ta_geometry.h"
#include "emergent_project.h"
#include "css_qt.h"             // for the cssiSession
#include "ta_qtclipdata.h"
#include "ta_qt.h"
#include "ta_qtgroup.h"
#include "ta_qtviewer.h"

#include "iflowlayout.h"
#include "icolor.h"
#include "ilineedit.h"
#include "iscrollarea.h"

#include "imisc_so.h"
#include "NewNetViewHelper.h"

#include <qapplication.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <QGroupBox>
#include <qlayout.h>
#include <QScrollArea>
#include <qpushbutton.h>
#include <QTreeWidget>
#include <qwidget.h>

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/SoEventManager.h>

#include <limits.h>
#include <float.h>

//////////////////////////
//   ScaleRange         //
//////////////////////////

void ScaleRange::SetFromScale(ColorScale& cs) {
  auto_scale = cs.auto_scale;
  min = cs.min;
  max = cs.max;
}

void ScaleRange::SetFromScaleRange(ColorScale& cs) {
  cs.auto_scale = auto_scale;
  cs.SetMinMax(min, max);
}

void ScaleRange::UpdateAfterEdit_impl() {
  taOBase::UpdateAfterEdit_impl(); // skip over taNbase to avoid c_name thing!
}

//////////////////////////
//   nvDataView         //
//////////////////////////

NetView* nvDataView::getNetView()
{
  if (!m_nv) {
    // Cache the value on the first request.
    m_nv = GET_MY_OWNER(NetView);
  }
  return m_nv;
}

void nvDataView::Initialize()
{
  m_nv = NULL;
}

void nvDataView::CutLinks()
{
  m_nv = NULL;
  inherited::CutLinks();
}

//////////////////////////
//   UnitView           //
//////////////////////////

void UnitView::Initialize()
{
  m_nv = NULL;
  picked = 0;
}

void UnitView::CutLinks()
{
  m_nv = NULL;
  inherited::CutLinks();
}

NetView* UnitView::getNetView()
{
  if (!m_nv) {
    if (LayerView* layv = lay()) {
      // Cache the value on the first request.
      m_nv = layv->getNetView();
    }
  }
  return m_nv;
}

void UnitView::Render_pre()
{
  NetView* nv = getNetView();
  float max_x = nv->max_size.x;
  float max_y = nv->max_size.y;
  float max_z = nv->max_size.z;
  float un_spc= nv->view_params.unit_spacing;

  Unit* unit = this->unit(); //cache
  Layer* lay = unit->own_lay();
  float disp_scale = lay->disp_scale;

  switch (nv->unit_disp_mode) {
  case NetView::UDM_CIRCLE:
    setNode(new T3UnitNode_Circle(this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  case NetView::UDM_RECT:
    setNode(new T3UnitNode_Rect(this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  case NetView::UDM_BLOCK:
    setNode(new T3UnitNode_Block(this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  case NetView::UDM_CYLINDER:
    setNode(new T3UnitNode_Cylinder(this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  }

  taVector2i upos;  unit->LayerDispPos(upos);
  node_so()->transform()->translation.setValue
    (disp_scale * ((float)(upos.x + 0.5f) / max_x), 0.0f,
     -disp_scale * (((float)(upos.y + 0.5f) / max_y)));
  inherited::Render_pre();
}


//////////////////////////
//   UnitGroupView      //
//////////////////////////

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
      val = uvd_hist.FastEl(co.x, co.y, nv->unit_disp_idx, midx);
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
    } else if ((nm=="s") || (nm == "r")) {
      UpdateUnitViewBase_Con_impl(midx, (nm=="s"), disp_md->name.after('.'), src_u);
    } else if (nm=="bias") {
      UpdateUnitViewBase_Bias_impl(midx, disp_md);
    } else { // sub-member of unit
      UpdateUnitViewBase_Sub_impl(midx, disp_md);
    }
  }
}

void UnitGroupView::UpdateUnitViewBase_Con_impl(int midx, bool is_send, String nm, Unit* src_u) {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
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

void UnitGroupView::UpdateUnitViewBase_Bias_impl(int midx, MemberDef* disp_md) {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  taVector2i coord;
  for(coord.y = 0; coord.y < lay->flat_geom.y; coord.y++) {
    for(coord.x = 0; coord.x < lay->flat_geom.x; coord.x++) {
      Unit* unit = lay->UnitAtCoord(coord);
      uvd_bases.Set(NULL, coord.x, coord.y, midx);
      if (!unit) continue;  // rest will be null too, but we loop to null disp_base
      if(unit->lesioned()) continue;
      if(unit->bias.size == 0) continue;
      Connection* con = unit->bias.Cn(0);
      uvd_bases.Set(disp_md->GetOff(con), coord.x, coord.y, midx);
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

// this callback is registered in NetView::Render_pre

void UnitGroupView_MouseCB(void* userData, SoEventCallback* ecb) {
  NetView* nv = (NetView*)userData;
  T3DataViewFrame* fr = nv->GetFrame();
  SoMouseButtonEvent* mouseevent = (SoMouseButtonEvent*)ecb->getEvent();
  SoMouseButtonEvent::Button but = mouseevent->getButton();
  if(!SoMouseButtonEvent::isButtonReleaseEvent(mouseevent, but)) return; // only releases
  bool got_one = false;
  for(int i=0;i<fr->root_view.children.size;i++) {
    taDataView* dv = fr->root_view.children[i];
    if(dv->InheritsFrom(&TA_NetView)) {
      NetView* tnv = (NetView*)dv;
      T3ExaminerViewer* viewer = tnv->GetViewer();
      SoRayPickAction rp( viewer->getViewportRegion());
      SbVec2s mpos_orig = mouseevent->getPosition();
      SbVec2s mpos = mpos_orig;
      int fail_cnt = 0;
      do {
        rp.setPoint(mpos);
        rp.apply(viewer->quarter->getSoEventManager()->getSceneGraph()); // event mgr has full graph!
        SoPickedPoint* pp = rp.getPickedPoint(0);
        if(!pp) {
//        taMisc::Info("no pick:", String(fail_cnt));
          int pm_x = fail_cnt % 4 < 2 ? -1 : 1;
          int pm_y = (fail_cnt % 4) % 2 == 0 ? -1 : 1;
          // search in increasingly wide area around central point to see if we pick
          mpos.setValue(mpos_orig[0] + pm_x * (fail_cnt / 4),
                        mpos_orig[1] + pm_y * (fail_cnt / 4));
          fail_cnt++;
          continue;
        }
        SoNode* pobj = pp->getPath()->getNodeFromTail(2);
        if(!pobj) {
          //    taMisc::Info("no pobj");
          fail_cnt++;
          continue;
        }
        //       taMisc::Info("obj typ: ", pobj->getTypeId().getName());
        if(!pobj->isOfType(T3UnitGroupNode::getClassTypeId())) {
          pobj = pp->getPath()->getNodeFromTail(3);
          //    taMisc::Info("2: obj typ: ", pobj->getTypeId().getName());
          if(!pobj->isOfType(T3UnitGroupNode::getClassTypeId())) {
            pobj = pp->getPath()->getNodeFromTail(1);
            //    taMisc::Info("3: obj typ: ", pobj->getTypeId().getName());
            if(pobj->getName() == "WtLines") {
              // disable selecting of wt lines!
              ecb->setHandled();
              //            taMisc::Info("wt lines bail");
              return;
            }
            //    taMisc::Info("not unitgroupnode! bail");
            fail_cnt++;
            continue;
          }
        }
        UnitGroupView* act_ugv = static_cast<UnitGroupView*>(((T3UnitGroupNode*)pobj)->dataView());
        Layer* lay = act_ugv->layer(); //cache
        float disp_scale = lay->disp_scale;

        SbVec3f pt = pp->getObjectPoint(pobj);
        int xp = (int)((pt[0] * tnv->max_size.x) / disp_scale);
        int yp = (int)(-(pt[2] * tnv->max_size.y) / disp_scale);

        if((xp >= 0) && (xp < lay->disp_geom.x) && (yp >= 0) && (yp < lay->disp_geom.y)) {
          Unit* unit = lay->UnitAtDispCoord(xp, yp);
          if(unit && tnv->unit_src != unit) {
            tnv->setUnitSrc(NULL, unit);
            tnv->InitDisplay();   // this is apparently needed here!!
            tnv->UpdateDisplay();
          }
        }
        //       else {
        //      taMisc::Info("coords off");
        //       }
        got_one = true;
      } while(!got_one && fail_cnt < 100);
    }  // for
  } // if
  if(got_one)
    ecb->setHandled();
}

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

  ugrp_so->setGeom(lay->disp_geom.x, lay->disp_geom.y, nv->max_size.x,
                   nv->max_size.y, nv->max_size.z, lay->disp_scale);

  inherited::Render_pre();
}

void UnitGroupView::Render_impl() {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  NetView* nv = getNetView();

  //set origin: 0,0,0
//   taVector3i& pos = ugrp->pos;
//   float disp_scale = lay->disp_scale;
//   taTransform* ft = transform(true);
//   ft->translate.SetXYZ(disp_scale * ((float)pos.x / nv->max_size.x),
//                     disp_scale * ((float)pos.z / nv->max_size.z),
//                     disp_scale * ((float)-pos.y / nv->max_size.y));

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

  float max_z = MIN(nv->max_size.x, nv->max_size.y); // smallest XY
  max_z = MAX(max_z, nv->max_size.z); // make sure Z isn't bigger
  float trans = nv->view_params.unit_trans;
  float font_z = nv->font_sizes.unit*2.0f;
  float font_y = .45f / nv->max_size.y;
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

  float disp_scale = lay->disp_scale;

  int n_geom = lay->flat_geom.Product();
  int n_per_vtx = 8;
  int tot_vtx =  n_geom * n_per_vtx;
  vertex.setNum(tot_vtx);
  color.setNum(n_geom);

  float spacing = nv->view_params.unit_spacing;
  float max_z = MIN(nv->max_size.x, nv->max_size.y); // smallest XY
  max_z = MAX(max_z, nv->max_size.z); // make sure Z isn't bigger

  float max_xy = MAX(nv->max_size.x, nv->max_size.y);
  if(nv->snap_bord_disp) {
    spacing += max_xy * 0.0005f * nv->snap_bord_width; // todo: play with this constant
    if(spacing > 0.25f) spacing = 0.25f;
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
    }
    SoFont* fnt = (SoFont*)un_txt->getChild(2);
    fnt->size.setValue(ufontsz);
  }

  SbVec3f* vertex_dat = vertex.startEditing();

  String val_str = "0.0";       // initial default
  String unit_name;
  T3Color col;
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
      float xp = disp_scale * (((float)upos.x + spacing) / nv->max_size.x);
      float yp = -disp_scale * (((float)upos.y + spacing) / nv->max_size.y);
      float xp1 = disp_scale * (((float)upos.x+1 - spacing) / nv->max_size.x);
      float yp1 = -disp_scale * (((float)upos.y+1 - spacing) / nv->max_size.y);
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
    }
  }
  coords.finishEditing();
  norms.finishEditing();
  mats.finishEditing();

  UpdateUnitValues_blocks(); // hand off to next guy to adjust block heights
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
  float max_z = MIN(nv->max_size.x, nv->max_size.y); // smallest XY
  max_z = MAX(max_z, nv->max_size.z); // make sure Z isn't bigger

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

  float szx = (float)lay->flat_geom.x / nv->max_size.x;
  float szy = (float)lay->flat_geom.y / nv->max_size.y;

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

  float max_xy = MAX(nv->max_size.x, nv->max_size.y);
  float spacing = 0.5f * max_xy * 0.0005f * nv->snap_bord_width; // todo: play with this constant
  if(spacing > .1f) spacing = .1f;
  float max_z = MAX(max_xy, nv->max_size.z); // make sure Z isn't bigger
  float zp = (spacing * 2.0f) / max_z;

  taVector2i pos;
  taVector2i upos;
  for(pos.y=0; pos.y<lay->flat_geom.y; pos.y++) {
    for(pos.x=0; pos.x<lay->flat_geom.x; pos.x++) { // right to left
      Unit* unit = lay->UnitAtCoord(pos);
      if(unit)
        lay->UnitDispPos(unit, upos);
      float xp = disp_scale * (((float)upos.x + spacing) / nv->max_size.x);
      float yp = -disp_scale * (((float)upos.y + spacing) / nv->max_size.y);
      float xp1 = disp_scale * (((float)upos.x+1 - spacing) / nv->max_size.x);
      float yp1 = -disp_scale * (((float)upos.y+1 - spacing) / nv->max_size.y);
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

////////////////////////////////////////////////////
//   nvhDataView

void nvhDataView::Copy_(const nvhDataView& cp) {
  m_hcolor = cp.m_hcolor;
}

void nvhDataView::setHighlightColor(const T3Color& color) {
  m_hcolor = color;
  DoHighlightColor(true);
}


////////////////////////////////////////////////////
//   LayerView

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

void LayerView::DataUpdateAfterEdit_impl() {
  inherited::DataUpdateAfterEdit_impl();
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

  setNode(new T3LayerNode(this, show_drag));
  DoHighlightColor(false);

  inherited::Render_pre();
}

void LayerView::Render_impl() {
  Layer* lay = this->layer(); //cache
  if(!lay) return;
  NetView* nv = getNetView();

  taVector3i& pos = lay->pos;      // with layer groups as real things now, use this!
  //  taVector3i pos; lay->GetAbsPos(pos);
  taTransform* ft = transform(true);
  ft->translate.SetXYZ((float)pos.x / nv->max_size.x,
                       (float)pos.z / nv->max_size.z,
                       (float)-pos.y / nv->max_size.y);

  T3LayerNode* node_so = this->node_so(); // cache
  if(!node_so) return;
  if(lay->Iconified()) {
    node_so->setGeom(1, 1, nv->max_size.x, nv->max_size.y, nv->max_size.z, 1.0f);
  }
  else {
    node_so->setGeom(lay->disp_geom.x, lay->disp_geom.y,
                     nv->max_size.x, nv->max_size.y, nv->max_size.z, lay->disp_scale);
  }
  node_so->setCaption(data()->GetName().chars());

  float max_xy = MAX(nv->max_size.x, nv->max_size.y);
  float lay_wd = T3LayerNode::width / max_xy;
  lay_wd = MIN(lay_wd, T3LayerNode::max_width);
  float fx = (float)lay->scaled_disp_geom.x / nv->max_size.x;

  // ensure that the layer label does not go beyond width of layer itself!
  float eff_lay_font_size = nv->font_sizes.layer;
  float lnm_wd = (eff_lay_font_size * lay->name.length()) / t3Misc::char_ht_to_wd_pts;
  if(lnm_wd > fx) {
    eff_lay_font_size = (fx / (float)lay->name.length()) * t3Misc::char_ht_to_wd_pts;
  }
  eff_lay_font_size = MAX(eff_lay_font_size, nv->font_sizes.layer_min);
  node_so->resizeCaption(eff_lay_font_size);


  SbVec3f tran(0.0f, -eff_lay_font_size, lay_wd);
  node_so->transformCaption(tran);

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

  float fx = (float)lay->disp_geom.x / nv->max_size.x;
  float fy = (float)lay->disp_geom.y / nv->max_size.y;
  float xfrac = .5f * fx;
  float yfrac = .5f * fy;

  const SbVec3f& trans = dragger->translation.getValue();
  float new_x = trans[0] * nv->max_size.x;
  float new_y = trans[1] * nv->max_size.y;
  if(new_x < 0.0f)      new_x -= .5f; // add an offset to effect rounding.
  else                  new_x += .5f;
  if(new_y < 0.0f)      new_y -= .5f;
  else                  new_y += .5f;

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Layer Move", net, false, NULL); // save at net
  }

  lay->pos.x += (int)new_x;
//   if(lay->pos.x < 0) lay->pos.x = 0;
  lay->pos.y += (int)new_y;
//   if(lay->pos.y < 0) lay->pos.y = 0;

  laynd->txfm_shape()->translation.setValue(xfrac, 0.0f, -yfrac); // reset!
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);

  lay->DataChanged(DCR_ITEM_UPDATED);
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
  float new_z = trans[0] * nv->max_size.z;
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

  lay->DataChanged(DCR_ITEM_UPDATED);
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

  taVector3i pos; lay->GetAbsPos(pos);
  viewer->main_xform = nv->main_xform; // first get the network

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->main_xform.rotate.x, nv->main_xform.rotate.y,
                           nv->main_xform.rotate.z), nv->main_xform.rotate.rot);

  float szx = ((float)lay->scaled_disp_geom.x / nv->max_size.x);
  float szy = ((float)lay->scaled_disp_geom.y / nv->max_size.y);

  // translate to layer offset + indent into layer
  SbVec3f trans;
  trans[0] = nv->main_xform.scale.x * (((float)pos.x / nv->max_size.x) + .05f * szx);
  trans[1] = nv->main_xform.scale.y * ((((float)pos.z + 0.5f) / nv->max_size.z));
  trans[2] = nv->main_xform.scale.z * (((float)-pos.y / nv->max_size.y) - .05f * szy);
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

  T3DataViewFrame* fr = GetFrame();
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

//////////////////////////
//   PrjnView           //
//////////////////////////

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
  if(!prjn->projected)
    prjn_trans = .8f;

  SoMaterial* mat = node_so()->material(); //cache
  if (apply) {
    mat->diffuseColor.setValue(m_hcolor);
    mat->transparency.setValue(0.0f);
  } else {
    mat->diffuseColor.setValue(SbColor(1.0f, .9f, .5f)); // very light orange
    mat->transparency.setValue(prjn_trans);
  }
  nd->setArrowColor(SbColor(1.0f, .8f, 0.0f), prjn_trans);
}

void PrjnView::Render_pre() {
  NetView* nv = getNetView();
  Projection* prjn = this->prjn(); // cache
  setNode(new T3PrjnNode(this, prjn->projected, nv->view_params.prjn_width));
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

  taVector3f src;             // source and dest coords
  taVector3f dst;
  taVector3i lay_fr_pos; lay_fr->GetAbsPos(lay_fr_pos);
  taVector3i lay_to_pos; lay_to->GetAbsPos(lay_to_pos);

  float max_xy = MAX(nv->max_size.x, nv->max_size.y);
  float lay_ht = T3LayerNode::height / max_xy;
  float lay_wd = T3LayerNode::width / max_xy;
  lay_wd = MIN(lay_wd, T3LayerNode::max_width);

  // y = network z coords -- same for all cases
  src.y = ((float)lay_fr_pos.z) / nv->max_size.z;
  dst.y = ((float)lay_to_pos.z) / nv->max_size.z;

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

  if(nv->view_params.prjn_disp == NetViewParams::B_F) {
    // origin is *back* center
    src.x = ((float)lay_fr_pos.x + .5f * (float)lay_fr->scaled_disp_geom.x) / nv->max_size.x;
    src.z = -((float)(lay_fr_pos.y + lay_fr->scaled_disp_geom.y) / nv->max_size.y) - lay_wd;

    // dest is *front* *center*
    dst.x = ((float)lay_to_pos.x + .5f * (float)lay_to->scaled_disp_geom.x) / nv->max_size.x;
    dst.z = -((float)lay_to_pos.y / nv->max_size.y) + lay_wd;
  }
  else if(nv->view_params.prjn_disp == NetViewParams::L_R_F) { // easier to see
    // origin is *front* left
    src.x = ((float)lay_fr_pos.x) / nv->max_size.x + lay_wd;
    src.z = -((float)(lay_fr_pos.y) / nv->max_size.y) + lay_wd;

    // dest is *front* right
    dst.x = ((float)lay_to_pos.x + (float)lay_to->scaled_disp_geom.x) / nv->max_size.x - lay_wd;
    dst.z = -((float)lay_to_pos.y / nv->max_size.y) + lay_wd;
  }
  else if(nv->view_params.prjn_disp == NetViewParams::L_R_B) { // out of the way
    // origin is *back* left
    src.x = ((float)lay_fr_pos.x) / nv->max_size.x + lay_wd;
    src.z = -((float)(lay_fr_pos.y + lay_fr->scaled_disp_geom.y) / nv->max_size.y) - lay_wd;

    // dest is *back* right
    dst.x = ((float)lay_to_pos.x + (float)lay_to->scaled_disp_geom.x) / nv->max_size.x - lay_wd;
    dst.z = -((float)(lay_to_pos.y  + lay_to->scaled_disp_geom.y) / nv->max_size.y) - lay_wd;
  }

  if(dst.y == src.y && dst.x == src.x && dst.z == src.z) {
    dst.x += lay_wd;            // give it some minimal something
  }

  transform(true)->translate.SetXYZ(src.x, src.y, src.z);
  node_so->setEndPoint(SbVec3f(dst.x - src.x, dst.y - src.y, dst.z - src.z));

  // caption location is half way
  if(nv->view_params.prjn_name) {
    taVector3f cap((dst.x - src.x) / 2.0f - .05f, (dst.y - src.y) / 2.0f, (dst.z - src.z) / 2.0f);
    node_so->setCaption(prjn->name.chars());
    node_so->transformCaption(cap);
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

////////////////////////////////////////////////////
//   LayerGroupView

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

void LayerGroupView::DataUpdateAfterEdit_impl() {
  inherited::DataUpdateAfterEdit_impl();
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

  setNode(new T3LayerGroupNode(this, show_drag, hide_lines));
  DoHighlightColor(false);

  inherited::Render_pre();
}

void LayerGroupView::Render_impl() {
  Layer_Group* lgp = this->layer_group(); //cache
  NetView* nv = getNetView();

  taVector3i pos; lgp->GetAbsPos(pos);
  taTransform* ft = transform(true);
  if(root_laygp) {
    ft->translate.SetXYZ((float)pos.x / nv->max_size.x,
                         ((float)pos.z) / nv->max_size.z,
                         (float)-pos.y / nv->max_size.y);
  }
  else {
    ft->translate.SetXYZ(((float)pos.x) / nv->max_size.x,
                         ((float)pos.z) / nv->max_size.z,
                         (float)-pos.y / nv->max_size.y);
  }

  T3LayerGroupNode* node_so = this->node_so(); // cache
  if(!node_so) return;
  node_so->setGeom(lgp->pos.x, lgp->pos.y, lgp->pos.z,
                   lgp->max_disp_size.x, lgp->max_disp_size.y, lgp->max_disp_size.z,
                   nv->max_size.x, nv->max_size.y, nv->max_size.z);

  if(!node_so->hideLines()) {
    node_so->drawStyle()->lineWidth = nv->view_params.laygp_width;

    node_so->setCaption(data()->GetName().chars());
    float lay_wd_y = T3LayerNode::width / nv->max_size.y;
    float lay_ht_z = T3LayerNode::height / nv->max_size.z;
    float fx = (float)lgp->max_disp_size.x / nv->max_size.x;
    lay_wd_y = MIN(lay_wd_y, T3LayerNode::max_width);

    // ensure that the layer label does not go beyond width of layer itself!
    float eff_lay_font_size = nv->font_sizes.layer;
    float lnm_wd = (eff_lay_font_size * lgp->name.length()) / t3Misc::char_ht_to_wd_pts;
    if(lnm_wd > fx) {
      eff_lay_font_size = (fx / (float)lgp->name.length()) * t3Misc::char_ht_to_wd_pts;
    }
    node_so->resizeCaption(eff_lay_font_size);

    SbVec3f tran(0.0f, -eff_lay_font_size - 2.0f * lay_ht_z, lay_wd_y);
    node_so->transformCaption(tran);
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

  float fx = ((float)lgp->max_disp_size.x + 2.0f * T3LayerNode::width) / nv->max_size.x;
  float fy = ((float)lgp->max_disp_size.y + 2.0f * T3LayerNode::width) / nv->max_size.y;
  float fz = ((float)(lgp->max_disp_size.z-1) + 4.0f * T3LayerNode::height) / nv->max_size.z;
  float xfrac = (.5f * fx) - (T3LayerNode::width / nv->max_size.x);
  float yfrac = (.5f * fy) - (T3LayerNode::width / nv->max_size.y);
  float zfrac = (.5f * fz) - 2.0f * (T3LayerNode::height / nv->max_size.z);

  const SbVec3f& trans = dragger->translation.getValue();
  float new_x = trans[0] * nv->max_size.x;
  float new_y = trans[1] * nv->max_size.y;
  if(new_x < 0.0f)      new_x -= .5f; // add an offset to effect rounding.
  else                  new_x += .5f;
  if(new_y < 0.0f)      new_y -= .5f;
  else                  new_y += .5f;

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Layer Group Move", net, false, NULL); // save at net
  }

  lgp->pos.x += (int)new_x;
  if(lgp->pos.x < 0) lgp->pos.x = 0;
  lgp->pos.y += (int)new_y;
  if(lgp->pos.y < 0) lgp->pos.y = 0;

  laynd->txfm_shape()->translation.setValue(xfrac, zfrac, -yfrac); // reset!
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);

  lgp->DataChanged(DCR_ITEM_UPDATED);
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

  float fz = (float)lgp->max_disp_size.z / nv->max_size.z;
  float zfrac = .5f * fz;

  const SbVec3f& trans = dragger->translation.getValue();
  float new_z = trans[0] * nv->max_size.z;
  if(new_z < 0.0f)      new_z -= .5f;
  else                  new_z += .5f;

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Layer Group Move", net, false, NULL); // save at net
  }

  lgp->pos.z += (int)new_z;
  if(lgp->pos.z < 0) lgp->pos.z = 0;

  const SbVec3f& shptrans = laynd->txfm_shape()->translation.getValue();
  laynd->txfm_shape()->translation.setValue(shptrans[0], zfrac, shptrans[2]); // reset!
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);

  lgp->DataChanged(DCR_ITEM_UPDATED);
  nv->net()->LayerPos_Cleanup(); // reposition everyone to avoid conflicts

  nv->UpdateDisplay();
}


///////////////////////////////////////////////////////////////////////
//      NetViewObjView

void NetViewObjView::Initialize(){
  data_base = &TA_NetViewObj;
}

void NetViewObjView::Destroy() {
  CutLinks();
}

void NetViewObjView::SetObj(NetViewObj* ob) {
  if (Obj() == ob) return;
  SetData(ob);
  if (ob) {
    if (!name.contains(ob->name)) {
      SetName(ob->name);
    }
  }
}

void NetViewObjView::Render_pre() {
  T3ExaminerViewer* vw = GetViewer();
  bool show_drag = false;
  if(vw)
    show_drag = vw->interactionModeOn();

  NetView* nv = GET_MY_OWNER(NetView);
  if(nv && !nv->lay_mv) show_drag = false;

  setNode(new T3NetViewObj(this, show_drag));
  SoSeparator* ssep = node_so()->shapeSeparator();

  NetViewObj* ob = Obj();
  if(ob) {
    if(ob->obj_type == NetViewObj::OBJECT) {
      int acc = access(ob->obj_fname, F_OK);
      if (acc == 0) {
        SoInput in;
        if ((access(ob->obj_fname, F_OK) == 0) && in.openFile(ob->obj_fname)) {
          SoSeparator* root = SoDB::readAll(&in);
          if (root) {
            ssep->addChild(root);
            goto finish;
          }
        }
      }
      taMisc::Warning("object file:", ob->obj_fname, "not found or unable to be loaded!");
      ob->obj_type = NetViewObj::TEXT;
      ob->text = "file: " + taMisc::GetFileFmPath(ob->obj_fname) + " not loaded!";
    }
    if(ob->obj_type == NetViewObj::TEXT) {
      SoSeparator* tsep = new SoSeparator();
      SoComplexity* cplx = new SoComplexity;
      cplx->value.setValue(taMisc::text_complexity);
      tsep->addChild(cplx);
      SoFont* fnt = new SoFont();
      fnt->name = (const char*)taMisc::t3d_font_name;
      fnt->size.setValue(ob->font_size);
      tsep->addChild(fnt);
      SoAsciiText* txt = new SoAsciiText();
      SoMFString* mfs = &(txt->string);
      mfs->setValue(ob->text.chars());
      tsep->addChild(txt);
      ssep->addChild(tsep);
    }
  }
 finish:

  inherited::Render_pre();
}

void NetViewObjView::Render_impl() {
  inherited::Render_impl();

  T3NetViewObj* node_so = (T3NetViewObj*)this->node_so(); // cache
  if(!node_so) return;
  NetViewObj* ob = Obj();
  if(!ob) return;

  SoTransform* tx = node_so->transform();
  tx->translation.setValue(ob->pos.x, ob->pos.y, ob->pos.z);
  tx->scaleFactor.setValue(ob->scale.x, ob->scale.y, ob->scale.z);
  tx->rotation.setValue(SbVec3f(ob->rot.x, ob->rot.y, ob->rot.z), ob->rot.rot);

  if(ob->set_color) {
    SoMaterial* mat = node_so->material();
    mat->diffuseColor.setValue(ob->color.r, ob->color.g, ob->color.b);
    mat->transparency.setValue(1.0f - ob->color.a);
  }
}

// callback for netview transformer dragger
void T3NetViewObj_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3NetViewObj* nvoso = (T3NetViewObj*)userData;
  NetViewObjView* nvov = static_cast<NetViewObjView*>(nvoso->dataView());
  NetViewObj* nvo = nvov->Obj();
  NetView* nv = GET_OWNER(nvov, NetView);

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nvo->rot.x, nvo->rot.y, nvo->rot.z), nvo->rot.rot);

  SbVec3f trans = dragger->translation.getValue();
  cur_rot.multVec(trans, trans); // rotate the translation by current rotation
  trans[0] *= nvo->scale.x;  trans[1] *= nvo->scale.y;  trans[2] *= nvo->scale.z;
  taVector3f tr(trans[0], trans[1], trans[2]);
  nvo->pos += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
  taVector3f sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;        // prevent scale from going to small too fast!!
  nvo->scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);
    nvo->rot.SetXYZR(axis[0], axis[1], axis[2], angle);
  }

//   float h = 0.04f; // nominal amount of height, so we don't vanish
  nvoso->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  nvoso->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  nvoso->txfm_shape()->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

  nv->UpdateDisplay();
}

//////////////////////////
//   NetView            //
//////////////////////////

void NetViewFontSizes::Initialize() {
  net_name = .05f;
  net_vals = .05f;
  layer = .04f;
  layer_min = 0.01f;
  layer_vals = .03f;
  prjn = .01f;
  unit = .02f;
  un_nm_len = 3;
}

void NetViewParams::Initialize() {
  xy_square = false;
  unit_spacing = .05f;
  prjn_disp = L_R_B;
  prjn_name = false;
  prjn_width = .002f;
  prjn_trans = .5f;
  lay_trans = .5f;
  unit_trans = 0.6f;
  laygp_width = 1.0f;
  show_laygp = true;
}

/*

  Scale Range semantics

  The user can choose for each variable viewed whether to autoscale, and if not, what is the scale.
  Since only one scale can be in effect at once, and the scales are keyed by the display name
  (ex. "act", "r.wt", etc.), the scale system is keyed to this value.

*/

// Add a new NetView object to the frame for the given Network.
NetView* NetView::New(Network* net, T3DataViewFrame*& fr) {
  NewNetViewHelper new_net_view(fr, net, "network");
  // true == Allow only one NetView instances in a frame
  // for a given network.
  if (!new_net_view.isValid(true)) return NULL;

  // create NetView
  NetView* nv = new NetView();
  nv->SetData(net);
  nv->GetMaxSize();
  fr->AddView(nv);

  // make sure we've got it all rendered:
  nv->main_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, .35f);
  nv->BuildAll();

  new_net_view.showFrame();
  return nv;
}

NetView* Network::FindView() {
  taDataLink* dl = data_link();
  if(dl) {
    taDataLinkItr itr;
    NetView* el;
    FOR_DLC_EL_OF_TYPE(NetView, el, dl, itr) {
      return el;
    }
  }
  return NULL;
}

NetView* Network::FindMakeView(T3DataViewFrame* fr) {
  taDataLink* dl = data_link();
  if(dl) {
    taDataLinkItr itr;
    NetView* el;
    FOR_DLC_EL_OF_TYPE(NetView, el, dl, itr) {
      el->InitDisplay();
      el->UpdateDisplay();
      fr = el->GetFrame();
      if(fr) {
        MainWindowViewer* mwv = GET_OWNER(fr, MainWindowViewer);
        if(mwv) {
          mwv->SelectT3ViewTabName(fr->name);
        }
      }
      return el;
    }
  }

  return NetView::New(this, fr);
}

String Network::GetViewVar() {
  NetView* nv = FindView();
  if(!nv || !nv->unit_disp_md) return _nilString;
  return nv->unit_disp_md->name;
}

bool Network::SetViewVar(const String& view_var) {
  NetView* nv = FindView();
  if(!nv) return false;
  nv->SelectVar(view_var, false, true); // add, update
  return true;
}

Unit* Network::GetViewSrcU() {
  NetView* nv = FindView();
  if(nv) return nv->unit_src;
  return NULL;
}

bool Network::SetViewSrcU(Unit* un) {
  NetView* nv = FindView();
  if(!nv) return false;
  nv->setUnitSrc(NULL, un);
  nv->InitDisplay();
  nv->UpdateDisplay();
  return true;
}

void Network::PlaceNetText(NetTextLoc net_text_loc, float scale) {
  NetView* nv = FindView();
  if(!nv) return;

  nv->net_text_xform.scale = scale;

  switch(net_text_loc) {
  case NT_BOTTOM:               // x, z, y from perspective of netview
    nv->net_text_xform.translate.SetXYZ(0.0f, -0.5f, 0.0f);
    nv->net_text_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 0.0f);
    nv->net_text_rot = 0.0f;
    break;
  case NT_TOP_BACK:
    nv->net_text_xform.translate.SetXYZ(0.0f, 1.0f, -1.0f);
    nv->net_text_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 0.5f * taMath_float::pi);
    nv->net_text_rot = -90.0f;
    break;
  case NT_LEFT_BACK:
    nv->net_text_xform.translate.SetXYZ(-1.0f, 0.0f, -1.0f);
    nv->net_text_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 0.5f * taMath_float::pi);
    nv->net_text_rot = -90.0f;
    break;
  case NT_RIGHT_BACK:
    nv->net_text_xform.translate.SetXYZ(1.0f, 0.0f, -1.0f);
    nv->net_text_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 0.5f * taMath_float::pi);
    nv->net_text_rot = -90.0f;
    break;
  case NT_LEFT_MID:
    nv->net_text_xform.translate.SetXYZ(-1.0f, 0.0f, -0.5f);
    nv->net_text_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 0.5f * taMath_float::pi);
    nv->net_text_rot = -90.0f;
    break;
  case NT_RIGHT_MID:
    nv->net_text_xform.translate.SetXYZ(1.0f, 0.0f, -0.5f);
    nv->net_text_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 0.5f * taMath_float::pi);
    nv->net_text_rot = -90.0f;
    break;
  }
  nv->Render();
}

void Network::HistMovie(int x_size, int y_size, const String& fname_stub) {
  NetView* nv = FindView();
  if(!nv) return;
  nv->HistMovie(x_size, y_size, fname_stub);
}

void NetView::Initialize() {
  data_base = &TA_Network;
  nvp = NULL;
  display = true;
  lay_mv = true;
  net_text = true;
  show_iconified = false;
  net_text_xform.translate.SetXYZ(0.0f, 1.0f, -1.0f); // start at top back
  net_text_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 0.5f * taMath_float::pi); // start at right mid
  net_text_xform.scale = 0.7f;
  net_text_rot = -90.0f;

  unit_con_md = false;
  unit_disp_md = NULL;
  unit_disp_idx = -1;
  n_counters = 0;
  hist_idx = 0;
  hist_save = true;
  hist_max = 100;
  hist_ff = 5;

  unit_scrng = NULL;
  unit_md_flags = MD_UNKNOWN;
  unit_disp_mode = UDM_BLOCK;
  unit_text_disp = UTD_NONE;
  wt_line_disp = false;
  wt_line_width = 4.0f;
  wt_line_thr = .8f;
  wt_line_swt = false;
  wt_prjn_k_un = 4;
  wt_prjn_k_gp = 1;
  snap_bord_disp = false;
  snap_bord_width = 4.0f;

  no_init_on_rerender = false;
  hist_reset_req = false;
}

void NetView::Destroy() {
//nn  Reset();
  CutLinks();
}

void NetView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(lay_disp_modes, this);
  taBase::Own(scale, this);
  taBase::Own(scale_ranges, this);
  taBase::Own(cur_unit_vals, this);
  taBase::Own(ctr_hist, this);
  taBase::Own(ctr_hist_idx, this);
  taBase::Own(max_size, this);
  taBase::Own(font_sizes, this);
  taBase::Own(view_params, this);
  taBase::Own(net_text_xform, this);

  ctr_hist_idx.matrix = &ctr_hist;
}

void NetView::CutLinks() {
  // the nvp refs some of our stuff, so we have to nuke it
  if (nvp) {
    delete nvp; // should delete our ref
    nvp = NULL;
  }
  view_params.CutLinks();
  font_sizes.CutLinks();
  max_size.CutLinks();
  ctr_hist_idx.CutLinks();
  ctr_hist.CutLinks();
  cur_unit_vals.CutLinks();
  scale_ranges.CutLinks();
  scale.CutLinks();
  lay_disp_modes.CutLinks();
  inherited::CutLinks();
}

void NetView::CopyFromView(NetView* cp) {
  Copy_(*cp);
  T3DataViewMain::CopyFromViewFrame(cp);
}

String NetView::HistMemUsed() {
  String rval = "no units";
  if(net()) {
    int mem = net()->n_units * membs.size * sizeof(float) * hist_max;
    rval = String((float)mem / 1.0e6f) + "MB";
  }
  return rval;
}

bool NetView::HistBackAll() {
  if(!hist_save) return false;
  if(hist_idx >= ctr_hist_idx.length) return false;
  hist_idx = ctr_hist_idx.length;
  UpdateDisplay(true);
  return true;
}

bool NetView::HistBackF() {
  if(!hist_save) return false;
  if(hist_idx >= ctr_hist_idx.length) return false;
  hist_idx += hist_ff;
  hist_idx = MIN(ctr_hist_idx.length, hist_idx);
  UpdateDisplay(true);
  return true;
}

bool NetView::HistBack1() {
  if(!hist_save) return false;
  if(hist_idx >= ctr_hist_idx.length) return false;
  hist_idx++;
  UpdateDisplay(true);
  return true;
}

bool NetView::HistFwd1() {
  if(!hist_save) return false;
  if(hist_idx < 1) return false;
  hist_idx--;
  UpdateDisplay(true);
  return true;
}

bool NetView::HistFwdF() {
  if(!hist_save) return false;
  if(hist_idx < 1) return false;
  hist_idx -= hist_ff;
  hist_idx = MAX(0, hist_idx);
  UpdateDisplay(true);
  return true;
}

bool NetView::HistFwdAll() {
  if(!hist_save) return false;
  if(hist_idx < 1) return false;
  hist_idx = 0;
  UpdateDisplay(true);
  return true;
}

void NetView::HistMovie(int x_size, int y_size, const String& fname_stub) {
  T3DataViewFrame* fr = GetFrame();
  if(!fr) return;
  fr->SetImageSize(x_size, y_size);
  taMisc::ProcessEvents();
  int ctr = 0;
  while(hist_idx > 0) {
    String fnm = fname_stub + taMisc::LeadingZeros(ctr, 5) + ".png";
    fr->SaveImageAs(fnm, DataViewer::PNG);
    hist_idx--;
    UpdateDisplay(false);
    taMisc::ProcessEvents();
    ctr++;
    fr->SetImageSize(x_size, y_size);
    taMisc::ProcessEvents();
    taMisc::ProcessEvents();
    taMisc::ProcessEvents();
  }
  UpdateDisplay(true);
}

void NetView::unTrappedKeyPressEvent(QKeyEvent* e) {
  bool got_arw = false;
  taVector2i dir(0,0);
  if(e->key() == Qt::Key_Right) {
    got_arw = true;
    dir.x=1;
  }
  else if(e->key() == Qt::Key_Left) {
    got_arw = true;
    dir.x=-1;
  }
  else if(e->key() == Qt::Key_Up) {
    got_arw = true;
    dir.y=1;
  }
  else if(e->key() == Qt::Key_Down) {
    got_arw = true;
    dir.y=-1;
  }
  if(!got_arw) return;
  if(!(bool)unit_src) return;
  Layer* lay = unit_src->own_lay();
  if(!lay) return;
  taVector2i pos;
  lay->UnitLogPos(unit_src, pos);
  pos += dir;
  if(pos.x < 0 || pos.y < 0) return;
  Unit* nw_u = lay->UnitAtCoord(pos);
  if(nw_u) {
    setUnitSrc(NULL, nw_u);
    InitDisplay();   // this is apparently needed here!!
    UpdateDisplay();
  }
}

void NetView::ChildUpdateAfterEdit(taBase* child, bool& handled) {
  if(taMisc::is_loading || !taMisc::gui_active) return;
  TypeDef* typ = child->GetTypeDef();
  if (typ->InheritsFrom(&TA_ScaleRange)) {
    handled = true;
  }
  if (!handled)
    inherited::ChildUpdateAfterEdit(child, handled);
}

void NetView::BuildAll() { // populates all T3 guys
  if(!net()) {
    Reset();
    return;
  }
  GetMaxSize();

  Network* nt = net();

  // cannot preserve LayerView objects, so recording disp_mode info separately
  // layers grab from us when made, instead of pushing on them.
  // first pass is to fix things up based on layer names only
  for(int i = lay_disp_modes.size - 1; i >= 0; --i) {
    NameVar dmv = lay_disp_modes.FastEl(i);
    int li = nt->layers.FindLeafNameIdx(dmv.name);
    if (li < 0) {
      lay_disp_modes.RemoveIdx(i);
    }
  }
  // add layers not in us, move in position
  for(int li = 0; li < nt->layers.leaves; ++li) {
    Layer* ly = nt->layers.Leaf(li);
    if(!ly) continue;           // can happen, apparently!
    int i = lay_disp_modes.FindName(ly->name);
    if(i < 0) {
      NameVar dmv;
      dmv.name = ly->name;
      dmv.value = -1;           // uninit val
      lay_disp_modes.Insert(dmv, li);
    }
    else if(i != li) {
      lay_disp_modes.MoveIdx(i, li);
    }
  }

  { // delegate everything to the layers group
    LayerGroupView* lv = new LayerGroupView();
    lv->SetData(&nt->layers);
    lv->root_laygp = true;      // root guy 4 sure!
    children.Add(lv);
    lv->BuildAll();
  }

  FOREACH_ELEM_IN_GROUP(Layer, lay, net()->layers) {
    if(lay->lesioned() || lay->Iconified()) continue;
    FOREACH_ELEM_IN_GROUP(Projection, prjn, lay->projections) {
      if((prjn->from.ptr() == NULL) || prjn->from->lesioned()
         || prjn->from->Iconified()) continue;
      PrjnView* pv = new PrjnView();
      pv->SetData(prjn);
      //nn prjns.Add(pv); // this is automatic from the childadding thing
      children.Add(pv);
    }
  }

  FOREACH_ELEM_IN_GROUP(NetViewObj, obj, net()->view_objs) {
    NetViewObjView* ov = new NetViewObjView();
    ov->SetObj(obj);
    children.Add(ov);
  }
}

void NetView::SetLayDispMode(const String& lay_nm, int disp_md) {
  int i = lay_disp_modes.FindName(lay_nm);
  if(i < 0) return;
  lay_disp_modes.FastEl(i).value = disp_md;
}

int NetView::GetLayDispMode(const String& lay_nm) {
  int i = lay_disp_modes.FindName(lay_nm);
  if(i < 0) return -1;
  return lay_disp_modes.FastEl(i).value.toInt();
}

void NetView::ChildAdding(taDataView* child_) {
  inherited::ChildAdding(child_);
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if (!child) return;
  TypeDef* typ = child->GetTypeDef();
  if(typ->InheritsFrom(&TA_PrjnView)) {
    prjns.AddUnique(child);
  }
}

void NetView::ChildRemoving(taDataView* child_) {
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if(child) {
    prjns.RemoveEl(child);      // just try it
  }
  inherited::ChildRemoving(child_);
}

void NetView::UpdateName() {
  if (net()) {
    if (!name.contains(net()->name))
      SetName(net()->name + "_View");
  }
  else {
    if (name.empty())
      SetName("no_network");
  }
}

void NetView::DataUpdateAfterEdit_impl() {
  UpdateName();
  InitDisplay(true);
  UpdateDisplay();
}

void NetView::DataUpdateAfterEdit_Child_impl(taDataView* chld) {
  // called when lays/specs are updated; typically just update spec view
  UpdatePanel();
}

void NetView::Dump_Load_post() {
  inherited::Dump_Load_post();
  if(taMisc::is_undo_loading) return; // none of this.
  // do full rebuild!
  Reset();
  BuildAll();
  Render();
}

taBase::DumpQueryResult NetView::Dump_QuerySaveMember(MemberDef* md) {
  // don't save Layers/Prjn's, they have no persist state, and just get rebuilt
  if (md->name == "children") {
    return DQR_NO_SAVE;
  } else
    return inherited::Dump_QuerySaveMember(md);
}

UnitView* NetView::FindUnitView(Unit* unit) {
  UnitView* uv = NULL;
  taDataLink* dl = unit->data_link();
  if (!dl) return NULL;
  taDataLinkItr i;
  FOR_DLC_EL_OF_TYPE(UnitView, uv, dl, i) {
    if (uv->GetOwner(&TA_NetView) == this)
      return uv;
  }
  return NULL;
}

// this fills a member group with the valid memberdefs from the units and connections
void NetView::GetMembs() {
  if(!net()) return;

  // try as hard as possible to find a unit to view if nothing selected -- this
  // minimizes issues with history etc
  if(!unit_src) {
    if(unit_src_path.nonempty()) {
      MemberDef* umd;
      Unit* nu = (Unit*)net()->FindFromPath(unit_src_path, umd);
      if(nu) setUnitSrc(NULL, nu);
    }
    if(!unit_src && net()->layers.leaves > 0) {
      Layer* lay = net()->layers.Leaf(net()->layers.leaves-1);
      if(lay->units.leaves > 0)
        setUnitSrc(NULL, lay->units.Leaf(0));
    }
  }

  setUnitDispMd(NULL);
  membs.Reset();
  TypeDef* prv_td = NULL;

  // first do the unit variables
  FOREACH_ELEM_IN_GROUP(Layer, lay, net()->layers) {
    FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
      TypeDef* td = u->GetTypeDef();
      if(td == prv_td) continue; // don't re-scan!
      prv_td = td;

      for(int m=0; m<td->members.size; m++) {
        MemberDef* md = td->members.FastEl(m);
        if((md->HasOption("NO_VIEW") || md->HasOption("HIDDEN") ||
            md->HasOption("READ_ONLY")))
          continue;
        if((md->type->InheritsFrom(&TA_float) || md->type->InheritsFrom(&TA_double))
           && (membs.FindName(md->name)==NULL))
        {
          MemberDef* nmd = md->Clone();
          membs.Add(nmd);       // index now reflects position in list...
          nmd->idx = md->idx;   // so restore it to the orig one
        }       // check for nongroup owned sub fields (ex. bias)
        else if(md->type->DerivesFrom(&TA_taBase) && !md->type->DerivesFrom(&TA_taGroup)) {
          if(md->type->ptr > 1) continue; // only one level of pointer tolerated
          TypeDef* nptd;
          if(md->type->ptr > 0) {
            taBase** par_ptr = (taBase**)md->GetOff((void*)u);
            if(*par_ptr == NULL) continue; // null pointer
            nptd = (*par_ptr)->GetTypeDef(); // get actual type of connection
          }
          else
            nptd = md->type;
          int k;
          for(k=0; k<nptd->members.size; k++) {
            MemberDef* smd = nptd->members.FastEl(k);
            if(smd->type->InheritsFrom(&TA_float) || smd->type->InheritsFrom(&TA_double)) {
              if((smd->HasOption("NO_VIEW") || smd->HasOption("HIDDEN") ||
                  smd->HasOption("READ_ONLY")))
                continue;
              String nm = md->name + "." + smd->name;
              if(membs.FindName(nm)==NULL) {
                MemberDef* nmd = smd->Clone();
                nmd->name = nm;
                membs.Add(nmd);
                nmd->idx = smd->idx;
              }
            }
          }
        }
      }

      // then do bias weights
      if(u->bias.size) {
        TypeDef* td = u->bias.con_type;
        for(int k=0; k<td->members.size; k++) {
          MemberDef* smd = td->members.FastEl(k);
          if(smd->type->InheritsFrom(&TA_float) || smd->type->InheritsFrom(&TA_double)) {
            if((smd->HasOption("NO_VIEW") || smd->HasOption("HIDDEN") ||
                smd->HasOption("READ_ONLY")))
              continue;
            String nm = "bias." + smd->name;
            if(membs.FindName(nm)==NULL) {
              MemberDef* nmd = smd->Clone();
              nmd->name = nm;
              membs.Add(nmd);
              nmd->idx = smd->idx;
            }
          }
        }
      }
    }
  }

  // then, only do the connections if any Unit guys, otherwise we are
  // not built yet, so we leave ourselves empty to signal that
  if (membs.size > 0) {
    FOREACH_ELEM_IN_GROUP(Layer, lay, net()->layers) {
      FOREACH_ELEM_IN_GROUP(Projection, prjn, lay->projections) {
        TypeDef* td = prjn->con_type;
        if(td == prv_td) continue; // don't re-scan!
        prv_td = td;
        int k;
        for (k=0; k<td->members.size; k++) {
          MemberDef* smd = td->members.FastEl(k);
          if(smd->type->InheritsFrom(&TA_float) || smd->type->InheritsFrom(&TA_double)) {
            if((smd->HasOption("NO_VIEW") || smd->HasOption("HIDDEN") ||
                smd->HasOption("READ_ONLY")))
              continue;
            String nm = "r." + smd->name;
            if(membs.FindName(nm)==NULL) {
              MemberDef* nmd = smd->Clone();
              nmd->name = nm;
              membs.Add(nmd);
              nmd->idx = smd->idx;
            }
            nm = "s." + smd->name;
            if(membs.FindName(nm)==NULL) {
              MemberDef* nmd = smd->Clone();
              nmd->name = nm;
              membs.Add(nmd);
              nmd->idx = smd->idx;
            }
          }
        }
      }
    }
  }

  // remove any stale items from sel list, but only if we were built
  if (membs.size > 0) {
    String_Array& oul = cur_unit_vals;
    for (int i=oul.size-1; i>=0; i--) {
      if (!membs.FindName(oul[i]))
        oul.RemoveIdx(i,1);
    }
  }
}

void NetView::InitCtrHist(bool force) {
  int chld_idx = 0;
  TypeDef* td = net()->GetTypeDef();
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    if(net()->HasUserData(md->name) && !net()->GetUserDataAsBool(md->name)) continue;
    chld_idx++;
  }
  n_counters = chld_idx;
  MatrixGeom nwgm(2, n_counters, hist_max);
  bool init_idx = force;
  if(ctr_hist.geom != nwgm) {
    ctr_hist.SetGeomN(nwgm); // just set here -- likely to be same..
    init_idx = true;
  }
  if(init_idx) {
    ctr_hist_idx.Reset();
    hist_reset_req = true;      // tell everyone about it
    hist_idx = 0;
  }
}

void NetView::GetUnitColor(float val,  iColor& col, float& sc_val) {
  iColor fl;  iColor tx;
  scale.GetColor(val,sc_val,&fl,&tx);
  col = fl;
}

void NetView::GetUnitDisplayVals(UnitGroupView* ugrv, taVector2i& co, float& val, T3Color& col,
                                 float& sc_val) {
  sc_val = scale.zero;
  void* base = NULL;
  if(unit_disp_md && unit_md_flags != MD_UNKNOWN)
    val = ugrv->GetUnitDisplayVal(co, base);
  if(!base) {
    col.setValue(.8f, .8f, .8f); // lt gray
    return;
  }
  iColor tc;
  GetUnitColor(val, tc, sc_val);
  col.setValue(tc.redf(), tc.greenf(), tc.bluef());
}

void NetView::InitDisplay(bool init_panel) {
  // note: init display is kind of an odd bird -- subsumed under Render but also avail
  // independently -- deals with some kinds of changes but not really full structural
  // I guess it is just a "non structural state update" container..
  GetMaxSize();
  GetMembs();

  hist_reset_req = false;       // this flag is used to sync history index resetting among
                                // all the history elements in unit groups and network
  InitCtrHist();

  if (init_panel) {
    InitPanel();
    UpdatePanel();
  }
  // select "act" by default, if nothing selected, or saved selection not restored yet (first after load) -- skip if still not initialized
  if (!unit_disp_md && (membs.size > 0)) {
    MemberDef* md = NULL;
    if (cur_unit_vals.size > 0) {
      md = membs.FindName(cur_unit_vals[0]);
    }
    if(md) {
      setUnitDispMd(md);
    }
    else { // default
      SelectVar("act", false, false);
    }
  }

  // this init display will also set hist_reset_req for unit groups, if needed
  if(children.size > 0) {
    LayerGroupView* lv = (LayerGroupView*)children.FastEl(0);
    lv->InitDisplay();
  }
  if(hist_reset_req) {          // someone reset history somewhere -- sync everyone!
    InitCtrHist(true);          // force!
    if(children.size > 0) {
      LayerGroupView* lv = (LayerGroupView*)children.FastEl(0);
      lv->InitDisplay();        // unit groups will reset in here
    }
  }
  hist_reset_req = false;
}

void NetView::InitPanel() {
  if (!nvp) return;
  nvp->InitPanel();
}

void NetView::InitScaleRange(ScaleRange& sr) {
  //TODO: initialize these from a master copy kept as defaults for the application
  // each type, leabra, bp, etc. can have customized defaults
//TEMP: just set from -1 to 1, with no auto
  sr.auto_scale = false;
  sr.min = -1.0f;
  sr.max =  1.0f;
}

void NetView::Layer_DataUAE(LayerView* lv) {
  // simplest solution is just to call DataUAE on all prns...
  for (int i = 0; i < prjns.size; ++i) {
    PrjnView* pv = (PrjnView*)prjns.FastEl(i);
    pv->DataUpdateAfterEdit_impl();
  }
}

void NetView::NewLayer(int x, int y) {
  Layer* lay = net()->layers.NewEl(1);
  lay->pos.x = x; lay->pos.y = y;
  lay->UpdateAfterEdit();
}

void NetView::GetMaxSize() {
  if(!net()) return;
  net()->UpdateMaxDispSize();
  max_size = net()->max_disp_size;
  max_size.z -= (max_size.z - 1.0f) / max_size.z; // leave 1 extra layer's worth of room..
  if(view_params.xy_square) {
    max_size.x = MAX(max_size.x, max_size.y);
    max_size.y = max_size.x;
  }
}

void NetView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!nvp) {
    nvp = new NetViewPanel(this);
    vw->RegisterPanel(nvp);
    vw->t3vs->Connect_SelectableHostNotifySignal(nvp,
      SLOT(viewWin_NotifySignal(ISelectableHost*, int)) );
  }
}

const iColor NetView::bgColor(bool& ok) const {
  ok = true;
  return scale.background;
}


void NetView::Render_pre() {
  if(!no_init_on_rerender)
    InitDisplay();
  no_init_on_rerender = false;

  bool show_drag = true;;
  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->syncViewerMode();
    show_drag = vw->interactionModeOn();
  }
  if(!lay_mv) show_drag = false;

  setNode(new T3NetNode(this, show_drag, net_text, show_drag && lay_mv));
  SoMaterial* mat = node_so()->material(); //cache
  mat->diffuseColor.setValue(0.0f, 0.5f, 0.5f); // blue/green
  mat->transparency.setValue(0.5f);

  if(vw && vw->interactionModeOn()) {
    SoEventCallback* ecb = new SoEventCallback;
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), UnitGroupView_MouseCB, this);
    node_so()->addChild(ecb);
  }

  if(vw) {                      // add hot buttons to viewer
    MemberDef* md;
    for (int i=0; i < membs.size; i++) {
      md = membs[i];
      if(!md->HasOption("VIEW_HOT")) continue;
      vw->addDynButton(md->name, md->desc); // add the button
    }
  }
  inherited::Render_pre();
}

// callback for netview transformer dragger
void T3NetNode_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3NetNode* netnd = (T3NetNode*)userData;
  NetView* nv = static_cast<NetView*>(netnd->dataView());

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->main_xform.rotate.x, nv->main_xform.rotate.y,
                           nv->main_xform.rotate.z), nv->main_xform.rotate.rot);

  SbVec3f trans = dragger->translation.getValue();
  cur_rot.multVec(trans, trans); // rotate the translation by current rotation
  trans[0] *= nv->main_xform.scale.x;  trans[1] *= nv->main_xform.scale.y;
  trans[2] *= nv->main_xform.scale.z;
  taVector3f tr(trans[0], trans[1], trans[2]);
  nv->main_xform.translate += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
  taVector3f sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;        // prevent scale from going to small too fast!!
  nv->main_xform.scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);
    nv->main_xform.rotate.SetXYZR(axis[0], axis[1], axis[2], angle);
  }

  float h = 0.04f; // nominal amount of height, so we don't vanish
  netnd->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  netnd->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  netnd->txfm_shape()->translation.setValue(.5f, .5f * h, -.5f);
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

  nv->UpdateDisplay();
}

// callback for netview net text transformer dragger
void T3NetText_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3NetNode* netnd = (T3NetNode*)userData;
  NetView* nv = static_cast<NetView*>(netnd->dataView());

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->net_text_xform.rotate.x, nv->net_text_xform.rotate.y,
                           nv->net_text_xform.rotate.z), nv->net_text_xform.rotate.rot);

  SbVec3f trans = dragger->translation.getValue();
  cur_rot.multVec(trans, trans); // rotate the translation by current rotation
  trans[0] *= nv->net_text_xform.scale.x;  trans[1] *= nv->net_text_xform.scale.y;
  trans[2] *= nv->net_text_xform.scale.z;
  taVector3f tr(trans[0], trans[1], trans[2]);
  nv->net_text_xform.translate += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
  taVector3f sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;        // prevent scale from going to small too fast!!
  nv->net_text_xform.scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);

    nv->net_text_xform.rotate.SetXYZR(axis[0], axis[1], axis[2], angle);
  }

  // reset the drag guy: note that drag is still connected to the drag xform so you
  // need to reset dragger first, then the xform!
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  netnd->netTextDragXform()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  netnd->netTextDragXform()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  netnd->netTextDragXform()->translation.setValue(0.0f, 0.0f, 0.0f);

  nv->UpdateDisplay();
}

void NetView::Render_impl() {
  // font properties percolate down to all other elements, unless set there
  taTransform* ft = transform(true);
  *ft = main_xform;

  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->syncViewerMode();
    if(unit_disp_md) {
      int but_no = vw->dyn_buttons.FindName(unit_disp_md->name);
      if(but_no >= 0) {
        vw->setDynButtonChecked(but_no, true, true); // mutex
      }
      else {
        vw->setDynButtonChecked(0, false, true); // mutex -- turn all off
      }
    }
  }

  GetMaxSize();
  T3NetNode* node_so = this->node_so(); //cache
  if(!node_so) return;
  node_so->resizeCaption(font_sizes.net_name);

  String cap_txt = data()->GetName() + " Value: ";
  if(unit_disp_md)
    cap_txt += unit_disp_md->name;

  if(node_so->shapeDraw())
    node_so->shapeDraw()->lineWidth = view_params.laygp_width;

  node_so->setCaption(cap_txt.chars());

  if (scale.auto_scale) {
    UpdateAutoScale();
    if (nvp) {
      nvp->ColorScaleFromData();
    }
  }

  if(net_text) {
    SoTransform* tx = node_so->netTextXform();
    net_text_xform.CopyTo(tx);

    Render_net_text();
  }

  if((bool)wt_prjn_lay) {
    // this does all the heavy lifting: projecting into unit wt_prjn
    // if wt_line_thr < 0 then zero intermediates
    net()->ProjectUnitWeights(unit_src, (int)wt_prjn_k_un, (int)wt_prjn_k_gp,
              wt_line_swt, (wt_prjn_k_un > 0 && wt_line_thr < 0.0f));
  }

  Render_wt_lines();

  inherited::Render_impl();
  //  taiMisc::RunPending();
}

void NetView::Render_net_text() {
  T3NetNode* node_so = this->node_so(); //cache
  SoSeparator* net_txt = node_so->netText();
  if(!net_txt) return;          // screwup

  TypeDef* td = net()->GetTypeDef();
  int per_row = 2;

  int chld_idx = 0;
  int cur_row = 0;
  int cur_col = 0;
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    if(net()->HasUserData(md->name) && !net()->GetUserDataAsBool(md->name)) continue;
    chld_idx++;
    if(md->type->InheritsFrom(&TA_taString) || md->type->InheritsFormal(&TA_enum)) {
      if(cur_col > 0) {
        cur_row++;
        cur_col=0;
      }
      cur_row++;
      cur_col=0;
    }
    else {
      cur_col++;
      if(cur_col >= per_row) {
        cur_col = 0;
        cur_row++;
      }
    }
  }
  int n_rows = cur_row;
//  int n_texts = chld_idx;

  int txt_st_off = 3 + 1;       // 3 we add below + 1 transform
  if(node_so->netTextDrag())
    txt_st_off+=2;              // dragger + extra xform

  bool build_text = false;

  if(net_txt->getNumChildren() < txt_st_off) { // haven't made basic guys yet
    T3DataViewFrame* fr = GetFrame();
    iColor txtcolr = fr->GetTextColor();
    build_text = true;
    SoBaseColor* bc = new SoBaseColor;
    bc->rgb.setValue(txtcolr.redf(), txtcolr.greenf(), txtcolr.bluef());
    net_txt->addChild(bc);
    // doesn't seem to make much diff:
    SoComplexity* cplx = new SoComplexity;
    cplx->value.setValue(taMisc::text_complexity);
    net_txt->addChild(cplx);
    SoFont* fnt = new SoFont();
    fnt->size.setValue(font_sizes.net_vals);
    fnt->name = (const char*)taMisc::t3d_font_name;
    net_txt->addChild(fnt);
  }
  else if(net_txt->getNumChildren() != txt_st_off + chld_idx) {
    // if not adding up, nuke existing and rebuild
    int nc = net_txt->getNumChildren();
    for(int i=nc-1;i>=txt_st_off;i--) {
      net_txt->removeChild(i);
    }
    build_text = true;
  }

  float rot_rad = net_text_rot / taMath_float::deg_per_rad;

  chld_idx = 0;
  cur_row = 0;
  cur_col = 0;
  // todo: could optimize 1st 3 counters to be on 1 row to save a row..
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    if(net()->HasUserData(md->name) && !net()->GetUserDataAsBool(md->name)) continue;
    if(build_text) {
      SoSeparator* tsep = new SoSeparator;
      net_txt->addChild(tsep);
      SoTransform* tr = new SoTransform;
      tsep->addChild(tr);
      bool cur_str = false;
      if((md->type->InheritsFrom(&TA_taString) || md->type->InheritsFormal(&TA_enum))) {
        cur_str = true;
        if(cur_col > 0) { // go to next
          cur_row++;
          cur_col=0;
        }
      }
      float xv = 0.05f + (float)cur_col / (float)(per_row);
      float yv = ((float)(cur_row+1.0f) / (float)(n_rows + 2.0f));
      tr->translation.setValue(xv, 0.0f, -yv);
      tr->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), rot_rad);
      SoAsciiText* txt = new SoAsciiText();
      txt->justification = SoAsciiText::LEFT;
      tsep->addChild(txt);
      if(cur_str) {
        cur_row++;
        cur_col=0;
      }
      else {
        cur_col++;
        if(cur_col >= per_row) {
          cur_col = 0;
          cur_row++;
        }
      }
    }
    SoSeparator* tsep = (SoSeparator*)net_txt->getChild(chld_idx + txt_st_off);
    SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
    String el = md->name + ": ";
    String val = md->type->GetValStr(md->GetOff((void*)net()));;
    if(hist_idx > 0) {
      int cidx = (ctr_hist_idx.length - hist_idx);
      int midx = ctr_hist_idx.CircIdx(cidx);
      if(ctr_hist.InRange(chld_idx, midx)) {
        val = ctr_hist.SafeEl(chld_idx, midx);
      }
    }
    el += val;
    txt->string.setValue(el.chars());
    chld_idx++;
  }
}

void NetView::Render_wt_lines() {
  T3NetNode* node_so = this->node_so(); //cache

  bool do_lines = (bool)unit_src && wt_line_disp;
  Layer* src_lay = NULL;
  if(unit_src)
    src_lay =GET_OWNER(unit_src, Layer);
  if(!src_lay || src_lay->Iconified()) do_lines = false;
  SoIndexedLineSet* ils = node_so->wtLinesSet();
  SoDrawStyle* drw = node_so->wtLinesDraw();
  SoVertexProperty* vtx_prop = node_so->wtLinesVtxProp();

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
  taVector3i src_lay_pos; src_lay->GetAbsPos(src_lay_pos);

  drw->style = SoDrawStyleElement::LINES;
  drw->lineWidth = MAX(wt_line_width, 0.0f);
  vtx_prop->materialBinding.setValue(SoMaterialBinding::PER_PART_INDEXED); // part = line segment = same as FACE but likely to be faster to compute line segs?

  // count the number of lines etc
  int n_prjns = 0;
  int n_vtx = 0;
  int n_coord = 0;
  int n_mat = 0;

  bool swt = wt_line_swt;

  if(wt_line_width >= 0.0f) {
    for(int g=0;g<(swt ? unit_src->send.size : unit_src->recv.size);g++) {
      taOBase* cg = (swt ? (taOBase*)unit_src->send.FastEl(g) : (taOBase*)unit_src->recv.FastEl(g));
      Projection* prjn = (swt ? ((SendCons*)cg)->prjn : ((RecvCons*)cg)->prjn);
      if(!prjn || !prjn->from || !prjn->layer) continue;
      if(prjn->from->Iconified()) continue;

      n_prjns++;
      int n_con = 0;
      for(int i=0;i<(swt ? ((SendCons*)cg)->size : ((RecvCons*)cg)->size); i++) {
        float wt = (swt ? ((SendCons*)cg)->Cn(i)->wt : ((RecvCons*)cg)->Cn(i)->wt);
        if(wt >= wt_line_thr) n_con++;
      }

      n_vtx += 1 + n_con;   // one for recv + senders
      n_coord += 3 * n_con; // start, end -1 for each coord
      n_mat += n_con;       // one per line
    }
  }

  if((bool)wt_prjn_lay && (wt_line_width >= 0.0f)) {
    int n_con = 0;
    FOREACH_ELEM_IN_GROUP(Unit, u, wt_prjn_lay->units) {
      if(fabsf(u->wt_prjn) >= wt_line_thr) n_con++;
    }

    n_vtx += 1 + n_con;   // one for recv + senders
    n_coord += 3 * n_con; // start, end -1 for each coord
    n_mat += n_con;       // one per line
  }

  vertex.setNum(n_vtx);
  coords.setNum(n_coord);
  color.setNum(n_mat);
  mats.setNum(n_mat);

  if(wt_line_width < 0.0f) return;

  SbVec3f* vertex_dat = vertex.startEditing();
  int32_t* coords_dat = coords.startEditing();
  uint32_t* color_dat = color.startEditing();
  int32_t* mats_dat = mats.startEditing();
  int v_idx = 0;
  int c_idx = 0;
  int cidx = 0;
  int midx = 0;

  // note: only want layer_rel for ru_pos
  taVector2i ru_pos; unit_src->LayerDispPos(ru_pos);
  taVector3f src;             // source and dest coords
  taVector3f dst;

  float max_xy = MAX(max_size.x, max_size.y);
  float lay_ht = T3LayerNode::height / max_xy;

  iColor tc;
  T3Color col;
  float sc_val;
  float trans = view_params.unit_trans;

  for(int g=0;g<(swt ? unit_src->send.size : unit_src->recv.size);g++) {
    taOBase* cg = (swt ? (taOBase*)unit_src->send.FastEl(g) : (taOBase*)unit_src->recv.FastEl(g));
    Projection* prjn = (swt ? ((SendCons*)cg)->prjn : ((RecvCons*)cg)->prjn);
    if(!prjn || !prjn->from || !prjn->layer) continue;
    if(prjn->from->Iconified()) continue;
    Layer* lay_fr = (swt ? prjn->layer : prjn->from);
    Layer* lay_to = (swt ? prjn->from : prjn->layer);
    taVector3i lay_fr_pos; lay_fr->GetAbsPos(lay_fr_pos);
    taVector3i lay_to_pos; lay_to->GetAbsPos(lay_to_pos);

    // y = network z coords -- same for all cases
    src.y = ((float)lay_to_pos.z) / max_size.z;
    dst.y = ((float)lay_fr_pos.z) / max_size.z;

    // move above/below layer plane
    if(src.y < dst.y) { src.y += lay_ht; dst.y -= lay_ht; }
    else if(src.y > dst.y) { src.y -= lay_ht; dst.y += lay_ht; }
    else { src.y += lay_ht; dst.y += lay_ht; }

    src.x = ((float)lay_to_pos.x + (float)ru_pos.x + .5f) / max_size.x;
    src.z = -((float)lay_to_pos.y + (float)ru_pos.y + .5f) / max_size.y;

    int ru_idx = v_idx;
    vertex_dat[v_idx++].setValue(src.x, src.y, src.z);

    for(int i=0;i<(swt ? ((SendCons*)cg)->size : ((RecvCons*)cg)->size); i++) {
      Unit* su = (swt ? ((SendCons*)cg)->Un(i) : ((RecvCons*)cg)->Un(i));
      float wt = (swt ? ((SendCons*)cg)->Cn(i)->wt : ((RecvCons*)cg)->Cn(i)->wt);
      if(fabsf(wt) < wt_line_thr) continue;

      // note: only want layer_rel for ru_pos
      taVector2i su_pos; su->LayerDispPos(su_pos);
      dst.x = ((float)lay_fr_pos.x + (float)su_pos.x + .5f) / max_size.x;
      dst.z = -((float)lay_fr_pos.y + (float)su_pos.y + .5f) / max_size.y;

      coords_dat[cidx++] = ru_idx; coords_dat[cidx++] = v_idx; coords_dat[cidx++] = -1;
      mats_dat[midx++] = c_idx; // one per

      vertex_dat[v_idx++].setValue(dst.x, dst.y, dst.z);

      // color
      GetUnitColor(wt, tc, sc_val);
      col.setValue(tc.redf(), tc.greenf(), tc.bluef());
      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      color_dat[c_idx++] = T3Color::makePackedRGBA(col.r, col.g, col.b, alpha);
    }
  }

  if((bool)wt_prjn_lay) {
    taVector3i wt_prjn_lay_pos; wt_prjn_lay->GetAbsPos(wt_prjn_lay_pos);

    // y = network z coords -- same for all cases
    src.y = ((float)src_lay_pos.z) / max_size.z;
    dst.y = ((float)wt_prjn_lay_pos.z) / max_size.z;

    // move above/below layer plane
    if(src.y < dst.y) { src.y += lay_ht; dst.y -= lay_ht; }
    else if(src.y > dst.y) { src.y -= lay_ht; dst.y += lay_ht; }
    else { src.y += lay_ht; dst.y += lay_ht; }

    src.x = ((float)src_lay_pos.x + (float)ru_pos.x + .5f) / max_size.x;
    src.z = -((float)src_lay_pos.y + (float)ru_pos.y + .5f) / max_size.y;

    int ru_idx = v_idx;
    vertex_dat[v_idx++].setValue(src.x, src.y, src.z);

    FOREACH_ELEM_IN_GROUP(Unit, su, wt_prjn_lay->units) {
      float wt = su->wt_prjn;
      if(fabsf(wt) < wt_line_thr) continue;

      taVector3i su_pos; su->GetAbsPos(su_pos);
      dst.x = ((float)wt_prjn_lay_pos.x + (float)su_pos.x + .5f) / max_size.x;
      dst.z = -((float)wt_prjn_lay_pos.y + (float)su_pos.y + .5f) / max_size.y;

      coords_dat[cidx++] = ru_idx; coords_dat[cidx++] = v_idx; coords_dat[cidx++] = -1;
      mats_dat[midx++] = c_idx; // one per

      vertex_dat[v_idx++].setValue(dst.x, dst.y, dst.z);

      // color
      GetUnitColor(wt, tc, sc_val);
      col.setValue(tc.redf(), tc.greenf(), tc.bluef());
      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      color_dat[c_idx++] = T3Color::makePackedRGBA(col.r, col.g, col.b, alpha);
    }
  }

  vertex.finishEditing();
  color.finishEditing();
  coords.finishEditing();
  mats.finishEditing();
}

void NetView::Reset_impl() {
  hist_idx = 0;
  ctr_hist_idx.Reset();
  prjns.Reset();
  inherited::Reset_impl();
}

void NetView::SelectVar(const char* var_name, bool add, bool update) {
  if (!add)
    cur_unit_vals.Reset();
  MemberDef* md = (MemberDef*)membs.FindName(var_name);
  if (md) {
    //nnif (cur_unit_vals.FindEl(var_name) < 0)
    cur_unit_vals.AddUnique(var_name);
  }
  setUnitDispMd(md);
  if (update) UpdateDisplay();
}

void NetView::SetScaleData(bool auto_scale_, float min_, float max_, bool update_panel) {
  // the cbar is automatically linked to the scale, so it always has these vals!!
//   if ((scale.auto_scale == auto_scale_) && (scale.min == min_) && (scale.max == max_)) return;
  scale.auto_scale = auto_scale_;
  if (!auto_scale_)
    scale.SetMinMax(min_, max_);
  if (unit_scrng) {
    unit_scrng->SetFromScale(scale);
  }
  if (update_panel) UpdatePanel();
}

void NetView::SetScaleDefault() {
  if (unit_scrng) {
    InitScaleRange(*unit_scrng);
    unit_scrng->SetFromScaleRange(scale);
  }
  else {
    scale.auto_scale = false;
    scale.SetMinMax(-1.0f, 1.0f);
  }
}

void NetView::SetColorSpec(ColorScaleSpec* color_spec) {
  scale.SetColorSpec(color_spec);
  UpdateDisplay(true);          // true causes button to remain pressed..
}

void NetView::setUnitSrc(UnitView* uv, Unit* unit) {
  if (unit_src.ptr() == unit) return; // no change
  // if there was existing unit, unpick it
  if ((bool)unit_src) {
    UnitView* uv_src = FindUnitView(unit_src);
    if (uv_src) {
      uv_src->picked = false;
    }
  }
  unit_src = unit;
  if ((bool)unit_src) {
    if(uv)
      uv->picked = true;
    unit_src_path = unit_src->GetPath(NULL, net());
  }
  hist_idx = 0;                 // reset index to current time for new unit selection
}

void NetView::setUnitDisp(int value) {
  MemberDef* md = membs.PosSafeEl(value);
  setUnitDispMd(md);
}

void NetView::setUnitDispMd(MemberDef* md) {
  if(md == unit_disp_md) return;
  unit_disp_md = md;
  unit_disp_idx = membs.FindEl(md);
  unit_scrng = NULL;
  unit_md_flags = MD_UNKNOWN;
  unit_con_md = false;
  if (!unit_disp_md) return;
  if (unit_disp_md) {
    if (unit_disp_md->type->InheritsFrom(&TA_float))
      unit_md_flags = MD_FLOAT;
    else if (unit_disp_md->type->InheritsFrom(&TA_double))
      unit_md_flags = MD_DOUBLE;
    else if (unit_disp_md->type->InheritsFrom(&TA_int))
      unit_md_flags = MD_INT;
  }
  if(unit_disp_md->name.startsWith("r.") || md->name.startsWith("s."))
    unit_con_md = true;
  String nm = unit_disp_md->name;
  unit_scrng = scale_ranges.FindName(nm);
  if (unit_scrng == NULL) {
    unit_scrng = (ScaleRange*)(scale_ranges.New(1,&TA_ScaleRange));
    unit_scrng->var_name = nm;
    InitScaleRange(*unit_scrng);
  }
  unit_scrng->SetFromScaleRange(scale);
}

void NetView::UpdateViewerModeForMd(MemberDef* md) {
  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    no_init_on_rerender = true; // prevent loss of history..
    if(md->name.startsWith("r.") || md->name.startsWith("s.")) {
      vw->setInteractionModeOn(true, true); // select! -- true = re-render
    }
    else {
      vw->setInteractionModeOn(false, true); // not needed -- true = re-render
    }
  }
}


void NetView::SetHighlightSpec(BaseSpec* spec) {
  if(children.size > 0) {
    LayerGroupView* lv = (LayerGroupView*)children.FastEl(0);
    lv->SetHighlightSpec(spec);
  }
  // check projections
  for(int i = 0; i < prjns.size; ++i) {
    PrjnView* pv = (PrjnView*)prjns.FastEl(i);
    pv->SetHighlightSpec(spec);
  }
}

bool NetView::UsesSpec(taBase* obj, BaseSpec* spec) {
  if(!spec) return false;
  TypeDef* otyp = obj->GetTypeDef();
  for (int i = 0; i < otyp->members.size; ++i) {
    MemberDef* md = otyp->members.FastEl(i);
    //note: specs stored in specptr objs, which are inline objs
    if (md->type->DerivesFrom(&TA_SpecPtr_impl)) {
      SpecPtr_impl* sptr = (SpecPtr_impl*)md->GetOff(obj);
      if (sptr->GetSpec() == spec) return true;
    }
  }
  if(spec->InheritsFrom(&TA_ConSpec) && obj->InheritsFrom(&TA_Layer)) {
    // could be a bias spec..
    Layer* lay = (Layer*)obj;
    UnitSpec* us = (UnitSpec*)lay->unit_spec.GetSpec();
    if(us) {
      ConSpec* bs = (ConSpec*)us->bias_spec.GetSpec();
      if(bs == spec) return true;
    }
  }
  return false;
}

void NetView::UpdateAutoScale() {
  bool updated = false;
  if(children.size > 0) {
    LayerGroupView* lv = (LayerGroupView*)children.FastEl(0);
    lv->UpdateAutoScale(updated);
  }
  if (updated) { //note: could really only not be updated if there were no items
    scale.SymRange();           // keep it symmetric
    if (unit_scrng) {
      unit_scrng->SetFromScale(scale); // update values
    }
    if (nvp) {
      nvp->ColorScaleFromData();
    }
  } else { // set scale back to default so it doesn't look bizarre
    scale.SetMinMax(-1.0f, 1.0f);
  }
}

void NetView::UpdateDisplay(bool update_panel) { // redoes everything
  if (update_panel) UpdatePanel();
//   taMisc::Info("UpdateDisplay");
  Render_impl();
}

void NetView::UpdateUnitValues() { // *actually* only does unit value updating
  if(children.size == 0) return;
  if(hist_save)
    SaveCtrHist();
  if (scale.auto_scale) {
    UpdateAutoScale();
    if (nvp) {
      nvp->ColorScaleFromData();
    }
  }
//   taMisc::Info("UpdateUnitValues");
  LayerGroupView* lv = (LayerGroupView*)children.FastEl(0);
  lv->UpdateUnitValues();
}

void NetView::SaveCtrHist() {
  TypeDef* td = net()->GetTypeDef();

  // bump up the frame circ idx..
  int circ_idx = ctr_hist_idx.CircAddLimit(hist_max);
  int eff_hist_idx = ctr_hist_idx.CircIdx(circ_idx);

  int chld_idx = 0;
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    if(net()->HasUserData(md->name) && !net()->GetUserDataAsBool(md->name)) continue;
    String val = md->type->GetValStr(md->GetOff((void*)net()));
    ctr_hist.Set(val, chld_idx, eff_hist_idx);
    chld_idx++;
  }
}

void NetView::DataUpdateView_impl() {
  if (!display) return;
  HistFwdAll();			// update to current point in history when updated externally
  UpdateUnitValues();
  if(net_text) {
    Render_net_text();
  }
  Render_wt_lines();
}

void NetView::UpdatePanel() {
  if (!nvp) return;
  nvp->UpdatePanel();
}

void NetView::viewWin_NotifySignal(ISelectableHost* src, int op) {
  if (op != ISelectableHost::OP_SELECTION_CHANGED) return;
  ISelectable* ci = src->curItem(); // first selected item, if any
  if (!ci) return;
  TypeDef* typ = ci->GetTypeDef();
  if (!typ->InheritsFrom(&TA_UnitView)) return;
  UnitView* uv = (UnitView*)ci->This();
  Unit* unit_new = uv->unit();
  setUnitSrc(uv, unit_new);
  InitDisplay();
  UpdateDisplay();
}

// all the slots:


//////////////////////////
//   NetViewPanel       //
//////////////////////////

NetViewPanel::NetViewPanel(NetView* dv_)
:inherited(dv_)
{
  int font_spec = taiMisc::fonMedium;
  m_cur_spec = NULL;
  req_full_render = false;
  req_full_build = false;

  T3ExaminerViewer* vw = dv_->GetViewer();
  if(vw) {
    connect(vw, SIGNAL(dynbuttonActivated(int)), this, SLOT(dynbuttonActivated(int)) );
    connect(vw, SIGNAL(unTrappedKeyPressEvent(QKeyEvent*)), this, SLOT(unTrappedKeyPressEvent(QKeyEvent*)) );
  }

  QWidget* widg = new QWidget();
  layTopCtrls = new QVBoxLayout(widg); //layWidg->addLayout(layTopCtrls);
  layTopCtrls->setSpacing(2);
  layTopCtrls->setMargin(2);

  layViewParams = new QVBoxLayout(); layTopCtrls->addLayout(layViewParams);
  layViewParams->setSpacing(2);
  layViewParams->setMargin(0);

  ////////////////////////////////////////////////////////////////////////////
  layDispCheck = new QHBoxLayout();  layViewParams->addLayout(layDispCheck);
  chkDisplay = new QCheckBox("Display", widg);
  connect(chkDisplay, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkDisplay);
  layDispCheck->addSpacing(taiM->hsep_c);

  chkLayMove = new QCheckBox("Lay\nMv", widg);
  chkLayMove->setToolTip("Turn on the layer moving controls when in the manipulation mode (red arrow) of viewer -- these can sometimes interfere with viewing weights, so you can turn them off here (but then you won't be able to move layers around in the GUI)");
  connect(chkLayMove, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkLayMove);
  layDispCheck->addSpacing(taiM->hsep_c);

  chkNetText = new QCheckBox("Net\nTxt", widg);
  chkNetText->setToolTip("Turn on the network text display at the base of the network, showing the current state of various counters and stats");
  connect(chkNetText, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkNetText);
  layDispCheck->addSpacing(taiM->hsep_c);

  lblTextRot = taiM->NewLabel("Txt\nRot", widg, font_spec);
  lblTextRot->setToolTip("Rotation of the network text in the Z axis -- set to -90 if text overall is rotated upright in the display");
  layDispCheck->addWidget(lblTextRot);
  fldTextRot = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layDispCheck->addWidget(fldTextRot->GetRep());
  ((iLineEdit*)fldTextRot->GetRep())->setCharWidth(6);
  layDispCheck->addSpacing(taiM->hsep_c);

  lblUnitText = taiM->NewLabel("Unit:\nText", widg, font_spec);
  lblUnitText->setToolTip("What text to display for each unit (values, names)");
  layDispCheck->addWidget(lblUnitText);
  cmbUnitText = dl.Add(new taiComboBox(true, TA_NetView.sub_types.FindName("UnitTextDisplay"),
                                this, NULL, widg, taiData::flgAutoApply));
  layDispCheck->addWidget(cmbUnitText->GetRep());
  layDispCheck->addSpacing(taiM->hsep_c);

  lblDispMode = taiM->NewLabel("Style", widg, font_spec);
  lblDispMode->setToolTip("How to display unit values.  3d Block (default) is optimized\n\
 for maximum speed.");
  layDispCheck->addWidget(lblDispMode);
  cmbDispMode = dl.Add(new taiComboBox(true, TA_NetView.sub_types.FindName("UnitDisplayMode"),
    this, NULL, widg, taiData::flgAutoApply));
  layDispCheck->addWidget(cmbDispMode->GetRep());
  layDispCheck->addSpacing(taiM->hsep_c);

  lblPrjnDisp = taiM->NewLabel("Prjn\nDisp", widg, font_spec);
  lblPrjnDisp->setToolTip("How to display projections between layers:\n\
L_R_F: Left = sender, Right = receiver, all arrows at the Front of the layer\n\
L_R_B: Left = sender, Right = receiver, all arrows at the Back of the layer\n\
B_F: Back = sender, Front = receiver, all arrows in the middle of the layer");
  layDispCheck->addWidget(lblPrjnDisp);
  cmbPrjnDisp = dl.Add(new taiComboBox(true, TA_NetViewParams.sub_types.FindName("PrjnDisp"),
                                this, NULL, widg, taiData::flgAutoApply));
  layDispCheck->addWidget(cmbPrjnDisp->GetRep());
  layDispCheck->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layFontsEtc = new QHBoxLayout();  layViewParams->addLayout(layFontsEtc);

  chkShowIconified = new QCheckBox("Show\nIcon", widg);
  chkShowIconified->setToolTip("Show iconified layers -- if this is off, then iconified layers are not displayed at all -- otherwise they are displayed with their name and optional iconified value, but projections are not displayed in any case");
  connect(chkShowIconified, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layFontsEtc->addWidget(chkShowIconified);

  lblPrjnWdth = taiM->NewLabel("Prjn\nWdth", widg, font_spec);
  lblPrjnWdth->setToolTip("Width of projection lines -- .002 is default (very thin!) -- increase if editing projections so they are easier to select.");
  layFontsEtc->addWidget(lblPrjnWdth);
  fldPrjnWdth = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldPrjnWdth->GetRep());
  ((iLineEdit*)fldPrjnWdth->GetRep())->setCharWidth(6);
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblUnitTrans = taiM->NewLabel("Trans\nparency", widg, font_spec);
  lblUnitTrans->setToolTip("Unit maximum transparency level: 0 = all units opaque; 1 = inactive units are completely invisible.\n .6 = default; transparency is inversely related to value magnitude.");
  layFontsEtc->addWidget(lblUnitTrans);
  fldUnitTrans = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldUnitTrans->GetRep());
  ((iLineEdit*)fldUnitTrans->GetRep())->setCharWidth(6);  layFontsEtc->addSpacing(taiM->hsep_c);

  lblUnitFont = taiM->NewLabel("Font\nSize", widg, font_spec);
  lblUnitFont->setToolTip("Unit text font size (as a proportion of entire network display). .02 is default.");
  layFontsEtc->addWidget(lblUnitFont);
  fldUnitFont = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldUnitFont->GetRep());
  ((iLineEdit*)fldUnitFont->GetRep())->setCharWidth(6);
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblLayFont = taiM->NewLabel("Layer\nFont Sz", widg, font_spec);
  lblLayFont->setToolTip("Layer name font size (as a proportion of entire network display). .04 is default.");
  layFontsEtc->addWidget(lblLayFont);
  fldLayFont = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldLayFont->GetRep());
  ((iLineEdit*)fldLayFont->GetRep())->setCharWidth(6);
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblMinLayFont = taiM->NewLabel("Min Sz", widg, font_spec);
  lblMinLayFont->setToolTip("Minimum layer name font size (as a proportion of entire network display) -- prevents font from shrinking too small for small layers. .01 is default.");
  layFontsEtc->addWidget(lblMinLayFont);
  fldMinLayFont = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldMinLayFont->GetRep());
  ((iLineEdit*)fldMinLayFont->GetRep())->setCharWidth(6);
  layFontsEtc->addSpacing(taiM->hsep_c);

  chkXYSquare = new QCheckBox("XY\nSquare", widg);
  chkXYSquare->setToolTip("Make the X and Y size of network the same, so that unit cubes are always square (but can waste a certain amount of display space).");
  connect(chkXYSquare, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layFontsEtc->addWidget(chkXYSquare);

  chkLayGp = new QCheckBox("Lay\nGp", widg);
  chkLayGp->setToolTip("Display boxes around layer groups.");
  connect(chkLayGp, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layFontsEtc->addWidget(chkLayGp);

  layFontsEtc->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layDisplayValues = new QVBoxLayout();  layTopCtrls->addLayout(layDisplayValues); //gbDisplayValues);
  layDisplayValues->setSpacing(2);
  layDisplayValues->setMargin(0);

  layColorScaleCtrls = new QHBoxLayout();  layDisplayValues->addLayout(layColorScaleCtrls);

  chkSnapBord = new QCheckBox("Snap\nBord", widg);
  chkSnapBord->setToolTip("Whether to display unit snapshot value snap as a border around units");
  connect(chkSnapBord, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layColorScaleCtrls->addWidget(chkSnapBord);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  lblSnapBordWdth = taiM->NewLabel("Bord\nWdth", widg, font_spec);
  lblSnapBordWdth->setToolTip("Width of snap border lines");
  layColorScaleCtrls->addWidget(lblSnapBordWdth);
  fldSnapBordWdth = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldSnapBordWdth->GetRep());
  ((iLineEdit*)fldSnapBordWdth->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  lblUnitSpacing = taiM->NewLabel("Unit\nSpace", widg, font_spec);
  lblUnitSpacing->setToolTip("Spacing between units, as a proportion of the total width of the unit box");
  layColorScaleCtrls->addWidget(lblUnitSpacing);
  fldUnitSpacing = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldUnitSpacing->GetRep());
  ((iLineEdit*)fldUnitSpacing->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  chkWtLines = new QCheckBox("wt\nLines", widg);
  chkWtLines->setToolTip("Whether to display connection weight values as colored lines, with color and transparency varying as a function of magnitude");
  connect(chkWtLines, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layColorScaleCtrls->addWidget(chkWtLines);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  chkWtLineSwt = new QCheckBox("s.wt", widg);
  chkWtLineSwt->setToolTip("Display the sending weights out of the unit instead of the receiving weights into it");
  connect(chkWtLineSwt, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layColorScaleCtrls->addWidget(chkWtLineSwt);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  lblWtLineWdth = taiM->NewLabel("Wdth", widg, font_spec);
  lblWtLineWdth->setToolTip("Width of weight lines -- 0 = thinnest lines (-1 = no lines, redundant with turning wt_lines off)");
  layColorScaleCtrls->addWidget(lblWtLineWdth);
  fldWtLineWdth = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldWtLineWdth->GetRep());
  ((iLineEdit*)fldWtLineWdth->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  lblWtLineThr = taiM->NewLabel("Thr", widg, font_spec);
  lblWtLineThr->setToolTip("Threshold for displaying weight lines: weight magnitudes below this value are not shown -- if a layer to project onto is selected (Wt Prjn) then if this value is < 0, intermediate units in the weight projection that are below the K un threshold will be zeroed.");
  layColorScaleCtrls->addWidget(lblWtLineThr);
  fldWtLineThr = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldWtLineThr->GetRep());
  ((iLineEdit*)fldWtLineThr->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  int list_flags = taiData::flgNullOk | taiData::flgAutoApply;

  lblWtPrjnLay = taiM->NewLabel("Wt\nPrjn", widg, font_spec);
  lblWtPrjnLay->setToolTip("Layer to project weight values onto, from currently selected unit in view -- values are visible on all units in the wt_prjn unit variable if this setting is non-null -- setting this value causes expensive weight projection computation for every update");
  layColorScaleCtrls->addWidget(lblWtPrjnLay);
  gelWtPrjnLay = dl.Add(new taiGroupElsButton(&TA_Layer_Group, this, NULL, widg, list_flags));
  layColorScaleCtrls->addWidget(gelWtPrjnLay->GetRep());

  lblWtPrjnKUn = taiM->NewLabel("K un", widg, font_spec);
  lblWtPrjnKUn->setToolTip("Number of top K strongest units to propagate weight projection values through to other layers -- smaller numbers produce more selective and often interpretable results, though they are somewhat less representative.");
  layColorScaleCtrls->addWidget(lblWtPrjnKUn);
  fldWtPrjnKUn = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldWtPrjnKUn->GetRep());
  ((iLineEdit*)fldWtPrjnKUn->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  lblWtPrjnKGp = taiM->NewLabel("K gp", widg, font_spec);
  lblWtPrjnKGp->setToolTip("Number of top K strongest unit groups (where groups are present) to propagate weight projection values through to other layers (-1 or 0 to turn off this feature) -- smaller numbers produce more selective and often interpretable results, though they are somewhat less representative.");
  layColorScaleCtrls->addWidget(lblWtPrjnKGp);
  fldWtPrjnKGp = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldWtPrjnKGp->GetRep());
  ((iLineEdit*)fldWtPrjnKGp->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  layColorScaleCtrls->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layColorBar = new QHBoxLayout();  layDisplayValues->addLayout(layColorBar);

  chkAutoScale = new QCheckBox("Auto\nScale", widg);
  chkAutoScale->setToolTip("Automatically scale min and max values of colorscale based on values of variable being displayed");
  connect(chkAutoScale, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layColorBar->addWidget(chkAutoScale);

  butScaleDefault = new QPushButton("Defaults", widg);
  butScaleDefault->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butScaleDefault->setMaximumWidth(taiM->maxButtonWidth() / 2);
  layColorBar->addWidget(butScaleDefault);
  connect(butScaleDefault, SIGNAL(pressed()), this, SLOT(butScaleDefault_pressed()) );

  cbar = new HCScaleBar(&(dv_->scale), ScaleBar::RANGE, true, true, widg);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(Changed()) );
//  cbar->setMaximumWidth(30);
//   layColorSCaleCtrls->addWidget(cbar); // stretchfact=1 so it stretches to fill the space
  layColorBar->addWidget(cbar); // stretchfact=1 so it stretches to fill the space

  butSetColor = new QPushButton("Colors", widg);
  butSetColor->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butSetColor->setMaximumWidth(taiM->maxButtonWidth() / 2);
  layColorBar->addWidget(butSetColor);
  connect(butSetColor, SIGNAL(pressed()), this, SLOT(butSetColor_pressed()) );
//  layDisplayValues->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layHistory = new QHBoxLayout();  layDisplayValues->addLayout(layHistory);

  histTB = new QToolBar(widg);
  layHistory->addWidget(histTB);

  histTB->setMovable(false);
  histTB->setFloatable(false);

  chkHist = new QCheckBox("Hist: Save", histTB);
  chkHist->setToolTip("Save display value history, which can then be replayed using VCR-style buttons in this toolbar -- value to the right is number of steps to save");
  connect(chkHist, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  histTB->addWidget(chkHist);

  fldHistMax = dl.Add(new taiField(&TA_int, this, NULL, widg));
  fldHistMax->rep()->setCharWidth(4);
  histTB->addWidget(fldHistMax->GetRep());

  histTB->addSeparator();

  actBack_All = histTB->addAction("|<");
  actBack_All->setToolTip("Back all the way to first saved history of display values");
  connect(actBack_All, SIGNAL(triggered()), this, SLOT(hist_back_all()) );

  actBack_F = histTB->addAction("<<");
  actBack_F->setToolTip("Back ff steps in history of display values (edit field on far right to change ff steps to take)");
  connect(actBack_F, SIGNAL(triggered()), this, SLOT(hist_back_f()) );

  actBack = histTB->addAction("<");
  actBack->setToolTip("Back one step in history of display values");
  connect(actBack, SIGNAL(triggered()), this, SLOT(hist_back()) );

  actFwd = histTB->addAction(">" );
  actFwd->setToolTip("Forward one step in history of display values");
  connect(actFwd, SIGNAL(triggered()), this, SLOT(hist_fwd()) );

  actFwd_F = histTB->addAction(">>" );
  actFwd_F->setToolTip("Forward ff steps in history of display values (edit field on far right to change ff steps to take)");
  connect(actFwd_F, SIGNAL(triggered()), this, SLOT(hist_fwd_f()) );

  actFwd_All = histTB->addAction(">|" );
  actFwd_All->setToolTip("Forward all the way to current display values (will now track current values as they come in)");
  connect(actFwd_All, SIGNAL(triggered()), this, SLOT(hist_fwd_all()) );

  histTB->addSeparator();

  QLabel* lblbk = taiM->NewLabel("Hist, Pos:", histTB, font_spec);
  lblbk->setToolTip("number of stored display states in the history buffer right now, and current position relative to the last update (pos numbers = further back in time)");
  histTB->addWidget(lblbk);

  lblHist = taiM->NewLabel("100, 100", histTB, font_spec);
  lblHist->setToolTip("number of steps back in history currently viewing");
  histTB->addWidget(lblHist);

  histTB->addSeparator();

  QLabel* lblff = taiM->NewLabel("ff:", histTB, font_spec);
  lblff->setToolTip("number of steps to take when going fast-forward or fast-back through history");
  histTB->addWidget(lblff);

  fldHistFF = dl.Add(new taiField(&TA_int, this, NULL, widg));
  fldHistFF->rep()->setCharWidth(4);
  histTB->addWidget(fldHistFF->GetRep());

  histTB->addSeparator();

  actMovie = histTB->addAction("Movie");
  actMovie->setToolTip("record individual frames of the netview display from current position through to the end of the history buffer, as movie frames -- uses default 640x480 size with images saved as movie_img_xxx.png -- use mjpeg tools http://mjpeg.sourceforge.net/ (pipe png2yuv into mpeg2enc) to compile the individual PNG frames into an MPEG movie, which can then be transcoded (e.g., using VLC) into any number of other formats");
  connect(actMovie, SIGNAL(triggered()), this, SLOT(hist_movie()) );

  layHistory->addStretch();
//   histTB->addStretch();

//   iSplitter* splt = new iSplitter(Qt::Vertical);
//   setCentralWidget(splt);

//   splt-> addWidget(widg);

  setCentralWidget(widg);

  tw = new QTabWidget(this);
  ////////////////////////////////////////////////////////////////////////////
  lvDisplayValues = new QTreeWidget();
  tw->addTab(lvDisplayValues, "Unit Display Values");
  lvDisplayValues->setRootIsDecorated(false); // makes it look like a list
  QStringList hdr;
  hdr << "Value" << "Description";
  lvDisplayValues->setHeaderLabels(hdr);
  lvDisplayValues->setSortingEnabled(false);
  lvDisplayValues->setSelectionMode(QAbstractItemView::SingleSelection);
  //layDisplayValues->addWidget(lvDisplayValues, 1);
  connect(lvDisplayValues, SIGNAL(itemSelectionChanged()), this, SLOT(lvDisplayValues_selectionChanged()) );

  ////////////////////////////////////////////////////////////////////////////
  // Spec tree

  tvSpecs = new iTreeView(NULL, iTreeView::TV_AUTO_EXPAND);
  tw->addTab(tvSpecs, "Spec Explorer");
  tvSpecs->setDefaultExpandLevels(6); // shouldn't generally be more than this
  tvSpecs->setColumnCount(2);
  tvSpecs->setSortingEnabled(false);// only 1 order possible
  tvSpecs->setHeaderText(0, "Spec");
//   tvSpecs->setColumnWidth(0, 160); // more width for spec column
  tvSpecs->setHeaderText(1, "Description");
  tvSpecs->setColFormat(1, iTreeView::CF_ELIDE_TO_FIRST_LINE); // in case of multi-line specs
  tvSpecs->setColKey(1, taBase::key_desc); //note: ProgVars and Els have nice disp_name desc's
  //enable dnd support
  tvSpecs->setDragEnabled(true);
  tvSpecs->setAcceptDrops(true);
  tvSpecs->setDropIndicatorShown(true);
  tvSpecs->setHighlightRows(true);

  if(dv_->net()) {
    taBase* specs_ = &(dv_->net()->specs);
    MemberDef* md = dv_->net()->GetTypeDef()->members.FindName("specs");
    if (specs_) {
      taiDataLink* dl = (taiDataLink*)specs_->GetDataLink();
      if (dl) {
        dl->CreateTreeDataNode(md, tvSpecs, NULL, "specs");
      }
    }
  }

  tvSpecs->resizeColumnToContents(0); // just make sure everythign fits

  tvSpecs->Connect_SelectableHostNotifySignal(this,
    SLOT(tvSpecs_Notify(ISelectableHost*, int)) );
//   connect(tvSpecs, SIGNAL(ItemSelected(iTreeViewItem*)),
//     this, SLOT(tvSpecs_ItemSelected(iTreeViewItem*)) );
  connect(tvSpecs, SIGNAL(CustomExpandFilter(iTreeViewItem*, int, bool&)),
    this, SLOT(tvSpecs_CustomExpandFilter(iTreeViewItem*, int, bool&)) );

//   layOuter->setStretchFactor(scr, 0); // so it only uses exact spacing
  // so doesn't have tiny scrollable annoying area:
  // (the tradeoff is that if you squish the whole thing, eventually you can't
  // get at all the properties)
//   scr->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  layTopCtrls->addWidget(tw);

//   layOuter->addWidget(tw, 2);
//   splt->addWidget(tw);

  ////////////////////////////////////////////////////////////////////////////
  // Command Buttons
  widCmdButtons = new QWidget(widg);
  iFlowLayout* fl = new iFlowLayout(widCmdButtons);
//  layTopCtrls->addWidget(widCmdButtons);
  layOuter->addWidget(widCmdButtons);

  meth_but_mgr = new iMethodButtonMgr(widCmdButtons, fl, widCmdButtons);
  meth_but_mgr->Constr(getNetView()->net());

  MakeButtons(layOuter);
}

NetViewPanel::~NetViewPanel()
{
  if (NetView *nv = getNetView()) {
    nv->nvp = NULL;
  }
}

void NetViewPanel::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();
  ++updating;
  NetView* nv = getNetView(); // cache
  if (!nv) return;
  if(req_full_build) {
    req_full_build = false;
    nv->Reset();
    nv->BuildAll();
  }
  if(req_full_render) {
    req_full_render = false;
    nv->Render();
  }

  chkDisplay->setChecked(nv->display);
  chkLayMove->setChecked(nv->lay_mv);
  chkNetText->setChecked(nv->net_text);
  fldTextRot->GetImage((String)nv->net_text_rot);
  cmbUnitText->GetEnumImage(nv->unit_text_disp);
  cmbDispMode->GetEnumImage(nv->unit_disp_mode);
  cmbPrjnDisp->GetEnumImage(nv->view_params.prjn_disp);
  fldPrjnWdth->GetImage((String)nv->view_params.prjn_width);

  chkSnapBord->setChecked(nv->snap_bord_disp);
  fldSnapBordWdth->GetImage((String)nv->snap_bord_width);
  fldUnitSpacing->GetImage((String)nv->view_params.unit_spacing);

  chkWtLines->setChecked(nv->wt_line_disp);
  chkWtLineSwt->setChecked(nv->wt_line_swt);
  fldWtLineWdth->GetImage((String)nv->wt_line_width);
  fldWtLineThr->GetImage((String)nv->wt_line_thr);
  fldWtPrjnKUn->GetImage((String)nv->wt_prjn_k_un);
  fldWtPrjnKGp->GetImage((String)nv->wt_prjn_k_gp);
  gelWtPrjnLay->GetImage(&(nv->net()->layers), nv->wt_prjn_lay.ptr());

  chkShowIconified->setChecked(nv->show_iconified);
  fldUnitTrans->GetImage((String)nv->view_params.unit_trans);
  fldUnitFont->GetImage((String)nv->font_sizes.unit);
  fldLayFont->GetImage((String)nv->font_sizes.layer);
  fldMinLayFont->GetImage((String)nv->font_sizes.layer_min);
  chkXYSquare->setChecked(nv->view_params.xy_square);
  chkLayGp->setChecked(nv->view_params.show_laygp);

  chkHist->setChecked(nv->hist_save);
  fldHistMax->GetImage((String)nv->hist_max);
  lblHist->setText(String(nv->ctr_hist_idx.length) + ", " + String(nv->ctr_hist_idx.length-nv->hist_idx) + "  ");
  fldHistFF->GetImage((String)nv->hist_ff);

  // update var selection
  int i = 0;
  QTreeWidgetItemIterator it(lvDisplayValues);
  QTreeWidgetItem* item = NULL;
  while (*it) {
    item = *it;
    bool is_selected = (nv->cur_unit_vals.FindEl(item->text(0)) >= 0);
    item->setSelected(is_selected);
    // if list is size 1 make sure that there is a scale_range entry for this one
    ++it;
    ++i;
  }
  // spec highlighting
  BaseSpec* cspc = m_cur_spec; // to see if it changes, if not, we force redisplay
  iTreeViewItem* tvi = dynamic_cast<iTreeViewItem*>(tvSpecs->currentItem());
  tvSpecs_ItemSelected(tvi); // manually invoke slot
  if (cspc == m_cur_spec)
    setHighlightSpec(m_cur_spec, true);

  ColorScaleFromData();
  --updating;
}

void NetViewPanel::GetValue_impl() {
  inherited::GetValue_impl();
  NetView* nv = getNetView(); // cache
  if (!nv) return;
  req_full_render = true;       // everything requires a re-render
  req_full_build = false;

  nv->display = chkDisplay->isChecked();
  nv->lay_mv = chkLayMove->isChecked();
  nv->net_text = chkNetText->isChecked();
  nv->net_text_rot = (float)fldTextRot->GetValue();

  int i;
  cmbUnitText->GetEnumValue(i);
  nv->unit_text_disp = (NetView::UnitTextDisplay)i;

  // unit disp mode is only guy requiring full build!
  cmbDispMode->GetEnumValue(i);
  req_full_build = req_full_build || (nv->unit_disp_mode != i);
  nv->unit_disp_mode = (NetView::UnitDisplayMode)i;

  cmbPrjnDisp->GetEnumValue(i);
  nv->view_params.prjn_disp = (NetViewParams::PrjnDisp)i;

  nv->view_params.prjn_width = (float)fldPrjnWdth->GetValue();

  nv->view_params.unit_trans = (float)fldUnitTrans->GetValue();
  nv->font_sizes.unit = (float)fldUnitFont->GetValue();
  nv->font_sizes.layer = (float)fldLayFont->GetValue();
  nv->font_sizes.layer_min = (float)fldMinLayFont->GetValue();

  nv->snap_bord_disp = chkSnapBord->isChecked();
  nv->snap_bord_width = (float)fldSnapBordWdth->GetValue();
  nv->view_params.unit_spacing = (float)fldUnitSpacing->GetValue();

  nv->wt_line_disp = chkWtLines->isChecked();
  nv->wt_line_swt = chkWtLineSwt->isChecked();
  nv->wt_line_width = (float)fldWtLineWdth->GetValue();
  nv->wt_line_thr = (float)fldWtLineThr->GetValue();
  nv->wt_prjn_k_un = (float)fldWtPrjnKUn->GetValue();
  nv->wt_prjn_k_gp = (float)fldWtPrjnKGp->GetValue();
  nv->wt_prjn_lay = (Layer*)gelWtPrjnLay->GetValue();
  bool cur_si = chkShowIconified->isChecked();
  if(cur_si != nv->show_iconified) {
    nv->show_iconified = cur_si;
    req_full_build = true;
  }
  nv->view_params.xy_square = chkXYSquare->isChecked();
  nv->view_params.show_laygp = chkLayGp->isChecked();

  nv->hist_save = chkHist->isChecked();
  nv->hist_max = (int)fldHistMax->GetValue();
  nv->hist_ff = (int)fldHistFF->GetValue();

  nv->SetScaleData(chkAutoScale->isChecked(), cbar->min(), cbar->max(), false);
}

void NetViewPanel::CopyFrom_impl() {
  if (NetView *nv = getNetView()) {
    nv->CallFun("CopyFromView");
  }
}

void NetViewPanel::butScaleDefault_pressed() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->SetScaleDefault();
    nv->UpdateDisplay(true);
  }
}

void NetViewPanel::butSetColor_pressed() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->CallFun("SetColorSpec");
  }
}

void NetViewPanel::hist_back_all() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistBackAll();
  }
}

void NetViewPanel::hist_back_f() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistBackF();
  }
}

void NetViewPanel::hist_back() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistBack1();
  }
}

void NetViewPanel::hist_fwd() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistFwd1();
  }
}

void NetViewPanel::hist_fwd_f() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistFwdF();
  }
}

void NetViewPanel::hist_fwd_all() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistFwdAll();
  }
}

void NetViewPanel::hist_movie() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistMovie();
  }
}

void NetViewPanel::ColorScaleFromData() {
  if (NetView *nv = getNetView()) {
    ++updating;
    cbar->UpdateScaleValues();
    if (chkAutoScale->isChecked() != nv->scale.auto_scale) {
      //note: raises signal on widget! (grr...)
      chkAutoScale->setChecked(nv->scale.auto_scale);
    }
    --updating;
  }
}

void NetViewPanel::GetVars() {
  NetView *nv = getNetView();
  if (!nv) return;

  lvDisplayValues->clear();
  if (nv->membs.size == 0) return;

  MemberDef* md;
  QTreeWidgetItem* lvi = NULL;
  for (int i=0; i < nv->membs.size; i++) {
    md = nv->membs[i];
    if (md->HasOption("NO_VIEW")) continue;
    QStringList itm;
    itm << md->name << md->desc;
    lvi = new QTreeWidgetItem(lvDisplayValues, itm);
  }
  lvDisplayValues->resizeColumnToContents(0);
}

void NetViewPanel::InitPanel() {
  if (NetView *nv = getNetView()) {
    ++updating;
    // fill monitor values
    GetVars();
    --updating;
  }
}

void NetViewPanel::lvDisplayValues_selectionChanged() {
  if (updating) return;

  NetView *nv = getNetView();
  if (!nv) return;

  // redo the list each time, to guard against stale values
  nv->cur_unit_vals.Reset();
  QList<QTreeWidgetItem*> items(lvDisplayValues->selectedItems());
  QTreeWidgetItem* item = NULL;
  for (int j = 0; j < items.size(); ++j) {
    item = items.at(j);
    nv->cur_unit_vals.Add(item->text(0));
  }

  if (MemberDef *md = (MemberDef*) nv->membs.FindName(nv->cur_unit_vals.SafeEl(0))) {
    nv->setUnitDispMd(md);
    nv->UpdateViewerModeForMd(md);
  }
  ColorScaleFromData();
  //nv->InitDisplay(false);
  // note: init will reset history etc and is now unnec for updating view guys..
  nv->UpdateDisplay(false);
}

void NetViewPanel::setHighlightSpec(BaseSpec* spec, bool force) {
  if ((spec == m_cur_spec) && !force) return;
  m_cur_spec = spec;
  if (NetView *nv = getNetView()) {
    nv->SetHighlightSpec(spec);
  }
}

void NetViewPanel::tvSpecs_CustomExpandFilter(iTreeViewItem* item,
  int level, bool& expand)
{
  if (level < 1) return; // always expand root level
  // by default, we only expand specs themselves, not the args, objs, etc.
  // and then ONLY if that spec itself has child specs
  taiDataLink* dl = item->link();
  TypeDef* typ = dl->GetDataTypeDef();
  // check for spec itself (DEF_CHILD) and children list
  if (typ->InheritsFrom(&TA_BaseSpec)) {
    BaseSpec* spec = (BaseSpec*)dl->data();
    if (spec->children.size > 0) return;
  }
  else if (typ->DerivesFrom(&TA_BaseSpec_Group))
    return;
  expand = false;
}

void NetViewPanel::tvSpecs_Notify(ISelectableHost* src, int op) {
  NetView *nv = getNetView();
  if (!nv) return;

  switch (op) {
    //case ISelectableHost::OP_GOT_FOCUS:
    //  return;

    case ISelectableHost::OP_SELECTION_CHANGED:
    {
      taBase* new_base = NULL;
      ISelectable* si = src->curItem();
      if (si && si->link()) {
        new_base = si->link()->taData(); // NULL if not a taBase, shouldn't happen
      }
      setHighlightSpec((BaseSpec*)new_base);
      //    nv->UpdateDisplay(true);

      break;
    }

    //case ISelectableHost::OP_DESTROYING:
    //  return;

    default:
      return;
  }
}

void NetViewPanel::tvSpecs_ItemSelected(iTreeViewItem* item) {
  NetView *nv = getNetView();
  if (!nv) return;

  BaseSpec* spec = NULL;
  if (item) {
    taBase* ld_ = (taBase*)item->linkData();
    if (ld_->InheritsFrom(TA_BaseSpec))
      spec = (BaseSpec*)ld_;
  }
  setHighlightSpec(spec);
//   nv->UpdateDisplay(true);
}


void NetViewPanel::viewWin_NotifySignal(ISelectableHost* src, int op) {
  if (NetView *nv = getNetView()) {
    nv->viewWin_NotifySignal(src, op);
  }
}

void NetViewPanel::dynbuttonActivated(int but_no) {
  NetView *nv = getNetView();
  if (!nv) return;

  T3ExaminerViewer* vw = nv->GetViewer();
  if (!vw) return;

  taiAction* dyb = vw->getDynButton(but_no);
  if (!dyb) return;

  String nm = dyb->text();
  nv->cur_unit_vals.Reset();
  nv->cur_unit_vals.Add(nm);
  if (MemberDef *md = (MemberDef*)nv->membs.FindName(nm)) {
    nv->setUnitDispMd(md);
    nv->UpdateViewerModeForMd(md);
    vw->setDynButtonChecked(but_no, true, true); // mutex
  }
  ColorScaleFromData();

//   nv->InitDisplay(false);
  nv->UpdateDisplay(true);     // update panel
}

void NetViewPanel::unTrappedKeyPressEvent(QKeyEvent* e) {
  if (NetView *nv = getNetView()) {
    nv->unTrappedKeyPressEvent(e);
  }
}
