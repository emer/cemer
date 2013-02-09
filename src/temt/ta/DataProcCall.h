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

#ifndef DataProcCall_h
#define DataProcCall_h 1

// parent includes:
#include <StaticMethodCall>

// member includes:

// declare all other types mentioned but not required to include:
class ProgEl; // 


taTypeDef_Of(DataProcCall);

class TA_API DataProcCall : public StaticMethodCall { 
  // call a taDataProc (data processing/database) function
INHERITED(StaticMethodCall)
public:
  override String	GetToolbarName() const { return "data proc()"; }
  override bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const;

  TA_BASEFUNS_NOCOPY(DataProcCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

#endif // DataProcCall_h
