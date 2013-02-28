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
#include <BuiltinTypeDefs>

#include <taMisc>

#ifndef NO_TA_BASE
#include <taBase>
#include <UserDataItem_List>
#include <taiType>
#include <taiEdit>
#include <taiViewType>
#include <DynEnum>
#include <taSmartRef>
#include <tabMisc>
#include <taRootBase>
#include <dumpMisc>

#ifdef DMEM_COMPILE
#include <DMemShare>
#endif
#endif

using namespace std;


void TypeDef::Initialize() {
  type = VOID;
  init_flag = IF_NONE;
  owner = NULL;
  size = 0;
  source_start = -1;
  source_end = -1;

#ifdef TA_GUI
  it = NULL;
  ie = NULL;
  iv = NULL;
#endif
#ifndef NO_TA_BASE
  plugin = NULL; // set true by TypeSpace::SetOwner if initing a plugin
  instance = NULL;
  defaults = NULL;
  schema = NULL;
#endif

  parents.name = "parents";
  parents.owner = this;
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
  templ_defs.name = "templ_defs";
  templ_defs.owner = this;
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
		 int type_flags, uint siz, bool global_obj)
#else
TypeDef::TypeDef(const char* nm, const char* dsc, const char* inop, const char* op,
		 const char* lis, const char* src_file, int src_st, int src_ed,
		 int type_flags, uint siz, void** inst, bool toks, bool global_obj)
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
  name = nm;
  desc = dsc;
  taMisc::CharToStrArray(opts,op);
  taMisc::CharToStrArray(inh_opts,inop);
  taMisc::CharToStrArray(lists,lis);
#ifndef NO_TA_BASE
  CleanupCats(true);            // save the last one for initialization
#endif
  type = (TypeType)type_flags;
  size = siz;
  if(global_obj)
    taRefN::Ref(this);          // reference if static (non-new'ed) global object
}

TypeDef::TypeDef(const char* nm, int type_flags, bool global_obj, uint siz,
                 const char* c_nm)
:inherited()
{
  Initialize();
  name = nm; 
  type = (TypeType)type_flags;
  size = siz; // note: may get updated later
  if(IsAnyPtr()) {
    size = sizeof(void*);
  }
  if (c_nm) c_name = c_nm;
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
#ifndef NO_TA_BASE
  plugin = cp.plugin;
  instance = cp.instance ;
  //TODO: copy the schema
// don't copy the tokens..
#endif
  c_name        = cp.c_name;
  type          = cp.type    ;
  size          = cp.size    ;

  inh_opts      = cp.inh_opts ;

  parents       = cp.parents  ;
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
  templ_defs    = cp.templ_defs;

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

void TypeDef::CopyActualType(const TypeDef& cp) {
  int my_mods = type & ALL_MODS;
  type = cp.type;               // grab their values
  ClearType(ALL_MODS);     // erase any of their mods
  SetType((TypeType)my_mods);        // reinstate my mods
}

#ifndef NO_TA_BASE
void TypeDef::AddUserDataSchema(UserDataItemBase* item) {
  if (!schema) {
    schema = new UserDataItem_List;
  }
  schema->Add(item);
}
#endif

bool TypeDef::IsSubType() const {
  if(!HasType(SUBTYPE)) return false;
  if(owner == NULL || owner->owner == NULL) {
    taMisc::Warning("type marked SUBTYPE has null owner!", name);
    const_cast<TypeDef*>(this)->ClearType(SUBTYPE);
    return false;
  }
  return true;
}

bool TypeDef::IsBasePointerType() const {
#ifndef NO_TA_BASE
  if((IsPointer() && IsTaBase()) ||
     (!IsAnyPtr() && IsSmartPtr() && (DerivesFrom(TA_taSmartPtr) ||
                                      DerivesFrom(TA_taSmartRef))) )
    return true;
#endif
  return false;
}

bool TypeDef::IsVarCompat() const {
  // a few "blockers"
  if (IsRef() || IsPtrPtr() || IsVoid()) return false;
#ifndef NO_TA_BASE
  if (IsPointer()) {
    if(IsTaBase()) return true;
    else return false;          // no other pointers supported
  }
#endif
  if (!IsAtomic()) return false;
  // ok, hopefully the rest are ok!
  return true;
}

String TypeDef::GetTypeEnumString() const {
  String rval;
  if(HasType(VOID)) rval += "|TypeDef::VOID";
  if(HasType(BOOL)) rval += "|TypeDef::BOOL";
  if(HasType(INTEGER)) rval += "|TypeDef::INTEGER";
  if(HasType(ENUM)) rval += "|TypeDef::ENUM";
  if(HasType(FLOAT)) rval += "|TypeDef::FLOAT";
  if(HasType(STRING)) rval += "|TypeDef::STRING";
  if(HasType(VARIANT)) rval += "|TypeDef::VARIANT";
  if(HasType(SMART_PTR)) rval += "|TypeDef::SMART_PTR";
  if(HasType(SMART_INT)) rval += "|TypeDef::SMART_INT";
  if(HasType(CLASS)) rval += "|TypeDef::CLASS";
  if(HasType(TEMPLATE)) rval += "|TypeDef::TEMPLATE";
  if(HasType(TEMPL_INST)) rval += "|TypeDef::TEMPL_INST";
  if(HasType(STRUCT)) rval += "|TypeDef::STRUCT";
  if(HasType(UNION)) rval += "|TypeDef::UNION";
  if(HasType(FUNCTION)) rval += "|TypeDef::FUNCTION";
  if(HasType(METHOD)) rval += "|TypeDef::METHOD";
  if(HasType(TABASE)) rval += "|TypeDef::TABASE";
  if(HasType(SIGNED)) rval += "|TypeDef::SIGNED";
  if(HasType(UNSIGNED)) rval += "|TypeDef::UNSIGNED";
  
  if(HasType(POINTER)) rval += "|TypeDef::POINTER";
  if(HasType(PTR_PTR)) rval += "|TypeDef::PTR_PTR";
  if(HasType(REFERENCE)) rval += "|TypeDef::REFERENCE";
  if(HasType(ARRAY)) rval += "|TypeDef::ARRAY";
  if(HasType(CONST)) rval += "|TypeDef::CONST";
  if(HasType(SUBTYPE)) rval += "|TypeDef::SUBTYPE";

  if(rval.startsWith("|")) rval = rval.after("|");
  return rval;
}


TypeDef* TypeDef::FindGlobalTypeName(const String& nm, bool err_not_found) {
  if(nm.contains("::")) {
    String typnm = nm.before("::");
    String subnm = nm.after("::");
    TypeDef* typ = taMisc::types.FindName(typnm);
    if(!typ) {
      if(err_not_found) {
        taMisc::Error("FindGlobalTypeName: type named:", typnm, "not found!");
      }
      return NULL;
    }
    TypeDef* sub = typ->FindSubType(subnm);
    if(!sub) {
      if(err_not_found) {
        taMisc::Error("FindGlobalTypeName: sub type named:", subnm,
                      "not found in parent class of type:", typnm);
      }
      return NULL;
    }
    return sub;
  }
  TypeDef* typ = taMisc::types.FindName(nm);
  if(!typ && err_not_found) {
    taMisc::Error("FindGlobalTypeName: type named:", nm, "not found!");
  }
  return typ;
}

void TypeDef::AddNewGlobalType(bool make_derived) {
  taMisc::types.Add(this);
  if(make_derived) {
    MakeMainDerivedTypes_impl(taMisc::types);
  }
}

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
  for(int i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    if(md->owner == &(old->members))
      members.ReplaceIdx(i, md->Clone());
  }
  for(int i=0; i<methods.size; i++) {
    MethodDef* md = methods.FastEl(i);
    if(md->owner == &(old->methods))
      methods.ReplaceIdx(i, md->Clone());
  }
}

