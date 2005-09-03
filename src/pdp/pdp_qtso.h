/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
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

// pdp_qt.h : Qt and Inventor stuff for pdp

#ifndef pdp_qt_h
#define pdp_qt_h 1

#include "pdpbase.h"
#include "spec.h"
#include "ta_qttype.h"
#include "ta_qtdata.h"
#include "ta_qtdialog.h"
//#include "array_qt.h"
#include "t3viewer.h"
#include "pdp_TA_type.h"


// forwards this file
class pdpDataViewer;

class ProcessDialog : public taiEditDataHost {
  // #IGNORE
  Q_OBJECT
public:
  ProcessDialog(void* base, TypeDef* tp, bool readonly = false)
    : taiEditDataHost(base, tp, readonly) { };
  ProcessDialog()				{ };
  ~ProcessDialog();

  virtual bool	CtrlPanel()	{ return false; }

public slots:
  void		Ok();		// override - stop the process when these are hit..
  void		Cancel(); // override
};

class Process_RunDlg : public ProcessDialog {
  // #IGNORE
public:
  Process_RunDlg(void* base, TypeDef* tp, bool readonly = false)
    : ProcessDialog(base, tp, readonly) {};
  Process_RunDlg() {};
  ~Process_RunDlg();

  bool	CtrlPanel()	{ return true; }

  bool ShowMember(MemberDef* md);
};

class taiProcess : public taiEdit {
  // adds control buttons to the edit window
public:
  taiProcess*	run_ie;

  int		BidForEdit(TypeDef* td);
  override int 	Edit(void* base, bool readonly = false, const iColor* bgclr = NULL); // unusual case, so we override

