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

#include "TypeDef.h"
#include <MemberDef>
#include <MethodDef>
#include <EnumDef>
#include <taHashTable>
#include <taMisc>

TypeDef* TypeDef::GetCommonSubtype(TypeDef* typ1, TypeDef* typ2) {
  // search up typ1's tree until a common subtype is found
  // note: doesn't matter which obj's tree we go up, so we just pick typ1
  TypeDef* rval = typ1;
  while (rval && (!typ2->InheritsFrom(rval))) {
    // note: we only search up the primary inheritance hierarchy
    rval = rval->parents.SafeEl(0);
  }
  return rval;
}

void TypeDef::Initialize() {
  owner = NULL;
  size = 0;
  ptr = 0;
  ref = false;
  internal = false;
  formal = false;
  source_start = -1;
  source_end = -1;

#ifdef TA_GUI
  it = NULL;
  ie = NULL;
  iv = NULL;
#endif
#ifdef NO_TA_BASE
  pre_parsed = false;   // true if previously parsed by maketa
#else
  is_subclass = false;
  plugin = NULL; // set true by TypeSpace::SetOwner if initing a plugin
  instance = NULL;
  defaults = NULL;
  schema = NULL;
#endif

  parents.name = "parents";
  parents.owner = this;
  par_formal.name = "par_formal";
  par_formal.owner = this;
  par_cache.name = "par_cache";
  par_cache.owner = this;
  children.name = "children";
  children.owner = this;
  tokens.name = "tokens";
  tokens.owner = this;

  enum_vals.name = "enum_vals";
  enum_vals.owner = this;
  sub_types.name = "sub_types";
  sub_types.owner = this;
  members.name = "members";
  members.owner = this;
  properties.name = "properties";
  properties.owner = this;
  methods.name = "methods";
  methods.owner = this;
  templ_pars.name = "templ_pars";
  templ_pars.owner = this;
  m_cacheInheritsNonAtomicClass = 0;
#if (!defined(NO_TA_BASE) && defined(DMEM_COMPILE))
  dmem_type = NULL;
#endif
}

TypeDef::TypeDef()
:inherited()
{
  Initialize();
}

TypeDef::TypeDef(const char* nm)
:inherited()
{
  Initialize();
  name = nm;
}

#ifdef NO_TA_BASE
TypeDef::TypeDef(const char* nm, const char* dsc, const char* inop, const char* op,
		 const char* lis,
		 uint siz, int ptrs, bool refnc, bool global_obj)
#else
TypeDef::TypeDef(const char* nm, const char* dsc, const char* inop, const char* op,
		 const char* lis, const char* src_file, int src_st, int src_ed,
		 uint siz, void** inst, bool toks, int ptrs, bool refnc, bool global_obj)
#endif
:inherited()
{
  Initialize();
#ifndef NO_TA_BASE
  instance = inst;
  tokens.keep = toks;
  source_file = src_file;
  source_start = src_st;
  source_end = src_ed;
#endif
  name = nm; desc = dsc;
  c_name = nm;
  taMisc::CharToStrArray(opts,op);
  taMisc::CharToStrArray(inh_opts,inop);
  taMisc::CharToStrArray(lists,lis);
#ifndef NO_TA_BASE
  CleanupCats(true);            // save the last one for initialization
#endif
  size = siz;
  ptr = ptrs;
  ref = refnc;
  if(global_obj)
    taRefN::Ref(this);          // reference if static (non-new'ed) global object
}

TypeDef::TypeDef(const char* nm, bool intrnl, int ptrs, bool refnc, bool forml,
                 bool global_obj, uint siz, const char* c_nm
)
:inherited()
{
  Initialize();
  name = nm; internal = intrnl; ptr = ptrs; ref = refnc; formal = forml;
  size = siz; // note: may get updated later
  if (c_nm) c_name = c_nm;
  else c_name = name;
  if(ptr > 0) size = sizeof(void*);
  if(global_obj)
    taRefN::Ref(this);          // reference if static (non-new'ed) global object
}

TypeDef::TypeDef(const TypeDef& cp)
:inherited(cp)
{
  Initialize();
  Copy_(cp);
}

void TypeDef::Copy(const TypeDef& cp) {
  inherited::Copy(cp);
  Copy_(cp);
}

void TypeDef::Copy_(const TypeDef& cp) {
#ifdef NO_TA_BASE
  pre_parsed    = cp.pre_parsed;
#else
  is_subclass   = cp.is_subclass;
  plugin = cp.plugin;
  instance = cp.instance ;
  //TODO: copy the schema
// don't copy the tokens..
#endif
  c_name        = cp.c_name;
  size          = cp.size    ;
  ptr           = cp.ptr     ;
  ref           = cp.ref     ;
  internal      = cp.internal;
  formal        = cp.formal  ;

  inh_opts      = cp.inh_opts ;

  parents       = cp.parents  ;
  par_formal    = cp.par_formal;
  par_cache     = cp.par_cache;
  children      = cp.children ; // not sure about this one..

// don't copy the it's
//   it         = cp.it       ;
//   ie         = cp.ie      ;
// or the defaults
//  defaults    = cp.defaults ;

  enum_vals     = cp.enum_vals;
  //  sub_types         = cp.sub_types;
  sub_types.Duplicate(cp.sub_types);// important: add to subtypes..
  members       = cp.members;
  properties    = cp.properties;
  methods       = cp.methods;
  templ_pars    = cp.templ_pars;

  sub_types.ReplaceParents(cp.sub_types, sub_types); // make our sub types consistent
  DuplicateMDFrom(&cp);         // duplicate members owned by source
  UpdateMDTypes(cp.sub_types, sub_types); // since sub-types are new, point to them
}

TypeDef::~TypeDef() {
#ifndef NO_TA_BASE
# ifdef DMEM_COMPILE
  if (dmem_type) {
    delete (MPI_Datatype_PArray*)dmem_type;
    dmem_type = NULL;
  }
# endif
  if (defaults) {
    taBase::UnRef(defaults);
    defaults = NULL;
  }
  if (schema) {
    delete schema;
    schema = NULL;
  }
# ifndef NO_TA_GUI
  taRefN::SafeUnRefDone(it); it = NULL;
  if (ie) {delete ie; ie = NULL;}
  if (iv) {delete iv; iv = NULL;}
# endif // !NO_TA_GUI
#endif // !NO_TA_BASE
  if((owner == &taMisc::types) && !taMisc::not_constr) // destroying..
    taMisc::not_constr = true;
}

#ifndef NO_TA_BASE
void TypeDef::AddUserDataSchema(UserDataItemBase* item) {
  if (!schema) {
    schema = new UserDataItem_List;
  }
  schema->Add(item);
}
#endif

void TypeDef::CleanupCats(bool save_last) {
  if(save_last) {
    bool got_op = false;
    for(int i=opts.size-1; i>=0; i--) {
      String op = opts[i];
      if(!op.contains("CAT_")) continue;
      if(got_op) {
        opts.RemoveIdx(i);
      } // remove all other previous ones
      else got_op = true;
    }
    got_op = false;
    for(int i=inh_opts.size-1; i>=0; i--) {
      String op = inh_opts[i];
      if(!op.contains("CAT_")) continue;
      if(got_op) {
        inh_opts.RemoveIdx(i);
      } // remove all other previous ones
      else got_op = true;
    }
  }
  else {                        // save first
    bool got_op = false;
    for(int i=0; i< opts.size;i++) {
      String op = opts[i];
      if(!op.contains("CAT_")) continue;
      if(got_op) { opts.RemoveIdx(i); i--; }    // remove all other previous ones
      else got_op = true;
    }
    got_op = false;
    for(int i=0; i<inh_opts.size;i++) {
      String op = inh_opts[i];
      if(!op.contains("CAT_")) continue;
      if(got_op) {inh_opts.RemoveIdx(i); i--; } // remove all other previous ones
      else got_op = true;
    }
  }
}

void TypeDef::DuplicateMDFrom(const TypeDef* old) {
  int i;
  for(i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    if(md->owner == &(old->members))
      members.ReplaceIdx(i, md->Clone());
  }
  for(i=0; i<methods.size; i++) {
    MethodDef* md = methods.FastEl(i);
    if(md->owner == &(old->methods))
      methods.ReplaceIdx(i, md->Clone());
  }
}

bool TypeDef::InheritsNonAtomicClass() const {
  if (m_cacheInheritsNonAtomicClass == 0) {
    // set cache
    m_cacheInheritsNonAtomicClass = (InheritsFormal(TA_class)
        && !InheritsFrom(TA_taString)
        && !InheritsFrom(TA_Variant)
#ifndef NO_TA_BASE
        && !InheritsFrom(TA_taSmartPtr)
        && !InheritsFrom(TA_taSmartRef)
# if TA_USE_QT
        && !InheritsFrom(TA_QAtomicInt)
# endif
#endif
    ) ? 1 : -1;
  }
  return (m_cacheInheritsNonAtomicClass == 1);
}

bool TypeDef::IsBasePointerType() const {
#ifndef NO_TA_BASE
  if(((ptr == 1) && DerivesFrom(TA_taBase)) ||
     ((ptr == 0) && (DerivesFrom(TA_taSmartPtr) ||
                     DerivesFrom(TA_taSmartRef))) )
    return true;
#endif
  return false;
}

TypeDef* TypeDef::GetStemBase() const {
  if(HasOption("STEM_BASE")) return const_cast<TypeDef*>(this);
  // first breadth
  for(int i=0; i < parents.size; i++) {
    TypeDef* par = parents.FastEl(i);
    if(par->HasOption("STEM_BASE"))
      return par;
  }
  // then depth recursion
  for(int i=0; i < parents.size; i++) {
    TypeDef* rval = parents.FastEl(i)->GetStemBase();
    if(rval) return rval;
  }
  return NULL;
}

void TypeDef::UpdateMDTypes(const TypeSpace& ol, const TypeSpace& nw) {
  int i;
  for(i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    if(md->owner != &members)   // only for members we own
      continue;

    int j;
    for(j=0; j<ol.size; j++) {
      TypeDef* old_st = ol.FastEl(j);
      TypeDef* new_st = nw.FastEl(j);   // assumes one-to-one correspondence

      if(md->type == old_st)
        md->type = new_st;
    }
  }
  for(i=0; i<methods.size; i++) {
    MethodDef* md = methods.FastEl(i);
    if(md->owner != &methods)
      continue;

    int j;
    for(j=0; j<ol.size; j++) {
      TypeDef* old_st = ol.FastEl(j);
      TypeDef* new_st = nw.FastEl(j);   // assumes one-to-one correspondence

      if(md->type == old_st)
        md->type = new_st;

      md->arg_types.ReplaceLinkAll(old_st, new_st);
    }
  }
}

TypeDef*  TypeDef::FindTypeWithMember(const char* nm, MemberDef** md){
  *md = members.FindName(nm);
  if (*md) return this;

  for (int i = 0; i < children.size; i++) {
    if (TypeDef *td = children[i]->FindTypeWithMember(nm,md)) {
      return td;
    }
  }
  return NULL;
}

bool TypeDef::CheckList(const String_PArray& lst) const {
  for (int i = 0; i < lists.size; i++) {
    if (lst.FindEl(lists.FastEl(i)) >= 0)
      return true;
  }
  return false;
}


TypeDef* TypeDef::GetNonPtrType() const {
  if(ptr == 0)    return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while(rval->GetParent() != NULL) {
    rval = rval->GetParent();
    if(rval->ptr == 0)
      return rval;
  }
  return rval;                  // always return something
}

TypeDef* TypeDef::GetNonRefType() const {
  if(!ref)    return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while(rval->GetParent() != NULL) {
    rval = rval->GetParent();
    if(!rval->ref)
      return rval;
  }
  return rval;                  // always return something
}

