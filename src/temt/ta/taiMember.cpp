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

#include "taiMember.h"
#include <taBase>
#include <MemberDef>
#include <taiData>
#include <taiDataDeck>
#include <taiSigLink>
#include <taiBitBox>
#include <EnumDef>
#include <taiTypeOfEnum>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>

// macro for doing safe casts of types -- helps to uncover errors
// when the actual class is not the expected class
// (of course this NEVER happens... uh, huh... ;) )
#define QCAST_MBR_SAFE_EXIT(qtyp, rval, dat) \
  qtyp rval = qobject_cast<qtyp>(dat); \
  if (!rval) { \
    taMisc::Error("QCAST_MBR_SAFE_EXIT: expect " #qtyp "for mbr", \
    mbr->name, "but was:", \
      dat->metaObject()->className()); \
    return; \
  }

void taiMember::EndScript(const void* base) {
  if(!taMisc::record_on)
    return;
  if((((taBase*)base)->GetOwner() == NULL) && ((taBase*)base != tabMisc::root))
    return;     // no record for unowned objs (except root)!
  taMisc::record_script << "}" << "\n";
}

bool taiMember::isReadOnly(taiData* dat, IDataHost* host_) {
  // ReadOnly if parent type is RO, or par is RO, OR directives state RO
  bool rval = taiType::isReadOnly(dat, host_);
  rval = rval || mbr->HasOption("READ_ONLY") || //note: 'IV' only for legacy support
    mbr->HasOption("IV_READ_ONLY") || mbr->HasOption("GUI_READ_ONLY");
  return rval;
}

bool taiMember::handlesReadOnly() const {
  return mbr->type->it->handlesReadOnly();
}

taiData* taiMember::GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
                               taiType* parent_type_, int flags_, MemberDef*)
{//note: we ignore MemberDef because we use our own
  bool ro = isReadOnly(par, host_);
  // we must communicate read_only when getting item
  //TODO: probably should also use parent_type in determining ro, as base class does
  if (ro)
    flags_ |= taiData::flgReadOnly;
  if (((mbr->HasOption(TypeItem::opt_inline)) ||
       (mbr->HasOption(TypeItem::opt_edit_inline)))
    && mbr->type->it->allowsInline())
    flags_ |= taiData::flgInline;
  if (mbr->HasOption(TypeItem::opt_EDIT_DIALOG)) // if a string field, puts up an editor button
    flags_ |= taiData::flgEditDialog;
  if (mbr->HasOption(TypeItem::opt_APPLY_IMMED))
    flags_ |= taiData::flgAutoApply;
  if (mbr->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ = flags_ & ~taiData::flgAutoApply;
  if (mbr->HasOption("NO_EDIT_APPLY_IMMED"))
    flags_ |= taiData::flgNoEditDialogAutoApply; // just in case this is needed
  if (mbr->HasOption("NO_ALPHA")) // for color types only, ignored by others
    flags_ |= taiData::flgNoAlpha; // just in case this is needed

  ro = (flags_ & taiData::flgReadOnly); // just for clarity and parity with Image/Value
  taiData* rval = NULL;
  if (ro || !isCondEdit()) { // condedit is irrelevant
    rval = GetArbitrateDataRep(host_, par, gui_parent_, flags_, mbr);
  }
  else { // rw && condEdit
    taiDataDeck* deck = new taiDataDeck(typ, host_, par, gui_parent_, flags_);

    deck->InitLayout();
    // rw child -- always the impl of this guy
    gui_parent_ = deck->GetRep();
    taiData* child = GetArbitrateDataRep(host_, deck, gui_parent_, flags_, mbr);
    deck->AddChildWidget(child->GetRep());
    child->SetMemberDef(mbr); // not used much, ex. used by taiField for edit dialog info

    // ro child, either ro of this guy, or default if we don't handle ro
    flags_ |= taiData::flgReadOnly;
    child = GetArbitrateDataRep(host_, deck, gui_parent_, flags_, mbr);
    deck->AddChildWidget(child->GetRep());
    child->SetMemberDef(mbr); // not used much, ex. used by taiField for edit dialog info
    deck->EndLayout();
    rval = deck;
  }
  rval->SetMemberDef(mbr); // not used much, ex. used by taiField for edit dialog info
  return rval;
}

taiData* taiMember::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_, MemberDef*)
{
  taiData* dat = mbr->type->it->GetDataRep(host_, par, gui_parent_, this, flags_, mbr);
  return dat;
}

