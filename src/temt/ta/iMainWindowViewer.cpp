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

#include "iMainWindowViewer.h"

#include <taiWidgetMenu>
#include <iAction>
#include <iBrowseHistory>
#include <iToolBar>
#include <iFrameViewer>
#include <iDockViewer>
#include <iDialogSearch>
#include <iTreeView>
#include <ISelectable_PtrList>
#include <iTreeViewItem>
#include <iPanelViewer>
#include <ISelectableHost>
#include <taiWidgetMenuBar>
#include <iClipData>
#include <DockViewer>
#include <taGuiDialog>
#include <BrowseViewer>
#include <iBrowseViewer>
#include <cssConsoleWindow>
#include <iPanelBase>
#include <taFiler>
#include <PanelViewer>
#include <ToolBar>
#include <taProject>
#include <iSplitter>
#include <iTabView>
#include <T3PanelViewer>
#include <iHelpBrowser>
#include <iSubversionBrowser>
#include <iTextBrowser>
#include <iWidget_List>
#include <iRect>
#include <Program>
#include <MethodSpace>
#include <TypeSpace>
#include <MethodDef>
#include <iDialogChoice>
#include <KeyBindings>
#include <KeyBindings_List>
#include <iDialogKeyBindings>

#include <taMisc>
#include <taiMisc>
#include <tabMisc>
#include <taRootBase>
#include <taDataProc>
#include <taDataAnal>
#include <taDataGen>
#include <taImageProc>
#include <taWikiURL>

#include <QFileInfo>
#include <QUrl>
#include <QDesktopServices>
#include <QKeyEvent>
#include <QSignalMapper>
#include <QApplication>
#include <QPalette>
#include <QLineEdit>
#include <QTextEdit>
#include <QTextCursor>
#include <QClipboard>
#include <QWebView>
#include <QMenuBar>

int iMainWindowViewer::s_next_unique_id;
const QString iMainWindowViewer::cmd_str = "Ctrl+";

iMainWindowViewer::iMainWindowViewer(MainWindowViewer* viewer_, QWidget* parent)
: inherited(parent, (Qt::Window
    | Qt:: WindowSystemMenuHint
    | Qt::WindowMinMaxButtonsHint
#if (QT_VERSION >= 0x040500)
    | Qt::WindowCloseButtonHint
#endif
))
, IViewerWidget(viewer_)
{
  Init();
  m_is_root = viewer_->isRoot(); // need to do before Constr
  m_is_proj_browser = viewer_->isProjBrowser(); // need to do before Constr
  m_is_proj_viewer = viewer_->isProjViewer(); // need to do before Constr
  m_unique_id = s_next_unique_id++;
  // note: caller will still do a virtual Constr() on us after new
}

iMainWindowViewer::~iMainWindowViewer() {
  // this is a fix from http://bugreports.qt.nokia.com/browse/QTBUG-5279
  //   if (qt_mouseover == this)
  //     qt_mouseover = 0;
  // this doesn't work here -- too late in game
  //   if(isVisible())
  //     hide();                  // prevents crash later on mac..
  taiMisc::active_wins.RemoveEl(this);
  //TODO: need to delete menu, but just doing a delete causes an exception (prob because Qt
  // has already deleted the menu items
  if (menu) menu->deleteLater();
  menu = NULL;
}

#if defined(TA_OS_MAC) && (QT_VERSION >= 0x050200)
// defined in mac_objc_code.mm objective C file:
// per bug ticket: https://bugreports.qt-project.org/browse/QTBUG-38815
extern void TurnOffTouchEventsForWindow(QWindow* qtWindow);
#endif


void iMainWindowViewer::Init() {
  setAttribute(Qt::WA_DeleteOnClose);
#if defined(TA_OS_MAC) && (QT_VERSION >= 0x050200)
  setAttribute(Qt::WA_AcceptTouchEvents, false); // this doesn't work like it should
#endif

  //note: only a bare init -- most stuff done in virtual Constr() called after new
  brow_hist = new iBrowseHistory(this);
  cur_main_focus = LEFT_BROWSER;
  cur_sub_focus = MAIN_TREE;
  menu = NULL;
  body = NULL;
  last_clip_handler = NULL;
  last_sel_server = NULL;
  m_is_root = false;
  m_is_proj_browser = false;
  m_is_proj_viewer = false;
  m_close_proj_now = false; // only ever set once

  // allow win to be any size, even bigger than screen -- esp important for
  // multi-monitor situations, so you can size across screens
  // note that we do constrain windows on restore to be on screen, even if
  // their metrics indicated very big -- VERY IMPORTANT FOR MAC
  //  iSize ss = taiM->scrn_s;
  //  setMaximumSize(ss.width(), ss.height());

  //for some GD reason, we can't get the icon to show by doing this:
  //setWindowIcon(QApplication::windowIcon()); // supposed to be automatic, but doesn't seem to be...
  // so we curse and swear, and do this:
  QString app_ico_nm = ":/images/" + taMisc::app_name + "_32x32.png";
  QPixmap app_ico(app_ico_nm);
  setWindowIcon(app_ico);

  setFont(taiM->dialogFont(taiM->ctrl_size));

  (void)statusBar(); // creates the status bar

  // these are modal, so NULL them for when unused
  toolsMenu = NULL;
  historyBackAction = NULL;
  historyForwardAction = NULL;
  fileNewAction = NULL;
  fileOpenAction = NULL;
  fileSaveAction = NULL;
  fileSaveAsAction = NULL;
  fileSaveNotesAction = NULL;
  fileOpenSvnBrowserAction = NULL;
  fileSvnCommitAction = NULL;
  fileUpdateChangeLogAction = NULL;
  fileSaveAllAction = NULL;
  fileOpenFromWebMenu = NULL;
  filePublishProjectOnWebMenu = NULL;
  filePublishProjectOnWebAction = NULL;
  fileCloseAction = NULL;
  fileCloseWindowAction = NULL;
  fileQuitAction = NULL;
}

iAction* iMainWindowViewer::AddAction(iAction* act) {
  actions.Add(act); // refs the act
  // note: don't parent Actions because we manage them manually in our lists
  // note: because Qt only activates acts with shortcuts if visible, we need
  // to add the shorcutted guys to something visible... how about... us!
  if (!act->shortcut().isEmpty())
    this->addAction(act);
  return act;
}

void iMainWindowViewer::AddDockViewer(iDockViewer* dv, Qt::DockWidgetArea in_area) {
  if (!dv) return;
  addDockWidget(in_area, dv);
  // create a menu entry to show/hide it, regardless if visible now
  iAction* act = toolBarMenu->AddItem("Programming", taiWidgetMenu::toggle,
      iAction::men_act, this, SLOT(this_DockSelect(iAction*)), (void*)dv);
  if (dv->isVisible() != act->isChecked())
    act->setChecked(dv->isVisible()); // note: triggers action
  //TODO: maybe need to hook up signals for undocking
}

void iMainWindowViewer::AddFrameViewer(iFrameViewer* fv, int at_index) {
  if (at_index < 0) {
    body->addWidget(fv);
    at_index = body->count() - 1;
  }
  else {
    body->insertWidget(at_index, fv);
  }
  fv->m_window = this;
  //TODO: this stretch thing isn't working -- replace with sizing
  body->setStretchFactor(at_index, fv->stretchFactor());

  connect(this, SIGNAL(SelectableHostNotifySignal(ISelectableHost*, int)),
      fv, SLOT(SelectableHostNotifySlot_External(ISelectableHost*, int)) );
  connect(fv, SIGNAL(SelectableHostNotifySignal(ISelectableHost*, int)),
      this, SLOT(SelectableHostNotifySlot(ISelectableHost*, int)) );

  iAction* act = frameMenu->AddItem(fv->viewer()->GetName(), taiWidgetMenu::toggle,
      iAction::men_act, this, SLOT(this_FrameSelect(iAction*)), (void*)fv);

  //TODO: the show decision should probably be elsewhere
  if (fv->viewer()->isVisible())
    fv->viewer()->Show(); // always needed when adding guys to visible

  if (fv->viewer()->isVisible() != act->isChecked())
    act->setChecked(fv->isVisible()); // note: triggers action

}

// this guy exists because we must always be able to add a panel,
// so if there isn't already a panel viewer, we have to add one
void iMainWindowViewer::AddPanel(iPanelBase* panel, bool new_tab) {
  iPanelViewer* itv = GetTabViewer(true);
  if (new_tab) {
    itv->AddPanelNewTab(panel);
  } else {
    itv->AddPanel(panel); // typically for ctrl panels
  }
}

iPanelViewer* iMainWindowViewer::GetTabViewer(bool force) {
  int idx;
  PanelViewer* tv = (PanelViewer*)viewer()->FindFrameByType(&TA_PanelViewer, idx);
  if (!tv) {
    if (!force) return NULL;
    tv = (PanelViewer*)viewer()->AddFrameByType(&TA_PanelViewer);
    tv->Constr(); // parented when added
    AddFrameViewer(tv->widget(), 1); // usually in middle
  }
  return tv->widget();
}

void iMainWindowViewer::EditItem(taiSigLink* link, bool not_in_cur) {
  iPanelViewer* itv = GetTabViewer(true);
  itv->ShowLink(link, not_in_cur);
}

void iMainWindowViewer::AddApplicationToolBar(iToolBar* itb) {
  if (!itb->parent())
    itb->setParent(this); // needs parent otherwise will leak
  itb->m_window = this;
  addToolBar(Qt::TopToolBarArea, itb); // Qt methodTODO: should specify area
  // create a menu entry to show/hide it, regardless if visible now
  toolBarMenu->AddItem(itb->objectName(), taiWidgetMenu::toggle,
      iAction::men_act, this, SLOT(this_ToolBarSelect(iAction*)), (void*)itb);
  // if initially invisible, hide it
  ToolBar* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (!tb->visible) {
    tb->Hide();
  }
}

void iMainWindowViewer::ch_destroyed() {
  last_clip_handler = NULL;
  UpdateUi();
}

void iMainWindowViewer::closeEvent(QCloseEvent* e) {
  // always closing if force-quitting or we no longer have our mummy
  CancelOp cancel_op = ((taMisc::quitting == taMisc::QF_FORCE_QUIT) || (!m_viewer)) ?
      CO_NOT_CANCELLABLE : CO_PROCEED;
  closeEvent_Handler(e, cancel_op);
  // now, if we are the last proj window, close us!
  if (m_close_proj_now) {
    hide();                     // prevent a possible bug on mac associated with hide and delete
    taiMiscCore::ProcessEvents();
    curProject()->CloseLater();
  }
}

void iMainWindowViewer::raise() {
  QMainWindow::raise();
  AlignCssConsole();
  ProjDirToCurrent();
}

void iMainWindowViewer::moveEvent(QMoveEvent* e) {
  QMainWindow::moveEvent(e);
  //  taProject* prj = curProject();
  AlignCssConsole();
}

void iMainWindowViewer::resizeEvent(QResizeEvent* e) {
  inherited::resizeEvent(e);
  // use this to check for initializing the hacky frame_s value
  if ((taiM->frame_s.h + taiM->frame_s.w) == 0) {
    QRect r = frameGeometry();
    taiM->frame_s.h = r.height() - height();
    taiM->frame_s.w = r.width() - width();
  }
  AlignCssConsole();
}

void iMainWindowViewer::ProjDirToCurrent() {
  taProject* prj = curProject();
  if(!prj) return;
  prj->ProjDirToCurrent();
}

bool iMainWindowViewer::AlignCssConsole() {
  taProject* prj = curProject();
  if(!prj) return false;
  if(!taMisc::console_win) return false;
  QRect r = frameGeometry();
  int nw_top = r.bottom() + 1;
  int nw_ht = taiM->scrn_s.h - nw_top - 64; // leave a fixed amount of space at bottom.
  if(nw_ht < 40) nw_ht = 40;
  taMisc::console_win->LockedNewGeom(r.left(), nw_top, r.width(), nw_ht);
  return true;
}

void iMainWindowViewer::showEvent(QShowEvent* e) {
  inherited::showEvent(e);
  MainWindowViewer* db = viewer();
  if (!db) return;
  taBase* data = db->data();
  if(!data) return;
  tabMisc::DelayedFunCall_gui(data, "WindowShowHook");
  // make it delayed so window should actuall show first!
  AlignCssConsole();
  ProjDirToCurrent();
#ifdef TA_OS_MAC
  // per this bug with 2.8.x on mac, we need to regain focus:  https://bugreports.qt-project.org/browse/QTBUG-22911
  // setFocus();
#endif
// doesn't work here: https://bugreports.qt-project.org/browse/QTBUG-38815
// #if defined(TA_OS_MAC) && (QT_VERSION >= 0x050200)
//   TurnOffTouchEventsForWindow(windowHandle());
// #endif
}

void iMainWindowViewer::hideEvent(QHideEvent* e) {
  inherited::hideEvent(e);
  MainWindowViewer* db = viewer();
  if (!db) return;
  taBase* data = db->data();
  if(!data) return;
  tabMisc::DelayedFunCall_gui(data, "WindowHideHook");
}

void iMainWindowViewer::Constr_impl() {
  Constr_MainMenu_impl();
  Constr_Menu_impl();
  // we always put the fileclose or quit action at bottom of menu
  fileMenu->insertSeparator();
  fileMenu->AddAction(fileCloseWindowAction);

  //note: on Mac, this Quit will get moved to app menu (needed on all toplevel wins)
#ifndef TA_OS_MAC
  if (isRoot())
#endif
  {
    fileMenu->AddAction(fileQuitAction);
  }

  body = new iSplitter(); // def is hor
  setCentralWidget(body);
  body->installEventFilter(this); // translate keys..
  body->show();

  taiMisc::active_wins.AddUnique(this);
}

void iMainWindowViewer::Constr_MainMenu_impl() {
  if (menu) return;
  // create a taiWidgetMenu wrapper around the window's provided menubar
#ifdef TA_OS_MAC
  if(isRoot()) {
    menu = new taiWidgetMenuBar(this, taiMisc::fonBig, new QMenuBar(0));
    // no parent = application-wide menu bar, instead of window specific
  }
  else {
    // the problem here is that we don't seem to have a way to find the gobal menu
    // bar defined for root object!  we could just find it directly somehow..
    menu = new taiWidgetMenuBar(this, taiMisc::fonBig, menuBar());
  }
#else
  menu = new taiWidgetMenuBar(this, taiMisc::fonBig, menuBar());
#endif

  fileMenu = menu->AddSubMenu("F&ile");
  editMenu = menu->AddSubMenu("&Edit");
  viewMenu = menu->AddSubMenu("&View");
  show_menu = menu->AddSubMenu("&Show");
  ctrlMenu = menu->AddSubMenu("&Control");
  connect(show_menu->menu(), SIGNAL(aboutToShow()), this, SLOT(showMenu_aboutToShow()));
  // if (!(taMisc::show_gui & TypeItem::NO_EXPERT))
  toolsMenu = menu->AddSubMenu("&Tools");
  connect(toolsMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(toolsMenu_aboutToShow()));
  dataMenu = menu->AddSubMenu("&Data");
  connect(dataMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(dataMenu_aboutToShow()));
  windowMenu = menu->AddSubMenu("&Window");
  connect(windowMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(windowMenu_aboutToShow()));
  helpMenu = menu->AddSubMenu("&Help");
  connect(editMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(editMenu_aboutToShow()));
}

void iMainWindowViewer::Constr_Menu_impl()
{
  // Project actions are always created for menu consistency,
  // but are selectively enabled based on context.

  // Each of these functions creates action objects, adds them to menus,
  // and wires the signals/slots connections.
  Constr_FileMenu();
  Constr_EditMenu();
  Constr_ViewMenu();
  Constr_ShowMenu();
  Constr_ControlMenu();
  Constr_DataMenu();
  Constr_ToolsMenu();
  Constr_HelpMenu();
  Constr_WindowMenu();
}

