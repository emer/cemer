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

#include "NetView.h"
#include <Layer>
#include <Projection>
#include <T3NetNode>
#include <iViewPanelOfNetwork>
#include <LayerGroupView>
#include <PrjnView>
#include <UnitGroupView>
#include <UnitView>
#include <NewViewHelper>
#include <T3Panel>
#include <iT3Panel>
#include <iT3ViewspaceWidget>
#include <T3ExaminerViewer>
#include <taSigLinkItr>
#include <MemberDef>
#include <taMath_float>
#include <MainWindowViewer>
#include <T3LayerNode>
#include <T3UnitGroupNode>
#include <QKeyEvent>
#include <QApplication>
#include <taSvg>
#include <UserDataItem_List>
#include <taProject>
#include <DataTable>
#include <ProgExprBase>

#include <T3Misc>
#include <taMisc>

#ifdef DEBUG
#include <QElapsedTimer>
#endif


#ifdef TA_QT3D

#include <T3TwoDText>
#include <T3LineStrip>

#else // TA_QT3D

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/SoEventManager.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>
#endif // TA_QT3D

TA_BASEFUNS_CTORS_DEFN(NetViewParams);

TA_BASEFUNS_CTORS_DEFN(NetViewFontSizes);

TA_BASEFUNS_CTORS_DEFN(NetView);

void NetViewFontSizes::Initialize() {
  net_name = .05f;
  net_vals = .05f;
  layer = .04f;
  layer_min = 0.01f;
  layer_vals = .03f;
  prjn = .01f;
  unit = .02f;
  un_nm_len = 3;
  un_val_prec = 2;
}

void NetViewParams::Initialize() {
  xy_square = false;
  unit_spacing = .05f;
  prjn_disp = L_R_B;
  prjn_name = false;
  prjn_width = .002f;
  prjn_trans = .5f;
  lay_trans = 0.5f;
  unit_trans = 0.6f;
  laygp_width = 1.0f;
  show_laygp = true;
}