  void Initialize();
  void Destroy();
  TAQT_EDIT_INSTANCE(taiProcess, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


class taiProcessRunBox : public taiProcess {
  // just has the control buttons
public:
  int		BidForEdit(TypeDef*) 	{ return 0; }
  TAQT_EDIT_INSTANCE(taiProcessRunBox, taiProcess);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};

class taiSpecMember : public taiMember {
  // special for the spec type member (adds the unique box)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  void		CmpOrigVal(taiData* dat, void* base, bool& first_diff);

  virtual bool	NoCheckBox(taiDataHost* host_); // deterimine if check box should be used

  TAQT_MEMBER_INSTANCE(taiSpecMember, taiMember);
protected:
  override taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void		GetImage_impl(taiData* dat, void* base);
};


//////////////////////////
//   ipdpDataViewer	//
//////////////////////////

class ipdpDataViewer: public iT3DataViewer { //#IGNORE
friend class pdpDataViewer;
public:

  pdpDataViewer*	viewer() {return (pdpDataViewer*)m_viewer;}

  ipdpDataViewer(void* root_, TypeDef* typ_, pdpDataViewer* viewer_,
      QWidget* parent = NULL);
  ~ipdpDataViewer();

protected:
//  override void 	SetActionsEnabled_impl();
};

//////////////////////////
//   pdpDataViewer	//
//////////////////////////

class pdpDataViewer: public T3DataViewer { // master viewer for network, i/o, etc.
#ifndef __MAKETA__
  typedef pdpDataViewer inherited;
#endif
public:
  static pdpDataViewer*	New(Project* proj); // populates all elements (all layers, prjns, i/o, etc.)

  Project*		proj() {return (Project*)m_data;} // aka data
  ipdpDataViewer*	net_win() {return (ipdpDataViewer*)m_window;}

  TA_BASEFUNS(pdpDataViewer)
protected:
  override void		Constr_Window_impl();  // #IGNORE implement this to set the m_window instance
private:
  void		Initialize();
  void		Destroy();
};



////////////////////////////////////////////////////////////////////////////////////////
/*TODO
//////////////////////////////////
//     Project Viewer!		//
//////////////////////////////////

#include <iv_graphic/graphic.h>
#include <iv_graphic/graphic_viewer.h>
#include <iv_graphic/graphic_text.h>

class ivColor;		// #IGNORE
class ivStyle;		// #IGNORE
class ivTransformer;	// #IGNORE
class GlyphViewer;	// #IGNORE
class ivWindow;		// #IGNORE
class ivTelltaleGroup;	// #IGNORE
class ivButton;		// #IGNORE
class ivAction;		// #IGNORE
class ivDialogKit;	// #IGNORE
class ivScrollBox;	// #IGNORE
class ivPatch;		// #IGNORE
class ivFont;		// #IGNORE
class ivPolyGlyph;	// #IGNORE

class Process_Group;
class Stat_Group;

// predeclarations of all types:

class Project_G;
class ProjObj_G;
class ProcHier_G;
class SchedProc_G;
class SProcGroup_G;
class StatGroup_G;
class ProcObj_G;
class GroupObj_G;
class SpecObj_G;
class ProjViewer;
class ProjEditor;
class ProjViewState;

class Project_G : public GraphicMaster {
public:
  Project*	proj;
  ProjEditor*	owner;
  ivColor*	border;	        // 'unselected' border color
  const ivBrush* defbrush;	// default brush
  const ivBrush* widebrush;	// wide brush for connection lines
  const ivBrush* widedashbrush;	// wide, dashed for updater lines
  FloatTwoDCoord max_size;	// for the whole Project (sets scaling)
  FloatTwoDCoord obj_size;	// size of each object
  bool		reinit_display;	// subobjs set this flag to reinit after popping out
  ivTransformer* tx;
  FloatTwoDCoord span;		// size in coords of display

  virtual void	Reset();
  virtual void	Build();
  virtual void	SpecBuild(BaseSpec* spec, int xcol, float cx, float& cy);

  virtual ProjObj_G* FindObject(TAPtr ob); // find graphic for given object
  virtual ProcObj_G* FindProcess(Process* ob, bool sel_only = false);
  // find graphic for given object, optionally only the selected one
  virtual ProcHier_G* FindProcHier(SchedProcess* ob, bool sel_only = false);
  // find process hierarchy object

  bool		update_from_state(ivCanvas* c);

  bool 		select(const ivEvent& e, Tool& tool, bool);
  bool		effect_select(bool set_select);

  void 		allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext);
  void 		ReCenter();
  void 		ScaleCenter(const ivAllocation& a);

  Project_G(Project*);
  ~Project_G();
  GRAPHIC_BASEFUNS(Project_G);
};

class ProjObj_G : public Rectangle {
  // generic project object (network, environment, log)
public:
  TAPtr			obj;	//  the object
  Project_G*		projg;
  bool			draw_links; // draw the links for this object to others?
  bool			editb_used; // edit button (RMB) used in selecting
  bool			extendb_used; // extend-select (MMB) used in selecting
  int			sel_count;  // number of times selected
  bool			sel_target; // true if this object was actual object of selection
  int			cur_pos; 	// current position (index)
  int			move_pos; // moving offset
  int			max_pos; // maximum move amount
  float			move_incr; // how much one move is

  void 		draw_gs(ivCanvas* c, Graphic* gs); // overall draw routine
  virtual void	draw_label(ivCanvas* c, Graphic* gs); // draws just the label
  virtual void	draw_the_links(ivCanvas* c, Graphic* gs); // draws the links

  virtual void 	render_text(ivCanvas* c, const char* chr, float x, float y);
  virtual void	draw_line_left(ProjObj_G* og, float offset, ivColor* clr, const ivBrush* br,
			       ivCanvas* c, Graphic* gs);
  // draw line to object on my left
  virtual void	draw_line_right(ProjObj_G* og, float offset, ivColor* clr, const ivBrush* br,
				ivCanvas* c, Graphic* gs);
  // draw line to object on my right

  bool  	select(const ivEvent& e, Tool& tool, bool unselect);
  bool		effect_select(bool set_select);

  bool		graspable();

  bool	grasp_move(const ivEvent& e,Tool& tool,ivCoord, ivCoord);
  bool	manip_move(const ivEvent& e,Tool& tool,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
		   ivCoord cx, ivCoord cy);
  bool 	effect_move(const ivEvent& ev, Tool& tl, ivCoord ix,
		    ivCoord iy, ivCoord fx, ivCoord fy);

  virtual ProjViewState* GetMyViewState();

  ivGlyph*	clone() const;
  ProjObj_G(TAPtr ob, Project_G* ng);
  virtual ~ProjObj_G();
  GRAPHIC_BASEFUNS(ProjObj_G);
};

class ProcHier_G : public PolyGraphic {
  // entire process hierarchy
public:
  SchedProcess*		top;		// top of the hierarchy
  Project_G*		projg;
  bool			iconify; 	// if true, don't show entire hierarchy
  FloatTwoDCoord 	obj_size;	// size of entire group of objs

  bool		selectable() { return false; }
  bool		graspable() { return false; }

  virtual ProcObj_G* FindProcess(Process* ob, bool sel_only = false);
  // find graphic for given object, optionally only the selected one

  virtual void	Build();
  virtual ProjViewState* GetMyViewState();

  ivGlyph*	clone() const;
  ProcHier_G(SchedProcess* ob, Project_G* ng);
  virtual ~ProcHier_G();
  GRAPHIC_BASEFUNS(ProcHier_G);
};

class SchedProc_G : public PolyGraphic {
  // a sched process object, has sub-objects that are processes
public:
  SchedProcess*		sproc;		// process
  ProcHier_G*		hierg;
  Project_G*		projg;
  bool			iconify; 	// if true, don't show entire set of objs
  FloatTwoDCoord 	obj_size;	// size of entire group of objs

  bool		selectable() { return false; }
  bool		graspable() { return false; }

  virtual ProcObj_G* FindProcess(Process* ob, bool sel_only = false);
  // find graphic for given object, optionally only the selected one

  virtual void	Build();	// create all the sub-procs
  virtual ProjViewState* GetMyViewState();

  ivGlyph*	clone() const;
  SchedProc_G(SchedProcess* ob, ProcHier_G* hg, Project_G* pg);
  virtual ~SchedProc_G();
  GRAPHIC_BASEFUNS(SchedProc_G);
};

class SProcGroup_G : public ProjObj_G {
  // group of sub-processes in a sched proc
public:
  enum WhichGroup { INIT, LOOP, FINAL };

  WhichGroup	mygp;		// which group this is
  Process_Group* pgp;		// process group
  SchedProc_G*	spg;		// our parent


  bool		graspable() { return false; }

  static Process_Group* GetGroup(SchedProcess* sp, WhichGroup gp);
  static const char* GetLabel(WhichGroup gp);

  void 		draw_label(ivCanvas* c, Graphic* gs);

  ivGlyph*	clone() const;
  SProcGroup_G(WhichGroup gp, SchedProc_G* sg, Project_G* pg);
  virtual ~SProcGroup_G();
  GRAPHIC_BASEFUNS(SProcGroup_G);
};

class StatGroup_G : public ProjObj_G {
  // group of stats in a sched proc
public:
  enum WhichGroup { LOOP, FINAL };

  WhichGroup	mygp;		// which group this is
  Stat_Group* 	sgp;		// process group
  SchedProc_G*	spg;		// our parent

  bool		graspable() { return false; }

  static Stat_Group* GetGroup(SchedProcess* sp, WhichGroup gp);
  static const char* GetLabel(WhichGroup gp);

  void 		draw_label(ivCanvas* c, Graphic* gs);

  ivGlyph*	clone() const;
  StatGroup_G(WhichGroup gp, SchedProc_G* sg, Project_G* pg);
  virtual ~StatGroup_G();
  GRAPHIC_BASEFUNS(SProcGroup_G);
};

class ProcObj_G : public ProjObj_G {
  // a process object (incl stat), has pointers to network and environment
public:
  Process*	proc;		// obj = process
  SchedProc_G*	spg;
  bool		sched_proc;	// this is a sched proc to represent entire group of assoc procs

  bool		grasp_move(const ivEvent& e,Tool& tool,ivCoord, ivCoord);
  void 		draw_the_links(ivCanvas* c, Graphic* gs);

  ivGlyph*	clone() const;
  ProcObj_G(Process* ob, SchedProc_G* sg, Project_G* pg);
  virtual ~ProcObj_G();
  GRAPHIC_BASEFUNS(ProcObj_G);
};

class GroupObj_G : public ProjObj_G {
  // a group of any type of object
public:
  taGroup_impl*	group;

  void		draw_label(ivCanvas* c, Graphic* gs);

  ivGlyph*	clone() const;
  GroupObj_G(taGroup_impl* gp, Project_G* pg);
  virtual ~GroupObj_G();
  GRAPHIC_BASEFUNS(GroupObj_G);
};

class SpecObj_G : public ProjObj_G {
  // a specification object, for spec mode
public:
  BaseSpec*	spec;


  ivGlyph*	clone() const;
  SpecObj_G(BaseSpec* sp, Project_G* pg);
  virtual ~SpecObj_G();
  GRAPHIC_BASEFUNS(SpecObj_G);
};

class ProjViewer : public GlyphViewer {
  // ##NO_INSTANCE ##NO_MEMBERS ##NO_TOKENS ##NO_CSS viewer for Project
public:
  Project* 	proj;
  void 		init_graphic() 		{ initgraphic();}
  void 		allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext);
  ProjViewer(Project* p, float w, float h, const ivColor* bg);
};

class ivGlyph;			// #IGNORE
class tbScrollBox;		// #IGNORE
class ivPolyGlyph;		// #IGNORE
class ivPatch;			// #IGNORE
class PScaleBar;		// #IGNORE
class ColorScale;		// #IGNORE
class ivButton;			// #IGNORE
class HiLightButton;		// #IGNORE
class taiMenu;			// #IGNORE
class ivInputHandler;		// #IGNORE
class ivDeck;			// #IGNORE

class ProjEditor {
  // ##NO_INSTANCE ##NO_MEMBERS ##NO_TOKENS ##NO_CSS editor for Project
public:
  enum IconifyActions {
    NO_ICONIFY, ICONIFY_ALL, ICONIFY, DE_ICONIFY, VIEW, DUP_SPEC, ICONIFY_COUNT
  };
  enum LinksActions {
    NO_LINKS, SHOW_LINKS, NOSHOW_LINKS, SHOW_AGGS, NOSHOW_AGGS, LINKS_COUNT
  };
  enum EditActions {
    NO_EDIT, EDIT_OBJ, EDIT_COUNT
  };
  enum UpdtActions {
    NO_UPDT, ADD_UPDATER, REMOVE_UPDATER, CHANGE_TYPE, SET_PROC_FLAGS, UPDT_COUNT
  };
  enum AggActions {
    NO_AGG, SET_AGG_TYPE, SET_AGG_LINK, NEW_STAT_SP, NEW_SPEC_CHILD, NEW_SPEC_GP2, AGG_COUNT
  };
  enum CtrlActions {
    NO_CTRL, CTRL_PANEL, SET_SPEC, DUP_OBJ, DUP_PROC, DUP_STAT, CTRL_COUNT
  };
  enum ProcActions {
    NO_PROC, NEW_PROC_SP, NEW_AGG, XFER_OBJ, SHOW_SPEC, PROC_COUNT
  };
  enum NewActions {
    NO_NEW, NEW_SUBSUPER, NEW_PROC, NEW_STAT, NEW_LINK, NEW_SPROC, NEW_SPEC, NEW_COUNT
  };
  enum RemoveActions {
    NO_REMOVE, REMOVE_OBJ, REMOVE_LINK, NEW_PROC_GP, NEW_SPEC_GP, REMOVE_COUNT
  };

  Project* 		owner;
  ProjViewer* 		viewer;
  Project_G*		projg;
  ivWindow*		win;
  bool			spec_mode; 	// if true, editing specs, not project objects

  ivGlyph*	  	body;		// the entire contents of window
  ivPatch*	  	print_patch;	// encapsulates the printable region of viewer

  IconifyActions   	iconify_action;
  LinksActions   	links_action;
  EditActions   	edit_action;
  UpdtActions   	updt_action;
  AggActions 	  	agg_action;
  CtrlActions   	ctrl_action;
  ProcActions 	  	proc_action;
  NewActions 	  	new_action;
  RemoveActions   	remove_action;

  ivTelltaleGroup* 	tool_gp;        // radio button group of tools
  ivButton*	  	actbutton[9]; 	// tool action buttons
  ivDeck*		toggle_spec_but;
  ivDeck*		icon_but;
  ivDeck*		links_but;
  ivDeck*		edit_but;
  ivDeck*		updt_but;
  ivDeck* 		agg_but;
  ivDeck*		ctrl_but;
  ivDeck* 		proc_but;
  ivDeck* 		new_but;
  ivDeck* 		remove_but;
  ivPatch*	  	buttonpatch;

  virtual void		Init(); 	// initialize
  virtual ivGlyph* 	GetLook();	// gets the whole thing

  virtual void	update_from_state();
  virtual void	InitDisplay(); 		// redo the whole display
  virtual void	UpdateDisplay(); 	// update

  // get a 'pallete button' that is mutually exclusive
  virtual ivButton*	MEButton(ivTelltaleGroup* gp, char* txt, ivAction* a);

  virtual void	ToggleSpecEdit(); // switch between spec editing and std editing

  virtual void 	SelectActButton(int toolnumber);
  virtual void	FixEditorButtons();
  virtual void	SetSelect(); 	// sets cursor and tool mode to Select
  virtual void	SetMove();	// sets cursor and tool mode to Move
  virtual void	SetReShape();
  virtual void	SetReScale();
  virtual void	SetRotate();
  virtual void	SetAlter();

  virtual ProjViewState* GetViewState(const char* nm);

  virtual void	Minimize();	// shrink to smallest size
  virtual void	Maximize();	// expand to fit all items
  virtual void	Iconify();	// toggle iconify status of selected obj(s)
  virtual void	ShowLinks();	// toggle link-showing status of selected obj(s)
  virtual void	EditSelections();
  virtual void	UpdateSelections();
  virtual void	SetAggType();
  virtual void	AggStats();
  virtual void	SetProcFlags();
  virtual void	CtrlPanel();
  virtual void	DupeSelections();
  virtual void	NewProcs();
  virtual void	NewObjects();
  virtual void	RemoveSelections();
  virtual void	NetSetSpec();
  virtual void	NetShowSpec();

  ProjEditor(Project* pr, ivWindow* w);
  virtual ~ProjEditor();
};

*/
#endif // pdp_qt_h
