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

#include "taUndoDiffSrc.h"
#include <taUndoRec>
#include <taSigLinkItr>

#include <taMisc>


void taUndoDiffSrc::Initialize() {
  last_diff_n = 0;
  last_diff_pct = 0.0f;
}

void taUndoDiffSrc::InitFmRec(taUndoRec* urec) {
  mod_time = urec->mod_time;
  save_top = urec->save_top;
  save_top_path = urec->save_top_path;
  save_data = urec->save_data;
  diff.Reset();                 // just in case..
}

void taUndoDiffSrc::EncodeDiff(taUndoRec* rec) {
  if(diff.data_a.line_st.size > 0) { // already done
    diff.ReDiffB(save_data, rec->save_data, false, false, false); // trim, no ignore case
  }
  else {                            // first time
    diff.DiffStrings(save_data, rec->save_data, false, false, false); // trim, no ignore case
  }
  diff.GetEdits(rec->diff_edits);       // save to guy
  last_diff_n = diff.GetLinesChanged(); // counts up total lines changed in diffs
  last_diff_pct = (.5f * (float)last_diff_n) / (float)diff.data_a.lines;
  // lines changed tends to double-count..
  taMisc::LogInfo("last_diff_n: ", String(last_diff_n), " pct: ", String(last_diff_pct));
  // now nuke rec's saved data!!
  rec->save_data = _nilString;
  // if need to debug, turn this off and turn on comparison below..
}

int taUndoDiffSrc::UseCount() {
  taSigLink* dl = data_link();
  if(!dl) return 0;
  int cnt = 0;
  taSigLinkItr itr;
  taSmartRef* el;
  FOR_DLC_EL_OF_TYPE(taSmartRef, el, dl, itr) {
    taBase* spo = el->GetOwner();
    if(!spo) continue;
    cnt++;
  }
  return cnt;
}
