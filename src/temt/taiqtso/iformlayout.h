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


/****************************************************************************
** $Id: qt/examples/customlayout/flow.h   2.3.2   edited 2001-01-26 $
**
** Definition of simple flow layout for custom layout example
**
** Created : 979899
**
** Copyright (C) 1997 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef IFORMLAYOUT_H
#define IFORMLAYOUT_H

#include "taiqtso_def.h"

#if (QT_VERSION >= 0x040400)

#include <QFormLayout>

#ifndef __MAKETA__
# include <QBitArray>
# include <QMap>
#endif

typedef QMap<int, QWidget*> QMap_int_QWidget;

class TAIQTSO_API iFormLayout : public QFormLayout {
INHERITED(QFormLayout)
public:
  bool		isVisible(int row);
  void		setVisible(int row, bool val);
  
  iFormLayout(QWidget *parent = NULL);
  ~iFormLayout();


public: // required or desired overrides

protected:
#ifndef __MAKETA__
  QBitArray		m_vis; // visible guys -- out of range implies visible
  QMap_int_QWidget	m_inv_labels; // invisible labels
  QMap_int_QWidget	m_inv_fields; // invisible fields	
#endif

private:
  void init();
};

#endif // Qt 4.4
#endif
