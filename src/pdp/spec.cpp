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



// spec.cc

#include "spec.h"
#include "pdpshell.h"
#ifdef TA_GUI
  #include "ta_qtgroup.h"
#endif

#ifdef V3_COMPAT
#include "v3_compat.h"
#endif

//////////////////////////////////
//	BaseSpec_Group		//
//////////////////////////////////

bool BaseSpec_Group::nw_itm_def_arg = false;


void BaseSpec_Group::Initialize() {
  SetBaseType(&TA_BaseSpec);
//  SetAdapter(new BaseSpec_GroupAdapter(this));
}

const iColor* BaseSpec_Group::GetEditColor() {
  return pdpMisc::GetObjColor(GET_MY_OWNER(ProjectBase), el_base);
}

BaseSpec* BaseSpec_Group::FindSpecType(TypeDef* td) {
  // breadth-first search
  BaseSpec* bs;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(bs->GetTypeDef() == td)
      return bs;    // use equals to find type of spec object
  }
  // then check the children
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef())))
      continue;			// no hope..
    BaseSpec* rval = bs->children.FindSpecType(td);
    if(rval != NULL)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecInherits(TypeDef* td, TAPtr for_obj) {
  // breadth-first search
  BaseSpec* bs;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(bs->InheritsFrom(td)) {
      if((for_obj == NULL) || (bs->CheckObjectType_impl(for_obj)))
	return bs;    // object must also be sufficient..
    }
  }
  // then check the children
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef())))
      continue;			// no hope..
    BaseSpec* rval = bs->children.FindSpecInherits(td, for_obj);
    if(rval != NULL)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecTypeNotMe(TypeDef* td, BaseSpec* not_me) {
  // breadth-first search
  BaseSpec* bs;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if((bs->GetTypeDef() == td) && (bs != not_me))
      return bs;    // use equals to find type of spec object
  }
  // then check the children
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef()) || (bs != not_me)))
      continue;			// no hope..
    BaseSpec* rval = bs->children.FindSpecTypeNotMe(td, not_me);
    if(rval != NULL)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecInheritsNotMe(TypeDef* td, BaseSpec* not_me, TAPtr for_obj) {
  // breadth-first search
  BaseSpec* bs;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(bs->InheritsFrom(td) && (bs != not_me)) {
      if((for_obj == NULL) || (bs->CheckObjectType_impl(for_obj)))
	return bs;    // object must also be sufficient..
    }
  }
  // then check the children
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef()) || (bs != not_me)))
      continue;			// no hope..
    BaseSpec* rval = bs->children.FindSpecInheritsNotMe(td, not_me, for_obj);
    if(rval != NULL)
      return rval;
  }
  return NULL;
}


