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

#include "netstru.h"
#include "nifti_reader.h" // TalairachAtlas
#include "ta_geometry.h" // FloatTDCoord
class Network;
class Layer;
class Unit;

class NetworkVoxelMapper
{
public:
  NetworkVoxelMapper(Network *network);
  ~NetworkVoxelMapper();
  void AssignVoxels();

private:
  class LayerInfo;

  void CreateLayerMap();
  QList<FloatTDCoord> GetVoxelsInArea(QString brain_area);
  void AssignVoxelsInArea(QString brain_area, QList<FloatTDCoord> voxels);
  void AssignVoxelsToLayers(QList<FloatTDCoord> voxels, QList<unsigned> subvoxel_idxs, unsigned voxel_divisions);
  bool HandleEmptyBrainArea(unsigned num_voxels, QString brain_area, QList<Layer *> layers);
  void ClearVoxelAssignmentForLayer(LayerInfo *li);
  void MakeLayerInfos(QList<Layer *> layers);
  void ClearLayerInfos();
  double ComputeLayerPercentages();
  void RemoveZeroFillLayers();
  unsigned GetVoxelDivisions(unsigned num_voxels, unsigned &num_subvoxels);
  unsigned GetNeededSubvoxelCount(unsigned num_subvoxels);
  QList<unsigned> GetSubvoxelIndexes(unsigned num_units, unsigned num_subvoxels);
  FloatTDCoord GetCoord(unsigned subvoxel_idx, const QList<FloatTDCoord> &voxels, unsigned voxel_divisions);

private:
  QMultiHash<QString, Layer *> m_layer_map;
  Network *m_network;
  BrainAtlasProxy* m_atlas;

  // Info on layers for the current iteration of AssignVoxelsInArea().
  QList<LayerInfo *> m_layer_info;
};

#endif // NETWORK_VOXEL_MAPPER_H
