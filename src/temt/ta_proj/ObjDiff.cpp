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

#include "ObjDiff.h"

TA_BASEFUNS_CTORS_DEFN(ObjDiff);

#include <ObjDiffRec>
#include <int_PArray>
#include <taStringDiff>
#include <iDialogObjDiffBrowser>

#include <taMisc>
#include <taiMisc>

void ObjDiff::Initialize() {
}


int ObjDiff::Diff(taBase* obj_a, taBase* obj_b) {
  diffs.Reset();                      // clear out my list of diffs

  a_top = obj_a;
  b_top = obj_b;

  // first get flat reps of the trees
  a_tree.FlatTreeOf(a_top);
  b_tree.FlatTreeOf(b_top);

  taStringDiff str_diff;

  a_tree.HashToIntArray(str_diff.data_a.data);
  b_tree.HashToIntArray(str_diff.data_b.data);

  str_diff.Diff_impl("", "");       // null strings, use int hash's

  nest_pars.Reset();
  for(int i=0;i<str_diff.diffs.size;i++) {
    taStringDiffItem& df = str_diff.diffs[i];

    if(df.delete_a == df.insert_b) {
      for(int l=1; l<df.delete_a; l++) {
        ObjDiffRec* cur_par = DiffAddParents(df.start_a+l, df.start_b+l);
        ObjDiffRec* rec = NewRec(i, ObjDiffRec::A_B_DIFF, df.start_a+l, df.start_b+l);
        rec->par_rec = cur_par;
        rec->nest_level = cur_par->nest_level + 1;
      }
    }
    else {
      if(df.delete_a > 0) {     // a records exist, b do not..
        for(int l=0; l<df.delete_a; l++) {
          ObjDiffRec* cur_par = DiffAddParents(df.start_a+l, df.start_b);
          ObjDiffRec* rec = NewRec(i, ObjDiffRec::A_NOT_B, df.start_a + l, df.start_b);
          rec->par_rec = cur_par;
          rec->nest_level = cur_par->nest_level + 1;
        }
      }
      if(df.insert_b > 0) {     // b records exist, a do not..
        for(int l=0; l<df.insert_b; l++) {
          ObjDiffRec* cur_par = DiffAddParents(df.start_a, df.start_b+1);
          ObjDiffRec* rec = NewRec(i, ObjDiffRec::B_NOT_A, df.start_a, df.start_b + l);
          rec->par_rec = cur_par;
          rec->nest_level = cur_par->nest_level + 1;
        }
      }
    }
  }
  nest_pars.Reset();
  return 0;                     // todo get size
}

ObjDiffRec* ObjDiff::NewRec(int idx, int flags, int a_idx, int b_idx) {
  ObjDiffRec* rec = (ObjDiffRec*)diffs.New(1);
  rec->flags = (ObjDiffRec::DiffFlags)flags;
  rec->diff_no = idx;
  rec->a_idx = a_idx;
  rec->b_idx = b_idx;
  rec->a_src = a_tree.SafeEl(a_idx);
  rec->b_src = b_tree.SafeEl(b_idx);
  return rec;
}

bool ObjDiff::DisplayDialog(bool modal_dlg) {
  iDialogObjDiffBrowser* odb = iDialogObjDiffBrowser::New(this, taiMisc::defFontSize);
  bool rval = odb->Browse(modal_dlg);
  // if !modal, browser now owns the diffs list and will delete it when it dies -- and it will
  // be responsible for performing any actions that get done..
  return rval;
}

