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

#include "ImgProcThreadBase.h"

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ImgProcThreadBase);
TA_BASEFUNS_CTORS_DEFN(ImgProcCallThreadMgr);
TA_BASEFUNS_CTORS_DEFN(ImgProcCallTask);


void ImgProcCallTask::Initialize() {
}

void ImgProcCallTask::Destroy() {
}

void ImgProcCallTask::run() {
  ImgProcCallThreadMgr* mg = mgr();
  ImgProcThreadBase* base = mg->img_proc();

  meth_call.call(base, task_id); // task id indicates threading, and which thread
}

/////////////////////////////////
//              Mgr

void ImgProcCallThreadMgr::Initialize() {
  task_type = &TA_ImgProcCallTask;
}

void ImgProcCallThreadMgr::Destroy() {
}

void ImgProcCallThreadMgr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // if(n_threads > 4)             // right now we can't handle more than 4
  //   n_threads = 4;
  if(n_threads % 2 != 0) {      // must be even number!!
    n_threads = n_threads / 2;
    n_threads *= 2;
    n_threads = MAX(n_threads, 1);
  }
}

void ImgProcCallThreadMgr::Run(ThreadImgProcCall& meth_call) {
  const int nt = tasks.size;
  for(int i=0;i<nt;i++) {
    ImgProcCallTask* ntt = (ImgProcCallTask*)tasks[i];
    ntt->meth_call = meth_call;
  }

  inherited::Run();
}

/////////////////////////////////
//              Base

static void split_geom_half(const int geom, const int thr, int& st, int& ed) {
  const int hlf = geom / 2;
  st = thr * hlf;
  if(thr == 0)
    ed = hlf;
  else
    ed = geom;
}


bool ImgProcThreadBase::GetThread2DGeom(int thr_no, const taVector2i& geom,
                                        taVector2i& start, taVector2i& end) {
  start = 0;
  end = geom;
  // put it all on y:
  float perthr = ((float)geom.y / (float)threads.n_threads);
  start.y = (int) ((float)thr_no * perthr);
  end.y = (int) ((float)(thr_no+1) * perthr);
  if(thr_no == threads.n_threads-1)
    end.y = geom.y;             // just double checking..
  
  return true;
  // old strategy below here: splits up memory so not as efficient!
  switch(threads.n_threads) {
  case 1: {
    start = 0;
    end = geom;
    break;
  }
  case 2: {
    start.x = 0; end.x = geom.x; // full x
    split_geom_half(geom.y, thr_no, start.y, end.y);
    break;
  }
  case 4: {
    int xhalf = thr_no % 2;
    int yhalf = thr_no / 2;
    split_geom_half(geom.x, xhalf, start.x, end.x);
    split_geom_half(geom.y, yhalf, start.y, end.y);
    break;
  }
  default: {
    TestError(true, "GetThread2DGeom",
              "thread count not supported yet for image processing!",
              String(threads.n_threads));
    return false;
  }
  }
  return true;
}
