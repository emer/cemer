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

#include "taiSpecMember.h"
#include <BaseSpec>
#include <BaseSubSpec>
#include <taiWidgetPlusToggle>
#include <IWidgetHost>
#include <iCheckBox>

#include <taMisc>


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

bool taiSpecMember::NoCheckBox(IWidgetHost* host_) const {
  if (!host_) return true;
  void* base = host_->Base();
  TypeDef* typ = host_->GetRootTypeDef();
  if (!base || !typ) return true;

  if(typ->InheritsFrom(TA_BaseSpec_Group))
    return false;               // always use a check box for these..

  if(typ->InheritsFrom(TA_BaseSpec)) {
    BaseSpec* bs = (BaseSpec*)base;
    if(bs->FindParent() != NULL)
      return false;             // owner has a parent spec, needs a box
    return true;                // no owner, no box..
  }
  else if(typ->InheritsFrom(TA_BaseSubSpec)) {
    BaseSubSpec* bs = (BaseSubSpec*)base;
    if(bs->FindParent() != NULL)
      return false;             // owner has a parent spec, needs a box
    return true;                // no owner, no box..
  }
  else {
    taBase* tap = (taBase*)base;
    BaseSpec* bs = (BaseSpec*)tap->GetOwner(&TA_BaseSpec); // find an owner..
    if(bs == NULL)
      return true;              // no owner, no box..
    if(bs->FindParent() != NULL)
      return false;             // owner is owned by a spec, needs a box..
  }
  return true;                  // default is to not have box...
}

taiWidget* taiSpecMember::GetArbitrateDataRep(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent, int flags_, MemberDef* mbr_)
{
  if (!mbr_) mbr_ = mbr;
  bool no_check_box = NoCheckBox(host_);
  if (no_check_box) {
    taiWidget* rdat;
    if (HasLowerBidder())
      rdat = LowerBidder()->GetWidgetRep(host_, par, gui_parent, NULL, flags_, mbr_);
    else
      rdat = taiMember::GetWidgetRep_impl(host_, par, gui_parent, flags_, mbr_);
    return rdat;
  }
  else {
    taiWidgetPlusToggle* rval = new taiWidgetPlusToggle(NULL, host_, par, gui_parent, flags_);
    rval->InitLayout();
    taiWidget* rdat;
    if (HasLowerBidder())
      rdat = LowerBidder()->GetWidgetRep(host_, rval, rval->GetRep(), NULL, flags_, mbr_);
    else
      rdat = taiMember::GetWidgetRep_impl(host_, rval, rval->GetRep(), flags_, mbr_);
    rval->data = rdat;
    rval->AddChildWidget(rdat->GetRep());
    rval->EndLayout();
    rval->but_rep->setToolTip("click this button to override the parent settings for this parameter -- if this is not checked, then whatever parameter is set on the parent spec will be automatically inherited by this child spec");
    return rval;
  }
}

void taiSpecMember::GetArbitrateImage(taiWidget* dat, const void* base) {
  IWidgetHost* host_ = dat->host;
  bool no_check_box = NoCheckBox(host_);
  if (no_check_box) {
    if (HasLowerBidder())
      LowerBidder()->GetImage(dat,base);
    else
      taiMember::GetImage_impl(dat, base);
  }
  else {
    taiWidgetPlusToggle* rval = dynamic_cast<taiWidgetPlusToggle*>(dat);
    if(!rval || !rval->data) {
      taMisc::Error("spec mbr bug: null data in:", mbr->name);
      return;
    }
    if (HasLowerBidder())
      LowerBidder()->GetImage(rval->data,base);
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

void taiSpecMember::GetArbitrateMbrValue(taiWidget* dat, void* base, bool& first_diff) {
  IWidgetHost* host_ = dat->host;
  bool no_check_box = NoCheckBox(host_);
  if (no_check_box) {
    if (HasLowerBidder())
      LowerBidder()->GetMbrValue(dat, base, first_diff);
    else
      taiMember::GetMbrValue(dat, base, first_diff);
    return;
  }

  taiWidgetPlusToggle* rval = dynamic_cast<taiWidgetPlusToggle*>(dat);
  if(!rval || !rval->data) {
    taMisc::Error("spec mbr bug: null data in:", mbr->name);
    return;
  }
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

  if(!rval->data) {
    taMisc::Error("spec mbr bug: null data in:", mbr->name);
    return;
  }

  if (HasLowerBidder())
    LowerBidder()->GetMbrValue(rval->data, base, first_diff);
  else
    taiMember::GetMbrValue(rval->data, base, first_diff);
}

void taiSpecMember::CmpOrigVal(taiWidget* dat, const void* base, bool& first_diff) {
  if(!taMisc::record_on || !typ->IsActualTaBase())
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

  taMisc::record_script << "  ths->SetUnique(\"" << mbr->name << "\", "
                        << new_val << ");" << "\n";
}


