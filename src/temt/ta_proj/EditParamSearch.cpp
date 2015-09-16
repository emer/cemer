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

#include "EditParamSearch.h"

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(EditParamSearch);

void EditParamSearch::Initialize() {
  search = false;
  srch = NO;
  min_val = 0.0f;
  max_val = 1.0f;
  next_val = 0.0f;
  incr = 0.1f;
  record = false;
}

void EditParamSearch::Destroy() {
}

void EditParamSearch::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) {
		taVersion v783(7, 8, 3);
		if (taMisc::loading_version < v783) { // update from enum to bool..
			if (srch == SET) {
				search = false;
				record = true;
			}
			if (srch == SRCH) {
				search = true;
				record = true;
			}
		}
  }
  if (search) record = true;
}