TypeDef* TypeDef::GetTemplType() const {
  if(InheritsFormal(TA_template)) return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while((rval = rval->GetParent()) != NULL) {
    if(rval->InheritsFormal(TA_template))
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetTemplInstType() const {
  if(InheritsFormal(TA_templ_inst)) return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while((rval = rval->GetParent()) != NULL) {
    if(rval->InheritsFormal(TA_templ_inst))
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetNonConstType() const {
  if(!DerivesFrom(TA_const))    return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while((rval = rval->parents.Peek()) != NULL) { // use the last parent, not the 1st
    if(!rval->DerivesFrom(TA_const))
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetNonConstNonRefType() const {
  if(!(DerivesFrom(TA_const) || ref))  return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while((rval = rval->parents.Peek()) != NULL) { // use the last parent, not the 1st
    if (!(rval->DerivesFrom(TA_const) || rval->ref))
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetClassType() const {
  if(InheritsFormal(&TA_class)) return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while((rval = rval->parents.Peek()) != NULL) { // use the last parent, not the 1st
    if(rval->InheritsFormal(&TA_class))
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetPluginType() const {
#ifdef NO_TA_BASE
  return NULL;
#else
  return plugin;
#endif
}

String TypeDef::GetPtrString() const {
  String rval; int i;
  for(i=0; i<ptr; i++) rval += "*";
  return rval;
}


String TypeDef::Get_C_Name() const {
  String rval;
  if(ref) {
    TypeDef *nrt = GetNonRefType();
    if (!nrt) {
      //taMisc::Error("Null NonRefType in TypeDef::Get_C_Name()", name);
      return name;
    }
    if (nrt == this) return name + "&";
    rval = nrt->Get_C_Name() + "&";
    return rval;
  }

  if (ptr > 0) {
    TypeDef *npt = GetNonPtrType();
    if (!npt) {
      //taMisc::Error("Null NonPtrType in TypeDef::Get_C_Name()", name);
      return name;
    }
    if (npt == this) return name + GetPtrString();
    rval = npt->Get_C_Name() + GetPtrString();
    return rval;
  }

  // combo type
  if ((parents.size > 1) && !InheritsFormal(TA_class)) {
    for (int i = 0; i < parents.size; i++) {
      TypeDef* pt = parents.FastEl(i);
      rval += pt->Get_C_Name();
      if (i < parents.size-1) rval += " ";
    }
    return rval;
  }

  // on some local list and not internal
  // (which were not actually delcared at this scope anyway)
  if (!(internal) && (owner != NULL) && (owner->owner != NULL) && (owner->owner != this)) {
    rval = owner->owner->Get_C_Name() + "::";
  }

  if (InheritsFormal(TA_templ_inst) && (templ_pars.size > 0)) {
    TypeDef* tmpar = GetTemplParent();
    if (!tmpar) {
      taMisc::Error("Null TemplParent in TypeDef::Get_C_Name()", name);
      return name;
    }
    rval += tmpar->name + "<"; // note: name is always its valid c_name
    for (int i = 0; i < templ_pars.size; i++) {
      rval += templ_pars.FastEl(i)->Get_C_Name();
      if (i < templ_pars.size-1)
        rval += ",";
    }
    rval += ">";
    return rval;
  }

  //note: normally, c_name should be valid, but may be cases, ex. templates, dynamic types, etc
  // where c_name was not set or updated, so most contexts the name is the same
  if (c_name.empty())
    rval += name;                       // the default
  else
    rval += c_name;                     // the default
  return rval;
}

TypeDef* TypeDef::AddParent(TypeDef* it, int p_off) {
  if(parents.LinkUnique(it))
    par_off.Add(p_off);         // it was unique, add offset
  // only add to children if not internal (except when parent is)
  bool templ = InheritsFormal(TA_template); // cache
  if (templ || (!internal || it->internal))
    it->children.Link(this);
  // since templs don't call AddParClass, we have to determine
#ifndef NO_TA_BASE
  // if it is a subclass here...
  if (templ && it->InheritsFormal(TA_class))
    is_subclass = true;
#endif

  opts.DupeUnique(it->inh_opts);
  inh_opts.DupeUnique(it->inh_opts);    // and so on

  if(InheritsFrom(TA_taBase))
    opts.AddUnique(opt_instance);       // ta_bases always have an instance

#ifndef NO_TA_BASE
  CleanupCats(false);           // save first guy for add parent!
#endif

  // no need to get all this junk for internals
  if(internal && !templ) return it;

  // use the old one because the parent does not have precidence over existing
  enum_vals.BorrowUniqNameOld(it->enum_vals);
  sub_types.BorrowUniqNameOld(it->sub_types);
  members.BorrowUniqNameOld(it->members);
  properties.BorrowUniqNameOld(it->properties);
  methods.BorrowUniqNameOld(it->methods);
  return it;
}

void TypeDef::AddParents(TypeDef* p1, TypeDef* p2, TypeDef* p3, TypeDef* p4,
                         TypeDef* p5, TypeDef* p6) {
  if(p1 != NULL)    AddParent(p1);
  if(p2 != NULL)    AddParent(p2);
  if(p3 != NULL)    AddParent(p3);
  if(p4 != NULL)    AddParent(p4);
  if(p5 != NULL)    AddParent(p5);
  if(p6 != NULL)    AddParent(p6);
}

void TypeDef::AddClassPar(TypeDef* p1, int p1_off, TypeDef* p2, int p2_off,
                          TypeDef* p3, int p3_off, TypeDef* p4, int p4_off,
                          TypeDef* p5, int p5_off, TypeDef* p6, int p6_off)
{
#ifndef NO_TA_BASE
  is_subclass = true;
#endif
  bool mi = false;
  if(p1 != NULL)    AddParent(p1,p1_off);
  if(p2 != NULL)    { AddParent(p2,p2_off); mi = true; }
  if(p3 != NULL)    AddParent(p3,p3_off);
  if(p4 != NULL)    AddParent(p4,p4_off);
  if(p5 != NULL)    AddParent(p5,p5_off);
  if(p6 != NULL)    AddParent(p6,p6_off);

  if(mi)            ComputeMembBaseOff();
}

void TypeDef::AddParFormal(TypeDef* p1, TypeDef* p2, TypeDef* p3, TypeDef* p4,
                           TypeDef* p5, TypeDef* p6) {
  if(p1 != NULL)    par_formal.LinkUnique(p1);
  if(p2 != NULL)    par_formal.LinkUnique(p2);
  if(p3 != NULL)    par_formal.LinkUnique(p3);
  if(p4 != NULL)    par_formal.LinkUnique(p4);
  if(p5 != NULL)    par_formal.LinkUnique(p5);
  if(p6 != NULL)    par_formal.LinkUnique(p6);
}

void TypeDef::AddTemplPars(TypeDef* p1, TypeDef* p2, TypeDef* p3, TypeDef* p4,
                           TypeDef* p5, TypeDef* p6) {
  if(p1 != NULL)    templ_pars.Link(p1);
  if(p2 != NULL)    templ_pars.Link(p2);
  if(p3 != NULL)    templ_pars.Link(p3);
  if(p4 != NULL)    templ_pars.Link(p4);
  if(p5 != NULL)    templ_pars.Link(p5);
  if(p6 != NULL)    templ_pars.Link(p6);
}

void TypeDef::CacheParents() {
  par_cache.Reset();            // justin case
  for(int i=0; i<parents.size; i++) {
    parents.FastEl(i)->CacheParents_impl(this);
  }
  par_cache.BuildHashTable(par_cache.size + 2, taHashTable::KT_PTR); // little extra, cache on pointer vals
}

void TypeDef::CacheParents_impl(TypeDef* src_typ) {
  src_typ->par_cache.LinkUnique(this);
  for(int i=0; i<parents.size; i++) {
    parents.FastEl(i)->CacheParents_impl(src_typ);
  }
}

void TypeDef::ComputeMembBaseOff() {
  int i;
  for(i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    TypeDef* mo = md->GetOwnerType();

    if((mo == this) || (mo == NULL))
      continue;

    int base_off = GetParOff(mo);
    if(base_off > 0) {          // only those that need it!
      MemberDef* nmd = md->Clone();
      nmd->base_off = base_off;
      members.ReplaceIdx(i, nmd);
    }
    else if(base_off < 0) {
      taMisc::Error("ComputeMembBaseOff(): parent type not found:",mo->name,
                     "in type of:", name);
    }
  }
}

bool TypeDef::FindChildName(const char* nm) const {
  if (children.FindName(nm))
    return true;
  for (int i = 0; i < children.size; i++) {
    if (children.FastEl(i)->FindChildName(nm))
      return true;
  }
  return false;
}
bool TypeDef::FindChild(TypeDef* it) const {
  if (children.FindEl(it) >= 0)
    return true;
  for (int i = 0; i < children.size; i++) {
    if (children.FastEl(i)->FindChild(it))
      return true;
  }
  return false;
}

void* TypeDef::GetParAddr(const char* it, void* base) const {
  if (name == it) return base;   // you are it!
  int anidx = parents.FindNameIdx(it);
  if (anidx >= 0) {
    return (void*)((char*)base + par_off[anidx]);
  }
  for (int i = 0; i < parents.size; i++) {
    void* nw_base = (void*)((char*)base + par_off[i]);
    void* rval = parents.FastEl(i)->GetParAddr(it, nw_base);
    if (rval) return rval;
  }
  return NULL;
}

void* TypeDef::GetParAddr(TypeDef* it, void* base) const {
  if (it == this) return base;     // you are it!
  int anidx = parents.FindEl(it);
  if (anidx >= 0) {
    return (void*)((char*)base + par_off[anidx]);
  }
  for (int i = 0; i < parents.size; i++) {
    void* nw_base = (void*)((char*)base + par_off[i]);
    void* rval = parents.FastEl(i)->GetParAddr(it, nw_base);
    if (rval) return rval;
  }
  return NULL;
}


int TypeDef::GetParOff(TypeDef* it, int boff) const {
  int use_boff = (boff >= 0) ? boff : 0;
  if (it == this) return use_boff; // you are it!
  int anidx = parents.FindEl(it);
  if (anidx >= 0) {
    return use_boff + par_off[anidx];
  }
  for (int i = 0; i < parents.size; i++) {
    int rval = parents.FastEl(i)->GetParOff(it, use_boff + par_off[i]);
    if (rval >= 0) return rval;
  }
  return -1;
}

const String TypeDef::GetPathName() const {
//TODO: this routine may not even be used!
  // are we owned?
  // are we an EnumDef?

  String rval;
  TypeDef* owtp = GetOwnerType();
  if (owtp) {
    rval = owtp->GetPathName() + "::";
  }
  rval += name;
  return rval;
}

TypeDef* TypeDef::GetPtrType() const {
  TypeDef* rval = children.FindName(name + "_ptr");
  if (rval) {
    // make sure its ptr count is one more than ours!
    if (rval->ptr != (ptr + 1)) {
      rval = NULL;
    }
  }
  if (!rval) {
    // need to make one, we use same pattern as maketa
    rval = new TypeDef(name + "_ptr", internal, ptr + 1, 0, 0, 0);
    taMisc::types.Add(rval);
    // unconstify us, this is an internal operation, still considered "const" access
    rval->AddParent(const_cast<TypeDef*>(this));
  }
  return rval;
}

String TypeDef::GetTemplName(const TypeSpace& inst_pars) const {
  String rval = name;
  int i;
  for(i=0; i<inst_pars.size; i++) {
    rval += String("_") + inst_pars.FastEl(i)->name + "_";
  }
  return rval;
}

TypeDef* TypeDef::GetTemplParent() const {
  int i;
  for(i=0; i<parents.size; i++) {
    if(parents.FastEl(i)->InheritsFormal(TA_template))
      return parents.FastEl(i);
  }
  return NULL;
}

bool TypeDef::IgnoreMeth(const String& nm) const {
  if(!InheritsFormal(TA_class))
    return false;
  if(ignore_meths.FindEl(nm) >= 0)
    return true;

  int i;
  for(i=0; i<parents.size; i++) {
    if(parents.FastEl(i)->IgnoreMeth(nm))
      return true;
  }
  return false;
}

bool TypeDef::IsAnchor() const {
  return ((ptr == 0) && !ref && !DerivesFrom(TA_const)
    && (enum_vals.size == 0));
}

bool TypeDef::IsClass() const {
  return InheritsFormal(TA_class);
}

bool TypeDef::IsEnum() const {
  return (enum_vals.size > 0);
}

MemberDef* TypeDef::FindMemberPathStatic(TypeDef*& own_td, int& net_base_off,
                                         ta_memb_ptr& net_mbr_off,
                                         const String& path, bool warn) {
  void* cur_base_off = NULL;
  net_mbr_off = 0;
  if(!own_td || path.empty()) {
    return NULL;                // no warning..
  }
  String pth = path;
  while(pth.contains('.')) {
    String bef = pth.before('.');
    pth = pth.after('.');
    MemberDef* md = own_td->members.FindName(bef);
    if(!md) {
      if(warn) {
        taMisc::Warning("FindMemberPathStatic: member:", bef, "not found in object type:",
                        own_td->name);
      }
      return NULL;
    }
    else {
      own_td = md->type;
      cur_base_off = md->GetOff(cur_base_off);
      net_mbr_off = *((ta_memb_ptr*)&cur_base_off);
    }
  }
  MemberDef* md = own_td->members.FindName(pth);
  if(!md) {
    if(warn) {
      taMisc::Warning("FindMemberPathStatic: member:", pth, "not found in object type:",
                      own_td->name);
    }
    return NULL;
  }
  cur_base_off = md->GetOff(cur_base_off);
  net_mbr_off = *((ta_memb_ptr*)&cur_base_off);
  return md;
}

EnumDef* TypeDef::FindEnum(const String& nm) const {
  EnumDef* rval = enum_vals.FindName(nm);
  if (rval) return rval;

  for (int i = 0; i < sub_types.size; i++) {
    TypeDef* td = sub_types.FastEl(i);
    if (td->InheritsFormal(TA_enum)) {
      rval = td->FindEnum(nm);
      if (rval) return rval;
    }
  }

  // try to look for an aka now..
  for (int i = 0; i < enum_vals.size; i++) {
    EnumDef* ed = enum_vals.FastEl(i);
    String aka = ed->OptionAfter("AKA_");
    if (aka.empty()) continue;
    if (aka == nm) return ed;
  }

  return NULL;
}

int TypeDef::GetEnumVal(const String& nm, String& enum_tp_nm) const {
  EnumDef* rval = FindEnum(nm);
  if (rval != NULL) {
    if ((rval->owner != NULL) && (rval->owner->owner != NULL))
      enum_tp_nm = rval->owner->owner->name;
    return rval->enum_no;
  }
  return -1;
}

String TypeDef::GetEnumString(const String& enum_tp_nm, int enum_val) const {
  EnumDef* rval;
  if(enum_vals.size > 0) {
    rval = enum_vals.FindNo(enum_val);
    if(rval != NULL) return rval->name;
  }
  int i;
  for(i=0; i < sub_types.size; i++) {
    TypeDef* td = sub_types.FastEl(i);
    if(td->InheritsFormal(TA_enum) && (enum_tp_nm.empty() || (td->name == enum_tp_nm))) {
      rval = td->enum_vals.FindNo(enum_val);
      if(rval != NULL) return rval->name;
    }
  }
  return "";
}

const String TypeDef::Get_C_EnumString(int enum_val, bool show_scope) const {
  STRING_BUF(rval, 80); // extends if needed

  // If rendering string for CSS, need to scope the enum value properly.
  String scope_prefix;
  if (show_scope) {
    // Enum values are scoped using the enclosing class.
    if (TypeDef* par_td = GetOwnerType()) {
      scope_prefix = par_td->GetPathName() + "::";
    }
  }

  // If this enum type is marked #BITS, then render the set of enabled bits.
  if (HasOption(opt_bits)) {
    for (int i = 0; i < enum_vals.size; ++i) {
      EnumDef* ed = enum_vals[i];
      if (ed->HasOption("NO_BIT") || ed->HasOption("NO_SAVE")) continue;
      if (enum_val & ed->enum_no) {
        if (!rval.empty()) rval.cat("|");
        rval.cat(scope_prefix).cat(ed->name);
      }
    }

    // Probably better to reutrn "0" than an empty string if no bits were set.
    if (!rval.empty()) {
      return rval;
    }
  }
  // Not a #BITS enum, so just look for the single value that matches.
  else if (EnumDef* ed = enum_vals.FindNo(enum_val)) {
    rval.cat(scope_prefix).cat(ed->name);
    return rval;
  }

  // Unable to render as BITS or as value, so just spit out the raw value.
  // If outputting for CSS, need to provide a cast.
  if (show_scope) {
    rval.cat("((").cat(scope_prefix).cat(name).cat(")")
        .cat(String(enum_val)).cat(")");
    return rval;
  }

  // Otherwise just stringify the value.
  return String(enum_val);
}

String TypeDef::GetEnumPrefix() const {
  String prfx;
  for(int i=0;i<enum_vals.size;i++) {
    EnumDef* ed = enum_vals.FastEl(i);
    if(prfx.nonempty()) {
      if(ed->name.startsWith(prfx)) continue;
      return _nilString;
    }
    if(!ed->name.contains('_')) return _nilString;
    prfx = ed->name.through('_');
  }
  return prfx;
}

#ifndef NO_TA_BASE
int TypeDef::FindTokenR(void* addr, TypeDef*& aptr) const {
  int rval = tokens.FindEl(addr);
  if (rval >= 0) {
    aptr = const_cast<TypeDef*>(this);
    return rval;
  }

  // depth-first...
  for (int i = 0; i < children.size; i++) {
    rval = children.FastEl(i)->FindTokenR(addr, aptr);
    if (rval >= 0) return rval;
  }
  aptr = NULL;
  return -1;
}

int TypeDef::FindTokenR(const char* nm, TypeDef*& aptr) const {
  int rval = tokens.FindNameIdx(nm);
  if (rval >= 0) {
    aptr = const_cast<TypeDef*>(this);
    return rval;
  }

  // depth-first...
  for (int i = 0; i < children.size; i++) {
    rval = children.FastEl(i)->FindTokenR(nm, aptr);
    if (rval >= 0) return rval;
  }
  aptr = NULL;
  return -1;
}
#endif // ndef NO_TA_BASE

bool TypeDef::HasEnumDefs() const {
  for (int i = 0; i < sub_types.size; ++i) {
    TypeDef* td = sub_types.FastEl(i);
    if (td->enum_vals.size > 0) return true;
  }
  return false;
}

bool TypeDef::HasSubTypes() const {
  bool rval = false;
  for (int i = 0; i < sub_types.size; ++i) {
    TypeDef* td = sub_types.FastEl(i);
    if (td->enum_vals.size == 0) {
      rval = true;
      break;
    }
  }
  return rval;
}

/*obs void TypeDef::Register(void* it) {
  if(taMisc::in_init)           // don't register the instance tokens
    return;
  if((taMisc::keep_tokens != taMisc::ForceTokens) &&
     (!tokens.keep || (taMisc::keep_tokens == taMisc::NoTokens)))
    return;

  TypeDef* par = GetParent();   // un-register from parent..
  int pos;
  if(par && (par->tokens.keep ||
             (taMisc::keep_tokens == taMisc::ForceTokens))
     && ((pos = par->tokens.FindEl(it)) >= 0))
  {
    par->tokens.RemoveIdx(pos);
    par->tokens.sub_tokens.ref();       // sub class got a new token..
  }
  if(par)                       // only register if you have a parent...
    tokens.Link(it);
}*/

void TypeDef::RegisterFinal(void* it) {
  if(taMisc::in_init)           // don't register the instance tokens
    return;
  if((taMisc::keep_tokens != taMisc::ForceTokens) &&
     (!tokens.keep || (taMisc::keep_tokens == taMisc::NoTokens)))
    return;

  TypeDef* par = GetParent();
  if (!par) return; // we only register if have parent
#if defined(DEBUG) && !defined(NO_TA_BASE) // semi-TEMP, until 100% verified
  if (par->tokens.FindEl(it) >= 0) {
    String msg;
    msg << "attempt to reregister token of type(addr): " <<     ((taBase*)it)->GetTypeDef()->name << "(" << it << ")\n";
    taMisc::Info(msg);
    return;
  }
#endif
  tokens.Link(it);
  while (par) {
    par->tokens.sub_tokens.ref();       // sub class got a new token..
    par = par->GetParent();
  }
}

bool TypeDef::ReplaceParent(TypeDef* old_tp, TypeDef* new_tp) {
  int anidx = parents.FindEl(old_tp);
  if (anidx >= 0) {
    parents.ReplaceLinkIdx(anidx, new_tp);
    name.gsub(old_tp->name, new_tp->name);
    return true;
  }
  bool rval = false;
  for (int i = 0; i < parents.size; i++) {
    if (parents.FastEl(i)->ReplaceParent(old_tp, new_tp)) {
      rval = true;
      name.gsub(old_tp->name, new_tp->name); // updt name at all levels
    }
  }
  return rval;
}

void TypeDef::SetTemplType(TypeDef* templ_par, const TypeSpace& inst_pars) {
  if(inst_pars.size != templ_pars.size) {
    String defn_no(templ_pars.size);
    String inst_no(inst_pars.size);
    taMisc::Error("Template",name,"defined with",defn_no,"parameters, instantiated with",
                   inst_no);
    String msg;
    msg << "Defined with parameters: ";
    templ_pars.Print(msg);
    msg << "\nInstantiated with parameters: ";
    inst_pars.Print(msg);
    taMisc::Error(msg);
    return;
  }

  parents.Reset();                      // bag the template's parents
  parents.LinkUnique(templ_par);        // parent is the templ_par
  par_formal.RemoveEl(&TA_template);
  par_formal.Link(&TA_templ_inst);      // now a template instantiation
  templ_par->children.LinkUnique(this);
  internal = false;                     // not internal any more
  children.Reset();                     // don't have any real children..

  // todo: need to add support for arbitrary strings here, which are not just types

  int i;
  for(i=0; i<inst_pars.size; i++) {
    TypeDef* defn_tp = templ_par->templ_pars.FastEl(i); // type as defined
    TypeDef* inst_tp = inst_pars.FastEl(i);  // type as instantiated

    templ_pars.ReplaceLinkIdx(i, inst_tp); // actually replace it

    // update sub-types based on defn_tp (go backwards to get most extended types 1st)
    int j;
    for(j=sub_types.size-1; j>=0; j--) {
      sub_types.FastEl(j)->ReplaceParent(defn_tp, inst_tp);
    }
  }

  // update to use new types
  UpdateMDTypes(templ_par->templ_pars, templ_pars);
}

/*obs void TypeDef::unRegister(void* it) {
  if((taMisc::keep_tokens != taMisc::ForceTokens) &&
     (!tokens.keep || (taMisc::keep_tokens == taMisc::NoTokens)))
    return;

  if(!tokens.RemoveEl(it)) {    // if we couldn't find this one, must be a sub-tok..
    int subt = (int)(tokens.sub_tokens) - 1;
    tokens.sub_tokens = MAX(subt, 0); // might blow down..
  }
}*/

void TypeDef::unRegisterFinal(void* it) {
  if((taMisc::keep_tokens != taMisc::ForceTokens) &&
     (!tokens.keep || (taMisc::keep_tokens == taMisc::NoTokens)))
    return;

  if (tokens.RemoveEl(it)) {
    TypeDef* par = GetParent();
    while (par) {
      par->tokens.sub_tokens.deref();
      par = par->GetParent();
    }
  }
}


//////////////////////////////////
//      Get/SetVal              //
//////////////////////////////////

String TypeDef::GetValStr_enum(const void* base, void* par, MemberDef* memb_def,
                               StrContext sc, bool force_inline) const
{
  int enval = *static_cast<const int *>(base);
  bool show_scope = false;
  return Get_C_EnumString(enval, show_scope);
}

String TypeDef::GetValStr_class_inline(const void* base_, void* par, MemberDef* memb_def,
                                       StrContext sc, bool force_inline) const
{
  void* base = (void*)base_; // hack to avoid having to go through entire code below and fix
  String rval;
  if (sc != SC_DISPLAY) rval = "{";
  for(int i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    // if streaming, do full save check, else just check for NO_SAVE
    if (sc == SC_STREAMING) {
      if (!md->DumpMember(base))
        continue;
    } else if (sc == SC_DISPLAY) {
      if (!md->ShowMember(taMisc::USE_SHOW_GUI_DEF, SC_EDIT, taMisc::SHOW_CHECK_MASK))
        continue;
    } else {
      if(md->HasOption("NO_SAVE"))
        continue;
    }
    if(sc == SC_DISPLAY) {
      bool condshow = md->GetCondOptTest("CONDSHOW", this, base);
      if(!condshow) continue;
      bool condedit = md->GetCondOptTest("CONDEDIT", this, base);
      if(!condedit) continue;
      bool non_def = false;
      if(md->GetDefaultStatus(base) == MemberDef::NOT_DEF) {
        non_def = true;
        rval += "<font style=\"background-color: yellow\">&nbsp;";
      }
      rval += md->name + "&nbsp;";
      if(non_def) {
        rval += "</font>";
      }
      if(md->type->DerivesFormal(TA_enum) || md->type->ptr > 0
#ifndef NO_TA_BASE
         || md->type->DerivesFrom(TA_taSmartPtr) || md->type->DerivesFrom(TA_taSmartRef)
#endif
         )
        rval += "<font style=\"background-color: LightGrey\">&nbsp;&nbsp;";
      else
        rval += "<font style=\"background-color: white\">&nbsp;&nbsp;";
      if(md->type->InheritsFrom(TA_taString))     rval += "\"";
      rval += md->type->GetValStr(md->GetOff(base), base, md, sc, force_inline);
      if(md->type->InheritsFrom(TA_taString))     rval += "\"";
      rval += "&nbsp;&nbsp;</font>&nbsp;&nbsp;&nbsp;&nbsp;";
    }
    else {
      rval += md->name + "=";
      if(md->type->InheritsFrom(TA_taString))     rval += "\"";
      rval += md->type->GetValStr(md->GetOff(base), base, md, sc, force_inline);
      if(md->type->InheritsFrom(TA_taString))     rval += "\"";
      rval += ": ";
    }
  }
  if (sc != SC_DISPLAY) rval += "}";
  return rval;
}

namespace { // anonymous
  // Functions to format float/double values consistently across platforms.

  void NormalizeRealString(String &str) {
    // Make NaN and infinity representations consistent.
    // Windows may use "1.#QNAN" or "-1.#IND" for nan.
    if (str.contains_ci("nan") || str.contains_ci("ind")) {
      str = "nan";
      return;
    }

    // Windows uses "1.#INF" or "-1.#INF" for infinities.
    if (str.contains_ci("inf")) {
      str = (str.elem(0) == '-') ? "-inf" : "inf";
      return;
    }

    // Get rid of leading zeros in the exponent, since mac/win aren't
    // consistent in how many they output for padding.
    int exponent = str.index_ci("e+");
    if (exponent == -1) exponent = str.index_ci("e-");
    if (exponent > 0) {
      exponent += 2;
      int first_non_zero = exponent;
      while (str.elem(first_non_zero) == '0') ++first_non_zero;
      if (first_non_zero > exponent) {
        str.del(exponent, first_non_zero - exponent);
      }
    }

    // Convert , to . for intl contexts so files are always uniform
    str.repl(",", ".");
  }

  String FormatFloat(float f, TypeDef::StrContext sc) {
    switch (sc) {
      case TypeDef::SC_STREAMING: {
        String ret(f, "%.7g");
        NormalizeRealString(ret);
        return ret;
      }
      default:
        return String(f);
    }
  }

  String FormatDouble(double d, TypeDef::StrContext sc) {
    switch (sc) {
      case TypeDef::SC_STREAMING: {
        String ret(d, "%.16lg");
        NormalizeRealString(ret);
        return ret;
      }
      default:
        return String(d);
    }
  }
}

String TypeDef::GetValStr(const void* base_, void* par, MemberDef* memb_def,
                          StrContext sc, bool force_inline) const
{
  if(sc == SC_DEFAULT)
    sc = (taMisc::is_saving) ? SC_STREAMING : SC_VALUE;
  void* base = (void*)base_; // hack to avoid having to go through entire code below and fix
  // if its void, odds are its a function..
  if (InheritsFrom(TA_void) || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun;
    if(memb_def != NULL)
      fun = TA_taRegFun.methods.FindOnListAddr(*((ta_void_fun*)base),
                                                 memb_def->lists, lidx);
    else
      fun = TA_taRegFun.methods.FindAddr(*((ta_void_fun*)base), lidx);
    if(fun != NULL)
      return fun->name;
    else if(*((void**)base) == NULL)
      return String::con_NULL;
    return String((ta_intptr_t)*((void**)base));
  }
  if (ptr == 0) {
    if (DerivesFrom(TA_bool)) {
      bool b = *((bool*)base);
      switch (sc) {
      case SC_STREAMING: return (b) ? String::con_1 : String::con_0;
      case SC_DISPLAY: return (b) ? String("+") : String("-");
      default:
        return String(b);
      }
    }
    // note: char is generic, and typically we won't use signed char
    else if (DerivesFrom(TA_char)) {
      switch (sc) {
      case SC_STREAMING: return String((int)*((char*)base));
      default:
        return String(*((char*)base));
      }
    }
    // note: explicit use of signed char is treated like a number
    else if ((DerivesFrom(TA_signed_char))) {
      return String((int)*((signed char*)base)); // treat as numbers
    }
    // note: explicit use of unsigned char is "byte" in ta/pdp
    else if ((DerivesFrom(TA_unsigned_char))) {
      return String((uint)*((unsigned char*)base)); // treat bytes as numbers
    }
    else if(DerivesFrom(TA_short)) {
      return String((int)*((short*)base));
    }
    else if(DerivesFrom(TA_unsigned_short)) {
      return String((uint)*((unsigned short*)base));
    }
    else if(DerivesFrom(TA_int)) {
#ifndef NO_TA_BASE
      if(sc == SC_DISPLAY && par && memb_def && memb_def->HasOption("DYNENUM_ON_enum_type")) {
        // shameless hack to get dyn enum to display text value
        DynEnum* dye = (DynEnum*)par;
        return dye->NameVal();
      }
      else
#endif
        return String(*((int*)base));
    }
    else if(DerivesFrom(TA_unsigned_int)) {
      return String(*((uint*)base));
    }
    else if(DerivesFrom(TA_int64_t)) {
      return String(*((int64_t*)base));
    }
    else if(DerivesFrom(TA_uint64_t)) {
      return String(*((uint64_t*)base));
    }
    else if(DerivesFrom(TA_float)) {
      return FormatFloat(*static_cast<const float *>(base), sc);
    }
    else if(DerivesFrom(TA_double)) {
      return FormatDouble(*static_cast<const double *>(base), sc);
    }
    else if(DerivesFormal(TA_enum)) {
      return GetValStr_enum(base, par, memb_def, sc, force_inline);
    }
    else if(DerivesFrom(TA_taString))
      return *((String*)base);
    // in general, Variant is handled by recalling this routine on its rep's typdef
    else if (DerivesFrom(TA_Variant)) {
      TypeDef* typ;
      void* var_base;
      Variant& var = *((Variant*)base);
      //note: TA_void does not deal with this properly, so don't indirect...
      if (var.type() == Variant::T_Invalid)
        return _nilString;
        //NOTE: maybe we should indirect, rather than return NULL directly...
      if (var.isNull()) return String::con_NULL;
      var.GetRepInfo(typ, var_base);
      return typ->GetValStr(var_base, NULL, memb_def, sc, force_inline);
    }
#ifndef NO_TA_BASE
# ifdef TA_USE_QT
    else if(DerivesFrom(TA_taAtomicInt)) {
      return String((int)(*((taAtomicInt*)base)));
    }
# endif
    else if(DerivesFrom(TA_taBase)) {
      taBase* rbase = (taBase*)base;
      if(rbase)
        return rbase->GetValStr(par, memb_def, sc, force_inline);
    }
    else if(DerivesFrom(TA_taArray_impl)) {
      taArray_impl* gp = (taArray_impl*)base;
      if(gp != NULL) {
        String nm = " Size: ";
        nm += String(gp->size);
        nm += String(" (") + name + ")";
        return nm;
      }
      return name;
    }
    else if (DerivesFrom(TA_taSmartPtr)) {
      // we just delegate to taBase* since we are binary compatible
      return TA_taBase_ptr.GetValStr(base_, par, memb_def, sc, force_inline);
    }
    else if (DerivesFrom(TA_taSmartRef)) {
      taSmartRef& ref = *((taSmartRef*)base);
      taBase* rbase = ref;
      if (rbase) {
        if ((rbase->GetOwner() != NULL) || (rbase == tabMisc::root)) {
          switch (sc) {
          case SC_STREAMING:
            return dumpMisc::path_tokens.GetPath(rbase);        // use path tokens when saving..
          case SC_DISPLAY:
            return rbase->GetName();
          default:
            return rbase->GetPathNames();
          }
        } else
          return String((intptr_t)rbase);
      } else  return String::con_NULL;
    }
#endif
    else if(DerivesFormal(TA_class) &&
            (force_inline || HasOption("INLINE") || HasOption("INLINE_DUMP")))
    {
      return GetValStr_class_inline(base_, par, memb_def, sc, force_inline);
    }
    else if(DerivesFormal(TA_struct))
      return "struct " + name;
    else if(DerivesFormal(TA_union))
      return "union " + name;
    else if(DerivesFormal(TA_class))
      return "class " + name;
    else if(DerivesFrom(TA_void))
      return "void";
  }
  else if(ptr == 1) {
#ifndef NO_TA_BASE
    if(DerivesFrom(TA_taBase)) {
      return taBase::GetValStr_ptr(this, base_, par, memb_def, sc, force_inline);
    }
    else
#endif
    if (DerivesFrom(TA_TypeDef)) {
      TypeDef* td = *((TypeDef**)base);
      if (td) {
        return td->GetPathName();
      } else
        return String::con_NULL;
    }
    else if (DerivesFrom(TA_MemberDef)) {
      MemberDef* md = *((MemberDef**)base);
      if (md) {
        return md->GetPathName();
      } else
        return String::con_NULL;
    }
    else if (DerivesFrom(TA_MethodDef)) {
      MethodDef* md = *((MethodDef**)base);
      if (md) {
        return md->GetPathName();
      } else
        return String::con_NULL;
    }
  }
  return name;
}

void TypeDef::SetValStr_enum(const String& val, void* base, void* par, MemberDef* memb_def,
                             StrContext sc, bool force_inline) {
  String strval = val;
  if(strval.contains(')')) {
    strval = strval.after(')');
    if(strval.empty())  // oops
      strval = val;
  }
  strval.gsub(" ",""); strval.gsub("\t",""); strval.gsub("\n","");
  if(strval.contains("::")) {
    String tp_nm = strval.before("::");
    String en_nm = strval.after("::");
    TypeDef* td = taMisc::FindTypeName(tp_nm);
    if(!td) {
      taMisc::Warning("Enum type name:", tp_nm, "not found in list of registered types -- cannot set value for string:", strval);
      return;
    }
    td->SetValStr_enum(en_nm, base, par, memb_def, sc, force_inline);
    return;
  }
  if(strval.contains('|')) { // bits
    int bits = 0;
    while(strval.nonempty()) {
      String curstr = strval;
      if(strval.contains('|')) {
        curstr = strval.before('|');
        strval = strval.after('|');
      }
      else
        strval = _nilString;
      EnumDef* ed = FindEnum(curstr);
      if(ed) {
        bits |= ed->enum_no;
      }
      else {
        taMisc::Warning("Enum named:", curstr, "not found in enum type:", name);
      }
    }
    *((int*)base) = bits;
    return;
  }
  else {
    EnumDef* ed = FindEnum(strval);
    if(ed) {
      *((int*)base) = ed->enum_no;
      return;
    }
    int intval = (int)strval;
    *((int*)base) = intval;
    return;
  }
}

void TypeDef::SetValStr_class_inline(const String& val, void* base, void* par,
                                     MemberDef* memb_def, StrContext sc, bool force_inline) {
  String rval = val;
  rval = rval.after('{');
  while(rval.contains(':')) {
    int st_pos = rval.index('=');
    String mb_nm = rval.before(st_pos);
    String next_val = rval.after(st_pos);
    int pos = 0;
    int next_val_len = next_val.length();
    int c = next_val[pos];
    if(c == '\"') {             // "
      st_pos++;
      next_val = next_val.after(0);
      next_val_len--;
      c = next_val[pos];
      while((c != '\"') && (pos < next_val_len)) c = next_val[++pos]; // "
    }
    else {
      int depth = 0;
      while(!(((c == ':') || (c == '}')) && (depth <= 0)) && (pos < next_val_len)) {
        if(c == '{')  depth++;
        if(c == '}')  depth--;
        c = next_val[++pos];
      }
    }
    String mb_val = next_val.before(pos);
    rval = rval.after(pos + st_pos + 1); // next position
    if(c == '\"')                            // "
      rval = rval.after(':');   // skip the semi-colon which was not groked
    mb_nm.gsub(" ", "");
    MemberDef* md = members.FindName(mb_nm);
    if(md == NULL) {            // try to find a name with an aka..
      int a;
      for(a=0;a<members.size;a++) {
        MemberDef* amd = members.FastEl(a);
        String aka = amd->OptionAfter("AKA_");
        if(aka.empty()) continue;
        if(aka == mb_nm) {
          md = amd;
          break;
        }
      }
    }
    if((md != NULL) && !mb_val.empty()) { // note: changed par to base here..
      md->type->SetValStr(mb_val, md->GetOff(base), base /* par */, md, sc, true);
      // force inline!
    }
  }
}

void TypeDef::SetValStr(const String& val, void* base, void* par, MemberDef* memb_def,
                        StrContext sc, bool force_inline)
{
  if (sc == SC_DEFAULT)
    sc = (taMisc::is_loading) ? SC_STREAMING : SC_VALUE;

  if(InheritsFrom(TA_void) || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    MethodDef* fun = TA_taRegFun.methods.FindName(val);
    if((fun != NULL) && (fun->addr != NULL))
      *((ta_void_fun*)base) = fun->addr;
    return;
  }
  if (ptr == 0) {
    if(DerivesFrom(TA_bool)) {
      *((bool*)base) = val.toBool();
    }
    else if(DerivesFrom(TA_int))
      *((int*)base) = val.toInt();
    else if(DerivesFrom(TA_float))
      *((float*)base) = val.toFloat();
    else if(DerivesFormal(TA_enum)) {
      SetValStr_enum(val, base, par, memb_def, sc, force_inline);
    }
    else if(DerivesFrom(TA_taString))
      *((String*)base) = val;
    // in general, Variant is handled by recalling this routine on its rep's typdef, then fixing null
    else if (DerivesFrom(TA_Variant)) {
      TypeDef* typ;
      void* var_base;
      Variant& var = *((Variant*)base);
      // if it doesn't have a type, then it will just become a string
      // (we can't let TA_void get processed...)
      if (var.type() == Variant::T_Invalid) {
        // don't do anything for empty string
        if (!val.empty())
          var = val;
        return;
      }
      var.GetRepInfo(typ, var_base);
      typ->SetValStr(val, var_base, par, memb_def, sc, force_inline);
      var.UpdateAfterLoad();
    }
    // note: char is treated as an ansi character
    else if (DerivesFrom(TA_char))
    //TODO: char conversion heuristics
      *((char*)base) = val.toChar();
    // signed char is treated like a number
    else if (DerivesFrom(TA_signed_char))
      *((signed char*)base) = (signed char)val.toShort();
    // unsigned char is "byte" in ta/pdp and treated like a number
    else if (DerivesFrom(TA_unsigned_char))
      *((unsigned char*)base) = (unsigned char)val.toUShort();
    else if(DerivesFrom(TA_short))
      *((short*)base) = val.toShort();
    else if(DerivesFrom(TA_unsigned_short))
      *((unsigned short*)base) = val.toUShort();
    else if(DerivesFrom(TA_unsigned_int))
      *((uint*)base) = val.toUInt();
    else if(DerivesFrom(TA_int64_t))
      *((int64_t*)base) = val.toInt64();
    else if(DerivesFrom(TA_uint64_t))
      *((uint64_t*)base) = val.toUInt64();
    else if(DerivesFrom(TA_double))
      *((double*)base) = val.toDouble();
#ifndef NO_TA_BASE
# ifdef TA_USE_QT
    else if(DerivesFrom(TA_QAtomicInt)) {
       (*((QAtomicInt*)base)) = val.toInt();
    }
# endif
    else if(DerivesFrom(TA_taBase)) {
      taBase* rbase = (taBase*)base;
      if(rbase)
        rbase->SetValStr(val, par, memb_def, sc, force_inline);
    }
    else if(DerivesFrom(TA_taArray_impl)) {
      taArray_impl* gp = (taArray_impl*)base;
      if(gp != NULL)
        gp->InitFromString(val);
    }
    else if (DerivesFrom(TA_taSmartPtr)) {
      // we just delegate, since we are binary compat
      TA_taBase_ptr.SetValStr(val, base, par, memb_def, sc, force_inline);
      return;
    }
    else if(DerivesFrom(TA_taSmartRef) && (tabMisc::root)) {
      taBase* bs = NULL;
      if ((val != String::con_NULL) && (val != "Null")) {
        String tmp_val(val); // FindFromPath can change it
        if (sc == SC_STREAMING) {
          bs = dumpMisc::path_tokens.FindFromPath(tmp_val, this, base, par, memb_def);
          if (!bs)return;       // indicates deferred
        } else {
          MemberDef* md = NULL;
          bs = tabMisc::root->FindFromPath(tmp_val, md);
          if(!bs) {
            taMisc::Warning("*** Invalid Path in SetValStr:",val);
            return;
          }
          if(md) {
            if (md->type->ptr == 1) {
              bs = *((taBase**)bs);
              if(bs == NULL) {
                taMisc::Warning("*** Null object at end of path in SetValStr:",val);
                return;
              }
            } else if(md->type->ptr != 0) {
              taMisc::Warning("*** ptr count greater than 1 in path:", val);
              return;
            }
          }
        }
      }
      taSmartRef& ref = *((taSmartRef*)base);
      ref = bs;
    }
#endif
    else if(DerivesFormal(TA_class) &&
            (force_inline || HasOption("INLINE") || HasOption("INLINE_DUMP"))) {
      SetValStr_class_inline(val, base, par, memb_def, sc, force_inline);
    }
  }
  else if(ptr == 1) {
    bool is_null = ((val == "NULL") || (val == "(NULL)"));
#ifndef NO_TA_BASE
    if (DerivesFrom(TA_taBase)) {
      if (tabMisc::root) {
        if (is_null)
          taBase::DelPointer((taBase**)base);
        else
          taBase::SetValStr_ptr(val, this, base, par, memb_def, sc, force_inline);
      }
    }
    else
#endif
    if (is_null) {
      *((void**)base) = NULL;
    }
    else if(DerivesFrom(TA_TypeDef)) {
      TypeDef* td = taMisc::types.FindTypeR(val);
      if(td != NULL)
        *((TypeDef**)base) = td;
    }
    else if(DerivesFrom(TA_MemberDef)) {
      String fqtypnm = val.before("::", -1); // before final ::
      String mbnm = val.after("::", -1); // after final ::
      if(!fqtypnm.empty() && !mbnm.empty()) {
        TypeDef* td = taMisc::types.FindTypeR(fqtypnm);
        if(td != NULL) {
          MemberDef* md = td->members.FindName(mbnm);
          if(md != NULL)
            *((MemberDef**)base) = md;
        }
      }
    }
    else if(DerivesFrom(TA_MethodDef)) {
      String fqtypnm = val.before("::", -1); // before final ::
      String mthnm = val.after("::", -1);
      if(!fqtypnm.empty() && !mthnm.empty()) {
        TypeDef* td = taMisc::types.FindTypeR(fqtypnm);
        if(td != NULL) {
          MethodDef* md = td->methods.FindName(mthnm);
          if(md != NULL)
            *((MethodDef**)base) = md;
        }
      }
    }
  }
}

int TypeDef::ReplaceValStr_class(const String& srch, const String& repl, const String& mbr_filt,
                                 void* base, void* par, TypeDef* par_typ, MemberDef* memb_def,
                                 StrContext sc)
{
  int rval = 0;
  for(int i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    // if streaming, do full save check, else just check for NO_SAVE
    if (sc == SC_STREAMING) {
      if (!md->DumpMember(base))
        continue;
    }
    else if (sc == SC_DISPLAY) {
      if (!md->ShowMember(taMisc::USE_SHOW_GUI_DEF, SC_EDIT, taMisc::SHOW_CHECK_MASK))
        continue;
    }
    else {
      if(md->HasOption("NO_SAVE"))
        continue;
    }
    if(sc == SC_DISPLAY) {
      bool condshow = md->GetCondOptTest("CONDSHOW", this, base);
      if(!condshow) continue;
      bool condedit = md->GetCondOptTest("CONDEDIT", this, base);
      if(!condedit) continue;
    }
    rval += md->type->ReplaceValStr(srch, repl, mbr_filt, md->GetOff(base), base, this, md, sc);
  }
  return rval;
}

int TypeDef::ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
                           void* base, void* par, TypeDef* par_typ, MemberDef* memb_def,
                           StrContext sc)
{
  if(ptr == 0) {
#ifndef NO_TA_BASE
    if(DerivesFrom(TA_taBase)) {
      taBase* rbase = (taBase*)base;
      if(rbase)
        return rbase->ReplaceValStr(srch, repl, mbr_filt, par, par_typ, memb_def, sc);
    }
    else
#endif
      if(DerivesFormal(TA_class) &&
            !(DerivesFrom(TA_taString) || DerivesFrom(TA_Variant)
#ifndef NO_TA_BASE
              || DerivesFrom(TA_QAtomicInt) || DerivesFrom(TA_taArray_impl)
              || DerivesFrom(TA_taSmartPtr) || DerivesFrom(TA_taSmartRef)
#endif
              )
         )
    {
      return ReplaceValStr_class(srch, repl, mbr_filt, base, par, par_typ, memb_def, sc);
    }
  }
  // only apply filtering to the terminal leaves case here, not to higher level owners
  if(memb_def && mbr_filt.nonempty() && !memb_def->name.contains(mbr_filt))
    return 0;
  String str = GetValStr(base, par, memb_def, sc, false);
  // note: just using literal replace here, not regexp..
  if(!str.contains(srch)) return 0;
  String orig = str;
  int rval = str.gsub(srch, repl);
  SetValStr(str, base, par, memb_def, sc, false);
  String repl_info = String("orig val: ") + orig + " new val: " + str;
#ifndef NO_TA_BASE
  if(par_typ && par && par_typ->DerivesFrom(TA_taBase)) {
    if(memb_def) {
      taMisc::Info("Replaced string value in member:", memb_def->name, "of type:", name,
                   "in", par_typ->name, "object:",((taBase*)par)->GetPathNames(),repl_info);
    }
    else {
      taMisc::Info("Replaced string value in type:", name, "in", par_typ->name, "object:",
                   ((taBase*)par)->GetPathNames(), repl_info);
    }
  }
  else
#endif
    {
    if(memb_def) {
      taMisc::Info("Replaced string value in member:", memb_def->name, "of type:", name,
                   repl_info);
    }
    else {
      taMisc::Info("Replaced string value in type:", name, repl_info);
    }
  }
  return rval;
}

bool TypeDef::IsVarCompat() const {
  // a few "blockers"
  if (ref || (ptr > 1) || InheritsFrom(TA_void)) return false;
#ifndef NO_TA_BASE
  if ((ptr ==1) && !InheritsFrom(TA_taBase)) return false;
#endif
  if (InheritsNonAtomicClass()) return false;
  // ok, hopefully the rest are ok!
  return true;
}

const Variant TypeDef::GetValVar(const void* base_, const MemberDef* memb_def) const
{
  void* base = (void*)base_; // hack to avoid having to go through entire code below and fix
  // if its void, odds are its a function..
  if (InheritsFrom(TA_void) || ((memb_def) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun;
    if(memb_def != NULL)
      fun = TA_taRegFun.methods.FindOnListAddr(*((ta_void_fun*)base),
                                                 memb_def->lists, lidx);
    else
      fun = TA_taRegFun.methods.FindAddr(*((ta_void_fun*)base), lidx);
    if (fun != NULL)
      return fun->name;
    else if(*((void**)base) == NULL)
      return Variant((void*)NULL);//String::con_NULL;
    else
      return String((ta_intptr_t)*((void**)base)); //TODO: is this the best??
  }
  if (ptr == 0) {
    if (DerivesFrom(TA_bool)) {
      bool b = *((bool*)base);
      return b; //T_Bool
    }
    // note: char is generic char, and typically we won't use signed char
    else if (DerivesFrom(TA_char)) {
      return *((char*)base); // T_Char
    }
    // note: explicit use of signed char is treated like a number
    else if ((DerivesFrom(TA_signed_char))) {
      return (int)*((signed char*)base); // T_Int
    }
    // note: explicit use of unsigned char is "byte" in ta/pdp
    else if ((DerivesFrom(TA_unsigned_char))) {
      return (uint)*((unsigned char*)base);  // T_UInt
    }
    else if(DerivesFrom(TA_short)) {
      return (int)*((short*)base);  // T_Int
    }
    else if(DerivesFrom(TA_unsigned_short)) {
      return (uint)*((unsigned short*)base);  // T_UInt
    }
    else if(DerivesFrom(TA_int)) {
      return *((int*)base);  // T_Int
    }
    else if(DerivesFrom(TA_unsigned_int)) {
      return *((uint*)base);  // T_UInt
    }
    else if(DerivesFrom(TA_int64_t)) {
      return *((int64_t*)base);  // T_Int64
    }
    else if(DerivesFrom(TA_uint64_t)) {
      return *((uint64_t*)base);  // T_UInt64
    }
    else if(DerivesFrom(TA_float)) {
      return *((float*)base); // T_Double
    }
    else if(DerivesFrom(TA_double)) {
      return *((double*)base); // T_Double
    }
    else if(DerivesFormal(TA_enum)) {
      int en_val = *((int*)base);
      String rval = GetEnumString("", en_val);
      if(rval.empty()) rval = (String)en_val;
      return rval;  // T_String
    }
    else if(DerivesFrom(TA_taString))
      return *((String*)base); // T_String
    else if(DerivesFrom(TA_Variant)) {
      return *((Variant*)base);
    }
#ifndef NO_TA_BASE
    //WARNING: there could be ref-count issues if base has not been ref'ed at least once!
    else if(DerivesFrom(TA_taBase)) {
      taBase* rbase = (taBase*)base;
      //WARNING: there could be ref-count issues if base has not been ref'ed at least once!
      return rbase; // T_Base
    }
    else if (DerivesFrom(TA_taSmartPtr)) {
      taBase* rbase = (taBase*)base;
      return rbase; // T_Base
    }
    else if (DerivesFrom(TA_taSmartRef)) {
      taBase* rbase = *((taSmartRef*)base);
      return rbase; // T_Base
    }
#endif
    // NOTE: other value types are not really supported, just fall through to return invalid
  }
  else if (ptr == 1) {
    if (DerivesFrom(TA_char)) {
      return *((char**)base); // T_String
    }
#ifndef NO_TA_BASE
    else if (DerivesFrom(TA_taBase)) {
      //NOTE: strictly speaking, we should be returning a generic ptr which points to the
      // base value, but in practice, this is never what we want, since members that
      // are taBase*'s are ubiquitous, what we actually want is a reference to the thing
      // being pointed to, ie., the content of the variable
      taBase* rbase = *((taBase**)base);
      return rbase; // T_Base
    }
#endif
  }
  // other types and degress of indirection not really supported
  return _nilVariant;
}

bool TypeDef::ValIsDefault(const void* base, const MemberDef* memb_def,
    int for_show) const
{
  // some cases are simple, for non-class values
  if ((InheritsFrom(TA_void) || ((memb_def) && (memb_def->fun_ptr != 0))) ||
    (ptr > 0) ||
    !InheritsNonAtomicClass()
  ){
    return ValIsEmpty(base, memb_def); // note: show not used for single guy
  } else { // instance of a class, so must recursively determine
    // just find all eligible guys, and return true if none fail
    for (int i = 0; i < members.size; ++i) {
      MemberDef* md = members.FastEl(i);
      if (!md || !md->ShowMember(taMisc::USE_SHOW_GUI_DEF, TypeItem::SC_ANY,
        for_show)) continue;
      if (!md->ValIsDefault(base, for_show))
        return false;
    }
    return true;
  }
}

bool TypeDef::ValIsEmpty(const void* base_, const MemberDef* memb_def) const
{
  void* base = (void*)base_; // hack to avoid having to go through entire code below and fix
  // if its void, odds are its a function..
  if (InheritsFrom(TA_void) || ((memb_def) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun;
    if(memb_def != NULL)
      fun = TA_taRegFun.methods.FindOnListAddr(*((ta_void_fun*)base),
                                                 memb_def->lists, lidx);
    else
      fun = TA_taRegFun.methods.FindAddr(*((ta_void_fun*)base), lidx);
    if (fun)
      return false;
    else
      return !(*((void**)base));
  }
  if (ptr == 0) {
    if (DerivesFrom(TA_bool)) {
      bool b = *((bool*)base);
      return !b; //T_Bool
    }
    // note: char is generic char, and typically we won't use signed char
    else if (DerivesFrom(TA_char)) {
      return (*((char*)base) == '\0');
    }
    // note: explicit use of signed char is treated like a number
    else if ((DerivesFrom(TA_signed_char))) {
      return (*((signed char*)base) == 0);
    }
    // note: explicit use of unsigned char is "byte" in ta/pdp
    else if ((DerivesFrom(TA_unsigned_char))) {
      return (*((unsigned char*)base) == 0);
    }
    else if(DerivesFrom(TA_short)) {
      return (*((short*)base) == 0);
    }
    else if(DerivesFrom(TA_unsigned_short)) {
      return (*((unsigned short*)base) == 0);
    }
    else if(DerivesFrom(TA_int)) {
      return (*((int*)base) == 0);
    }
    else if(DerivesFrom(TA_unsigned_int)) {
      return (*((uint*)base) == 0);
    }
    else if(DerivesFrom(TA_int64_t)) {
      return (*((int64_t*)base) == 0);
    }
    else if(DerivesFrom(TA_uint64_t)) {
      return (*((uint64_t*)base) == 0);  // T_UInt64
    }
    else if(DerivesFrom(TA_float)) {
      return (*((float*)base) == 0); // T_Double
    }
    else if(DerivesFrom(TA_double)) {
      return (*((double*)base) == 0); // T_Double
    }
    else if(DerivesFormal(TA_enum)) {
      return (*((int*)base) == 0); // T_Int
    }
    else if(DerivesFrom(TA_taString))
      return ((*((String*)base)).empty()); // T_String
    else if(DerivesFrom(TA_Variant)) {
      return ((*((Variant*)base)).isDefault());
    }
#ifndef NO_TA_BASE
    else if (DerivesFrom(TA_taSmartPtr)) {
      taBase* rbase = (taBase*)base;
      return !(rbase); // T_Base
    }
    else if (DerivesFrom(TA_taSmartRef)) {
      taBase* rbase = *((taSmartRef*)base);
      return !(rbase); // T_Base
    }
#endif
    // must be some other value or a class -- default to saying no to empty
    else return false;
    // NOTE: other value types are not really supported, just fall through to return invalid
  }
  else // (ptr >= 1)
    return !(*((void**)base)); // only empty if NULL
}

void TypeDef::SetValVar(const Variant& val, void* base, void* par,
                        MemberDef* memb_def)
{
  if(InheritsFrom(TA_void) || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    MethodDef* fun = TA_taRegFun.methods.FindName(val.toString());
    if((fun != NULL) && (fun->addr != NULL))
      *((ta_void_fun*)base) = fun->addr;
    return;
  }
  if (ptr == 0) {
    if(DerivesFrom(TA_bool)) {
      *((bool*)base) = val.toBool(); return;
    }
    else if(DerivesFrom(TA_int)) {
      *((int*)base) = val.toInt(); return;}
    else if(DerivesFrom(TA_float)) {
      *((float*)base) = val.toFloat(); return;}
    else if(DerivesFormal(TA_enum)) {
      // if it is a number, assume direct value, otherwise it is a string
      if (val.isNumeric()) {
        *((int*)base) = val.toInt();
        return;
      }
      SetValStr_enum(val.toString(), base, par, memb_def);
    }
    else if(DerivesFrom(TA_taString)) {
      *((String*)base) = val.toString(); return;}
    // in general, Variant is handled by recalling this routine on its rep's typdef, then fixing null
    else if (DerivesFrom(TA_Variant)) {
      *((Variant*)base) = val; return;
    }
    // note: char is treated as an ansi character
    else if (DerivesFrom(TA_char)) {
    //TODO: char conversion heuristics
      *((char*)base) = val.toChar(); return;}
    // signed char is treated like a number
    else if (DerivesFrom(TA_signed_char)) {
      *((signed char*)base) = (signed char)val.toInt(); return;}
    // unsigned char is "byte" in ta/pdp and treated like a number
    else if (DerivesFrom(TA_unsigned_char)) {
      *((unsigned char*)base) = val.toByte(); return;}
    else if(DerivesFrom(TA_short)) {
      *((short*)base) = (short)val.toInt(); return;}
    else if(DerivesFrom(TA_unsigned_short)) {
      *((unsigned short*)base) = (unsigned short)val.toUInt(); return;}
    else if(DerivesFrom(TA_unsigned_int)) {
      *((uint*)base) = val.toUInt(); return;}
    else if(DerivesFrom(TA_int64_t)) {
      *((int64_t*)base) = val.toInt64(); return;}
    else if(DerivesFrom(TA_uint64_t)) {
      *((uint64_t*)base) = val.toUInt64(); return;}
    else if(DerivesFrom(TA_double)) {
      *((double*)base) = val.toDouble(); return;}
#ifndef NO_TA_BASE
    else if(DerivesFrom(TA_taList_impl)) {
      //TODO: not handled!
    }
    else if(DerivesFrom(TA_taArray_base)) {
      taArray_base* gp = (taArray_base*)base;
      if(gp != NULL)
        gp->InitFromString(val.toString());
      return;
    }
    else if(DerivesFrom(TA_taArray_impl)) {
      taArray_impl* gp = (taArray_impl*)base;
      if (gp != NULL) {
        gp->InitFromString(val.toString());
      }
      return;
    }
    else if (DerivesFrom(TA_taSmartPtr)) {
      // we just delegate, since we are binary compat
      TA_taBase_ptr.SetValVar(val, base, par, memb_def);
      return;
    }
    else if(DerivesFrom(TA_taSmartRef)) {
      //VERY DANGEROUS!!!! No type checking!!!!
      taSmartRef& ref = *((taSmartRef*)base);
      ref = val.toBase();
      return;
    }
#endif
  }
  else if(ptr == 1) {
#ifndef NO_TA_BASE
    if (DerivesFrom(TA_taBase)) {
      taBase* bs = val.toBase();
      if (bs && !bs->GetTypeDef()->DerivesFrom(this)) {
        taMisc::Warning("Attempt to set member of type", this->name, " from ",
          bs->GetTypeDef()->name);
        return;

      }
      if (memb_def  && memb_def->HasOption("OWN_POINTER")) {
        if(par == NULL)
          taMisc::Warning("*** NULL parent for owned pointer:");
        else
          taBase::OwnPointer((taBase**)base, bs, (taBase*)par);
      }
      else {
        if (memb_def && memb_def->HasOption("NO_SET_POINTER"))
          (*(taBase**)base) = bs;
        else
          taBase::SetPointer((taBase**)base, bs);
      }
      return;
    }
#endif
  }
  // if we get to here, the value can't be assigned
  taMisc::Warning("Type value was not assigned from Variant.");
}


#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
#define DMEM_TYPE (*(MPI_Datatype_PArray*)(dmem_type))
void TypeDef::AssertDMem_Type() {
  if (dmem_type) return;
  dmem_type = new MPI_Datatype_PArray;
}

MPI_Datatype TypeDef::GetDMemType(int share_set) {
  AssertDMem_Type();
  if(DMEM_TYPE.size > share_set) return DMEM_TYPE[share_set];
  if(DMEM_TYPE.size < share_set) GetDMemType(share_set-1);

  MPI_Datatype primitives[members.size];
  MPI_Aint byte_offsets[members.size];
  int block_lengths[members.size];

  int curr_prim = 0;
  for (int m = 0; m < members.size; m++) {
    MemberDef* md = members.FastEl(m);
    String shrset = md->OptionAfter("DMEM_SHARE_SET_");
    if(shrset.empty()) continue;
    if (md->type->ptr > 0) {
      if(taMisc::dmem_proc == 0) {
        taMisc::Error("WARNING: DMEM_SHARE_SET Specified for a pointer.",
                      "Pointers can not be shared.");
        continue;
      }
    }
    if (md->type->InheritsFormal(TA_class)) {
      primitives[curr_prim] = md->type->GetDMemType(share_set);
    }
    else if (md->type->InheritsFrom(TA_double)) {
      primitives[curr_prim] = MPI_DOUBLE;
    }
    else if (md->type->InheritsFrom(TA_float)) {
      primitives[curr_prim] = MPI_FLOAT;
    }
    else if (md->type->InheritsFrom(TA_int)) {
      primitives[curr_prim] = MPI_INT;
    }
    else if (md->type->InheritsFrom(TA_enum)) {
      primitives[curr_prim] = MPI_INT;
    }
    else if (md->type->InheritsFrom(TA_long)) {
      primitives[curr_prim] = MPI_LONG;
    }
    else {
      taMisc::Error("WARNING: DMEM_SHARE_SET Specified for an unrecognized type.",
                    "unrecoginized types can not be shared.");
      continue;
    }

    byte_offsets[curr_prim] = (ulong)md->GetOff((void *)0x100) - 0x100;
    block_lengths[curr_prim] = 1;
    curr_prim++;
  }
  MPI_Datatype new_type;
  if (!curr_prim) {
    new_type = MPI_DATATYPE_NULL;
    taMisc::Error("Null type, set:", String(share_set),", type:",name);
  }
  else {
    MPI_Type_struct(curr_prim, block_lengths, byte_offsets, primitives, &new_type);
    MPI_Type_commit(&new_type);
  }
  DMEM_TYPE.Add(new_type);
  return new_type;
}

#endif

void TypeDef::CopyFromSameType(void* trg_base, void* src_base,
                               MemberDef* memb_def)
{
  // if its void, odds are it is a fun pointer
  if(InheritsFrom(TA_void) || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun;
    if(memb_def != NULL)
      fun = TA_taRegFun.methods.FindOnListAddr(*((ta_void_fun*)src_base),
                                                 memb_def->lists, lidx);
    else
      fun = TA_taRegFun.methods.FindAddr(*((ta_void_fun*)src_base), lidx);
    if((fun != NULL) || (memb_def != NULL))
      *((ta_void_fun*)trg_base) = *((ta_void_fun*)src_base); // must be a funptr
    else
      *((void**)trg_base) = *((void**)src_base); // otherwise just a voidptr
    return;
  }
  if (ptr == 0) {
    // internal types can simply be bit copied
    if (internal) {
      memcpy(trg_base, src_base, size);
    }
    else if(DerivesFormal(&TA_enum)) {
      memcpy(trg_base, src_base, size); // bit copy
    }
    else if (DerivesFrom(TA_Variant))
      *((Variant*)trg_base) = *((Variant*)src_base);
    else if (DerivesFrom(TA_taString))
      *((String*)trg_base) = *((String*)src_base);
#ifndef NO_TA_BASE
    else if(DerivesFrom(TA_taSmartRef))
      *((taSmartRef*)trg_base) = *((taSmartRef*)src_base);
    else if(DerivesFrom(TA_taSmartPtr))
      *((taSmartPtr*)trg_base) = *((taSmartPtr*)src_base);
    else if(DerivesFrom(TA_taBase)) {
      taBase* rbase = (taBase*)trg_base;
      taBase* sbase = (taBase*)src_base;
      if(sbase->InheritsFrom(rbase->GetTypeDef()) || rbase->InheritsFrom(sbase->GetTypeDef())) // makin it safe..
        rbase->UnSafeCopy(sbase);
    }
#endif
    else if(DerivesFormal(TA_class))
      members.CopyFromSameType(trg_base, src_base);
  }
  else if(ptr >= 1) {
#ifndef NO_TA_BASE
    if((ptr == 1) && DerivesFrom(TA_taBase)) {
      taBase** rbase = (taBase**)trg_base;
      taBase** sbase = (taBase**)src_base;
      taBase::SetPointer(rbase, *sbase);
    }
    else {
#endif
      *((void**)trg_base) = *((void**)src_base);
#ifndef NO_TA_BASE
    }
#endif
  }
}

void TypeDef::CopyOnlySameType(void* trg_base, void* src_base,
                               MemberDef* memb_def)
{
  if(InheritsFormal(TA_class)) {
#ifndef NO_TA_BASE
    if(InheritsFrom(TA_taBase)) {
      taBase* src = (taBase*)src_base;
      // I actually inherit from the other guy, need to use their type for copying!
      if((src->GetTypeDef() != this) && InheritsFrom(src->GetTypeDef())) {
        src->GetTypeDef()->CopyOnlySameType(trg_base, src_base);
        return;
      }
    }
#endif // NO_TA_BASE
    members.CopyOnlySameType(trg_base, src_base);
  }
  else
    CopyFromSameType(trg_base, src_base, memb_def);
}

void TypeDef::MemberCopyFrom(int memb_no, void* trg_base, void* src_base) {
  if(memb_no < members.size)
    members[memb_no]->CopyFromSameType(trg_base, src_base);
}

bool TypeDef::CompareSameType(Member_List& mds, TypeSpace& base_types,
                              voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                              void* trg_base, void* src_base,
                              int show_forbidden, int show_allowed, bool no_ptrs,
                              bool test_only) {
  if(InheritsFormal(TA_class)) {
    return members.CompareSameType(mds, base_types, trg_bases, src_bases,
                                   this, trg_base, src_base,
                                   show_forbidden, show_allowed, no_ptrs, test_only);
  }
  else {
    taMisc::Error("CompareSameType called on non-class object -- does not work!");
    return false;
  }
}

String& TypeDef::PrintInherit_impl(String& strm) const {
  for(int i=0; i<parents.size; i++) {
    TypeDef* par = parents.FastEl(i);
    strm << par->name;
    if(par_off[i] > 0)
      strm << " +" << par_off[i];
    if(par->parents.size > 0)
      strm << " : ";
    par->PrintInherit_impl(strm);
    if(i < parents.size-1)
      strm << ", ";
  }
  return strm;
}

String& TypeDef::PrintInherit(String& strm) const {
  for(int i=0; i<par_formal.size; i++) {
    strm << par_formal.FastEl(i)->name << " ";
  }

  strm << name;
  if(parents.size > 0)
    strm << " : ";
  PrintInherit_impl(strm);
  return strm;
}

String& TypeDef::PrintTokens(String& strm, int indent) const {
  return tokens.Print(strm, indent);
}

String& TypeDef::PrintType(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  PrintInherit(strm);
//  if(taMisc::type_info_ == taMisc::ALL_INFO)
  strm << " (Sz: " << String(size) << ")";
  if(InheritsFormal(TA_class) || InheritsFormal(TA_enum))
    strm << " {";
  else
    strm << ";";
  if(!desc.empty()) {
    if(InheritsFormal(TA_class)) {
      strm << "\n";
      taMisc::IndentString(strm, indent) << "//" << desc ;
    }
    else
      strm << "\t//" << desc ;
  }

  PrintType_OptsLists(strm);
  strm << "\n";
  if(InheritsFormal(TA_class)) {
    if(sub_types.size > 0) {
      strm << "\n";
      taMisc::IndentString(strm, indent+1) << "// sub-types\n";
      int i;
      for(i=0; i<sub_types.size; i++) {
        if(!(sub_types.FastEl(i)->internal))
          sub_types.FastEl(i)->PrintType(strm,indent+1) << "\n";
      }
    }
    members.PrintType(strm, indent+1);
    methods.PrintType(strm, indent+1);
    taMisc::IndentString(strm, indent) << "} ";
    if(children.size > 0) {
      strm << " children: ";
      int i;
      for(i=0; i<children.size; i++) {
        strm << children.FastEl(i)->name;
        if(i < children.size - 1)
          strm << ", ";
      }
    }
    strm << "\n";
  }
  else if(InheritsFormal(TA_enum)) {
    enum_vals.PrintType(strm, indent+1);
    taMisc::IndentString(strm, indent) << "}\n";
  }
  return strm;
}

String& TypeDef::Print(String& strm, void* base, int indent) const {
  taMisc::IndentString(strm, indent);
#ifndef NO_TA_BASE
  if(DerivesFrom(TA_taBase)) {
    taBase* rbase;
    if(ptr == 0)
      rbase = (taBase*)base;
    else if(ptr == 1)
      rbase = *((taBase**)base);
    else
      rbase = NULL;

    if(rbase)
      strm << rbase->GetPathNames();
    else
      strm << Get_C_Name();
  }
  else
#endif
    strm << Get_C_Name();

#ifndef NO_TA_BASE
  if(InheritsFrom(TA_taBase)) {
    taBase* rbase = (taBase*)base;
    strm << " " << rbase->GetName()
         << " (refn=" << taBase::GetRefn(rbase) << ")";
  }
#endif

  if(InheritsFormal(TA_class)) {
    strm << " {\n";
    members.Print(strm, base, indent+1);
    taMisc::IndentString(strm, indent) << "}\n";
  }
  else {
    strm << " = " << GetValStr(base) << "\n";
  }
  return strm;
}

String TypeDef::GetHTMLLink(bool gendoc) const {
  STRING_BUF(rval, 32); // extends if needed
  TypeDef* npt = const_cast<TypeDef*>(this);
  if(npt->ptr > 0)
    npt = npt->GetNonPtrType();
  if(npt && npt->ref)
    npt = npt->GetNonRefType();
  if(npt && npt->DerivesFrom(TA_const))
    npt = npt->GetNonConstType();
  if(npt) {
    if(npt->InheritsFormal(TA_class)) {
      if(gendoc)
        rval.cat("<a href=\"").cat(npt->name).cat(".html\">").cat(Get_C_Name()).cat("</a>");
      else
        rval.cat("<a href=\"ta:.Type.").cat(npt->name).cat("\">").cat(Get_C_Name()).cat("</a>");
    }
    else if(npt->InheritsFormal(TA_enum)) {
      rval.cat("<a href=\"#").cat(npt->name).cat("\">").cat(Get_C_Name()).cat("</a>");
    }
    else {
      rval.cat(Get_C_Name());
    }
  }
  else {
    rval.cat(Get_C_Name());
  }
  return rval;
}

String TypeDef::GetHTMLSubType(bool gendoc, bool short_fmt) const {
  STRING_BUF(rval, (short_fmt ? 100 : 300)); // extends if needed
  String own_typ;
  TypeDef* ot = GetOwnerType();
  if(ot) {
    own_typ.cat(ot->name).cat("::");
  }
  if(InheritsFormal(TA_class)) {
    // todo: not yet supported by maketa..
  }
  else if(InheritsFormal(TA_enum)) {
    if(short_fmt) {
      rval.cat("enum <b><a href=\"#").cat(name).cat("\">").cat(own_typ).cat(name).cat("</a></b> { ");
      for(int i=0;i<enum_vals.size;i++) {
        EnumDef* ed = enum_vals[i];
        if(i > 0) rval.cat(", ");
        rval.cat(ed->name);
      }
      rval.cat(" }");
    }
    else {
      rval.cat("<h3 class=\"flags\"><a name=\"").cat(name).cat("\"></a>");
      rval.cat("enum ").cat(own_typ).cat(name).cat("</h3>\n");
      rval.cat("<p>").cat(trim(desc).xml_esc()).cat("</p>\n");
      rval.cat("<p><table border=\"1\" cellpadding=\"2\" cellspacing=\"1\" width=\"100%\">\n");
      rval.cat("<tr><th width=\"25%\">Constant</th><th width=\"15%\">Value</th><th width=\"60%\">Description</th></tr>\n");
      for(int i=0;i<enum_vals.size;i++) {
        EnumDef* ed = enum_vals[i];
        rval.cat("<tr><td valign=\"top\"><tt>").cat(own_typ).cat(ed->name).cat("</tt></td>");
        rval.cat("<td align=\"center\" valign=\"top\"><tt>0x").cat(String(ed->enum_no,"%08X")).cat("</tt></td>");
        rval.cat("<td valign=\"top\">").cat(trim(ed->desc).xml_esc()).cat("</td></tr>\n");
      }
      rval.cat("</table></p>\n");
    }
  }
  else {
    rval.cat("typedef ");
    for(int i=0;i<parents.size;i++) {
      rval.cat(parents[i]->Get_C_Name()).cat(" ");
    }
    rval.cat(name);
  }
  return rval;
}

String TypeDef::GetHTML(bool gendoc) const {
  if(!InheritsFormal(TA_class)) return GetHTMLLink(gendoc);

  STRING_BUF(rval, 9096); // extends if needed

  String wiki_help_url = taMisc::GetWikiURL(taMisc::web_help_wiki, true); // add index

  // preamble
  rval.cat("<iframe src=\"").cat(wiki_help_url).cat(name).cat("\" width=\"99%\" height=\"40%\" style=\"border:5px solid #000000\"></iframe>\n");
  rval.cat("<head>\n");
  // following is magic code viewer incantation
  rval.cat("<script>function writesrc(t,signature){t.innerHTML='<iframe width=\"100%\" height=\"50%\" src=\"http://grey.colorado.edu/gendoc.py?q='+signature+'\"></iframe>';}</script>\n");
  rval.cat("<title>").cat(taMisc::app_name).cat(" ").cat(taMisc::version).cat(" ").cat(name).cat(" Class Reference</title>\n");
  // include style sheet right in the thing
  rval.cat("<style type=\"text/css\">\n");
  rval.cat("h3.fn,span.fn { margin-left: 1cm;  text-indent: -1cm; }\n");
  rval.cat("a:link { color: #004faf; text-decoration: none }\n");
  rval.cat("a:visited { color: #672967; text-decoration: none }\n");
  rval.cat("a.obsolete { color: #661100; text-decoration: none }\n");
  rval.cat("a.compat { color: #661100; text-decoration: none }\n");
  rval.cat("a.obsolete:visited { color: #995500; text-decoration: none }\n");
  rval.cat("a.compat:visited { color: #995500; text-decoration: none }\n");
  rval.cat("td.postheader { font-family: sans-serif }\n");
  rval.cat("tr.address { font-family: sans-serif }\n");
  rval.cat("body { background: #ffffff; color: black }\n");
  rval.cat("table tr.odd { background: #f0f0f0; color: black; }\n");
  rval.cat("table tr.even { background: #e4e4e4; color: black; }\n");
  rval.cat("table.annotated th { padding: 3px; text-align: left }\n");
  rval.cat("table.annotated td { padding: 3px; }\n");
  rval.cat("table tr pre { padding-top: none; padding-bottom: none; padding-left: none; padding-right: none; border: none; background: none }\n");
  rval.cat("tr.qt-style { background: #66B036; color: black }\n");
  rval.cat("body pre { padding: 0.2em; border: #e7e7e7 1px solid; background: #f1f1f1; color: black }\n");
  rval.cat("span.preprocessor, span.preprocessor a { color: darkblue; }\n");
  rval.cat("span.comment { color: darkred; font-style: italic }\n");
  rval.cat("span.string,span.char { color: darkgreen; }\n");
  rval.cat("span.fakelink{text-decoration:underline;color:blue;cursor:pointer;font-size:small;}\n");
  rval.cat(".title { text-align: center }\n");
  rval.cat(".subtitle { font-size: 0.8em }\n");
  rval.cat(".small-subtitle { font-size: 0.65em }\n");
  rval.cat("</style>\n");
  //  rval.cat("<link href=\"classic.css\" rel=\"stylesheet\" type=\"text/css\" />\n");
  rval.cat("</head>\n");
  rval.cat("<body>\n");
  rval.cat("<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"100%\">\n");
  rval.cat("<tr>\n");
  rval.cat("<td align=\"left\" valign=\"top\" width=\"32\"><a href=\"").cat(taMisc::web_home).cat("\">");
  rval.cat(taMisc::app_name).cat("</a> ").cat(taMisc::version).cat("</td>\n");
  rval.cat("<td width=\"1\">&nbsp;&nbsp;</td><td class=\"postheader\" valign=\"center\">");
  rval.cat("<a href=\"").cat(taMisc::web_home).cat("\"><font color=\"#004faf\">Home</font></a>&nbsp;&middot; ");
  rval.cat("<a href=\"").cat(wiki_help_url).cat(name).cat("\"><font color=\"#004faf\">Wiki Docs For: ").cat(name).cat("</font></a>&nbsp;&middot; ");
  rval.cat("<a href=\"ta:.Type.").cat(name).cat("\"><font color=\"#004faf\">Emergent&nbsp;Help&nbsp;Browser</font></a>&nbsp;\n");

  rval.cat("<td align=\"right\" valign=\"top\" width=\"230\"></td></tr></table><h1 class=\"title\">");
  rval.cat(name).cat(" Class Reference<br />\n");
  // <span class=\"small-subtitle\">[<a href="qtgui.html">QtGui</a> module]</span>
  rval.cat("</h1>\n\n");

  rval.cat("<p>").cat(trim(desc).xml_esc()).cat("</p>\n");
  rval.cat("<p>See for more info: <a href=\"").cat(wiki_help_url).cat(name).cat("\">Wiki Docs For: ").cat(name).cat("</a></p>\n\n");

  // NOTE: for include file tracking -- not a bad idea to actually record this info in maketa!!
  //  rval.cat("<pre> #include &lt;QTextDocument&gt;</pre>\n");

  if(par_cache.size > 0) {
    int inhi = 0;
    rval.cat("<p>Inherits From: ");
    for(int i=0;i<par_cache.size; i++) {
      TypeDef* par = par_cache[i];
      if(par->InheritsFormal(TA_templ_inst)) continue; // none of those
      if(inhi > 0) rval.cat(", ");
      rval.cat(par->GetHTMLLink(gendoc));
      inhi++;
    }
    rval.cat("</p>\n\n");
  }

  if(children.size > 0) {
    int inhi = 0;
    rval.cat("<p>Inherited By: ");
    for(int i=0;i<children.size; i++) {
      TypeDef* par = children[i];
      if(par->InheritsFormal(TA_templ_inst)) continue; // none of those
      if(inhi > 0) rval.cat(", ");
      rval.cat(par->GetHTMLLink(gendoc));
      inhi++;
    }
    rval.cat("</p>\n\n");
  }

  if(taMisc::help_detail >= taMisc::HD_DETAILS) {
    rval.cat("<p> Size: ").cat(String(this->size)).cat("</p>\n");
    if(opts.size > 0) {
      rval.cat("<p>").cat(GetOptsHTML()).cat("</p>\n");
    }
  }

  rval.cat("<p>Index: <a href=\"#subtypes\">SubTypes</a>, ");

  rval.cat("<a href=\"#members\">Members</a>, ");
  rval.cat("<a href=\"#methods\">Methods</a>, ");
  rval.cat("<a href=\"#expert_members\">Expert Members</a>, ");
  rval.cat("<a href=\"#expert_methods\">Expert Methods</a></p>");

  rval.cat("<a name=\"subtypes\"></a>\n");
  rval.cat("<h3>Sub Types</h3>\n");
  if(sub_types.size > 0) {
    rval.cat("<ul>\n");
    for(int i=0;i<sub_types.size;i++) {
      TypeDef* st = sub_types[i];
      if(!st->InheritsFormal(&TA_enum)) continue;
      if((this != &TA_taBase) && (st->GetOwnerType() == &TA_taBase)) continue;
      if(st->GetOwnerType()->InheritsFormal(TA_templ_inst)) continue;
      rval.cat("<li>").cat(st->GetHTMLSubType(gendoc, true)).cat("</li>\n"); // true=short fmt
    }
    rval.cat("</ul>\n");
  }

  /////////////////////////////////////////////////////
  // now for full detail render

  if(sub_types.size > 0) {
    rval.cat("<hr />\n");
    rval.cat("<h2>SubType Documentation</h2>\n");

    for(int i=0;i<sub_types.size;i++) {
      TypeDef* st = sub_types[i];
      if(!st->InheritsFormal(&TA_enum)) continue;
      if((this != &TA_taBase) && (st->GetOwnerType() == &TA_taBase)) continue;
      if(st->GetOwnerType()->InheritsFormal(TA_templ_inst)) continue;
      rval.cat(st->GetHTMLSubType(gendoc, false));
    }
  }

  ///////////////////////////////////////////////////////
  //    REGULAR members and methods
  {
    rval.cat("<hr />\n");
    rval.cat("<h2>Regular (preferred) Member and Method Documentation</h2>\n");

    /////////////////////////////////////////////////////////////////////
    // collect the members and methods into string arrays for sorting
    String_PArray memb_idx;
    for(int i=0;i<members.size;i++) {
      MemberDef* md = members[i];
      if(md->HasOption("NO_SHOW") || md->HasOption("HIDDEN") || md->HasOption("EXPERT"))
        continue;
      if((this != &TA_taBase) && (md->GetOwnerType() == &TA_taBase)) continue;
      String cat = md->GetCat();
      if(cat.empty()) cat = "_NoCategory";
      String key = cat + ":" + md->name;
      memb_idx.Add(key);
    }
    memb_idx.Sort();              // sorts by category then key, in effect

    String_PArray meth_idx;
    for(int i=0;i<methods.size;i++) {
      MethodDef* md = methods[i];
      if(md->HasOption("EXPERT")) continue;
      if((this != &TA_taBase) && (md->GetOwnerType() == &TA_taBase)) continue;
      String cat = md->GetCat();
      if(cat.empty()) cat = "_NoCategory";
      String key = cat + ":" + md->name;
      meth_idx.Add(key);
    }
    meth_idx.Sort();              // sorts by category then key, in effect

    rval.cat(GetHTMLMembMeth(memb_idx, meth_idx, "", "", gendoc));
  }

  ///////////////////////////////////////////////////////
  //    EXPERT members and methods
  {
    rval.cat("<hr />\n");
    rval.cat("<h2>Expert Member and Method Documentation</h2>\n");

    /////////////////////////////////////////////////////////////////////
    // collect the members and methods into string arrays for sorting
    String_PArray memb_idx;
    for(int i=0;i<members.size;i++) {
      MemberDef* md = members[i];
      if(!(md->HasOption("NO_SHOW") || md->HasOption("HIDDEN") || md->HasOption("EXPERT")))
        continue;
      if((this != &TA_taBase) && (md->GetOwnerType() == &TA_taBase)) continue;
      String cat = md->GetCat();
      if(cat.empty()) cat = "_NoCategory";
      String key = cat + ":" + md->name;
      memb_idx.Add(key);
    }
    memb_idx.Sort();              // sorts by category then key, in effect

    String_PArray meth_idx;
    for(int i=0;i<methods.size;i++) {
      MethodDef* md = methods[i];
      if(!md->HasOption("EXPERT")) continue;
      if((this != &TA_taBase) && (md->GetOwnerType() == &TA_taBase)) continue;
      String cat = md->GetCat();
      if(cat.empty()) cat = "_NoCategory";
      String key = cat + ":" + md->name;
      meth_idx.Add(key);
    }
    meth_idx.Sort();              // sorts by category then key, in effect

    rval.cat(GetHTMLMembMeth(memb_idx, meth_idx, "Expert ", "expert_", gendoc));
  }

  /////////////////////////
  // postscript/footer
  rval.cat("<p /><address><hr /><div align=\"center\">\n");
  rval.cat("<table width=\"100%\" cellspacing=\"0\" border=\"0\"><tr class=\"address\">\n");
  rval.cat("<td width=\"70%\" align=\"left\">Copyright &copy; 2011 Regents of the University of Colorado, Carnegie Mellon University, Princeton University.</td>\n");
  //  rval.cat("<td width=\"40%\" align=\"center\"><a href=\"trademarks.html\">Trademarks</a></td>\n");
  rval.cat("<td width=\"30%\" align=\"right\"><div align=\"right\"> ").cat(taMisc::app_name).cat(" ").cat(taMisc::version).cat("</div></td>\n");
  rval.cat("</tr></table></div></address></body>\n");
  rval.cat("</html>\n");
  return rval;
}


String TypeDef::GetHTMLMembMeth(String_PArray& memb_idx, String_PArray& meth_idx,
        const String& label_prefix, const String& link_prefix, bool gendoc) const {

  String rval;

  //////////////////////////////////////////
  // first pass output

  rval.cat("<a name=\"" + link_prefix + "members\"></a>\n");
  rval.cat("<h3>" + label_prefix + "Members</h3>\n");
  if(memb_idx.size > 0) {
    String prv_cat;
    for(int i=0;i<memb_idx.size;i++) {
      String key = memb_idx[i];
      String cat = key.before(':');
      String mnm = key.after(':');
      MemberDef* md = members.FindName(mnm);
      if(cat != prv_cat) {
        if(prv_cat.nonempty()) rval.cat("</ul>\n"); // terminate prev
        rval.cat("<h4>" + label_prefix + "Member Category: <a href=\"#MbrCat-").cat(cat).cat("\">").cat(cat).cat("</a></h4>\n");
        rval.cat("<ul>\n");
        prv_cat = cat;
      }
      rval.cat("<li>").cat(md->GetHTML(gendoc, true)).cat("</li>\n"); // true = short_fmt
    }
    rval.cat("</ul>\n");
  }

  rval.cat("<a name=\"" + link_prefix + "methods\"></a>\n");
  rval.cat("<h3>" + label_prefix + "Methods</h3>\n");
  if(meth_idx.size > 0) {
    String prv_cat;
    for(int i=0;i<meth_idx.size;i++) {
      String key = meth_idx[i];
      String cat = key.before(':');
      String mnm = key.after(':');
      MethodDef* md = methods.FindName(mnm);
      if(cat != prv_cat) {
        if(prv_cat.nonempty()) rval.cat("</ul>\n"); // terminate prev
        rval.cat("<h4>" + label_prefix + "Method Category: <a href=\"#MthCat-").cat(cat).cat("\">").cat(cat).cat("</a></h4>\n");
        rval.cat("<ul>\n");
        prv_cat = cat;
      }
      rval.cat("<li>").cat(md->GetHTML(gendoc, true)).cat("</li>\n"); // true = short_fmt
    }
    rval.cat("</ul>\n");
  }

  if(memb_idx.size > 0) {
    rval.cat("<hr />\n");
    rval.cat("<h2>Member Documentation</h2>\n");

    String prv_cat;
    for(int i=0;i<memb_idx.size;i++) {
      String key = memb_idx[i];
      String cat = key.before(':');
      String mnm = key.after(':');
      MemberDef* md = members.FindName(mnm);
      if(cat != prv_cat) {
        rval.cat("<a name=\"MbrCat-").cat(cat).cat("\"></a>\n");
        rval.cat("<h3>Member Category: ").cat(cat).cat("</h3>\n");
        prv_cat = cat;
      }
      rval.cat(md->GetHTML(gendoc, false)).cat("\n"); // extra cr for good readability
    }
  }

  if(methods.size > 0) {
    rval.cat("<hr />\n");
    rval.cat("<h2>Method Documentation</h2>\n");

    String prv_cat;
    for(int i=0;i<meth_idx.size;i++) {
      String key = meth_idx[i];
      String cat = key.before(':');
      String mnm = key.after(':');
      MethodDef* md = methods.FindName(mnm);
      if(cat != prv_cat) {
        rval.cat("<a name=\"MthCat-").cat(cat).cat("\"></a>\n");
        rval.cat("<h3>Method Category: ").cat(cat).cat("</h3>\n");
        prv_cat = cat;
      }
      rval.cat(md->GetHTML(gendoc, false)).cat("\n"); // extra cr for good readability
    }
  }
  return rval;
}

String TypeDef::Includes() {
  String inc_str;
  TypeSpace inc_list;

  inc_list.Link(this);		// don't redo ourselves!
  inc_list.Link(&TA_Variant);	// exclude common things
  inc_list.Link(&TA_taString);	

  inc_str << "\n// parent includes:";
  for(int i=0; i< parents.size; i++) {
    TypeDef* par = parents[i];
    if(par->InheritsFormal(TA_templ_inst)) {
      for(int j=0; j<par->templ_pars.size; j++) {
	TypeDef* tp = par->templ_pars[j];
	if(tp->IsClass() && tp->IsAnchor()) {
	  inc_str << "\n#include <" << tp->name << ">";
	  inc_list.Link(tp);
	}
      }
      par = par->parents[0];	// now go to template itself
    }
    inc_str << "\n#include <" << par->name << ">";
    inc_list.Link(par);
  }

  inc_str << "\n\n// member includes:";
  for(int i=0; i< members.size; i++) {
    MemberDef* md = members[i];
    if(md->GetOwnerType() != this) continue;
    TypeDef* mtyp = md->type;
    if(mtyp->IsClass() && mtyp->IsAnchor() && mtyp->name != "taBasePtr") {
      if(inc_list.FindEl(mtyp) < 0) {
	inc_str << "\n#include <" << mtyp->name << ">";
	inc_list.Link(mtyp);
      }
    }
  }

  inc_str << "\n\n// declare all other types mentioned but not required to include:";
  for(int i=0; i< members.size; i++) {
    MemberDef* md = members[i];
    if(md->GetOwnerType() != this) continue;
    TypeDef* cltyp = md->type->GetClassType();
    if(cltyp && cltyp->IsClass() && cltyp->IsAnchor()) {
      if(inc_list.FindEl(cltyp) < 0) {
	inc_str << "\nclass " << cltyp->name << "; // ";
	inc_list.Link(cltyp);
      }
    }
  }
  for(int i=0; i< methods.size; i++) {
    MethodDef* md = methods[i];
    if(md->GetOwnerType() != this) continue;
    { // return type
      TypeDef* argt = md->type;
      TypeDef* cltyp = argt->GetClassType();
      if(cltyp && cltyp->IsClass() && cltyp->IsAnchor()) {
	if(inc_list.FindEl(cltyp) < 0) {
	  inc_str << "\nclass " << cltyp->name << "; // ";
	  inc_list.Link(cltyp);
	}
      }
    }
    for(int j=0; j< md->arg_types.size; j++) {
      TypeDef* argt = md->arg_types[j];
      TypeDef* cltyp = argt->GetClassType();
      if(cltyp && cltyp->IsClass() && cltyp->IsAnchor()) {
	if(inc_list.FindEl(cltyp) < 0) {
	  inc_str << "\nclass " << cltyp->name << "; // ";
	  inc_list.Link(cltyp);
	}
      }
    }
  }
  inc_str << "\n\n\n";
  return inc_str;
}


bool TypeDef::CreateNewSrcFiles(const String& top_path, const String& src_dir) {
  bool new_file = taMisc::CreateNewSrcFiles(name, top_path, src_dir);
  String fname = name;
  if(!new_file)
    fname += "_new";		// create a new guy..

  String src_path = top_path + "/" + src_dir + "/";
  String hfile = src_path + name + ".h";

  fstream hstrm;
  hstrm.open(hfile, ios::in);
  String hstr;
  hstr.Load_str(hstrm);
  hstrm.close();

  if(hstr.contains("\n// parent includes:"))
    hstr = hstr.before("\n// parent includes:");
  else if(hstr.contains("\n#include"))
    hstr = hstr.before("\n#include");
  else
    hstr = hstr.before("\n#endif");

  String incs = Includes();

  hstr << incs;
  
  String cmd;
  cmd << "sed -n " << source_start << "," << source_end << "p "
      << src_path << source_file << " > " << src_path << "create_new_src_src";
  taMisc::ExecuteCommand(cmd);
  
  fstream srcstrm;
  srcstrm.open(src_path + "create_new_src_src", ios::in);
  String srcstr;
  srcstr.Load_str(srcstrm);
  srcstrm.close();

  hstr << srcstr << "\n";

  hstr << "#endif // " << name << "_h\n";

  fstream strm;
  strm.open(src_path + fname + ".h", ios::out);
  hstr.Save_str(strm);
  strm.close();

  return new_file;
}

#ifndef NO_TA_BASE

#include <taObjDiffRec>
#include <taObjDiff_List>

void TypeDef::GetObjDiffVal(taObjDiff_List& odl, int nest_lev, const void* base,
                            MemberDef* memb_def, const void* par,
                            TypeDef* par_typ, taObjDiffRec* par_od) const {
  if(InheritsFrom(TA_taBase)) {
    taBase* rbase = (taBase*)base;
    if(rbase) {
      rbase->GetObjDiffVal(odl, nest_lev, memb_def, par, par_typ, par_od);
    }
    return;
  }
  // always just add a record for this guy
  taObjDiffRec* odr = new taObjDiffRec(odl, nest_lev, const_cast<TypeDef*>(this),
                                       memb_def, (void*)base,
                                       (void*)par, par_typ, par_od);
  odl.Add(odr);
//   if(ptr > 0) {
//     // this will be set wrong here..  need some complex logic probaly to fix it..
//   }

  // then check for classes
  if(ptr == 0 && InheritsNonAtomicClass()) {
    GetObjDiffVal_class(odl, nest_lev, base, memb_def, par, par_typ, odr);
  }
}

void TypeDef::GetObjDiffVal_class(taObjDiff_List& odl, int nest_lev, const void* base,
                                  MemberDef* memb_def, const void* par, TypeDef* par_typ,
                                  taObjDiffRec* par_od) const {
  MemberDef* last_md = NULL;
  for(int i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    if(md->HasOption("NO_SAVE") || md->HasOption("READ_ONLY") ||
       md->HasOption("GUI_READ_ONLY")
       || md->HasOption("HIDDEN"))
      continue;
    if(par_od && par_od->mdef) {
      // object is a member
      if(md->HasOption("HIDDEN_INLINE")) continue;
    }
    if(md->HasOption("DIFF_LAST")) {
      last_md = md;
      continue;
    }
    // handle this in display -- doing it here causes imbalances in diffs that are bad!
    // if(!md->GetCondOptTest("CONDSHOW", this, base))
    //   continue;
    if(md->name == "user_data_") {
      continue;                 // too much clutter for now..
    }
    md->type->GetObjDiffVal(odl, nest_lev+1, md->GetOff(base), md, base, const_cast<TypeDef*>(this), par_od);
  }
  if(last_md) {
    last_md->type->GetObjDiffVal(odl, nest_lev+1, last_md->GetOff(base), last_md, base,
                                 const_cast<TypeDef*>(this), par_od);
  }
}

#endif // NO_TA_BASE

#ifdef NO_TA_BASE

//////////////////////////////////////////////////////////
//  dummy versions of dump load/save: see ta_dump.cc    //
//////////////////////////////////////////////////////////

int TypeDef::Dump_Save_Path(ostream&, void*, void*, int) {
  return true;
}

int TypeDef::Dump_Save_PathR(ostream&, void*, void*, int) {
  return false;
}

int TypeDef::Dump_Save_Value(ostream&, void*, void*, int) {
  return true;
}

int TypeDef::Dump_Save_impl(ostream&, void*, void*, int) {
  return true;
}

int TypeDef::Dump_Save_inline(ostream&, void*, void*, int) {
  return true;
}

int TypeDef::Dump_Save(ostream&, void*, void*, int) {
  return true;
}

int TypeDef::Dump_SaveR(ostream&, void*, void*, int) {
  return false;
}

int TypeDef::Dump_Load_Path(istream&, void*&, void*, TypeDef*&, String&, const char*) {
  return true;
}

int TypeDef::Dump_Load_Path_impl(istream&, void*&, void*, String) {
  return true;
}

int TypeDef::Dump_Load_Value(istream&, void*, void*) {
  return true;
}

int TypeDef::Dump_Load_impl(istream&, void*, void*, const char*) {
  return true;
}

int TypeDef::Dump_Load(istream&, void*, void*, void**) {
  return true;
}

#endif // NO_TA_BASE
