// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "BrainView.h"
#include <Layer>
#include <NewViewHelper>
#include <T3BrainNode>
#include <T3NetNode>
#include <iViewPanelOfBrain>
#include <BrainVolumeView>
#include <NiftiReader>
#include <iMethodButtonMgr>
#include <T3Panel>
#include <iT3Panel>
#include <T3ExaminerViewer>
#include <iT3ViewspaceWidget>
#include <taMath_float>
#include <T3Color>
#include <taSigLinkItr>

#include <taMisc>


#include <QObject>
#include <QFile>

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
#include <Inventor/actions/SoGLRenderAction.h>

TA_BASEFUNS_CTORS_DEFN(BrainViewParams);

TA_BASEFUNS_CTORS_DEFN(BrainView);

void BrainViewParams::Initialize() {
  unit_trans = 0.6f;
  net_name = .05f;
  net_vals = .05f;
  laygp_width = 1.0f;
} 

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

BrainView* BrainView::New(Network* net, T3Panel*& fr) {
  NewViewHelper newNetView(fr, net, "network");
  if (!newNetView.isValid()) return NULL;

  // set a black background color
  taColor c; c.Set(0.0f, 0.0f, 0.0f, 1.0f);
  fr->bg_color = c;
  c.Set(0.8f, 0.8f, 0.8f, 1.0f);
  fr->text_color = c;

  // create BrainView
  BrainView* bv = new BrainView();
  bv->SetData(net);
  fr->AddView(bv);

  // make sure we've got it all rendered:
  bv->main_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, .35f);
  bv->BuildAll();

  newNetView.showFrame();
  return bv;
}

void BrainView::Initialize() {
  data_base = &TA_Network;
  bvp = NULL;
  display = true;
  net_text = false;
  unit_src = NULL;
  lay_mv = true;

  unit_con_md = false;
  unit_disp_md = NULL;
  unit_disp_idx = -1;
  n_counters = 0;

  unit_sr = NULL;
  unit_md_flags = MD_UNKNOWN;

  state_valid = false;
  brain_data_name = "NULL";
  brain_dimensions.SetXYZ(182,218,182);
  view_plane = AXIAL;
  slice_start = 1;
  slice_end = 182;
  num_slices = 182;
  lock_num_slices = false;
  slice_spacing = 1;
  slice_transparency = 90;
  last_state_change = NONE;
  color_brain = false;
  color_brain_regexp = ".*/.*/.*/.*/.*";
  view_params.unit_trans = 0.9f;  // with high-fill pct, unit trans should be much higher
  show_atlas = false;
  brain_area_regexp = ".*/.*/.*/.*/Hippocampus"; // default to show Hippocampus
  state_items_stale = true;

  QString data_path(taMisc::app_dir.toQString());
  if (data_path.size() != 0) {
    data_path += "/data/standard/MNI152_T1_1mm_brain.nii";
    SetDataName(data_path);
    if (!IsValid()) {
      taMisc::Info("Failed to set path to brain data...", data_path.toStdString().c_str());
    }
  }
}

void BrainView::Destroy() {
  CutLinks();
}

void BrainView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(scale, this);
  taBase::Own(scale_ranges, this);
  taBase::Own(cur_unit_vals, this);
  taBase::Own(view_params, this);
  taBase::Own(net_state_text, this);
}

void BrainView::CutLinks() {
  // the bvp refs some of our stuff, so we have to nuke it
  delete bvp; // should delete our ref
  bvp = 0;

  view_params.CutLinks();
  cur_unit_vals.CutLinks();
  scale_ranges.CutLinks();
  scale.CutLinks();
  net_state_text.CutLinks();
  inherited::CutLinks();
}

BrainView* Network::FindBrainView() {
  taSigLink* dl = sig_link();
  if(dl) {
    taSigLinkItr itr;
    BrainView* el;
    FOR_DLC_EL_OF_TYPE(BrainView, el, dl, itr) {
      return el;
    }
  }
  return NULL;
}

