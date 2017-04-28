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

#ifndef ObjDiff_h
#define ObjDiff_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ObjDiffRec_List>
#include <FlatTreeEl_List>
#include <taSmartRefT>
#include <voidptr_Array>

// declare all other types mentioned but not required to include:
class ObjDiffRec; //
class Patch; //

taTypeDef_Of(ObjDiff);

class TA_API ObjDiff : public taNBase {
  // #CAT_ObjDiff master object diff object -- manages everything for object differencing
INHERITED(taNBase)
public:
  taBaseRef             a_top;  // top-level A object for comparison
  taBaseRef             b_top;  // top-level B object for comparison
  FlatTreeEl_List       a_tree; // #NO_SAVE flattened object hierarchy for A -- source of diffs
  FlatTreeEl_List       b_tree; // #NO_SAVE flattened object hierarchy for B -- source of diffs
  
  ObjDiffRec_List       diffs;
  // #NO_SAVE diff records, in a parallel, side-by-side format
  voidptr_Array         nest_pars;
  // #IGNORE parent records for nesting -- last parent at each level of nesting

  virtual int  Diff(taBase* obj_a, taBase* obj_b);
  // perform a diff on two objects, returns number of differences

  virtual bool  DisplayDialog(bool modal_dlg);
  // display a dialog showing the differences -- returns true if user said Ok to apply changes -- if non-modal, then dialog manages everything and does the diffs itself

  virtual bool  DoDiffEdits(Patch* patch_a, Patch* patch_b);
  // perform edits as selected in the diff records -- if patch objects are passed, then patches are generated instead of actually doing the edits
  
  virtual ObjDiffRec* NewRec(int idx, int flags, int a_idx, int b_idx);
  // #IGNORE get a new diff record
  
  virtual ObjDiffRec* DiffAddParents(int a_idx, int b_idx);
  // #IGNORE add all parents to diff record up to point of a and b indexes, relative to last items in diff -- returns the current parent for new record to be added

  TA_SIMPLE_BASEFUNS(ObjDiff);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ObjDiff_h
