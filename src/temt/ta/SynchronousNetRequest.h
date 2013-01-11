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

#ifndef SynchronousNetRequest_h
#define SynchronousNetRequest_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QObject>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class QNetworkAccessManager;
class QNetworkReply;
class QUrl;

// This class provides a synchronous way to perform HTTP requests.
// It wraps the QNetworkAccessManager, QNetworkRequest, and QNetworkReply
// functionality provided by Qt so the user doesn't need to explicitly set
// up signals and slots.  It is possible, though not required, to connect
// a signal to this class's cancel() slot.  Also, a subclass may override
// isAborted() to return true if some external condition indicates the
// transfer should be aborted.

class SynchronousNetRequest : public QObject
{
  Q_OBJECT

public:
  SynchronousNetRequest(QNetworkAccessManager *qnam = 0);
  virtual ~SynchronousNetRequest();

  // Operations to perform HTTP requests (GET or POST).
  // As a convenience, if the request was successful, the reply object
  // is returned.  If the request failed, it is still possible to get
  // the reply object using getReply(), in order to check for errors.
  // If the request was cancelled, getReply() will return null.
  QNetworkReply * httpGet(const QUrl &url);
  QNetworkReply * httpPost(const QUrl &url);
  QNetworkReply * httpPost(const QUrl &url, const char *filename);
  QNetworkReply * httpPost(const QUrl &url, const char *data, int size);

  QNetworkReply * getReply();
  QNetworkReply * getReplyIfSuccess();

public slots:
  void cancel();

private slots:
  void finished();

private:
  void reset();
  void waitForReply();
  virtual bool isAborted();

  QNetworkAccessManager *m_netManager;
  QNetworkReply *m_reply;
  bool m_isCancelled;
  bool m_isFinished;
};
#endif

#endif // SynchronousNetRequest_h
