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


#ifndef ITEXTBROWSER_H
#define ITEXTBROWSER_H

#include "taiqtso_def.h"

#include <QTextBrowser>


class TAIQTSO_API iTextBrowser: public QTextBrowser { 
INHERITED(QTextBrowser)
  Q_OBJECT
public:
  
  override void		setSource(const QUrl& url);
  
  iTextBrowser(QWidget* parent = 0);
  ~iTextBrowser();
  
signals:
  void 			setSourceRequest(iTextBrowser* tb,
    const QUrl& url, bool& cancel);

private:
  void init();
};

#endif
