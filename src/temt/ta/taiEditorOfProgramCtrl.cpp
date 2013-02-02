// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "taiEditorOfProgramCtrl.h"
#include <Program>
#include <Program_Group>
#include <ProgVar>
#include <DynEnum>

#include <iLabel>
#include <taiWidgetField>
#include <taiWidgetComboBox>
#include <taiWidgetFieldIncr>
#include <taiMember>
#include <taiWidgetBitBox>
#include <taiMemberOfDynEnum>

#include <SigLinkSignal>
#include <taMisc>
#include <taiMisc>

TypeInst_Of(DynEnum);           // TAI_DynEnum

taiEditorOfProgramCtrl::taiEditorOfProgramCtrl(Program* prog, bool read_only_,
                                           bool modal_, QObject* parent)
: taiEditorOfClass(prog, prog->GetTypeDef(), read_only_, modal_, parent)
{
  membs.SetMinSize(MS_CNT); // note: our own MS_CNT
  membs.def_size = 0; // we handle everything
  //  use_show = false;
  refs.setOwner(this); // these update frequently
  refs_struct.setOwner(this); // these are same for prog lifetime
  if (prog) { // better have a value!
    refs_struct.Add(prog);
    // note: we deftly solve the problem of reacting to new vars/args
    // by simply putting those lists on our ref list, which notifies us
    refs_struct.Add(&(prog->args));
    refs_struct.Add(&(prog->vars));
  }
  Program_Group* pg = GET_OWNER(prog, Program_Group);
  if (pg) { // better exist!
    refs_struct.Add(pg);
  }
  sel_edit_mbrs = true;
}

taiEditorOfProgramCtrl::~taiEditorOfProgramCtrl() {
}

bool taiEditorOfProgramCtrl::ShowMember(MemberDef* md) const {
  return false;
}

void taiEditorOfProgramCtrl::Cancel_impl() {
  refs.setOwner(NULL);
  refs.Reset(); // release all the guys we are linked to
  refs_struct.setOwner(NULL);
  refs_struct.Reset(); // release all the guys we are linked to
  inherited::Cancel_impl();
}

void taiEditorOfProgramCtrl::Enum_Members() {
  // we show all sections
  for (int j = 0; j < MS_CNT; ++j) {
    show_set(j) = true;
  }
}