void TypeDef::UpdateMDTypes(const TypeSpace& ol, const TypeSpace& nw) {
  for(int i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    if(md->owner != &members)   // only for members we own
      continue;

    for(int j=0; j<ol.size; j++) {
      TypeDef* old_st = ol.FastEl(j);
      TypeDef* new_st = nw.FastEl(j);   // assumes one-to-one correspondence

      if(md->type == old_st)
        md->type = new_st;
      if(md->type->IsTemplInst()) {
        if(md->type->templ_pars.ReplaceLinkAll(old_st, new_st)) {
          // update name after replacing
          md->type->name = md->type->name.before("_"); // todo: brazen hack
          md->type->name = md->type->GetTemplInstName(md->type->templ_pars);
        }
      }
    }
  }
  for(int i=0; i<methods.size; i++) {
    MethodDef* md = methods.FastEl(i);
    if(md->owner != &methods)
      continue;

    for(int j=0; j<ol.size; j++) {
      TypeDef* old_st = ol.FastEl(j);
      TypeDef* new_st = nw.FastEl(j);   // assumes one-to-one correspondence

      if(md->type == old_st)
        md->type = new_st;

      if(md->type->IsTemplInst()) {
        md->type->templ_pars.ReplaceLinkAll(old_st, new_st);
      }

      md->arg_types.ReplaceLinkAll(old_st, new_st);
    }
  }
}

bool TypeDef::CheckList(const String_PArray& lst) const {
  for (int i = 0; i < lists.size; i++) {
    if (lst.FindEl(lists.FastEl(i)) >= 0)
      return true;
  }
  return false;
}

TypeDef* TypeDef::GetNonPtrType() const {
  if(!IsAnyPtr())    return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while(rval->GetParent() != NULL) {
    rval = rval->GetParent();
    if(!rval->IsAnyPtr())
      return rval;
  }
  return rval;                  // always return something
}

TypeDef* TypeDef::GetNonRefType() const {
  if(!IsRef())    return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while(rval->GetParent() != NULL) {
    rval = rval->GetParent();
    if(!rval->IsRef())
      return rval;
  }
  return rval;                  // always return something
}

