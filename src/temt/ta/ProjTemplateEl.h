// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef ProjTemplateEl_h
#define ProjTemplateEl_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <String_Array>

// declare all other types mentioned but not required to include:
class taProject; // 
class Project_Group; // 


TypeDef_Of(ProjTemplateEl);

class TA_API ProjTemplateEl: public taNBase {
  // #INSTANCE #INLINE #CAT_Project an element in the project template listing
INHERITED(taNBase)
public:
  String        desc;           // #EDIT_DIALOG description of what this project does and when it should be used
  String        tags;           // #EDIT_DIALOG list of comma-separated tags describing function of project
  String_Array  tags_array;     // #HIDDEN #READ_ONLY #NO_SAVE parsed version of the tags list, for internal use to actually operate on the tags
  String        lib_name;       // #EDIT_DIALOG name of template that contains this project
  String        URL;            // #EDIT_DIALOG full URL to find this project -- file: for files
  String        filename;       // #EDIT_DIALOG file name given to this project
  String        date;           // #EDIT_DIALOG last modify date for this project

  virtual taProject* NewProject(Project_Group* new_owner);
  // #MENU #MENU_ON_Object #MENU_CONTEXT create a new project based on this template
  virtual bool  LoadProject(taProject* prog);
  // load into given project; true if loaded, false if not

  virtual bool  ParseProjFile(const String& fnm, const String& path);
  // get project information from project template info file name and associated meta data

  virtual void  ParseTags(); // parse list of tags into tags_array

  override String GetDesc() const { return desc; }
  override String GetTypeName() const { return lib_name; }
  // This shows up in chooser instead of ProjTemplateEl!

  TA_SIMPLE_BASEFUNS(ProjTemplateEl);
protected:

private:
  void  Initialize();
  void  Destroy();
};

#endif // ProjTemplateEl_h
