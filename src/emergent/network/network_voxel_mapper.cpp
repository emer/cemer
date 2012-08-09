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

#include <algorithm>
#include <cassert>
#include "netstru.h" // Network, Layer, Unit
#include "ta_type.h" // taMisc::Warning()
#include "brainstru.h"

namespace { // anonymous
  // Set to 0 to turn off debug output.
  // Set to 1 to enable debug output and test brain areas.
  // Set to 2 to see all assigned voxels too.
  const int DEBUG_LEVEL = 0;
}

///////////////////////////////////////
//   NetworkVoxelMapper::LayerInfo   //
///////////////////////////////////////

class NetworkVoxelMapper::LayerInfo
{
public:
  LayerInfo(Layer *layer);
  Unit *GetUnit(unsigned idx) const;

  const Layer *lay;
  const unsigned num_units;

  double adjusted_fill_pct;
  unsigned num_subvoxels;
};

NetworkVoxelMapper::LayerInfo::LayerInfo(Layer *layer)
  : lay(layer)
  , num_units(layer->UnitAccess_NUnits(Layer::ACC_LAY))
  , adjusted_fill_pct(0.0)
  , num_subvoxels(0)
{
}

Unit *
NetworkVoxelMapper::LayerInfo::GetUnit(unsigned idx) const
{
  assert(idx < num_units);
  Unit *unit = lay->UnitAccess(Layer::ACC_LAY, idx, 0);
  // May be null, if Layer was just created and Network not rebuilt.
  return unit;
}

///////////////////////////////////////
//        NetworkVoxelMapper         //
///////////////////////////////////////

NetworkVoxelMapper::NetworkVoxelMapper(Network *network)
  : m_layer_map()
  , m_network(network)
  , m_atlas(m_network->brain_atlas)
  , m_layer_info()
{
}

NetworkVoxelMapper::~NetworkVoxelMapper()
{
  ClearLayerInfos();
}

void
NetworkVoxelMapper::AssignVoxels()
{
  if (m_network->brain_atlas.ptr() == NULL ){
    taMisc::Warning("No atlas_name specified in network;",
      "cannot map units to voxel coordinates.");
    return;
  }

  m_network->StructUpdate(true);  // prevent any upddates during mapping

  // Create a map of layer pointers.
  CreateLayerMap();

  // Iterate through brain areas.
  QList<QString> brain_areas = m_layer_map.uniqueKeys();
  foreach(const QString &brain_area, brain_areas) {
    // Get the collection of voxel coordinates associated with that
    // brain area.
    QList<FloatTDCoord> voxels = GetVoxelsInArea(brain_area);

    // Assign voxel coordinates and sizes to all units.
    AssignVoxelsInArea(brain_area, voxels);
  }

  m_network->StructUpdate(false); // trigger update after mapping
}

void
NetworkVoxelMapper::CreateLayerMap()
{
  // Walk network layers and map brain areas to layer pointers.
  for (int lay_idx = 0; lay_idx < m_network->layers.leaves; ++lay_idx) {
    if (Layer *layer = m_network->layers.Leaf(lay_idx)) {
      // Ignore lesioned and iconified layers.
      if (layer->lesioned() || layer->Iconified()) continue;

      // If no brain_area was specified for this layer, warn the user,
      // but continue to add its units to the map so they can be set
      // to zero size later.
      if (layer->brain_area.empty()) {
        // do not warn: mapping often called on nets with nothing set, and some
        // you just don't want to map in any case.  it is easy to see if empty..
        //taMisc::Warning("No brain_area specified for layer", layer->name,
        //  "; units will not be rendered in BrainView.");
      }

      m_layer_map.insert(layer->brain_area.toQString(), layer);
    }
  }
}

