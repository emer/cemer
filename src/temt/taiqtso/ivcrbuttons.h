// Copyright, 1995-2005, Regents of the University of Colorado,
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

#ifndef IVCRBUTTONS_H
#define IVCRBUTTONS_H

#include "qtdefs.h"

#include <qpushbutton.h>
#include <qhbuttongroup.h>

class iVcrButtons: public QVButtonGroup {
  Q_OBJECT
#ifndef __MAKETA__
typedef QVButtonGroup inherited;
#endif
public:
  enum ButtonIds {
    BUT_BEG_ID,
    BUT_FREV_ID,
    BUT_REV_ID,
    BUT_STOP_ID,
    BUT_PAUSE_ID,
    BUT_PLAY_ID,
    BUT_FFWD_ID,
    BUT_END_ID,
  };

  enum Buttons {
    BUT_BEG		=0x001,
    BUT_FREV		=0x002,
    BUT_REV		=0x004,
    BUT_STOP		=0x008,
    BUT_PAUSE		=0x010,
    BUT_PLAY		=0x020,
    BUT_FFWD		=0x040,
    BUT_END		=0x080,
  };

  bool			hilight() {return mhilight;}
  virtual void		setHilight(bool value);
  iVcrButtons(QWidget* parent = 0, const char* name = 0);
  iVcrButtons(int buttons, QWidget* parent = 0, const char* name = 0);

protected:
  void		init(int buttons);
  bool mhilight; // inner margin
};


#endif

