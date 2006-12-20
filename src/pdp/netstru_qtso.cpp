/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// stuff to implement unit view..

#include "netstru_qtso.h"

//obs #include "netstru_so.h"
#include "ta_geometry.h"
#include "pdp_project.h"
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
#include <Q3ListView>
#include <QScrollArea>
#include <qpushbutton.h>
#include <qwidget.h>

#include <Inventor/SbLinear.h>
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDirectionalLight.h>
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
//temp
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

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

  switch (nv->unit_disp_mode) {
  case NetView::UDM_CIRCLE:
    m_node_so = new T3UnitNode_Circle(this, max_x, max_y, max_z, un_spc);
    break;
  case NetView::UDM_RECT:
    m_node_so = new T3UnitNode_Rect(this, max_x, max_y, max_z, un_spc);
    break;
  case NetView::UDM_BLOCK:
    m_node_so = new T3UnitNode_Block(this, max_x, max_y, max_z, un_spc);
    break;
  case NetView::UDM_CYLINDER:
    m_node_so = new T3UnitNode_Cylinder(this, max_x, max_y, max_z, un_spc);
    break;
  }

  Unit* unit = this->unit(); //cache
  // note: pos s/b invariant
  node_so()->transform()->translation.setValue
    ((float)(unit->pos.x + 0.5f) / max_x,
     0.0f,
     -((float)(unit->pos.y + 0.5f) / max_y));
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

void UnitGroupView::UpdateUnitViewBase(MemberDef* disp_md, Unit* src_u) {
  Unit_Group* ugrp = this->ugrp(); //cache
  AllocUnitViewData();
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
	  if (act_md == NULL)	continue;
	  Connection* con = tcong->FindConFrom(src_u);
	  if (con == NULL) continue;
	  uvd.disp_base = act_md->GetOff(con);
	  break; //TODO: is this right????
	}
      }
      else {
	for(int g=0;g<unit->send.size;g++) {
	  SendCons* tcong = unit->send.FastEl(g);
	  MemberDef* act_md = tcong->con_type->members.FindName(nm);
	  if (act_md == NULL)	continue;
	  Connection* con = tcong->FindConFrom(src_u);
	  if (con == NULL) continue;
	  uvd.disp_base = act_md->GetOff(con);
	  break; //TODO: is this right????
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
    if(unit->bias.cons.size == 0) continue;
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
      nptd = ((TAPtr)par_base)->GetTypeDef(); // get actual typedef
    }
    MemberDef* smd = nptd->members.FindName(nm);
    if (smd == NULL)  continue;
    uvd.disp_base = smd->GetOff(par_base);
  }
}

void UnitGroupView_MouseCB(void* userData, SoEventCallback* ecb) {
  UnitGroupView* ugv = (UnitGroupView*)userData;
  NetView* nv = ugv->nv();
  T3DataViewFrame* frame = GET_OWNER(nv, T3DataViewFrame);
  if(!frame) return;
  SoQtViewer* viewer = frame->widget()->ra();
  SoMouseButtonEvent* mouseevent = (SoMouseButtonEvent*)ecb->getEvent();
  SoRayPickAction rp( viewer->getViewportRegion());
  rp.setPoint(mouseevent->getPosition());
  rp.apply(viewer->getSceneManager()->getSceneGraph());

  SoPickedPoint* pp = rp.getPickedPoint(0);
  if(!pp) return;
  SoNode* pobj = pp->getPath()->getNodeFromTail(2);
//   cerr << "obj typ: " << pobj->getTypeId().getName() << endl;
  if(!pobj->isOfType(T3UnitGroupNode::getClassTypeId())) {
//     cerr << "not unitgroupnode!" << endl;
    return;
  }
  UnitGroupView* act_ugv = (UnitGroupView*)((T3UnitGroupNode*)pobj)->dataView;
  SbVec3f pt = pp->getObjectPoint(pobj); 
//   cerr << "got: " << pt[0] << " " << pt[1] << " " << pt[2] << endl;
  int xp = (int)(pt[0] * nv->max_size.x);
  int yp = (int)-(pt[2] * nv->max_size.y);
  Unit_Group* ugrp = act_ugv->ugrp();
  if(xp >= 0 && xp < ugrp->geom.x && yp >= 0 && yp < ugrp->geom.y) {
    Unit* unit = ugrp->FindUnitFmCoord(xp, yp);
    if(unit) nv->setUnitSrc(NULL, unit);
  }
  nv->InitDisplay();
  nv->UpdateDisplay();
  ecb->setHandled();
}

void UnitGroupView::Render_pre() {
  NetView* nv = this->nv();
  Unit_Group* ugrp = this->ugrp(); //cache

  bool no_units = true;
  if(nv->unit_disp_mode != NetView::UDM_BLOCK)
    no_units = false;

  AllocUnitViewData();
  m_node_so = new T3UnitGroupNode(this, no_units);
  //NOTE: we create/adjust the units in the Render_impl routine
  T3UnitGroupNode* ugrp_so = node_so(); // cache

//   SoMaterial* mat = ugrp_so->material(); //cache
//   mat->diffuseColor.setValue(0.5f, 0.0f, 0.0f);
  //  mat->specularColor.setValue(0.5f, 0.0f, 0.0f);
//   mat->shininess.setValue(.9f);
//   mat->transparency.setValue(0.5f);

  ugrp_so->setGeom(ugrp->geom.x, ugrp->geom.y, nv->max_size.x, nv->max_size.y, nv->max_size.z);

  SoEventCallback* ecb = new SoEventCallback;
  ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), UnitGroupView_MouseCB, this);
  ugrp_so->addChild(ecb);

  inherited::Render_pre();
}

void UnitGroupView::Render_impl() {
  Unit_Group* ugrp = this->ugrp(); //cache
  NetView* nv = this->nv();

  //set origin: 0,0,0
  TDCoord& pos = ugrp->pos;
  FloatTransform* ft = transform(true);
  ft->translate.SetXYZ((float)pos.x / nv->max_size.x,
		       (float)pos.z / nv->max_size.z,
		       (float)-pos.y / nv->max_size.y);

  inherited::Render_impl();
}

