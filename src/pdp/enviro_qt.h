// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



#ifndef enviro_qt_h
#define enviro_qt_h

#include "pdpbase.h"
#include "enviro.h"
#include "colorbar.h"
#include "ta_qtdata.h"

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
class ivFieldEditor;	// #IGNORE

// predeclarations of all types:

class EnviroView;
class Environment_G;
class ObjNameEdit;
class PatSpecEdit;
class EventBox_G;
class Event_G;
class Pattern_G;
class NValBox_G;
class EnvViewer;
class EnvEditor;

class Environment_G : public GraphicMaster {
public:
  Environment*	env;
  EnvEditor*	owner;
  ivColor*	border;	        // 'unselected' border color
  const ivBrush* defbrush;	// default brush
  FloatTDCoord	max_size;	// for the whole environment (sets scaling)
  TDCoord	evt_geom;	// layout for the displayed events
  bool		reinit_display;	// subobjs set this flag to reinit after popping out
  ivTransformer* tx;
  FloatTwoDCoord span;		// size in coords of display

  virtual void	Reset();
  virtual void	Build();
  virtual void	RevertValues();
  virtual void	ApplyValues();

  virtual ivColor* GetLabelColor();

  bool		update_from_state(ivCanvas* c);

  bool 		select(const ivEvent& e, Tool& tool, bool);
  bool		effect_select(bool set_select);

  void 		allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext);
  void 		ReCenter();
  void 		ScaleCenter(const ivAllocation& a);

  // translate x,y unit-wise coordinates into screen coords
  ivCoord xcscale(ivCoord xval) { return xval / max_size.x; }
  ivCoord ycscale(ivCoord yval) { return yval / max_size.y; }

  // translate x,y unit-wise coordinates into text coords
  ivCoord xtscale(ivCoord xval) { return GetCurrentXformScale() * xcscale(xval); }
  ivCoord ytscale(ivCoord yval) { return GetCurrentYformScale() * ycscale(yval); }

  Environment_G(Environment*);
  ~Environment_G();
  GRAPHIC_BASEFUNS(Environment_G);
};

class ObjNameEdit : public taNBase {
  // edit object name
public:
  TAPtr	obj;		// #HIDDEN the object

  void	SetObj(TAPtr);
  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy();
  TA_BASEFUNS(ObjNameEdit);
};

class ObjValueEdit : public taBase {
  // edit value
public:
  String	name;		// #READ_ONLY #SHOW_IV name of the field being edited
  String	value;		// the value
  MemberDef*	mbr;		// #HIDDEN member definition
  TAPtr		obj;		// #HIDDEN the object

  void	SetObj(TAPtr ob, MemberDef* md);
  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy();
  TA_BASEFUNS(ObjValueEdit);
};

class PatSpecEdit : public taNBase {
  // edit critical pattern spec values
public:
  PatternSpec::PatTypes	 type;    	// Type of pattern
  PatternSpec::PatLayer  to_layer;    	// which network layer to present pattern to
  String        layer_name;  		// name of layer
  int           layer_num;   		// number of layer

  PatternSpec* obj;		// #HIDDEN the object

  void	SetObj(PatternSpec*);
  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy();
  TA_BASEFUNS(PatSpecEdit);
};

class EditText_G : public NoScale_Text_G {
public:
  bool		graspable() { return false; }

  bool	effect_select(bool set_select);

  EditText_G(TAPtr,GraphicMaster* ,const ivFont* , const ivColor* ,
		 const char*, ivTransformer*);
  GRAPHIC_BASEFUNS(EditText_G);
};

class Event_G : public PolyGraphic {
  // represents both events and event specs
public:
  EventSpec*		espec;
  Event* 		evt;
  Environment_G*	envg;
  TDCoord		move_pos;
  ObjNameEdit*		name_edit;
  ObjValueEdit*		value_edit;

