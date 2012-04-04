//
// Author: Kuba Ober <kuba@mareimbrium.org>
// Downloaded from: http://www.ibib.waw.pl/~winnie
//
// License: Public domain
//

#ifndef INTERCEPTOR_H
#define INTERCEPTOR_H

#include "config.h"
#include "taiqtso_def.h"
#include <qobject.h>

class QTextStream;
class QSocketNotifier;

class Interceptor : public QObject {
  Q_OBJECT
public:
  Interceptor(QObject * p = 0);
  ~Interceptor();

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

#endif
