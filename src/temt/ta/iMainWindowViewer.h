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

#ifndef iMainWindowViewer_h
#define iMainWindowViewer_h 1

// parent includes:
#include <IViewerWidget>
#ifndef __MAKETA__
#include <QMainWindow>
#include <QPointer>
#endif

// member includes:
#include <MainWindowViewer>
#include <iAction_List>

// declare all other types mentioned but not required to include:
class taiWidgetMenu; //
class iAction; //
class iBrowseHistory; //
class iToolBar; //
class iFrameViewer; //
class iDockViewer; //
class iDialogSearch; //
class iTreeView; //
class ISelectable_PtrList; //
class iTreeViewItem; //
class iPanelViewer; //
class ISelectableHost; //
class iPanelBase; //
class iSplitter; //
class QSignalMapper; //
class iRect; //


class TA_API iMainWindowViewer: public QMainWindow, public IViewerWidget {
// ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS gui portion of the taViewer
  Q_OBJECT
INHERITED(QMainWindow)
friend class taSigLink;
friend class MainWindowViewer;
public:
#ifndef __MAKETA__
  enum CustomEventType {
    CET_FILE_CLOSE              = QEvent::User + 1,
  };
#endif

  enum MainWinLocs {            // names for major locations within window
    LEFT_BROWSER,
    MIDDLE_PANEL,
    RIGHT_VIEWER,
  };
  enum WinSubLocs {             // sub-locations within windows
    MAIN_TREE,                  // main tree view within left browser
    PROG_TREE,                  // program tree view browser
    PROG_EDIT,                  // program edit panel
    MIDDLE_TABS,                // middle tabs
    MIDDLE_EDIT,                // middle edit panel
    RIGHT_TABS,                 // right tabs
    RIGHT_VIEW,                 // right viewer
  };

  MainWinLocs           cur_main_focus; // where is our current major focus located?
  WinSubLocs            cur_sub_focus; // where is our current sub focus within main?

  iAction_List          actions; // our own list of all created actions
  iBrowseHistory*       brow_hist;
  taiWidgetMenuBar*     menu;           // menu bar -- note: we use the window's built-in QMenu
  iSplitter*            body;           // #IGNORE body of the window
  
  QSignalMapper*        signalMapperForViews;   // #IGNORE used to map several actions to one action and pass a value
  QSignalMapper*        signalMapperForDataProc;   // #IGNORE used to map several actions to one action and pass a value
  QSignalMapper*        signalMapperForDataAnal;   // #IGNORE used to map several actions to one action and pass a value
  QSignalMapper*        signalMapperForDataGen;   // #IGNORE used to map several actions to one action and pass a value
  QSignalMapper*        signalMapperForImageProc;   // #IGNORE used to map several actions to one action and pass a value
  
  taiWidgetMenu*              fileMenu;
  taiWidgetMenu*              fileOpenRecentMenu;
  taiWidgetMenu*              fileExportMenu; // submenu -- empty and disabled in base
  taiWidgetMenu*              editMenu;
  taiWidgetMenu*              viewMenu;
  taiWidgetMenu*              show_menu;
  taiWidgetMenu*              ctrlMenu;
  taiWidgetMenu*              frameMenu;   // enumeration of all Frame guys
  taiWidgetMenu*              toolBarMenu; // enumeration of all ToolBar guys
  taiWidgetMenu*              dockMenu;    // enumeration of all Dock guys
  taiWidgetMenu*              dataMenu;
  taiWidgetMenu*              toolsMenu;
  taiWidgetMenu*              windowMenu;  // on-demand
  taiWidgetMenu*              helpMenu;
  taiWidgetMenu*              processMenu;        // data_proc methods
  taiWidgetMenu*              analysisMenu;       // data_analysis methods
  taiWidgetMenu*              generateMenu;       // data_gen methods
  taiWidgetMenu*              processImageMenu;   // image_proc methods
  
