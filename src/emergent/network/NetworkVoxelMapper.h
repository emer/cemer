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

#ifndef NetworkVoxelMapper_h
#define NetworkVoxelMapper_h 1

// parent includes:
#include "network_def.h"

// member includes:
#include <QList>
#include <QMultiHash>
#include <QString>

// declare all other types mentioned but not required to include:
class Network;
class Layer;
class Unit;
class taVector3f;
class taBrainAtlas;


class E_API NetworkVoxelMapper {
public:
  NetworkVoxelMapper(Network *network);
  ~NetworkVoxelMapper();
  void AssignVoxels();

private:
  class LayerInfo;

  void CreateLayerMap();
  QList<taVector3f> GetVoxelsInArea(QString brain_area);
  void AssignVoxelsInArea(QString brain_area, QList<taVector3f> voxels);
  void AssignVoxelsToLayers(QList<taVector3f> voxels, QList<unsigned> subvoxel_idxs, unsigned voxel_divisions);
  bool HandleEmptyBrainArea(unsigned num_voxels, QString brain_area, QList<Layer *> layers);
  void ClearVoxelAssignmentForLayer(LayerInfo *li);
  void MakeLayerInfos(QList<Layer *> layers);
  void ClearLayerInfos();
  double ComputeLayerPercentages();
  void RemoveZeroFillLayers();
  unsigned GetVoxelDivisions(unsigned num_voxels, unsigned &num_subvoxels);
  unsigned GetNeededSubvoxelCount(unsigned num_subvoxels);
  QList<unsigned> GetSubvoxelIndexes(unsigned num_units, unsigned num_subvoxels);
  taVector3f GetCoord(unsigned subvoxel_idx, const QList<taVector3f> &voxels, unsigned voxel_divisions);

private:
  QMultiHash<QString, Layer *> m_layer_map;
  Network *m_network;
  taBrainAtlas* m_atlas;

  // Info on layers for the current iteration of AssignVoxelsInArea().
  QList<LayerInfo *> m_layer_info;
};

#endif // NetworkVoxelMapper_h
