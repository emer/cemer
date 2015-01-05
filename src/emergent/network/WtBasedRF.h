// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef WtBasedRF_h
#define WtBasedRF_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// member includes:
#include <DataTable>
#include <float_Matrix>
#include <Network>

// declare all other types mentioned but not required to include:

eTypeDef_Of(WtBasedRF);

class E_API WtBasedRF : public taNBase {
  // #STEM_BASE ##CAT_Network
INHERITED(taNBase)
public:

  TA_SIMPLE_BASEFUNS(WtBasedRF);
protected:
  void	UpdateAfterEdit_impl();
  void	CheckThisConfig_impl(bool quiet, bool& rval) override;

private:
  void Initialize();
  void Destroy()     { };
};

#endif // WtBasedRF_h
