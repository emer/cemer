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

#include "taObjDiff_List.h"
#include <int_PArray>
#include <taStringDiff>
#include <TypeDef>
#include <taBase>
#include <taMisc>


void taObjDiff_List::Initialize() {
#ifndef NO_TA_BASE
  tab_obj_a = NULL;
  tab_obj_b = NULL;
#endif
  src_a = NULL;
  src_b = NULL;
}

taObjDiff_List::~taObjDiff_List() {
  Reset();
#ifndef NO_TA_BASE
  tab_obj_a = NULL;
  tab_obj_b = NULL;
#endif
  if(src_a) {
    delete src_a;
    src_a = NULL;
  }
  if(src_b) {
    delete src_b;
    src_b = NULL;
  }
}

String  taObjDiff_List::El_GetName_(void* it) const { return ((taObjDiffRec*)it)->name; }
taPtrList_impl*  taObjDiff_List::El_GetOwnerList_(void* it) const { return ((taObjDiffRec*)it)->owner; }
void*   taObjDiff_List::El_SetOwner_(void* it_) {
  if (!it_) return it_;
  taObjDiffRec* it = (taObjDiffRec*)it_;
  it->owner = this;
  return it_;

}
void    taObjDiff_List::El_SetIndex_(void* it, int i){ ((taObjDiffRec*)it)->idx = i; }

void*   taObjDiff_List::El_Ref_(void* it)         { taRefN::Ref((taObjDiffRec*)it); return it; }
void*   taObjDiff_List::El_unRef_(void* it)       { taRefN::unRef((taObjDiffRec*)it); return it; }
void    taObjDiff_List::El_Done_(void* it)        { taRefN::Done((taObjDiffRec*)it); }
void*   taObjDiff_List::El_MakeToken_(void* it)  { return (void*)((taObjDiffRec*)it)->MakeToken(); }
void*   taObjDiff_List::El_Copy_(void* trg, void* src)
{ ((taObjDiffRec*)trg)->Copy(*((taObjDiffRec*)src)); return trg; }

void taObjDiff_List::HashToIntArray(int_PArray& array) {
  for(int i=0; i<size; i++) {
    taObjDiffRec* rec = FastEl(i);
    array.Add(rec->hash_code);
  }
}

void taObjDiff_List::SetAllFlag(int flg) {
  for(int i=0; i<size; i++) {
    taObjDiffRec* rec = FastEl(i);
    rec->SetDiffFlag((taObjDiffRec::DiffFlags)flg);
  }
}

bool taObjDiff_List::DiffFlagParents(taObjDiffRec* rec) {
  if(rec->nest_level == 0) return false;

  voidptr_PArray* nestpars;
  if(rec->HasDiffFlag(taObjDiffRec::SRC_A))
    nestpars = &nest_a_pars;
  else
    nestpars = &nest_b_pars;

  int rec_nest = rec->nest_level;
  nestpars->SetSize(rec_nest+1);

  int par_nest = rec_nest;
  taObjDiffRec* par_rec = rec;
  while(par_rec->par_odr != (taObjDiffRec*)nestpars->FastEl(par_nest-1)) {
    nestpars->FastEl(par_nest-1) = (void*)par_rec->par_odr;
    par_rec = par_rec->par_odr;
    par_nest = par_rec->nest_level;
    if(par_nest == 0) break;
  }

  bool rval = false;
  for(int i=0;i<rec_nest;i++) {
    par_rec = (taObjDiffRec*)nestpars->FastEl(i);
    if(par_rec != rec) {
      par_rec->SetDiffFlag(taObjDiffRec::DIFF_PAR);
      par_rec->n_diffs++;
      if(par_rec->diff_no_start < 0)
        par_rec->diff_no_start = rec->diff_no;
      else
        par_rec->diff_no_start = MIN(rec->diff_no, par_rec->diff_no_start);
      par_rec->diff_no_end = MAX(rec->diff_no, par_rec->diff_no_end);
      rval = true;              // got one..
    }
  }

  nestpars->FastEl(rec_nest) = (void*)rec; // always add current as potential next parent
  return rval;
}

void taObjDiff_List::CreateSrcs() {
  Reset();                      // reset me for good measure too, just to free up any remaining junk
  if(src_a) {
    src_a->Reset();
  }
  else {
    src_a = new taObjDiff_List;
  }
  if(src_b) {
    src_b->Reset();
  }
  else {
    src_b = new taObjDiff_List;
  }
}

