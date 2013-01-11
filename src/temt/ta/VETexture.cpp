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

#include "VETexture.h"

void VETexture::Initialize() {
  mode = MODULATE;
  wrap_horiz = REPEAT;
  wrap_vert = REPEAT;
  offset = 0.0f;
  scale = 1.0f;
  rot = 0.0f;
  center = 0.0f;
  blend_color.r = 0.0f;
  blend_color.g = 0.0f;
  blend_color.b = 0.0f;
  blend_color.no_a = true;
  idx = -1;
}

// in VEWorldView.cpp: void VETexture::SetTexture(SoTexture2* sotx)


bool VETexture::NeedsTransform() {
  if(offset == 0.0f && scale == 1.0f && rot == 0.0f) return false;
  return true;
}

// in VEWorldView.cpp:  void VETexture::SetTransform(SoTexture2Transform* sotx)

void VETexture::SetTexture(SoTexture2* sotx) {
  if(fname.empty()) return;
  taMisc::TestError(this, !SoImageEx::SetTextureFile(sotx, fname),
     "Could not set texture from fname:", fname);
  if(wrap_horiz == REPEAT)
    sotx->wrapS = SoTexture2::REPEAT;
  else
    sotx->wrapS = SoTexture2::CLAMP;
  if(wrap_vert == REPEAT)
    sotx->wrapT = SoTexture2::REPEAT;
  else
    sotx->wrapT = SoTexture2::CLAMP;
  switch (mode) {
  case MODULATE:
    sotx->model = SoTexture2::MODULATE;
    break;
  case DECAL:
    sotx->model = SoTexture2::DECAL;
    break;
  case BLEND:
    sotx->model = SoTexture2::BLEND;
    break;
  case REPLACE:
    sotx->model = SoTexture2::REPLACE;
    break;
  }
  if(mode == BLEND) {
    sotx->blendColor.setValue(blend_color.r, blend_color.g, blend_color.b);
  }
}

