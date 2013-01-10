// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// ta_project.h

#ifndef TA_PROJECT_H
#define TA_PROJECT_H

#include "ta_group.h"
#include "ta_defaults.h"
#include "ta_datatable.h"
#include "ta_viewspec.h"
#include "ta_viewer.h"
#include "ta_program.h"
#include "ta_plugin_p.h"
#include "ta_qtclipdata.h"
#include "colorscale.h"
#include "ta_seledit.h"
#include "ta_time.h"
#include "ta_thread.h"

class taDoc;
class taWizard;
class taProject;
class Project_Group;
class taRootBase;
class iPluginEditor;//

////////////////////////////////////////////////////////////////////////
//              taDoc -- documents


//class TA_API UserData_DocLink: public UserDataItemBase {

//class TA_API Doc_Group : public taGroup<taDoc> {

////////////////////////////////////////////////////////////////////////
//              taWizard -- wizards

//class TA_API taWizard : public taNBase {

//class TA_API Wizard_Group : public taGroup<taWizard> {

////////////////////////////////////////////////////////////////////////
//              SelectEdit -- control panels

//class TA_API SelectEdit_Group : public taGroup<SelectEdit> {
  // ##CAT_Display ##EXPAND_DEF_1 group of select edit dialog objects

////////////////////////////////////////////////////////////////////////
//              Undo/Redo System

//class TA_API taUndoDiffSrc_List : public taList<taUndoDiffSrc> {

//class TA_API taUndoRec : public taOBase {

//class TA_API taUndoRec_List : public taList<taUndoRec> {

////////////////////////////////
//      Undo Threading

//class UndoDiffThreadMgr;

// class TA_API UndoDiffTask : public taTask {
  // threading task for computing the diff on undo save data -- fork this off to save time

//class TA_API UndoDiffThreadMgr : public taThreadMgr {
  // #INLINE thread manager for UndoDiff tasks -- manages threads and tasks, and coordinates threads running the tasks

////////////////////////////////
//      Undo Manager

//class TA_API taUndoMgr : public taOBase {
  // ##CAT_Undo undo manager -- handles the basic undo functionality

/////////////////////////////////////////////////
//              Project proper

//class TA_API taProjVersion : public taOBase {

//class TA_API taLicense : public taOBase {

//////////////////////////////////////
//              taProject

// class TA_API taProject : public taFBase {

//class TA_API CssProject : public taProject {

//////////////////////////////////
//      Project Template        //
//////////////////////////////////

//class TA_API ProjTemplateEl: public taNBase {

// class TA_API ProjTemplateEl_List : public taList<ProjTemplateEl> {

// class TA_API Project_Group : public taGroup<taProject> {

class TA_API PluginWizard : public taWizard {
  // #STEM_BASE ##CAT_Wizard ##EXT_wiz wizard to create a new Plugin
INHERITED(taWizard)
public:
  enum PluginType { // the type of plugin
    UserPlugin,         // created in your emergent_user/plugins folder and only available for your login on your computer
    SystemPlugin,       // created in your computer's emergent/plugins folder -- makes plugin available to everyone, but may require Administrator/root access on your system
  };

  String                plugin_name;
  // the name, which must be a valid C identifier, and must not cause name clashes with existing classes or loaded plugins (this will be checked during Validate) -- lower case by convention
  String                class_name_prefix;
  // #READ_ONLY #SHOW capitalized version of plugin_name, for classes and similar contexts
  PluginType            plugin_type;
  // the type -- this controls the visibility of the plugin (just you, or everyone on your system) -- on Unix and some Windows systems, you will need administrator rights to install a system plugin
  bool                  validated;
  // #NO_SHOW passed validation test
  bool                  created;
  // #NO_SHOW create plugin was run
  String                plugin_location;
  // folder where to create the plugin (folder name should usually be same as plugin_name)
  String                desc;
  // description of the plugin -- what does it do?  the more you can provide here, the better it will be for others who might want to use your plugin..
  String                uniqueId;
  // IMPORTANT: this MUST actually be a unique identifier string -- if two plugins have the same uniqueId, only the last one encountered will be loaded
  String                url;
  // can be a website where the plugin is described, or avail for download, etc
  taVersion             version;
  // current version number of the plugin -- can later be updated by editing the CMakeLists.txt file in the plugin directory

  virtual bool          Validate();
  // #BUTTON validate all the provided parameters, prior to making the Plugin
  virtual bool          Create();
  // #BUTTON #CONDEDIT_ON_validated  create the plugin -- must be validated first
  virtual bool          Compile();
  // #BUTTON #CONDEDIT_ON_created  compile the plugin from the existing source code -- must be created first -- does a make and a make install
  virtual bool          Clean();
  // #BUTTON #CONDEDIT_ON_created #CONFIRM remove (clean) the plugin, e.g., to prevent it from being loaded if it is causing problems -- this just removes the compiled library object file and does not remove the source code from the install directory, so it can be regenerated from that later if desired
  virtual bool          Editor();
  // #BUTTON open the plugin file editor to edit plugin source files right here..
  virtual bool          LoadWiz(const String& wiz_file);
  // #BUTTON #FILE_DIALOG_LOAD #FILETYPE_PluginWizard #EXT_wiz load an existing wizard configuration file saved from a prior wizard create step -- do this first to enable the Editor and Compile functions to work if you haven't just Created a new plugin
  virtual bool          SaveWiz();
  // #BUTTON save current wizard configuration info to the PluginWizard.wiz file into the current plugin_location directory (only valid after directory created)

  virtual bool          MakeAllPlugins(bool user_only = true);
  // #BUTTON re-make all the currently-installed plugins (useful e.g., when main source is updated) -- user_only = only make the User plugins, not the System ones.  Can also do this by running emergent --make_all_plugins or --make_all_user_plugins at command line

  virtual bool          ShowWiz();
  // show the wizard

  TA_BASEFUNS_NOCOPY(PluginWizard);
protected:
  String                src_dir;
  String_PArray         files;
#ifndef __MAKETA__
  QPointer<iPluginEditor> editor;
#endif

  override void         UpdateAfterEdit_impl();
  override void         CheckThisConfig_impl(bool quiet, bool& ok);

  virtual void          AddTemplatedFiles(bool upgrade_only);
  // populate the file list, for files to have their content templatized
  virtual void          AddCopyFiles(bool upgrade_only);
  // populate the file list, for files merely to copy
  virtual void          CreateDestFile(const String& src_file,
                                       const String& dst_file, bool& ok);
  virtual void          TemplatizeFile(const String& src_file,
                                       const String& src, String& dst, bool& ok);

private:
  void  Initialize();
  void  Destroy()       { };
};

class TestWizard:  public taWizard {
  // test wizard -- during devel, all Wizards must inherit from this
INHERITED(taWizard)
public:
  int           inty; // #PAGE_0_Start
  float         floaty;
  String        outputy; // #PAGE_1_Output

  void          LaunchWizard(); // #BUTTON launch the wizard

  TA_BASEFUNS(TestWizard);
private:
  SIMPLE_COPY(TestWizard);
  void  Initialize() {inty=0; floaty=1;}
  void  Destroy()       { CutLinks(); }
};


#endif
