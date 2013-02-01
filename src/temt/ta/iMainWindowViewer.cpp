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

#include <taiMenu>
#include <taiAction>
#include <iBrowseHistory>
#include <iToolBar>
#include <iFrameViewer>
#include <iDockViewer>
#include <iSearchDialog>
#include <iTreeView>
#include <ISelectable_PtrList>
#include <iTreeViewItem>
#include <iTabViewer>
#include <ISelectableHost>
#include <taiMenuBar>
#include <taiClipData>
#include <DockViewer>
#include <taGuiDialog>
#include <BrowseViewer>
#include <iBrowseViewer>
#include <iDataPanel>
#include <taFiler>
#include <PanelViewer>
#include <ToolBar>
#include <taProject>
#include <iSplitter>
#include <iTabView>
#include <T3DataViewer>
#include <iHelpBrowser>
#include <iTextBrowser>
#include <Widget_List>
#include <iRect>

#include <taMisc>
#include <taiMisc>
#include <tabMisc>
#include <taRootBase>

#include <QFileInfo>
#include <QUrl>
#include <QDesktopServices>
#include <QKeyEvent>



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
  , IDataViewWidget(viewer_)
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

void iMainWindowViewer::Init() {
  setAttribute(Qt::WA_DeleteOnClose);
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
  fileUpdateChangeLogAction = NULL;
  fileSaveAllAction = NULL;
  fileOpenFromWebMenu = NULL;
  filePublishDocsOnWebMenu = NULL;
  filePublishProjectOnWebAction = NULL;
  fileCloseAction = NULL;
  fileCloseWindowAction = NULL;
  fileQuitAction = NULL;
}

taiAction* iMainWindowViewer::AddAction(taiAction* act) {
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
  taiAction* act = dockMenu->AddItem(dv->viewer()->GetName(), taiMenu::toggle,
    taiAction::men_act, this, SLOT(this_DockSelect(taiAction*)), (void*)dv);
  if (dv->isVisible() != act->isChecked())
    act->setChecked(dv->isVisible()); // note: triggers action
  //TODO: maybe need to hook up signals for undocking
}

void iMainWindowViewer::AddFrameViewer(iFrameViewer* fv, int at_index) {
  if (at_index < 0) {
    body->addWidget(fv);
    at_index = body->count() - 1;
  } else
    body->insertWidget(at_index, fv);
  fv->m_window = this;
//TODO: this stretch thing isn't working -- replace with sizing
  body->setStretchFactor(at_index, fv->stretchFactor());

  connect(this, SIGNAL(SelectableHostNotifySignal(ISelectableHost*, int)),
    fv, SLOT(SelectableHostNotifySlot_External(ISelectableHost*, int)) );
  connect(fv, SIGNAL(SelectableHostNotifySignal(ISelectableHost*, int)),
    this, SLOT(SelectableHostNotifySlot(ISelectableHost*, int)) );

  taiAction* act = frameMenu->AddItem(fv->viewer()->GetName(), taiMenu::toggle,
    taiAction::men_act, this, SLOT(this_FrameSelect(taiAction*)), (void*)fv);

  //TODO: the show decision should probably be elsewhere
  if (fv->viewer()->isVisible())
    fv->viewer()->Show(); // always needed when adding guys to visible

  if (fv->viewer()->isVisible() != act->isChecked())
    act->setChecked(fv->isVisible()); // note: triggers action

}

// this guy exists because we must always be able to add a panel,
// so if there isn't already a panel viewer, we have to add one
void iMainWindowViewer::AddPanel(iDataPanel* panel, bool new_tab) {
  iTabViewer* itv = GetTabViewer(true);
  if (new_tab) {
    itv->AddPanelNewTab(panel);
  } else {
    itv->AddPanel(panel); // typically for ctrl panels
  }
}

iTabViewer* iMainWindowViewer::GetTabViewer(bool force) {
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
  iTabViewer* itv = GetTabViewer(true);
  itv->ShowLink(link, not_in_cur);
}

