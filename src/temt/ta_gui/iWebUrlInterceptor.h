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

#ifndef iWebUrlInterceptor_h
#define iWebUrlInterceptor_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__

#ifdef USE_QT_WEBENGINE
#include <QWebEngineUrlRequestInterceptor>
#else // USE_QT_WEBENGINE
// nothing
#endif // USE_QT_WEBENGINE
#endif

// member includes:

// declare all other types mentioned but not required to include:

#ifdef USE_QT_WEBENGINE

class iWebUrlInterceptor : public QWebEngineUrlRequestInterceptor {
  // catch the url request and pass to a UrlHandler
  Q_OBJECT
  INHERITED(QWebEngineUrlRequestInterceptor)
public:
  bool          interceptRequest(QWebEngineUrlRequestInfo &info);
};

#endif // USE_QT_WEBENGINE

#endif // iWebUrlInterceptor_h