void iMainWindowViewer::Constr_FileMenu()
{
  QKeySequence ks_new = taiMisc::GetSequenceFromAction(taiMisc::MENU_CONTEXT, taiMisc::MENU_NEW);
  fileNewAction = AddAction(new iAction("&New Project...", ks_new, "fileNewAction"));
  fileNewAction->setIcon(QIcon(":/images/filenew.png"));
  fileNewAction->setIconText("New");

  QKeySequence ks_open = taiMisc::GetSequenceFromAction(taiMisc::MENU_CONTEXT, taiMisc::MENU_OPEN);
  fileOpenAction = AddAction(new iAction("&Open Project...", ks_open, "fileOpenAction"));
  fileOpenAction->setIcon(QIcon(QPixmap(":/images/project_open_icon.png")));
  fileOpenAction->setIconText("Open");

  // so standard we don't offer custom key binding
  fileSaveAction = AddAction(new iAction("&Save Project", QKeySequence(QKeySequence::Save), "fileSaveAction"));
  fileSaveAction->setIcon(QIcon(QPixmap(":/images/save_icon.png")));
  fileSaveAction->setIconText("Save");

  // so standard we don't offer custom key binding
  fileSaveAsAction = AddAction(new iAction("Save Project &As...", QKeySequence(QKeySequence::SaveAs), "fileSaveAsAction"));
  fileSaveAsAction->setIcon(QIcon(QPixmap(":/images/save_as_icon.png")));
  fileSaveAsAction->setIconText("Save As");
  
  fileSaveNotesAction = AddAction(new iAction("Save Note &Changes...", QKeySequence(), "fileSaveNotesAction"));
  fileSaveAsTemplateAction = AddAction(new iAction("Save As &Template...", QKeySequence(), "fileSaveAsTemplate"));
  fileUpdateChangeLogAction = AddAction(new iAction("&Updt Change Log...", QKeySequence(), "fileUpdateChangeLogAction"));
  fileSaveAllAction = AddAction(new iAction("Save A&ll Projects", QKeySequence(), "fileSaveAllAction"));

  fileOpenSvnBrowserAction = AddAction(new iAction("SVN Browser", QKeySequence(), "fileOpenSvnBrowserAction"));
  fileOpenSvnBrowserAction->setToolTip(taiMisc::ToolTipPreProcess("open a subversion browser for the directory that this project is in"));
  fileOpenSvnBrowserAction->setIcon(QIcon(QPixmap(":/images/svn_browse_icon.png")));
  fileOpenSvnBrowserAction->setIconText("Browse");

  fileSvnCommitAction = AddAction(new iAction("SVN Commit...", QKeySequence(), "fileSvnCommitAction"));
  fileSvnCommitAction->setToolTip(taiMisc::ToolTipPreProcess("commit this project file (saves first) into svn (only if already added to svn repository -- use SvnBrowser to do that if necessary)"));
  fileSvnCommitAction->setIcon(QIcon(QPixmap(":/images/svn_commit_icon.png")));
  fileSvnCommitAction->setIconText("Commit");

  
  fileCloseAction = AddAction(new iAction("Close Project", QKeySequence(), "fileCloseAction"));
  fileCloseAction->setIcon(QIcon(QPixmap(":/images/project_close_icon.png")));
  fileCloseAction->setIconText("Close");

  fileOptionsAction = AddAction(new iAction("&Options...", QKeySequence(), "fileOptionsAction"));
  fileOptionsAction->setToolTip(taiMisc::ToolTipPreProcess("Set various GUI preferences and other settings"));
  fileOptionsAction->setIcon(QIcon(QPixmap(":/images/preferences_icon.png")));
  fileOptionsAction->setIconText("Prefs");

  // Build menu items.
  fileMenu->AddAction(fileNewAction);
  fileMenu->AddAction(fileOpenAction);
  fileOpenRecentMenu = fileMenu->AddSubMenu("Open &Recent");
  fileMenu->AddAction(fileSaveAction);
  fileMenu->AddAction(fileSaveAsAction);
  // fileExportMenu = fileMenu->AddSubMenu("Export"); // submenu -- empty and disabled in base
  // fileMenu->AddAction(filePrintAction);

  fileMenu->insertSeparator();
  fileMenu->AddAction(fileSaveNotesAction);
  fileMenu->AddAction(fileSaveAsTemplateAction);
  fileMenu->AddAction(fileUpdateChangeLogAction);
  fileMenu->AddAction(fileSaveAllAction);
  fileMenu->insertSeparator();

  fileMenu->AddAction(fileOpenSvnBrowserAction);
  fileMenu->AddAction(fileSvnCommitAction);

  fileMenu->insertSeparator();
  fileOpenFromWebMenu = fileMenu->AddSubMenu("Open Project from &Web");
  if (!isRoot()) {
    filePublishProjectOnWebMenu = fileMenu->AddSubMenu("Publish &Project on Web");
    fileUpdateProjectOnWebMenu = fileMenu->AddSubMenu("Update Project on Web");
    fileUploadFilesForProjectOnWebMenu = fileMenu->AddSubMenu("Upload Project Files to Web");
  }
//  fileUpdateProjectOnWebAction = AddAction(new iAction("Update Project on Web", QKeySequence(), "fileUpdateProjectOnWebAction"));
//  fileUploadFilesForProjectOnWebAction = AddAction(new iAction("Upload Project Files to Web", QKeySequence(), "fileUploadFilesForProjectOnWebAction"));
//  fileMenu->AddAction(fileUpdateProjectOnWebAction);
//  fileMenu->AddAction(fileUploadFilesForProjectOnWebAction);

  fileMenu->insertSeparator();
  fileMenu->AddAction(fileCloseAction);
  fileMenu->AddAction(fileOptionsAction);
  
  fileRecentsClearAction = AddAction(new iAction("Clear Menu", QKeySequence(), "fileRecentsClearAction"));
  connect(fileRecentsClearAction, SIGNAL(Action()), this, SLOT(fileClearRecentsMenu()));

  fileCloseWindowAction = AddAction(new iAction("C&lose Window", QKeySequence(cmd_str + "W"), "fileCloseWindowAction"));
  connect(fileCloseWindowAction, SIGNAL(Action()), this, SLOT(fileCloseWindow()));

  connect(fileNewAction, SIGNAL(Action()), this, SLOT(fileNew()));
  connect(fileOpenAction, SIGNAL(Action()), this, SLOT(fileOpen()));
  connect(fileOpenRecentMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(fileOpenRecent_aboutToShow()));
  connect(fileSaveAllAction, SIGNAL(Action()), this, SLOT(fileSaveAll()));

  // Connect "open from web" for all windows (even root).
  connect(fileOpenFromWebMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(fileOpenFromWeb_aboutToShow()));

  // TBD: is this the same as checking if (!isRoot()) ??
  // Logic for isProjShower() is unclear.
  if (isProjShower()) {
    //    if (!curProject()->save_as_only) {
    connect(fileSaveAction, SIGNAL(Action()), this, SLOT(fileSave()));
    //    }
    //    else {
    //      fileSaveAction->setEnabled(false);
    //    }
    connect(fileSaveAsAction, SIGNAL(Action()), this, SLOT(fileSaveAs()));
    connect(fileSaveNotesAction, SIGNAL(Action()), this, SLOT(fileSaveNotes()));
    connect(fileSaveAsTemplateAction, SIGNAL(Action()), this, SLOT(fileSaveAsTemplate()));
    connect(fileUpdateChangeLogAction, SIGNAL(Action()), this, SLOT(fileUpdateChangeLog()));

    connect(fileOpenSvnBrowserAction, SIGNAL(Action()), this, SLOT(fileOpenSvnBrowser()));
    connect(fileSvnCommitAction, SIGNAL(Action()), this, SLOT(fileSvnCommit()));

    // Connect "publish" options only for project windows.
    connect(filePublishProjectOnWebMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(filePublishProjectOnWeb_aboutToShow()));
    connect(fileUpdateProjectOnWebMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(fileUpdateProjectOnWeb_aboutToShow()));
    connect(fileUploadFilesForProjectOnWebMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(fileUploadFilesForProjectOnWeb_aboutToShow()));
//    connect(fileUpdateProjectOnWebAction, SIGNAL(Action()), this, SLOT(fileUpdateProjectOnWeb()));
//    connect(fileUploadFilesForProjectOnWebAction, SIGNAL(Action()), this, SLOT(fileUploadFilesForProjectOnWeb()));

    connect(fileCloseAction, SIGNAL(Action()), this, SLOT(fileClose()));
  }
  else {
    fileSaveAction->setEnabled(false);
    fileSaveAsAction->setEnabled(false);
    fileSaveNotesAction->setEnabled(false);
    fileSaveAsTemplateAction->setEnabled(false);
    fileUpdateChangeLogAction->setEnabled(false);
    fileCloseAction->setEnabled(false);

    fileOpenSvnBrowserAction->setEnabled(false);
    fileSvnCommitAction->setEnabled(false);
  }

  // Disable "Publish Project" -- won't be enabled until docs are first published.
//  filePublishProjectOnWebAction->setEnabled(false);

  // connect(filePrintAction, SIGNAL(activated()), this, SLOT(filePrint()));
  connect(fileOptionsAction, SIGNAL(Action()), this, SLOT(fileOptions()));

#ifndef TA_OS_MAC
  if (isRoot())
#endif
  {
    fileQuitAction = AddAction(new iAction("&Quit", QKeySequence(cmd_str + "Q"),
        "fileQuitAction"));
    connect(fileQuitAction, SIGNAL(Action()), this, SLOT(fileQuit()));
  }
}

void iMainWindowViewer::Constr_EditMenu()
{
  // so standard we don't offer custom key binding
  editUndoAction = AddAction(new iAction("&Undo", QKeySequence::Undo, "editUndoAction"));
  editUndoAction->setIcon(QIcon(QPixmap(":/images/editundo.png")));
  editRedoAction = AddAction(new iAction("&Redo", QKeySequence::Redo, "editRedoAction"));
  editRedoAction->setIcon(QIcon(QPixmap(":/images/editredo.png")));

  editCutAction = AddAction(new iAction(iClipData::EA_CUT, "Cu&t", QKeySequence::Cut, "editCutAction"));
  editCutAction->setIcon(QIcon(QPixmap(":/images/editcut.png")));
  editCopyAction = AddAction(new iAction(iClipData::EA_COPY, "&Copy", QKeySequence::Copy, "editCopyAction"));
  editCopyAction->setIcon(QIcon(QPixmap(":/images/editcopy.png")));

  // Note: we twiddle the visibility, shortcuts, and accelerator for the Paste and Link guys
  QPixmap editpaste(":/images/editpaste.png");
  editPasteAction = AddAction(new iAction(iClipData::EA_PASTE, "&Paste", QKeySequence::Paste, "editPasteAction"));
  editPasteAction->setIcon(QIcon(editpaste));
  editPasteIntoAction = AddAction(new iAction(iClipData::EA_PASTE_INTO, "&Paste Into", QKeySequence::Paste, "editPasteIntoAction"));
  editPasteIntoAction->setIcon(QIcon(editpaste));
  editPasteAssignAction = AddAction(new iAction(iClipData::EA_PASTE_ASSIGN, "&Paste Assign", QKeySequence(), "editPasteAssignAction"));
  editPasteAssignAction->setIcon(QIcon(editpaste));
  editPasteAppendAction = AddAction(new iAction(iClipData::EA_PASTE_APPEND, "&Paste Append", QKeySequence(), "editPasteAppendAction"));
  editPasteAppendAction->setIcon(QIcon(editpaste));
  QKeySequence ks_delete = taiMisc::GetSequenceFromAction(taiMisc::MENU_CONTEXT, taiMisc::MENU_DELETE);
  editDeleteAction = AddAction(new iAction(iClipData::EA_DELETE, "&Delete", ks_delete, "editDeleteAction"));
  QKeySequence ks_dupe = taiMisc::GetSequenceFromAction(taiMisc::MENU_CONTEXT, taiMisc::MENU_DUPLICATE);
  editDupeAction = AddAction(new iAction(iClipData::EA_DUPE, "Duplicate", ks_dupe, "editDuplicateAction"));

  editLinkAction = AddAction(new iAction(iClipData::EA_LINK, "&Link", QKeySequence(), "editLinkAction"));
  editLinkIntoAction = AddAction(new iAction(iClipData::EA_LINK, "&Link Into", QKeySequence(), "editLinkIntoAction"));
  editUnlinkAction = AddAction(new iAction(iClipData::EA_LINK, "Unlin&k", QKeySequence(), "editUnlinkAction"));

  QKeySequence ks_find = taiMisc::GetSequenceFromAction(taiMisc::MENU_CONTEXT, taiMisc::MENU_FIND);
  editFindAction = AddAction(new iAction(0, "&Find...", ks_find, "editFindAction"));
  editFindAction->setIcon(QIcon(QPixmap(":/images/find_icon.png")));
  QKeySequence ks_find_next = taiMisc::GetSequenceFromAction(taiMisc::MENU_CONTEXT, taiMisc::MENU_FIND_NEXT);
  editFindNextAction = AddAction(new iAction(0, "Find &Next", ks_find_next, "editFindNextAction"));

  // Build menu items.
  editMenu->AddAction(editUndoAction);
  editMenu->AddAction(editRedoAction);

  editMenu->insertSeparator();
  editMenu->AddAction(editCutAction);
  editMenu->AddAction(editCopyAction);
  editMenu->AddAction(editDupeAction);
  editMenu->AddAction(editPasteAction);
  editMenu->AddAction(editPasteIntoAction);
  editMenu->AddAction(editPasteAssignAction);
  editMenu->AddAction(editPasteAppendAction);
  editMenu->AddAction(editLinkAction);
  editMenu->AddAction(editLinkIntoAction);
  editMenu->AddAction(editUnlinkAction);
  editMenu->AddAction(editDeleteAction);

  editMenu->insertSeparator();
  editMenu->AddAction(editFindAction);
  editMenu->AddAction(editFindNextAction);

  // Make connections.
  connect(editUndoAction, SIGNAL(Action()), this, SLOT(editUndo()));
  connect(editRedoAction, SIGNAL(Action()), this, SLOT(editRedo()));
  connect(editCutAction, SIGNAL(Action()), this, SLOT(editCut()));
  connect(editCopyAction, SIGNAL(Action()), this, SLOT(editCopy()));
  connect(editPasteAction, SIGNAL(Action()), this, SLOT(editPaste()));
  connect(editPasteIntoAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editPasteAssignAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editPasteAppendAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editLinkAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editLinkIntoAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editUnlinkAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editDeleteAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editDupeAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editFindAction, SIGNAL(Action()), this, SLOT(editFind()));
  connect(editFindNextAction, SIGNAL(Action()), this, SLOT(editFindNext()));
  
  editUndoAction->setEnabled(false);
  editRedoAction->setEnabled(false);
  editCutAction->setEnabled(false);
  editCopyAction->setEnabled(false);
  editPasteAction->setEnabled(false);
  editPasteIntoAction->setEnabled(false);
  editPasteAssignAction->setEnabled(false);
  editPasteAppendAction->setEnabled(false);
  editDeleteAction->setEnabled(false);
  editDupeAction->setEnabled(false);
  
  // don't show on startup - and only later when sensible
  editPasteIntoAction->setVisible(false);
  editPasteAssignAction->setVisible(false);
  editPasteAppendAction->setVisible(false);
}

