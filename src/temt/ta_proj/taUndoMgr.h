// Copyright 2013-2017, Regents of the University of Colorado,
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

#ifndef taUndoMgr_h
#define taUndoMgr_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <UndoDiffThreadMgr>
#include <taUndoDiffSrc_List>
#include <taUndoRec_List>
#include <TimeUsedHR>

// declare all other types mentioned but not required to include:
class taBase; // 


taTypeDef_Of(taUndoMgr);

class TA_API taUndoMgr : public taOBase {
  // ##CAT_Undo undo manager -- handles the basic undo functionality
INHERITED(taOBase)
public:
  UndoDiffThreadMgr     diff_threads; // #NO_SAVE #HIDDEN threading support for computing diffs
  taUndoDiffSrc_List    undo_srcs;    // #TREE_SHOW #NO_SAVE diff source records
  taUndoRec_List        undo_recs;    // #TREE_SHOW #NO_SAVE the undo records
  int                   cur_undo_idx;   // #READ_ONLY #NO_SAVE logical index into undo record list where the next undo/redo will operate -- actually +1 relative to index to undo -- 0 = no more undos -- goes to the end for each SaveUndo, moves back/forward for Undo/Redo
  int                   undo_depth;     // #NO_SAVE how many undo's to keep around
  bool                  save_load_file; // #NO_SAVE save a copy of the file that is loaded during an undo or redo -- file name is "undo_load_file.txt" in cur directory -- useful for debugging issues
  taUndoRec*            rec_to_diff;    // #IGNORE for threading system, rec for diffing
  TimeUsedHR            undo_time_used; // #NO_SAVE undo time used recording
  TimeUsedHR            undo_diff_time_used; // #NO_SAVE copy of undo diff time used data

  void          Nest(bool nest);
  // call in pairs, to indicate nested contexts
  virtual bool  SaveUndo(taBase* mod_obj, const String& action, taBase* save_top = NULL,
                         bool force_proj_save = false, taBase* undo_save_owner = NULL);
  // save data for purposes of later being able to undo it -- takes a pointer to object that is being modified, a brief description of the action being performed (e.g., "Edit", "Cut", etc), and the top-level object below which current state information will be saved -- this must be *known to encapsulate all changes* that result from the modification, and also be sufficiently persistent so as to be around when undoing and redoing might be requested -- it defaults to the GetUndoBarrier object or, if that is NULL, the owner of this mgr, which is typically the project (unless force_proj_save is set, in which case it always uses the project).  undo_save_owner is the list/group object where some kind of structural modification is taking place (add, remove, move) that could affect other objects
  virtual void  PurgeUnusedSrcs();
  // remove any undo_srcs that are not currently being used

  virtual bool  Undo(int index = -1);
  // undo the most recent action
  virtual bool  Redo(int index = -1);
  // redo the most recent action that was undone

  virtual int   UndosAvail();
  // return the number of undo actions currently available
  virtual int   RedosAvail();
  // return the number of redo actions currently available

  virtual void  ReportStats(bool show_list = false, bool show_diffs = false);
  // #BUTTON #NO_SAVE_UNDO report (on cout) the current undo statistics in terms of # of records and total amount of ram taken, etc -- if show_list, show full list of current undo info, if show_diffs, then show full diffs of changes from orig source data (requires show_list too)

  virtual void  SaveCurSrcRec(const String& fname = "undo_cur_src_rec.proj");
  // #BUTTON #NO_SAVE_UNDO save current source record (full source of project last saved for undo) to given file name -- helpful for debugging what is being saved during undo saving..

  TA_SIMPLE_BASEFUNS(taUndoMgr);
protected:
  int                   nest_count; // #IGNORE -- +/- before after nesting contexts
  int                   loop_count; //
  virtual bool  LoadFromRec_impl(taUndoRec* urec);

private:
  void  Initialize();
  void  Destroy()       { CutLinks(); }
};

#endif // taUndoMgr_h
