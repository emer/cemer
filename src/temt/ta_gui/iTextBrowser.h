// Copyright 2015, Regents of the University of Colorado,
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

#ifndef iTextBrowser_h
#define iTextBrowser_h 1

#include "ta_def.h"

#include <QTextBrowser>


class TA_API iTextBrowser: public QTextBrowser { 
INHERITED(QTextBrowser)
  Q_OBJECT
public:
  
  void		setSource(const QUrl& url) override;
  
  iTextBrowser(QWidget* parent = 0);
  ~iTextBrowser();
  
signals:
  void 			setSourceRequest(iTextBrowser* tb,
    const QUrl& url, bool& cancel);

private:
  void init();
};

#endif // iTextBrowser_h
