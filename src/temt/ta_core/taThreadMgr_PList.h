// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef taThreadMgr_PList_h
#define taThreadMgr_PList_h 1

// parent includes:
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
class taThreadMgr; //

taTypeDef_Of(taThreadMgr_PList);

class TA_API taThreadMgr_PList : public taPtrList<taThreadMgr> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Thread list of thread managers
  //INHERITED(taPtrList<taManagedThread>)
  public:
};

#endif // taThreadMgr_PList_h
