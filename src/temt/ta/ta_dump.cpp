// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_dump.cpp

#include "ta_dump.h"
#include "ta_variant.h"
#include "ta_project.h"
#include "ta_TA_type.h"

// for process pending events
#ifdef TA_GUI
# include "ta_qt.h"
# include "css_qt.h"
# include "css_console.h"
#endif

#include <QTimer>

/* Version 4.x Note on Error handling

  Version 3.x had a mechanism whereby any taMisc::Error call with a non-
  alphanum first character in the error msg would only print to cerr
  not raise an error dialog. In v4.x we have added a taMisc::Warning
  call for this purpose. So any call in the dump code that previously
  did not raise a gui window has been converted to a Warning.

*/
taBase_PtrList 	dumpMisc::update_after;
taBase_PtrList  dumpMisc::post_update_after;
DumpPathSubList	dumpMisc::path_subs;
DumpPathTokenList dumpMisc::path_tokens;
VPUList 	dumpMisc::vpus;


void dumpMisc::PostUpdateAfter() {
  taMisc::is_post_loading++;
  for (int i=0; i < dumpMisc::post_update_after.size; i++) {
    TAPtr tmp = dumpMisc::post_update_after.FastEl(i);
    tmp->Dump_Load_post();
  }
  dumpMisc::post_update_after.Reset();
  taMisc::is_post_loading--;
}  


//////////////////////////
//	VPUnref		//
//////////////////////////


VPUnref::VPUnref(void* base_, TAPtr par, const String& p, MemberDef* md) {
  base = base_; parent = par; path = p; memb_def = md;
  name = String((intptr_t)base);
}

TAPtr VPUnref::Resolve() {
  MemberDef* md;
  TAPtr bs = tabMisc::root->FindFromPath(path, md);
  if(!bs)
    return NULL;
  if(md) {
    if(md->type->ptr == 1) {
      bs = *((TAPtr*)bs);
      if(!bs)
	return NULL;
    }
    else if(md->type->ptr != 0) {
      taMisc::Warning("*** ptr count != 0 in path:", path);
      return NULL;
    }
  }

  if (memb_def && memb_def->type->InheritsFrom(&TA_taSmartRef)) {
    taSmartRef& ref = *((taSmartRef*)base);
    ref = bs;
  } else {// assume it is taBase_ptr or (binary-compat) taBasePtr
    if((memb_def != NULL) && memb_def->HasOption("OWN_POINTER")) {
      if(parent == NULL)
        taMisc::Warning("*** NULL parent for owned pointer:",path);
      else
        taBase::OwnPointer((TAPtr*)base, bs, parent);
    } else 
      taBase::SetPointer((TAPtr*)base, bs);
  }

  if(taMisc::verbose_load >= taMisc::MESSAGES)
    taMisc::Warning("<== Resolved Reference:",path);
  if(parent != NULL)
    parent->UpdateAfterEdit();
  return bs;
}

void VPUList::Resolve() {
  if(size <= 0)
    return;
  int i=0;
  do {
    if(FastEl(i)->Resolve() != NULL)
      RemoveIdx(i);		// take off the list if resolved!
    else {
      VPUnref* vp = (VPUnref*)FastEl(i);
      String par_path;
      if(vp->parent != NULL)
	par_path = vp->parent->GetPath();
      taMisc::Warning("*** Could not resolve following path:",vp->path,
		    "in object:",par_path);
      i++;
    }
  } while(i < size);
}

void VPUList::Add(void* b, TAPtr par, const String& p, MemberDef* md) {
  AddUniqNameOld(new VPUnref(b,par,p,md));
  if(taMisc::verbose_load >= taMisc::MESSAGES)
    taMisc::Warning("==> Unresolved Reference:",p);
}


//////////////////////////
//	PathSub		//
//////////////////////////


DumpPathSub::DumpPathSub(TypeDef* td, TAPtr par, const String& o, const String& n) {
  type = td; parent = par;
  old_path = o; new_path = n;
}

void DumpPathSubList::Add(TypeDef* td, TAPtr par, String& o, String& n) {
  int o_i = o.length()-1;
  int n_i = n.length()-1;
  int o_last_sep = -1;		// position of last separator character
  int n_last_sep = -1;
  // remove any trailing parts of name that are common to both..
  // often only the leading part of name is new, so no need to
  // replicate the same fix over and over..
  while ((o_i >= 0) && (n_i >= 0) && (o[o_i] == n[n_i])) {
    if(o[o_i] == '.') {
      o_last_sep = o_i;
      n_last_sep = n_i;
    }
    o_i--; n_i--;
  }
  String* op = (String*)&o;
  String* np = (String*)&n;
  String ot, nt;
  if(o_last_sep > 0) {
    op = &ot;
    np = &nt;
    ot = o.before(o_last_sep);
    nt = n.before(n_last_sep);
  }
  if(taMisc::verbose_load >= taMisc::MESSAGES) {
    String ppath = par->GetPath();
    taMisc::Warning("---> New Path Fix, old:",*op,"new:",*np,"in:",ppath);
  }
  DumpPathSub* nwsb = new DumpPathSub(td, par, *op, *np);
  Add(nwsb);
  if(par != tabMisc::root) {	// if local path, then add a global path fix too
    String ppath = par->GetPath();
    unFixPath(td, tabMisc::root, ppath); // un fix the parent if necessary
    String long_o = ppath + *op;
    String long_n = ppath + *np;
    nwsb = new DumpPathSub(td, tabMisc::root, long_o, long_n);
    Add(nwsb);
    if(taMisc::verbose_load >= taMisc::MESSAGES) {
      taMisc::Warning("---> New Global Path Fix, old:",long_o,"new:",long_n);
    }
  }
}

void DumpPathSubList::FixPath(TypeDef*, TAPtr par, String& path) {
  // search backwards to get latest fix
  int i;
  for(i=size-1; i>=0; i--) {
    DumpPathSub* dp = FastEl(i);
    if(dp->parent != par) // scope by parent
      continue;
    if(path.contains(dp->old_path)) {
      path.gsub(dp->old_path, dp->new_path);
      return;			// done!
    }
  }
}

void DumpPathSubList::unFixPath(TypeDef*, TAPtr par, String& path) {
  // search backwards to get latest fix
  int i;
  for(i=size-1; i>=0; i--) {
    DumpPathSub* dp = FastEl(i);
    if(dp->parent != par) // scope by parent
      continue;
    if(path.contains(dp->new_path)) {
      path.gsub(dp->new_path, dp->old_path);
      return;			// done!
    }
  }
}


//////////////////////////
//	Path Token	//
//////////////////////////

DumpPathToken::DumpPathToken(TAPtr obj, const String& pat, const String& tok_id) {
  object = obj;
  path = pat;
  token_id = tok_id;
}

void DumpPathTokenList::Add(TAPtr obj, const String& pat) {
  String tok_id = "$";
  tok_id += String(size) + "$";
  DumpPathToken* tok = new DumpPathToken(obj, pat, tok_id);
  Add(tok);
}