BaseSpec* BaseSpec_Group::FindSpecName(const char* nm) {
  int idx;
  BaseSpec* rval = (BaseSpec*)FindLeafName((char*)nm, idx);
  if(rval != NULL)
    return rval;
  BaseSpec* bs;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    rval = bs->children.FindSpecName(nm);
    if(rval != NULL)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindParent() {
  return GET_MY_OWNER(BaseSpec);
}

/*obs void BaseSpec_Group::GenMenu_impl(taiMenu* menu) {
  PDPMGroup::GenMenu_impl(menu);

  menu->AddSep();
  mc.member = SLOT(NewChildSpec_mc(taiMenuEl*));

  taiMenu* sub = menu->AddSubMenu("New Child");
  itm_list->GetMenu(sub, &mc);
} */
#ifdef TA_GUI
void BaseSpec_Group::NewChildSpec_mc(taiAction* sel) {
/*TODO  if(win_owner == NULL) return;
  if((sel != NULL) && (sel->usr_data != NULL)) {
    TAPtr itm = (TAPtr)sel->usr_data;
    if(itm->InheritsFrom(TA_BaseSpec)) {
      // call with 0 causes menu to come up..
      TAPtr rval = ((BaseSpec*)itm)->children.New(0);
      if(rval != NULL) {
//	taMisc::DelayedMenuUpdate(this);
	itm->UpdateAfterEdit();
      }
    }
  } */
}
#endif
BaseSpec* BaseSpec_Group::FindMakeSpec(const char* nm, TypeDef* tp, bool& nw_itm, const char* alt_nm) {
  nw_itm = false;
  BaseSpec* sp = NULL;
  if(nm != NULL) {
    sp = (BaseSpec*)FindName(nm);
    if((sp == NULL) && (alt_nm != NULL)) {
      sp = (BaseSpec*)FindName(alt_nm);
      if(sp != NULL) sp->name = nm;
    }
  }
  else {
    sp = (BaseSpec*)FindType(tp);
  }
  if(sp == NULL) {
    sp = (BaseSpec*)NewEl(1, tp);
    if(nm != NULL)
      sp->name = nm;
    nw_itm = true;
  }
  else if(!sp->InheritsFrom(tp)) {
    Remove(sp);
    sp = (BaseSpec*)NewEl(1, tp);
    if(nm != NULL)
      sp->name = nm;
    nw_itm = true;
  }
  return sp;
}

bool BaseSpec_Group::RemoveSpec(const char* nm, TypeDef* tp) {
  if(nm != NULL)
    return RemoveName(nm);

  int idx = Find(tp);
  if(idx >= 0)
    return Remove(idx);
  return false;
}

//////////////////////////////////
//	BaseSpec		//
//////////////////////////////////

bool BaseSpec::nw_itm_def_arg = false;

void BaseSpec::Initialize() {
  min_obj_type = &TA_taBase;
  int i;
  for(i=0; i< MAX_SPEC_LONGS; i++)
    unique[i] = 0;
}

void BaseSpec::Copy_(const BaseSpec& cp) {
  //  min_obj_type = cp.min_obj_type;  // don't do this -- could be going between types
  int i;
  for(i=0; i< MAX_SPEC_LONGS; i++)
    unique[i] = cp.unique[i];
  children = cp.children;
}

void BaseSpec::Destroy() {
  CutLinks();
}

void BaseSpec::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(children, this);
  children.SetBaseType(GetTypeDef());
  if(!taMisc::is_loading)
    UpdateSpec();
}

void BaseSpec::CutLinks() {
  children.CutLinks();
  taNBase::CutLinks();
}

void BaseSpec::UpdateAfterEdit() {
  taNBase::UpdateAfterEdit();
  UpdateSpec();
}

BaseSpec* BaseSpec::NewChild() {
  BaseSpec* rval = (BaseSpec*)children.NewEl(1);
  rval->UpdateSpec();
  return rval;
}

BaseSpec* BaseSpec::FindMakeChild(const char* nm, TypeDef* td, bool& nw_itm, const char* alt_nm) {
  if(td == NULL) td = children.el_typ;
  return children.FindMakeSpec(nm, td, nw_itm, alt_nm);
}

BaseSpec* BaseSpec::FindParent() {
  return GET_MY_OWNER(BaseSpec);
}

bool BaseSpec::GetUnique(char* memb_nm) {
  MemberDef* md = FindMember(memb_nm);
  if(md != NULL)
    return GetUnique(md->idx);
  return false;
}

bool BaseSpec::RemoveChild(const char* nm, TypeDef* td) {
  if(td == NULL) td = children.el_typ;
  return children.RemoveSpec(nm, td);
}

void BaseSpec::SetUnique(char* memb_nm, bool onoff) {
  MemberDef* md = FindMember(memb_nm);
  if(md != NULL)
    SetUnique(md->idx, onoff);
}

void BaseSpec::SetUnique(int memb_no, bool onoff) {
  if(memb_no < TA_BaseSpec.members.size)
    return;
  memb_no -= TA_BaseSpec.members.size; // always relative to the evespec..
  int lng_no = memb_no / (sizeof(long) * 8);
  int bit_no = memb_no % (sizeof(long) * 8);
  if(lng_no >= MAX_SPEC_LONGS) {
    taMisc::Error("Number of members", String(memb_no), "exceeds spec limit in",
	     GetTypeDef()->name, "of", String(256));
    return;
  }
  if(onoff)
    unique[lng_no] |= 1 << bit_no;
  else
    unique[lng_no] &= ~(1 << bit_no);
}

