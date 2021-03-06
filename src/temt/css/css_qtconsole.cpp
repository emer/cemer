// Copyright 2007-2018, Regents of the University of Colorado,
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


#include "css_qtconsole.h"
#include "css_basic_types.h"
#include "css_builtin.h"
#include "css_ta.h"

#include <taMisc>
#include <taiMisc>
#include <EnumDef>
#include <Program>

//////////////////////////////////////////////////////////////////////////
//

QcssConsole* QcssConsole::theInstance = NULL;

QcssConsole::~QcssConsole() {
}

QcssConsole::QcssConsole(QObject* parent, cssCmdShell* cs) :
  inherited((QWidget*)parent, "css> ", true)
{
  cmd_shell = cs;
  setFontNameSize(taMisc::font_names.general, taMisc::font_sizes.console);
  setPager(false);              // we have our own front-end pager
}

QcssConsole* QcssConsole::getInstance(QObject* parent, cssCmdShell* cs) {
  if(theInstance != NULL) return theInstance;
  theInstance = new QcssConsole(parent, cs);
  return theInstance;
}

QString QcssConsole::interpretCommand(QString command, int* res) {
  *res = 0;
  cmd_shell->AcceptNewLine_Qt(command, false);
  inherited::interpretCommand(command, res);
  return "";
}

void QcssConsole::resizeEvent(QResizeEvent* e) {
  inherited::resizeEvent(e);
  taMisc::display_width = MIN(maxCols, taMisc::max_display_width);
  taMisc::display_height = maxLines;
}

void QcssConsole::keyPressEvent(QKeyEvent *key_event) {
  taiMisc::BoundAction action = taiMisc::GetActionFromKeyEvent(taiMisc::CONSOLE_CONTEXT, key_event);
  
  switch (action) {
    case taiMisc::CONSOLE_STOP:
      cmd_shell->src_prog->Stop();
    default:
      inherited::keyPressEvent(key_event);
  }
}

/////////////////////////////////////////////
// autocompletion

int QcssConsole::autocompletePath(String cmd_b4, String cmd, QStringList& lst) {
  int st_len = lst.size();
  if(cssBI::root == NULL) return 0;

  String par_path = cmd;
  par_path = par_path.before('.',-1);
  String mb_name = cmd;
  mb_name = mb_name.after('.', -1);
  int len = mb_name.length();
  taBase* parent = NULL;
  taBase* root = (taBase*)cssBI::root->ptr;
  if(par_path == "") {
    parent = root;
  }
  else {
    MemberDef* md = NULL;
    parent = root->FindFromPath(par_path, md);
  }

  if(!parent) return 0;
  TypeDef* par_td = parent->GetTypeDef();

  par_path = cmd_b4 + par_path + ".";           // add back for returning

  for(int i=0;i<par_td->members.size;i++) {
    MemberDef* md = par_td->members.FastEl(i);
    if(md->name(0,len) == mb_name)
      lst.append(par_path + md->name);
  }
  for(int i=0;i<par_td->static_members.size;i++) {
    MemberDef* md = par_td->static_members.FastEl(i);
    if(md->name(0,len) == mb_name)
      lst.append(par_path + md->name);
  }
  for(int i=0;i<par_td->methods.size;i++) {
    MethodDef* md = par_td->methods.FastEl(i);
    if(md->name(0,len) == mb_name)
      lst.append(par_path + md->name);
  }
  return lst.size() - st_len;
}