int DumpPathTokenList::FindObj(TAPtr obj) {
  int i;
  for(i=0; i<size; i++) {
    if(FastEl(i)->object == obj)
      return i;
  }
  return -1;
}

String DumpPathTokenList::GetPath(TAPtr obj) {
  int idx = FindObj(obj);
  if(idx >= 0)
    return FastEl(idx)->token_id;

  String path = obj->GetPath();
  Add(obj, path);
  DumpPathToken* tok = FastEl(size-1);
  path += tok->token_id;	// this marks the token right then and there..
  return path;
}

void DumpPathTokenList::NewLoadToken(String& pat, String& tok_id) {
  int tok_num = (int)tok_id;
  Add(NULL, pat);
  DumpPathToken* tok = FastEl(size-1);
  if(tok_num != size-1) {
    taMisc::Warning("*** Path Tokens out of order, on list:", tok->token_id, "in file:",
		   tok_id);
    tok->object = NULL;
    tok->path = "";
    String null_pat;
    while(size <= tok_num) Add(NULL, null_pat);	// catch up if possible
    tok = FastEl(tok_num);
    tok->path = pat;
  }
  if(taMisc::verbose_load >= taMisc::MESSAGES) {
    taMisc::Warning("---> New Path Token:",tok_id,"path:",pat);
  }
}

TAPtr DumpPathTokenList::FindFromPath(String& pat, TypeDef* td, void* base,
				      void* par, MemberDef* memb_def)
{
  if(pat[0] == '$') {		// its a token path, not a real one..
    String num = pat(1,pat.index('$',-1)-1); // get the number
    DumpPathToken* tok = SafeEl((int)num);
    if(tok == NULL) {
      taMisc::Warning("*** Path Token Not Created Yet:", pat);
      return NULL;
    }
    if((tok->object == NULL) && (base != NULL)) {
      dumpMisc::vpus.Add(base, (TAPtr)par, pat, memb_def);
      return NULL;
    }
    return tok->object;		// this is what we're looking for
  }
  DumpPathToken* tok = NULL;
  if(pat.lastchar() == '$') {	// path contains definition of new token
    String token_id = pat.after('$');
    token_id = token_id.before('$');
    pat = pat.before('$');
    NewLoadToken(pat, token_id); // make the token
    tok = FastEl(size-1);
  }

  dumpMisc::path_subs.FixPath(td, tabMisc::root, pat);
  MemberDef* md = NULL;
  TAPtr rval = tabMisc::root->FindFromPath(pat, md);
  if(!rval) {
    if(base != NULL)
      dumpMisc::vpus.Add(base, (TAPtr)par, pat, memb_def);
    return NULL;
  }

  if(md) {
    if(md->type->ptr == 1) {
      rval = *((TAPtr*)rval);
      if(!rval) {
	if(base != NULL)
	  dumpMisc::vpus.Add(base, (TAPtr)par, pat, memb_def);
	return NULL;
      }
    }
    else if(md->type->ptr != 0) {
      taMisc::Warning("*** ptr count greater than 0 in path:", pat);
      return NULL;
    }
  }
  if(tok != NULL)
    tok->object = rval;
  return rval;
}



//////////////////////////////////////////////////////////
// 			dump save			//
//////////////////////////////////////////////////////////

int MemberSpace::Dump_Save(ostream& strm, void* base, void* par, int indent) {
  int rval = true;
  int i;
  for(i=0; i<size; ++i) {
    if (!FastEl(i)->Dump_Save(strm, base, par, indent))
      rval = false;
  }
  return rval;
}

int MemberSpace::Dump_SaveR(ostream& strm, void* base, void* par, int indent) {
  int rval = false;
  int i;
  for(i=0; i<size; ++i) {
    if(FastEl(i)->Dump_SaveR(strm, base, par, indent))
      rval = true;
  }
  return rval;
}

int MemberSpace::Dump_Save_PathR(ostream& strm, void* base, void* par, int indent) {
  int rval = false;
  int i;
  for(i=0; i<size; ++i) {
    if (FastEl(i)->Dump_Save_PathR(strm, base, par, indent))
      rval = true;
  }
  return rval;
}

bool MemberDef::DumpMember(void* par) {
  if ((is_static && !HasOption("SAVE")) || HasOption("NO_SAVE"))
    return false;
  // if taBase, query it for member save
  TypeDef* par_typ = GetOwnerType();
  if (par && par_typ && par_typ->InheritsFrom(&TA_taBase)) {
    taBase* par_ = (taBase*)par;
    taBase::DumpQueryResult dqr = par_->Dump_QuerySaveMember(this); 
    if (dqr == taBase::DQR_NO_SAVE) return false;
    else if (dqr == taBase::DQR_SAVE) return true;
    // else default, so fall through
  } 
  
  // first, check explicit rules
  if (HasOption("SAVE"))
    return true;
  else if (HasOption("NO_SAVE"))
    return false;
  else if (HasOption("NO_SAVE_EMPTY")) {
    void* new_base = GetOff(par);
    return !(type->ValIsEmpty(new_base, this));
  }
  else if (HasOption("NO_SAVE_DEF")) {
    // note: if no DEF then this will behave like NO_SAVE_EMPTY
    void* new_base = GetOff(par);
    return !(ValIsDefault(par, taMisc::SHOW_CHECK_MASK));
  }
  // embedded types (simple or objects) get saved by default
  else if ((type->ptr == 0))
    return true;
  // ok, so it is a ptr -- some types get saved by default
  else if (type->DerivesFrom(TA_taBase) ||
     type->DerivesFrom(TA_TypeDef) ||
     type->DerivesFrom(TA_MemberDef) ||
     type->DerivesFrom(TA_MethodDef))
    return true;
  else
    // if its a pointer object you own
    return false;
}