void iMainWindowViewer::AddToolBar(iToolBar* itb) {
  if (!itb->parent())
    itb->setParent(this); // needs parent otherwise will leak
  itb->m_window = this;
  addToolBar(itb); //TODO: should specify area
  // create a menu entry to show/hide it, regardless if visible now
  toolBarMenu->AddItem(itb->objectName(), taiMenu::toggle,
    taiAction::men_act, this, SLOT(this_ToolBarSelect(taiAction*)), (void*)itb);
  // if initially invisible, hide it
  ToolBar* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (!tb->visible) {
    tb->Hide();
  }
//nn  toolbars.append(tb);
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
  taProject* prj = curProject();
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
  if(!(taMisc::console_options & taMisc::CO_GUI_TRACKING)) return false;
  QRect r = frameGeometry();
  int nw_top = r.bottom() + 1;
  int nw_ht = taiM->scrn_s.h - nw_top - 64; // leave a fixed amount of space at bottom.
  if(nw_ht < 40) nw_ht = 40;
  taMisc::console_win->resize(r.width(), nw_ht);
  taMisc::console_win->move(r.left(), nw_top);
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
  setFocus();
#endif
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

  if (!isRoot()) {
    fileMenu->AddAction(fileCloseWindowAction);
  }

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
  // create a taiMenu wrapper around the window's provided menubar
  menu = new taiMenuBar(this, taiMisc::fonBig, menuBar());

  fileMenu = menu->AddSubMenu("F&ile");
  editMenu = menu->AddSubMenu("&Edit");
  viewMenu = menu->AddSubMenu("&View");
  show_menu = menu->AddSubMenu("&Show");
  ctrlMenu = menu->AddSubMenu("&Control");
  connect(show_menu->menu(), SIGNAL(aboutToShow()), this, SLOT(showMenu_aboutToShow()));
  // if (!(taMisc::show_gui & TypeItem::NO_EXPERT))
  toolsMenu = menu->AddSubMenu("&Tools");
  windowMenu = menu->AddSubMenu("&Window");
  connect(windowMenu->menu(), SIGNAL(aboutToShow()),
    this, SLOT(windowMenu_aboutToShow()));

  helpMenu = menu->AddSubMenu("&Help");;
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
  Constr_ToolsMenu();
  Constr_HelpMenu();
}

void iMainWindowViewer::Constr_FileMenu()
{
  fileNewAction = AddAction(new taiAction("&New Project...", QKeySequence(), "fileNewAction"));
  fileNewAction->setIcon(QIcon(":/images/filenew.png"));

  fileOpenAction = AddAction(new taiAction("&Open Project...", QKeySequence(), "fileOpenAction"));
  fileOpenAction->setIcon(QIcon(QPixmap(":/images/fileopen.png")));

  fileSaveAction = AddAction(new taiAction("&Save Project", QKeySequence(cmd_str + "S"), "fileSaveAction"));
  fileSaveAction->setIcon(QIcon(QPixmap(":/images/filesave.png")));

  fileSaveAsAction = AddAction(new taiAction("Save Project &As...", QKeySequence(), "fileSaveAsAction"));
  // filePrintAction = AddAction(new taiAction("&Print...", QKeySequence(), "filePrintAction"));
  // filePrintAction->setIcon(QIcon(QPixmap(":/images/fileprint.png")));

  fileSaveNotesAction = AddAction(new taiAction("Save Note &Changes", QKeySequence(), "fileSaveNotesAction"));
  fileSaveAsTemplateAction = AddAction(new taiAction("Save As &Template", QKeySequence(), "fileSaveAsTemplate"));
  fileUpdateChangeLogAction = AddAction(new taiAction("&Updt Change Log", QKeySequence(), "fileUpdateChangeLogAction"));
  fileSaveAllAction = AddAction(new taiAction("Save A&ll Projects", QKeySequence(), "fileSaveAllAction"));

  // fileOpenFromWebMenu and filePublishDocsOnWebMenu created below as submenus.
  filePublishProjectOnWebAction = AddAction(new taiAction("Publish &Project on Web", QKeySequence(), "filePublishProjectOnWebAction"));

  fileCloseAction = AddAction(new taiAction("Close Project", QKeySequence(), "fileCloseAction"));
  fileOptionsAction = AddAction(new taiAction("&Options...", QKeySequence(), "fileOptionsAction"));

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
  fileOpenFromWebMenu = fileMenu->AddSubMenu("Open Project from &Web");
  if (!isRoot()) {
    filePublishDocsOnWebMenu = fileMenu->AddSubMenu("Publish Project &Docs on Web");
    fileMenu->AddAction(filePublishProjectOnWebAction);
  }

  fileMenu->insertSeparator();
  fileMenu->AddAction(fileCloseAction);
  fileMenu->AddAction(fileOptionsAction);

  // Make connections.
  if (!isRoot()) {
    fileCloseWindowAction = AddAction(new taiAction("C&lose Window", QKeySequence(), "fileCloseWindowAction"));
    connect(fileCloseWindowAction, SIGNAL(Action()), this, SLOT(fileCloseWindow()));
  }

  connect(fileNewAction, SIGNAL(Action()), this, SLOT(fileNew()));
  connect(fileOpenAction, SIGNAL(Action()), this, SLOT(fileOpen()));
  connect(fileOpenRecentMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(fileOpenRecent_aboutToShow()));
  connect(fileSaveAllAction, SIGNAL(Action()), this, SLOT(fileSaveAll()));

  // Connect "open from web" for all windows (even root).
  connect(fileOpenFromWebMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(fileOpenFromWeb_aboutToShow()));

  // TBD: is this the same as checking if (!isRoot()) ??
  // Logic for isProjShower() is unclear.
  if (isProjShower()) {
    connect(fileSaveAction, SIGNAL(Action()), this, SLOT(fileSave()));
    connect(fileSaveAsAction, SIGNAL(Action()), this, SLOT(fileSaveAs()));
    connect(fileSaveNotesAction, SIGNAL(Action()), this, SLOT(fileSaveNotes()));
    connect(fileSaveAsTemplateAction, SIGNAL(Action()), this, SLOT(fileSaveAsTemplate()));
    connect(fileUpdateChangeLogAction, SIGNAL(Action()), this, SLOT(fileUpdateChangeLog()));

    // Connect "publish" options only for project windows.
    if (filePublishDocsOnWebMenu) {
      connect(filePublishDocsOnWebMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(filePublishDocsOnWeb_aboutToShow()));
    }
    connect(filePublishProjectOnWebAction, SIGNAL(Action()), this, SLOT(filePublishProjectOnWeb()));

    connect(fileCloseAction, SIGNAL(Action()), this, SLOT(fileClose()));
    // disable the CloseWindow to help emphasize that Closing will close proj
    //no, not needed    fileCloseWindowAction->setEnabled(false);
  }
  else {
    fileSaveAction->setEnabled(false);
    fileSaveAsAction->setEnabled(false);
    fileSaveNotesAction->setEnabled(false);
    fileSaveAsTemplateAction->setEnabled(false);
    fileUpdateChangeLogAction->setEnabled(false);
    fileCloseAction->setEnabled(false);
  }

  // Disable "Publish Project" -- won't be enabled until docs are first published.
  filePublishProjectOnWebAction->setEnabled(false);

  // connect(filePrintAction, SIGNAL(activated()), this, SLOT(filePrint()));
  connect(fileOptionsAction, SIGNAL(Action()), this, SLOT(fileOptions()));

#ifndef TA_OS_MAC
  if (isRoot())
#endif
  {
    fileQuitAction = AddAction(new taiAction("&Quit", QKeySequence(cmd_str + "Q"),
      "fileQuitAction"));
    connect(fileQuitAction, SIGNAL(Action()), this, SLOT(fileQuit()));
  }
}

