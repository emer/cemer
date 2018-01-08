// Copyright 2013-2018, Regents of the University of Colorado,
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
#include <taMisc>

#if (QT_VERSION >= 0x050000)
#include <QUrlQuery>
#endif

#ifdef USE_QT_WEBENGINE

#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QContextMenuEvent>
#include <QWebEngineDownloadItem>
#include <QStandardPaths>
#include <QFileDialog>
#include <iNetworkAccessManager>

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
  QWebEnginePage* pg = new iWebPage(temtProfile(), this);
  setPage(pg);
  connect(temt_profile, SIGNAL(downloadRequested(QWebEngineDownloadItem*)),
          this, SLOT(downloadRequested(QWebEngineDownloadItem*)));
  connect(pg, SIGNAL(authenticationRequired(const QUrl &, QAuthenticator*)),
          this,
          SLOT(authenticationRequired(const QUrl &, QAuthenticator*)));
  connect
    (pg,
     SIGNAL(proxyAuthenticationRequired(const QUrl &, QAuthenticator *, const QString &)),
     this,
     SLOT(proxyAuthenticationRequired(const QUrl &, QAuthenticator *, const QString &)));
}

QWebEngineProfile* iWebView::temtProfile() {
  if(temt_profile) return temt_profile;
  temt_profile = new QWebEngineProfile("edu_colorado_ccnlab_emergent_webview");
  temt_profile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
  temt_profile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

  QWebEngineSettings* set = temt_profile->settings();
  int brow_fs = taMisc::GetCurrentFontSize("browser");
  set->setFontSize(QWebEngineSettings::DefaultFontSize, brow_fs);
  set->setFontSize(QWebEngineSettings::DefaultFixedFontSize, brow_fs);
  return temt_profile;
}

void iWebView::downloadRequested(QWebEngineDownloadItem* down) {
  if(!down || down->state() != QWebEngineDownloadItem::DownloadRequested)
    return;
  QString defaultLocation =
    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

  QString defaultFileName = QFileInfo
    (defaultLocation, QFileInfo(down->path()).fileName()).absoluteFilePath();
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                  defaultFileName);
  if (fileName.isEmpty()) {
    down->cancel();
    return;
  }
  down->setPath(QFileInfo(fileName).absoluteFilePath());
  down->accept();
}

void iWebView::authenticationRequired(const QUrl& url, QAuthenticator* auth) {
  taiMisc::net_access_mgr->provideAuthenticationUrl(url, auth);
}

void iWebView::proxyAuthenticationRequired(const QUrl& url, QAuthenticator* auth,
                                           const QString& whatisthis) {
  // todo: this is not yet supported!
}

QWebEngineView* iWebView::createWindow(QWebEnginePage::WebWindowType type) {
  QWebEngineView* rval = NULL;
  emit sigCreateWindow(type, rval); // calls iPanelOfDocView handler
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

void iWebView::contextMenuEvent(QContextMenuEvent *event) {
  QMenu* menu = page()->createStandardContextMenu();
  connect(menu, &QMenu::aboutToHide, menu, &QObject::deleteLater);
  menu->addSeparator();
  if(hasSelection()) {
    menu->addAction(pageAction(QWebEnginePage::Cut));
    menu->addAction(pageAction(QWebEnginePage::Copy));
  }
  menu->addAction(pageAction(QWebEnginePage::Paste));
  menu->addAction(pageAction(QWebEnginePage::Undo));
  menu->addAction(pageAction(QWebEnginePage::Redo));
  menu->addAction(pageAction(QWebEnginePage::SelectAll));
  menu->addSeparator();
  menu->addAction(pageAction(QWebEnginePage::OpenLinkInThisWindow));
  menu->addAction(pageAction(QWebEnginePage::OpenLinkInNewWindow));
  menu->addAction(pageAction(QWebEnginePage::CopyLinkToClipboard));
  menu->addAction(pageAction(QWebEnginePage::DownloadLinkToDisk));
  menu->addAction(pageAction(QWebEnginePage::DownloadImageToDisk));
  menu->addAction(pageAction(QWebEnginePage::DownloadMediaToDisk));
  menu->popup(event->globalPos());
}

/////////////////////////////////////////////////////////////////////
#elif defined(USE_QT_WEBVIEW)

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

/////////////////////////////////////////////////////////////////////
#else

iWebView::iWebView(QWidget* parent, iPanelOfDocView* docview)
  : inherited(parent)
  , own_docview(docview)
{
}

void iWebView::cleanupWeb() {
  
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

