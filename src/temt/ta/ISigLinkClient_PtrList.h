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

#ifndef ISigLinkClient_PtrList_h
#define ISigLinkClient_PtrList_h 1

// parent includes:
#include <taPtrList>
#include <ISigLinkClient>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(ISigLinkClient_PtrList);

class TA_API ISigLinkClient_PtrList: public taPtrList<ISigLinkClient> {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
    ~ISigLinkClient_PtrList() {} // MAKETA requires at least one member item
};

#endif // ISigLinkClient_PtrList_h
