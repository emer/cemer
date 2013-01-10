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

#include "VisRegionSpecBaseList.h"

VisRegionSpecBase* VisRegionSpecBaseList::FindRetinalRegion(VisRegionParams::Region reg) {
  for(int i=0;i<size;i++) {
    VisRegionSpecBase* fs = (VisRegionSpecBase*)FastEl(i);
    if(fs->region.region == reg)
      return fs;
  }
  return NULL;
}

VisRegionSpecBase* VisRegionSpecBaseList::FindRetinalRes(VisRegionParams::Resolution res) {
  for(int i=0;i<size;i++) {
    VisRegionSpecBase* fs = (VisRegionSpecBase*)FastEl(i);
    if(fs->region.res == res)
      return fs;
  }
  return NULL;
}

VisRegionSpecBase* VisRegionSpecBaseList::FindRetinalRegionRes(VisRegionParams::Region reg,
                                                       VisRegionParams::Resolution res) {
  for(int i=0;i<size;i++) {
    VisRegionSpecBase* fs = (VisRegionSpecBase*)FastEl(i);
    if((fs->region.region == reg) && (fs->region.res == res))
      return fs;
  }
  VisRegionSpecBase* rval = FindRetinalRes(res);
  if(rval) return rval;
  rval = FindRetinalRegion(reg);
  if(rval) return rval;
  return NULL;
}

