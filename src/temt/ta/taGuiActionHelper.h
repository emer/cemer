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

#ifndef taGuiActionHelper_h
#define taGuiActionHelper_h 1

// parent includes:
#ifndef __MAKETA__
#include <QObject>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class taGuiAction; //


class TA_API taGuiActionHelper : public QObject {
 // #IGNORE this is a helper QObject that handles signals/slots
Q_OBJECT
friend class taGuiAction;
public:

public slots:
  void          UrlAction() { action->UrlAction(); }

protected:
  taGuiAction*  action;

  taGuiActionHelper(taGuiAction* wid) { action = wid; }
};

#endif // taGuiActionHelper_h
