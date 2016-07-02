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

#ifndef iWebView_h
#define iWebView_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__

#ifdef USE_QT_WEBENGINE
#include <QWebEngineView>
#include <QWebEngineUrlRequestInterceptor>
class QWebEngineProfile;
class iPanelOfDocView;
#else // USE_QT_WEBENGINE
#include <QWebView>
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

  iWebUrlInterceptor(QObject *p = Q_NULLPTR);
  
  void          interceptRequest(QWebEngineUrlRequestInfo &info);
};

class iWebView: public QWebEngineView {
  Q_OBJECT
  INHERITED(QWebEngineView);
public:

  static iPanelOfDocView*       last_docview;
  // hacky workaround to figure out who clicked link last, for interceptor
  
  static bool handleTaLinkClick(const QUrl& url);
  // handle special ta: resource requests for special interactive links -- returns true if it is a special ta: link and was handled, and false if a regular link to be handled by the view
  
  static QWebEngineProfile*     temtProfile();
  // returns the default temt-library web engine profile, which establishes settings and link redirection, etc

  static void           cleanupWeb();
  // get rid of our profile
  
  iWebView(QWidget* parent = 0, iPanelOfDocView* docview = 0);
signals:
  void            sigCreateWindow(QWebEnginePage::WebWindowType type,
                                  QWebEngineView*& window);
protected:
  static QWebEngineProfile*     temt_profile;
  static iWebUrlInterceptor*    url_interceptor;
  iPanelOfDocView*              own_docview;
  // owning doc view
  

  QWebEngineView* createWindow(QWebEnginePage::WebWindowType type) override;
  void childEvent(QChildEvent* ev) override;
};


/////////////////////////////////////////////////////////////////////
#else // USE_QT_WEBENGINE


class iWebView: public QWebView {
  Q_OBJECT
  INHERITED(QWebView);
public:
  static iPanelOfDocView*       last_docview;
  // hacky workaround to figure out who clicked link last, for interceptor
  
  static bool handleTaLinkClick(const QUrl& url);
  // handle special ta: resource requests for special interactive links -- returns true if it is a special ta: link and was handled, and false if a regular link to be handled by the view

  static void           cleanupWeb();
  // cleanup any web stuff before exiting
  
  iWebView(QWidget* parent = 0, iPanelOfDocView* docview = 0);

signals:
  void          sigCreateWindow(QWebPage::WebWindowType type,
                                QWebView*& window);
protected:
  QWebView* createWindow(QWebPage::WebWindowType type) override;
  void keyPressEvent(QKeyEvent* e) override;
  iPanelOfDocView*              own_docview;
  // owning doc view
};

#endif // USE_QT_WEBENGINE


#endif // iWebView_h
