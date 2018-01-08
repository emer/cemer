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

#ifndef MainWindowViewer_h
#define MainWindowViewer_h 1

// parent includes:
#include <TopLevelViewer>

// member includes:
#include <ToolBar_List>
#include <FrameViewer_List>
#include <DockViewer_List>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class iMainWindowViewer; // #IGNORE
class taBase; // 
class MemberDef; // 
class taProject; // 
class DockViewer; // 
class FrameViewer; // 
class ToolBar; // 
class BrowseViewer; // 
class PanelViewer; // 
class T3PanelViewer; // 
class taiWidgetActions_List; //
class taiWidgetMenuBar; // #IGNORE
class taiWidgetActions; //


taTypeDef_Of(MainWindowViewer);

class TA_API MainWindowViewer : public TopLevelViewer {
  // ##DEF_NAME_ROOT_Browser ##EXPAND the uber controller for main windows
INHERITED(TopLevelViewer)
friend class taSigLink;
friend class ToolBar_List;
friend class FrameView_List;
friend class DockView_List;
//friend class WindowState;
public:
  static TypeDef*       def_browser_type; // #HIDDEN #NO_SAVE type of the default browser, us unless replaced
  static TypeDef*       def_viewer_type; // #HIDDEN #NO_SAVE type of the default viewer, us unless replaced

  static MainWindowViewer* NewBrowser(taBase* root, MemberDef* root_md = NULL, bool is_root = false);
    // makes a standard 2-pane taBase browser
  static MainWindowViewer* NewTypeInfoBrowser(void* root, TypeDef* root_typ, MemberDef* root_md = NULL);
    // convenience class: makes a 2-pane class browser (browse+panels)
  static MainWindowViewer* NewProjectBrowser(taProject* proj);
    // makes a standard 3-pane project viewer (3-view) or 2 (2x2) -- returns the guy with tree
  static MainWindowViewer* NewProjectViewer(taProject* proj);
    // makes a standard 3d viewer, with panel (useful for more graphs, etc.)
  static MainWindowViewer* NewEditDialog(taBase* root);
    // makes a 1-pane taBase window only showing the edit panels
  static MainWindowViewer* FindEditDialog(taBase* root);
    // find an existing edit dialog associated with given root item

  static MainWindowViewer* GetDefaultProjectBrowser(taProject* proj = NULL);
    // get the default pb for given project, or whatever one is current if NULL
  static MainWindowViewer* GetDefaultProjectViewer(taProject* proj = NULL);
    // get the default pv for given project, or whatever one is current if NULL -- for 3-pane B==V for 2x2 B is the tree guy V is the T3 guy

  bool                  m_is_root; // #READ_ONLY #HIDDEN
  bool                  m_is_viewer_xor_browser; // #READ_ONLY #HIDDEN (weird, for compat w <=4.0.6)
  bool                  m_is_proj_viewer; // #READ_ONLY #HIDDEN
  bool                  m_is_dialog; // #READ_ONLY #HIDDEN when we use the viewer as an edit dialog
  int                   revert_to_tab_no; // #READ_ONLY #HIDDEN tab number to revert to in wait proc..

#ifdef TA_GUI
  taiWidgetActions_List* ta_menus; // #IGNORE menu representations (from methods, non-menubuttons only)
  taiWidgetMenuBar*     menu; // #IGNORE menu bar -- note: partially managed by the window
  taiWidgetActions*     cur_menu; // #IGNORE for building menu
#endif
  ToolBar_List          toolbars; // #EXPERT
  FrameViewer_List      frames; // the frames shown in the center splitter area
  DockViewer_List       docks; // #EXPERT currently docked windows -- removed if they undock

  bool                  isDialog() const {return m_is_dialog;}
  bool                  isRoot() const override {return m_is_root;}
  bool                  isRootLevelView() const override {return true;}
  bool                  isProjBrowser() const;
    // main proj window with tree browser (always t for 3-pane guy)
  bool                  isProjViewer() const {return m_is_proj_viewer;}
    // main proj window with t3 guy (always t for 3-pane guy)
  bool                  isProjShower() const; // if a proj viewer or browser
  void                  setBrowserViewer(bool is_browser, bool is_viewer); // use this to set the weird  bits

  inline iMainWindowViewer* widget() {return (iMainWindowViewer*)inherited::widget();}
  // #IGNORE

  virtual void          AddDock(DockViewer* dv);
    // add the supplied dock