int MemberDef::Dump_Save(ostream& strm, void* base, void* par, int indent) {
  //note: confusing, but base is actually the parent
  if(!DumpMember(base))
    return false;
  void* new_base = GetOff(base);
  TypeDef* eff_type = type;
  
  // embedded classes can never be Variant, and are completely handled in this block
  if (type->InheritsNonAtomicClass()) {
    taMisc::indent(strm, indent, 1) << name;
    if (type->InheritsFrom(TA_taBase)) {
      TAPtr rbase = (TAPtr)new_base;
      rbase->Dump_Save_inline(strm, (TAPtr)base, indent);
    }
    else
      type->Dump_Save_inline(strm, new_base, base, indent);
    return true;
  }
  
  // otherwise, we could have a Variant, in which case we will indirect
  else if (type->InheritsFrom(TA_Variant)) {
    Variant& var = *((Variant*)(new_base));
    var.GetRepInfo(eff_type, new_base);
    // we need to output a spurious name and type info for taBase types
    // so they are guaranteed to have the type info before encountering taBase ops in the load
    if (var.isBaseType()) {
      taMisc::indent(strm, indent, 1) << name;
      var.Dump_Save_Type(strm);
      strm << ";\n";
    } 
  }
  
  if ((eff_type->ptr == 1) && (eff_type->DerivesFrom(TA_taBase))) {
    TAPtr tap = *((TAPtr*)new_base);
    if((tap != NULL) &&	(tap->GetOwner() == base)) { // wholly owned subsidiary
      return tap->Dump_Save_impl(strm, (TAPtr)base, indent);
    }
    if((tap != NULL) && (tap->GetOwner() == NULL)) { // no owner, fake path name
      strm << tap->GetTypeDef()->name << " @*(." << name << ")";
      tap->Dump_Save_Value(strm, (TAPtr)base, indent);
//NOTE: HACK ALERT...      
// we only need the extra "};" when not saving a INLINE object 
      if (!(tap->HasOption("INLINE_DUMP") && !tap->HasUserDataList()))
        taMisc::indent(strm, indent, 1) << "};\n";
      return true;
    }
  }

  taMisc::indent(strm, indent, 1) << name;

  bool save_value = true;
  if (type->InheritsFrom(TA_Variant)) {
    Variant& var = *((Variant*)(new_base));
    var.Dump_Save_Type(strm);
    // we don't try to save null atomics
    if (var.isAtomic() && var.isNull()) {
      save_value = false;
    }
  }
  if (save_value) {
    String str = eff_type->GetValStr(new_base, base, this);
    strm << "=";
    if (eff_type->InheritsFrom(TA_taString)) {
      // note: it won't stream an empty string
      taMisc::write_quoted_string(strm, str);
    } else {
      strm << str;
    }
  }
  strm << ";\n";
  return true;
}


int MemberDef::Dump_SaveR(ostream& strm, void* base, void* par, int indent) {
  //note: confusing, but base is actually the parent
  if(!DumpMember(base))
    return false;

  int rval = false;
  void* new_base = GetOff(base);

  if (type->InheritsNonAtomicClass()) {
    if(type->InheritsFrom(TA_taBase)) {
      TAPtr rbase = (TAPtr)new_base;
      rval = rbase->Dump_SaveR(strm, (TAPtr)base, indent);
    }
    else
      rval = type->Dump_SaveR(strm, new_base, base, indent);
  }
  else {
    TypeDef* eff_type = type;
    // variant taBase types will get indirected
    if (type->InheritsFrom(TA_Variant)) {
      Variant& var = *((Variant*)(new_base));
      if (!var.isBaseType()) {
        var.GetRepInfo(eff_type, new_base);
      }
    }
  
    if ((eff_type->ptr == 1) && (eff_type->DerivesFrom(TA_taBase))) {
      TAPtr tap = *((taBase **)(new_base));
      if ((tap != NULL) && (tap->GetOwner() == base)) { // wholly owned subsidiary
        tap->Dump_Save_impl(strm, (TAPtr) base, indent);
        rval = tap->Dump_SaveR(strm, (TAPtr)base, indent);
      }
    }
  }
  return rval;
}

int MemberDef::Dump_Save_PathR(ostream& strm, void* base, void* par, int indent) {
  //note: confusing, but base is actually the parent
  if(!DumpMember(base))
    return false;
  if(HasOption("NO_SAVE_PATH_R") || HasOption("LINK_GROUP")) // don't save these ones..
    return false;

  int rval = false;
  void* new_base = GetOff(base);
  
  
  if (type->InheritsNonAtomicClass()) {
    if(type->InheritsFrom(TA_taBase)) {
      TAPtr rbase = (TAPtr)new_base;
      rval = rbase->Dump_Save_PathR(strm, (TAPtr)base, indent);
    }
    else
      rval = type->Dump_Save_PathR(strm, new_base, (TAPtr)base, indent);
  }
  else {
    // if it is a Variant, we are only interested in non-null taBase ptrs
    // in which case, we'll output just the var type info for the variant,
    // then proceed in this routine with the new type
    TypeDef* eff_type = type; 
    if (type->InheritsFrom(TA_Variant)) {
      Variant& var = *((Variant*)(new_base));
      TypeDef* var_typ; void* var_data;
      var.GetRepInfo(var_typ, var_data);
      // we dump type info if taBase* and not null
      if (var.isBaseType() && !var.isNull()) {
        eff_type = var_typ;
        new_base = var_data;
        taMisc::indent(strm, indent, 1) << name;
        var.Dump_Save_Type(strm);
        strm << ";\n";
      }
    }
    
    if ((eff_type->ptr == 1) && (eff_type->DerivesFrom(TA_taBase))) {
      TAPtr tap = *((taBase **)(new_base));
      if((tap != NULL) &&	(tap->GetOwner() == base)) { // wholly owned subsidiary
        strm << "\n";			// actually saving a path: put a newline
        taMisc::indent(strm, indent, 1);
        tap->Dump_Save_Path(strm, (taBase*) base, indent);
        strm << " {";
        if(tap->Dump_Save_PathR(strm, tap, indent+1))
          taMisc::indent(strm, indent, 1);
        strm << "};\n";
      }
    }
  }
  return rval;
}

int TypeDef::Dump_Save_Path(ostream& strm, void* base, void* par, int) {
  strm << name << " ";
  if(InheritsFrom(TA_taBase)) {
    TAPtr rbase = (TAPtr)base;
    if(rbase->GetOwner() == NULL)
      strm << "NULL";
    else {
      // its a relative path if you have a parent (period)
      if(par != NULL)
	strm << "@";
      strm << rbase->GetPath(NULL, (TAPtr)par);
    }
  }
  return true;
}

int TypeDef::Dump_Save_PathR(ostream& strm, void* base, void* par, int indent) {
  if(InheritsFormal(TA_class)) {
    return members.Dump_Save_PathR(strm, base, par, indent);
  }
  return false;
}

int TypeDef::Dump_Save_Value(ostream& strm, void* base, void* par, int indent) {
/*  if (DerivesFrom(TA_Variant)) {
    if (base == NULL) return false;
    Variant& var = *((Variant*)(base));
    bool is_null = var.isNull();
    strm << " " << (int)var.type() << " " << (is_null) ? '1' : '0';
    if ((var.type() != Variant::T_Invalid) && !is_null) {
      TypeDef* var_typ; void* var_data;
      var.GetRepInfo(var_typ, var_data);
      strm << " = " << var_typ->GetValStr(var_data, par);
    }
    strm << ";\n";
  }
  else */
  if (InheritsNonAtomicClass()) {
    // semi-hack to not do INLINE if taBase has user data
    bool inline_dump = HasOption("INLINE_DUMP");
    if (inline_dump && DerivesFrom(&TA_taOBase) && (ptr == 0)) {
      inline_dump = !((taOBase*)base)->HasUserDataList();
    }
    if(inline_dump) {
      strm << " " << GetValStr(base, par, NULL, TypeDef::SC_STREAMING) << ";\n";
    }
    else {
      strm << " {\n";
      members.Dump_Save(strm, base, par, indent+1);
    }
  } //NOTE: shouldn't this never happen???
  else {
taMisc::Warning("TypeDef::Dump_Save_Value unexpectedly doing atomic value dump for type: ",
  name);
    strm << " = " << GetValStr(base, par) << "\n";
  }
  return true;
}