  virtual void	Build();
  virtual void	RevertValues();
  virtual void	ApplyValues();

  bool		selectable();
  bool		graspable() { return false; }
  bool     	select(const ivEvent& e, Tool& tool, bool unselect);
  bool		effect_select(bool set_select);

  // cancel these out
  bool	grasp_move(const ivEvent&,Tool&,ivCoord, ivCoord){return true;}
  bool	manip_move(const ivEvent& ,Tool&,ivCoord,ivCoord,ivCoord,
			   ivCoord,ivCoord,ivCoord) {return true;}
  bool	grasp_scale(const ivEvent&, Tool&,ivCoord,ivCoord,ivCoord,ivCoord,
			    ivCoord,ivCoord) {return true;}
  bool	manip_scale(const ivEvent&,Tool&,ivCoord,ivCoord,ivCoord,ivCoord,
			    ivCoord,ivCoord,ivCoord,ivCoord) {return true;}

  ivGlyph*	clone() const;
  Event_G(EventSpec* es, Event* ev, Environment_G* ng);
  virtual ~Event_G();
  GRAPHIC_BASEFUNS(Event_G);
};

class EventBox_G : public Polygon {
  // box representing event
public:
  Event_G*	 evtg;
  Environment_G* envg;

  void 		SetPoints(ivCoord x,ivCoord y);

  bool		selectable();
  bool		graspable() { return false; }
  bool     	select(const ivEvent& e, Tool& tool, bool unselect);
  bool		effect_select(bool set_select);

  // cancel these out
  bool	grasp_move(const ivEvent&,Tool&,ivCoord, ivCoord){return true;}
  bool	manip_move(const ivEvent& ,Tool&,ivCoord,ivCoord,ivCoord,
			   ivCoord,ivCoord,ivCoord) {return true;}
  bool	grasp_scale(const ivEvent&, Tool&,ivCoord,ivCoord,ivCoord,ivCoord,
			    ivCoord,ivCoord) {return true;}
  bool	manip_scale(const ivEvent&,Tool&,iCoord,iCoord,iCoord,iCoord,
			    iCoord,iCoord,iCoord,iCoord) {return true;}

  EventBox_G(Event_G* eg, Environment_G* eng);
  GRAPHIC_BASEFUNS(EventBox_G);
};

class Pattern_G : public PolyGraphic {
  // collects all the aspects of the pattern
public:
  PatternSpec* 		patspec;
  Pattern* 		pat;
  Environment_G*	envg;
  TDCoord		move_pos;
  PatSpecEdit*		ps_edit;
  ObjNameEdit*		name_edit;
  ObjValueEdit*		value_edit;

  float_RArray		val_buffer;	// for editing

  virtual void	Position();
  virtual void	Build();
  virtual void	RevertValues();
  virtual void	ApplyValues();

  bool		selectable();
  bool     	select(const ivEvent& e, Tool& tool, bool unselect);
  bool		effect_select(bool set_select);

  bool	grasp_move(const ivEvent& e,Tool& tool,iCoord, iCoord);
  bool	manip_move(const ivEvent& e,Tool& tool,iCoord ix, iCoord iy, iCoord lx, iCoord ly,
		   iCoord cx, iCoord cy);
  bool 	effect_move(const ivEvent& ev, Tool& tl, iCoord ix,
		    iCoord iy, iCoord fx, iCoord fy);
  bool	grasp_stretch(const ivEvent&, Tool&,iCoord ix, iCoord iy);
  bool	manip_stretch(const ivEvent&, Tool&,iCoord ix, iCoord iy, iCoord lx, iCoord ly,
		      iCoord cx, iCoord cy);
  bool	effect_stretch(const ivEvent&, Tool&,iCoord ix,iCoord iy, iCoord fx, iCoord fy);