bool BaseSpec::GetUnique(int memb_no) {
  if(memb_no < TA_BaseSpec.members.size)
    return false;
  memb_no -= TA_BaseSpec.members.size; // always relative to the evespec..
  int lng_no = memb_no / (sizeof(long) * 8);
  int bit_no = memb_no % (sizeof(long) * 8);
  if(lng_no >= MAX_SPEC_LONGS) {
    taMisc::Error("Number of members", String(memb_no), "exceeds spec limit in",
	     GetTypeDef()->name, "of", String(256));
    return false;
  }
  return unique[lng_no] & (1 << bit_no) ? 1 : 0;
}

void BaseSpec::UpdateSpec() {
  BaseSpec* parent = FindParent();
  if(parent != NULL) {
    TypeDef* td = GetTypeDef();
    int i;
    for(i=TA_BaseSpec.members.size; i< td->members.size; i++)
      UpdateMember(parent, i);
  }
  UpdateSubSpecs();
  UpdateChildren();
}

void BaseSpec::UpdateMember(BaseSpec* from, int memb_no) {
  if((from == NULL) || (memb_no < TA_BaseSpec.members.size))
    return;
  TypeDef* td = GetTypeDef();
  TypeDef* frm_td = from->GetTypeDef();
  if(memb_no < frm_td->members.size) {	// parent must have this member
    MemberDef* md = td->members[memb_no];
    if(frm_td->members[memb_no] == md) { 	// must be the same member
      // don't copy read only or hidden members! (usually set automatically
      // and might depend on local variables)
      if(!GetUnique(memb_no) &&
	 !(md->HasOption("READ_ONLY") || md->HasOption("HIDDEN") ||
	   md->HasOption("NO_INHERIT")))
      {
	if(md->type->InheritsFrom(TA_taList_impl)) {
	  ((taList_impl*)md->GetOff((void*)this))->EnforceSize
	    (((taList_impl*)md->GetOff((void*)from))->size);
	}
	if(md->type->InheritsFrom(TA_taArray_impl)) {
	  ((taArray_impl*)md->GetOff((void*)this))->EnforceSize
	    (((taArray_impl*)md->GetOff((void*)from))->size);
	}
	MemberCopyFrom(memb_no, from);
	DataChanged(DCR_ITEM_UPDATED); //obs tabMisc::NotifyEdits(this);
      }
    }
  }
}

void BaseSpec::UpdateChildren() {
  //NOTE: this routine may no longer be necessary, since the variable not_used_ok was removed
  BaseSpec* kid;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, kid, children., i) {
    kid->UpdateAfterEdit();
  }
}

bool BaseSpec::CheckType(TypeDef* td) {
  if(td == NULL) {
    taMisc::Error("*** For spec:", name, ", NULL type.",
                  "should be at least:", min_obj_type->name);
    return false;
  }
  if(!CheckType_impl(td)) {
    taMisc::Error("*** For spec:", name, ", incorrect type:", td->name,
                   "should be at least:", min_obj_type->name);
    return false;
  }
  return true;
}

bool BaseSpec::CheckObjectType(TAPtr obj) {
  if(obj == NULL) {
    taMisc::Error("*** For spec:", name, ", NULL Object.",
		  "should be at least:", min_obj_type->name);
    return false;
  }
  if(!CheckObjectType_impl(obj)) {
    taMisc::Error("*** For spec:", name, ", incorrect type of obj:", obj->GetPath(),
		   "of type:", obj->GetTypeDef()->name,
		   "should be at least:", min_obj_type->name);
    return false;
  }
  return true;
}

bool BaseSpec::CheckType_impl(TypeDef* td) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if(td->InheritsFrom(TA_BaseSpec) ||
     (td->InheritsFrom(TA_Projection) && InheritsFrom(TA_ConSpec)) ||
     (td->InheritsFrom(TA_Layer) && InheritsFrom(TA_UnitSpec)))
    return true;

  if(!td->InheritsFrom(min_obj_type))
    return false;

  return true;
}

bool BaseSpec::CheckObjectType_impl(TAPtr obj) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if(obj->InheritsFrom(TA_BaseSpec) ||
     (obj->InheritsFrom(TA_Projection) && InheritsFrom(TA_ConSpec)) ||
     (obj->InheritsFrom(TA_Layer) && InheritsFrom(TA_UnitSpec)))
    return true;

  if(!obj->InheritsFrom(min_obj_type))
    return false;

  return true;
}