void Network::FindBrainViews(T3DataView_List* dv_list) {
  taSigLink* dl = sig_link();
  if(dl) {
    taSigLinkItr itr;
    BrainView* el;
    FOR_DLC_EL_OF_TYPE(BrainView, el, dl, itr) {
      dv_list->Link(el);
    }
  }
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

  Network* nt = net();

  // Set the name of the structural brain image data - NOT the atlas!
  QString data_path(taMisc::app_dir.toQString());
  data_path += "/data/standard/MNI152_T1_1mm_brain.nii";

  SetDataName(data_path);
  if (!IsValid()) {
    taMisc::Error("Invalid brain view data");
    return;
  }

  BrainVolumeView* bvv = new BrainVolumeView();
  bvv->SetData(nt);
  children.Add(bvv);
  bvv->BuildAll();
  
  GetNetTextItems();
}

void BrainView::UpdateName() {
  if (net()) {
    if (!name.contains(net()->name))
      SetName(net()->name + "_BrainView");
  }
  else {
    if (name.empty())
      SetName("no_network");
  }
}

void BrainView::SigRecvUpdateAfterEdit_impl() {
  UpdateName();
  InitDisplay(true);
  UpdateDisplay();
}

void BrainView::SigRecvUpdateAfterEdit_Child_impl(taDataView* chld) {
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
  EmitAll(); //update panel widgets with our state
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

  NetworkState_cpp* net_state = net()->net_state;
  if(!net_state->IsBuiltIntact()) return;
  
  // try as hard as possible to find a unit to view if nothing selected -- this
  // minimizes issues with history etc
  if(!unit_src) {
    if(unit_src_path.nonempty()) {
      UnitState_cpp* nu = net()->GetUnitStateFromPath(unit_src_path);
      if(nu) setUnitSrc(nu);
    }
    if(!unit_src && net()->layers.leaves > 0) {
      Layer* lay = net()->layers.Leaf(net()->layers.leaves-1);
      if(lay->n_units_built > 0)
        setUnitSrc(lay->GetUnitState(net_state, 0));
    }
  }

  setUnitDispMd(NULL);
  membs.Reset();
  TypeDef* prv_td = NULL;

  // only get units that have been mapped to voxel coords...
  FOREACH_ELEM_IN_GROUP(Layer, lay, net()->layers) {
    if (lay->lesioned() || lay->Iconified() || lay->brain_area.empty()) continue;
    // for(int ui = 0; ui < lay->n_units_built; ui++) {
    //   UNIT_STATE* u = lay->GetUnitState(net, ui);
    //   if (!u->voxels || u->voxels->size == 0) continue;
    //   // TODO: for now, assumes only one voxel per unit.  Update to handle multiple.
    //   Voxel *voxel = u->voxels->FastEl(0);
    //   if (voxel->size == 0) continue;
    //   if (u->lesioned()) continue;

    //   TypeDef* td = u->GetTypeDef();
    //   if(td == prv_td) continue; // don't re-scan!
    //   prv_td = td;

    //   for(int m=0; m<td->members.size; m++) {
    //     MemberDef* md = td->members.FastEl(m);
    //     if((md->HasOption("NO_VIEW") || md->IsInvisible()))
    //       continue;
    //     if((md->type->InheritsFrom(&TA_float) || md->type->InheritsFrom(&TA_double))
    //        && (membs.FindName(md->name)==NULL))
    //     {
    //       MemberDef* nmd = md->Clone();
    //       membs.Add(nmd);       // index now reflects position in list...
    //       nmd->idx = md->idx;   // so restore it to the orig one
    //     }       // check for nongroup owned sub fields (ex. bias)
    //     else if(md->type->DerivesFrom(&TA_taBase) && !md->type->DerivesFrom(&TA_taGroup)) {
    //       if(md->type->IsPtrPtr()) continue; // only one level of pointer tolerated
    //       TypeDef* nptd;
    //       if(md->type->IsPointer()) {
    //         taBase** par_ptr = (taBase**)md->GetOff((void*)u);
    //         if(*par_ptr == NULL) continue; // null pointer
    //         nptd = (*par_ptr)->GetTypeDef(); // get actual type of connection
    //       }
    //       else
    //         nptd = md->type;
    //       int k;
    //       for(k=0; k<nptd->members.size; k++) {
    //         MemberDef* smd = nptd->members.FastEl(k);
    //         if(smd->type->InheritsFrom(&TA_float) || smd->type->InheritsFrom(&TA_double)) {
    //           if((smd->HasOption("NO_VIEW") || smd->IsInvisible()))
    //             continue;
    //           String nm = md->name + "." + smd->name;
    //           if(membs.FindName(nm)==NULL) {
    //             MemberDef* nmd = smd->Clone();
    //             nmd->name = nm;
    //             membs.Add(nmd);
    //             nmd->idx = smd->idx;
    //           }
    //         }
    //       }
    //     }
    //   }

//      // then do bias weights
//      if(u->bias.size) {
//        TypeDef* td = u->bias.con_type;
//        for(int k=0; k<td->members.size; k++) {
//          MemberDef* smd = td->members.FastEl(k);
//          if(smd->type->InheritsFrom(&TA_float) || smd->type->InheritsFrom(&TA_double)) {
//            if((smd->HasOption("NO_VIEW") || smd->IsInvisible()))
//              continue;
//            String nm = "bias." + smd->name;
//            if(membs.FindName(nm)==NULL) {
//              MemberDef* nmd = smd->Clone();
//              nmd->name = nm;
//              membs.Add(nmd);
//              nmd->idx = smd->idx;
//            }
//          }
//        }
//      }
    // }
  }

  // then, only do the connections if any Unit guys, otherwise we are
  // not built yet, so we leave ourselves empty to signal that
//  if (membs.size > 0) {
//    FOREACH_ELEM_IN_GROUP(Layer, lay, net()->layers) {
//      FOREACH_ELEM_IN_GROUP(Projection, prjn, lay->projections) {
//        TypeDef* td = prjn->con_type;
//        if(td == prv_td) continue; // don't re-scan!
//        prv_td = td;
//        int k;
//        for (k=0; k<td->members.size; k++) {
//          MemberDef* smd = td->members.FastEl(k);
//          if(smd->type->InheritsFrom(&TA_float) || smd->type->InheritsFrom(&TA_double)) {
//            if((smd->HasOption("NO_VIEW") || smd->IsInvisible()))
//              continue;
//            String nm = "r." + smd->name;
//            if(membs.FindName(nm)==NULL) {
//              MemberDef* nmd = smd->Clone();
//              nmd->name = nm;
//              membs.Add(nmd);
//              nmd->idx = smd->idx;
//            }
//            nm = "s." + smd->name;
//            if(membs.FindName(nm)==NULL) {
//              MemberDef* nmd = smd->Clone();
//              nmd->name = nm;
//              membs.Add(nmd);
//              nmd->idx = smd->idx;
//            }
//          }
//        }
//      }
//    }
//  }

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

void BrainView::GetUnitDisplayVals(BrainVolumeView* bvv, UnitState_cpp* u, float& val, iColor& col, float& sc_val) {
  sc_val = scale.zero;
  void* base = NULL;
  if(unit_disp_md && unit_md_flags != MD_UNKNOWN)
    val = bvv->GetUnitDisplayVal(u, base);
  if(!u) {
    col.setRgb(.8f, .8f, .8f); // lt gray
    return;
  }
  GetUnitColor(val, col, sc_val);
}

void BrainView::GetUnitDisplayVals(BrainVolumeView* bvv, taVector2i& co, float& val,
                                   iColor& col, float& sc_val) {
//  sc_val = scale.zero;
//  void* base = NULL;
//  if(unit_disp_md && unit_md_flags != MD_UNKNOWN)
//    val = bvv->GetUnitDisplayVal(co, base);
//  if(!base) {
//    col.setRgb(.8f, .8f, .8f); // lt gray
//    return;
//  }
//  GetUnitColor(val, col, sc_val);
}

void BrainView::InitDisplay(bool init_panel) {
  // note: init display is kind of an odd bird -- subsumed under Render but also avail
  // independently -- deals with some kinds of changes but not really full structural
  // I guess it is just a "non structural state update" container..
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

void BrainView::OnWindowBind_impl(iT3Panel* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!bvp) {
    bvp = new iViewPanelOfBrain(this);
    vw->RegisterPanel(bvp);
    vw->t3vs->Connect_SelectableHostNotifySignal(bvp,
      SLOT(viewWin_NotifySignal(ISelectableHost*, int)));
    EmitAll(); //update panel gui widgets
  }
}

const iColor BrainView::bgColor(bool& ok) const {
  ok = true;
  return scale.background;
}


// this callback is registered in BrainView::Render_pre

#ifndef TA_QT3D
void BrainVolumeView_MouseCB(void* userData, SoEventCallback* ecb) {
  BrainView* bv = (BrainView*)userData;
  T3Panel* fr = bv->GetFrame();
  SoMouseButtonEvent* mouseevent = (SoMouseButtonEvent*)ecb->getEvent();
  SoMouseButtonEvent::Button but = mouseevent->getButton();
  if(!SoMouseButtonEvent::isButtonReleaseEvent(mouseevent, but)) return; // only releases
  bool got_one = false;
  for(int i=0;i<fr->root_view.children.size;i++) {
    taDataView* dv = fr->root_view.children[i];
    if(dv->InheritsFrom(&TA_BrainView)) {
      BrainView* tnv = (BrainView*)dv;
      T3ExaminerViewer* viewer = tnv->GetViewer();
      SoRayPickAction rp(viewer->getViewportRegion());
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
      got_one = true;
    }
  }
  if(got_one)
    ecb->setHandled();
}
#endif

void BrainView::Render_pre() {
  InitDisplay();


  bool show_drag = true;;
  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->syncViewerMode();
    show_drag = vw->interactionModeOn();
  }
  if(!lay_mv) show_drag = false;

#ifdef TA_QT3D
  setNode(new T3NetNode(NULL, this, show_drag, net_text, show_drag));
#else // TA_QT3D
  setNode(new T3NetNode(this, show_drag, net_text, show_drag));
  SoMaterial* mat = node_so()->material(); //cache
  mat->diffuseColor.setValue(0.0f, 0.5f, 0.5f); // blue/green
  mat->transparency.setValue(0.5f);
#endif // TA_QT3D

  if(vw && vw->interactionModeOn()) {
#ifndef TA_QT3D
    SoEventCallback* ecb = new SoEventCallback;
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), BrainVolumeView_MouseCB, this);
    node_so()->addChild(ecb);
