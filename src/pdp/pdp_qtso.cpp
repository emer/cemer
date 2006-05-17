// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.




#include "pdp_qtso.h"
#include "pdpshell.h"
#include "ta_qt.h"
#include "ta_qtdialog.h"
#include "netstru_qtso.h"
#include "program.h"

#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoTransform.h>


//////////////////////////
//  taiProgVarType	//
//////////////////////////

int taiProgVarType::BidForType(TypeDef* td) {
  if (td->InheritsFrom(TA_ProgVar)) 
    return (inherited::BidForType(td) +1);
  else  return 0;
}

taiData* taiProgVarType::GetDataRepInline_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) 
{
  //note: we use a static New function because of funky construction-time virtual functions
  taiProgVarBase* rval;
  if (typ->InheritsFrom(TA_ObjectProgVar))
    rval = taiObjectProgVar::New(typ, host_, par, gui_parent_, flags_);
  else if (typ->InheritsFrom(TA_EnumProgVar))
    rval = taiEnumProgVar::New(typ, host_, par, gui_parent_, flags_);
  else 
    rval = taiProgVar::New(typ, host_, par, gui_parent_, flags_);
  return rval;
}



//////////////////////////////////
//	taiSpecMember		//
//////////////////////////////////

int taiSpecMember::BidForMember(MemberDef* md, TypeDef* td) {
  if((td->InheritsFrom(TA_BaseSpec) || td->InheritsFrom(TA_BaseSubSpec))
     && !(md->HasOption("NO_INHERIT")))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

// put some check here for if the spec base has no spec parent
//  (((TA_BaseSpec *) dlg->cur_base)->owner->GetOwner(dlg->typ) == NULL))

//   if((dlg != NULL) && (dlg->cur_base != NULL) && (dlg->typ != NULL) &&
//      dlg->typ->InheritsFrom(TA_BaseSpec) &&
//      (((BaseSpec*)dlg->cur_base)->GetOwner(&TA_BaseSpec) == NULL))
//   {

bool taiSpecMember::NoCheckBox(IDataHost* host_) const {
  void* base = host_->Base();
  TypeDef* typ = host_->GetBaseTypeDef();
  if((host_ == NULL) || (base == NULL) || (typ == NULL))
    return true;

  if(typ->InheritsFrom(TA_BaseSpec_MGroup))
    return false;		// always use a check box for these..

  if(typ->InheritsFrom(TA_BaseSpec)) {
    BaseSpec* bs = (BaseSpec*)base;
    if(bs->FindParent() != NULL)
      return false;		// owner has a parent spec, needs a box
    return true;		// no owner, no box..
  } else if(typ->InheritsFrom(TA_BaseSubSpec)) {
    BaseSubSpec* bs = (BaseSubSpec*)base;
    if(bs->FindParent() != NULL)
      return false;		// owner has a parent spec, needs a box
    return true;		// no owner, no box..
  } else {
    TAPtr tap = (TAPtr)base;
    BaseSpec* bs = (BaseSpec*)tap->GetOwner(&TA_BaseSpec); // find an owner..
    if(bs == NULL)
      return true;		// no owner, no box..
    if(bs->FindParent() != NULL)
      return false;		// owner is owned by a spec, needs a box..
  }
  return true;			// default is to not have box...
}


taiData* taiSpecMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent, int flags_) {
  if (NoCheckBox(host_)) {
    taiData* rdat;
    if (m_sub_types != NULL)
      rdat = sub_types()->GetDataRep(host_, par, gui_parent);
    else
      rdat = taiMember::GetDataRep_impl(host_, par, gui_parent, flags_);
    return rdat;
  } else {
    taiPlusToggle* rval = new taiPlusToggle(NULL, host_, par, gui_parent);
    rval->InitLayout();
    taiData* rdat;
    if (m_sub_types)
      rdat = sub_types()->GetDataRep(host_, rval, rval->GetRep(), NULL, flags_);
    else
      rdat = taiMember::GetDataRep_impl(host_, rval, rval->GetRep(), flags_);
    rval->data = rdat;
    rval->AddChildWidget(rdat->GetRep());
    rval->EndLayout();
    return rval;
  }
}

void taiSpecMember::GetImage_impl(taiData* dat, const void* base) {
  IDataHost* host_ = dat->host;
  if (NoCheckBox(host_)) {
    if (m_sub_types != NULL)
      sub_types()->GetImage(dat,base);
    else
      taiMember::GetImage_impl(dat, base);
  } else {
    taiPlusToggle* rval = (taiPlusToggle*)dat;
    if (m_sub_types != NULL)
      sub_types()->GetImage(rval->data,base);
    else
      taiMember::GetImage_impl(rval->data, base);
    bool uniq = false;
    if(typ->InheritsFrom(TA_BaseSpec)) {
      BaseSpec* es = (BaseSpec*)base;
      uniq = es->GetUnique(mbr->idx);
    }
    else {
      BaseSubSpec* es = (BaseSubSpec*)base;
      uniq = es->GetUnique(mbr->idx);
    }
    rval->GetImage(uniq);
    if(uniq)
      rval->orig_val = "true";
    else
      rval->orig_val = "false";
  }
}

void taiSpecMember::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
  IDataHost* host_ = dat->host;
  if(NoCheckBox(host_)) {
    if (m_sub_types != NULL)
      sub_types()->GetMbrValue(dat, base, first_diff);
    else
      taiMember::GetMbrValue(dat, base, first_diff);
    return;
  }

  taiPlusToggle* rval = (taiPlusToggle*)dat;
  if(typ->InheritsFrom(TA_BaseSpec)) {
    BaseSpec* es = (BaseSpec*)base;
    es->SetUnique(mbr->idx,rval->GetValue());
//    es->UpdateMbr(mbr->idx);
  }
  else {
    BaseSubSpec* es = (BaseSubSpec*)base;
    es->SetUnique(mbr->idx,rval->GetValue());
//    es->UpdateMbr(mbr->idx);
  }

  CmpOrigVal(dat, base, first_diff);
  if (m_sub_types != NULL)
    sub_types()->GetMbrValue(rval->data, base, first_diff);
  else
    taiMember::GetMbrValue(rval->data, base, first_diff);
}

void taiSpecMember::CmpOrigVal(taiData* dat, void* base, bool& first_diff) {
  if((taMisc::record_script == NULL) || !typ->InheritsFrom(TA_taBase))
    return;
  String new_val;
  bool uniq = false;
  if(typ->InheritsFrom(TA_BaseSpec)) {
    BaseSpec* es = (BaseSpec*)base;
    uniq = es->GetUnique(mbr->idx);
  }
  else {
    BaseSubSpec* es = (BaseSubSpec*)base;
    uniq = es->GetUnique(mbr->idx);
  }
  if(uniq)
    new_val = "true";
  else
    new_val = "false";
  if(dat->orig_val == new_val)
    return;
  if(first_diff)
    StartScript(base);
  first_diff = false;

  *taMisc::record_script << "  ths->SetUnique(\"" << mbr->name << "\", "
			   << new_val << ");" << endl;
}


//////////////////////////////////
//   taiProgVarBase		//
//////////////////////////////////

taiProgVarBase::taiProgVarBase(TypeDef* typ_, IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags)
: inherited(typ_, host_, par, gui_parent_, flags)
{
  m_changing = 0;
}

taiProgVarBase::~taiProgVarBase() {
}

void taiProgVarBase::Constr(QWidget* gui_parent_) { 
  QWidget* rep_ = new QWidget(gui_parent_);
  SetRep(rep_);
  rep_->setMaximumHeight(taiM->max_control_height(defSize()));
  if (host != NULL) {
    SET_PALETTE_BACKGROUND_COLOR(rep_,*(host->colorOfCurRow()));
  }
  InitLayout();
  Constr_impl(gui_parent_, (mflags & flgReadOnly));
  EndLayout();
}

void taiProgVarBase::Constr_impl(QWidget* gui_parent_, bool read_only_) { 
  QWidget* rep_ = GetRep();
  QLabel* lbl = new QLabel("ignore", rep_);
  AddChildWidget(lbl, taiM->hsep_c);

  tglIgnore = new taiToggle(&TA_bool, host, this, rep_, mflags & flgReadOnly);
  AddChildWidget(tglIgnore->GetRep(), taiM->hsep_c);
  
  lbl = new QLabel("name", rep_);
  AddChildWidget(lbl, taiM->hsep_c);

  fldName = new taiField(&TA_taString, host, this, rep_, mflags & flgReadOnly);
  AddChildWidget(fldName->GetRep(), taiM->hsep_c);
}

void taiProgVarBase::GetImage(const ProgVar* var) {
  tglIgnore->GetImage(var->ignore);
  fldName->GetImage(var->name);
}

void taiProgVarBase::GetValue(ProgVar* var) const {
  var->ignore = tglIgnore->GetValue();
  var->name = fldName->GetValue();
}
  

//////////////////////////////////
//   taiProgVar		//
//////////////////////////////////

taiProgVar* taiProgVar::New(TypeDef* typ_, IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags)
{
  taiProgVar* rval = new taiProgVar(typ_, host_, par, gui_parent_, flags);
  rval->Constr(gui_parent_);
  return rval;
}

taiProgVar::taiProgVar(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags)
: inherited(typ_, host_, par, gui_parent_, flags)
{
}

taiProgVar::~taiProgVar() {
}

void taiProgVar::Constr_impl(QWidget* gui_parent_, bool read_only_) { 
  inherited::Constr_impl(gui_parent_, read_only_);
  QWidget* rep_ = GetRep();
  vfVariant = new taiVariant(host, this, rep_, 
    (mflags & flgReadOnly) | taiVariantBase::flgNoPtr | taiVariantBase:: flgNoBase);
  AddChildWidget(vfVariant->GetRep(), taiM->hsep_c);
}

void taiProgVar::GetImage(const ProgVar* var) {
  inherited::GetImage(var);
  vfVariant->GetImage(var->value);
}

void taiProgVar::GetValue(ProgVar* var) const {
  inherited::GetValue(var);
  vfVariant->GetValue(var->value);
}
  

//////////////////////////////////
//   taiEnumProgVar		//
//////////////////////////////////

taiEnumProgVar* taiEnumProgVar::New(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags)
{
  taiEnumProgVar* rval = new taiEnumProgVar(typ_, host_, par, gui_parent_, flags);
  rval->Constr(gui_parent_);
  return rval;
}

taiEnumProgVar::taiEnumProgVar(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags)
: inherited(typ_, host_, par, gui_parent_, flags)
{
}

taiEnumProgVar::~taiEnumProgVar() {
}

void taiEnumProgVar::Constr_impl(QWidget* gui_parent_, bool read_only_) { 
  inherited::Constr_impl(gui_parent_, read_only_);
  QWidget* rep_ =  GetRep();
  QLabel* lbl = new QLabel("enum type",rep_);
  AddChildWidget(lbl, taiM->hsep_c);
  thEnumType = new taiTypeHier(taiActions::popupmenu, taiMisc::defFontSize, 
    &TA_taBase, host, this, rep_, (mflags & flgReadOnly));
  thEnumType->enum_mode = true;
  thEnumType->GetMenu();
  AddChildWidget(thEnumType->GetRep(), taiM->hsep_c);
  lbl = new QLabel("enum value",rep_);
  AddChildWidget(lbl, taiM->hsep_c);
  cboEnumValue = new taiComboBox(true, NULL, host, this, rep_, (mflags & flgReadOnly));
  AddChildWidget(cboEnumValue->GetRep(), taiM->hsep_c);
}

void taiEnumProgVar::DataChanged_impl(taiData* chld) {
  inherited::DataChanged_impl(chld);
  if (m_changing > 0) return;
  ++m_changing;
  if (chld == thEnumType) {
    cboEnumValue->SetEnumType(thEnumType->GetValue());
    //note: prev value of value may no longer be a valid enum value!
  }
  --m_changing;
}

void taiEnumProgVar::GetImage(const ProgVar* var_) {
  inherited::GetImage(var_);
  const EnumProgVar* var = (const EnumProgVar*)var_;
  thEnumType->GetImage(var->enum_type);
  cboEnumValue->SetEnumType(var->enum_type);
  cboEnumValue->GetEnumImage(var->value.toInt());
}

void taiEnumProgVar::GetValue(ProgVar* var_) const {
  inherited::GetValue(var_);
  EnumProgVar* var = (EnumProgVar*)var_;
  var->enum_type = thEnumType->GetValue();
  int val;
  cboEnumValue->GetEnumValue(val); // 0 if no valid type
  var->value = val;
}
  

//////////////////////////////////
//   taiObjectProgVar		//
//////////////////////////////////

taiObjectProgVar* taiObjectProgVar::New(TypeDef* typ_, IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags)
{
  taiObjectProgVar* rval = new taiObjectProgVar(typ_, host_, par, gui_parent_, flags);
  rval->Constr(gui_parent_);
  return rval;
}

taiObjectProgVar::taiObjectProgVar(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags)
: inherited(typ_, host_, par, gui_parent_, flags)
{
}

taiObjectProgVar::~taiObjectProgVar() {
}

void taiObjectProgVar::Constr_impl(QWidget* gui_parent_, bool read_only_) { 
  inherited::Constr_impl(gui_parent_, read_only_);
  QWidget* rep_ =  GetRep();
  QLabel* lbl = new QLabel("val_type",rep_);
  AddChildWidget(lbl, taiM->hsep_c);
  thValType = new taiTypeHier(taiActions::popupmenu, taiMisc::defFontSize, 
    &TA_taBase, host, this, rep_, (mflags & flgReadOnly));
  thValType->GetMenu();
  AddChildWidget(thValType->GetRep(), taiM->hsep_c);
  //TODO: make_new and init token
  lbl = new QLabel("make new",rep_);
  AddChildWidget(lbl, taiM->hsep_c);
  chkMakeNew = new taiToggle(&TA_bool, host, this, rep_, (mflags & flgReadOnly));
  AddChildWidget(chkMakeNew->GetRep(), taiM->hsep_c);
  lblObjectValue = new QLabel("value",rep_);
  AddChildWidget(lblObjectValue, taiM->hsep_c);
  tkObjectValue = new taiToken(taiActions::popupmenu, taiMisc::defFontSize, 
     thValType->typ, host, this, rep_, ((mflags & flgReadOnly) | flgNullOk));
  AddChildWidget(tkObjectValue->GetRep(), taiM->hsep_c);
}

void taiObjectProgVar::DataChanged_impl(taiData* chld) {
  inherited::DataChanged_impl(chld);
  if (m_changing > 0) return;
  ++m_changing;
  if (chld == chkMakeNew) {
    MakeNew_Setting(chkMakeNew->GetValue());
  } else if (chld == thValType) {
    tkObjectValue->SetTypeScope(thValType->GetValue());
    // previous token may no longer be in scope!
  }
  --m_changing;
}

void taiObjectProgVar::GetImage(const ProgVar* var_) {
  inherited::GetImage(var_);
  const ObjectProgVar* var = (const ObjectProgVar*)var_;
  thValType->GetImage(var->val_type);
  chkMakeNew->GetImage(var->make_new);
  MakeNew_Setting(var->make_new);
  if (var->make_new) {
    //maybe should initialize the token with NULL???
  } else {
    tkObjectValue->SetTypeScope(var->val_type, NULL); // no scope
    tkObjectValue->GetImage(var->value.toBase(), NULL);
  }
}

void taiObjectProgVar::GetValue(ProgVar* var_) const {
  inherited::GetValue(var_);
  ObjectProgVar* var = (ObjectProgVar*)var_;
  var->val_type = thValType->GetValue(); 
  var->make_new = chkMakeNew->GetValue();
  if (var->make_new) {
    var->value.setBase(NULL);
  } else {
    var->value.setBase(tkObjectValue->GetValue());
  }
}
  
void taiObjectProgVar::MakeNew_Setting(bool value) {
  if (value) {
    tkObjectValue->GetRep()->setVisible(false);
    lblObjectValue->setVisible(false);
  } else {
    lblObjectValue->setVisible(true);
    tkObjectValue->GetRep()->setVisible(true);
  }
}



//////////////////////////
//   ipdpDataViewer	//
//////////////////////////

ipdpDataViewer::ipdpDataViewer(void* root_, TypeDef* typ_, pdpDataViewer* viewer_,
  QWidget* parent)
:iT3DataViewer(root_, typ_, (T3DataViewer*)viewer_, parent)
{
}

ipdpDataViewer::~ipdpDataViewer()
{
}

/*void iNetworkViewer::SetActionsEnabled_impl() {
  //todo
} */


//////////////////////////
//	pdpDataViewer	//
//////////////////////////

pdpDataViewer* pdpDataViewer::New(ProjectBase* proj_) {
  pdpDataViewer* rval = new pdpDataViewer();
  proj_->AddDataView(rval);
  return rval;
}

void pdpDataViewer::Initialize() {
  data_base = &TA_ProjectBase;
}

void pdpDataViewer::Destroy() {
}

void pdpDataViewer::Constr_Window_impl() {
  m_window = new ipdpDataViewer(proj(), proj()->GetTypeDef(), this);
//note:multi is default  viewer_win()->sel_mode = iT3DataViewer::SM_MULTI;
}

