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

#include "Weights_List.h"
#include <Network>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(Weights_List);

int Weights_List::Dump_Load_Value(std::istream& strm, taBase* par) {
  int rval = inherited::Dump_Load_Value(strm, par);
  Network* net = GET_MY_OWNER(Network);
  if(net) {
    // we show up during loading after all the units have been processed, 
    // so we are the trigger for allocating based on what has been specified
    // so far!  rval == 2 is for first pass..
    if(rval == 2) {
      net->UpdtAfterNetMod();
      net->Connect_Alloc();       // take the network
    }
    else {
      net->Init_Weights_post();
      net->Connect_VecChunk(); 
    }      
  }
  return rval;
}
