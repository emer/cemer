// Co2018ght 2017-2017, Regents of the University of Colorado,
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

#ifndef ObjDiff_h
#define ObjDiff_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ObjDiffRec_List>
#include <taSmartRefT>

// declare all other types mentioned but not required to include:
class ObjDiffRec; //
class Patch; //
class int_Array; //
class int_PArray; //
class taMatrix; //
class taArray_base; //

taTypeDef_Of(ObjDiff);

class TA_API ObjDiff : public taNBase {
  // #CAT_ObjDiff master object diff object -- manages everything for object differencing
INHERITED(taNBase)
public:
  taBaseRef             a_top;  // top-level A object for comparison
  taBaseRef             b_top;  // top-level B object for comparison
  bool                  modify_a; // apply changes to A after generating patches?
  bool                  modify_b; // apply changes to B after generating patches?
  bool                  a_only;   // only allow changes to a-side -- for diffing against a fixed B record -- affects the diff browser and patch creation
  bool                  same_proj; // are a and b in the same project?  need to fix paths
  String                a_path_names; // path to a in names
  String                a_path_idx; // path to a in index
  String                b_path_names; // path to b in names
  String                b_path_idx; // path to b in index
  
  
  ObjDiffRec_List       diffs;
  // #NO_SAVE diff records, in a parallel, side-by-side format

  virtual int   Diff(taBase* obj_a, taBase* obj_b);
  // top-level entry point: perform a diff on two objects, returns number of differences

  virtual bool  DisplayDialog(bool modal_dlg);
  // display a dialog showing the differences -- returns true if user said Ok to apply changes -- if non-modal, then dialog manages everything and does the diffs itself

  virtual int   GeneratePatches();
  // generate Patch records to implement selected actions in these diffs -- if modify_a, b flags are set, then patches are immediately applied -- otherwise they are just saved in the Project patches group -- returns number of patch records generated

  //////////////////////////////////////////////
  //    IMPL below

  virtual int  DiffObjs(ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj);
  // diff two objects and everything below them -- if no diffs below, we don't do anything
  
  virtual int  DiffLists(ObjDiffRec* par_rec, taList_impl* list_a, taList_impl* list_b);
  // generate diffs based on two lists
  virtual void AddListContext_Before(ObjDiffRec* par_rec, taList_impl* list_a, int a_idx, taList_impl* list_b, int b_idx, int_Array& a_ok, int_Array& b_ok, int chunk);
    // context items before list diffs
  virtual void AddListContext_After(ObjDiffRec* par_rec, taList_impl* list_a, int a_idx, taList_impl* list_b, int b_idx, int_Array& a_ok, int_Array& b_ok, int chunk);
    // context items before list diffs
  
  virtual int  DiffMembers(ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj);
  // generate diffs for members of given objects, EXCLUDING any lists
  
  virtual int  DiffMember(ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj, MemberDef* md);
  // generate diffs for given member

  virtual int  DiffMemberLists(ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj);
  // generate diffs for any members that are lists

  virtual int  DiffMatrix(ObjDiffRec* par_rec, taMatrix* a_mat, taMatrix* b_mat);
  // diff two matricies -- only looks at geometry and size if not direct target of overall diff (or DataTable, DataCol)

  virtual int  DiffArray(ObjDiffRec* par_rec, taArray_base* a_ary, taArray_base* b_ary);
  // diff two arrays -- only looks at  size if not direct target of overall diff 
  
  virtual ObjDiffRec* NewParRec(ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj,
                                MemberDef* md = NULL);
  // new parent record -- for context
  
  virtual ObjDiffRec* NewListDiff
    (ObjDiffRec* par_rec, int flags, taList_impl* list_a, int a_idx,
     taList_impl* list_b, int b_idx, int chunk);
  // new diff record of objects on a list
  
  virtual ObjDiffRec* NewMatrixDiff
    (ObjDiffRec* par_rec, int flags, taMatrix* a_mat, int a_idx, taMatrix* b_mat, int b_idx);
  // new diff record on matrix values -- per frame..
  
  virtual ObjDiffRec* NewArrayDiff
    (ObjDiffRec* par_rec, int flags, taArray_base* a_mat, int a_idx, taArray_base* b_mat, int b_idx);
  // new diff record on array values
  
  virtual ObjDiffRec* NewListContext
    (ObjDiffRec* par_rec, int flags, taList_impl* list_a, int a_idx,
     taList_impl* list_b, int b_idx, int_Array& a_ok,
     int_Array& b_ok, int a_off, int b_off, int chunk);

  virtual int DiffMemberStrings
    (ObjDiffRec* par_rec, taBase* a_obj, taBase* b_obj, MemberDef* md,
     const String& a_val, const String& b_val);
  // if string values differ, create a new MEMBER diff record

  virtual ObjDiffRec* NewRec
    (ObjDiffRec* par_rec, int flags, int a_idx, int b_idx, taBase* a_obj, taBase* b_obj);
  // get a new diff record and populate it with basic info
  
  virtual void RollBack(int rollback);
  // roll-back all records added up to given index -- undo provisional add of parents if no diffs actually generated

  virtual void  HashList(taList_impl* list, int_PArray& array);
  // generate hash codes of type & name of objs on list

  virtual void  HashMatrix(taMatrix* mat, int_PArray& array);
  // generate hash codes of matrix -- does a frame (row in data table) at a time

  virtual void  HashArray(taArray_base* mat, int_PArray& array);
  // generate hash codes of matrix -- does a frame (row in data table) at a time

  virtual String PtrPath(TypeDef* td, void* addr, taBase* top);
  // get pointer path at given address of given type, relative to given top if possible

  void FastIdxRemove(int_Array& ary, int idx);
  // fast remove of index from array -- starts at idx and moves backward
  int FastIdxFind(int_Array& ary, int idx);
  // fast find of index from array -- starts at idx and moves backward

  virtual void SetCurSubgp(taBase* obj, taProject* proj);
  virtual void ReorderProjSubgps(Patch* pat);
  virtual void NamePatch(Patch* patch, taBase* from_obj, taBase* to_obj);

  virtual bool FixObjPaths(String& val, bool a_or_b);
  // fix the paths in dump file value val for within-project diffs
  
  virtual void GenPatch_CopyAB(ObjDiffRec* rec, Patch* patch);
  virtual void GenPatch_CopyBA(ObjDiffRec* rec, Patch* patch);
  virtual void GenPatch_DelA(ObjDiffRec* rec, Patch* patch);
  virtual void GenPatch_DelB(ObjDiffRec* rec, Patch* patch);
  virtual void GenPatch_AddA(ObjDiffRec* rec, Patch* patch, ObjDiffRec* prv_rec);
  virtual void GenPatch_AddB(ObjDiffRec* rec, Patch* patch, ObjDiffRec* prv_rec);
  
  TA_SIMPLE_BASEFUNS(ObjDiff);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ObjDiff_h
