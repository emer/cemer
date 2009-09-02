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

//obs #include "netstru_so.h"
#include "ta_geometry.h"
#include "emergent_project.h"
#include "css_qt.h"		// for the cssiSession
#include "ta_qtclipdata.h"
#include "ta_qt.h"
#include "ta_qtgroup.h"
#include "ta_qtviewer.h"

#include "iflowlayout.h"
#include "icolor.h"
#include "ilineedit.h"

#include "imisc_so.h"
/*
#include <ta_misc/picker.bm>
#include <ta_misc/picker_mask.bm>

*/

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

//#include <OS/file.h>

//#include <OS/math.h>
#include <limits.h>
#include <float.h>
//nn #include <unistd.h>

NetView* nvDataView::nv() {
  if (!m_nv)
    m_nv = GET_MY_OWNER(NetView);
  return m_nv;
}

void nvDataView::Initialize() {
  m_nv = NULL;
}

void nvDataView::CutLinks() {
  m_nv = NULL;
  inherited::CutLinks();
}

//////////////////////////
//   UnitView		//
//////////////////////////

void UnitView::Initialize() {
  m_nv = NULL;
  picked = 0;
}

void UnitView::CutLinks() {
  m_nv = NULL;
  inherited::CutLinks();
}

NetView* UnitView::nv() {
  if (!m_nv) {
   UnitGroupView* ugrv = this->ugrv();
   if (ugrv)
     m_nv = ugrv->nv();
  }
  return m_nv;
}

void UnitView::Render_pre() {
  NetView* nv = this->nv();
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

  // note: pos s/b invariant
  node_so()->transform()->translation.setValue
    (disp_scale * ((float)(unit->pos.x + 0.5f) / max_x),
     0.0f,
     -disp_scale * (((float)(unit->pos.y + 0.5f) / max_y)));
  inherited::Render_pre();
}


//////////////////////////
//   UnitViewData	//
//////////////////////////

void UnitViewData::Initialize() {
  disp_base = NULL;
}

//////////////////////////
//   UnitViewData_PArray//
//////////////////////////

void UnitViewData_PArray::SetGeom(TwoDCoord& c) {
  m_x = c.x;
  SetSize(c.x * c.y); //note: may not shrink alloc
}


//////////////////////////
//   UnitGroupView	//
//////////////////////////

void UnitGroupView::Initialize() {
  data_base = &TA_Unit_Group;
  m_lv = NULL;
}


void UnitGroupView::Destroy() {
  Reset();
}

void UnitGroupView::AllocUnitViewData() {
  //note: allocate based on geom, not current size, in case not built yet
  uvd_arr.SetGeom(ugrp()->geom);
}

void UnitGroupView::BuildAll() {
  Reset(); // in case where we are rebuilding
  AllocUnitViewData();
  NetView* nv = this->nv();
  if(nv->unit_disp_mode == NetView::UDM_BLOCK) return; // optimized

  Unit_Group* ugrp = this->ugrp(); //cache
  TwoDCoord coord;
  for (coord.y = 0; coord.y < ugrp->geom.y; ++(coord.y)) {
    for (coord.x = 0; coord.x < ugrp->geom.x; ++(coord.x)) {
      Unit* unit = ugrp->FindUnitFmCoord(coord);
      if (!unit) break; // there won't be any more units

      UnitView* uv = new UnitView();
      uv->SetData(unit);//obsunit->AddDataView(uv);
      children.Add(uv);
    }
  }
}

void UnitGroupView::InitDisplay() {
  AllocUnitViewData(); // make sure we have correct space in uvd array
  NetView* nv = this->nv();
  UpdateUnitViewBase(nv->unit_disp_md, nv->unit_src, nv->unit_con_md);
}

float UnitGroupView::GetUnitDisplayVal(const TwoDCoord& co, int unit_md_flags) {
  NetView* nv = this->nv();
  float val = nv->scale.zero;
  void* base = uvd_arr.FastEl(co).disp_base;

  if (base) switch (unit_md_flags) {
  case NetView::MD_FLOAT:
    val = *((float*)base); break;
  case NetView::MD_DOUBLE:
    val = *((double*)base); break;
  case NetView::MD_INT:
    val = *((int*)base); break;
//  default:
//    val = 0.0f; break;
  }
  return val;
}

void UnitGroupView::UpdateUnitViewBase(MemberDef* disp_md, Unit* src_u, bool& con_md) {
  Unit_Group* ugrp = this->ugrp(); //cache
  AllocUnitViewData();
  con_md = false;
  if (disp_md == NULL) { // just clear all
    TwoDCoord coord;
    while ((coord.x < ugrp->geom.x) && (coord.y < ugrp->geom.y)) {
      UnitViewData& uvd = uvd_arr.FastEl(coord);
      uvd.disp_base = NULL;
      if (++(coord.x) >= ugrp->geom.x) {coord.x = 0; ++(coord.y);}
    }
    return;
  }

  String nm = disp_md->name.before(".");
  if (nm.empty()) { // direct unit member
    UpdateUnitViewBase_Unit_impl(disp_md);
  } else if ((nm=="s") || (nm == "r")) {
    con_md = true;
    UpdateUnitViewBase_Con_impl((nm=="s"), disp_md->name.after('.'), src_u);
  } else if (nm=="bias") {
    UpdateUnitViewBase_Bias_impl(disp_md);
  } else { // sub-member of unit
    UpdateUnitViewBase_Sub_impl(disp_md);
  }
}

void UnitGroupView::UpdateUnitViewBase_Con_impl(bool is_send, String nm, Unit* src_u) {
  Unit_Group* ugrp = this->ugrp(); //cache
  TwoDCoord coord;
  for(coord.y = 0; coord.y < ugrp->geom.y; coord.y++) {
    for(coord.x = 0; coord.x < ugrp->geom.x; coord.x++) {
      Unit* unit = ugrp->FindUnitFmCoord(coord);
      UnitViewData& uvd = uvd_arr.FastEl(coord);
      uvd.disp_base = NULL;
      if (!unit) continue;  // rest will be null too, but we loop to null disp_base

      if (is_send) {
	for(int g=0;g<unit->recv.size;g++) {
	  RecvCons* tcong = unit->recv.FastEl(g);
	  MemberDef* act_md = tcong->con_type->members.FindName(nm);
	  if (!act_md)	continue;
	  Connection* con = tcong->FindConFrom(src_u);
	  if (!con) continue;
	  uvd.disp_base = act_md->GetOff(con);
	  break;		// once you've got one, done!
	}
      }
      else {
	for(int g=0;g<unit->send.size;g++) {
	  SendCons* tcong = unit->send.FastEl(g);
	  MemberDef* act_md = tcong->con_type->members.FindName(nm);
	  if (!act_md)	continue;
	  Connection* con = tcong->FindConFrom(src_u);
	  if (!con) continue;
	  uvd.disp_base = act_md->GetOff(con);
	  break;		// once you've got one, done!
	}
      }
    }
  }
}

void UnitGroupView::UpdateUnitViewBase_Bias_impl(MemberDef* disp_md) {
  Unit_Group* ugrp = this->ugrp(); //cache
  TwoDCoord coord;
  while ((coord.x < ugrp->geom.x) && (coord.y < ugrp->geom.y)) {
    Unit* unit = ugrp->FindUnitFmCoord(coord);
    UnitViewData& uvd = uvd_arr.FastEl(coord);
    //NOTE: do loop advance here, so we can continue -- ***coord's are now next beyond current***
    if (++(coord.x) >= ugrp->geom.x) {coord.x = 0; ++(coord.y);}
    uvd.disp_base = NULL;
    if (!unit) continue;  // rest will be null too, but we loop to null disp_base
    if(unit->bias.size == 0) continue;
    Connection* con = unit->bias.Cn(0);
    uvd.disp_base = disp_md->GetOff(con);
  }
}

void UnitGroupView::UpdateUnitViewBase_Unit_impl(MemberDef* disp_md) {
  Unit_Group* ugrp = this->ugrp(); //cache
  TwoDCoord coord;
  while ((coord.x < ugrp->geom.x) && (coord.y < ugrp->geom.y)) {
    Unit* unit = ugrp->FindUnitFmCoord(coord);
    UnitViewData& uvd = uvd_arr.FastEl(coord);
    //NOTE: do loop advance here, so we can continue -- ***coord's are now next beyond current***
    if (++(coord.x) >= ugrp->geom.x) {coord.x = 0; ++(coord.y);}
    uvd.disp_base = NULL;
    if (!unit) continue;  // rest will be null too, but we loop to null disp_base
    uvd.disp_base = disp_md->GetOff(unit);
  }
}