int BaseSpec::Dump_Save_Value(ostream& strm, TAPtr par, int indent) {
  strm << " {\n";
  String tmpstr = "unique";
  taMisc::fmt_sep(strm, tmpstr, 0, indent+1, 1);
  strm << " = {";

  TypeDef* td = GetTypeDef();
  int i;
  int cnt=0;
  for(i=TA_BaseSpec.members.size; i< td->members.size; i++) {
    if(GetUnique(i)) {
      strm << td->members[i]->name << "; ";
      cnt++;
      if((cnt % 6) == 0) {
	strm << "\n";
	taMisc::indent(strm, indent+2, 1);
      }
    }
  }
  strm << "};\n";

  GetTypeDef()->members.Dump_Save(strm, (void*)this, (void*)par, indent+1);

  return true;
}


int BaseSpec::Dump_Load_Value(istream& strm, TAPtr par) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)    return EOF;
  if(c == ';')    return 2;  // signal that just a path was loaded..
  if(c == '}') {
    if(strm.peek() == ';') strm.get();
    return 2;
  }

  if(c != '{') {
    taMisc::Error("*** Missing '{' in dump file for spec object:",name);
    return false;
  }

  // this is now like MemberSpace::Dump_Load
  c = taMisc::read_word(strm, true);
  if(c == EOF) return EOF;
  if(c == '}') {
    strm.get();			// get the bracket (above was peek)
    if(strm.peek() == ';') strm.get(); // skip past ending semi
    return 2;
  }

  String mb_name = taMisc::LexBuf;
  if(mb_name == "unique") {
    // first turn all others off, since default is off
    int i;
    for(i=TA_BaseSpec.members.size; i<GetTypeDef()->members.size; i++)
      SetUnique(i, false);
    while(true) {
      c = taMisc::skip_white(strm);
      if(c == '=') {
	c = taMisc::read_till_lb_or_semi(strm);
	if(c == '{') {
	  while(true) {
	    c = taMisc::read_till_rb_or_semi(strm);
	    if(c == EOF)  return EOF;
	    if(c == '}')	break;
	    if(c == ';')
	      SetUnique((char*)taMisc::LexBuf, true);
	  }
	  if(c == EOF)	return EOF;
	  break;		// successful
	}
      }
      taMisc::Error("*** Bad 'unique' Formatting in dump file for type:",
		      GetTypeDef()->name);
      taMisc::skip_past_err(strm);
      break;
    }
  }
  else {			// not 'unique' so pass it to std function
    return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par,
					   (const char*)mb_name, c);
  }
  return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
}

//////////////////////////////////
//	BaseSubSpec		//
//////////////////////////////////

void BaseSubSpec::Initialize() {
  int i;
  for(i=0; i< MAX_SPEC_LONGS; i++)
    unique[i] = 0;
}

void BaseSubSpec::Copy_(const BaseSubSpec& cp) {
  int i;
  for(i=0; i< MAX_SPEC_LONGS; i++)
    unique[i] = cp.unique[i];
}

void BaseSubSpec::Destroy() {
}

void BaseSubSpec::InitLinks() {
  taNBase::InitLinks();
  if(!taMisc::is_loading)
    UpdateSpec();
}

void BaseSubSpec::UpdateAfterEdit() {
  taNBase::UpdateAfterEdit();
  UpdateSpec();
}

BaseSpec* BaseSubSpec::FindParentBaseSpec() {
  return GET_MY_OWNER(BaseSpec);
}

BaseSubSpec* BaseSubSpec::FindParent() {
  BaseSpec* bso = FindParentBaseSpec();
  if(bso == NULL)	return NULL;
  BaseSpec* bsoo = bso->FindParent(); // parent's owner
  if(bsoo == NULL)	return NULL;

  String my_path = GetPath(NULL, bso); // get my path to owner..
  MemberDef* md;
  BaseSubSpec* from = (BaseSubSpec*)bsoo->FindFromPath(my_path, md);
  if((from == NULL) || !from->InheritsFrom(TA_BaseSubSpec))
    return NULL;			// corresponding subspec object not found..
  return from;
}

void BaseSubSpec::SetUnique(char* memb_nm, bool onoff) {
  MemberDef* md = FindMember(memb_nm);
  if(md != NULL)
    SetUnique(md->idx, onoff);
}

