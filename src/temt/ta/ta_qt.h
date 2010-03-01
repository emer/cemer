// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_qt.h: Basic global file for qt

#ifndef TA_QT_H
#define TA_QT_H

#include "ta_stdef.h"
#include "ta_base.h"

#include "ifont.h"
#include "igeometry.h"
#ifndef __MAKETA__
# include <QRect>
#endif

//nn? #include <unistd.h>

// external classes:
class taiData;
class taiEditDataHost;
class IDataViewWidget;
class iMainWindowViewer;
class iDockViewer;
class iNetworkAccessManager;	// #IGNORE

typedef taPtrList<QWidget> 	Widget_List;  // #IGNORE list of widgets

class taiHostDialog_List;		// #IGNORE
typedef taPtrList<taiEditDataHost> taiEditDataHost_List; // #IGNORE

//////////////////////////////
//       taiMisc          //
//////////////////////////////

class TA_API iTopLevelWindow_List: public taPtrList<IDataViewWidget> {
public:
  iMainWindowViewer*	SafeElAsMainWindow(int i); 
    // returns item cast as requested, or NULL if not of that type
  iMainWindowViewer*	FindMainWindowById(int id); 
    // find the main window with indicated uniqueId NULL if not of that type
  iDockViewer*		SafeElAsDockWindow(int i);
    // returns item cast as requested, or NULL if not of that type
  iMainWindowViewer*	Peek_MainWindow(); // finds the topmost main window, NULL if none
  iDockViewer*		Peek_DockWindow(); // finds the topmost undocked dock win, NULL if none
  void			GotFocus_MainWindow(iMainWindowViewer* imw); // puts to top of list
  void			GotFocus_DockWindow(iDockViewer* idv); // puts to top of list
};

class TA_API taiMisc: public taiMiscCore { 
INHERITED(taiMiscCore)
 //  miscellaneous stuff for tai
  Q_OBJECT
public:
  //NOTE: all xxSize values use the ls 4 bits of every enum -- this enables
  // any size enum to be passed to the metric functions that take size specs
  enum SizeSpec {
    defSize		= 0x000, //medium
    sizSmall		= 0x001,
    sizMedium		= 0x002,
    sizBig		= 0x003,

    // mask
    siz_mask		= 0x00F
  };

  enum FontSpecEnum {
    //choose 1:
    defFontSize		= 0x000, // medium, or inherit from parent (as applicable)
    fonSmall		= 0x001,
    fonMedium		= 0x002,
    fonBig		= 0x003,

    // choose up to 1:
    def_stretch		= 0x000, // default (not stretched)
    fonSkinny		= 0x010,
    fonWide		= 0x030,

    // choose any:
    fonItalic		= 0x100,
    fonBold		= 0x200,
    fonUnderline	= 0x300,

    // pre-baked combinations (some for legacy compatibility)
    fonBig_italic		= 0x103,
    fonSkinny_small	= 0x011,

    // masks
    fonSize_mask	= 0x00F,
    fonStretch_mask	= 0x0F0
  };

  enum DialogSpec {
    //choose 1:
    defDialogSize	= 0x000, // default is medium
    dlgMini		= 0x002,
    dlgSmall		= 0x004,
    dlgMedium		= 0x006,
    dlgBig		= 0x008,

    // choose 1:
    dlgHor		= 0x000, // the default -- Hor == ~Ver
    dlgVer		= 0x010,

    // prebaked:
    hdlg		= 0x000, // default h dialog
    vdlg		= 0x010, // default v dialog
    hdlg_s		= 0x004,
    vdlg_s		= 0x014,
    hdlg_m		= 0x006,
    vdlg_m		= 0x016,
    hdlg_b		= 0x008,
    vdlg_b		= 0x018,

    // masks
    dlgSize_mask	= 0x00F,
    dlgOrient_mask	= 0x010

  };
  
  static const int	FONT_MED; // #DEF_1 pts to subtract from BIG
  static const int	FONT_SM;  // #DEF_2 pts to subtract frm BIG


  static taiHostDialog_List	active_dialogs;	// #IGNORE list of active (NoBlock) dialogs
  static taiEditDataHost_List	active_edits;	// #IGNORE list of active edit dialogs
  static taiEditDataHost_List	css_active_edits; // #IGNORE list of css active edit dialogs (note: prev in cssiSession)
  static iTopLevelWindow_List	active_wins; // #IGNORE currently open windows
  static iNetworkAccessManager* net_access_mgr; // #IGNORE network access manager for web browser