void taiEditorOfProgramCtrl::Constr_Data_Labels() {
  dat_cnt = 0;
  Program* prog = this->prog();
  refs.Reset();
  membs.ResetItems(); // all Meths and data
  if (!prog) return; // defensive
  String nm;
  String help_text;

  // Program guys (just a few key guys), no label
  {
    MemberSpace& ms = prog->GetTypeDef()->members;
    for (int i = 0; i < ms.size; ++i) {
      MemberDef* md = ms.FastEl(i);
      if (md->im == NULL) continue; // this puppy won't show nohow!set_grp
      // just have a fixed list of guys we show
      if ((md->name == "name") || (md->name == "desc") || (md->name == "short_nm")
          || (md->name == "flags")) {
        memb_el(MS_PROG).Add(md);
      }
    }
    int idx = 0;
    // we can just use the default worker bee routine
    Constr_Data_Labels_impl(idx, &memb_el(MS_PROG), &data_el(MS_PROG));
  }

//   taiMemberWidgets* ms = membs.SafeEl(MS_GP);
//   ms->text = "Items from Program_Group";
//   ms->desc = "useful items from the Program_Group to which this Program belongs";
//   iLabel* lbl = new iLabel(ms->text.chars(), body);
//   AddSectionLabel(-1, lbl,ms->desc.chars());

  taiMemberWidgets* ms = NULL;
  iLabel* lbl = NULL;
  // args and vars
  for (int j = MS_ARGS; j <= MS_VARS; ++j) {
    ms = membs.SafeEl(j);
    ProgVar_List* pvl = NULL;
    switch (j) {
    case MS_ARGS: {
      ms->text = "Program args";
      ms->desc = "the arguments to the program";
      pvl = &prog->args;
      lbl = new iLabel(ms->text.chars(), body);
      AddSectionLabel(-1, lbl,ms->desc.chars());
      break;
    }
    case MS_VARS: {
      ms->text = "Program vars";
      ms->desc = "the variables used inside the program";
      pvl = &prog->vars;
      lbl = new iLabel(ms->text.chars(), body);
      AddSectionLabel(-1, lbl, ms->desc.chars());
      } break;
    default: continue; // shouldn't happen!
    }


    for (int i = 0; i < pvl->size; ++i) {
      ProgVar* pv = pvl->FastEl(i);
      if(!pv->HasVarFlag(ProgVar::CTRL_PANEL)) continue;
      MemberDef* md = pv->GetValMemberDef();
      memb_el(j).Add(md);
      taiWidget* mb_dat;
      int flags_ = 0;
      if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY))
        flags_ |= taiWidget::flgReadOnly;
      if((pv->var_type == ProgVar::T_HardEnum) || (pv->var_type == ProgVar::T_DynEnum)) {
        if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) {
          mb_dat = new taiWidgetField(NULL, this, NULL, body, flags_);
        }
        else if((pv->var_type == ProgVar::T_HardEnum && pv->hard_enum_type &&
                 pv->hard_enum_type->HasOption("BITS")) ||
                (pv->dyn_enum_val.enum_type && pv->dyn_enum_val.enum_type->bits)) {
          mb_dat = new taiWidgetBitBox(NULL, this, NULL, body, flags_ | taiWidget::flgAutoApply);
        }
        else {
          mb_dat = new taiWidgetComboBox(true, NULL, this, NULL, body, flags_ | taiWidget::flgAutoApply);
        }
      }
      else if (pv->var_type == ProgVar::T_Int) {
        taiWidgetFieldIncr* int_rep = new taiWidgetFieldIncr(NULL, this, NULL, body, flags_);
        int_rep->setMinimum(INT_MIN);
        mb_dat = int_rep;
      }
      else {
        mb_dat = md->im->GetWidgetRep(this, NULL, body, NULL, flags_);
      }
      // we need to manually set the md into the dat...
      // need to check for enums, because md is the type, not the val
      if (pv->var_type == ProgVar::T_HardEnum)
        md = pv->FindMember("int_val");
      else if (pv->var_type == ProgVar::T_DynEnum) {
        // special case -- we will be setting the base to the DynEnum, not pv
        // and herein need to set the md for the nested dyn_val, which
        // conceivably may not even exist, so we do this via the instance
        md = TAI_DynEnum->FindMember("value");
      }
      mb_dat->SetMemberDef(md); // usually done by im, but we are manual here...

      data_el(j).Add(mb_dat);
      QWidget* data = mb_dat->GetRep();
      //int row = AddData(-1, data);
      nm = pv->name;
      help_text = pv->desc;
      AddNameData(-1, nm, help_text, data, mb_dat/*, md*/);
      refs.Add(pv);
      ++dat_cnt;
    }
  } // j == set
}

void taiEditorOfProgramCtrl::SigDestroying_Ref(taBase_RefList* ref, taBase* base) {
  // we need to rebuild...
  if (ref == &refs)
    ReShow_Async();
  // otherwise, pgp or prog are destroying, so don't bother
}

void taiEditorOfProgramCtrl::SigEmit_Ref(taBase_RefList* ref, taBase* base,
    int sls, void* op1, void* op2)
{
  if (ignoreSigEmit()) return; // not visible, so ignore!
  Program* prog = this->prog(); //cache
  if (!prog) return;
  // ignore list delete msgs, since the obj itself should notify
  if (ref == &refs_struct) {
    if ((base == &(prog->args)) ||(base == &(prog->vars))) {
      if ((sls <= SLS_LIST_INIT) ||  (sls == SLS_LIST_ITEM_REMOVE) ||
        (sls > SLS_LIST_SORTED)
      ) return;
    }
    Program_Group* pg = GET_OWNER(prog, Program_Group);
    // for step, only interested in group-as-object item update
    if (base == pg) {
      if ((sls > SLS_ITEM_UPDATED_ND))
        return;
    }
  }

  //note: don't need to check for is_loading because we defer until after
  // we need to do a fullblown reshow, to handle things like name changes of vars, etc.
  if(!apply_req)                // already doing delayed apply
    ReShow_Async();
}

MemberDef* taiEditorOfProgramCtrl::GetMemberPropsForSelect(int sel_idx, taBase** base,
    String& lbl, String& desc)
{
  return NULL;
/*
  MemberDef* md = NULL;
  if (!(membs.GetFlatDataItem(sel_idx, &md) && md))
    return NULL;
  if (base) *base = cur_base;
  String tlbl = ((taBase*)cur_base)->GetName().elidedTo(16);
  lbl = tlbl;
  return md;*/
}