bool BaseSubSpec::GetUnique(char* memb_nm) {
  MemberDef* md = FindMember(memb_nm);
  if(md != NULL)
    return GetUnique(md->idx);
  return false;
}

void BaseSubSpec::SetUnique(int memb_no, bool onoff) {
  if(memb_no < TA_BaseSubSpec.members.size)
    return;
  memb_no -= TA_BaseSubSpec.members.size; // always relative to the evespec..
  int lng_no = memb_no / (sizeof(long) * 8);
  int bit_no = memb_no % (sizeof(long) * 8);
  if(lng_no >= MAX_SPEC_LONGS) {
    taMisc::Error("Number of members", String(memb_no), "exceeds spec limit in",
	     GetTypeDef()->name, "of", String(256));
    return;
  }
  if(onoff)
    unique[lng_no] |= 1 << bit_no;
  else
    unique[lng_no] &= ~(1 << bit_no);
}

bool BaseSubSpec::GetUnique(int memb_no) {
  if(memb_no < TA_BaseSubSpec.members.size)
    return false;
  memb_no -= TA_BaseSubSpec.members.size; // always relative to the evespec..
  int lng_no = memb_no / (sizeof(long) * 8);
  int bit_no = memb_no % (sizeof(long) * 8);
  if(lng_no >= MAX_SPEC_LONGS) {
    taMisc::Error("Number of members", String(memb_no), "exceeds spec limit in",
	     GetTypeDef()->name, "of", String(256));
    return false;
  }
  return unique[lng_no] & (1 << bit_no) ? 1 : 0;
}

void BaseSubSpec::UpdateSpec() {
  BaseSubSpec* parent = FindParent();
  if(parent != NULL) {
    TypeDef* td = GetTypeDef();
    int i;
    for(i=TA_BaseSubSpec.members.size; i< td->members.size; i++)
      UpdateMember(parent, i);
  }
}

void BaseSubSpec::UpdateMember(BaseSubSpec* from, int memb_no) {
  if((from == NULL) || (memb_no < TA_BaseSubSpec.members.size))
    return;
  TypeDef* td = GetTypeDef();
  TypeDef* frm_td = from->GetTypeDef();
  if(memb_no < frm_td->members.size) {	// parent must have this member
    MemberDef* md = td->members[memb_no];
    if(frm_td->members[memb_no] == md) { 	// must be the same member
      // don't copy read only or hidden members! (usually set automatically
      // and might depend on local variables)
      if(!GetUnique(memb_no) &&
	 !(md->HasOption("READ_ONLY") || md->HasOption("HIDDEN") ||
	   md->HasOption("NO_INHERIT")))
      {
	MemberCopyFrom(memb_no, from);
	DataChanged(DCR_ITEM_UPDATED); //obstabMisc::NotifyEdits(this);
      }
    }
  }
}

int BaseSubSpec::Dump_Save_Value(ostream& strm, TAPtr par, int indent) {
  strm << " {\n";
  String tmpstr = "unique";
  taMisc::fmt_sep(strm, tmpstr, 0, indent+1, 1);
  strm << " = {";

  TypeDef* td = GetTypeDef();
  int i;
  int cnt=0;
  for(i=TA_BaseSubSpec.members.size; i< td->members.size; i++) {
    if(GetUnique(i)) {
      strm << td->members[i]->name << "; ";
      cnt++;
      if((cnt % 6) == 0) {
	strm << "\n";
	taMisc::indent(strm, indent+2, 1);
      }
    }
  }
  strm << "};\n";

  GetTypeDef()->members.Dump_Save(strm, (void*)this, (void*)par, indent+1);

  return true;
}


