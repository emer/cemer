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

#ifndef ProjTemplates_h
#define ProjTemplates_h 1

// parent includes:
#include <ProjTemplateEl_List>

// member includes:

// declare all other types mentioned but not required to include:
class taProject; // 
class ProjTemplateEl; // 
class Project_Group; // 


TypeDef_Of(ProjTemplates);

class TA_API ProjTemplates: public ProjTemplateEl_List {
  // #INSTANCE #INLINE #CAT_Project the set of project templates available
INHERITED(ProjTemplateEl_List)
public:
  bool                  not_init; // list has not been initialized yet

  void          FindProjects();         // search paths to find all available projects
  taProject*    NewProject(ProjTemplateEl* proj_type, Project_Group* new_owner);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #NO_SAVE_ARG_VAL create a new project in new_owner of given type -- new_owner is group where project will be created
  taProject*    NewProjectFmName(const String& proj_nm, Project_Group* new_owner);
  // create a new project (lookup by name) (NULL if not found); new_owner is group where project will be created

  TA_SIMPLE_BASEFUNS(ProjTemplates);
protected:

private:
  void  Initialize();
  void  Destroy() { CutLinks(); }
};

#endif // ProjTemplates_h
