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

#ifndef FSLBrainAtlasFileComposer_h
#define FSLBrainAtlasFileComposer_h 1

// parent includes:
#include <BrainAtlasFileComposer>

// member includes:

// declare all other types mentioned but not required to include:
class QDomDocument;
class QDomElement; 
class QDomNode;

class EMERGENT_API FSLBrainAtlasFileComposer : public BrainAtlasFileComposer {
public:
  FSLBrainAtlasFileComposer(const QString& filename);
  virtual ~FSLBrainAtlasFileComposer();
  
  void ComposeHeader(const BrainAtlasInfo& info);
  void ComposeLabels(const QList<BrainAtlasLabel>& labels);
  bool Save();

private:
  QDomElement addElement(QDomDocument& document, QDomNode& node, const QString& tag, const QString& text=QString());
  
  QString m_filename;
  QDomDocument* m_atlas_dom;
};

#endif // FSLBrainAtlasFileComposer_h
