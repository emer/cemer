// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

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
template class taList<DataConnector>;
template class taList<SourceChannel>;
template class taList<SinkChannel>;

// datatable.h:
template class taList<DataItem>;
template class taGroup<DataArray_impl>;
template class DataArray<float_RArray>;
template class DataArray<DString_Array>;
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