void taiEditorOfProgramCtrl::GetValue_Membs_def() {
  Program* prog = this->prog(); //cache
  if (!prog) return;

  // prog stuff
  if (show_set(MS_PROG) && (data_el(MS_PROG).size > 0)) {
    GetValue_impl(&memb_el(MS_PROG), data_el(MS_PROG), prog);
  }

  // group stuff
//   if (show_set(MS_GP) && (data_el(MS_GP).size > 0)) {
//     Program_Group* pg = GET_OWNER(prog, Program_Group);
//     GetValue_impl(&memb_el(MS_GP), data_el(MS_GP), pg);
//   }

  bool first_diff = true;
  for (int j = MS_ARGS; j <= MS_VARS; ++j) {
    if (!show_set(j)) continue;
    ProgVar_List* pvl = NULL;
    switch (j) {
    case MS_ARGS: pvl = &prog->args; break;
    case MS_VARS: pvl = &prog->vars; break;
    default: continue; // shouldn't happen!
    }

    int cnt = 0;
    for (int i = 0; i < pvl->size; ++i) {
      ProgVar* pv = pvl->FastEl(i);
      if(!pv->HasVarFlag(ProgVar::CTRL_PANEL))
        continue;
      MemberDef* md = memb_el(j).SafeEl(cnt);
      taiWidget* mb_dat = data_el(j).SafeEl(cnt++);
      //note: code below is "risky" ex if visiblity update ctrl changes etc.
      // then the type values can be wrong -- so we strongly cast
      if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) continue; // do this after the cnt++!
      if (!md || !mb_dat) {
        taMisc::DebugInfo("taiEditorOfProgramCtrl:GetValue_impl: ran out of controls!");
        break;
      }
      if(pv->var_type == ProgVar::T_HardEnum) {
        if(pv->hard_enum_type && pv->hard_enum_type->HasOption("BITS")) {
          taiWidgetBitBox* tmb_dat = dynamic_cast<taiWidgetBitBox*>(mb_dat);
          if (pv->TestError(!tmb_dat, "expected taiWidgetBitBox, not: ",
                            mb_dat->metaObject()->className())) continue;
          tmb_dat->GetValue(pv->int_val);
        }
        else {
          taiWidgetComboBox* tmb_dat = dynamic_cast<taiWidgetComboBox*>(mb_dat);
          //note: use of pv for tests is just a hook, pv not really germane
          if (pv->TestError(!tmb_dat, "expected taiWidgetComboBox, not: ",
                            mb_dat->metaObject()->className())) continue;
          tmb_dat->GetEnumValue(pv->int_val); // todo: not supporting first_diff
        }
      }
      else if(pv->var_type == ProgVar::T_DynEnum) { // todo: not supporting first_diff
        if(pv->dyn_enum_val.enum_type && pv->dyn_enum_val.enum_type->bits) {
          taiWidgetBitBox* tmb_dat = dynamic_cast<taiWidgetBitBox*>(mb_dat);
          if (pv->TestError(!tmb_dat, "expected taiWidgetBitBox, not: ",
                            mb_dat->metaObject()->className())) continue;
          tmb_dat->GetValue(pv->dyn_enum_val.value);
        }
        else {
          taiWidgetComboBox* tmb_dat = dynamic_cast<taiWidgetComboBox*>(mb_dat);
          if (pv->TestError(!tmb_dat, "expected taiWidgetComboBox, not: ",
                            mb_dat->metaObject()->className())) continue;
          tmb_dat->GetValue(pv->dyn_enum_val.value);
        }
      }
      else if(pv->var_type == ProgVar::T_Int) { // todo: not supporting first_diff
        taiWidgetFieldIncr* tmb_dat = dynamic_cast<taiWidgetFieldIncr*>(mb_dat);
        if (pv->TestError(!tmb_dat, "expected taiWidgetFieldIncr, not: ",
          mb_dat->metaObject()->className())) continue;
        pv->int_val = tmb_dat->GetValue();
      }
      else {
        md->im->GetMbrValue(mb_dat, (void*)pv, first_diff);
      }
      pv->UpdateAfterEdit();
      if(!first_diff) {         // always reset!
        taiMember::EndScript((void*)pv);
        first_diff = true;
      }
    }
  }
}