  static int			busy_count; // levels of busy

  static TypeSpace		arg_types; 	// list of all taiArgTypes
  static void (*Update_Hook)(taBase*);
  // #IGNORE called after apply in a dialog, etc. obj is the object that was edited
#ifndef __MAKETA__
  static QPointer<iMainWindowViewer> main_window; // the main window (dialogs are parented to this)

#endif
  static taBase_PtrList		unopened_windows;
  // #HIDDEN unopened windows waiting to be opened
  
  static void			GetWindowList(Widget_List& rval);
    // returns, as widgets, all "top level" windows, for use in a Windows menu; you can only use these transiently, i.e. in an on-demand popup
  static taiMisc* 		New(bool gui, QObject* parent = NULL);
   // #IGNORE initialize Qt interface system -- launch main window if gui; 
  
  static void   OpenWindows(); //  open all unopened windows
  static void 	WaitProc();	// waiting process function
  // update menus relevant to the given object, which might have changed
  static void	ScriptIconify(void* obj, int onoff); // record iconify command for obj to script
//obs  static int	SetIconify(void* obj, int onoff); // set iconified field of winbase obj to onoff


  static void	Update(taBase* obj); // #IGNORE update stuff after modification (uses hook fun)

  static void	PurgeDialogs();
  // remove any 'NoBlock' dialogs from active list (& delete them)

//obs  static bool	RevertEdits(void* obj, TypeDef* td);
  // revert any open edit dialogs for given object
//  static bool	ReShowEdits(void* obj, TypeDef* td, bool force = true);
  // rebuilds any open edit dialogs for object; if force=true, doesn't prompt user if changes, just does it
  static taiEditDataHost* FindEdit(void* base, iMainWindowViewer* not_in_win = NULL);
  // find first active edit dialog or panel for this object; for panels, if not_in_win specified, then must be active in a tab (not buried) in some win other than specified
  static taiEditDataHost* FindEditDialog(void* base, bool read_only);
  // find an active (non-modal) edit dialog with same read_only state for the object
  static taiEditDataHost* FindEditPanel(void* base, bool read_only,
    iMainWindowViewer* not_in_win = NULL);

  static void	Cleanup(int err); // #IGNORE function to be called upon exit to clean stuff up
  
  void		InitMetrics(bool reinit=false);	// initializes all the sizes/fonts/etc. -- QApplication object must be created
  void 		AdjustFont(int fontSpec, iFont& font); // sets the font according to the spec parameter
  void		ResolveEditChanges(CancelOp& cancel_op); // resolve all changes on ALL edits panels and dialogs
  void		ResolveViewerChanges(CancelOp& cancel_op); // resolve all changes on ALL top level viewers
  override void Busy_(bool busy);// impl for taMisc, puts system in a 'busy' state (pointer, no input)
  override void CheckConfigResult_(bool ok);


  static bool	KeyEventCtrlPressed(QKeyEvent* e);
  // #IGNORE process given event to see if the ctrl key was pressed -- uses MetaModifier on Mac = actual Ctrl key..
  static bool	KeyEventFilterEmacs_Nav(QObject* obj, QKeyEvent* e);
  // #IGNORE translate emacs navigation key sequences into equivalent arrow events and re-post as new events -- returns true if procssed, otherwise false
  static bool	KeyEventFilterEmacs_Edit(QObject* obj, QKeyEvent* e);
  // #IGNORE translate emacs editing (includes nav + copy/paste, undo) key sequences into equivalent arrow events and re-post as new events -- returns true if procssed, otherwise false
  static bool	KeyEventFilterEmacs_Clip(QObject* obj, QKeyEvent* e);
  // #IGNORE translate emacs copy/paste/undo only (no nav) key sequences into equivalent arrow events and re-post as new events -- returns true if procssed, otherwise false

protected:
  static void	SetWinCursors();
  // #IGNORE sets cursors for all active windows based on busy and record status
  static void	RestoreWinCursors();
  // #IGNORE restores cursors to previous state -- Set/Restore always called in pairs
  static void	ScriptRecordingGui_(bool start); // callback from taMisc
  
  
public:
  QProgressDialog*	load_dlg;       // #IGNORE load dialog

/*nn  void		CreateLoadDialog(const char* caption = NULL);
  void		SetLoadDialog(char* tpname, int totalSteps = 0); // use 0 when Step won't be called
  void		StepLoadDialog(int stepNum); // (optionally) call with 1..totalSteps
//  static bool	IsLoadDialogCancelled(); //optional call, to check if user has cancelled
  void		RemoveLoadDialog(); */

protected slots:
  void		LoadDialogDestroyed();

