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

#include "iHelpBrowser.h"
#include <iSize>
#include <NameVar>
#include <iSplitter>
#include <iMainWindowViewer>
#include <iLineEdit>
#include <iWebView>
#include <iNetworkAccessManager>
#include <MemberDef>
#include <MethodDef>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QProgressBar>
#include <QToolButton>
#include <QTimer>
#include <QScrollBar>
#include <QStatusBar>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QCoreApplication>
#include <QKeyEvent>


class QSleazyFakeTreeWidget: public QTreeWidget {
public:
    using QTreeWidget::sizeHintForColumn;
};

iHelpBrowser* iHelpBrowser::inst;

iHelpBrowser* iHelpBrowser::instance() {
  if (!inst) {
    inst = new iHelpBrowser();
    iSize sz = taiM->dialogSize(taiMisc::dlgBig);
    inst->resize(sz.width(), (int)(1.2f * (float)sz.height())); // a bit bigger than .6h
    inst->show();
    inst->raise();
    taiMiscCore::ProcessEvents(); // run default stuff
    taiMiscCore::ProcessEvents(); // run default stuff
    taiMiscCore::ProcessEvents(); // run default stuff
  } else {
    inst->show();
    inst->raise();
  }
  return inst;
}

bool iHelpBrowser::IsUrlExternal(const String& url) {
  return (url.startsWith("http:") ||
    url.startsWith("https:") ||
    url.startsWith("file:")
  );
}

void iHelpBrowser::StatLoadEnum(TypeDef* typ) {
  instance()->LoadEnum(typ);
}

void iHelpBrowser::StatLoadMember(MemberDef* mbr) {
  instance()->LoadMember(mbr);
}

void iHelpBrowser::StatLoadMethod(MethodDef* mth) {
  instance()->LoadMethod(mth);
}

void iHelpBrowser::StatLoadType(TypeDef* typ) {
  instance()->LoadType(typ);
}

void iHelpBrowser::StatLoadUrl(const String& url) {
  instance()->LoadUrl(url);
}

String iHelpBrowser::UrlToTabText(const String& url) {
  String base_url;
  String anchor = url.after("#");
  if (anchor.empty())
    base_url = url;
  else
    base_url = url.before("#");
  if (base_url.contains(".Type.")) {
    return base_url.after(".Type.");
  }
  // replace with wiki names to shorten
  String nwurl = url;
  String idx = "/index.php/";
  for(int i=0;i<taMisc::wikis.size;i++) {
    NameVar& nv = taMisc::wikis[i];
    String wurl = nv.value.toString();
    if(nwurl.startsWith(wurl)) {
      String nm = nwurl.after(wurl);
      if(nm.startsWith(idx)) nm = nm.after(idx);
      nwurl = nv.name + " " + nm;
      break;
    }
  }
  return nwurl;
}


// note: we parent to main_win so something will delete it
iHelpBrowser::iHelpBrowser()
:inherited(taiMisc::main_window)
{
  init();
}

