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
typedef QMap<int, QLayout*> QMap_int_QLayout;

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
  QBitArray		m_inv; // invisible guys -- out of range implies visible
  QMap_int_QWidget	m_inv_labels; // invisible labels
  QMap_int_QLayout	m_inv_fields; // invisible fields	
#endif
  int			GetIndexFromRowRole(int row, ItemRole role); 
    // hacky method to get raw index, ex. for takeAt use
private:
  void init();
};

#endif // Qt 4.4
#endif