  void 		desktopWidget_resized(int screen);
  void 		desktopWidget_workAreaResized(int screen);

#ifdef __MAKETA__
};
#else
public:
  int		ctrl_size;	//  size for controls etc. -- default is sizMedium
  // metric properties -- initialized in InitMetrics()
  int		hsep_c;		// separator values (in pixels) are for "small spaces" between items
  int		vsep_c;

  int		hspc_c;		// space values (in pixels) are for "large spaces" between items
  int		vspc_c;
  int		dlgm_c;		// dialog margin (in pixels) -- empty space around dialog contents
  iSize		scrn_s;		// screen size, excluding task bars, etc. (use for maximum dialog size)
  iSize		frame_s;	// normal window frame size -- we can only determine this from a shown window (esp on X) -- the startup routine sets it from the main window
  QRect		scrn_geom;	// #IGNORE available space on main screen (esp useful for evil Mac...)
  QCursor*	wait_cursor;	// cursor used for waiting
  QCursor*	record_cursor;	// cursor used for recording
  QBitmap*	icon_bitmap;	// icon bitmap
  float 	edit_darkbg_brightness;
  float 	edit_lightbg_brightness;
  
  iSize		dialogSize(int dialogSpec); // return starting dialog size for given spec

  iFont		buttonFont(int fontSpec); // get a button font
  iFont		dialogFont(int fontSpec = 0); // get a dialog font
  iFont		menuFont(int fontSpec);  // get a menu font
  iFont		nameFont(int fontSpec);  // get a name (label) font

  // control height metrics
  int		button_height(int sizeSpec); // can pass any XxxSpec that uses size
  int		combo_height(int sizeSpec); // for QComboBox
  int		label_height(int sizeSpec); // for QLabel, QCheckBox, QRadioButton
  int		text_height(int sizeSpec); // can pass any XxxSpec that uses size -- for: QLineEdit, QSpinBox,
  int		max_control_height(int sizeSpec); // maximum height needed
  int		maxButtonWidth(int sizeSpec = 0) const; // maximum width allowed

  QLabel*	NewLabel(const String& text, QWidget* parent = NULL, int fontSpec = 0);
  // convenience, for making a label with indicated fontspec
  void		FormatButton(QAbstractButton* but, const String& text,
			     int font_spec = 0); // sets max width, text, and adds a tooltip
    
  static  iColor ivBrightness_to_Qt_lightdark(const QColor& qtColor, float ivBrightness);
  // applies a legacy IV brightness factor to a Qt Color
  String 	color_to_string(const iColor& color); 
  // returns a string value (appropriate for setting in a style) for the color

  taiMisc(QObject* parent = NULL);
  ~taiMisc();
protected:
  int		base_height;	// implementation defined base control height used to derive other heights
  iFont 	mbig_button_font; // largest button font -- others are derived from this
  iFont		mbig_dialog_font;
  iFont 	mbig_menu_font; // largest menu font -- others are derived from this
  iFont		mbig_name_font; // for labels
  const char*	mclassname;
  signed char	mbutton_ht[3]; // for s/m/b
  signed char	mlabel_ht[3];  // for s/m/b
  signed char	mtext_ht[3];  // for s/m/b
  short int	max_button_width;

  override void		Init(bool gui); // NOTE: called from static New
  override int		Exec_impl();
  override void 	OnQuitting_impl(CancelOp& cancel_op); // pre-quit resolves changes
  override void		Quit_impl(CancelOp cancel_op);

  void		HandleScreenGeomChange(); // reinit, and make sure all wins visible
  void		HandleScreenGeomChange_Window(const QRect& old_scrn_geom, QWidget* win); // handle a change for the window
};

#endif

extern TA_API taiMisc* taiM_; // use taiM macro instead


#endif // TA_QT_H
