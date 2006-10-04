// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

// ta_viewer.h: non-gui definitions of classes mediated window viewing

#ifndef TA_VIEWER_H
#define TA_VIEWER_H

#include "ta_base.h"

#include "ta_TA_type.h"

// externals (most in in ta_qtviewer.h, some in ta_qtdata.h)

class taProject;

#ifdef TA_GUI
  class taiMenuBar;
  class taiMenu_List;
  class taiActions;
  class taiDataLink;
  class ISelectable;
  class IDataViewWidget;
  class iFrameViewer;
  class iBrowseViewer;
  class iTabViewer;
  class iTabViewer;
  class iMainWindowViewer;
  class iDockViewer;
  class iToolBoxDockViewer;
  class iTabBar;
  class iTabView;
  class iDataPanel;
  class iDataPanelSet;
  class iDataPanel_PtrList;
  class iTabView_PtrList;
  class iToolBar;
  class iToolBar_List; //
#else // !TA_GUI
// note: QObject/QWidget below are always the dummy guys
# typedef taiMenuBar QWidget;
# typedef taiMenu_List VoidClass;
# typedef taiActions QObject;
# typedef taiDataLink taDataLink;
# typedef ISelectable VoidClass;
# typedef IDataViewWidget VoidClass;
# typedef iFrameViewer QWidget;
# typedef iBrowseViewer QWidget;
# typedef iTabViewer QWidget;
# typedef iTabViewer QWidget;
# typedef iMainWindowViewer QWidget;
# typedef iDockViewer QWidget;
# typedef iToolBoxDockViewer QWidget;
# typedef iTabBar QWidget;
# typedef iTabView QWidget;
# typedef iDataPanel QWidget;
# typedef iDataPanelSet QWidget;
# typedef iDataPanel_PtrList VoidClass;
# typedef iTabView_PtrList VoidClass;
# typedef iToolBar QWidget;
# typedef iToolBar_List  VoidClass;
#endif // def TA_GUI

// forwards this file

class WindowState;
class ToolBar_List;
class DataViewer;
class   ToolBar;
class   TopLevelViewer;
class     DockViewer;
class     MainWindowViewer;
class   FrameViewer;
class     BrowseViewer;
class       tabBrowseViewer;
class       ClassBrowseViewer;
class     PanelViewer;
class ISelectable;
class ISelectable_PtrList;
class DynMethodDesc; // #IGNORE
class DynMethod_PtrList; // #IGNORE
class ISelectableHost;


class TA_API DataViewer : public taDataView {
  // #NO_TOKENS #VIRT_BASE the base type for objects with a gui window rep of some kind
INHERITED(taDataView)
friend class taDataLink;
friend class DataViewer_List;
friend class MainWindowViewer;
//friend class WindowState;
public:
  enum PrintFmt {
    POSTSCRIPT,
    JPEG,
    TIFF,
    PDF //NOTE: new for Qt version
  };
  
  static void		GetFileProps(TypeDef* td, String& fltr, bool& cmprs);
  // #IGNORE get file properties for given type
  
  String		name;		// name of the object
  bool			display_toggle;  // #DEF_true 'true' if display should be updated
  bool			visible; // #HIDDEN whether toolbar window is being shown to user

  virtual bool		deleteOnWinClose() const {return false;}
  inline IDataViewWidget* dvwidget() const {return m_dvwidget;}
  override bool		isMapped() const; // only true if in gui mode and gui stuff exists 
  MainWindowViewer*	parent() const {return (MainWindowViewer*)m_parent;} 
  override TypeDef*	parentType() const {return &TA_MainWindowViewer;} 
  QWidget*		widget();
  virtual iMainWindowViewer* window() {return NULL;}
    // #IGNORE valid if is, or is within, a main window
  
  // view state properties (don't require to be mapped)
  virtual bool		isVisible() const; // whether we are supposed to be showing or not (view state)
  virtual void		setVisible(bool value, bool update_view = true); // whether we are supposed to be showing or not (view state)

  virtual void 		Constr(QWidget* gui_parent = NULL); // #IGNORE constrs the gui this class NOTE: only called directly for gui tops or for items added after mapping; all others recursively call _impl, then _post
  void 			CloseWindow() {DoActions(CLOSE_WIN_IMPL);}	
   // #IGNORE closes the window or panel, removing our reference
 
  virtual void  	Show();		// make the item visible, if this is applicable
  virtual void		Hide();		// hide (but don't delete) the item, if applicable

  virtual void 		GetWinState(); // copy gui state to us (override impl)
  virtual void		SetWinState(); // set gui state from us (override impl)