void UnitGroupView::ValToDispText(float val, String& str) {
  float val_abs = fabs(val);
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
  if(nv->unit_disp_mode == NetView::UDM_BLOCK) {
    Render_impl_blocks();
    return;
  }

  T3UnitGroupNode* node_so = this->node_so(); // cache

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
  NetView* nv = this->nv(); //cache
  Unit_Group* ugrp = this->ugrp(); //cache
  T3UnitGroupNode* node_so = this->node_so(); // cache
  SoIndexedTriangleStripSet* sits = node_so->shape();

  SoVertexProperty* vtx_prop = new SoVertexProperty;
  sits->vertexProperty.setValue(vtx_prop); // note: vp refs/unrefs automatically
  SoMFVec3f& vertex = vtx_prop->vertex;
  SoMFVec3f& normal = vtx_prop->normal;
  SoMFUInt32& color = vtx_prop->orderedRGBA;

  vtx_prop->normalBinding.setValue(SoNormalBinding::PER_FACE_INDEXED);
  vtx_prop->materialBinding.setValue(SoMaterialBinding::PER_PART_INDEXED);

  normal.setNum(5);
  normal.startEditing();
  int idx=0;
  normal.set1Value(idx++, 0.0f, 0.0f, -1.0f); // back = 0
  normal.set1Value(idx++, 1.0f, 0.0f, 0.0f); // right = 1
  normal.set1Value(idx++, -1.0f, 0.0f, 0.0f); // left = 2
  normal.set1Value(idx++, 0.0f, 0.0f, 1.0f); // front = 3
  normal.set1Value(idx++, 0.0f, 1.0f, 0.0f); // top = 4

  // vertex plan: 5 vals per unit: the 0 (first) and the act. each unit renders the lower left
  // corner; add one extra point at ends for the top and right sides of the last guys

  int n_geom = ugrp->geom.Product();
  int n_per_vtx = 8;
  int tot_vtx =  n_geom * n_per_vtx;
  vertex.setNum(tot_vtx);
  vertex.startEditing();

  color.setNum(n_geom);
  color.startEditing();

  float trans = nv->view_params.unit_trans;
  float spacing = nv->view_params.unit_spacing;
  float max_z = MIN(nv->max_size.x, nv->max_size.y); // smallest XY
  max_z = MAX(max_z, nv->max_size.z); // make sure Z isn't bigger

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
      SoBaseColor* bc = new SoBaseColor;
      bc->rgb.setValue(0, 0, 0); //black is default for text
      un_txt->addChild(bc);
      SoFont* fnt = new SoFont();
      fnt->name = "Arial";
      un_txt->addChild(fnt);
    }
    SoFont* fnt = (SoFont*)un_txt->getChild(1);
    fnt->size.setValue(ufontsz);
  }

  String val_str;
  String unit_name;
  float val;
  float sc_val;
  T3Color col;
  TwoDCoord pos;
  int v_idx = 0;
  int c_idx = 0;
  int t_idx = 2;		// base color + font
  // these go in normal order; indexes are backwards
  for(pos.y=0; pos.y<ugrp->geom.y; pos.y++) {
    for(pos.x=0; pos.x<ugrp->geom.x; pos.x++) { // right to left
      nv->GetUnitDisplayVals(this, pos, val, col, sc_val);
      float xp = ((float)pos.x + spacing) / nv->max_size.x;
      float yp = -((float)pos.y + spacing) / nv->max_size.y;
      float xp1 = ((float)pos.x+1 - spacing) / nv->max_size.x;
      float yp1 = -((float)pos.y+1 - spacing) / nv->max_size.y;
      float zp = .5f * sc_val / max_z;
      vertex.set1Value(v_idx++, xp, 0.0f, yp); // 00_0 = 0
      vertex.set1Value(v_idx++, xp1, 0.0f, yp); // 10_0 = 0
      vertex.set1Value(v_idx++, xp, 0.0f, yp1); // 01_0 = 0
      vertex.set1Value(v_idx++, xp1, 0.0f, yp1); // 11_0 = 0

      vertex.set1Value(v_idx++, xp, zp, yp); // 00_v = 1
      vertex.set1Value(v_idx++, xp1, zp, yp); // 10_v = 2
      vertex.set1Value(v_idx++, xp, zp, yp1); // 01_v = 3
      vertex.set1Value(v_idx++, xp1, zp, yp1); // 11_v = 4

      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      color.set1Value(c_idx++, T3Color::makePackedRGBA(col.r, col.g, col.b, alpha));

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
	if(nv->unit_text_disp & NetView::UTD_VALUES) {
	  ValToDispText(val, val_str);
	}
	if(nv->unit_text_disp & NetView::UTD_NAMES) {
	  Unit* unit = ugrp->FindUnitFmCoord(pos);
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
  color.finishEditing();

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

  coords.startEditing();
  norms.startEditing();
  mats.startEditing();
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

      float zval = vertex[c00_v][1]; // "y" coord = 1

      if(zval < 0.0f) {			 // do "top" first which is actually bottom!
	coords.set1Value(cidx++, c01_v); // 0
	coords.set1Value(cidx++, c11_v); // 1
	coords.set1Value(cidx++, c00_v); // 2
	coords.set1Value(cidx++, c10_v); // 3
	coords.set1Value(cidx++, -1); // -1 -- 5 total

	norms.set1Value(nidx++, 4); // top
	norms.set1Value(nidx++, 4); // top -- 2 total

	mats.set1Value(midx++, mat_idx);
      }

      // back - right
      //     1    3
      //   0    2     
      //     x    5  
      //   x    4   

      coords.set1Value(cidx++, c01_0); // 0
      coords.set1Value(cidx++, c01_v); // 1
      coords.set1Value(cidx++, c11_0); // 2
      coords.set1Value(cidx++, c11_v); // 3
      coords.set1Value(cidx++, c10_0); // 4
      coords.set1Value(cidx++, c10_v); // 5
      coords.set1Value(cidx++, -1); // -1  -- 7 total

      norms.set1Value(nidx++, 0); // back
      norms.set1Value(nidx++, 0); // back
      norms.set1Value(nidx++, 1); // right
      norms.set1Value(nidx++, 1); // right -- 4 total

      mats.set1Value(midx++, mat_idx);

      // left - front
      //     1    x 
      //   0    x   
      //     3    5
      //   2    4   

      coords.set1Value(cidx++, c01_0); // 0
      coords.set1Value(cidx++, c01_v); // 1
      coords.set1Value(cidx++, c00_0); // 2
      coords.set1Value(cidx++, c00_v); // 3
      coords.set1Value(cidx++, c10_0); // 4
      coords.set1Value(cidx++, c10_v); // 5
      coords.set1Value(cidx++, -1); // -1 -- 7 total

      norms.set1Value(nidx++, 2); // left
      norms.set1Value(nidx++, 2); // left
      norms.set1Value(nidx++, 3); // front
      norms.set1Value(nidx++, 3); // front -- 4 total

      mats.set1Value(midx++, mat_idx);

      // triangle strip order is 0 1 2, 2 1 3, 2 3 4
      // top
      //     0    1
      //   x    x  
      //     2    3
      //   x    x  

      if(zval >= 0.0f) {		 // only if higher..
	coords.set1Value(cidx++, c01_v); // 0
	coords.set1Value(cidx++, c11_v); // 1
	coords.set1Value(cidx++, c00_v); // 2
	coords.set1Value(cidx++, c10_v); // 3
	coords.set1Value(cidx++, -1); // -1 -- 5 total

	norms.set1Value(nidx++, 4); // top
	norms.set1Value(nidx++, 4); // top -- 2 total

	mats.set1Value(midx++, mat_idx);
      }

      // total coords = 7 + 7 + 5 = 19
      // total norms = 4 + 4 + 2 = 10
      // total mats = 3
    }
  }
  coords.finishEditing();
  norms.finishEditing();
  mats.finishEditing();
}

