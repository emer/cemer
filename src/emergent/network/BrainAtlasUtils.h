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

#ifndef BrainAtlasUtils_h
#define BrainAtlasUtils_h 1

// parent includes:
#include "network_def.h"

// member includes:
#include <QSet>
#include <QList>

// declare all other types mentioned but not required to include:
class QList;
class QColor;
class QString;
class QStringList;
class BrainAtlasInfo;


class EMERGENT_API BrainAtlasUtils {
  //    contains static functions useful for manipulating a BrainAtlas
public:
  
  static QList<QColor>  Colors(const BrainAtlas& atlas, const QString& labels_regex=".*");
  static QStringList    Labels(const BrainAtlas& atlas, const QString& labels_regex=".*");
  static QSet<int>      Indexes(const BrainAtlas& atlas, const QString& labelsRegexp);
  static QList<int>     IndexList(const BrainAtlas& atlas, const QString& labelsRegexp);
  static QString        Label(const BrainAtlas& atlas, unsigned int index);
  static unsigned int   Index(const BrainAtlas& atlas, const QString& label);
  static QColor   	Color(const BrainAtlas& atlas, unsigned int index);

  static QStringList    AtlasesAvailable(const QString& atlas_dir);
  static BrainAtlasInfo ParseAtlasHeader(const QString& filename);
  
  static void           SetLabels(BrainAtlas& atlas, const QStringList& labels);
  static void           SetColors(BrainAtlas& atlas, const QList<QColor>& colors);
  static void   	SetColor(BrainAtlas& atlas, QColor clr, unsigned int index);
};  

#endif // BrainAtlasUtils_h