  virtual void		ResolveChanges(CancelOp& cancel_op); // resolve all changes (if mapped)
  virtual void 		WindowClosing(CancelOp& cancel_op) {} 
   // cb from m_widget, subordinate wins may not be cancellable
  virtual void		WidgetDeleting(); // lets us do any cleanup -- override the impl
  
  bool 	SetName(const String& nm) {name = nm; return true;}
  String GetName() const {return name; }
  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const DataViewer& cp);
  COPY_FUNS(DataViewer, taDataView)
  TA_DATAVIEWFUNS(DataViewer, taDataView) //


protected:
  // from taDataView
  override void		CloseWindow_impl(); // closes the widget, only called if mapped, default calls the Close on the IDVW
  
  virtual void		Constr_impl(QWidget* gui_parent);
  virtual IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent) {return NULL;} 
    // implement this to create and set the m_widget instance -- only called if !m_widget
  override void		Constr_post();
  override void 	Dump_Save_pre();
  virtual void		WidgetDeleting_impl(); // lets us do any cleanup -- override the impl
  virtual void 		GetWinState_impl() {} // set gui state; only called if mapped
  virtual void 		SetWinState_impl() {} // fetch gui state; only called if mapped
  virtual void		Show_impl(); // only called if mapped (note: replaced in toplevelviewer)
  virtual void		Hide_impl(); // only called if mapped (note: replaced in toplevelviewer)
  virtual void		ResolveChanges_impl(CancelOp& cancel_op); // if mapped

private:
  IDataViewWidget*	m_dvwidget; // this guy can be dangerous, so we bury it
  
  void 	Initialize();
  void	Destroy();
};

class TA_API DataViewer_List: public DataView_List { // #NO_TOKENS
INHERITED(DataView_List)
public:
  
  TA_DATAVIEWLISTFUNS(DataViewer_List, DataView_List, DataViewer)

private:
  void 	Initialize() { SetBaseType(&TA_DataViewer);}
  void	Destroy() {}
};


class TA_API FrameViewer : public DataViewer {
  // #NO_TOKENS #VIRT_BASE for views that can be in the splitter of a MainWindowViewer
INHERITED(DataViewer)
public:
  
  inline iFrameViewer* widget() {return (iFrameViewer*)inherited::widget();} // lex override
  
  inline MainWindowViewer* mainWindowViewer() {return parent();}
  override iMainWindowViewer* window();
  
//  void	InitLinks();
//  void	CutLinks(); //
//  void 	Copy_(const FrameViewer& cp);
//  COPY_FUNS(FrameViewer, DataViewer) //
  TA_DATAVIEWFUNS(FrameViewer, DataViewer) //
protected:

private:
  void 	Initialize();
  void	Destroy() {CutLinks();}
};

class TA_API FrameViewer_List: public DataViewer_List { // #NO_TOKENS
INHERITED(DataViewer_List)
public:
  TA_DATAVIEWLISTFUNS(FrameViewer_List, DataViewer_List, FrameViewer)
private:
  void 	Initialize() { SetBaseType(&TA_FrameViewer);}
  void	Destroy() {}
};


class TA_API BrowseViewer : public FrameViewer {
  // #NO_TOKENS the base type for browser frames (tree of objects or classes)
INHERITED(FrameViewer)
friend class iDataBrowser;
public:

  TypeDef*		root_typ; // type of the root item
  MemberDef* 		root_md; // if the root item is a member, NULL otherwise
  
  virtual void*		root() {return NULL;} // subtype must supply
  inline TypeDef*	rootType() {return root_typ;}
  inline MemberDef*	rootMemb() {return root_md;}

  inline iBrowseViewer*	widget() {return (iBrowseViewer*)inherited::widget();}

  void 	Copy_(const BrowseViewer& cp);
  COPY_FUNS(BrowseViewer, FrameViewer) //
  TA_DATAVIEWFUNS(BrowseViewer, FrameViewer) //
protected:
  override void		Render_pre(); // 
private:
  void			Initialize();
  void			Destroy() {CutLinks();}
};

class TA_API tabBrowseViewer : public BrowseViewer {
  // #NO_TOKENS for browsing when root is a taBase
INHERITED(BrowseViewer)
friend class iDataBrowser;
public:
  static tabBrowseViewer*	New(TAPtr root, MemberDef* md = NULL);
    // convenience function

  taSmartRef		m_root; 
  
  override void*	root() {return (void*)m_root.ptr();} 
  