// Get the collection of voxels associated with the given brain area,
// in an atlas-specific manner.
QList<FloatTDCoord>
NetworkVoxelMapper::GetVoxelsInArea(QString brain_area)
{
  QList<FloatTDCoord> voxels;

  if (DEBUG_LEVEL > 0) {
    // Some test brain areas.
    if (brain_area == "TEST0") {
      for (int x = -2; x < 0; ++x) {
        for (int y = -2; y < 0; ++y) {
          for (int z = 70; z < 71; ++z) {
            voxels << FloatTDCoord(x, y, z);
          }
        }
      }
      return voxels;
    }
    if (brain_area == "TEST1") {
      for (int x = 0; x < 20; ++x) {
        for (int y = 0; y < 20; ++y) {
          for (int z = 50; z < 51; ++z) {
            voxels << FloatTDCoord(x, y, z);
          }
        }
      }
      return voxels;
    }
    if (brain_area == "TEST2") {
      for (int x = -40; x < 0; x += 4) {
        for (int y = 0; y < 40; y += 4) {
          for (int z = 0; z < 40; z += 10) {
            voxels << FloatTDCoord(x, y, z);
          }
        }
      }
      return voxels;
    }
  }

  // Return no voxels if no brain area was specified.
  if (!brain_area.isEmpty()) {
    // Get the list of voxels, first by their i,j,k indices, then convert
    // to x,y,z coordinates.
    voxels = m_atlas->Atlas().VoxelCoordinates(brain_area);
  }

  return voxels;
}

void
NetworkVoxelMapper::AssignVoxelsInArea(QString brain_area, QList<FloatTDCoord> voxels)
{
  // Each unit's voxels' coordinates and sizes will be based on:
  // * the layer's percent-fill parameter (voxel_fill_pct)
  // * how many layers map to the same brain area
  // * the number of units vs. voxels per brain area

  // Get the list of layers that need voxel assignments for this brain area.
  QList<Layer *> layers = m_layer_map.values(brain_area);

  // Get the pool of voxels to use for this brain area.
  unsigned num_voxels = voxels.size();
  if (HandleEmptyBrainArea(num_voxels, brain_area, layers)) {
    return;
  }

  // Make a list of LayerInfos to collect data as we calculate it for
  // the current set of layers.
  MakeLayerInfos(layers);

  // Determine how much coverage of the brain area is requested by the layers.
  double total_pct = ComputeLayerPercentages();

  // Take care of any layers that have their percent fill parameter set to 0.
  RemoveZeroFillLayers();

  // Get the number of subdivisions each voxel needs along each dimension
  // in order to accomodate the number of units in each layer.
  // IOW, a voxel will be divided into 1, 8, 27, or 64, sub-voxels.
  // (This may result in many more sub-voxels than units in some cases.)
  unsigned num_subvoxels = 0; // out param.
  unsigned voxel_divisions = GetVoxelDivisions(num_voxels, num_subvoxels);
  unsigned needed_subvoxels = GetNeededSubvoxelCount(num_subvoxels);

  if (DEBUG_LEVEL > 0) {
    std::cout
      << "Area: " << brain_area.toStdString()
      << "\n  num_voxels: " << num_voxels
      << "\n  voxel_divisions: " << voxel_divisions
      << "\n  num_subvoxels: " << num_subvoxels
      << "\n  total_pct: " << total_pct
      << "\n  needed_subvoxels: " << needed_subvoxels
      << std::endl;
  }

  // Get a list of subvoxel indices to be assigned to units.  This produces
  // a well-distributed list of subvoxels for *all* layers to share.
  QList<unsigned> subvoxel_idxs =
    GetSubvoxelIndexes(needed_subvoxels, num_subvoxels);

  AssignVoxelsToLayers(voxels, subvoxel_idxs, voxel_divisions);
}

