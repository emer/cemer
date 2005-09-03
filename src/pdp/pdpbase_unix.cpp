#include "pdpbase.h"

#include "ta_filer.h"

#ifdef TA_GUI
#include "ta_qt.h"
#endif

#include "pdpshell.h"

#include <signal.h>
#include <memory.h>
#include <sstream>

// for saving a recovery file if program crashes, is killed, etc.
void pdpMisc::SaveRecoverFile(int err) {
  static bool has_crashed = false;
  signal(err, SIG_DFL);		// disable catcher

#ifdef TA_GUI
  taiMisc::Cleanup(err);	// cleanup stuff in tai
#endif
  if(err == SIGUSR1)
    cerr << "PDP++ saving network file(s) from signal: ";
  else if((err == SIGUSR2) || (err == SIGALRM))
    cerr << "PDP++ saving project file(s) from signal: ";
  else
    cerr << "PDP++ saving recover file(s) and exiting from signal: ";
  taMisc::Decode_Signal(err);
  cerr << "\n";

  if(has_crashed) {
    cerr << "PDP++ unable to save recover file...sorry\n";
    exit(err);
  }
  has_crashed = true;		// to prevent recursive crashing..

  String net_sfx = ".net";	net_sfx += taMisc::compress_sfx;
  String wts_sfx = ".wts";  	wts_sfx += taMisc::compress_sfx;
  String proj_sfx = ".proj";	proj_sfx += taMisc::compress_sfx;

  String home_dir = getenv("HOME"); // home directory if curent dir does not work.
  taFiler* gf = taFiler_CreateInstance();		// use a getfile for compressed writes..
  taRefN::Ref(gf);
  Project* p;
  int cnt = 0;
  for (int i = 0; i < pdpMisc::root->projects.size; ++i) {
    p = root->projects.FastEl(i);
    if(err == SIGUSR1) {	// usr1 is to save network at that point
      Network* net;
      taLeafItr ni;
      FOR_ITR_EL(Network, net, p->networks., ni) {
	      String use_sfx = net_sfx;
	      if(net->usr1_save_fmt == Network::JUST_WEIGHTS)
	        use_sfx = wts_sfx;
	      cnt = get_unique_file_number(cnt, "PDP++NetSave.", use_sfx);
	      gf->fname = "PDP++NetSave." + String(cnt) + use_sfx;
	      ostream* strm = gf->open_write();
	      if(((strm == NULL) || strm->bad()) && !home_dir.empty()) {
	        // try it with the home diretory
	        cerr << "Error saving in current directory, trying home directory";
	        gf->Close();
	        gf->fname = home_dir + "/" + gf->fname;
	        strm = gf->open_write();
	      }
	      if((strm == NULL) || strm->bad())
	        cerr << "SaveNetwork: could not open file: " << gf->fname << "\n";
	      else {
	        if(net->usr1_save_fmt == Network::JUST_WEIGHTS)
	          net->WriteWeights(*strm, net->wt_save_fmt);
	        else
	          net->Save(*strm);
	      }
	      gf->Close();
	      cnt++;
      }
    } else { // save project
      String prfx;
      if ((err == SIGUSR2) || (err == SIGALRM))
	      prfx = "PDP++Project.";
      else
	      prfx = "PDP++Recover.";
      cnt = get_unique_file_number(cnt, prfx, proj_sfx);
      gf->fname = prfx + String(cnt) + proj_sfx;
      ostream* strm = gf->open_write();
      if(((strm == NULL) || strm->bad()) && !home_dir.empty()) {
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
  }

  if (gf) taRefN::unRefDone(gf);
  if((err == SIGALRM) || (err == SIGUSR1) || (err == SIGUSR2)) {
    taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) SaveRecoverFile);
    has_crashed = false;
  } else {
    kill(getpid(), err);		// activate signal
  }
}

