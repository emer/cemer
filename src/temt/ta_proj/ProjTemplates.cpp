// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "ProjTemplates.h"
#include <NameVar>

#include <taMisc>

#include <QDir>
#include <QStringList>

TA_BASEFUNS_CTORS_DEFN(ProjTemplates);

void ProjTemplates::Initialize() {
  init = false;
}

void ProjTemplates::FindProjects() {
  Reset();                      // clear existing
  for(int pi=0; pi< taMisc::proj_template_paths.size; pi++) {
    NameVar pathvar = taMisc::proj_template_paths[pi];
    String path = pathvar.value.toString();
    String lib_name = pathvar.name;
    QDir dir(path);
    QStringList files = dir.entryList();
    for(int i=0;i<files.size();i++) {
      String fl = files[i];
      if(!fl.contains(".proj")) continue;
      ProjTemplateEl* pe = new ProjTemplateEl;
      pe->lib_name = lib_name;
      if(pe->ParseProjFile(fl, path))
        Add(pe);
      else
        delete pe;
    }
  }
  init = true;
}

taProject* ProjTemplates::NewProject(ProjTemplateEl* proj_type, Project_Group* new_owner) {
  if(proj_type == NULL) return NULL;
  return proj_type->NewProject(new_owner);
}

taProject* ProjTemplates::NewProjectFmName(const String& proj_nm, Project_Group* new_owner) {
  ProjTemplateEl* el = FindName(proj_nm);
  if(!el) {
    taMisc::Error("NewProjectFmName: project template name not found:", proj_nm);
    return NULL;
  }
  return NewProject(el, new_owner);
}

