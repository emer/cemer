// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef Network_Group_h
#define Network_Group_h 1

// parent includes:
#include <taGroup>
#include <Network>

// member includes:
#include <taBrainAtlas_List>

// declare all other types mentioned but not required to include:

eTypeDef_Of(Network_Group);

class E_API Network_Group : public taGroup<Network> {
  // ##FILETYPE_Network ##EXT_net ##COMPRESS ##CAT_Network ##TOKENS a group of networks
INHERITED(taGroup<Network>)
public:
  static taBrainAtlas_List  brain_atlases;  // #IGNORE Brain atlases available to select for network voxel mapping

  String       GetTypeDecoKey() const override { return "Network"; }

  void         RestorePanels();

  void  InitLinks() override;
  void  CutLinks() override;
  TA_BASEFUNS_NOCOPY(Network_Group); //
private:
  void  Initialize()            {SetBaseType(&TA_Network);}
  void  Destroy()               { };
};

#endif // Network_Group_h
