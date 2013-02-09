// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef ProjectBase_h
#define ProjectBase_h 1

// parent includes:
#include <taProject>

// member includes:
#include <Network_Group>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ProjectBase);

class E_API ProjectBase : public taProject {
  // ##FILETYPE_Project ##EXT_proj ##COMPRESS #VIRT_BASE Base class for a pdp project (do not create one of these -- create an algorithm-specific version)
INHERITED(taProject)
public:
  Network_Group		networks;	// Networks of interconnected units

  virtual Network* 	GetNewNetwork(TypeDef* typ = NULL);
  // get a new network object
  virtual Network* 	GetDefNetwork();
  // get default network from project

#ifndef __MAKETA__
  override void		SaveRecoverFile_strm(std::ostream& strm);
#endif

  virtual void	AutoBuildNets();
  // build networks according to their own individual auto_bulid modes
  
  override void	Dump_Load_post();

  void	UpdateAfterEdit();
  void 	InitLinks_impl(); // special, for this class only
  void 	InitLinks_post(); // special, for this class only
  void	CutLinks_impl(); // special, so we can do base class guys first (esp. viewers)
  void	Copy_(const ProjectBase& cp);
  TA_BASEFUNS(ProjectBase);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
};

#endif // ProjectBase_h