  iAction*            historyBackAction;
  iAction*            historyForwardAction;
  iAction*            fileNewAction;
  iAction*            fileOpenAction;
  iAction*            fileSaveAction;
  iAction*            fileSaveAsAction;
  iAction*            fileSaveNotesAction;
  iAction*            fileSaveAsTemplateAction;
  iAction*            fileOpenSvnBrowserAction;
  iAction*            fileSvnCommitAction;
  iAction*            fileUpdateChangeLogAction;
  iAction*            fileSaveAllAction;
  taiWidgetMenu*      fileOpenFromWebMenu;
  taiWidgetMenu*      filePublishDocsOnWebMenu;
  iAction*            filePublishProjectOnWebAction;
  iAction*            fileCloseAction;
  iAction*            fileOptionsAction;
  iAction*            filePrintAction;
  iAction*            fileCloseWindowAction; //note: special, because it is always at bottom of File menu for non-root
  iAction*            fileQuitAction; //note: only on app window, except on all in Mac
  iAction*            fileRecentsClearAction;
  
  iAction*            editUndoAction;
  iAction*            editRedoAction;
  iAction*            editCutAction;
  iAction*            editCopyAction;
  iAction*            editDupeAction;
  iAction*            editDeleteAction;
  iAction*            editPasteAction;
  iAction*            editPasteIntoAction;
  iAction*            editPasteAssignAction;
  iAction*            editPasteAppendAction;
  iAction*            editLinkAction;
  iAction*            editLinkIntoAction;
  iAction*            editUnlinkAction;
  iAction*            editFindAction;
  iAction*            editFindNextAction;
  
  iAction_List         dataProcCopyActions;
  iAction_List         dataProcOrderActions;
  iAction_List         dataProcSelectActions;
  iAction_List         dataProcColumnsActions;
  iAction_List         dataAnalStatsActions;
  iAction_List         dataAnalDistanceActions;
  iAction_List         dataAnalHighDimActions;
  iAction_List         dataAnalCleanActions;
  iAction_List         dataAnalGraphActions;
  iAction_List         dataGenBasicActions;
  iAction_List         dataGenListsActions;
  iAction_List         dataGenDrawActions;
  iAction_List         dataGenRandomActions;
  iAction_List         dataGenFeatPatsActions;
  iAction_List         dataGenFilesActions;
  iAction_List         imageProcTransformActions;
  iAction_List         imageProcFilterActions;
  iAction_List         imageProcNoiseActions;
  iAction_List         imageProcImageProcActions;
  
  iAction*            viewRefreshAction;
  iAction*            viewSplitVerticalAction;
  iAction*            viewSplitHorizontalAction;
  iAction*            viewCloseCurrentViewAction;
  iAction*            viewSetSaveViewAction;

  iAction*            viewBrowseOnlyAction;   // all possible combinations of the main frames
  iAction*            viewPanelsOnlyAction;
  iAction*            viewBrowseAndPanelsAction;
  iAction*            viewT3OnlyAction;
  iAction*            viewBrowseAndT3Action;
  iAction*            viewPanelsAndT3Action;
  iAction*            viewAllFramesAction;

  iAction*            toolsTypeInfoBrowseAction;
  iAction*            toolsHelpBrowseAction;
  iAction*            toolsSvnBrowseActionEmergent;
  iAction*            toolsSvnBrowseActionSvn1;
  iAction*            toolsSvnBrowseActionSvn2;
  iAction*            toolsSvnBrowseActionSvn3;

  iAction*            ctrlStopAction;
  iAction*            ctrlContAction;
  iAction*            progStatusAction; // program run status and button

  iAction*            helpHelpAction;
  iAction*            helpAboutAction;
  iAction*            helpFileBugAction;
  
  iAction*            windowMinimizeAction;
  iAction*            windowZoomAction;

  // Hack - Qt5.2 & OS X 10.9 window menu not appearing - add dummy action to make it visible until a window is created
  iAction*            dummyWindowAction;

  QObject*              clipHandler() {return last_clip_handler;} // obj (if any) controlling clipboard handling