void iMainWindowViewer::Constr_ViewMenu()
{
  KeyBindings* bindings = taMisc::key_binding_lists->SafeEl(static_cast<int>(taMisc::current_key_bindings));

  viewBrowseOnlyAction = AddAction(new iAction("Navigator Only", bindings->KeySequence(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_NAVIGATOR_ONLY), "viewBrowseOnlyAction"));
  viewPanelsOnlyAction = AddAction(new iAction("Editor Only", bindings->KeySequence(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_EDITOR_ONLY), "viewPanelsOnlyAction"));
  viewBrowseAndPanelsAction = AddAction(new iAction("Navigator and Editor", bindings->KeySequence(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_NAVIGATOR_AND_EDITOR), "viewBrowseAndPanelsAction"));
  viewT3OnlyAction = AddAction(new iAction("Visualizer Only", bindings->KeySequence(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_VISUALIZER_ONLY), "viewT3OnlyAction"));
  viewBrowseAndT3Action = AddAction(new iAction("Navigator and Visualizer", bindings->KeySequence(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_NAVIGATOR_AND_VISUALIZER), "viewBrowseAndT3Action"));
  viewPanelsAndT3Action = AddAction(new iAction("Editor and Visualizer", bindings->KeySequence(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_EDITOR_AND_VISUALIZER), "viewPanelsAndT3Action"));
  viewAllFramesAction = AddAction(new iAction("All Frames", bindings->KeySequence(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_ALL_FRAMES), "viewAllFramesAction"));

  viewRefreshAction = AddAction(new iAction("&Refresh", QKeySequence("F5"), "viewRefreshAction"));

  // Forward and back buttons -- note: on Win the icons don't show up if Action has text
  historyBackAction = AddAction(new iAction("Back",
      QKeySequence(Qt::ControlModifier + Qt::Key_Left), "historyBackAction"));
  historyBackAction->setToolTip(taiMisc::ToolTipPreProcess("Move back in object browsing history\nCtrl + Left"));
  historyBackAction->setStatusTip(historyBackAction->toolTip());

  historyForwardAction = AddAction(new iAction("Forward",
      QKeySequence(Qt::ControlModifier + Qt::Key_Right), "historyForwardAction"));
  historyForwardAction->setToolTip(taiMisc::ToolTipPreProcess("Move forward in object browsing history\nCtrl + Right"));
  historyForwardAction->setStatusTip(historyForwardAction->toolTip());

  // When window first created, there is no history, so manually disable.
  historyBackAction->setEnabled(false);
  historyForwardAction->setEnabled(false);

  // Build menu items.
  viewMenu->AddAction(viewRefreshAction);

  viewMenu->insertSeparator();
  viewMenu->AddAction(historyBackAction);
  viewMenu->AddAction(historyForwardAction);

  viewMenu->insertSeparator();
  frameMenu = viewMenu->AddSubMenu("Frames");
  toolBarMenu = viewMenu->AddSubMenu("Toolbars");

  viewMenu->insertSeparator();
  viewResetViewAction = AddAction(new iAction("Reset Frames", bindings->KeySequence(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_RESET_FRAMES), "viewResetViewAction"));
  viewMenu->AddAction(viewBrowseOnlyAction);
  viewMenu->AddAction(viewPanelsOnlyAction);
  viewMenu->AddAction(viewBrowseAndPanelsAction);
  viewMenu->AddAction(viewT3OnlyAction);
  viewMenu->AddAction(viewBrowseAndT3Action);
  viewMenu->AddAction(viewPanelsAndT3Action);
  viewMenu->AddAction(viewAllFramesAction);
  viewMenu->AddAction(viewResetViewAction);

  viewSetSaveViewAction = viewMenu->AddItem("Save View State", taiWidgetMenu::toggle,
      iAction::men_act, this, SLOT(this_SaveView(iAction*)));
  if (curProject() != NULL)
    viewSetSaveViewAction->setChecked(curProject()->save_view);  // reinstate setting

  // Make connections.
  connect(historyBackAction, SIGNAL(triggered()), brow_hist, SLOT(back()));
  connect(brow_hist, SIGNAL(back_enabled(bool)), historyBackAction, SLOT(setEnabled(bool)));

  connect(historyForwardAction, SIGNAL(triggered()), brow_hist, SLOT(forward()));
  connect(brow_hist, SIGNAL(forward_enabled(bool)), historyForwardAction, SLOT(setEnabled(bool)));

  connect(this, SIGNAL(SelectableHostNotifySignal(ISelectableHost*, int)),
      brow_hist, SLOT(SelectableHostNotifying(ISelectableHost*, int)));
  connect(brow_hist, SIGNAL(select_item(taiSigLink*)),
      this, SLOT(slot_AssertBrowserItem(taiSigLink*)));

  connect(viewResetViewAction, SIGNAL(Action()), this, SLOT(ViewReset()));
  connect(viewRefreshAction, SIGNAL(Action()), this, SLOT(viewRefresh()));

  signalMapperForViews = new QSignalMapper (this) ;
  connect (viewBrowseOnlyAction, SIGNAL(triggered()), signalMapperForViews, SLOT(map())) ;
  connect (viewPanelsOnlyAction, SIGNAL(triggered()), signalMapperForViews, SLOT(map())) ;
  connect (viewBrowseAndPanelsAction, SIGNAL(triggered()), signalMapperForViews, SLOT(map())) ;
  connect (viewT3OnlyAction, SIGNAL(triggered()), signalMapperForViews, SLOT(map())) ;
  connect (viewBrowseAndT3Action, SIGNAL(triggered()), signalMapperForViews, SLOT(map())) ;
  connect (viewPanelsAndT3Action, SIGNAL(triggered()), signalMapperForViews, SLOT(map())) ;
  connect (viewAllFramesAction, SIGNAL(triggered()), signalMapperForViews, SLOT(map())) ;

  signalMapperForViews->setMapping (viewBrowseOnlyAction, 1) ;
  signalMapperForViews->setMapping (viewPanelsOnlyAction, 2) ;
  signalMapperForViews->setMapping (viewBrowseAndPanelsAction, 3) ;
  signalMapperForViews->setMapping (viewT3OnlyAction, 4) ;
  signalMapperForViews->setMapping (viewBrowseAndT3Action, 5) ;
  signalMapperForViews->setMapping (viewPanelsAndT3Action, 6) ;
  signalMapperForViews->setMapping (viewAllFramesAction, 7) ;

  connect (signalMapperForViews, SIGNAL(mapped(int)), this, SLOT(ShowHideFrames(int))) ;
}

void iMainWindowViewer::Constr_ShowMenu()
{
  // These two items are presets for the other toggle flags.
  show_menu->AddItem("Normal &only", taiWidgetMenu::normal, iAction::men_act,
      this, SLOT(ShowChange(iAction*)), 0);
  show_menu->AddItem("&All", taiWidgetMenu::normal, iAction::men_act,
      this, SLOT(ShowChange(iAction*)), 1);

  // Toggle flags.
  // Note: correct toggles are set dynamically when user drops down menu.
  show_menu->AddSep();
  show_menu->AddItem("&Normal", taiWidgetMenu::toggle, iAction::men_act,
      this, SLOT(ShowChange(iAction*)), 2);
  show_menu->AddItem("E&xpert", taiWidgetMenu::toggle, iAction::men_act,
      this, SLOT(ShowChange(iAction*)), 3);
  show_menu->AddItem("&Hidden", taiWidgetMenu::toggle, iAction::men_act,
      this, SLOT(ShowChange(iAction*)), 4);
}

void iMainWindowViewer::Constr_WindowMenu()
{
  QKeySequence ks_minimize = taiMisc::GetSequenceFromAction(taiMisc::MENU_CONTEXT, taiMisc::MENU_MINIMIZE);
  windowMinimizeAction = AddAction(new iAction("&Minimize", ks_minimize, "windowMinimizeAction"));
  windowMenu->AddAction(windowMinimizeAction);
  windowZoomAction = AddAction(new iAction("&Zoom", QKeySequence(), "windowZoomAction"));
  windowMenu->AddAction(windowZoomAction);

  connect(windowMinimizeAction, SIGNAL(Action()), this, SLOT(WindowMinimize()));
  connect(windowZoomAction, SIGNAL(Action()), this, SLOT(WindowZoom()));
}

void iMainWindowViewer::Constr_ControlMenu()
{
  KeyBindings* bindings = taMisc::key_binding_lists->SafeEl(static_cast<int>(taMisc::current_key_bindings));

  ctrlStopAction = AddAction(new iAction("Stop", QKeySequence(bindings->KeySequence(taiMisc::MENU_CONTEXT, taiMisc::MENU_STOP)), "ctrlStopAction"));
  ctrlStopAction->setIcon(QIcon(QPixmap(":/images/stop_icon.png")));
  ctrlStopAction->setToolTip(taiMisc::ToolTipPreProcess("Stop: stop whatever program is currently running -- execution can be resumed with the Cont continue button."));

  ctrlContAction = AddAction(new iAction("Cont", QKeySequence(bindings->KeySequence(taiMisc::MENU_CONTEXT, taiMisc::MENU_CONTINUE)), "ctrlContAction"));
  ctrlContAction->setIcon(QIcon(QPixmap(":/images/play_icon.png")));
  ctrlContAction->setToolTip(taiMisc::ToolTipPreProcess("Continue: continue running the last program that was run, from wherever it was last stopped"));
  
  ctrlStepAction = AddAction(new iAction("Step", QKeySequence(bindings->KeySequence(taiMisc::MENU_CONTEXT, taiMisc::MENU_STEP)), "ctrlStepAction"));
  ctrlStepAction->setIcon(QIcon(QPixmap(":/images/step_icon.png")));
  ctrlStepAction->setToolTip(taiMisc::ToolTipPreProcess("Step: Step again"));
  
  progStatusAction = AddAction(new iAction("Program Status", QKeySequence(), "progStatusAction"));
  progStatusAction->setToolTip(taiMisc::ToolTipPreProcess("Current Program status -- click button to see the current Global Backtrace of programs run"));
  
  // Build menu items.
  ctrlMenu->AddAction(ctrlStopAction);
  ctrlMenu->AddAction(ctrlContAction);
  ctrlMenu->AddAction(ctrlStepAction);
  ctrlMenu->AddAction(progStatusAction);

  // Make connections.
  connect(ctrlStopAction, SIGNAL(Action()), this, SLOT(ctrlStop()));
  connect(ctrlContAction, SIGNAL(Action()), this, SLOT(ctrlCont()));
  connect(ctrlStepAction, SIGNAL(Action()), this, SLOT(ctrlStep()));
  connect(progStatusAction, SIGNAL(Action()), this, SLOT(progStatus()));
}