void taiEditorOfProgramCtrl::GetImage_Membs()
{
  Program* prog = this->prog(); //cache
  if (!prog) return;

  // prog stuff
  if (show_set(MS_PROG) && (data_el(MS_PROG).size > 0)) {
    GetImage_impl(&memb_el(MS_PROG), data_el(MS_PROG), prog);
  }

  for (int j = MS_ARGS; j <= MS_VARS; ++j) {
    if (!show_set(j)) continue;
    ProgVar_List* pvl = NULL;
    switch (j) {
    case MS_ARGS: pvl = &prog->args; break;
    case MS_VARS: pvl = &prog->vars; break;
    default: continue; // shouldn't happen!
    }

    int cnt = 0;
    for (int i = 0; i < pvl->size; ++i) {
      ProgVar* pv = pvl->FastEl(i);
      if(!pv->HasVarFlag(ProgVar::CTRL_PANEL)) continue;
      MemberDef* md = memb_el(j).SafeEl(cnt);
      taiWidget* mb_dat = data_el(j).SafeEl(cnt++);
      if (!md || !mb_dat) {
        taMisc::DebugInfo("taiEditorOfProgramCtrl:GetImage_impl: ran out of controls!");
        break;
      }
      // set base, for ctxt menu, so it won't try to use the Program (which is not the base)
      mb_dat->SetBase(pv); // for all, except HardEnum which is nested again
      if(pv->var_type == ProgVar::T_HardEnum) {
        if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) {
          taiWidgetField* tmb_dat = dynamic_cast<taiWidgetField*>(mb_dat);
          if (pv->TestError(!tmb_dat, "expected taiWidgetField, not: ",
                            mb_dat->metaObject()->className())) continue;
          tmb_dat->GetImage(pv->GenCssInitVal());
        }
        else if(pv->hard_enum_type && pv->hard_enum_type->HasOption("BITS")) {
          taiWidgetBitBox* tmb_dat = dynamic_cast<taiWidgetBitBox*>(mb_dat);
          if (pv->TestError(!tmb_dat, "expected taiWidgetBitBox, not: ",
                            mb_dat->metaObject()->className())) continue;
          tmb_dat->SetEnumType(pv->hard_enum_type);
          tmb_dat->GetImage(pv->int_val);
        }
        else {
          taiWidgetComboBox* tmb_dat = dynamic_cast<taiWidgetComboBox*>(mb_dat);
          if (pv->TestError(!tmb_dat, "expected taiWidgetComboBox, not: ",
                            mb_dat->metaObject()->className())) continue;
          tmb_dat->SetEnumType(pv->hard_enum_type);
          tmb_dat->GetEnumImage(pv->int_val);
        }
      }
      else if(pv->var_type == ProgVar::T_DynEnum) {
        mb_dat->SetBase(&pv->dyn_enum_val);
        if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) {
          taiWidgetField* tmb_dat = dynamic_cast<taiWidgetField*>(mb_dat);
          if (pv->TestError(!tmb_dat, "expected taiWidgetField, not: ",
                            mb_dat->metaObject()->className())) continue;
          tmb_dat->GetImage(pv->GenCssInitVal());
        }
        else if(pv->dyn_enum_val.enum_type && pv->dyn_enum_val.enum_type->bits) {
          taiWidgetBitBox* tmb_dat = dynamic_cast<taiWidgetBitBox*>(mb_dat);
          if (pv->TestError(!tmb_dat, "expected taiWidgetBitBox, not: ",
                            mb_dat->metaObject()->className())) continue;
          taiMemberOfDynEnum::UpdateDynEnumBits(tmb_dat, pv->dyn_enum_val);
        }
        else {
          taiWidgetComboBox* tmb_dat = dynamic_cast<taiWidgetComboBox*>(mb_dat);
          if (pv->TestError(!tmb_dat, "expected taiWidgetComboBox, not: ",
                            mb_dat->metaObject()->className())) continue;
          taiMemberOfDynEnum::UpdateDynEnumCombo(tmb_dat, pv->dyn_enum_val);
        }
      }
      else if(pv->var_type == ProgVar::T_Int) { // todo: not supporting first_diff
        taiWidgetFieldIncr* tmb_dat = dynamic_cast<taiWidgetFieldIncr*>(mb_dat);
        if (pv->TestError(!tmb_dat, "expected taiWidgetFieldIncr, not: ",
          mb_dat->metaObject()->className())) continue;
        tmb_dat->GetImage(pv->int_val);
      }
      else {
        md->im->GetImage(mb_dat, (void*)pv);
      }
    }
  }
}
