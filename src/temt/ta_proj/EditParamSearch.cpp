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
  record = true;
  search = false;
  srch = NO;
  min_val = 0.0f;
  max_val = 1.0f;
  next_val = 0.0f;
  incr = 0.1f;
}

void EditParamSearch::Destroy() {
}

void EditParamSearch::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) {
    taVersion v783(7, 8, 3);
    if (taMisc::loading_version < v783) { // update from enum to bool..
      range = String(min_val) + ":" + String(max_val) + ":" + String(incr);
      if (srch == SET) {
        search = false;
        record = true;
      }
      else if (srch == SRCH) {
        search = true;
        record = true;
      }
      else {
        record = false;
        search = false;
      }
    }
  }
  if (search) {
    record = true;
    ParseRange();
  }
}

/**
 * Parse a string of the form "1;2;3:1:5;10:2:20" into a list of ranges.
 * Allocates a variety of memory that needs to be freed again.
 * We need to use a list of pointers as the objects aren't immutable.
 */
bool EditParamSearch::ParseRange() {
  int idx, idx2;
  int start_pos = 0;
  int start_pos2 = 0;

  srch_vals.Reset();
  
  idx = range.index(',', start_pos);

  //Iterate over all ; in the range string
  while (idx >= 0) {
    String sub_range = range.at(start_pos, idx - start_pos);
    bool ok = ParseSubRange(sub_range);
    if(!ok) return false;
    start_pos = idx + 1;
    idx = range.index(',', start_pos);
  }
  if(start_pos < range.length()) {
    String sub_range = range.after(start_pos - 1);
    bool ok = ParseSubRange(sub_range);
    if(!ok) return false;
  }
  return (srch_vals.size > 0);  // not ok if no vals.
}

/**
 * Parse a string of the form minValue:maxValue:increment into a structure.
 */
bool EditParamSearch::ParseSubRange(const String& sub_range) {
  int start_pos = 0;
  int idx = sub_range.index(':', start_pos);

  //We assume that we can either have a sigular value, or we need to specify a start value, end value and increment.
  if (idx >= 0) {
    String start_s = sub_range.at(start_pos, idx - start_pos);
    String step_s = "1";
    String end_s;
    start_pos = idx + 1;
    int sidx = sub_range.index(':', start_pos);
    if (sidx > 0) {
      end_s = sub_range.at(start_pos, sidx - start_pos);
      step_s = sub_range.after(sidx);
    }
    else {
      step_s = sub_range.after(idx); // original index
    }
    bool ok;
    double start = start_s.toDouble(&ok);
    if(TestError(!ok, "ParseSubRange",
                 "could not convert string to floating point value:", start_s)) {
      return false;
    }
    double end = end_s.toDouble(&ok);
    if(TestError(!ok, "ParseSubRange",
                 "could not convert string to floating point value:", end_s)) {
      return false;
    }
    double increment = step_s.toDouble(&ok);
    if(TestError(!ok, "ParseSubRange",
                 "could not convert string to floating point value:", step_s)) {
      return false;
    }

    if(TestError(((end < start) || ((end - start) / increment <= 0.0)),
                 "ParseSubRange",
                 "Range specified does not provide a valid set of values: " + sub_range)) {
      return false;
    }

    for(double val = start; val <= end; val += increment) {
      srch_vals.Add(val);
    }
  }
  else {
    bool ok;
    double start = sub_range.toDouble(&ok);
    if(TestError(!ok, "ParseSubRange",
                 "could not convert string to floating point value:", sub_range)) {
      return false;
    }
    srch_vals.Add(start);
  }
  return true;
}
