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
//obs #include "stats.h"
//obs #include "sched_proc.h"
#include "ta_qtclipdata.h"
#include "ta_qt.h"
#include "ta_qtgroup.h"
#include "css_qt.h"		// for the cssiSession

#include "iflowlayout.h"
#include "icolor.h"

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
//temp
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

//#include <OS/file.h>

//#include <OS/math.h>
#include <limits.h>
#include <float.h>
//nn #include <unistd.h>

// netview stuff
const float NET_VIEW_INITIAL_X = 330.0f;
const float NET_VIEW_INITIAL_Y = 300.0f;
const float NET_VIEW_X_MARG = 8.0f;
const float NET_VIEW_Y_MARG = 16.0f;

// projection_g stuff
const float SELF_CON_DIST = .03f;

#define DIST(x,y) sqrt((double) ((x * x) + (y*y)))
/*
//////////////////////////////////
//	  netstru.h		//
//////////////////////////////////

T3LayerNode* LayerSpec::MakeSo(Layer* layer, T3NetNode* par) {
  T3LayerNode* rval = NULL; //new T3LayerNode(layer, par);
  return rval;
}

T3UnitNode* UnitSpec::MakeSo(Unit* unit, T3LayerNode* par) {
  T3UnitNode* rval = NULL;//new T3UnitNode(unit, par);
  return rval;
}
*/


//////////////////////////////////
//	  T3PrjnNode		//
//////////////////////////////////
/*
void T3PrjnNode::Render_pre() {
  rot_prjn = new SoTransform();
  addChild(rot_prjn);
  trln_prjn = new SoTransform();
  addChild(trln_prjn);

  //color and material
  SoBaseColor* col = new SoBaseColor;
  col->rgb = SbColor(.5f, .5f, .5f); // medium gray
  addChild(col);

  float rad = 0.01f;
  line_prjn = new SoCylinder();
  line_prjn->radius = rad; //height is variable, depends on length
  addChild(line_prjn);

  trln_arr = new SoTransform();
  addChild(trln_arr);

  arr_prjn = new SoCone();
  arr_prjn->height =  3.0f * rad;
  arr_prjn->bottomRadius = 2.0f * rad;
  addChild(arr_prjn);
}

void T3PrjnNode::Render_impl() {
  // find the total receive num, and our ordinal
  Projection* prjn = this->prjn(); // cache
  Layer* lay_fr = prjn->from;
  Layer* lay_to = prjn->layer;

  rcv_num = lay_to->projections.FindLeaf(prjn);
  tot_num = lay_to->projections.leaves;
  if (tot_num == 0) return; // error, shouldn't happen

  // origin is front center of origin layer
  FloatTDCoord src((lay_fr->pos.x + lay_fr->act_geom.x) / 2.0f, (float)lay_fr->pos.y, (float)lay_fr->pos.z);
  SetOrigin(src.x, src.y, src.z);

  // dest is the equally spaced target front on dest
  float arr_h = 0.0f;
  FloatTDCoord dst((lay_to->pos.x) + (rcv_num + 1.0f) * ((lay_to->act_geom.x) / (tot_num + 1.0f)),
      (float)lay_to->pos.y, (float)lay_to->pos.z);
  float dist = src.Dist(dst);
  line_prjn->height = dist - arr_h;
  // txfm
  rot_prjn->rotation.setValue(SbRotation(SbVec3f(0, 1.0f, 0),
    SbVec3f(dst.x - src.x, dst.z - src.z, -(dst.y - src.y)))); // from vec/ to vec
  trln_prjn->translation.setValue(0.0f, line_prjn->height.getValue() / 2.0f, 0.0f);
  trln_arr->translation.setValue(0.0f, line_prjn->height.getValue() / 2.0f, 0.0f); //note: already txlted by 1/2 height
}
*/
/*todo
void T3LayerNode::RenderSetup(SoGroup* par_so) {
  IGadget::RenderSetup(par_so);
}

void T3LayerNode::RenderContentPre(SoGroup* par_so) {
  SoMaterial* mat_frm = new SoMaterial;
  mat_frm->emissiveColor.setValue(0.0f, 0.0f, 0.5f); // blue glow!
  mat_frm->transparency.setValue(0.5f);
  par_so->addChild(mat_frm);

  SoCube* frm = new SoCube;
  //note: following not right for multi-group layers...
  frm->width = lay->geom.x;
  frm->depth = lay->geom.y;
  frm->height = 0.0f;
  par_so->addChild(frm);
}

void T3LayerNode::RenderChildren(SoGroup* par_so) {
  Layer* lay;
  taLeafItr i;
  FOR_ITR_EL(Layer, lay, net->layers., i) {
    if (lay->own_net == NULL) continue; // indication that layer is just about to be removed!
    T3LayerNode* lay_so = new T3LayerNode(lay, this);
    lay_so->setOrigin(lay->pos.x , lay->pos.z, -(lay->pos.y));

    render_child(lay_so, par_so);
  }
}
*/