void iMainWindowViewer::Constr_DataMenu() {
  signalMapperForDataProc = new QSignalMapper (this);
  signalMapperForDataAnal = new QSignalMapper (this);
  signalMapperForDataGen = new QSignalMapper (this);
  signalMapperForImageProc = new QSignalMapper (this);
  
  processMenu = dataMenu->AddSubMenu("Process");
  analysisMenu = dataMenu->AddSubMenu("Analyze");
  generateMenu = dataMenu->AddSubMenu("Generate");
  processImageMenu = dataMenu->AddSubMenu("Process Image");
  
  MethodSpace* methods;
  MethodDef* mdef;
  TypeDef* type;

  // Build the action lists for taDataProc methods
  type = &TA_taDataProc;
  methods = &type->methods;
  for (int i=0; i<methods->size; i++) {
    mdef = methods->FastEl(i);
    if (mdef == NULL)
      continue;
    bool show = true;
    if (mdef->HasOption("MENU_BUTTON")) {
      TypeSpace* args = &mdef->arg_types;
      for (int j=0; j<args->size; j++) {
        TypeDef* arg_type = args->FastEl(j);
        if (arg_type->DerivesFromName("DataOpBaseSpec") || arg_type->DerivesFromName("DataOpList")) {
          show = false;
          break;
        }
      }
      
      if (show) {
        String label = mdef->name + "…"; taMisc::SpaceLabel(label);
        if (mdef->HasOption("CAT_Copy")) {
          String actionString = "dataProcess" + mdef->name + "Action";
          dataProcCopyActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));  // put the method name into usr_data - needed for later call to actual method
        }
        if (mdef->HasOption("CAT_Order")) {
          String actionString = "dataProcess" + mdef->name + "Action";
          dataProcOrderActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
        else if (mdef->HasOption("CAT_Select")) {
          String actionString = "dataProcess" + mdef->name + "Action";
          dataProcSelectActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
        else if (mdef->HasOption("CAT_Columns")) {
          String actionString = "dataProcess" + mdef->name + "Action";
          dataProcColumnsActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
      }
    }
  }
  
  for (int i=0; i<dataProcCopyActions.size; i++) {
    processMenu->AddAction(dataProcCopyActions[i]);
    connect (dataProcCopyActions[i], SIGNAL(Action()), signalMapperForDataProc, SLOT(map())) ;
    signalMapperForDataProc->setMapping
      (dataProcCopyActions[i], dataProcCopyActions[i]->usr_data.toQString());
  }
  processMenu->AddSep();
  for (int i=0; i<dataProcOrderActions.size; i++) {
    processMenu->AddAction(dataProcOrderActions[i]);
    connect (dataProcOrderActions[i], SIGNAL(Action()), signalMapperForDataProc, SLOT(map())) ;
    signalMapperForDataProc->setMapping
      (dataProcOrderActions[i], dataProcOrderActions[i]->usr_data.toQString());
  }
  processMenu->AddSep();
  for (int i=0; i<dataProcSelectActions.size; i++) {
    processMenu->AddAction(dataProcSelectActions[i]);
    connect (dataProcSelectActions[i], SIGNAL(Action()), signalMapperForDataProc, SLOT(map())) ;
    signalMapperForDataProc->setMapping
      (dataProcSelectActions[i], dataProcSelectActions[i]->usr_data.toQString());
  }
  processMenu->AddSep();
  for (int i=0; i<dataProcColumnsActions.size; i++) {
    processMenu->AddAction(dataProcColumnsActions[i]);
    connect (dataProcColumnsActions[i], SIGNAL(Action()), signalMapperForDataProc, SLOT(map())) ;
    signalMapperForDataProc->setMapping
      (dataProcColumnsActions[i], dataProcColumnsActions[i]->usr_data.toQString());
  }
  
  // Build the action lists for taDataAnal methods
  type = &TA_taDataAnal;
  methods = &type->methods;
  for (int i=0; i<methods->size; i++) {
    mdef = methods->FastEl(i);
    if (mdef == NULL)
      continue;
    bool show = true;
    if (mdef->HasOption("MENU_BUTTON")) {
      TypeSpace* args = &mdef->arg_types;
      // if there were methods we didn't want to show (perhaps they require a spec) put that logic here
      if (show) {
        String label = mdef->name + "…"; taMisc::SpaceLabel(label);
        if (mdef->HasOption("CAT_Stats")) {
          String actionString = "dataAnal" + mdef->name + "Action";
          dataAnalStatsActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));  // put the method name into usr_data - needed for later call to actual method
        }
        else if (mdef->HasOption("CAT_Distance")) {
          String actionString = "dataAnal" + mdef->name + "Action";
          dataAnalDistanceActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
        else if (mdef->HasOption("CAT_HighDim")) {
          String actionString = "dataAnal" + mdef->name + "Action";
          dataAnalHighDimActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
        else if (mdef->HasOption("CAT_Clean")) {
          String actionString = "dataAnal" + mdef->name + "Action";
          dataAnalCleanActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
        else if (mdef->HasOption("CAT_Graph")) {
          String actionString = "dataAnal" + mdef->name + "Action";
          dataAnalGraphActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
      }
    }
  }

  for (int i=0; i<dataAnalStatsActions.size; i++) {
    analysisMenu->AddAction(dataAnalStatsActions[i]);
    connect (dataAnalStatsActions[i], SIGNAL(Action()), signalMapperForDataAnal, SLOT(map())) ;
    signalMapperForDataAnal->setMapping
      (dataAnalStatsActions[i], dataAnalStatsActions[i]->usr_data.toQString());
  }
  analysisMenu->AddSep();
  for (int i=0; i<dataAnalDistanceActions.size; i++) {
    analysisMenu->AddAction(dataAnalDistanceActions[i]);
    connect (dataAnalDistanceActions[i], SIGNAL(Action()), signalMapperForDataAnal, SLOT(map())) ;
    signalMapperForDataAnal->setMapping
      (dataAnalDistanceActions[i], dataAnalDistanceActions[i]->usr_data.toQString());
  }
  analysisMenu->AddSep();
  for (int i=0; i<dataAnalHighDimActions.size; i++) {
    analysisMenu->AddAction(dataAnalHighDimActions[i]);
    connect (dataAnalHighDimActions[i], SIGNAL(Action()), signalMapperForDataAnal, SLOT(map())) ;
    signalMapperForDataAnal->setMapping
      (dataAnalHighDimActions[i], dataAnalHighDimActions[i]->usr_data.toQString());
  }
  analysisMenu->AddSep();
  for (int i=0; i<dataAnalCleanActions.size; i++) {
    analysisMenu->AddAction(dataAnalCleanActions[i]);
    connect (dataAnalCleanActions[i], SIGNAL(Action()), signalMapperForDataAnal, SLOT(map())) ;
    signalMapperForDataAnal->setMapping
      (dataAnalCleanActions[i], dataAnalCleanActions[i]->usr_data.toQString());
  }
  analysisMenu->AddSep();
  for (int i=0; i<dataAnalGraphActions.size; i++) {
    analysisMenu->AddAction(dataAnalGraphActions[i]);
    connect (dataAnalGraphActions[i], SIGNAL(Action()), signalMapperForDataAnal, SLOT(map())) ;
    signalMapperForDataAnal->setMapping
      (dataAnalGraphActions[i], dataAnalGraphActions[i]->usr_data.toQString());
  }
  
  // Build the action lists for taDataGen methods
  type = &TA_taDataGen;
  methods = &type->methods;
  for (int i=0; i<methods->size; i++) {
    mdef = methods->FastEl(i);
    if (mdef == NULL)
      continue;
    bool show = true;
    if (mdef->HasOption("MENU_BUTTON")) {
      TypeSpace* args = &mdef->arg_types;
      for (int j=0; j<args->size; j++) {
        TypeDef* arg_type = args->FastEl(j);
        if (arg_type->DerivesFromName("SubMatrixOpSpec")) {
          show = false;
          break;
        }
      }
      
      if (show) {
        String label = mdef->name + "…"; taMisc::SpaceLabel(label);
        if (mdef->HasOption("CAT_Basic")) {
          String actionString = "dataGen" + mdef->name + "Action";
          dataGenBasicActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));  // put the method name into usr_data - needed for later call to actual method
        }
        if (mdef->HasOption("CAT_Lists")) {
          String actionString = "dataGen" + mdef->name + "Action";
          dataGenListsActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
        if (mdef->HasOption("CAT_Draw")) {
          String actionString = "dataGen" + mdef->name + "Action";
          dataGenDrawActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
        if (mdef->HasOption("CAT_Random")) {
          String actionString = "dataGen" + mdef->name + "Action";
          dataGenRandomActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
        if (mdef->HasOption("CAT_FeatPats")) {
          String actionString = "dataGen" + mdef->name + "Action";
          dataGenFeatPatsActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
        if (mdef->HasOption("CAT_Files")) {
          String actionString = "dataGen" + mdef->name + "Action";
          dataGenFilesActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
      }
    }
  }
  
  for (int i=0; i<dataGenBasicActions.size; i++) {
    generateMenu->AddAction(dataGenBasicActions[i]);
    connect (dataGenBasicActions[i], SIGNAL(Action()), signalMapperForDataGen, SLOT(map())) ;
    signalMapperForDataGen->setMapping
      (dataGenBasicActions[i], dataGenBasicActions[i]->usr_data.toQString());
  }
  generateMenu->AddSep();
  for (int i=0; i<dataGenListsActions.size; i++) {
    generateMenu->AddAction(dataGenListsActions[i]);
    connect (dataGenListsActions[i], SIGNAL(Action()), signalMapperForDataGen, SLOT(map())) ;
    signalMapperForDataGen->setMapping
      (dataGenListsActions[i], dataGenListsActions[i]->usr_data.toQString());
  }
  generateMenu->AddSep();
  for (int i=0; i<dataGenDrawActions.size; i++) {
    generateMenu->AddAction(dataGenDrawActions[i]);
    connect (dataGenDrawActions[i], SIGNAL(Action()), signalMapperForDataGen, SLOT(map())) ;
    signalMapperForDataGen->setMapping
      (dataGenDrawActions[i], dataGenDrawActions[i]->usr_data.toQString());
  }
  generateMenu->AddSep();
  for (int i=0; i<dataGenRandomActions.size; i++) {
    generateMenu->AddAction(dataGenRandomActions[i]);
    connect (dataGenRandomActions[i], SIGNAL(Action()), signalMapperForDataGen, SLOT(map())) ;
    signalMapperForDataGen->setMapping
      (dataGenRandomActions[i], dataGenRandomActions[i]->usr_data.toQString());
  }
  generateMenu->AddSep();
  for (int i=0; i<dataGenFeatPatsActions.size; i++) {
    generateMenu->AddAction(dataGenFeatPatsActions[i]);
    connect (dataGenFeatPatsActions[i], SIGNAL(Action()), signalMapperForDataGen, SLOT(map())) ;
    signalMapperForDataGen->setMapping
      (dataGenFeatPatsActions[i], dataGenFeatPatsActions[i]->usr_data.toQString());
  }
  generateMenu->AddSep();
  for (int i=0; i<dataGenFilesActions.size; i++) {
    generateMenu->AddAction(dataGenFilesActions[i]);
    connect (dataGenFilesActions[i], SIGNAL(Action()), signalMapperForDataGen, SLOT(map())) ;
    signalMapperForDataGen->setMapping
      (dataGenFilesActions[i], dataGenFilesActions[i]->usr_data.toQString());
  }

  // Build the action lists for taImageProc methods
  type = &TA_taImageProc;
  methods = &type->methods;
  for (int i=0; i<methods->size; i++) {
    mdef = methods->FastEl(i);
    if (mdef == NULL)
      continue;
    bool show = true;
    if (mdef->HasOption("MENU_BUTTON")) {
      TypeSpace* args = &mdef->arg_types;
      // if there were methods we didn't want to show (perhaps they require a spec) put that logic here
      if (show) {
        String label = mdef->name + "…"; taMisc::SpaceLabel(label);
        if (mdef->HasOption("CAT_Transform")) {
          String actionString = "imageProc" + mdef->name + "Action";
          imageProcTransformActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));  // put the method name into usr_data - needed for later call to actual method
        }
        if (mdef->HasOption("CAT_Filter")) {
          String actionString = "imageProc" + mdef->name + "Action";
          imageProcFilterActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
        if (mdef->HasOption("CAT_Noise")) {
          String actionString = "imageProc" + mdef->name + "Action";
          imageProcNoiseActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
        if (mdef->HasOption("CAT_ImageProc")) {
          String actionString = "imageProc" + mdef->name + "Action";
          imageProcImageProcActions.Add(new iAction(mdef->name, label, QKeySequence(), actionString));
        }
      }
    }
  }
  
  for (int i=0; i<imageProcTransformActions.size; i++) {
    processImageMenu->AddAction(imageProcTransformActions[i]);
    connect (imageProcTransformActions[i], SIGNAL(Action()), signalMapperForImageProc, SLOT(map())) ;
    signalMapperForImageProc->setMapping
      (imageProcTransformActions[i], imageProcTransformActions[i]->usr_data.toQString());
  }
  processImageMenu->AddSep();
  for (int i=0; i<imageProcFilterActions.size; i++) {
    processImageMenu->AddAction(imageProcFilterActions[i]);
    connect (imageProcFilterActions[i], SIGNAL(Action()), signalMapperForImageProc, SLOT(map())) ;
    signalMapperForImageProc->setMapping
      (imageProcFilterActions[i], imageProcFilterActions[i]->usr_data.toQString());
  }
  processImageMenu->AddSep();
  for (int i=0; i<imageProcNoiseActions.size; i++) {
    processImageMenu->AddAction(imageProcNoiseActions[i]);
    connect (imageProcNoiseActions[i], SIGNAL(Action()), signalMapperForImageProc, SLOT(map())) ;
    signalMapperForImageProc->setMapping
      (imageProcNoiseActions[i], imageProcNoiseActions[i]->usr_data.toQString());
  }
  processImageMenu->AddSep();
  for (int i=0; i<imageProcImageProcActions.size; i++) {
    processImageMenu->AddAction(imageProcImageProcActions[i]);
    connect (imageProcImageProcActions[i], SIGNAL(Action()), signalMapperForImageProc, SLOT(map())) ;
    signalMapperForImageProc->setMapping
      (imageProcImageProcActions[i], imageProcImageProcActions[i]->usr_data.toQString());
  }
  
  connect (signalMapperForDataProc, SIGNAL(mapped(QString)), this, SLOT(DataProcLauncher(QString))) ;
  connect (signalMapperForDataAnal, SIGNAL(mapped(QString)), this, SLOT(DataAnalLauncher(QString))) ;
  connect (signalMapperForDataGen, SIGNAL(mapped(QString)), this, SLOT(DataGenLauncher(QString))) ;
  connect (signalMapperForImageProc, SIGNAL(mapped(QString)), this, SLOT(ImageProcLauncher(QString))) ;
}

void iMainWindowViewer::Constr_ToolsMenu()
{
  toolsChooseKeyBindingsAction = AddAction(new iAction(0, "Choose Key Bindings...", QKeySequence(), "toolsChooseKeyBindingsAction"));
  toolsDiffProjectsAction = AddAction(new iAction(0, "Projects...", QKeySequence(), "toolsDiffProjectsAction"));
  toolsDiffProgramsAction = AddAction(new iAction(0, "Programs...", QKeySequence(), "toolsDiffProgramsAction"));
  toolsDiffDataTablesAction = AddAction(new iAction(0, "Data Tables...", QKeySequence(), "toolsDiffDataTablesAction"));
  toolsDiffNetworksAction = AddAction(new iAction(0, "Networks...", QKeySequence(), "toolsDiffNetworksAction"));
  toolsDiffLayersAction = AddAction(new iAction(0, "Layers...", QKeySequence(), "toolsDiffLayersAction"));
  toolsDiffSpecsAction = AddAction(new iAction(0, "Specs...", QKeySequence(), "toolsDiffSpecsAction"));
  
  toolsHelpBrowseAction = AddAction(new iAction(0, "&Help Browser", QKeySequence(), "toolsHelpBrowseAction"));
  toolsTypeInfoBrowseAction = AddAction(new iAction(0, "Type Info Browser", QKeySequence(), "toolsTypeInfoBrowseAction"));
  
  toolsSvnBrowseActionEmergent = AddAction(new iAction(0, "&SVN Browser Emergent", QKeySequence(), "toolsSvnBrowseActionEmergent"));
  toolsSvnBrowseActionSvn1 = AddAction
  (new iAction(0, String("SVN Browser Repo ") + taMisc::svn_repo1_url.name,
               QKeySequence(), "toolsSvnBrowseActionSvn1"));
  toolsSvnBrowseActionSvn2 = AddAction
  (new iAction(0, String("SVN Browser Repo ") + taMisc::svn_repo2_url.name,
               QKeySequence(), "toolsSvnBrowseActionSvn2"));
  toolsSvnBrowseActionSvn3 = AddAction
  (new iAction(0, String("SVN Browser Repo ") + taMisc::svn_repo3_url.name,
               QKeySequence(), "toolsSvnBrowseActionSvn3"));
  
  toolsOpenServerAction = AddAction(new iAction(0, "&Open Remote Server...", QKeySequence(), "toolsOpenServerAction"));
  toolsCloseServerAction = AddAction(new iAction(0, "&Close Remote Server", QKeySequence(), "toolsCloseServerAction"));
  
  
  // Build menu items.
  if (toolsMenu) {
    diffCompareMenu = toolsMenu->AddSubMenu("Diff Compare");
    diffCompareMenu->AddAction(toolsDiffProjectsAction);
    diffCompareMenu->AddAction(toolsDiffProgramsAction);
    diffCompareMenu->AddAction(toolsDiffDataTablesAction);
    diffCompareMenu->AddAction(toolsDiffNetworksAction);
    diffCompareMenu->AddAction(toolsDiffLayersAction);
    diffCompareMenu->AddAction(toolsDiffSpecsAction);
    toolsMenu->AddAction(toolsChooseKeyBindingsAction);
    toolsMenu->insertSeparator();
    toolsMenu->AddAction(toolsHelpBrowseAction);
    toolsMenu->AddAction(toolsTypeInfoBrowseAction);
    toolsMenu->insertSeparator();
    toolsMenu->AddAction(toolsSvnBrowseActionEmergent);
    if(taMisc::svn_repo1_url.name.nonempty())
      toolsMenu->AddAction(toolsSvnBrowseActionSvn1);
    if(taMisc::svn_repo2_url.name.nonempty())
      toolsMenu->AddAction(toolsSvnBrowseActionSvn2);
    if(taMisc::svn_repo3_url.name.nonempty())
      toolsMenu->AddAction(toolsSvnBrowseActionSvn3);
    toolsMenu->insertSeparator();
    toolsMenu->AddAction(toolsOpenServerAction);
    toolsMenu->AddAction(toolsCloseServerAction);
  }
  
  // Make connetions.
  connect(toolsDiffProjectsAction, SIGNAL(triggered()),
          this, SLOT(toolsDiffProjects()));
  connect(toolsDiffProgramsAction, SIGNAL(triggered()),
          this, SLOT(toolsDiffPrograms()));
  connect(toolsDiffDataTablesAction, SIGNAL(triggered()),
          this, SLOT(toolsDiffDataTables()));
  connect(toolsDiffNetworksAction, SIGNAL(triggered()),
          this, SLOT(toolsDiffNetworks()));
  connect(toolsDiffLayersAction, SIGNAL(triggered()),
          this, SLOT(toolsDiffLayers()));
  connect(toolsDiffSpecsAction, SIGNAL(triggered()),
          this, SLOT(toolsDiffSpecs()));
  connect(toolsHelpBrowseAction, SIGNAL(triggered()),
          this, SLOT(toolsHelpBrowser()));
  connect(toolsSvnBrowseActionEmergent, SIGNAL(triggered()),
          this, SLOT(toolsSvnBrowserEmergent()));
  connect(toolsSvnBrowseActionSvn1, SIGNAL(triggered()),
          this, SLOT(toolsSvnBrowserSvn1()));
  connect(toolsSvnBrowseActionSvn2, SIGNAL(triggered()),
          this, SLOT(toolsSvnBrowserSvn2()));
  connect(toolsSvnBrowseActionSvn3, SIGNAL(triggered()),
          this, SLOT(toolsSvnBrowserSvn3()));
  connect(toolsTypeInfoBrowseAction, SIGNAL(triggered()),
          this, SLOT(toolsTypeInfoBrowser()));
  connect(toolsOpenServerAction, SIGNAL(triggered()),
          this, SLOT(toolsOpenRemoteServer()));
  connect(toolsCloseServerAction, SIGNAL(triggered()),
          this, SLOT(toolsCloseRemoteServer()));
  connect(toolsChooseKeyBindingsAction, SIGNAL(triggered()),
          this, SLOT(toolsChooseKeyBindings()));
}

void iMainWindowViewer::Constr_HelpMenu()
{
  String s = taMisc::app_name + " help on the web (F1)";
  helpHelpAction = AddAction(new iAction("&Help", QKeySequence("F1"), "helpHelpAction"));
  helpHelpAction->setIcon(QIcon(QPixmap(":/images/help_icon.png")));
  helpHelpAction->setToolTip(taiMisc::ToolTipPreProcess(s));
  helpHelpAction->setStatusTip(s);

  helpAboutAction = AddAction(new iAction("&About", QKeySequence(), "helpAboutAction"));
  helpFileBugAction = AddAction(new iAction("Report Bug", QKeySequence(), "helpFileBugAction"));
  helpFileBugAction->setIcon(QIcon(QPixmap(":/images/report_bug_icon.png")));
  helpFileBugAction->setToolTip(taiMisc::ToolTipPreProcess("Opens web page for reporting a problem or requesting an enhancement"));

  // Build menu items.
  helpMenu->AddAction(helpHelpAction);
  helpMenu->AddAction(helpFileBugAction);
  helpMenu->insertSeparator();
  helpMenu->AddAction(helpAboutAction);

  // Make connetions.
  connect(helpHelpAction, SIGNAL(Action()), this, SLOT(helpHelp()));
  connect(helpAboutAction, SIGNAL(Action()), this, SLOT(helpAbout()));
  connect(helpFileBugAction, SIGNAL(Action()), this, SLOT(FileBugReport()));
}


taProject* iMainWindowViewer::curProject() const {
  taProject* rval = NULL;
  if (isProjShower()) {
    MainWindowViewer* db = viewer();
    if (!db) return NULL; // shouldn't happen
    rval = dynamic_cast<taProject*>(db->data());
  }
  return rval;
}

taProject* iMainWindowViewer::myProject() const {
  MainWindowViewer* db = viewer();
  if (!db) return NULL; // shouldn't happen
  // first, check if a project browser, if so, then we have our puppy
  taBase* data = db->data();
  if (!data) return NULL;
  taProject* rval = dynamic_cast<taProject*>(data);
  // otherwise, will need to get its parent
  if (!rval) {
    rval = (taProject*)data->GetOwner(&TA_taProject);
  }
  return rval;
}

void iMainWindowViewer::emit_EditAction(int param) {
  emit EditAction(param);
  taiMisc::RunPending();
  UpdateUi();
}

/*obs void iMainWindowViewer::emit_selectionChanged()
{
  emit selectionChanged();
} */

void iMainWindowViewer::toolsChooseKeyBindings() {
  if (!key_bindings_dialog) {
    key_bindings_dialog = iDialogKeyBindings::New(this);
  }
  key_bindings_dialog->setAttribute(Qt::WA_DeleteOnClose);
  key_bindings_dialog->show();
  key_bindings_dialog->raise();
  key_bindings_dialog->activateWindow();
}

void iMainWindowViewer::Find(taiSigLink* root, const String& find_str) {
  // if an instance doesn't exist, need to make one; we tie it to ourself
  if (!search_dialog) {
    search_dialog = iDialogSearch::New(0, this);
  }
  iDialogSearch* dlg = search_dialog;

  dlg->setRoot(root);
  dlg->show();
  dlg->raise();
  dlg->activateWindow();
  if(find_str.nonempty())
    dlg->setSearchStr(find_str);
}

void iMainWindowViewer::Replace(taiSigLink* root, ISelectable_PtrList& sel_items,
    const String& srch, const String& repl) {

  taGuiDialog Dlg1;
  String curow;

  bool rval = false;

  static String sr_val;
  if(srch.nonempty()) sr_val = srch;
  static String rp_val;
  if(repl.nonempty()) rp_val = repl;
  static String mb_flt;

  Dlg1.Reset();
  Dlg1.prompt = "Search and Replace";
  Dlg1.win_title = "Search and Replace";
  Dlg1.AddWidget("main", "", "");
  Dlg1.AddVBoxLayout("mainv", "", "main", "");
  curow = "instr";
  Dlg1.AddHBoxLayout(curow, "mainv", "", "");
  Dlg1.AddLabel("Instructions", "main", curow,
      "label=Enter strings to search and replace for in currently selected items\nmember filter restricts replace to specific member field names\n(search does not support regular expressions);");
  Dlg1.AddSpace(20, "mainv");
  curow = "srchrow";
  Dlg1.AddHBoxLayout(curow, "mainv", "", "");
  String sttip = "tooltip=enter string value to search for;";
  Dlg1.AddLabel("srchlbl", "main", curow, "label=Search for:    ;"+sttip);
  Dlg1.AddStringField(&sr_val, "srch", "main", curow, sttip);
  curow = "replrow";
  Dlg1.AddHBoxLayout(curow, "mainv", "", "");
  String rttip = "tooltip=enter string value to replace with;";
  Dlg1.AddLabel("repllbl", "main", curow, "label=Replace with: ;"+rttip);
  Dlg1.AddStringField(&rp_val, "repl", "main", curow, rttip);
  Dlg1.AddSpace(20, "mainv");
  curow = "mbfltrow";
  Dlg1.AddHBoxLayout(curow, "mainv", "", "");
  String mttip =
      "tooltip=The member name must contain the specified string (e.g. 'name' or 'lrate')\nin order for the Replace operation to apply. If the member filter is left blank, \nthen the Replace operation applies to all members.;";
  Dlg1.AddLabel("mbfltlbl", "main", curow, "label=Member filter: ;"+mttip);
  Dlg1.AddStringField(&mb_flt, "mbflt", "main", curow, mttip);
  Dlg1.AddSpace(20, "mainv");

  int drval = Dlg1.PostDialog(true);
  if(drval == 0) {
    return;
  }

  taProject* proj = curProject();
  if(proj) {
    proj->undo_mgr.SaveUndo(proj, "ReplaceValStr", NULL, false, proj); // global save
  }

  for (int i = 0; i < sel_items.size; ++i) {
    ISelectable* ci = sel_items.SafeEl(i);
    if (!ci) continue;
    taBase* tab =  ci->taData();// is the effLink data
    if (!tab) continue;
    tab->ReplaceValStr(sr_val, rp_val, mb_flt);
  }
}

void iMainWindowViewer::editUndo() {
  taProject* proj = myProject();
  if (!proj) return;
  proj->undo_mgr.Undo();
}

void iMainWindowViewer::editRedo() {
  taProject* proj = myProject();
  if (!proj) return;
  proj->undo_mgr.Redo();
}

void iMainWindowViewer::editFind() {
  // assume root of this window's browser
  taiSigLink* root = NULL;

  MainWindowViewer* db = viewer();
  BrowseViewer* bv = NULL;
  int idx;
  if (db) {
    bv = (BrowseViewer*)db->FindFrameByType(&TA_BrowseViewer, idx);
    if (bv) root = bv->rootLink();
  }
  if (!root) root = (taiSigLink*)tabMisc::root->GetSigLink();
  Find(root);
}

void iMainWindowViewer::editFindNext() {
  // if we don't have a find window, just do Find
  if (search_dialog) {
    search_dialog->FindNext();
  } else {
    editFind();
  }
}

void iMainWindowViewer::fileCloseWindow() {
  if (isRoot() && this->widget()->isActiveWindow()) {
    int chs = taMisc::Choice("Quit Emergent?", "&Cancel", "&Quit");
    if (chs == 1) {
      this->widget()->close();
    }
    return;
  }

  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    if (widget->isActiveWindow()) {
      widget->close();
      break;
    }
  }
}

// note: all the fileXxx for projects are safe -- they don't get enabled
//  unless they are applicable (ex Save only if it is a viewer)
void iMainWindowViewer::fileNew() {
  if (!tabMisc::root) return;
  //   tabMisc::root->projects.CallFun("New_gui"); // let user choose type
  tabMisc::root->projects.CallFun("NewFromTemplate"); // let user choose type
  //should automatically open
}

void iMainWindowViewer::fileOpen() {
  if (!tabMisc::root) return;
  TypeDef* td = &TA_taProject;
  // get filename ourself, so we can check if it is already open!
  taFiler* flr = tabMisc::root->projects.GetFiler(td);
  taRefN::Ref(flr);
  //TODO: context path  flr->setFileName(fname);
  if (flr->GetFileName(taFiler::foOpen)) {
    String fname = flr->FileName();
    fileOpenFile(fname);
  }
  taRefN::unRefDone(flr);
}

void iMainWindowViewer::fileOpenRecent_aboutToShow() {
  // Clear and rebuild submenu.
  fileOpenRecentMenu->Reset();

  // Populate with recent files.
  for (int i = 0; i < tabMisc::root->recent_files.size; ++i) {
    String filename = taMisc::CompressFilePath(tabMisc::root->recent_files[i]);
    
    fileOpenRecentMenu->AddItem(filename, iAction::var_act,
      this, SLOT(fileOpenFile(const Variant&)), filename);
  }
  fileOpenRecentMenu->AddSep();
  fileOpenRecentMenu->AddAction(fileRecentsClearAction);
}

void iMainWindowViewer::fileOpenFile(const Variant& fname_) {
  String fname = taMisc::ExpandFilePath(fname_.toString());
  // check if already open
  taProject* proj = NULL;
  // canonicalize name, for comparison to open projects
  QFileInfo fi(fname);
  bool exists = fi.exists();
  fname = fi.canonicalFilePath(); // why is this needed if exists is false?
  if (exists) {
    for (int i = 0; i < tabMisc::root->projects.size; ++i) {
      proj = tabMisc::root->projects.FastEl(i);
      if (proj && proj->file_name == fname) {
        iDialogChoice::ConfirmDialog(NULL, "Project already open -- will come to front", "", false);
        break; // break out of loop
      }
      proj = NULL; // in case we fall out of loop
    }
  }
  // if proj has a value, then we should view the existing, else open it
  bool clear_dirty = true; // only for new loads, or old views when not dirty already
  if (proj) {
    if (proj->isDirty()) clear_dirty = false; // don't clear if still dirty
    // this will automatically view it if already open
    proj->AssertDefaultProjectBrowser(true);
  } else {
    taBase* el = NULL;
    tabMisc::root->projects.Load(fname, &el);
    proj = (taProject*)el;
    //note: gets autoviewed in its postload routine
  }
  tabMisc::root->AddRecentFile(fname);
  //note: the viewing will have set us dirty again, after load cleared it
  if (proj && clear_dirty)
    proj->setDirty(false);
}

void iMainWindowViewer::fileQuit() {
  // note: following does a save of prefs
  taiMiscCore::Quit(CO_PROCEED); // not forced, until main closes
}

void iMainWindowViewer::fileSave() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->Save();
}

void iMainWindowViewer::fileSaveAs() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->SaveAs();
}

void iMainWindowViewer::fileSaveNotes() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->SaveNoteChanges();
}

void iMainWindowViewer::fileSaveAsTemplate() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->CallFun("SaveAsTemplate"); // need to prompt for args
}

void iMainWindowViewer::fileUpdateChangeLog() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->UpdateChangeLog();
}