//NOTE: we always delegate to _impl because those do the readonly delegation by
// calling the non-impl version of their functions
void taiMember::GetImage(taiData* dat, const void* base) {
  bool is_visible = true;
  if (isCondShow()) {
    //note: we don't care if processed or not -- flag defaults make it editable
    is_visible = mbr->GetCondOptTest("CONDSHOW", typ, base);
    dat->setVisible(is_visible);
  }
  // note: we must always fall through and get image, even when invisible, for
  // when we change visibility, so the result is valid

  // note: must use identical criteria for ro here as when we did GetDataRep
  bool ro = dat->HasFlag(taiData::flgReadOnly) ;

  if (ro || !isCondEdit()) { // condedit is irrelevant
    GetArbitrateImage(dat, base);
  }
  else { // rw && condEdit
    QCAST_MBR_SAFE_EXIT(taiDataDeck*, deck, dat)
    int img = !mbr->GetCondOptTest("CONDEDIT", typ, base);
    deck->GetImage(img); // this is the one that is visible
    // NOTE: we must *always* get both images, so the val img is valid if we switch to rw
    GetArbitrateImage(deck->data_el.FastEl(0), base);
    GetArbitrateImage(deck->data_el.FastEl(1), base);
  }
}

void taiMember::CheckProcessCondEnum(taiTypeOfEnum* et, taiData* dat,
    const void* base)
{
  taiBitBox* bb = dynamic_cast<taiBitBox*>(dat); // should be, except maybe if ro
  if (!bb) return;
  for (int i = 0; i < mbr->type->enum_vals.size; ++i) {
    EnumDef* ed = mbr->type->enum_vals.FastEl(i);
    if (ed->OptionAfter("COND").empty() || ed->HasOption("NO_BIT") || ed->HasOption("IGNORE") ||
      ed->HasOption("NO_SHOW"))
      continue;

    bool is_visible = ed->GetCondOptTest("CONDSHOW", typ, base);
    if (is_visible)
      bb->no_show &= ~ed->enum_no;
    else
      bb->no_show |= ed->enum_no;

    bool is_editable = ed->GetCondOptTest("CONDEDIT", typ, base);
    if (is_editable)
      bb->no_edit &= ~ed->enum_no;
    else
      bb->no_edit |= ed->enum_no;
  }
}

void taiMember::GetImage_impl(taiData* dat, const void* base) {
  mbr->type->it->SetCurParObjType((void*)base, typ);
  // special: if enum type, do the detailed bit-level COND processing
  taiTypeOfEnum* et = dynamic_cast<taiTypeOfEnum*>(mbr->type->it);
  if (et && et->isCond()) {
    CheckProcessCondEnum(et, dat, base);
  }
  mbr->type->it->GetImage(dat, mbr->GetOff(base));
  mbr->type->it->ClearCurParObjType();
  GetOrigVal(dat, base);
}

void taiMember::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
  bool is_visible = true;
  if (isCondShow()) {
    is_visible = mbr->GetCondOptTest("CONDSHOW", typ, base);
    dat->setVisible(is_visible);
  }
  // note: we must always fall through and get value, even when invisible, for
  // when we change visibility, so the result is actually saved!

  // note: must use identical criteria for ro here as when we did GetDataRep
  bool ro = dat->HasFlag(taiData::flgReadOnly);
  if (ro) return; // duh!

  if (!isCondEdit()) {
    // can never be ro! so it was always this one:
    GetArbitrateMbrValue(dat, base, first_diff);
  } else { // note: we only do this if we aren't RO, otherwise there is no point
    QCAST_MBR_SAFE_EXIT(taiDataDeck*, deck, dat)
    // NOTE: we must *always* get the rw val in case we had switched editability
    dat = deck->data_el.FastEl(0);
    GetArbitrateMbrValue(dat, base, first_diff);
  }
  CmpOrigVal(dat, base, first_diff);
}

taiMember::DefaultStatus taiMember::GetDefaultStatus(String memb_val) {
  String defval = mbr->OptionAfter("DEF_");
  if (defval.empty()) return NO_DEF;
  // hack for , in real numbers in international settings
  // note: we would probably never have a variant member with float default
  if (mbr->type->DerivesFrom(TA_float) ||
    mbr->type->DerivesFrom(TA_double))
  {
    memb_val.gsub(",", "."); // does a makeUnique
  }
  return (memb_val == defval) ? EQU_DEF : NOT_DEF;
}

void taiMember::GetOrigVal(taiData* dat, const void* base) {
  dat->orig_val = mbr->type->GetValStr(mbr->GetOff(base));
  // if a default value was specified, compare and set the highlight accordingly
  switch (mbr->GetDefaultStatus(base)) {
  case MemberDef::NOT_DEF: dat->setHighlight(true); break;
  case MemberDef::EQU_DEF: dat->setHighlight(false); break;
  default: break; // compiler food
  }
}

void taiMember::GetMbrValue_impl(taiData* dat, void* base) {
  mbr->type->it->SetCurParObjType(base, typ);
  mbr->type->it->GetValue(dat, mbr->GetOff(base));
  mbr->type->it->ClearCurParObjType();
}

