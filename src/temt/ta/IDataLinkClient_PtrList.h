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

#ifndef IDataLinkClient_PtrList_h
#define IDataLinkClient_PtrList_h 1

// parent includes:
#include <taPtrList>
#include <IDataLinkClient>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(IDataLinkClient_PtrList);

class TA_API IDataLinkClient_PtrList: public taPtrList<IDataLinkClient> {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
    ~IDataLinkClient_PtrList() {} // MAKETA requires at least one member item
};

#endif // IDataLinkClient_PtrList_h