int TypeDef::Dump_Save_impl(ostream& strm, void* base, void* par, int indent) {
  if(base == NULL)
    return false;

  if((ptr == 1) && DerivesFrom(TA_taBase)) {
    TAPtr tap = *((taBase **)(base));
    if((tap != NULL) &&	(tap->GetOwner() == par)) { // wholly owned subsidiary
      return tap->Dump_Save_impl(strm, (TAPtr) par, indent);
    }
    return false;
  }

  taMisc::indent(strm, indent, 1);
  if(InheritsFrom(TA_taBase)) {
    TAPtr rbase = (TAPtr)base;
    rbase->Dump_Save_Path(strm, (TAPtr)par, indent);
    rbase->Dump_Save_Value(strm, (TAPtr)par, indent);
  }
  else {
    Dump_Save_Path(strm, base, par, indent);
    Dump_Save_Value(strm, base, par, indent);
  }
  if(InheritsNonAtomicClass()) {
    bool inline_dump = HasOption("INLINE_DUMP");
    if (inline_dump && DerivesFrom(&TA_taOBase) && (ptr == 0)) {
      inline_dump = !((taOBase*)base)->HasUserDataList();
    }
    if (!inline_dump) {
      if(InheritsFrom(TA_taBase)) {
        TAPtr rbase = (TAPtr)base;
        rbase->Dump_SaveR(strm, rbase, indent+1);
      }
      else
        Dump_SaveR(strm, base, base, indent+1);
      taMisc::indent(strm, indent, 1) << "};\n";
    }
  }
//   if(InheritsFormal(TA_class) && !HasOption("INLINE_DUMP")) {
//     members.Dump_SaveR(strm, base, par, indent+1);
//     taMisc::indent(strm, indent, 1) << "};\n";
//   }
  return true;
}

int TypeDef::Dump_Save_inline(ostream& strm, void* base, void* par, int indent) {
  if(base == NULL)
    return false;

  if((ptr == 1) && DerivesFrom(TA_taBase)) {
    TAPtr tap = *((taBase **)(base));
    if((tap != NULL) &&	(tap->GetOwner() == par)) { // wholly owned subsidiary
      return tap->Dump_Save_impl(strm, (TAPtr) par, indent);
    }
    return false;
  }

  if(InheritsFrom(TA_taBase)) {
    TAPtr rbase = (TAPtr)base;
    rbase->Dump_Save_Value(strm, (TAPtr)par, indent);
  }
  else {
    Dump_Save_Value(strm, base, par, indent);
  }
  // note: we don't do our INLINE hack here, because this code only looks
  // for *non* INLINE guys in this context...
  if(InheritsNonAtomicClass() &&
     !HasOption("INLINE_DUMP"))
  {
    if(InheritsFrom(TA_taBase)) {
      TAPtr rbase = (TAPtr)base;
      rbase->Dump_SaveR(strm, rbase, indent+1);
    }
    else
      Dump_SaveR(strm, base, base, indent+1);
    taMisc::indent(strm, indent, 1) << "};\n";
  }
//   if(InheritsFormal(TA_class) && !HasOption("INLINE_DUMP")) {
//     members.Dump_SaveR(strm, base, par, indent+1);
//     taMisc::indent(strm, indent, 1) << "};\n";
//   }
  return true;
}

int TypeDef::Dump_Save(ostream& strm, void* base, void* par, int indent) {
  if (base == NULL)
    return false;

  ++taMisc::is_saving;
  dumpMisc::path_tokens.Reset();
  tabMisc::root->plugin_deps.Reset();

  strm << "// ta_Dump File v2.0\n";   // be sure to check version with Load
  taMisc::strm_ver = 2;
  if (InheritsFrom(TA_taBase)) {
    TAPtr rbase = (TAPtr)base;
    rbase->Dump_Save_pre();
    rbase->Dump_Save_GetPluginDeps();
    // if any plugins were used, write out the list of deps
    taPluginBase_List* plst = &(tabMisc::root->plugin_deps);
    if (plst->size > 0) {
      taBase* pl_par = NULL;//tabMisc::root
      plst->Dump_Save_Path(strm, pl_par, indent);
      strm << " { ";
      if (plst->Dump_Save_PathR(strm, tabMisc::root, indent+1))
        taMisc::indent(strm, indent, 1);
      strm << "};\n";
      plst->Dump_Save_impl(strm, (TAPtr)par, indent);
    }
    
    // now, write out the object itself
    rbase->Dump_Save_Path(strm, (TAPtr)par, indent);
    strm << " { ";
    if(rbase->Dump_Save_PathR(strm, (TAPtr)par, indent+1))
      taMisc::indent(strm, indent, 1);
    strm << "};\n";
    rbase->Dump_Save_impl(strm, (TAPtr)par, indent);
  } else {
    Dump_Save_Path(strm, base, par, indent);
    if (InheritsNonAtomicClass()) {
      strm << " { ";
      if(Dump_Save_PathR(strm, base, par, indent))
	taMisc::indent(strm, indent, 1);
      strm << "};\n";
    } else {
      strm << ";\n";
    }

    Dump_Save_impl(strm, base, par, indent);
  }
  --taMisc::is_saving;
  dumpMisc::path_tokens.Reset();
  return true;
}

int TypeDef::Dump_SaveR(ostream&, void*, void*, int) {
  return false;			// nothing to save
}


//////////////////////////////////////////////////////////
// 			dump load			//
//////////////////////////////////////////////////////////

