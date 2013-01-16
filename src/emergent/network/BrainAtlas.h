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

#ifndef BrainAtlas_h
#define BrainAtlas_h 1

// parent includes:
#include "network_def.h"

// member includes:
#include <QList>

// declare all other types mentioned but not required to include:
class BrainAtlasLabel;
class taVector3f;
class QString;


class EMERGENT_API BrainAtlas {
public:
  BrainAtlas();
  virtual ~BrainAtlas();
  
  virtual QString   Name() const = 0;
  virtual QString   AtlasFilename() const = 0;
  virtual QString   Description() const = 0;
  virtual QString   ImageFilename() const = 0;
  virtual QList<BrainAtlasLabel>  Labels(const QString& labels_regexp=QString(".*")) const = 0;
  virtual QList<taVector3f>  VoxelCoordinates(const QString& labels_regexp=QString(".*")) const = 0;
  virtual void SetLabels(const QList<BrainAtlasLabel>& labels ) = 0;
  virtual bool Save(const QString& filename) = 0;
};

#endif // BrainAtlas_h
