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

#include "taGenDoc.h"
#include <TypeDef>

using namespace std;



bool taGenDoc::TypeDef_Filter_Type(TypeDef* td, TypeSpace* ts) {
  /////////////////////////////////////////////////////////////
  // 	Filters! Returns true if you should filter this TypeDef

  // We only want "actual" types, not pointers or references to types, etc...
  if(td->IsNotActual())
    return true;

  if(!td->IsClass())
    return true;

  return false;
}

void taGenDoc::GenDoc(TypeSpace* ts) {
  for(int i=0;i<ts->size;i++) {
    TypeDef* td = ts->FastEl(i);
    if (TypeDef_Filter_Type(td, ts)) continue;

    String fnm = td->name + ".html";
    String html = td->GetHTML(true); // gendoc = true
    fstream strm;
    strm.open(fnm, ios::out);
    strm << html << endl;
    strm.close();
  }
}