int MemberSpace::Dump_Load(istream& strm, void* base, void* par,
			   const char* prv_read_nm, int prv_c) {
  if(taMisc::verbose_load >= taMisc::TRACE) {
    const char* nm = (prv_read_nm != NULL) ? prv_read_nm : "NULL";
    cerr << "Entering MemberSpace::Dump_Load, type: " << owner->name
	 << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	 << ", prv_read_nm = " << nm
	 << ", prv_c = " << prv_c << "\n"; // endl;
    taMisc::FlushConsole();
  }

  int rval = 2;			// default assumption is that no members are loaded
  do {
    int c;
    if(prv_read_nm != NULL) {
      taMisc::LexBuf = prv_read_nm;
      c = prv_c;
      prv_read_nm = NULL;
    }
    else {
      c = taMisc::read_word(strm, true);
    }
    if(c == EOF) {
      if(taMisc::verbose_load >= taMisc::MESSAGES)
	taMisc::Warning("<<< EOF in MemberSpace::Dump_Load", taMisc::LexBuf);
      return EOF;
    }
    if(c == '}') {
      strm.get();		// get the bracket (above was peek)
      if(strm.peek() == ';') strm.get(); // skip past ending semi
      if(taMisc::verbose_load >= taMisc::TRACE) {
	const char* nm = (prv_read_nm != NULL) ? prv_read_nm : "NULL";
	cout << "}, Leaving MemberSpace::Dump_Load, type: " << owner->name
	     << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	     << ", prv_read_nm = " << nm
	     << ", prv_c = " << prv_c << "\n"; // endl;
	taMisc::FlushConsole();
      }
      return rval;
    }
    if(c == '$') {		// got a path token
      strm.get();
      c = taMisc::read_word(strm);
      String tok_nm = taMisc::LexBuf;
      strm.get();		// read the end of token
      c = taMisc::skip_white(strm); // get the equals
      c = taMisc::read_till_lb_or_semi(strm);
      if(c == EOF) {
	if(taMisc::verbose_load >= taMisc::MESSAGES)
	  taMisc::Warning("<<< EOF in MemberSpace::Dump_Load", taMisc::LexBuf);
	return EOF;
      }
      String path_val = taMisc::LexBuf;
      dumpMisc::path_tokens.NewLoadToken(path_val, tok_nm);
      continue;			// keep on truckin
    }

    String mb_name = taMisc::LexBuf;
    if((c == ' ') || (c == '\n') || (c == '\t')) { // skip past white
      strm.get();
      c = taMisc::skip_white(strm, true);
    }
    int tmp;
    if((c == '@') || (c == '.') || (c == '*')) {	// got a path
      // pass the mb_name, which is the type name, to the load function...
      tmp = TA_taBase.Dump_Load_impl(strm, NULL, base, mb_name);
    }
    else if (c == '[') {
      // finished with members, mat data coming up (so exit members)
      return 3; // special kludgy code to tell Matrix::Dump_Load to expect mat vals
    } else {
      MemberDef* md = FindName(mb_name);
      if(md == NULL) {		// try to find a name with an aka..
	int a;
	for(a=0;a<size;a++) {
	  MemberDef* amd = FastEl(a);
	  String aka = amd->OptionAfter("AKA_");
	  if(aka.empty()) continue;
	  if(aka == mb_name) {
	    md = amd;
	    break;
	  }
	}
      }
      if(md != NULL) {
        // if md is a Variant, expect the type/null codes -- load them before proceeding
        
        TypeDef* type = md->type; 
        if (type->DerivesFrom(TA_Variant)) {
          Variant& var = *((Variant*)(md->GetOff(base)));
          // read vartype(int) and null (1/0):
          //NOTE: we don't actually bail if not found, to support possible case where
          // a non-variant member was changed to be a variant -- however, the loading
          // could fail because it may not be set to the the correct type
          if (!var.Dump_Load_Type(strm, c)) {
            taMisc::Warning("*** expected variant type information for member:", md->name,
              "in type:", md->GetOwnerType()->name);
          }
        }
	tmp = md->Dump_Load(strm, base, par);
	if(tmp == EOF) {
	  if(taMisc::verbose_load >= taMisc::MESSAGES)
	    taMisc::Warning("<<< EOF in MemberSpace::Dump_Load::else:", mb_name);
	  return EOF;
	}
	if(tmp == false)
	  taMisc::skip_past_err(strm);
      }
      else {
	taMisc::Warning("*** Member:",mb_name,"not found in type:",
			owner->name, "(this is likely just harmless version skew)");
	int sv_vld = taMisc::verbose_load;
	taMisc::verbose_load = taMisc::SOURCE;
	taMisc::skip_past_err(strm);
	taMisc::verbose_load = (taMisc::LoadVerbosity)sv_vld;
	cerr << "\n\n"; // endl << endl << flush;
// 	taMisc::FlushConsole();
      }
      rval = true;		// member was loaded, do update after edit
    }
  } while (1);
  if(taMisc::verbose_load >= taMisc::TRACE) {
    const char* nm = (prv_read_nm != NULL) ? prv_read_nm : "NULL";
    cerr << "Leaving MemberSpace::Dump_Load, type: " << owner->name
	 << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	 << ", prv_read_nm = " << nm
	 << ", prv_c = " << prv_c << "\n"; // endl;
    taMisc::FlushConsole();
  }
  return rval;
}

int MemberDef::Dump_Load(istream& strm, void* base, void* par) {
   //NOTE: this routine can be called in either of two loading contexts:
   // 1) loading the Path portion
   // 2) loading the Value portion
   // Variants of pointer subtypes stream their type info during the Path phase
//NOTE: BA 2006-12-01 we shouldn't check on load: if it is in dump file, then
  // we should load it, otherwise many dynamic and regression issues arise
//  if(!DumpMember(par)) return false;

  if(taMisc::verbose_load >= taMisc::TRACE) {
    cerr << "Entering MemberDef::Dump_Load, member: " << name
	 << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	 << "\n"; // endl;
    taMisc::FlushConsole();
  }
  void* new_base = GetOff(base);
  int rval;
  TypeDef* eff_type = type; // overridden for variants

  if (type->InheritsNonAtomicClass()) {
    if(type->InheritsFrom(TA_taBase)) {
      TAPtr rbase = (TAPtr)new_base;
      rval = rbase->Dump_Load_impl(strm, (TAPtr)base);
    }
    else
      rval = type->Dump_Load_impl(strm, new_base, base);
    if (taMisc::verbose_load >= taMisc::TRACE) {
      cerr << "Leaving MemberDef::Dump_Load, member: " << name
	   << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	   << "\n"; // endl;
      taMisc::FlushConsole();
    }
    return rval;
  }
  else {
    // if we are a variant, then adjust the eff_type and proceed
    int c = 0; // streaming char
    if (eff_type->DerivesFrom(TA_Variant)) {
      Variant& var = *((Variant*)(new_base));
      // we hopefully already loaded type info, but in any event, must proceed based on 
      // what is the current type of the variant
      // there can only be data if it was valid and not null, otherwise we should expect a ;
      var.GetRepInfo(eff_type, new_base);
    }
    
    if ((eff_type->ptr == 1) && eff_type->DerivesFrom(TA_taBase)
            && HasOption("OWN_POINTER"))
    {
      c = taMisc::skip_white(strm, true);
      if (c == '{') {
        // a taBase object that was saved as a member, but is now a pointer..
        TAPtr rbase = *((TAPtr*)new_base);
        if(rbase == NULL) {	// it's a null object, can't load into it
          taMisc::Warning("*** Can't load into NULL pointer object for member:", name,
                        "in eff_type:",GetOwnerType()->name);
          return false;
        }
        // treat it as normal..
        rval = rbase->Dump_Load_impl(strm, (TAPtr)base);
        if(taMisc::verbose_load >= taMisc::TRACE) {
          cerr << "Leaving MemberDef::Dump_Load, member: " << name
              << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
              << "\n"; // endl;
	  taMisc::FlushConsole();
        }
        //TODO: if we allow Variant taBase pointers, need to detect and do FixNull
        // if (type->InheritsFormal(TA_Variant) ... do FixNull
        return rval;
      }
    }
  
    c = taMisc::skip_white(strm); // read next char, skipping white space
    // if semi, no value is present, so just exit
    if (c == ';') 
      return true;
    // otherwise, it better be an = 
    if (c != '=') {
      taMisc::Warning("*** Missing '=' in dump file for member:", name,
                    "in eff_type:",GetOwnerType()->name);
      return false;
    }
    
/*obs    //note: in v3, we based expectation of " on string type, but
    // we should rather let input stream tell us
    c = taMisc::skip_white(strm, true); // don't read next char, just skip ws
    
    if (eff_type->InheritsFrom(TA_taString)) {
      c = taMisc::read_till_quote(strm); // get 1st quote
      if (c == '\"')			  // "
        c = taMisc::read_till_quote_semi(strm);// then till second followed by semi
    } else {
      c = taMisc::read_till_rb_or_semi(strm);
    }
  
    if(c != ';') {
      taMisc::Warning("*** Missing ';' in dump file for member:", name,
                    "in eff_type:",GetOwnerType()->name);
      return true;		// don't scan any more after this err..
    }*/
  
    c = taMisc::skip_white(strm, true); // don't read next char, just skip ws
    // in 4.x, we let the stream tell us if a quoted string is coming...
    if (c == '\"') {
      c = taMisc::skip_till_start_quote_or_semi(strm); // duh, has to succeed!
      c = taMisc::read_till_end_quote_semi(strm); // 
      
    } else {
      c = taMisc::read_till_rb_or_semi(strm);
    }
  
    if (c != ';') {
      taMisc::Warning("*** Missing ';' in dump file for member:", name,
                    "in eff_type:",GetOwnerType()->name);
      return true;		// don't scan any more after this err..
    }
    eff_type->SetValStr(taMisc::LexBuf, new_base, base, this);
    if(taMisc::verbose_load >= taMisc::TRACE) {
      cerr << "Leaving MemberDef::Dump_Load, member: " << name
          << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
          << "\n"; // endl;
      taMisc::FlushConsole();
    }
    return true;
  }
}

