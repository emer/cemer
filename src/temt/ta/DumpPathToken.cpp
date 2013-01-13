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

#include "DumpPathToken.h"
#include <DumpPathTokenList>
#include <taBase>
#include <dumpMisc>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>


DumpPathToken::DumpPathToken(taBase* obj, const String& pat, const String& tok_id) {
  object = obj;
  path = pat;
  token_id = tok_id;
}

void DumpPathTokenList::Reset() {
  inherited::Reset();
  obj_hash_table.Reset();
}

DumpPathTokenList::DumpPathTokenList() {
  obj_hash_table.key_type = taHashTable::KT_PTR;
  BuildHashTable(10000); // this is big enough for almost anything -- typically not nearly that many paths
}

void DumpPathTokenList::ReInit(int obj_hash_size) {
  Reset();
  obj_hash_table.Alloc(obj_hash_size);
}

int DumpPathTokenList::FindObj(taBase* obj) {
  return obj_hash_table.FindHashValPtr(obj);
}

DumpPathToken* DumpPathTokenList::AddObjPath(taBase* obj, const String& pat) {
  String tok_id;
  if(taMisc::save_old_fmt)
    tok_id = String("$") + String(size) + "$"; // use old-style numbered tokens
  else
    tok_id = String("$") + pat + "$"; // tok_id is now always just the path!
  int idx = size;
  DumpPathToken* tok = new DumpPathToken(obj, pat, tok_id);
  Add(tok);
  if(obj)			// don't add for nulls!
    obj_hash_table.AddHashPtr(obj, idx);
  return tok;
}

String DumpPathTokenList::GetPath(taBase* obj) {
  int idx = FindObj(obj);
  if(idx >= 0)
    return FastEl(idx)->token_id;

  String path;
  if(taMisc::save_use_name_paths)
    path = obj->GetPathNames();
  else
    path = obj->GetPath();
  DumpPathToken* tok = AddObjPath(obj, path);
  if(taMisc::save_old_fmt) {
    path += tok->token_id;
    return path;
  }
  path += "$$";	// this marks this as a new token to be stored..
  // if obj is outside of root path, provide extra info about the object
  // that can be used during loading for finding object of correct type/name
  // even if it is not on the right path..
  if(!path.startsWith(dumpMisc::dump_root_path)) {
    path += "<" + obj->GetTypeDef()->name + "," + obj->GetName() + ">";
  }
  return path;
}

DumpPathToken* DumpPathTokenList::NewLoadToken(const String& pat, const String& tok_id) {
  DumpPathToken* tok = AddObjPath(NULL, pat);
  if(taMisc::strm_ver < 3) {
    int tok_num = (int)tok_id;
    if(tok_num != size-1) {
      taMisc::Warning("Path Tokens out of order, on list:", tok->token_id, "in file:",
		      tok_id);
      tok->object = NULL;
      tok->path = "";
      String null_pat;
      while(size <= tok_num) 
	AddObjPath(NULL, null_pat);	// catch up if possible
      tok = FastEl(tok_num);
      tok->path = pat;
    }
  }
  if(taMisc::verbose_load >= taMisc::MESSAGES) {
    taMisc::Warning("---> New Path Token:",tok_id,"path:",pat);
  }
  return tok;
}