int QcssConsole::autocompleteScoped(String cmd_b4, String cmd, QStringList& lst) {
  int st_len = lst.size();
  String par_path = cmd;
  par_path = par_path.before("::",-1);
  String mb_name = cmd;
  mb_name = mb_name.after("::", -1);
  int len = mb_name.length();
//  taBase* parent = NULL;
  TypeDef* par_td = taMisc::FindTypeName(par_path);
  if (par_td == NULL) {
    cssProgSpace* src_prog = cssMisc::cur_top->GetSrcProg();
    cssElPtr elp = cssMisc::cur_top->types.FindName(par_path);
    if (!elp) {
      if (src_prog) {
        elp = src_prog->types.FindName(par_path);
      }
    }

    if (elp) {
      par_path = cmd_b4 + par_path + "::";              // add back for returning
      if(elp.El()->GetType() == cssEl::T_ClassType) {
        cssClassType* par_el = (cssClassType*)elp.El()->GetNonRefObj();
        for(int i=0;i<par_el->members->size;i++) {
          cssEl* md = par_el->members->FastEl(i);
          if(md->name(0,len) == mb_name)
            lst.append(par_path + md->name);
        }
        for(int i=0;i<par_el->methods->size;i++) {
          cssEl* md = par_el->methods->FastEl(i);
          if(md->name(0,len) == mb_name)
            lst.append(par_path + md->name);
        }
        for(int i=0;i<par_el->types->size;i++) {
          cssEl* st = par_el->types->FastEl(i);
          if(st->name(0,len) == mb_name)
            lst.append(par_path + st->name);
          if(st->GetType() == cssEl::T_EnumType) {
            cssEnumType* et = (cssEnumType*)st->GetNonRefObj();
            for(int j=0;j<et->enums->size; j++) {
              cssEl* en = et->enums->FastEl(j);
              if(en->name(0,len) == mb_name)
                lst.append(par_path + en->name);
            }
          }
        }
      }
      else if(elp.El()->GetType() == cssEl::T_EnumType) {
        cssEnumType* et = (cssEnumType*)elp.El()->GetNonRefObj();
        for(int j=0;j<et->enums->size; j++) {
          cssEl* en = et->enums->FastEl(j);
          if(en->name(0,len) == mb_name)
            lst.append(par_path + en->name);
        }
      }

      return lst.size() - st_len;
    }
    return 0;
  }

  par_path = cmd_b4 + par_path + "::";          // add back for returning

  for(int i=0;i<par_td->members.size;i++) {
    MemberDef* md = par_td->members.FastEl(i);
    if(md->name(0,len) == mb_name)
      lst.append(par_path + md->name);
  }
  for(int i=0;i<par_td->methods.size;i++) {
    MethodDef* md = par_td->methods.FastEl(i);
    if(md->name(0,len) == mb_name)
      lst.append(par_path + md->name);
  }
  for(int i=0;i<par_td->sub_types.size;i++) {
    TypeDef* st = par_td->sub_types.FastEl(i);
    if(st->name(0,len) == mb_name)
      lst.append(par_path + st->name);
    if(st->IsEnum()) {
      for(int j=0;j<st->enum_vals.size; j++) {
        EnumDef* en = st->enum_vals.FastEl(j);
        if(en->name(0,len) == mb_name)
          lst.append(par_path + en->name);
      }
    }
  }
  return lst.size() - st_len;
}

static cssSpace* qcss_console_get_spc(cssProgSpace* top, int spc_idx) {
  int extra_spcs = 3;
  if(cssMisc::cur_class != NULL)
    extra_spcs++;

  cssSpace* spc = NULL;

  if(spc_idx == 0)
    spc = &(top->types);
  else if(spc_idx == 1)
    spc = &(top->types);
  else if(spc_idx == 2)
    spc = &cssMisc::TypesSpace;
  else if((cssMisc::cur_class != NULL) && (spc_idx == 3))
    spc = cssMisc::cur_class->types;
  else
    spc = top->GetParseSpace(spc_idx-extra_spcs);
  return spc;
}

int QcssConsole::autocompleteKeyword(String cmd_b4, String cmd, QStringList& lst) {
  int st_len = lst.size();
  int len = cmd.length();
  int spc_idx = 0;
  cssSpace* spc = NULL;
  cssProgSpace* top = cssMisc::cur_top;
  cssProgSpace* src_prog = cssMisc::cur_top->GetSrcProg();
  do {
    spc = qcss_console_get_spc(top, spc_idx);
    if(spc == NULL) {
      if(top == cssMisc::cur_top) {
        if(src_prog != NULL) {
          top = src_prog; continue;
        }
      }
      break;
    }

    for(int i=0;i<spc->size;i++) {
      cssEl* el = spc->FastEl(i);
      if(el->name(0,len) == cmd)
        lst.append(cmd_b4 + el->name);
    }
    spc_idx++;
  } while(spc != NULL);
  return lst.size() - st_len;
}

QStringList QcssConsole::autocompleteCommand(QString q_cmd) {
  String cmd = q_cmd;
  String fcmd = q_cmd;
  String cmd_b4;
  if(cmd.contains(' ')) {
    cmd = fcmd.after(' ',-1);
    cmd_b4 = fcmd.through(' ',-1);
  }

  QStringList lst;

  if(cmd[0] == '.') {           // path
    autocompletePath(cmd_b4, cmd, lst);
  }
  else if(cmd[0] == '"') {              // filename
    cmd_b4 += '"';
    lst = autocompleteFilename(cmd.after(0), cmd_b4);
  }
  else if(cmd.contains("::")) { // scoped type
    autocompleteScoped(cmd_b4, cmd, lst);
  }
  else {                        // keyword
    autocompleteKeyword(cmd_b4, cmd, lst);
  }
  return lst;
}

void QcssConsole::ctrlCPressed() {
  cssMisc::Warning(NULL, "User Interrupt");
  if(cssMisc::cur_top)
    cssMisc::cur_top->Stop();
  Program::stop_req = true;     // notify any running programs to stop too
}