/*

void T3UnitGroupNode::Render_impl() {
  float h = 0.06f; // nominal amount of height, so we don't vanish
  Unit_Group* ugrp = this->ugrp(); //cache
  // set origin
  SetOrigin(ugrp->pos);

  TDCoord geom = ugrp->geom;

  // set size/pos of cube
  txfm_ugrp->translation.setValue(geom.x/2.0f, h/2.0f, -geom.y/2.0f);
  shp_ugrp->width = geom.x - 0.1f; // .05 inset
  shp_ugrp->height = h;
  shp_ugrp->depth = geom.y - 0.1f;
}

*/

/*nn T3UnitNode_PtrList::~T3UnitNode_PtrList() {
  Reset();
}

void* T3UnitNode_PtrList::El_Ref_(void* it_)	{
  T3UnitNode* it = (T3UnitNode*)it_;
  it->ref();
  return it_;
}

void* T3UnitNode_PtrList::El_unRef_(void* it_) {
  T3UnitNode* it = (T3UnitNode*)it_;
  it->unref();
  return it_;
} */

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
  switch (nv->unit_disp_mode) {
  case NetView::UDM_CIRCLE: 	m_node_so = new T3UnitNode_Circle(this); break;
  case NetView::UDM_RECT: 	m_node_so = new T3UnitNode_Rect(this); break;
  case NetView::UDM_BLOCK: 	m_node_so = new T3UnitNode_Block(this); break;
  case NetView::UDM_CYLINDER: 	m_node_so = new T3UnitNode_Cylinder(this); break;
  }

  Unit* unit = this->unit(); //cache
  // note: pos s/b invariant
  node_so()->transform()->translation.setValue(
    (float)(unit->pos.x) + 0.5f, 0.0f, -((float)(unit->pos.y) + 0.5f));
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
  //TODO: summary mode
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
  float val = 0.0f;
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

void UnitGroupView::Render_pre() {
  Unit_Group* ugrp = this->ugrp(); //cache
  AllocUnitViewData();
  m_node_so = new T3UnitGroupNode(this);
  //NOTE: we create/adjust the units in the Render_impl routine
  T3UnitGroupNode* ugrp_so = node_so(); // cache

//  SoMaterial* mat = ugrp_so->material(); //cache
//  mat->diffuseColor.setValue(0.0f, 0.5f, 0.0f); // green
//  mat->transparency.setValue(0.5f);

  // TODO: if not built, then use geom from layer
  ugrp_so->setGeom(ugrp->geom.x, ugrp->geom.y);

  inherited::Render_pre();
}