int TypeDef::Dump_Load_Path(istream& strm, void*& base, void* par,
			    TypeDef*& td, String& path, const char* typnm)
{
  if(taMisc::verbose_load >= taMisc::TRACE) {
    const char* nm = (typnm != NULL) ? typnm : "NULL";
    cerr << "Entering TypeDef::Dump_Load_Path, type: " << name
	 << ", path = " << path
	 << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	 << ", typnm = " << nm
	 << "\n"; // endl;
    taMisc::FlushConsole();
  }
  td = NULL;
  int c;
  c = taMisc::read_till_lb_or_semi(strm, true);
  if(c == EOF) {
    if(taMisc::verbose_load >= taMisc::MESSAGES)
      taMisc::Warning("<<< EOF in Dump_Load_Path:", name, taMisc::LexBuf);
    return EOF;
  }

  // the format is usually: type path, but if tpnm is passed, then it is used
  bool has_type = true;
  if(taMisc::LexBuf.length() == 0)
    has_type = false;		// doesn't have anything
  else if(taMisc::LexBuf.lastchar() == ' ') // trailing spaces..
    taMisc::LexBuf = taMisc::LexBuf.before(' ',-1); // we hope this doesn't happen

  int spc_idx = -1;
  if(has_type && ((spc_idx = taMisc::LexBuf.index(' ')) < 0)) // we still think we have a type but don't
    has_type = false;

  String tpnm;
  if(typnm != NULL) {
    tpnm = typnm;
    if(has_type)
      path = taMisc::LexBuf.after(spc_idx);
    else
      path = taMisc::LexBuf;	// just read the whole path..
    has_type = true;		// actually do have type name
  }
  else {
    if(has_type) {
      tpnm = taMisc::LexBuf.before(spc_idx);
      path = taMisc::LexBuf.after(spc_idx);
    }
    else {
      path = taMisc::LexBuf;
    }
  }
  if(!has_type)
    td = this;			// assume this is the right type..
  else {
    td = taMisc::types.FindName(tpnm);
    if(td == NULL) {
      taMisc::Warning("*** Unknown type:",tpnm,"in Dump_Load_Path");
      return false;
    }

    if(!DerivesFrom(TA_taBase)) {
      if(!DerivesFrom(td)) {
	taMisc::Warning("*** Type mismatch, expecting:",name,"Got:",td->name);
	return false;
      }
    }
  }

  if((base != NULL) || !(td->DerivesFrom(TA_taBase)) || (path == "")) {
    if(taMisc::verbose_load >= taMisc::TRACE) {
      const char* nm = (typnm != NULL) ? typnm : "NULL";
      cerr << "true Leaving TypeDef::Dump_Load_Path, type: " << name
	   << ", path = " << path
	   << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	   << ", typnm = " << nm
	   << "\n"; // endl;
      taMisc::FlushConsole();
    }
    return true;		// nothing left to do here, actually
  }

  int rval = td->Dump_Load_Path_impl(strm, base, par, path);
  if(taMisc::verbose_load >= taMisc::TRACE) {
    const char* nm = (typnm != NULL) ? typnm : "NULL";
    cerr << "Leaving TypeDef::Dump_Load_Path, type: " << name
	 << ", path = " << path
	 << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	 << ", typnm = " << nm
	 << ", rval = " << rval
	 << "\n"; // endl;
    taMisc::FlushConsole();
  }
  return rval;
}

int TypeDef::Dump_Load_Path_impl(istream&, void*& base, void* par, String path) {
  bool ptr_flag = false;	// pointer was loaded
  String orig_path;
  TAPtr find_base = NULL;	// where to find item from
  if(taMisc::verbose_load >= taMisc::TRACE) {
    cerr << "Entering TypeDef::Dump_Load_Path_impl, type: " << name
	 << ", path = " << path
	 << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	 << "\n"; // endl;
    taMisc::FlushConsole();
  }

  if(path.firstchar() == '@') {		// relative to current parent..
    if(par == NULL) {
      taMisc::Warning("*** Relative path with NULL parent:", path);
      return false;
    }
    path = path.after('@');
    find_base = (TAPtr)par;	// we better get a taBase
  }

  if(path.firstchar() == '*') {
    ptr_flag = true;
    path = path.after('*');
  }

  // strip any leading parens, etc
  while(path.firstchar() == '(') path = path.after('(');
  while(path.lastchar() == ')') path = path.before(')',-1);

  if(find_base==NULL)
    find_base = tabMisc::root;	// search from top if not relative

  dumpMisc::path_subs.FixPath(NULL, find_base, path);  // fix the path name.. (substitution)
  orig_path = path;				  // original is post-substitution

  if((path == "root") || (path == ".")) {
    base = (void*)find_base;
    if(taMisc::verbose_load >= taMisc::TRACE) {
      cerr << "root Leaving TypeDef::Dump_Load_Path_impl, type: " << name
	   << ", path = " << path
	   << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	   << "\n"; // endl;
      taMisc::FlushConsole();
    }
    return true;
  }

  int delim_pos = taBase::GetLastPathDelimPos(path);
  String el_path;
  if(path[delim_pos] == '[')
    el_path = path.from(delim_pos);
  else
    el_path = path.after(delim_pos);
  String ppar_path;
  if(delim_pos > 0)
    ppar_path = path.before(delim_pos);

  MemberDef* ppar_md = NULL;
  taBase* ppar = find_base->FindFromPath(ppar_path, ppar_md); // path-parent

  if(!ppar) {
    taMisc::Warning("*** Could not find a parent for:",el_path,"in",ppar_path);
    return false;
  }

  if(ppar_md && !ppar_md->type->InheritsFrom(TA_taBase)) {
    taMisc::Warning("*** Parent must be a taBase type for:",el_path,"in",ppar_path,
	"type:",ppar_md->type->name);
    return false;
  }

  // this is where the parent loads the child!  target type is this type
  void* nw_base = NULL;
  if(ptr_flag)
    nw_base = ppar->Dump_Load_Path_ptr(el_path, this);
  else
    nw_base = ppar->Dump_Load_Path_parent(el_path, this);

  if(nw_base) {
    base = nw_base;
    return true;
  }
  else {
    return false;
  }
}