void UnitGroupView::Reset_impl() {
  inherited::Reset_impl();
  uvd_arr.SetSize(0);
}


//////////////////////////
//   nvhDataView	//
//////////////////////////

void nvhDataView::Copy_(const nvhDataView& cp) {
  m_hcolor = cp.m_hcolor;
}

void nvhDataView::setHighlightColor(const T3Color& color) {
  m_hcolor = color;
  DoHighlightColor(true);
} 

//////////////////////////
//   LayerView	//
//////////////////////////

void LayerView::Initialize() {
  data_base = &TA_Layer;
}

void LayerView::Destroy() {
  Reset();
}

void LayerView::BuildAll() {
  Reset(); //for when we are invoked after initial construction
//  String node_nm;
  Layer* lay = layer(); //cache
  Unit_Group* ugrp;
  UnitGroupView* ugv;
  if (!lay->unit_groups) { // single ugrp
    ugrp = &(lay->units);
    ugv = new UnitGroupView;
    ugv->SetData(ugrp);//obs ugrp->AddDataView(ugv);
    ugrps.Add(ugv);  // no side-effects -- better to add to this first
    children.Add(ugv);
    ugv->BuildAll();

//    int flags = 0;// BrListViewItem::DNF_UPDATE_NAME | BrListViewItem::DNF_CAN_BROWSE| BrListViewItem::DNF_CAN_DRAG;
  } else { // multi-ugrps
    for (int j = 0; j < lay->gp_geom.n; ++j) {
      ugrp = (Unit_Group*)lay->units.SafeGp(j);
      if (!ugrp) break; // maybe not built yet???

      ugv = new UnitGroupView;
      ugv->SetData(ugrp);//obs ugrp->AddDataView(ugv);
      ugrps.Add(ugv);  // no side-effects -- better to add to this first
      children.Add(ugv);
      ugv->BuildAll();

//      int flags = 0;// BrListViewItem::DNF_UPDATE_NAME | BrListViewItem::DNF_CAN_BROWSE| BrListViewItem::DNF_CAN_DRAG;
//      last_child_node = dl->CreateT3Node(par_node, last_child_node, node_nm, flags);
    }
  }
}

void LayerView::ChildRemoving(taDataView* child_) {
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if (!child) goto done;
  if (ugrps.RemoveEl(child)) goto done;
done:
  inherited::ChildRemoving(child_);
}


void LayerView::DataUpdateAfterEdit_impl() {
  inherited::DataUpdateAfterEdit_impl();
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
    mat->diffuseColor.setValue(0.0f, 0.5f, 0.5f); // cyan
    mat->transparency.setValue(nv->view_params.lay_trans);
  }
} 

void LayerView::Render_pre() {
  m_node_so = new T3LayerNode(this);
  DoHighlightColor(false);

  inherited::Render_pre();
}

void LayerView::Render_impl() {
  Layer* lay = this->layer(); //cache
  NetView* nv = this->nv();

  // set origin: 0,.5,0 in allocated area
  TDCoord& pos = lay->pos;
  FloatTransform* ft = transform(true);
  ft->translate.SetXYZ((float)pos.x / nv->max_size.x,
		       (float)(pos.z + 0.5f) / nv->max_size.z,
		       (float)-pos.y / nv->max_size.y);

  T3LayerNode* node_so = this->node_so(); // cache
  node_so->setGeom(lay->act_geom.x, lay->act_geom.y, nv->max_size.x, nv->max_size.y, nv->max_size.z);
  node_so->setCaption(data()->GetName().chars());
  node_so->resizeCaption(nv->font_sizes.layer);
  inherited::Render_impl();
}

void LayerView::Reset_impl() {
  ugrps.Reset();
  inherited::Reset_impl();
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
  SoMaterial* mat = node_so()->material(); //cache
  if (apply) {
    mat->diffuseColor.setValue(m_hcolor);
    mat->transparency.setValue(0.0f);
  } else {
    mat->diffuseColor.setValue(SbColor(1, 1, 1)); // white
    mat->transparency.setValue(nv->view_params.prjn_trans);
  }
} 