  // cancel these out
  bool	grasp_scale(const ivEvent&, Tool&,iCoord,iCoord,iCoord,iCoord,
			    iCoord,iCoord) {return true;}
  bool	manip_scale(const ivEvent&,Tool&,iCoord,iCoord,iCoord,iCoord,
			    iCoord,iCoord,iCoord,iCoord) {return true;}

  ivGlyph*	clone() const;
  Pattern_G(PatternSpec* ps, Pattern* pt, Environment_G* ng);
  virtual ~Pattern_G();
  GRAPHIC_BASEFUNS(Pattern_G);
};

class PatternBox_G : public Polygon {
public:
  PatternSpec* 		patspec;
  Pattern* 		pat;
  Environment_G*	envg;
  TDCoord		move_pos;

  virtual void	Position();
  virtual void	Build();

  virtual void 	SetPoints(iCoord x0,iCoord y0, iCoord x1, iCoord y1,
			iCoord x2,iCoord y2, iCoord x3, iCoord y3);

  bool	selectable();
  bool  select(const ivEvent& e, Tool& tool, bool unselect);
  bool	effect_select(bool set_select);

  void 	draw_gs (ivCanvas* c, Graphic* gs);
  bool	update_from_state(ivCanvas* c);

  virtual void render_text(ivCanvas* c, ScaleBar* cbar, float val, String& str,
			   FloatTwoDCoord& ll, FloatTwoDCoord& ur, bool from_top=false);
  virtual void render_color(ivCanvas* c, ScaleBar* cbar, float val,
			    FloatTwoDCoord& ll, FloatTwoDCoord& ur);
  virtual void render_area(ivCanvas* c, ScaleBar* cbar, float val,
			   FloatTwoDCoord& ll, FloatTwoDCoord& ur);
  virtual void render_linear(ivCanvas* c, ScaleBar* cbar, float val,
			     FloatTwoDCoord& ll, FloatTwoDCoord& ur);

  // cancel these ones
  bool	grasp_stretch(const ivEvent&, Tool&,iCoord, iCoord) {return true;}
  bool	manip_stretch(const ivEvent&, Tool&,iCoord, iCoord, iCoord, iCoord,
    iCoord, iCoord) {return true;}
  bool	effect_stretch(const ivEvent&, Tool&,iCoord,iCoord, iCoord, iCoord)
    { return true;}
  bool	grasp_move(const ivEvent&,Tool&,iCoord, iCoord){return true;}
  bool	manip_move(const ivEvent& ,Tool&,iCoord,iCoord,iCoord,
			   iCoord,iCoord,iCoord) {return true;}
  bool	grasp_scale(const ivEvent&, Tool&,iCoord,iCoord,iCoord,iCoord,
			    iCoord,iCoord) {return true;}
  bool	manip_scale(const ivEvent&,Tool&,iCoord,iCoord,iCoord,iCoord,
			    iCoord,iCoord,iCoord,iCoord) {return true;}

  ivGlyph*	clone() const;
  PatternBox_G(PatternSpec* ps, Pattern* pt, Environment_G* ng);
  virtual ~PatternBox_G();
  GRAPHIC_BASEFUNS(PatternBox_G);
};

class NValBox_G : public Polygon {
  // box representing the location of the n_vals
public:
  Pattern_G*	 patg;
  Environment_G* envg;
  TDCoord	 nval_pos;
  TDCoord	 move_pos;

  virtual void	Position();
  virtual void 	Build();

  bool	selectable();
  bool  select(const ivEvent& e, Tool& tool, bool unselect);
  bool	effect_select(bool set_select);
  void 	draw_gs (ivCanvas* c, Graphic* gs);