////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//     Project Viewer!		//
//////////////////////////////////
/*
#include <iv_misc/dastepper.h>
#include <pdp/procs_extra.h>

#include <ta/enter_iv.h>
#include <InterViews/brush.h>
#include <InterViews/color.h>
#include <InterViews/tformsetter.h>
#include <InterViews/transformer.h>
#include <InterViews/action.h>
#include <InterViews/box.h>
#include <InterViews/layout.h>
#include <InterViews/window.h>
#include <InterViews/canvas.h>
#include <InterViews/session.h>
#include <InterViews/event.h>
#include <InterViews/style.h>
#include <InterViews/telltale.h>
#include <InterViews/geometry.h>
#include <InterViews/patch.h>
#include <InterViews/background.h>
#include <InterViews/cursor.h>
#include <InterViews/bitmap.h>
#include <InterViews/deck.h> // for hilit buttons
#include <InterViews/font.h>
#include <InterViews/polyglyph.h> // for height field neighbors
#include <IV-look/kit.h>
#include <OS/file.h>
#include <OS/math.h>
#include <ta/leave_iv.h>

#include <limits.h>
#include <float.h>
#include <unistd.h>

const float PROJV_INITIAL_X = 400.0f;
const float PROJV_INITIAL_Y = 100.0f;
const float PROJV_MIN_Y = 34.0f;
#ifndef CYGWIN
const float PROJV_BUTTONS_Y = 234.f; // size of window w/buttons
#else
const float PROJV_BUTTONS_Y = 264.f; // size of window w/buttons
#endif
const float PROJV_X_MARG = 8.0f;
const float PROJV_Y_MARG = 8.0f;
const float PROJV_OBJ_HEIGHT = 1.3f;
const float PROJV_OBJ_Y_SPC = 4.0f; // space between objs
const float PROJV_OBJ_X_SPC = 40.0f; // space between objs
const float PROJV_INDENT = 5.0f; // indent processes
const float PROJV_LINK_OFFSET = 4.0f; // indent processes
const float PROJV_TXT_SPC = 3.0f; // text spacing
const int   PROJV_SPEC_COLS = 3; // number of spec columns
const float PROJV_SPEC_DEPTH = 7.0f; // max number of levels of sub-spec embeddings anticipated

//////////////////////////////////
//	   Project_G		//
//////////////////////////////////

Project_G::Project_G(Project* n) {
  proj = n;
  defbrush = new ivBrush(BRUSH_SOLID,DEFAULT_BRUSH_WIDTH);
  ivResource::ref(defbrush);

  widebrush = new ivBrush(BRUSH_SOLID,2.0f);
  ivResource::ref(widebrush);

  widedashbrush = new ivBrush(BRUSH_DASH,2.0f);
  ivResource::ref(widedashbrush);

  border = new ivColor(0.0, 0.0, 0.0, 1.0);
  ivResource::ref(border);

  reinit_display = false;
  taBase::Own(selectgroup, proj); // these are conceptually owned by the Project..
  taBase::Own(pickgroup, proj);	  // ditto
  tx = NULL;
}


Project_G::~Project_G() {
  // Reset(); // do not call this since there is a flush in Reset();
  selectgroup.RemoveAll(); // just do these two instead
  pickgroup.RemoveAll();

  ivResource::unref(defbrush); defbrush = NULL;
  ivResource::unref(widebrush); widebrush = NULL;
  ivResource::unref(widedashbrush); widedashbrush = NULL;
  ivResource::unref(border);	border = NULL;
}

void Project_G::Reset() {
  remove_all();
  selectgroup.RemoveAll();
  pickgroup.RemoveAll();
  ivResource::flush(); // clear our all defered unrefs.
}

void Project_G::SpecBuild(BaseSpec* spec, int xcol, float cx, float& cy) {
  // does a spec and all of its sub-specs
  float cur_cx = cx + xcol * (obj_size.x + PROJV_SPEC_DEPTH * PROJV_INDENT);
  cy -= obj_size.y + PROJV_OBJ_Y_SPC;
  SpecObj_G* prob = new SpecObj_G(spec, this);
  append_(prob);
  prob->translate(cur_cx, cy);
  const ivColor* objclr = proj->GetObjColor(spec->GetTypeDef());
  if(objclr != NULL) prob->fill(objclr);
  if(spec->children.leaves > 0) {
    cy -= obj_size.y;
    GroupObj_G* grob = new GroupObj_G(&(spec->children), this);
    append_(grob);
    grob->translate(cur_cx, cy);
    grob->fill(proj->the_colors.FastEl(Project::GEN_GROUP)->color);
    ProjViewState* gp_vst = proj->view_states.FindName(spec->children.GetPath());
    if(gp_vst != NULL) {
      if(gp_vst->iconify) {
	grob->fill(proj->the_colors.FastEl(Project::BACKGROUND)->color);
	gp_vst->in_use = true;
      }
    }
    if(!((gp_vst != NULL) && gp_vst->iconify)) {
      taLeafItr i;
      BaseSpec* sub;
      FOR_ITR_EL(BaseSpec, sub, spec->children., i) {
	SpecBuild(sub, xcol, cx + PROJV_INDENT, cy);
      }
    }
  }
}

void Project_G::Build() {
  Reset();			// clear out exiting layers here

  proj->view_states.UnSetInUse();

  ivAllotment& ax = _a.x_allotment();
  ivAllotment& ay = _a.y_allotment();
  span.x = ax.span();		// overall size of display
  span.y = ay.span();

  ivFontBoundingBox bbox;
  proj->view_font.fnt->font_bbox(bbox);
  float fy = bbox.ascent() + bbox.descent();
  float fx = proj->view_font.fnt->width('m');
  obj_size.x = (float)(proj->obj_width) * fx;
  obj_size.y = PROJV_OBJ_HEIGHT * fy;
  float cx = PROJV_X_MARG;
  max_size.Initialize();

  float st_cx = cx;
  float cy = span.y;
  taGroup_impl* gp;
  int i;

  if(owner->spec_mode) {
    int xcol = 0;
    int max_xcol = 0;
    float total_max_cy = cy;
    float st_cy = cy;
    FOR_ITR_GP(taGroup_impl, gp, proj->specs., i) {
      ProjViewState* gp_vst = NULL;
      bool in_gp = false;
      if(gp != &(proj->specs)) {
	cy -= obj_size.y + PROJV_OBJ_Y_SPC;
	total_max_cy = MIN(total_max_cy, cy);
	GroupObj_G* grob = new GroupObj_G(gp, this);
	append_(grob);
	grob->translate(cx + xcol * (obj_size.x + PROJV_SPEC_DEPTH * PROJV_INDENT), cy);
	grob->fill(proj->the_colors.FastEl(Project::GEN_GROUP)->color);
	cx = st_cx + PROJV_INDENT;
	in_gp = true;
	gp_vst = proj->view_states.FindName(gp->GetPath());
	if(gp_vst != NULL) {
	  if(gp_vst->iconify) {
	    grob->fill(proj->the_colors.FastEl(Project::BACKGROUND)->color);
	    gp_vst->in_use = true;
	  }
	}
      }
      st_cy = cy;
      int j;
      for(j=0;j<gp->size;j++) {
	BaseSpec* spec = (BaseSpec*)gp->FastEl_(j);
	if((gp_vst != NULL) && gp_vst->iconify) {
	  ProjViewState* vst = proj->view_states.FindName(spec->GetPath());
	  if(vst != NULL) vst->in_use = true;
	  continue;
	}
	float cur_cy = cy;
	SpecBuild(spec, xcol, cx, cur_cy);
	total_max_cy = MIN(total_max_cy, cur_cy);
	if(!in_gp) {
	  xcol++;
	  max_xcol = MAX(max_xcol, xcol);
	  if(xcol > PROJV_SPEC_COLS) {
	    xcol = 0;
	    cy = total_max_cy - obj_size.y;
	  }
	}
	else {
	  cy = cur_cy;
	}
      }
      if(in_gp) {
	cy = st_cy + obj_size.y + PROJV_OBJ_Y_SPC;
	xcol++;
	max_xcol = MAX(max_xcol, xcol);
	if(xcol > PROJV_SPEC_COLS) {
	  xcol = 0;
	  cy = total_max_cy - obj_size.y;
	}
      }
      cx = st_cx;
    }
    // now add any empty groups!
    cy -= obj_size.y + PROJV_OBJ_Y_SPC;
    int i;
    for(i=0;i<proj->specs.gp.size;i++) {
      gp = (taGroup_impl*)proj->specs.gp.FastEl(i);
      if(gp->leaves != 0) continue;
      GroupObj_G* grob = new GroupObj_G(gp, this);
      append_(grob);
      grob->translate(cx + xcol * (obj_size.x + PROJV_SPEC_DEPTH * PROJV_INDENT), cy);
      grob->fill(proj->the_colors.FastEl(Project::GEN_GROUP)->color);
      xcol++;
      max_xcol = MAX(max_xcol, xcol);
      if(xcol > PROJV_SPEC_COLS) {
	xcol = 0;
	cy = total_max_cy - obj_size.y;
      }
    }

    cx += obj_size.x + PROJV_OBJ_X_SPC;
    max_size.y = span.y - total_max_cy;
    max_size.x = st_cx + max_xcol * (obj_size.x + PROJV_SPEC_DEPTH * PROJV_INDENT);
    // protect all the other view states from being removed
    int vsi;
    for(vsi=0;vsi<proj->view_states.size;vsi++) {
      ProjViewState* vst = proj->view_states.FastEl(vsi);
      if(!vst->name.contains(".specs"))
	vst->in_use = true;
    }
  }
  else {
    FOR_ITR_GP(taGroup_impl, gp, proj->networks., i) {
      ProjViewState* gp_vst = NULL;
      if(gp != &(proj->networks)) {
	cy -= obj_size.y + PROJV_OBJ_Y_SPC;
	GroupObj_G* grob = new GroupObj_G(gp, this);
	append_(grob);
	grob->translate(cx, cy);
	grob->fill(proj->the_colors.FastEl(Project::GEN_GROUP)->color);
	cx = st_cx + PROJV_INDENT;
	gp_vst = proj->view_states.FindName(gp->GetPath());
	if(gp_vst != NULL) {
	  if(gp_vst->iconify) {
	    gp_vst->in_use = true;
	    grob->fill(proj->the_colors.FastEl(Project::BACKGROUND)->color);
	  }
	}
      }
      int j;
      for(j=0;j<gp->size;j++) {
	Network* net = (Network*)gp->FastEl_(j);
	if((gp_vst != NULL) && gp_vst->iconify) {
	  ProjViewState* vst = proj->view_states.FindName(net->GetPath());
	  if(vst != NULL) vst->in_use = true;
	  continue;
	}
	cy -= obj_size.y + PROJV_OBJ_Y_SPC;
	ProjObj_G* prob = new ProjObj_G(net, this);
	append_(prob);
	prob->translate(cx, cy);
	prob->fill(proj->the_colors.FastEl(Project::NETWORK)->color);
      }
      cx = st_cx;
    }
    cy -= PROJV_OBJ_Y_SPC;	// stack env below nets w/ extra space
    st_cx = cx = PROJV_X_MARG;
    FOR_ITR_GP(taGroup_impl, gp, proj->environments., i) {
      ProjViewState* gp_vst = NULL;
      if(gp != &(proj->environments)) {
	cy -= obj_size.y + PROJV_OBJ_Y_SPC;
	GroupObj_G* grob = new GroupObj_G(gp, this);
	append_(grob);
	grob->translate(cx, cy);
	grob->fill(proj->the_colors.FastEl(Project::GEN_GROUP)->color);
	cx = st_cx + PROJV_INDENT;
	gp_vst = proj->view_states.FindName(gp->GetPath());
	if(gp_vst != NULL) {
	  if(gp_vst->iconify) {
	    grob->fill(proj->the_colors.FastEl(Project::BACKGROUND)->color);
	    gp_vst->in_use = true;
	  }
	}
      }
      int j;
      for(j=0;j<gp->size;j++) {
	Environment* env = (Environment*)gp->FastEl_(j);
	if((gp_vst != NULL) && gp_vst->iconify) {
	  ProjViewState* vst = proj->view_states.FindName(env->GetPath());
	  if(vst != NULL) vst->in_use = true;
	  continue;
	}
	cy -= obj_size.y + PROJV_OBJ_Y_SPC;
	ProjObj_G* prob = new ProjObj_G(env, this);
	append_(prob);
	prob->translate(cx, cy);
	prob->fill(proj->the_colors.FastEl(Project::ENVIRONMENT)->color);
      }
      cx = st_cx;
    }
    cx += obj_size.x + PROJV_OBJ_X_SPC;
    float envnet_ysz = span.y - cy;

    st_cx = cx;
    cy = span.y;
    float mx_xsz = 0;
    FOR_ITR_GP(taGroup_impl, gp, proj->processes., i) {
      if(gp != &(proj->processes)) {
	cy -= obj_size.y + PROJV_OBJ_Y_SPC;
	GroupObj_G* grob = new GroupObj_G(gp, this);
	append_(grob);
	grob->translate(cx, cy);
	grob->fill(proj->the_colors.FastEl(Project::GEN_GROUP)->color);
	cx = st_cx + PROJV_INDENT;
      }
      int j;
      for(j=0;j<gp->size;j++) {
	SchedProcess* sp = (SchedProcess*)gp->FastEl_(j);
	if(sp->super_proc != NULL) continue;
	ProcHier_G* prob = new ProcHier_G(sp, this);
	append_(prob);
	prob->Build();
	cy -= prob->obj_size.y + PROJV_OBJ_Y_SPC;
	prob->translate(cx, cy);
	mx_xsz = MAX(mx_xsz, prob->obj_size.x);
      }
      cx = st_cx;
    }
    // now add any empty process groups!
    int i;
    for(i=0;i<proj->processes.gp.size;i++) {
      gp = (taGroup_impl*)proj->processes.gp.FastEl(i);
      if(gp->leaves != 0) continue;
      cy -= obj_size.y + PROJV_OBJ_Y_SPC;
      GroupObj_G* grob = new GroupObj_G(gp, this);
      append_(grob);
      grob->translate(cx, cy);
      grob->fill(proj->the_colors.FastEl(Project::GEN_GROUP)->color);
    }

    cx += mx_xsz + .5f * PROJV_OBJ_X_SPC;	// smaller space
    float proc_ysz = span.y - cy;

    st_cx = cx;
    cy = span.y;
    FOR_ITR_GP(taGroup_impl, gp, proj->logs., i) {
      ProjViewState* gp_vst = NULL;
      if(gp != &(proj->logs)) {
	cy -= obj_size.y + PROJV_OBJ_Y_SPC;
	GroupObj_G* grob = new GroupObj_G(gp, this);
	append_(grob);
	grob->translate(cx, cy);
	grob->fill(proj->the_colors.FastEl(Project::GEN_GROUP)->color);
	cx = st_cx + PROJV_INDENT;
	gp_vst = proj->view_states.FindName(gp->GetPath());
	if(gp_vst != NULL) {
	  if(gp_vst->iconify) {
	    grob->fill(proj->the_colors.FastEl(Project::BACKGROUND)->color);
	    gp_vst->in_use = true;
	  }
	}
      }
      int j;
      for(j=0;j<gp->size;j++) {
	PDPLog* logo = (PDPLog*)gp->FastEl_(j);
	if((gp_vst != NULL) && gp_vst->iconify) {
	  ProjViewState* vst = proj->view_states.FindName(logo->GetPath());
	  if(vst != NULL) vst->in_use = true;
	  continue;
	}
	cy -= obj_size.y + PROJV_OBJ_Y_SPC;
	ProjObj_G* prob = new ProjObj_G(logo, this);
	append_(prob);
	prob->translate(cx, cy);
	prob->fill(proj->the_colors.FastEl(Project::PDPLOG)->color);
      }
      cx = st_cx;
    }
    cx += obj_size.x + PROJV_OBJ_X_SPC;
    float log_ysz = span.y - cy;

    max_size.y = MAX(envnet_ysz, proc_ysz);
    max_size.y = MAX(max_size.y, log_ysz);
    max_size.x = cx;

  // now go back and center everything relative to the overall height
    float dy = .5f * (envnet_ysz - max_size.y);
    int cnt = 0;
    FOR_ITR_GP(taGroup_impl, gp, proj->networks., i) {
      ProjViewState* gp_vst = NULL;
      if(gp != &(proj->networks)) {
	GroupObj_G* grob = (GroupObj_G*)component_(cnt++);
	grob->translate(0.0f, dy);
	gp_vst = proj->view_states.FindName(gp->GetPath());
      }
      int j;
      for(j=0;j<gp->size;j++) {
	if((gp_vst != NULL) && gp_vst->iconify) continue;
	ProjObj_G* prob = (ProjObj_G*)component_(cnt++);
	prob->translate(0.0f, dy);
      }
    }
    FOR_ITR_GP(taGroup_impl, gp, proj->environments., i) {
      ProjViewState* gp_vst = NULL;
      if(gp != &(proj->environments)) {
	GroupObj_G* grob = (GroupObj_G*)component_(cnt++);
	grob->translate(0.0f, dy);
	gp_vst = proj->view_states.FindName(gp->GetPath());
      }
      int j;
      for(j=0;j<gp->size;j++) {
	if((gp_vst != NULL) && gp_vst->iconify) continue;
	ProjObj_G* prob = (ProjObj_G*)component_(cnt++);
	prob->translate(0.0f, dy);
      }
    }

    dy = .5f * (proc_ysz - max_size.y);
    FOR_ITR_GP(taGroup_impl, gp, proj->processes., i) {
      if(gp != &(proj->processes)) {
	GroupObj_G* grob = (GroupObj_G*)component_(cnt++);
	grob->translate(0.0f, dy);
      }
      int j;
      for(j=0;j<gp->size;j++) {
	SchedProcess* sp = (SchedProcess*)gp->FastEl_(j);
	if(sp->super_proc != NULL) continue;
	ProcHier_G* prob = (ProcHier_G*)component_(cnt++);
	prob->translate(0.0f, dy);
      }
    }
    for(i=0;i<proj->processes.gp.size;i++) {
      gp = (taGroup_impl*)proj->processes.gp.FastEl(i);
      if(gp->leaves != 0) continue;
      GroupObj_G* grob = (GroupObj_G*)component_(cnt++);
      grob->translate(0.0f, dy);
    }

    dy = .5f * (log_ysz - max_size.y);
    FOR_ITR_GP(taGroup_impl, gp, proj->logs., i) {
      ProjViewState* gp_vst = NULL;
      if(gp != &(proj->logs)) {
	GroupObj_G* grob = (GroupObj_G*)component_(cnt++);
	grob->translate(0.0f, dy);
	gp_vst = proj->view_states.FindName(gp->GetPath());
      }
      int j;
      for(j=0;j<gp->size;j++) {
	if((gp_vst != NULL) && gp_vst->iconify) continue;
	ProjObj_G* prob = (ProjObj_G*)component_(cnt++);
	prob->translate(0.0f, dy);
      }
    }
    // protect all the other view states from being removed
    int vsi;
    for(vsi=0;vsi<proj->view_states.size;vsi++) {
      ProjViewState* vst = proj->view_states.FastEl(vsi);
      if(vst->name.contains(".specs"))
	vst->in_use = true;
    }
  }

  proj->view_states.RemoveNotInUse();

  ivExtension ext;
  if((owner!= NULL) && (owner->viewer != NULL) &&
     (owner->viewer->canvas() != NULL)) {
    allocate(owner->viewer->canvas(),_a,ext);
    safe_damage_me(owner->viewer->canvas());
  }
}

ProjObj_G* Project_G::FindObject(TAPtr ob) {
  if(ob->InheritsFrom(TA_Process)) {
    return FindProcess((Process*)ob);
  }
  else {
    int i;
    for(i=0;i<count_();i++) {
      Graphic* g = component_(i);
      if(!g->InheritsFrom(&TA_ProjObj_G)) continue;
      ProjObj_G* prob = (ProjObj_G*)g;
      if(prob->obj == ob) return prob;
    }
  }
  return NULL;
}

ProcObj_G* Project_G::FindProcess(Process* ob, bool sel_only) {
  int i;
  for(i=0;i<count_();i++) {
    Graphic* g = component_(i);
    if(!g->InheritsFrom(&TA_ProcHier_G)) continue;
    ProcHier_G* ph = (ProcHier_G*)g;
    ProcObj_G* prob = ph->FindProcess(ob, sel_only);
    if(prob != NULL) return prob;
  }
  return NULL;
}

ProcHier_G* Project_G::FindProcHier(SchedProcess* ob, bool sel_only) {
  int i;
  for(i=0;i<count_();i++) {
    Graphic* g = component_(i);
    if(!g->InheritsFrom(&TA_ProcHier_G)) continue;
    ProcHier_G* ph = (ProcHier_G*)g;
    if(ph->top == ob) {
      if(sel_only) {
	if(ph->is_selected())
	  return ph;
      }
      else return ph;
    }
  }
  return NULL;
}

void ProjectGSelectEffect(void* obj){
  ((Project_G*)obj)->owner->FixEditorButtons();
}

bool Project_G::update_from_state(ivCanvas* c) {
  bool result = GraphicMaster::update_from_state(c);
  return result;
}

bool Project_G::effect_select(bool set_select) {
  bool result = GraphicMaster::effect_select(set_select);
//    if(reinit_display) {
//      reinit_display = false;
//      owner->InitDisplay();
//    }
  return result;
}

bool Project_G::select(const ivEvent& e, Tool& tool, bool b) {
  bool result = GraphicMaster::select(e,tool,b);
  if(reinit_display) {
    reinit_display = false;
    owner->InitDisplay();
  }
  else {
    owner->FixEditorButtons();
  }
  return result;
}

void Project_G::ScaleCenter(const ivAllocation& a){
  ivAllocation b(a);
  ivAllotment& ax = _a.x_allotment();
  ivAllotment& ay = _a.y_allotment();
  ivAllotment& bx = b.x_allotment();
  ivAllotment& by = b.y_allotment();
  translate((bx.span()-ax.span())/2.0, (by.span()-ay.span())/2.0);
  translate(bx.begin()-ax.begin(), by.begin()-ay.begin());
  translate(0, 3);
}

void Project_G::ReCenter(){
  // clear out old allocation so Project is rescaled/translated.
  ivAllocation a = _a; //temp for clearing
  ivAllocation b;
  _a = b; // zero's
  ScaleCenter(a);
  _a = a;
}

void Project_G::allocate (ivCanvas* c, const ivAllocation& a, ivExtension& ext) {
  if(tx) {
    transformer(tx);
    ivResource::unref(tx);
    tx = NULL;
    _a = a;
  }
  else {
    if (!_a.equals(a, 0.001)) {
      if((owner!= NULL) && (owner->owner != NULL)){
	ScaleCenter(a);
      }
      _a = a;
    }
  }
  if (c != nil) {
    PolyGraphic::allocate(c, a, ext);
  }
}

//////////////////////////////////
//	   ProjObj_G		//
//////////////////////////////////

ProjObj_G::ProjObj_G(TAPtr ob, Project_G* ng)
  : Rectangle(ng->defbrush, ng->border, NULL, 0.0f, 0.0f, ng->obj_size.x, ng->obj_size.y, NULL)
{
  obj = ob;
  projg = ng;
  draw_links = false;

  editb_used = false;
  extendb_used = false;
  sel_count = 0;
  sel_target = false;
  ProjViewState* vst = projg->proj->view_states.FindName(obj->GetPath());
  if(vst != NULL) {
    vst->in_use = true;
    if(vst->selected) {
      effect_select(true);
      sel_count = 10;		// put over the top on autosel
    }
    draw_links = vst->draw_links;
  }
}

ProjObj_G::~ProjObj_G() {
}

ivGlyph* ProjObj_G::clone() const {
  return new ProjObj_G(obj, projg);
}

ProjViewState* ProjObj_G::GetMyViewState() {
  return projg->owner->GetViewState(obj->GetPath());
}

bool ProjObj_G::select(const ivEvent& e, Tool& tool, bool unselect) {
  int but = Graphic::GetButton(e);
  editb_used = false;
  extendb_used = false;
  if(but == ivEvent::right)
    editb_used = true;
  else if(but == ivEvent::middle)
    extendb_used = true;
  PointObj pt(e.pointer_x(), e.pointer_y());
  if(contains(pt))
    sel_target = true;
  else
    sel_target = false;
  return Rectangle::select(e, tool, unselect);
}

bool ProjObj_G::effect_select(bool set_select) {
  bool dbl_click = false;
  if(!extendb_used && !editb_used && sel_target && !set_select && is_selected()) {
    sel_count++;
    if(obj->InheritsFrom(&TA_Process)) {
      if(sel_count <= 2)
	set_select = true;	// override
      if(sel_count == 2)
	dbl_click = true;
    }
    else {
      if(sel_count <= 1)
	set_select = true;	// override
      if(sel_count == 1)
	dbl_click = true;
    }
  }
  bool temp = Rectangle::effect_select(set_select);
  ProjViewState* vst = GetMyViewState();
  if(set_select) {
    vst->selected = true;
    projg->selectgroup.LinkUnique(obj);
    if(editb_used) {
      obj->Edit();		// don't wait
      editb_used = false;	// don't do it again!
    }
    if(dbl_click) {
      if(obj->InheritsFrom(&TA_SchedProcess)) {
	vst->iconify = !vst->iconify;
	projg->reinit_display = true;
      }
      else if(obj->InheritsFrom(&TA_WinBase)) {
	WinBase* wb = (WinBase*)obj;
	if((wb->iconified) || !wb->IsMapped()) {
	  wb->ViewWindow();
	  taiMisc::RecordScript(wb->GetPath() + "->ViewWindow();\n");
	}
	else {
	  wb->Iconify();
	  taiMisc::RecordScript(wb->GetPath() + "->Iconify();\n");
	}
	projg->reinit_display = true;
      }
      else if(obj->InheritsFrom(&TA_taGroup_impl)) {
	vst->iconify = !vst->iconify;
	projg->reinit_display = true;
      }
      else {
	obj->Edit();		// default is to edit obj
      }
    }
  }
  else {
    vst->selected = false;
    sel_count = 0;
    projg->selectgroup.Remove(obj);
  }
  sel_target = false;
  return temp;
}

void ProjObj_G::draw_gs(ivCanvas* c, Graphic* gs) {
  if(_ctrlpts <= 0)
    return;

  const ivBrush* brush = gs->brush();
  const ivColor* stroke = gs->stroke();
  const ivColor* fill = gs->fill();
  if(brush == nil || stroke == nil) return;

  ivTransformer* tx = gs->transformer();
  if (tx != nil) {
    c->push_transform();
    c->transform(*tx);
  }

  c->new_path();
  c->move_to(_x[0], _y[0]);
  for (int i = 1; i < _ctrlpts; ++i) {
    c->line_to(_x[i], _y[i]);
  }
  if (_closed) {
    c->close_path();
  }
  if (fill != nil) {
    c->fill(fill);
  }
  c->stroke(stroke, brush);

  draw_label(c, gs);

  if (tx != nil)  c->pop_transform();

  if(draw_links)
    draw_the_links(c, gs);
}

void ProjObj_G::draw_label(ivCanvas* c, Graphic*) {
  render_text(c, obj->GetName(), PROJV_TXT_SPC, PROJV_TXT_SPC);
}

void ProjObj_G::draw_the_links(ivCanvas* c, Graphic* gs) {
  if(!obj->InheritsFrom(&TA_WinMgr)) return;
  WinMgr* mgr = (WinMgr*)obj;
  WinView* vw = (WinView*)mgr->views.SafeEl(0);
  if(vw == NULL) return;
  SchedProcess* updtr;
  taLeafItr i;
  FOR_ITR_EL(SchedProcess, updtr, vw->updaters., i) {
    ProcObj_G* procg = projg->FindProcess(updtr);
    if(procg != NULL) {
      if(obj->InheritsFrom(TA_Network)) {
	draw_line_right
	  (procg, -PROJV_LINK_OFFSET, projg->proj->the_colors.FastEl(Project::NETWORK)->color,
	   projg->widedashbrush, c, gs);
      }
      else if(obj->InheritsFrom(TA_Environment)) {
	draw_line_right
	  (procg, -PROJV_LINK_OFFSET, projg->proj->the_colors.FastEl(Project::ENVIRONMENT)->color,
	   projg->widedashbrush, c, gs);
      }
      else if(obj->InheritsFrom(TA_PDPLog)) {
	draw_line_left
	  (procg, PROJV_LINK_OFFSET, projg->proj->the_colors.FastEl(Project::PDPLOG)->color,
	   projg->widebrush, c, gs);
      }
    }
  }
}

void ProjObj_G::render_text(ivCanvas* c, const char* chr, float x, float y) {
  ivFontBoundingBox bbox;
  String str = chr;

  ivColor* clr = projg->proj->the_colors.FastEl(Project::TEXT)->color;

  int len = str.length();
  float cx = x;
  float cy = y;
  int j;
  for(j=0;j<len;j++){	// draw each character
    projg->proj->view_font.fnt->char_bbox(str[j], bbox);
    float nxtx = cx + bbox.width();
    if(nxtx > _x[1]) break;
    c->character((const ivFont*)(projg->proj->view_font.fnt),str[j],8,clr,cx,cy);
    cx = nxtx;
  }
}

void ProjObj_G::draw_line_left(ProjObj_G* og, float offset, ivColor* clr, const ivBrush* br,
			       ivCanvas* c, Graphic* gs) {
  ivCoord* xc; ivCoord* yc;
  og->ctrlpts(xc, yc);

  float mymid = .5f * (_y[0] + _y[2]);
  float othmid = .5f * (yc[0] + yc[2]);

  float sx = _x[2]; float sy = mymid + offset;
  ivTransformer* mytx = gs->transformer();
  if(mytx != nil) {
    mytx->transform(sx, sy);
  }

  float ex = xc[0] + 2.0f; float ey = othmid + offset;
  Graphic ogs;
  og->total_gs(ogs);
  ivTransformer* othtx = ogs.transformer();
  if(othtx != nil) {
    othtx->transform(ex, ey);
  }
  c->new_path();
  c->move_to(sx, sy);
  c->line_to(ex, ey);
  c->stroke(clr, br);
}

void ProjObj_G::draw_line_right(ProjObj_G* og, float offset, ivColor* clr, const ivBrush* br,
				ivCanvas* c, Graphic* gs) {
  ivCoord* xc; ivCoord* yc;
  og->ctrlpts(xc, yc);

  float mymid = .5f * (_y[0] + _y[2]);
  float othmid = .5f * (yc[0] + yc[2]);

  float sx = _x[0]; float sy = mymid + offset;
  ivTransformer* mytx = gs->transformer();
  if(mytx != nil) {
    mytx->transform(sx, sy);
  }

  float ex = xc[2]; float ey = othmid + offset;
  Graphic ogs;
  og->total_gs(ogs);
  ivTransformer* othtx = ogs.transformer();
  if(othtx != nil) {
    othtx->transform(ex, ey);
  }
  c->new_path();
  c->move_to(sx, sy);
  c->line_to(ex, ey);
  c->stroke(clr, br);
}

bool ProjObj_G::graspable() {
  if(obj->InheritsFrom(TA_SchedProcess) ||
     obj->InheritsFrom(TA_taList_impl)) return false;
  return true;
}

bool ProjObj_G::grasp_move(const ivEvent&,Tool&,ivCoord, ivCoord) {
  // assume i'm a network, enviro, or log -- diff one for ProcObj_G
  projg->owner->FixEditorButtons();
  move_pos = 0;
  max_pos = 0;
  cur_pos = 0;
  move_incr = projg->obj_size.y + PROJV_OBJ_Y_SPC;
  if((obj->GetOwner() == NULL) || !obj->GetOwner()->InheritsFrom(TA_taGroup_impl)) return false;
  taGroup_impl* own_gp = (taGroup_impl*)obj->GetOwner();
  cur_pos = own_gp->FindEl(obj);
  max_pos = own_gp->size;
  return true;
}

bool ProjObj_G::manip_move(const ivEvent& ,Tool& ,ivCoord, ivCoord iy, ivCoord lx, ivCoord ly, ivCoord cx, ivCoord cy) {
  ivCoord dx = cx - lx;
  ivCoord dy = cy - ly;
  if((dx == 0) && (dy == 0))
    return true;

  dy = (cy - iy);
  int npos = (int)(dy / move_incr); // new pos relative to orig pos
  int dnpos = npos - move_pos;	// subtract off previous moved position, change in position
  if(dnpos != 0) {
    int curabspos = cur_pos - npos; // npos moves opposite
    if((curabspos >= 0) && (curabspos < max_pos)) {
      translate(0.0f, (float)dnpos * move_incr);
      move_pos = npos;
    }
  }
  return true;
}

bool ProjObj_G::effect_move(const ivEvent&,Tool&,ivCoord,ivCoord,ivCoord, ivCoord) {
  if(move_pos == 0) return true;
  projg->reinit_display = true;
  taGroup_impl* own_gp = (taGroup_impl*)obj->GetOwner();
  int nw_pos = cur_pos - move_pos;
  own_gp->Move(cur_pos, nw_pos);
  String recordstring = own_gp->GetPath() + ".Move("
    + String(cur_pos) + ", " + String(nw_pos) + ");\n";
  taiMisc::RecordScript(recordstring);
  move_pos = 0;
  winbMisc::DelayedMenuUpdate(own_gp);
  return true;
}

//////////////////////////////////
//	   ProcHier_G		//
//////////////////////////////////

ProcHier_G::ProcHier_G(SchedProcess* ob, Project_G* ng) {
  top = ob;
  projg = ng;
  iconify = false;
}

ProcHier_G::~ProcHier_G() {
}

ivGlyph* ProcHier_G::clone() const {
  return new ProcHier_G(top, projg);
}

ProjViewState* ProcHier_G::GetMyViewState() {
  return projg->owner->GetViewState(top->GetPath());
}


ProcObj_G* ProcHier_G::FindProcess(Process* ob, bool sel_only) {
  int i;
  for(i=0;i<count_();i++) {
    Graphic* g = component_(i);
    if(g->InheritsFrom(&TA_ProcObj_G)) {
      ProcObj_G* prob = (ProcObj_G*)g;
      if(prob->proc == ob) {
	if(sel_only) {
	  if(prob->is_selected())
	    return prob;
	}
	else return prob;
      }
    }
    else if(g->InheritsFrom(&TA_SchedProc_G)) {
      SchedProc_G* ph = (SchedProc_G*)g;
      ProcObj_G* prob = ph->FindProcess(ob, sel_only);
      if(prob != NULL) return prob;
    }
  }
  return NULL;
}

void ProcHier_G::Build() {
  obj_size.Initialize();

  ProjViewState* vst = projg->proj->view_states.FindName(top->GetPath());
  if(vst != NULL) {
    iconify = vst->iconify;
    vst->in_use = true;
  }

  if(iconify) {
    ProcObj_G* prob = new ProcObj_G(top, NULL, projg);
    append_(prob);
    prob->fill(projg->proj->the_colors.FastEl(Project::BACKGROUND)->color);
    obj_size = projg->obj_size;

    SchedProcess* sp = top->sub_proc;
    while(sp != NULL) {
      ProjViewState* vst = projg->proj->view_states.FindName(sp->GetPath());
      if(vst != NULL)		// keep sub-guys in use
	vst->in_use = true;
      sp = sp->sub_proc;
    }
    return;
  }
  SchedProcess* sp = top;
  while(sp != NULL) {
    SchedProc_G* prob = new SchedProc_G(sp, this, projg);
    append_(prob);
    prob->Build();
    obj_size.y += prob->obj_size.y;
    sp = sp->sub_proc;
  }

  float cx = 0.0f;
  float cy = obj_size.y;
  int i;
  for(i=0;i<count_();i++) {
    SchedProc_G* prob = (SchedProc_G*)component_(i);
    cy -= prob->obj_size.y;
    prob->translate(cx, cy);
    obj_size.x = MAX(obj_size.x, prob->obj_size.x + cx);
    cx += PROJV_INDENT;
  }
}

//////////////////////////////////
//	SchedProc_G		//
//////////////////////////////////

SchedProc_G::SchedProc_G(SchedProcess* ob, ProcHier_G* hg, Project_G* pg) {
  sproc = ob;
  hierg = hg;
  projg = pg;
  iconify = false;
}

SchedProc_G::~SchedProc_G() {
}

ivGlyph* SchedProc_G::clone() const {
  return new SchedProc_G(sproc, hierg, projg);
}

ProjViewState* SchedProc_G::GetMyViewState() {
  return projg->owner->GetViewState(sproc->GetPath());
}

ProcObj_G* SchedProc_G::FindProcess(Process* ob, bool sel_only) {
  int i;
  for(i=0;i<count_();i++) {
    ProcObj_G* prob = (ProcObj_G*)component_(i);
    if(prob->proc == ob) {
      if(sel_only) {
	if(prob->is_selected())
	  return prob;
      }
      else return prob;
    }
  }
  return NULL;
}

void SchedProc_G::Build() {
  obj_size.Initialize();

  obj_size.x = projg->obj_size.x;

  // this is the object itself
  ProcObj_G* ths = new ProcObj_G(sproc, this, projg);
  append_(ths);

  ProjViewState* vst = projg->proj->view_states.FindName(sproc->GetPath());
  if(vst != NULL) {
    iconify = vst->iconify;
    ths->draw_links = vst->draw_links;
    vst->in_use = true;
  }

  if(iconify) {
    obj_size = projg->obj_size;
    ths->fill(projg->proj->the_colors.FastEl(Project::BACKGROUND)->color);
    return;
  }

  taLeafItr i;
  Stat* st;
  FOR_ITR_EL(Stat, st, sproc->loop_stats., i) {
    ProcObj_G* prob = new ProcObj_G(st, this, projg);
    append_(prob);
    obj_size.y += projg->obj_size.y;
  }
  FOR_ITR_EL(Stat, st, sproc->final_stats., i) {
    ProcObj_G* prob = new ProcObj_G(st, this, projg);
    append_(prob);
    obj_size.y += projg->obj_size.y;
  }
  Process* pr;
  FOR_ITR_EL(Process, pr, sproc->init_procs., i) {
    ProcObj_G* prob = new ProcObj_G(pr, this, projg);
    append_(prob);
    obj_size.y += projg->obj_size.y;
  }
  FOR_ITR_EL(Process, pr, sproc->loop_procs., i) {
    ProcObj_G* prob = new ProcObj_G(pr, this, projg);
    append_(prob);
    obj_size.y += projg->obj_size.y;
  }
  FOR_ITR_EL(Process, pr, sproc->final_procs., i) {
    ProcObj_G* prob = new ProcObj_G(pr, this, projg);
    append_(prob);
    obj_size.y += projg->obj_size.y;
  }

  if(count_() > 1) {		// sub-objs!
    obj_size.x += (2.0f * projg->obj_size.x); // extra doublewide
    obj_size.y += projg->obj_size.y; // add one more for my label
  }
  else {
    obj_size.y = projg->obj_size.y;
  }

  ths->translate(0.0f, obj_size.y - projg->obj_size.y);
  ths->fill(projg->proj->the_colors.FastEl(Project::SCHED_PROC)->color);

  String txt = "final_procs";
  int len = txt.length();
  ivFontBoundingBox bbox;
  float sub_wdth = 0.0f;
  int j;
  for(j=0;j<len;j++) {
    projg->proj->view_font.fnt->char_bbox(txt[j], bbox);
    sub_wdth += bbox.width();
  }
  sub_wdth += 2.0f * PROJV_TXT_SPC;
  float subgx = projg->obj_size.x - sub_wdth;
  ivColor* subclr = projg->proj->the_colors.FastEl(Project::STAT_GROUP)->color;

  float cx = projg->obj_size.x;
  float cy = obj_size.y - projg->obj_size.y;
  int idx = 1;			// start at 1 for ths obj=0
  ivColor* clr = projg->proj->the_colors.FastEl(Project::STAT_PROC)->color;

  ivColor* hiclr = projg->proj->the_colors.FastEl(Project::STAT_AGG)->color;
  ivColor* inactclr = projg->proj->the_colors.FastEl(Project::INACTIVE)->color;
  ivColor* stopcritclr = projg->proj->the_colors.FastEl(Project::STOP_CRIT)->color;
  ivColor* aggclr = projg->proj->the_colors.FastEl(Project::AGG_STAT)->color;

  StatGroup_G* stg = NULL;
  float sty = cy;
  FOR_ITR_EL(Stat, st, sproc->loop_stats., i) {
    cy -= projg->obj_size.y;
    if(stg == NULL) {
      stg = new StatGroup_G(StatGroup_G::LOOP, this, projg); append_(stg);
    }
    ProcObj_G* prob = (ProcObj_G*)component_(idx++);
    ivCoord* yc;  ivCoord* xc;  prob->ctrlpts(xc, yc);		// get coords
    xc[0] = xc[1] = 1.75f * projg->obj_size.x; // stats are extra wide
    prob->recompute_shape();
    prob->translate(cx, cy);  prob->fill(clr);
    ProjViewState* vst = projg->proj->view_states.FindName(st->GetPath()); // agg stat links
    if((vst != NULL) && vst->draw_links) prob->fill(hiclr);
    else if(!st->mod.flag) prob->fill(inactclr);
    else if(st->HasStopCrit()) prob->fill(stopcritclr);
    else if(st->time_agg.from != NULL) prob->fill(aggclr);
  }
  if(stg != NULL) {
    stg->translate(subgx, cy);	// at bottom of where it ended up
    ivCoord* yc;  ivCoord* xc;  stg->ctrlpts(xc, yc);		// get coords
    yc[1] = yc[2] = sty - cy; xc[0] = xc[1] = sub_wdth;
    stg->recompute_shape(); stg->fill(subclr);
  }
  sty = cy; stg = NULL;
  FOR_ITR_EL(Stat, st, sproc->final_stats., i) {
    cy -= projg->obj_size.y;
    if(stg == NULL) {
      stg = new StatGroup_G(StatGroup_G::FINAL, this, projg);  append_(stg);
    }
    ProcObj_G* prob = (ProcObj_G*)component_(idx++);
    ivCoord* yc;  ivCoord* xc;  prob->ctrlpts(xc, yc);		// get coords
    xc[0] = xc[1] = 1.75f * projg->obj_size.x; // stats are extra wide
    prob->recompute_shape();
    prob->translate(cx, cy); prob->fill(clr);
    ProjViewState* vst = projg->proj->view_states.FindName(st->GetPath()); // agg stat links
    if((vst != NULL) && vst->draw_links) prob->fill(hiclr);
    else if(!st->mod.flag) prob->fill(inactclr);
    else if(st->HasStopCrit()) prob->fill(stopcritclr);
    else if(st->time_agg.from != NULL) prob->fill(aggclr);
  }
  if(stg != NULL) {
    stg->translate(subgx, cy);	// at bottom of where it ended up
    ivCoord* yc;  ivCoord* xc;  stg->ctrlpts(xc, yc);		// get coords
    yc[1] = yc[2] = sty - cy; xc[0] = xc[1] = sub_wdth;
    stg->recompute_shape(); stg->fill(subclr);
  }

  clr = projg->proj->the_colors.FastEl(Project::OTHER_PROC)->color;
  subclr = projg->proj->the_colors.FastEl(Project::SUBPROC_GROUP)->color;

  SProcGroup_G* subpg = NULL;
  sty = cy;
  FOR_ITR_EL(Process, pr, sproc->init_procs., i) {
    cy -= projg->obj_size.y;
    if(subpg == NULL) {
      subpg = new SProcGroup_G(SProcGroup_G::INIT, this, projg);  append_(subpg);
    }
    ProcObj_G* prob = (ProcObj_G*)component_(idx++);
    prob->translate(cx, cy);
    if(!pr->mod.flag) prob->fill(inactclr);
    else prob->fill(clr);
  }
  if(subpg != NULL) {
    subpg->translate(subgx, cy);
    ivCoord* yc;  ivCoord* xc;  subpg->ctrlpts(xc, yc);		// get coords
    yc[1] = yc[2] = sty - cy; xc[0] = xc[1] = sub_wdth;
    subpg->recompute_shape(); subpg->fill(subclr);
  }
  sty = cy; subpg = NULL;
  FOR_ITR_EL(Process, pr, sproc->loop_procs., i) {
    cy -= projg->obj_size.y;
    if(subpg == NULL) {
      subpg = new SProcGroup_G(SProcGroup_G::LOOP, this, projg); append_(subpg);
    }
    ProcObj_G* prob = (ProcObj_G*)component_(idx++);
    prob->translate(cx, cy);
    if(!pr->mod.flag) prob->fill(inactclr);
    else prob->fill(clr);
  }
  if(subpg != NULL) {
    subpg->translate(subgx, cy);
    ivCoord* yc;  ivCoord* xc;  subpg->ctrlpts(xc, yc);		// get coords
    yc[1] = yc[2] = sty - cy; xc[0] = xc[1] = sub_wdth;
    subpg->recompute_shape(); subpg->fill(subclr);
  }
  sty = cy; subpg = NULL;
  FOR_ITR_EL(Process, pr, sproc->final_procs., i) {
    cy -= projg->obj_size.y;
    if(subpg == NULL) {
      subpg = new SProcGroup_G(SProcGroup_G::FINAL, this, projg); append_(subpg);
    }
    ProcObj_G* prob = (ProcObj_G*)component_(idx++);
    prob->translate(cx, cy);
    if(!pr->mod.flag) prob->fill(inactclr);
    else prob->fill(clr);
  }
  if(subpg != NULL) {
    subpg->translate(subgx, cy);
    ivCoord* yc;  ivCoord* xc;  subpg->ctrlpts(xc, yc);		// get coords
    yc[1] = yc[2] = sty - cy; xc[0] = xc[1] = sub_wdth;
    subpg->recompute_shape(); subpg->fill(subclr);
  }
}

//////////////////////////////////
//	   SProcGroup_G		//
//////////////////////////////////

SProcGroup_G::SProcGroup_G(WhichGroup gp, SchedProc_G* sg, Project_G* pg)
  : ProjObj_G(GetGroup(sg->sproc, gp), pg)
{
  mygp = gp;
  pgp = (Process_Group*)obj;
  spg = sg;
}

SProcGroup_G::~SProcGroup_G() {
}

ivGlyph* SProcGroup_G::clone() const {
  return new SProcGroup_G(mygp, spg, projg);
}

Process_Group* SProcGroup_G::GetGroup(SchedProcess* sp, WhichGroup gp) {
  switch(gp) {
  case INIT:
    return &(sp->init_procs);
  case LOOP:
    return &(sp->loop_procs);
  case FINAL:
    return &(sp->final_procs);
  }
  return NULL;
}

const char* SProcGroup_G::GetLabel(WhichGroup gp) {
  switch(gp) {
  case INIT:
    return "init_procs";
  case LOOP:
    return "loop_procs";
  case FINAL:
    return "final_procs";
  }
  return "";
}

void SProcGroup_G::draw_label(ivCanvas* c, Graphic*) {
  float yc = PROJV_TXT_SPC;
  float yspan = (_y[2] - _y[0]);
  if(yspan > 1.1f * projg->obj_size.y)
    yc = (.5f * yspan) - .3f * projg->obj_size.y;
  render_text(c, GetLabel(mygp), PROJV_TXT_SPC, yc);
}

//////////////////////////////////
//	   StatGroup_G		//
//////////////////////////////////

StatGroup_G::StatGroup_G(WhichGroup gp, SchedProc_G* sg, Project_G* pg)
  : ProjObj_G(GetGroup(sg->sproc, gp), pg)
{
  mygp = gp;
  sgp = (Stat_Group*)obj;
  spg = sg;
}

StatGroup_G::~StatGroup_G() {
}

ivGlyph* StatGroup_G::clone() const {
  return new StatGroup_G(mygp, spg, projg);
}

Stat_Group* StatGroup_G::GetGroup(SchedProcess* sp, WhichGroup gp) {
  switch(gp) {
  case LOOP:
    return &(sp->loop_stats);
  case FINAL:
    return &(sp->final_stats);
  }
  return NULL;
}

const char* StatGroup_G::GetLabel(WhichGroup gp) {
  switch(gp) {
  case LOOP:
    return "loop_stats";
  case FINAL:
    return "final_stats";
  }
  return "";
}

void StatGroup_G::draw_label(ivCanvas* c, Graphic*) {
  float yc = PROJV_TXT_SPC;
  float yspan = (_y[2] - _y[0]);
  if(yspan > 1.1f * projg->obj_size.y)
    yc = (.5f * yspan) - .3f * projg->obj_size.y;
  render_text(c, GetLabel(mygp), PROJV_TXT_SPC, yc);
}

//////////////////////////////////
//	   ProcObj_G		//
//////////////////////////////////

ProcObj_G::ProcObj_G(Process* ob, SchedProc_G* sg, Project_G* pg)
  : ProjObj_G(ob, pg)
{
  proc = ob;
  spg = sg;
  sched_proc = false;
}

ProcObj_G::~ProcObj_G() {
}

ivGlyph* ProcObj_G::clone() const {
  return new ProcObj_G(proc, spg, projg);
}

void ProcObj_G::draw_the_links(ivCanvas* c, Graphic* gs) {
  if(proc->InheritsFrom(TA_Stat)) return; // stats are highlighted via color

  if(proc->network != NULL) {
    ProjObj_G* netg = projg->FindObject(proc->network);
    if(netg != NULL) {
      ivColor* clr = projg->proj->the_colors.FastEl(Project::NETWORK)->color;
      draw_line_left(netg, PROJV_LINK_OFFSET, clr, projg->widebrush, c, gs);
    }
  }
  if(proc->environment != NULL) {
    ProjObj_G* envg = projg->FindObject(proc->environment);
    if(envg != NULL) {
      ivColor* clr = projg->proj->the_colors.FastEl(Project::ENVIRONMENT)->color;
      draw_line_left(envg, PROJV_LINK_OFFSET, clr, projg->widebrush, c, gs);
    }
  }
  if(proc->InheritsFrom(&TA_SchedProcess)) {
    SchedProcess* sp = (SchedProcess*)proc;
    if(sp->InheritsFrom(TA_MultiEnvProcess)) {
      MultiEnvProcess* mep = (MultiEnvProcess*)sp;
      int i;
      for(i=0;i<mep->environments.size;i++) {
	Environment* env = (Environment*)mep->environments.FastEl(i);
	if(env != mep->environment) {	// didn't already draw it
	  ProjObj_G* envg = projg->FindObject(env);
	  if(envg != NULL) {
	    ivColor* clr = projg->proj->the_colors.FastEl(Project::ENVIRONMENT)->color;
	    draw_line_left(envg, PROJV_LINK_OFFSET, clr, projg->widebrush, c, gs);
	  }
	}
      }
    }
    taLeafItr i;
    PDPLog* lg;
    FOR_ITR_EL(PDPLog, lg, sp->logs., i) {
      ProjObj_G* lgg = projg->FindObject(lg);
      if(lgg != NULL) {
	ivColor* clr = projg->proj->the_colors.FastEl(Project::PDPLOG)->color;
	draw_line_right(lgg, 0.0f, clr, projg->widebrush, c, gs);
      }
    }
    WinView* wv;
    FOR_ITR_EL(WinView, wv, sp->displays., i) {
      WinMgr* mg = wv->mgr;
      ProjObj_G* mgg = projg->FindObject(mg);
      if(mgg != NULL) {
	ivColor* clr = projg->proj->the_colors.FastEl(Project::NETWORK)->color;
	if(mg->InheritsFrom(TA_Environment))
	  clr = projg->proj->the_colors.FastEl(Project::ENVIRONMENT)->color;
	draw_line_left(mgg, -PROJV_LINK_OFFSET, clr, projg->widedashbrush, c, gs);
      }
    }
  }
}

bool ProcObj_G::grasp_move(const ivEvent& e,Tool& ts, ivCoord ix, ivCoord iy) {
  bool rval = ProjObj_G::grasp_move(e, ts, ix, iy);
  move_incr = projg->obj_size.y; // only diff is the sie of the move increment
  return rval;
}


//////////////////////////////////
//	   GroupObj_G		//
//////////////////////////////////

GroupObj_G::GroupObj_G(taGroup_impl* gp, Project_G* pg)
  : ProjObj_G(gp, pg)
{
  group = gp;
}

GroupObj_G::~GroupObj_G() {
}

ivGlyph* GroupObj_G::clone() const {
  return new GroupObj_G(group, projg);
}

void GroupObj_G::draw_label(ivCanvas* c, Graphic*) {
  String nm = obj->GetName();
  if(nm.empty()) {
    if(group->InheritsFrom(TA_BaseSpec_MGroup)) {
      if(group->owner == &(projg->proj->specs.gp))
	nm = "Group:";
      else
	nm = "children:";
    }
    else
      nm = "Group:";
  }
  else {
    nm += ":";
  }
  render_text(c, nm, PROJV_TXT_SPC, PROJV_TXT_SPC);
}

//////////////////////////////////
//	   SpecObj_G		//
//////////////////////////////////

SpecObj_G::SpecObj_G(BaseSpec* sp, Project_G* pg)
  : ProjObj_G(sp, pg)
{
  spec = sp;
}

SpecObj_G::~SpecObj_G() {
}

ivGlyph* SpecObj_G::clone() const {
  return new SpecObj_G(spec, projg);
}

//////////////////////////////////
//	  ProjViewer		//
//////////////////////////////////

//  static ivCursor* picker_cursor = nil;
static ivCursor* mywindow_cursor = nil;

ProjViewer::ProjViewer(Project* p, float w, float h, const ivColor* bg)
: GlyphViewer(w,h,bg) {
  proj = p;
};

void ProjViewer::allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext) {
  GlyphViewer::allocate(c,a,ext);
  Project_G* projg = (Project_G*)_root;
  projg->transformer(NULL);
  projg->no_text_extent = true;
  init_graphic();
  projg->no_text_extent = false;
  projg->ReCenter();
  Update_All();
}

#include <ta/enter_iv.h>
declareActionCallback(ProjEditor)
implementActionCallback(ProjEditor)
declare_taiMenuCallback(ProjEditor)
implement_taiMenuCallback(ProjEditor)
#include <ta/leave_iv.h>

//////////////////////////////////
//	   ProjEditor		//
//////////////////////////////////

ProjEditor::ProjEditor(Project* pr, ivWindow* w) {
  owner = pr;
  viewer = NULL;
  projg = NULL;
  win = w;
  spec_mode = false;

  body = NULL;
  print_patch = NULL;

  iconify_action = ICONIFY_ALL;
  links_action = NO_LINKS;
  edit_action = NO_EDIT;
  updt_action = NO_UPDT;
  agg_action = NO_AGG;
  ctrl_action = NO_CTRL;
  proc_action = NO_PROC;
  new_action = NO_NEW;
  remove_action = NO_REMOVE;

  tool_gp = NULL;
  buttonpatch = NULL;
  icon_but = NULL;
  links_but = NULL;
  edit_but = NULL;
  updt_but = NULL;
  agg_but = NULL;
  ctrl_but = NULL;
  proc_but = NULL;
  new_but = NULL;
  remove_but = NULL;
}

ProjEditor::~ProjEditor() {
  ivResource::unref(projg); 	projg = NULL;
  ivResource::unref(viewer); 	viewer = NULL;
  ivResource::unref(body); 	body = NULL;

  ivResource::unref(tool_gp); tool_gp = NULL;
  ivResource::unref(buttonpatch); buttonpatch = NULL;
}

void ProjEditor::Init() {
  projg = new Project_G(owner);
  projg->owner = this;
  ivResource::ref(projg);
  projg->Build();

  viewer = new ProjViewer(owner, PROJV_INITIAL_X, PROJV_INITIAL_Y,
			  owner->the_colors.FastEl(Project::BACKGROUND)->color);
  ivResource::ref(viewer);
  viewer->root(projg);
  if(projg->transformer()) {
    ivTransformer* t = projg->transformer();
    ivTransformer idnty;
    *t = idnty;			// return to identity
    projg->translate(-(.5f * (float)PROJV_INITIAL_X),
		    -(.5f * (float)PROJV_INITIAL_Y));
  }
}

ivButton* ProjEditor::MEButton(ivTelltaleGroup* gp, char* txt, ivAction* a) {
  ivWidgetKit* wkit = ivWidgetKit::instance();
  ivTelltaleState* t = new Teller((TelltaleFlags)0, (ivAction*)NULL);
  ivButton* rval = new NoUnSelectButton
    (wkit->palette_button_look(wkit->label(txt), t), wkit->style(),t,a);
  rval->state()->set(ivTelltaleState::is_choosable | ivTelltaleState::is_toggle |
	ivTelltaleState::is_enabled, true);
  rval->state()->join(gp);
  return rval;
}

ivGlyph* ProjEditor::GetLook() {
  if(!taMisc::iv_active) return NULL;

  ivLayoutKit* layout = ivLayoutKit::instance();
  ivWidgetKit* wkit = ivWidgetKit::instance();

  print_patch = new ivPatch(viewer);

  //////////////////////////////////////
  // the editing functions
  //////////////////////////////////////
  tool_gp = new ivTelltaleGroup;
  ivResource::ref(tool_gp);

  toggle_spec_but = new ivDeck(2);
  toggle_spec_but->append(wkit->push_button("View Specs",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::ToggleSpecEdit)));
  toggle_spec_but->append(wkit->push_button("View Project",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::ToggleSpecEdit)));
  int i;
  for(i=0;i<9;i++){
    actbutton[i]=NULL;
  }
  actbutton[Tool::select] = MEButton
    (tool_gp,"Select",
     new ActionCallback(ProjEditor)(this,&ProjEditor::SetSelect));
  ((NoUnSelectButton *) actbutton[Tool::select])->chooseme();

  actbutton[Tool::move] =  MEButton
    (tool_gp,"Move",
     new ActionCallback(ProjEditor)(this,&ProjEditor::SetMove));

  // actions
  ivButton* min_but = wkit->push_button("Minimize",new ActionCallback(ProjEditor)
					 (this, &ProjEditor::Minimize));
  ivButton* max_but = wkit->push_button("Maximize",new ActionCallback(ProjEditor)
					  (this, &ProjEditor::Maximize));

  icon_but = new ivDeck(ICONIFY_COUNT);
  icon_but->append(wkit->push_button("Iconify All",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::Iconify)));
  ((ivButton*)icon_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  icon_but->append(wkit->push_button("Iconify All",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::Iconify)));
  icon_but->append(wkit->push_button("Iconify",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::Iconify)));
  icon_but->append(wkit->push_button("DeIconify",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::Iconify)));
  icon_but->append(wkit->push_button("View",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::Iconify)));
  icon_but->append(wkit->push_button("Duplicate Spec",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::DupeSelections)));

  links_but = new ivDeck(LINKS_COUNT);
  links_but->append(wkit->push_button("No Links",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::ShowLinks)));
  ((ivButton*)links_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  links_but->append(wkit->push_button("Show Links",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::ShowLinks)));
  links_but->append(wkit->push_button("No Links",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::ShowLinks)));
  links_but->append(wkit->push_button("Show Aggs",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::ShowLinks)));
  links_but->append(wkit->push_button("No Aggs",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::ShowLinks)));

  edit_but = new ivDeck(EDIT_COUNT);
  edit_but->append(wkit->push_button("Edit",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::EditSelections)));
  ((ivButton*)edit_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  edit_but->append(wkit->push_button("Edit",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::EditSelections)));

  ivButton* init_but = new ScriptButton("Init",new ActionCallback(ProjEditor)
					(this,&ProjEditor::InitDisplay),
					owner->GetPath() + ".InitDisplay();\n");

  updt_but = new ivDeck(UPDT_COUNT);
  updt_but->append(wkit->push_button("Update",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::UpdateSelections)));
  ((ivButton*)updt_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  updt_but->append(wkit->push_button("Add Updater",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::UpdateSelections)));
  updt_but->append(wkit->push_button("Rmv Updater",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::UpdateSelections)));
  updt_but->append(wkit->push_button("Change Type",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::UpdateSelections)));
  updt_but->append(wkit->push_button("Set Flags",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::SetProcFlags)));

  agg_but = new ivDeck(AGG_COUNT);
  agg_but->append(wkit->push_button("Aggregation",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::AggStats)));
  ((ivButton*)agg_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  agg_but->append(wkit->push_button("Set Agg Type",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::SetAggType)));
  agg_but->append(wkit->push_button("Set Agg Link",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::AggStats)));
  agg_but->append(wkit->push_button("New Stat",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::AggStats)));
  agg_but->append(wkit->push_button("New Child",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::AggStats)));
  agg_but->append(wkit->push_button("New Spec in Gp",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::AggStats)));

  ctrl_but = new ivDeck(CTRL_COUNT);
  ctrl_but->append(wkit->push_button("Ctrl Panel",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::CtrlPanel)));
  ((ivButton*)ctrl_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  ctrl_but->append(wkit->push_button("Ctrl Panel",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::CtrlPanel)));
  ctrl_but->append(wkit->push_button("Set Spec",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::NetSetSpec)));
  ctrl_but->append(wkit->push_button("Duplicate Obj",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::DupeSelections)));
  ctrl_but->append(wkit->push_button("Duplicate Proc",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::DupeSelections)));
  ctrl_but->append(wkit->push_button("Duplicate Stat",new ActionCallback(ProjEditor)
				     (this, &ProjEditor::DupeSelections)));

  proc_but = new ivDeck(PROC_COUNT);
  proc_but->append(wkit->push_button("New Process",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NewProcs)));
  ((ivButton*)proc_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  proc_but->append(wkit->push_button("New Process",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NewProcs)));
  proc_but->append(wkit->push_button("New Agg",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NewProcs)));
  proc_but->append(wkit->push_button("Transfer Obj",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NewProcs)));
  proc_but->append(wkit->push_button("Show Spec",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NetShowSpec)));

  new_but = new ivDeck(NEW_COUNT);
  new_but->append(wkit->push_button("New Process",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NewObjects)));
  ((ivButton*)new_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  new_but->append(wkit->push_button("New Sub/Super",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NewObjects)));
  new_but->append(wkit->push_button("New Process",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NewObjects)));
  new_but->append(wkit->push_button("New Stat",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NewObjects)));
  new_but->append(wkit->push_button("New Link",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NewObjects)));
  new_but->append(wkit->push_button("New Scd Proc",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NewObjects)));
  new_but->append(wkit->push_button("New Spec",new ActionCallback(ProjEditor)
				    (this, &ProjEditor::NewObjects)));

  remove_but = new ivDeck(REMOVE_COUNT);
  remove_but->append(wkit->push_button("Remove", new ActionCallback(ProjEditor)
				       (this, &ProjEditor::RemoveSelections)));
  ((ivButton*)remove_but->component(0))->state()->set(ivTelltaleState::is_enabled,false);
  remove_but->append(wkit->push_button("Remove Obj(s)", new ActionCallback(ProjEditor)
				       (this, &ProjEditor::RemoveSelections)));
  remove_but->append(wkit->push_button("Remove Link", new ActionCallback(ProjEditor)
				       (this, &ProjEditor::RemoveSelections)));
  remove_but->append(wkit->push_button("New Proc Gp", new ActionCallback(ProjEditor)
				       (this, &ProjEditor::RemoveSelections)));
  remove_but->append(wkit->push_button("New Spec Gp", new ActionCallback(ProjEditor)
				       (this, &ProjEditor::RemoveSelections)));

  icon_but->flip_to(0);
  links_but->flip_to(0);
  edit_but->flip_to(0);
  updt_but->flip_to(0);
  agg_but->flip_to(0);
  ctrl_but->flip_to(0);
  proc_but->flip_to(0);
  new_but->flip_to(0);
  remove_but->flip_to(0);

  buttonpatch =
    new ivPatch(layout->vbox
		(layout->hbox(taiM->medium_button(min_but),
			      taiM->medium_button(max_but)),
		 layout->hcenter(layout->hflexible(toggle_spec_but),0),
		 wkit->menu_item_separator_look(),
		 layout->hbox(taiM->medium_button(actbutton[Tool::select]),
			      taiM->medium_button(actbutton[Tool::move])),
		 wkit->menu_item_separator_look(),
		 layout->hbox(taiM->medium_button(edit_but),
			      taiM->medium_button(init_but)),
		 layout->hbox(taiM->medium_button(icon_but),
			      taiM->medium_button(links_but)),
		 layout->hbox(taiM->medium_button(updt_but),
			      taiM->medium_button(agg_but)),
		 layout->hbox(taiM->medium_button(ctrl_but),
			      taiM->medium_button(proc_but)),
		 layout->hbox(taiM->medium_button(new_but),
			      taiM->medium_button(remove_but))
		 ));
  ivResource::ref(buttonpatch);

  body = wkit->inset_frame
    (layout->hbox
     (layout->vcenter
      (wkit->outset_frame
       (new ivBackground(layout->vflexible(buttonpatch), wkit->background())),0),
      layout->vcenter
      (wkit->inset_frame(layout->flexible(print_patch)),0)));

  ivResource::ref(body);		// gotta ref the body..

  InitDisplay();
  return body;
}


void ProjEditor::UpdateDisplay(){ // ignores display_toggle
  viewer->update_from_state();
  if((win->is_mapped()) && (win->bound())) win->repair();
}

void ProjEditor::update_from_state() {
  UpdateDisplay();
}

void ProjEditor::InitDisplay(){
  projg->Build();
  projg->transformer(NULL);
  projg->no_text_extent = true;
  viewer->init_graphic();
  projg->no_text_extent = false;
  projg->ReCenter();
  viewer->Update_All();
  FixEditorButtons();
}

void ProjEditor::ToggleSpecEdit() {
  spec_mode = !spec_mode;
  InitDisplay();
}

void ProjEditor::Minimize() {
  if(projg == NULL) return;
  owner->ReSize(owner->win_pos.wd, PROJV_MIN_Y);
}

void ProjEditor::Maximize() {
  if(projg == NULL) return;
  float new_y = projg->max_size.y + PROJV_MIN_Y + PROJV_Y_MARG;
  new_y = MAX(new_y, PROJV_BUTTONS_Y);	// always include buttons!
  owner->ReSize(owner->win_pos.wd, new_y);
}

static bool proj_editor_net_obj_for_spec_selected(Network* net, BaseSpec* spec) {
  bool objsel = false;
  if((net != NULL) && (net->views.size > 0)) {
    NetView* nv = (NetView*)net->views.DefaultEl();
    taBase_List* nsgp = nv->GetSelectGroup();
    if(nsgp->size > 0) {
      TAPtr nsob = nsgp->FastEl(0);
      if(spec->InheritsFrom(TA_UnitSpec) &&
	 (nsob->InheritsFrom(TA_Layer) || nsob->InheritsFrom(TA_Unit)))
	objsel = true;
      else if(spec->InheritsFrom(TA_ConSpec) && nsob->InheritsFrom(TA_Projection))
	objsel = true;
      else if(spec->InheritsFrom(TA_ProjectionSpec) && nsob->InheritsFrom(TA_Projection))
	objsel = true;
      else if(spec->InheritsFrom(TA_LayerSpec) && nsob->InheritsFrom(TA_Layer))
	objsel = true;
    }
  }
  return objsel;
}

static bool proj_editor_net_obj_selected(Network* net) {
  bool objsel = false;
  if((net != NULL) && (net->views.size > 0)) {
    NetView* nv = (NetView*)net->views.DefaultEl();
    taBase_List* nsgp = nv->GetSelectGroup();
    if(nsgp->size > 0) {
      TAPtr nsob = nsgp->FastEl(0);
      if(nsob->InheritsFrom(TA_Layer) || nsob->InheritsFrom(TA_Unit) ||
	 nsob->InheritsFrom(TA_Projection))
	objsel = true;
    }
  }
  return objsel;
}

void ProjEditor::FixEditorButtons(){
  if(buttonpatch == NULL) return;

  taBase_List& selectgroup = projg->selectgroup;

  edit_action = NO_EDIT;
  updt_action = NO_UPDT;
  agg_action = NO_AGG;
  ctrl_action = NO_CTRL;
  proc_action = NO_PROC;
  if(spec_mode) {
    new_action = NEW_SPEC;
    remove_action = NEW_SPEC_GP;
    links_action = NO_LINKS;
    iconify_action = NO_ICONIFY;
  }
  else {
    new_action = NEW_SPROC;
    remove_action = NEW_PROC_GP;
    links_action = NOSHOW_LINKS;
    iconify_action = ICONIFY_ALL;
  }

  int objcount=0; int sproccount=0; int proccount=0; int statcount=0;
  int sprocgcount=0; int statgcount=0;
  int gengcount=0; int speccount=0;
  TAPtr frstobj= NULL;
  Process* frstproc= NULL;
  Stat* frststat= NULL;
  SchedProcess* frstsproc= NULL;
  Process_Group* frstsprog = NULL;
  Stat_Group* frststatg = NULL;
  taGroup_impl* frstgeng = NULL;
  BaseSpec* frstspec = NULL;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_SchedProcess)) {
      if(frstsproc == NULL) frstsproc = (SchedProcess*)o;
      sproccount++;
    }
    else if(o->InheritsFrom(&TA_Stat)) {
      if(frststat == NULL) frststat = (Stat*)o;
      statcount++;
    }
    else if(o->InheritsFrom(&TA_Process)) {
      if(frstproc == NULL) frstproc = (Process*)o;
      proccount++;
    }
    else if(o->InheritsFrom(&TA_Process_Group)) {
      if(frstsprog == NULL) frstsprog = (Process_Group*)o;
      sprocgcount++;
    }
    else if(o->InheritsFrom(&TA_Stat_Group)) {
      if(frststatg == NULL) frststatg = (Stat_Group*)o;
      statgcount++;
    }
    else if(o->InheritsFrom(&TA_taGroup_impl)) {
      if(frstgeng == NULL) frstgeng = (taGroup_impl*)o;
      gengcount++;
    }
    else if(o->InheritsFrom(&TA_BaseSpec)) {
      if(frstspec == NULL) frstspec = (BaseSpec*)o;
      speccount++;
    }
    else {
      if(frstobj == NULL) frstobj = o;
      objcount++;
    }
  }

  if(objcount + sproccount + statcount + proccount + sprocgcount + statgcount + gengcount + speccount > 0) {
    edit_action = EDIT_OBJ;
    if((sprocgcount == 0) && (statgcount == 0))
      remove_action = REMOVE_OBJ;
  }

  if((sproccount == 1) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    // everything with one schedproc
    ProjViewState* vst = owner->view_states.FindName(frstsproc->GetPath());
    if((vst != NULL) && vst->iconify)
      iconify_action = DE_ICONIFY;
    else
      iconify_action = ICONIFY;
    if((vst != NULL) && vst->draw_links)
      links_action = NOSHOW_LINKS;
    else
      links_action = SHOW_LINKS;
    updt_action = CHANGE_TYPE;
    agg_action = NEW_STAT_SP;
    ctrl_action = CTRL_PANEL;
    proc_action = NEW_PROC_SP;
    new_action = NEW_SUBSUPER;
  }
  if((sproccount == 2) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    // do iconify based on 1st element
    new_action = NEW_LINK;	// link two processes together
    ctrl_action = CTRL_PANEL;
  }
  if((sproccount == 1) && (statcount == 0) && (proccount == 0) && (objcount == 1) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    // make or remove link!
    if(frstobj->InheritsFrom(TA_Network)) {
      Network* net = (Network*)frstobj;
      if(frstsproc->network == net)
	remove_action = REMOVE_LINK;
      else
	new_action = NEW_LINK;
      NetView* vw = (NetView*)net->views.SafeEl(0);
      if((vw != NULL) && (vw->updaters.FindEl(frstsproc) >= 0))
	updt_action = REMOVE_UPDATER;
      else
	updt_action = ADD_UPDATER;
    }
    else if(frstobj->InheritsFrom(TA_Environment)) {
      Environment* env = (Environment*)frstobj;
      if(frstsproc->environment == env)
	remove_action = REMOVE_LINK;
      else
	new_action = NEW_LINK;
      if(frstsproc->InheritsFrom(&TA_MultiEnvProcess)) {
	if(((MultiEnvProcess*)frstsproc)->environments.FindEl(env) >= 0)
	  remove_action = REMOVE_LINK;
	else
	  new_action = NEW_LINK;
      }
      EnviroView* vw = (EnviroView*)env->views.SafeEl(0);
      if((vw != NULL) && (vw->updaters.FindEl(frstsproc) >= 0))
	updt_action = REMOVE_UPDATER;
      else
	updt_action = ADD_UPDATER;
    }
    else if(frstobj->InheritsFrom(TA_PDPLog)) {
      PDPLog* lg = (PDPLog*)frstobj;
      if(lg->log_proc.FindEl(frstsproc) >= 0)
	updt_action = REMOVE_UPDATER;
      else
	updt_action = ADD_UPDATER;
    }
  }
  if((sproccount == 0) && (statcount +  proccount == 1) && (objcount == 1) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    Process* prc = frstproc;
    if(statcount > 0)
      prc = frststat;
    // make or remove link for other procs!
    if(frstobj->InheritsFrom(TA_Network)) {
      Network* net = (Network*)frstobj;
      if(prc->network == net)
	remove_action = REMOVE_LINK;
      else
	new_action = NEW_LINK;
    }
    else if(frstobj->InheritsFrom(TA_Environment)) {
      Environment* env = (Environment*)frstobj;
      if(prc->environment == env)
	remove_action = REMOVE_LINK;
      else
	new_action = NEW_LINK;
    }
  }
  if((sproccount == 0) && (statcount > 1) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    if(statcount == 2)
      agg_action = SET_AGG_LINK;
    agg_action = SET_AGG_TYPE;
    updt_action = SET_PROC_FLAGS;
  }
  if((sproccount == 0) && (statcount == 1) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    ProjViewState* vst = owner->view_states.FindName(frststat->GetPath());
    if((vst != NULL) && vst->draw_links)
      links_action = NOSHOW_AGGS;
    else
      links_action = SHOW_AGGS;
    agg_action = SET_AGG_TYPE;
    proc_action = NEW_AGG;
    ctrl_action = DUP_STAT;
    updt_action = SET_PROC_FLAGS;
  }
  if((sproccount == 0) && (statcount == 0) && (proccount == 1) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    ctrl_action = DUP_PROC;
    updt_action = SET_PROC_FLAGS;
  }
  if((sproccount == 0) && (statcount == 0) && (proccount > 1) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    updt_action = SET_PROC_FLAGS;
  }
  if((sproccount == 0) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 1) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    // new process
    new_action = NEW_PROC;
  }
  if((sproccount == 0) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 1) && (gengcount == 0) && (speccount == 0)) {
    // new stat
    new_action = NEW_STAT;
  }
  if((sproccount == 0) && (statcount +  proccount == 1) && (objcount == 0) &&
     (sprocgcount + statgcount == 1) && (gengcount == 0) && (speccount == 0)) {
    // transfer!
    proc_action = XFER_OBJ;
  }
  if((sproccount == 1) && (statcount + proccount == 1) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    // transfer!
    proc_action = XFER_OBJ;
  }
  if((sproccount == 1) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 1) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    // link into process group
    if(frstsprog->FindEl(frstsproc) >= 0)
      remove_action = REMOVE_LINK;
    else
      new_action = NEW_LINK;
  }
  if((sproccount == 0) && (statcount == 0) && (proccount == 0) && (objcount == 1) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    // iconify/view other objs
    if(frstobj->InheritsFrom(TA_WinBase)) {
      WinBase* wb = (WinBase*)frstobj;
      if(wb->iconified || !wb->IsMapped())
	iconify_action = VIEW;
      else
	iconify_action = ICONIFY;
    }
    updt_action = CHANGE_TYPE;
    ctrl_action = DUP_OBJ;
    ProjViewState* vst = owner->view_states.FindName(frstobj->GetPath());
    if((vst != NULL) && vst->draw_links)
      links_action = NOSHOW_LINKS;
    else
      links_action = SHOW_LINKS;
  }
  if((sproccount > 2) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 1) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    ctrl_action = CTRL_PANEL;
  }
  if((sproccount == 0) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 1) && (speccount == 0)) {
    // iconify general group
    ProjViewState* vst = owner->view_states.FindName(frstgeng->GetPath());
    if((vst != NULL) && vst->iconify)
      iconify_action = DE_ICONIFY;
    else
      iconify_action = ICONIFY;
  }
  if((sproccount == 0) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 1)) {
    // one spec active: select/set in network, new child.  This
    // makes SET_SPEC conditional on what is selected in the netview
    // so you know if it is ok to set the spec.  netview selections
    // also need to update the display.
    Network* net = (Network*)owner->networks.DefaultEl();
    bool canset = proj_editor_net_obj_for_spec_selected(net, frstspec);
    if(canset)
      ctrl_action = SET_SPEC;
    proc_action = SHOW_SPEC;
    agg_action = NEW_SPEC_CHILD;
    updt_action = CHANGE_TYPE;
    iconify_action = DUP_SPEC;
  }
  if((sproccount == 0) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 1) && (speccount == 1)) {
    // spec and group: transfer
    proc_action = XFER_OBJ;
  }
  if((sproccount == 0) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 2)) {
    // two specs: transfer
    proc_action = XFER_OBJ;
  }
  if((sproccount == 0) && (statcount == 0) && (proccount == 0) && (objcount == 1) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 1) && (speccount == 0)) {
    // generic object: transfer
    proc_action = XFER_OBJ;
  }
  if((sproccount == 1) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 1) && (speccount == 0)) {
    // sched proc transfer
    if(frstsproc->super_proc == NULL) // only move top proc!
      proc_action = XFER_OBJ;
  }
  if((sproccount == 0) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 1) && (speccount == 0)) {
    // specgroup? new child
    if(frstgeng->InheritsFrom(&TA_BaseSpec_MGroup)) {
      if(frstgeng->owner == &(owner->specs.gp))
	agg_action = NEW_SPEC_GP2;
      else
	agg_action = NEW_SPEC_CHILD;
    }
  }
  if(spec_mode && (sproccount == 0) && (statcount == 0) && (proccount == 0) && (objcount == 0) &&
     (sprocgcount == 0) && (statgcount == 0) && (gengcount == 0) && (speccount == 0)) {
    // spec mode and nothing active: check if there is something in the netview where
    // we can show specs of the selected object in the network
    Network* net = (Network*)owner->networks.DefaultEl();
    bool canset = proj_editor_net_obj_selected(net);
    if(canset)
      proc_action = SHOW_SPEC;
  }

  toggle_spec_but->flip_to(spec_mode);
  icon_but->flip_to(iconify_action);
  links_but->flip_to(links_action);
  edit_but->flip_to(edit_action);
  updt_but->flip_to(updt_action);
  agg_but->flip_to(agg_action);
  proc_but->flip_to(proc_action);
  ctrl_but->flip_to(ctrl_action);
  new_but->flip_to(new_action);
  remove_but->flip_to(remove_action);

  buttonpatch->redraw();
  if((win->is_mapped()) && (win->bound())) win->repair();
}

ProjViewState* ProjEditor::GetViewState(const char* nm) {
  ProjViewState* vst;
  if((vst = owner->view_states.FindName(nm)) == NULL) {
    vst = (ProjViewState*)owner->view_states.New(1, &TA_ProjViewState);
    vst->name = nm;
  }
  return vst;
}

void ProjEditor::Iconify() {
  taBase_List& selectgroup = projg->selectgroup;
  if(selectgroup.size == 0) { // iconify all
    if(spec_mode) {
      int i;
      taGroup_impl* gp;
      FOR_ITR_GP(taGroup_impl, gp, owner->specs., i) {
	ProjViewState* vst = GetViewState(gp->GetPath());
	vst->iconify = true;
      }
    }
    else {
      SchedProcess* sp;
      taLeafItr i;
      FOR_ITR_EL(SchedProcess, sp, owner->processes., i) {
	if(sp->super_proc != NULL) continue;
	ProjViewState* vst = GetViewState(sp->GetPath());
	vst->iconify = true;
      }
      Network* net;
      FOR_ITR_EL(Network, net, owner->networks., i) {
	net->Iconify();
	taiMisc::RecordScript(net->GetPath() + "->Iconify();\n");
      }
      Environment* env;
      FOR_ITR_EL(Environment, env, owner->environments., i) {
	env->Iconify();
	taiMisc::RecordScript(env->GetPath() + "->Iconify();\n");
      }
      PDPLog* logo;
      FOR_ITR_EL(PDPLog, logo, owner->logs., i) {
	logo->Iconify();
	taiMisc::RecordScript(logo->GetPath() + "->Iconify();\n");
      }
    }
    InitDisplay();
  }
  else {
    TAPtr o;  int i;
    for(i=0; i<selectgroup.size; i++) {
      o = selectgroup.FastEl(i);
      if(o->InheritsFrom(&TA_WinBase)) {
	WinBase* wb = (WinBase*)o;
	if((wb->iconified) || !wb->IsMapped()) {
	  wb->ViewWindow();
	  taiMisc::RecordScript(wb->GetPath() + "->ViewWindow();\n");
	}
	else {
	  wb->Iconify();
	  taiMisc::RecordScript(wb->GetPath() + "->Iconify();\n");
	}
      }
      else {
	ProjViewState* vst = GetViewState(o->GetPath());
	vst->iconify = !vst->iconify;
	InitDisplay();
	break;			// be done, otherwise might do more damage!
      }
    }
    FixEditorButtons();
  }
}

void ProjEditor::ShowLinks() {
  taBase_List& selectgroup = projg->selectgroup;
  TAPtr o;  int i;
  if(selectgroup.size == 0) { // no links!
    for(i=0;i<owner->view_states.size;i++)
      owner->view_states.FastEl(i)->draw_links = false;
    InitDisplay();
    return;
  }
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_Stat)) {
      Stat* st = (Stat*)o;
      ProjViewState* vst = GetViewState(st->GetPath());
      vst->draw_links = !vst->draw_links;
      Stat* pr = st->FindAggregator();
      while(pr != NULL) {
	ProjViewState* nvst = GetViewState(pr->GetPath());
	nvst->draw_links = vst->draw_links;
	pr = pr->FindAggregator();
      }
      // also go other direction
      pr = st->time_agg.from;
      while(pr != NULL) {
	ProjViewState* nvst = GetViewState(pr->GetPath());
	nvst->draw_links = vst->draw_links;
	pr = pr->time_agg.from;
      }
    }
    else {
      ProjViewState* vst = GetViewState(o->GetPath());
      vst->draw_links = !vst->draw_links;
      if(vst->draw_links && o->InheritsFrom(TA_WinMgr)) { // make sure process is not iconified!
	WinMgr* mgr = (WinMgr*)o;
	WinView* vw = (WinView*)mgr->views.SafeEl(0);
	if(vw == NULL) continue;
	SchedProcess* updtr;
	taLeafItr j;
	FOR_ITR_EL(SchedProcess, updtr, vw->updaters., j) {
	  ProcObj_G* procg = projg->FindProcess(updtr);
	  if(procg == NULL) {	// could not find it!
	    SchedProcess* topproc = updtr->GetTopProc();
	    ProjViewState* nvst = GetViewState(topproc->GetPath());
	    nvst->iconify = false;
	  }
	}
      }
    }
  }
  InitDisplay();
}

void ProjEditor::EditSelections() {
  taBase_List& selectgroup = projg->selectgroup;
  if(selectgroup.size > 1){
    selectgroup.Edit();
  }
  else if (selectgroup.size == 1) {
    selectgroup[0]->Edit();
  }
}

void ProjEditor::UpdateSelections() {
  taBase_List& selectgroup = projg->selectgroup;
  Process_MGroup pg;
  Network_MGroup ng;
  Environment_MGroup eg;
  PDPLog_MGroup lg;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_SchedProcess)) pg.Link((SchedProcess*)o);
    if(o->InheritsFrom(&TA_Network)) ng.Link((Network*)o);
    if(o->InheritsFrom(&TA_Environment)) eg.Link((Environment*)o);
    if(o->InheritsFrom(&TA_PDPLog)) lg.Link((PDPLog*)o);
  }
  if(pg.size > 0 && (ng.size + eg.size + lg.size > 0)) {
    for(i=0;i<pg.size;i++) {
      SchedProcess* pr = (SchedProcess*)pg.FastEl(i);
      int j;
      for(j=0;j<ng.size;j++) {
	Network* net = (Network*)ng.FastEl(j);
	NetView* vw = (NetView*)net->views.SafeEl(0);
	if(vw == NULL)
	  vw = (NetView*)net->views.NewEl(1);
	if(vw->updaters.FindEl(pr) >= 0) {
	  DMEM_GUI_RUN_IF {
	    vw->RemoveUpdater(pr);
	  }
	  taiMisc::RecordScript(vw->GetPath() + "->RemoveUpdater(" + pr->GetPath() + ");\n");
	}
	else {
	  DMEM_GUI_RUN_IF {
	    vw->AddUpdater(pr);
	  }
	  taiMisc::RecordScript(vw->GetPath() + "->AddUpdater(" + pr->GetPath() + ");\n");
	}
      }
      for(j=0;j<eg.size;j++) {
	Environment* env = (Environment*)eg.FastEl(j);
	EnviroView* vw = (EnviroView*)env->views.SafeEl(0);
	if(vw == NULL)
	  vw = (EnviroView*)env->views.NewEl(1);
	if(vw->updaters.FindEl(pr) >= 0) {
	  DMEM_GUI_RUN_IF {
	    vw->RemoveUpdater(pr);
	  }
	  taiMisc::RecordScript(vw->GetPath() + "->RemoveUpdater(" + pr->GetPath() + ");\n");
	}
	else {
	  DMEM_GUI_RUN_IF {
	    vw->AddUpdater(pr);
	  }
	  taiMisc::RecordScript(vw->GetPath() + "->AddUpdater(" + pr->GetPath() + ");\n");
	}
      }
      for(j=0;j<lg.size;j++) {
	PDPLog* l = (PDPLog*)lg.FastEl(j);
	if(l->log_proc.FindEl(pr) >= 0) {
	  DMEM_GUI_RUN_IF {
	    l->RemoveUpdater(pr);
	  }
	  taiMisc::RecordScript(l->GetPath() + "->RemoveUpdater(" + pr->GetPath() + ");\n");
	}
	else {
	  l->AddUpdater(pr);
	  if(taMisc::record_script != NULL)
	    taiMisc::RecordScript(l->GetPath() + "->AddUpdater(" + pr->GetPath() + ");\n");
	}
      }
    }
  }
  else {			// change type
    for(i=0; i<selectgroup.size; i++) {
      o = selectgroup.FastEl(i);
      TypeDef* td = o->GetTypeDef();
      if(o->InheritsFrom(TA_Environment)) td = &TA_Environment;
      if(o->InheritsFrom(TA_SchedProcess)) td = &TA_SchedProcess;
      if(o->InheritsFrom(TA_Stat)) td = &TA_Stat;
      if(o->InheritsFrom(TA_PDPLog)) td = &TA_PDPLog;
      if(o->InheritsFrom(TA_BaseSpec)) td = &TA_BaseSpec;
      String prompt = String("Change object (") + o->GetName() + ") from type: " +
	o->GetTypeDef()->name + " to:";
      TypeDef* newt = taiTypeDialog::GetType(td, prompt, o->GetTypeDef());
      if(newt != NULL) {
	String recordscript = o->GetPath() + "->ChangeMyType(" + newt->name + ");\n";
	DMEM_GUI_RUN_IF {
	  o->ChangeMyType(newt);
	}
	taiMisc::RecordScript(recordscript);
      }
    }
  }
  InitDisplay();
}

void ProjEditor::AggStats() {
  taBase_List& selectgroup = projg->selectgroup;
  Process_Group pg;
  Stat_Group sg;
  BaseSpec_MGroup spg;
  taBase_Group spgg;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_SchedProcess)) pg.Link((SchedProcess*)o);
    if(o->InheritsFrom(&TA_Stat)) sg.Link((Stat*)o);
    if(o->InheritsFrom(&TA_BaseSpec)) spg.Link((BaseSpec*)o);
    if(o->InheritsFrom(&TA_BaseSpec_MGroup)) spgg.Link((BaseSpec_MGroup*)o);
  }
  if(sg.size == 1) {	// one stat = set aggregator opr
    Stat* sp = (Stat*)sg.FastEl(0);
    TypeDef* enm = TA_Aggregate.sub_types.FindName("Operator");
    if(enm != NULL) {
      String prompt = String("Change aggregator on Stat: ") + sp->name;
      int nwval = taiEnumDialog::GetEnum(enm, prompt, sp->time_agg.op);
      if(nwval >= 0) {
	DMEM_GUI_RUN_IF {
	  sp->time_agg.op = (Aggregate::Operator)nwval;
	  sp->UpdateAfterEdit();
	}
	taiMisc::RecordScript(sp->GetPath() + "->time_agg.op = " + String(nwval) + ";\n");
      }
    }
  }
  else if(sg.size == 2) { // set first to aggregate from second
    Stat* sp1 = (Stat*)sg.FastEl(1);
    Stat* sp2 = (Stat*)sg.FastEl(0);
    if(sp1->GetTypeDef() != sp2->GetTypeDef()) {
      taMisc::Error("Aggregator types must be the same!");
      return;
    }
    String choice_string = String("Set: ") + sp1->name + " to aggregate from: " + sp2->name;
    int chs = taMisc::Choice(choice_string,"Yes","Other Direction", "Cancel");
    if(chs == 0) {
      DMEM_GUI_RUN_IF {
	sp1->time_agg.SetFrom(sp2);
      }
      taiMisc::RecordScript(sp1->GetPath() + "->time_agg.SetFrom(" + sp2->GetPath() + ");\n");
    }
    else if(chs == 1) {
      DMEM_GUI_RUN_IF {
	sp2->time_agg.SetFrom(sp1);
      }
      taiMisc::RecordScript(sp2->GetPath() + "->time_agg.SetFrom(" + sp1->GetPath() + ");\n");
    }
  }
  else if(pg.size > 0) {	// create stuff for the sched process
    for(i=0;i<pg.size;i++) {
      SchedProcess* sp = (SchedProcess*)pg.FastEl(i);
      sp->CallFun("NewStat");
    }
  }
  else if(spg.size == 1) {
    BaseSpec* sp = (BaseSpec*)spg.FastEl(0);
    TAPtr rval = sp->children.New(0); // causes menu to come up
    if(rval != NULL) {
      winbMisc::DelayedMenuUpdate(sp->GetOwner());
      sp->UpdateAfterEdit();
    }
  }
  else if(spgg.size == 1) {
    BaseSpec_MGroup* sp = (BaseSpec_MGroup*)spgg.FastEl(0);
    TAPtr rval = sp->New(0); // causes menu to come up
    if(rval != NULL) {
      winbMisc::DelayedMenuUpdate(sp);
      sp->GetOwner()->UpdateAfterEdit();
    }
  }
  InitDisplay();
}

void ProjEditor::SetAggType() {
  taBase_List& selectgroup = projg->selectgroup;
  Stat_Group sg;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_Stat)) sg.Link((Stat*)o);
  }
  if(sg.size == 0) return;
  Stat* sp = (Stat*)sg.FastEl(0);
  TypeDef* enm = TA_Aggregate.sub_types.FindName("Operator");
  if(enm != NULL) {
    String prompt;
    if(sg.size == 1) prompt= String("Change aggregator on Stat: ") + sp->name;
    else prompt = "Change aggregators on all selected stats";
    int nwval = taiEnumDialog::GetEnum(enm, prompt, sp->time_agg.op);
    if(nwval >= 0) {
      for(i=0;i<sg.size;i++) {
	Stat* ms = (Stat*)sg.FastEl(i);
	DMEM_GUI_RUN_IF {
	  ms->time_agg.op = (Aggregate::Operator)nwval;
	  ms->UpdateAfterEdit();
	}
	taiMisc::RecordScript(ms->GetPath() + "->time_agg.op = " + String(nwval) + ";\n");
      }
    }
  }
  InitDisplay();
}

void ProjEditor::SetProcFlags() {
  taBase_List& selectgroup = projg->selectgroup;
  Stat_Group sg;
  Process_Group pg;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_Stat)) sg.Link((Stat*)o);
    else if(o->InheritsFrom(&TA_Process)) pg.Link((Process*)o);
  }
  if(sg.size + pg.size == 0) return;
  if(pg.size == 0) {
    // only stats: also ask about log_stat flag
    int chs = taMisc::Choice("Set flags on selected Stat(s) (Init = reset stat vals and aggregations)",
			     "Run", "No Run", "Log", "No Log", "Init: In Loop", "Init: Start Only", "Init: None", "Cancel");
    if(chs == 7) return;
    for(i=0;i<sg.size;i++) {
      Stat* ms = (Stat*)sg.FastEl(i);
      if(chs == 0) {
	DMEM_GUI_RUN_IF {
	  ms->mod.flag = true;  ms->UpdateAfterEdit();
	}
	taiMisc::RecordScript(ms->GetPath() + "->mod.flag = true;\n");
      }
      else if(chs == 1) {
	DMEM_GUI_RUN_IF {
	  ms->mod.flag = false;  ms->UpdateAfterEdit();
	}
	taiMisc::RecordScript(ms->GetPath() + "->mod.flag = false;\n");
      }
      else if(chs == 2) {
	DMEM_GUI_RUN_IF {
	  ms->log_stat = true;  ms->UpdateAfterEdit();
	}
	taiMisc::RecordScript(ms->GetPath() + "->log_stat = true;\n");
      }
      else if(chs == 3) {
	DMEM_GUI_RUN_IF {
	  ms->log_stat = false;  ms->UpdateAfterEdit();
	}
	taiMisc::RecordScript(ms->GetPath() + "->log_stat = false;\n");
      }
      else if(chs == 4) {
	DMEM_GUI_RUN_IF {
	  ms->loop_init = Stat::INIT_IN_LOOP;  ms->UpdateAfterEdit();
	}
	taiMisc::RecordScript(ms->GetPath() + "->loop_init = Stat::INIT_IN_LOOP;\n");
      }
      else if(chs == 5) {
	DMEM_GUI_RUN_IF {
	  ms->loop_init = Stat::INIT_START_ONLY;  ms->UpdateAfterEdit();
	}
	taiMisc::RecordScript(ms->GetPath() + "->loop_init = Stat::INIT_START_ONLY;\n");
      }
      else if(chs == 6) {
	DMEM_GUI_RUN_IF {
	  ms->loop_init = Stat::NO_INIT;  ms->UpdateAfterEdit();
	}
	taiMisc::RecordScript(ms->GetPath() + "->loop_init = Stat::NO_INIT;\n");
      }
    }
  }
  else {
    int chs = taMisc::Choice("Set 'mod.flag' on selected Process(es): determines whether process is run or not",
			     "Run", "No Run", "Cancel");
    if(chs == 2) return;
    for(i=0;i<pg.size;i++) {
      Process* mp = (Process*)pg.FastEl(i);
      if(chs == 0) {
	DMEM_GUI_RUN_IF {
	  mp->mod.flag = true;  mp->UpdateAfterEdit();
	}
	taiMisc::RecordScript(mp->GetPath() + "->mod.flag = true;\n");
      }
      else if(chs == 1) {
	DMEM_GUI_RUN_IF {
	  mp->mod.flag = false;  mp->UpdateAfterEdit();
	}
	taiMisc::RecordScript(mp->GetPath() + "->mod.flag = false;\n");
      }
    }
  }
  InitDisplay();
}

void ProjEditor::NewProcs() {
  taBase_List& selectgroup = projg->selectgroup;
  Process_Group spg;
  Process_Group sg;
  Process_Group pg;
  taBase_Group stg;
  taBase_Group prg;
  taBase_Group geng;
  BaseSpec_MGroup specg;
  taBase_Group objg;		// generic objs
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_SchedProcess)) spg.Link((SchedProcess*)o);
    else if(o->InheritsFrom(&TA_Stat)) sg.Link((Stat*)o);
    else if(o->InheritsFrom(&TA_Process)) pg.Link((Process*)o);
    else if(o->InheritsFrom(&TA_Process_Group)) prg.Link((Process_Group*)o);
    else if(o->InheritsFrom(&TA_Stat_Group)) stg.Link((Stat_Group*)o);
    else if(o->InheritsFrom(&TA_taGroup_impl)) geng.Link((taGroup_impl*)o);
    else if(o->InheritsFrom(&TA_BaseSpec)) specg.Link((BaseSpec*)o);
    else objg.Link(o);
  }
  if((prg.size == 1) && (pg.size == 1)) { // transfer to group
    Process_Group* tg = (Process_Group*)prg.FastEl(0);
    Process* pr = (Process*)pg.FastEl(0);
    taiMisc::RecordScript(tg->GetPath() + "->Transfer(" + pr->GetPath() + ");\n");
    DMEM_GUI_RUN_IF {
      tg->Transfer(pr);
    }
    winbMisc::DelayedMenuUpdate(tg);
  }
  else if((stg.size == 1) && (sg.size == 1)) {
    Stat_Group* tg = (Stat_Group*)stg.FastEl(0);
    Stat* st = (Stat*)sg.FastEl(0);
    taiMisc::RecordScript(tg->GetPath() + "->Transfer(" + st->GetPath() + ");\n");
    DMEM_GUI_RUN_IF {
      tg->Transfer(st);
    }
    winbMisc::DelayedMenuUpdate(tg);
  }
  else if((spg.size == 1) && (sg.size == 1)) { // transfer to gp on schedproc
    SchedProcess* sp = (SchedProcess*)spg.FastEl(0);
    Stat* st = (Stat*)sg.FastEl(0);
    String choice_string = String("Which stat subgroup should: ") + st->name + " be transfered into on"
      + sp->name;
    int gpch = taMisc::Choice(choice_string,"Loop Stats", "Final Stats", "Cancel");
    if(gpch == 0) {
      taiMisc::RecordScript(sp->GetPath() + "->loop_stats.Transfer(" + st->GetPath() + ");\n");
      DMEM_GUI_RUN_IF {
	sp->loop_stats.Transfer(st);
      }
    }
    else if(gpch == 1) {
      taiMisc::RecordScript(sp->GetPath() + "->final_stats.Transfer(" + st->GetPath() + ");\n");
      DMEM_GUI_RUN_IF {
	sp->final_stats.Transfer(st);
      }
    }
    winbMisc::DelayedMenuUpdate(sp);
  }
  else if((spg.size == 1) && (pg.size == 1)) { // transfer to gp on schedproc
    SchedProcess* sp = (SchedProcess*)spg.FastEl(0);
    Process* pr = (Process*)pg.FastEl(0);
    String choice_string = String("Which process subgroup should: ") + pr->name + " be transfered into on"
      + sp->name;
    int gpch = taMisc::Choice(choice_string,"Init Procs", "Loop Procs", "Final Procs", "Cancel");
    if(gpch == 0) {
      taiMisc::RecordScript(sp->GetPath() + "->init_procs.Transfer(" + pr->GetPath() + ");\n");
      DMEM_GUI_RUN_IF {
	sp->init_procs.Transfer(pr);
      }
    }
    else if(gpch == 1) {
      taiMisc::RecordScript(sp->GetPath() + "->loop_procs.Transfer(" + pr->GetPath() + ");\n");
      DMEM_GUI_RUN_IF {
	sp->loop_procs.Transfer(pr);
      }
    }
    else if(gpch == 2) {
      taiMisc::RecordScript(sp->GetPath() + "->final_procs.Transfer(" + pr->GetPath() + ");\n");
      DMEM_GUI_RUN_IF {
	sp->final_procs.Transfer(pr);
      }
    }
    winbMisc::DelayedMenuUpdate(sp);
  }
  else if((geng.size == 1) && (specg.size == 1)) { // transfer a spec
    taGroup_impl* tg = (taGroup_impl*)geng.FastEl(0);
    BaseSpec* pr = (BaseSpec*)specg.FastEl(0);
    taiMisc::RecordScript(tg->GetPath() + "->Transfer(" + pr->GetPath() + ");\n");
    DMEM_GUI_RUN_IF {
      tg->Transfer(pr);
    }
    winbMisc::DelayedMenuUpdate(tg);
  }
  else if((specg.size == 2)) { // transfer a spec to a spec
    BaseSpec* pr1 = (BaseSpec*)specg.FastEl(0);
    BaseSpec* pr2 = (BaseSpec*)specg.FastEl(1);
    String choice_string = String("Transfer spec: ") + pr2->name + " into children of spec: " + pr1->name + " or other way around?";
    int gpch = taMisc::Choice(choice_string,"Yes", "Other Direction", "Cancel");
    if(gpch == 2) return;
    if(gpch == 1) {
      pr1 = (BaseSpec*)specg.FastEl(1);
      pr2 = (BaseSpec*)specg.FastEl(0);
    }
    BaseSpec* pr1own = GET_OWNER(pr1,BaseSpec);
    if(pr2 == pr1own) {		// transfering our parent into us, first transfer us into parent's group
      taGroup_impl* pr2own = (taGroup_impl*)pr2->GetOwner();
      taiMisc::RecordScript(pr2own->GetPath() + ".Transfer(" + pr1->GetPath() + ");\n");
      DMEM_GUI_RUN_IF {
	pr2own->Transfer(pr1);
      }
    }
    taiMisc::RecordScript(pr1->children.GetPath() + ".Transfer(" + pr2->GetPath() + ");\n");
    DMEM_GUI_RUN_IF {
      pr1->children.Transfer(pr2);
    }
    winbMisc::DelayedMenuUpdate(pr1);
  }
  else if((geng.size == 1) && (objg.size == 1)) { // transfer another object
    taGroup_impl* tg = (taGroup_impl*)geng.FastEl(0);
    TAPtr pr = objg.FastEl(0);
    taiMisc::RecordScript(tg->GetPath() + "->Transfer(" + pr->GetPath() + ");\n");
    DMEM_GUI_RUN_IF {
      tg->Transfer(pr);
    }
    winbMisc::DelayedMenuUpdate(tg);
  }
  else if((geng.size == 1) && (spg.size == 1)) { // transfer schedule process
    SchedProcess* pr = (SchedProcess*)spg.FastEl(0);
    taGroup_impl* tg = (taGroup_impl*)geng.FastEl(0);
    do {
      taiMisc::RecordScript(tg->GetPath() + "->Transfer(" + pr->GetPath() + ");\n");
      DMEM_GUI_RUN_IF {
	tg->Transfer(pr);
      }
      pr = pr->sub_proc;
    } while(pr != NULL);
    winbMisc::DelayedMenuUpdate(tg);
  }
  else {
    for(i=0;i<spg.size;i++) {
      SchedProcess* sp = (SchedProcess*)spg.FastEl(i);
      sp->CallFun("NewProcess");
    }
    for(i=0;i<sg.size;i++) {
      Stat* sp = (Stat*)sg.FastEl(i);
      TypeDef* enm = TA_Aggregate.sub_types.FindName("Operator");
      if(enm != NULL) {
	String prompt = String("Create new aggregator of Stat: ") + sp->name + " using aggregator:";
	int nwval = taiEnumDialog::GetEnum(enm, prompt, sp->time_agg.op);
	if(nwval >= 0) {
	  DMEM_GUI_RUN_IF {
	    sp->CreateAggregates((Aggregate::Operator)nwval);
	    sp->UpdateAfterEdit();
	  }
	  taiMisc::RecordScript(sp->GetPath() + "->CreateAggregates(" + String(nwval) + ");\n");
	  winbMisc::DelayedMenuUpdate(sp);
	}
      }
    }
  }
  InitDisplay();
}

void ProjEditor::CtrlPanel() {
  taBase_List& selectgroup = projg->selectgroup;
  Process_Group pg;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_SchedProcess)) pg.Link((SchedProcess*)o);
  }
  for(i=0;i<pg.size;i++) {
    SchedProcess* sp = (SchedProcess*)pg.FastEl(i);
    sp->ControlPanel();
    taiMisc::RecordScript(sp->GetPath() + "->ControlPanel();\n");
  }
}

void ProjEditor::NewObjects() {
  taBase_List& selectgroup = projg->selectgroup;
  Process_Group pg;
  Process_Group spg;
  Network_MGroup ng;
  Environment_MGroup eg;
  PDPLog_MGroup lg;
  TALOG proc_gps;
  TALOG stat_gps;
  TALOG proc_mgps;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_Process)) pg.Link((Process*)o);
    if(o->InheritsFrom(&TA_SchedProcess)) spg.Link((SchedProcess*)o);
    if(o->InheritsFrom(&TA_Network)) ng.Link((Network*)o);
    if(o->InheritsFrom(&TA_Environment)) eg.Link((Environment*)o);
    if(o->InheritsFrom(&TA_PDPLog)) lg.Link((PDPLog*)o);
    if(o->InheritsFrom(&TA_Process_MGroup)) proc_mgps.Link((Process_MGroup*)o);
    if(o->InheritsFrom(&TA_Process_Group)) proc_gps.Link((Process_Group*)o);
    if(o->InheritsFrom(&TA_Stat_Group)) stat_gps.Link((Stat_Group*)o);
  }
  if((pg.size > 0) && (ng.size + eg.size + lg.size > 0)) {	// set link to network, env or log
    for(i=0;i<pg.size;i++) {
      Process* pr = (Process*)pg.FastEl(i);
      int j;
      for(j=0;j<ng.size;j++) {
	Network* n = (Network*)ng.FastEl(j);
	DMEM_GUI_RUN_IF {
	  pr->CopyPNEPtrs(n, pr->environment);
	}
	taiMisc::RecordScript(pr->GetPath() + "->network = " + n->GetPath() + ";\n");
	pr->UpdateAfterEdit();
      }
      for(j=0;j<eg.size;j++) {
	Environment* e = (Environment*)eg.FastEl(j);
	DMEM_GUI_RUN_IF {
	  pr->CopyPNEPtrs(pr->network, e);
	}
	taiMisc::RecordScript(pr->GetPath() + "->environment = " + e->GetPath() + ";\n");
	if(pr->InheritsFrom(&TA_MultiEnvProcess)) {
	  DMEM_GUI_RUN_IF {
	    ((MultiEnvProcess*)pr)->environments.LinkUnique(e);
	  }
	  taiMisc::RecordScript(pr->GetPath() + "->environments.LinkUnique(" + e->GetPath() + ");\n");
	}
	pr->UpdateAfterEdit();
      }
      if(pr->InheritsFrom(TA_SchedProcess)) {
	for(j=0;j<lg.size;j++) {
	  PDPLog* l = (PDPLog*)lg.FastEl(j);
	  DMEM_GUI_RUN_IF {
	    l->AddUpdater((SchedProcess*)pr);
	  }
	  taiMisc::RecordScript(l->GetPath() + "->AddUpdater(" + pr->GetPath() + ");\n");
	}
      }
    }
  }
  else if((pg.size > 0) && (proc_gps.size > 0)) { // link process into group
    for(i=0; i<proc_gps.size; i++) {
      Process_Group* prog = (Process_Group*)proc_gps.FastEl(i);
      int j;
      for(j=0; j<pg.size; j++) {
	Process* pr = (Process*)pg.FastEl(j);
	DMEM_GUI_RUN_IF {
	  prog->LinkUnique(pr);
	}
	taiMisc::RecordScript(prog->GetPath() + "->LinkUnique(" + pr->GetPath() + ");\n");
      }
    }
  }
  else if(spg.size == 1) { // new in process
    SchedProcess* sp = (SchedProcess*)spg.FastEl(0);
    sp->CallFun("NewSchedProc");
  }
  else if(spg.size == 2) { // link together processes
    SchedProcess* sp1 = (SchedProcess*)spg.FastEl(1);
    SchedProcess* sp2 = (SchedProcess*)spg.FastEl(0);

    String choice_string = String("Link: ") + sp2->name + " into process subgroup on: " + sp1->name;
    int chs = taMisc::Choice(choice_string,"Yes","Other Direction", "Cancel");

    if(chs == 2) return;
    if(chs == 1) {
      SchedProcess* tmp = sp1; sp1 = sp2; sp2 = tmp;  // swap
    }

    choice_string = String("Which process subgroup should: ") + sp2->name + " be linked into on"
      + sp1->name;
    int gpch = taMisc::Choice(choice_string,"Init Procs","Loop Procs", "Final Procs", "Cancel");

    if(gpch == 0) {
      DMEM_GUI_RUN_IF {
	sp1->init_procs.LinkUnique(sp2);
      }
      taiMisc::RecordScript(sp1->GetPath() + "->init_procs.LinkUnique(" + sp2->GetPath() + ");\n");
    }
    else if(gpch == 1) {
      DMEM_GUI_RUN_IF {
	sp1->loop_procs.LinkUnique(sp2);
      }
      taiMisc::RecordScript(sp1->GetPath() + "->loop_procs.LinkUnique(" + sp2->GetPath() + ");\n");
    }
    else if(gpch == 2) {
      DMEM_GUI_RUN_IF {
	sp1->final_procs.LinkUnique(sp2);
      }
      taiMisc::RecordScript(sp1->GetPath() + "->final_procs.LinkUnique(" + sp2->GetPath() + ");\n");
    }
  }
  else if(proc_gps.size > 0) { // new sub-process in this group
    for(i=0; i<proc_gps.size; i++) {
      Process_Group* prog = (Process_Group*)proc_gps.FastEl(i);
      gpivGroupNew::New(prog);
    }
  }
  else if(proc_mgps.size > 0) { // new sub-process in this group
    for(i=0; i<proc_mgps.size; i++) {
      Process_MGroup* prog = (Process_MGroup*)proc_mgps.FastEl(i);
      gpivGroupNew::New(prog);
    }
  }
  else if(stat_gps.size > 0) { // new stats
    for(i=0; i<stat_gps.size; i++) {
      Stat_Group* prog = (Stat_Group*)stat_gps.FastEl(i);
      gpivGroupNew::New(prog);
    }
  }
  else {			// default action: new stuff!
    if(spec_mode) {
      owner->specs.New(0);
      winbMisc::DelayedMenuUpdate(&(owner->specs));
    }
    else {
      owner->processes.New(0);
      winbMisc::DelayedMenuUpdate(&(owner->processes));
    }
  }
  InitDisplay();
}


void ProjEditor::DupeSelections() {
  taBase_List& selectgroup = projg->selectgroup;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    taiMisc::RecordScript(o->GetPath() + "->DuplicateMe();\n");
    DMEM_GUI_RUN_IF {
      o->DuplicateMe();
    }
  }
  InitDisplay();
}

void ProjEditor::RemoveSelections() {
  projg->safe_damage_me(viewer->canvas());
  taBase_List& selectgroup = projg->selectgroup;
  Process_Group pg;
  Network_MGroup ng;
  Environment_MGroup eg;
  TALOG proc_gps;
  TALOG stat_gps;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_Process)) pg.Link((Process*)o);
    else if(o->InheritsFrom(&TA_Network)) ng.Link((Network*)o);
    else if(o->InheritsFrom(&TA_Environment)) eg.Link((Environment*)o);
    else if(o->InheritsFrom(&TA_Process_Group)) proc_gps.Link((Process_Group*)o);
    else if(o->InheritsFrom(&TA_Stat_Group)) stat_gps.Link((Stat_Group*)o);
  }

  if((proc_gps.size > 0) || (stat_gps.size > 0)) {
    int okdel = taMisc::Choice("Are you sure you want to remove these objects?","Yes - Delete","No - Cancel");
    if(okdel == 1) return;
    int j;
    for(j=proc_gps.size-1;j>=0;j--) {
      Process_Group* gp = (Process_Group*)proc_gps.FastEl(j);
      for(i=pg.size-1;i>=0;i--) {
	Process* pr = (Process*)pg.FastEl(i);
	taiMisc::RecordScript(gp->GetPath() + "->RemoveEl(" + pr->GetPath() + ");\n");
	DMEM_GUI_RUN_IF {
	  gp->Remove(pr);
	}
      }
    }
    for(j=stat_gps.size-1;j>=0;j--) {
      Stat_Group* gp = (Stat_Group*)proc_gps.FastEl(j);
      for(i=pg.size-1;i>=0;i--) {
	Process* pr = (Process*)pg.FastEl(i);
	taiMisc::RecordScript(gp->GetPath() + "->RemoveEl(" + pr->GetPath() + ");\n");
	DMEM_GUI_RUN_IF {
	  gp->Remove(pr);
	}
      }
    }
  }
  else if((pg.size == 1) && (ng.size + eg.size > 0)) {	// a process and a net or env--remove link
    int okdel = taMisc::Choice("Are you sure you want to remove these objects?","Yes - Delete","No - Cancel");
    if(okdel == 1) return;
    Process* pr = (Process*)pg.FastEl(0);
    int j;
    for(j=0;j<ng.size;j++) {
      Network* n = (Network*)ng.FastEl(j);
      if(pr->network == n) {
	DMEM_GUI_RUN_IF {
	  pr->CopyPNEPtrs(NULL, pr->environment);	// set to null!
	  pr->UpdateAfterEdit();
	}
	taiMisc::RecordScript(pr->GetPath() + "->network = NULL;\n");
      }
    }
    for(j=0;j<eg.size;j++) {
      Environment* e = (Environment*)eg.FastEl(j);
      if(pr->environment == e) {
	DMEM_GUI_RUN_IF {
	  pr->CopyPNEPtrs(pr->network, NULL);
	  pr->UpdateAfterEdit();
	}
	taiMisc::RecordScript(pr->GetPath() + "->environment = NULL;\n");
      }
      if(pr->InheritsFrom(&TA_MultiEnvProcess)) {
	taiMisc::RecordScript(pr->GetPath() + "->environments.RemoveEl(" + e->GetPath() + ");\n");
	DMEM_GUI_RUN_IF {
	  ((MultiEnvProcess*)pr)->environments.Remove(e);
	}
	pr->UpdateAfterEdit();
      }
    }
  }
  else if(selectgroup.size > 0) {
    int okdel = taMisc::Choice("Are you sure you want to remove these objects?","Yes - Delete","No - Cancel");
    if(okdel == 1) return;
    for(i=selectgroup.size-1;i>=0;i--){
      o = selectgroup[i];
      selectgroup.Remove(i);
      taList_impl* og = (taList_impl*)(o)->GetOwner(&TA_taList_impl);
      if(og != NULL) {
	if(o->InheritsFrom(&TA_taGroup_impl)) {
	  // a group that is not owned by one.. remove all
	  taGroup_impl* gp = (taGroup_impl*)o;
	  if(!gp->owner->InheritsFrom(&TA_taList_impl)) {
	    taiMisc::RecordScript(gp->GetPath() + "->Reset();\n");
	    DMEM_GUI_RUN_IF {
	      gp->Reset();
	    }
	    winbMisc::DelayedMenuUpdate(gp);
	    continue;
	  }
	}
	if(o->InheritsFrom(&TA_SchedProcess)) {
	  SchedProcess* sp = (SchedProcess*)o;
	  ProcObj_G* prob = projg->FindProcess(sp, true); // find selected
	  if((prob != NULL) && (prob->spg != NULL) && (prob->spg->sproc != sp)) {
	    // not me -- remove link!
	    SchedProcess* ownsp = prob->spg->sproc;
	    // gotta be in one of these!
	    if(ownsp->init_procs.FindEl(sp) >= 0) {
	      DMEM_GUI_RUN_IF {
		ownsp->init_procs.Remove(sp);
	      }
	      taiMisc::RecordScript(ownsp->GetPath() + "->init_procs.RemoveEl(" + sp->GetPath() + ");\n");
	    }
	    if(ownsp->loop_procs.FindEl(sp) >= 0) {
	      DMEM_GUI_RUN_IF {
		ownsp->loop_procs.Remove(sp);
	      }
	      taiMisc::RecordScript(ownsp->GetPath() + "->loop_procs.RemoveEl(" + sp->GetPath() + ");\n");
	    }
	    if(ownsp->final_procs.FindEl(sp) >= 0) {
	      DMEM_GUI_RUN_IF {
		ownsp->final_procs.Remove(sp);
	      }
	      taiMisc::RecordScript(ownsp->GetPath() + "->final_procs.RemoveEl(" + sp->GetPath() + ");\n");
	    }
	    continue;
	  }
	}
	taiMisc::RecordScript(o->GetPath() + "->Close();\n");
	DMEM_GUI_RUN_IF {
	  og->Close_Child(o);
	}
	if(og->InheritsFrom(TA_taSubGroup)) {
	  winbMisc::DelayedMenuUpdate(og->GetOwner());
	}
      }
    }
  }
  else {			// default action: new groups!
    if(spec_mode) {
      owner->specs.gp.New(0);
      winbMisc::DelayedMenuUpdate(&(owner->specs.gp));
    }
    else {
      owner->processes.gp.New(0);
      winbMisc::DelayedMenuUpdate(&(owner->processes.gp));
    }
  }
  projg->safe_damage_me(viewer->canvas());
  InitDisplay();
}

void ProjEditor::NetSetSpec() {
  taBase_List& selectgroup = projg->selectgroup;
  BaseSpec* sp = NULL;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_BaseSpec)) { sp = (BaseSpec*)o; break; }
  }
  if(sp == NULL) return;
  Network* net = (Network*)owner->networks.DefaultEl();
  if(net == NULL) return;
  if(net->views.size == 0) net->views.NewEl(1);
  NetView* nv = (NetView*)net->views[0];
  nv->ViewWindow();
  if(sp->InheritsFrom(TA_UnitSpec)) {
    taiMisc::RecordScript(nv->GetPath() + "->SetUnitSpec(" + sp->GetPath() + ");\n");
    DMEM_GUI_RUN_IF {
      nv->SetUnitSpec((UnitSpec*)sp);
    }
  }
  else if(sp->InheritsFrom(TA_ConSpec)) {
    taiMisc::RecordScript(nv->GetPath() + "->SetConSpec(" + sp->GetPath() + ");\n");
    DMEM_GUI_RUN_IF {
      nv->SetConSpec((ConSpec*)sp);
    }
  }
  else if(sp->InheritsFrom(TA_ProjectionSpec)) {
    taiMisc::RecordScript(nv->GetPath() + "->SetPrjnSpec(" + sp->GetPath() + ");\n");
    DMEM_GUI_RUN_IF {
      nv->SetPrjnSpec((ProjectionSpec*)sp);
    }
  }
  else if(sp->InheritsFrom(TA_LayerSpec)) {
    taiMisc::RecordScript(nv->GetPath() + "->SetLayerSpec(" + sp->GetPath() + ");\n");
    DMEM_GUI_RUN_IF {
      nv->SetLayerSpec((LayerSpec*)sp);
    }
  }
}

void ProjEditor::NetShowSpec() {
  Network* net = (Network*)owner->networks.DefaultEl();
  if(net == NULL) return;
  if(net->views.size == 0) net->views.NewEl(1);
  NetView* nv = (NetView*)net->views.DefaultEl();

  taBase_List& selectgroup = projg->selectgroup;
  BaseSpec* sp = NULL;
  TAPtr o;  int i;
  for(i=0; i<selectgroup.size; i++) {
    o = selectgroup.FastEl(i);
    if(o->InheritsFrom(&TA_BaseSpec)) { sp = (BaseSpec*)o; break; }
  }
  if(sp == NULL) {		// now job is to show specs applicable to object!
    taBase_List* nsgp = nv->GetSelectGroup();
    if(nsgp->size == 0) return;
    TAPtr nsob = nsgp->FastEl(0);
    if(nsob->InheritsFrom(TA_Layer)) {
      Layer* lay = (Layer*)nsob;
      if(lay->unit_spec.spec != NULL) owner->Select(lay->unit_spec.spec);
      LayerSpec* lsp = lay->GetLayerSpec();
      if(lsp != NULL) owner->Select(lsp);
    }
    if(nsob->InheritsFrom(TA_Projection)) {
      Projection* prjn = (Projection*)nsob;
      if(prjn->spec.spec != NULL) owner->Select(prjn->spec.spec);
      if(prjn->con_spec.spec != NULL) owner->Select(prjn->con_spec.spec);
    }
    if(nsob->InheritsFrom(TA_Unit)) {
      Unit* unit = (Unit*)nsob;
      if(unit->spec.spec != NULL) owner->Select(unit->spec.spec);
    }
  }
  else {
    nv->ViewWindow();
    if(sp->InheritsFrom(TA_UnitSpec))
      nv->ShowUnitSpec((UnitSpec*)sp);
    else if(sp->InheritsFrom(TA_ConSpec))
      nv->ShowConSpec((ConSpec*)sp);
    else if(sp->InheritsFrom(TA_ProjectionSpec))
      nv->ShowPrjnSpec((ProjectionSpec*)sp);
    else if(sp->InheritsFrom(TA_LayerSpec))
      nv->ShowLayerSpec((LayerSpec*)sp);
  }
}

void ProjEditor::SelectActButton(int toolnumber){
  if(viewer == NULL)
    return;
  viewer->cur_tool(toolnumber);
  if(actbutton[toolnumber] != NULL)
    ((NoUnSelectButton *) actbutton[toolnumber])->chooseme();
}

void ProjEditor::SetSelect(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::select);
}

void ProjEditor::SetMove(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::move);
}

void ProjEditor::SetReScale(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::scale);
}

void ProjEditor::SetReShape(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::stretch);
}

void ProjEditor::SetRotate(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::rotate);
}

void ProjEditor::SetAlter(){
  // Set select cursor
  if(win) win->cursor(mywindow_cursor);
  if(viewer) viewer->cur_tool(Tool::alter);
}

*/
