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

#include "BrainView.h"
//#include "NewNetViewHelper.h"
//#include "netstru.h" // Network
#include "T3BrainNode.h"

#include "BrainViewPanel.h"
#include "BrainVolumeView.h"

#include <Inventor/SoEventManager.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

Network*
BrainView::net() const {
  return (Network*)data();
}

T3NetNode*
BrainView::node_so() const {
  return (T3NetNode*)inherited::node_so();
}

ISelectable::GuiContext
BrainView::shType() const {
  return GC_DUAL_DEF_VIEW;
}

bool
BrainView::hasViewProperties() const {
  return true;
}

BrainView* BrainView::New(Network* net, T3DataViewFrame*& fr) {
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
  
  // create BrainView
  BrainView* nv = new BrainView();
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

void BrainView::Initialize() {
  data_base = &TA_Network;
  bvp = NULL;
  display = true;
  lay_mv = true;
  net_text = true;
  net_text_xform.translate.SetXYZ(0.0f, 1.0f, -1.0f); // start at top back
  net_text_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 0.5f * taMath_float::pi); // start at right mid
  net_text_xform.scale = 0.7f;
  net_text_rot = -90.0f;

  unit_con_md = false;
  unit_disp_md = NULL;
  unit_disp_idx = -1;
  n_counters = 0;

  unit_sr = NULL;
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
}

void BrainView::Destroy() {
  CutLinks();
}

void BrainView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(lay_disp_modes, this);
  taBase::Own(scale, this);
  taBase::Own(scale_ranges, this);
  taBase::Own(cur_unit_vals, this);
  taBase::Own(max_size, this);
  taBase::Own(font_sizes, this);
  taBase::Own(view_params, this);
  taBase::Own(net_text_xform, this);
}

void BrainView::CutLinks() {
  // the bvp refs some of our stuff, so we have to nuke it
  if (bvp) {
    delete bvp; // should delete our ref
    bvp = NULL;
  }
  view_params.CutLinks();
  font_sizes.CutLinks();
  max_size.CutLinks();
  cur_unit_vals.CutLinks();
  scale_ranges.CutLinks();
  scale.CutLinks();
  lay_disp_modes.CutLinks();
  inherited::CutLinks();
}

void BrainView::CopyFromView(BrainView* cp) {
  Copy_(*cp);
  T3DataViewMain::CopyFromViewFrame(cp);
}

void BrainView::ChildUpdateAfterEdit(taBase* child, bool& handled) {
  if(taMisc::is_loading || !taMisc::gui_active) return;
  TypeDef* typ = child->GetTypeDef();
  if (typ->InheritsFrom(&TA_ScaleRange)) {
    handled = true;
  }
  if (!handled)
    inherited::ChildUpdateAfterEdit(child, handled);
}

