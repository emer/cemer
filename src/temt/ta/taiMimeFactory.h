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

#ifndef taiMimeFactory_h
#define taiMimeFactory_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiMimeFactory: public taNBase { // ##NO_CSS ##NO_MEMBERS
INHERITED(taNBase)
public:
  static const String 	text_plain;
  
  static QByteArray	StrToByteArray(const String& str);
  static QByteArray	StrToByteArray(const QString& str);
    // convenience, for converting strings to bytearrays
    
  TA_BASEFUNS_NOCOPY(taiMimeFactory);//
private:
  void	Initialize() {}
  void	Destroy() {}
};

#endif // taiMimeFactory_h
