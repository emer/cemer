// Co2018ght 2013-2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef Weights_List_h
#define Weights_List_h 1

// parent includes:
#include "network_def.h"
#include <taList>
#include <Weights>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(Weights_List);

class E_API Weights_List : public taList<Weights> {
  // #NO_TOKENS List of weights objects
INHERITED(taList<Weights>)
public:

  int  Dump_Load_Value(std::istream& strm, taBase* par=NULL) override;

  TA_BASEFUNS_NOCOPY(Weights_List);
private:
  void Initialize()  { SetBaseType(&TA_Weights); };
  void Destroy()     { };
};

#endif // Weights_List_h