  void	UpdateAfterEdit(); // if root deletes, our window must die
  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const tabBrowseViewer& cp);
  COPY_FUNS(tabBrowseViewer, BrowseViewer) //
  TA_DATAVIEWFUNS(tabBrowseViewer, BrowseViewer) //
  
protected:
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // #IGNORE

  
private:
  void			Initialize() {}
  void			Destroy() {CutLinks();}
};

class TA_API ClassBrowseViewer : public BrowseViewer {
  // #NO_TOKENS represents a class browser instance
INHERITED(BrowseViewer)
public:
  static ClassBrowseViewer*	New(void* root, TypeDef* root_typ, MemberDef* root_md = NULL); 
    // accepts any TypeItem or XxxSpace guy

  String		root_str; // #READ_ONLY #NO_SHOW a string version, so we can stream the guy

  override void*	root() {return m_root;} 
  void			setRoot(void* root, TypeDef* root_typ, MemberDef* root_md = NULL); // use this to set the root
  
  void	UpdateAfterEdit();
  void 	Copy_(const ClassBrowseViewer& cp);
  COPY_FUNS(ClassBrowseViewer, BrowseViewer) //
  TA_DATAVIEWFUNS(ClassBrowseViewer, BrowseViewer) //
protected:
  void* 		m_root; // #IGNORE
  
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // #IGNORE
  void			StrToRoot();
  void			RootToStr();
private:
  void			Initialize();
  void			Destroy() {}
};


class TA_API PanelViewer : public FrameViewer {
  // #NO_TOKENS the base type for browser frames (tree of objects or classes)
INHERITED(FrameViewer)
friend class iDataPanel;
public:

  inline iTabViewer*	widget() {return (iTabViewer*)inherited::widget();}

  TA_DATAVIEWFUNS(PanelViewer, FrameViewer) //
protected:
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // #IGNORE
private:
  void			Initialize();
  void			Destroy() {CutLinks();}
};



class TA_API WindowState : public taOBase {
  // ##NO_TOKENS #INLINE #INLINE_DUMP Window geometry (position, size) saved in 1.0f-relative coordinates TODO: make this UserData
INHERITED(taOBase)
public:
  static float	Offs(float cur, float by); // offset cur by 'by' amount (0 > by >= 1.0); wraps if >1

  float	 	lft; 		// left (horizontal)
  float		top;  		// top (vertical) NOTE: was "bottom" in Iv version
  float		wd;		// width
  float		ht;		// height
  bool		iconified;	// window has been minimized
  
  TopLevelViewer*	topLevelViewer();

  void			GetWinState();	// get the window state from parent TopLevelViewer
  void			SetWinState();	// set the window state from parent TopLevelViewer
  void 			ScriptWinState(ostream& strm = cout);

  void	UpdateAfterEdit();
  void 	Copy_(const WindowState& cp);
  COPY_FUNS(WindowState, taOBase)
  TA_BASEFUNS(WindowState)
private:
  void 	Initialize();
  void 	Destroy() {}
};



class TA_API TopLevelViewer : public DataViewer {
  // #NO_TOKENS #VIRT_BASE stuff that is common to anything that can be a top-level window
INHERITED(DataViewer)
public:
    // can be provided to put msg up on closing
  override bool		deleteOnWinClose() const;
  bool			isIconified() const {return win_state.iconified;} 
    // 'true' if the window is iconified, n/a if not topLevel
    //TODO: make UserData
  bool			openOnLoad() const {return false;} 
    // 'true' if the viewer should be opened after loading (note: still must check if topLevel)
    // TODO: define impl somehow 
  virtual bool		isRoot() const {return false;} // only true for main proj window
  virtual bool		isTopLevel() const {return true;} // to differentiate, when it could be either
  WindowState&		winState(); // we get from UserData
  
  virtual void		ViewWindow();
    // #MENU #MENU_ON_Object either de-iconfiy if exists or create a new window if doesn't
  virtual void  	Iconify();		// #MENU iconify the window (saves iconified state)
  virtual void		DeIconify();		// deiconify the window (saves deiconified state)
  virtual void 		ScriptWinState() 		{ winState().ScriptWinState(cout); }
    // #NO_SCRIPT generate script code to position the window
  virtual void		SetWinName();		// #IGNORE set the window name 
  
  override void 	WindowClosing(CancelOp& cancel_op);
  
  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const TopLevelViewer& cp);
  COPY_FUNS(TopLevelViewer, DataViewer) //
  TA_DATAVIEWFUNS(TopLevelViewer, DataViewer) //