TypeDef* TypeDef::GetNonConstType() const {
  if(!IsConst())    return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while((rval = rval->parents.Peek()) != NULL) { // use the last parent, not the 1st
    if(!rval->IsConst())
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetActualType() const {
  if(IsActual()) return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while((rval = rval->parents.Peek()) != NULL) { // use the last parent, not the 1st
    if(rval->IsActual())
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetActualClassType() const {
  if(!IsClass()) return NULL;
  return GetActualType();
}

TypeDef* TypeDef::GetTemplType() const {
  if(IsTemplate()) return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while((rval = rval->GetParent()) != NULL) {
    if(rval->IsTemplate())
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetTemplInstType() const {
  if(IsTemplInst()) return const_cast<TypeDef*>(this);

  TypeDef* rval = const_cast<TypeDef*>(this);
  while((rval = rval->GetParent()) != NULL) {
    if(rval->IsTemplInst())
      return rval;
  }
  return NULL;
}

TypeDef::TypeType TypeDef::GetPtrTypeFlag() const {
  TypeType rval = POINTER;
  if(IsPointer()) {
    rval = PTR_PTR;
  }
  else if(IsPtrPtr()) {
    taMisc::Error("GetPtrTypeFlag: cannot get a pointer to a pointer-pointer!", name);
    rval = PTR_PTR;
  }
  return rval;
}

TypeDef* TypeDef::GetPtrType() const {
  if(owner)
    return GetPtrType_impl(*owner);
  return GetPtrType_impl(taMisc::types);
}

TypeDef* TypeDef::GetPtrType_impl(TypeSpace& make_spc) const {
  String nm = name + "_ptr";
  TypeDef* rval = children.FindName(nm);
  TypeType ptr_flag = GetPtrTypeFlag();
  if (!rval) {
    // need to make one, we use same pattern as maketa
    rval = new TypeDef(nm, type, false, 0);
    make_spc.Add(rval);
    // unconstify us, this is an internal operation, still considered "const" access
    rval->AddParent(const_cast<TypeDef*>(this));
    // if(rval->IsVoid() && !rval->name.contains("void")) {
    //   cerr << "oops -- GetPtrType created a void!: " << rval->name
    //        << " " << rval->GetTypeEnumString() << endl;
    // }
  }
  rval->ClearType(ANY_PTR);   // get rid of any existing pointer flags
  rval->SetType(ptr_flag);    // set new ones
  return rval;
}

TypeDef* TypeDef::GetRefType() const {
  if(owner)
    return GetRefType_impl(*owner);
  return GetRefType_impl(taMisc::types);
}

TypeDef* TypeDef::GetRefType_impl(TypeSpace& make_spc) const {
  String nm = name + "_ref";
  TypeDef* rval = children.FindName(nm);
  if (!rval) {
    // need to make one, we use same pattern as maketa
    rval = new TypeDef(nm, type, false, size);
    make_spc.Add(rval);
    // unconstify us, this is an internal operation, still considered "const" access
    rval->AddParent(const_cast<TypeDef*>(this));
    // if(rval->IsVoid() && !rval->name.contains("void")) {
    //   cerr << "oops -- GetRefType created a void!: " << rval->name
    //        << " " << rval->GetTypeEnumString() << endl;
    // }
  }
  rval->SetType(REFERENCE);   // set new ones
  return rval;
}

TypeDef* TypeDef::GetConstType() const {
  if(owner)
    return GetConstType_impl(*owner);
  return GetConstType_impl(taMisc::types);
}

TypeDef* TypeDef::GetConstType_impl(TypeSpace& make_spc) const {
  String nm = "const_" + name;
  TypeDef* rval = children.FindName(nm);
  if (!rval) {
    // need to make one, we use same pattern as maketa
    rval = new TypeDef(nm, type, false, size);
    make_spc.Add(rval);
    // unconstify us, this is an internal operation, still considered "const" access
    rval->AddParent(const_cast<TypeDef*>(this));
    // if(rval->IsVoid() && !rval->name.contains("void")) {
    //   cerr << "oops -- GetConstType created a void!: " << rval->name
    //        << " " << rval->GetTypeEnumString() << endl;
    // }
  }
  rval->SetType(CONST);   // set new ones
  return rval;
}

TypeDef* TypeDef::GetArrayType() const {
  if(owner)
    return GetArrayType_impl(*owner);
  return GetArrayType_impl(taMisc::types);
}

TypeDef* TypeDef::GetArrayType_impl(TypeSpace& make_spc) const {
  String nm = name + "_ary";
  TypeDef* rval = children.FindName(nm);
  if (!rval) {
    // need to make one, we use same pattern as maketa
    rval = new TypeDef(nm, type, false, size);
    make_spc.Add(rval);
    // unconstify us, this is an internal operation, still considered "const" access
    rval->AddParent(const_cast<TypeDef*>(this));
  }
  rval->SetType(ARRAY);   // set new ones
  return rval;
}

void TypeDef::MakeMainDerivedTypes() {
  if(owner)
    return MakeMainDerivedTypes_impl(*owner);
  return MakeMainDerivedTypes_impl(taMisc::types);
}

void TypeDef::MakeMainDerivedTypes_impl(TypeSpace& make_spc) {
  TypeDef* ptr = GetPtrType_impl(make_spc);
  if(!ptr->IsPtrPtr()) {
    ptr->GetPtrType_impl(make_spc);
  }
  TypeDef* ref = GetRefType_impl(make_spc);
  ptr->GetRefType_impl(make_spc);
  GetConstType_impl(make_spc);
  ref->GetConstType_impl(make_spc);
  ptr->GetConstType_impl(make_spc);
}


TypeDef* TypeDef::GetPluginType() const {
#ifdef NO_TA_BASE
  return NULL;
#else
  return plugin;
#endif
}

String TypeDef::GetPtrString() const {
  if(IsPointer()) return "*";
  if(IsPtrPtr()) return "**";
  return _nilString;
}


String TypeDef::Get_C_Name() const {
  if(c_name.nonempty()) {
    return c_name; // cached -- send it!
  }

  String rval;
  if(namespc.nonempty())
    rval += namespc + "::";

  if (IsSubType() && (owner != NULL) && (owner->owner != NULL) && (owner->owner != this)) {
    rval += owner->owner->Get_C_Name() + "::";
  }

  if(IsRef()) {
    TypeDef *nrt = GetNonRefType();
    if (!nrt) {
      taMisc::Error("Null NonRefType in TypeDef::Get_C_Name()", name);
      rval += name;
      return rval;
    }
    if (nrt == this) return name + "&";
    rval += nrt->Get_C_Name() + "&";
    const_cast<TypeDef*>(this)->c_name = rval;              // cache
    return rval;
  }

  if(IsAnyPtr()) {
    TypeDef *npt = GetNonPtrType();
    if (!npt) {
      taMisc::Error("Null NonPtrType in TypeDef::Get_C_Name()", name);
      rval += name;
      return rval;
    }
    if (npt == this) return name + GetPtrString();
    rval += npt->Get_C_Name() + GetPtrString();
    const_cast<TypeDef*>(this)->c_name = rval;              // cache
    return rval;
  }

  if(IsConst()) {
    TypeDef *nrt = GetNonConstType();
    if (!nrt) {
      taMisc::Error("Null NonConstType in TypeDef::Get_C_Name()", name);
      rval += name;
      return rval;
    }
    if (nrt == this) return "const " + name;
    rval += "const " + nrt->Get_C_Name();
    const_cast<TypeDef*>(this)->c_name = rval;              // cache
    return rval;
  }

  // combo type
  if ((parents.size > 1) && !IsClass()) {
    for (int i = 0; i < parents.size; i++) {
      TypeDef* pt = parents.FastEl(i);
      rval += pt->Get_C_Name();
      if (i < parents.size-1) rval += " ";
    }
    const_cast<TypeDef*>(this)->c_name = rval;              // cache
    return rval;
  }

  if (IsTemplInst() && (templ_pars.size > 0)) {
    TypeDef* tmpar = GetTemplType();
    if (!tmpar) {
      taMisc::Error("Null TemplParent in TypeDef::Get_C_Name()", name);
      rval += name;
      return rval;
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

  rval += name;                       // the default
  const_cast<TypeDef*>(this)->c_name = rval;
  return rval;
}

const String TypeDef::GetPathName() const {
  String rval;
  TypeDef* owtp = GetOwnerType();
  if (owtp) {
    rval = owtp->GetPathName() + "::";
  }
  rval += name;
  return rval;
}

String TypeDef::GetUniqueName() const {
  String rval;
  TypeDef* owtp = GetOwnerType();
  if (owtp) {
    rval = owtp->GetUniqueName() + "_";
  }
  rval += name;
  return rval;
}

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

TypeDef* TypeDef::AddParent(TypeDef* it, int p_off) {
  if(it == NULL) return NULL;

  if(parents.LinkUnique(it))
    par_off.Add(p_off);         // it was unique, add offset

  it->children.Link(this);

  opts.DupeUnique(it->inh_opts);
  inh_opts.DupeUnique(it->inh_opts);    // and so on

  // note: type flags are set explicitly prior to calling AddParent!

  if(IsTaBase())
    AddOption(opt_instance);       // ta_bases always have an instance

#ifndef NO_TA_BASE
  CleanupCats(false);           // save first guy for add parent!
#endif

  // use the old one because the parent does not have precidence over existing
  enum_vals.BorrowUniqNameOld(it->enum_vals);
  sub_types.BorrowUniqNameOld(it->sub_types);
  members.BorrowUniqNameOld(it->members);
  properties.BorrowUniqNameOld(it->properties);
  methods.BorrowUniqNameOld(it->methods);
  return it;
}

TypeDef* TypeDef::AddParentName(const char* nm, int p_off) {
  TypeDef* par = FindGlobalTypeName(nm, false);
  if(!par) {
    par = new TypeDef(nm);
    par->type = type;           // assume same kind of thing
#ifdef DEBUG
    if(!(par->name.startsWith("Q") || par->name.startsWith("So")
         || par->name.startsWith("Sb") || par->name.startsWith("i")
         || par->name.startsWith("I"))) {
      taMisc::Info("AddParentName -- added new unknown type:",
                   nm, "as parent of type:", name);
    }
#endif
  }

  if(parents.LinkUnique(par))
    par_off.Add(p_off);         // it was unique, add offset

  par->children.Link(this);

  opts.DupeUnique(par->inh_opts);
  inh_opts.DupeUnique(par->inh_opts);    // and so on

  // note: type flags are set explicitly prior to calling AddParent!

  if(IsTaBase())
    AddOption(opt_instance);       // ta_bases always have an instance

#ifndef NO_TA_BASE
  CleanupCats(false);           // save first guy for add parent!
#endif
  
  return par;
}

void TypeDef::AddParentData() {
  for(int i=0; i< parents.size; i++) {
    TypeDef* par = parents[i];
    if(!par->HasInitFlag(IF_PARENT_DATA_ADDED)) {
      par->AddParentData();
    }

    // we already have our data, so we have to put the parent stuff first..
    enum_vals.BorrowUniqNameOldFirst(par->enum_vals);
    sub_types.BorrowUniqNameOldFirst(par->sub_types);
    members.BorrowUniqNameOldFirst(par->members);
    properties.BorrowUniqNameOldFirst(par->properties);
    methods.BorrowUniqNameOldFirst(par->methods);
  }
  SetInitFlag(IF_PARENT_DATA_ADDED);
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

void TypeDef::AddParentNames(const char* p1, const char* p2, const char* p3,
                             const char* p4, const char* p5, const char* p6) {
  if(p1 != NULL)    AddParentName(p1);
  if(p2 != NULL)    AddParentName(p2);
  if(p3 != NULL)    AddParentName(p3);
  if(p4 != NULL)    AddParentName(p4);
  if(p5 != NULL)    AddParentName(p5);
  if(p6 != NULL)    AddParentName(p6);
}

void TypeDef::AddClassPar(TypeDef* p1, int p1_off, TypeDef* p2, int p2_off,
                          TypeDef* p3, int p3_off, TypeDef* p4, int p4_off,
                          TypeDef* p5, int p5_off, TypeDef* p6, int p6_off)
{
  if(p1 != NULL)    AddParent(p1,p1_off);
  if(p2 != NULL)    AddParent(p2,p2_off);
  if(p3 != NULL)    AddParent(p3,p3_off);
  if(p4 != NULL)    AddParent(p4,p4_off);
  if(p5 != NULL)    AddParent(p5,p5_off);
  if(p6 != NULL)    AddParent(p6,p6_off);
  // note: need to call ComputeMembBaseOff(); once all the info is in place for 
  // all parents 
}

void TypeDef::SetParOffsets(int p1_off, int p2_off,
                            int p3_off, int p4_off,
                            int p5_off, int p6_off)
{
  par_off.SetSize(parents.size);
  if(par_off.size >= 1)    par_off[0] = p1_off;
  if(par_off.size >= 2)    par_off[1] = p2_off;
  if(par_off.size >= 3)    par_off[2] = p3_off;
  if(par_off.size >= 4)    par_off[3] = p4_off;
  if(par_off.size >= 5)    par_off[4] = p5_off;
  if(par_off.size >= 6)    par_off[5] = p6_off;
  // note: need to call ComputeMembBaseOff(); once all the info is in place!
  // all parents 
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

void TypeDef::AddTemplParNames(const char* p1, const char* p2, const char* p3,
                               const char* p4, const char* p5, const char* p6) {
  if(p1 != NULL)    AddTemplParName(p1);
  if(p2 != NULL)    AddTemplParName(p2);
  if(p3 != NULL)    AddTemplParName(p3);
  if(p4 != NULL)    AddTemplParName(p4);
  if(p5 != NULL)    AddTemplParName(p5);
  if(p6 != NULL)    AddTemplParName(p6);
}

void TypeDef::AddTemplParName(const char* pn) {
  TypeDef* td = FindGlobalTypeName(pn, false);
  if(!td) {
    td = new TypeDef(pn);
    td->AddNewGlobalType(false);
    taMisc::Info("AddTemplParName -- added new unknown void type as template parameter:",
                 pn);
  }
  templ_pars.Link(td);
}

void TypeDef::CacheParents() {
  par_cache.Reset();            // justin case
  for(int i=0; i<parents.size; i++) {
    parents.FastEl(i)->CacheParents_impl(this);
  }
  par_cache.BuildHashTable(par_cache.size + 2, taHashTable::KT_PTR); // little extra, cache on pointer vals
  SetInitFlag(IF_CACHE_HASH);
}

void TypeDef::CacheParents_impl(TypeDef* src_typ) {
  src_typ->par_cache.LinkUnique(this);
  for(int i=0; i<parents.size; i++) {
    parents.FastEl(i)->CacheParents_impl(src_typ);
  }
}

void TypeDef::ComputeMembBaseOff() {
  for(int i=0; i<members.size; i++) {
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
  SetInitFlag(IF_MEMBER_BASE_OFFS);
}

void TypeDef::CallInitClass() {
  if(HasInitFlag(IF_INIT_CLASS)) return; // already done

  for(int i=0; i< parents.size; i++) {
    TypeDef* par = parents[i];
    if(!par->HasInitFlag(IF_INIT_CLASS)) {
      par->CallInitClass();
    }
  }
  
  SetInitFlag(IF_INIT_CLASS);   // we're doing it one way or another

  MethodDef* md = methods.FindName("initClass");
  if(!md)
    md = methods.FindName("InitClass");
  if(!md) {
    return;
  }
  if(!(md->is_static && md->addr && (md->arg_types.size == 0) )) {
    return;
  }
  // call the init function
  md->addr();
}

bool TypeDef::IgnoreMeth(const String& nm) const {
  if(!IsClass())
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

String TypeDef::GetTemplInstName(const TypeSpace& inst_pars) const {
  String rval = name;
  int i;
  for(i=0; i<inst_pars.size; i++) {
    rval += String("_") + inst_pars.FastEl(i)->name + "_";
  }
  return rval;
}

void TypeDef::SetTemplType(TypeDef* templ_par, const TypeSpace& inst_pars) {
  if(inst_pars.size < (templ_pars.size - templ_defs.size)) {
    String defn_no(templ_pars.size-templ_defs.size);
    String inst_no(inst_pars.size);
    taMisc::Error("Template",name,"requires a minimum of",defn_no,
                  "parameters, instantiated with",
                   inst_no);
    String msg;
    msg << "Defined with parameters: ";
    templ_pars.Print(msg);
    if(templ_defs.size > 0) {
      msg << "and defaults: ";
      templ_defs.Print(msg);
    }
    msg << "\nInstantiated with parameters: ";
    inst_pars.Print(msg);
    taMisc::Error(msg);
    return;
  }

  ClearType(TEMPLATE);
  SetType(TEMPL_INST);       // update types
  
  parents.Reset();                      // bag the template's parents
  parents.LinkUnique(templ_par);        // parent is the templ_par
  templ_par->children.LinkUnique(this);
  children.Reset();                     // don't have any real children..

  // todo: need to add support for arbitrary strings here, which are not just types
  
  bool some_args_not_real = false;
  int defcnt = 0;
  for(int i=0; i<templ_pars.size; i++) {
    TypeDef* defn_tp = templ_par->templ_pars.FastEl(i); // type as defined
    TypeDef* inst_tp = NULL;
    if(i < inst_pars.size) {
      inst_tp = inst_pars.FastEl(i);  // type as instantiated
    }
    else {
      inst_tp = templ_defs.SafeEl(defcnt++);
    }

    templ_pars.ReplaceLinkIdx(i, inst_tp); // actually replace it

    // update sub-types based on defn_tp (go backwards to get most extended types 1st)
    for(int j=sub_types.size-1; j>=0; j--) {
      sub_types.FastEl(j)->ReplaceParent(defn_tp, inst_tp);
    }
    if(FindGlobalTypeName(inst_tp->name, false) == NULL) { // couldn't find it
      some_args_not_real = true;
    }
  }
  if(some_args_not_real) {      // yet to be realized
    SetType(TI_ARGS_NOTINST);
  }

  // update to use new types
  UpdateMDTypes(templ_par->templ_pars, templ_pars);
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

TypeDef* TypeDef::FindSubType(const String& sub_nm) const {
  return sub_types.FindName(sub_nm);
}

EnumDef* TypeDef::FindEnum(const String& nm) const {
  EnumDef* rval = enum_vals.FindName(nm);
  if (rval) return rval;

  for (int i = 0; i < sub_types.size; i++) {
    TypeDef* td = sub_types.FastEl(i);
    if (td->IsEnum()) {
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
    if(td->IsEnum() && (enum_tp_nm.empty() || (td->name == enum_tp_nm))) {
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
    CopyActualType(*new_tp);
    return true;
  }
  bool rval = false;
  for (int i = 0; i < parents.size; i++) {
    if (parents.FastEl(i)->ReplaceParent(old_tp, new_tp)) {
      rval = true;
      name.gsub(old_tp->name, new_tp->name); // updt name at all levels
      CopyActualType(*new_tp);
    }
  }
  return rval;
}

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
    }
    else if (sc == SC_DISPLAY) {
      if (!md->ShowMember(TypeItem::USE_SHOW_GUI_DEF, SC_EDIT, TypeItem::SHOW_CHECK_MASK))
        continue;
    }
    else {
      if(md->is_static || md->HasOption("NO_SAVE"))
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
      if(md->type->IsEnum() || md->type->IsAnyPtr() || md->type->IsSmartPtr())
        rval += "<font style=\"background-color: LightGrey\">&nbsp;&nbsp;";
      else
        rval += "<font style=\"background-color: white\">&nbsp;&nbsp;";
      if(md->type->IsString())     rval += "\"";
      rval += md->type->GetValStr(md->GetOff(base), base, md, sc, force_inline);
      if(md->type->IsString())     rval += "\"";
      rval += "&nbsp;&nbsp;</font>&nbsp;&nbsp;&nbsp;&nbsp;";
    }
    else {
      rval += md->name + "=";
      if(md->type->IsString())     rval += "\"";
      void* m_base = md->GetOff(base);
      if(m_base == base && md->type == this) {
        taMisc::Warning("self-pointer in:", md->name, "of type", md->type->name,
                        "in class of type:", name);
      }
      else {
        rval += md->type->GetValStr(m_base, base, md, sc, force_inline);
      }
      if(md->type->IsString())     rval += "\"";
      rval += ": ";
    }
  }
  if (sc != SC_DISPLAY) rval += "}";
  return rval;
}

String TypeDef::GetValStr(const void* base_, void* par, MemberDef* memb_def,
                          StrContext sc, bool force_inline) const
{
  if(sc == SC_DEFAULT)
    sc = (taMisc::is_saving) ? SC_STREAMING : SC_VALUE;
  void* base = (void*)base_; // hack to avoid having to go through entire code below and fix
  // if its void, odds are its a function..
  if(IsVoidPtr() || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun = NULL;
    if(memb_def != NULL)
      fun = taMisc::FindRegFunListAddr(*((ta_void_fun*)base),
                                       memb_def->lists, lidx);
    else
      fun = taMisc::FindRegFunAddr(*((ta_void_fun*)base), lidx);
    if(fun != NULL)
      return fun->name;
    else if(*((void**)base) == NULL)
      return String::con_NULL;
    return String((ta_intptr_t)*((void**)base));
  }
  if(IsNotPtr()) {
    if(IsAtomic()) {
      if(IsBool()) {
	bool b = *((bool*)base);
	switch (sc) {
	case SC_STREAMING: return (b) ? String::con_1 : String::con_0;
	case SC_DISPLAY: return (b) ? String("+") : String("-");
	default:
	  return String(b);
	}
      }
      else if(IsInt()) {
	// note: char is generic, and typically we won't use signed char
	if (DerivesFrom(TA_char)) {
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
	  if(sc == SC_DISPLAY && par && memb_def &&
	     memb_def->HasOption("DYNENUM_ON_enum_type")) {
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
      }
      else if(IsFloat()) {
	if(DerivesFrom(TA_float)) {
	  return taMisc::StreamFormatFloat(*static_cast<const float *>(base), sc);
	}
	else if(DerivesFrom(TA_double)) {
	  return taMisc::StreamFormatDouble(*static_cast<const double *>(base), sc);
	}
      }
      else if(IsEnum()) {
	return GetValStr_enum(base, par, memb_def, sc, force_inline);
      }
    }
    else if(IsAtomicEff()) {
      if(IsString()) {
	return *((String*)base);
      }
      else if (IsVariant()) { // in general, Variant is handled by recalling this routine on its rep's typdef

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
      else if (DerivesFrom(TA_taSmartPtr)) {
	// we just delegate to taBase* since we are binary compatible
	return TA_taBase.GetPtrType()->GetValStr(base_, par, memb_def, sc, force_inline);
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
	  }
	  else {
	    return String((intptr_t)rbase);
	  }
	}
	else {
	  return String::con_NULL;
	}
      }
      else if(DerivesFrom(TA_taAtomicInt)) {
	return String((int)(*((taAtomicInt*)base)));
      }
      else if(DerivesFrom(TA_taBasicAtomicInt)) {
	return String((int)(*((taBasicAtomicInt*)base)));
      }
#endif
    }
#ifndef NO_TA_BASE
    else if(IsTaBase()) {
      taBase* rbase = (taBase*)base;
      if(rbase) {
        return rbase->GetValStr(par, memb_def, sc, force_inline);
      }
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
#endif
    else if(IsClass() &&
            (force_inline || HasOption("INLINE") || HasOption("INLINE_DUMP")))
      {
        return GetValStr_class_inline(base_, par, memb_def, sc, force_inline);
      }
    else if(IsStruct())
      return "struct " + name;
    else if(IsUnion())
      return "union " + name;
    else if(IsClass())
      return "class " + name;
    else if(IsVoid())
      return "void";
  }
  else if(IsPointer()) {
#ifndef NO_TA_BASE
    if(IsTaBase()) {
      return taBase::GetValStr_ptr(this, base_, par, memb_def, sc, force_inline);
    }
    else
#endif
    if (DerivesFrom(TA_TypeDef)) {
      TypeDef* td = *((TypeDef**)base);
      if (td) {
        return td->GetPathName();
      }
      else {
        return String::con_NULL;
      }
    }
    else if (DerivesFrom(TA_MemberDef)) {
      MemberDef* md = *((MemberDef**)base);
      if (md) {
        return md->GetPathName();
      }
      else {
        return String::con_NULL;
      }
    }
    else if (DerivesFrom(TA_MethodDef)) {
      MethodDef* md = *((MethodDef**)base);
      if (md) {
        return md->GetPathName();
      }
      else {
        return String::con_NULL;
      }
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

  if(IsVoidPtr() || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun = taMisc::FindRegFunName(val, lidx);
    if((fun != NULL) && (fun->addr != NULL))
      *((ta_void_fun*)base) = fun->addr;
    return;
  }
  if(IsNotPtr()) {
    if(IsAtomic()) {
      if(IsBool()) {
        *((bool*)base) = val.toBool();
      }
      else if(IsInt()) {
        if(DerivesFrom(TA_int))
          *((int*)base) = val.toInt();
        // note: char is treated as an ansi character
        else if (DerivesFrom(TA_char)) //TODO: char conversion heuristics
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
      }
      else if(IsFloat()) {
        if(DerivesFrom(TA_float))
          *((float*)base) = val.toFloat();
        else if(DerivesFrom(TA_double))
          *((double*)base) = val.toDouble();
      }
      else if(IsEnum()) {
        SetValStr_enum(val, base, par, memb_def, sc, force_inline);
      }
    }
    else if(IsAtomicEff()) {
      if(IsString()) {
        *((String*)base) = val;
      }
      else if (IsVariant()) {
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
#ifndef NO_TA_BASE
      else if (DerivesFrom(TA_taSmartPtr)) {
        // we just delegate, since we are binary compat
        TA_taBase.GetPtrType()->SetValStr(val, base, par, memb_def, sc, force_inline);
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
              if (md->type->IsPointer()) {
                bs = *((taBase**)bs);
                if(bs == NULL) {
                  taMisc::Warning("*** Null object at end of path in SetValStr:",val);
                  return;
                }
              }
              else if(md->type->IsPtrPtr()) {
                taMisc::Warning("*** ptr count greater than 1 in path:", val);
                return;
              }
            }
          }
        }
        taSmartRef& ref = *((taSmartRef*)base);
        ref = bs;
      }
      else if(DerivesFrom(TA_taAtomicInt)) {
        (*((taAtomicInt*)base)) = val.toInt();
      }
      // else if(DerivesFrom(TA_taBasicAtomicInt)) {
      //   (*((taBasicAtomicInt*)base)) = val.toInt();
      // }
#endif
    }
#ifndef NO_TA_BASE
    else if(IsTaBase()) {
      taBase* rbase = (taBase*)base;
      if(rbase) {
        rbase->SetValStr(val, par, memb_def, sc, force_inline);
      }
    }
    else if(DerivesFrom(TA_taArray_impl)) {
      taArray_impl* gp = (taArray_impl*)base;
      if(gp != NULL)
        gp->InitFromString(val);
    }
#endif
    else if(IsClass() &&
            (force_inline || HasOption("INLINE") || HasOption("INLINE_DUMP"))) {
      SetValStr_class_inline(val, base, par, memb_def, sc, force_inline);
    }
  }
  else if(IsPointer()) {
    bool is_null = ((val == "NULL") || (val == "(NULL)"));
#ifndef NO_TA_BASE
    if (IsTaBase()) {
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
      if (!md->ShowMember(TypeItem::USE_SHOW_GUI_DEF, SC_EDIT, TypeItem::SHOW_CHECK_MASK))
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
  if(IsNotPtr()) {
#ifndef NO_TA_BASE
    if(IsTaBase()) {
      taBase* rbase = (taBase*)base;
      if(rbase)
        return rbase->ReplaceValStr(srch, repl, mbr_filt, par, par_typ, memb_def, sc);
    }
    else
#endif
      if(IsActualClassNoEff()) {
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
  if(par_typ && par && par_typ->IsTaBase()) {
    if(memb_def) {
      taMisc::Info("Replaced string value in member:", memb_def->name, "of type:", name,
                   "in", par_typ->name,
                   "object:",((taBase*)par)->GetPathNames(),repl_info);
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

const Variant TypeDef::GetValVar(const void* base_, const MemberDef* memb_def) const
{
  void* base = (void*)base_; // hack to avoid having to go through entire code below and fix
  // if its void, odds are its a function..
  if (IsVoidPtr() || ((memb_def) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun = NULL;
    if(memb_def != NULL)
      fun = taMisc::FindRegFunListAddr(*((ta_void_fun*)base),
                                       memb_def->lists, lidx);
    else
      fun = taMisc::FindRegFunAddr(*((ta_void_fun*)base), lidx);
    if (fun != NULL)
      return fun->name;
    else if(*((void**)base) == NULL)
      return Variant((void*)NULL);//String::con_NULL;
    else
      return String((ta_intptr_t)*((void**)base)); //TODO: is this the best??
  }
  if(IsNotPtr()) {
    if(IsAtomic()) {
      if(IsBool()) {
        bool b = *((bool*)base);
        return b; //T_Bool
      }
      else if(IsInt()) {
        // note: char is generic char, and typically we won't use signed char
        if (DerivesFrom(TA_char)) {
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
      }
      else if(IsFloat()) {
        if(DerivesFrom(TA_float)) {
          return *((float*)base); // T_Double
        }
        else if(DerivesFrom(TA_double)) {
          return *((double*)base); // T_Double
        }
      }
      else if(IsEnum()) {
        int en_val = *((int*)base);
        String rval = GetEnumString("", en_val);
        if(rval.empty()) rval = (String)en_val;
        return rval;  // T_String
      }
    }
    else if(IsAtomicEff()) {
      if(IsString()) {
        return *((String*)base); // T_String
      }
      else if(IsVariant()) {
        return *((Variant*)base);
      }
#ifndef NO_TA_BASE
      else if (DerivesFrom(TA_taSmartPtr)) {
        taBase* rbase = (taBase*)base;
        return rbase; // T_Base
      }
      else if (DerivesFrom(TA_taSmartRef)) {
        taBase* rbase = *((taSmartRef*)base);
        return rbase; // T_Base
      }
      else if(DerivesFrom(TA_taAtomicInt)) {
	return (int)(*((taAtomicInt*)base));
      }
      else if(DerivesFrom(TA_taBasicAtomicInt)) {
	return (int)(*((taBasicAtomicInt*)base));
      }
#endif
    }
#ifndef NO_TA_BASE
    //WARNING: there could be ref-count issues if base has not been ref'ed at least once!
    else if(IsTaBase()) {
      taBase* rbase = (taBase*)base;
      //WARNING: there could be ref-count issues if base has not been ref'ed at least once!
      return rbase; // T_Base
    }
#endif
    // NOTE: other value types are not really supported, just fall through to return invalid
  }
  else if (IsPointer()) {
    if (DerivesFrom(TA_char)) {
      return *((char**)base); // T_String
    }
#ifndef NO_TA_BASE
    else if (IsTaBase()) {
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
      (IsAnyPtr()) || !IsActualClassNoEff()) {
    return ValIsEmpty(base, memb_def); // note: show not used for single guy
  }
  else { // instance of a class, so must recursively determine
    // just find all eligible guys, and return true if none fail
    for (int i = 0; i < members.size; ++i) {
      MemberDef* md = members.FastEl(i);
      if (!md || !md->ShowMember(TypeItem::USE_SHOW_GUI_DEF, TypeItem::SC_ANY,
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
  if (IsVoidPtr() || ((memb_def) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun = NULL;
    if(memb_def != NULL)
      fun = taMisc::FindRegFunListAddr(*((ta_void_fun*)base),
                                       memb_def->lists, lidx);
    else
      fun = taMisc::FindRegFunAddr(*((ta_void_fun*)base), lidx);
    if (fun)
      return false;
    else
      return !(*((void**)base));
  }
  if (IsNotPtr()) {
    if(IsAtomic()) {
      if(IsBool()) {
        bool b = *((bool*)base);
        return !b; //T_Bool
      }
      else if(IsInt()) {
        // note: char is generic char, and typically we won't use signed char
        if (DerivesFrom(TA_char)) {
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
      }
      else if(IsFloat()) {
        if(DerivesFrom(TA_float)) {
          return (*((float*)base) == 0); // T_Double
        }
        else if(DerivesFrom(TA_double)) {
          return (*((double*)base) == 0); // T_Double
        }
      }
      else if(IsEnum()) {
        return (*((int*)base) == 0); // T_Int
      }
    }
    else if(IsAtomicEff()) {
      if(IsString())
        return ((*((String*)base)).empty()); // T_String
      else if(IsVariant()) {
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
      else if(DerivesFrom(TA_taAtomicInt)) {
	return (*((taAtomicInt*)base)) == 0;
      }
      else if(DerivesFrom(TA_taBasicAtomicInt)) {
	return (*((taBasicAtomicInt*)base)) == 0;
      }
#endif
    }
    // must be some other value or a class -- default to saying no to empty
    else return false;
    // NOTE: other value types are not really supported, just fall through to return    invalid
  }
  else if(IsPointer()) {
    return !(*((void**)base)); // only empty if NULL
  }
  return false;
}

void TypeDef::SetValVar(const Variant& val, void* base, void* par,
                        MemberDef* memb_def)
{
  if(IsVoid() || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun = taMisc::FindRegFunName(val.toString(), lidx);
    if((fun != NULL) && (fun->addr != NULL))
      *((ta_void_fun*)base) = fun->addr;
    return;
  }
  if (IsNotPtr()) {
    if(IsAtomic()) {
      if(IsBool()) {
        *((bool*)base) = val.toBool(); return;
      }
      else if(IsInt()) {
        if(DerivesFrom(TA_int)) {
          *((int*)base) = val.toInt(); return;
        }
        else if (DerivesFrom(TA_char)) { // TODO: char conversion heuristics
          *((char*)base) = val.toChar(); return;
        }
        // signed char is treated like a number
        else if (DerivesFrom(TA_signed_char)) {
          *((signed char*)base) = (signed char)val.toInt(); return;
        }
        // unsigned char is "byte" in ta/pdp and treated like a number
        else if (DerivesFrom(TA_unsigned_char)) {
          *((unsigned char*)base) = val.toByte(); return;
        }
        else if(DerivesFrom(TA_short)) {
          *((short*)base) = (short)val.toInt(); return;
        }
        else if(DerivesFrom(TA_unsigned_short)) {
          *((unsigned short*)base) = (unsigned short)val.toUInt(); return;
        }
        else if(DerivesFrom(TA_unsigned_int)) {
          *((uint*)base) = val.toUInt(); return;
        }
        else if(DerivesFrom(TA_int64_t)) {
          *((int64_t*)base) = val.toInt64(); return;
        }
        else if(DerivesFrom(TA_uint64_t)) {
          *((uint64_t*)base) = val.toUInt64(); return;
        }
      }
      else if(IsFloat()) {
        if(DerivesFrom(TA_float)) {
          *((float*)base) = val.toFloat(); return;
        }
        else if(DerivesFrom(TA_double)) {
          *((double*)base) = val.toDouble(); return;
        }
      }
      else if(IsEnum()) {
        // if it is a number, assume direct value, otherwise it is a string
        if (val.isNumeric()) {
          *((int*)base) = val.toInt();
          return;
        }
        SetValStr_enum(val.toString(), base, par, memb_def);
      }
    }
    else if(IsAtomicEff()) {
      if(IsString()) {
        *((String*)base) = val.toString(); return;
      }
      else if (IsVariant()) {
        *((Variant*)base) = val; return;
      }
#ifndef NO_TA_BASE
      else if (DerivesFrom(TA_taSmartPtr)) {
        // we just delegate, since we are binary compat
        TA_taBase.GetPtrType()->SetValVar(val, base, par, memb_def);
        return;
      }
      else if(DerivesFrom(TA_taSmartRef)) {
        //VERY DANGEROUS!!!! No type checking!!!!
        taSmartRef& ref = *((taSmartRef*)base);
        ref = val.toBase();
        return;
      }
      else if(DerivesFrom(TA_taAtomicInt)) {
	(*((taAtomicInt*)base)) = val.toInt(); return;
      }
      // else if(DerivesFrom(TA_taBasicAtomicInt)) {
      //   (*((taBasicAtomicInt*)base)) = val.toInt(); return;
      // }
#endif
    }
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
#endif
  }
  else if(IsPointer()) {
#ifndef NO_TA_BASE
    if (IsTaBase()) {
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
    if (md->type->IsAnyPtr()) {
      if(taMisc::dmem_proc == 0) {
        taMisc::Error("WARNING: DMEM_SHARE_SET Specified for a pointer.",
                      "Pointers can not be shared.");
        continue;
      }
    }
    if (md->type->IsActualClassNoEff()) {
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
    else if (md->type->IsEnum()) {
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
  if(IsVoidPtr() || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun = NULL;
    if(memb_def != NULL)
      fun = taMisc::FindRegFunListAddr(*((ta_void_fun*)src_base),
                                       memb_def->lists, lidx);
    else
      fun = taMisc::FindRegFunAddr(*((ta_void_fun*)src_base), lidx);
    if((fun != NULL) || (memb_def != NULL))
      *((ta_void_fun*)trg_base) = *((ta_void_fun*)src_base); // must be a funptr
    else
      *((void**)trg_base) = *((void**)src_base); // otherwise just a voidptr
    return;
  }
  if(IsNotPtr()) {
    if(IsAtomic()) {
      memcpy(trg_base, src_base, size);
    }
    else if(IsAtomicEff()) {
      if(IsVariant()) {
	*((Variant*)trg_base) = *((Variant*)src_base);
      }
      else if (IsString()) {
	*((String*)trg_base) = *((String*)src_base);
      }
#ifndef NO_TA_BASE
      else if(DerivesFrom(TA_taSmartRef)) {
	*((taSmartRef*)trg_base) = *((taSmartRef*)src_base);
      }
      else if(DerivesFrom(TA_taSmartPtr)) {
	*((taSmartPtr*)trg_base) = *((taSmartPtr*)src_base);
      }
      else if(DerivesFrom(TA_taAtomicInt)) {
	*((taAtomicInt*)trg_base) = *((taAtomicInt*)src_base);
      }
      else if(DerivesFrom(TA_taBasicAtomicInt)) {
	*((taBasicAtomicInt*)trg_base) = *((taBasicAtomicInt*)src_base);
      }
#endif
    }
#ifndef NO_TA_BASE
    else if(IsTaBase()) {
      taBase* rbase = (taBase*)trg_base;
      taBase* sbase = (taBase*)src_base;
      rbase->Copy(sbase);
    }
#endif
    else if(IsClass()) {
      members.CopyFromSameType(trg_base, src_base);
    }
  }
  else if(IsAnyPtr()) {
#ifndef NO_TA_BASE
    if((IsPointer()) && IsTaBase()) {
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
  if(IsActualClassNoEff()) {
#ifndef NO_TA_BASE
    if(IsTaBase()) {
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
  else {
    CopyFromSameType(trg_base, src_base, memb_def);
  }
}

void TypeDef::MemberCopyFrom(int memb_no, void* trg_base, void* src_base) {
  if(memb_no < members.size) {
    members[memb_no]->CopyFromSameType(trg_base, src_base);
  }
}

bool TypeDef::CompareSameType(Member_List& mds, TypeSpace& base_types,
                              voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                              void* trg_base, void* src_base,
                              int show_forbidden, int show_allowed, bool no_ptrs,
                              bool test_only) {
  if(IsActualClassNoEff()) {
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
  if(IsClass())
    strm << "class ";
  else if(IsEnum())
    strm << "enum ";
  else if(IsStruct())
    strm << "struct ";
  else if(IsUnion())
    strm << "union ";

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
  if(IsClass() || IsEnum())
    strm << " {";
  else
    strm << ";";
  if(!desc.empty()) {
    if(IsClass()) {
      strm << "\n";
      taMisc::IndentString(strm, indent) << "//" << desc ;
    }
    else
      strm << "\t//" << desc ;
  }

  PrintType_OptsLists(strm);
  strm << "\n";
  if(IsClass()) {
    if(sub_types.size > 0) {
      strm << "\n";
      taMisc::IndentString(strm, indent+1) << "// sub-types\n";
      int i;
      for(i=0; i<sub_types.size; i++) {
        if(sub_types.FastEl(i)->IsActual())
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
  else if(IsEnum()) {
    enum_vals.PrintType(strm, indent+1);
    taMisc::IndentString(strm, indent) << "}\n";
  }
  return strm;
}

String& TypeDef::Print(String& strm, void* base, int indent) const {
  taMisc::IndentString(strm, indent);
#ifndef NO_TA_BASE
  if(IsTaBase()) {
    taBase* rbase;
    if(!IsAnyPtr())
      rbase = (taBase*)base;
    else if(IsPointer())
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
  if(IsTaBase()) {
    taBase* rbase = (taBase*)base;
    strm << " " << rbase->GetName()
         << " (refn=" << taBase::GetRefn(rbase) << ")";
  }
#endif

  if(IsActualClassNoEff()) {
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
  TypeDef* npt = GetActualType();
  if(npt) {
    if(npt->IsClass()) {
      if(gendoc)
        rval.cat("<a href=\"").cat(npt->name).cat(".html\">").cat(Get_C_Name()).cat("</a>");
      else
        rval.cat("<a href=\"ta:.Type.").cat(npt->name).cat("\">").cat(Get_C_Name()).cat("</a>");
    }
    else if(npt->IsEnum()) {
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
  if(IsClass()) {
    // todo: not yet supported by maketa..
  }
  else if(IsEnum()) {
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
  if(!IsActualClassNoEff()) return GetHTMLLink(gendoc);

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

  String incnm = "&lt;" + name + "&gt";
  String srconly = taMisc::GetFileFmPath(source_file);
  if(!srconly.startsWith(name))
    incnm = "\"" + srconly + "\"";

  String srcpath = source_file;
  if(!taMisc::in_dev_exe) {
    srcpath = taMisc::app_dir.before("/share/") + "/include/" +
      taMisc::default_app_install_folder_name + "/" + srconly;
    // hack for grey
    if(srcpath.contains("/mnt/raid/grey/"))
      srcpath.gsub("/mnt/raid/grey/", "/usr/");
  }

  rval.cat("<pre> #include ").cat(incnm).cat("</pre>\n");
  rval.cat("<p>defined at: <a href=\"file://").cat(srcpath).cat("\">").cat(srcpath).cat("</a> ");
  rval.cat(":").cat((String)source_start).cat("-").cat((String)source_end).cat("</p>\n");

  if(par_cache.size > 0) {
    int inhi = 0;
    rval.cat("<p>Inherits From: ");
    for(int i=0;i<par_cache.size; i++) {
      TypeDef* par = par_cache[i];
      if(par->IsTemplInst()) continue; // none of those
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
      if(par->IsTemplInst()) continue; // none of those
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
      if(!st->IsEnum()) continue;
#ifndef NO_TA_BASE
      if((this != &TA_taBase) && (st->GetOwnerType() == &TA_taBase)) continue;
#endif
      if(st->GetOwnerType()->IsTemplInst()) continue;
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
      if(!st->IsEnum()) continue;
#ifndef NO_TA_BASE
      if((this != &TA_taBase) && (st->GetOwnerType() == &TA_taBase)) continue;
#endif
      if(st->GetOwnerType()->IsTemplInst()) continue;
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
#ifndef NO_TA_BASE
      if((this != &TA_taBase) && (md->GetOwnerType() == &TA_taBase)) continue;
#endif
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
#ifndef NO_TA_BASE
      if((this != &TA_taBase) && (md->GetOwnerType() == &TA_taBase)) continue;
#endif
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
#ifndef NO_TA_BASE
      if((this != &TA_taBase) && (md->GetOwnerType() == &TA_taBase)) continue;
#endif
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
#ifndef NO_TA_BASE
      if((this != &TA_taBase) && (md->GetOwnerType() == &TA_taBase)) continue;
#endif
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


#ifndef NO_TA_BASE

#include <taObjDiffRec>
#include <taObjDiff_List>

void TypeDef::GetObjDiffVal(taObjDiff_List& odl, int nest_lev, const void* base,
                            MemberDef* memb_def, const void* par,
                            TypeDef* par_typ, taObjDiffRec* par_od) const {
  if(IsTaBase()) {
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
//   if(IsAnyPtr()) {
//     // this will be set wrong here..  need some complex logic probaly to fix it..
//   }

  // then check for classes
  if(IsActualClassNoEff()) {
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