void
NetworkVoxelMapper::AssignVoxelsToLayers(
  QList<FloatTDCoord> voxels,
  QList<unsigned> subvoxel_idxs,
  unsigned voxel_divisions)
{
  // Determine the size to render the (sub)voxel cubes in mm per edge.
  double voxel_size = 1.0 / voxel_divisions;

  // Assign voxels to each unit.
  // Initialize "idx" outside the main loop since it indexes into the
  // subvoxel_idxs list -- i.e., use each subvoxel exactly once.
  unsigned idx = 0;
  foreach(LayerInfo *li, m_layer_info) {
    if (DEBUG_LEVEL > 0) {
      std::cout << "\n  li->num_subvoxels: " << li->num_subvoxels
                << "\n  li->num_units: " << li->num_units << std::endl;
    }

    // The current layer has some number of units and some number of
    // subvoxels to be assigned to it.  Use Bresenham's algo again to
    // determine which subvoxels go for each unit.
    assert(li->num_subvoxels >= li->num_units);

    // Initialize D differently here, since we want all units to get
    // an approximately equal number of voxels, vs. having half-filled
    // units at the front and back.
    int dx = 2 * li->num_subvoxels;
    int dy = 2 * li->num_units;
    int D = dy - dx + 1;

    // Point at the first unit in the layer to start with, then when
    // the algo "bumps" the Y dimension, move on to the next unit.
    unsigned unit_idx = 0;
    Unit *unit = li->GetUnit(unit_idx);
    unit->voxels.Reset();

    // Repeat until all subvoxels for this layer have been assigned.
    for (unsigned count = 0; count < li->num_subvoxels; ++count) {
      // Get the next subvoxel index.
      unsigned subvoxel_idx = subvoxel_idxs[idx++];

      // Make the voxel.
      Voxel *voxel = new Voxel;
      voxel->coord = GetCoord(subvoxel_idx, voxels, voxel_divisions);
      voxel->size = unit->lesioned() ? 0.0 : voxel_size;

      // Add it to this unit's voxel list.
      unit->voxels.Add(voxel);

      if (DEBUG_LEVEL > 1) {
        std::cout << "  Added voxel to unit " << li->lay->name.chars()
          << "(" << unit_idx << "): vox# " << count << ": " << *voxel
          << std::endl;
      }

      if (D > 0) {
        D -= dx;

        // Move to next unit.  Do a bounds check before updating the pointer.
        if (++unit_idx >= li->num_units) {
          // This normally happens before the loop exits properly, but check
          // for the (hopefully impossible) case where the algorithm doesn't
          // get to assign all voxels to units.
          if (count + 1 != li->num_subvoxels) {
            std::ostringstream os;
            os << "AssignVoxelsToLayers: algo terminated prematurely ("
               << count << " / " << li->num_subvoxels
               << ") with " << li->num_units << " units."
               << std::endl;
            taMisc::Warning(os.str().c_str());
          }
          break;
        }

        // Update the pointer and reset the new unit's voxel list.
        unit = li->GetUnit(unit_idx);
        unit->voxels.Reset();
      }

      D += dy;
    }
  }
}

bool
NetworkVoxelMapper::HandleEmptyBrainArea(
  unsigned num_voxels, QString brain_area, QList<Layer *> layers)
{
  bool brain_area_is_empty = false;

  // If no voxels exist for this brain area, or no brain area label was
  // specified, then wipe out all voxel assignments.
  if (num_voxels == 0 || brain_area.isEmpty()) {
    brain_area_is_empty = true;

    // If there *is* text in the brain area, warn the user, since it's
    // possible there is a typo in the regexp (or wrong atlas used, etc.).
    if (!brain_area.isEmpty()) {
      taMisc::Warning("No voxels were found for brain area",
        brain_area.toLatin1(), "in atlas", m_network->brain_atlas->name);
    }

    // Clear all voxel assignments for units in these layers.
    foreach(Layer *layer, layers) {
      LayerInfo li(layer);
      ClearVoxelAssignmentForLayer(&li);
    }
  }

  return brain_area_is_empty;
}

void
NetworkVoxelMapper::ClearVoxelAssignmentForLayer(LayerInfo *li)
{
  // Iterate through units.
  for (unsigned idx = 0; idx < li->num_units; ++idx) {
    // Reset each unit's voxel list.
    if (Unit *unit = li->GetUnit(idx)) {
      unit->voxels.Reset();
    }
  }
}