  virtual FrameViewer*  FindFrameByType(TypeDef* typ, int& at_index, int from_index = 0);
    // find the first frame and index of given type from the given starting index;
  virtual void          AddFrame(FrameViewer* fv, int at_index);
    // add the supplied frame
  virtual FrameViewer*  AddFrameByType(TypeDef* typ, int at_index = -1);
    // add a new frame of given type at index (-1 at end); no window made yet

  virtual DockViewer*   FindDockViewerByName(const String& dv_name);
  virtual ToolBar*      FindToolBarByType(TypeDef* typ, const String& tb_name); // finds existing toolbar by name and type; NULL if not found
  virtual bool          AddToolBar(ToolBar* tb); // add a new toolbar; true if added (won't add a duplicate)
  virtual ToolBar*      AddToolBarByType(TypeDef* typ, const String& tb_name); // add a new toolbar by type; return inst if added (won't add a duplicate)

  virtual bool          SelectPanelTabNo(int tab_no);
  // select PanelViewer (middle edit panel) tab by number
  virtual bool          SelectPanelTabName(const String& tab_name);
  // select PanelViewer  (middle edit panel) tab by name

  virtual bool          SelectT3ViewTabNo(int tab_no);
  // select T3PanelViewer (3d view) (right view panel) tab by number
  virtual bool          SelectT3ViewTabName(const String& tab_name);
  // select T3PanelViewer (3d view) (right view panel) tab by name
  virtual bool          SelectT3ViewTabName_temp(const String& tab_name);
  // select T3PanelViewer (3d view) (right view panel) tab by name, and then revert to previously-selected tab in the wait proc
  virtual void          RevertToPrevT3TabNo();
  // #EXPERT revert to the previous T3 tab (waitproc callback)
  

  BrowseViewer*         GetLeftBrowser();
  // get the BrowseViewer (left browser panel)
  PanelViewer*          GetMiddlePanel();
  // get the PanelViewer (middle edit panel)
  T3PanelViewer*        GetRightViewer();
  // get the T3PanelViewer (right viewer panel)
  
  BrowseViewer*         GetNavigator() { return GetLeftBrowser(); }

  void         FrameSizeToSize(iSize& sz) override;
  bool         GetWinState() override;
  bool         SetWinState() override;
  void         ResetSplitterState();
  void         ResolveChanges(CancelOp& cancel_op) override;

  void  UpdateAfterEdit() override;
  void  InitLinks() override;
  void  CutLinks() override;
  TA_DATAVIEWFUNS(MainWindowViewer, TopLevelViewer) //

public: // Action methods
/*  virtual void        FileNewAction(){}
  virtual void  FileOpenAction(){}
  virtual void  FileSaveAction(){}
  virtual void  FileSaveAsAction(){}
  virtual void  FileSaveAllAction(){}
  virtual void  FileCloseAction(){} */
  virtual void  FileOptionsAction(); //
  virtual void  FilePrintAction(){}
  virtual void  FileCloseAction(); // #ACT #MM_&File|&Close #MENUGP_LAST #MENU_GP_FileClose Quit Action(root) or Close Window Action(non-root)
  virtual void  EditUndoAction(); // #ACT
  virtual void  EditRedoAction(); // #ACT
  virtual void  EditCutAction(); // #ACT
  virtual void  EditCopyAction(); // #ACT
  virtual void  EditPasteAction(); // #ACT
  virtual void  EditFindAction(); // #ACT
  virtual void  ViewRefreshAction() {}  // #ACT rebuild/refresh the current view
  virtual void  HelpIndexAction(); // #ACT
  virtual void  HelpContentsAction(); // #ACT
  virtual void  HelpAboutAction() {} // #ACT

protected:
  // from taDataView
  void         SigEmit_Child(taBase* child, int sls, void* op1, void* op2) override;
  void         DoActionChildren_impl(DataViewAction act) override; // just one act
  void         CloseWindow_impl() override;

  //from DataView
  void         Constr_impl(QWidget* gui_parent) override;
  IViewerWidget* ConstrWidget_impl(QWidget* gui_parent) override;
  void         WidgetDeleting_impl() override;
  void         Show_impl() override; // only called if mapped; de-iconifies
  void         Hide_impl() override; // only called if mapped; iconifies

  // from TopLevelView
  void         MakeWinName_impl() override;


  virtual void          ConstrToolBars_impl();
  virtual void          ConstrFrames_impl();
  virtual void          ConstrDocks_impl();
  void                  OnToolBarAdded(ToolBar* tb, bool post_constr);


private:
  void  Copy_(const MainWindowViewer& cp);
  void  Initialize();
  void  Destroy();
};

#endif // MainWindowViewer_h
