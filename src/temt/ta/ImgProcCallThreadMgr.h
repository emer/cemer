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

#ifndef ImgProcCallThreadMgr_h
#define ImgProcCallThreadMgr_h 1

// parent includes:
#include <taThreadMgr>
#include <ImgProcCallTask>

// member includes:

// declare all other types mentioned but not required to include:
class ImgProcThreadBase; // 

class TA_API ImgProcCallThreadMgr : public taThreadMgr {
  // #INLINE thread manager for ImgProcCall tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  int		min_units;	// #MIN_1 #NO_SAVE NOTE: not saved -- initialized from user prefs.  minimum number of computational units of work required to use threads at all -- if less than this number, all will be computed on the main thread to avoid threading overhead which may be more than what is saved through parallelism, if there are only a small number of things to compute.
  int		nibble_chunk;	// #MIN_1 #DEF_8 #NO_SAVE NOTE: not saved -- initialized from user prefs.  how many units does each thread grab to process while nibbling?  Too small a value results in increased contention and inefficiency, while too large a value results in poor load balancing across processors.

  taAtomicInt	nibble_i;	// #IGNORE current nibble index -- atomic incremented by working threads to nibble away the rest..
  int		n_cmp_units;	// #IGNORE number of compute units to perform -- max of the nibbling..

  ImgProcThreadBase*	img_proc() 	{ return (ImgProcThreadBase*)owner; }

  override void		Run(ThreadImgProcCall* img_proc_call, int n_cmp_un);
  // #IGNORE run given function, splitting n_cmp_units computational units evenly across the available threads
  
  TA_BASEFUNS_NOCOPY(ImgProcCallThreadMgr);
private:
  void	Initialize();
  void	Destroy();
};

#endif // ImgProcCallThreadMgr_h