  taProject*            myProject() const; // project of which this viewer is a part
  taProject*            curProject() const; // only if we are a projviewer
  inline bool           isRoot() const {return m_is_root;} // if this is app root, closing quits
  inline bool           isProjBrowser() const {return m_is_proj_browser;} // if a project browser, persistent
  inline bool           isProjViewer() const {return m_is_proj_viewer;} // if a project viewer, persistent
  inline bool           isProjShower() const
    {return m_is_proj_viewer || m_is_proj_browser;} // if either

  int                   uniqueId() const {return m_unique_id;} // for urls
  inline MainWindowViewer* viewer() const {return (MainWindowViewer*)m_viewer;}

  virtual iAction*    AddAction(iAction* act); // add the action to the list, returning the instance (for convenience)
  void                  AddPanel(iPanelBase* panel, bool new_tab = true);
    // insures we have a iPanelViewer; adds panel; if requested, adds a new tab, sets panel active in it
  virtual void          AddApplicationToolBar(iToolBar* tb); // add the toolbar, showing it if it is mapped
  virtual void          AddFrameViewer(iFrameViewer* fv, int at_index = -1); // -1=end
#ifndef __MAKETA__
  virtual void          AddDockViewer(iDockViewer* dv,
    Qt::DockWidgetArea in_area = Qt::BottomDockWidgetArea);
  QPointer<iDialogSearch> search_dialog;
  QPointer<iTreeView>   cur_tree_view;  // current tree viewer (set in focus event on iTreeView)

  void                  Find(taiSigLink* root, const String& find_str="");
  // common find called by main menu, and context menu finds
  void                  Replace(taiSigLink* root, ISelectable_PtrList& sel_items,
                                const String& srch="", const String& repl="");
  // replace called by main menu, and context menu
#endif

  //////////////////////////////////////////////////////////////
  // navigation around the window

  virtual iTreeView*    GetMainTreeView();
  // the main one from the iBrowseViewer
  virtual iTreeView*    GetCurTreeView();
  // the tree view can be a sub-viewer (e.g., program editor) instead of the main one

  virtual bool          FocusCurTreeView();
  // send focus back to current tree view

  virtual bool          FocusLeftBrowser();
  // send focus to left browser
  virtual bool          FocusMiddlePanel();
  // send focus to middle panel
  virtual bool          FocusRightViewer();
  // send focus to right viewer

  virtual void          FocusIsLeftBrowser();
  // update main window that focus is currently in left browser -- sometimes it gets confused..
  virtual void          FocusIsMiddlePanel(WinSubLocs sub_foc = MIDDLE_EDIT);
  // update main window that focus is currently in middle browser with given sub focus -- sometimes it gets confused..
  virtual void          FocusIsRightViewer(WinSubLocs sub_foc = RIGHT_TABS);
  // update main window that focus is currently in right viewer with given sub focus -- sometimes it gets confused..

  virtual bool          MoveFocusLeft();
  // shift focus to the left of current (wraps around)
  virtual bool          MoveFocusRight();
  // shift focus to the right of current (wraps around)
  virtual bool          ShiftCurTabRight();
  // shift tab in currently focused panel to the right
  virtual bool          ShiftCurTabLeft();
  // shift tab in currently focused panel to the left

  virtual bool          KeyEventFilterWindowNav(QObject* obj, QKeyEvent* e);
  // process window navigation key events (Ctrl/Alt J, L, Tab) -- returns true if processed

  iTreeViewItem*        AssertBrowserItem(taiSigLink* link);
  iTreeViewItem*        BrowserExpandAllItem(taiSigLink* link);
  iTreeViewItem*        BrowserCollapseAllItem(taiSigLink* link);

  bool                  AssertPanel(taiSigLink* link, bool new_tab = false,
                                    bool new_tab_lock = true);
  // used for things like wizards and edits; note: ntl ignored if !nt
  void                  EditItem(taiSigLink* link, bool not_in_cur = false);
  // edit this guy in a new panel, making a tab viewer if necessary
  int                   GetEditActions();
  // after a change in selection, update the available edit actions (cut, copy, etc.)
  iPanelViewer*           GetTabViewer(bool force = false);
  // get the tab viewer, or make one if force