int TypeDef::Dump_Load_Value(istream& strm, void* base, void* par) {
  if(taMisc::verbose_load >= taMisc::TRACE) {
    cerr << "Entering TypeDef::Dump_Load_Value, type: " << owner->name
	 << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	 << "\n"; // endl;
    taMisc::FlushConsole();
  }
  int c = taMisc::skip_white(strm);
  if(c == EOF) {
    if(taMisc::verbose_load >= taMisc::MESSAGES)
      taMisc::Warning("<<< EOF in Dump_Load_Value:", name);
    return EOF;
  }
  if(c == ';')    return 2;  // signal that just a path was loaded..
  if(c == '}') {
    if(strm.peek() == ';') strm.get();
    if(taMisc::verbose_load >= taMisc::TRACE) {
      cerr << "} Leaving TypeDef::Dump_Load_Value, type: " << owner->name
	   << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	   << "\n"; // endl;
      taMisc::FlushConsole();
    }
    return 2;
  }

  // semi-hack to properly load taBase INLINE types that have user data
  // in that case, we streamed out as if no INLINE, and the cheat to detect
  // this is that the { doesn't have the member right after, but a newline
  bool inline_dump = HasOption("INLINE_DUMP");
  if (inline_dump) {
    // note: pre 4.0.19 streams had a space after { in path sections
    char c = strm.peek();
    inline_dump = !((c == ' ') || (c == '\n'));
  }
  if(inline_dump) {
    if(c != '{') {
      taMisc::Warning("*** Missing '{' in dump file for inline type:", name);
      return false;
    }
    c = taMisc::read_till_rb_or_semi(strm);
    if(c != '}') {
      taMisc::Warning("*** Missing '}' in dump file for inline type:", name);
      return false;
    }
    taMisc::LexBuf = String("{") + taMisc::LexBuf; // put lb back in..
    SetValStr(taMisc::LexBuf, base);
  }
  else if(InheritsFormal(TA_class)) {
    if(c != '{') {
      taMisc::Warning("*** Missing '{' in dump file for type:",name);
      return false;
    }
    return members.Dump_Load(strm, base, par);
  }
  else {
    c = taMisc::skip_white(strm);
    if(c == EOF) {
      if(taMisc::verbose_load >= taMisc::MESSAGES)
	taMisc::Warning("<<< EOF in Dump_Load_Value::else:", name);
      return EOF;
    }
    if(c != '=') {
      taMisc::Warning("*** Missing '=' in dump file for type:",name);
      return false;
    }

    c = taMisc::read_till_rb_or_semi(strm);
    if(c == EOF) {
      if(taMisc::verbose_load >= taMisc::MESSAGES)
	taMisc::Warning("<<< EOF in Dump_Load_Value::rb", name);
      return EOF;
    }

    SetValStr(taMisc::LexBuf, base);
  }
  if(taMisc::verbose_load >= taMisc::TRACE) {
    cerr << "Leaving TypeDef::Dump_Load_Value, type: " << owner->name
	 << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	 << "\n"; // endl;
    taMisc::FlushConsole();
  }
  return true;
}

int TypeDef::Dump_Load_impl(istream& strm, void* base, void* par, const char* typnm) {
  if(taMisc::verbose_load >= taMisc::TRACE) {
    const char* nm = (typnm != NULL) ? typnm : "NULL";
    cerr << "Entering TypeDef::Dump_Load_impl, type: " << owner->name
	 << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	 << ", typnm = " << nm
	 << "\n"; // endl;
    taMisc::FlushConsole();
  }
  TypeDef* td = NULL;
  String path;
  int rval = Dump_Load_Path(strm, base, par, td, path, typnm);
  if((taMisc::verbose_load >= taMisc::TRACE) && (td != NULL))
    cerr << "Loading: " << td->name << " " << path << " rval: " << rval << "\n";
  if((rval > 0) && (base != NULL)) {
    if(td->InheritsFrom(TA_taBase)) {
      TAPtr rbase = (TAPtr)base;
      rval = rbase->Dump_Load_Value(strm, (TAPtr)par);
      if(rval==1) {
	if (rbase->HasOption("IMMEDIATE_UPDATE"))
	  rbase->UpdateAfterEdit();
	else if(!rbase->HasOption("NO_UPDATE_AFTER")) {
	  dumpMisc::update_after.Link(rbase);
	}
	// post load is a separate option, compatible with IMMED or NO_UA
	if (!taMisc::is_post_loading && rbase->HasOption("DUMP_LOAD_POST")) {
	  dumpMisc::post_update_after.Link(rbase);
	}
      }
    }
    else {
      rval = td->Dump_Load_Value(strm, base, par);
    }
    if(rval == EOF) {
      if(taMisc::verbose_load >= taMisc::MESSAGES)
	taMisc::Warning("<<< EOF in Dump_Load_impl:", name);
      return EOF;
    }
    if(rval == false) {
      // read till next rbracket, (skipping over inner ones..)
      // be sure to revise dump_loads to return false on error
      int c = taMisc::skip_past_err(strm);
      if(c == EOF) return EOF;
      if(taMisc::verbose_load >= taMisc::TRACE) {
	const char* nm = (typnm != NULL) ? typnm : "NULL";
	cerr << "err Leaving TypeDef::Dump_Load_impl, type: " << owner->name
	     << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	     << ", typnm = " << nm
	     << "\n"; // endl;
	taMisc::FlushConsole();
      }
      return true;		// we already scanned past error
    }
  }
  else {
    // make sure we skip until we hit the end of this one (rbracket)
    // not just the semicolon at the end of some data item..
    if(strm.peek() == '{')
      strm.get();		// if we're at the start of a bracket, get it first
    if(strm.peek() == '=') {	// if we're reading a path and it didn't work, need to get lb!
      taMisc::read_till_lbracket(strm, false);
    }
    int c;
    //    if((path.length() > 1) && (path[1] == '*')) // for path items, use non-peek, else use peek
    //    cerr << "***HERE***" << endl;
    c = taMisc::skip_past_err_rb(strm, false);
//     else
//       c = taMisc::skip_past_err_rb(strm, true);
    if(c == EOF) return EOF;
    if(taMisc::verbose_load >= taMisc::TRACE) {
      const char* nm = (typnm != NULL) ? typnm : "NULL";
      cerr << "err rb Leaving TypeDef::Dump_Load_impl, type: " << owner->name
	   << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	   << ", typnm = " << nm
	   << "\n"; // endl;
      taMisc::FlushConsole();
    }
    return true;		// already scanned past error
  }
  if(taMisc::verbose_load >= taMisc::TRACE) {
    const char* nm = (typnm != NULL) ? typnm : "NULL";
    cerr << "Leaving TypeDef::Dump_Load_impl, type: " << owner->name
	 << ", par = " << String((ta_intptr_t)par) << ", base = " << String((ta_intptr_t)base)
	 << ", typnm = " << nm
	 << ", rval = " << rval
	 << "\n"; // endl;
    taMisc::FlushConsole();
  }
  return rval;
}

