/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// ta_qt.h: Basic global file for qt

#ifndef TA_QT_H
#define TA_QT_H

#include "ta_stdef.h"
#include "ta_base.h"

#include "ifont.h"
#include "igeometry.h"

//nn? #include <unistd.h>

/*obs#if (!(defined(WINDOWS) || defined(CYGWIN)))
#include <X11/X.h>
struct _XDisplay;		// #IGNORE
typedef unsigned long XWindow;		// #IGNORE
#endif*/

// external classes:
class taiData;
class taiEditDataHost;
class iDataViewer;
class Dialog;

typedef taPtrList<QWidget> 	Window_List;  // #IGNORE list of windows

typedef taPtrList<QWidget> 	Widget_List;  // #IGNORE list of widgets

class taiDialog_List;		// #IGNORE
typedef taPtrList<taiEditDataHost> taiEditDataHost_List; // #IGNORE
typedef taPtrList<iDataViewer> 	iDataViewer_PtrList; // #IGNORE


//////////////////////////////
//       taiMisc          //
//////////////////////////////


class taiMisc: public QObject { // #NO_TOKENS #INSTANCE miscellaneous stuff for tai
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

  enum FontSpec {
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
    dlgSmall		= 0x001,
    dlgMedium		= 0x002,
    dlgBig		= 0x003,

    // choose 1:
    dlgHor		= 0x000, // the default -- Hor == ~Ver
    dlgVer		= 0x010,

    // prebaked:
    hdlg		= 0x000, // default h dialog
    vdlg		= 0x010, // default v dialog
    hdlg_s		= 0x001,
    vdlg_s		= 0x011,
    hdlg_m		= 0x002,
    vdlg_m		= 0x012,
    hdlg_b		= 0x003,
    vdlg_b		= 0x013,

    // masks
    dlgSize_mask	= 0x00F,
    dlgOrient_mask	= 0x010

  };

  static bool			gui_active;
  // #READ_ONLY 'true' when we have a gui (mainwindow), false if not (ex. after main win closes)

  static Window_List		active_wins; 	// #IGNORE list of active windows, dynamically ordered from least-recently active (idx=0) to currently active (idx=max)
//obs  static Window_List		delete_wins; 	// #IGONRE list of windows to delete (delayed)
  static taiDialog_List		active_dialogs;	// #IGNORE list of active (NoBlock) dialogs
  static taiEditDataHost_List	active_edits;	// #IGNORE list of active edit dialogs
  static taiEditDataHost_List	css_active_edits; // #IGNORE list of css active edit dialogs (note: prev in cssiSession)
  static iDataViewer_PtrList	viewer_wins; // #IGNORE currently open viewer windows

  static int			busy_count; // levels of busy

  static TypeSpace		arg_types; 	// list of all taiArgTypes
  static void (*Update_Hook)(TAPtr);
  // #IGNORE called after apply in a dialog, etc. obj is the object that was edited

  static QWidget*		main_window; // the main window

  static taBase_PtrList		unopened_windows;
  // #HIDDEN unopened windows waiting to be opened
  static void   OpenWindows(); //  open all unopened windows
  static int 	WaitProc();	// waiting process function
  // update menus relevant to the given object, which might have changed
  static void	ScriptIconify(void* obj, int onoff); // record iconify command for obj to script
  static int	SetIconify(void* obj, int onoff); // set iconified field of winbase obj to onoff

  static void	Initialize(bool gui, const char* classname_ = "");	// #IGNORE initialize Qt interface system -- launch main window if gui

  static void	Update(TAPtr obj); // #IGNORE update stuff after modification (uses hook fun)

  static void	DoneBusy_impl();// #IGNORE implements the done busy function

//  static void	SetWinCursor(iWindow* win);
  // #IGNORE sets cursor for given window based on busy and record status
  static int	RunPending();	// run any pending qt events that might need processed

  static void	PurgeDialogs();
  // remove any 'NoBlock' dialogs from active list (& delete them)

  static bool	RevertEdits(void* obj, TypeDef* td);
  // revert any open edit dialogs for given object

  static bool	CloseEdits(void* obj, TypeDef* td);
  // close any open edit dialogs for object or sub-objs

  static bool	NotifyEdits(void* obj, TypeDef* td);
  // notifies any open edit dialogs for given object that a change in its data has occured

  static bool	ReShowEdits(void* obj, TypeDef* td, bool force = true);
  // rebuilds any open edit dialogs for object; if force=true, doesn't prompt user if changes, just does it

  static taiEditDataHost* FindEdit(void* obj, TypeDef* td, iDataViewer* not_in_win = NULL);
  // find first active edit dialog or panel for this object; for panels, if not_in_win specified, then must be active in a tab (not buried) in some win other than specified

  static void SetMainWindow(QWidget* win); // #IGNORE called by whomever creates the main window

