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

#include "iDialogSearch.h"
#include <QTreeWidget>
#include <taiWidgetBitBox>
#include <iLineEdit>
#include <iMainWindowViewer>
//#include <iHelpBrowser>
#include <iTextBrowser>
#include <taiSigLink>
#include <taProject>

#include <taMisc>
#include <taiMisc>

#include <QDialog>
#include <QVBoxLayout>
#include <QToolButton>
#include <QStatusBar>
#include <QDesktopServices>

class QSleazyFakeTreeWidget: public QTreeWidget {
public:
    using QTreeWidget::sizeHintForColumn;
};

iDialogSearch* iDialogSearch::New(int ft, iMainWindowViewer* par_window_)
{
  iDialogSearch* rval = new iDialogSearch(par_window_);
  Qt::WindowFlags wflg = rval->windowFlags();
  wflg &= ~Qt::WindowStaysOnTopHint;
  rval->setWindowFlags(wflg);
  rval->setFont(taiM->dialogFont(ft));
  rval->Constr();
  return rval;
}

iDialogSearch::iDialogSearch(iMainWindowViewer* par_window_)
:inherited(par_window_)
{
  init();
}

iDialogSearch::~iDialogSearch() {
  setRoot(NULL, false);
}

void iDialogSearch::init() {
  m_options = SO_DEF_OPTIONS;
  m_changing = 0;
  m_stop = false;
  for (int i = 0; i < num_sorts; ++i)
    m_sorts[i] = -1;
  m_items.InitLinks();
  m_items.NewCol(DataCol::VT_INT, "row");
  m_items.NewCol(DataCol::VT_INT, "level");
  m_items.NewCol(DataCol::VT_STRING, "headline");
  m_items.NewCol(DataCol::VT_STRING, "href");
  m_items.NewCol(DataCol::VT_STRING, "desc");
  m_items.NewCol(DataCol::VT_STRING, "hits");
  m_items.NewCol(DataCol::VT_INT, "relev");
  m_items.NewCol(DataCol::VT_STRING, "path");
  setSizeGripEnabled(true);
  resize(taiM->dialogSize(taiMisc::hdlg_m)); // don't hog too much screen size
}

void iDialogSearch::Constr() {
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);

  TypeDef* typ = TA_iDialogSearch.sub_types.FindName("SearchOptions");
  bbOptions = new taiWidgetBitBox(true, typ, NULL, NULL, this);
  bbOptions->GetImage(m_options);
  layOuter->addWidget(bbOptions->GetRep());

  QHBoxLayout* lay = new QHBoxLayout();
  lay->setMargin(0);
  lay->setSpacing(0);
  search = new iLineEdit(this);
  search->setToolTip("Enter text to search for in item names, descriptions, and contents.");
  lay->addWidget(search, 1);
  btnGo = new QToolButton(this);
  btnGo->setText("&Go");
//TODO: trap Enter so user can hit Enter in field  ((QToolButton*)btnGo)->setDefault(true);
  lay->addWidget(btnGo);
  lay->addSpacing(taiM->vsep_c);
  btnStop = new QToolButton(this);
  btnStop->setText("X");
  btnStop->setToolTip("stop search");
  lay->addWidget(btnStop);
  lay->addSpacing(taiM->hspc_c);
  layOuter->addLayout(lay);

  results = new iTextBrowser(this);
  layOuter->addWidget(results, 1); // results is item to expand in host
  results->setHtml("Enter search words in the box above.<br>Enclose phrases in \" (quotation marks).<br>You can exclude items by preceding a search word or phrase with - (minus).<br>You can change the sort order by clicking on the column header link.");

  status_bar = new QStatusBar(this);
  layOuter->addWidget(status_bar);

  connect(search, SIGNAL(returnPressed()), this, SLOT(go_clicked()) );
  connect(btnGo, SIGNAL(clicked()), this, SLOT(go_clicked()) );
  connect(btnStop, SIGNAL(clicked()), this, SLOT(stop_clicked()) );
  connect(results, SIGNAL(setSourceRequest(iTextBrowser*, const QUrl&, bool&)),
    this, SLOT(results_setSourceRequest(iTextBrowser*, const QUrl&, bool&)) );
#if (QT_VERSION >= 0x050000)
  connect(results, SIGNAL(highlighted(const QString&)),
    status_bar, SLOT(showMessage(const QString&)) );
