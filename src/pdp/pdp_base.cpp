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
#include "css_machine.h"
#include "pdp_project.h"

#ifdef TA_GUI
# include "ta_qt.h"
//# include <qbitmap.h>
#endif

#include <signal.h>

#ifdef TA_GUI
#define pdp_bitmap_width 64
#define pdp_bitmap_height 64
static unsigned char pdp_bitmap_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x83, 0xff,
  0x1f, 0xfc, 0xff, 0x00, 0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00,
  0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00, 0x10, 0x1f, 0x82, 0xf8,
  0x10, 0xc4, 0x87, 0x00, 0x10, 0x32, 0x82, 0x10, 0x11, 0x84, 0x8c, 0x00,
  0x10, 0x22, 0x82, 0x10, 0x11, 0x84, 0x88, 0x00, 0x10, 0x32, 0x82, 0x10,
  0x11, 0x84, 0x8c, 0x00, 0x10, 0x1e, 0x82, 0x10, 0x11, 0x84, 0x87, 0x00,
  0x10, 0x02, 0x82, 0x10, 0x11, 0x84, 0x80, 0x00, 0x10, 0x02, 0x82, 0x10,
  0x11, 0x84, 0x80, 0x00, 0x10, 0x0f, 0x82, 0xf8, 0x10, 0xc4, 0x83, 0x00,
  0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00, 0x10, 0x00, 0x82, 0x00,
  0x10, 0x04, 0x80, 0x00, 0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00,
  0xf0, 0xff, 0x83, 0xff, 0x1f, 0xfc, 0xff, 0x00, 0x00, 0x78, 0x00, 0x50,
  0x00, 0xf0, 0x00, 0x00, 0x00, 0x90, 0x03, 0x88, 0x00, 0x4e, 0x00, 0x00,
  0x00, 0x20, 0x0c, 0x04, 0xc1, 0x21, 0x00, 0x00, 0x00, 0x40, 0x70, 0x02,
  0x3a, 0x10, 0x00, 0x00, 0x00, 0x40, 0x80, 0x03, 0x06, 0x08, 0x00, 0x00,
  0x00, 0x80, 0x80, 0xdc, 0x05, 0x04, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8,
  0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x22, 0x07, 0x13, 0x01, 0x00, 0x00,
  0x00, 0x00, 0xf4, 0x00, 0xbc, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07,
  0xff, 0x3f, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
  0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0x20, 0x08, 0x04, 0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x08, 0x04,
  0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x3e, 0x04, 0xf1, 0x21, 0x00, 0x00,
  0x00, 0x20, 0x08, 0x04, 0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x08, 0x04,
  0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
  0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0xe0, 0xff, 0x07, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xf4, 0x01,
  0x7c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x42, 0x9e, 0x23, 0x06, 0x00, 0x00,
  0x00, 0x00, 0x81, 0xf9, 0x11, 0x08, 0x00, 0x00, 0x00, 0x80, 0x00, 0x07,
  0x1e, 0x10, 0x00, 0x00, 0x00, 0x40, 0xf0, 0x0c, 0xe2, 0x21, 0x00, 0x00,
  0x00, 0x20, 0x0e, 0x10, 0x01, 0xde, 0x00, 0x00, 0x00, 0xf0, 0x01, 0xe0,
  0x00, 0xe0, 0x01, 0x00, 0xe0, 0xff, 0x07, 0xff, 0x3f, 0xf8, 0xff, 0x01,
  0x20, 0x00, 0x04, 0x01, 0x20, 0x08, 0x00, 0x01, 0x20, 0x00, 0x04, 0x01,
  0x20, 0x08, 0x00, 0x01, 0x20, 0x3e, 0x04, 0xf1, 0x21, 0x88, 0x0f, 0x01,
  0x20, 0x64, 0x04, 0x21, 0x22, 0x08, 0x19, 0x01, 0x20, 0x44, 0x04, 0x21,
  0x22, 0x08, 0x11, 0x01, 0x20, 0x64, 0x04, 0x21, 0x22, 0x08, 0x19, 0x01,
  0x20, 0x3c, 0x04, 0x21, 0x22, 0x08, 0x0f, 0x01, 0x20, 0x04, 0x04, 0x21,
  0x22, 0x08, 0x01, 0x01, 0x20, 0x04, 0x04, 0x21, 0x22, 0x08, 0x01, 0x01,
  0x20, 0x1e, 0x04, 0xf1, 0x21, 0x88, 0x07, 0x01, 0x20, 0x00, 0x04, 0x01,
  0x20, 0x08, 0x00, 0x01, 0x20, 0x00, 0x04, 0x01, 0x20, 0x08, 0x00, 0x01,
  0x20, 0x00, 0x04, 0x01, 0x20, 0x08, 0x00, 0x01, 0xe0, 0xff, 0x07, 0xff,
  0x3f, 0xf8, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#endif


