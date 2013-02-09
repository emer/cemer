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

#ifndef BrainAtlasRegexpPopulator_h
#define BrainAtlasRegexpPopulator_h 1

// parent includes:
#include "network_def.h"
#include <iDialogRegexpPopulator>

// member includes:
#ifndef __MAKETA__
#include <QList>
#include <QColor>
#include <QStringList>
#endif

// declare all other types mentioned but not required to include:
class taBrainAtlas; //

eTypeDef_Of(BrainAtlasRegexpPopulator);

class E_API BrainAtlasRegexpPopulator : public iDialogRegexpPopulator {
public:
  BrainAtlasRegexpPopulator();
  override QStringList getHeadings(bool editor_mode, int& extra_cols) const;
  override QStringList getLabels() const;
  override void setLabels(const QStringList& labels);
  override QString getSeparator() const;
  override void    setSource(const void *fieldOwner);
  override void adjustTitle(QString &title, const void *fieldOwner) const;

  QList<QColor> getColors() const;
  void setColors(const QList<QColor>& colors);
private:
  mutable taBrainAtlas* m_atlas;
  mutable QStringList m_labels;
  mutable QStringList m_headings;
  mutable QString m_filepath;
  mutable QList<QColor> m_colors;
};

#endif // BrainAtlasRegexpPopulator_h
