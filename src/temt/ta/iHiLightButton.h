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

#ifndef iHiLightButton_h
#define iHiLightButton_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QPushButton>
#endif

// member includes:
#include <taString>
#include <iColor>

// declare all other types mentioned but not required to include:

class TA_API iHiLightButton: public QPushButton {
  // ##IGNORE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS combines v3.2 Script Button and iHiLightButton -- set script_ to script name, to enable ScriptButton functionality
public:
  int                   mouse_button;   // (Qt::Button) the mouse button that pressed the button (defaults to NoButton, when actuated by keyboard)

  bool                  hiLight() {return mhiLight;} // #GET_HiLight
  virtual void          setHiLight(bool value); // #SET_HiLight
  iHiLightButton(QWidget* parent = 0, const char* script_ = 0);
  iHiLightButton(const String& text, QWidget* parent, const char* script_ = 0);
  ~iHiLightButton();

  QSize                 sizeHint() const; // override

protected:
  iColor                mhiLight_color;
  bool                  mhiLight;
  String                mscript;

  void                  init(const char* script_); // #IGNORE
  void                  released(); // override

};


#endif // iHiLightButton_h
