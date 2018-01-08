// Copyright 2013-2017, Regents of the University of Colorado,
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

#ifndef taGuiWidget_QObj_h
#define taGuiWidget_QObj_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QObject>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class taGuiWidget; //


class TA_API taGuiWidget_QObj : public QObject {
 // #IGNORE this is a helper QObject that handles signals/slots
Q_OBJECT
friend class taGuiWidget;
public:

public slots:
 void          UrlAction();

protected:
  taGuiWidget*  widget;

  taGuiWidget_QObj(taGuiWidget* wid) { widget = wid; }
};


#endif // taGuiWidget_QObj_h