void taObjDiff_List::Diff() {
  if(!src_a || !src_b) {
    taMisc::Error("taObjDiff_List::Diff() -- source lists not created yet -- programmer error");
    return;
  }

  taStringDiff diff;

  Reset();                      // clear out my list of diffs
  src_a->HashToIntArray(diff.data_a.data);
  src_b->HashToIntArray(diff.data_b.data);

  src_a->SetAllFlag(taObjDiffRec::SRC_A);
  src_b->SetAllFlag(taObjDiffRec::SRC_B);

  diff.Diff_impl("", "");       // null strings, use int hash's

  nest_a_pars.Reset();
  nest_b_pars.Reset();
  for(int i=0;i<diff.diffs.size;i++) {
    taStringDiffItem& df = diff.diffs[i];

    if(df.delete_a == df.insert_b) {
      for(int l=0; l<df.delete_a; l++) {
        taObjDiffRec* rec_a = src_a->SafeEl(df.start_a + l);
        rec_a->SetDiffFlag(taObjDiffRec::DIFF_CHG);
        rec_a->diff_no = i;
        DiffFlagParents(rec_a);
        Link(rec_a);            // link all the actual diff items to main list
        taObjDiffRec* rec_b = src_b->SafeEl(df.start_b + l);
        rec_b->SetDiffFlag(taObjDiffRec::DIFF_CHG);
        rec_b->diff_no = i;
        DiffFlagParents(rec_b);
        rec_a->diff_odr = rec_b; // this is the paired guy
        rec_b->diff_odr = rec_a; // bidir pairing: why not..
#ifndef NO_TA_BASE
        if(rec_a->type->InheritsFrom(&TA_taBase)) { // must have owner to do diff actd
          if(!((taBase*)rec_a->addr)->GetOwner())
            rec_a->SetDiffFlag(taObjDiffRec::SUB_NO_ACT);
        }
        if(rec_b->type->InheritsFrom(&TA_taBase)) { // must have owner to do diff actd
          if(!((taBase*)rec_b->addr)->GetOwner())
            rec_b->SetDiffFlag(taObjDiffRec::SUB_NO_ACT);
        }
#endif
      }
    }
    else {
      if(df.delete_a > 0) {
        taObjDiffRec* rec_b = src_b->SafeEl(df.start_b);
        if(!rec_b) {
          rec_b = src_b->Peek();
        }
        // find a ta_base!
#ifndef NO_TA_BASE
        rec_b = rec_b->GetOwnTaBaseRec();
#endif
        rec_b->SetDiffFlag(taObjDiffRec::DIFF_DEL);
        rec_b->diff_no = i;
        DiffFlagParents(rec_b);

        taObjDiffRec* rec0 = src_a->SafeEl(df.start_a);
        int del_nest = rec0->nest_level;
        for(int l=0; l<df.delete_a; l++) {
          taObjDiffRec* rec_a = src_a->SafeEl(df.start_a + l);
          rec_a->SetDiffFlag(taObjDiffRec::DIFF_DEL);
          rec_a->diff_no = i;
          if(DiffFlagParents(rec_a)) // reset nest level if parents needed to be added
            del_nest = rec_a->nest_level;
          Link(rec_a);
          rec_a->diff_odr = rec_b; // starting point in b..
          if(rec_a->mdef) {     // never actionable
            rec_a->SetDiffFlag(taObjDiffRec::SUB_NO_ACT);
          }
          else {
            if(rec_a->nest_level > del_nest) {
              rec_a->SetDiffFlag(taObjDiffRec::SUB_NO_ACT);
            }
#ifndef NO_TA_BASE
            if(rec_a->type->InheritsFrom(&TA_taBase)) { // must have owner to do diff actd
              if(!((taBase*)rec_a->addr)->GetOwner())
                rec_a->SetDiffFlag(taObjDiffRec::SUB_NO_ACT);
            }
            else {
              // and cannot be a non-ta-base for add/del
              rec_a->SetDiffFlag(taObjDiffRec::SUB_NO_ACT);
            }
#endif
          }
        }
      }
      if(df.insert_b > 0) {
        taObjDiffRec* rec_a = src_a->SafeEl(df.start_a);
        if(!rec_a) {
          rec_a = src_a->Peek();
        }
#ifndef NO_TA_BASE
        rec_a = rec_a->GetOwnTaBaseRec();
#endif
        rec_a->SetDiffFlag(taObjDiffRec::DIFF_ADD);
        rec_a->diff_no = i;
        DiffFlagParents(rec_a);

        taObjDiffRec* rec0 = src_b->SafeEl(df.start_b);
        int add_nest = rec0->nest_level;
        for(int l=0; l<df.insert_b; l++) {
          taObjDiffRec* rec_b = src_b->SafeEl(df.start_b + l);
          rec_b->SetDiffFlag(taObjDiffRec::DIFF_ADD);
          rec_b->diff_no = i;
          if(DiffFlagParents(rec_b))
            add_nest = rec_b->nest_level;
          Link(rec_b);
          rec_b->diff_odr = rec_a; // starting point in a..
          if(rec_b->mdef) {     // never actionable
            rec_b->SetDiffFlag(taObjDiffRec::SUB_NO_ACT);
          }
          else {
            if(rec_b->nest_level > add_nest) {
              rec_b->SetDiffFlag(taObjDiffRec::SUB_NO_ACT);
            }
#ifndef NO_TA_BASE
            if(rec_b->type->InheritsFrom(&TA_taBase)) { // must have owner to do diff actd
              if(!((taBase*)rec_b->addr)->GetOwner())
                rec_b->SetDiffFlag(taObjDiffRec::SUB_NO_ACT);
            }
            else {
              // and cannot be a non-ta-base for add/del
              rec_b->SetDiffFlag(taObjDiffRec::SUB_NO_ACT);
            }
#endif
          }
        }
      }
    }
  }

  // String strm;
  // Print(strm);
  // taMisc::Info(strm);

  // this is not good: much better to just elide in display
  // DiffPurgeNoDiffs(src_a);
  // DiffPurgeNoDiffs(src_b);

  nest_a_pars.Reset();
  nest_b_pars.Reset();
}

int taObjDiff_List::DiffPurgeNoDiffs(taObjDiff_List* src) {
  int n_purge = 0;
  for(int i=src->size-1; i>=0; i--) {
    taObjDiffRec* rec = src->FastEl(i);
    if(rec->n_diffs == 0) {     // easy -- lets see what that does..
      src->RemoveIdx(i);
      n_purge++;
      continue;
    }
  }
  return n_purge;
}
