/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
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
template class taGroup<NetConduit>; // netstru.h
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
template class taPlainArray<UnitViewData>; // netstru_qtso.h
template class taPtrList<LogView>; // pdplog.h


#endif

