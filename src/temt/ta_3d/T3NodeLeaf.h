// Copyright 2017, Regents of the University of Colorado,
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

#ifndef T3NodeLeaf_h
#define T3NodeLeaf_h 1

// parent includes:
#include <T3Node>

// member includes:

// declare all other types mentioned but not required to include:

#ifdef TA_QT3D

class TA_API T3NodeLeaf: public T3Node {
  // a base class for major owner nodes
  Q_OBJECT
  INHERITED(T3Node)
public:
  T3NodeLeaf(Qt3DNode* parent = 0, T3DataView* dataView_ = NULL);
  ~T3NodeLeaf();
};


#else // TA_QT3D

taTypeDef_Of(T3NodeLeaf);

class TA_API T3NodeLeaf: public T3Node {
  // ##NO_INSTANCE ##NO_TOKENS  an base class for PDP project items, like networks, etc.
#ifndef __MAKETA__
typedef T3Node inherited;

  TA_SO_NODE_HEADER(T3NodeLeaf);
#endif // def __MAKETA__
public:
  static void		initClass();

  SoFont*		captionFont(bool auto_create = false) override;
  SoSeparator*		shapeSeparator() override {return this;}

  T3NodeLeaf(T3DataView* dataView_ = NULL);

protected:
  SoSeparator*		captionSeparator(bool auto_create = false) override;

  ~T3NodeLeaf();
};

#endif // TA_QT3D

#endif // T3NodeLeaf_h
