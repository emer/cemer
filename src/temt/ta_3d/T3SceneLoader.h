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

#ifndef T3SceneLoader_h
#define T3SceneLoader_h 1

// parent includes:
#include <T3Entity>

// member includes:
#include <Qt3DRenderer/QSceneLoader>

// declare all other types mentioned but not required to include:

class TA_API T3SceneLoader : public T3Entity {
  // loads a scene from a file
  Q_OBJECT
  INHERITED(T3Entity)
public:
  QUrl  source;
  Qt3D::QSceneLoader* loader;

  virtual void  setSource(const QUrl& src);
  
  T3SceneLoader(Qt3DNode* parent = 0);
  ~T3SceneLoader();
public slots:
  virtual void  updateSource(); // update to new source
};

#endif // T3SceneLoader_h
