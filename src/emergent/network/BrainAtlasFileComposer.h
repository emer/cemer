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

#ifndef BrainAtlasFileComposer_h
#define BrainAtlasFileComposer_h 1

// parent includes:
#include "network_def.h"

// member includes:
#include <QList>

// declare all other types mentioned but not required to include:
class BrainAtlasLabel;
class BrainAtlasInfo;


class E_API BrainAtlasFileComposer {
public:
  BrainAtlasFileComposer(const QString& filename);
  virtual ~BrainAtlasFileComposer();
  virtual void ComposeHeader(const BrainAtlasInfo& info) = 0;
  virtual void ComposeLabels(const QList<BrainAtlasLabel>& labels) = 0;
  virtual bool Save() = 0;
};

#endif // BrainAtlasFileComposer_h
