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

#ifndef ImageProcCall_h
#define ImageProcCall_h 1

// parent includes:
#include <StaticMethodCall>
#include "network_def.h"

// member includes:

// declare all other types mentioned but not required to include:
class ProgEl; // 


eTypeDef_Of(ImageProcCall);

class E_API ImageProcCall : public StaticMethodCall { 
  // call a taImageProc function
INHERITED(StaticMethodCall)
public:
  String	GetToolbarName() const override { return "img proc()"; }
  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  TA_BASEFUNS_NOCOPY(ImageProcCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

#endif // ImageProcCall_h
