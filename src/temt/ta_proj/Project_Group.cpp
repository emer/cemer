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

#include <taProject>
#include "Project_Group.h"

#include <QFileInfo>
#include <QDir>
#include "taFiler"
#include "taMediaWiki"
#include "taMisc"

TA_BASEFUNS_CTORS_DEFN(Project_Group);


using namespace std;

void Project_Group::InitLinks() {
  inherited::InitLinks();
  if(proj_templates.not_init) {
    taBase::Ref(proj_templates);
    proj_templates.FindProjects();
  }
}


int Project_Group::Load(const String& fname, taBase** loaded_obj_ptr) {
  // chg working dir to that of project -- simplifies lots of stuff immensely
  QFileInfo fi(fname);
  QDir::setCurrent(fi.absolutePath());
  int rval = inherited::Load(String(fi.absoluteFilePath().toUtf8().data()), loaded_obj_ptr);
  return rval;
}

int Project_Group::Load_strm(istream& strm, taBase* par, taBase** loaded_obj_ptr) {
  int rval = inherited::Load_strm(strm, par, loaded_obj_ptr);
  // note: used to do Dump_Load_post here but now it is done where it should be..
  return rval;
}

int Project_Group::LoadFromWiki(const String wiki, const String project_name) {
  int rval = 0;
  String path = QDir::tempPath() + "/" + project_name + ".proj";
  QFileInfo fi(path);
  if (!taMediaWiki::DownloadFile(wiki, project_name, path)) {
    taMisc::Error("Failed to download project " + project_name + " from wiki " + wiki);
    return rval;
  };
  if (!fi.exists()) {
    taMisc::Error("Failed to download project " + project_name + " from wiki " + wiki);
    return rval;
  }
  taFiler* flr = GetLoadFiler(path, _nilString, -1, _nilString);
  taBase* el = NULL;
  rval = Load_strm(*flr->istrm, el);
  QDir qdir;
  qdir.remove(path);
  return rval;
}

ProjTemplates Project_Group::proj_templates;

taProject* Project_Group::NewFromTemplate(ProjTemplateEl* proj_type) {
  return proj_templates.NewProject(proj_type, this);
}

taProject* Project_Group::NewFromTemplateByName(const String& prog_nm) {
  return proj_templates.NewProjectFmName(prog_nm, this);
}