#ifndef TA_QT3D
void UnitGroupView_MouseCB(void* userData, SoEventCallback* ecb) {
  NetView* nv = (NetView*)userData;
  nv->selected_unit_var = _nilString;

  T3Panel* fr = nv->GetFrame();
  SoMouseButtonEvent* mouseevent = (SoMouseButtonEvent*)ecb->getEvent();
  SoMouseButtonEvent::Button but = mouseevent->getButton();
  if(!SoMouseButtonEvent::isButtonReleaseEvent(mouseevent, but))
    return; // only releases
  bool inspect_mode = false;
  bool monitor_unit = false;
  if (QApplication::keyboardModifiers().testFlag(Qt::AltModifier) == true) {
    inspect_mode = true;
  }
  if (QApplication::keyboardModifiers().testFlag(Qt::MetaModifier) == true) {
    monitor_unit = true;
  }
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
        // taMisc::Info("obj typ: ", pobj->getTypeId().getName());
        if(!pobj->isOfType(T3UnitGroupNode::getClassTypeId())) {
          pobj = pp->getPath()->getNodeFromTail(3);
          // taMisc::Info("2: obj typ: ", pobj->getTypeId().getName());
          if(!pobj->isOfType(T3UnitGroupNode::getClassTypeId())) {
            pobj = pp->getPath()->getNodeFromTail(1);
            // taMisc::Info("3: obj typ: ", pobj->getTypeId().getName());
            if(pobj->getName() == "WtLines") {
              // disable selecting of wt lines!
              ecb->setHandled();
              // taMisc::Info("wt lines bail");
              return;
            }
            //    taMisc::Info("not unitgroupnode! bail");
            fail_cnt++;
            continue;
          }
        }
        UnitGroupView* act_ugv =
          static_cast<UnitGroupView*>(((T3UnitGroupNode*)pobj)->dataView());
        Layer* lay = act_ugv->layer(); //cache
        Network* net = lay->own_net;
        float disp_scale = lay->disp_scale;
        
        SbVec3f pt = pp->getObjectPoint(pobj);
        int xp = (int)((pt[0] * tnv->eff_max_size.x) / disp_scale);
        int yp = (int)(-(pt[2] * tnv->eff_max_size.y) / disp_scale);
        
        if((xp >= 0) && (xp < lay->disp_geom.x) && (yp >= 0) && (yp < lay->disp_geom.y)) {
          UnitState_cpp* unit = lay->UnitAtDispCoord(xp, yp);
          if (unit && nv->unit_disp_md) {
            if(nv->unit_con_md && nv->unit_src) {
              // see UnitGroupView::UpdateUnitViewBase_Con_impl for relevant code
              // todo: could perhaps put this in a common method or something..
              bool check_prjn = (nv->prjn_starts_with.nonempty());
              String unm = nv->unit_disp_md->name;
              String sr = unm.before('.');
              bool is_send = (sr == "s");
              String nm = unm.after('.');
              if (is_send) {
                for(int g=0;g<unit->NRecvConGps(net->net_state);g++) {
                  ConState_cpp* tcong = unit->RecvConState(net->net_state, g);
                  PrjnState_cpp* pjs = tcong->GetPrjnState(net->net_state);
                  Projection* prjn = net->PrjnFromState(pjs);
                  if(check_prjn && prjn &&
                     !prjn->name.startsWith(nv->prjn_starts_with))
                    continue;
                  MemberDef* act_md = tcong->ConType(net)->members.FindName(nm);
                  if (!act_md)  continue;
                  int con = tcong->FindConFromIdx(nv->unit_src->flat_idx);
                  if (con < 0) continue;
                  // have to use safe b/c could be PtrCon and other side might be gone..
                  nv->last_sel_unit_val = (String)tcong->CnSafeFast(con, act_md->idx, net->net_state);
                  break;                // once you've got one, done!
                }
              }
              else {
                for(int g=0;g<unit->NSendConGps(net->net_state);g++) {
                  ConState_cpp* tcong = unit->SendConState(net->net_state, g);
                  PrjnState_cpp* pjs = tcong->GetPrjnState(net->net_state);
                  Projection* prjn = net->PrjnFromState(pjs);
                  if(check_prjn && prjn && prjn->MainIsActive() &&
                     !prjn->name.startsWith(nv->prjn_starts_with))
                    continue;
                  MemberDef* act_md = tcong->ConType(net)->members.FindName(nm);
                  if (!act_md)  continue;
                  int con = tcong->FindConFromIdx(nv->unit_src->flat_idx);
                  if (con < 0) continue;
                  // have to use safe b/c could be PtrCon and other side might be gone..
                  nv->last_sel_unit_val = (String)tcong->CnSafeFast(con, act_md->idx, net->net_state);
                  break;                // once you've got one, done!
                }
              }
            }
            else if (monitor_unit) {
              if (net) {
                nv->CallFun("GetUnitMonitorVar");
                if (nv->selected_unit_var.nonempty()) {
                  nv->MonitorUnit(lay, unit->lay_un_idx, nv->selected_unit_var);
                }
              }
            }
            else {
              nv->last_sel_unit_val = nv->unit_disp_md->GetValStr((void*)unit);
            }
          }
          
          if(unit && tnv->unit_src != unit) {
            if (!inspect_mode) {
              tnv->setUnitSrc(NULL, unit);
            }
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
#endif

/*
 
 Scale Range semantics

  The user can choose for each variable viewed whether to autoscale, and if not, what is the scale.
  Since only one scale can be in effect at once, and the scales are keyed by the display name
  (ex. "act", "r.wt", etc.), the scale system is keyed to this value.

*/

// Add a new NetView object to the frame for the given Network.
NetView* NetView::New(Network* net, T3Panel*& fr) {
  NewViewHelper new_net_view(fr, net, "network");
  // true == Allow only one NetView instances in a frame
  // for a given network.
  if (!new_net_view.isValid(true)) return NULL;

  // create NetView
  NetView* nv = new NetView();
  nv->SetData(net);
  nv->GetMaxSize();
  fr->AddView(nv);

  // make sure we've got it all rendered:
  nv->BuildAll();
  
  new_net_view.showFrame();
  return nv;
}

NetView* Network::FindView() {
  taSigLink* dl = sig_link();
  if(dl) {
    taSigLinkItr itr;
    NetView* el;
    FOR_DLC_EL_OF_TYPE(NetView, el, dl, itr) {
      return el;
    }
  }
  return NULL;
}

void Network::FindViews(T3DataView_List* dv_list) {
  taSigLink* dl = sig_link();
  if(dl) {
    taSigLinkItr itr;
    NetView* el;
    FOR_DLC_EL_OF_TYPE(NetView, el, dl, itr) {
      dv_list->Link(el);
    }
  }
}

NetView* Network::FindMakeView(T3Panel* fr) {
  taSigLink* dl = sig_link();
  if(dl) {
    taSigLinkItr itr;
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

UnitState_cpp* Network::GetViewSrcU() {
  NetView* nv = FindView();
  if(nv) return nv->unit_src;
  return NULL;
}

bool Network::SetViewSrcU(UnitState_cpp* un) {
  NetView* nv = FindView();
  if(!nv) return false;
  nv->setUnitSrc(NULL, un);
  nv->InitDisplay();
  nv->UpdateDisplay();
  return true;
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
  lay_layout = THREE_D;
  prev_lay_layout = lay_layout;
  unit_src = NULL;
  lay_mv = true;
  net_text = false;
  show_iconified = false;
  main_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, .35f);
  state_items_stale = true;
  
  con_type = ANY_CON;
  prjn_starts_with = "";
  unit_disp_md = NULL;
  unit_disp_idx = -1;
  n_counters = 0;
  hist_idx = 0;
  hist_save = true;
  hist_max = 100;
  hist_ff = 5;
  movie_size.x = 720;
  movie_size.y = 720;

  unit_scrng = NULL;
  unit_md_flags = MD_UNKNOWN;
  unit_con_md = false;
  unit_wt_act = false;
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
  last_sel_unit_val = "";
  selected_unit_var = "";
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
  taBase::Own(net_state_text, this);
  taBase::Own(hot_vars, this);
  taBase::Own(ctr_hist, this);
  taBase::Own(ctr_hist_idx, this);
  taBase::Own(max_size, this);
  taBase::Own(max_size2d, this);
  taBase::Own(eff_max_size, this);
  taBase::Own(font_sizes, this);
  taBase::Own(view_params, this);

  ctr_hist_idx.matrix = &ctr_hist;
}

void NetView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) {	// set to initial
    prev_lay_layout = lay_layout;
    taVersion v822(8, 2, 2);
    if(taMisc::loading_version < v822) { // one-time update..
      if(view_params.unit_trans == 0.4f)
        view_params.unit_trans = 0.6f;        // fixed transparency issues
      if(view_params.lay_trans == 0.4f)
        view_params.lay_trans = 0.5f;
    }
    taVersion v832(8, 3, 2);
    if(taMisc::loading_version < v832) { // one-time update
      if (net()) {
        UserDataItem_List* ud_list = net()->user_data_;
        if (ud_list) {
          ud_list->Reset();
        }
      }
    }
  }
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
  net_state_text.CutLinks();
  scale_ranges.CutLinks();
  scale.CutLinks();
  lay_disp_modes.CutLinks();
  inherited::CutLinks();
}

void NetView::CopyFromView(NetView* cp) {
  Copy_(*cp);
  T3DataViewMain::CopyFromViewFrame(cp);
}

void NetView::DataUnitsXForm(taVector3f& pos, taVector3f& size) {
  float tmpz = pos.z;
  float tmpzs = size.z;
  pos.x /= eff_max_size.x;
  size.x /= eff_max_size.x;
  if(lay_layout == THREE_D) {
    pos.z = pos.y / eff_max_size.y;
    size.z = size.y / eff_max_size.y;
    pos.y = tmpz / eff_max_size.z;
    size.y = tmpzs / eff_max_size.z;
  }
  else {
    pos.z = pos.y / eff_max_size.y;
    size.z = size.y / eff_max_size.y;
    pos.y = tmpz;
    size.y = tmpzs;
  }
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
  T3Panel* fr = GetFrame();
  if(!fr) return;
  fr->SetImageSize(x_size, y_size);
  taMisc::ProcessEvents();
  int ctr = 0;
  while(hist_idx > 0) {
    String fnm = fname_stub + taMisc::LeadingZeros(ctr, 5) + ".png";
    fr->SaveImageAs(fnm, taViewer::PNG);
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
  Network* nt = net();
  if(!nt || !nt->IsBuiltIntact()) return;

  Layer* lay = nt->StateLayer(unit_src->own_lay_idx);
  taVector2i pos;
  pos.SetXY(unit_src->pos_x, unit_src->pos_y);
  pos += dir;
  if(pos.x < 0 || pos.y < 0) return;
  UnitState_cpp* nw_u = lay->GetUnitStateFlatXY(nt->net_state, pos.x, pos.y);
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
  GetMembs();
  
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

  if(view_params.prjn_disp != NetViewParams::NONE) {
    FOREACH_ELEM_IN_GROUP(Layer, lay, net()->layers) {
      if(lay->lesioned() || lay->Iconified()) continue;
      for(int pi=0; pi < lay->projections.size; pi++) {
        Projection* prjn = lay->projections[pi];
        if(prjn->MainNotActive() || prjn->from->Iconified() || !prjn->disp) continue;
        PrjnView* pv = new PrjnView();
        pv->SetData(prjn);
        //nn prjns.Add(pv); // this is automatic from the childadding thing
        children.Add(pv);
      }
    }
  }

  BuildAnnotations();
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

void NetView::SigRecvUpdateAfterEdit_impl() {
  UpdateName();
  inherited::SigRecvUpdateAfterEdit_impl();
  // InitDisplay(true);
  // UpdateDisplay();
}

void NetView::SigRecvUpdateAfterEdit_Child_impl(taDataView* chld) {
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
  }
  else {
    return inherited::Dump_QuerySaveMember(md);
  }
}

UnitView* NetView::FindUnitView(UnitState_cpp* unit) {
  UnitView* uv = NULL;
  // todo: we can't do this!?
  // taSigLink* dl = unit->sig_link();
  // if (!dl) return NULL;
  // taSigLinkItr i;
  // FOR_DLC_EL_OF_TYPE(UnitView, uv, dl, i) {
  //   if (uv->GetOwner(&TA_NetView) == this)
  //     return uv;
  // }
  return NULL;
}


UnitState_cpp* NetView::UnitSrc() {
  Network* nt = net();
  if(!nt || !nt->IsBuiltIntact()) {
    return NULL;
  }
  return nt->GetUnitStateFromPath(unit_src_path);
}
  
// this fills a member group with the valid memberdefs from the units and connections
void NetView::GetMembs() {
  if(!net()) return;

  Network* nt = net();
  if(!nt || !nt->IsBuiltIntact()) {
    membs.Reset();
    unit_src = NULL;
    setUnitDispMd(NULL);
    return;
  }

  // try as hard as possible to find a unit to view if nothing selected -- this
  // minimizes issues with history etc
  // if(!unit_src) {
  // always get a new src
  if(unit_src_path.nonempty()) {
    UnitState_cpp* nu = nt->GetUnitStateFromPath(unit_src_path);
    if(nu) setUnitSrc(NULL, nu);
  }
  if(!unit_src && nt->layers.leaves > 0) {
    Layer* lay = nt->layers.Leaf(nt->layers.leaves-1);
    if(lay->n_units_built > 0) {
      setUnitSrc(NULL, lay->GetUnitState(nt->net_state, 0));
    }
  }
  // }

  setUnitDispMd(NULL);
  membs.Reset();

  // there is now only one global unit variables type!
  {
    TypeDef* td = nt->UnitStateType();

    for(int m=0; m<td->members.size; m++) {
      MemberDef* md = td->members.FastEl(m);
      if((md->HasHidden() || md->HasReadOnly() || md->HasOption("NO_VIEW")))
        continue;
      if(md->type->InheritsFrom(&TA_float) || md->type->InheritsFrom(&TA_double)) {
        MemberDef* nmd = md->Clone();
        membs.Add(nmd);       // index now reflects position in list...
        nmd->idx = md->idx;   // so restore it to the orig one
      }
    }
  }

  TypeDef* prv_td = NULL;
  // then, only do the connections if any Unit guys, otherwise we are
  // not built yet, so we leave ourselves empty to signal that
  if (membs.size > 0) {
    FOREACH_ELEM_IN_GROUP(Layer, lay, nt->layers) {
      for(int pi=0; pi < lay->projections.size; pi++) {
        Projection* prjn = lay->projections[pi];
        TypeDef* td = prjn->con_type;
        if(td == prv_td) continue; // don't re-scan!
        prv_td = td;
        int k;
        for (k=0; k<td->members.size; k++) {
          MemberDef* smd = td->members.FastEl(k);
          if(smd->type->InheritsFrom(&TA_float) || smd->type->InheritsFrom(&TA_double)) {
            if((smd->IsInvisible() || smd->HasOption("NO_VIEW")))
              continue;
            String nm = "r." + smd->name;
            if(membs.FindName(nm)==NULL) {
              MemberDef* nmd = smd->Clone();
              nmd->name = nm;
              membs.Add(nmd);
              nmd->idx = smd->idx;
              if(smd->name == "wt") {
                nmd = smd->Clone();
                nmd->name = "r.wt*act";
                nmd->desc = "receiving weight times the sending activation value -- shows the net input contribution from each sending unit";
                membs.Add(nmd);
                nmd->idx = smd->idx;
              }
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
  TypeDef* td = net()->GetTypeDef();
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    if(net()->HasUserData(md->name) && !net()->GetUserDataAsBool(md->name)) continue;
  }
  n_counters = net_state_text.state_items.size;
  
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

void NetView::GetUnitDisplayVals
(UnitGroupView* ugrv, const taVector2i& co, float& val, iColor& col, float& sc_val) {
  sc_val = scale.zero;
  void* base = NULL;
  if(unit_disp_md && unit_md_flags != MD_UNKNOWN)
    val = ugrv->GetUnitDisplayVal(co, base);
  if(!base) {
    col.setRgb(.7f, .7f, .7f); // lt gray
    return;
  }
  GetUnitColor(val, col, sc_val);
}

void NetView::InitDisplay(bool init_panel) {
  // note: init display is kind of an odd bird -- subsumed under Render but also avail
  // independently -- deals with some kinds of changes but not really full structural
  // I guess it is just a "non structural state update" container..
  GetMaxSize();

  if(lay_layout != prev_lay_layout) {
    if(lay_layout == THREE_D)
      main_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, .35f);
    else
      main_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 1.5707963f);
    prev_lay_layout = lay_layout;
  }

  hist_reset_req = false;       // this flag is used to sync history index resetting among
                                // all the history elements in unit groups and network
  InitCtrHist(false);           // non-force version, to make sure there is SOMETHING there
  // doesn't do anything if existing is up-to-date

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
  // TODO: initialize these from a master copy kept as defaults for the application
  // each type, leabra, bp, etc. can have customized defaults
  // TEMP: just set from -1 to 1, with no auto
  sr.auto_scale = false;
  sr.min = -1.0f;
  sr.max =  1.0f;
}

void NetView::Layer_DataUAE(LayerView* lv) {
  // simplest solution is just to call DataUAE on all prns...
  for (int i = 0; i < prjns.size; ++i) {
    PrjnView* pv = (PrjnView*)prjns.FastEl(i);
    pv->SigRecvUpdateAfterEdit_impl();
  }
}

void NetView::NewLayer(int x, int y) {
  Layer* lay = net()->layers.NewEl(1);
  // todo: use rel positioning by default!
  taVector3i nps;
  nps.z = lay->pos_abs.z;
  nps.x = x; nps.y = y;
  lay->SetAbsPos(nps);
  lay->UpdateAfterEdit();
}

void NetView::GetMaxSize() {
  if(!net()) return;
  net()->UpdateLayerGroupGeom();
  max_size = net()->max_disp_size;
  max_size.z -= (max_size.z - 1.0f) / max_size.z; // leave 1 extra layer's worth of room..
  if(view_params.xy_square) {
    max_size.x = MAX(max_size.x, max_size.y);
    max_size.y = max_size.x;
  }

  max_size2d.x = net()->max_disp_size2d.x;
  max_size2d.y = net()->max_disp_size2d.y;
  max_size2d.z = 1.0f;
  if(view_params.xy_square) {
    max_size2d.x = MAX(max_size2d.x, max_size2d.y);
    max_size2d.y = max_size2d.x;
  }

  if(lay_layout == THREE_D) {
    eff_max_size = max_size;
  }
  else {
    eff_max_size = max_size2d;
  }
}

void NetView::OnWindowBind_impl(iT3Panel* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!nvp) {
    nvp = new iViewPanelOfNetwork(this);
    vw->RegisterPanel(nvp);
    if(vw->t3vs) {
      vw->t3vs->Connect_SelectableHostNotifySignal(nvp,
        SLOT(viewWin_NotifySignal(ISelectableHost*, int)) );
    }
  }
}

const iColor NetView::bgColor(bool& ok) const {
  ok = true;
  return scale.background;
}


#ifndef TA_QT3D

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

#endif // TA_QT3D

bool NetView::InitHotVars() {
  if(hot_vars.size > 0) {       // cleanup hot vars if already there
    if(membs.size > 0) {
      for(int i=hot_vars.size-1; i>=0; i--) {
        if(membs.FindNameIdx(hot_vars[i]) < 0) { // not on membs list
          hot_vars.RemoveIdx(i);
        }
      }
    }
    return false;
  }
  // init with programmed hot vars
  MemberDef* md;
  for (int i=0; i < membs.size; i++) {
    md = membs[i];
    if(!md->HasOption("VIEW_HOT")) continue;
    hot_vars.Add(md->name);
  }
  return true;
}

void NetView::Render_pre() {
  InitHotVars();
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

#ifdef TA_QT3D
  setNode(new T3NetNode(NULL, this, show_drag, net_text, show_drag && lay_mv,
			lay_layout == TWO_D));
#else // TA_QT3D
  setNode(new T3NetNode(this, show_drag, net_text, show_drag && lay_mv,
			lay_layout == TWO_D));
  SoMaterial* mat = node_so()->material(); //cache
  mat->diffuseColor.setValue(0.0f, 0.5f, 0.5f); // blue/green
  mat->transparency.setValue(0.5f);
#endif // TA_QT3D

  scale.SetColorSpec(scale.spec);  // Call set to force the saved color to be restored

  if(vw && vw->interactionModeOn()) {
#ifndef TA_QT3D
    SoEventCallback* ecb = new SoEventCallback;
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), UnitGroupView_MouseCB, this);
    node_so()->addChild(ecb);
#endif
  }
  inherited::Render_pre();
}

void NetView::Render_impl() {
  // font properties percolate down to all other elements, unless set there

  Network* nt = net();
  if(!nt->IsBuiltIntact()) {
    unit_src = NULL;
    setUnitDispMd(NULL);
  }
  
  taTransform* ft = transform(true);
  *ft = main_xform;

  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    InitHotVars();
    if(hot_vars.size != vw->dyn_buttons.size) {
      vw->removeAllDynButtons();
      MemberDef* md;
      for (int i=0; i < membs.size; i++) {
        md = membs[i];
        if(hot_vars.FindEl(md->name) < 0) continue;
        vw->addDynButton(md->name, md->desc);
      }
    }
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
  if(unit_disp_md) {
    cap_txt += unit_disp_md->name;
    if (!last_sel_unit_val.empty()) {
      cap_txt += " = " +  last_sel_unit_val;
    }
  }

#ifdef TA_QT3D

#else // TA_QT3D
  if(node_so->shapeDraw())
    node_so->shapeDraw()->lineWidth = view_params.laygp_width;
#endif // TA_QT3D

  node_so->setCaption(cap_txt.chars());

  if(lay_layout == NetView::TWO_D) {
#ifdef TA_QT3D
    node_so->caption->TranslateXLeftTo(QVector3D(-0.5f, 0.0f, 0.5f));
    node_so->caption->RotateDeg(QVector3D(1.0f, 0.0f, 0.0f), -90.0f);
#else // TA_QT3D
    SbVec3f tran(0.0f, 0.0f, 0.05f);
    SbRotation rot(SbVec3f(1.0f, 0.0f, 0.0f), -1.5707963f);
    node_so->transformCaption(rot, tran);
#endif // TA_QT3D
  }

  if (scale.auto_scale) {
    UpdateAutoScale();
    if (nvp) {
      nvp->ColorScaleFromData();
    }
  }
  
  if(net_text) {
    RenderStateValues();
  }
  else {
    vw->ClearStateValues();
  }
  
  if((bool)wt_prjn_lay) {
    // this does all the heavy lifting: projecting into unit wt_prjn
    // if wt_line_thr < 0 then zero intermediates
    net()->ProjectUnitWeights(unit_src, (int)wt_prjn_k_un, (int)wt_prjn_k_gp,
                              wt_line_swt, (wt_prjn_k_un > 0 && wt_line_thr < 0.0f));
  }

  Render_wt_lines();

  inherited::Render_impl();
}

void NetView::RenderStateValues() {
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
        if(hist_idx > 0) {
          int cidx = (ctr_hist_idx.length - hist_idx);
          int midx = ctr_hist_idx.CircIdx(cidx);
          if(ctr_hist.InRange(i, midx)) {
            val = ctr_hist.SafeEl(i, midx);
          }
        }
        else {
          val = md->GetValStr((void*)net());
        }
      }
    }
    else {  // monitor var
      if(hist_idx > 0) {
        int cidx = (ctr_hist_idx.length - hist_idx);
        int midx = ctr_hist_idx.CircIdx(cidx);
        if(ctr_hist.InRange(i, midx)) {
          val = ctr_hist.SafeEl(i, midx);
        }
      }
      else {
        Network* nt = net();
        if (nt) {
          DataTable* monitor_data = &nt->mon_data;
          if (monitor_data) {
            if (monitor_data->rows > 0) {
              DataCol* dc = monitor_data->FindColName(var);
              if (dc && dc->is_matrix) {
                for (int j=0; j<dc->cell_size(); j++) {
                  val += monitor_data->GetValAsVarM(var, -1, j).toString();
                  if (j < dc->cell_size() - 1) {
                    val += " : ";
                  }
                }
              }
              else {
                val = monitor_data->GetValAsVar(var, -1).toString();
                val = val.repl("\n", " ");
              }
            }
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

void NetView::Render_wt_lines() {
  T3NetNode* node_so = this->node_so(); //cache
  if(!node_so) return;

  Network* nt = net();
  if(!nt || !nt->IsBuiltIntact()) return;
  
  bool do_lines = (bool)unit_src && wt_line_disp;
  Layer* src_lay = NULL;
  if(unit_src) {
    src_lay = nt->StateLayer(unit_src->own_lay_idx);
  }
  if(!src_lay || src_lay->Iconified()) do_lines = false;

  taVector3i src_lay_pos;
  if(src_lay)
    src_lay->GetAbsPos(src_lay_pos);

  bool swt = wt_line_swt;
  
  // note: only want layer_rel for ru_pos
  taVector2i ru_pos;
  if(unit_src) {
    ru_pos.SetXY(unit_src->disp_pos_x, unit_src->disp_pos_y);
  }
  taVector3f src;             // source and dest coords
  taVector3f dst;

  float max_xy = MAX(max_size.x, max_size.y);
  float lay_ht = T3LayerNode::height / max_xy;

  float trans = view_params.unit_trans;

  iColor tc;
  float sc_val;

#ifdef TA_QT3D

  T3LineStrip* ls = node_so->wt_lines;
  ls->restart();
  
  if(!do_lines || wt_line_width < 0.0f) {
    return;
  }

  ls->setNodeUpdating(true);
  ls->line_width = MAX(wt_line_width, 0.0f);

  for(int g=0;g<(swt ? unit_src->NSendConGps() : unit_src->NRecvConGps());g++) {
    ConState_cpp* cg = (swt ? unit_src->SendConState(g) : unit_src->RecvConState(g));
    Projection* prjn = cg->prjn;
    if(!prjn || !prjn->IsActive()) continue;
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

    int ru_idx = ls->pointCount();
    ls->moveTo(src);

    for(int i=0;i< cg->size; i++) {
      UnitState_cpp* su = cg->UnState(i,nt);
      float wt = cg->Cn(i, ConState_cpp::WT, nt);
      if(fabsf(wt) < wt_line_thr) continue;

      // note: only want layer_rel for ru_pos
      taVector2i su_pos; su->LayerDispPos(su_pos);
      dst.x = ((float)lay_fr_pos.x + (float)su_pos.x + .5f) / max_size.x;
      dst.z = -((float)lay_fr_pos.y + (float)su_pos.y + .5f) / max_size.y;

      ls->lineTo(dst);
      if(i < cg->size-1) {
        ls->moveToIdx(ru_idx);
      }
      // color
      GetUnitColor(wt, tc, sc_val);
      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      tc.setAlpha(alpha);
      uint32_t pclr = T3Misc::makePackedRGBA(tc);
      ls->addColor(pclr);
      if(i == 0)
        ls->addColor(pclr);
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

    int ru_idx = ls->pointCount();
    ls->moveTo(src);
    bool first = true;

    FOREACH_ELEM_IN_GROUP(Unit, su, wt_prjn_lay->units) {
      float wt = su->wt_prjn;
      if(fabsf(wt) < wt_line_thr) continue;

      taVector2i su_pos; su->LayerDispPos(su_pos);
      dst.x = ((float)wt_prjn_lay_pos.x + (float)su_pos.x + .5f) / max_size.x;
      dst.z = -((float)wt_prjn_lay_pos.y + (float)su_pos.y + .5f) / max_size.y;

      ls->lineTo(dst);
      ls->moveToIdx(ru_idx);

      // color
      GetUnitColor(wt, tc, sc_val);
      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      tc.setAlpha(alpha);
      uint32_t pclr = T3Misc::makePackedRGBA(tc);
      ls->addColor(pclr);
      if(first) {
        ls->addColor(pclr);
        first = false;
      }
    }
  }

  ls->setNodeUpdating(false);

#else // TA_QT3D

  SoIndexedLineSet* ils = node_so->wtLinesSet();
  SoDrawStyle* drw = node_so->wtLinesDraw();
  SoVertexProperty* vtx_prop = node_so->wtLinesVtxProp();

  SoMFVec3f& vertex = vtx_prop->vertex;
  SoMFUInt32& color = vtx_prop->orderedRGBA;
  SoMFInt32& coords = ils->coordIndex;
  SoMFInt32& mats = ils->materialIndex;

  if(!do_lines ||  wt_line_width < 0.0f) {
    vertex.setNum(0);
    color.setNum(0);
    coords.setNum(0);
    mats.setNum(0);
    return;
  }

  drw->style = SoDrawStyleElement::LINES;
  drw->lineWidth = MAX(wt_line_width, 0.0f);
  vtx_prop->materialBinding.setValue(SoMaterialBinding::PER_PART_INDEXED); // part = line segment = same as FACE but likely to be faster to compute line segs?

  // count the number of lines etc
  int n_prjns = 0;
  int n_vtx = 0;
  int n_coord = 0;
  int n_mat = 0;

  const int ncg = (swt ? unit_src->NSendConGps(nt->net_state) : unit_src->NRecvConGps(nt->net_state));
  if(wt_line_width >= 0.0f) {
    for(int g=0; g < ncg; g++) {
      ConState_cpp* cg = (swt ? unit_src->SendConState(nt->net_state, g) :
                          unit_src->RecvConState(nt->net_state, g));
      PrjnState_cpp* pjs = cg->GetPrjnState(nt->net_state);
      Projection* prjn = nt->PrjnFromState(pjs);
      if(!prjn || !prjn->MainIsActive()) continue;
      if(prjn->from->Iconified()) continue;

      n_prjns++;
      int n_con = 0;
      for(int i=0;i< cg->size; i++) {
        float wt = cg->Cn(i,ConState_cpp::WT,nt->net_state);
        if(wt >= wt_line_thr) n_con++;
      }
      n_vtx += 1 + n_con;   // one for recv + senders
      n_coord += 3 * n_con; // start, end -1 for each coord
      n_mat += n_con;       // one per line
    }
  }

  if((bool)wt_prjn_lay && (wt_line_width >= 0.0f)) {
    int n_con = 0;
    for(int ui = 0; ui < wt_prjn_lay->n_units_built; ui++) {
      UNIT_STATE* su = wt_prjn_lay->GetUnitState(nt->net_state, ui);
      if(fabsf(su->wt_prjn) >= wt_line_thr) n_con++;
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

  for(int g=0; g < ncg; g++) {
    ConState_cpp* cg = (swt ? unit_src->SendConState(nt->net_state, g) :
                        unit_src->RecvConState(nt->net_state, g));
    PrjnState_cpp* pjs = cg->GetPrjnState(nt->net_state);
    Projection* prjn = nt->PrjnFromState(pjs);
    if(!prjn || !prjn->MainIsActive()) continue;
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

    for(int i=0;i< cg->size; i++) {
      UnitState_cpp* su = cg->UnState(i,nt->net_state);
      float wt = cg->Cn(i, ConState_cpp::WT, nt->net_state);
      if(fabsf(wt) < wt_line_thr) continue;

      // note: only want layer_rel for ru_pos
      taVector2i su_pos; //su->LayerDispPos(su_pos);
      su_pos.SetXY(su->disp_pos_x, su->disp_pos_y);
      dst.x = ((float)lay_fr_pos.x + (float)su_pos.x + .5f) / max_size.x;
      dst.z = -((float)lay_fr_pos.y + (float)su_pos.y + .5f) / max_size.y;

      coords_dat[cidx++] = ru_idx; coords_dat[cidx++] = v_idx; coords_dat[cidx++] = -1;
      mats_dat[midx++] = c_idx; // one per

      vertex_dat[v_idx++].setValue(dst.x, dst.y, dst.z);

      // color
      GetUnitColor(wt, tc, sc_val);
      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      tc.setAlpha(alpha);
      color_dat[c_idx++] = T3Misc::makePackedRGBA(tc);
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

    for(int ui = 0; ui < wt_prjn_lay->n_units_built; ui++) {
      UNIT_STATE* su = wt_prjn_lay->GetUnitState(nt->net_state, ui);
      float wt = su->wt_prjn;
      if(fabsf(wt) < wt_line_thr) continue;

      taVector2i su_pos; //su->LayerDispPos(su_pos);
      su_pos.SetXY(su->disp_pos_x, su->disp_pos_y);
      dst.x = ((float)wt_prjn_lay_pos.x + (float)su_pos.x + .5f) / max_size.x;
      dst.z = -((float)wt_prjn_lay_pos.y + (float)su_pos.y + .5f) / max_size.y;

      coords_dat[cidx++] = ru_idx; coords_dat[cidx++] = v_idx; coords_dat[cidx++] = -1;
      mats_dat[midx++] = c_idx; // one per

      vertex_dat[v_idx++].setValue(dst.x, dst.y, dst.z);

      // color
      GetUnitColor(wt, tc, sc_val);
      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      tc.setAlpha(alpha);
      color_dat[c_idx++] = T3Misc::makePackedRGBA(tc);
    }
  }

  vertex.finishEditing();
  color.finishEditing();
  coords.finishEditing();
  mats.finishEditing();
#endif // TA_QT3D
  
}

void NetView::SaveImageSVG(const String& svg_fname) {
  T3ExaminerViewer* vw = GetViewer();
  if(!vw) return;
  render_svg = true;
  svg_str = "";
  svg_str << taSvg::Header(vw, this);
  // taSvg::cur_inst->coord_off.y = -1.0f;
  Render_impl();
  RenderAnnoteSvg();
  svg_str << taSvg::Footer();
  render_svg = false;
  svg_str.SaveToFile(svg_fname);
  Render_impl();
}

void NetView::Reset_impl() {
  unit_src = NULL;
  hist_idx = 0;
  ctr_hist_idx.Reset();
  prjns.Reset();
  inherited::Reset_impl();
}

void NetView::ClearCaption() {
  last_sel_unit_val = "";
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

void NetView::setUnitSrc(UnitView* uv, UnitState_cpp* unit) {
  if (unit_src == unit) return; // no change
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
    unit_src_path = net()->GetUnitStatePath(unit_src);
  }
}

void NetView::setUnitDisp(int value) {
  MemberDef* md = membs.PosSafeEl(value);
  setUnitDispMd(md);
}

void NetView::setUnitDispMd(MemberDef* md) {
  switch(con_type) {
  case NetView::ANY_CON:
    prjn_starts_with = "";
    break;
  case NetView::STD_CON:
    prjn_starts_with = "Fm_";
    break;
  case NetView::INHIB_CON:
    prjn_starts_with = "Inhib_";
    break;
  case NetView::CTXT_CON:
    prjn_starts_with = "Ctxt_";
    break;
  case NetView::DEEP_CON:
    prjn_starts_with = "Deep_";
    break;
  case NetView::MARKER_CON:
    prjn_starts_with = "Marker_";
    break;
  }

  if(md == unit_disp_md) return;
  unit_disp_md = md;
  unit_disp_idx = membs.FindEl(md);
  unit_scrng = NULL;
  unit_md_flags = MD_UNKNOWN;
  unit_con_md = false;
  unit_wt_act = false;
  if (!unit_disp_md) return;
  if (unit_disp_md) {
    if (unit_disp_md->type->InheritsFrom(&TA_float))
      unit_md_flags = MD_FLOAT;
    else if (unit_disp_md->type->InheritsFrom(&TA_double))
      unit_md_flags = MD_DOUBLE;
    else if (unit_disp_md->type->InheritsFrom(&TA_int))
      unit_md_flags = MD_INT;
  }
  if(unit_disp_md->name.startsWith("r.") || md->name.startsWith("s.")) {
    unit_con_md = true;
  }
  if(unit_disp_md->name == "r.wt*act") {
    unit_wt_act = true;
  }
  String nm = unit_disp_md->name;
  unit_scrng = scale_ranges.FindName(nm);
  if (unit_scrng == NULL) {
    unit_scrng = (ScaleRange*)(scale_ranges.New(1,&TA_ScaleRange));
    unit_scrng->var_name = nm;
    InitScaleRange(*unit_scrng);
    if(md->HasOption("AUTO_SCALE")) {
      unit_scrng->auto_scale = true;
    }
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
  Network* nt = net();
  if(!nt->IsBuiltIntact()) {
    unit_src = NULL;
    setUnitDispMd(NULL);
    return;
  }
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
  
  for(int i=0; i<net_state_text.state_items.size; i++) {
    NetViewStateItem* item = net_state_text.GetItem(i);
    if (item->net_member) {
      MemberSpace* ms = &td->members;
      MemberDef* md = ms->FindName(item->name);
      if (md) {
        String val = md->GetValStr((void*)net());
        ctr_hist.Set(val, i, eff_hist_idx);
      }
    }
    else {
      Network* nt = net();
      if (nt) {
        DataTable* monitor_data = &nt->mon_data;
        if (monitor_data) {
          if (monitor_data->rows > 0) {
            String val = monitor_data->GetValAsVar(item->name, -1).toString();
            ctr_hist.Set(val, i, eff_hist_idx);
          }
        }
      }
    }
  }
}

void NetView::SigRecvUpdateView_impl() {
  if (!display) return;
  Network* nt = net();
  if(!nt->IsBuiltIntact()) {
    unit_src = NULL;
    setUnitDispMd(NULL);
    return;
  }
  HistFwdAll();			// update to current point in history when updated externally
  UpdateUnitValues();

  T3ExaminerViewer* vw = GetViewer();
  if (vw) {
    if(net_text) {
      RenderStateValues();
    }
    else {
      vw->ClearStateValues();
    }
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
  UnitState_cpp* unit_new = uv->unit;
  setUnitSrc(uv, unit_new);
  InitDisplay();
  UpdateDisplay();
}

void NetView::GetNetTextItems() {
  if(!net()) return;
  if(!state_items_stale) return;
  
  net_state_text.GetItems(net());
  
  state_items_stale = false;
  if (nvp) {
    nvp->GetNetVars(); // make sure the view control panel is updated
  }
  RenderStateValues(); // force the NetText display to update
  UpdatePanel();
}

int NetView::GetNetTextItemWidth(const String& name) {
  return net_state_text.GetItemDisplayWidth(name);
}

void NetView::SetNetTextItemWidth(const String& name, int width) {
  net_state_text.SetItemDisplayWidth(name, width);
  T3ExaminerViewer* vw = GetViewer();
  if (vw) {
    vw->state_labels_inited = false;
    RenderStateValues();
  }
  UpdatePanel();
}

void NetView::MonitorUpdate() {
  state_items_stale = true;  // some net monitor item change
  GetNetTextItems();
}

void NetView::GetUnitMonitorVar(const String& variable) {
  selected_unit_var = variable;
}

void NetView::MonitorUnit(Layer* layer, int unit, const String& var) {
  if (net() && layer && unit != -1 && var.nonempty()) {
    NetMonItem* mon_item = net()->monitor.AddUnit(layer, unit, var);
  }
}

String NetView::GetArgForCompletion(const String& method, const String& arg) {
  if (method == "GetUnitMonitorVar") {
    return "layer";
  }
  return "";
}

void NetView::GetArgCompletionList(const String& method, const String& arg, const String_Array& arg_values, taBase* arg_obj, const String& cur_txt, Completions& completions) {
  if (method == "GetUnitMonitorVar") {
    TypeDef* td = taMisc::FindTypeName("UnitState_cpp");
    if (!td) {
      return;
    }
    MemberSpace mbr_space = td->members;
    for (int i = 0; i < mbr_space.size; ++i) {
      MemberDef* md = mbr_space.FastEl(i);
      String category = md->OptionAfter("CAT_");
      if (category == "Statistic" || category == "Counter"
          || category == "Bias" || category == "Activation" || category == "Learning") {
        if (!md->HasOption("HIDDEN")) {
          completions.member_completions.Link(md);
        }
      }
    }
  }
}