void
NetworkVoxelMapper::MakeLayerInfos(QList<Layer *> layers)
{
  ClearLayerInfos();
  foreach(Layer *layer, layers) {
    m_layer_info << new LayerInfo(layer);
  }
}

void
NetworkVoxelMapper::ClearLayerInfos()
{
  while (!m_layer_info.isEmpty()) {
    delete m_layer_info.takeFirst();
  }
}

double
NetworkVoxelMapper::ComputeLayerPercentages()
{
  double total_pct = 0.0;
  foreach(LayerInfo *li, m_layer_info) {
    // Clip percentages to fit within the 0.0 to 1.0 range.
    double pct = li->lay->voxel_fill_pct;
    pct = std::max(pct, 0.0);
    pct = std::min(pct, 1.0);

    // Save the adjusted percent value, which will be normalized if needed.
    li->adjusted_fill_pct = pct;

    // Calculate the total of all percentages.
    total_pct += pct;
  }

  // If more than 100% coverage requested, scale back each layer's request.
  if (total_pct >= 1.0) {
    foreach(LayerInfo *li, m_layer_info) {
      li->adjusted_fill_pct /= total_pct;
    }
    total_pct = 1.0;
  }

  return total_pct;
}

void
NetworkVoxelMapper::RemoveZeroFillLayers()
{
  const double EPSILON = 0.00001;

  QMutableListIterator<LayerInfo *> iter(m_layer_info);
  while (iter.hasNext()) {
    LayerInfo *li = iter.next();
    if (li->adjusted_fill_pct < EPSILON) {
      ClearVoxelAssignmentForLayer(li);
      iter.remove();
      delete li;
    }
  }
}

unsigned
NetworkVoxelMapper::GetVoxelDivisions(
  unsigned num_voxels, unsigned &num_subvoxels)
{
  // As a default, assume no voxel splitting is needed.
  unsigned divs = 1;
  num_subvoxels = num_voxels;

  // Check each layer to see if it will have enough (sub)voxels for its units.
  foreach(LayerInfo *li, m_layer_info) {
    // Compute the number of subvoxels that will be assigned to this layer,
    // and compare that against the number of units (including lesioned ones).
    // Lesioned units still get assigned voxel coords, but since they
    // won't be drawn, "holes" will appear in the brain view.
    while (li->adjusted_fill_pct * num_subvoxels < li->num_units) {
      ++divs;
      num_subvoxels = num_voxels * divs * divs * divs;
    }
  }

  return divs;
}

unsigned
NetworkVoxelMapper::GetNeededSubvoxelCount(unsigned num_subvoxels)
{
  // Now that we know how many subdivisions there are, determine how many
  // subvoxels each layer will be assigned, and tally them all up.
  unsigned needed_subvoxels = 0;
  foreach(LayerInfo *li, m_layer_info) {
    // This is guaranteed to be >= the number of units in the layer.
    li->num_subvoxels = static_cast<unsigned>(
      li->adjusted_fill_pct * num_subvoxels);

    // Tally.
    needed_subvoxels += li->num_subvoxels;
  }

  return needed_subvoxels;
}

QList<unsigned>
NetworkVoxelMapper::GetSubvoxelIndexes(
  unsigned needed_subvoxels, unsigned num_subvoxels)
{
  // Choose evenly distributed indices for needed_subvoxels
  // out of num_subvoxels.
  QList<unsigned> subvoxel_idxs;

  // Use Bresenham line algo to determine which subvoxels to use.
  assert(num_subvoxels >= needed_subvoxels);
  int dx = 2 * num_subvoxels;
  int dy = 2 * needed_subvoxels;
  int D = dy - num_subvoxels;

  for (unsigned idx = 0; idx < num_subvoxels; ++idx) {
    if (D > 0) {
      subvoxel_idxs << idx;
      D -= dx;
    }
    D += dy;
  }

  assert(subvoxel_idxs.size() == (int)needed_subvoxels);
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
