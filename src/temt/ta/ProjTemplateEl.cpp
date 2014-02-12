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

#include "ProjTemplateEl.h"
#include <Project_Group>
#include <taProject>

#include <taMisc>

#include <QFileInfo>

TA_BASEFUNS_CTORS_DEFN(ProjTemplateEl);

using namespace std;


void ProjTemplateEl::Initialize() {
}

void ProjTemplateEl::Destroy() {
}

taProject* ProjTemplateEl::NewProject(Project_Group* new_owner) {
  // todo: need to support full URL types -- assumed to be file right now
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  taBase* pg;
//   String oldpath = QDir::currentPath();
//   taMisc::Info("oldpath", oldpath);
  new_owner->taGroup<taProject>::Load(path, &pg); // avoid setting current wd based on load
  if(pg) {
    pg->SetFileName("");        // nuke association with template file!
  }
//   QDir::setCurrent(oldpath);
  return (taProject*)pg;
}

bool ProjTemplateEl::LoadProject(taProject* proj) {
  // todo: need to support full URL types -- assumed to be file right now
  // also this barfs and is not supported!
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  proj->Load(path);
  proj->SetFileName("");
  return true;
}

bool ProjTemplateEl::ParseProjFile(const String& fnm, const String& path) {
  filename = fnm;
  URL = "file:" + path + "/" + filename;
  String infofnm = filename.before(".proj") + ".tmplt"; // template info
  String openfnm = path + "/" + infofnm;
  fstream strm;
  strm.open(openfnm, ios::in);
  if(strm.bad() || strm.eof()) {
    taMisc::Error("ProjTemplateEl::ParseProgFile: could not open file name:", openfnm);
    return false;
  }
  bool rval = false;
  int c = taMisc::read_till_eol(strm);
  while((c != EOF) && !strm.eof() && !strm.bad()) {
    if(taMisc::LexBuf.contains("name=")) {
      name = taMisc::LexBuf.after("name=");
      name.gsub("\"", "");
      if(name.lastchar() == ';') name = name.before(';');
    }
    if(taMisc::LexBuf.contains("tags=")) {
      tags = taMisc::LexBuf.after("tags=");
      tags.gsub("\"", "");
      if(tags.lastchar() == ';') tags = tags.before(';');
      ParseTags();
    }
    if(taMisc::LexBuf.contains("desc=")) {
      desc = taMisc::LexBuf.after("desc=");
      desc.gsub("\"", "");
      if(desc.lastchar() == ';') desc = desc.before(';');
      rval = true;
      break;
    }
    c = taMisc::read_till_eol(strm);
  }
  strm.close();
  // todo: should use QUrlInfo instead -- need QtNetwork module access!
  QFileInfo urlinfo(openfnm);
  QDateTime mod = urlinfo.lastModified();
  date = mod.toString(Qt::ISODate);
  return rval;
}

void ProjTemplateEl::ParseTags() {
  tags_array.Reset();
  if(tags.empty())
    return;
  String tmp = tags;
  while(tmp.contains(',')) {
    String tag = tmp.before(',');
    tag.gsub(" ","");           // nuke spaces
    tags_array.Add(tag);
    tmp = tmp.after(',');
  }
  if(!tmp.empty()) {
    tmp.gsub(" ","");           // nuke spaces
    tags_array.Add(tmp);
  }
}
