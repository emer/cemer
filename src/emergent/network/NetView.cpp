// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include <taMisc>

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

void UnitGroupView_MouseCB(void* userData, SoEventCallback* ecb) {
  NetView* nv = (NetView*)userData;
  T3Panel* fr = nv->GetFrame();
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
        int xp = (int)((pt[0] * tnv->eff_max_size.x) / disp_scale);
        int yp = (int)(-(pt[2] * tnv->eff_max_size.y) / disp_scale);

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
  lay_layout = THREE_D;
  prev_lay_layout = lay_layout;
  lay_mv = true;
  net_text = true;
  show_iconified = false;
  main_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, .35f);
  net_text_xform.translate.SetXYZ(1.0f, 0.0f, -0.5f); // right mid
  //  net_text_xform.translate.SetXYZ(0.0f, 1.0f, -1.0f); // start at top back
  net_text_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 0.5f * taMath_float::pi); // start at right mid
  net_text_xform.scale = 0.5f;
  net_text_rot = -90.0f;

  unit_con_md = false;
  con_type = ANY_CON;
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
  taBase::Own(max_size2d, this);
  taBase::Own(eff_max_size, this);
  taBase::Own(font_sizes, this);
  taBase::Own(view_params, this);
  taBase::Own(net_text_xform, this);

  ctr_hist_idx.matrix = &ctr_hist;
}

void NetView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) {	// set to initial
    prev_lay_layout = lay_layout;
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
  pos /= eff_max_size;
  size /= eff_max_size;
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
         || prjn->from->Iconified() || prjn->off || !prjn->disp) continue;
      PrjnView* pv = new PrjnView();
      pv->SetData(prjn);
      //nn prjns.Add(pv); // this is automatic from the childadding thing
      children.Add(pv);
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
  InitDisplay(true);
  UpdateDisplay();
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
  } else
    return inherited::Dump_QuerySaveMember(md);
}

UnitView* NetView::FindUnitView(Unit* unit) {
  UnitView* uv = NULL;
  taSigLink* dl = unit->sig_link();
  if (!dl) return NULL;
  taSigLinkItr i;
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
          if(md->type->IsPtrPtr()) continue; // only one level of pointer tolerated
          TypeDef* nptd;
          if(md->type->IsPointer()) {
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

  if(lay_layout != prev_lay_layout) {
    if(lay_layout == THREE_D)
      main_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, .35f);
    else
      main_xform.rotate.SetXYZR(1.0f, 0.0f, 0.0f, 1.5707963f);
    prev_lay_layout = lay_layout;
  }

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
    pv->SigRecvUpdateAfterEdit_impl();
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

  setNode(new T3NetNode(this, show_drag, net_text, show_drag && lay_mv,
			lay_layout == TWO_D));
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

  if(lay_layout == NetView::TWO_D) {
    SbVec3f tran(0.0f, 0.0f, 0.05f);
    SbRotation rot(SbVec3f(1.0f, 0.0f, 0.0f), -1.5707963f);
    node_so->transformCaption(rot, tran);
  }

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
    if(md->type->InheritsFrom(&TA_taString) || md->type->IsEnum()) {
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
    T3Panel* fr = GetFrame();
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
      if((md->type->InheritsFrom(&TA_taString) || md->type->IsEnum())) {
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

  Network* nt = net();

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
        float wt = (swt ? ((SendCons*)cg)->Cn(i,BaseCons::WT,nt) :
                    ((RecvCons*)cg)->Cn(i, BaseCons::WT, nt));
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
      Unit* su = (swt ? ((SendCons*)cg)->Un(i,nt) : ((RecvCons*)cg)->Un(i,nt));
      float wt = (swt ? ((SendCons*)cg)->Cn(i, BaseCons::WT, nt) :
                  ((RecvCons*)cg)->Cn(i, BaseCons::WT, nt));
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

void NetView::SigRecvUpdateView_impl() {
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