ObjDiffRec* ObjDiff::DiffAddParents(int a_idx, int b_idx) {
  FlatTreeEl* a_src = a_tree.SafeEl(a_idx);
  FlatTreeEl* b_src = b_tree.SafeEl(b_idx);
  if(!a_src || !b_src) {
    return NULL;  // shouldn't happen
  }

  if(nest_pars.size > 0) {
    ObjDiffRec* cur_par = (ObjDiffRec*)nest_pars.Peek();
    FlatTreeEl* a_par = a_src->parent_el;
    FlatTreeEl* b_par = b_src->parent_el;
    if(cur_par->a_src == a_par && cur_par->b_src == b_par) {
      return cur_par;           // current one is good
    }
  }
  
  FlatTreeEl_List a_stack;
  FlatTreeEl_List b_stack;
  ObjDiffRec* found_par = NULL;
  int par_level = 0;
  while(true) {
    FlatTreeEl* a_par = a_src->parent_el;
    FlatTreeEl* b_par = b_src->parent_el;
    if(!a_par || !b_par)        // top!
      break;
    // find a parent on stack that already has parent records -- if found, stop there
    for(int pi=nest_pars.size-1; pi>=0; pi--) {
      ObjDiffRec* prec = (ObjDiffRec*)nest_pars[pi];
      if(prec->a_src == a_par && prec->b_src == b_par) {
        found_par = prec;
        par_level = pi;
        break;
      }
    }
    a_stack.Link(a_par);
    b_stack.Link(b_par);
    a_src = a_par;
    b_src = b_par;
  }
  // now we have a list of parents in the a, b stacks, and a possible parent record
  // add in *descending* order from current level
  ObjDiffRec* cur_par = found_par;
  int new_pars = a_stack.size;  // same as b
  int max_lev = par_level + a_stack.size;
  nest_pars.SetSize(max_lev + 1);
  for(int i=0; i<new_pars; i++) {
    // take from the top first
    FlatTreeEl* a_par = a_stack.FastEl(new_pars - 1 - i);
    FlatTreeEl* b_par = b_stack.FastEl(new_pars - 1 - i);
    ObjDiffRec* rec = NewRec(-1, ObjDiffRec::DIFF_PAR, a_par->idx, b_par->idx);
    rec->par_rec = cur_par;
    int lev = par_level + i;
    rec->nest_level = lev;
    nest_pars[lev] = rec;
    cur_par = rec;              // new cur
  }
  return cur_par;
}


// bool ObjDiff::DiffFlagParents(ObjDiffRec* rec) {
//   if(rec->nest_level == 0) return false;

//   voidptr_PArray* nestpars;
//   if(rec->HasDiffFlag(ObjDiffRec::SRC_A))
//     nestpars = &nest_a_pars;
//   else
//     nestpars = &nest_b_pars;

//   int rec_nest = rec->nest_level;
//   nestpars->SetSize(rec_nest+1);

//   int par_nest = rec_nest;
//   ObjDiffRec* par_rec = rec;
//   while(par_rec->par_odr != (ObjDiffRec*)nestpars->FastEl(par_nest-1)) {
//     nestpars->FastEl(par_nest-1) = (void*)par_rec->par_odr;
//     par_rec = par_rec->par_odr;
//     par_nest = par_rec->nest_level;
//     if(par_nest == 0) break;
//   }

//   bool rval = false;
//   for(int i=0;i<rec_nest;i++) {
//     par_rec = (ObjDiffRec*)nestpars->FastEl(i);
//     if(par_rec != rec) {
//       par_rec->SetDiffFlag(ObjDiffRec::DIFF_PAR);
//       par_rec->n_diffs++;
//       if(par_rec->diff_no_start < 0)
//         par_rec->diff_no_start = rec->diff_no;
//       else
//         par_rec->diff_no_start = MIN(rec->diff_no, par_rec->diff_no_start);
//       par_rec->diff_no_end = MAX(rec->diff_no, par_rec->diff_no_end);
//       rval = true;              // got one..
//     }
//   }

//   nestpars->FastEl(rec_nest) = (void*)rec; // always add current as potential next parent
//   return rval;
// }


bool ObjDiff::DoDiffEdits(Patch* patch_a, Patch* patch_b) {
  return true;
}