int TypeDef::Dump_Load(istream& strm, void* base, void* par, void** el_) {
//WARNING: DO NOT put any calls to eventloop in the load code -- it will cause crashes
  if (el_) *el_ = NULL; //default if error
  if(base == NULL) {
    taMisc::Warning("*** Cannot load into NULL");
    return false;
  }

  dumpMisc::path_subs.Reset();
  dumpMisc::path_tokens.Reset();
  dumpMisc::vpus.Reset();
  dumpMisc::update_after.Reset();
  // NOTE: post_update_after can include load commands so don't reset here
  // -- is auto reset after running!
  //  dumpMisc::post_update_after.Reset();
  tabMisc::root->plugin_deps.Reset();

  int c;
  c = taMisc::read_till_eol(strm);
  if(c == EOF) return EOF;
  if (taMisc::LexBuf.contains("// ta_Dump File v1.0")) {
    taMisc::strm_ver = 1;
  } else if(taMisc::LexBuf.contains("// ta_Dump File v2.0")) {
    taMisc::strm_ver = 2;
  } else {
    taMisc::Warning("*** Dump file does not have proper format id:", taMisc::LexBuf);
    return false;
  }

  TypeDef* td;
  String path;
  int rval = Dump_Load_Path(strm, base, par, td, path); // non-null base just gets type
  if(rval <= 0) {
    taMisc::Warning("*** Dump load aborted due to errors");
    return false;
  }
  
  if(!td->InheritsFrom(TA_taBase)) {
    taMisc::Warning("*** Only taBase objects may be loaded, not:", td->name);
    return false;
  }

  if(taMisc::verbose_load >= taMisc::TRACE) {
    cout << "Loading: " << td->name << " " << path << " rval: " << rval << "\n";
  }

  // locals needed below, so we can use a jump
  String new_path;
  taBase* el = NULL; // the loaded element
  
  ++taMisc::is_loading;

  // check for plugin deps, if so, load those in and resume
  if (td->InheritsFrom(TA_taPluginBase_List) && 
    (path == ".plugin_deps")) 
  {
    rval = tabMisc::root->Dump_Load_Value(strm, NULL); // read it
    rval = Dump_Load_impl(strm, NULL, tabMisc::root); 	// base is given by the path..
    // resolve plugin dependencies, and warn/abort if not all are present
    if (!tabMisc::root->VerifyHasPlugins()) {
      rval = false;
      goto endload;
    }

    //read again -- get the actual path
    rval = Dump_Load_Path(strm, base, par, td, path); // non-null base just gets type
  }


  if(InheritsFrom(td)) {		// we are the same as load token
    el = (TAPtr)base;			// so use the given base
  }
  else {
    TAPtr par = (TAPtr)base;		// given base must be a parent
    el = par->New(1,td);		// create one of the saved type
    if(el == NULL) {
      taMisc::Warning("*** Could not make a:",td->name,"in:",par->GetPath());
      rval = false;
      goto endload;
    }
  }


  new_path = el->GetPath();
  if(new_path != path)
    dumpMisc::path_subs.Add(td, tabMisc::root, path, new_path);

  rval = el->Dump_Load_Value(strm, (TAPtr)par); // read it

  while(rval != EOF) {
    rval = Dump_Load_impl(strm, NULL, par); 	// base is given by the path..
  }

  dumpMisc::vpus.Resolve(); 			// try to cache out references.

  for (int i=0; i<dumpMisc::update_after.size; i++) {
    TAPtr tmp = dumpMisc::update_after.FastEl(i);
    if(taBase::GetRefn(tmp) <= 1) {
      taMisc::Warning("*** Object: of type:",
		      tmp->GetTypeDef()->name,"named:",tmp->GetName(),"is unowned!");
      taBase::Ref(tmp);
    }
    tmp->UpdateAfterEdit();
  }
  rval = true;
  
endload:
  --taMisc::is_loading;

  dumpMisc::update_after.Reset(); //note: don't reset post list!
  dumpMisc::path_subs.Reset();
  dumpMisc::path_tokens.Reset();
  dumpMisc::vpus.Reset();
  if (el_) *el_ = (void*)el;
  // if there were any post guys, send a msg to the taiMisc object, who will call us
  // back when the eventloop gets processed next
  if (dumpMisc::post_update_after.size > 0) {
    if (!taMisc::is_post_loading) {
      QTimer::singleShot(0, taiMC_, SLOT(PostUpdateAfter()) );
    } else { // shouldn't happen???
      dumpMisc::post_update_after.Reset(); // if it does happen, don't leave around
    }
  }
  return rval;
}


void taBase::Dump_Save_GetPluginDeps() {
  if (!tabMisc::root) return;
  // check me!
  tabMisc::root->CheckAddPluginDep(this->GetTypeDef());
  
  // then check my members
  MemberSpace& ms = GetTypeDef()->members;
  for (int i = 0; i < ms.size; ++i) {
    MemberDef* md = ms.FastEl(i);
    if (!md) continue; // shouldn't happen
    if (!md->DumpMember(this)) continue;
    // ok, check if embedded taBase, or owned pointer
    TypeDef* td = md->type; 
    //TODO: very obscure, but could possibly be a taBase in a Variant
    if (!td->DerivesFrom(&TA_taBase)) continue;
    taBase* ta = NULL;
    if (td->ptr == 0) { // embedded
      ta = (taBase*)md->GetOff(this);
    } else if (td->ptr == 1) { // ptr to... but needs to be owned!
      ta = *(taBase**)md->GetOff(this);
      if (!ta || (ta->GetOwner() != this)) continue;
    } else continue;// ptr > 1, not supposed to be saveable!
    ta->Dump_Save_GetPluginDeps();
  }
}

void taList_impl::Dump_Save_GetPluginDeps() {
  inherited_taBase::Dump_Save_GetPluginDeps();
  if (!Dump_QuerySaveChildren()) return;
  
  for (int i = 0; i < size; ++i) {
    taBase* itm = (taBase*)FastEl_(i);
    if (!itm) continue;
    
    // we check item for plugin if we'll save it,
    // i.e. if we own it, or it is a LINK_SAVE link
    taBase* own = itm->GetOwner();
    if ((own == this) ||
      (!own && itm->HasOption("LINK_SAVE"))
    ){
      itm->Dump_Save_GetPluginDeps();
    }
  }
}

void taGroup_impl::Dump_Save_GetPluginDeps() {
  inherited::Dump_Save_GetPluginDeps();
  gp.Dump_Save_GetPluginDeps();
}

