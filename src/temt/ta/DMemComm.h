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

#ifndef DMemComm_h
#define DMemComm_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:

#ifdef DMEM_COMPILE
#include <mpi.h>

#include <int_Array>

/////////////////////////////////////////////////////////////////
//	communicator: use this to define groups of communicating units

taTypeDef_Of(DMemComm);

class TA_API DMemComm : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS ##NO_UPDATE_AFTER ##CAT_DMem defines a communicator group for dmem communication
  INHERITED(taBase)
public:
  MPI_Comm	comm;		// #IGNORE the mpi communicator id
  MPI_Group	group;		// #IGNORE the mpi group id
  int		nprocs;		// #IGNORE number of processors in comm
  int_Array	ranks;		// #IGNORE proc numbers of members of the group
  int		this_proc;	// #IGNORE the rank of this processor within communicator

  void	CommAll();
  // #IGNORE use all the processors (world group)
  void	CommSelf();
  // #IGNORE we are a group of just our self
  void	CommSubGpInner(int sub_gp_size);
  // #IGNORE procs are organized into subgroups of given size, with nprocs / sub_gp_size such groups -- get the inner group for this processor (gp size = sub_gp_size)
  void	CommSubGpOuter(int sub_gp_size);
  // #IGNORE procs are organized into subgroups of given size, with nprocs / sub_gp_size such groups -- get the outer group for this processor

  int	GetThisProc();
  // #IGNORE get the rank of this processor relative to communicator

  void	MakeCommFmRanks();
  // #IGNORE make the comm from the ranks
  void	FreeComm();
  // #IGNORE free the comm & group 

  TA_BASEFUNS_LITE(DMemComm);
private:
  NOCOPY(DMemComm)
  void 	Initialize();
  void 	Destroy();
};

#else

taTypeDef_Of(DMemComm);

class TA_API DMemComm : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS ##NO_UPDATE_AFTER ##CAT_DMem defines a communicator group for dmem communication
  INHERITED(taBase)
public:

  //  void	Dummy(const DMemComm&) { };
  TA_BASEFUNS_LITE(DMemComm);
private:
  NOCOPY(DMemComm)
  void 	Initialize() { };
  void 	Destroy() { };
};

#endif // DMEM_COMPILE

#endif // DMemComm_h
