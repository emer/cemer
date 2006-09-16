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

// ta_project.h

#ifndef TA_PROJECT_H
#define TA_PROJECT_H

#include "ta_group.h"
#include "ta_defaults.h"
#include "ta_viewer.h"
#include "program.h"

#ifdef TA_GUI
  #include "ta_seledit.h"
#endif




class TA_API taWizard : public taNBase {
  // ##BUTROWS_2 ##EDIT_WIDTH_60 wizard for automating construction of simulation objects
INHERITED(taNBase)
public:
  bool		auto_open;	// open this wizard upon startup

//  void	UpdateAfterEdit();
  void 	InitLinks();
  void	CutLinks(); 
  SIMPLE_COPY(taWizard);
  COPY_FUNS(taWizard, taWizard);
  TA_BASEFUNS(taWizard);
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};

class TA_API Wizard_Group : public taGroup<taWizard> {
  // group of wizard objects
INHERITED(taGroup<taWizard>)
public:
  virtual void	AutoEdit();

  void	Initialize() 		{ SetBaseType(&TA_taWizard); }
  void 	Destroy()		{ };
  TA_BASEFUNS(Wizard_Group);
};


class TA_API TypeDefault_Group : public taGroup<TypeDefault> {
  // #DEF_PATH_$PDPDIR$/defaults group of type default objects
INHERITED(taGroup<TypeDefault>)
public:
  int	Dump_Load_Value(istream& strm, TAPtr par=NULL);
  // reset members before loading..

  void	Initialize() 		{ SetBaseType(&TA_TypeDefault); }
  void 	Destroy()		{ };
  TA_BASEFUNS(TypeDefault_Group);
};


class TA_API SelectEdit_Group : public taGroup<SelectEdit> {
  // group of select edit dialog objects
INHERITED(taGroup<SelectEdit>)
public:
  virtual void	AutoEdit();

  void	Initialize() 		{ SetBaseType(&TA_SelectEdit); }
  void 	Destroy()		{ };
  TA_BASEFUNS(SelectEdit_Group);
};


class TA_API taProject : public taFBase {
  // ##FILETYPE_Project ##EXT_proj ##COMPRESS #HIDDEN A taProject has everything
INHERITED(taFBase)
public:
  String		desc;		// #EDIT_DIALOG description of the project
  
  TypeDefault_Group	defaults;	// #NO_FIND #NO_SAVE default initial settings for objects
  Wizard_Group    	wizards;	// Wizards for automatically configuring simulation objects
  SelectEdit_Group	edits;		// special edit dialogs for selected elements
  DataViewer_List	viewers;	// any top-level viewers that have been saved
  Program_Group		programs;	// Gui-based programs to run simulations and other processing

  bool			use_sim_log; 	// record project changes in the SimLog file
  String		prev_file_nm; 	// #READ_ONLY #SHOW previous file name for this project

  String		defaults_str;
  // #READ_ONLY #NO_SAVE string representation of basic defaults for the subclass; set in constructor
  String		defaults_file; 	// #READ_ONLY #NO_SAVE default name of defaults file, typically like "bp.def"

  virtual void		LoadDefaults() {}
  // load defaults according to root::default_file or precompiled defaults

  virtual const iColor* GetObjColor(TypeDef* td) {return NULL;} // #IGNORE get default color for object (for edit, project view)
  virtual const iColor* GetObjColor(int view_color) {return NULL;} // #IGNORE get default color for object (for edit, project view)

  virtual void 		MakeDefaultViewer(bool auto_opn); // make the default viewer(s)
  virtual void 		MakeDefaultWiz(bool auto_opn) {} // make the default wizard(s)

  virtual MainWindowViewer* NewViewer(); 
    // create a new, empty viewer -- note: window not opened yet
  virtual void	UpdateSimLog();
  // #MENU update simulation log (SimLog) for this project, storing the name of the project and the description as entered here.  click off use_simlog if you are not using this feature
  
  override int		Load(istream& strm, TAPtr par=NULL, void** el = NULL);
  override int		Save(ostream& strm, TAPtr par=NULL, int indent=0);
  override int 		SaveAs(ostream& strm, TAPtr par=NULL, int indent=0);

  String GetDesc() const {return desc;}
  bool	SetFileName(const String& val);
  void	UpdateAfterEdit();
  virtual void		InitLinks_impl(); // #IGNORE use this instead of InitLinks in subclasses
  void	CutLinks();
  void	Copy_(const taProject& cp);
  COPY_FUNS(taProject, taFBase);
  TA_BASEFUNS(taProject);
  
protected:
  virtual void 		InitLinks_post(); // #IGNORE called after all _impls: does LoadDefaults and launches wiz
  MainWindowViewer*	GetDefaultViewer(); // gets one if there is, else NULL
  virtual MainWindowViewer* MakeViewer_impl(); // make a standard viewer for this project type
  
private:
  void 	InitLinks(); // don't try to use this -- use _impl instead
  void	Initialize();
  void 	Destroy() { CutLinks(); }
};


class TA_API Project_Group : public taGroup<taProject> {
INHERITED(taGroup<taProject>)
friend class taProject;
public:
  int		Load(istream& strm, TAPtr par=NULL);

  void	Initialize() 		{SetBaseType(&TA_taBase);} // upclassed in pdp
  void 	Destroy()		{ };
  TA_BASEFUNS(Project_Group);
};


class TA_API taRootBase: public taNBase {
  // #VIRT_BASE #NO_INSTANCE #NO_TOKENS basic methods that the root/app object must support
INHERITED(taNBase)
public:
  String		version_no; 	// #READ_ONLY #SHOW current version number
  Project_Group		projects; 	// #NO_SAVE The projects
  
  virtual void  Settings() = 0;		// #MENU #MENU_ON_Object edit global settings/parameters (taMisc)
  virtual void	SaveConfig() = 0;		// #MENU #CONFIRM save current configuration to file ~/.pdpconfig that is automatically loaded at startup: IMPORTANT: DO NOT HAVE A PROJECT LOADED!
  virtual void	LoadConfig() = 0;		// #MENU #CONFIRM load current configuration from file ~/.pdpconfig that is automatically loaded at startup
  virtual void	Info() = 0;			// #MENU get information/copyright notice
  // #MENU #ARGC_0 #USE_RVAL #NO_REVERT_AFTER use object browser to find an object, starting with initial path if given
  virtual void	Quit() = 0;
  // #MENU #CONFIRM #MENU_SEP_BEFORE #NO_REVERT_AFTER quit from software..
  virtual void	SaveAll() = 0; // saves all the contents of the app object
  
  void	InitLinks();
  void	CutLinks();
  TA_ABSTRACT_BASEFUNS(taRootBase)
private:
  void	Initialize();
  void	Destroy();
};



#endif