  void                  setFrameGeometry(const iRect& r);
  void                  setFrameGeometry(int left, int top, int width, int height);
  //bogus: see Qt docs on geometry under X
  bool                  AlignCssConsole();
  // align css console to our window, if applicable
  void			ProjDirToCurrent();
  // set current project directory to current directory
  virtual void         raise();

  static iMainWindowViewer* GetViewerForObj(taBase* obj);
  // useful utility to get main window viewer for any given object

  iMainWindowViewer(MainWindowViewer* viewer_, QWidget* parent = NULL);
    // uses: WFlags flags = (WType_TopLevel | WStyle_SysMenu | WStyle_MinMax | WDestructiveClose)
  ~iMainWindowViewer();

public slots:
  void slot_AssertBrowserItem(taiSigLink* link) {
    AssertBrowserItem(link);
  }

  virtual void  fileNew();      // New Project (in new viewer)
  virtual void  fileOpen();     // Open Project (in new viewer)
  virtual void  fileOpenRecent_aboutToShow();
  virtual void  fileOpenFile(const Variant& file); // for recent menu
  virtual void  fileSave();     // Save Project (only enabled if viewer)
  virtual void  fileSaveAs();   // SaveAs Project (only enabled if viewer)
  virtual void  fileSaveNotes();        // SaveNoteChanges Project (only enabled if viewer)
  virtual void  fileSaveAsTemplate();   // SaveAsTemplate Project (only enabled if viewer)
  virtual void  fileOpenSvnBrowser();   // open svn browser for project
  virtual void  fileSvnCommit();        // commit project file to svn
  virtual void  fileUpdateChangeLog();  // UpdateChangeLog Project (only enabled if viewer)
  virtual void  fileSaveAll();          // Save All Projects (always enabled)
  virtual void  fileOpenFromWeb_aboutToShow();
  virtual void  fileOpenFromWeb(const Variant &repo);      // Open Project from Web (in new viewer)
  virtual void  filePublishDocsOnWeb_aboutToShow();
  virtual void  filePublishDocsOnWeb(const Variant &repo); // Publish Project Documentation on Web
  virtual void  filePublishProjectOnWeb(); // Publish Project on Web
  virtual void  fileClose();    // Close Project (only enabled if viewer)
  virtual void  fileOptions();  // edits taMisc
  virtual void  filePrint();
  virtual void  fileCloseWindow();// (non-root only)
  virtual void  fileQuit(); // (root) or all on Mac (needed for App menu)
  virtual void  fileClearRecentsMenu();

  virtual void  editMenu_aboutToShow();
  virtual void  editUndo();
  virtual void  editRedo();
  virtual void  editFind();
  virtual void  editFindNext();
  virtual void  viewRefresh() { Refresh(); UpdateUi(); } // manually rebuild/refresh the current view
  virtual void  viewSaveView(); // save the current view state

  virtual void  ShowHideFrames(int combo);
  virtual void  showMenu_aboutToShow();
  virtual void  ShowChange(iAction* sender);  // when show/hide menu changes

  void          toolsTypeInfoBrowser();
  void          toolsHelpBrowser();
  void          toolsSvnBrowserEmergent();
  void          toolsSvnBrowserSvn1();
  void          toolsSvnBrowserSvn2();
  void          toolsSvnBrowserSvn3();

  virtual void  ctrlStop();
  virtual void  ctrlCont();
  virtual void  progStatus();
  
  virtual void  windowMenu_aboutToShow();
  void          windowActivateByName(const Variant& title);
  virtual void  WindowMinimize();
  virtual void  WindowZoom();
  virtual void  helpHelp();
  virtual void  helpAbout();
  virtual void  FileBugReport();

  virtual void  mnuEditAction(iAction* mel);

  void          SetClipboardHandler(QObject* handler_obj,
    const char* edit_enabled_slot = NULL,
    const char* edit_action_slot = NULL,
    const char* actions_enabled_slot = NULL,
    const char* update_ui_signal = NULL); // see "Clipboard Handling" in .cpp

  void          SelectableHostNotifySlot(ISelectableHost* src_host, int op);
    // see "Selection Handling" in .cpp

