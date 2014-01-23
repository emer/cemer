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

#ifndef iClipToolWidget_h
#define iClipToolWidget_h 1

#include "ta_def.h"
#include <QToolButton>
#include <QPointer>

class iClipWidgetAction;

class TA_API iClipToolWidget: public QToolButton {
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
  
  void 			mousePressEvent(QMouseEvent* event) override;
  void 			mouseReleaseEvent(QMouseEvent* event) override;
  void 			mouseMoveEvent(QMouseEvent* event) override;
private:
  void			Init();
#endif
};

#endif // iClipToolWidget_h