void iMainWindowViewer::fileOpenSvnBrowser() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->SvnBrowser();
}

void iMainWindowViewer::fileSvnCommit() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->SvnCommit();
}

void iMainWindowViewer::fileSaveAll() {
  if (!tabMisc::root) return;
  tabMisc::root->SaveAll();
}

void iMainWindowViewer::fileOpenFromWeb_aboutToShow()
{
  // Clear and rebuild submenu.
  fileOpenFromWebMenu->Reset();
  String wiki_name = "Emergent repository";
  String label = wiki_name + "...";
  
  for(int i=0;i<taMisc::wikis.size; i++) {
    String wiki_name = taMisc::wikis[i].name;
    String label = wiki_name + "...";
    fileOpenFromWebMenu->AddItem(label, iAction::var_act, this, SLOT(fileOpenFromWeb(const Variant &)), wiki_name);
  }
}

void iMainWindowViewer::fileOpenFromWeb(const Variant &repo)
{
  String repositoryName = repo.toString();
  // TODO.
}

void iMainWindowViewer::filePublishProjectOnWeb_aboutToShow()
{
  // Clear and rebuild submenu.
  filePublishProjectOnWebMenu->Reset();
  for(int i=0;i<taMisc::wikis.size; i++) {
    String wiki_name = taMisc::wikis[i].name;
    String label = wiki_name + "...";
    filePublishProjectOnWebMenu->AddItem(label, iAction::var_act, this, SLOT(filePublishProjectOnWeb(const Variant &)), wiki_name);
  }
}

void iMainWindowViewer::filePublishProjectOnWeb(const Variant &repo)
{
  String repositoryName = repo.toString();
  
  taProject *proj = curProject();
  if (!repositoryName.empty() && proj) {
    proj->PublishProjectOnWeb(repositoryName);
  }
}

void iMainWindowViewer::fileUpdateProjectOnWeb_aboutToShow()
{
  // Clear and rebuild submenu.
  fileUpdateProjectOnWebMenu->Reset();
  for(int i=0;i<taMisc::wikis.size; i++) {
    String wiki_name = taMisc::wikis[i].name;
    String label = wiki_name + "...";
    fileUpdateProjectOnWebMenu->AddItem(label, iAction::var_act, this, SLOT(fileUpdateProjectOnWeb(const Variant &)), wiki_name);
  }
}

void iMainWindowViewer::fileUpdateProjectOnWeb(const Variant &repo)
{
  String repositoryName = repo.toString();

  taProject *proj = curProject();
  if (proj) {
    proj->UpdateProjectOnWeb(repositoryName);  // repository name held by project
  }
}

void iMainWindowViewer::fileUploadFilesForProjectOnWeb_aboutToShow()
{
  // Clear and rebuild submenu.
  fileUploadFilesForProjectOnWebMenu->Reset();
  for(int i=0;i<taMisc::wikis.size; i++) {
    String wiki_name = taMisc::wikis[i].name;
    String label = wiki_name + "...";
    fileUploadFilesForProjectOnWebMenu->AddItem(label, iAction::var_act, this, SLOT(fileUploadFilesForProjectOnWeb(const Variant &)), wiki_name);
  }
}

void iMainWindowViewer::fileUploadFilesForProjectOnWeb(const Variant &repo)
{
  String repositoryName = repo.toString();

  taProject *proj = curProject();
  if (proj) {
    proj->UploadFilesForProjectOnWeb(repositoryName); // repository name held by project
  }
}


void iMainWindowViewer::fileClose() {
  if(taMisc::is_saving) {
    taMisc::Warning("Cannot close: currently saving -- try again later");
  }
  if(taMisc::is_undo_loading || taMisc::is_undo_saving) {
    taMisc::Warning("Cannot close: currently undoing -- try again later");
  }
  if(taMisc::is_loading) {
    taMisc::Warning("Cannot close: currently loading -- try again later");
  }
  taProject* proj = curProject();
  if (!proj) return;
  // Check for dirty/save
  if (proj->isDirty()) {
    int chs= taMisc::Choice("The project: " + taMisc::GetFileFmPath(proj->file_name) + " has unsaved changes -- do you want to save before closing it?",
        "&Save", "&Don't Save", "&Cancel");

    switch (chs) {
    case 0:
      SaveData();
      break;
    case 1:
      break;
    case 2:
      return;
    }
  }
  hide();       // prevent a possible bug on mac associated with hide and delete
  taiMiscCore::ProcessEvents();
  proj->CloseLater();
}

void iMainWindowViewer::fileOptions() {
  if (!tabMisc::root) return;
  tabMisc::root->Options();
}

void iMainWindowViewer::ctrlStop() {
  Program::SetStopReq(Program::SR_USER_STOP, "Main window Stop Button");
}

void iMainWindowViewer::ctrlCont() {
  if(!Program::last_run_prog) {
    taMisc::Error("Continue: cannot continue because there is no record of which program was running previously");
    return;
  }
  taMisc::Info("Continue: running program:", Program::last_run_prog->name);
  Program::last_run_prog->Run_Gui();
}

void iMainWindowViewer::ctrlStep() {
  if(!Program::last_step_prog) {
    taMisc::Error("Step: cannot step because there is no record of which step was last run");
    return;
  }
  Program::last_run_prog->Step(Program::last_step_prog);
}

void iMainWindowViewer::progStatus() {
  if(Program::last_run_prog) {
    Program::last_run_prog->GlobalTrace();
  }
  else {
    taMisc::Info("No global trace available -- no last run program");
  }
}

void iMainWindowViewer::DataProcLauncher(QString method_name) {
  String meth_name(method_name.toStdString().c_str());
  if (myProject()) {
    taBase* inst = myProject()->FindMakeNewDataProc(&TA_taDataProc, "");
    if (inst) {
      inst->CallFun(meth_name);
    }
  }
}

