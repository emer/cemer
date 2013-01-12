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

void RndSeed::Initialize() {
  GetCurrent();
}

void RndSeed::GetCurrent() {
  // Get the RNG's current state data.
  const MTRndState &state = MTRnd::get_state();

  // Save that state data to this object.
  seed.SetSize(MTRnd::N);
  for (int idx = 0; idx < seed.size; ++idx) {
    seed.FastEl(idx) = (long)state.mt[idx];
  }
  mti = state.mti;
  is_gauss_double_cached = state.is_gauss_double_cached;
  cached_gauss_double = state.cached_gauss_double;
}

void RndSeed::NewSeed() {
  MTRnd::seed_time_pid();
  GetCurrent();
}

void RndSeed::OldSeed() {
  // Ensure saved seed is valid.
  seed.SetSize(MTRnd::N);
  bool all_zero = true;
  for (int idx = 0; idx < seed.size; ++idx) {
    if (seed.FastEl(idx) != 0) {
      all_zero = false;
      break;
    }
  }
  if (all_zero) {
    taMisc::Warning("*** RndSeed::OldSeed: random seed is all zero and this doesn't work; getting current random seed!");
    GetCurrent();
    return;
  }

  // Restore saved seed to the RNG.
  MTRndState state;
  for (int idx = 0; idx < seed.size; ++idx) {
    state.mt[idx] = (ulong)seed.FastEl(idx);
  }
  state.mti = mti;
  state.is_gauss_double_cached = is_gauss_double_cached;
  state.cached_gauss_double = cached_gauss_double;
  MTRnd::set_state(state);
}

void RndSeed::Init(ulong i) {
  MTRnd::seed(i);
  GetCurrent();
}

#ifdef DMEM_COMPILE
void RndSeed::DMem_Sync(MPI_Comm comm) {
  if(taMisc::dmem_nprocs <= 1)
    return;

  // just blast the first guy to all members of the same communicator
  DMEM_MPICALL(MPI_Bcast(seed.el, MTRnd::N, MPI_LONG, 0, comm),
               "Process::SyncAllSeeds", "Bcast");
  OldSeed();            // then get my seed!
}

#else

void RndSeed::DMem_Sync(MPI_Comm) {
}

#endif // DMEM_COMPILE
