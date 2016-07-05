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

#include "iWebView.h"

#include <taiMisc>
#include <iMainWindowViewer>
#include <iPanelOfDocView>
#include <taDoc>
#include <taProject>

#if (QT_VERSION >= 0x050000)
#include <QUrlQuery>
#endif

#ifdef USE_QT_WEBENGINE

#include <QWebEngineProfile>
#include <QWebEngineSettings>

iWebPage::iWebPage(QWidget* parent) :
  inherited(parent)
{
}

iWebPage::iWebPage(QWebEngineProfile *profile, QObject* parent) :
  inherited(profile, parent)
{
}

bool iWebPage::acceptNavigationRequest(const QUrl &url, NavigationType type,
                                       bool isMainFrame) {
  QObject* own = parent();
  iPanelOfDocView* dv = NULL;
  if(own->inherits("iWebView")) {
    iWebView* wv = (iWebView*)own;
    dv = wv->own_docview;
  }
  bool ta_handle = iWebView::handleTaLinkClick(url, dv);
  if(ta_handle) {
    return false;
  }
  return inherited::acceptNavigationRequest(url, type, isMainFrame);
}


QWebEngineProfile* iWebView::temt_profile = NULL;

iWebView::iWebView(QWidget* parent, iPanelOfDocView* docview)
  : inherited(parent)
  , own_docview(docview)
{
  setPage(new iWebPage(temtProfile(), this));
}

QWebEngineProfile* iWebView::temtProfile() {
  if(temt_profile) return temt_profile;
  temt_profile = new QWebEngineProfile("edu_colorado_ccnlab_emergent_webview");
  temt_profile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
  temt_profile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
  // QWebEngineSettings* set = temt_profile->settings();
  // todo: could set font size etc
  return temt_profile;
}

QWebEngineView* iWebView::createWindow(QWebEnginePage::WebWindowType type) {
  QWebEngineView* rval = NULL;
  emit sigCreateWindow(type, rval);
  if (!rval)
    rval = inherited::createWindow(type);
  return rval;
}

void iWebView::cleanupWeb() {
  if(temt_profile) {
    delete temt_profile;
    temt_profile = NULL;
  }
}

// this is needed to actually get event filter on a web view
void iWebView::childEvent(QChildEvent* ev) {
  if(own_docview) {
    if(ev->added()) {
      ev->child()->installEventFilter(own_docview);
    }
  }
}


#else // USE_QT_WEBENGINE

iWebView::iWebView(QWidget* parent, iPanelOfDocView* docview)
  : inherited(parent)
  , own_docview(docview)
{
}

QWebView* iWebView::createWindow(QWebPage::WebWindowType type) {
  QWebView* rval = NULL;
  emit sigCreateWindow(type, rval);
  if (!rval)
    rval = inherited::createWindow(type);
  return rval;
}

void iWebView::cleanupWeb() {
  
}

void iWebView::keyPressEvent(QKeyEvent* e) {
  taiMisc::UpdateUiOnCtrlPressed(this, e);
  inherited::keyPressEvent(e);
}

#endif // USE_QT_WEBENGINE

bool iWebView::handleTaLinkClick(const QUrl& url, iPanelOfDocView* docview) {
  String path = url.toString();
  bool ta_path = false;
  QUrl new_url(url);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq(url);
#else
#endif
  if(path.startsWith("ta:") || path.startsWith(".")) {
    ta_path = true;
  }
  String qry;
#if (QT_VERSION >= 0x050000)
  if(!ta_path && urq.hasQueryItem("title")) { // wiki versions of our action urls get translated into queries with title=...
    qry = urq.queryItemValue("title");
  }
#else
  if(!ta_path && url.hasQueryItem("title")) { // wiki versions of our action urls get translated into queries with title=...
    qry = url.queryItemValue("title");
  }
#endif
  if(qry.nonempty()) {
    if(qry.startsWith("ta:") || qry.startsWith(".")) {
      if(!qry.startsWith("ta:"))
        qry = "ta:"+qry;        // rectify
      new_url.setUrl(qry);      // start from there.
      ta_path = true;
    }
  }
  if(!ta_path) {
    return false;               // we don't do anything with it
  }

  iMainWindowViewer* imv = NULL;
  if(docview && docview->doc()) {
    taDoc* doc = docview->doc();
    taProject* proj = GET_OWNER(doc, taProject);
    if(proj) {
      MainWindowViewer* mwv = proj->GetDefaultProjectBrowser();
      if(mwv) {
        imv = mwv->widget();
      }
    }
  }
  else {
    imv = taiMisc::active_wins.Peek_MainWindow();
  }
  // tag with window id:
  if (!new_url.hasFragment()) {
    new_url.setFragment("#winid_" + QString::number(imv->uniqueId()));
  }

  // this will send the message over the thread system to the window slot!
  QMetaObject::invokeMethod(imv, "taUrlHandler",
                            Q_ARG(const QUrl&, new_url));

  return true;
}