  // cancel these out
  bool	grasp_scale(const ivEvent&, Tool&,iCoord,iCoord,iCoord,iCoord,
		    iCoord,iCoord) {return true;}
  bool	manip_scale(const ivEvent&,Tool&,iCoord,iCoord,iCoord,iCoord,
		    iCoord,iCoord,iCoord,iCoord) {return true;}
  // but implement these
  bool	grasp_move(const ivEvent& e,Tool& tool,iCoord, iCoord);
  bool	manip_move(const ivEvent& e,Tool& tool,iCoord ix, iCoord iy, iCoord lx, iCoord ly,
		   iCoord cx, iCoord cy);
  bool 	effect_move(const ivEvent& ev, Tool& tl, iCoord ix,
		    iCoord iy, iCoord fx, iCoord fy);

  NValBox_G(Pattern_G* pg, Environment_G* eng);
  GRAPHIC_BASEFUNS(NValBox_G);
};


class EnvViewer : public GlyphViewer {
  // ##NO_INSTANCE ##NO_MEMBERS ##NO_TOKENS ##NO_CSS viewer for environment
public:
  EnviroView* 	envv;
  void 		init_graphic() 		{ initgraphic();}
  void 		allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext);
  EnvViewer(EnviroView *n, float w, float h, const ivColor* bg);
};

class ivGlyph;			// #IGNORE
class tbScrollBox;		// #IGNORE
class ivPolyGlyph;		// #IGNORE
class ivPatch;			// #IGNORE
class PScaleBar;		// #IGNORE
class ColorScale;		// #IGNORE
class ivButton;			// #IGNORE
class HiLightButton;		// #IGNORE
class taivMenu;			// #IGNORE
class ivInputHandler;		// #IGNORE
class ivDeck;			// #IGNORE

class EnvEditor {
  // ##NO_INSTANCE ##NO_MEMBERS ##NO_TOKENS ##NO_CSS editor for environment
public:
  enum TransDupeActions {
    NO_TRANS_DUPE, TRANS_EVENT, DUPE_EVENT, DUPE_EVGP, TRANS_DUPE_COUNT
  };
  enum SpecTypeActions {
    NO_SPEC_TYPE, SET_SPEC_TYPE, CHANGE_TYPE, SELECT_EVENTS, SPEC_TYPE_COUNT
  };
  enum NewActions {
    NEW_SPEC, NEW_PATTERNS, NEW_TO_LAYER, HOT_NEW_SPEC, NEW_COUNT
  };
  enum RemoveActions {
    NO_REMOVE, REMOVE_SPECS, REMOVE_PATTERNS, REMOVE_UNITS, REMOVE_COUNT
  };
  enum EditActions {
    NO_EDIT, EDIT_SPECS, EDIT_PATTERNS, EDIT_NAMES, EDIT_VALS, EDIT_OBJS, EDIT_COUNT
  };
  enum New2Actions {
    NO_NEW2, NEW_CHILD, EDIT_VALUE_NAMES, NEW2_COUNT
  };
  enum MiscActions {
    NO_MISC, LAYOUT_UPDT, MISC_COUNT
  };
  enum CopyActions {
    NO_COPY, COPY_SPEC, COPY_PAT, COPY_COUNT
  };
  enum DupeActions {
    NO_DUPE, DUPE_SPEC, DUPE_PAT, DUPE_COUNT
  };
  enum EditMode {
    EVENTS, SPECS
  };

  EditMode		edit_mode;	// two modes of editing
  EnviroView*		owner;
  EnvViewer* 		viewer;
  Environment_G*	envg;
  ivWindow*		win;
  Environment* 		env;

  int			patflags; 	// current setting of patflags menu
  bool			specflags; 	// true if setting spec flags

  MemberSpace		event_membspace; // List of memberdefs displayable in the event header space
  TypeSpace     	event_typespace; // Types associated with membspace
  MemberDef* 		event_header;	 // what to display in the event header area
  TypeDef*		eventhead_type;	 // current event header member's type

  MemberSpace		pat_membspace;	// List of memberdefs displayable in the pattern header space
  TypeSpace     	pat_typespace;	// Types associated with membspace
  MemberDef* 		pattern_header; // what to display in the pattern header area
  TypeDef*		pathead_type;	// current pattern header member's type