protected:
  WindowState		win_state; // TEMP: until is UserData
  String		win_name;
  
  override void 	GetWinState_impl();//TODO: we can eliminate these with UserData system
  override void 	SetWinState_impl();
  virtual void		MakeWinName_impl() {} // set win_name, impl in subs
  
private:
  void 	Initialize();
  void	Destroy() {CutLinks();}
};


class TA_API DockViewer : public TopLevelViewer {
  // #VIRT_BASE the controller for dock windows, which can float, or be in a MainWindow
INHERITED(TopLevelViewer)
public:
  enum DockViewerFlags { // #BITS controls behavior
    DV_NONE		= 0, // #NO_BIT
    DV_CLOSABLE		= 0x01,	// #BIT true if we are allowed to close it
    DV_MOVABLE		= 0x02,	// #BIT true if we are allowed to move it around
    DV_FLOATABLE	= 0x04	// #BIT true if we are allowed to undock it
#ifndef __MAKETA__
    ,DV_ALL		= DV_CLOSABLE | DV_MOVABLE | DV_FLOATABLE
#endif
  };
  
  DockViewerFlags	dock_flags; // #READ_ONLY #SHOW how this dock window is allowed to behave
  inline iDockViewer*	widget() {return (iDockViewer*)inherited::widget();}
  
  TA_DATAVIEWFUNS(DockViewer, TopLevelViewer) //
  
protected:
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); 
  //override void		MakeWinName_impl(); each subguy will need this
  
private:
  void 	Initialize();
  void	Destroy() {}
};


class TA_API DockViewer_List: public DataViewer_List { // #NO_TOKENS
INHERITED(DataViewer_List)
public:
  TA_DATAVIEWLISTFUNS(DockViewer_List, DataViewer_List, DockViewer)
private:
  void 	Initialize() { SetBaseType(&TA_DockViewer);}
  void	Destroy() {}
};


class TA_API ToolBoxDockViewer : public DockViewer {
  // floatable dockable toolbox window
INHERITED(DockViewer)
public:
  inline iToolBoxDockViewer*	widget() {return (iToolBoxDockViewer*)inherited::widget();}
  
  TA_DATAVIEWFUNS(ToolBoxDockViewer, DockViewer) //
  
protected:
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); //note: in _qt.h file
  //override void		MakeWinName_impl(); each subguy will need this
  
private:
  void 	Initialize();
  void	Destroy() {}
};



class TA_API ToolBar: public DataViewer {// ##NO_TOKENS proxy for Toolbars
friend class iToolBar;
//nn? friend class MainWindowViewer;
INHERITED(DataViewer)
public:
  float			lft;  	// #HIDDEN when undocked, fractional position on screen
  float			top;	// #HIDDEN when undocked, fractional position on screen
  Orientation		o; // whether hor or vert

  inline iToolBar*	widget() {return (iToolBar*)inherited::widget();} // #IGNORE lex override
  override iMainWindowViewer*	window();

  void	Copy_(const ToolBar& cp);
  COPY_FUNS(ToolBar, DataViewer)
  TA_DATAVIEWFUNS(ToolBar, DataViewer)

protected:
  override void		 Constr_impl(QWidget* gui_parent);
#ifdef TA_GUI
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // in qt file
#endif
  override void		WidgetDeleting_impl();
  override void 	GetWinState_impl();//TODO: we can eliminate these with UserData system
  override void 	SetWinState_impl();
private:
  void 	Initialize();
  void	Destroy() {}
};


class TA_API ToolBar_List: public DataViewer_List {
INHERITED(DataViewer_List)
public:
  TA_DATAVIEWLISTFUNS(ToolBar_List, DataViewer_List, ToolBar)
private:
  void			Initialize() {SetBaseType(&TA_ToolBar);}
  void			Destroy() {}
};


class TA_API MainWindowViewer : public TopLevelViewer {
  // #NO_TOKENS #VIRT_BASE the uber controller for main windows
INHERITED(TopLevelViewer)
friend class taDataLink;
friend class ToolBar_List;
friend class FrameView_List;
friend class DockView_List;
//friend class WindowState;
public:
  static TypeDef*	def_browser_type; // type of the default browser, us unless replaced
  static TypeDef*	def_viewer_type; // type of the default viewer, us unless replaced
  
  static MainWindowViewer* NewBrowser(TAPtr root, MemberDef* root_md = NULL, bool is_root = false); 
    // makes a standard 2-pane taBase browser
  static MainWindowViewer* NewClassBrowser(void* root, TypeDef* root_typ, MemberDef* root_md = NULL); 
    // convenience class: makes a 2-pane class browser (browse+panels)
  static MainWindowViewer* NewProjectBrowser(taProject* proj); 
    // makes a standard 3-pane project viewer

