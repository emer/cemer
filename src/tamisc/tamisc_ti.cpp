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



// regexp for TAMISC_API template searching: '<[a-z|A-Z|_]\{2,\}>'

// tamisc_ti.cc: TAMISC_API template instantiation file for ta_misc library

#include "tdgeometry.h"
#include "datatable.h"
#include "datagraph.h"
#include "colorscale.h"
#ifdef TA_GUI
//#include "fontspec.h"
#include "t3viewer.h"
#include "datatable_qtso.h"
#endif

//template class TAMISC_API taPtrList<GraphLine>;

// tdgeometry
template class TAMISC_API taArray<ValIdx>;
  const ValIdx ValIdx_Array::blank;

// datatable.h:
template class TAMISC_API taGroup<DataArray_impl>;
template class TAMISC_API DataArray<String_Matrix>;
template class TAMISC_API DataArray<Variant_Matrix>;
template class TAMISC_API DataArray<float_Matrix>;
template class TAMISC_API DataArray<int_Matrix>;
template class TAMISC_API DataArray<byte_Matrix>;
template class TAMISC_API taGroup<DA_ViewSpec>; //

// datagraph.h:
template class TAMISC_API taList<YAxisSpec>; //
template class TAMISC_API taList<GraphletSpec>; //

//colorscale.h:
template class TAMISC_API taList<RGBA>;
template class TAMISC_API taList<TAColor>;
template class TAMISC_API taGroup<ColorScaleSpec>;
template class TAMISC_API taList<ScaleRange>;

#ifdef TA_GUI
//template class TAMISC_API taList<Xform>; // xform.h
template class TAMISC_API SoPtr<T3Node>; // in t3node_so.h
template class TAMISC_API taPtrList<T3DataView>; // in t3viewer.h
template class TAMISC_API taList<T3DataView>; // in t3viewer.h

// datatable_qtso.h
template class TAMISC_API taPlainArray<Qt::Alignment>;
#endif
