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

#ifndef taUndoRec_h
#define taUndoRec_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <taDateTime>
#include <taBaseRef>
#include <taUndoDiffSrcRef>
#include <taStringDiffEdits>

// declare all other types mentioned but not required to include:


class TA_API taUndoRec : public taOBase {
  // ##CAT_Undo one undo record -- saves all necessary state information
INHERITED(taOBase)
public:
  String        mod_obj_path;   // path to the object that was just about to be modified, after this record was saved -- relative to owner of taUndoMgr (typically the project)
  String        mod_obj_name;   // name of modified object
  String        action;         // a brief description of the action performed
  taDateTime    mod_time;       // time (to seconds level of resolution) when obj was modified
  taBaseRef     save_top;       // top-level object under which the data was saved
  String        save_top_path;  // path to the save_top -- in case it disappears -- again relative to mgr owner
  String        save_data;      // dump-file save from the save_top object (save_top->Save_String)
  taUndoDiffSrcRef diff_src;    // if this is non-null, use it as the source for saving a diff against
  taStringDiffEdits diff_edits; // edit list for reconstructing original data from diff against diff_src

  void          EncodeMyDiff();

  String        GetData();      // get the data for this save, either by diff or straight data

  TA_SIMPLE_BASEFUNS(taUndoRec);
private:
  void  Initialize();
  void  Destroy()       { CutLinks(); }
};

#endif // taUndoRec_h
