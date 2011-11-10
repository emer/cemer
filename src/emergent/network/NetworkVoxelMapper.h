// Copyright, 1995-2011, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#ifndef NETWORK_VOXEL_MAPPER_H
#define NETWORK_VOXEL_MAPPER_H

#include <QList>
#include <QMultiHash>
#include <QString>

//#include "netstru.h" // Network, Unit
class Network;
class Unit;

#include "ta_geometry.h" // FloatTDCoord

class NetworkVoxelMapper
{
public:
  NetworkVoxelMapper(Network *network);
  void assignVoxels();

private:
  void createUnitMap(Network *network);
  QList<TDCoord> getVoxelsInArea(const QString &brainArea);
  void assignVoxelsInArea(const QString &brainArea, const QList<TDCoord> &voxels);
  double getVoxelSize(unsigned numUnits, unsigned numVoxels);
  unsigned getVoxelDivisions(unsigned numUnits, unsigned numVoxels);
  QList<unsigned> getSubvoxelIndexes(unsigned numUnits, unsigned numSubvoxels);
  FloatTDCoord getCoord(unsigned subvoxelIdx, const QList<TDCoord> &voxels, unsigned voxelDivisions);

  QMultiHash<QString, Unit *> unitMap_;
  QString atlasName_;
};

#endif // NETWORK_VOXEL_MAPPER_H
