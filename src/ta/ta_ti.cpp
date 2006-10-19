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


// regexp for TA_API template searching: '<[a-z|A-Z|_]\{2,\}>'

// ta_ti.cc: TA_API template instantiation file for ta library

#include "ta_list.h"
#include "ta_type.h"
#ifndef NO_TA_BASE
  #include "ta_matrix.h"
  #include "ta_group.h"
  #include "ta_dump.h"
  #include "ta_script.h"
  #include <QList>
#  ifdef TA_GUI
#    include "ta_qtgroup.h"
#    include "ta_qtviewer.h"
#    include "ta_qtclipdata.h"
#  endif
#endif

template class TA_API taPtrList<taHashEl>;
template class TA_API taPtrList<taHashBucket>;
template class TA_API taPlainArray<String>;
//  const String String_PArray::blank = "";
template class TA_API taPlainArray<int>;
//  const int int_PArray::blank = 0;
template class TA_API taPtrList<EnumDef>;
template class TA_API taPtrList<void>;
template class TA_API taPtrList<MemberDef>;
template class TA_API taPtrList<MethodDef>;
template class TA_API taPtrList<TypeDef>;
template class TA_API taPtrList<IDataLinkClient>;

#ifndef NO_TA_BASE
template class TA_API taFixedArray<int>;
  const int int_FixedArray::blank = 0;
template class TA_API taPtrList<taBase>;
template class TA_API taPtrList_base<taBase>;
template class TA_API taList<taBase>;
template class TA_API taArray<int>;
  const int int_Array::blank = 0;
template class TA_API taArray<float>;
  const float float_Array::blank = 0.0f;
template class TA_API taArray<double>;
  const double double_Array::blank = 0.0;
template class TA_API taArray<String>;
  const String String_Array::blank = "";
template class TA_API taArray<void*>;
  const voidptr voidptr_Array::blank = NULL;
template class TA_API taPtrList<VPUnref>;
template class TA_API taPtrList<DumpPathSub>;
template class TA_API taPtrList<DumpPathToken>;


// ta_matrix.h
template class TA_API taMatrixT<Variant>;
  const Variant Variant_Matrix::blank;
template class TA_API taMatrixT<String>;
  const String String_Matrix::blank;
template class TA_API taMatrixT<double>;
  const double double_Matrix::blank = 0.0;
template class TA_API taMatrixT<float>;
  const float float_Matrix::blank = 0.0f;
template class TA_API taMatrixT<int>;
  const int int_Matrix::blank = 0;
template class TA_API taMatrixT<unsigned char>;
  const unsigned char byte_Matrix::blank = '\0';
template class TA_API taMatrixT<rgb_t>;
  const rgb_t rgb_Matrix::blank;

template class TA_API taList<taGroup_impl>;
template class TA_API taGroup<taBase>;

template class TA_API taPtrList<ScriptBase>; // in script_base.h
// template class TA_API taGroup<Script>; // in script_base.h

#ifdef TA_OS_MAC // for some reason, these need to be here for MacOS
  template class TA_API QList<QObject*>;
  template class TA_API QList<int>;
#endif // def MAC

#ifdef TA_GUI
template class TA_API taPtrList<QWidget>;
//template class TA_API taPtrList<iWindow>;
template class TA_API taPtrList<taiDataLink>;
template class TA_API taPtrList<taiDataHost>;
//obs template class TA_API taPtrList<taiEditDialog>;
template class TA_API taPtrList<taiEditDataHost>;
template class TA_API taPtrList<taiData>;
template class TA_API taPtrList<taiAction>;
template class TA_API taPtrList<taiActions>;
template class TA_API taPtrList<taiType>;
template class TA_API taPtrList<gpiList_ElData>;
template class TA_API taPtrList<iDataViewer>; // in ta_qt.h
template class TA_API taPtrList<taiMimeItem>; // in ta_qtclipdata.h
template class TA_API QList<taiToolBar*>; // in ta_qtviewer.h
template class TA_API taList<ToolBar>; // in ta_qtviewer.h
template class TA_API taPtrList<ISelectable>; // in ta_qtviewer.h
template class TA_API taPtrList<DynMethodDesc>; // in ta_qtviewer.h
template class TA_API taList<DataViewer>; // in ta_qtviewer.h
template class TA_API taPtrList<iDataPanel>; // in ta_qtviewer.h
template class TA_API taPtrList<iTabView>;
#endif // def TA_GUI

#endif // !def NO_TA_BASE