void UnitGroupView::Render_impl() {
  Unit_Group* ugrp = this->ugrp(); //cache
  //set origin: 0,0,0
  TDCoord& pos = ugrp->pos;
  FloatTransform* ft = transform(true);
  ft->translate.SetXYZ(pos.x, pos.z, -pos.y);

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

void UnitGroupView::Render_impl_children() {
  NetView* nv = this->nv(); //cache
  T3UnitGroupNode* node_so = this->node_so(); // cache

  //TODO: summary view

  // for efficiency we assume responsibility for the _impl of UnitViews
  T3UnitNode* unit_so;
  TwoDCoord co;
  float val;
  T3Color col;
  String val_str;
  // if displaying unit text, set up the viewing props in the unitgroup
  if (nv->unit_text_disp != NetView::UTD_NONE) {
    SoFont* font = node_so->unitCaptionFont(true);
    font->size.setValue(0.2f); // is in same units as geometry units of network
  }

  for (int i = 0; i < children.size; ++i) {
    UnitView* uv = (UnitView*)children.FastEl(i);
    Unit* unit = uv->unit();
    unit_so = uv->node_so();
    if (!unit_so || !unit) continue; // shouldn't happen
    nv->GetUnitDisplayVals(this, unit->pos, val, col);
    // Value
    unit_so->setAppearance(val, col);
    //TODO: we maybe shouldn't alter picked here, because that is expensive -- do only when select changes
    // except that might be complicated, ex. when Render() called,
    unit_so->setPicked(uv->picked);

    // Text (if any)
    SoAsciiText* at = unit_so->captionNode(false);
    if (nv->unit_text_disp == NetView::UTD_NONE) {
      if (at)
        at->string.setValue( "");
    } else { // text of some kind
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

void UnitGroupView::Reset_impl() {
  inherited::Reset_impl();
  uvd_arr.SetSize(0);
}

/*
void UnitGroupView::CreateChildren(T3Node* par_node) {
  String node_nm;
  Unit* unit;
  Unit_Group* ugrp = data(); //cache
  TDCoord geom = ugrp->geom; // if zero, need to get our geom from the layer (not built yet)
  if ((geom.x == 0) && (geom.y == 0) && (geom.z == 1)) {
    Layer* lay = ugrp->own_lay;
    // check if single grp or multi
    if (lay->geom.z == 1) {
      geom = lay->geom;
    } else {
      //TODO:
    }
  }
  bool is_built = ugrp->CheckBuild(); // if built, lets us know if we've run out of units
  TwoDCoord coord;
  for (coord.y = 0; coord.y < geom.y; coord.y++) {
    for (coord.x = 0; coord.x < geom.x; coord.x++) {
      unit = ugrp->FindUnitFmCoord(coord); // can be null if not populated
      if (is_built && !unit) goto exit; // no more actual units, and shouldn't show placeholders

      if (unit) node_nm = unit->GetName();
      else node_nm = _nilString;
      int flags = 0;// BrListViewItem::DNF_UPDATE_NAME | BrListViewItem::DNF_CAN_BROWSE| BrListViewItem::DNF_CAN_DRAG;
      T3UnitNode* unit_nd = T3UnitNode::New(unit, coord, par_node, last_child_node, node_nm, flags);
      last_child_node = unit_nd;
    }
  }

exit:
  return;
}
*/


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

void LayerView::Render_pre() {
  m_node_so = new T3LayerNode(this);

//  SoMaterial* mat = node_so()->material(); //cache
//  mat->diffuseColor.setValue(0.4f, 0.4f, 0.4f); // gray

  inherited::Render_pre();
}

void LayerView::Render_impl() {
  Layer* lay = this->layer(); //cache
  // set origin: 0,.5,0 in allocated area
  TDCoord& pos = lay->pos;
  FloatTransform* ft = transform(true);
  ft->translate.SetXYZ(pos.x, (pos.z + 0.5f), -pos.y);

  T3LayerNode* node_so = this->node_so(); // cache
  node_so->setGeom(lay->act_geom.x, lay->act_geom.y);
  node_so->setCaption(data()->GetName().chars());
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


void PrjnView::Render_pre() {
  m_node_so = new T3PrjnNode(this);
  inherited::Render_pre();
}

void PrjnView::Render_impl() {
  T3PrjnNode* node_so = this->node_so(); // cache

  // find the total receive num, and our ordinal
  Projection* prjn = this->prjn(); // cache
  Layer* lay_fr = prjn->from;
  Layer* lay_to = prjn->layer;

  int rcv_num = lay_to->projections.FindLeafEl(prjn);
  int tot_num = lay_to->projections.leaves;
  if (tot_num == 0) return; // error, shouldn't happen

  // origin is front center of origin layer, in Inventor coords
  FloatTDCoord src(
    (lay_fr->pos.x + lay_fr->act_geom.x) / 2.0f,
    (lay_fr->pos.z + 0.5f),
    -((float)lay_fr->pos.y)
  );
  transform(true)->translate.SetXYZ(src.x, src.y, src.z);

  // dest is the equally spaced target front on dest
//  float arr_h = 0.0f;
  FloatTDCoord dst(
    (float)(lay_to->pos.x) + (rcv_num + 1.0f) * ((lay_to->act_geom.x) / (tot_num + 1.0f)),
    (lay_to->pos.z + 0.5f),
    -((float)lay_to->pos.y)
  );

  node_so->setEndPoint(SbVec3f(dst.x - src.x, dst.y - src.y, dst.z - src.z));

  // caption location is half way
  FloatTDCoord cap((dst.x - src.x) / 2.0f, (dst.y - src.y) / 2.0f, (dst.z - src.z) / 2.0f);

  SoFont* font = node_so->captionFont(true);
  font->size.setValue(0.1f); // is in same units as geometry units of network, smaller than most others
  SbVec3f translate(cap.x + 0.05f, cap.y, cap.z);
  node_so->transformCaption(translate);
  node_so->setCaption(data()->GetName().chars());

/* was
  float dist = src.Dist(dst);
  line_prjn->height = dist - arr_h;
  // txfm
  rot_prjn->rotation.setValue(SbRotation(SbVec3f(0, 1.0f, 0),
    SbVec3f(dst.x - src.x, dst.z - src.z, -(dst.y - src.y)))); // from vec/ to vec
  trln_prjn->translation.setValue(0.0f, line_prjn->height.getValue() / 2.0f, 0.0f);
  trln_arr->translation.setValue(0.0f, line_prjn->height.getValue() / 2.0f, 0.0f); //note: already txlted by 1/2 height
*/
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

void NetView::GetUnitColor(float val,  iColor& col) {
  const iColor* fl;  const iColor* tx;
/*TODO  if ((act_md == NULL) || (base == NULL) || (md_type < 0)) {
    fl = cbar->bar->scale->nocolor.color;
    tx = cbar->bar->scale->nocolor.contrastcolor;
    if(fl != fill()) {
      fill(fl);
      damage_me(c);
      return true;
    }
    return false;
  }  */

  scale.GetColor(val,&fl,&tx);
  col = fl;
}

void NetView::GetUnitDisplayVals(UnitGroupView* ugrv, TwoDCoord& co, float& val,  T3Color& col) {
  iColor tc;
  if ((unit_disp_md == NULL) || (unit_md_flags == MD_UNKNOWN)) {
    val = 0.0f;
    col.setValue(.25f, .25f, .25f); // dk gray
    return;
  }

  val = ugrv->GetUnitDisplayVal(co, unit_md_flags);
  GetUnitColor(val, tc);
  col.setValue(tc.redf(), tc.greenf(), tc.bluef());
}

void NetView::InitDisplay(bool init_panel) {
  if (!display) return;
  GetMembs();
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
  if (check_build && (!display || !net()->CheckBuild(true))) return; // needs to be built
  UnitGroupView* ugrv;
  taListItr j;
  FOR_ITR_EL(UnitGroupView, ugrv, lv->children., j) {
    InitDisplay_UnitGroup(ugrv, false);
  }
}

void NetView::InitDisplay_UnitGroup(UnitGroupView* ugrv, bool check_build) {
  if (check_build && (!display || !net()->CheckBuild(true))) return; // needs to be built
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
  T3NetNode* node_so = this->node_so(); //cache
  SoFont* font = node_so->captionFont(true);
  font->size.setValue(0.4f); // is in same units as geometry units of network
  node_so->setCaption(data()->GetName().chars());

  if (!display || !net()->CheckBuild(true)) return; // no display, or needs to be built
  if (scale.auto_scale) {
    UpdateAutoScale();
    if (nvp) {
      nvp->ColorScaleFromData();
      taiMisc::RunPending(); // so that panel is correct if 3d rendering takes a long time
    }
  }
  inherited::Render_impl();
  taiMisc::RunPending();
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
  if (!display || !net()->CheckBuild(true)) return; // no display, or needs to be built
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
  updating = 0;
  QWidget* widg = new QWidget();
  //note: we don't set the values of all controls here, because dv does an immediate refresh
  layOuter = new QVBoxLayout(widg);

  layDispCheck = new QHBoxLayout(layOuter);
  chkDisplay = new QCheckBox("Display", widg);
  layDispCheck->addWidget(chkDisplay);
  layDispCheck->addSpacing(taiM->hspc_c);

  lblUnitText = taiM->NewLabel("Unit Text", widg, font_spec);
  layDispCheck->addWidget(lblUnitText);
  cmbUnitText = new taiComboBox(true, TA_NetView.sub_types.FindName("UnitTextDisplay"),
    NULL, NULL, widg);
  layDispCheck->addWidget(cmbUnitText->GetRep());
  layDispCheck->addSpacing(taiM->hspc_c);

  lblDispMode = taiM->NewLabel("Unit Style", widg, font_spec);
  layDispCheck->addWidget(lblDispMode);
  cmbDispMode = new taiComboBox(true, TA_NetView.sub_types.FindName("UnitDisplayMode"),
    NULL, NULL, widg);
  layDispCheck->addWidget(cmbDispMode->GetRep());
  layDispCheck->addStretch();
  
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

  layOuter->addSpacing(taiM->vspc_c);
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
  nv_->Render();
}

void NetViewPanel::cmbUnitText_itemChanged(int itm) {
  if (updating) return;
  NetView* nv_;
  if (!(nv_ = nv())) return;

  nv_->unit_text_disp = (NetView::UnitTextDisplay)itm;
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

void NetViewPanel::viewWin_NotifySignal(ISelectableHost* src, int op) {
  NetView* nv_;
  if (!(nv_ = nv())) return;
  nv_->viewWin_NotifySignal(src, op);
}


/*obs
//////////////////////////////////
// 	tabNetworkLink 		//
//////////////////////////////////

tabNetworkDataView::tabNetworkDataView(Network* data_, taiDataBrowser* browser_)
:tabDataView(data_, browser_)
{
}

taiDataPanel* tabNetworkDataView::CreateDataPanel(taiTreeDataNode* sel_node) {
  taiDataPanel* rval = new taiNetworkDataPanel(this, sel_node);
  return rval;
}

//////////////////////////////////
// 	taiNetworkDataPanel 	//
//////////////////////////////////

taiNetworkDataPanel::taiNetworkDataPanel(tabNetworkDataView* link_, taiTreeDataNode* sel_node_, QWidget* parent, const char* name)
:taiDataPanel(link_, sel_node_, parent, name)
{
  net_so = new T3NetNode(net());
  mparentItem = sel_node;
  root = NULL;
  layOuter = new QVBoxLayout(this);

  //create the so viewer
  t3vsw = new T3ViewspaceWidget(this);
  SoQtRenderArea* ra_ = new SoQtExaminerViewer(t3vsw); //TEMP
  t3vsw->setRenderArea(ra_);

  layOuter->addWidget(t3vsw);
  Render();
}

taiNetworkDataPanel::~taiNetworkDataPanel() {
  delete net_so;
  if (root) {
    root->unref();
    root = NULL;
  }
}

SoQtRenderArea* taiNetworkDataPanel::ra() {
  if (t3vsw) return t3vsw->renderArea();
  else      return NULL;
}

void taiNetworkDataPanel::Render() { // ENTIRE ROUTINE IS TEMP

    root = new SoSeparator;
    root->ref();

  net_so->Render(root);

    // Use one of the convenient SoQt viewer classes.
    ra()->setSceneGraph(root);
    ra()->show();
}

*/







/*

//////////////////////////
//	PrjnView	//
//////////////////////////

void PrjnView::Initialize() {
  data_base = &TA_Projection;
}

void PrjnView::Destroy() {
  Clear();
}

void PrjnView::Clear_impl() {
  prjn_so = (T3PrjnNode*)NULL;
}

void PrjnView::RenderContentPre_impl(SoGroup*) {
  prjn_so->RenderContentPre();
}

void PrjnView::RenderSetup_impl(SoGroup* par_so) {
  prjn_so = new T3PrjnNode(this, prjn());
  par_so->addChild(prjn_so);
}

//void NetView::UpdateAfterEdit() {
//}


//////////////////////////
//	LayerView	//
//////////////////////////

void LayerView::Initialize() {
  data_base = &TA_Layer;
  prjns.SetBaseType(&TA_PrjnView);
}

void LayerView::Destroy() {
  Clear();
  CutLinks();
}

void LayerView::InitLinks() {
  T3Node::InitLinks(); // net is now our view
  taBase::Own(prjns, this);
  if (lay()) {
    prjns.SetData(lay()); // sends notifications
  }
}

void LayerView::CutLinks() {
  prjns.CutLinks();
  T3Node::CutLinks();
}

void LayerView::Copy_(const LayerView& cp) {
  Reset();
  prjns = cp.prjns;
}

void LayerView::BuildAll() { // populates everything
  Reset();
  // sending projections
  taLeafItr j;
  Projection* prjn;
  FOR_ITR_EL(Projection, prjn, lay()->projections., j) { // receiving projections
    PrjnView* pv = new PrjnView();
    prjn->AddDataView(pv);
    prjns.Add(pv);
  }
}

void LayerView::Clear_impl() {
  prjns.Clear();
  lay_so = (T3LayerNode*)NULL;
}

void LayerView::DataDestroying() {
  Reset();
}

void LayerView::RenderChildren_impl(SoGroup* ) {
  //TODO: render the groups; use lay_so as the parent
  if (lay()->geom.z == 1) { // simple: render only units
      SoSeparator* u_so = new SoSeparator;
      lay_so->addChild(u_so);
      RenderUnits(u_so, lay()->geom, &lay()->units);
  } else { // render using groups
  }
  // render the projections last
//TODO  prjns.Render(lay_so); //TODO: should we use our par, or the net_so???
}

void LayerView::RenderContentPre_impl(SoGroup*) {
  lay_so->RenderContentPre();
}

void LayerView::RenderSetup(SoGroup* par_so) {
  setOrigin(lay()->pos.x, lay()->pos.z, -(lay()->pos.y));
  T3Node::RenderSetup(par_so);
  lay_so = new T3LayerNode(this, lay());
  par_so->addChild(lay_so);
}

void LayerView::RenderUnits(SoGroup* par_so, const TDCoord& geom, Unit_Group* ugrp) {
  if ((geom.x == 0) || (geom.y == 0)) return;
  //note: ugrp can be null if rendering place holders
  TwoDCoord coord;
  bool is_built = false; // if built, lets us know if we've run out of units
  if (ugrp) is_built = ugrp->CheckBuild();

  T3RenderHelper rh(par_so);
  // T3UnitNode's are groups, not separators, so we have to do everything relative
  // default color, if no other supplied
  SoBaseColor * col = new SoBaseColor;
  col->rgb = SbColor(.25f, .25f, .25f); // dark gray
  par_so->addChild(col);
  col = NULL;

  // initial unit offset from centroid of group -- render relative to center of cells
  rh.moveTo(-(geom.x - 1)/2.0f, 0, (geom.y - 1)/2.0f);

  // transform to get us to next row -- shared
  SoTransform* xf_nexty = new SoTransform;
  xf_nexty->ref(); // in case not used
  xf_nexty->translation.setValue(-(geom.x - 1), 0.0f, -1.0f); //move to next row, and back to start

  // transform to get us to next unit in row -- shared
  SoTransform* xf_nextx = new SoTransform;
  xf_nextx->ref(); // in case not used
  xf_nextx->translation.setValue(1.0f, 0, 0); //move to next cell

  for (coord.y = 0; coord.y < geom.y; coord.y++) {
    if (coord.y > 0) par_so->addChild(xf_nexty);

    for (coord.x = 0; coord.x < geom.x; coord.x++) {
      if (coord.x > 0) par_so->addChild(xf_nextx);

      Unit* unit = NULL;
      if (ugrp) unit = ugrp->FindUnitFmCoord(coord); // can be null if not populated
      if (is_built && !unit) goto exit; // no more actual units, and shouldn't show placeholders

      T3UnitNode* unit_so;
      TwoDCoord pos;
      if (unit) {
        //TEMP: replace this scheme with a color caching scheme, that only outputs
        // a new color if the unit is differently colored than previous unit
        if (!col) {
          col = new SoBaseColor;
          col->rgb = SbColor(1, 1, 0); // yellow
          col->ref();
          par_so->addChild(col);
        }
        pos = unit->pos;
        unit_so = new T3UnitNode(this, unit, T3UnitNode::DEFAULT);
      } else {
        pos = coord;
        unit_so = new T3UnitNode(this, NULL, T3UnitNode::EMPTY);
      }
      par_so->addChild(unit_so);
    }
  }

exit:
  if (col) col->unref();
  xf_nextx->unref();
  xf_nexty->unref();

}

void LayerView::Reset_impl() {
  prjns.Reset();
}

//void NetView::UpdateAfterEdit() {
//}


//////////////////////////
//	NetView		//
//////////////////////////

void NetView::Initialize() {
  data_base = &TA_Network;
  layers.SetBaseType(&TA_LayerView);
}

void NetView::Destroy() {
  CutLinks();
}

void NetView::InitLinks() {
  T3Node::InitLinks(); // net is now our view
  taBase::Own(layers, this);
  if (net()) {
    layers.SetData(net()); // sends notifications
  }
}

void NetView::CutLinks() {
  layers.CutLinks();
  T3Node::CutLinks();
}

void NetView::Copy_(const NetView& cp) {
  Reset();
  layers = cp.layers;
}

void NetView::Clear_impl() {
  layers.Clear();
  net_so = (T3NetNode*)NULL;
}

void NetView::ChildList_DataDataChanged(T3DataView_List* list, int dcr, void* op1_, void* op2_) {
  if (list == &layers) {
    switch (dcr) {
    case DCR_GROUP_ITEM_INSERT: { // layer inserted somewhere in the hierarchy
      Layer* lay = (Layer*) op1_;
      LayerView* lv = new LayerView();
      lay->AddDataView(lv);
      layers.Add(lv);
      if (net_so.ptr()) lv->Render(net_so);
    } break;
    }
  }
}


void NetView::RenderSetup_impl(SoGroup* par_so) {
  net_so = new T3NetNode(this, net());
  par_so->addChild(net_so);
}

void NetView::RenderContentPre_impl(SoGroup* par_so) {
}

void NetView::RenderChildren_impl(SoGroup*) {
  layers.Render(net_so); //TODO: should we use our par, or the net_so???
}

void NetView::Reset_impl() {
  layers.Reset();
}

//void NetView::UpdateAfterEdit() {
//}
*/
