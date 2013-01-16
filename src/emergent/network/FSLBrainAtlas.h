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

#ifndef FSLBrainAtlas_h
#define FSLBrainAtlas_h 1

// parent includes:
#include <BrainAtlas>

// member includes:
#include <QList>
#include <BrainAtlasLabel>
#include <taVector3f>

// declare all other types mentioned but not required to include:

class EMERGENT_API FSLBrainAtlas : public BrainAtlas {
public:
  FSLBrainAtlas(const QString& filename);
  ~FSLBrainAtlas();
  
  QString   Name() const;
  QString   AtlasFilename() const;  
  QString   Description() const;
  QString   ImageFilename() const;
  QList<BrainAtlasLabel>  Labels(const QString& labels_regexp=QString(".*")) const;
  QList<taVector3f>  VoxelCoordinates(const QString& labels_regexp=QString(".*")) const;
  void      SetLabels(const QList<BrainAtlasLabel>& labels);
  bool      Save(const QString& filename);

protected:
  FSLBrainAtlas();
  FSLBrainAtlas(const FSLBrainAtlas&);
  FSLBrainAtlas operator=(const FSLBrainAtlas&);
  
  QString m_name;
  QString m_filename;
  QString m_description;
  QString m_image_filename;
  QList<BrainAtlasLabel> m_labels;
};

#endif // FSLBrainAtlas_h
