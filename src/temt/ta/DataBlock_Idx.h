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

#ifndef DataBlock_Idx_h
#define DataBlock_Idx_h 1

// parent includes:
#include <DataBlock>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataBlock_Idx);

class TA_API DataBlock_Idx : public DataBlock {
 // #VIRT_BASE #NO_INSTANCE base class for a data source and/or sink that supports random access to data via an index
INHERITED(DataBlock)
public:
  override bool         ReadAvailable() const
  { return ((rd_itr >= 0) && (rd_itr < ItemCount())); }

  override bool         ReadNext()
  { if (rd_itr < -1) return false;  ++rd_itr;
    if (rd_itr >= ItemCount()) {rd_itr = -2; return false;}
    return ReadItem_impl(); }
  override bool         ReadItem(int idx)
  { if(idx < 0) idx = ItemCount()-1; if((idx < 0) || (idx >= ItemCount())) return false;
    rd_itr = idx;  return ReadItem_impl();}

  override bool         WriteAvailable() const
    {return ((wr_itr >= 0) && (wr_itr < ItemCount()));}
  override bool         WriteNext()
    {if (wr_itr < -1) return false;  ++wr_itr;
     if ((wr_itr == ItemCount()) && (isSinkGrowable())) {AddItem_impl(1);}
     if (wr_itr >= ItemCount()) {wr_itr = -2; return false;}
     return WriteItem_impl();}
  override bool         WriteItem(int idx)
    {if ((idx == ItemCount()) && (isSinkGrowable())) {AddItem_impl(1);}
     if(idx < 0) idx = ItemCount()-1; if ((idx < 0) || (idx >= ItemCount())) return false;
     wr_itr = idx;  return WriteItem_impl();}

  override int          ReadIndex() const {return rd_itr;}
  override int          WriteIndex() const {return wr_itr;}

  TA_ABSTRACT_BASEFUNS(DataBlock_Idx);
protected:
  int                   rd_itr;
  // an int iterator for reading: -3=ReadItem error, -2=EOF, -1=BOF, >=0 is valid item
  int                   wr_itr;
  // an int iterator for writing: -3=WriteItem error, -2=EOF, -1=BOF, >=0 is valid item

  override void         ReadItrInit() {rd_itr = -1;}
  override void         WriteItrInit() {wr_itr = -1;}
private:
  void  Copy_(const DataBlock_Idx& cp) { rd_itr = -2; }
  void                  Initialize() {rd_itr = -2; wr_itr = -2;}
  void                  Destroy() {}
};

#endif // DataBlock_Idx_h
