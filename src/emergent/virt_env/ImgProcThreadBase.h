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

#ifndef ImgProcThreadBase_h
#define ImgProcThreadBase_h 1

// parent includes:
#include <taNBase>
#include <taThreadMgr>
#include "network_def.h"

// member includes:
#include <taVector2i>

// declare all other types mentioned but not required to include:
class ImgProcThreadBase; // 
class ImgProcCallThreadMgr; //

// this is the standard function call taking the thread number int value
// all threaded functions MUST use this call signature!
#ifdef __MAKETA__
typedef void* ThreadImgProcCall;
#else
typedef taTaskMethCall1<ImgProcThreadBase, void, int> ThreadImgProcCall;
typedef void (ImgProcThreadBase::*ThreadImgProcMethod)(int);
#endif

#define IMG_THREAD_CALL(meth) { ThreadImgProcCall meth_call((ThreadImgProcMethod)(&meth));\
  threads.Run(meth_call); }

eTypeDef_Of(ImgProcCallTask);

class E_API ImgProcCallTask : public taTask {
INHERITED(taTask)
public:
  ThreadImgProcCall    meth_call;	// #IGNORE method to call on the object

  void run() override;

  ImgProcCallThreadMgr* mgr() { return (ImgProcCallThreadMgr*)owner->GetOwner(); }

  TA_BASEFUNS_NOCOPY(ImgProcCallTask);
private:
  void	Initialize();
  void	Destroy();
};


eTypeDef_Of(ImgProcCallThreadMgr);

class E_API ImgProcCallThreadMgr : public taThreadMgr {
  // #INLINE thread manager for ImgProcCall tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  ImgProcThreadBase*	img_proc() 	{ return (ImgProcThreadBase*)owner; }

  void		Run(ThreadImgProcCall& meth_call);
  // #IGNORE run given function, passing thread number as arg -- splits computation into n_threads components
  
  TA_BASEFUNS_NOCOPY(ImgProcCallThreadMgr);
protected:
  void UpdateAfterEdit_impl() override;
  
private:
  void	Initialize();
  void	Destroy();
};

eTypeDef_Of(ImgProcThreadBase);

class E_API ImgProcThreadBase : public taNBase {
  // #VIRT_BASE ##CAT_Image base class for image-processing code that uses threading -- defines a basic interface for thread calls to deploy filtering or other intensive computations
  INHERITED(taNBase)
public:
  ImgProcCallThreadMgr threads; // #CAT_Threads parallel threading of image processing computation

  virtual bool GetThread2DGeom(int thr_no, const taVector2i& geom,
                               taVector2i& start, taVector2i& end);
  // get start and end coords for a given thread to process, based on total 2d geometry -- total number of threads guaranteed to be an even number!
  
  TA_SIMPLE_BASEFUNS(ImgProcThreadBase);
private:
  void 	Initialize() { };
  void	Destroy() { };
};

#endif // ImgProcThreadBase_h
