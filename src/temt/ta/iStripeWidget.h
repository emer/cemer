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

#ifndef iStripeWidget_h
#define iStripeWidget_h 1

#include "ta_def.h"

#include <qsize.h>
#include <QWidget>


class TA_API iStripeWidget: public QWidget { // #IGNORE provides a horizontal striped background, to highlight items
public:
  QSize 		minimumSizeHint () const override;
  void 			setColors(const QColor& hilight, const QColor& bg); 
    // convenient way to set both
  void			setHiLightColor(const QColor& val);
  inline int		stripeHeight() const {return mstripeHeight;}
  void			setStripeHeight(int val);
  void			setTopMargin(int val);
  void			setBottomMargin(int val); // helpful to insure room for scrollbars

  void			clearLater(); // delete all objects

  iStripeWidget(QWidget* parent = NULL);
  ~iStripeWidget();
protected:
  QColor		mhiLightColor;
  int			mstripeHeight; // default is 25;
  int			mtopMargin; // default is 0;
  int			mbottomMargin; // default is 0, suggest: 15;

  void		paintEvent(QPaintEvent* pev) override;
};


#endif // iStripeWidget_h