  static void	Cleanup(int err); // #IGNORE function to be called upon exit to clean stuff up
  void		InitMetrics();	// initializes all the sizes/fonts/etc. -- QApplication object must be created
  void 		AdjustFont(int fontSpec, iFont& font); // sets the font according to the spec parameter
protected:
  static void	SetWinCursors();
  // #IGNORE sets cursors for all active windows based on busy and record status
  static void	RestoreWinCursors();
  // #IGNORE restores cursors to previous state -- Set/Restore always called in pairs
  static void 	Busy_(bool busy);		// callback from taMisc, puts system in a 'busy' state (pointer, no input)
  static void	ScriptRecordingGui_(bool start); // callback from taMisc
  static void	DelayedMenuUpdate_(TAPtr obj);
  // add object to list to be updated later (by Wait_UpdateMenus)
public:
  QProgressDialog*	load_dlg;       // #IGNORE load dialog

/*nn  void		CreateLoadDialog(const char* caption = NULL);
  void		SetLoadDialog(char* tpname, int totalSteps = 0); // use 0 when Step won't be called
  void		StepLoadDialog(int stepNum); // (optionally) call with 1..totalSteps
//  static bool	IsLoadDialogCancelled(); //optional call, to check if user has cancelled
  void		RemoveLoadDialog(); */

protected slots:
  void		MainWindowDestroyed();
  void		LoadDialogDestroyed();

#ifdef __MAKETA__
};
#else
protected:
  int		base_height;	// implementation defined base control height used to derive other heights
  iFont 	mbig_button_font; // largest button font -- others are derived from this
  iFont		mbig_dialog_font;
  iFont 	mbig_menu_font; // largest menu font -- others are derived from this
  iFont		mbig_name_font; // for labels
  const char*	mclassname;

public:
  int		ctrl_size;	//  size for controls etc. -- default is sizMedium
  // metric properties -- initialized in InitMetrics()
  int		hsep_c;		// separator values (in pixels) are for "small spaces" between items
  int		vsep_c;

  int		hspc_c;		// space values (in pixels) are for "large spaces" between items
  int		vspc_c;
  int		dlgm_c;		// dialog margin (in pixels) -- empty space around dialog contents
  iSize		scrn_s;		// screen size, excluding task bars, etc. (use for maximum dialog size)
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

/* TODO:
  ivCoord  	vsep_c;		// separators are for "small spaces" between items
  ivCoord 	hsep_c;
  QWidget*	vsep;
  QWidget*	hsep;
  QWidget*	vfsep;		// fixed versions
  QWidget*	hfsep;

  ivCoord	vspc_c;		// spaces are for "large spaces" between items
  ivCoord	hspc_c;
  QWidget*	vspc;
  QWidget*	hspc;
  QWidget*	vfspc;		// fixed (non stretchable) versions
  QWidget*	hfspc;

  // make a fixed size button

  QWidget*	small_button(QWidget* b);
  QWidget*	medium_button(QWidget* b);
  QWidget*	big_button(QWidget* b);

  // make a natural size flexible button

  QWidget*	small_flex_button(QWidget* b);
  QWidget*	medium_flex_button(QWidget* b);
  QWidget*	big_flex_button(QWidget* b);


*/
//OBS  const iColor* font_foreground;
  const char* 	classname() const {return mclassname;} // replaces the Iv:session->instance()->classname()
  float 	edit_darkbg_brightness;
  float 	edit_lightbg_brightness;
static  iColor	ivBrightness_to_Qt_lightdark(const QColor& qtColor, float ivBrightness); // applies a legacy IV brightness factor to a Qt Color
//OBS  const iColor* edit_darkbg;	// edit dialog darker background color
//OBS  const iColor* edit_lightbg;	// edit dialog lighter background color (for highlighting)
  String 	color_to_string(const iColor& color); // returns a string value (appropriate for setting in a style) for the color
//OBS:  String 	color_to_string(const ivColor* clr); // returns a string value (appropriate for setting in a style) for the color


//  int		small_button_width;
//  int		medium_button_width;
//  int		big_button_width;

  QCursor*	wait_cursor;	// cursor used for waiting
  QCursor*	record_cursor;	// cursor used for recording
  QBitmap*	icon_bitmap;	// icon bitmap

//OBS  int GetButton(const ivEvent& e); // applies keyboard mods
  void		MainWindowClosing(bool& cancel); // called by main_window in close event -- we can cancel it

/*TODO: rewrite for Qt #if (!(defined(WINDOWS) || defined(CYGWIN)))
  static void DumpJpeg(_XDisplay* dpy, XWindow win, const char* fnm, int quality=85, int xstart=0, int ystart=0, int width=-1, int height=-1);
  // dump window to jpeg file
  static void DumpJpegIv(iWindow* win, const char* fnm, int quality=85, int xstart=0, int ystart=0, int width=-1, int height=-1);
  // dump interviews window to jpeg file
  static void DumpTiff(_XDisplay* dpy, XWindow win, const char* fnm, int xstart=0, int ystart=0, int width=-1, int height=-1);
  // dump window to tiff file
  static void DumpTiffIv(iWindow* win, const char* fnm, int xstart=0, int ystart=0, int width=-1, int height=-1);
  // dump interviews window to tiff file
#endif */

  taiMisc(const char* classname_);
  taiMisc(); // #IGNORE for ta_TA only
  ~taiMisc();
protected:
  void init(); // #IGNORE
};
#endif

extern taiMisc* taiM;	// this is an instance to use for all seasons..


#endif // TA_QT_H
