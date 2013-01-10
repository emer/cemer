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

#ifndef taiDataDeck_h
#define taiDataDeck_h 1

// parent includes:
#include <taiCompData>

// member includes:

// declare all other types mentioned but not required to include:

// contains sub-data's within a deck (aka Qt Stack) -- can toggle between them
class TA_API taiDataDeck : public taiCompData {
  Q_OBJECT
public:
  inline QWidget* rep() {return (QWidget*)m_rep;}

  taiDataDeck(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);

  virtual void  GetImage(int i);
};

#endif // taiDataDeck_h
