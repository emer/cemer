// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.


#include "pdp_base.h"

#include "ta_filer.h"

#ifdef TA_GUI
#include "ta_qt.h"
#endif

#include "pdp_project.h"
#include "ta_filer.h"

#include <memory.h>
#include <malloc.h>
#include <sstream>

// for saving a recovery file if program exception occurs
// TODO: wrap in an exception handler itself
void pdpMisc::SaveRecoverFile(int err) {
  try {
#ifdef TA_GUI
    taiMisc::Cleanup(err);	// cleanup stuff in tai
#endif
    cerr << "PDP++ saving project file(s) from exception.\n";

    String proj_sfx = ".proj";	proj_sfx += taMisc::compress_sfx;

    String home_dir = getenv("HOME"); // home directory if curent dir does not work.
    taFiler* gf = taFiler::New();		// use a getfile for compressed writes..
    taRefN::Ref(gf);
    taProject* p;
    int cnt = 0;
    for (int i = 0; i < pdpMisc::root->projects.size; ++i) {
      p = root->projects.FastEl(i);
      String prfx = "PDP++Recover.";
      cnt = get_unique_file_number(cnt, prfx, proj_sfx);
      gf->setFileName(prfx + String(cnt) + proj_sfx);
      ostream* strm = gf->open_write();
      if (((strm == NULL) || strm->bad()) && !home_dir.empty()) {
	      // try it with the home diretory
	      cerr << "Error saving in current directory, trying home directory";
	      gf->Close();
	      gf->setFileName(home_dir + "/" + gf->fname());
	      strm = gf->open_write();
      }
      if((strm == NULL) || strm->bad())
	      cerr << "SaveRecoverFile: could not open file: " << gf->fileName() << "\n";
      else
	      p->Save_strm(*strm);
      gf->Close();
      cnt++;
    }

    if (gf) taRefN::unRefDone(gf);
  } catch (...) {
    cerr << "PDP++ unable to save recover file...sorry\n";
    exit(err);
  }
}