void PrjnView::Render_pre() {
  NetView* nv = this->nv();
  m_node_so = new T3PrjnNode(this, nv->view_params.prjn_width);
  DoHighlightColor(false);
  inherited::Render_pre();
}

void PrjnView::Render_impl() {
  T3PrjnNode* node_so = this->node_so(); // cache
  NetView* nv = this->nv();

  // find the total receive num, and our ordinal
  Projection* prjn = this->prjn(); // cache
  Layer* lay_fr = prjn->from;
  Layer* lay_to = prjn->layer;

  // origin is *back* center of origin layer, in Inventor coords (add .5f to z..)
  FloatTDCoord src( ((float)lay_fr->pos.x + .5f * (float)lay_fr->act_geom.x) / nv->max_size.x,
		    ((float)lay_fr->pos.z+.5f) / nv->max_size.z,
		    -((float)(lay_fr->pos.y + lay_fr->act_geom.y) / nv->max_size.y)
		    );
  transform(true)->translate.SetXYZ(src.x, src.y, src.z);

  // dest is the equally spaced target front on dest
//   float rcv_num = (float)lay_to->projections.FindLeafEl(prjn) + 1.0f; // indent
//   float tot_num = (float)lay_to->projections.leaves + 1.0f;
//   float	x_spc = (float)lay_to->act_geom.x / tot_num;

//   FloatTDCoord dst( ((float)lay_to->pos.x + rcv_num * x_spc) / nv->max_size.x,
// 		    ((float)lay_to->pos.z+.5f) / nv->max_size.z,
// 		    -((float)lay_to->pos.y / nv->max_size.y)
// 		    );

  // dest is *front* *center* -- keeps it clean
  FloatTDCoord dst( ((float)lay_to->pos.x + .5f * (float)lay_to->act_geom.x) / nv->max_size.x,
		    ((float)lay_to->pos.z+.5f) / nv->max_size.z,
		    -((float)lay_to->pos.y / nv->max_size.y)
		    );

  node_so->setEndPoint(SbVec3f(dst.x - src.x, dst.y - src.y, dst.z - src.z));

  // caption location is half way
  FloatTDCoord cap((dst.x - src.x) / 2.0f - .05f, (dst.y - src.y) / 2.0f, (dst.z - src.z) / 2.0f);
  node_so->setCaption(prjn->name.chars());
  node_so->transformCaption(cap);
  node_so->resizeCaption(nv->font_sizes.prjn);

  inherited::Render_impl();
}

void PrjnView::Reset_impl() {
  inherited::Reset_impl();
}

//////////////////////////
//   NetView		//
//////////////////////////

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
  nv->font_sizes = net->font_sizes;
  nv->view_params = net->view_params;
  fr->AddView(nv);
  return nv;
}


void NetView::Initialize() {
  data_base = &TA_Network;
  unit_disp_md = NULL;
  unit_sr = NULL;
  unit_md_flags = MD_UNKNOWN;
  nvp = NULL;
  display = true;
  unit_disp_mode = UDM_BLOCK;
  unit_text_disp = UTD_NONE;
  unit_src = NULL;
}

void NetView::Destroy() {
//nn  Reset();
  CutLinks();
}

void NetView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(scale, this);
  taBase::Own(scale_ranges, this);
  taBase::Own(ordered_uvg_list, this);
  taBase::Own(max_size, this);
  taBase::Own(font_sizes, this);
  taBase::Own(view_params, this);
}

void NetView::CutLinks() {
  // the nvp refs some of our stuff, so we have to nuke it
  if (nvp) {
    delete nvp; // should delete our ref
    nvp = NULL;
  }
  ordered_uvg_list.CutLinks();
  scale_ranges.CutLinks();
  scale.CutLinks();
  inherited::CutLinks();
}

void NetView::ChildUpdateAfterEdit(TAPtr child, bool& handled) {
  if(taMisc::is_loading || !taMisc::gui_active) return;
  TypeDef* typ = child->GetTypeDef();
  if (typ->InheritsFrom(&TA_ScaleRange)) {
  /*TODO  NetView* nv = GET_MY_OWNER(NetView);
    if((nv == NULL)||(nv->editor == NULL)||(nv->editor->netg == NULL)) return;
    if(nv->ordered_uvg_list.size == 1) {
      MemberDef* md =
        (MemberDef *) nv->editor->netg->membs[nv->ordered_uvg_list[0]];
      if((md != NULL) && (md->name == name)){
        nv->auto_scale = nv->editor->auto_scale = auto_scale;
        nv->editor->as_ckbox->state()->set(ivTelltaleState::is_chosen,auto_scale);
        nv->scale_min = minmax.min;
        nv->scale_max = minmax.max;
        nv->editor->cbar->SetMinMax(minmax.min,minmax.max);
        nv->editor->UpdateDisplay();
      }
    }
        */
    handled = true;
  }
  if (!handled)
    inherited::ChildUpdateAfterEdit(child, handled);
}


void NetView::BuildAll() { // populates everything
  Reset();
  if(!net()) return;
  GetMaxSize();
  Layer* lay;
  taLeafItr i;
  FOR_ITR_EL(Layer, lay, net()->layers., i) {
    LayerView* lv = new LayerView();
    lv->SetData(lay);//obs lay->AddDataView(lv);
    //nn layers.Add(lv); // no side-effects -- better to add to this first
    children.Add(lv);
    lv->BuildAll();
  }
  FOR_ITR_EL(Layer, lay, net()->layers., i) {
    Projection* prjn;
    taLeafItr j;
    FOR_ITR_EL(Projection, prjn, lay->projections., j) {
      PrjnView* pv = new PrjnView();
      pv->SetData(prjn);//obs prjn->AddDataView(pv);
      //nn prjns.Add(pv); // no side-effects -- better to add to this first
      children.Add(pv);
    }
  }
}

void NetView::ChildAdding(taDataView* child_) {
  inherited::ChildAdding(child_);
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if (!child) return;
  TypeDef* typ = child->GetTypeDef();
  if (typ->InheritsFrom(&TA_LayerView)) {
    layers.AddUnique(child);
  } else if (typ->InheritsFrom(&TA_PrjnView)) {
    prjns.AddUnique(child);
  }
}

