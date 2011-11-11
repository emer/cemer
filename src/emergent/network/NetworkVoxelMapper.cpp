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

#include "NetworkVoxelMapper.h"

#include <cassert>
#include "netstru.h" // Network, Unit
#include "ta_type.h" // taMisc::Warning()

namespace { // anonymous
  double
  cubeRoot(double val)
  {
    return std::pow(val, 1.0/3.0);
  }
}

NetworkVoxelMapper::NetworkVoxelMapper(Network *network)
  : unitMap_()
  , atlasName_(network->atlas_name.toQString())
{
  // The network pointer is not stored, so create a map of unit pointers.
  createUnitMap(network);
}

void
NetworkVoxelMapper::createUnitMap(Network *network)
{
  // Walk network layers and map unit pointers to brain areas.
  for (int layIdx = 0; layIdx < network->layers.leaves; ++layIdx)
  {
    if (Layer *layer = network->layers.Leaf(layIdx))
    {
      // TBD: check if the layer is lesioned?  Or just assign
      // coordinates to any layer that has a brain area associated?
      // If no brain area is associated, still need to assign coords?
      // Could assign size <= 0.0 to indicate not to render.
      //if (layer->HasLayerFlag(Layer::LESIONED))

      // If no brain_area was specified for this layer, warn the user,
      // but continue to add its units to the map so they can be set
      // to zero voxel_size later.
      if (layer->brain_area.empty())
      {
        taMisc::Warning("No brain_area specified for layer", layer->name,
          "; units will not be rendered in BrainView.");
      }

      // Get the number of units in this layer.
      Layer::AccessMode mode = Layer::ACC_LAY;
      int numUnitsInLayer = layer->UnitAccess_NUnits(mode);

      // Iterate through units.
      for (int unitIdx = 0; unitIdx < numUnitsInLayer; ++unitIdx)
      {
        Unit *unit = layer->UnitAccess(mode, unitIdx, 0);
        unitMap_.insert(layer->brain_area.toQString(), unit);
      }
    }
  }
}

void
NetworkVoxelMapper::assignVoxels()
{
  if (atlasName_.isEmpty())
  {
    taMisc::Warning("No atlas_name specified in network;",
      "cannot map units to voxel coordinates.");
    return;
  }

  // Iterate through brain areas.
  QList<QString> brainAreas = unitMap_.uniqueKeys();
  foreach(const QString &brainArea, brainAreas)
  {
    // Get the collection of voxel coordinates associated with that
    // brain area.
    QList<TDCoord> voxels = getVoxelsInArea(brainArea);

    // Assign voxel coordinates and sizes to all units.
    assignVoxelsInArea(brainArea, voxels);
  }
}

QList<TDCoord>
NetworkVoxelMapper::getVoxelsInArea(const QString &brainArea)
{
  QList<TDCoord> voxels;
  // TBD: atlas-specific code to get the collection of voxels
  // associated with the given brain area.
  return voxels;
}

void
NetworkVoxelMapper::assignVoxelsInArea(const QString &brainArea, const QList<TDCoord> &voxels)
{
  // Get the list of units that need voxel assignments for this brain area.
  QList<Unit *> units = unitMap_.values(brainArea);

  // Each unit's voxel coordinate and size will be based on the number of
  // units vs. voxels per brain area.
  unsigned numUnits = unitMap_.count(brainArea);
  unsigned numVoxels = voxels.count();

  // Check if any voxels were found for this brain area.
  if (numVoxels == 0)
  {
    if (!brainArea.isEmpty())
    {
      // Already warned about empty brain_area fields in createUnitMap().
      // This warning is to catch typos.
      taMisc::Warning("No voxels were found for brain area",
        brainArea.toLatin1(), "in atlas", atlasName_.toLatin1());
    }

    // Not much can be done other than assigning each unit to render at 0.0 size.
    foreach(Unit *unit, units)
    {
      unit->voxel_size = 0.0;
    }

    return;
  }

  // Get the size to render each unit in the brain view.
  double voxelSize = getVoxelSize(numUnits, numVoxels);

  // Get the number of subdivisions each voxel needs along each dimension
  // in order to accomodate the number of units.  This isn't necessarily
  // related to voxelSize.
  unsigned voxelDivisions = getVoxelDivisions(numUnits, numVoxels);

  // Calculate the total number of subvoxels after division, and get a list
  // of indices for which subvoxels will be assigned to units.
  unsigned numSubvoxels =
    numVoxels * voxelDivisions * voxelDivisions * voxelDivisions;
  QList<unsigned> subvoxelIdxs = getSubvoxelIndexes(numUnits, numSubvoxels);

  // Assign coordinates to each unit.
  assert(subvoxelIdxs.size() == units.size());
  unsigned idx = 0;
  foreach(Unit *unit, units)
  {
    unsigned subvoxelIdx = subvoxelIdxs[idx++];
    unit->voxel = getCoord(subvoxelIdx, voxels, voxelDivisions);
    unit->voxel_size = voxelSize;
  }
}