  virtual void  UpdateUi();
    // Clipboard server: called by cliphandler after major events, to refresh menus, toolbars, etc.

#ifndef __MAKETA__
  void          taUrlHandler(const QUrl& url);
  // QDesktopServices::setUrlHandler -- "ta" scheme set to call this guy to process all our "internal" links
  void          httpUrlHandler(const QUrl& url);
  // QDesktopServices::setUrlHandler -- "http" scheme set to call this guy to re-route all web access through our internal browser

signals:
  void          EditAction(int ea);
    // Clipboard server: param is one of the iClipData editAction values
  void          GetEditActionsEnabled(int& ea);
    // Clipboard server: param is one of the iClipData EditAction values
  void          SetActionsEnabled();
    // Clipboard server: enable/disable actions

  void          SelectableHostNotifySignal(ISelectableHost* src_host, int op);
    // see "Selection Handling" in .cpp
#endif

public: // IViewerWidget i/f
  bool         isDirty() const override;
  QWidget*     widget() override {return this;}
  void         SaveData() override;
protected:
  void         Constr_impl() override;
  void         ResolveChanges_impl(CancelOp& cancel_op) override; // only for project browsers
  void         Refresh_impl() override;

protected slots:
  void                  ch_destroyed(); // cliphandler destroyed (just in case it doesn't deregister)

  virtual void          this_FrameSelect(iAction* me); // user has selected or unselected one of the frames
  virtual void          this_ToolBarSelect(iAction* me); // user has selected or unselected one of the toolbars
  virtual void          this_DockSelect(iAction* me); // user has selected or unselected one of the docks
  virtual void          this_SaveView(iAction* me); // user does/doesn't want current view saved with project

virtual void          DataProcLauncher(QString method_name);
virtual void          DataAnalLauncher(QString method_name);
virtual void          DataGenLauncher(QString method_name);
virtual void          ImageProcLauncher(QString method_name);

protected:
  static int            s_next_unique_id;

  bool                  m_is_root; // true if this is a root window (has Quit menu)
  bool                  m_is_proj_browser; // true if this is a project browser (false for simple browsers)
  bool                  m_is_proj_viewer; // true if this is a project viewer
  bool                  m_close_proj_now; // flag used to pass command to close proj
  int                   m_unique_id;
  bool                  tools_dock_was_visible;  // holds on to the visibility of the programming dock so it can be restored when the either the tree or the panels are restored

  void         closeEvent(QCloseEvent* ev) override;
//nn  void             customEvent(QEvent* ev) override;
  bool         event(QEvent* ev) override;
  void         resizeEvent(QResizeEvent* ev) override;
  void         moveEvent(QMoveEvent* ev) override;
  void         showEvent(QShowEvent* ev) override;
  void         hideEvent(QHideEvent* ev) override;
  bool         eventFilter(QObject *obj, QEvent *event) override;
  void         changeEvent(QEvent* ev) override;

  virtual void          emit_EditAction(int param); // #IGNORE param is one of the iClipData editAction values; desc can trap this and implement virtually, if desired
  virtual void          Constr_MainMenu_impl(); // #IGNORE constructs the main menu items, and loads static images
  virtual void          Constr_Menu_impl(); // #IGNORE constructs the menu and actions; MUST construct all static actions
  virtual void          SelectableHostNotifying_impl(ISelectableHost* src_host, int op);
    // called when we should handle this for sure

#ifndef _MAKETA__
private:
  void Constr_FileMenu();
  void Constr_EditMenu();
  void Constr_ViewMenu();
  void Constr_ShowMenu();
  void Constr_ControlMenu();
  void Constr_DataMenu();
  void Constr_ToolsMenu();
  void Constr_HelpMenu();
  void Constr_WindowMenu();

  static const QString cmd_str;
  QObject* last_clip_handler; //we need to remember this, because you can't anonymously disconnect signals from your own slots
  ISelectableHost* last_sel_server; // last guy to get focus
  void                  Init();
#endif
};

#endif // iMainWindowViewer_h
