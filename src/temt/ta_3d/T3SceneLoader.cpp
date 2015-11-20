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

#include "T3SceneLoader.h"

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DInput;

T3SceneLoader::T3SceneLoader(Qt3DNode* parent)
  : inherited(parent)
  , loader(new QSceneLoader(this))
{
  addComponent(loader);
}

T3SceneLoader::~T3SceneLoader() {
  
}

void T3SceneLoader::setSource(const QUrl& src) {
  source = src;
  updateSource();
}

void T3SceneLoader::updateSource() {
  loader->setSource(source);
}

