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



// regexp for template searching: '<[a-z|A-Z|_]\{2,\}>'

// tamisc_ti.cc: template instantiation file for ta_misc library

#include "tdgeometry.h"
#include "ta_data.h"
#include "datatable.h"
#include "datagraph.h"
#include "colorscale.h"
#ifdef TA_GUI
#include "fontspec.h"
#include "t3viewer.h"
#endif

//template class taPtrList<GraphLine>;

// tdgeometry
template class taArray<ValIdx>;
  const ValIdx ValIdx_Array::blank;

// ta_data.h

// datatable.h:
template class taList<DataItem>;
template class taGroup<DataArray_impl>;
template class DataArray<String_Matrix>;
template class DataArray<float_Matrix>;
template class DataArray<int_Matrix>;
template class DataArray<byte_Matrix>;
template class taGroup<DA_ViewSpec>; //

// datagraph.h:
template class taList<YAxisSpec>; //
template class taList<GraphletSpec>; //

//colorscale.h:
template class taList<RGBA>;
template class taList<TAColor>;
template class taGroup<ColorScaleSpec>;
template class taList<ScaleRange>;

#ifdef TA_GUI
template class taList<Xform>; // xform.h
template class taList<ViewLabel>;//fontspec.h
template class SoPtr<T3Node>; // in t3node_so.h
template class taPtrList<T3DataView>; // in t3viewer.h
template class taList<T3DataView>; // in t3viewer.h
#endif