void BrainView::BuildAll() { // populates all T3 guys
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

  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, net()->layers., li) {
    Projection* prjn;
    taLeafItr j;
    FOR_ITR_EL(Projection, prjn, lay->projections., j) {
      PrjnView* pv = new PrjnView();
      pv->SetData(prjn);
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

void BrainView::SetLayDispMode(const String& lay_nm, int disp_md) {       
  int i = lay_disp_modes.FindName(lay_nm);
  if(i < 0) return;
  lay_disp_modes.FastEl(i).value = disp_md;
}

int BrainView::GetLayDispMode(const String& lay_nm) {
  int i = lay_disp_modes.FindName(lay_nm);
  if(i < 0) return -1;
  return lay_disp_modes.FastEl(i).value.toInt();
}

void BrainView::ChildAdding(taDataView* child_) {
  inherited::ChildAdding(child_);
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if (!child) return;
  TypeDef* typ = child->GetTypeDef();
  if(typ->InheritsFrom(&TA_PrjnView)) {
    prjns.AddUnique(child);
  }
}

void BrainView::ChildRemoving(taDataView* child_) {
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if(child) {
    prjns.RemoveEl(child);      // just try it
  }
  inherited::ChildRemoving(child_);
}

void BrainView::UpdateName() {
  if (net()) {
    if (!name.contains(net()->name))
      SetName(net()->name + "_View");
  }
  else {
    if (name.empty())
      SetName("no_network");
  }
}

void BrainView::DataUpdateAfterEdit_impl() {
  UpdateName();
  InitDisplay(true);
  UpdateDisplay();
}

void BrainView::DataUpdateAfterEdit_Child_impl(taDataView* chld) {
  // called when lays/specs are updated; typically just update spec view
  UpdatePanel();
}

void BrainView::Dump_Load_post() {
  inherited::Dump_Load_post();
  if(taMisc::is_undo_loading) return; // none of this.
  // do full rebuild!
  Reset();                      
  BuildAll();
  Render();
}

taBase::DumpQueryResult BrainView::Dump_QuerySaveMember(MemberDef* md) {
  // don't save Layers/Prjn's, they have no persist state, and just get rebuilt
  if (md->name == "children") {
    return DQR_NO_SAVE;
  } else
    return inherited::Dump_QuerySaveMember(md);
} 

UnitView* BrainView::FindUnitView(Unit* unit) {
  UnitView* uv = NULL;
  taDataLink* dl = unit->data_link();
  if (!dl) return NULL;
  taDataLinkItr i;
  FOR_DLC_EL_OF_TYPE(UnitView, uv, dl, i) {
    if (uv->GetOwner(&TA_BrainView) == this)
      return uv;
  }
  return NULL;
}

// this fills a member group with the valid memberdefs from the units and connections
void BrainView::GetMembs() { 
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
    String_Array& oul = cur_unit_vals;
    for (int i=oul.size-1; i>=0; i--) { 
      if (!membs.FindName(oul[i]))
        oul.RemoveIdx(i,1);
    }
  }
}

void BrainView::GetUnitColor(float val,  iColor& col, float& sc_val) {
  iColor fl;  iColor tx;
  scale.GetColor(val,sc_val,&fl,&tx);
  col = fl;
}

void BrainView::GetUnitDisplayVals(BrainVolumeView* bvv, TwoDCoord& co, float& val, T3Color& col,
                                 float& sc_val) {
  sc_val = scale.zero;
  void* base = NULL;
  if(unit_disp_md && unit_md_flags != MD_UNKNOWN)
    val = bvv->GetUnitDisplayVal(co, base);
  if(!base) {
    col.setValue(.8f, .8f, .8f); // lt gray
    return;
  }
  iColor tc;
  GetUnitColor(val, tc, sc_val);
  col.setValue(tc.redf(), tc.greenf(), tc.bluef());
}

void BrainView::InitDisplay(bool init_panel) {
  // note: init display is kind of an odd bird -- subsumed under Render but also avail
  // independently -- deals with some kinds of changes but not really full structural
  // I guess it is just a "non structural state update" container..
  GetMaxSize();
  GetMembs();

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

void BrainView::InitPanel() {
  if (!bvp) return;
  bvp->InitPanel();
}

void BrainView::InitScaleRange(ScaleRange& sr) {
  sr.auto_scale = false;
  sr.min = -1.0f;
  sr.max =  1.0f;
}

void BrainView::Layer_DataUAE(LayerView* lv) {
  // simplest solution is just to call DataUAE on all prns...
  for (int i = 0; i < prjns.size; ++i) {
    PrjnView* pv = (PrjnView*)prjns.FastEl(i);
  }
}

void BrainView::GetMaxSize() {
  if(!net()) return;
  net()->UpdateMaxDispSize();
  max_size = net()->max_disp_size;
  max_size.z -= (max_size.z - 1.0f) / max_size.z; // leave 1 extra layer's worth of room..
  if(view_params.xy_square) {
    max_size.x = MAX(max_size.x, max_size.y);
    max_size.y = max_size.x;
  }
}

void BrainView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!bvp) {
    bvp = new BrainViewPanel(this);
    vw->RegisterPanel(bvp);
    vw->t3vs->Connect_SelectableHostNotifySignal(bvp,
      SLOT(viewWin_NotifySignal(ISelectableHost*, int)) );
  }
}

const iColor BrainView::bgColor(bool& ok) const {
  ok = true;
  return scale.background;
}


// this callback is registered in BrainView::Render_pre

void BrainVolumeView_MouseCB(void* userData, SoEventCallback* ecb) {
  BrainView* bv = (BrainView*)userData;
  T3DataViewFrame* fr = bv->GetFrame();
  SoMouseButtonEvent* mouseevent = (SoMouseButtonEvent*)ecb->getEvent();
  SoMouseButtonEvent::Button but = mouseevent->getButton();
  if(!SoMouseButtonEvent::isButtonReleaseEvent(mouseevent, but)) return; // only releases
  bool got_one = false;
  for(int i=0;i<fr->root_view.children.size;i++) {
    taDataView* dv = fr->root_view.children[i];
    if(dv->InheritsFrom(&TA_BrainView)) {
      BrainView* tnv = (BrainView*)dv;
      T3ExaminerViewer* viewer = tnv->GetViewer();
      SoRayPickAction rp( viewer->getViewportRegion());
      rp.setPoint(mouseevent->getPosition());
      rp.apply(viewer->quarter->getSoEventManager()->getSceneGraph()); // event mgr has full graph!
      SoPickedPoint* pp = rp.getPickedPoint(0);
      if(!pp) continue;
      SoNode* pobj = pp->getPath()->getNodeFromTail(2);
      if(!pobj) continue;
      if(!pobj->isOfType(T3BrainNode::getClassTypeId())) {
        pobj = pp->getPath()->getNodeFromTail(3);
        if(!pobj->isOfType(T3BrainNode::getClassTypeId())) {
          pobj = pp->getPath()->getNodeFromTail(1);
          if(pobj->getName() == "WtLines") {
            // disable selecting of wt lines!
            ecb->setHandled();
            return;
          }
          continue;
        }
      }
      BrainVolumeView* act_ugv = static_cast<BrainVolumeView*>(((T3BrainNode*)pobj)->dataView());
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
      got_one = true;
    }
  }
  if(got_one)
    ecb->setHandled();
}

