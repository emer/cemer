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



// enviro_qtso.h

#ifndef ENVIRO_QTSO_H
#define ENVIRO_QTSO_H

#include "pdpview_qt.h"
#include "enviro.h"

class EnviroView : public PDPView {
  // ##SCOPE_Environment Graphical view of an Environment
public:
  enum  EventLayout {
    HORIZONTAL,	 		// display selected events in order horizontally then vertically
    VERTICAL 			// display selected events in order vertically then horizontally
  };

  enum ValDispMode {		// ways of displaying values
    COLOR,			// color indicates value
    AREA,			// area indicates value
    LINEAR 			// linear size of square side indicates value
  };

  enum ValTextMode {		// how to display text for values
    NONE,			// no text (default)
    VALUES,			// values only
    NAMES,			// #AKA_LABELS value_name names only
    BOTH 			// both unit values and names
  };

  EventLayout	  event_layout;	// layout of the events when mulitple are displayed
  bool		  auto_scale;	// whether to update the min/max range automatically
  ValDispMode	  val_disp_mode; // method of displaying values
  ColorScaleSpec* colorspec;	// color scale to use
  ValTextMode	  val_text;	// how to display the text values
  bool		  no_border;	// do not display a border around the values
  FontSpec	  view_font;	// the font to use for the labels (pattern names, etc) in the display
  FontSpec	  value_font;	// the font to use for the values
  String	  event_header;	// #READ_ONLY what member to display in the event header
  String	  pattern_header; // #READ_ONLY what member to display in the pattern header
  String	  pattern_blocks; // #READ_ONLY what to display in the pattern blocks

  MinMax          scale_range;		// #HIDDEN range of scalebar
//TODO  EnvEditor*	  editor;		// #READ_ONLY #NO_SAVE environment editor for this one
  Event_MGroup    events_displayed;	// #HIDDEN #NO_SAVE
  TALOG		  ev_gps_displayed;	// #HIDDEN #NO_SAVE
  BaseSpec_MGroup specs_displayed;	// #HIDDEN #NO_SAVE

  virtual void 	SetColorSpec(ColorScaleSpec* colors);
  // #MENU #MENU_ON_Actions #NULL_OK set the color spectrum to use for color-coding values (NULL = use default)
  virtual void 	SetLayout(EventLayout layout = HORIZONTAL);
  // #MENU set the order in which events are displayed when selected (horizontal = fill horizontally then vertically, vertical = vertical then horizontal)
  virtual void 	SetBorder(bool border = true);
  // #MENU display a line border around the event values, or not (not is appropriate for continuous images)
  virtual void	SetViewFontSize(int point_size = 10);
  // #MENU #MENU_SEP_BEFORE set the point size of the view labels font (used for pattern names, etc).
  virtual void	SetValueFontSize(int point_size = 10);
  // #MENU set the point size of the value font (used for unit values)

  virtual void	SelectEvents(int start=0, int n_events=-1);
  // #MENU #MENU_SEP_BEFORE select events to view starting with given event continuing until n (-1 = to end)
  virtual void	DeselectEvents(int start=0, int n_events=-1);
  // #MENU deselect events to view starting with given event continuing until n (-1 = to end)

  virtual void	SelectEvent(Event* ev);
  // select given event for display
  virtual void	DeselectEvent(Event* ev);
  // deselect given event for display
  virtual void	SelectEventGp(Event_MGroup* eg);
  // select given event group for display
  virtual void	SelectGpEvents(Event_MGroup* eg);
  // select all the events within given event group (and deselect group itself) for display
  virtual void	DeselectEventGp(Event_MGroup* eg);
  // deselect given event group for display
  virtual void	DeselectAllEvents();
  // deselect all events for display

  virtual void	SetEventSpec(EventSpec* es);
  // #MENU #MENU_SEP_BEFORE Set event spec for selected events
  virtual void	ChangeEventType(TypeDef* new_type);
  // #MENU #TYPE_Event Change event types for selected events
  virtual void	ChangeEventGpType(TypeDef* new_type);
  // #MENU #TYPE_Event_MGroup Change event group types for selected event groups
  virtual void	DuplicateEvents();
  // #MENU Duplicate selected events
  virtual void	DuplicateEventGps();
  // #MENU Duplicate selected event groups

  virtual void	SetPatLabel(const char* pat_lbl);
  // set display of events to include pattern label displaying pat_lbl info, specified as Type::member, or NONE for nothing
  virtual void	SetEventLabel(const char* evt_lbl);
  // set display of events to include event label displaying evt_lbl info, specified as Type::member, or NONE for nothing

  void 		InitDisplay();
  void		UpdateDisplay(TAPtr updtr=NULL);
//obs  void		UpdateMenus_impl();

  void 		GetBodyRep();	// #IGNORE
  QWidget*	GetPrintData();	// #IGNORE

  virtual Network* GetDefaultNetwork();
  // #IGNORE get default network from project
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Environment); }
#endif
  void	UpdateAfterEdit();
  void 	Initialize();
  void 	InitLinks();
  void  CutLinks();
  void 	Destroy();
  void	Copy_(const EnviroView& cp);
  COPY_FUNS(EnviroView, PDPView);
  TA_BASEFUNS(EnviroView);
};

#endif
