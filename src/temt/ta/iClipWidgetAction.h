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

#ifndef iClipWidgetAction_h
#define iClipWidgetAction_h 1

#include "ta_def.h"

#include <QWidgetAction>
#include <QStringList>

class QMimeData;

class TA_API iClipWidgetAction: public QWidgetAction {
  // for making drag/copy guys from a taBase instance (ex. see programs_qtso)
INHERITED(QWidgetAction)
  Q_OBJECT
public:
  
  virtual QMimeData* 	mimeData() const = 0;
  virtual QStringList   mimeTypes() const = 0;
  
  iClipWidgetAction(QObject* parent = NULL);
  
public slots:
  void			copyToClipboard(); // copy the mimedata to the clipboard

protected:
  QWidget* 	createWidget(QWidget* parent) CPP11_OVERRIDE;
};

#endif // iClipWidgetAction_h