  ivGlyph*	  	body;		// the entire contents of window
  ivPatch*		eddeck_patch;	// patch around the edit deck
  ivDeck*		edit_deck; 	// editor deck

  ivPatch*		pospatch;       // position indicator
  ivButton*		disp_ckbox;     // display on/off check box
  ivButton*       	as_ckbox;	// auto scale toggle checkbox

  taivMenu*		vtextmenu; 	// value text menu
  taivMenu*		dispmdmenu; 	// display mode menu
  taivMenu*		eventheadmenu;	// menu for pattern header choices
  taivMenu*		patheadmenu;	// menu for pattern header choices
  taivHierMenu*		patflagsmenu;	// menu for pattern flags

  HiLightButton*  	new_event_button; // button for creating new events
  HiLightButton*  	apply_button;	   // button for applying pattern changes
  ivPatch*     	  	apply_button_patch;	// goes around apply_button

  tbScrollBox* 	  	event_names;	// contains all the event names
  ivPatch*     	  	evnm_patch;	// goes around the names
  tbScrollBox* 	  	spec_names;	// contains all the spec names
  ivPatch*     	  	spnm_patch;	// goes around the names

  ivPatch*	  	print_patch;	// encapsulates the printable region of viewer

  ColorScale*		scale;		// internal colorscale
  PScaleBar* 		cbar;		// colorbar at bottom of screen

  EventSpec*		one_espec_sel; 	// only one eventspec is selected: default to operate on patterns of this spec
  TransDupeActions	trans_dupe_action;
  SpecTypeActions	spec_type_action;
  NewActions 	  	new_action;
  RemoveActions   	remove_action;
  EditActions 	  	edit_action;
  New2Actions 	  	new2_action;
  MiscActions	  	misc_action;
  CopyActions 	  	copy_action;
  DupeActions	  	dupe_action;

  ivTelltaleGroup* 	tool_gp;        // radio button group of tools
  ivButton*	  	actbutton[9]; 	// tool action buttons
  ivDeck* 		trans_dupe_but;
  ivDeck* 		spec_type_but;
  ivDeck* 		new_but;	// new button options (stacked in a deck)
  ivDeck* 		remove_but;
  ivDeck* 		edit_but;
  ivDeck* 		new2_but;
  ivDeck* 		misc_but;
  ivDeck* 		copy_but;
  ivDeck* 		dupe_but;
  ivPatch*	  	ev_butpatch; // group of event buttons
  ivPatch*	  	sp_butpatch; // group of spec buttons

  virtual void		Init(); 	// initialize
  virtual ivGlyph* 	GetLook();	// gets the whole thing

  virtual void	update_from_state();
  virtual void	InitDisplay(); 		// redo the whole display
  virtual void	UpdateDisplay(); 	// update

  virtual void	EditSpecs();	// switch to editing specs
  virtual void	EditEvents();	// switch to editing events

  virtual void	BuildVTextMenu(); // make the value text menu
  virtual void	BuildDispMdMenu(); // make the value text menu
  virtual void	BuildPatHeadMenu(); // create menu for pattern header choices
  virtual void	BuildEventHeadMenu(); // create menu for pattern header choices
  virtual void	BuildPatFlagsMenu(); // create a menu for the pattern flags
  virtual void	AddTypes(MemberSpace* ms, TypeSpace* ts, TypeDef* td);

  // get a 'pallete button' that is mutually exclusive
  virtual ivButton*	MEButton(ivTelltaleGroup* gp, char* txt, ivAction* a);

  virtual void	SetVText(taivMenuEl* sel); // function called when user selects
  virtual void	SetDispMd(taivMenuEl* sel); // function called when user selects
  virtual void	SetPatHead(taivMenuEl* sel); // function called when user selects
  virtual void	SetPatHeadStr(const char* nm); // set based on type::mbr string
  virtual void	SetEventHead(taivMenuEl* sel); // event header menu callback
  virtual void	SetEventHeadStr(const char* nm);  // set based on type::mbr string
  virtual void	SetPatFlags(taivMenuEl* sel); // patflags menu callback