void iMainWindowViewer::Constr_EditMenu()
{
  editUndoAction = AddAction(new taiAction("&Undo", QKeySequence(cmd_str + "Z"), "editUndoAction"));
  editUndoAction->setIcon(QIcon(QPixmap(":/images/editundo.png")));
  editRedoAction = AddAction(new taiAction("&Redo", QKeySequence(cmd_str + "Shift+Z"), "editRedoAction"));
  editRedoAction->setIcon(QIcon(QPixmap(":/images/editredo.png")));

  editCutAction = AddAction(new taiAction(taiClipData::EA_CUT, "Cu&t", QKeySequence(cmd_str + "X"), "editCutAction"));
  editCutAction->setIcon(QIcon(QPixmap(":/images/editcut.png")));
  editCopyAction = AddAction(new taiAction(taiClipData::EA_COPY, "&Copy", QKeySequence(cmd_str + "C"), "editCopyAction"));
  editCopyAction->setIcon(QIcon(QPixmap(":/images/editcopy.png")));

  // Note: we twiddle the visibility, shortcuts, and accelerator for the Paste and Link guys
  editDupeAction = AddAction(new taiAction(taiClipData::EA_DUPE, "Duplicate  (Ctrl+M)", QKeySequence(), "editDuplicateAction"));
  QPixmap editpaste(":/images/editpaste.png");
  editPasteAction = AddAction(new taiAction(taiClipData::EA_PASTE, "&Paste", QKeySequence(cmd_str + "V"), "editPasteAction"));
  editPasteAction->setIcon(QIcon(editpaste));
  editPasteIntoAction = AddAction(new taiAction(taiClipData::EA_PASTE_INTO, "&Paste Into", QKeySequence(cmd_str + "V"), "editPasteIntoAction"));
  editPasteIntoAction->setIcon(QIcon(editpaste));
  editPasteAssignAction = AddAction(new taiAction(taiClipData::EA_PASTE_ASSIGN, "&Paste Assign", QKeySequence(cmd_str + "V"), "editPasteAssignAction"));
  editPasteAssignAction->setIcon(QIcon(editpaste));
  editPasteAppendAction = AddAction(new taiAction(taiClipData::EA_PASTE_APPEND, "&Paste Append", QKeySequence(cmd_str + "V"), "editPasteAppendAction"));
  editPasteAppendAction->setIcon(QIcon(editpaste));
  editDeleteAction = AddAction(new taiAction(taiClipData::EA_DELETE, "&Delete", QKeySequence("Ctrl+D"), "editDeleteAction"));
  // editDeleteAction->setIcon(QIcon(editpaste));

  editLinkAction = AddAction(new taiAction(taiClipData::EA_LINK, "&Link", QKeySequence(), "editLinkAction"));
  editLinkIntoAction = AddAction(new taiAction(taiClipData::EA_LINK, "&Link Into", QKeySequence(), "editLinkIntoAction"));
  editUnlinkAction = AddAction(new taiAction(taiClipData::EA_LINK, "Unlin&k", QKeySequence(), "editUnlinkAction"));

  editFindAction = AddAction(new taiAction(0, "&Find...", QKeySequence(), "editFindAction"));
  editFindNextAction = AddAction(new taiAction(0, "Find &Next", QKeySequence("F3"), "editFindNextAction"));

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
  connect(editCutAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editCopyAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editDupeAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editPasteAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editPasteIntoAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editPasteAssignAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editPasteAppendAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editLinkAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editLinkIntoAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editUnlinkAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editDeleteAction, SIGNAL(IntParamAction(int)), this, SIGNAL(EditAction(int)));
  connect(editFindAction, SIGNAL(Action()), this, SLOT(editFind()));
  connect(editFindNextAction, SIGNAL(Action()), this, SLOT(editFindNext()));
}

