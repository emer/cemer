// Copyright 2017, Regents of the University of Colorado,
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

#ifndef Project_Group_h
#define Project_Group_h 1

// parent includes:
#include <taGroup>

// member includes:
#include <ProjTemplates>

// declare all other types mentioned but not required to include:
class taProject; //
class ProjTemplateEl; // 

taTypeDef_Of(taProject);
taTypeDef_Of(Project_Group);

class TA_API Project_Group : public taGroup<taProject> {
  //  ##CAT_Project group of projects
INHERITED(taGroup<taProject>)
friend class taProject;
public:

  static ProjTemplates  proj_templates; // #NO_SAVE #TREE_HIDDEN #HIDDEN library of available projects

  taProject*    NewFromTemplate(ProjTemplateEl* proj_type);
  // #BUTTON #MENU_CONTEXT #FROM_LIST_proj_templates #NO_SAVE_ARG_VAL #CAT_Project create a new project from a library of existing project templates
  taProject*    NewFromTemplateByName(const String& proj_nm);
  // #CAT_Project create a new project from a library of existing project templates, looking up by name (NULL if name not found)

  int          Load_strm(std::istream& strm, taBase* par=NULL, taBase** loaded_obj_ptr = NULL) override;
  int          Load(const String& fname="", taBase** loaded_obj_ptr = NULL) override;
  int          LoadFromWiki(const String wiki, const String project_name);

  void  InitLinks() override;
  TA_BASEFUNS_NOCOPY(Project_Group);
private:
  void  Initialize()            { SetBaseType(&TA_taProject); }
  void  Destroy()               { };
};

#endif // Project_Group_h
