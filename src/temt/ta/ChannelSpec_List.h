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

#ifndef ChannelSpec_List_h
#define ChannelSpec_List_h 1

// parent includes:
#include <ChannelSpec>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class DataBlock; // 


class TA_API ChannelSpec_List: public taList<ChannelSpec> {
  // ##CAT_Data a list of channel specifications
INHERITED(taList<ChannelSpec>)
public:
  void          UpdateDataBlockSchema(DataBlock* db);

  override int          NumListCols() const {return 6;}
  override const        KeyString GetListColKey(int col) const;
  override String       GetColHeading(const KeyString& key) const;
  String GetColHeadingIdx(int col) const;

  TA_BASEFUNS_NOCOPY(ChannelSpec_List);
private:
  void          Initialize() {SetBaseType(&TA_ChannelSpec);}
  void          Destroy() {}
};

#endif // ChannelSpec_List_h