void iMainWindowViewer::DataAnalLauncher(QString method_name) {
  String meth_name(method_name.toStdString().c_str());
  if (myProject()) {
    taBase* inst = myProject()->FindMakeNewDataProc(&TA_taDataAnal, "");
    if (inst) {
      inst->CallFun(meth_name);
    }
  }
}

void iMainWindowViewer::DataGenLauncher(QString method_name) {
  String meth_name(method_name.toStdString().c_str());
  if (myProject()) {
    taBase* inst = myProject()->FindMakeNewDataProc(&TA_taDataGen, "");
    if (inst) {
      inst->CallFun(meth_name);
    }
  }
}

void iMainWindowViewer::ImageProcLauncher(QString method_name) {
  String meth_name(method_name.toStdString().c_str());
  if (myProject()) {
    taBase* inst = myProject()->FindMakeNewDataProc(&TA_taImageProc, "");
    if (inst) {
      inst->CallFun(meth_name);
    }
  }
}

iTreeView* iMainWindowViewer::GetMainTreeView() {
  MainWindowViewer* db = viewer();
  if (!db) return NULL;
  BrowseViewer* bv = db->GetLeftBrowser();
  if (!bv) return NULL;
  iBrowseViewer* ibv = bv->widget();
  if (!ibv) return NULL;
  return ibv->lvwDataTree;
}

iTreeView* iMainWindowViewer::GetCurTreeView() {
  if(cur_tree_view) return cur_tree_view;
  return GetMainTreeView();
}

bool iMainWindowViewer::FocusCurTreeView() {
  iTreeView* itv = GetCurTreeView();
  if(!itv) return false;
  itv->setFocus();
  if(itv == GetMainTreeView()) {
    cur_main_focus = LEFT_BROWSER;
    cur_sub_focus = MAIN_TREE;
  }
  else {
    cur_main_focus = MIDDLE_PANEL;
    cur_sub_focus = PROG_TREE;
  }
  return true;
}

bool iMainWindowViewer::FocusLeftBrowser() {
  iTreeView* itv = GetMainTreeView();
  if(itv) {
    itv->setFocus();
    cur_main_focus = LEFT_BROWSER;
    cur_sub_focus = MAIN_TREE;
    return true;
  }
  return false;
}

bool iMainWindowViewer::FocusMiddlePanel() {
  //  iTreeView* mtv = GetMainTreeView();
  MainWindowViewer* db = viewer();
  if(!db) return false;
  PanelViewer* pv = db->GetMiddlePanel();
  if(!pv || !pv->widget()) return false;
  iPanelViewer* itv = pv->widget();
  iPanelBase* idp = itv->tabView()->curPanel();
  if(idp) {
    QWidget* ffwid = idp->firstTabFocusWidget();
    if(ffwid) {
      ffwid->setFocus();
      cur_main_focus = MIDDLE_PANEL;
      cur_sub_focus = PROG_TREE; // probably..
      return true;
    }
  }
  itv->tabView()->tabBar()->setFocus();
  cur_main_focus = MIDDLE_PANEL;
  cur_sub_focus = MIDDLE_TABS;
  return true;
}

bool iMainWindowViewer::FocusRightViewer() {
  MainWindowViewer* db = viewer();
  if(!db) return false;
  T3PanelViewer* pv = db->GetRightViewer();
  if(!pv || !pv->widget()) return false;
  if(pv->tabBar())
    pv->tabBar()->setFocus();
  cur_main_focus = RIGHT_VIEWER;
  cur_sub_focus = RIGHT_TABS;
  return true;
}

void iMainWindowViewer::FocusIsLeftBrowser() {
  cur_main_focus = LEFT_BROWSER;
  cur_sub_focus = MAIN_TREE;
}

void iMainWindowViewer::FocusIsMiddlePanel(WinSubLocs sub_loc) {
  cur_main_focus = MIDDLE_PANEL;
  cur_sub_focus = sub_loc;
}

void iMainWindowViewer::FocusIsRightViewer(WinSubLocs sub_loc) {
  cur_main_focus = RIGHT_VIEWER;
  cur_sub_focus = sub_loc;
}

bool iMainWindowViewer::MoveFocusLeft() {
  switch(cur_main_focus) {
  case LEFT_BROWSER:
    return FocusRightViewer();
  case MIDDLE_PANEL:
    return FocusLeftBrowser();
  case RIGHT_VIEWER:
    return FocusMiddlePanel();
  }
  return false;
}

bool iMainWindowViewer::MoveFocusRight() {
  switch(cur_main_focus) {
  case LEFT_BROWSER:
    return FocusMiddlePanel();
  case MIDDLE_PANEL:
    return FocusRightViewer();
  case RIGHT_VIEWER:
    return FocusLeftBrowser();
  }
  return false;
}

bool iMainWindowViewer::ShiftCurTabRight() {
  MainWindowViewer* db = viewer();
  if(!db) return false;
  if(cur_main_focus == RIGHT_VIEWER) {
    T3PanelViewer* pv = db->GetRightViewer();
    if(pv && pv->tabBar()) {
      pv->tabBar()->selectNextTab();
    }
  }
  else {
    iPanelViewer* itv = GetTabViewer();
    if(itv) {
      itv->tabBar()->selectNextTab();
    }
  }
  return true;
}

bool iMainWindowViewer::ShiftCurTabLeft() {
  MainWindowViewer* db = viewer();
  if(!db) return false;
  if(cur_main_focus == RIGHT_VIEWER) {
    T3PanelViewer* pv = db->GetRightViewer();
    if(pv && pv->tabBar()) {
      pv->tabBar()->selectPrevTab();
    }
  }
  else {
    iPanelViewer* itv = GetTabViewer();
    if(itv) {
      itv->tabBar()->selectPrevTab();
    }
  }
  return true;
}

iTreeViewItem* iMainWindowViewer::AssertBrowserItem(taiSigLink* link) {
  iTreeView* itv = GetCurTreeView(); // note: use current
  if(!itv) return NULL;
  // note: waitproc is now insulated against recurrent calls..
  // make sure previous operations are finished
  taiMiscCore::ProcessEvents();
  iTreeViewItem* rval = itv->AssertItem(link);
  if (rval) {
    if(!(itv == cur_tree_view && rval == itv->topLevelItem(0))) {
      // never assert the first item on a non-main browser -- likely the owner of a sub browser and that is very distracting for scrolling
      activateWindow();
      itv->setFocus();
      itv->clearExtSelection();
      // clear the selection first: select does NOT show the middle panel viewer 
      // work if it is already selected!!!
      itv->setCurrentItem(NULL, 0, QItemSelectionModel::Clear);
      itv->scrollTo(rval);
      itv->setCurrentItem(rval, 0, QItemSelectionModel::ClearAndSelect);
    }
  }
  else if(itv == cur_tree_view) { // try again with main
    itv = GetMainTreeView();
    itv->clearExtSelection();
    rval = itv->AssertItem(link);
    if (rval) {
      activateWindow();
      itv->setFocus();
      // clear the selection first: select does NOT show the middle panel viewer 
      // work if it is already selected!!!
      itv->setCurrentItem(NULL, 0, QItemSelectionModel::Clear);
      itv->scrollTo(rval);
      itv->setCurrentItem(rval, 0, QItemSelectionModel::ClearAndSelect);
    }
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
  return rval;
}

iTreeViewItem* iMainWindowViewer::BrowserExpandAllItem(taiSigLink* link) {
  iTreeView* itv = GetCurTreeView(); // note: use current
  if(!itv) return NULL;
  // note: waitproc is now insulated against recurrent calls..
  // make sure previous operations are finished
  taiMiscCore::ProcessEvents();
  iTreeViewItem* rval = itv->AssertItem(link);
  if (rval) {
    itv->ExpandAllUnder(rval);
  }
  else if(itv == cur_tree_view) { // try again with main
    itv = GetMainTreeView();
    rval = itv->AssertItem(link);
    if (rval) {
      itv->ExpandAllUnder(rval);
    }
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
  return rval;
}

iTreeViewItem* iMainWindowViewer::BrowserCollapseAllItem(taiSigLink* link) {
  iTreeView* itv = GetCurTreeView(); // note: use current
  if(!itv) return NULL;
  // make sure previous operations are finished
  taiMiscCore::ProcessEvents();
  iTreeViewItem* rval = itv->AssertItem(link);
  if (rval) {
    itv->CollapseAllUnder(rval);
  }
  else if(itv == cur_tree_view) { // try again with main
    itv = GetMainTreeView();
    rval = itv->AssertItem(link);
    if (rval) {
      itv->CollapseAllUnder(rval);
    }
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
  return rval;
}

bool iMainWindowViewer::AssertPanel(taiSigLink* link,
    bool new_tab, bool new_tab_lock)
{
  if (!new_tab) new_tab_lock = false;
  iPanelViewer* itv = GetTabViewer(); // should exist
  if (!itv) return false;

  iPanelBase* pan = itv->tabView()->GetDataPanel(link);
  if (!pan) return false; // shouldn't happen

  if (new_tab) {
    itv->AddPanelNewTab(pan, new_tab_lock);
  } else {
    itv->AddPanel(pan);
  }
  // make sure previous operations are finished
  taiMiscCore::ProcessEvents();
  return true;
}

// this is only for internally-generated ta:// links

void iMainWindowViewer::taUrlHandler(const QUrl& url) {
  // URLs are usually suffixed with a "#winid_xxx" where xxx is the uniqueId()
  // of the window in which is embedded the doc viewer
  // if no WinId then we use that of the topmost window

  // NOTE: URLs only open in the main project browser for that project
  String path = url.path(); // will only be part before #, if any

  // TypeBrowser invocations are dependency-free so we check them first
  if(path.startsWith(".Type.")) {
    iHelpBrowser::StatLoadUrl(String(url.toString()));
    return;
  }

  int win_id = 0;
  iMainWindowViewer* top_win = NULL;

  // we usually embed the uniqueId of the win that had the invoking url
  String frag_str;
  if(url.hasFragment())
    frag_str = String(url.fragment());
  if(frag_str.startsWith("winid_")) {
    win_id = frag_str.after("winid_").toInt(); // 0 if empty or not an int
    top_win = taiMisc::active_wins.FindMainWindowById(win_id);
  }
  
  if(top_win == NULL) {         // fallback
    top_win = taiMisc::active_wins.Peek_MainWindow();
    if (top_win)
      win_id = top_win->uniqueId();
  }
  
  // get the project -- should be able to get from any viewer/browser
  taProject* proj = NULL;
  if (top_win) {
    proj = top_win->myProject();
  }
  if (!proj) {
    // not the top window so locate the project (works unless 2 projects with same name)
    if(path.startsWith(".projects")) {
      String prj_name = path.after("[\"");
      prj_name = prj_name.before("\"]");
      proj = taRootBase::instance()->projects.FindName(prj_name);
    }
  }
  
  // for uniformity and simplicity, we look up the canonical windows
  // and corresponding viewers for the tree/panels and panels/t3 frames
  // note that these are the same for 2-pane
  MainWindowViewer* proj_brow = NULL;
  MainWindowViewer* proj_view = NULL;
  if (proj) {
    proj_brow = proj->GetDefaultProjectBrowser();
    proj_view = proj->GetDefaultProjectViewer();
  }
  iMainWindowViewer* iproj_brow = NULL;
  if (proj_brow)
    iproj_brow = proj_brow->widget();
  /*nn  iMainWindowViewer* iproj_view = NULL;
   if (proj_view)
   iproj_view = proj_view->widget(); */
  
  // IMPORTANT NOTE: You *must* check ALL objects for NULL in the following
  // cascades, because there are conditions under which it is possible for
  // something not to have a value
  if(path.startsWith(".T3Tab.")) {
    String tbnm = path.after(".T3Tab.");
    if(!proj_view || !proj_view->SelectT3ViewTabName(tbnm)) {
      taMisc::Warning("ta: URL -- could not activate 3D View Tab named:", tbnm);
    }
  }
  else if(path.startsWith(".PanelTab.")) {
    String tbnm = path.after(".PanelTab.");
    if(!proj_brow || !proj_brow->SelectPanelTabName(tbnm)) {
      taMisc::Warning("ta: URL -- could not activate Panel Tab named:", tbnm);
    }
  }
  else if(path.startsWith(".Wiki.")) {
    String new_path = taMisc::GetWikiURL(taMisc::web_help_wiki, true) + path.after(".Wiki.");
    iHelpBrowser::StatLoadUrl(new_path);
  }
  else {
    String fun_call;
    if(path.endsWith("()")) {   // function call!
      fun_call = path.after(".",-1);
      fun_call = fun_call.before("()");
      path = path.before(".",-1);       // get part before last call
    }
    taBase* tab = NULL;
    MemberDef* md;
    if(path.startsWith(".projects")) {
      tab = tabMisc::root->FindFromPath(path, md);
    }
    else {
      if(path.startsWith("."))
        path = path.after(".");
      if (proj)
        tab = proj->FindFromPath(path, md);
    }
    if (!tab) {
      taMisc::Warning("ta: URL",path,"not found as a path to an object!");
      return;
    }
    if(fun_call.nonempty()) {
      tab->CallFun(fun_call);
    }
    else {
      taiSigLink* link = (taiSigLink*)tab->GetSigLink();
      if (!link) {
        taMisc::Warning("ta: URL",path,"does not have a valid SigLink -- cannot select in browser!");
        return;
      }
      if(!iproj_brow) {
        taMisc::Warning("ta: URL",path,"could not find project browser to select item");
        return;
      }
      Program* prg = (Program*)tab->GetOwner(&TA_Program);
      if(prg) {
        prg->BrowserSelectMe();
        taMisc::RunPending();
        if(url.hasFragment() && frag_str.startsWith("progln_")) {
          // this is for the GlobalTrace listing in Programs that renders line numbers
          // in fragment
          int lno = (int)frag_str.after("progln_");
          prg->ViewCssScript(lno);
        }
        else {
          prg->ViewProgEditor();
          taMisc::RunPending();
          prg->BrowserSelectMe_ProgItem((taOBase*)tab);
        }
      }
      else {
        //  iTreeViewItem* item =
        iproj_brow->AssertBrowserItem(link);
      }
    }
  }
}

void iMainWindowViewer::httpUrlHandler(const QUrl& url) {
  String urlString = url.toString();
  if (urlString.contains("bugzilla")) {
    QDesktopServices::openUrl(url);    // user's default browser
  }
  else {
    // just use the help browser for any misc links
    iHelpBrowser::StatLoadUrl(url.toString());
  }
}

bool iMainWindowViewer::event(QEvent* ev) {
  bool rval = inherited::event(ev);
  if(ev->type() == QEvent::WindowActivate) {
    taiMisc::active_wins.GotFocus_MainWindow(this);
    AlignCssConsole();
    ProjDirToCurrent();
  }
  return rval;
}

bool iMainWindowViewer::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() != QEvent::KeyPress) {
    // the following can be useful for event debugging 
    // #if DEBUG
    // if((event->type() != QEvent::Paint) && (event->type() != QEvent::ChildRemoved)
    //    && (event->type() != QEvent::ChildAdded) && (event->type() != QEvent::UpdateLater)
    //    && (event->type() != QEvent::Show) && (event->type() != QEvent::LayoutRequest)
    //    && (event->type() != QEvent::Timer)&& (event->type() != QEvent::Move)
    //    && (event->type() != QEvent::Resize) && (event->type() != QEvent::InputMethodQuery)
    //    ) {
    //   std::cerr << event->type() << std::endl;
    //   if(event->type() == QEvent::FocusIn) {
    //     std::cerr << "focus!" << std::endl;
    //   }
    // }
    // #endif 
    return inherited::eventFilter(obj, event);
  }
  QKeyEvent* e = static_cast<QKeyEvent *>(event);
  // this ends up being redundant with all the relevant control-specific cases..
  // taiMisc::UpdateUiOnCtrlPressed(this, e);
  
//  if(KeyEventFilterWindowNav(obj, e))
//    return true;
//  if(taiMisc::KeyEventFilterEmacs_Clip(obj, e))
//    return true;
  return inherited::eventFilter(obj, event);
}

int iMainWindowViewer::GetEditActions() {
  int rval = 0;
  emit GetEditActionsEnabled(rval);
  return rval;
}

void iMainWindowViewer::keyPressEvent(QKeyEvent* key_event) {
  taiMisc::UpdateUiOnCtrlPressed(this, key_event);
  
  taiMisc::BoundAction action = taiMisc::GetActionFromKeyEvent(taiMisc::PROJECTWINDOW_CONTEXT, key_event);
  switch (action) {
    case taiMisc::PROJECTWINDOW_FRAME_LEFT: // move left between regions
    case taiMisc::PROJECTWINDOW_FRAME_LEFT_II:
      MoveFocusLeft();
      key_event->accept();
      return;
    case taiMisc::PROJECTWINDOW_FRAME_RIGHT: // move right between regions
    case taiMisc::PROJECTWINDOW_FRAME_RIGHT_II:
      MoveFocusRight();
      key_event->accept();
      return;
      // these need key bindings
    case taiMisc::PROJECTWINDOW_SHIFT_TAB_LEFT: // switch tab
    case taiMisc::PROJECTWINDOW_SHIFT_TAB_LEFT_II:
      ShiftCurTabLeft();
      key_event->accept();
      return;
    case taiMisc::PROJECTWINDOW_SHIFT_TAB_RIGHT: // switch tab
    case taiMisc::PROJECTWINDOW_SHIFT_TAB_RIGHT_II:
      ShiftCurTabRight();
      key_event->accept();
      return;
    default:
      inherited::keyPressEvent(key_event);
  }
}

void iMainWindowViewer::viewSaveView() {
  viewer()->GetWinState();
}

void iMainWindowViewer::editCut() {
  QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(focusWidget());
  if (lineEdit) {
    lineEdit->cut();
    return;
  }
  QWebView*  webViewEdit = dynamic_cast<QWebView*>(focusWidget());
  if (webViewEdit) {
    webViewEdit->page()->triggerAction(QWebPage::Cut);
    return;
  }
  QTextEdit*  textEdit = dynamic_cast<QTextEdit*>(focusWidget());
  if (textEdit) {
    textEdit->cut();
  }
  else {  // else object
    emit_EditAction(iClipData::EA_CUT);
  }
}

void iMainWindowViewer::editCopy() {
  QWidget* widg = focusWidget();
  
  QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(focusWidget());
  if (lineEdit) // if text
  {
    lineEdit->copy();
    return;
  }
  QWebView*  webViewEdit = dynamic_cast<QWebView*>(focusWidget());
  if (webViewEdit) {
    webViewEdit->page()->triggerAction(QWebPage::Copy);
    return;
  }
  QTextEdit*  textEdit = dynamic_cast<QTextEdit*>(focusWidget());
  if (textEdit) {
    textEdit->copy();
    return;
  }
  else {  // else object
    emit_EditAction(iClipData::EA_COPY);
  }
}

void iMainWindowViewer::editPaste() {
  QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(focusWidget());
  if (lineEdit)  // if text
  {
    lineEdit->paste();
    return;
  }
  QWebView*  webViewEdit = dynamic_cast<QWebView*>(focusWidget());
  if (webViewEdit) {
    webViewEdit->page()->triggerAction(QWebPage::Paste);
    return;
  }
  QTextEdit*  textEdit = dynamic_cast<QTextEdit*>(focusWidget());
  if (textEdit) {
    textEdit->paste();
    return;
  }
  else {  // else object
    emit_EditAction(iClipData::EA_PASTE);
  }
}

void iMainWindowViewer::editMenu_aboutToShow() {
  UpdateUi();
}

void iMainWindowViewer::windowMenu_aboutToShow() {
  // Clear and rebuild submenu.
  windowMenu->Reset();
  windowMenu->AddAction(windowMinimizeAction);
  windowMenu->AddAction(windowZoomAction);
  windowMenu->AddSep();
  
  // Populate with current windows.
  iWidget_List wl;
  taiMisc::GetWindowList(wl);
  for (int i = 0; i < wl.size; ++i) {
    QWidget* wid = wl.FastEl(i);
    if (wid->isWindow()) {
      String title = wid->windowTitle();
      String label = title;
      if (wid->isWindowModified()) {
        label = label + "*";
      }
      windowMenu->AddItem(label, iAction::var_act,
                          this, SLOT(windowActivateByName(const Variant&)), title);
    }
  }
}

void iMainWindowViewer::windowActivateByName(const Variant& title_) {
  iWidget_List wl;
  taiMisc::GetWindowList(wl);
  String title = title_.toString();
  QWidget* wid;
  for (int i = 0; i < wl.size; ++i) {
    wid = wl.FastEl(i);
    if (wid->isWindow()) {
      String wid_title = wid->windowTitle();
      if (title == wid_title)
        break;
    }
  }
  if (!wid)
    return;
  if (wid->isMinimized()) {
    wid->showMaximized();
  }
  wid->activateWindow();
  wid->raise();
}

void iMainWindowViewer::WindowMinimize() {
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    if (widget->isActiveWindow()) {
      widget->showMinimized();
      break;
    }
  }
}