void BrainView::Render_pre() {
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
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), BrainVolumeView_MouseCB, this);
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

void BrainView::Render_impl() {
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
    if (bvp) {
      bvp->ColorScaleFromData();
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
    net()->ProjectUnitWeights(unit_src, wt_prjn_k_un, wt_prjn_k_gp, wt_line_swt, 
                              (wt_prjn_k_un > 0 && wt_line_thr < 0.0f));
  }

  Render_wt_lines();

  inherited::Render_impl();
}

void BrainView::Render_net_text() {
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
    el += val;
    txt->string.setValue(el.chars());
    chld_idx++;
  }
}

void BrainView::Render_wt_lines() {
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
  TwoDCoord ru_pos; unit_src->LayerDispPos(ru_pos);
  FloatTDCoord src;             // source and dest coords
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
      TwoDCoord su_pos; su->LayerDispPos(su_pos);
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

void BrainView::Reset_impl() {
  prjns.Reset();
  inherited::Reset_impl();
}

void BrainView::SelectVar(const char* var_name, bool add, bool update) {
  if (!add)
    cur_unit_vals.Reset();
  MemberDef* md = (MemberDef*)membs.FindName(var_name);
  if (md) {
    cur_unit_vals.AddUnique(var_name);
  }
  setUnitDispMd(md);
  if (update) UpdateDisplay();
}

void BrainView::SetScaleData(bool auto_scale_, float min_, float max_, bool update_panel) {
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

void BrainView::SetScaleDefault() {
  if (unit_sr)  {
    InitScaleRange(*unit_sr);
    unit_sr->SetFromScaleRange(scale);
  } else {
    scale.auto_scale = false;
    scale.SetMinMax(-1.0f, 1.0f);
  }
}

void BrainView::SetColorSpec(ColorScaleSpec* color_spec) {
  scale.SetColorSpec(color_spec);
  UpdateDisplay(true);          // true causes button to remain pressed..
}

void BrainView::setUnitSrc(UnitView* uv, Unit* unit) {
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
}

void BrainView::setUnitDisp(int value) {
  MemberDef* md = membs.PosSafeEl(value);
  setUnitDispMd(md);
}

void BrainView::setUnitDispMd(MemberDef* md) {
  if (md == unit_disp_md) return;
  unit_disp_md = md;
  unit_disp_idx = membs.FindEl(md);
  unit_sr = NULL;
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
  unit_sr = scale_ranges.FindName(nm);
  if (unit_sr == NULL) {
    unit_sr = (ScaleRange*)(scale_ranges.New(1,&TA_ScaleRange));
    unit_sr->var_name = nm;
    InitScaleRange(*unit_sr);
  }
  unit_sr->SetFromScaleRange(scale);
}

void BrainView::UpdateViewerModeForMd(MemberDef* md) {
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


void BrainView::SetHighlightSpec(BaseSpec* spec) {
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

bool BrainView::UsesSpec(taBase* obj, BaseSpec* spec) {
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

void BrainView::UpdateAutoScale() {
  bool updated = false;
  if(children.size > 0) {
    LayerGroupView* lv = (LayerGroupView*)children.FastEl(0);
    lv->UpdateAutoScale(updated);
  }
  if (updated) { //note: could really only not be updated if there were no items
    scale.SymRange();           // keep it symmetric
    if (unit_sr)
      unit_sr->SetFromScale(scale); // update values
    if (bvp) {
      bvp->ColorScaleFromData();
    }
  } else { // set scale back to default so it doesn't look bizarre
    scale.SetMinMax(-1.0f, 1.0f);
  }
}

void BrainView::UpdateDisplay(bool update_panel) { // redoes everything
  if (update_panel) UpdatePanel();
  Render_impl();
}

void BrainView::UpdateUnitValues() { // *actually* only does unit value updating
  if(children.size == 0) return;
  LayerGroupView* lv = (LayerGroupView*)children.FastEl(0);
  lv->UpdateUnitValues();
}

void BrainView::DataUpdateView_impl() {
  if (!display) return;
  UpdateUnitValues();
  if(net_text) {
    Render_net_text();
  }
  Render_wt_lines();
}

void BrainView::UpdatePanel() {
  if (!bvp) return;
  bvp->UpdatePanel();
}

void BrainView::viewWin_NotifySignal(ISelectableHost* src, int op) {
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
