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

#ifndef taiTsvMimeItem_h
#define taiTsvMimeItem_h 1

// parent includes:
#include <taiTabularDataMimeItem>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiTsvMimeItem);

class TA_API taiTsvMimeItem: public taiTabularDataMimeItem { // this class handles generic TSV data, ex. from Excel and other spreadsheets
  INHERITED(taiTabularDataMimeItem)
public: // i/f for tabular data guy
  TA_BASEFUNS_NOCOPY(taiTsvMimeItem);

public: // TAI_xxx instance interface -- used for dynamic creation
  taiMimeItem* Extract(taiMimeSource* ms,
    const String& subkey = _nilString) CPP11_OVERRIDE; //NOTE: we typically only ask for this type if we *don't* get a Matrix or Table, so we don't waste time decoding manually
private:
  void  Initialize();
  void  Destroy() {}
};

#endif // taiTsvMimeItem_h
