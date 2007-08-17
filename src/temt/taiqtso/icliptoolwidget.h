// Copyright, 1995-2007, Regents of the University of Colorado,
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


// icliptoolwidget.h -- toolbar widget for copy/drag tools

#ifndef ICLIPTOOLWIDGET_H
#define ICLIPTOOLWIDGET_H

#include "taiqtso_def.h"

#include <QIcon>
#include <QPoint>
#include <QPointer>
#include <QStringList>
#include <QToolButton>
#include <QWidgetAction>

class QMimeData;

class TAIQTSO_API iClipWidgetAction: public QWidgetAction {
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
  override QWidget* 	createWidget(QWidget* parent);
};

class TAIQTSO_API iClipToolWidget: public QToolButton {
// a toolbar widget that copies to clipboard and is drag enabled
INHERITED(QToolButton)
  Q_OBJECT
public:

  bool			autoCopy() const {return m_autoCopy;}
    // whether a click automatically copies data to clipboard
  void			setAutoCopy(bool value);
  
  bool			dragEnabled() const {return m_dragEnabled;}
  void			setDragEnabled(bool value);

  iClipToolWidget(iClipWidgetAction* cwa = NULL, QWidget* parent = NULL);

  
#ifndef __MAKETA__
protected:
  QPointer<iClipWidgetAction> m_cwa;
  bool			m_autoCopy;
  bool			m_dragEnabled;
  QPoint		dragStartPosition;
  
  virtual Qt::DropActions supportedDropActions () const; // def is Copy
  
  void 			mousePressEvent(QMouseEvent* event); // override
  void 			mouseReleaseEvent(QMouseEvent* event); // override
  void 			mouseMoveEvent(QMouseEvent* event); // override
private:
  void			Init();
#endif
};

#endif

