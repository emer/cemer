// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// regexp for template searching: '<[a-z|A-Z|_]\{2,\}>'

// pdp_ti.cc: template instantiation file for pdp library


#include "pdpbase.h"
#include "pdpshell.h"
#include "netstru_extra.h"
#include "pdplog.h"

#ifdef TA_GUI
//#include "pdp_qtso.h"
#include "netstru_qtso.h"
#endif

template class taPtrList<Unit>;
template class taList<SchedItem>;
template class taList<Unit>;
template class taList<TessEl>;
template class taList<LinkPrjnConPtr>;
template class taList<GpTessEl>;
//template class taList<NetViewGraphic>;
//template class taList<NetViewScaleRange>;
template class taList<Environment>;
template class taGroup<Environment>;
template class taGroup<Event>;
template class taGroup<TypeDefault>;
template class taGroup<Layer>; // netstru.h
template class taGroup<Network>; // netstru.h
template class taGroup<Process>; // process.h
template class taGroup<PDPLog>; // pdplog.h
template class taGroup<Project>; // pdpshell.h
template class taGroup<Wizard>; // netstru.h


template class taGroup<Unit>;

template class SpecPtr<ConSpec>;
template class SpecPtr<UnitSpec>;
template class SpecPtr<ProjectionSpec>;
template class SpecPtr<EventSpec>;

#ifdef TA_GUI
template class taGroup<SelectEdit>; // pdpshell.h
template class taBasicArray<UnitViewData>; // netstru_qtso.h
  const UnitViewData UnitViewData_BArray::blank;
template class taPtrList<LogView>; // pdplog.h


#endif