//////////////////////////
//	pdpMisc		//
//////////////////////////

bool 		pdpMisc::nw_itm_def_arg = false;
PDPRoot* 	pdpMisc::root = NULL;
String		pdpMisc::user_spec_def;
float		pdpMisc::pdpZScale = 4.0f;

int pdpMisc::Main(int argc, const char *argv[]) {
  cssMisc::prompt = "pdp++";
  if(!taRootBase::Startup_Main(argc, argv, ta_Init_pdp, &TA_PDPRoot)) return 1;

  root = (PDPRoot*)tabMisc::root;
   //always use our wait proc, since there is a predefined chain backwards anyways...
  taMisc::WaitProc = pdpMisc::WaitProc;
#if ((!defined(DMEM_COMPILE)) && (!defined(TA_OS_WIN))) 
  taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) SaveRecoverFile);
#endif
  root->colorspecs.SetDefaultColor();	// create colorspecs even if nogui, since they are referred to in projects
  root->LoadConfig();
  cssMisc::TopShell->cmd_prog->CompileRunClear(".pdpinitrc");
  if(taRootBase::Startup_Run())
    return 0;
  else
    return 2;
}

#ifdef TA_GUI
SelectEdit* pdpMisc::FindSelectEdit(ProjectBase* prj) {
  return (SelectEdit*)prj->edits.DefaultEl();
}

SelectEdit* pdpMisc::FindMakeSelectEdit(ProjectBase* prj) {
  SelectEdit* rval = (SelectEdit*)prj->edits.DefaultEl();
  if(rval != NULL) return rval;
  rval = (SelectEdit*)prj->edits.New(1, &TA_SelectEdit);
  return rval;
}
#endif

ColorScaleSpec* pdpMisc::GetDefaultColor() {
  if((root == NULL) || !taMisc::gui_active)	return NULL;
  return (ColorScaleSpec*)root->colorspecs.DefaultEl();
}

int get_unique_file_number(int st_no, const char* prefix, const char* suffix) {
  String prfx = prefix;
  String fname;
  int i;
  for(i=st_no; i<10000; i++) {	// stop at 10,000
    fname = prfx + String(i) + suffix;
    int acc = access(fname, R_OK);
    if(acc != 0)
      break;			// its ok..
  }
  fstream strm;
  strm.open(fname, ios::out);	// this should hold the place for the file
  strm.close();	strm.clear();		// while it is being saved, etc..
  return i;
}

void pdpMisc::WaitProc() {
#ifdef DMEM_COMPILE
  if((taMisc::dmem_nprocs > 1) && (taMisc::dmem_proc == 0)) {
    DMem_WaitProc();
  }
#endif

#ifdef TA_GUI
  if(taMisc::gui_active) {
    taiMisc::OpenWindows();
  }
  taiMisc::WaitProc();
#else
  taiMiscCore::WaitProc();
#endif
}

Network* pdpMisc::GetNewNetwork(ProjectBase* prj, TypeDef* typ) {
  if(prj == NULL) return NULL;
  Network* rval = (Network*)prj->networks.New(1, typ);
#ifdef TA_GUI
  taiMisc::RunPending();
#endif
  return rval;
}

Network* pdpMisc::GetDefNetwork(ProjectBase* prj) {
  if(prj == NULL) return NULL;
  return (Network*)prj->networks.DefaultEl();
}

#ifdef DMEM_COMPILE
// todo: move this stuff down to ta

static cssProgSpace* dmem_space1 = NULL;
static cssProgSpace* dmem_space2 = NULL;