taBase* DumpPathTokenList::FindFromPath(String& pat, TypeDef* td, void* base,
				      void* par, MemberDef* memb_def)
{

  if(pat[0] == '$') {		// its a token path, not a real one..
    String pat_act = pat(1, pat.length()-2);	// actual path = nuke $ $'s
    DumpPathToken* tok = NULL;
    if(taMisc::strm_ver < 3) {
      tok = SafeEl((int)pat_act);	// just index of token item
      if(tok == NULL) {
	taMisc::Warning("Path Token Not Created Yet:", pat_act);
	return NULL;
      }
      if((tok->object == NULL) && (base != NULL)) {
	dumpMisc::vpus.AddVPU(base, (taBase*)par, pat_act, memb_def); // saving actual path here
	return NULL;
      }
      return tok->object;		// this is what we're looking for
    }
    else {
      tok = FindName(pat_act);	// using hash coded name lookup on raw path itself!
      if(tok) {
	if(tok->object)
	  return tok->object;
	else {
	  if(base) {
	    dumpMisc::vpus.AddVPU(base, (taBase*)par, pat_act, memb_def);
	  }
	}
	return NULL;
      }
      // note: strm_ver >=3 with a $path$ without a tok will fall through to make a new token..
      pat = pat_act + "$$";	// just trun it into a simple definition version
    }
  }

  // target type and name, if present -- to assist in finding objects if not found
  TypeDef* trg_td = NULL;
  String   trg_nm;

  DumpPathToken* tok = NULL;
  int tok_idx = -1;
  if(pat.lastchar() == '$'|| pat.lastchar() == '>') { // path contains definition of new token
    String token_id = pat.after('$');
    token_id = token_id.before('$');
    if(pat.lastchar() == '>') {	// name, type hint
      String typnm = pat.between('<', '>');
      trg_nm = typnm.after(',');
      typnm = typnm.before(',');
      trg_td = taMisc::FindTypeName(typnm);
    }
    pat = pat.before('$');
    if(token_id.empty())	// version 3 = its just the path
      token_id = pat;
    tok = NewLoadToken(pat, token_id); // make the token
    tok_idx = size-1;
  }

  dumpMisc::path_subs.FixPath(td, tabMisc::root, pat);
  MemberDef* md = NULL;
  taBase* rval = tabMisc::root->FindFromPath(pat, md);
  if(rval && md && (md->type->ptr == 1)) { // deref ptr
    rval = *((taBase**)rval);
  }
  rval = FixPathFind(pat, rval, trg_td, trg_nm); // try to fix based on trg info
  if(!rval) {
    if(base != NULL)
      dumpMisc::vpus.AddVPU(base, (taBase*)par, pat, memb_def);
    return NULL;
  }
  if(tok) {
    if(!(tok->object) && rval) { // we're going to set this guy -- add to hash table!
      obj_hash_table.AddHashPtr(rval, tok_idx); // only case for this is last guy
    }
    tok->object = rval;
  }
  return rval;
}

taBase* DumpPathTokenList::FixPathFind(const String& pat, taBase* init_find,
				       TypeDef* trg_typ, const String& trg_nm) {
  if(!trg_typ) return init_find; // typ info should be on all cases if present at all
  bool name_ok = true;
  bool type_ok = true;
  if(init_find) {
    if(!init_find->InheritsFrom(trg_typ)) {
      // not good -- must inherit from proper type!
      type_ok = false;
    }
    if(trg_nm.nonempty() && (init_find->GetName() != trg_nm)) {
      // see if we can find one with right name..
      name_ok = false;
    }
    if(!name_ok || !type_ok) goto find_new;
    return init_find;		// must be good, go with it!
  }

 find_new:			// find a new guy -- find a root to search from
  taList_impl* find_root = NULL;
  if(init_find && init_find->GetOwner()) {
    // find any kind of list owner!
    find_root = (taList_impl*)init_find->GetOwner(&TA_taList_impl);
  }

  if(!find_root) {		// didn't get anything -- try walking up path to prev [
    // the get owner thing should have worked but this is just a failsafe..
    if(pat.contains('[')) {
      String owpth = pat.before('[', -1);
      MemberDef* md;
      find_root = (taList_impl*)tabMisc::root->FindFromPath(owpth, md);
      if(find_root && !find_root->InheritsFrom(&TA_taList_impl)) {
	find_root = (taList_impl*)find_root->GetOwner(&TA_taList_impl);
      }
    }
  }

  if(!find_root) return init_find; // couldn't do anything better here.

  taBase* rval = NULL;
  if(find_root->InheritsFrom(&TA_taGroup_impl)) {
    taGroup_impl* gprt = (taGroup_impl*)find_root;
    if(gprt->root_gp)
      gprt = gprt->root_gp; // go up to root of initial find owner
    if(trg_nm.nonempty()) {
      rval = gprt->FindLeafName_(trg_nm);
      if(!rval && !type_ok) {
	// didn't find name -- only if our orig did not have a good type, search for new type
	rval = gprt->FindLeafType_(trg_typ);
      }
    }
    else { // try type..
      rval = gprt->FindLeafType_(trg_typ);
    }
  }
  else {
    // no leaf searches for plain lists..
    if(trg_nm.nonempty()) {
      rval = (taBase*)find_root->FindName_(trg_nm);
      // don't worry about type here -- could have changed -- name is a good enough match
      if(!rval && !type_ok) {
	// didn't find name -- only if our orig did not have a good type, search for new type
	rval = find_root->FindType_(trg_typ);
      }
    }
    else {  // try type
      rval = find_root->FindType_(trg_typ);
    }
  }
  if(rval) return rval;		// if we got something, use it!
  return init_find;		// otherwise fall back on this
}

