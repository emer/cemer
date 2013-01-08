// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef iRegexpDialogPopulator_h
#define iRegexpDialogPopulator_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:
class QStringList; // 
class QString; // 


class TA_API iRegexpDialogPopulator {
  // ##INSTANCE #NO_INSTANCE #VIRT_BASE helper class that populates regexp dialog information
public:
  virtual QStringList getHeadings(bool editor_mode, int& extra_cols) const = 0;
  virtual QStringList getLabels() const = 0;
  virtual void        setLabels(const QStringList& labels) = 0;
  virtual QString getSeparator() const = 0;
  virtual void    setSource(const void *fieldOwner) = 0;
  virtual void adjustTitle(QString &title, const void *fieldOwner) const { }
  virtual ~iRegexpDialogPopulator() { }
};

#endif // iRegexpDialogPopulator_h
