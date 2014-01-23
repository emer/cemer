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

#ifndef taTask_List_h
#define taTask_List_h 1

// parent includes:
#include <taTask>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taTask_List);

class TA_API taTask_List : public taList<taTask> {
  // ##CAT_Thread a list of tasks
  INHERITED(taList<taTask>)
public:
  String 	GetTypeDecoKey() const CPP11_OVERRIDE { return "Task"; }
  TA_BASEFUNS_NOCOPY(taTask_List);
private:
  void	Initialize() {SetBaseType(&TA_taTask);}
  void 	Destroy()		{Reset(); }; //
};

#endif // taTask_List_h
