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

// ta_ti.cc: template instantiation file for ta library

#include "ta_list.h"
#include "ta_type.h"
#ifndef NO_TA_BASE
  #include "ta_matrix.h"
  #include "ta_group.h"
  #include "ta_dump.h"
  #include "ta_script.h"
#  ifdef TA_GUI
#    include "ta_qtgroup.h"
#    include "ta_qtbrowse.h"
#    include "ta_qtviewer.h"
#    include "ta_qtclipdata.h"
#  endif
#endif

template class taPtrList<taHashEl>;
template class taPtrList<taHashBucket>;
template class taFixedArray<int>;
  const int int_FixedArray::blank = 0;
template class taPlainArray<String>;
  const String String_PArray::blank = "";
template class taPlainArray<int>;
  const int int_PArray::blank = 0;
template class taPtrList<EnumDef>;
template class taPtrList<void>;
template class taPtrList<MemberDef>;
template class taPtrList<MethodDef>;
template class taPtrList<TypeDef>;
template class taPtrList<IDataLinkClient>;

#ifndef NO_TA_BASE
template class taPtrList<taBase>;
template class taPtrList_base<taBase>;
template class taList<taBase>;
template class taArray<int>;
  const int int_Array::blank = 0;
template class taArray<float>;
  const float float_Array::blank = 0.0f;
template class taArray<double>;
  const double double_Array::blank = 0.0;
template class taArray<String>;
  const String String_Array::blank = "";
template class taArray<long>;
  const long long_Array::blank = 0L;
template class taArray<void*>;
  const voidptr voidptr_Array::blank = NULL;
template class taPtrList<VPUnref>;
template class taPtrList<DumpPathSub>;
template class taPtrList<DumpPathToken>;


// ta_matrix.h
template class taMatrix<unsigned char>;
  const unsigned char byte_Matrix::blank = 0;
template class taMatrix<float>;
  const float float_Matrix::blank = 0.0f;

template class taList<taGroup_impl>;
template class taGroup<taBase>;

template class taPtrList<ScriptBase>; // in script_base.h
template class taGroup<Script>; // in script_base.h


#ifdef TA_GUI
template class taPtrList<QWidget>;
//template class taPtrList<iWindow>;
template class taPtrList<taiDataLink>;
template class taPtrList<taiDataHost>;
//obs template class taPtrList<taiEditDialog>;
template class taPtrList<taiEditDataHost>;
template class taPtrList<taiData>;
template class taPtrList<taiMenuEl>;
template class taPtrList<taiMenu>;
template class taPtrList<taiType>;
template class taPtrList<gpiList_ElData>;
template class QPtrList<iAction>; // in ta_qtdata.h
template class taPtrList<iDataViewer>; // in ta_qt.h
template class taPtrList<taiMimeItem>; // in ta_qtclipdata.h
template class QPtrList<taiToolBar>; // in ta_qtviewer.h
template class taList<ToolBar>; // in ta_qtviewer.h
template class taPtrList<ISelectable>; // in ta_qtviewer.h
template class taPtrList<DynMethodDesc>; // in ta_qtviewer.h
template class taList<DataViewer>; // in ta_qtviewer.h
template class taPtrList<iDataPanel>; // in ta_qtviewer.h
template class taPtrList<iTabView>;
#ifdef DARWIN // for some reason, these need to be here for MacOS
template class QPtrList<QObject>;
template class QValueList<int>;
template class QValueListPrivate<int>;
template class QMemArray<char>;
#endif // def DARWIN
#endif // def TA_GUI

#endif // !def NO_TA_BASE
