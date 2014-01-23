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

#ifndef iBaseClipWidgetAction_h
#define iBaseClipWidgetAction_h 1

// parent includes:
#include "ta_def.h"
#include <iClipWidgetAction>

// member includes:
#include <taString>
#include <taSmartRef>

// declare all other types mentioned but not required to include:
class taBase; // 


class TA_API iBaseClipWidgetAction: public iClipWidgetAction {
  // for making drag/copy guys from a taBase instance (ex. see programs_qtso)
INHERITED(iClipWidgetAction)
  Q_OBJECT
public:
  taBase*               base() const {return m_inst;}


  // tooltip defaults to key_desc of the instance
  iBaseClipWidgetAction(taBase* inst = NULL, QObject* parent = NULL);
  iBaseClipWidgetAction(const QIcon & icon, taBase* inst = NULL, QObject* parent = NULL);
  iBaseClipWidgetAction(const String& tooltip, const QIcon & icon,
    taBase* inst = NULL, QObject* parent = NULL);
  iBaseClipWidgetAction(const String& text,
    taBase* inst = NULL, QObject* parent = NULL);
  iBaseClipWidgetAction(const String& tooltip, const String& text,
    taBase* inst = NULL, QObject* parent = NULL);


protected:
  taSmartRef            m_inst;

  QMimeData*   mimeData() const override; // delegates to the inst
  QStringList  mimeTypes() const override; // the ta custom type
private:
  void                  Init(taBase* inst, String tooltip = _nilString);
};

#endif // iBaseClipWidgetAction_h