#endif
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

  T3NetNode* node_so = this->node_so(); //cache
  if(!node_so) return;
  node_so->resizeCaption(view_params.net_name);

  String cap_txt = data()->GetName() + " Value: ";
  if(unit_disp_md)
    cap_txt += unit_disp_md->name;

#ifdef TA_QT3D

#else // TA_QT3D
  if(node_so->shapeDraw())
    node_so->shapeDraw()->lineWidth = view_params.laygp_width;
#endif // TA_QT3D

  node_so->setCaption(cap_txt.chars());

  if (scale.auto_scale) {
    UpdateAutoScale();
    if (bvp) {
      bvp->ColorScaleFromData();
    }
  }

  if(net_text) {
    RenderStateValues();
  }
  else {
    vw->ClearStateValues();
  }

  inherited::Render_impl();
}

void BrainView::RenderStateValues() {
  if (!net_text) return;
  
  NameVar_Array net_state_strs;
  TypeDef* td = net()->GetTypeDef();
  
  for(int i=0; i<net_state_text.state_items.size; i++) {
    NetViewStateItem* item = net_state_text.GetItem(i);
    if (item->display == false) {
      continue;
    }
    String var = item->name;
    String val;
    String item_text = item->name + ": ";
    if (item->net_member) {
      MemberDef* md = td->members.FindName(var);
      if (md) {
        val = md->GetValStr((void*)net());
      }
    }
    else {  // monitor var
      Network* nt = net();
      if (nt) {
        DataTable* monitor_data = &nt->mon_data;
        if (monitor_data) {
          if (monitor_data->rows > 0) {
            val = monitor_data->GetValAsVar(item->name, -1).toString();
          }
        }
      }
    }
    item_text = item_text + val;
    net_state_strs.Add(NameVar(item_text, item->width));
    
  }
  T3ExaminerViewer* vw = GetViewer();
  if (vw) {
    vw->UpdateStateValues(net_state_strs);
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

void BrainView::setUnitSrc(UnitState_cpp* unit) {
  if (unit_src == unit) return; // no change
  // if there was existing unit, unpick it
  unit_src = unit;
  if(unit_src) {
    unit_src_path = net()->GetUnitStatePath(unit_src);
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
  BrainVolumeView* bvv = (BrainVolumeView*)children.FastEl(0);
  bvv->UpdateUnitValues();
}

void BrainView::SigRecvUpdateView_impl() {
  if (!display) return;
  UpdateUnitValues();
  if(net_text) {
    RenderStateValues();
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
//   UnitState_cpp* unit_new = uv->unit();
//   setUnitSrc(uv, unit_new);
//   InitDisplay();
//   UpdateDisplay();
}

void BrainView::AsyncRenderUpdate()
{
  BrainVolumeView* bvv = (BrainVolumeView*)children.FastEl(0);
  bvv->UpdateSlices();
}

bool BrainView::IsValid() const
{
  return state_valid;
}

String BrainView::DataName() const
{
  return brain_data_name;
}

taVector3i BrainView::Dimensions() const
{
  return brain_dimensions;
}

BrainView::AnatomicalPlane BrainView::ViewPlane() const
{
  return view_plane;
}

int BrainView::SliceStart() const
{
  return slice_start;
}

int BrainView::SliceEnd() const
{
  return slice_end;
}

bool BrainView::NumSlicesAreLocked() const
{
  return lock_num_slices;
}

int BrainView::SliceSpacing() const
{
  return slice_spacing;
}

int BrainView::SliceTransparency() const
{
  return slice_transparency;
}

float BrainView::SliceTransparencyXformed() const
{
  // if user is looking at a single slice, make fully opaque
  if (slice_end - slice_start == 0) {
    return 0.0f;
  }
  // otherwise obey transparency setting
  else {
    float offset = 100.0f;
    float logtr = taMath_float::log(offset + slice_transparency);
    float denom = taMath_float::log(offset + 100.0f);
    return logtr / denom;
  }
 }

int BrainView::UnitValuesTransparency() const
{
  return unit_val_transparency;
}

int BrainView::NumSlicesValid() const
{
  return (MaxSlices() - slice_start + 1);
}

int BrainView::NumSlices() const
{
  return num_slices;
}

int BrainView::MaxSlices() const
{
  int mx(0);
  if (view_plane == AXIAL) {
    mx = brain_dimensions.z;
  }
  else if (view_plane == SAGITTAL) {
    mx = brain_dimensions.x;
  }
  else { //CORONAL
    mx = brain_dimensions.y;
  }

  return mx;
}

bool BrainView::ColorBrain() const
{
  return color_brain;
}

String BrainView::ColorBrainRegexp() const
{
  return color_brain_regexp;
}

bool BrainView::ViewAtlas() const
{
  return show_atlas;
}

String BrainView::ViewAtlasRegexp() const
{
  return brain_area_regexp;
}

String BrainView::NetworkBrainAreas() const
{
  QString brain_areas("");
  Network* net = this->net();
  if (!net) return brain_areas;

  brain_areas = "(";
  int idx(0);
  FOREACH_ELEM_IN_GROUP(Layer, lay, net->layers) {
    if (lay->lesioned() || lay->Iconified() || lay->brain_area.empty()) continue;
    if (idx == 0) {
      brain_areas += lay->brain_area.toQString();
    }
    else {
      brain_areas += "|" + lay->brain_area.toQString();
    }
    idx++;
  }
  brain_areas += ")";

  if (brain_areas == "()") {
    brain_areas = "";
  }
  return brain_areas;
}

void BrainView::SetDataName(const String& data_name)
{
  if (data_name == brain_data_name) {
    return;
  }

  // be sure we can read a valid data set before deleting
  // our existing data set
  QFile f(data_name);
  if (!f.exists()) {
    taMisc::Info("File does not exist:", data_name);
    return;
  }

  NiftiReader tmp(data_name);
  if (!tmp.IsValid()) {
    taMisc::Info("NiftiReader could not read:", data_name);
    return;
  }

  brain_data_name = data_name;
  SetDimensions(tmp.XyzDimensions());
  last_state_change |= MAJOR;

  if (bvp) {
    bvp->EmitDataNameChanged(brain_data_name);
  }

  ValidateState();
  state_valid = true;
}

void BrainView::SetDimensions(const taVector3i& dimensions)
{
  if (dimensions == brain_dimensions)
    return;

  brain_dimensions = dimensions;
  last_state_change |= MAJOR;
  if (bvp) bvp->EmitDimensionsChanged(brain_dimensions);
  ValidateState();
}

void  BrainView::SetViewPlane(AnatomicalPlane plane)
{
  if (plane == view_plane)
    return;

  view_plane = plane;
  last_state_change |= MAJOR;
  if (bvp) bvp->EmitViewPlaneChanged(static_cast<int>(plane));
  ValidateState();
}

void  BrainView::SetViewPlane(int plane)
{
  SetViewPlane(static_cast<AnatomicalPlane>(plane));
}

void  BrainView::SetSliceStart(int start)
{
  if (start == slice_start)
    return;

  // if user has fixed the number of slices, we must update
  // sliceEnd if we change
  if (lock_num_slices) {
    if ((0 < start) && ((start + num_slices) <= MaxSlices())) {
      slice_start = start;
      SetSliceEnd(slice_start + num_slices);
      last_state_change |= MINOR;
      if (bvp) bvp->EmitSliceStartChanged(slice_start);
      ValidateState();
    }
  }
  else {
    // we only need to update the sliceEnd if we've pushed
    // past it
    if ((0 < start) && (start <= MaxSlices())) {
      slice_start = start;
      if (slice_start > slice_end) {
        SetSliceEnd(slice_start);
      }
      last_state_change |= MINOR;
      if (bvp) bvp->EmitSliceStartChanged(slice_start);
      ValidateState();
    }
  }
}

void  BrainView::SetSliceEnd(int end)
{
  if (end == slice_end)
    return;

  // if user has fixed the number of slices, we must update
  // sliceStart if we change
  if (lock_num_slices) {
    if ((0 < (end - num_slices)) && (end <= MaxSlices())) {
      slice_end = end;
      SetSliceStart(slice_end - num_slices);
      last_state_change |= MINOR;
      if (bvp) bvp->EmitSliceEndChanged(slice_end);
      ValidateState();
    }
  }
  else {
    // only need to update the sliceStart if we've pushed
    // past it
    if ((0 < end) && (end <= MaxSlices())) {
      slice_end = end;
      if (slice_end < slice_start) {
        SetSliceStart(slice_end);
      }
      last_state_change |= MINOR;
      if (bvp) bvp->EmitSliceEndChanged(slice_end);
      ValidateState();
    }
  }
}

void BrainView::SetLockSlices(int state)
{
  bool newState = false;
  if (static_cast<Qt::CheckState>(state) == Qt::Checked)
    newState = true;
  else
    newState = false;

  if (newState == lock_num_slices)
    return;

  lock_num_slices = newState;
  num_slices = slice_end - slice_start;
  last_state_change |= MINOR;
  EmitAndClearState();
}

void BrainView::SetSliceSpacing(int spacing)
{
  if (spacing == slice_spacing)
    return;

  slice_spacing = spacing;
  last_state_change |= MAJOR;
  if (bvp) bvp->EmitSliceSpacingChanged(slice_spacing);
  EmitAndClearState();
}

void  BrainView::SetSliceTransparency(int transparency)
{
  if (transparency == slice_transparency)
    return;

  slice_transparency = transparency;
  last_state_change |= MINOR;
  if (bvp) bvp->EmitSliceTransparencyChanged(slice_transparency);
  EmitAndClearState();
}

void  BrainView::SetUnitValuesTransparency(int transparency)
{
  if (transparency == unit_val_transparency)
    return;

  unit_val_transparency = transparency;
  last_state_change |= MINOR;
  if (bvp) bvp->EmitUnitValuesTransparencyChanged(unit_val_transparency);
  EmitAndClearState();
}

void BrainView::SetColorBrain(int state)
{
  bool newState = false;
  if (static_cast<Qt::CheckState>(state) == Qt::Checked)
    newState = true;
  else
    newState = false;

  if (newState == color_brain)
    return;

  color_brain = newState;
  last_state_change |= MAJOR;
  EmitAndClearState();
}

void BrainView::SetColorBrainRegexp(const String& regexp)
{
  if (regexp != color_brain_regexp) {
    color_brain_regexp = regexp;
    last_state_change |= MAJOR;
    EmitAndClearState();
  }
}

void BrainView::SetViewAtlas(int state)
{
  bool newState = false;
  if (static_cast<Qt::CheckState>(state) == Qt::Checked)
    newState = true;
  else
    newState = false;

  if (newState == show_atlas)
    return;

  show_atlas = newState;
  last_state_change |= MAJOR;
  EmitAndClearState();
}

void BrainView::SetViewAtlasRegexp(const String& regexp)
{
  if (regexp != brain_area_regexp) {
    brain_area_regexp = regexp;
    last_state_change |= MAJOR;
    EmitAndClearState();
  }
}

bool BrainView::ValidSliceStart() const
{
  return (slice_start <= MaxSlices()) ? true : false;
}

bool BrainView::ValidSliceEnd() const
{
  return (slice_end <= MaxSlices()) ? true : false;
}

void BrainView::ValidateState()
{
  // priority is:
  //    1. set Anatomical Plane (as it drives geometry limits)
  //    2. reset starting slice
  //    3. reset number of slices
  if (!ValidSliceStart())
    SetSliceStart(1);
  if (!ValidSliceEnd())
    SetSliceEnd(MaxSlices());
  EmitAndClearState();
}

void BrainView::EmitAndClearState()
{
  // iViewPanelOfBrain listens...
  if (bvp) bvp->EmitStateChanged(last_state_change);
  last_state_change = NONE;
}
void BrainView::EmitAll()
{
  // this allows us to update iViewPanelOfBrain with our state
  if (bvp) {
    bvp->EmitViewPlaneChanged(view_plane);
    bvp->EmitSliceStartChanged(slice_start);
    bvp->EmitSliceEndChanged(slice_end);
    bvp->EmitSliceTransparencyChanged(slice_transparency);
    bvp->EmitUnitValuesTransparencyChanged(unit_val_transparency);
    bvp->EmitColorBrainAreaRegexpChanged(color_brain_regexp);
    bvp->EmitViewAtlasRegexpChanged(brain_area_regexp);
  }
}

void BrainView::GetNetTextItems() {
  if(!net()) return;
  if(!state_items_stale) return;
  
  net_state_text.GetItems(net());
  
  state_items_stale = false;
}

int BrainView::GetNetTextItemWidth(const String& name) {
  return net_state_text.GetItemDisplayWidth(name);
}

void BrainView::SetNetTextItemWidth(const String& name, int width) {
  net_state_text.SetItemDisplayWidth(name, width);
  T3ExaminerViewer* vw = GetViewer();
  if (vw) {
    vw->state_labels_inited = false;
    RenderStateValues();
  }
  UpdatePanel();
}

void BrainView::MonitorUpdate() {
  state_items_stale = true;  // some net monitor item change
}

