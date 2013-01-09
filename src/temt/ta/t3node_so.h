// Copyright, 1995-2007, Regents of the University of Colorado,
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

#ifndef T3NODE_SO_H
#define T3NODE_SO_H

#include "icolor.h"
#include "igeometry.h"
#include "ta_matrix.h"
#include "colorscale.h"
#include "minmax.h"

#include "ta_def.h"

#ifdef __MAKETA__
//dummy defines
#define SoSFEnum int
#define SoSFVec2f int
#define uint32_t uint

#else
#include <Inventor/SbColor.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoImage.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTriangleStripSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/engines/SoCalculator.h>
#endif

// todo: need a common .h for defining colors etc

////////////////////////////////////////////////////////////////////////////////
//				Draggers!

// official defined colors for dragger controls: inactive = violet
// (set but diffuse and emissive)
const float drag_inact_clr_r = .5f;
const float drag_inact_clr_g = .3f;
const float drag_inact_clr_b = .5f;
const float drag_inact_clr_tr = 0.5f; // transparency

// official defined colors for dragger controls: active = yellow
// (set but diffuse and emissive)
const float drag_activ_clr_r = .5f;
const float drag_activ_clr_g = .5f;
const float drag_activ_clr_b = .0f;
const float drag_activ_clr_tr = 0.0f; // transparency

// official defined colors for background frames: transparent emerald (only diffuse)
const float frame_clr_r = .0f;
const float frame_clr_g = .5f;
const float frame_clr_b = .5f;
const float frame_clr_tr = 0.8f; // transparency



#endif