void NetView::ChildRemoving(taDataView* child_) {
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if (!child) goto done;
  if (layers.RemoveEl(child)) goto done;
  if (prjns.RemoveEl(child)) goto done;
done:
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

/*obsScaleRange* NetView::GetCurScaleRange() {
  MemberDef* md = NULL;
  if (ordered_uvg_list.size >= 1) {
    int i = ordered_uvg_list[0];
    md = membs.SafeEl(i);
  }
  String nm;
  if (md) {
    nm = md->name;
  } else {
    nm = "."; // always making one simplifies everything
  }
  ScaleRange* sr = scale_ranges.FindName(nm);
  if (sr == NULL) {
    sr = (scale_ranges.New(1,&TA_NetViewScaleRange));
    sr->name = nm;
    sr->SetFromScale(scale);
  }

  return sr;
} */

void NetView::GetMembs() { // this fills a member group with the valid
			     // memberdefs from the units and connections
  setUnitDispMd(NULL);
  membs.Reset();
  if(!net()) return;
  // first do the connections
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, net()->layers., l_itr) {
    Projection* prjn;
    taLeafItr p_itr;
    FOR_ITR_EL(Projection, prjn, lay->projections., p_itr) {
      TypeDef* td = prjn->con_type;
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

  // then do the unit variables
  FOR_ITR_EL(Layer, lay, net()->layers., l_itr) {
    TypeDef* prv_td = NULL;
    Unit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(Unit, u, lay->units., u_itr) {
      TypeDef* td = u->GetTypeDef();
      if(td == prv_td) continue; // don't re-scan!
      prv_td = td;

      if(u->bias.cons.size) {
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
	    TAPtr* par_ptr = (TAPtr*)md->GetOff((void*)u);
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
    }
  }

  int_Array& oul = ordered_uvg_list;
  int i;
  for(i=oul.size-1; i>=0; i--) { // make sure no oul index is beyond number of membs
    if(oul[i] >= membs.size)
      oul.RemoveIdx(i,1);
  }
}

void NetView::GetUnitColor(float val,  iColor& col, float& sc_val) {
  const iColor* fl;  const iColor* tx;
  scale.GetColor(val,&fl,&tx,sc_val);
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
//   if (!display) return;
  GetMembs();
  GetMaxSize();
  // select "act" by default, if nothing selected, or saved selection not restored yet (first after load)
  if (!unit_disp_md) {
    if (ordered_uvg_list.size > 0) {
      int idx = ordered_uvg_list[0];
      setUnitDispMd(membs.SafeEl(idx));
    }
  }
  if (!unit_disp_md) {
    SelectVar("act", false, false);
  }
  if (init_panel) {
    InitPanel();
    UpdatePanel();
  }
  // descend into sub items
  LayerView* lv;
  taListItr i;
  FOR_ITR_EL(LayerView, lv, children., i) {
    InitDisplay_Layer(lv, false);
  }
}

void NetView::InitDisplay_Layer(LayerView* lv, bool check_build) {
//   if (check_build && (!display || !net()->CheckBuild(true))) return; // needs to be built
  UnitGroupView* ugrv;
  taListItr j;
  FOR_ITR_EL(UnitGroupView, ugrv, lv->children., j) {
    InitDisplay_UnitGroup(ugrv, false);
  }
}

void NetView::InitDisplay_UnitGroup(UnitGroupView* ugrv, bool check_build) {
//   if (check_build && (!display || !net()->CheckBuild(true))) return; // needs to be built
  ugrv->AllocUnitViewData(); // make sure we have correct space in uvd array
  ugrv->UpdateUnitViewBase(unit_disp_md, unit_src);
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
  //TODO:
}

void NetView::GetMaxSize() {
  max_size = net()->max_size;
  if(view_params.xy_square) {
    max_size.x = MAX(max_size.x, max_size.y);
    max_size.y = max_size.x;
  }
}

void NetView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!nvp) {
    nvp = new NetViewPanel(this);
    vw->viewerWindow()->AddPanelNewTab(nvp);
    vw->t3vs->Connect_SelectableHostNotifySignal(nvp,
      SLOT(viewWin_NotifySignal(ISelectableHost*, int)) );
  }
}

void NetView::Render_pre() {
  InitDisplay();
  m_node_so = new T3NetNode(this);
  SoMaterial* mat = node_so()->material(); //cache
  mat->diffuseColor.setValue(0.0f, 0.5f, 0.5f); // blue/green
//  mat->transparency.setValue(0.5f);

  inherited::Render_pre();
}

void NetView::Render_impl() {
  // font properties percolate down to all other elements, unless set there
  GetMaxSize();
  T3NetNode* node_so = this->node_so(); //cache
  node_so->resizeCaption(font_sizes.net_name);
  node_so->setCaption(data()->GetName().chars());

  Render_net_text();

  inherited::Render_impl();
  taiMisc::RunPending();
}

void NetView::Render_net_text() {
  T3NetNode* node_so = this->node_so(); //cache

  bool build_text = false;
  SoSeparator* net_txt = node_so->netText();
  if(!net_txt) {
    net_txt = node_so->getNetText();
    build_text = true;
    node_so->addChild(net_txt);
    SoBaseColor* bc = new SoBaseColor;
    bc->rgb.setValue(0, 0, 0); //black is default for text
    net_txt->addChild(bc);
    SoFont* fnt = new SoFont();
    fnt->size.setValue(font_sizes.net_vals);
    fnt->name = "Arial";
    net_txt->addChild(fnt);
  }
  TypeDef* td = net()->GetTypeDef();
  int chld_idx = 0;
  int disp_idx = 0;
  int per_row = 2;
  int n_rows = 8;
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    if(build_text) {
      SoSeparator* tsep = new SoSeparator;
      net_txt->addChild(tsep);
      SoTranslation* tr = new SoTranslation;
      tsep->addChild(tr);
      int col_idx = disp_idx % per_row;
      int disp_inc = 1;
      if(md->type->InheritsFrom(&TA_taString)) {
	if(col_idx != 0) disp_idx += per_row - col_idx;
	col_idx = 0;
	disp_inc = per_row;
      }
      float xv = 0.05f + (float)col_idx / (float)(per_row);
      float yv = (float)((disp_idx / per_row)+1) / (float)(n_rows + 2.0f);
      tr->translation.setValue(xv, -.5f, -yv);
      SoAsciiText* txt = new SoAsciiText();
      txt->justification = SoAsciiText::LEFT;
      tsep->addChild(txt);
      disp_idx += disp_inc;
    }
    SoSeparator* tsep = (SoSeparator*)net_txt->getChild(chld_idx + 2);
    // 2 = base color + font, 1 per guy
    SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
    String el = md->name + ": " + md->type->GetValStr(md->GetOff((void*)net()));
    txt->string.setValue(el.chars());
    chld_idx++;
  }

//   if (!display || !net()->CheckBuild(true)) return; // no display, or needs to be built
  if (scale.auto_scale) {
    UpdateAutoScale();
    if (nvp) {
      nvp->ColorScaleFromData();
      taiMisc::RunPending(); // so that panel is correct if 3d rendering takes a long time
    }
  }
}

