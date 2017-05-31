// Copyright 2016, Regents of the University of Colorado,
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

#include "T3Misc.h"

const float T3Misc::pts_per_geom(72.0f);
const float T3Misc::geoms_per_pt(1/pts_per_geom);
const float T3Misc::char_ht_to_wd_pts(1.8f); // with proportional spacing, this is more accurate on average..
const float T3Misc::char_base_fract(0.20f); //TODO: find correct val from coin src

const float T3Misc::drag_inact_clr_r = .5f;
const float T3Misc::drag_inact_clr_g = .3f;
const float T3Misc::drag_inact_clr_b = .5f;
const float T3Misc::drag_inact_clr_tr = 0.5f; // transparency

// official defined colors for T3Misc::dragger controls: active = yellow
// (set both diffuse and emissive)
const float T3Misc::drag_activ_clr_r = .5f;
const float T3Misc::drag_activ_clr_g = .5f;
const float T3Misc::drag_activ_clr_b = .0f;
const float T3Misc::drag_activ_clr_tr = 0.0f; // transparency

// official defined colors for background frames: transparent emerald (only diffuse)
const float T3Misc::frame_clr_r = .0f;
const float T3Misc::frame_clr_g = .5f;
const float T3Misc::frame_clr_b = .5f;
const float T3Misc::frame_clr_tr = 0.8f; // transparency

QColor T3Misc::drag_inact_clr() {
  return QColor::fromRgbF(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b,
                          1.0f - drag_inact_clr_tr);
}

QColor T3Misc::drag_activ_clr() {
  return QColor::fromRgbF(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b,
                          1.0f - drag_activ_clr_tr);
}

QColor T3Misc::frame_clr() {
  return QColor::fromRgbF(frame_clr_r, frame_clr_g, frame_clr_b, frame_clr_tr);
}

uint32_t T3Misc::makePackedRGBA(float r, float g, float b, float a) {
  return makePackedRGBA((int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
}

uint32_t T3Misc::makePackedRGBA(int r, int g, int b, int a) {
  return ((r & 0xff) << 24) | ((g & 0xff) << 16) | ((b & 0xff) << 8) | (a & 0xff);
}

uint32_t T3Misc::makePackedRGBA(const QColor& clr) {
  return makePackedRGBA(clr.red(), clr.green(), clr.blue(), clr.alpha());
}

uint32_t T3Misc::makePackedRGBA(const iColor& clr) {
  return makePackedRGBA(clr.red(), clr.green(), clr.blue(), clr.alpha());
}
