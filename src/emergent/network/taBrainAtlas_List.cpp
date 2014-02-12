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

#include "taBrainAtlas_List.h"
#include <BrainAtlasUtils>
#include <BrainAtlasInfo>

#include <taMisc>

#include <QStringList>

TA_BASEFUNS_CTORS_DEFN(taBrainAtlas_List);

void taBrainAtlas_List::Initialize() {
  name = "brain_atlases";
  not_init = true;
}

void taBrainAtlas_List::LoadAtlases() {
  not_init = false;		// initialized now
  Reset();
  
  // Populate the list of available brain atlases.
  // Look in TWO locations:
  //     1) Emergent install location + /data/atlases
  //     2) User dir + /data/atlases
  QString emerAtlasPath(taMisc::app_dir.toQString());
  if (emerAtlasPath.size() != 0) {
    emerAtlasPath += "/data/atlases";
  }
  QString userAtlasPath(taMisc::user_dir.toQString());
  if (userAtlasPath.size() != 0) {
    userAtlasPath += "/data/atlases";
  }
  QStringList atlases = BrainAtlasUtils::AtlasesAvailable(emerAtlasPath);
  atlases += BrainAtlasUtils::AtlasesAvailable(userAtlasPath);
  foreach(QString atlas, atlases) {
    BrainAtlasInfo info(BrainAtlasUtils::ParseAtlasHeader(atlas));
    taBrainAtlas* ba = new taBrainAtlas(info);    
    Add(ba);
  }
}
