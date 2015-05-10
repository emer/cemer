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

#include "RndSeed.h"
#include <MTRnd>
#include <taMisc>

#include <DMemShare>

TA_BASEFUNS_CTORS_DEFN(RndSeed);

void RndSeed::Initialize() {
  rnd_seed = 0;
}

void RndSeed::NewSeed() {
  rnd_seed = MTRnd::GetTimePidSeed();
  MTRnd::InitSeeds(rnd_seed);
}

void RndSeed::OldSeed() {
  if(TestWarning(rnd_seed == 0, "OldSeed",
               "rnd_seed is currently 0 -- attempting to use uninitialized rnd_seed in OldSeed -- running NewSeed first!")) {
    NewSeed();
  }
  else {
    MTRnd::InitSeeds(rnd_seed);
  }
}

void RndSeed::Init(uint32_t i) {
  rnd_seed = i;
  MTRnd::InitSeeds(rnd_seed);
}

#ifdef DMEM_COMPILE
void RndSeed::DMem_Sync(MPI_Comm comm) {
  if(taMisc::dmem_nprocs <= 1)
    return;

  // just blast the first guy to all members of the same communicator
  DMEM_MPICALL(MPI_Bcast(&rnd_seed, 1, MPI_INT, 0, comm),
               "Process::SyncAllSeeds", "Bcast");
  OldSeed();            // then get my seed!
}

#else

void RndSeed::DMem_Sync(MPI_Comm) {
}

#endif // DMEM_COMPILE