int BaseSubSpec::Dump_Load_Value(istream& strm, TAPtr par) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)    return EOF;
  if(c == ';')    return 2;  // signal that just a path was loaded..
  if(c == '}') {
    if(strm.peek() == ';') strm.get();
    return 2;
  }

  if(c != '{') {
    taMisc::Error("*** Missing '{' in dump file for spec object:",name);
    return false;
  }

  // this is now like MemberSpace::Dump_Load
  c = taMisc::read_word(strm, true);
  if(c == EOF) return EOF;
  if(c == '}') {
    strm.get();			// get the bracket (above was peek)
    if(strm.peek() == ';') strm.get(); // skip past ending semi
    return 2;
  }

  String mb_name = taMisc::LexBuf;
  if(mb_name == "unique") {
    // first turn all others off, since default is off
    int i;
    for(i=TA_BaseSubSpec.members.size; i<GetTypeDef()->members.size; i++)
      SetUnique(i, false);
    while(true) {
      c = taMisc::skip_white(strm);
      if(c == '=') {
	c = taMisc::read_till_lb_or_semi(strm);
	if(c == '{') {
	  while(true) {
	    c = taMisc::read_till_rb_or_semi(strm);
	    if(c == EOF)  return EOF;
	    if(c == '}')	break;
	    if(c == ';')
	      SetUnique((char*)taMisc::LexBuf, true);
	  }
	  if(c == EOF)	return EOF;
	  break;		// successful
	}
      }
      taMisc::Error("*** Bad 'unique' Formatting in dump file for type:",
		      GetTypeDef()->name);
      taMisc::skip_past_err(strm);
      break;
    }
  }
  else {			// not 'unique' so pass it to std function
    return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par,
					   (const char*)mb_name, c);
  }
  return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
}


//////////////////////////////////
//	SpecPtr_impl		//
//////////////////////////////////

void SpecPtr_impl::Initialize() {
  owner = NULL;
  type = NULL;
  base_type = &TA_BaseSpec;
}

void SpecPtr_impl::Copy_(const SpecPtr_impl& cp) {
  type = cp.type;
  base_type = cp.base_type;
}

void SpecPtr_impl::UpdateAfterEdit() {
  taBase::UpdateAfterEdit();

  if((owner == NULL) || (type == NULL))
    return;

  BaseSpec* sp = GetSpec();
  if(sp != NULL) {
    if(sp->GetTypeDef() == type)
      return;
    else
      SetSpec(NULL);		// get rid of existing spec
  }

  GetSpecOfType();
}

void SpecPtr_impl::SetBaseType(TypeDef* td) {
  type = td;
  base_type = td;		// this doesn't get set by defaults
}

void SpecPtr_impl::SetDefaultSpec(TAPtr ownr, TypeDef* td) {
  if(type == NULL)
    type = td;

  if(base_type == &TA_BaseSpec)
    base_type = td;

  owner = (TAPtr)ownr;
  if(taBase::GetRefn(this) == 0) {
    taBase::Ref(this);		// refer to this object..
  }

  BaseSpec* sp = GetSpec();
  if((sp != NULL) && (sp->GetTypeDef() == type))
    return;

  // this is just like GetSpecOfType(), except it uses inherits!
  // thus, this won't create a new object unless absolutely necessary
  BaseSpec_Group* spgp = GetSpecGroup();
  if(spgp == NULL)
    return;
  // pass the ownr to this, so that min_obj_type can be checked
  sp = spgp->FindSpecInherits(type, owner);
  if((sp != NULL) && sp->InheritsFrom(type)) {
    SetSpec(sp);
    return;
  }

  sp = (BaseSpec*)spgp->NewEl(1, type);
  if(sp != NULL) {
    SetSpec(sp);
    taMisc::DelayedMenuUpdate(sp);
  }
}

BaseSpec_Group* SpecPtr_impl::GetSpecGroup() {
#ifdef V3_COMPAT
  ProjectBase* prj = GET_OWNER(owner,ProjectBase);
  if(prj && prj->InheritsFrom(&TA_Project)) { // a v3 project
    return &(((Project*)prj)->specs);
  }
#endif
  Network* net = GET_OWNER(owner,Network);
  if(net == NULL)
    return NULL;
  return &(net->specs);
}

void SpecPtr_impl::GetSpecOfType() {
  BaseSpec_Group* spgp = GetSpecGroup();
  if(spgp == NULL)
    return;
  BaseSpec* sp = spgp->FindSpecType(type);
  if((sp != NULL) && (sp->GetTypeDef() == type)) {
    SetSpec(sp);
    return;
  }

  sp = (BaseSpec*)spgp->NewEl(1, type);
  if(sp != NULL) {
    SetSpec(sp);
    taMisc::DelayedMenuUpdate(sp);
  }
}


