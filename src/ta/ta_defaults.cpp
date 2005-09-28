// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// ta_defaults.cc

#include "ta_defaults.h"
#include "ta_TA_type.h"

void TypeDefault::Initialize() {
  old_type = NULL;
  default_type = &TA_taBase;
  token = NULL;
  active_membs.SetBaseType(&TA_NameValue);
  int i;
  for(i=0; i< MAX_DFT_LONGS; i++)
    active[i] = 0;
}

void TypeDefault::Destroy() {
  if(token != NULL)
    taBase::unRefDone(token);
  if(default_type != NULL) {
    taBase_List* gp = default_type->defaults;
    if(gp != NULL) {
      taBase::Ref(this);
      taBase::Ref(this);	// make sure that the remove doesn't cause a delete..
      gp->Remove(this);
    }
  }
  token = NULL;
}

int TypeDefault::Dump_Load_Value(istream& strm, TAPtr par) {
  active_membs.Reset();		// remove all members before loading..
  int rval = taNBase::Dump_Load_Value(strm, par);
  if(rval == 1)
    UpdateAfterEdit();
  return rval;
}

void TypeDefault::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(active_membs, this);
}

void TypeDefault::SetActive(char* memb_nm, bool onoff) {
  MemberDef* md = FindMember(memb_nm);
  if(md != NULL)
    SetActive(md->idx, onoff);
}

bool TypeDefault::GetActive(char* memb_nm) {
  MemberDef* md = FindMember(memb_nm);
  if(md != NULL)
    return GetActive(md->idx);
  return false;
}

void TypeDefault::SetActive(int memb_no, bool onoff) {
  int lng_no = memb_no / (sizeof(long) * 8);
  int bit_no = memb_no % (sizeof(long) * 8);
  if(lng_no >= MAX_DFT_LONGS) {
    taMisc::Error("Number of members", String(memb_no), "exceeds member limit in",
		    GetTypeDef()->name, "of", String(256));
    return;
  }
  if(onoff)
    active[lng_no] |= 1 << bit_no;
  else
    active[lng_no] &= ~(1 << bit_no);
}

bool TypeDefault::GetActive(int memb_no) {
  int lng_no = memb_no / (sizeof(long) * 8);
  int bit_no = memb_no % (sizeof(long) * 8);
  if(lng_no >= MAX_DFT_LONGS) {
    taMisc::Error("Number of members", String(memb_no), "exceeds member limit in",
		    GetTypeDef()->name, "of", String(256));
    return false;
  }
  return active[lng_no] & (1 << bit_no) ? 1 : 0;
}

void TypeDefault::UpdateAfterEdit() {
  if(default_type == NULL)
    return;

  if((token == NULL) || (token->GetTypeDef() != default_type)) {

    if(old_type != NULL) {	// remove from previous typedef
      taBase_List* gp = old_type->defaults;
      if(gp != NULL) {
	taBase::Ref(this);	// make sure that the remove doesn't cause a delete..
	gp->Remove(this);
      }
    }

    if(token != NULL) {
      taBase::unRefDone(token);
    }
    int i;
    for(i=0; i< MAX_DFT_LONGS; i++)
      active[i] = 0;
    token = taBase::MakeToken(default_type);

    if(token != NULL) {		// make may have failed so here we check
//      taBase::Own(token, this); // maybe shouldn't own this here..
      taBase::Ref(token);
      token->SetName("Default_" + default_type->name);
      SetName(default_type->name);
      default_type->tokens.Remove(token); // don't count on list of tokens..
    }
    else {
      taMisc::Error("A default of", default_type->name," cannot be created",
		      "since that type does not generate tokens");
      default_type = &TA_taBase;
    }

    if(default_type->defaults == NULL) {
      default_type->defaults = new taBase_List;
      taBase::Ref(default_type->defaults);
      default_type->defaults->SetBaseType(&TA_TypeDefault);
    }
    taBase_List* gp = default_type->defaults;
    if(gp->Find(this) < 0) {	// add to the list
      gp->Link(this);
      taBase::unRef(this);	// don't inc the refcount, else it won't get killed..
      old_type = default_type;	// this is now the old type
    }
  }

  UpdateFromNameValue();
}

void TypeDefault::UpdateFromNameValue() {
  if((default_type == NULL) || (token == NULL))
    return;

  int i;
  for(i=0; i<active_membs.size; i++) {
    NameValue* nv = (NameValue*)active_membs.FastEl(i);
    MemberDef* md = default_type->members.FindName(nv->name);
    if(md != NULL) {
      SetActive(md->idx, true);
      md->type->SetValStr(nv->value, md->GetOff(token), token, md);
    }
  }
  token->UpdateAfterEdit();
}

void TypeDefault::UpdateToNameValue() {
  if(token == NULL)
    return;

  active_membs.RemoveAll();
  int i;
  for(i=0; i<default_type->members.size; i++) {
    if(GetActive(i)) {
      MemberDef* tmd = default_type->members.FastEl(i);
      NameValue* nv = (NameValue*)active_membs.New(1);
      nv->name = tmd->name;
      nv->value = tmd->type->GetValStr(tmd->GetOff(token), token, tmd);
    }
  }
}

void TypeDefault::SetTypeDefaults(TAPtr tok) {
  if((default_type == NULL) || (tok == NULL) || (token == NULL))
    return;

  if(!tok->InheritsFrom(default_type))
    return;

  int i;
  for(i=0; i<default_type->members.size; i++) {
    if(GetActive(i))
      tok->MemberCopyFrom(i, token);
  }
}

