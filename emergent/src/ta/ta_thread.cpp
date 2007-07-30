// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include "ta_thread.h"
  
int taAtomic::GetNextValue(volatile int& i, int&result, int maxval, int inc_by) {
  int old_i;
  int newval; // don't continue if success would put us out of bounds
  while (true) {
    old_i = i;
    newval = old_i + inc_by; 
    // don't continue if success would put us out of bounds
    if (newval > maxval) return false;
    if (TestAndSet(i, old_i, newval)) {
      result = newval;
      return true;
    }
  }
}

int taAtomic::GetPrevValue(volatile int& i, int&result, int minval, int dec_by) {
  int old_i;
  int newval; // don't continue if success would put us out of bounds
  while (true) {
    old_i = i;
    newval = old_i - dec_by; 
    // don't continue if success would put us out of bounds
    if (newval < minval) return false;
    if (TestAndSet(i, old_i, newval)) {
      result = newval;
      return true;
    }
  }
}
