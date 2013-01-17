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

#include "FSLBrainAtlas.h"
#include <FSLBrainAtlasFileParser>
#include <FSLBrainAtlasFileComposer>
#include <BrainAtlasInfo>
#include <NiftiReader>

#include <taMisc>

#include <QDomElement>

// todo: assert is not the best way to deal with errors in emergent..
#include <cassert>


FSLBrainAtlas::FSLBrainAtlas(const QString& filename) : BrainAtlas(),
  m_name(""),
  m_filename(""),
  m_description(""),
  m_image_filename("")
{
  m_labels.clear();
  
  // attempt to parse file header
  FSLBrainAtlasFileParser parser(filename);
  BrainAtlasInfo info(parser.ParseHeader());
  m_name = info.name;
  m_filename = info.atlas_filename;
  m_description = info.description;
  m_image_filename = info.image_filename;
  
  // parse labels and populate lists
  m_labels = parser.ParseLabels();
}

FSLBrainAtlas::~FSLBrainAtlas()
{
    m_labels.clear();
}
  
QString FSLBrainAtlas::Name() const
{
  return m_name;
}
QString FSLBrainAtlas::AtlasFilename() const
{
  return m_filename;
}

QString FSLBrainAtlas::Description() const
{
  return m_description;
}

QString FSLBrainAtlas::ImageFilename() const
{
  return m_image_filename;
}

QList<BrainAtlasLabel> FSLBrainAtlas::Labels(const QString& labels_regexp) const
{
  // Labels are fully populated during constructor's parsing
  // of file. This method returns a subset of those labels.
  QList<BrainAtlasLabel> list; 
  QRegExp regexp(labels_regexp);

  foreach(BrainAtlasLabel label, m_labels){
    if (label.Text().contains(regexp)){
      list.append(label);
    }
  }
  return list;
}

QList<taVector3f> FSLBrainAtlas::VoxelCoordinates(const QString& labels_regexp) const
{
  // Extract all voxel coordinates which match the label regexp.
  // If the regexp does not match, voxel list is empty
  
  QList<taVector3i> index_coords;
  QList<taVector3f> voxel_coords;
  
  // We don't currently support 2mm atlas image data 
  // files since we render to a 1mm brain 
  const QRegExp regexp("2mm");
  assert(!m_image_filename.contains(regexp));
  
  // get labels that match the regexp
  QList<BrainAtlasLabel> labels = Labels(labels_regexp);

  // now get the indexes for the matching labels
  QSet<int> match_indexes; 
  foreach( BrainAtlasLabel label, labels){
    match_indexes << label.Index();
  }

  // We need a NiftiReader to read the atlas image data
  NiftiReader img(m_image_filename);
  if (img.IsValid()) {
    
    ///////////////////////////////////////////////////////////////
    // @TODO Add support for 4D probabilistic atlases?
    //       Can use img.NumVolumes() to get number of volumes
    ///////////////////////////////////////////////////////////////
    
    // Get the dimensions of the data and a pointer to the data.
    // The data has already been byte-swapped for this platform.
    taVector3i size(img.XyzDimensions());
    const short *data = reinterpret_cast<const short *>(img.RawData());
    // i,j,k are the array indexes in the x,y,z dimensions, respectively.
    for (int k = 0; k < size.z; ++k) {
      for (int j = 0; j < size.y; ++j) {
        for (int i = 0; i < size.x; ++i) {
          short label_index = *data++;
          bool match = match_indexes.contains(label_index);
          if (match) {
            index_coords << taVector3i(i, j, k);
          }
        }
      }
    }  
  }
  
  // Transform each ijk-based (index) coordinate to an xyz-based (voxel) coordinate.
  foreach(const taVector3i &ijk_coord, index_coords) {
    voxel_coords << img.IjkToXyz(ijk_coord);
  }
  return voxel_coords;
}

void FSLBrainAtlas::SetLabels(const QList<BrainAtlasLabel>& labels)
{
  if (labels.size() != 0){
    m_labels.clear();
    m_labels = labels;  
  }
}

bool FSLBrainAtlas::Save(const QString& filename)
{
  // attempt to compose the file header
  FSLBrainAtlasFileComposer composer(filename);
  String fnm = "/" + taMisc::GetFileFmPath(m_image_filename);
  BrainAtlasInfo info = {m_name, m_description, m_filename, fnm};
  composer.ComposeHeader(info);
   
  // compose labels
  composer.ComposeLabels(m_labels); 

  // save to file
  return composer.Save();
}

