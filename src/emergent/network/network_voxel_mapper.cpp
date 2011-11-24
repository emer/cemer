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

#include "network_voxel_mapper.h"

#include <cassert>
#include "netstru.h" // Network, Unit
#include "ta_type.h" // taMisc::Warning()

namespace { // anonymous
  double
  CubeRoot(double val)
  {
    return std::pow(val, 1.0/3.0);
  }

  QString GetAtlasFilename() {
    // For now, only read from the Talairach atlas.
    String talairachFilename = taMisc::app_dir + "/data/atlases/talairach.nii";
    return talairachFilename.toQString();
  }
}

NetworkVoxelMapper::NetworkVoxelMapper(Network *network)
  : unit_map()
  , network(network)
  , atlas(GetAtlasFilename())
{
}

void
NetworkVoxelMapper::AssignVoxels()
{
  if (network->atlas_name.empty())
  {
    taMisc::Warning("No atlas_name specified in network;",
      "cannot map units to voxel coordinates.");
    return;
  }

  network->StructUpdate(true);	// prevent any upddates during mapping

  // Create a map of unit pointers.
  CreateUnitMap(network);

  // Iterate through brain areas.
  QList<QString> brain_areas = unit_map.uniqueKeys();
  foreach(const QString &brain_area, brain_areas)
  {
    // Get the collection of voxel coordinates associated with that
    // brain area.
    QList<FloatTDCoord> voxels = GetVoxelsInArea(brain_area);

    // Assign voxel coordinates and sizes to all units.
    AssignVoxelsInArea(brain_area, voxels);
  }

  network->StructUpdate(false);	// trigger update after mapping
}

void
NetworkVoxelMapper::CreateUnitMap(Network *network)
{
  // Walk network layers and map unit pointers to brain areas.
  for (int lay_idx = 0; lay_idx < network->layers.leaves; ++lay_idx)
  {
    if (Layer *layer = network->layers.Leaf(lay_idx))
    {
      // TBD: check if the layer is lesioned?  Or just assign
      // coordinates to any layer that has a brain area associated?
      // If no brain area is associated, still need to assign coords?
      // Could assign size <= 0.0 to indicate not to render.
      //if (layer->HasLayerFlag(Layer::LESIONED))
      if(layer->lesioned() || layer->Iconified()) continue;
      // always ignore lesioned and iconified layers

      // If no brain_area was specified for this layer, warn the user,
      // but continue to add its units to the map so they can be set
      // to zero voxel_size later.
      if (layer->brain_area.empty())
      {
	// do not warn: mapping often called on nets with nothing set, and some
	// you just don't want to map in any case.  it is easy to see if empty..
//         taMisc::Warning("No brain_area specified for layer", layer->name,
//           "; units will not be rendered in BrainView.");
      }

      // Get the number of units in this layer.
      Layer::AccessMode mode = Layer::ACC_LAY;
      int num_units_in_layer = layer->UnitAccess_NUnits(mode);

      // Iterate through units.
      for (int unit_idx = 0; unit_idx < num_units_in_layer; ++unit_idx)
      {
        Unit *unit = layer->UnitAccess(mode, unit_idx, 0);
        unit_map.insert(layer->brain_area.toQString(), unit);
      }
    }
  }
}

// Get the collection of voxels associated with the given brain area,
// in an atlas-specific manner.
QList<FloatTDCoord>
NetworkVoxelMapper::GetVoxelsInArea(const QString &brain_area)
{
  QList<FloatTDCoord> voxels;

  // Return no voxels if no brain area was specified.
  if (!brain_area.isEmpty()) {
    // Get the list of voxels, first by their i,j,k indices, then convert
    // to x,y,z coordinates.
    QList<TDCoord> voxelIdxs = atlas.GetVoxelsInArea(brain_area);
    voxels = atlas.GetVoxelCoords(voxelIdxs);
  }

  return voxels;
}

