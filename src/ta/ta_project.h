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

class taProject;

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
  // ##FILETYPE_Project ##EXT_proj ##COMPRESS #HIDDEN Base class for a project object containing all relevant info for a given instance -- all ta GUI-based systems should have one..
INHERITED(taFBase)
public:
  String		desc;	// #EDIT_DIALOG description of the project
  
  taBase_Group		templates; // templates for new objects -- copy new objects from here
  Wizard_Group    	wizards; // Wizards for automatically configuring simulation objects
  SelectEdit_Group	edits;	// special edit dialogs for selected elements
  Program_Group		programs; // Gui-based programs to run simulations and other processing
  DataViewer_List	viewers; // any top-level viewers that have been saved

  bool			m_dirty; // #HIDDEN #READ_ONLY #NO_SAVE
  bool			use_sim_log; 	// record project changes in the SimLog file

  override bool		isDirty() const {return m_dirty;}
  override void 	setDirty(bool value); 

  virtual const iColor* GetObjColor(TypeDef* td) {return NULL;} // #IGNORE get default color for object (for edit, project view)
  virtual const iColor* GetObjColor(int view_color) {return NULL;} // #IGNORE get default color for object (for edit, project view)

  MainWindowViewer*	GetDefaultProjectBrowser(); // gets one if there is, else NULL
  virtual void 		AssertDefaultProjectBrowser(bool auto_opn); 
    // make the default project browser is made, and optionally open it
  virtual void 		AssertDefaultWiz(bool auto_opn) {} 
    // make sure the default wizard(s) are made, and optionally open them
  virtual void		OpenNewProjectBrowser(String proj_browser_name = "NewProjectBrowserName");
    // #MENU #MENU_ON_View #MENU_CONTEXT

  virtual MainWindowViewer* NewProjectBrowser(); 
    // create a new, empty viewer -- note: window not opened yet
  virtual void	UpdateSimLog();
  // #MENU update simulation log (SimLog) for this project, storing the name of the project and the description as entered here.  click off use_simlog if you are not using this feature
  
  override int		Load(istream& strm, TAPtr par=NULL, void** el = NULL);
  override int 		SaveAs(ostream& strm, TAPtr par=NULL, int indent=0);

  String GetDesc() const {return desc;}
  void	UpdateAfterEdit();
  virtual void		InitLinks_impl(); // #IGNORE use this instead of InitLinks in subclasses
  virtual void		CutLinks_impl(); // #IGNORE use this instead of CutLinks in subclasses -- you can call this first to nuke the viewers etc. before your own stuff
  void	Copy_(const taProject& cp);
  COPY_FUNS(taProject, taFBase);
  TA_BASEFUNS(taProject);
  
protected:
  virtual void 		InitLinks_post(); // #IGNORE called after all _impls: does LoadDefaults and launches wiz
  void 	CutLinks(); // don't override this -- use _impl instead
  virtual MainWindowViewer* MakeProjectBrowser_impl(); // make a standard viewer for this project type
  
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
  taBase_List		templates;	// #NO_SAVE #SHOW objects used as templates -- do not use or mess with these!
  Project_Group		projects; 	// #NO_SAVE The projects
  DataViewer_List	viewers;	// global viewers (not saved)
  
  virtual void  Settings() = 0;		// #MENU #MENU_ON_Object edit global settings/parameters (taMisc)
  virtual void	SaveConfig() = 0;		// #MENU #CONFIRM save current configuration to file ~/.pdpconfig that is automatically loaded at startup: IMPORTANT: DO NOT HAVE A PROJECT LOADED!
  virtual void	LoadConfig() = 0;		// #MENU #CONFIRM load current configuration from file ~/.pdpconfig that is automatically loaded at startup
  virtual void	Info() = 0;			// #MENU get information/copyright notice
  // #MENU #ARGC_0 #USE_RVAL #NO_REVERT_AFTER use object browser to find an object, starting with initial path if given
  virtual void	SaveAll() = 0; // saves all the contents of the app object
  
  taBase*		GetTemplateInstance(TypeDef* typ); // get an instance of the indicated tab type, or NULL if not found
  
  void	InitLinks();
  void	CutLinks();
  TA_ABSTRACT_BASEFUNS(taRootBase)
protected:
  virtual void		AddTemplates(); // called in InitLinks -- extend to add new templates
  virtual taBase* 	GetTemplateInstance_impl(TypeDef* typ, taBase* base);
private:
  void	Initialize();
  void	Destroy();
};



#endif
