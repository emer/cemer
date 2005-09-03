#include "pdpbase.h"

#include "ta_filer.h"

#ifdef TA_GUI
#include "ta_qt.h"
#endif

#include "pdpshell.h"

#include <signal.h>
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
    taFiler* gf = taFiler_CreateInstance();		// use a getfile for compressed writes..
    taRefN::Ref(gf);
    Project* p;
    int cnt = 0;
    for (int i = 0; i < pdpMisc::root->projects.size; ++i) {
      p = root->projects.FastEl(i);
      String prfx = "PDP++Recover.";
      cnt = get_unique_file_number(cnt, prfx, proj_sfx);
      gf->fname = prfx + String(cnt) + proj_sfx;
      ostream* strm = gf->open_write();
      if (((strm == NULL) || strm->bad()) && !home_dir.empty()) {
	      // try it with the home diretory
	      cerr << "Error saving in current directory, trying home directory";
	      gf->Close();
	      gf->fname = home_dir + "/" + gf->fname;
	      strm = gf->open_write();
      }
      if((strm == NULL) || strm->bad())
	      cerr << "SaveRecoverFile: could not open file: " << gf->fname << "\n";
      else
	      p->Save(*strm);
      gf->Close();
      cnt++;
    }

    if (gf) taRefN::unRefDone(gf);
  } catch (...) {
    cerr << "PDP++ unable to save recover file...sorry\n";
    exit(err);
  }
}