void iMainWindowViewer::Constr_ViewMenu()
{
  viewRefreshAction = AddAction(new taiAction("&Refresh", QKeySequence("F5"), "viewRefreshAction"));
  viewSaveViewAction = AddAction(new taiAction("&Save View", QKeySequence(), "viewSaveViewAction"));

  // Forward and back buttons -- note: on Win the icons don't show up if Action has text
  historyBackAction = AddAction(new taiAction("Back",
    QKeySequence(Qt::ControlModifier + Qt::Key_Left), "historyBackAction"));
  historyBackAction->setToolTip("Move back in object browsing history\nCtrl + Left");
  historyBackAction->setStatusTip(historyBackAction->toolTip());

  historyForwardAction = AddAction(new taiAction("Forward",
    QKeySequence(Qt::ControlModifier + Qt::Key_Right), "historyForwardAction"));
  historyForwardAction->setToolTip("Move forward in object browsing history\nCtrl + Right");
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
  dockMenu = viewMenu->AddSubMenu("Dock Windows");

  viewMenu->insertSeparator();
  viewMenu->AddAction(viewSaveViewAction);

  // Make connections.
  connect(historyBackAction, SIGNAL(triggered()), brow_hist, SLOT(back()));
  connect(brow_hist, SIGNAL(back_enabled(bool)), historyBackAction, SLOT(setEnabled(bool)));

  connect(historyForwardAction, SIGNAL(triggered()), brow_hist, SLOT(forward()));
  connect(brow_hist, SIGNAL(forward_enabled(bool)), historyForwardAction, SLOT(setEnabled(bool)));

  connect(this, SIGNAL(SelectableHostNotifySignal(ISelectableHost*, int)),
    brow_hist, SLOT(SelectableHostNotifying(ISelectableHost*, int)));
  connect(brow_hist, SIGNAL(select_item(taiSigLink*)),
    this, SLOT(slot_AssertBrowserItem(taiSigLink*)));

  connect(viewRefreshAction, SIGNAL(Action()), this, SLOT(viewRefresh()));
  connect(viewSaveViewAction, SIGNAL(Action()), this, SLOT(viewSaveView()));
}

void iMainWindowViewer::Constr_ShowMenu()
{
  // These two items are presets for the other toggle flags.
  show_menu->AddItem("Normal &only", taiMenu::normal, taiAction::men_act,
      this, SLOT(ShowChange(taiAction*)), 0);
  show_menu->AddItem("&All", taiMenu::normal, taiAction::men_act,
      this, SLOT(ShowChange(taiAction*)), 1);

  // Toggle flags.
  // Note: correct toggles are set dynamically when user drops down menu.
  show_menu->AddSep();
  show_menu->AddItem("&Normal", taiMenu::toggle, taiAction::men_act,
      this, SLOT(ShowChange(taiAction*)), 2);
  show_menu->AddItem("E&xpert", taiMenu::toggle, taiAction::men_act,
      this, SLOT(ShowChange(taiAction*)), 3);
  show_menu->AddItem("&Hidden", taiMenu::toggle, taiAction::men_act,
      this, SLOT(ShowChange(taiAction*)), 4);
}

void iMainWindowViewer::Constr_ControlMenu()
{
  ctrlStopAction = AddAction(new taiAction("Stop", QKeySequence(), "ctrlStopAction"));
  ctrlStopAction->setIcon(QIcon(QPixmap(":/images/stop_icon.png")));
  ctrlStopAction->setToolTip("Stop: stop whatever program is currently running -- execution can be resumed with the Cont continue button.");

  ctrlContAction = AddAction(new taiAction("Cont", QKeySequence(), "ctrlContAction"));
  ctrlContAction->setIcon(QIcon(QPixmap(":/images/play_icon.png")));
  ctrlContAction->setToolTip("Continue: continue running the last program that was run, from wherever it was last stopped");

  // Build menu items.
  ctrlMenu->AddAction(ctrlStopAction);
  ctrlMenu->AddAction(ctrlContAction);

  // Make connections.
  connect(ctrlStopAction, SIGNAL(Action()), this, SLOT(ctrlStop()));
  connect(ctrlContAction, SIGNAL(Action()), this, SLOT(ctrlCont()));
}