void iMainWindowViewer::WindowZoom()  {
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    if (widget->isActiveWindow()) {
      widget->showMaximized();
      break;
    }
  }
}

void iMainWindowViewer::fileClearRecentsMenu() {
  tabMisc::root->ClearRecentFiles();
}

void iMainWindowViewer::filePrint() {
}

void iMainWindowViewer::helpHelp() {
  String url = taMisc::web_help_general;
  if (url.nonempty()) {
    QDesktopServices::openUrl(QUrl(url));
  }
}

void iMainWindowViewer::helpAbout() {
  if (tabMisc::root) tabMisc::root->About();
}

void iMainWindowViewer::FileBugReport() {
  QDesktopServices::openUrl(QUrl("http://grey.colorado.edu/bugzilla"));
}

void iMainWindowViewer::mnuEditAction(iAction* mel) {
  // called from context; cast obj to an iClipData::EditAction
  emit_EditAction(mel->usr_data.toInt());
}

void iMainWindowViewer::SelectableHostNotifying_impl(ISelectableHost* src_host, int op) {
  //TODO: should we do anything else with this ourself????
  emit SelectableHostNotifySignal(src_host, op);
  UpdateUi();
}

void iMainWindowViewer::Refresh_impl() {
  QObject* obj;
  foreach (obj, body->children()) {
    IViewerWidget* dvw = dynamic_cast<IViewerWidget*>(obj); // null if not type
    if (dvw) {
      dvw->Refresh();
    }
  }
}

void iMainWindowViewer::ViewReset() {
  ShowHideFrames(7);
  viewer()->ResetSplitterState();
}

void iMainWindowViewer::ResolveChanges_impl(CancelOp& cancel_op) {
  if (!isProjShower()) return; // changes only applied for proj showers

  taProject* proj = curProject();
  // only closing last browser is important (easier to check that here than below)
  if(!proj) return;             // does happen!
  MainWindowViewer* mwv = viewer();
  if (!mwv) return; // shouldn't happen
  if (!mwv->isProjBrowser()) return;
  // ok, if this isn't last, then bail
  int cnt = 0; // just bail if 2+
  for (int i = 0; i < proj->viewers.size; i++) {
    mwv = dynamic_cast<MainWindowViewer*>(proj->viewers.FastEl(i));
    if (!mwv || !(mwv->isProjBrowser() && mwv->isMapped())) continue;
    if (++cnt >= 2) return;
  }
  // ok, so here we are at the final project viewer, which will close the project!
  if (isDirty() && !(proj && proj->m_no_save)) {
    bool forced = (cancel_op == CO_NOT_CANCELLABLE);
    int chs;
    if (forced)
      chs= taMisc::Choice("The project: " + taMisc::GetFileFmPath(proj->file_name) + " has unsaved changes -- do you want to save before closing this window?",
          "&Save", "&Don't Save");
    else
      chs= taMisc::Choice("The project: " + taMisc::GetFileFmPath(proj->file_name) + " has unsaved changes -- do you want to save before closing this window?",
          "&Save", "&Don't Save", "&Cancel");

    switch (chs) {
    case 0:
      SaveData();
      break;
    case 1:
      if (proj)
        proj->m_no_save = true; // save so we don't prompt again
      break;
    case 2: //only possible if not forced
      cancel_op = CO_CANCEL;
      return;
    }
  }
  // if we make it here,  then we should be closed -- closeEvent will do this
  m_close_proj_now = true;
}

bool iMainWindowViewer::isDirty() const {
  taProject* proj = curProject(); // only if projviwer
  if (proj)
    return proj->isDirty();
  else
    return false;
}


void iMainWindowViewer::SaveData() {
  taProject* proj = curProject(); // only if projviwer
  // only impl for projects, because they are only thing we know how to save
  if (proj)
    proj->Save();
}

void iMainWindowViewer::SelectableHostNotifySlot(ISelectableHost* src_host, int op) {
  switch (op) {
  case ISelectableHost::OP_GOT_FOCUS: {
    // spec says that this automatically also makes the guy the cliphandler
    // if (last_sel_server == src_host) break; // nothing to do
    // there are other paths to SetClipboardHandler that do not set 
    // last_sel_server, so this prevents proper restoration of 
    // clipboardhandler.  Not sure what SelectableHostNotifyingSignal does really,
    // but probably it is good to redo that too?
    QObject* handler_obj = src_host->clipHandlerObj();
    SetClipboardHandler(handler_obj, ISelectableHost::edit_enabled_slot,
        ISelectableHost::edit_action_slot);
    if(last_sel_server != src_host) {
      SelectableHostNotifying_impl(src_host, op); // break out for clarity
    }
    last_sel_server = src_host;
  } break;
  case ISelectableHost::OP_SELECTION_CHANGED: {
    // if no handler, then this guy becomes handler implicitly, otherwise ignore
    if (last_sel_server && (last_sel_server != src_host)) break;
    SelectableHostNotifying_impl(src_host, op); // break out for clarity
  } break;
  case ISelectableHost::OP_DESTROYING: {
    if (last_sel_server == src_host) {
      if (last_clip_handler == src_host->clipHandlerObj()) {
        SetClipboardHandler(NULL); // might as well do this now
      }
      last_sel_server = src_host;
      UpdateUi();
    }
  } break;
  default: break; // shouldn't happen
  }
}

void iMainWindowViewer::SetClipboardHandler(QObject* handler_obj,
    const char* edit_enabled_slot,
    const char* edit_action_slot,
    const char* actions_enabled_slot,
    const char* update_ui_signal)
{
  if (last_clip_handler == handler_obj)
    return; // nothing to do
  // always disconnect first
  if (last_clip_handler) {
    disconnect(this, SIGNAL(GetEditActionsEnabled(int&)), last_clip_handler, NULL);
    disconnect(this, SIGNAL(EditAction(int)), last_clip_handler, NULL);
    disconnect(this, SIGNAL(SetActionsEnabled()), last_clip_handler, NULL);
    disconnect(last_clip_handler, NULL, this, SLOT(UpdateUi()));
    disconnect(last_clip_handler, SIGNAL(destroyed()), this, SLOT(ch_destroyed()));
  }
  // now connect, if supplied
  if (handler_obj) {
    connect(handler_obj, SIGNAL(destroyed()), this, SLOT(ch_destroyed()));
    connect(this, SIGNAL(GetEditActionsEnabled(int&)), handler_obj, edit_enabled_slot);
    connect(this, SIGNAL(EditAction(int)), handler_obj, edit_action_slot );
    if (actions_enabled_slot)
      connect(this, SIGNAL(SetActionsEnabled()), handler_obj, actions_enabled_slot );
    if (update_ui_signal)
      connect(handler_obj, update_ui_signal, this, SLOT(UpdateUi()) );
    // taMisc::Info("SetClipHandler to: type, name",
    //              handler_obj->metaObject()->className(),
    //              handler_obj->objectName().toLatin1());
  }
  last_clip_handler = handler_obj; // whether NULL or not
  UpdateUi();
}

void iMainWindowViewer::setFrameGeometry(const iRect& r) {
  //NOTE: this may only work before calling show() on X
  this->resize(r.size());   // set size
  this->move(r.topLeft());  // set position
}

void  iMainWindowViewer::setFrameGeometry(int left, int top, int width, int height) {
  setFrameGeometry(iRect(left, top, width, height));
}

iMainWindowViewer* iMainWindowViewer::GetViewerForObj(taBase* obj) {
  if(!taMisc::gui_active) return NULL;
  taProject* proj = GET_OWNER(obj, taProject);
  if(!proj) return NULL;

  // iterate to find all Browsers
  for (int i = 0; i < proj->viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(proj->viewers.FastEl(i));
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
    return imwv;
  }
  return NULL;
}

void iMainWindowViewer::ShowChange(iAction* sender) {
  int show = taMisc::show_gui;
  int new_show;
  if (sender->usr_data == 0)
    new_show = TypeItem::NORM_MEMBS;
  else if (sender->usr_data == 1)
    new_show = TypeItem::ALL_MEMBS;
  else {
    int mask;
    switch (sender->usr_data.toInt()) {
    case 2: mask = TypeItem::NO_NORMAL; break;
    case 3: mask = TypeItem::NO_EXPERT; break;
    case 4: mask = TypeItem::NO_HIDDEN; break;
    default: mask = 0; break; // should never happen
    }
    new_show = sender->isChecked() ? show & ~mask : show | mask;
  }
  if (new_show != taMisc::show_gui) {
    taMisc::show_gui = (TypeItem::ShowMembs)new_show;
    viewRefresh();
  }
}

void iMainWindowViewer::showMenu_aboutToShow() {
  int value = taMisc::show_gui;
  if (!show_menu) return;
  //note: nothing to do for the command items
  (*show_menu)[2]->setChecked(!(value & TypeItem::NO_NORMAL));
  (*show_menu)[3]->setChecked(!(value & TypeItem::NO_EXPERT));
  (*show_menu)[4]->setChecked(!(value & TypeItem::NO_HIDDEN));
}

void iMainWindowViewer::toolsMenu_aboutToShow() {
  toolsDiffProjectsAction->setEnabled(tabMisc::root->projects.size > 1);
  toolsDiffProgramsAction->setEnabled(tabMisc::root->projects.size > 0);
  toolsDiffDataTablesAction->setEnabled(tabMisc::root->projects.size > 0);
  toolsDiffNetworksAction->setEnabled(tabMisc::root->projects.size > 0);
  toolsDiffLayersAction->setEnabled(tabMisc::root->projects.size > 0);
  toolsDiffSpecsAction->setEnabled(tabMisc::root->projects.size > 0);
  toolsOpenServerAction->setEnabled(!taRootBase::instance()->IsServerOpen());
  toolsCloseServerAction->setEnabled(taRootBase::instance()->IsServerOpen());
}

void iMainWindowViewer::dataMenu_aboutToShow() {
  for (int i=0; i<dataProcCopyActions.size; i++) {
    dataProcCopyActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataProcOrderActions.size; i++) {
    dataProcOrderActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataProcSelectActions.size; i++) {
    dataProcSelectActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataProcColumnsActions.size; i++) {
    dataProcColumnsActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataAnalStatsActions.size; i++) {
    dataAnalStatsActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataAnalDistanceActions.size; i++) {
    dataAnalDistanceActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataAnalHighDimActions.size; i++) {
    dataAnalHighDimActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataAnalCleanActions.size; i++) {
    dataAnalCleanActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataAnalGraphActions.size; i++) {
    dataAnalGraphActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataGenBasicActions.size; i++) {
    dataGenBasicActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataGenListsActions.size; i++) {
    dataGenListsActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataGenDrawActions.size; i++) {
    dataGenDrawActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataGenRandomActions.size; i++) {
    dataGenRandomActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataGenFeatPatsActions.size; i++) {
    dataGenFeatPatsActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<dataGenFilesActions.size; i++) {
    dataGenFilesActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<imageProcTransformActions.size; i++) {
    imageProcTransformActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<imageProcFilterActions.size; i++) {
    imageProcFilterActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<imageProcNoiseActions.size; i++) {
    imageProcNoiseActions[i]->setEnabled(myProject());
  }
  for (int i=0; i<imageProcImageProcActions.size; i++) {
    imageProcImageProcActions[i]->setEnabled(myProject());
  }
}