#else
  connect(results, SIGNAL(highlighted(const QString&)),
    status_bar, SLOT(message(const QString&)) );
#endif
  search->setFocus();
}

void iDialogSearch::AddItem(const String& headline, const String& href,
    const String& desc, const String& hits, const String& path_long, int level, int relev)
{//note: newlines just to help make resulting str readable in debugger, etc.
  m_items.AddBlankRow();
  m_items.SetVal(m_row++, col_row, -1);
  m_items.SetVal(level, col_level, -1);
  m_items.SetVal(headline, col_headline, -1);
  m_items.SetVal(href, col_href, -1);
  m_items.SetVal(desc, col_desc, -1);
  m_items.SetVal(hits, col_hits, -1);
  m_items.SetVal(relev, col_relev, -1);
  m_items.SetVal(path_long, col_path, -1);
}

void iDialogSearch::SigLinkDestroying(taSigLink* dl) {
  Reset();
  RootSet(NULL);
}

void iDialogSearch::EndSection()
{
}

void iDialogSearch::FindNext() {
//TODO
}

void iDialogSearch::go_clicked() {
  Search();
}

void iDialogSearch::results_setSourceRequest(iTextBrowser* src,
  const QUrl& url, bool& cancel)
{
  if ((url.scheme() == "sort")) {
    int col = url.path().toInt();
    setFirstSort(col);
    Render();
  }
  else { // unknown, so forward to global, which is iMainWindowViewer::taUrlHandler
    QDesktopServices::openUrl(url);
  }
  cancel = true;
  //NOTE: we never let results call its own setSource because we don't want
  // link clicking to cause us to change our source page
}

void iDialogSearch::RootSet(taiSigLink* root) {
  String cap = "Find in: ";
  if (root) {
    root_path = root->GetPathNames();
    cap += root_path;
  }
  setWindowTitle(cap);
}

void iDialogSearch::ParseSearchString() {
  m_targets.Reset();
  m_kickers.Reset();
  String s = trim(String(search->text()));
  while (s.nonempty()) {
    bool k = false;
    // look for qualifier

    char c = s[0];
    if (c == '-') {
      k = true;
      s = s.from(1);
    } else if (c == '+') {
      s = s.from(1);
    }
    if (s.empty()) break;
    // look for quoted guys
    c = s[0];
    String t;
    if (c == '"') {
      t = s.after('"');
      if (t.contains('"')) {
        t = t.before('"');
        s = t.after('"');
      } else {
        s = _nilString;
      }
    } else if (s.contains(' ')) {
      t = s.before(" ", 1);
      s = s.after(" ", 1);
    } else {
      t = s;
      s = _nilString;
    }
    if (t.nonempty()) {
      if (k) m_kickers.AddUnique(t);
      else   m_targets.AddUnique(t);
    }
    s = triml(s);
  }
}

void iDialogSearch::Search() {
  if (!link()) return;
  // get the latest options
  bbOptions->GetValue(m_options);
  ParseSearchString();
  proc_events_timer.start();
  taMisc::Busy(true);
  Start();
  link()->Search(this);
  End();
  taMisc::Busy(false);
}

void iDialogSearch::Reset()
{
  m_stop = false;
  m_items.ResetData();
  m_row = 0;
  results->setText("");
  taiMisc::ProcessEvents();
}

void iDialogSearch::Start()
{
  Reset();
}

void iDialogSearch::End()
{
  Render();
}