  virtual void 	GetEventLabels();	   // gets event buttons
  virtual void  DisplayEvent(Event * ev);  // display given event
  virtual void	UnDisplayEvent(Event* ev); // turn off display of given event (if displayed)
  virtual void	UnDisplayAllEvents();	   // turn off display of all events
  virtual void	UnDisplayEvent_impl(Event* ev); // (doesn't remove from events_displayed)
  virtual void	UnDisplayAllEvents_impl();	// ditto

  virtual void  DisplayEventGp(Event_MGroup * eg);  // display given event group
  virtual void  DisplayGpEvents(Event_MGroup * eg);  // display events within a group
  virtual void	UnDisplayEventGp(Event_MGroup* eg); // turn off display of given event group

  virtual int	GetEventButIdx(Event* ev); // gets button index for event
  virtual int	GetEventGpButIdx(Event_MGroup* eg); // gets button index for event group
  virtual void 	RemoveEvent(Event* ev);
  virtual void 	RemoveEventGp(Event_MGroup* eg);
  virtual void 	NewEvent();	// callback for new event button
  virtual void 	DeleteEvents();	// callback for delete events button
  virtual void  AddEvent(Event* ev, int butidx); // butidx is index for button, -2 = find it
  virtual void  AddEventGp(Event_MGroup* eg, int butidx); // butidx is index for button, -2 = find it

  virtual void  HighlightApply(); // highlight the apply button
  virtual void	Apply();	   // Apply changes to all displayed events

  virtual void 	GetSpecLabels();	   // gets spec buttons
  virtual void  AddSpec(EventSpec* ev, int& leaf_idx);
  virtual void  DisplaySpec(EventSpec* ev);  // display given event
  virtual void	UnDisplaySpec(EventSpec* ev); // turn off display of given event (if displayed)
  virtual void	UnDisplayAllSpecs();	   // turn off display of all events
  virtual void	UnDisplaySpec_impl(EventSpec* ev); // (doesn't remove from events_displayed)
  virtual void	UnDisplayAllSpecs_impl();	// ditto

  virtual void 	RemoveSpec(EventSpec* ev);

  virtual void	ToggleDisplay(bool update=true);
  virtual void	MenuToggleDisplay(){ToggleDisplay();}
  virtual void	UpdateMinMaxScale();
  virtual void	CBarAdjustNotify();
  virtual void	ToggleAutoScale(bool update=true);
  virtual void	MenuToggleAutoScale(){ToggleAutoScale();};

  virtual void 	SelectActButton(int toolnumber);
  virtual void	SetSelect(); 	// sets cursor and tool mode to Select
  virtual void	SetMove();	// sets cursor and tool mode to Move
  virtual void	SetReShape();
  virtual void	SetReScale();
  virtual void	SetRotate();
  virtual void	SetAlter();

  virtual void	FixEditorButtons();
  virtual void	TransDupeEvents();
  virtual void	SpecTypeEvents();
  virtual void	NewObjects();
  virtual void	EditSelections();
  virtual void	RemoveSelections();
  virtual void	New2Objects();
  virtual void	EditValueNames();
  virtual void	MiscObjects();
  virtual void	CopyObjects();
  virtual void	DupeObjects();

  virtual void	UpdatePosivPatch(TDCoord& tdg)
  { UpdatePosivPatch(tdg.x, tdg.y, tdg.z); }
  virtual void	UpdatePosivPatch(int x, int y, int z);
  virtual void  UpdatePosivPatch(float val);
  virtual void	ClearPosivPatch();

  EnvEditor(Environment* en, EnviroView* ev, ivWindow* w);
  virtual ~EnvEditor();
};

#endif // enviro_iv_h