bool taiMember::isCondEdit() const { //return false; //TEMP
  String optedit = mbr->OptionAfter("CONDEDIT_");
  return optedit.nonempty();
}

bool taiMember::isCondShow() const {
  String optedit = mbr->OptionAfter("CONDSHOW_");
  return optedit.nonempty();
}

void taiMember::StartScript(const void* base) {
  if(!taMisc::record_on || !typ->IsActualTaBase())
    return;

  if((((taBase*)base)->GetOwner() == NULL) && ((taBase*)base != tabMisc::root))
    return;     // no record for unowned objs (except root)!
  taBase* tab = (taBase*)base;
  taMisc::record_script << "{ " << typ->name << "* ths = "
                        << tab->GetPathNames() << ";" << "\n";
}

void taiMember::CmpOrigVal(taiData* dat, const void* base, bool& first_diff) {
  if(!taMisc::record_on || !typ->IsActualTaBase())
    return;
  if((((taBase*)base)->GetOwner() == NULL) && ((taBase*)base != tabMisc::root))
    return;     // no record for unowned objs (except root)!
  String new_val = mbr->type->GetValStr(mbr->GetOff(base));
  if(dat->orig_val == new_val)
    return;
  if(first_diff)
    StartScript(base);
  first_diff = false;
  new_val.gsub("\"", "\\\"");   // escape the quotes..
  taMisc::record_script << "  ths->" << mbr->name << " = \""
                        << new_val << "\";" << "\n";
}

void taiMember::AddMember(MemberDef* md) {
  InsertThisIntoBidList(md->im);
}

TypeDef* taiMember::GetTargetType(const void* base) {
  TypeDef* targ_typ = typ; // may get overridden by comment directives
  if (!mbr)  return targ_typ;
  // a XxxDef* can have one of three options to specify the
  // target type for its XxxDef menu.
  // 1) a TYPE_xxxx in its comment directives
  // 2) a TYPE_ON_xxx in is comment directives, specifying the name
  //    of the member in the same object which is a TypeDef*
  // 3) Nothing, which defaults to the type of the object the memberdef
  //      is in.

  String mb_nm = mbr->OptionAfter("TYPE_ON_");
  if(!mb_nm.empty()) {
    if (base) {
      TypeDef* own_td = typ;
      ta_memb_ptr net_mbr_off = 0;      int net_base_off = 0;
      MemberDef* tdmd = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off,
                                                      mb_nm, false); // no warn
      if (tdmd && (tdmd->type->name == "TypeDef_ptr")) {
        targ_typ = *(TypeDef**)(MemberDef::GetOff_static(base, net_base_off, net_mbr_off));
      }
//      else {                 // try fully dynamic
//        void* adr; // discarded
//        tdmd = ((taBase*)base)->FindMembeR(mb_nm, adr); //TODO: highly unsafe cast!!
//        if (tdmd && (tdmd->type == &TA_TypeDef_ptr))
//          targ_typ = *((TypeDef **) tdmd->GetOff(base)); // this is not legal getoff!
//      }
    }
    return targ_typ;
  }

  mb_nm = mbr->OptionAfter("TYPE_");
  if (!mb_nm.empty()) {
    targ_typ = TypeDef::FindGlobalTypeName(mb_nm);
    return targ_typ;
  }
  return targ_typ;
}

taiData* taiMember::GetArbitrateDataRep(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_, MemberDef*)
{
  taiData* rval = NULL;

//  if (HasLowerBidder()) {
//    rval = LowerBidder()->GetDataRep(host_, rval, gui_parent_, NULL, flags_);
//  }
//  else
  {
    bool ro = (flags_ & taiData::flgReadOnly);
    if (ro && !handlesReadOnly()) {
      rval = taiMember::GetDataRep_impl(host_, par, gui_parent_, flags_, mbr);
    } else {
      rval = GetDataRep_impl(host_, par, gui_parent_, flags_, mbr);
    }
  }
  return rval;
}

void taiMember::GetArbitrateImage(taiData* dat, const void* base) {
//  if (HasLowerBidder()) {
//    LowerBidder()->GetImage(dat, base);
//  }
//  else
  {
    bool ro = (dat->HasFlag(taiData::flgReadOnly));
    if (ro && !handlesReadOnly()) {
      taiMember::GetImage_impl(dat, base);
    } else {
      GetImage_impl(dat, base);
    }
  }
}

void taiMember::GetArbitrateMbrValue(taiData* dat, void* base, bool& first_diff) {
//  if (HasLowerBidder()) {
//    LowerBidder()->GetMbrValue(dat, base, first_diff);
//  }
//  else
  {
    bool ro = (dat->HasFlag(taiData::flgReadOnly));
    if (ro && !handlesReadOnly()) {
      taiMember::GetMbrValue_impl(dat, base);
    } else {
      GetMbrValue_impl(dat, base);
    }
  }
}
