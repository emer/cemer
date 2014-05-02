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

#ifndef iInterceptor_h
#define iInterceptor_h 1

#include "ta_def.h"
#include <qobject.h>

class QTextStream;
class QSocketNotifier;

class iInterceptor : public QObject {
  Q_OBJECT
public:
  iInterceptor(QObject * p = 0);
  ~iInterceptor();

public slots:
  void initialize(int outFd = 1); // defaults to standard output
  void received();
  void finish();
  QTextStream *textIStream() {return m_stream;};
signals:
  void received(QTextStream *);

private:
  QTextStream * m_stream;
  QSocketNotifier * m_notifier;
  int m_pipeFd[2];
  int m_origFd;
  int m_origFdCopy;
};

#endif // iInterceptor_h