void pdpMisc::DMem_WaitProc(bool send_stop_to_subs) {
  if(dmem_space1 == NULL) dmem_space1 = new cssProgSpace;
  if(dmem_space2 == NULL) dmem_space2 = new cssProgSpace;

  if(DMemShare::cmdstream->bad() || DMemShare::cmdstream->eof()) {
    taMisc::Error("DMem: Error! cmstream is bad or eof.",
		  "Software will not respond to any commands, must quit!!");
  }
  while(DMemShare::cmdstream->tellp() > DMemShare::cmdstream->tellg()) {
    DMemShare::cmdstream->seekg(0, ios::beg);
    string str = DMemShare::cmdstream->str();
    String cmdstr = str.c_str();
    cmdstr = cmdstr.before((int)(DMemShare::cmdstream->tellp() - DMemShare::cmdstream->tellg()));
    // make sure to only get the part that is current -- other junk might be in there.
    cmdstr += '\n';
    if(taMisc::dmem_debug) {
      cerr << "proc 0 sending cmd: " << cmdstr;
    }
    DMemShare::cmdstream->seekp(0, ios::beg);

    int cmdlen = cmdstr.length();

    DMEM_MPICALL(MPI_Bcast((void*)&cmdlen, 1, MPI_INT, 0, MPI_COMM_WORLD),
		 "Proc 0 WaitProc", "MPI_Bcast - cmdlen");

    DMEM_MPICALL(MPI_Bcast((void*)(const char*)cmdstr, cmdlen, MPI_CHAR, 0, MPI_COMM_WORLD),
		 "Proc 0 WaitProc", "MPI_Bcast - cmd");

    if(taMisc::dmem_debug) {
      cerr << "proc 0 running cmd: " << cmdstr << endl;
    }
    // now run the command: it wasn't run before!
    cssProgSpace* sp = dmem_space1; // if first space is currently running, use another
    if(sp->state & (cssProg::State_Run | cssProg::State_Cont)) {
      if(taMisc::dmem_debug)
	cerr << "proc 0 using 2nd space!" << endl;
      sp = dmem_space2;
    }

    sp->CompileCode(cmdstr);
    sp->Run();
    sp->ClearAll();
  }
  if(send_stop_to_subs) {
    String cmdstr = "stop";
    int cmdlen = cmdstr.length();
    DMEM_MPICALL(MPI_Bcast((void*)&cmdlen, 1, MPI_INT, 0, MPI_COMM_WORLD),
		 "Proc 0 WaitProc, SendStop", "MPI_Bcast - cmdlen");
    DMEM_MPICALL(MPI_Bcast((void*)(const char*)cmdstr, cmdlen, MPI_CHAR, 0, MPI_COMM_WORLD),
		 "Proc 0 WaitProc, SendStop", "MPI_Bcast - cmdstr");
  }
}

int pdpMisc::DMem_SubEventLoop() {
  if(taMisc::dmem_debug) {
    cerr << "proc: " << taMisc::dmem_proc << " event loop start" << endl;
  }

  if(dmem_space1 == NULL) dmem_space1 = new cssProgSpace;
  if(dmem_space2 == NULL) dmem_space2 = new cssProgSpace;

  while(true) {
    int cmdlen;
    DMEM_MPICALL(MPI_Bcast((void*)&cmdlen, 1, MPI_INT, 0, MPI_COMM_WORLD),
		 "Proc n SubEventLoop", "MPI_Bcast");
    char* recv_buf = new char[cmdlen+2];
    DMEM_MPICALL(MPI_Bcast(recv_buf, cmdlen, MPI_CHAR, 0, MPI_COMM_WORLD),
		 "Proc n SubEventLoop", "MPI_Bcast");
    recv_buf[cmdlen] = '\0';
    String cmd = recv_buf;
    delete recv_buf;

    if(cmd.length() > 0) {
      if(taMisc::dmem_debug) {
       cerr << "proc " << taMisc::dmem_proc << " recv cmd: " << cmd << endl << endl;
      }
      if(cmd == "stop") {
	if(taMisc::dmem_debug)
	  cerr << "proc " << taMisc::dmem_proc << " got stop command, stopping out of sub event processing loop." << endl;
	return 1;
      }
      else if(!cmd.contains("Save(") && !cmd.contains("SaveAs(")) {
	if(taMisc::dmem_debug) {
	  cerr << "proc " << taMisc::dmem_proc << " running cmd: " << cmd << endl;
	}

	cssProgSpace* sp = dmem_space1; // if first space is currenntly running, use another
	if(sp->state & (cssProg::State_Run | cssProg::State_Cont)) {
	  if(taMisc::dmem_debug)
	    cerr << "proc " << taMisc::dmem_proc << " using 2nd space!" << endl;
	  sp = dmem_space2;
	}

	sp->CompileCode(cmd);
	sp->Run();
	sp->ClearAll();

	if(cmd.contains("Quit()")) {
	  if(taMisc::dmem_debug)
	    cerr << "proc " << taMisc::dmem_proc << " got quit command, quitting." << endl;
	  return 1;
	}
      }
    }
    else {
      cerr << "proc " << taMisc::dmem_proc << " received null command!" << endl;
    }
    // do basic wait proc here..
    tabMisc::WaitProc();
  }
  return 0;
}

#endif // DMEM

