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

#include "ActrChunk.h"

void ActrChunk::Initialize() {
  n_act = 0.0f;
  t_new = 0.0f;
  base_act = 0.0f;
}

void ActrChunk::Destroy() {
  CutLinks();
}

void ActrChunk::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!chunk_type) return;
  // enforce size
  vals.SetSize(chunk_type->slots.size);
  for(int i=0; i<chunk_type->slots.size; i++) {
    vals.labels[i] = chunk_type->slots[i];
  }
}

