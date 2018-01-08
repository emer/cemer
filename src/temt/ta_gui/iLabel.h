// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef iLabel_h
#define iLabel_h 1

#include "ta_def.h"

#include <QLabel>
#include <QVariant>

class IWidgetHost; //

class TA_API iLabel: public QLabel {
  // enhanced label
  Q_OBJECT
INHERITED(QLabel)
public:
  IWidgetHost*          host; // dialog or edit panel that this belongs to (optional)

  bool			highlight() {return mhighlight;}
  int			index() {return mindex;}
  inline QVariant	userData() const {return muser_data;}
    // put anything you want here
  void			setUserData(const QVariant& value);
  virtual void          updateBgColor();

  iLabel(QWidget* parent = 0);
  iLabel(const QString& text, QWidget* parent); 
  iLabel(int index_, const QString& text, QWidget* parent); 

public slots:
  void			setHighlight(bool value);
  void			setLighten(bool value);
  
#ifndef __MAKETA__
signals:
  void			contextMenuInvoked(iLabel* sender, QContextMenuEvent* e);
#endif

protected:
  bool 			mhighlight;
  bool 			mlighten;
  int			mindex;
  QVariant		muser_data;

  void 	contextMenuEvent (QContextMenuEvent* e) override; 
private:
  void		init();
};

#endif // iLabel_h
