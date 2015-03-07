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

#ifndef taProjPubInfo_h
#define taProjPubInfo_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(taProjPubInfo);

class TA_API taProjPubInfo : public taOBase {
  // <describe here in full detail in one extended line comment>
INHERITED(taOBase)
public:
  String              wiki_name;
  String              page_name;
  String              proj_name;
  String              proj_filename;
  String              proj_author;
  String              proj_email;
  String              proj_desc;
  String              proj_version;
  String              proj_keywords;
  
  TA_SIMPLE_BASEFUNS(taProjPubInfo);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // taProjPubInfo_h
