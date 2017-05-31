// Copyright 2017, Regents of the University of Colorado,
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

#ifndef iPanelOfDocView_h
#define iPanelOfDocView_h 1

// parent includes:
#include <iPanel>

// member includes:
#ifndef __MAKETA__
#include <iWebView>
#else
class iWebView; //
class QAction; //
#endif

// declare all other types mentioned but not required to include:
class iLineEdit; //
class QProgressBar; //
class taDoc; //
class QToolBar; //
class QLabel; //

#ifndef USE_QT_WEBENGINE
class QWebView; //
#endif


taTypeDef_Of(iPanelOfDocView);

class TA_API iPanelOfDocView: public iPanel {
  // a panel frame for displaying docs
  Q_OBJECT
INHERITED(iPanel)
public:
  QWidget*              wb_widg;  // overall widget for web browser case
  QVBoxLayout*          wb_box; // web browser vbox
  iWebView*             webview; // the web_doc case
  QToolBar*             url_bar; // toolbar in url box
  QLabel*               wiki_label;
  iLineEdit*            wiki_edit;
  QLabel*               url_label; // url label
  iLineEdit*            url_edit; // editor for url
  QAction*              fwd_but; // forward
  QAction*              bak_but; // backward
  QAction*              go_but; // load web page
  QProgressBar*         prog_bar;   // progress bar
  QAction*              seturl_but; // save this location as our url!
  QAction*              ext_brow_but; // open in external browser button..
  QLabel*               find_lbl;
  iLineEdit*            find_text;
  QAction*              find_clear;
  QAction*              find_prev;
  QAction*              find_next;

  bool                  go_back_after_load;
  // hacky workaround for engine url interceptor -- if set, then go back after load finishes
  
  virtual void          setDoc(taDoc* doc); // only called if changes after creation

  taDoc*                doc() {return (m_link) ? (taDoc*)(link()->data()) : NULL;}
  String                panel_type() const override {return "Doc";}


  QWidget*     firstTabFocusWidget() override;

  iPanelOfDocView(); // NOTE: use the setDoc api to (indirectly) set the SigLink
  ~iPanelOfDocView();

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iPanelOfDocView;}
  void         SigLinkDestroying(taSigLink* dl) override;
  bool         ignoreSigEmit() const override;
protected:
  taDoc*       m_doc; // ref managed through link; we just put ptr here to detect change
  bool         is_loading;

  void         SigEmit_impl(int sls, void* op1, void* op2) override;
  void         UpdatePanel_impl() override;
  bool         eventFilter(QObject *obj, QEvent *event) override;

#ifndef __MAKETA__
protected slots:
#ifdef USE_QT_WEBENGINE
  void                  doc_createWindow(QWebEnginePage::WebWindowType type, QWebEngineView*& window);
#else // USE_QT_WEBENGINE
  void                  doc_createWindow(QWebPage::WebWindowType type, QWebView*& window);
  void                  doc_linkClicked(const QUrl& url);
#endif // USE_QT_WEBENGINE
  void                  doc_goPressed();
  void                  doc_bakPressed();
  void                  doc_fwdPressed();
  void                  doc_seturlPressed();
  void                  doc_extBrowPressed();
  void                  doc_loadStarted();
  void                  doc_loadFinished(bool ok);

  void                  find_clear_clicked();
  void                  find_next_clicked(); // or return in find_text
  void                  find_prev_clicked();

//  void                        br_copyAvailable (bool yes);
#endif

};

#endif // iPanelOfDocView_h