void iMainWindowViewer::Constr_ToolsMenu()
{
  toolsClassBrowseAction = AddAction(new taiAction(0, "Class Browser", QKeySequence(), "toolsClassBrowseAction"));
  toolsTypeBrowseAction = AddAction(new taiAction(0, "&Help Browser", QKeySequence(), "toolsTypeBrowseAction"));

  // Build menu items.
  if (toolsMenu) {
    toolsMenu->AddAction(toolsClassBrowseAction);
    toolsMenu->AddAction(toolsTypeBrowseAction);
  }

  // Make connetions.
  connect(toolsClassBrowseAction, SIGNAL(triggered()),
    this, SLOT(toolsClassBrowser()));
  connect(toolsTypeBrowseAction, SIGNAL(triggered()),
    this, SLOT(toolsTypeBrowser()));
}

void iMainWindowViewer::Constr_HelpMenu()
{
  String s = taMisc::app_name + " Help on the web";
  helpHelpAction = AddAction(new taiAction("&Help", QKeySequence("F1"), "helpHelpAction"));
  helpHelpAction->setToolTip(s);
  helpHelpAction->setStatusTip(s);

  helpAboutAction = AddAction(new taiAction("&About", QKeySequence(), "helpAboutAction"));

  // Build menu items.
  helpMenu->AddAction(helpHelpAction);
  helpMenu->insertSeparator();
  helpMenu->AddAction(helpAboutAction);

  // Make connetions.
  connect(helpHelpAction, SIGNAL(Action()), this, SLOT(helpHelp()));
  connect(helpAboutAction, SIGNAL(Action()), this, SLOT(helpAbout()));
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

void iMainWindowViewer::Find(taiSigLink* root, const String& find_str) {
  // if an instance doesn't exist, need to make one; we tie it to ourself
  if (!search_dialog) {
    search_dialog = iSearchDialog::New(0, this);
  }
  iSearchDialog* dlg = search_dialog;

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
  taProject* proj = curProject();
  if (!proj) return;
  proj->undo_mgr.Undo();
}

void iMainWindowViewer::editRedo() {
  taProject* proj = curProject();
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
  close();
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
    String filename = tabMisc::root->recent_files[i];

    //taiAction* item =
    fileOpenRecentMenu->AddItemWithNumericAccel(
      filename,
      taiAction::var_act,
      this, SLOT(fileOpenFile(const Variant&)),
      filename);
  }
}