void UnitGroupView::UpdateUnitViewBase_Sub_impl(MemberDef* disp_md) {
  Unit_Group* ugrp = this->ugrp(); //cache
  TwoDCoord coord;
  while ((coord.x < ugrp->geom.x) && (coord.y < ugrp->geom.y)) {
    Unit* unit = ugrp->FindUnitFmCoord(coord);
    UnitViewData& uvd = uvd_arr.FastEl(coord);
    //NOTE: do loop advance here, so we can continue -- ***coord's are now next beyond current***
    if (++(coord.x) >= ugrp->geom.x) {coord.x = 0; ++(coord.y);}
    uvd.disp_base = NULL;
    if (!unit) continue;  // rest will be null too, but we loop to null disp_base

    String nm = disp_md->name.before(".");
    MemberDef* par_md = unit->FindMember((const char*)nm);
    if (par_md == NULL) continue;
    nm = disp_md->name.after(".");
    TypeDef* nptd = par_md->type;
    void* par_base = par_md->GetOff(unit);
    if (nptd->ptr == 1) {
      par_base = *((void**)par_base); // its a pointer, so de-reference it
      if (par_base == NULL) continue;
      nptd = ((taBase*)par_base)->GetTypeDef(); // get actual typedef
    }
    MemberDef* smd = nptd->members.FindName(nm);
    if (smd == NULL)  continue;
    uvd.disp_base = smd->GetOff(par_base);
  }
}

void UnitGroupView::UpdateAutoScale(bool& updated) {
  NetView* nv = this->nv();
  TwoDCoord co;
  Unit_Group* ugrp = this->ugrp(); //cache
  for (co.y = 0; co.y < ugrp->geom.y; ++co.y) {
    for (co.x = 0; co.x < ugrp->geom.x; ++co.x) {
      float val = GetUnitDisplayVal(co, nv->unit_md_flags); // get val for unit at co
      if(!updated) {
	nv->scale.SetMinMax(val, val);
	updated = true;
      }
      else
	nv->scale.UpdateMinMax(val);
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
      rp.setPoint(mouseevent->getPosition());
      rp.apply(viewer->quarter->getSoEventManager()->getSceneGraph()); // event mgr has full graph!
      SoPickedPoint* pp = rp.getPickedPoint(0);
      if(!pp) continue;
      SoNode* pobj = pp->getPath()->getNodeFromTail(2);
      if(!pobj) continue;
//       cerr << "obj typ: " << pobj->getTypeId().getName() << endl;
      if(!pobj->isOfType(T3UnitGroupNode::getClassTypeId())) {
	pobj = pp->getPath()->getNodeFromTail(3);
// 	cerr << "2: obj typ: " << pobj->getTypeId().getName() << endl;
	if(!pobj->isOfType(T3UnitGroupNode::getClassTypeId())) {
	  pobj = pp->getPath()->getNodeFromTail(1);
// 	  cerr << "3: obj typ: " << pobj->getTypeId().getName() << endl;
	  if(pobj->getName() == "WtLines") {
	    // disable selecting of wt lines!
	    ecb->setHandled();
	    return;
	  }
// 	  cerr << "not unitgroupnode!" << endl;
	  continue;
	}
      }
      UnitGroupView* act_ugv = (UnitGroupView*)((T3UnitGroupNode*)pobj)->dataView();
      Unit_Group* ugrp = act_ugv->ugrp();
      float disp_scale = ugrp->own_lay->disp_scale;

      SbVec3f pt = pp->getObjectPoint(pobj); 
      //   cerr << "got: " << pt[0] << " " << pt[1] << " " << pt[2] << endl;
      int xp = (int)((pt[0] * tnv->max_size.x) / disp_scale);
      int yp = (int)(-(pt[2] * tnv->max_size.y) / disp_scale);
      //   cerr << xp << ", " << yp << endl;
      xp -= ugrp->pos.x; yp -= ugrp->pos.y;
      
      if((xp >= 0) && (xp < ugrp->geom.x) && (yp >= 0) && (yp < ugrp->geom.y)) {
	Unit* unit = ugrp->FindUnitFmCoord(xp, yp);
	if(unit) tnv->setUnitSrc(NULL, unit);
      }
      tnv->InitDisplay();
      tnv->UpdateDisplay();
      got_one = true;
    }
  }
  if(got_one)
    ecb->setHandled();
}

void UnitGroupView::Render_pre() {
  NetView* nv = this->nv();
  Unit_Group* ugrp = this->ugrp(); //cache

  if(ugrp->own_lay && ugrp->own_lay->Iconified()) {
    return;			// don't render anything!
  }

  bool no_units = true;
  if(nv->unit_disp_mode != NetView::UDM_BLOCK)
    no_units = false;

  AllocUnitViewData();
  setNode(new T3UnitGroupNode(this, no_units));
  //NOTE: we create/adjust the units in the Render_impl routine
  T3UnitGroupNode* ugrp_so = node_so(); // cache

  ugrp_so->setGeom(ugrp->geom.x, ugrp->geom.y, nv->max_size.x, nv->max_size.y, nv->max_size.z, 
		   ugrp->own_lay->disp_scale);

  inherited::Render_pre();
}