// Determine the size in mm per edge of the (sub)voxel cube.
double
NetworkVoxelMapper::getVoxelSize(unsigned numUnits, unsigned numVoxels)
{
  // As long as there are at least as many voxels as units for a brain area,
  // each unit will get rendered as a full-size voxel.
  if (numVoxels >= numUnits)
  {
    return 1.0;
  }

  // Otherwise, if fewer voxels exist than units, then each unit
  // should be rendered in a scaled-down manner.
  double ratio = static_cast<double>(numVoxels) / numUnits;

  // Take the cube root to get the voxel edge-length.
  return cubeRoot(ratio);
}

unsigned
NetworkVoxelMapper::getVoxelDivisions(unsigned numUnits, unsigned numVoxels)
{
  // If there are at least as many voxels as units, then there is no need
  // to divide voxels up.
  if (numVoxels >= numUnits)
  {
    return 1;
  }

  // Otherwise, divide a voxel along each dimension into sub-voxels
  // (for example, into 8, 27, or 64 cubes).  This may result in many
  // more sub-voxels than units.
  unsigned voxelDivisions = static_cast<unsigned>(
    cubeRoot(static_cast<double>(numUnits) / numVoxels));

  // Account for off-by-one type errors due to rounding in
  // floating point math.
  while (voxelDivisions * voxelDivisions * voxelDivisions * numVoxels < numUnits)
  {
    ++voxelDivisions;
  }

  return voxelDivisions;
}

QList<unsigned>
NetworkVoxelMapper::getSubvoxelIndexes(unsigned numUnits, unsigned numSubvoxels)
{
  QList<unsigned> subvoxelIdxs;

  // Use Bresenham line algo to determine which subvoxels to use.
  assert(numSubvoxels >= numUnits);
  int dx = 2 * numSubvoxels;
  int dy = 2 * numUnits;
  int D = dy - numSubvoxels;

  for (unsigned idx = 0; idx < numSubvoxels; ++idx)
  {
    if (D > 0)
    {
      subvoxelIdxs << idx;
      D -= dx;
    }
    D += dy;
  }

  return subvoxelIdxs;
}

FloatTDCoord
NetworkVoxelMapper::getCoord(unsigned subvoxelIdx, const QList<TDCoord> &voxels, unsigned voxelDivisions)
{
  // Turn a subvoxel index into a voxel and x,y,z offset into that voxel.
  int temp = subvoxelIdx;
  int dz = temp % voxelDivisions;

  temp /= voxelDivisions;
  int dy = temp % voxelDivisions;

  temp /= voxelDivisions;
  int dx = temp % voxelDivisions;

  temp /= voxelDivisions;
  int voxelIdx = temp;
  assert(voxelIdx < voxels.size());

  FloatTDCoord coord(voxels[voxelIdx]);
  FloatTDCoord subvoxelOffset(dx, dy, dz);
  coord += subvoxelOffset / static_cast<float>(voxelDivisions);

  return coord;
}