void
NetworkVoxelMapper::AssignVoxelsInArea(const QString &brain_area, const QList<FloatTDCoord> &voxels)
{
  // Get the list of units that need voxel assignments for this brain area.
  QList<Unit *> units = unit_map.values(brain_area);

  // Each unit's voxel coordinate and size will be based on the number of
  // units vs. voxels per brain area.
  unsigned num_units = unit_map.count(brain_area);
  unsigned num_voxels = voxels.count();

  // Check if any voxels were found for this brain area.
  if (num_voxels == 0)
  {
    if (!brain_area.isEmpty())
    {
      // Already warned about empty brain_area fields in CreateUnitMap().
      // This warning is to catch typos.
      taMisc::Warning("No voxels were found for brain area",
        brain_area.toLatin1(), "in atlas", network->atlas_name);
    }

    // Not much can be done other than assigning each unit to render at 0.0 size.
    foreach(Unit *unit, units)
    {
      unit->voxel_size = 0.0;
    }

    return;
  }

  // Get the size to render each unit in the brain view.
  double voxel_size = GetVoxelSize(num_units, num_voxels);

  // Get the number of subdivisions each voxel needs along each dimension
  // in order to accomodate the number of units.  This isn't necessarily
  // related to voxel_size.
  unsigned voxel_divisions = GetVoxelDivisions(num_units, num_voxels);

  // Calculate the total number of subvoxels after division, and get a list
  // of indices for which subvoxels will be assigned to units.
  unsigned num_subvoxels =
    num_voxels * voxel_divisions * voxel_divisions * voxel_divisions;
  QList<unsigned> subvoxel_idxs = GetSubvoxelIndexes(num_units, num_subvoxels);

  // Assign coordinates to each unit.
  assert(subvoxel_idxs.size() == units.size());
  unsigned idx = 0;
  foreach(Unit *unit, units)
  {
    unsigned subvoxel_idx = subvoxel_idxs[idx++];
    unit->voxel = GetCoord(subvoxel_idx, voxels, voxel_divisions);
    unit->voxel_size = voxel_size;
  }
}

// Determine the size in mm per edge of the (sub)voxel cube.
double
NetworkVoxelMapper::GetVoxelSize(unsigned num_units, unsigned num_voxels)
{
  // As long as there are at least as many voxels as units for a brain area,
  // each unit will get rendered as a full-size voxel.
  if (num_voxels >= num_units)
  {
    return 1.0;
  }

  // Otherwise, if fewer voxels exist than units, then each unit
  // should be rendered in a scaled-down manner.
  double ratio = static_cast<double>(num_voxels) / num_units;

  // Take the cube root to get the voxel edge-length.
  return CubeRoot(ratio);
}

unsigned
NetworkVoxelMapper::GetVoxelDivisions(unsigned num_units, unsigned num_voxels)
{
  // If there are at least as many voxels as units, then there is no need
  // to divide voxels up.
  if (num_voxels >= num_units)
  {
    return 1;
  }

  // Otherwise, divide a voxel along each dimension into sub-voxels
  // (for example, into 8, 27, or 64 cubes).  This may result in many
  // more sub-voxels than units.
  unsigned voxel_divisions = static_cast<unsigned>(
    CubeRoot(static_cast<double>(num_units) / num_voxels));

  // Account for off-by-one type errors due to rounding in
  // floating point math.
  while (voxel_divisions * voxel_divisions * voxel_divisions * num_voxels < num_units)
  {
    ++voxel_divisions;
  }

  return voxel_divisions;
}

QList<unsigned>
NetworkVoxelMapper::GetSubvoxelIndexes(unsigned num_units, unsigned num_subvoxels)
{
  QList<unsigned> subvoxel_idxs;

  // Use Bresenham line algo to determine which subvoxels to use.
  assert(num_subvoxels >= num_units);
  int dx = 2 * num_subvoxels;
  int dy = 2 * num_units;
  int D = dy - num_subvoxels;

  for (unsigned idx = 0; idx < num_subvoxels; ++idx)
  {
    if (D > 0)
    {
      subvoxel_idxs << idx;
      D -= dx;
    }
    D += dy;
  }

  return subvoxel_idxs;
}

FloatTDCoord
NetworkVoxelMapper::GetCoord(unsigned subvoxel_idx, const QList<FloatTDCoord> &voxels, unsigned voxel_divisions)
{
  // Turn a subvoxel index into a voxel and x,y,z offset into that voxel.
  int temp = subvoxel_idx;
  int dz = temp % voxel_divisions;

  temp /= voxel_divisions;
  int dy = temp % voxel_divisions;

  temp /= voxel_divisions;
  int dx = temp % voxel_divisions;

  temp /= voxel_divisions;
  int voxel_idx = temp;
  assert(voxel_idx < voxels.size());

  FloatTDCoord coord(dx, dy, dz);
  coord /= static_cast<float>(voxel_divisions);
  coord += voxels[voxel_idx];

  return coord;
}
