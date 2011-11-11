// Copyright, 1995-2011, Regents of the University of Colorado,
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
  void AssignVoxels();

private:
  void CreateUnitMap(Network *network);
  QList<TDCoord> GetVoxelsInArea(const QString &brain_area);
  void AssignVoxelsInArea(const QString &brain_area, const QList<TDCoord> &voxels);
  double GetVoxelSize(unsigned num_units, unsigned num_voxels);
  unsigned GetVoxelDivisions(unsigned num_units, unsigned num_voxels);
  QList<unsigned> GetSubvoxelIndexes(unsigned num_units, unsigned num_subvoxels);
  FloatTDCoord GetCoord(unsigned subvoxel_idx, const QList<TDCoord> &voxels, unsigned voxel_divisions);

  QMultiHash<QString, Unit *> unit_map;
  QString atlas_name;
};

#endif // NETWORK_VOXEL_MAPPER_H
