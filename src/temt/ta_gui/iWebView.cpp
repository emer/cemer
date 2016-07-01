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

#include <QDesktopServices>

#if (QT_VERSION >= 0x050000)
#include <QUrlQuery>
#endif

#ifdef USE_QT_WEBENGINE

#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEnginePage>

iWebUrlInterceptor::iWebUrlInterceptor(QObject *p) :
  QWebEngineUrlRequestInterceptor(p)
{
}

void iWebUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info) {
  if(info.navigationType() == QWebEngineUrlRequestInfo::NavigationTypeLink) {
    QUrl url = info.requestUrl();
    bool ta_handle = iWebView::handleTaLinkClick(url);
    if(ta_handle) {
      QUrl page = info.firstPartyUrl();
      info.redirect(page);      // send it back to page
      // info.block(true);             // block anything further from happening
    }
    // otherwise, it is just a pass-through
  }
}

QWebEngineProfile* iWebView::temt_profile = NULL;
iWebUrlInterceptor* iWebView::url_interceptor = NULL;

iWebView::iWebView(QWidget* parent) :
  inherited(parent)
{
  setPage(new QWebEnginePage(temtProfile(), this));
}

QWebEngineProfile* iWebView::temtProfile() {
  if(temt_profile) return temt_profile;
  temt_profile = new QWebEngineProfile("edu_colorado_ccnlab_emergent_webview");
  url_interceptor = new iWebUrlInterceptor();
  temt_profile->setRequestInterceptor(url_interceptor);
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
  if(url_interceptor) {
    delete url_interceptor;
    url_interceptor = NULL;
  }
}

#else // USE_QT_WEBENGINE

iWebView::iWebView(QWidget* parent) :
  inherited(parent)
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

#endif // USE_QT_WEBENGINE

void iWebView::keyPressEvent(QKeyEvent* e) {
  taiMisc::UpdateUiOnCtrlPressed(this, e);
  inherited::keyPressEvent(e);
}

bool iWebView::handleTaLinkClick(const QUrl& url) {
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

  // handle it internally
  if (!new_url.hasFragment()) {
    // todo: need viewerWindow()
    // if (viewerWindow())
    //   new_url.setFragment("#winid_" + QString::number(viewerWindow()->uniqueId()));
  }
  // goes to: iMainWindowViewer::taUrlHandler  in ta_qtviewer.cpp
  QDesktopServices::openUrl(new_url);
  return true;
}