void iMainWindowViewer::fileOpenFile(const Variant& fname_) {
  String fname = fname_.toString();
  // check if already open
  taProject* proj = NULL;
  // canonicalize name, for comparison to open projects
  QFileInfo fi(fname);
  fname = fi.canonicalFilePath();
  for (int i = 0; i < tabMisc::root->projects.size; ++i) {
    proj = tabMisc::root->projects.FastEl(i);
    if (proj->file_name == fname) {
      int chs = taMisc::Choice("That project is already open -- it will be viewed instead",
                               "Ok", "Cancel");
      switch (chs) {
      case 0: break; // break out of switch -- we'll also break out of the loop
      case 1: return;
      }
      break; // break out of loop
    }
    proj = NULL; // in case we fall out of loop
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

void iMainWindowViewer::fileSaveAll() {
  if (!tabMisc::root) return;
  tabMisc::root->SaveAll();
}

void iMainWindowViewer::fileOpenFromWeb_aboutToShow()
{
  // Clear and rebuild submenu.
  fileOpenFromWebMenu->Reset();

  // TODO: For now, just one hard-coded value on menu.
  String repositoryName = "Emergent repository";
  String label = repositoryName + "...";

  fileOpenFromWebMenu->AddItemWithNumericAccel(
    label,
    taiAction::var_act,
    this, SLOT(fileOpenFromWeb(const Variant &)),
    repositoryName);
}

void iMainWindowViewer::fileOpenFromWeb(const Variant &repo)
{
  String repositoryName = repo.toString();
  // TODO.
}

void iMainWindowViewer::filePublishDocsOnWeb_aboutToShow()
{
  // Clear and rebuild submenu.
  filePublishDocsOnWebMenu->Reset();

  // For now, just one hard-coded value on menu.
  String repositoryName = "Emergent repository";
  String label = repositoryName + "...";

  filePublishDocsOnWebMenu->AddItemWithNumericAccel(
    label,
    taiAction::var_act,
    this, SLOT(filePublishDocsOnWeb(const Variant &)),
    repositoryName);
}

void iMainWindowViewer::filePublishDocsOnWeb(const Variant &repo)
{
  String repositoryName = repo.toString();

  if (taProject *proj = curProject()) {
    proj->PublishDocsOnWeb(repositoryName);
  }
}

void iMainWindowViewer::filePublishProjectOnWeb()
{
  if (taProject *proj = curProject()) {
    // TODO: proj->PublishProjectOnWeb();
  }
}

void iMainWindowViewer::fileClose() {
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
  iTreeView* mtv = GetMainTreeView();
  MainWindowViewer* db = viewer();
  if(!db) return false;
  PanelViewer* pv = db->GetMiddlePanel();
  if(!pv || !pv->widget()) return false;
  iTabViewer* itv = pv->widget();
  iDataPanel* idp = itv->tabView()->curPanel();
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
  T3DataViewer* pv = db->GetRightViewer();
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
    T3DataViewer* pv = db->GetRightViewer();
    if(pv && pv->tabBar()) {
      pv->tabBar()->selectNextTab();
    }
  }
  else {
    iTabViewer* itv = GetTabViewer();
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
    T3DataViewer* pv = db->GetRightViewer();
    if(pv && pv->tabBar()) {
      pv->tabBar()->selectPrevTab();
    }
  }
  else {
    iTabViewer* itv = GetTabViewer();
    if(itv) {
      itv->tabBar()->selectPrevTab();
    }
  }
  return true;
}

bool iMainWindowViewer::KeyEventFilterWindowNav(QObject* obj, QKeyEvent* e) {
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed) {
    switch(e->key()) {
    case Qt::Key_J: // move left between regions
      MoveFocusLeft();
      return true;
    case Qt::Key_L: // move right between regions
      MoveFocusRight();
      return true;
    case Qt::Key_Tab:
      if(e->modifiers() & Qt::ShiftModifier) {
        ShiftCurTabLeft();
      }
      else {
        ShiftCurTabRight();
      }
      return true;              // we absorb this event
    }
  }
  if(e->modifiers() & Qt::AltModifier) {
    switch(e->key()) {
    case Qt::Key_J: // move left between regions
      MoveFocusLeft();
      return true;
    case Qt::Key_L: // move right between regions
      MoveFocusRight();
      return true;
    }
  }
  return false;
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
      itv->CollapseAllUnder(rval);
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
  iTabViewer* itv = GetTabViewer(); // should exist
  if (!itv) return false;

  iDataPanel* pan = itv->tabView()->GetDataPanel(link);
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
// URLs are usually suffixed with a "#Xxx" where Xxx is the uniqueId()
// of the window in which is embedded the doc viewer
// if no WinId (ex. Find) then we use that of the topmost window

  //NOTE: URLs only open in the main project browser for that project
  String path = url.path(); // will only be part before #, if any

  // TypeBrowser invocations are dependency-free so we check them first
  if(path.startsWith(".Type.")) {
    iHelpBrowser::StatLoadUrl(String(url.toString()));
    return;
  }

  // we usually embed the uniqueId of the win that had the invoking url
  String win_id_str = String(url.fragment()).after("#");
  int win_id = win_id_str.toInt(); // 0 if empty or not an int
  if (win_id == 0) {
    iMainWindowViewer* top_win = taiMisc::active_wins.Peek_MainWindow();
    if (top_win)
      win_id = top_win->uniqueId();
  }

  iMainWindowViewer* idoc_win = NULL; // win from which url was invoked
  if (win_id != 0) { // problem if not found!!!
    idoc_win = taiMisc::active_wins.FindMainWindowById(win_id);
  }
//NOTE: if idoc_win is NULL, then the only really valid thing after this
// is a WIKI or Web url...

  // get the project -- should be able to get from any viewer/browser
  taProject* proj = NULL;
  if (idoc_win) {
    proj = idoc_win->myProject();
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

//IMPORTANT NOTE: You *must* check ALL objects for NULL in the following
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
      if (!link || !iproj_brow) {
        taMisc::Warning("ta: URL",path,"not found as a path to an object!");
        return;
      }
      Program* prg = (Program*)tab->GetThisOrOwner(&TA_Program);
      if(prg) {
        prg->BrowserSelectMe();
        taMisc::RunPending();
        if(url.hasFragment()) {
          prg->ViewCssScript();
          // todo: could do something with this!
          // int lno = (int)url.fragment();
          // if(lno > 0) {
          //   ProgLine* pl = prg->script_list->SafeEl(lno);
          // }
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
  // always just use the help browser for any misc links
  iHelpBrowser::StatLoadUrl(url.toString());
//   if(isProjShower()) {
//     taProject* prj = curProject();
//     if(prj) {
//       taDoc* browser = prj->FindMakeDoc("misc_browser", "", url.toString());
//       browser->EditDialog();
//       return;
//     }
//   }
//   else if(isRoot()) {
//     MainWindowViewer* db = viewer();
//     if(db) {
//       taRootBase* rt = dynamic_cast<taRootBase*>(db->data());
//       if(rt) {
//      taDoc* browser = rt->FindMakeDoc("misc_browser", "", url.toString());
//      browser->EditDialog();
//      return;
//       }
//     }
//   }
//   QDesktopServices::openUrl(url);    // fall back on default
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
    return inherited::eventFilter(obj, event);
  }
  QKeyEvent* e = static_cast<QKeyEvent *>(event);
  if(KeyEventFilterWindowNav(obj, e))
    return true;
  if(taiMisc::KeyEventFilterEmacs_Clip(obj, e))
    return true;
  return inherited::eventFilter(obj, event);
}

int iMainWindowViewer::GetEditActions() {
  int rval = 0;
  emit GetEditActionsEnabled(rval);
  return rval;
}

void iMainWindowViewer::viewSaveView() {
  viewer()->GetWinState();
}

void iMainWindowViewer::windowMenu_aboutToShow() {
  // Clear and rebuild submenu.
  windowMenu->Reset();

  // Populate with current windows.
  Widget_List wl;
  taiMisc::GetWindowList(wl);
  for (int i = 0; i < wl.size; ++i) {
    QWidget* wid = wl.FastEl(i);
    if (wid->isWindow()) {
      String title = wid->windowTitle();

      // Indicate whether the file has unsaved changes
      // (see MainWindowViewer::MakeWinName_impl())
      title.gsub("[*]", wid->isWindowModified() ? "*" : "");

      //taiAction* item =
      windowMenu->AddItemWithNumericAccel(
        title,
        taiAction::int_act,
        this, SLOT(windowActivate(int)),
        i);
    }
  }
}

void iMainWindowViewer::windowActivate(int win) {
  // populate with current windows -- should correspond to the ones enumerated
  Widget_List wl;
  taiMisc::GetWindowList(wl);
  QWidget* wid = wl.PosSafeEl(win);
  if (!wid) return;
  wid->activateWindow();
  wid->raise();
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

void iMainWindowViewer::mnuEditAction(taiAction* mel) {
   // called from context; cast obj to an taiClipData::EditAction
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
    IDataViewWidget* dvw = dynamic_cast<IDataViewWidget*>(obj); // null if not type
    if (dvw) {
      dvw->Refresh();
    }
  }
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
  if (proj) return proj->isDirty();
  else return false;
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
    if (last_sel_server == src_host) break; // nothing to do
    last_sel_server = src_host;
    QObject* handler_obj = src_host->clipHandlerObj();
    SetClipboardHandler(handler_obj, ISelectableHost::edit_enabled_slot,
      ISelectableHost::edit_action_slot);
    SelectableHostNotifying_impl(src_host, op); // break out for clarity
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
  if (last_clip_handler == handler_obj) return; // nothing to do
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
/*//TEMP
    taMisc::Warning("SetClipHandler to: type, name", handler_obj->metaObject()->className(),
      handler_obj->objectName());
} else {
    taMisc::Warning("SetClipHandler cleared");
// /TEMP */
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

void iMainWindowViewer::ShowChange(taiAction* sender) {
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

void iMainWindowViewer::this_DockSelect(taiAction* me) {
  iDockViewer* itb = (iDockViewer*)(me->usr_data.toPtr());
  DockViewer* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (me->isChecked()) { //note: check has already been toggled
    tb->Show();
  } else { //need to show
    tb->Hide();
  }
}

void iMainWindowViewer::this_FrameSelect(taiAction* me) {
  iFrameViewer* itb = (iFrameViewer*)(me->usr_data.toPtr());
  FrameViewer* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (me->isChecked()) { //note: check has already been toggled
    tb->Show();
  } else { //need to show
    tb->Hide();
  }
}

void iMainWindowViewer::this_ToolBarSelect(taiAction* me) {
  iToolBar* itb = (iToolBar*)(me->usr_data.toPtr());
  ToolBar* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (me->isChecked()) { //note: check has already been toggled
    tb->Show();
  } else { //need to show
    tb->Hide();
  }
}

void iMainWindowViewer::toolsClassBrowser() {
  MainWindowViewer* brows = MainWindowViewer::NewClassBrowser(&taMisc::types, &TA_TypeSpace);
  if (brows == NULL) return;
  brows->ViewWindow();
}

void iMainWindowViewer::toolsTypeBrowser() {
 // iHelpBrowser* tdd =
  iHelpBrowser::instance();
}

void iMainWindowViewer::UpdateUi() {
  int ea = GetEditActions();
  // some actions we always show, others we only show if available
  // editCutAction->setEnabled(ea & taiClipData::EA_CUT);
  // editCopyAction->setEnabled(ea & taiClipData::EA_COPY);
  // editDupeAction->setVisible(ea & taiClipData::EA_DUPE);
  editCutAction->setEnabled(true);
  editCopyAction->setEnabled(true);
  editDupeAction->setVisible(true);
  // we always show the plainjane Paste (enable or disable)
  // if more than one paste guy is enabled, no shortcuts/accelerators
  int paste_cnt = 0;
  if (ea & taiClipData::EA_PASTE) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_INTO) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_ASSIGN) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_APPEND) ++paste_cnt;
  editPasteAction->setEnabled(ea & taiClipData::EA_PASTE);
  editPasteIntoAction->setVisible(ea & taiClipData::EA_PASTE_INTO);
  editPasteAssignAction->setVisible(ea & taiClipData::EA_PASTE_ASSIGN);
  editPasteAppendAction->setVisible(ea & taiClipData::EA_PASTE_APPEND);
  if (ea & taiClipData::EA_PASTE_INTO) {
    if (paste_cnt > 1) {
      editPasteIntoAction->setText("Paste Into");
      editPasteIntoAction->setShortcut(QKeySequence());
    } else {
      editPasteIntoAction->setText("&Paste Into");
      editPasteIntoAction->setShortcut(QKeySequence(cmd_str + "V"));
    }
  }
  if (ea & taiClipData::EA_PASTE_ASSIGN)  {
    if (paste_cnt > 1) {
      editPasteAssignAction->setText("Paste Assign");
      editPasteAssignAction->setShortcut(QKeySequence());
    } else {
      editPasteAssignAction->setText("&Paste Assign");
      editPasteAssignAction->setShortcut(QKeySequence(cmd_str + "V"));
    }
  }
  if (ea & taiClipData::EA_PASTE_APPEND)  {
    if (paste_cnt > 1) {
      editPasteAppendAction->setText("Paste Append");
      editPasteAppendAction->setShortcut(QKeySequence());
    } else {
      editPasteAppendAction->setText("&Paste Append");
      editPasteAppendAction->setShortcut(QKeySequence(cmd_str + "V"));
    }
  }

  editDeleteAction->setEnabled(ea & taiClipData::EA_DELETE);

  // linking is currently not really used, so we'll not show by default
  // if we later add more linking capability, we may want to always enable,
  // just to hint user that it is sometimes available
  editLinkAction->setVisible(ea & taiClipData::EA_LINK);
  editLinkIntoAction->setVisible(ea & taiClipData::EA_LINK_INTO);

  if ((ea & taiClipData::EA_LINK2) == taiClipData::EA_LINK2) {
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

  editUnlinkAction->setVisible(ea & taiClipData::EA_UNLINK);

  taProject* proj = myProject();
  if(proj) {
    editUndoAction->setEnabled(proj->undo_mgr.UndosAvail() > 0);
    editRedoAction->setEnabled(proj->undo_mgr.RedosAvail() > 0);
  }

  bool css_running = (Program::global_run_state == Program::RUN);
  ctrlStopAction->setEnabled(css_running);
  ctrlContAction->setEnabled(!css_running && Program::last_run_prog);

  emit SetActionsEnabled();
}

// void iMainWindowViewer::windowActivationChange(bool oldActive) {
//   if (isActiveWindow()) {
//     int idx = taiMisc::active_wins.FindEl(this);
//     if (idx < 0) {
//       taMisc::Error("iMainWindowViewer::windowActivationChange", "Unexpectedly not in taiMisc::viewer_wins");
//     } else {
//       if (idx < (taiMisc::active_wins.size - 1)) {
//         // move us to the end
//         taiMisc::active_wins.MoveIdx(idx, taiMisc::active_wins.size - 1);
//       }
//     }
//   }
//   inherited::windowActivationChange(oldActive);
// }

void iMainWindowViewer::changeEvent(QEvent* ev) {
  if(ev->type() == QEvent::ActivationChange) {
    if (isActiveWindow()) {
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


//////////////////////////////////
//   taBase                     //
//////////////////////////////////

bool taBase::EditPanel(bool new_tab, bool pin_tab) {
  if(!taMisc::gui_active) return false;

  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;	// shouldn't happen
  taiSigLink* link = (taiSigLink*)GetSigLink();
  if (!link) return false;	// shouldn't happen

  MainWindowViewer* vwr = proj->GetDefaultProjectBrowser();
  if(!vwr) return false;	// shouldn't happen

  iMainWindowViewer* inst = vwr->widget();
  if (!inst) return false; // shouldn't happen!

  bool rval = false;
  // for new_tab, we open new locked panels,
  // for existing, we do a browse to the item
  if (new_tab) {
    rval = inst->AssertPanel(link, new_tab, pin_tab);
  } else {
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
    //if (vwr && (vwr->GetName() == "DefaultProjectBrowser")) return vwr;
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
    //if (vwr && (vwr->GetName() == "DefaultProjectBrowser")) return vwr;
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