void UnitGroupView::Render_impl() {
  Unit_Group* ugrp = this->ugrp(); //cache
  NetView* nv = this->nv();

  //set origin: 0,0,0
  TDCoord& pos = ugrp->pos;
  float disp_scale = ugrp->own_lay->disp_scale;
  FloatTransform* ft = transform(true);
  ft->translate.SetXYZ(disp_scale * ((float)pos.x / nv->max_size.x),
		       disp_scale * ((float)pos.z / nv->max_size.z),
		       disp_scale * ((float)-pos.y / nv->max_size.y));

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
  NetView* nv = this->nv(); //cache
  Unit_Group* ugrp = this->ugrp(); //cache
  if(!ugrp) return;
  Layer* lay = ugrp->own_lay;
  if(!lay) return;

  if(lay->Iconified() || !lv() || (lv()->disp_mode == LayerView::DISP_FRAME)) {
    return;			// don't render anything!
  }

  T3UnitGroupNode* node_so = this->node_so(); // cache
  if(!node_so) return;

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
  TwoDCoord co;
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

  for (int i = 0; i < children.size; ++i) {
    UnitView* uv = (UnitView*)children.FastEl(i);
    Unit* unit = uv->unit();
    unit_so = uv->node_so();
    if (!unit_so || !unit) continue; // shouldn't happen
    nv->GetUnitDisplayVals(this, unit->pos, val, col, sc_val);
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
    } else { // text of some kind
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

void UnitGroupView::Render_impl_blocks() {
  // this function just does all the memory allocation and static configuration
  // that doesn't depend on unit values, then it calls UpdateUnitValues
  // which sets all the values!

  NetView* nv = this->nv(); //cache
  Unit_Group* ugrp = this->ugrp(); //cache
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

  float disp_scale = ugrp->own_lay->disp_scale;

  int n_geom = ugrp->geom.Product();
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

  String val_str = "0.0";	// initial default
  String unit_name;
  T3Color col;
  TwoDCoord pos;
  int v_idx = 0;
  int t_idx = 3;		// base color + complexity + font
  // these go in normal order; indexes are backwards
  for(pos.y=0; pos.y<ugrp->geom.y; pos.y++) {
    for(pos.x=0; pos.x<ugrp->geom.x; pos.x++) { // right to left
      Unit* unit = ugrp->FindUnitFmCoord(pos);
      float xp = disp_scale * (((float)pos.x + spacing) / nv->max_size.x);
      float yp = -disp_scale * (((float)pos.y + spacing) / nv->max_size.y);
      float xp1 = disp_scale * (((float)pos.x+1 - spacing) / nv->max_size.x);
      float yp1 = -disp_scale * (((float)pos.y+1 - spacing) / nv->max_size.y);
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
// 	if(nv->unit_text_disp & NetView::UTD_VALUES) {
	  //	  ValToDispText(0.0f, val_str); // just use default
// 	}
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
  int nc_per_idx = 19;		// number of coords per index
  int nn_per_idx = 10;		// number of norms per index
  int nm_per_idx = 3;		// number of mats per index
  coords.setNum(n_geom * nc_per_idx);
  norms.setNum(n_geom * nn_per_idx);
  mats.setNum(n_geom * nm_per_idx);

  int nx = ugrp->geom.x;

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
  for(pos.y=ugrp->geom.y-1; pos.y>=0; pos.y--) { // go back to front
    for(pos.x=0; pos.x<ugrp->geom.x; pos.x++) { // right to left
      int c00_0 = (pos.y * nx + pos.x) * n_per_vtx;
      int c10_0 = c00_0 + 1;
      int c01_0 = c00_0 + 2;
      int c11_0 = c00_0 + 3;
      int c00_v = c00_0 + 4;
      int c10_v = c00_0 + 5;
      int c01_v = c00_0 + 6;
      int c11_v = c00_0 + 7;

      int mat_idx = (pos.y * nx + pos.x);

      // note: this optimization is incompatible with the current split of
      // basic structure render vs. values
//       float zval = vertex[c00_v][1]; // "y" coord = 1
//       if(zval < 0.0f) {			 // do "top" first which is actually bottom!
// 	coords_dat[cidx++] = (c01_v); // 0
// 	coords_dat[cidx++] = (c11_v); // 1
// 	coords_dat[cidx++] = (c00_v); // 2
// 	coords_dat[cidx++] = (c10_v); // 3
// 	coords_dat[cidx++] = (-1); // -1 -- 5 total

// 	norms_dat[nidx++] = (4); // top
// 	norms_dat[nidx++] = (4); // top -- 2 total

// 	mats_dat[midx++] = (mat_idx);
//       }

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

//       if(zval >= 0.0f) {		 // only if higher..
	coords_dat[cidx++] = (c01_v); // 0
	coords_dat[cidx++] = (c11_v); // 1
	coords_dat[cidx++] = (c00_v); // 2
	coords_dat[cidx++] = (c10_v); // 3
	coords_dat[cidx++] = (-1); // -1 -- 5 total

	norms_dat[nidx++] = (4); // top
	norms_dat[nidx++] = (4); // top -- 2 total

	mats_dat[midx++] = (mat_idx);
//       }

      // total coords = 7 + 7 + 5 = 19
      // total norms = 4 + 4 + 2 = 10
      // total mats = 3
    }
  }
  coords.finishEditing();
  norms.finishEditing();
  mats.finishEditing();

  UpdateUnitValues_blocks();		// hand off to next guy..
}

void UnitGroupView::UpdateUnitValues_blocks() {
  NetView* nv = this->nv(); //cache
  Unit_Group* ugrp = this->ugrp(); //cache
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
  TwoDCoord pos;
  int v_idx = 0;
  int c_idx = 0;
  int t_idx = 3;		// base color + font
  // these go in normal order; indexes are backwards
  for(pos.y=0; pos.y<ugrp->geom.y; pos.y++) {
    for(pos.x=0; pos.x<ugrp->geom.x; pos.x++) { // right to left
      nv->GetUnitDisplayVals(this, pos, val, col, sc_val);
      Unit* unit = ugrp->FindUnitFmCoord(pos);
      if(nv->unit_con_md && (unit == nv->unit_src.ptr())) {
	col.r = 0.0f; col.g = 1.0f; col.b = 0.0f;
      }
      float zp = .5f * sc_val / max_z;
      v_idx+=4;			// skip the _0 cases

      vertex_dat[v_idx++][1] = zp; // 00_v = 1
      vertex_dat[v_idx++][1] = zp; // 10_v = 2
      vertex_dat[v_idx++][1] = zp; // 01_v = 3
      vertex_dat[v_idx++][1] = zp; // 11_v = 4

      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
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
  NetView* nv = this->nv(); //cache
  Unit_Group* ugrp = this->ugrp(); //cache
  if(!ugrp) return;
  Layer* lay = ugrp->own_lay;
  if(!lay) return;
  if(lay->Iconified() || !lv() || lv()->disp_mode == LayerView::DISP_FRAME) {
    return;			// don't render anything!
  }
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

void UnitGroupView::Reset_impl() {
  inherited::Reset_impl();
  uvd_arr.SetSize(0);
}

void UnitGroupView::Render_impl_outnm() {
  // this function just does all the memory allocation and static configuration
  // that doesn't depend on unit values, then it calls UpdateUnitValues
  // which sets all the values!

  NetView* nv = this->nv(); //cache
  Unit_Group* ugrp = this->ugrp(); //cache
  T3UnitGroupNode* node_so = this->node_so(); // cache
  Layer* lay = ugrp->own_lay;
  if(!lay) return;

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

  float szx = (float)ugrp->geom.x / nv->max_size.x;
  float szy = (float)ugrp->geom.y / nv->max_size.y;

  float cx = .5f * szx;
  float cy = .5f * szy;

  SoTranslation* tr = (SoTranslation*)un_txt->getChild(3);
  tr->translation.setValue(cx, 0.0f, -cy);

  szx = 1.5f * szx / (float)nv->font_sizes.un_nm_len;

  float ufontsz = MIN(szx, szy);

  SoFont* fnt = (SoFont*)un_txt->getChild(2);
  fnt->size.setValue(ufontsz);

  UpdateUnitValues_outnm();		// hand off to next guy..
}

void UnitGroupView::UpdateUnitValues_outnm() {
  //  NetView* nv = this->nv(); //cache
  Unit_Group* ugrp = this->ugrp(); //cache
  T3UnitGroupNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  Layer* lay = ugrp->own_lay;
  if(!lay) return;

  SoSeparator* un_txt = node_so->unitText();

  SoAsciiText* txt = (SoAsciiText*)un_txt->getChild(4);
  SoMFString* mfs = &(txt->string);
  if(lay->unit_groups)
    mfs->setValue(ugrp->output_name.chars());
  else
    mfs->setValue(lay->output_name.chars());
}

void UnitGroupView::Render_impl_snap_bord() {
  NetView* nv = this->nv(); //cache
  T3UnitGroupNode* node_so = this->node_so(); //cache
  if(!node_so) return;

  Unit_Group* ugrp = this->ugrp(); //cache

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

  float disp_scale = ugrp->own_lay->disp_scale;

  int n_geom = ugrp->geom.Product();

  vertex.setNum(n_geom * 4);	// 4 virtex per
  coords.setNum(n_geom * 6);	// 6 coords per
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

  TwoDCoord pos;
  for(pos.y=0; pos.y<ugrp->geom.y; pos.y++) {
    for(pos.x=0; pos.x<ugrp->geom.x; pos.x++) { // right to left
      float xp = disp_scale * (((float)pos.x + spacing) / nv->max_size.x);
      float yp = -disp_scale * (((float)pos.y + spacing) / nv->max_size.y);
      float xp1 = disp_scale * (((float)pos.x+1 - spacing) / nv->max_size.x);
      float yp1 = -disp_scale * (((float)pos.y+1 - spacing) / nv->max_size.y);
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

  UpdateUnitValues_snap_bord();		// hand off to next guy..
}

void UnitGroupView::UpdateUnitValues_snap_bord() {
  NetView* nv = this->nv(); //cache

  if(!nv->snap_bord_disp) return;

  Unit_Group* ugrp = this->ugrp(); //cache
  T3UnitGroupNode* node_so = this->node_so(); // cache

  SoVertexProperty* vtx_prop = node_so->snapBordVtxProp();
  SoMFUInt32& color = vtx_prop->orderedRGBA;

  uint32_t* color_dat = color.startEditing();
  if(!color_dat) return;

  float trans = nv->view_params.unit_trans;
  float val;
  float sc_val;
  iColor tc;
  T3Color col;
  TwoDCoord pos;
  int c_idx = 0;
  for(pos.y=0; pos.y<ugrp->geom.y; pos.y++) {
    for(pos.x=0; pos.x<ugrp->geom.x; pos.x++) { // right to left
      val = 0.0f;
      Unit* unit = ugrp->FindUnitFmCoord(pos);
      if(unit) {
	val = unit->snap;
      }
      nv->GetUnitColor(val, tc, sc_val);
      col.setValue(tc.redf(), tc.greenf(), tc.bluef());

      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      color_dat[c_idx++] = T3Color::makePackedRGBA(col.r, col.g, col.b, alpha);
    }
  }

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
  NetView* n = nv();
  Layer* lay = layer(); //cache
  if(n && lay) {
    n->SetLayDispMode(lay->name, disp_mode);
  }
}

void LayerView::BuildAll() {
  NetView* n = nv();
  Layer* lay = layer(); //cache

  int dspmd = n->GetLayDispMode(lay->name);
  if(dspmd >= 0) disp_mode = (LayerView::DispMode)dspmd;

  Reset(); //for when we are invoked after initial construction
  Unit_Group* ugrp;
  UnitGroupView* ugv;
  if (!lay->unit_groups) { // single ugrp
    ugrp = &(lay->units);
    ugv = new UnitGroupView;
    ugv->SetData(ugrp);//obs ugrp->AddDataView(ugv);
    ugv->SetLayerView(this);
    children.Add(ugv);
    ugv->BuildAll();

//    int flags = 0;// BrListViewItem::DNF_UPDATE_NAME | BrListViewItem::DNF_CAN_BROWSE| BrListViewItem::DNF_CAN_DRAG;
  } else { // multi-ugrps
    for (int j = 0; j < lay->gp_geom.n; ++j) {
      ugrp = (Unit_Group*)lay->units.SafeGp(j);
      if (!ugrp) break; // maybe not built yet???

      ugv = new UnitGroupView;
      ugv->SetData(ugrp);//obs ugrp->AddDataView(ugv);
      ugv->SetLayerView(this);
      children.Add(ugv);
      ugv->BuildAll();

//      int flags = 0;// BrListViewItem::DNF_UPDATE_NAME | BrListViewItem::DNF_CAN_BROWSE| BrListViewItem::DNF_CAN_DRAG;
//      last_child_node = dl->CreateT3Node(par_node, last_child_node, node_nm, flags);
    }
  }
}

void LayerView::InitDisplay() {
  UnitGroupView* ugrv;
  taListItr j;
  FOR_ITR_EL(UnitGroupView, ugrv, children., j) {
    ugrv->InitDisplay();
  }
}

void LayerView::UpdateUnitValues() { // *actually* only does unit value updating
  int ch_idx = 0;
  Layer* lay = layer(); //cache
  if(!lay) return;
  if(!lay->unit_groups) { // single ugrp
    UnitGroupView* ugv = (UnitGroupView*)children.SafeEl(ch_idx++);
    if(ugv) ugv->UpdateUnitValues(); // if null, maybe not built yet
  }
  else { // multi-ugrps
    for (int j = 0; j < lay->gp_geom.n; ++j) {
      Unit_Group* ugrp = (Unit_Group*)lay->units.SafeGp(j);
      if (!ugrp) break; // maybe not built yet???
      UnitGroupView* ugv = (UnitGroupView*)children.SafeEl(ch_idx++);
      if(ugv) ugv->UpdateUnitValues();
    }
  }
}

void LayerView::UpdateAutoScale(bool& updated) {
  UnitGroupView* ugrv;
  taListItr j;
  FOR_ITR_EL(UnitGroupView, ugrv, children., j) {
    ugrv->UpdateAutoScale(updated);
  }
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
  NetView* nv = this->nv();
  
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

  NetView* nv = this->nv();
  if(!nv->lay_mv) show_drag = false;

  setNode(new T3LayerNode(this, show_drag));
  DoHighlightColor(false);

  inherited::Render_pre();
}

void LayerView::Render_impl() {
  Layer* lay = this->layer(); //cache
  NetView* nv = this->nv();

  TDCoord& pos = lay->pos;	// with layer groups as real things now, use this!
  //  TDCoord pos; lay->GetAbsPos(pos);
  FloatTransform* ft = transform(true);
  ft->translate.SetXYZ((float)pos.x / nv->max_size.x,
		       (float)pos.z / nv->max_size.z,
		       (float)-pos.y / nv->max_size.y);

  T3LayerNode* node_so = this->node_so(); // cache
  if(!node_so) return;
  if(lay->Iconified()) {
    node_so->setGeom(1, 1, nv->max_size.x, nv->max_size.y, nv->max_size.z, 1.0f);
  }
  else {
    node_so->setGeom(lay->act_geom.x, lay->act_geom.y,
		     nv->max_size.x, nv->max_size.y, nv->max_size.z, lay->disp_scale);
  }
  node_so->setCaption(data()->GetName().chars());

  float max_xy = MAX(nv->max_size.x, nv->max_size.y);
  float lay_wd = T3LayerNode::width / max_xy;
  lay_wd = MIN(lay_wd, T3LayerNode::max_width);
  float fx = (float)lay->scaled_act_geom.x / nv->max_size.x;

  // ensure that the layer label does not go beyond width of layer itself!
  float eff_lay_font_size = nv->font_sizes.layer;
  float lnm_wd = (eff_lay_font_size * lay->name.length()) / t3Misc::char_ht_to_wd_pts;
  if(lnm_wd > fx) {
    eff_lay_font_size = (fx / (float)lay->name.length()) * t3Misc::char_ht_to_wd_pts;
  }
  node_so->resizeCaption(eff_lay_font_size);


  SbVec3f tran(0.0f, -eff_lay_font_size, lay_wd);
  node_so->transformCaption(tran);

  inherited::Render_impl();
}

// callback for layer xy dragger
void T3LayerNode_XYDragFinishCB(void* userData, SoDragger* dragr) {
  SoTranslate2Dragger* dragger = (SoTranslate2Dragger*)dragr;
  T3LayerNode* laynd = (T3LayerNode*)userData;
  LayerView* lv = (LayerView*)laynd->dataView();
  Layer* lay = lv->layer();
  NetView* nv = lv->nv();

  float fx = (float)lay->act_geom.x / nv->max_size.x;
  float fy = (float)lay->act_geom.y / nv->max_size.y;
  float xfrac = .5f * fx;
  float yfrac = .5f * fy;

  const SbVec3f& trans = dragger->translation.getValue();
  float new_x = trans[0] * nv->max_size.x;
  float new_y = trans[1] * nv->max_size.y;
  if(new_x < 0.0f) 	new_x -= .5f; // add an offset to effect rounding.
  else			new_x += .5f;
  if(new_y < 0.0f) 	new_y -= .5f;
  else			new_y += .5f;

//   cerr << "lay: " << lay->name << " " << trans[0] << " " << trans[1] << " drg: " <<
//     drag_sc << " fx: " << fx << " fy: " << fy << " new: " << new_x << " " << new_y << endl;

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
  LayerView* lv = (LayerView*)laynd->dataView();
  Layer* lay = lv->layer();
  NetView* nv = lv->nv();

  const SbVec3f& trans = dragger->translation.getValue();
  float new_z = trans[0] * nv->max_size.z;
  if(new_z < 0.0f)	new_z -= .5f;
  else			new_z += .5f;

//   cerr << "lay: " << lay->name << " z:" << trans[0] << " new_z: " << new_z << endl;

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
  if(nv())
    nv()->Render();
}

void LayerView::DispOutputName() {
  disp_mode = DISP_OUTPUT_NAME;
  UpdateNetLayDispMode();
  if(nv())
    nv()->Render();
}

void LayerView::UseViewer(T3DataViewMain* viewer) {
  disp_mode = DISP_FRAME;
  UpdateNetLayDispMode();

  if(!viewer) return;
  NetView* nv = this->nv();
  Layer* lay = this->layer(); //cache
  if(!nv || !lay) return;

  TDCoord pos; lay->GetAbsPos(pos);
  viewer->main_xform = nv->main_xform; // first get the network

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->main_xform.rotate.x, nv->main_xform.rotate.y, 
			   nv->main_xform.rotate.z), nv->main_xform.rotate.rot);

  float szx = ((float)lay->scaled_act_geom.x / nv->max_size.x);
  float szy = ((float)lay->scaled_act_geom.y / nv->max_size.y);

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
  FloatTDCoord sc;
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
//   PrjnView		//
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
  NetView* nv = this->nv();
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
  NetView* nv = this->nv();
  Projection* prjn = this->prjn(); // cache
  setNode(new T3PrjnNode(this, prjn->projected, nv->view_params.prjn_width));
  DoHighlightColor(false);
  inherited::Render_pre();
}

void PrjnView::Render_impl() {
  T3PrjnNode* node_so = this->node_so(); // cache
  NetView* nv = this->nv();
  if(!node_so) return;

  // find the total receive num, and our ordinal
  Projection* prjn = this->prjn(); // cache
  Layer* lay_fr = prjn->from;
  Layer* lay_to = prjn->layer;

  if(lay_fr == NULL) lay_fr = lay_to;

  FloatTDCoord src;		// source and dest coords
  FloatTDCoord dst;
  TDCoord lay_fr_pos; lay_fr->GetAbsPos(lay_fr_pos);
  TDCoord lay_to_pos; lay_to->GetAbsPos(lay_to_pos);

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
    src.x = ((float)lay_fr_pos.x + .5f * (float)lay_fr->scaled_act_geom.x) / nv->max_size.x;
    src.z = -((float)(lay_fr_pos.y + lay_fr->scaled_act_geom.y) / nv->max_size.y) - lay_wd;

    // dest is *front* *center*
    dst.x = ((float)lay_to_pos.x + .5f * (float)lay_to->scaled_act_geom.x) / nv->max_size.x;
    dst.z = -((float)lay_to_pos.y / nv->max_size.y) + lay_wd;
  }
  else if(nv->view_params.prjn_disp == NetViewParams::L_R_F) { // easier to see
    // origin is *front* left
    src.x = ((float)lay_fr_pos.x) / nv->max_size.x + lay_wd;
    src.z = -((float)(lay_fr_pos.y) / nv->max_size.y) + lay_wd;

    // dest is *front* right
    dst.x = ((float)lay_to_pos.x + (float)lay_to->scaled_act_geom.x) / nv->max_size.x - lay_wd;
    dst.z = -((float)lay_to_pos.y / nv->max_size.y) + lay_wd;
  }
  else if(nv->view_params.prjn_disp == NetViewParams::L_R_B) { // out of the way
    // origin is *back* left
    src.x = ((float)lay_fr_pos.x) / nv->max_size.x + lay_wd;
    src.z = -((float)(lay_fr_pos.y + lay_fr->scaled_act_geom.y) / nv->max_size.y) - lay_wd;

    // dest is *back* right
    dst.x = ((float)lay_to_pos.x + (float)lay_to->scaled_act_geom.x) / nv->max_size.x - lay_wd;
    dst.z = -((float)(lay_to_pos.y  + lay_to->scaled_act_geom.y) / nv->max_size.y) - lay_wd;
  }

  if(dst.y == src.y && dst.x == src.x && dst.z == src.z) {
    dst.x += lay_wd;		// give it some minimal something
  }

  transform(true)->translate.SetXYZ(src.x, src.y, src.z);
  node_so->setEndPoint(SbVec3f(dst.x - src.x, dst.y - src.y, dst.z - src.z));

  // caption location is half way
  if(nv->view_params.prjn_name) {
    FloatTDCoord cap((dst.x - src.x) / 2.0f - .05f, (dst.y - src.y) / 2.0f, (dst.z - src.z) / 2.0f);
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
  root_laygp = false;		// set by NetView after init, during build
}

void LayerGroupView::Destroy() {
  Reset();
}

void LayerGroupView::BuildAll() {
  Reset(); //for when we are invoked after initial construction
//  String node_nm;

  Layer_Group* lgp = layer_group(); //cache
  for(int li=0;li<lgp->size;li++) {
    Layer* lay = lgp->FastEl(li);
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
//  NetView* nv = this->nv();
  
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

  NetView* nv = this->nv();
  if(!nv->lay_mv) show_drag = false;

  if(root_laygp)
    show_drag = false;		// never for root.

  setNode(new T3LayerGroupNode(this, show_drag, root_laygp));
  DoHighlightColor(false);

  inherited::Render_pre();
}

void LayerGroupView::Render_impl() {
  Layer_Group* lgp = this->layer_group(); //cache
  NetView* nv = this->nv();

  TDCoord pos; lgp->GetAbsPos(pos);
  FloatTransform* ft = transform(true);
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
		   lgp->max_size.x, lgp->max_size.y, lgp->max_size.z,
		   nv->max_size.x, nv->max_size.y, nv->max_size.z);

  if(!root_laygp) {
    node_so->drawStyle()->lineWidth = nv->view_params.laygp_width;

    node_so->setCaption(data()->GetName().chars());
    float lay_wd_y = T3LayerNode::width / nv->max_size.y;
    float lay_ht_z = T3LayerNode::height / nv->max_size.z;
    float fx = (float)lgp->max_size.x / nv->max_size.x;
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
  LayerGroupView* lv = (LayerGroupView*)laynd->dataView();
  Layer_Group* lgp = lv->layer_group();
  NetView* nv = lv->nv();

  float fx = ((float)lgp->max_size.x + 2.0f * T3LayerNode::width) / nv->max_size.x;
  float fy = ((float)lgp->max_size.y + 2.0f * T3LayerNode::width) / nv->max_size.y;
  float fz = ((float)(lgp->max_size.z-1) + 4.0f * T3LayerNode::height) / nv->max_size.z;
  float xfrac = (.5f * fx) - (T3LayerNode::width / nv->max_size.x);
  float yfrac = (.5f * fy) - (T3LayerNode::width / nv->max_size.y);
  float zfrac = (.5f * fz) - 2.0f * (T3LayerNode::height / nv->max_size.z);

  const SbVec3f& trans = dragger->translation.getValue();
  float new_x = trans[0] * nv->max_size.x;
  float new_y = trans[1] * nv->max_size.y;
  if(new_x < 0.0f) 	new_x -= .5f; // add an offset to effect rounding.
  else			new_x += .5f;
  if(new_y < 0.0f) 	new_y -= .5f;
  else			new_y += .5f;

//   cerr << "lay: " << lgp->name << " " << trans[0] << " " << trans[1] << " drg: " <<
//     drag_sc << " fx: " << fx << " fy: " << fy << " new: " << new_x << " " << new_y << endl;

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
  LayerGroupView* lv = (LayerGroupView*)laynd->dataView();
  Layer_Group* lgp = lv->layer_group();
  NetView* nv = lv->nv();

  float fz = (float)lgp->max_size.z / nv->max_size.z;
  float zfrac = .5f * fz;

  const SbVec3f& trans = dragger->translation.getValue();
  float new_z = trans[0] * nv->max_size.z;
  if(new_z < 0.0f)	new_z -= .5f;
  else			new_z += .5f;

//   cerr << "lay: " << lgp->name << " z:" << trans[0] << " new_z: " << new_z << endl;

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
//	NetViewObjView

void NetViewObjView::Initialize(){
  data_base = &TA_NetViewObj;
}

void NetViewObjView::Copy_(const NetViewObjView& cp) {
  name = cp.name;
}

void NetViewObjView::Destroy() {
  CutLinks();
}

bool NetViewObjView::SetName(const String& value) { 
  name = value;  
  return true; 
} 

void NetViewObjView::SetObj(NetViewObj* ob) {
  if (Obj() == ob) return;
  SetData(ob);
  if(ob) {
    if (name != ob->name) {
      name = ob->name;
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
  NetViewObjView* nvov = (NetViewObjView*)nvoso->dataView();
  NetViewObj* nvo = nvov->Obj();
  NetView* nv = GET_OWNER(nvov, NetView);

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nvo->rot.x, nvo->rot.y, nvo->rot.z), nvo->rot.rot);

  SbVec3f trans = dragger->translation.getValue();
//   cerr << "trans: " << trans[0] << " " << trans[1] << " " << trans[2] << endl;
  cur_rot.multVec(trans, trans); // rotate the translation by current rotation
  trans[0] *= nvo->scale.x;  trans[1] *= nvo->scale.y;  trans[2] *= nvo->scale.z;
  FloatTDCoord tr(trans[0], trans[1], trans[2]);
  nvo->pos += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
//   cerr << "scale: " << scale[0] << " " << scale[1] << " " << scale[2] << endl;
  FloatTDCoord sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;	// prevent scale from going to small too fast!!
  nvo->scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
//   cerr << "orient: " << axis[0] << " " << axis[1] << " " << axis[2] << " " << angle << endl;
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
//   NetView		//
//////////////////////////

void NetViewFontSizes::Initialize() {
  net_name = .05f;
  net_vals = .05f;
  layer = .04f;
  layer_vals = .03f;
  prjn = .01f;
  unit = .02f;
  un_nm_len = 3;
}

void NetViewParams::Initialize() {
  xy_square = false;
  unit_spacing = .05f;
  prjn_disp = L_R_F;
  prjn_name = false;
  prjn_width = .002f;
  prjn_trans = .5f;
  lay_trans = .5f;
  unit_trans = 0.6f;
  laygp_width = 1.0f;
}

/*

  Scale Range semantics

  The user can choose for each variable viewed whether to autoscale, and if not, what is the scale.
  Since only one scale can be in effect at once, and the scales are keyed by the display name
  (ex. "act", "r.wt", etc.), the scale system is keyed to this value.

*/
NetView* NetView::New(Network* net, T3DataViewFrame*& fr) {
  if (!net) return NULL;
  if (fr) {
    //note: even if fr specified, need to insure it is right proj for object
    if (!net->SameScope(fr, &TA_taProject)) {
      taMisc::Error("The viewer you specified is not in the same Project as the net.");
      return NULL;
    }
    // check if already viewing this obj there, warn user
    T3DataView* dv = fr->FindRootViewOfData(net);
    if (dv) {
      if (taMisc::Choice("This network is already shown in that frame -- would you like"
          " to show it in a new frame?", "&Ok", "&Cancel") != 0) return NULL;
      fr = NULL; // make a new one
    }
  } 
  if (!fr) {
    fr = T3DataViewer::GetBlankOrNewT3DataViewFrame(net);
  }
  if (!fr) return NULL; // unexpected...
  
  // create NetView
  NetView* nv = new NetView();
  nv->SetData(net);
  nv->GetMaxSize();
  fr->AddView(nv);

  // make sure we've got it all rendered:
  nv->main_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, .35f);
  nv->BuildAll();
  fr->Render();
  fr->ViewAll();
  if(fr->singleChild())
    fr->SaveCurView(0);
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

Unit* Network::GetViewSrcU() {
  NetView* nv = FindView();
  if(nv) return nv->unit_src;
  return NULL;
}

void Network::PlaceNetText(NetTextLoc net_text_loc, float scale) {
  NetView* nv = FindView();
  if(!nv) return;

  nv->net_text_xform.scale = scale;

  switch(net_text_loc) {
  case NT_BOTTOM:		// x, z, y from perspective of netview
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

void NetView::Initialize() {
  data_base = &TA_Network;
  unit_disp_md = NULL;
  unit_sr = NULL;
  unit_md_flags = MD_UNKNOWN;
  nvp = NULL;
  display = true;
  lay_mv = true;
  net_text = true;
  net_text_xform.translate.SetXYZ(0.0f, 1.0f, -1.0f); // start at top back
  net_text_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 0.5f * taMath_float::pi); // start at right mid
  net_text_xform.scale = 0.7f;
  net_text_rot = -90.0f;
  unit_disp_mode = UDM_BLOCK;
  unit_text_disp = UTD_NONE;
  unit_con_md = false;
  wt_line_disp = false;
  wt_line_width = 4.0f;
  wt_line_thr = .5f;
  wt_line_swt = false;
  snap_bord_disp = false;
  snap_bord_width = 4.0f;
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
  taBase::Own(ordered_uvg_list, this);
  taBase::Own(max_size, this);
  taBase::Own(font_sizes, this);
  taBase::Own(view_params, this);
  taBase::Own(net_text_xform, this);
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
  ordered_uvg_list.CutLinks();
  scale_ranges.CutLinks();
  scale.CutLinks();
  lay_disp_modes.CutLinks();
  inherited::CutLinks();
}

void NetView::CopyFromView(NetView* cp) {
  Copy_(*cp);
  // nothing else to do..
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

String NetView::GetLabel() const {
  Network* nt = net();
  if(nt) return nt->GetDisplayName();
  return "(no net)";
}

String NetView::GetName() const {
  Network* nt = net();
  if(nt) return nt->GetDisplayName();
  return "(no net)";
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
    if(!ly) continue;		// can happen, apparently!
    int i = lay_disp_modes.FindName(ly->name);
    if(i < 0) {
      NameVar dmv;
      dmv.name = ly->name;
      dmv.value = -1;		// uninit val
      lay_disp_modes.Insert(dmv, li);
    } 
    else if(i != li) {
      lay_disp_modes.MoveIdx(i, li);
    }
  }

  { // delegate everything to the layers group
    LayerGroupView* lv = new LayerGroupView();
    lv->SetData(&nt->layers);
    lv->root_laygp = true;	// root guy 4 sure!
    children.Add(lv);
    lv->BuildAll();
  }

  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, net()->layers., li) {
    Projection* prjn;
    taLeafItr j;
    FOR_ITR_EL(Projection, prjn, lay->projections., j) {
      PrjnView* pv = new PrjnView();
      pv->SetData(prjn);
      //nn prjns.Add(pv); // this is automatic from the childadding thing
      children.Add(pv);
    }
  }

  NetViewObj* obj;
  taLeafItr oi;
  FOR_ITR_EL(NetViewObj, obj, net()->view_objs., oi) {
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
    prjns.RemoveEl(child);	// just try it
  }
  inherited::ChildRemoving(child_);
}

void NetView::DataUpdateAfterEdit_impl() {
  UpdatePanel();
  // maybe rerender???
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
  setUnitDispMd(NULL);
  membs.Reset();
  TypeDef* prv_td = NULL;
  Layer* lay;
  taLeafItr l_itr;
  
  // first do the unit variables
  FOR_ITR_EL(Layer, lay, net()->layers., l_itr) {
    Unit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(Unit, u, lay->units., u_itr) {
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
	  membs.Add(nmd);	// index now reflects position in list...
	  nmd->idx = md->idx;	// so restore it to the orig one
	}	// check for nongroup owned sub fields (ex. bias)
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
    FOR_ITR_EL(Layer, lay, net()->layers., l_itr) {
      Projection* prjn;
      taLeafItr p_itr;
      FOR_ITR_EL(Projection, prjn, lay->projections., p_itr) {
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
    String_Array& oul = ordered_uvg_list;
    for (int i=oul.size-1; i>=0; i--) { 
      if (!membs.FindName(oul[i]))
        oul.RemoveIdx(i,1);
    }
  }
}

void NetView::GetUnitColor(float val,  iColor& col, float& sc_val) {
  iColor fl;  iColor tx;
  scale.GetColor(val,sc_val,&fl,&tx);
  col = fl;
}

void NetView::GetUnitDisplayVals(UnitGroupView* ugrv, TwoDCoord& co, float& val, T3Color& col,
				 float& sc_val) {
  val = scale.zero;
  sc_val = scale.zero;
  void* base = ugrv->uvd_arr.FastEl(co).disp_base;
  if(!base || !unit_disp_md || (unit_md_flags == MD_UNKNOWN)) {
    col.setValue(.8f, .8f, .8f); // lt gray
    return;
  }

  //  val = ugrv->GetUnitDisplayVal(co, unit_md_flags);
  switch (unit_md_flags) {
  case NetView::MD_FLOAT:
    val = *((float*)base); break;
  case NetView::MD_DOUBLE:
    val = *((double*)base); break;
  case NetView::MD_INT:
    val = *((int*)base); break;
  default:
    break;
  }

  iColor tc;
  GetUnitColor(val, tc, sc_val);
  col.setValue(tc.redf(), tc.greenf(), tc.bluef());
}

void NetView::InitDisplay(bool init_panel) {
  // this build all doesn't work -- nukes too many things that are not 
  // rebuilt later (requires full re-Render, not just Render_impl. not sure
  // why not just do that..  much more invasive I guess
//   BuildAll();	// rebuild views

  GetMaxSize();

  GetMembs();
  if (init_panel) {
    InitPanel();
    UpdatePanel();
  }
  // select "act" by default, if nothing selected, or saved selection not restored yet (first after load) -- skip if still not initialized
  if (!unit_disp_md && (membs.size > 0)) {
    MemberDef* md = NULL;
    if (ordered_uvg_list.size > 0) {
      md = membs.FindName(ordered_uvg_list[0]);
    }
    if (md) {
      setUnitDispMd(md);
    } else {// default
      SelectVar("act", false, false);
    }
  }

  if(children.size > 0) {
    LayerGroupView* lv = (LayerGroupView*)children.FastEl(0);
    lv->InitDisplay();
  }
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
  net()->UpdateMaxSize();
  max_size = net()->max_size;
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
  InitDisplay();
  
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

  if(vw) {			// add hot buttons to viewer
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
  NetView* nv = (NetView*)netnd->dataView();

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->main_xform.rotate.x, nv->main_xform.rotate.y, 
			   nv->main_xform.rotate.z), nv->main_xform.rotate.rot);

  SbVec3f trans = dragger->translation.getValue();
//   cerr << "trans: " << trans[0] << " " << trans[1] << " " << trans[2] << endl;
  cur_rot.multVec(trans, trans); // rotate the translation by current rotation
  trans[0] *= nv->main_xform.scale.x;  trans[1] *= nv->main_xform.scale.y;
  trans[2] *= nv->main_xform.scale.z;
  FloatTDCoord tr(trans[0], trans[1], trans[2]);
  nv->main_xform.translate += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
//   cerr << "scale: " << scale[0] << " " << scale[1] << " " << scale[2] << endl;
  FloatTDCoord sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;	// prevent scale from going to small too fast!!
  nv->main_xform.scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
//   cerr << "orient: " << axis[0] << " " << axis[1] << " " << axis[2] << " " << angle << endl;
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
  NetView* nv = (NetView*)netnd->dataView();

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->net_text_xform.rotate.x, nv->net_text_xform.rotate.y, 
			   nv->net_text_xform.rotate.z), nv->net_text_xform.rotate.rot);

  SbVec3f trans = dragger->translation.getValue();
//   cerr << "trans: " << trans[0] << " " << trans[1] << " " << trans[2] << endl;
  cur_rot.multVec(trans, trans); // rotate the translation by current rotation
  trans[0] *= nv->net_text_xform.scale.x;  trans[1] *= nv->net_text_xform.scale.y;
  trans[2] *= nv->net_text_xform.scale.z;
  FloatTDCoord tr(trans[0], trans[1], trans[2]);
  nv->net_text_xform.translate += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
//   cerr << "scale: " << scale[0] << " " << scale[1] << " " << scale[2] << endl;
  FloatTDCoord sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;	// prevent scale from going to small too fast!!
  nv->net_text_xform.scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
//   cerr << "orient: " << axis[0] << " " << axis[1] << " " << axis[2] << " " << angle << endl;
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
  //  cerr << "nv render_impl" << endl;
  FloatTransform* ft = transform(true);
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
  Render_wt_lines();

  inherited::Render_impl();
  //  taiMisc::RunPending();
}

void NetView::Render_net_text() {
  T3NetNode* node_so = this->node_so(); //cache
  SoSeparator* net_txt = node_so->netText();
  if(!net_txt) return;		// screwup

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

  int txt_st_off = 3 + 1;	// 3 we add below + 1 transform
  if(node_so->netTextDrag())
    txt_st_off+=2;		// dragger + extra xform

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
    String el = md->name + ": " + md->type->GetValStr(md->GetOff((void*)net()));
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
  TDCoord src_lay_pos; src_lay->GetAbsPos(src_lay_pos);
  
  drw->style = SoDrawStyleElement::LINES;
  drw->lineWidth = wt_line_width;
  vtx_prop->materialBinding.setValue(SoMaterialBinding::PER_PART_INDEXED); // part = line segment = same as FACE but likely to be faster to compute line segs?

  // count the number of lines etc
  int n_prjns = 0;
  int n_vtx = 0;
  int n_coord = 0;
  int n_mat = 0;

  bool swt = wt_line_swt;

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

  if((bool)wt_prjn_lay) {
    // this does all the heavy lifting: projecting into unit wt_prjn
    net()->ProjectUnitWeights(unit_src, wt_line_thr, swt);

    int n_con = 0;
    Unit* u;
    taLeafItr ui;
    FOR_ITR_EL(Unit, u, wt_prjn_lay->units., ui) {
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

  SbVec3f* vertex_dat = vertex.startEditing();
  int32_t* coords_dat = coords.startEditing();
  uint32_t* color_dat = color.startEditing();
  int32_t* mats_dat = mats.startEditing();
  int v_idx = 0;
  int c_idx = 0;
  int cidx = 0;
  int midx = 0;

  // note: only want layer_rel for ru_pos
  TwoDCoord ru_pos; unit_src->GetLayerAbsPos(ru_pos);
  FloatTDCoord src;		// source and dest coords
  FloatTDCoord dst;

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
    TDCoord lay_fr_pos; lay_fr->GetAbsPos(lay_fr_pos);
    TDCoord lay_to_pos; lay_to->GetAbsPos(lay_to_pos);
    
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
      TwoDCoord su_pos; su->GetLayerAbsPos(su_pos);
      dst.x = ((float)lay_fr_pos.x + (float)su_pos.x + .5f) / max_size.x;
      dst.z = -((float)lay_fr_pos.y + (float)su_pos.y + .5f) / max_size.y;

      coords_dat[cidx++] = ru_idx; coords_dat[cidx++] = v_idx; coords_dat[cidx++] = -1;
      mats_dat[midx++] = c_idx;	// one per

      vertex_dat[v_idx++].setValue(dst.x, dst.y, dst.z);

      // color
      GetUnitColor(wt, tc, sc_val);
      col.setValue(tc.redf(), tc.greenf(), tc.bluef());
      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      color_dat[c_idx++] = T3Color::makePackedRGBA(col.r, col.g, col.b, alpha);
    }
  }

  if((bool)wt_prjn_lay) {
    TDCoord wt_prjn_lay_pos; wt_prjn_lay->GetAbsPos(wt_prjn_lay_pos);
    
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

    Unit* su;
    taLeafItr ui;
    FOR_ITR_EL(Unit, su, wt_prjn_lay->units., ui) {
      float wt = su->wt_prjn;
      if(fabsf(wt) < wt_line_thr) continue;

      TDCoord su_pos; su->GetAbsPos(su_pos);
      dst.x = ((float)wt_prjn_lay_pos.x + (float)su_pos.x + .5f) / max_size.x;
      dst.z = -((float)wt_prjn_lay_pos.y + (float)su_pos.y + .5f) / max_size.y;

      coords_dat[cidx++] = ru_idx; coords_dat[cidx++] = v_idx; coords_dat[cidx++] = -1;
      mats_dat[midx++] = c_idx;	// one per

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
  prjns.Reset();
  inherited::Reset_impl();
}

void NetView::SelectVar(const char* var_name, bool add, bool update) {
  if (!add)
    ordered_uvg_list.Reset();
  MemberDef* md = (MemberDef*)membs.FindName(var_name);
  if (md) {
    //nnif (ordered_uvg_list.FindEl(var_name) < 0)
    ordered_uvg_list.AddUnique(var_name);
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
  if (unit_sr) {
    unit_sr->SetFromScale(scale);
  }
  if (update_panel) UpdatePanel();
}

void NetView::SetScaleDefault() {
  if (unit_sr)  {
    InitScaleRange(*unit_sr);
    scale.SetFromScaleRange(unit_sr);
  } else {
    scale.auto_scale = false;
    scale.SetMinMax(-1.0f, 1.0f);
  }
}

void NetView::SetColorSpec(ColorScaleSpec* color_spec) {
  scale.SetColorSpec(color_spec);
  UpdateDisplay(true);		// true causes button to remain pressed..
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
  }
}

void NetView::setUnitDisp(int value) {
  MemberDef* md = membs.PosSafeEl(value);
  setUnitDispMd(md);
}

void NetView::setUnitDispMd(MemberDef* md) {
  if (md == unit_disp_md) return;
  unit_disp_md = md;
  unit_sr = NULL;
  unit_md_flags = MD_UNKNOWN;
  if (!unit_disp_md) return;
  if (unit_disp_md) {
    if (unit_disp_md->type->InheritsFrom(&TA_float))
      unit_md_flags = MD_FLOAT;
    else if (unit_disp_md->type->InheritsFrom(&TA_double))
      unit_md_flags = MD_DOUBLE;
    else if (unit_disp_md->type->InheritsFrom(&TA_int))
      unit_md_flags = MD_INT;
  }

  String nm = unit_disp_md->name;
  unit_sr = scale_ranges.FindName(nm);
  if (unit_sr == NULL) {
    unit_sr = (ScaleRange*)(scale_ranges.New(1,&TA_ScaleRange));
    unit_sr->name = nm;
    InitScaleRange(*unit_sr);
  }
  scale.SetFromScaleRange(unit_sr);
}

void NetView::UpdateViewerModeForMd(MemberDef* md) {
  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
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
  return false;
}

void NetView::UpdateAutoScale() {
  bool updated = false;
  if(children.size > 0) {
    LayerGroupView* lv = (LayerGroupView*)children.FastEl(0);
    lv->UpdateAutoScale(updated);
  }
  if (updated) { //note: could really only not be updated if there were no items
    scale.SymRange();		// keep it symmetric
    if (unit_sr)
      unit_sr->SetFromScale(scale); // update values
    if (nvp) {
      nvp->ColorScaleFromData();
    }
  } else { // set scale back to default so it doesn't look bizarre
    scale.SetMinMax(-1.0f, 1.0f);
  }
}

void NetView::UpdateDisplay(bool update_panel) { // redoes everything
  if (update_panel) UpdatePanel();
  Render_impl();
}

void NetView::UpdateUnitValues() { // *actually* only does unit value updating
  if(children.size == 0) return;
  LayerGroupView* lv = (LayerGroupView*)children.FastEl(0);
  lv->UpdateUnitValues();
}

void NetView::DataUpdateView_impl() {
  if (!display) return;
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
//   NetViewPanel 	//
//////////////////////////

#define CMD_GEOM_X   2

NetViewPanel::NetViewPanel(NetView* dv_)
:inherited(dv_)
{
  int font_spec = taiMisc::fonMedium;
  m_cur_spec = NULL;
  req_full_redraw = false;

  T3ExaminerViewer* vw = dv_->GetViewer();
  if(vw)
    connect(vw, SIGNAL(dynbuttonActivated(int)), this, SLOT(dynbuttonActivated(int)) );
  
  QWidget* widg = new QWidget();
  layTopCtrls = new QVBoxLayout(widg); //layWidg->addLayout(layTopCtrls);
  layTopCtrls->setSpacing(taiM->vsep_c);
  layTopCtrls->setMargin(0);

  layViewParams = new QVBoxLayout(); layTopCtrls->addLayout(layViewParams);
  layViewParams->setSpacing(taiM->vsep_c);

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

  lblPrjnWdth = taiM->NewLabel("Prjn\nWdth", widg, font_spec);
  lblPrjnWdth->setToolTip("Width of projection lines -- .002 is default (very thin!) -- increase if editing projections so they are easier to select.");
  layFontsEtc->addWidget(lblPrjnWdth);
  fldPrjnWdth = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldPrjnWdth->GetRep());
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblUnitTrans = taiM->NewLabel("Trans\nparency", widg, font_spec);
  lblUnitTrans->setToolTip("Unit maximum transparency level: 0 = all units opaque; 1 = inactive units are completely invisible.\n .6 = default; transparency is inversely related to value magnitude.");
  layFontsEtc->addWidget(lblUnitTrans);
  fldUnitTrans = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldUnitTrans->GetRep());
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblUnitFont = taiM->NewLabel("Font\nSize", widg, font_spec);
  lblUnitFont->setToolTip("Unit text font size (as a proportion of entire network display). .02 is default.");
  layFontsEtc->addWidget(lblUnitFont);
  fldUnitFont = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldUnitFont->GetRep());
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblLayFont = taiM->NewLabel("Layer\nFont Sz", widg, font_spec);
  lblLayFont->setToolTip("Layer name font size (as a proportion of entire network display). .04 is default.");
  layFontsEtc->addWidget(lblLayFont);
  fldLayFont = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldLayFont->GetRep());
  layFontsEtc->addSpacing(taiM->hsep_c);

  chkXYSquare = new QCheckBox("XY\nSquare", widg);
  chkXYSquare->setToolTip("Make the X and Y size of network the same, so that unit cubes are always square (but can waste a certain amount of display space).");
  connect(chkXYSquare, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layFontsEtc->addWidget(chkXYSquare);
  layFontsEtc->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layDisplayValues = new QVBoxLayout();  layTopCtrls->addLayout(layDisplayValues); //gbDisplayValues);

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
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  lblUnitSpacing = taiM->NewLabel("Unit\nSpace", widg, font_spec);
  lblUnitSpacing->setToolTip("Spacing between units, as a proportion of the total width of the unit box"); 
  layColorScaleCtrls->addWidget(lblUnitSpacing);
  fldUnitSpacing = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldUnitSpacing->GetRep());
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
  lblWtLineWdth->setToolTip("Width of weight lines"); 
  layColorScaleCtrls->addWidget(lblWtLineWdth);
  fldWtLineWdth = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldWtLineWdth->GetRep());
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  lblWtLineThr = taiM->NewLabel("Thr", widg, font_spec);
  lblWtLineThr->setToolTip("Threshold for displaying weight lines: weight magnitudes below this value are not shown.");
  layColorScaleCtrls->addWidget(lblWtLineThr);
  fldWtLineThr = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldWtLineThr->GetRep());
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  int list_flags = taiData::flgNullOk | taiData::flgAutoApply;

  lblWtPrjnLay = taiM->NewLabel("Wt\nPrjn", widg, font_spec);
  lblWtPrjnLay->setToolTip("Layer to display weight projection values onto for selected unit (values are visible on all units in the wt_prjn unit variable if this setting is non-null)");
  layColorScaleCtrls->addWidget(lblWtPrjnLay);
  gelWtPrjnLay = dl.Add(new taiGroupElsButton(&TA_Layer_Group, this, NULL, widg, list_flags));
  layColorScaleCtrls->addWidget(gelWtPrjnLay->GetRep());
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
  
  taBase* specs_ = &(dv_->net()->specs);
  MemberDef* md = dv_->net()->GetTypeDef()->members.FindName("specs");
  if (specs_) {
    taiDataLink* dl = (taiDataLink*)specs_->GetDataLink();
    if (dl) {
      dl->CreateTreeDataNode(md, tvSpecs, NULL, "specs");
    }
  }

  tvSpecs->resizeColumnToContents(0); // just make sure everythign fits

  tvSpecs->Connect_SelectableHostNotifySignal(this,
    SLOT(tvSpecs_Notify(ISelectableHost*, int)) );
//   connect(tvSpecs, SIGNAL(ItemSelected(iTreeViewItem*)),
//     this, SLOT(tvSpecs_ItemSelected(iTreeViewItem*)) );
  connect(tvSpecs, SIGNAL(CustomExpandFilter(iTreeViewItem*, int, bool&)),
    this, SLOT(tvSpecs_CustomExpandFilter(iTreeViewItem*, int, bool&)) ); 
  
  layOuter->setStretchFactor(scr, 0); // so it only uses exact spacing
  // so doesn't have tiny scrollable annoying area:
  // (the tradeoff is that if you squish the whole thing, eventually you can't 
  // get at all the properties)
  scr->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  layOuter->addWidget(tw, 2);
  
  ////////////////////////////////////////////////////////////////////////////
  // Command Buttons
  widCmdButtons = new QWidget(widg);
  iFlowLayout* fl = new iFlowLayout(widCmdButtons);
//  layTopCtrls->addWidget(widCmdButtons);
  layOuter->addWidget(widCmdButtons);
  
  meth_but_mgr = new iMethodButtonMgr(widCmdButtons, fl, widCmdButtons); 
  meth_but_mgr->Constr(nv()->net());

//  setCentralWidget(widg);
  MakeButtons(layOuter);
}

NetViewPanel::~NetViewPanel() {
  NetView* nv_ = nv();
  if (nv_) {
    nv_->nvp = NULL;
  }
}

void NetViewPanel::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();
  ++updating;
  NetView* nv = this->nv(); // cache
  if (!nv) return;
  if (req_full_redraw) {
    req_full_redraw = false;
    nv->Reset();
    nv->BuildAll();
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
  gelWtPrjnLay->GetImage(&(nv->net()->layers), nv->wt_prjn_lay.ptr());

  fldUnitTrans->GetImage((String)nv->view_params.unit_trans);
  fldUnitFont->GetImage((String)nv->font_sizes.unit);
  fldLayFont->GetImage((String)nv->font_sizes.layer);
  chkXYSquare->setChecked(nv->view_params.xy_square);

  // update var selection
  int i = 0;
  QTreeWidgetItemIterator it(lvDisplayValues);
  QTreeWidgetItem* item = NULL;
  while (*it) {
    item = *it;
    bool is_selected = (nv->ordered_uvg_list.FindEl(item->text(0)) >= 0);
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
  NetView* nv = this->nv(); // cache
  if (!nv) return;
  req_full_redraw = true;	// not worth micro-managing: MOST changes require full redraw!

  nv->display = chkDisplay->isChecked();
  nv->lay_mv = chkLayMove->isChecked();
  nv->net_text = chkNetText->isChecked();
  nv->net_text_rot = (float)fldTextRot->GetValue();

  int i; 
  cmbUnitText->GetEnumValue(i);
  nv->unit_text_disp = (NetView::UnitTextDisplay)i;
  
  cmbDispMode->GetEnumValue(i);
  //  req_full_redraw = req_full_redraw || (nv->unit_disp_mode != i);
  nv->unit_disp_mode = (NetView::UnitDisplayMode)i;
  
  cmbPrjnDisp->GetEnumValue(i);
  nv->view_params.prjn_disp = (NetViewParams::PrjnDisp)i;
  
  nv->view_params.prjn_width = (float)fldPrjnWdth->GetValue();

  nv->view_params.unit_trans = (float)fldUnitTrans->GetValue();
  nv->font_sizes.unit = (float)fldUnitFont->GetValue();
  nv->font_sizes.layer = (float)fldLayFont->GetValue();

  nv->snap_bord_disp = chkSnapBord->isChecked();
  nv->snap_bord_width = (float)fldSnapBordWdth->GetValue();
  nv->view_params.unit_spacing = (float)fldUnitSpacing->GetValue();

  nv->wt_line_disp = chkWtLines->isChecked();
  nv->wt_line_swt = chkWtLineSwt->isChecked();
  nv->wt_line_width = (float)fldWtLineWdth->GetValue();
  nv->wt_line_thr = (float)fldWtLineThr->GetValue();
  nv->wt_prjn_lay = (Layer*)gelWtPrjnLay->GetValue();
  nv->view_params.xy_square = chkXYSquare->isChecked();

  nv->SetScaleData(chkAutoScale->isChecked(), cbar->min(), cbar->max(), false);
}

void NetViewPanel::CopyFrom_impl() {
  NetView* nv_; if (!(nv_ = nv())) return;
  nv_->CallFun("CopyFromView");
}

void NetViewPanel::butNewLayer_pressed() {
  NetView* nv_; if (!(nv_ = nv())) return;
  nv_->CallFun("NewLayer");
}

void NetViewPanel::butBuildAll_pressed() {
  NetView* nv_; if (!(nv_ = nv())) return;
  nv_->net()->Build();
}

void NetViewPanel::butConnectAll_pressed() {
  NetView* nv_; if (!(nv_ = nv())) return;
  nv_->net()->Connect();
}

void NetViewPanel::butScaleDefault_pressed() {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;

  nv_->SetScaleDefault();
  nv_->UpdateDisplay(true);
}

void NetViewPanel::butSetColor_pressed() {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;

  nv_->CallFun("SetColorSpec");
}

void NetViewPanel::ColorScaleFromData() {
  NetView* nv_;
  if (!(nv_ = nv())) return;

  ++updating;
  cbar->UpdateScaleValues();
  chkAutoScale->setChecked(nv_->scale.auto_scale); //note: raises signal on widget! (grr...)
  --updating;
}

void NetViewPanel::GetVars() {
  NetView* nv_;
  if (!(nv_ = nv())) return;

  lvDisplayValues->clear();
  if (nv_->membs.size == 0) return;

  MemberDef* md;
  QTreeWidgetItem* lvi = NULL;
  for (int i=0; i < nv_->membs.size; i++) {
    md = nv_->membs[i];
    QStringList itm;
    itm << md->name << md->desc;
    lvi = new QTreeWidgetItem(lvDisplayValues, itm);
  }
  lvDisplayValues->resizeColumnToContents(0);
}

void NetViewPanel::InitPanel() {
  NetView* nv_;
  if (!(nv_ = nv())) return;
  ++updating;
  // fill monitor values
  GetVars();
  --updating;
}

void NetViewPanel::lvDisplayValues_selectionChanged() {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;
  // redo the list each time, to guard against stale values
  nv_->ordered_uvg_list.Reset(); 
  QList<QTreeWidgetItem*> items(lvDisplayValues->selectedItems());
  QTreeWidgetItem* item = NULL;
  for (int j = 0; j < items.size(); ++j) {
    item = items.at(j);
    nv_->ordered_uvg_list.Add(item->text(0));
  }
  MemberDef* md = (MemberDef*)nv_->membs.FindName(nv_->ordered_uvg_list.SafeEl(0));
  if (md) {
    nv_->setUnitDispMd(md); 
    nv_->UpdateViewerModeForMd(md);
  }
  ColorScaleFromData();
  nv_->InitDisplay(false);
  nv_->UpdateDisplay(false);
}

void NetViewPanel::setHighlightSpec(BaseSpec* spec, bool force) {
  if ((spec == m_cur_spec) && !force) return;
  m_cur_spec = spec;
  NetView* nv = this->nv(); // cache
  nv->SetHighlightSpec(spec);
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
  NetView* nv_;
  if (!(nv_ = nv())) return;
  switch (op) {
  //case ISelectableHost::OP_GOT_FOCUS: return;
  case ISelectableHost::OP_SELECTION_CHANGED: {
    taBase* new_base = NULL;
    ISelectable* si = src->curItem();
    if (si && si->link()) {
      new_base = si->link()->taData(); // NULL if not a taBase, shouldn't happen
    }
    setHighlightSpec((BaseSpec*)new_base);
    //    nv_->UpdateDisplay(true);
  } break;
  //case ISelectableHost::OP_DESTROYING: return;
  default: return;
  }
}

void NetViewPanel::tvSpecs_ItemSelected(iTreeViewItem* item) {
  NetView* nv_;
  if (!(nv_ = nv())) return;
  BaseSpec* spec = NULL;
  if (item) {
    taBase* ld_ = (taBase*)item->linkData();
    if (ld_->InheritsFrom(TA_BaseSpec))
      spec = (BaseSpec*)ld_;
  }
  setHighlightSpec(spec);
//   nv_->UpdateDisplay(true);
}


void NetViewPanel::viewWin_NotifySignal(ISelectableHost* src, int op) {
  NetView* nv_;
  if (!(nv_ = nv())) return;
  nv_->viewWin_NotifySignal(src, op);
}

void NetViewPanel::dynbuttonActivated(int but_no) {
  NetView* nv_;
  if (!(nv_ = nv())) return;

  T3ExaminerViewer* vw = nv_->GetViewer();
  if(!vw) return;
  taiAction* dyb = vw->getDynButton(but_no);
  if(!dyb) return;
  String nm = dyb->text();
  nv_->ordered_uvg_list.Reset(); 
  nv_->ordered_uvg_list.Add(nm);
  MemberDef* md = (MemberDef*)nv_->membs.FindName(nm);
  if(md) {
    nv_->setUnitDispMd(md); 
    nv_->UpdateViewerModeForMd(md);
    vw->setDynButtonChecked(but_no, true, true); // mutex
  }
  ColorScaleFromData();
  nv_->InitDisplay(false);
  nv_->UpdateDisplay(true);	// update panel
}
