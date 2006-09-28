// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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
#include <QStringList>
#include <QToolButton>

class QMimeData;

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

  iClipToolWidget(QWidget* parent = NULL);

public slots:
  void			copyToClipboard(); // copy the mimedata to the clipboard
  
protected:
  bool			m_autoCopy;
  bool			m_dragEnabled;
  QPoint		dragStartPosition;
  
  virtual QMimeData* 	mimeData() const = 0;
  virtual QStringList   mimeTypes() const = 0;
  virtual Qt::DropActions supportedDropActions () const; // def is Copy
  
  void 			mousePressEvent(QMouseEvent* event); // override
  void 			mouseMoveEvent(QMouseEvent* event); // override
private:
  void			Init();

};

#endif

