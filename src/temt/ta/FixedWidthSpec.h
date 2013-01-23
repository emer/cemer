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

#ifndef FixedWidthSpec_h
#define FixedWidthSpec_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <FixedWidthColSpec_List>

// declare all other types mentioned but not required to include:
class DataTable;


TypeDef_Of(FixedWidthSpec);

class TA_API FixedWidthSpec: public taNBase {
  // ##CAT_Data ##TOKENS spec for doing a Fixed Width import of text into a DataTable
INHERITED(taNBase)
public:
  int           n_skip_lines; // #MIN_0 skip this many header lines
  FixedWidthColSpec_List col_specs; // #SHOW_TREE

  void  Load_Init(DataTable* dat); // #IGNORE called once before load
  void  AddRow(const String& ln); // add the row based on ln

  SIMPLE_LINKS(FixedWidthSpec);
  TA_BASEFUNS(FixedWidthSpec);
protected:
  DataTable* dat; // only used/valid during a load
private:
  SIMPLE_COPY(FixedWidthSpec)
  void  Initialize();
  void  Destroy() {CutLinks();}
};

TA_SMART_PTRS(FixedWidthSpec); //

#endif // FixedWidthSpec_h