  bool			m_is_root; // #READ_ONLY #SAVE #NO_SHOW
  bool			m_is_proj_viewer; // #READ_ONLY #SAVE #NO_SHOW
  
#ifdef TA_GUI
  taiMenu_List*		ta_menus; // #IGNORE menu representations (from methods, non-menubuttons only)
  taiMenuBar*		menu; // #IGNORE menu bar -- note: partially managed by the window
  taiActions*		cur_menu; // #IGNORE for building menu
#endif
  ToolBar_List		toolbars;	// 
  FrameViewer_List 	frames;	// the frames shown in the center splitter area
  DockViewer_List	docks; // currently docked windows -- removed if they undock

  override bool		isRoot() const {return m_is_root;}
  inline bool		isProjViewer() const {return m_is_proj_viewer;}
//parent note: we inherit MainWindowViewer type, but actually never have a taDataView parent
  inline iMainWindowViewer* widget() {return (iMainWindowViewer*)inherited::widget();} 
  override iMainWindowViewer* window() {return (iMainWindowViewer*)inherited::widget();} 


  FrameViewer*		FindFrameByType(TypeDef* typ, int& at_index = no_idx, int from_index = 0); 
    // find the first frame and index of given type from the given starting index; 
  void 			AddFrame(FrameViewer* fv, int at_index);
    // add the supplied frame 
  FrameViewer*		AddFrameByType(TypeDef* typ, int at_index = -1);
    // add a new frame of given type at index (-1 at end); no window made yet
    
  ToolBar*		FindToolBarByType(TypeDef* typ, const String& tb_name); // finds existing toolbar by name and type; NULL if not found
  bool 			AddToolBar(ToolBar* tb); // add a new toolbar; true if added (won't add a duplicate)
  ToolBar*		AddToolBarByType(TypeDef* typ, const String& tb_name); // add a new toolbar by type; return inst if added (won't add a duplicate)
  
  override void 	ResolveChanges(CancelOp& cancel_op);
    
  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const MainWindowViewer& cp);
  COPY_FUNS(MainWindowViewer, TopLevelViewer)
  TA_DATAVIEWFUNS(MainWindowViewer, TopLevelViewer) //

public: // Action methods
/*  virtual void 	FileNewAction(){}
  virtual void 	FileOpenAction(){}
  virtual void 	FileSaveAction(){}
  virtual void 	FileSaveAsAction(){}
  virtual void 	FileSaveAllAction(){}
  virtual void 	FileCloseAction(){} */
  virtual void 	FileOptionsAction(); // 
  virtual void 	FilePrintAction(){}
  virtual void 	FileCloseAction(); // #ACT #MM_&File|&Close #MENUGP_LAST #MENU_GP_FileClose Quit Action(root) or Close Window Action(non-root)
  virtual void	EditUndoAction(); // #ACT 
  virtual void	EditRedoAction(); // #ACT 
  virtual void	EditCutAction(); // #ACT 
  virtual void	EditCopyAction(); // #ACT 
  virtual void	EditPasteAction(); // #ACT 
  virtual void	EditFindAction(); // #ACT 
  virtual void	ViewRefreshAction() {}  // #ACT rebuild/refresh the current view
  virtual void	HelpIndexAction(); // #ACT 
  virtual void	HelpContentsAction(); // #ACT 
  virtual void	HelpAboutAction() {} // #ACT 

protected:
  // from taDataView 
  override void		DataChanged_Child(TAPtr child, int dcr, void* op1, void* op2);
  override void		DoActionChildren_impl(DataViewAction act); // just one act
  override void		CloseWindow_impl(); 
  
  //from DataView
  override void		Constr_impl(QWidget* gui_parent); 
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); 
  override void		WidgetDeleting_impl();
  override void		Show_impl(); // only called if mapped; de-iconifies
  override void		Hide_impl(); // only called if mapped; iconifies
//nn  override void		ResolveChanges_impl(CancelOp& cancel_op);

  // from TopLevelView
  override void		MakeWinName_impl();
  
  
  virtual void		ConstrMainMenu_impl();
  virtual void		ConstrToolBars_impl();
  virtual void		ConstrFrames_impl();
  virtual void		ConstrDocks_impl();
  void 			OnToolBarAdded(ToolBar* tb, bool post_constr);
  
  
private:
  void 	Initialize();
  void	Destroy();
};




#endif