void iDialogSearch::Render()
{
  // If the dialog isn't visible because the user clicked the close button
  // or hit escape to cancel the search, don't bother showing any results.
  if (!isVisible()) {
    return;
  }

  // If thousands of results, or if stop was clicked and hundreds of results,
  // ask the user if they really want to display them all, since it may take
  // a while to render all rows as HTML.
  int num_rows_to_render = m_items.rows;
  if (num_rows_to_render > 1000 || (m_stop && num_rows_to_render > 50)) {
    int choice = taMisc::Choice(
      "Warning: There are " + taString(num_rows_to_render) + " results.\n"
      "Displaying them all may be very slow.\n\n"
      "How many would you like to see?",
      "&None",
      "&20",
      num_rows_to_render >= 120 ? "&100" : 0,
      num_rows_to_render >= 600 ? "&500" : 0,
      "&All (be patient)");
    int limit = 0;
    switch (choice) {
      case 0:              break;
      case 1: limit = 20;  break;
      case 2: limit = 100; break;
      case 3: limit = 500; break;
      case 4: limit = num_rows_to_render; break;
    }
    num_rows_to_render = std::min(num_rows_to_render, limit);
  }

  taMisc::Busy(true);
  if (m_sorts[0] != -1) {
    m_items.Sort(m_sorts[0], true, m_sorts[1],
      true, m_sorts[2], true);
  }
  src = "<table border=1><tr><th>";
  if (m_sorts[0] == col_level) src += "nest";
  else src += "<a href=sort:" + String(col_level) + ">nest</a>";
  src += "</th><th>";
  if (m_sorts[0] == col_headline) src += "item";
  else src += "<a href=sort:" + String(col_headline) + ">item</a>";
  src += "</th><th>hits</th><th>";
  if (m_sorts[0] == col_relev) src += "relev";
  else src += "<a href=sort:" + String(col_relev) + ">relev</a>";
  src += "</th><th>";
  if (m_sorts[0] == col_path) src += "path";
  else src += "<a href=sort:" + String(col_path) + ">path</a>";
  src += "</th></tr>\n";
  for (int i = 0; i < num_rows_to_render; ++i) {
    int level = m_items.GetValAsInt(col_level, i);
    String headline =  m_items.GetValAsString(col_headline, i);
    String href =  m_items.GetValAsString(col_href, i);
    String desc =  m_items.GetValAsString(col_desc, i);
    String hits =  m_items.GetValAsString(col_hits, i);
    String path =  m_items.GetValAsString(col_path, i);
    path = path.after(root_path);
    int relev = m_items.GetValAsInt(col_relev, i);
    RenderItem(level, headline, href, desc, hits, path, relev);
  }
  src += "</table>";
  results->setHtml(src);
  taMisc::Busy(false);
}

void iDialogSearch::RenderItem(int level, const String& headline,
       const String& href, const String& desc, const String& hits,
       const String& path, int relev)
{
  STRING_BUF(b, 200);
  b += "<tr><td>" + String(level) + "</td><td>";
  // item
  b += "<a href=\"" + href;
  if(link() && link()->taData()) {
    taBase* tab = link()->taData();
    MainWindowViewer* mwv = tab->GetMyBrowser();
    if(mwv && mwv->widget()) {
      b += "#winid_" + String(mwv->widget()->uniqueId()); // mark our project window!
    }
  }
  b += "\">" + headline + "</a>";
  if (desc.nonempty()) {
  //TODO: need to somehow escape html nasties in this!!!
    b += "<br>" + desc ;
  }
  b += "</td><td>";
  // hits
  b += hits; // note: this will already have highlighting, breaks etc.
  b += "</td><td>";
  // relev
  b += String(relev);
  b += "</td><td>";
  // path
  b += path;
  b += "</td></tr>";
  src.cat(b);
}

void iDialogSearch::StartSection(const String& sec_name)
{
}


bool iDialogSearch::setFirstSort(int col) {
  if ((col < 0) || (col >= num_cols)) return false;
  if (m_sorts[0] == col) return false;
  // push down, also nuking any existing col guys (don't sort twice on same col)
  for (int i = num_sorts - 1; i >= 1; --i) {
    if (m_sorts[i-1] == col) m_sorts[i-1] = -1;
    m_sorts[i] = m_sorts[i-1];
  }
  m_sorts[0] = col;
  return true;
}

void iDialogSearch::setRoot(taiSigLink* root, bool update_gui) {
  if (link() != root) {
    if (link()) link()->RemoveSigClient(this);
    if (root) root->AddSigClient(this);
  }
  if (update_gui)
    RootSet(root);
}

void iDialogSearch::setSearchStr(const String& srch_str) {
  stop_clicked();
  if(stop()) {
    search->setText(srch_str);
    if(srch_str.nonempty())
      Search();
  }
}

String iDialogSearch::searchStr() const {
  return search->text();
}

bool iDialogSearch::stop() const {
  if(proc_events_timer.elapsed() > taMisc::css_gui_event_interval) {
    taiM->RunPending();
    const_cast<iDialogSearch*>(this)->proc_events_timer.restart();
  }
  return m_stop;
}

void iDialogSearch::stop_clicked() {
  m_stop = true;
}

void iDialogSearch::closeEvent(QCloseEvent * e) {
  m_stop = true;                // stop on close
  inherited::closeEvent(e);
}
