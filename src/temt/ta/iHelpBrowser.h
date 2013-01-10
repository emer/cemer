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

#ifndef iHelpBrowser_h
#define iHelpBrowser_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QMainWindow>
#endif

// member includes:

// declare all other types mentioned but not required to include:


class TA_API iHelpBrowser: public QMainWindow {
//   TypeDef documentation
INHERITED(QMainWindow)
  Q_OBJECT
public:
#ifndef __MAKETA__
  enum Roles { // extra roles, for additional data, etc.
    ObjUrlRole = Qt::UserRole + 1, // Url stored in this
//    ObjCatRole  // for object category string, whether shown or not
  };
#endif


#ifndef __MAKETA__

public: //  this is the public interface
  static iHelpBrowser* instance(); // we only create one instance, this gets it
  static void           StatLoadEnum(TypeDef* typ); // the enum set, not an individual guy
  static void           StatLoadMember(MemberDef* mbr);
  static void           StatLoadMethod(MethodDef* mth);
  static void           StatLoadType(TypeDef* typ);
  static void           StatLoadUrl(const String& url); // saves url to detect anchors
  static String         UrlToTabText(const String& url); // extracts tab text from url, for when url changes
  static bool           IsUrlExternal(const String& url); // true for http/s:, file:

  void                  LoadEnum(TypeDef* typ); // the enum set, not an individual guy
  void                  LoadMember(MemberDef* mbr);
  void                  LoadMethod(MethodDef* mth);
  void                  LoadType(TypeDef* typ, const String& anchor = _nilString);
  void                  LoadUrl(const String& url); // s/b a .Type. or http

public:

  QSplitter*            split;
  QAction*                actBack;
  QAction*                actForward;
  iLineEdit*              filter;
  QTreeWidget*            tv;
  iLineEdit*              url_text;
  QProgressBar*           prog_bar;
  QAction*                actGo;
  QAction*                actStop;
  QLabel*                 find_lbl;
  iLineEdit*              find_text;
  QAction*                find_clear;
  QAction*                find_prev;
  QAction*                find_next;
  QTabWidget*             tab; // note: use our own load() routine for urls
  QAbstractButton*        btnAdd; // new tab
  QStatusBar*           status_bar;

  String                curUrl() const {return m_curUrl;} // current pseudo Url (w/o #xxx anchor)
  QWebView*             curWebView(); // always returns one
  QWebView*             webView(int index);
  QWebView*             AddWebView(const String& label); // add a new tab

protected:
  static const int      num_sorts = 3;
  static iHelpBrowser* inst;

  String                m_curUrl;
  ContextFlag           m_changing; // true when we should ignore various changes
  QString               last_filter; // for checking if anything changed
  QString               last_find;
  QTimer*               timFilter; // timer for filter changes

  QWebView*             FindWebView(const String& url, int& idx); // find existing tab w/ this base url (#anchors ok)
  QWebView*             EmptyWebView(int& idx); // find existing tab w/ no url, else make new
  void                  SetFilter(const QString& filt); // apply a filter
  void                  ClearFilter(); // remove filtering
  void                  ApplyFiltering();
  void                  LoadType_impl(TypeDef* typ, const String& base_url,
    const String& anchor); // for loading Type guys
  void                  LoadExternal_impl(const String& url); // for loading External guys
  bool                  ShowItem(const QTreeWidgetItem* item) const;
  TypeDef*              GetTypeDef(QTreeWidgetItem* item);
  QTreeWidgetItem*      FindItem(TypeDef* typ); // find item from type -- we derefence type to base type
  QTreeWidgetItem*      FindItem(const String& typ_name); // find item from type name
  void                  ItemChanged(QTreeWidgetItem* item); // item changed to this, sync
  bool                  SetItem(TypeDef* typ); // set active item by TypeDef, true if set
  void                  showEvent(QShowEvent* e); // override
  void                  UpdateTreeItem(); // call after tab changes and url changes
  override bool         eventFilter(QObject *obj, QEvent *event);
  // translate emacs keys..

  iHelpBrowser();
  ~iHelpBrowser();

protected slots:
  void                  forward_clicked();
  void                  back_clicked();
  void                  addTab_clicked(); // or return in url_text
  void                  brow_createWindow(QWebPage::WebWindowType type, QWebView*& window);
  void                  brow_linkClicked(const QUrl& url);
  void                  brow_urlChanged(const QUrl& url);
  void                  page_unsupportedContent(QNetworkReply* reply);
  void                  filter_textChanged(const QString& text);
  void                  go_clicked(); // or return in url_text
  void                  stop_clicked(); // or return in url_text
  void                  show_timeout(); // for scrolling to item

  void                  find_clear_clicked();
  void                  find_next_clicked(); // or return in find_text
  void                  find_prev_clicked();

  void                  tab_currentChanged(int index);
  void                  tab_tabCloseRequested(int index);
  void                  timFilter_timeout();
  void                  tv_currentItemChanged(QTreeWidgetItem* curr, QTreeWidgetItem* prev);

private:
  void          init(); // called by constructors
  void          AddTypesR(TypeSpace* ts);
#endif // !__MAKETA__
};

#endif // iHelpBrowser_h
