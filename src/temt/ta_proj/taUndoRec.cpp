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

#include "taUndoRec.h"

TA_BASEFUNS_CTORS_DEFN(taUndoRec);

void taUndoRec::Initialize() {
}

String taUndoRec::GetData() {
  if(save_data.nonempty()) {
    return save_data;           // easy
  }
  String rval = diff_edits.GenerateB(diff_src->save_data); // generate against A
#if 0
  // this is no longer enabled because we are nuking the save_data for all cases..
  int oops = compare(rval, save_data); // double check!
  if(oops > 0) {
    fstream ostrm;
    ostrm.open("rec_regen.txt", ios::out);
    rval.Save_str(ostrm);
    ostrm.close();

    ostrm.open("rec_save_data.txt", ios::out);
    save_data.Save_str(ostrm);
    ostrm.close();

    taMisc::Error("taUndoRec GetData() did not recover original data -- n diffs:",
                  String(oops), "see rec_save_data.txt and rec_regen.txt for orig texts");
  }
#endif
  return rval;
}

void taUndoRec::EncodeMyDiff() {
  diff_src->EncodeDiff(this);
}

