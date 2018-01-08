// Copyright 2013-2017, Regents of the University of Colorado,
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
class iPanelOfDocView;

#ifdef USE_QT_WEBENGINE
#include <QWebEngineView>
#include <QWebEnginePage>
class QWebEngineProfile;
class QWebEngineDownloadItem;
class QAuthenticator;
#elif USE_QT_WEBVIEW
#include <QWebView>
#else // USE_QT_WEBENGINE
#include <iTextEdit>
#endif
#endif // __MAKETA__

// member includes:

// declare all other types mentioned but not required to include:

#ifdef USE_QT_WEBENGINE

class iWebPage: public QWebEnginePage {
  Q_OBJECT
  INHERITED(QWebEnginePage);
public:
  
  iWebPage(QWidget* parent = Q_NULLPTR);
  iWebPage(QWebEngineProfile *profile, QObject *parent = Q_NULLPTR);
    
protected:
  bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};


class iWebView: public QWebEngineView {
  Q_OBJECT
  INHERITED(QWebEngineView);
public:

  static bool handleTaLinkClick(const QUrl& url, iPanelOfDocView* docview);
  // handle special ta: resource requests for special interactive links -- returns true if it is a special ta: link and was handled, and false if a regular link to be handled by the view
  
  static QWebEngineProfile*     temtProfile();
  // returns the default temt-library web engine profile, which establishes settings and link redirection, etc

  static void           cleanupWeb();
  // get rid of our profile
  
  iPanelOfDocView*              own_docview;
  // owning doc view

  iWebView(QWidget* parent = 0, iPanelOfDocView* docview = 0);

public slots:
  void  downloadRequested(QWebEngineDownloadItem*);
  void  authenticationRequired(const QUrl& url, QAuthenticator* auth);
  void  proxyAuthenticationRequired(const QUrl& url, QAuthenticator* auth, const QString& );
  
signals:
  void            sigCreateWindow(QWebEnginePage::WebWindowType type,
                                  QWebEngineView*& window);
protected:
  static QWebEngineProfile*     temt_profile;
  
  QWebEngineView* createWindow(QWebEnginePage::WebWindowType type) override;
  void          childEvent(QChildEvent* ev) override;
  void          contextMenuEvent(QContextMenuEvent *event) override;
};


/////////////////////////////////////////////////////////////////////
#elif defined(USE_QT_WEBVIEW)

class iWebView: public QWebView {
  Q_OBJECT
  INHERITED(QWebView);
public:
  static bool handleTaLinkClick(const QUrl& url, iPanelOfDocView* docview);
  // handle special ta: resource requests for special interactive links -- returns true if it is a special ta: link and was handled, and false if a regular link to be handled by the view

  static void           cleanupWeb();
  // cleanup any web stuff before exiting
  
  iPanelOfDocView*              own_docview;
  // owning doc view

  iWebView(QWidget* parent = 0, iPanelOfDocView* docview = 0);

signals:
  void          sigCreateWindow(QWebPage::WebWindowType type,
                                QWebView*& window);
protected:
  QWebView* createWindow(QWebPage::WebWindowType type) override;
  void keyPressEvent(QKeyEvent* e) override;
};

/////////////////////////////////////////////////////////////////////
#else // no web browser

class iWebView: public iTextEdit {
  Q_OBJECT
  INHERITED(iTextEdit);
public:
  static bool handleTaLinkClick(const QUrl& url, iPanelOfDocView* docview);
  // handle special ta: resource requests for special interactive links -- returns true if it is a special ta: link and was handled, and false if a regular link to be handled by the view

  static void           cleanupWeb();
  // cleanup any web stuff before exiting
  
  iPanelOfDocView*              own_docview;
  // owning doc view

  QUrl url() { QUrl url; return url; }
  void load(const QUrl& url) { };

  void setHtml(const QString& text, const QUrl& url) { inherited::setHtml(text); }
  using inherited::setHtml;

  QString selectedText() { return ""; }
  
  iWebView(QWidget* parent = 0, iPanelOfDocView* docview = 0);
public slots:
  void back() { };
  void forward() { };
  void reload() { };
  void stop() { };

signals:
  void loadFinished(bool ok);
  void loadProgress(int progress);
  void loadStarted();
  void urlChanged(const QUrl& url);
  
};

#endif // USE_QT_WEBENGINE

#endif // iWebView_h
