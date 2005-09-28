// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// this is the main that is included to make pdplib executable

#include "pdpbase.h"
#include "pdpshell.h"
#include "pdp_TA_type.h"

int main(int argc, char* argv[]) {
  pdpMisc::Init_Hook = ta_Init_pdp; // use the pdp program to startup..
  pdpMisc::def_wizard = &TA_Wizard;
  return pdpMisc::Main(argc, argv);
}