void iMainWindowViewer::this_DockSelect(iAction* me) {
  iDockViewer* itb = (iDockViewer*)(me->usr_data.toPtr());
  DockViewer* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (me->isChecked()) { //note: check has already been toggled
    tb->Show();
  } else { //need to show
    tb->Hide();
  }
}

void iMainWindowViewer::this_FrameSelect(iAction* me) {
  iFrameViewer* itb = (iFrameViewer*)(me->usr_data.toPtr());
  FrameViewer* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (me->isChecked()) { //note: check has already been toggled
    tb->Show();
  } else { //need to show
    tb->Hide();
  }
}

void iMainWindowViewer::this_ToolBarSelect(iAction* me) {
  iToolBar* itb = (iToolBar*)(me->usr_data.toPtr());
  ToolBar* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (me->isChecked()) { //note: check has already been toggled
    tb->Show();
  } else { //need to show
    tb->Hide();
  }
}

void iMainWindowViewer::toolsDiffProjects() {
  taRootBase::instance()->ChooseForDiffCompare("taProject", NULL, "Project");
}

void iMainWindowViewer::toolsDiffPrograms() {
  taProject* prj = curProject();
  taRootBase::instance()->ChooseForDiffCompare("Program", prj);
}

void iMainWindowViewer::toolsDiffDataTables() {
  taProject* prj = curProject();
  taRootBase::instance()->ChooseForDiffCompare("DataTable", prj);
}

void iMainWindowViewer::toolsDiffNetworks() {
  taProject* prj = curProject();
  taRootBase::instance()->ChooseForDiffCompare("Network", prj);
}

void iMainWindowViewer::toolsDiffLayers() {
  taProject* prj = curProject();
  taRootBase::instance()->ChooseForDiffCompare("Layer", prj);
}

void iMainWindowViewer::toolsDiffSpecs() {
  taProject* prj = curProject();
  taRootBase::instance()->ChooseForDiffCompare("BaseSpec", prj, "Spec");
}

void iMainWindowViewer::toolsTypeInfoBrowser() {
  MainWindowViewer* brows = MainWindowViewer::NewTypeInfoBrowser(&taMisc::types, &TA_TypeSpace);
  if (brows == NULL) return;
  brows->ViewWindow();
}

void iMainWindowViewer::toolsHelpBrowser() {
  // iHelpBrowser* tdd =
  iHelpBrowser::instance();
}

void iMainWindowViewer::toolsSvnBrowserEmergent() {
  iSubversionBrowser::OpenBrowser("http://grey.colorado.edu/svn/emergent/emergent/trunk",
      "~/emergent");
}

void iMainWindowViewer::toolsSvnBrowserSvn1() {
  if(taMisc::svn_repos.size < 1) return;
  String url = taMisc::svn_repos[0].value.toString();
  String wc = taMisc::svn_wc_dirs[0].value.toString();
  iSubversionBrowser::OpenBrowser(url,wc);  
}

void iMainWindowViewer::toolsSvnBrowserSvn2() {
  if(taMisc::svn_repos.size < 2) return;
  String url = taMisc::svn_repos[1].value.toString();
  String wc = taMisc::svn_wc_dirs[1].value.toString();
  iSubversionBrowser::OpenBrowser(url,wc);
}

void iMainWindowViewer::toolsSvnBrowserSvn3() {
  if(taMisc::svn_repos.size < 3) return;
  String url = taMisc::svn_repos[2].value.toString();
  String wc = taMisc::svn_wc_dirs[2].value.toString();
  iSubversionBrowser::OpenBrowser(url,wc);  
}

void iMainWindowViewer::this_SaveView(iAction* me) {
  if (me->isChecked()) {
    curProject()->SetSaveView(true);
  } else { //need to show
    curProject()->SetSaveView(false);
  }
}

void iMainWindowViewer::UpdateUi() {
#if defined(TA_OS_MAC) && (QT_VERSION >= 0x050200)
  // this is the only place it seems to work..
  TurnOffTouchEventsForWindow(windowHandle());
#endif
  int ea = GetEditActions();

  // taMisc::DebugInfo("in uui");

  // these aren't for text
  editDupeAction->setEnabled(false);
  editDeleteAction->setEnabled(false);
  // these are not always visible
  editPasteIntoAction->setVisible(false);
  editPasteAssignAction->setVisible(false);
  editPasteAppendAction->setVisible(false);

  QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(focusWidget());
  QTextEdit*  textEdit = dynamic_cast<QTextEdit*>(focusWidget());
  QWebView*  webViewEdit = dynamic_cast<QWebView*>(focusWidget());

  if (lineEdit != NULL) {
    editCutAction->setEnabled(lineEdit->hasSelectedText());
    editCopyAction->setEnabled(lineEdit->hasSelectedText());
    
    QClipboard *clipboard = QApplication::clipboard();
    QString clip_text = clipboard->text();
    editPasteAction->setEnabled(!clip_text.isEmpty());
  }
  else if (textEdit != NULL) {
    editCutAction->setEnabled(textEdit->textCursor().hasSelection());
    editCopyAction->setEnabled(textEdit->textCursor().hasSelection());
    
    QClipboard *clipboard = QApplication::clipboard();
    QString clip_text = clipboard->text();
    editPasteAction->setEnabled(!clip_text.isEmpty());
  }
  else if (webViewEdit != NULL) {
    editCutAction->setEnabled(webViewEdit->selectedText().size() != 0);
    editCopyAction->setEnabled(webViewEdit->selectedText().size() != 0);
    
    QClipboard *clipboard = QApplication::clipboard();
    QString clip_text = clipboard->text();
    editPasteAction->setEnabled(!clip_text.isEmpty());
  }
  else {  // focus is not in text field
    editCopyAction->setEnabled(ea & iClipData::EA_COPY);
    editCutAction->setEnabled(ea & iClipData::EA_CUT);
    editDupeAction->setEnabled(ea & iClipData::EA_DUPE);
    editDeleteAction->setEnabled(ea & iClipData::EA_DELETE);
    
    int paste_cnt = 0;
    if (ea & iClipData::EA_PASTE) ++paste_cnt;
    if (ea & iClipData::EA_PASTE_INTO) ++paste_cnt;
    if (ea & iClipData::EA_PASTE_ASSIGN) ++paste_cnt;
    if (ea & iClipData::EA_PASTE_APPEND) ++paste_cnt;
    
    editPasteAction->setEnabled(ea & iClipData::EA_PASTE);
    editPasteIntoAction->setVisible(ea & iClipData::EA_PASTE_INTO);
    editPasteAssignAction->setVisible(ea & iClipData::EA_PASTE_ASSIGN);
    editPasteAppendAction->setVisible(ea & iClipData::EA_PASTE_APPEND);
  }

  // linking is currently not really used, so we'll not show by default
  // if we later add more linking capability, we may want to always enable,
  // just to hint user that it is sometimes available
  editLinkAction->setVisible(ea & iClipData::EA_LINK);
  editLinkIntoAction->setVisible(ea & iClipData::EA_LINK_INTO);

  if ((ea & iClipData::EA_LINK2) == iClipData::EA_LINK2) {
    // need to remove accelerators
    editLinkAction->setText("Link");
    editLinkAction->setShortcut(QKeySequence());
    editLinkIntoAction->setText("Link Into");
    editLinkIntoAction->setShortcut(QKeySequence());
  } else {
    // restore accelerators
    editLinkAction->setText("&Link");
    editLinkAction->setShortcut(QKeySequence());
    editLinkIntoAction->setText("&Link Into");
    editLinkIntoAction->setShortcut(QKeySequence());
  }

  editUnlinkAction->setVisible(ea & iClipData::EA_UNLINK);

  taProject* proj = myProject();
  if(proj) {
    editUndoAction->setEnabled(proj->undo_mgr.UndosAvail() > 0);
    editRedoAction->setEnabled(proj->undo_mgr.RedosAvail() > 0);
  }
  else {
    editUndoAction->setEnabled(false);
    editRedoAction->setEnabled(false);
  }

  bool css_running = (Program::global_run_state == Program::RUN);
  ctrlStopAction->setEnabled(css_running);
  ctrlContAction->setEnabled(!css_running && Program::last_run_prog);
  ctrlStepAction->setEnabled(!css_running && Program::last_step_prog);

  progStatusAction->setText(Program::GlobalStatus());

  if (tabMisc::root) {
    fileSaveAllAction->setEnabled(!tabMisc::root->projects.IsEmpty());
  }

  viewSetSaveViewAction->setEnabled(curProject() != NULL);
  viewBrowseOnlyAction->setEnabled(curProject() != NULL);
  viewPanelsOnlyAction->setEnabled(curProject() != NULL);
  viewBrowseAndPanelsAction->setEnabled(curProject() != NULL);
  viewT3OnlyAction->setEnabled(curProject() != NULL);
  viewBrowseAndT3Action->setEnabled(curProject() != NULL);
  viewPanelsAndT3Action->setEnabled(curProject() != NULL);
  viewAllFramesAction->setEnabled(curProject() != NULL);
  viewResetViewAction->setEnabled(curProject() != NULL);

  if (curProject()) {
    fileOpenSvnBrowserAction->setEnabled(!curProject()->GetDir().empty());
    fileSaveAction->setEnabled(!curProject()->save_as_only);
    viewSetSaveViewAction->setChecked(curProject()->save_view);  // keep menu insync in case someone else set the property
  }
  emit SetActionsEnabled();
}


void iMainWindowViewer::changeEvent(QEvent* ev) {
  if(ev->type() == QEvent::ActivationChange) {
    if (isActiveWindow()) {
      UpdateUi();
      int idx = taiMisc::active_wins.FindEl(this);
      if (idx < 0) {
        taMisc::Error("iMainWindowViewer::windowActivationChange", "Unexpectedly not in taiMisc::viewer_wins");
      } else {
        if (idx < (taiMisc::active_wins.size - 1)) {
          // move us to the end
          taiMisc::active_wins.MoveIdx(idx, taiMisc::active_wins.size - 1);
        }
      }
    }
  }
  inherited::changeEvent(ev);
}

void iMainWindowViewer::ShowHideFrames(int combo) {
  PanelViewer* pv_browse = (PanelViewer*)viewer()->GetLeftBrowser();
  PanelViewer* pv_panels = (PanelViewer*)viewer()->GetMiddlePanel();
  PanelViewer* pv_T3 = (PanelViewer*)viewer()->GetRightViewer();

  DockViewer* dv = viewer()->FindDockViewerByName("Tools");
  if (dv) {
    if ((pv_browse && pv_browse->isVisible()) || (pv_panels && pv_panels->isVisible()))
      tools_dock_was_visible = dv->isVisible();  // save the state for case when dock is hidden because the browse and panel frames are hidden
  }

  bool show_pv_browse = false;
  bool show_pv_panels = false;
  bool show_pv_T3 = false;

  switch (combo) {
  case 1:
    show_pv_browse = true;
    break;
  case 2:
    show_pv_panels = true;
    break;
  case 3:
    show_pv_browse = true;
    show_pv_panels = true;
    break;
  case 4:
    show_pv_T3 = true;
    if (dv)
      dv->Hide();
    break;
  case 5:
    show_pv_browse = true;
    show_pv_T3 = true;
    break;
  case 6:
    show_pv_panels = true;
    show_pv_T3 = true;
    break;
  case 7:
    show_pv_browse = true;
    show_pv_panels = true;
    show_pv_T3 = true;
    break;
  }

  if (pv_browse) {
    if (show_pv_browse) {
      pv_browse->Show();
      if (dv && tools_dock_was_visible)  // make sure dock is visible if was previously visible
        dv->Show();
    }
    else
      pv_browse->Hide();
  }
  if (pv_panels) {
    if (show_pv_panels) {
      pv_panels->Show();
      if (dv && tools_dock_was_visible)  // make sure dock is visible if was previously visible
        dv->Show();
    }
    else
      pv_panels->Hide();
  }
  if (pv_T3) {
    if (show_pv_T3)
      pv_T3->Show();
    else
      pv_T3->Hide();
  }
}

void iMainWindowViewer::toolsOpenRemoteServer() {
  taRootBase::instance()->CallFun("OpenRemoteServer");  // call fun so we get the port dialog
}

void iMainWindowViewer::toolsCloseRemoteServer() {
  taRootBase::instance()->CloseRemoteServer();
}

//////////////////////////////////
//   taBase                     //
//////////////////////////////////

bool taBase::EditPanel(bool new_tab, bool pin_tab) {
  if(!taMisc::gui_active) return false;

  taiSigLink* link = (taiSigLink*)GetSigLink();
  if (!link) return false;	// shouldn't happen

  iMainWindowViewer* inst = NULL;

  taProject* proj = GET_MY_OWNER(taProject);
  if(proj) {
    MainWindowViewer* vwr = proj->GetDefaultProjectBrowser(); // this is better
    if(!vwr) return false;	// shouldn't happen
    inst = vwr->widget();
  }
  else {
    inst = taiMisc::active_wins.Peek_MainWindow(); // works for root window..
  }

  if (!inst) return false; // shouldn't happen!

  bool rval = false;
  // for new_tab, we open new locked panels,
  // for existing, we do a browse to the item
  if (new_tab) {
    rval = inst->AssertPanel(link, new_tab, pin_tab);
  }
  else {
    rval = inst->AssertBrowserItem(link);
  }

  return rval;
}

bool taBase::BrowserSelectMe() {
  if(!taMisc::gui_active) return false;

  // first, check for an edit dialog and use that if found
  MainWindowViewer* edlg = MainWindowViewer::FindEditDialog(this);
  if(edlg) {
    edlg->Show();               // focus on it
    return true;
  }

  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;
  taiSigLink* link = (taiSigLink*)GetSigLink();
  if (!link) return false;

  bool rval = false;
  // iterate to find all Browsers
  for (int i = 0; i < proj->viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(proj->viewers.FastEl(i));
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;

    rval = rval || (bool)imwv->AssertBrowserItem(link);
  }
  return rval;
}

bool taBase::BrowserExpandAll() {
  if(!taMisc::gui_active) return false;
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;
  taiSigLink* link = (taiSigLink*)GetSigLink();
  if (!link) return false;

  bool rval = false;
  // iterate to find all Browsers
  for (int i = 0; i < proj->viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(proj->viewers.FastEl(i));
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;

    rval = rval || (bool)imwv->BrowserExpandAllItem(link);
  }
  return rval;
}

bool taBase::BrowserCollapseAll() {
  if(!taMisc::gui_active) return false;
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;
  taiSigLink* link = (taiSigLink*)GetSigLink();
  if (!link) return false;

  bool rval = false;
  // iterate to find all Browsers
  for (int i = 0; i < proj->viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(proj->viewers.FastEl(i));
    //if (vwr && (vwr->GetName() == "DefaultProjectBrowser")) return vwr;
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;

    rval = rval || (bool)imwv->BrowserCollapseAllItem(link);
  }
  return rval;
}

bool taBase::GuiFindFromMe(const String& find_str) {
  if(!taMisc::gui_active) return false;
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;
  taiSigLink* link = (taiSigLink*)GetSigLink();
  if (!link) return false;

  bool rval = false;
  // iterate to find all Browsers
  for (int i = 0; i < proj->viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(proj->viewers.FastEl(i));
    //if (vwr && (vwr->GetName() == "DefaultProjectBrowser")) return vwr;
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;

    rval= true;
    imwv->Find(link, find_str);
  }
  return rval;
}
