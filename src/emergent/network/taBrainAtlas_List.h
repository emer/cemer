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

#ifndef taBrainAtlas_List_h
#define taBrainAtlas_List_h 1

// parent includes:
#include <taList>
#include <taBrainAtlas>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(taBrainAtlas_List);

class EMERGENT_API taBrainAtlas_List : public taList<taBrainAtlas> {
  // #NO_TOKENS #NO_UPDATE_AFTER List of brain atlases.
  INHERITED(taList<taBrainAtlas>)
public:
  bool		not_init;	// list has not been initialized yet

  virtual void	LoadAtlases();	// load the list

  TA_BASEFUNS_SC(BrainAtlas_List);
private:
  void  Initialize();
  void  Destroy()               { };
};

#endif // taBrainAtlas_List_h
