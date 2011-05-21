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
#include "NewNetViewHelper.h"
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
  NewNetViewHelper newNetView(fr, net, "network");
  if (!newNetView.isValid()) return NULL;

  // create BrainView
  BrainView* nv = new BrainView();
  nv->SetData(net);
  nv->GetMaxSize();
  fr->AddView(nv);

  // make sure we've got it all rendered:
  nv->main_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, .35f);
  nv->BuildAll();

  newNetView.showFrame();
  return nv;
}

void BrainView::Initialize() {
  data_base = &TA_Network;
  bvp = NULL;
  display = true;
  net_text = true;
  lay_mv = false;
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
}

void BrainView::Destroy() {
  CutLinks();
}

void BrainView::InitLinks() {
  inherited::InitLinks();
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

  BrainVolumeView* bv = new BrainVolumeView();
  bv->SetData(nt);
  children.Add(bv);
  bv->BuildAll();
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

// this fills a member group with the valid memberdefs from the units and connections
void BrainView::GetMembs() { 
  if(!net()) return;

  // try as hard as possible to find a unit to view if nothing selected -- this 
  // minimizes issues with history etc
  if(!unit_src) {
    if(unit_src_path.nonempty()) {
      MemberDef* umd;
      Unit* nu = (Unit*)net()->FindFromPath(unit_src_path, umd);
      if(nu) setUnitSrc(nu);
    }
    if(!unit_src && net()->layers.leaves > 0) {
      Layer* lay = net()->layers.Leaf(net()->layers.leaves-1);
      if(lay->units.leaves > 0)
        setUnitSrc(lay->units.Leaf(0));
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
    BrainVolumeView* lv = (BrainVolumeView*)children.FastEl(0);
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
//       Layer* lay = act_ugv->layer(); //cache
//       float disp_scale = lay->disp_scale;

//       SbVec3f pt = pp->getObjectPoint(pobj); 
//       int xp = (int)((pt[0] * tnv->max_size.x) / disp_scale);
//       int yp = (int)(-(pt[2] * tnv->max_size.y) / disp_scale);
   
//       if((xp >= 0) && (xp < lay->disp_geom.x) && (yp >= 0) && (yp < lay->disp_geom.y)) {
//         Unit* unit = lay->UnitAtDispCoord(xp, yp);
//         if(unit && tnv->unit_src != unit) {
//           tnv->setUnitSrc(unit);
//           tnv->InitDisplay();   // this is apparently needed here!!
//           tnv->UpdateDisplay();
//         }
//       }
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

  setNode(new T3NetNode(this, show_drag, net_text, show_drag));
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

void BrainView::setUnitSrc(Unit* unit) {
  if (unit_src.ptr() == unit) return; // no change
  // if there was existing unit, unpick it
  unit_src = unit;
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

void BrainView::UpdateAutoScale() {
  bool updated = false;
  if(children.size > 0) {
    BrainVolumeView* lv = (BrainVolumeView*)children.FastEl(0);
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
  BrainVolumeView* lv = (BrainVolumeView*)children.FastEl(0);
  lv->UpdateUnitValues();
}

void BrainView::DataUpdateView_impl() {
  if (!display) return;
  UpdateUnitValues();
  if(net_text) {
    Render_net_text();
  }
}

void BrainView::UpdatePanel() {
  if (!bvp) return;
  bvp->UpdatePanel();
}

void BrainView::viewWin_NotifySignal(ISelectableHost* src, int op) {
  if (op != ISelectableHost::OP_SELECTION_CHANGED) return;
  ISelectable* ci = src->curItem(); // first selected item, if any
  if (!ci) return;
  // not currently needed
//   TypeDef* typ = ci->GetTypeDef();
//   if (!typ->InheritsFrom(&TA_UnitView)) return;
//   UnitView* uv = (UnitView*)ci->This();
//   Unit* unit_new = uv->unit();
//   setUnitSrc(uv, unit_new);
//   InitDisplay();
//   UpdateDisplay();
}