iHelpBrowser::~iHelpBrowser() {
  if (this == inst) {
    inst = NULL;
  }
  // disconnect in case it will be firing
  disconnect(this, SIGNAL(tv_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

void iHelpBrowser::init() {
  this->setAttribute(Qt::WA_DeleteOnClose, false); // keep alive when closed
  this->setWindowTitle("Help Browser");
//  this->setSizeGripEnabled(true);

  int font_spec = taiMisc::fonMedium;

  split = new iSplitter;

  QWidget* tvw = new QWidget;
  QVBoxLayout* lay_tv = new QVBoxLayout(tvw);
  lay_tv->setMargin(0);

  QToolBar* tool_bar = new QToolBar(tvw);
  lay_tv->addWidget(tool_bar);

  actBack = tool_bar->addAction("<");
  actBack->setToolTip("Back one step in browsing history -- not often relevant due to opening new pages for each unique URL");
  actBack->setStatusTip(actBack->toolTip());
  actForward = tool_bar->addAction(">" );
  actForward->setToolTip("Forward one step in browsing history  -- not often relevant due to opening new pages for each unique URL");
  actForward->setStatusTip(actForward->toolTip());
  tool_bar->addSeparator();

//   QLabel* lbl = taiM->NewLabel("search", tvw, font_spec);
  QLabel* lbl = new QLabel("search");
  lbl->setToolTip("Search for object type names to narrow the list below -- will find anything containing the text entered");
  tool_bar->addWidget(lbl);
  filter = new iLineEdit();
  filter->setToolTip(lbl->toolTip());
  tool_bar->addWidget(filter);

  tv = new QTreeWidget(tvw);
  tv->setColumnCount(2);
  // will always need a vert scroller so turn on so sizing is deterministic
  tv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//  tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QTreeWidgetItem* hdr = tv->headerItem();
  hdr->setText(0, "Type");
  hdr->setText(1, "Category");
  lay_tv->addWidget(tv, 1);

  QWidget* wid_tab = new QWidget;
  QVBoxLayout* lay_tab = new QVBoxLayout(wid_tab);
  lay_tab->setMargin(0);

  tool_bar = new QToolBar(wid_tab);
  lay_tab->addWidget(tool_bar);

  url_text = new iLineEdit();
  tool_bar->addWidget(url_text);

  prog_bar = new QProgressBar(wid_tab);
  prog_bar->setRange(0, 100);
  prog_bar->setMaximumWidth(30);
  tool_bar->addWidget(prog_bar);

  actGo = tool_bar->addAction("Go");
  actStop = tool_bar->addAction("X");
  actStop->setToolTip("Stop");

  // find within item
//   find_lbl = taiM->NewLabel("| find:", wid_tab, font_spec);
  tool_bar->addSeparator();
  find_lbl = new QLabel("find:");
  find_lbl->setToolTip("Find text string within currently viewed page");
  tool_bar->addWidget(find_lbl);
  find_text = new iLineEdit();
  find_text->setCharWidth(16);
  tool_bar->addWidget(find_text);
  find_clear = tool_bar->addAction("x");
  find_clear->setToolTip("Clear find text and reset any prior highlighting");
  find_prev = tool_bar->addAction("<");
  find_prev->setToolTip("Find previous occurrence of find: text within current page");
  find_next = tool_bar->addAction(">");
  find_next->setToolTip("Find next occurrence of find: text within current page");

  lay_tab->addWidget(tool_bar);
  tab = new QTabWidget(wid_tab);
  tab->setElideMode(Qt::ElideMiddle);
  tab->setUsesScrollButtons(true); // otherwise doesn't always, ex. on Mac
  btnAdd = new QToolButton;
  btnAdd->setText("+");
  btnAdd->setToolTip("add a new empty tab");
  tab->setCornerWidget(btnAdd, Qt::TopLeftCorner);
  lay_tab->addWidget(tab);

  setCentralWidget(split);
//  layOuter->addWidget(status_bar);

  tool_bar->installEventFilter(this); // translate keys..
  tv->installEventFilter(this); // translate keys..
  filter->installEventFilter(this); // translate keys..

  // add all types -- only non-virtual, base types
  AddTypesR(&taMisc::types);
  tv->setSortingEnabled(true);
  tv->sortByColumn(0, Qt::AscendingOrder);
  // fit tree to minimum -- we have to force splitter to resize unfortunately
  tv->resizeColumnToContents(0);
  int tv_width = tv->columnWidth(0) + ((QSleazyFakeTreeWidget*)tv)->sizeHintForColumn(1)
    + tv->verticalScrollBar()->width() + 40;
  tv->resize(tv_width, tv->height());
//  tv->resizeColumnToContents(1);

  split->addWidget(tvw);
  split->addWidget(wid_tab);

  timFilter = new QTimer(this);
  timFilter->setSingleShot(true);
  timFilter->setInterval(500);

  status_bar = statusBar(); // asserts

  connect(actGo, SIGNAL(triggered()), this, SLOT(go_clicked()) );
  connect(actStop, SIGNAL(triggered()), this, SLOT(stop_clicked()) );
  connect(btnAdd, SIGNAL(clicked()), this, SLOT(addTab_clicked()) );
  connect(url_text, SIGNAL(returnPressed()), this, SLOT(go_clicked()) );
  connect(actBack, SIGNAL(triggered()), this, SLOT(back_clicked()) );
  connect(actForward, SIGNAL(triggered()), this, SLOT(forward_clicked()) );

  connect(find_clear, SIGNAL(triggered()), this, SLOT(find_clear_clicked()) );
  connect(find_next, SIGNAL(triggered()), this, SLOT(find_next_clicked()) );
  connect(find_prev, SIGNAL(triggered()), this, SLOT(find_prev_clicked()) );
  connect(find_text, SIGNAL(returnPressed()), this, SLOT(find_next_clicked()) );

  connect(tv, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
    this, SLOT(tv_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
  connect(tab, SIGNAL(tabCloseRequested(int)),
    this, SLOT(tab_tabCloseRequested(int)) );
  connect(filter, SIGNAL(textChanged(const QString&)),
    this, SLOT(filter_textChanged(const QString&)) );
  connect(tab, SIGNAL(currentChanged(int)),
    this, SLOT(tab_currentChanged(int)) );
  connect(timFilter, SIGNAL(timeout()), this, SLOT(timFilter_timeout()) );

  AddWebView(_nilString); // so stuff lays out
}

void iHelpBrowser::addTab_clicked() {
  AddWebView("");
}

void iHelpBrowser::forward_clicked() {
  curWebView()->forward();
}

void iHelpBrowser::back_clicked() {
  curWebView()->back();
}

void iHelpBrowser::find_clear_clicked() {
  find_text->clear();
  last_find.clear();
#if (QT_VERSION >= 0x040600)
  curWebView()->page()->findText("", QWebPage::HighlightAllOccurrences);
#else
  curWebView()->page()->findText("");
#endif
}

void iHelpBrowser::find_next_clicked() {
  QString cur_find = find_text->text();
  if(cur_find != last_find) {
    // first one highlights all then goes to first one
#if (QT_VERSION >= 0x040600)
    curWebView()->page()->findText(cur_find, QWebPage::HighlightAllOccurrences);
#endif
    curWebView()->page()->findText(cur_find, QWebPage::FindWrapsAroundDocument);
    last_find = cur_find;
  }
  else {
    // subsequent ones go through one by one
    curWebView()->page()->findText(cur_find, QWebPage::FindWrapsAroundDocument);
  }
}

void iHelpBrowser::find_prev_clicked() {
  curWebView()->page()->findText(find_text->text(), QWebPage::FindWrapsAroundDocument | QWebPage::FindBackward);
}

void iHelpBrowser::AddTypesR(TypeSpace* ts) {
  for (int i = 0; i < ts->size; ++i) {
    TypeDef* typ = ts->FastEl(i);
    if (!typ->IsActualClass())
      continue;
    if (// typ->IsTemplInst() ||
        typ->HasOption("VIRT_BASE") ||
        typ->HasOption("HIDDEN") ||
        typ->HasOption("IGNORE"))
      continue;
    // get rid of the junk stub types by looking for empties...
    if ((typ->members.size == 0) && (typ->methods.size == 0))
      continue;
    QTreeWidgetItem* twi = new QTreeWidgetItem(tv);
    twi->setText(0, typ->name);
    twi->setText(1, typ->GetCat());
    twi->setData(0, Qt::UserRole, QVariant((ta_intptr_t)typ));
//    AddTypesR(&typ->children);
  }
}

QWebView* iHelpBrowser::AddWebView(const String& label) {
  ++m_changing;
  QWebView* brow = new iWebView;
  QWebPage* wp = brow->page();
  brow->setTextSizeMultiplier(taMisc::doc_text_scale * ((float)taMisc::font_size / 12.0f));
  wp->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
  wp->setNetworkAccessManager(taiMisc::net_access_mgr);
  int tidx = tab->addTab(brow, label.toQString());
  tab->setCurrentIndex(tidx); // not automatic
  url_text->setText("");// something else has to make it valid
  connect(brow, SIGNAL(linkClicked(const QUrl&)),
    this, SLOT(brow_linkClicked(const QUrl&)) );
  connect(brow, SIGNAL(statusBarMessage(const QString&)),
    status_bar, SLOT(showMessage(const QString&)) );
  connect(brow, SIGNAL(sigCreateWindow(QWebPage::WebWindowType,
    QWebView*&)), this, SLOT(brow_createWindow(QWebPage::WebWindowType,
    QWebView*&)) );
  connect(brow, SIGNAL(urlChanged(const QUrl&)),
    this, SLOT(brow_urlChanged(const QUrl&)) );
  // note: WebView doesn't show hover links in status by default so we do it
  connect(wp, SIGNAL(linkHovered(const QString&, const QString&, const QString&)),
    status_bar, SLOT(showMessage(const QString&)) );
  connect(wp, SIGNAL(unsupportedContent(QNetworkReply*)),
    this, SLOT(page_unsupportedContent(QNetworkReply*)) );
  connect(brow, SIGNAL(loadProgress(int)), prog_bar, SLOT(setValue(int)) );
  connect(brow, SIGNAL(loadStarted()), prog_bar, SLOT(reset()) );
  wp->setForwardUnsupportedContent(true);

  brow->installEventFilter(this); // translate keys..

  --m_changing;
  if (tab->count() > 1) {
#if (QT_VERSION >= 0x040500) //TEMP
    tab->setTabsClosable(true);
#endif
  }
  return brow;
}

void iHelpBrowser::ApplyFiltering() {
  ++m_changing;
  taMisc::Busy();
  QTreeWidgetItemIterator it(tv, QTreeWidgetItemIterator::All);
  QTreeWidgetItem* item;
  QString s;
  //QTreeWidgetItem* first_item = NULL;
  int n_items = 0;
  while ((item = *it)) {
    // TODO (maybe): don't hide NULL item
    bool show = ShowItem(item);
    tv->setItemHidden(item, !show);
    if(show) {
      n_items++;
    }
    ++it;
  }
  taMisc::DoneBusy();
  --m_changing;
}

void iHelpBrowser::brow_createWindow(QWebPage::WebWindowType type,
    QWebView*& window)
{
  if (type == QWebPage::WebBrowserWindow) {
    window = AddWebView(_nilString);
  }
}

void iHelpBrowser::brow_linkClicked(const QUrl& url)
{
  // forward to global, which is iMainWindowViewer::taUrlHandler
  // for .Type. urls (us) it just calls back to LoadUrl(url)
  QDesktopServices::openUrl(url);
}

void iHelpBrowser::brow_urlChanged(const QUrl& url)
{ // NOTE: we assume it is only the current visible guy who can do this
  ++m_changing;
  url_text->setText(url.toString());
  tab->setTabText(tab->currentIndex(), UrlToTabText(url.toString()));
  tab->setTabToolTip(tab->currentIndex(), UrlToTabText(url.toString()));
  --m_changing;
}

void iHelpBrowser::page_unsupportedContent(QNetworkReply* reply) {
  // this should only get called when a Open Window or Open Link gets ta: type of url
  QDesktopServices::openUrl(reply->url());
}

void iHelpBrowser::ClearFilter() {
  ++m_changing;
  taMisc::Busy();
  last_filter.clear();
  QTreeWidgetItemIterator it(tv, QTreeWidgetItemIterator::Hidden);
  QTreeWidgetItem* item;
  while ((item = *it)) {
    tv->setItemHidden(item, false);
    ++it;
  }
  taMisc::DoneBusy();
  --m_changing;
}

QWebView* iHelpBrowser::curWebView() {
  if (tab->count() == 0)
    return AddWebView(_nilString);
  return (QWebView*)tab->currentWidget();
}

QWebView* iHelpBrowser::EmptyWebView(int& idx) {
  QWebView* rval = NULL;
  for (idx = 0; idx < tab->count(); ++idx) {
    rval = webView(idx);
    String turl = rval->url().toString();
    if (turl.empty())
      return rval;
  }
  rval = AddWebView("");
  idx = tab->count() - 1;
  return rval;
}

void iHelpBrowser::filter_textChanged(const QString& /*text*/) {
  // following either starts timer, or restarts it
  timFilter->start();
}

QTreeWidgetItem* iHelpBrowser::FindItem(TypeDef* typ) {
  typ = typ->GetActualType();
  QTreeWidgetItemIterator it(tv);
  QTreeWidgetItem* rval;
  while ((rval = *it)) {
    if (GetTypeDef(rval) == typ)
      return rval;
    ++it;
  }
  return NULL;
}

QTreeWidgetItem* iHelpBrowser::FindItem(const String& typ_name_) {
  QString typ_name = typ_name_;
  QTreeWidgetItemIterator it(tv);
  QTreeWidgetItem* rval;
  while ((rval = *it)) {
    if (rval->text(0) == typ_name)
      return rval;
    ++it;
  }
  return NULL;
}

QWebView* iHelpBrowser::FindWebView(const String& url, int& idx) {
  String base_url = url; // common
  if (url.contains("#")) {
    base_url = url.before("#");
  }

  for (idx = 0; idx < tab->count(); ++idx) {
    QWebView* rval = webView(idx);
    String turl = rval->url().toString();
    if (turl.startsWith(base_url))
      return rval;
  }
  idx = -1;
  return NULL;
}

TypeDef* iHelpBrowser::GetTypeDef(QTreeWidgetItem* item) {
  return (TypeDef*)QVARIANT_TO_INTPTR(item->data(0, Qt::UserRole));
}

void iHelpBrowser::go_clicked() {
  String urltxt = url_text->text();
  if(urltxt.empty()) return;
  urltxt = taMisc::FixURL(urltxt);
  QUrl url(urltxt);
  QDesktopServices::openUrl(url);
}

void iHelpBrowser::ItemChanged(QTreeWidgetItem* item) {
  TypeDef* typ = GetTypeDef(item);
  LoadType(typ);
}

void iHelpBrowser::LoadEnum(TypeDef* typ) {
//TODO: maybe check if enum, maybe in debug mode? maybe not needed...
  LoadType(typ->GetOwnerType(), typ->name);
}

void iHelpBrowser::LoadMember(MemberDef* mbr) {
  LoadType(mbr->GetOwnerType(), mbr->name);
}

void iHelpBrowser::LoadMethod(MethodDef* mth) {
  LoadType(mth->GetOwnerType(), mth->name);
}

void iHelpBrowser::LoadType(TypeDef* typ, const String& anchor) {
  String base_url;
  if (typ) {
    typ = typ->GetNonPtrType();
    base_url = "ta:.Type." + typ->name ;
  }
  LoadType_impl(typ, base_url, anchor);
}

void iHelpBrowser::LoadUrl(const String& url) {
  String base_url = url;
  String anchor = url.after("#");
  if (anchor.nonempty())
    base_url = url.before("#");
  if (url.startsWith("ta:.Type.")) {
    String typ_name(base_url.after(".Type."));
    TypeDef* typ = TypeDef::FindGlobalTypeName(typ_name);
    LoadType_impl(typ, base_url, anchor);
  }
  else if(url.startsWith("http://.type.")) {
    String typ_name(base_url.after(".type."));
    String nw_url = "ta:.Type." + typ_name;
    LoadUrl(nw_url);            // convert..
  }
  else if (IsUrlExternal(url)) {
    LoadExternal_impl(url);
  }
  else {
    taMisc::Warning("Attempt to load unsupported url into Help Browser:",
      url);
  }
}

void iHelpBrowser::LoadExternal_impl(const String& url)
{
  last_find.clear();
  int idx;
  QWebView* wv = FindWebView(url, idx);
  if (!wv)
    wv = EmptyWebView(idx); // always succeeds
  ++m_changing;
  if (tab->currentIndex() != idx) {
    tab->setCurrentIndex(idx);
  }
  String tab_text(UrlToTabText(url));
  wv->load(QUrl(url.toQString()));
  tab->setTabText(idx, tab_text.toQString());
  tab->setTabToolTip(idx, tab_text.toQString());
  url_text->setText(wv->url().toString());
  --m_changing;
}

void iHelpBrowser::LoadType_impl(TypeDef* typ, const String& base_url,
    const String& anchor)
{
  last_find.clear();
  String html;
  String url = base_url;
  if (anchor.nonempty())
    url.cat("#").cat(anchor);
  QTreeWidgetItem* twi = NULL;
  String tab_text;
  if (typ) {
    html = typ->GetHTML();
    tab_text = typ->name;
    twi = FindItem(typ);
  }
  if (twi != tv->currentItem()) {
    ++m_changing;
    tv->setCurrentItem(twi, 0, QItemSelectionModel::ClearAndSelect);
    --m_changing;
  }
  int idx;
  QWebView* wv = FindWebView(url, idx);
  if (!wv)
    wv = EmptyWebView(idx); // always succeeds
  ++m_changing;
  if (tab->currentIndex() != idx) {
    tab->setCurrentIndex(idx);
  }
  wv->setHtml(html.toQString(), url.toQString());
  tab->setTabText(idx, tab_text.toQString());
  tab->setTabToolTip(idx, tab_text.toQString() + " url: " + wv->url().toString());
  url_text->setText(wv->url().toString());
  --m_changing;
}

bool iHelpBrowser::ShowItem(const QTreeWidgetItem* item) const {
  // we show the item unless it either doesn't meet filter criteria, or not in cat

/*  // category filter
  if (m_cat_filter != 0) {
    String act_cat = item->data(0, ObjCatRole).toString(); //s/b blank if none set
    String cat_txt = client()->catText(m_cat_filter - 1);// subtract 1 for 'all' item
    if(multi_cats && act_cat.contains(", ")) {
      bool any_match = false;
      while(true) {
        String cur_cat = act_cat.before(", ");
        act_cat = act_cat.after(", ");
        if(cur_cat == cat_txt) {
          any_match = true; break;
        }
        if(act_cat.contains(", ")) continue;
        if(act_cat == cat_txt) {
          any_match = true;
        }
        break;
      }
      if(!any_match) return false;
    }
    else {
      if (act_cat != cat_txt)
        return false;
    }
  }*/

  // filter text filter
  if (!last_filter.isEmpty()) {
    bool hide = true;
    QString s;
    int cols = tv->columnCount();
    for (int i = 0; i < cols; ++i) {
      s = item->text(i);
      if (s.contains(last_filter, Qt::CaseInsensitive)) {
        hide = false;
        break;
      }
    }
    if (hide) return false;
  }
  return true;
}

void iHelpBrowser::SetFilter(const QString& filt) {
  last_filter = filt;
  ApplyFiltering();
}

bool iHelpBrowser::SetItem(TypeDef* typ) {
  QTreeWidgetItem* item = FindItem(typ);
  if (item) {
    tv->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect); // should raise signal
  }
  return (item != NULL);
}

void iHelpBrowser::showEvent(QShowEvent* event) {
  inherited::showEvent(event);
  QTimer::singleShot(150, this, SLOT(show_timeout()) );
}

void iHelpBrowser::show_timeout() {
  QTreeWidgetItem* ci = tv->currentItem();
  if (ci)
    tv->scrollToItem(ci);

  tv->resizeColumnToContents(0);
  int tv_width = tv->columnWidth(0) + ((QSleazyFakeTreeWidget*)tv)->sizeHintForColumn(1)
    + tv->verticalScrollBar()->width() + 60;

  QList<int> sizes;
  sizes << tv_width << split->width() - tv_width;
  split->setSizes(sizes);
}

void iHelpBrowser::stop_clicked() {
  curWebView()->stop();
}

void iHelpBrowser::tab_currentChanged(int index) {
  if (m_changing) return; // already expected
  QWebView* wv = webView(index); // safe
  ++m_changing;
  if (wv) {
    url_text->setText(wv->url().toString());
  } else {
    url_text->setText("");
  }
  UpdateTreeItem();
  --m_changing;
}

void iHelpBrowser::tab_tabCloseRequested(int index) {
  if (tab->count() <= 1) return; // always 1;
  tab->removeTab(index);
  // don't let user close last
  if (tab->count() == 1) {
#if (QT_VERSION >= 0x040500) //TEMP
    tab->setTabsClosable(false);
#endif
  }
}

void iHelpBrowser::timFilter_timeout() {
  // if nothing has changed in text, do nothing
  QString text = filter->text();
  if (last_filter == text) return;
  // if we are already filtering, then don't reenter, but just try again
  if (m_changing) {
    timFilter->start();
    return;
  }
  if (text.isEmpty()) ClearFilter();
  else SetFilter(text);
}

void iHelpBrowser::tv_currentItemChanged(QTreeWidgetItem* curr, QTreeWidgetItem* prev) {
  if (m_changing) return;
  ItemChanged(curr);
}

void iHelpBrowser::UpdateTreeItem() {
  String url = curWebView()->url().toString();
  if (!url.startsWith("ta:.Type.")) return;

  String typ_name = url.after("ta:.Type.");
  if (typ_name.contains("#"))
    typ_name = typ_name.before("#");

  QTreeWidgetItem* twi = FindItem(typ_name);
  if (twi != tv->currentItem()) {
    ++m_changing;
    tv->setCurrentItem(twi, 0, QItemSelectionModel::ClearAndSelect);
    --m_changing;
  }
}

QWebView* iHelpBrowser::webView(int index) {
  if ((index < 0) || (index >= tab->count()))
    return NULL;
  return (QWebView*)tab->widget(index);
}

bool iHelpBrowser::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() != QEvent::KeyPress) {
    return inherited::eventFilter(obj, event);
  }

//  QCoreApplication* app = QCoreApplication::instance();
  QKeyEvent* e = static_cast<QKeyEvent *>(event);
  if(taiMisc::KeyEventFilterEmacs_Edit(obj, e))
    return true;
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed && e->key() == Qt::Key_S) {
    if(find_text->hasFocus())
      filter->setFocus();
    else
      find_text->setFocus();
    return true;                // we absorb this event
  }
  return inherited::eventFilter(obj, event);
}
