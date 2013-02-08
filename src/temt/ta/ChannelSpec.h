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

#ifndef ChannelSpec_h
#define ChannelSpec_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class MatrixGeom; // 
class String_Matrix; // 


TypeDef_Of(ChannelSpec);

class TA_API ChannelSpec: public taNBase {
  // ##CAT_Data describes a channel of data in a DataBlock (e.g., a column of a datatable)
INHERITED(taNBase)
public:
  int                   chan_num; // #SHOW #READ_ONLY #NO_SAVE the column number (-1=at end)
  ValType               val_type; // the type of data the channel uses
  //String              disp_opts;      // viewer display options

  virtual bool          isMatrix() const {return false;}
  virtual const MatrixGeom& cellGeom() const; // #IGNORE
  virtual bool          usesCellNames() const {return false;}
  virtual const String_Matrix& cellNames() const; // #IGNORE

  TA_BASEFUNS(ChannelSpec);
private:
  void  Copy_(const ChannelSpec& cp);
  void          Initialize();
  void          Destroy() {}
};

#endif // ChannelSpec_h