void NetView::Reset_impl() {
  unit_src = NULL;
  prjns.Reset();
  layers.Reset();
  inherited::Reset_impl();
}

void NetView::SelectVar(const char* var_name, bool add, bool update) {
  int idx;
  if (!add)
    ordered_uvg_list.Reset();
  MemberDef* md = (MemberDef*)membs.FindName(var_name, idx);
  if (md) {
    if (ordered_uvg_list.FindEl(idx) < 0)
      ordered_uvg_list.Add(idx);
  }
  setUnitDispMd(md);
  if (update) UpdateDisplay();
}

void NetView::SetScaleData(bool auto_scale_, float min_, float max_, bool update_panel) {
  if ((scale.auto_scale == auto_scale_) && (scale.min == min_) && (scale.max == max_)) return;
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

void NetView::setUnitSrc(UnitView* uv, Unit* unit) {
  if (unit_src == unit) return; // no change
  // if there was existing unit, unpick it
  if (unit_src) {
    UnitView* uv_src = FindUnitView(unit_src);
    if (uv_src) {
      uv_src->picked = false;
    }
  }
  unit_src = unit;
  if (unit_src) {
    if(uv)
      uv->picked = true;
  }
}

void NetView::setUnitDisp(int value) {
  MemberDef* md = membs.SafeEl(value);
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

void NetView::UpdateAutoScale() {
  LayerView* lv;
  taListItr i;
  scale.SetMinMax(99999.0f, -99999.0f);
  bool updated = false;
  FOR_ITR_EL(LayerView, lv, children., i) {
    UnitGroupView* ugrv;
    taListItr j;
    FOR_ITR_EL(UnitGroupView, ugrv, lv->children., j) {
      TwoDCoord co;
      Unit_Group* ugrp = ugrv->ugrp(); //cache
      for (co.y = 0; co.y < ugrp->geom.y; ++co.y) {
        for (co.x = 0; co.x < ugrp->geom.x; ++co.x) {
          float val = ugrv->GetUnitDisplayVal(co, unit_md_flags); // get val for unit at co
          updated = scale.UpdateMinMax(val) || updated; //note: func call must come first!!
        }
      }
    }
  }
  if (updated) { //note: could really only not be updated if there were no items
    if (unit_sr)
      unit_sr->SetFromScale(scale); // update values
    if (nvp) {
      nvp->ColorScaleFromData();
    }
  } else { // set scale back to default so it doesn't look bizarre
    scale.SetMinMax(-1.0f, 1.0f);
  }
}

void NetView::UpdateDisplay(bool update_panel) { // updates dynamic values, esp. Unit values
  if (!display) return;
  if (update_panel) UpdatePanel();
  Render_impl();
}

void NetView::UpdatePanel() {
  if (!nvp) return;
  nvp->GetImage();
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
  updating = 0;
  QWidget* widg = new QWidget();
  //note: we don't set the values of all controls here, because dv does an immediate refresh
  layOuter = new QVBoxLayout(widg);
  layOuter->setSpacing(taiM->vspc_c);

  layDispCheck = new QHBoxLayout(layOuter);
  chkDisplay = new QCheckBox("Display", widg);
  layDispCheck->addWidget(chkDisplay);
  layDispCheck->addSpacing(taiM->hspc_c);

  lblUnitText = taiM->NewLabel("Unit: Text", widg, font_spec);
  layDispCheck->addWidget(lblUnitText);
  cmbUnitText = new taiComboBox(true, TA_NetView.sub_types.FindName("UnitTextDisplay"),
    NULL, NULL, widg);
  layDispCheck->addWidget(cmbUnitText->GetRep());
  layDispCheck->addSpacing(taiM->hspc_c);

  lblDispMode = taiM->NewLabel("Style", widg, font_spec);
  layDispCheck->addWidget(lblDispMode);
  cmbDispMode = new taiComboBox(true, TA_NetView.sub_types.FindName("UnitDisplayMode"),
    NULL, NULL, widg);
  layDispCheck->addWidget(cmbDispMode->GetRep());
  layDispCheck->addStretch();
  
  lblUnitTrans = taiM->NewLabel("Trans", widg, font_spec);
  layDispCheck->addWidget(lblUnitTrans);
  fldUnitTrans = new taiField(&TA_float, NULL, NULL, widg);
  layDispCheck->addWidget(fldUnitTrans->GetRep());
  layDispCheck->addSpacing(taiM->hspc_c);
  connect(fldUnitTrans->rep(), SIGNAL(editingFinished()), this, SLOT(fldUnitTrans_textChanged()) );

  lblUnitFont = taiM->NewLabel("Font Sz", widg, font_spec);
  layDispCheck->addWidget(lblUnitFont);
  fldUnitFont = new taiField(&TA_float, NULL, NULL, widg);
  layDispCheck->addWidget(fldUnitFont->GetRep());
  //  layDispCheck->addSpacing(taiM->hspc_c);
  connect(fldUnitFont->rep(), SIGNAL(editingFinished()), this, SLOT(fldUnitFont_textChanged()) );

  lblLayFont = taiM->NewLabel("Lay Font Sz", widg, font_spec);
  layDispCheck->addWidget(lblLayFont);
  fldLayFont = new taiField(&TA_float, NULL, NULL, widg);
  layDispCheck->addWidget(fldLayFont->GetRep());
  //  layDispCheck->addSpacing(taiM->hspc_c);
  connect(fldLayFont->rep(), SIGNAL(editingFinished()), this, SLOT(fldLayFont_textChanged()) );

  gbDisplayValues = new QGroupBox("Display Values", widg);
  layOuter->addWidget(gbDisplayValues, 1);
  layDisplayValues = new QVBoxLayout(gbDisplayValues);

  layColorScaleCtrls = new QHBoxLayout(layDisplayValues);
  
  chkAutoScale = new QCheckBox("auto scale", gbDisplayValues);
  layColorScaleCtrls->addWidget(chkAutoScale);

  butScaleDefault = new QPushButton("Set Defaults", gbDisplayValues);
  butScaleDefault->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layColorScaleCtrls->addWidget(butScaleDefault);
  connect(butScaleDefault, SIGNAL(pressed()), this, SLOT(butScaleDefault_pressed()) );
  layColorScaleCtrls->addStretch();
  
  cbar = new HCScaleBar(&(dv_->scale), ScaleBar::RANGE, true, true, gbDisplayValues);
//  cbar->setMaximumWidth(30);
  layDisplayValues->addWidget(cbar); // stretchfact=1 so it stretches to fill the space
  
  
  lvDisplayValues = new Q3ListView(gbDisplayValues);
  lvDisplayValues->addColumn("Value", 80);
  lvDisplayValues->addColumn("Description");
  lvDisplayValues->setShowSortIndicator(false);
  lvDisplayValues->setSorting(-1); // not sorted, shown in add order
  lvDisplayValues->setSelectionMode(Q3ListView::Extended);
  layDisplayValues->addWidget(lvDisplayValues, 1);

  // Spec tree
  gbSpecs = new QGroupBox("Specs", widg);
  layOuter->addWidget(gbSpecs, 1);
  laySpecs = new QVBoxLayout(gbSpecs);
  tvSpecs = new iTreeView(gbSpecs);
  laySpecs->addWidget(tvSpecs, 1);
  tvSpecs->setColumnCount(2);
  tvSpecs->setSortingEnabled(false);// only 1 order possible
  tvSpecs->setHeaderText(0, "Spec");
  tvSpecs->setHeaderText(1, "Description");
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
  connect(tvSpecs, SIGNAL(ItemSelected(iTreeViewItem*)),
    this, SLOT(tvSpecs_ItemSelected(iTreeViewItem*)) );
  

  // Command Buttons
  widCmdButtons = new iMethodButtonFrame(nv()->net(), widg);

  layOuter->addWidget(widCmdButtons);

  // add all base pdp commands -- you can add more in a derived constructor

  butBuildAll = new QPushButton("Build All", widCmdButtons);
  AddCmdButton(butBuildAll);
  connect(butBuildAll, SIGNAL(pressed()), this, SLOT(butBuildAll_pressed()) );

  butConnectAll = new QPushButton("ConnectAll", widCmdButtons);
  AddCmdButton(butConnectAll);
  connect(butConnectAll, SIGNAL(pressed()), this, SLOT(butConnectAll_pressed()) );

  butNewLayer = new QPushButton("&New Layer", widCmdButtons);
  AddCmdButton(butNewLayer);
  connect(butNewLayer, SIGNAL(pressed()), this, SLOT(butNewLayer_pressed()) );


//  layOuter->addStretch();

  setCentralWidget(widg);
  
  connect(cbar, SIGNAL(scaleValueChanged()),
    this, SLOT(cbar_scaleValueChanged()) );
  connect(chkDisplay, SIGNAL(toggled(bool)), this, SLOT(chkDisplay_toggled(bool)) );
  connect(lvDisplayValues, SIGNAL(selectionChanged()),
    this, SLOT(lvDisplayValues_selectionChanged()) );
  connect(cmbUnitText, SIGNAL(itemChanged(int)), this, SLOT(cmbUnitText_itemChanged(int)) );
  connect(cmbDispMode, SIGNAL(itemChanged(int)), this, SLOT(cmbDispMode_itemChanged(int)) );
  connect(chkAutoScale, SIGNAL(toggled(bool)), this, SLOT(chkAutoScale_toggled(bool)) );
}

NetViewPanel::~NetViewPanel() {
  NetView* nv_ = nv();
  if (nv_) {
    nv_->nvp = NULL;
  }
  if (cmbDispMode) {delete cmbDispMode; cmbDispMode = NULL;}
  if (cmbUnitText) {delete cmbUnitText; cmbUnitText = NULL;}
}


void NetViewPanel::AddCmdButton(QWidget* but) {
  widCmdButtons->lay->addWidget(but);
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

void NetViewPanel::chkAutoScale_toggled(bool on) {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;

  nv_->SetScaleData(on, cbar->min(), cbar->max(), false);
  nv_->UpdateDisplay(false);
}

void NetViewPanel::chkDisplay_toggled(bool on) {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;

  nv_->display = on;
  if (on) nv_->Render(); //note: may never have actually rendered yet!
}


void NetViewPanel::cmbDispMode_itemChanged(int itm) {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;

  if (nv_->unit_disp_mode == (NetView::UnitDisplayMode)itm) return;
  nv_->unit_disp_mode = (NetView::UnitDisplayMode)itm;
  nv_->BuildAll();		// very strong
//   nv_->InitDisplay(false); // need strong reset
  //  nv_->Render();
}

void NetViewPanel::cmbUnitText_itemChanged(int itm) {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;

  nv_->unit_text_disp = (NetView::UnitTextDisplay)itm;
  nv_->UpdateDisplay(false);
}

void NetViewPanel::fldUnitTrans_textChanged() {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;

  nv_->view_params.unit_trans = (float)fldUnitTrans->GetValue();
  nv_->UpdateDisplay(false);
}

void NetViewPanel::fldUnitFont_textChanged() {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;

  nv_->font_sizes.unit = (float)fldUnitFont->GetValue();
  nv_->UpdateDisplay(false);
}

void NetViewPanel::fldLayFont_textChanged() {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;

  nv_->font_sizes.layer = (float)fldLayFont->GetValue();
  nv_->UpdateDisplay(false);
}

void NetViewPanel::cbar_scaleValueChanged() {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;
  //note: user changed value, so must no longer be autoscale
  ++updating;
  chkAutoScale->setChecked(false); //note: raises signal on widget! (grr...)
  --updating;

  nv_->SetScaleData(false, cbar->min(), cbar->max(), false);
  nv_->UpdateDisplay(false);
}

void NetViewPanel::ColorScaleFromData() {
  NetView* nv_;
  if (!(nv_ = nv())) return;

  ++updating;
  cbar->UpdateScaleValues();
  chkAutoScale->setChecked(nv_->scale.auto_scale); //note: raises signal on widget! (grr...)
  --updating;
}

void NetViewPanel::GetImage_impl() {
  inherited::GetImage_impl();
  NetView* nv = this->nv(); // cache
  cmbUnitText->GetImage(nv->unit_text_disp);
  cmbDispMode->GetImage(nv->unit_disp_mode);
  chkDisplay->setChecked(nv->display);
  fldUnitTrans->GetImage((String)nv->view_params.unit_trans);
  fldUnitFont->GetImage((String)nv->font_sizes.unit);
  fldLayFont->GetImage((String)nv->font_sizes.layer);
  // update var selection
  int i = 0;
  Q3ListViewItemIterator it(lvDisplayValues);
  Q3ListViewItem* item;
  while ((item = it.current())) {
    bool is_selected = (nv->ordered_uvg_list.FindEl(i) >= 0);
    lvDisplayValues->setSelected(item, is_selected);
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
}


void NetViewPanel::GetVars() {
  NetView* nv_;
  if (!(nv_ = nv())) return;

  lvDisplayValues->clear();
  if (nv_->membs.size == 0) return;

  MemberDef* md;
  Q3ListViewItem* lvi = NULL;
  for (int i=0; i < nv_->membs.size; i++) {
    md = nv_->membs[i];
    lvi = new Q3ListViewItem(lvDisplayValues, lvi, md->name, md->desc);
  }
}

void NetViewPanel::ExpandAllSpecs() {
  QTreeWidgetItemIterator it(tvSpecs, QTreeWidgetItemIterator::HasChildren);
  QTreeWidgetItem* item;
  while ((item = *it)) { 
    tvSpecs->setItemExpanded(item, true);
    ++it;
  }
  // size first N-1 cols
  int cols = tvSpecs->columnCount(); // cache
  // make columns nice sizes (not last)
  for (int i = 0; i < (cols - 1); ++i) {
    tvSpecs->resizeColumnToContents(i);
  }
}

void NetViewPanel::InitPanel() {
  NetView* nv_;
  if (!(nv_ = nv())) return;
  ++updating;
  // fill monitor values
  GetVars();
  ExpandAllSpecs();
  --updating;
}

void NetViewPanel::lvDisplayValues_selectionChanged() {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;
  // iterate the list -- remove unselected items, and add selected items
  int i = 0;
  //redo the list each time, to guard against stale values
  nv_->ordered_uvg_list.Reset(); 
  Q3ListViewItemIterator it(lvDisplayValues);
  Q3ListViewItem* item;
  while ((item = it.current())) {
    if (lvDisplayValues->isSelected(item)) {
      if (nv_->ordered_uvg_list.FindEl(i) < 0) {
        nv_->ordered_uvg_list.Add(i);
      }
    } else {
      nv_->ordered_uvg_list.RemoveIdx(i); // note: may not exist
    }
    ++it;
    ++i;
  }
  nv_->setUnitDisp(nv_->ordered_uvg_list.SafeEl(0)); // -1 if nothing
  ColorScaleFromData();
  nv_->InitDisplay(false);
  nv_->UpdateDisplay(false);
}

bool UsesSpec(taBase* obj, BaseSpec* spec) {
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

void NetViewPanel::setHighlightSpec(BaseSpec* spec, bool force) {
  if ((spec == m_cur_spec) && !force) return;
  m_cur_spec = spec;
  // We use a completely generic mechanism, that looks for
  // the spec being assigned to a SpecPtr_impl member of the object
  // check layers
  NetView* nv = this->nv(); // cache
  for (int i = 0; i < nv->layers.size; ++i) {
    LayerView* lv = (LayerView*)nv->layers.FastEl(i);
    if (m_cur_spec) {
      Layer* lay = lv->layer();
      if (lay && UsesSpec(lay, m_cur_spec)) {
        lv->setHighlightColor(T3Color(m_cur_spec->GetEditColorInherit()));
        continue;
      }
    }
    lv->setDefaultColor();
  }
  // check projections
  for (int i = 0; i < nv->prjns.size; ++i) {
    PrjnView* pv = (PrjnView*)nv->prjns.FastEl(i);
    if (m_cur_spec) {
      Projection* prjn = pv->prjn();
      if (prjn && UsesSpec(prjn, m_cur_spec)) {
        pv->setHighlightColor(T3Color(m_cur_spec->GetEditColorInherit()));
        continue;
      }
    }
    pv->setDefaultColor();
  }
  
}

void NetViewPanel::tvSpecs_ItemSelected(iTreeViewItem* item) {
  BaseSpec* spec = NULL;
  if (item) {
    taBase* ld_ = (taBase*)item->linkData();
    if (ld_->InheritsFrom(TA_BaseSpec))
      spec = (BaseSpec*)ld_;
  }
  setHighlightSpec(spec);

}


void NetViewPanel::viewWin_NotifySignal(ISelectableHost* src, int op) {
  NetView* nv_;
  if (!(nv_ = nv())) return;
  nv_->viewWin_NotifySignal(src, op);
}