/*
static void DoDiffEdits_SetRelPath(taBase* par_obj, taObjDiffRec* srec, taObjDiffRec* drec,
                                   Patch* patch) {
  MemberDef* md;
  taBase* new_guy = par_obj->FindFromPath(srec->value, md);
  if(patch) {
    String str_val = "NULL";
    if(new_guy)
      str_val = new_guy->GetPathFromProj();
    PatchRec* pr = patch->NewPatchRec_Assign(par_obj, str_val);
    if(drec->mdef) {
      pr->mbr_path = drec->mdef->name;
    }
    return;
  }
  if(drec->type->IsPointer() && drec->type->IsTaBase()) {
    if(drec->mdef && drec->mdef->HasOption("OWN_POINTER")) {
      if(!drec->par_addr)
        taMisc::Warning("*** NULL parent for owned pointer:",drec->GetDisplayName());
      else
        taBase::OwnPointer((taBase**)drec->addr, new_guy, (taBase*)drec->par_addr);
    }
    else {
      if(drec->mdef && drec->mdef->HasOption("NO_SET_POINTER"))
        (*(taBase**)drec->addr) = new_guy;
      else
        taBase::SetPointer((taBase**)drec->addr, new_guy);
    }
  }
  else if(drec->type->InheritsFrom(TA_taSmartRef)) {
    ((taSmartRef*)drec->addr)->set(new_guy);
  }
  else if(drec->type->InheritsFrom(TA_taSmartPtr)) {
    ((taSmartPtr*)drec->addr)->set(new_guy);
  }
}

bool ObjDiff::DoDiffEdits(Patch* patch_a, Patch* patch_b) {
  StructUpdate(true);

  taProject* proj_a = (taProject*)diffs.tab_obj_a->GetOwner(&TA_taProject);
  taProject* proj_b = (taProject*)diffs.tab_obj_b->GetOwner(&TA_taProject);

  String save_str;              // string for saving files to
  
  //  for(int i=diffs.size-1; i>= 0; i--) { // go backwards to minimize knock-on effects
  for(int i=0; i < diffs.size; i++) { // forwards usually better actually
    taObjDiffRec* rec = diffs[i];
    if(!rec->HasDiffFlag(taObjDiffRec::ACT_MASK)) continue;
    if(!rec->addr || !rec->type) continue; // sanity checks..

    // NOTE: rec is A, rec->diff_odr is B
    
    bool ta_bases = false;
    bool tab_diff_typ = false;
    bool taptr = false;
    taBase* tab_a = NULL;
    taBase* tab_b = NULL;;
    // make sure pointers are still current
    if(rec->type->IsActualTaBase()) {
      if(rec->tabref) {
        if(!((taBaseRef*)rec->tabref)->ptr())
          continue;
        tab_a = (taBase*)rec->addr;
      }
      if(rec->diff_odr) {
        if(rec->diff_odr->tabref) {
          if(!((taBaseRef*)rec->diff_odr->tabref)->ptr())
            continue;
          tab_b = (taBase*)rec->diff_odr->addr;
        }
      }
      if(tab_a && tab_b)
        ta_bases = true;
      if((!rec->mdef && (rec->type != rec->diff_odr->type))) {
        if(!ta_bases) continue; // no can do
        tab_diff_typ = true;
      }
    }
    else if(((rec->type->IsPointer()) && rec->type->IsTaBase()) ||
            rec->type->InheritsFrom(TA_taSmartRef) ||
            rec->type->InheritsFrom(TA_taSmartPtr)) {
      taptr = true;
    }

    taBase* tab_par_a = NULL;
    taBase* tab_par_b = NULL;
    if(rec->par_type && rec->par_type->IsActualTaBase()) {
      // make sure *parent* pointer is still current
      if(rec->par_odr && rec->par_odr->tabref) {
        if(!((taBaseRef*)rec->par_odr->tabref)->ptr())
          continue;
        tab_par_a = (taBase*)rec->par_addr;
      }
    }
    if(rec->diff_odr && rec->diff_odr->par_type &&
       rec->diff_odr->par_type->IsActualTaBase()) {
      // make sure *parent* pointer is still current
      if(rec->diff_odr->par_odr && rec->diff_odr->par_odr->tabref) {
        if(!((taBaseRef*)rec->diff_odr->par_odr->tabref)->ptr())
          continue;
        tab_par_b = (taBase*)rec->diff_odr->par_addr;
      }
    }

    String tab_a_path;
    String tab_b_path;
    if(tab_par_a) tab_a_path = tab_par_a->DisplayPath();
    if(tab_par_b) tab_b_path = tab_par_b->DisplayPath();

    //////////////////////////////////
    // Copy

    if(rec->HasDiffFlag(taObjDiffRec::ACT_COPY_BA)) {
      taMisc::Info("Copying B -> A:", tab_b_path, "->", tab_a_path, "\n",
                   rec->GetDisplayName(), "=", rec->diff_odr->value);
      if(tab_diff_typ) {
        // need to replace old guy with new one
        if(patch_a) {
          tab_b->Save_String(save_str);
          PatchRec* pr = patch_a->NewPatchRec_Replace(tab_a, save_str);
        }
        else {
          taBase* down = tab_a->GetOwner();
          if(down) {
            down->CopyChildBefore(tab_b, tab_a);
            tab_a->Close();       // nuke old guy
          }
        }
      }
      else {
        if(rec->diff_odr->HasDiffFlag(taObjDiffRec::VAL_PATH_REL)) {
          DoDiffEdits_SetRelPath(diffs.tab_obj_a, rec->diff_odr, rec, patch_a);
        }
        else if(taptr) {
          DoDiffEdits_SetRelPath(proj_a, rec->diff_odr, rec, patch_a);
        }
        else {
          if(ta_bases) {
            if(patch_a) {
              tab_b->Save_String(save_str);
              PatchRec* pr = patch_a->NewPatchRec_Assign(tab_a, save_str);
            }
            else {
              rec->type->CopyFromSameType(rec->addr, rec->diff_odr->addr);
              tab_a->SetName(tab_b->GetName()); // need to copy names too!
            }
          }
          else {
            if(patch_a) {
              if(rec->mdef) {
                String sval = rec->diff_odr->type->GetValStr
                  (rec->diff_odr->addr, tab_b, rec->diff_odr->mdef, TypeDef::SC_STREAMING, true);
                PatchRec* pr = patch_a->NewPatchRec_Assign(tab_par_a, sval);
                pr->mbr_path = rec->mdef->name;
              }
              else {
                taMisc::Info("non-taBase non-member copy -- no can do..");
                continue;
              }
            }
            else {
              rec->type->CopyFromSameType(rec->addr, rec->diff_odr->addr);
            }
          }
        }
        if(!patch_a && tab_par_a) {
          tab_par_a->MemberUpdateAfterEdit(rec->mdef);
          tab_par_a->UpdateAfterEdit();
        }
      }
      continue;
    }

    if(rec->HasDiffFlag(taObjDiffRec::ACT_COPY_AB)) {
      taMisc::Info("Copying A -> B:", tab_a_path, "->", tab_b_path, "\n",
                   rec->diff_odr->GetDisplayName(), "=", rec->value);
      if(tab_diff_typ) {
        // need to replace old guy with new one
        if(patch_b) {
          tab_a->Save_String(save_str);
          PatchRec* pr = patch_b->NewPatchRec_Replace(tab_b, save_str);
        }
        else {
          taBase* down = tab_b->GetOwner();
          if(down) {
            down->CopyChildBefore(tab_a, tab_b);
            tab_b->Close();       // nuke old guy
          }
        }
      }
      else {
        if(rec->HasDiffFlag(taObjDiffRec::VAL_PATH_REL)) {
          DoDiffEdits_SetRelPath(diffs.tab_obj_b, rec, rec->diff_odr, patch_b);
        }
        else if(taptr) {
          DoDiffEdits_SetRelPath(proj_b, rec, rec->diff_odr, patch_b);
        }
        else {
          if(ta_bases) {
            if(patch_b) {
              tab_a->Save_String(save_str);
              PatchRec* pr = patch_b->NewPatchRec_Assign(tab_b, save_str);
            }
            else {
              rec->diff_odr->type->CopyFromSameType(rec->diff_odr->addr, rec->addr);
              tab_b->SetName(tab_a->GetName()); // need to copy names too!
            }
          }
          else {
            if(patch_b) {
              if(rec->diff_odr->mdef) {
                String sval = rec->type->GetValStr
                  (rec->addr, tab_a, rec->mdef, TypeDef::SC_STREAMING, true);
                PatchRec* pr = patch_b->NewPatchRec_Assign(tab_par_b, sval);
                pr->mbr_path = rec->diff_odr->mdef->name;
              }
              else {
                taMisc::Info("non-taBase non-member copy -- no can do..");
                continue;
              }
            }
            else {
              rec->diff_odr->type->CopyFromSameType(rec->diff_odr->addr, rec->addr);
            }
          }
          if(!patch_b && tab_par_b) {
            tab_par_b->MemberUpdateAfterEdit(rec->diff_odr->mdef);
            tab_par_b->UpdateAfterEdit();
          }
        }
      }
      continue;
    }

    if(!ta_bases) continue;     // only ta bases from this point on!

    //////////////////////////////////
    //          Add
    bool add = false;
    bool added = false;
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_ADD_A)) { // do add before del..
      taMisc::Info("Adding A to B:", tab_a_path, "\n", rec->GetDisplayName());
      add = true;
      // if(rec->diff_odr->nest_level == rec->nest_level) {
      taBase* down = tab_b->GetOwner();
      if(down) {
        if(patch_b) {
          tab_a->Save_String(save_str);
          PatchRec* pr = patch_b->NewPatchRec_Insert
            (tab_b, down, save_str, tab_a->GetTypeDef()->name);
        }
        else {
          down->CopyChildBefore(tab_a, tab_b);
        }
        added = true;
      }
    }
    //   }
    //   else if(tab_par_b) {
    //     // todo: not sure any of this logic makes sense -- we said to put it in b so
    //     // why doesn't it just go in b!?
    //     if(rec->diff_odr->nest_level < rec->nest_level) {
    //       // for last obj in list, dest is now another member in parent obj..
    //       taMisc::Info("diff nest -- rec:", String(rec->nest_level), "diff:",
    //                    String(rec->diff_odr->nest_level),"rec path:", tab_a_path,
    //                    "diff path:", tab_b_path);
    //       if(rec->par_odr->mdef) {
    //         // find member in dest par (parents always ta base..)
    //         if(rec->diff_odr->par_odr->mdef &&
    //            rec->diff_odr->par_odr->mdef->name == rec->par_odr->mdef->name) {
    //           // parent is the guy!
    //           if(patch_b) {
    //             tab_a->Save_String(save_str);
    //             PatchRec* pr = patch_a->NewPatchRec_Insert
    //               (tab_par_b, tab_par_b, save_str, tab_a->GetTypeDef()->name);
    //           }
    //           else {
    //             tab_par_b->CopyChildBefore(tab_a, NULL); // NULL goes to end..
    //           }
    //           added = true;
    //         }
    //         else {
    //           MemberDef* dmd;
    //           void* mbase = tab_par_b->FindMembeR(rec->par_odr->mdef->name, dmd);
    //           if(dmd && dmd->type->IsActualTaBase()) { // it should!
    //             taBase* down = (taBase*)mbase;
    //             down->CopyChildBefore(tab_a, NULL); // NULL goes to end..
    //             added = true;
    //           }
    //         }
    //       }
    //       else { // go one level higher
    //         taObjDiffRec* parpar_a = rec->par_odr->par_odr;
    //         if(parpar_a->mdef) {
    //           taBase* tabparpar_b = tab_par_b->GetOwner();
    //           // find member in dest par (parents always ta base..)
    //           MemberDef* dmd;
    //           void* mbase = tabparpar_b->FindMembeR(parpar_a->mdef->name, dmd);
    //           if(dmd && dmd->type->IsActualTaBase()) { // it should!
    //             taBase* down = (taBase*)mbase;
    //             down->CopyChildBefore(tab_a, NULL); // NULL goes to end..
    //             added = true;
    //           }
    //         }
    //       }
    //     }
    //     else if(rec->diff_odr->nest_level > rec->nest_level) {
    //       taBase* down = tab_par_b;
    //       for(int k=0;k < rec->diff_odr->nest_level - rec->nest_level;k++) {
    //         down = down->GetOwner();
    //         if(!down) break;
    //       }
    //       if(down) {
    //         down->CopyChildBefore(tab_a, NULL); // no ref info avail -- probably end
    //         added = true;
    //       }
    //     }
    //   }
    // }
    
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_ADD_B)) { // do add before del..
      taMisc::Info("Adding B to A:", tab_b_path, "\n", rec->GetDisplayName());
      add = true;
      // if(rec->diff_odr->nest_level == rec->nest_level) {
      taBase* down = tab_a->GetOwner();
      if(down) {
        if(patch_a) {
          tab_b->Save_String(save_str);
          PatchRec* pr = patch_a->NewPatchRec_Insert
            (tab_a, down, save_str, tab_b->GetTypeDef()->name);
        }
        else {
          down->CopyChildBefore(tab_b, tab_a);
        }
        added = true;
      }
    }

    if(add && !added) {
      taMisc::Warning("NOTE: Last Add was NOT successful!", rec->GetDisplayName());
    }

    //////////////////////////////////
    //          Del

    if(rec->tabref && !((taBaseRef*)rec->tabref)->ptr()) continue;
    // double-check

    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_DEL_A)) {
      taMisc::Info("Deleting A:", tab_a_path, "\n", rec->GetDisplayName());
      if(patch_a) {
        tab_a->Save_String(save_str);
        PatchRec* pr = patch_a->NewPatchRec_Delete(tab_a, save_str);
      }
      else {
        tab_a->Close();
      }
    }
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_DEL_B)) {
      taMisc::Info("Deleting B:", tab_b_path, "\n", rec->GetDisplayName());
      if(patch_b) {
        tab_b->Save_String(save_str);
        PatchRec* pr = patch_b->NewPatchRec_Delete(tab_b, save_str);
      }
      else {
        tab_b->Close();
      }
    }
  }
  StructUpdate(false);
  return true;
}

*/
