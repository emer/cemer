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

#include "TemtClient.h"
#include <TemtClient_QObj>
#include <MainWindowViewer>
#include <ProgVar>
#include <TemtServer>
#include <DataTable>
#include <taImage.h>
#include <MemberDef>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>

#include <QTcpSocket>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QString>

#include <sstream>

#if (QT_VERSION >= 0x050000)
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#endif

TA_BASEFUNS_CTORS_DEFN(TemtClient);

using namespace std;

TemtClient_QObj* TemtClient::adapter() {
  return (TemtClient_QObj*)taOABase::adapter;
}

String TemtClient::ReadQuotedString(const String& str, int& p, bool& err) {
  // conservative buff
  int max = str.length() - 1;
  STRING_BUF(rval, ((max - p) + 1));
  ++p; // skip opening "
  
  char c;
  while ((p <= max) && !err) {
    c = str[p++];
    if (c == '\"') break; // discard closing " and exit
    // check for escaped char, process that if so
    if (c == '\\') {
      if (p > max) { // missing esc operand
        err = true;
        break; // unexpected
      }
      c = str[p++];
      switch (c) {
        case 'n': c = '\n'; break;
        case 't': c = '\t'; break;
        case 'v': c = '\v'; break;
        case 'b': c = '\b'; break;
        case 'r': c = '\r'; break;
        case 'f': c = '\f'; break;
        case '?': c = '?'; break;
        case '\\': c = '\\'; break;
        case '\'': c = '\'';
        case '\"': c = '\"';
        default: {
          // we expect 3 octal digits (note: restriction from general \o \oo \ooo)
          // look for octal format, else just use next char (prob an error)
          String oct;
          for (int i = 0; i < 3; ++i) {
            // not octal char, or ran out unexpectedly
            if (!((c >= '0') && (c <= '9'))) {
              err = true;
              return rval; // unexpected exit
            }
            oct.cat(c);
            // if not last already, read next
            if (i < 2) {
              if (p > max) {
                err = true;
                return rval; // unexpected exit
              }
              c = str[p++];
            }
          }
          // convert from octal
          c = (char)strtol(oct.chars(), NULL, 8);
        } break;
      }
    }
    rval.cat(c);
  }
  return rval;
}

String TemtClient::NextToken(const String& str, int& p, bool& err) {
  int max = str.length() - 1;
  // skip ws
  char c;
  while ((p <= max) && !err) {
    c = str[p];
    if (!isspace(c)) break;
    ++p;
  }
  if (p > max) return _nilString; // end
  
  // look for " and put us in quoted mode if so
  c = str[p];
  if (c == '\"')
    return ReadQuotedString(str, p, err);
  
  // single = is a token
  if (c == '=') {
    ++p;
    return "=";
  }
  
  // ok, read a raw token, which is chars up to ws or =
  String rval;
  while (p <= max) {
    c = str[p];
    if ((c == '=') || isspace(c)) break;
    rval.cat(c);
    ++p;
  }
  return rval;
}

void TemtClient::Initialize() {
  state = CS_READY; // implicit in fact we were created
  server = NULL;
  SetAdapter(new TemtClient_QObj(this));
  msg_format = TemtClient::NATIVE;  // set based on format of incoming message
  
#if (QT_VERSION >= 0x050000)
  json_format = QJsonDocument::Indented;  // can be changed by client
#endif
}

void TemtClient::Destroy() {
  CloseClient();
}

void TemtClient::Copy_(const TemtClient& cp) {
  //NOTE: not designed to be copied
  CloseClient();
  if (server != cp.server) server = NULL;
}

void TemtClient::CloseClient() {
  if (!isConnected()) return;
  if (sock) {
    sock->disconnectFromHost();
    // probably called back immediately, so check again
    if (sock) {
      sock = NULL;
    }
  }
  setState(CS_DISCONNECTED);
}

void TemtClient::cmdCloseProject() {
  //TEMP
  SendError("CloseProject not implemented yet");
}

void TemtClient::cmdEcho() {
  String r = cmd;
  for (int i = 0; i < pos_params.size; ++i) {
    r.cat(" ");
    r.cat(pos_params.FastEl(i));
  }
  for (int i = 0; i < name_params.size; ++i) {
    r.cat(" ");
    NameVar& nv = name_params.FastEl(i);
    r.cat(nv.name + "=" + nv.value.toString());
  }
  SendReply( r);
}

void TemtClient::cmdOpenProject() {
  String proj_file = taMisc::ExpandFilePath(name_params.GetVal("project").toString());
  if (proj_file == "") {
    SendError("Did not specify valid project file");
    return;
  }
  taProject* proj = NULL;
  // canonicalize name, for comparison to open projects
  QFileInfo fi(proj_file);
  bool exists = fi.exists();
  if (exists) {
    proj_file = fi.canonicalFilePath();
    for (int i = 0; i < tabMisc::root->projects.size; ++i) {
      proj = tabMisc::root->projects.FastEl(i);
      if (proj && proj->file_name == proj_file) {
        break; // break out of loop
      }
      proj = NULL; // in case we fall out of loop
    }
  } else {
    SendError("file \"" + proj_file + "\" not found");
    return;
  }
  // if proj has a value, then we should view the existing, else open it
  bool clear_dirty = true; // only for new loads, or old views when not dirty already
  if (proj) {
    SendOk();
    return;
  } else {
    taBase* el = NULL;
    tabMisc::root->projects.Load(proj_file, &el);
    proj = dynamic_cast<taProject*>(el);
    if (proj) {
      taMisc::Info("Yeah, got project open");
      //if (taMisc::gui_active) {
      //  MainWindowViewer::NewProjectBrowser(proj);
      //}
      SendOk();
      return;
    } else {
      SendError("file \"" + proj_file + "\" not found");
      return;
    }
  }
}

void TemtClient::cmdRunProgram(bool sync) {
  // store position param to name/value params - backwards compatibility
  if (msg_format == TemtClient::NATIVE) {
    String pnm = pos_params.SafeEl(0);
    name_params.SetVal("program", pnm);
  }
  
  // now we are native/json agnostic
  String pnm = name_params.GetVal("program").toString();
  if (pnm.empty()) {
    SendError("RunProgram: program name expected", TemtClient::NOT_FOUND);
    return;
  }
  
  taProject* proj = GetCurrentProject();
  if (!proj) {
    SendError("RunProgram " + pnm + ": no project open", TemtClient::NOT_FOUND);
    return;
  }
  
  // get program, make sure exists
  Program* prog = GetAssertProgram(pnm);
  if (!prog) return;
  
  // check if a prog already running
  //  Program::RunState grs = Program::GetGlobalRunState();
  /* TEMP ignore global run state, assume synchronous dispatch
   if (!((grs == Program::DONE) || (grs == Program::NOT_INIT))) {
   SendError("RunProgram " + pnm + ": program already running");
   return;
   }
   */
  // check that not already running! (but ok if it is Stopped)
  Program::RunState rs = prog->run_state;
  if (rs == Program::RUN) {
    SendError("RunProgram " + pnm + ": is already running", TemtClient::RUNTIME);
    return;
  }
  
  // check that it is initialized, otherwise call Init
  if (rs == Program::NOT_INIT) {
    //NOTE: Init is synchronous
    prog->Init();
    if (prog->ret_val != Program::RV_OK) {
      SendError("RunProgram " + pnm + "->Init() failed with ret_val: "
                + String(prog->ret_val), TemtClient::RUNTIME);
      return;
    }
  }
  
  // run
  if (sync) {
    prog->Run();
    //TEMP: only way it can't be DONE is if a runtime error occurred
    if (prog->run_state != Program::DONE) {
      SendError("RunProgram " + pnm + "->Run() failed due to a runtime error", TemtClient::RUNTIME);
      return;
    }
    // /TEMP
    if (prog->ret_val != Program::RV_OK) {
      SendError("RunProgram " + pnm + "->Run() failed with ret_val: "
                + String(prog->ret_val), TemtClient::RUNTIME);
      return;
    }
  } else { // async
    adapter()->SetProg(prog);
    QTimer::singleShot(0, adapter(), SLOT(prog_Run()));
  }
  SendOk();
}

void TemtClient::cmdSetData() {
  if (msg_format == TemtClient::NATIVE) {
    SendError("SetData only implemented for JSON");
  }
  
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON) {
    String tnm = name_params.GetVal("table").toString();
    DataTable* tab = GetAssertTable(tnm);
    if (!tab) return;
    
    if (tableData.empty()) {
      SendError("Missing parameter - 'data'", TemtClient::MISSING_PARAM);
      return;
    }
    
    int row_from, rows;
    int row_to = 0;
    bool valid = CalcRowParams("Set", tab, row_from, rows, row_to); // get start row and number of rows to get, remove, etc.
    if (!valid)
      return;
    
    int cell = 0;
    if (!name_params.GetVal("cell").isNull()) {
      cell = name_params.GetVal("cell").toInt();
    }
    bool create = false;  // default - don't create new columns
    if (!name_params.GetVal("create").isNull()) {
      create = name_params.GetVal("create").toBool();
    }
    
    if (!ValidateJSON_HasMember(tableData, "columns")) {  // first check columns existence
      SendError("Columns member not found in data", TemtClient::RUNTIME);
      return;
    }
    
    if (create == false) {  // check that columns exist
      if (!ValidateJSON_ColumnNames(tab, tableData)) {
        return;
      }
    }
    
    bool result = tab->SetDataFromJSON(tableData, row_from, cell);  // row -1 for append
    if (result) {
      SendOk();
    }
    else {
      SendError("SetData: " + tab->json_error_msg, TemtClient::RUNTIME);
    }
  }
#endif
}

Program* TemtClient::GetAssertProgram(const String& pnm) {
  // does many checks, to make sure the prog exists
  if (pnm.empty()) {
    SendError("program name expected", TemtClient::NOT_FOUND);
    return NULL;
  }
  
  // make sure project
  taProject* proj = GetCurrentProject();
  if (!proj) {
    SendError("no project open", TemtClient::NOT_FOUND);
    return NULL;
  }
  
  Program* prog = NULL;
  //Check if program name is a path
  if (pnm.startsWith(".")) {
    MemberDef* md = NULL;
    taBase * obj = NULL;
    if (pnm.startsWith(".projects")) {
      obj = tabMisc::root->FindFromPath(pnm, md);
    } else if (pnm.startsWith(".programs")) {
      obj = proj->FindFromPath(pnm, md);
    } else {
      obj = proj->programs.FindFromPath(pnm, md);
    }
    if(obj) {
      if(obj->InheritsFrom(&TA_Program)) {
        prog = (Program*)obj;
      } else {
        SendError("Path '" + pnm + "' does not point to a Program", TemtClient::NOT_FOUND);
        return NULL;
      }
    } else {
      SendError("Program '" + pnm + "' not found", TemtClient::NOT_FOUND);
      return NULL;
    }
  } else { //Normal program name
    // get program, make sure exists
      prog = proj->programs.FindLeafName(pnm);
  }
  if (!prog) {
    SendError("Program '" + pnm + "' not found", TemtClient::NOT_FOUND);
    return NULL;
  }
  return prog;
}

DataTable* TemtClient::GetAssertTable(const String& nm) {
  // does many checks, to make sure the table and prog exists
  if (nm.empty()) {
    SendError("table name expected", TemtClient::MISSING_PARAM);
    return NULL;
  }
  
  // make sure project
  taProject* proj = GetCurrentProject();
  if (!proj) {
    SendError("no project open", TemtClient::NO_OPEN_PROJECT);
    return NULL;
  }
  
  DataTable* tab = NULL;
  // if a local table, then resolve
  String pnm = nm.before(".");
  if (pnm.empty()) {
    // global table
    tab = proj->data.FindLeafName(nm);
    
    if (!tab) {
      taBase* obj = NULL;
      MemberDef* md = NULL;
      if (nm.startsWith(".projects")) {
        obj = tabMisc::root->FindFromPath(nm, md);
      }
      else {
        obj = proj->FindFromPath(nm, md);
      }
      if(obj) {
        if(obj->InheritsFrom(&TA_DataTable)) {
          tab = (DataTable*)obj;
        } else {
          SendError("Path '" + nm + "' does not point to a DataTable", TemtClient::NOT_FOUND);
          return NULL;
        }
      }
    }
  }
  else { // local table
    Program* prog = GetAssertProgram(pnm);
    if (!prog) return NULL; //note: will already have sent error
    String tnm = nm.after(".");
    tab = dynamic_cast<DataTable*>(prog->objs.FindName(tnm));
  }
  if (!tab) {
    SendError("Table '" + nm + "' not found", TemtClient::NOT_FOUND);
    return NULL;
  }
  return tab;
}

bool TemtClient::TableParams::ValidateParams(TemtClient::TableParams::Cmd cmd, bool mat_) {
  // defaults for values -- some of the guys below alter these, before we read them
  rows = 1;
  header = false;
  markers = false;
  
  // cmd decodes
  bool get = false;
  bool get_set = false;
  bool remove = false;
  bool append = false;
  bool is_cell = false;
  switch (cmd) {
    case TemtClient::TableParams::Get:
      get = true;
      get_set = true;
      break;
    case TemtClient::TableParams::Append:
      append = true;
      header = true;
      break;
    case TemtClient::TableParams::Remove:
      rows = -1;
      remove = true;
      break;
    case TemtClient::TableParams::Set:
      get_set = true;
      break;
    case TemtClient::TableParams::Cell:
      is_cell = true;
      mat = mat_;
      cell = 0;
      break;
  }
  
  // cell guys are pos params, so do them, and bail
  // also, they use uniony guys
  if (is_cell) {
    bool ok = true;
    // get col, row, and cell params
    col = tc->pos_params.SafeEl(1).toInt(&ok);
    if (col < 0) col = tab->cols() + col;
    // check in bounds
    if (!ok || (col < 0) || (col >= tab->cols())) {
      tc->SendError("col '" + String(col) + "' out of bounds");
      return false;
    }
    
    row = tc->pos_params.SafeEl(2).toInt(&ok);
    if (row < 0) row = tab->rows + row;
    if (!ok || (row < 0) || (row >= tab->rows)) {
      tc->SendError("row '" + String(row) + "' out of bounds");
      return false;
    }
    
    if (mat)
      cell = tc->pos_params.SafeEl(3).toInt(&ok);
    if (!ok || (cell < 0) || (cell >= tab->data.FastEl(col)->cell_geom.Product())) {
      tc->SendError("cell '" + String(cell) + "' out of bounds");
      return false;
    }
    return true;
  }
  
  // all the possible params -- not all defined for all cmds
  rows = tc->name_params.GetValDef("rows", rows).toInt();
  row_from = tc->name_params.GetValDef("row_from", 0).toInt();
  row_to = tc->name_params.GetValDef("row_to", -1).toInt();
  col_from = tc->name_params.GetValDef("col_from", 0).toInt();
  col_to = tc->name_params.GetValDef("col_to", -1).toInt();
  header = tc->name_params.GetValDef("header", header).toBool();
  markers = tc->name_params.GetValDef("markers", markers).toBool();
  
  // init vals that need to be correctly set even if validation fails
  if (header) lines = 1; else lines = 0;
  
  // validate and normalize the params
  if (row_from < 0) row_from = tab->rows + row_from;
  if (row_to < 0) row_to = tab->rows + row_to;
  if (get_set) {
    if ((row_from < 0) || (row_from >= tab->rows)) {
      tc->SendError("row_from '" + String(row_from) + "' out of bounds");
      return false;
    }
    
    if ((row_to < 0) || (row_to >= tab->rows)) {
      tc->SendError("row_to '" + String(row_to) + "' out of bounds");
      return false;
    }
  }
  
  if (append) {
    if (rows < 0) {
      tc->SendError("rows '" + String(rows) + "' out of bounds");
      return false;
    }
    row_from = tab->rows;
    row_to = (row_from + rows) - 1;
  }
  
  if (remove) {
    // note: only ok for row_from to be out of bounds if empty
    if ((row_from < 0) && (tab->rows > 0)) {
      tc->SendError("row_from '" + String(row_from) + "' out of bounds");
      return false;
    }
    if (rows < 0) {
      rows = tab->rows;
    }
    // adjust rows to be true rows
    if ((row_from + rows) > tab->rows)
      rows = tab->rows - row_from;
    return true; // skip all the other stuff
  }
  
  // make sure lines is valid now, before possibly bailing on other conditions
  lines += (row_to - row_from) + 1;
  
  if (col_from < 0) col_from = tab->cols() + col_from;
  if ((col_from < 0) || (col_from >= tab->cols())) {
    tc->SendError("col_from '" + String(col_from) + "' out of bounds");
    return false;
  }
  
  if (col_to < 0) col_to = tab->cols() + col_to;
  if ((col_to < 0) || (col_to >= tab->cols())) {
    tc->SendError("col_to '" + String(col_to) + "' out of bounds");
    return false;
  }
  
  return true;
}

void TemtClient::cmdAppendData() {
  if (msg_format == TemtClient::NATIVE) {
    String tnm = pos_params.SafeEl(0);
    name_params.SetVal("table", tnm);
  }
  
  String tnm = name_params.GetVal("table").toString();
  DataTable* tab = GetAssertTable(tnm);
  if (!tab)
    return;
  
  // note: ok if running
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON) {
    bool result = false;
    if (!ValidateJSON_HasMember(tableData, "columns")) {  // first check columns existence
      SendError("Columns member not found in data", TemtClient::RUNTIME);
      return;
    }
    if (!ValidateJSON_ColumnNames(tab, tableData)) {  // next check column names
      return;  // send error done by validator
    }
    
    result = tab->SetDataFromJSON(tableData, -1);  // true for append
    if (result) {
      SendOk();
    }
    else {
      SendError("AppendData: " + tab->json_error_msg, TemtClient::RUNTIME);
    }
  }
  else {
#endif
    TableParams p(this, tab);
    bool cmd_ok = p.ValidateParams(TemtClient::TableParams::Append);
    //NOTE: p will have already sent an ERROR if cmd_ok is false
    // any subsequent failure must send an ERROR
    
    
    // ok, now we must block/waiting until all expected lines received
    // note: some of the expected lines may already be in lines buffer
    setState(CS_DATA_IN);
    
    while ((lines.size < p.lines) && (state != CS_DISCONNECTED)) {
      // note: signals not invoked again inside this event loop
      if (sock->canReadLine())
        sock_readyRead();
      else
        sock->waitForReadyRead();
    }
    
    if (state == CS_DISCONNECTED)
      return;
    
    setState(CS_READY);
    
    if (!cmd_ok) { // error of some occurred, so we just accepted lines
      // must remove first n lines
      while ((p.lines > 0) && (lines.size > 0)) {
        lines.RemoveIdx(0);
        p.lines--;
      }
      return;
    }
    
    // trivial case with no lines
    if (p.lines == 0) goto ok_exit;
    
    {
      //note: we must have enough lines to have made it here
      tab->StructUpdate(true);
      int ln_num = 0; // to do header/init stuff
      while ((p.lines > 0) && (lines.size > 0)) {
        String ln;
        //note: parser needs the eol, so we have to add it back in
        ln = lines.FastEl(0) + '\n';
        lines.RemoveIdx(0);
        p.lines--;
        // simplest way to be compat with arcane DataTable load api
        // is to tack the markers on if we aren't expecting them
        if (!p.markers) {
          if (p.header && (ln_num == 0))
            ln = "_H:\t" + ln;
          else ln = "_D:\t" + ln;
        }
        // note: making a new one each loop simplifies things
        istringstream istr(ln.chars());
        
        tab->LoadDataRowEx_strm(istr, DataTable::TAB,
                                true, // quote_str
                                (ln_num == 0) // clear load schema on first guy
                                );
        ++ln_num;
      }
      tab->StructUpdate(false);
    }
  ok_exit:
    // ok, we did!
    SendOk();
#if (QT_VERSION >= 0x050000)
  }
#endif
}

void TemtClient::cmdGetData() {
  if (msg_format == TemtClient::NATIVE) {
    String tnm = pos_params.SafeEl(0);
    name_params.SetVal("table", tnm);
  }
  
  String tnm = name_params.GetVal("table").toString();
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON) {
    String col_name = "";
    
    if (!name_params.GetVal("column").isNull()) {
      col_name = name_params.GetVal("column").toString();
    }
    
    int row_from, rows;
    int row_to = 0;
    bool valid = CalcRowParams("Get", tab, row_from, rows, row_to); // get start row and number of rows to get, remove, etc.
    if (!valid)
      return;
    
    QJsonObject json_root_obj;
    bool result = tab->GetDataAsJSON(json_root_obj, col_name, row_from, rows);
    if (result) {
      // actual result object inserted by call on the data table
      json_root_obj.insert("status", QString("OK"));
      QJsonDocument json_doc(json_root_obj);
      QByteArray theString = json_doc.toJson(json_format);
      Write(theString.data());
    }
    else {
      SendError("GetData: " + tab->json_error_msg, TemtClient::RUNTIME);
    }
  }
  else {
#endif
    TableParams p(this, tab);
    if (!p.ValidateParams()) return;
    
    // ok, we can do it!
    SendOk("lines=" + String(p.lines));
    
    ostringstream ostr;
    String ln;
    
    // header row, if any
    if (p.header) {
      tab->SaveHeader_strm(ostr, DataTable::TAB, p.markers);
      ln = ostr.str().c_str();
      Write(ln);
    }
    
    // rows
    for (int row = p.row_from; row <= p.row_to; ++row) {
      ostr.str("");
      ostr.clear();
      tab->SaveDataRow_strm(ostr, row, DataTable::TAB,
                            true, // quote_str
                            p.markers, // row_mark
                            p.col_from, p.col_to);
      ln = ostr.str().c_str();
      Write(ln);
    }
#if (QT_VERSION >= 0x050000)
  }
#endif
}

void TemtClient::cmdGetDataCell() {
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON) {
    SendError("For JSON use GetData, specify column and row", TemtClient::UNSPECIFIED);
  }
#endif
  
  String tnm = pos_params.SafeEl(0);
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  TableParams p(this, tab);
  if (!p.ValidateParams(TableParams::Cell, false)) return;
  
  cmdGetDataCell_impl(p);
}

void TemtClient::cmdGetDataMatrixCell() {
  if (msg_format == TemtClient::NATIVE) {
    String tnm = pos_params.SafeEl(0);
    name_params.SetVal("table", tnm);
  }
  
  String tnm = name_params.GetVal("table").toString();
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON) {
    String col_name = name_params.GetVal("column").toString();
    int row_from = name_params.GetVal("row_from").toInt();
    int cell = name_params.GetVal("cell").toInt();
    
    QJsonObject json_root_obj;
    bool result = tab->GetDataMatrixCellAsJSON(json_root_obj, col_name, row_from, cell);
    if (result) {
      // actual result object inserted by call on the data table
      json_root_obj.insert("status", QString("OK"));
      QJsonDocument json_doc(json_root_obj);
      QByteArray theString = json_doc.toJson(json_format);
      Write(theString.data());
    }
    else {
      SendError("GetDataMatrixCell: " + tab->json_error_msg, TemtClient::RUNTIME);
    }
  }
  else {
#endif
    TableParams p(this, tab);
    if (!p.ValidateParams(TableParams::Cell, true))
      return;
    cmdGetDataCell_impl(p);
#if (QT_VERSION >= 0x050000)
  }
#endif
}

void TemtClient::cmdGetDataCell_impl(TableParams& p) {
  Variant v = p.tab->GetMatrixFlatVal(p.col, p.row, p.cell);
  String res;
  if (v.isStringType())
    res = String::StringToCppLiteral(v.toString());
  else res = v.toString();
  return SendOk(res);
}

void TemtClient::cmdSetDataCell() {  // for json just like cmdSetData with fixed row count of 1
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON) {
    String tnm = name_params.GetVal("table").toString();
    DataTable* tab = GetAssertTable(tnm);
    if (!tab) return;
    
    int row = name_params.GetVal("row_from").toInt();
    tab->SetDataFromJSON(tableData, row, 1);  // 1 is total number of rows to set
    SendOk();
  }
#endif
  
  String tnm = pos_params.SafeEl(0);
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  TableParams p(this, tab);
  if (!p.ValidateParams(TableParams::Cell, false)) return;
  
  cmdSetDataCell_impl(p);
}

void TemtClient::cmdSetDataMatrixCell() {
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON) {
    SendError("For JSON use SetData, specify row and cell and send one value");
    return;
  }
#endif
  
  String tnm = pos_params.SafeEl(0);
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  TableParams p(this, tab);
  if (!p.ValidateParams(TableParams::Cell, true)) return;
  
  cmdSetDataCell_impl(p);
}

void TemtClient::cmdSetDataCell_impl(TableParams& p) {
  int param = p.mat ? 4 : 3;
  if (param >= pos_params.size) {
    SendError("not enough params");
    return;
  }
  String val = pos_params.FastEl(param);
  p.tab->SetMatrixFlatVal(val, p.col, p.row, p.cell);
  SendOk();
}

void TemtClient::cmdGetVar() {
  if (msg_format == TemtClient::NATIVE) {
    String pnm = pos_params.SafeEl(0);
    name_params.SetVal("program", pnm);
  }
  
  String pnm = name_params.GetVal("program").toString();
  if (pnm.empty()) {
    SendError("Missing parameter - 'Program'", TemtClient::MISSING_PARAM);
    return;
  }
  
  Program* prog = GetAssertProgram(pnm);
  if (!prog) return;
  // note: ok if running
  
  if (msg_format == TemtClient::NATIVE) {
    // 2nd param must be a var name
    String str = pos_params.SafeEl(1);;
    name_params.SetVal("var_name", str);
  }
  
  String nm = name_params.GetVal("var_name").toString();
  
  // note: check name first, because GetVar raises error
  if (!prog->HasVar(nm)) {
    SendError("Var '" + nm + "' not found", TemtClient::NOT_FOUND);
    return;
  }
  ProgVar* var = prog->FindVarName(nm);
  if (!var) { // shouldn't happen
    SendError("Var '" + nm + "' could unexpectedly not be retrieved", TemtClient::NOT_FOUND);
    return;
  }
  
  if (msg_format == TemtClient::NATIVE) {
    String val;
    // get the value, possibly converting
    if (var->var_type == ProgVar::T_String) {
      val = String::StringToCppLiteral(var->string_val);
    }
    else {
      val = var->GetVar().toString();
    }
    // send message
    SendOk(val);
  }

#if (QT_VERSION >= 0x050000)
  // Send the message from here because the result is not in JSON format yet
  else if (msg_format == TemtClient::JSON) {
    bool error = false;
    QJsonObject json_root_obj;
    json_root_obj.insert("status", QString("OK"));
    switch (var->var_type) {
      case ProgVar::T_Bool:
        json_root_obj.insert("result", QJsonValue(var->bool_val));
        break;
      case ProgVar::T_Int:
        json_root_obj.insert("result", QJsonValue(var->int_val));
        break;
      case ProgVar::T_Real:
        json_root_obj.insert("result", QJsonValue(var->real_val));
        break;
      case ProgVar::T_String:
        json_root_obj.insert("result", QJsonValue(QString(var->string_val.chars())));
        break;
      case ProgVar::T_HardEnum:
	json_root_obj.insert("result", QJsonValue(var->int_val));
        break;
      case ProgVar::T_DynEnum:
        json_root_obj.insert("result", QJsonValue(var->dyn_enum_val.value));
        break;
      case ProgVar::T_Object:
        SendErrorJSON("Program variable is an object pointer", TemtClient::RUNTIME);
        error = true;
        break;
      default:
        SendErrorJSON("ProgVar type unknown - report as bug", TemtClient::RUNTIME);
        error = true;
        break;
    }
    if (!error) {
      QJsonDocument json_doc(json_root_obj);
      QByteArray theString = json_doc.toJson(json_format);
      Write(theString.data());
    }
  }
#endif
}

void TemtClient::cmdGetRunState() {
  int run_state = 0; // temp
  
  String name = name_params.GetVal("program").toString();
  if (pos_params.size == 0 && name.empty()) {
    // global version
    run_state = Program::global_run_state;
  }
  else {
    // program version
    if (msg_format == TemtClient::NATIVE) {
      String pnm = pos_params.SafeEl(0);
      name_params.SetVal("program", pnm);
    }
    String pnm = name_params.GetVal("program").toString();
    Program* prog = GetAssertProgram(pnm);
    if (!prog) return;
    run_state = prog->run_state;
  }

  SendOk(String(run_state));
}

void TemtClient::cmdRemoveData() {
  if (msg_format == TemtClient::NATIVE) {
    String tnm = pos_params.SafeEl(0);
    name_params.SetVal("table", tnm);
  }
  
  String tnm = name_params.GetVal("table").toString();
  DataTable* tab = GetAssertTable(tnm);
  if (!tab)
    return;
  // note: ok if running
  
  if (msg_format == TemtClient::NATIVE) {
    TableParams p(this, tab);
    bool cmd_ok = p.ValidateParams(TemtClient::TableParams::Remove);
    if (!cmd_ok) return;
    
    // ok if none; noop if start > rows
    if ((p.row_from >= 0) && (p.row_from < tab->rows)) {
      if (!tab->RemoveRows(p.row_from, p.rows)) {
        SendError("RemoveRows command on table '" + tnm + "' did not succeed", TemtClient::RUNTIME);
        return;
      }
    }
    SendOk();
  }
  else if (msg_format == TemtClient::JSON) {
    int row_from, rows;
    int row_to = 0;
    bool valid = CalcRowParams("remove", tab, row_from, rows, row_to); // get start row and number of rows to get, remove, etc.
    if (valid) {
      if (!tab->RemoveRows(row_from, rows)) {
        SendError("RemoveRows command on table '" + tnm + "' did not succeed", TemtClient::RUNTIME);
        return;
      }
      else {
        SendOk();
      }
    }
  }
}

void TemtClient::cmdSetVar() {
  if (msg_format == TemtClient::NATIVE) {
    String pnm = pos_params.SafeEl(0);
    
    Program* prog = GetAssertProgram(pnm);
    if (!prog) return;
    // note: ok if running
    // note: would work for 0 params
    
    // verify all params
    String nm;
    for (int i = 0; i < name_params.size; ++i) {
      nm = name_params.FastEl(i).name;
      // note: check name first, because GetVar raises error
      if (!prog->HasVar(nm)) {
        SendError("Var '" + nm + "' not found", TemtClient::NOT_FOUND);
        return;
      }
      // check if type ok to set -- assume it will be found since name is ok
      ProgVar* var = prog->FindVarName(nm);
      if (!var) continue; // shouldn't happen, but should get caught next stage
      if (var->var_type == ProgVar::T_Object) {
        SendError("Var '" + nm + "' is an Object--setting is not supported", TemtClient::RUNTIME);
        return;
      }
    }
    
    // set
    for (int i = 0; i < name_params.size; ++i) {
      NameVar& nv = name_params.FastEl(i);
      if (!prog->SetVar(nv.name, nv.value)) {
        SendError("An error occurred while seeting Var or Arg '" + nm + "'", TemtClient::RUNTIME);
        return;
      }
    }
    SendOk("vars set");
  }
  
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON) {
    String pnm = name_params.GetVal("program").toString();
    Program* prog = GetAssertProgram(pnm);
    if (!prog)
      return;
    
    String var_name = name_params.GetVal("var_name").toString();
    if (var_name.empty()) {
      SendError("var_name missing", TemtClient::NOT_FOUND);
      return;
    }
    if (!prog->HasVar(var_name)) {
      SendError("Var '" + var_name + "' not found", TemtClient::NOT_FOUND);
      return;
    }
    
    // check if type ok to set -- assume it will be found since name is ok
    ProgVar* var = prog->FindVarName(var_name);
    if (var->var_type == ProgVar::T_Object) {
      SendError("Var '" + var_name + "' is an Object--setting is not supported", TemtClient::RUNTIME);
      return;
    }
    
    // set
    String var_value = name_params.GetVal("var_value").toString();
    if (var_value.empty()) {
      SendError("var_value missing", TemtClient::NOT_FOUND);
      return;
    }
    if (!prog->SetVar(var_name, var_value)) {
      SendError("An error occurred while seeting Var or Arg '" + var_name + "'", TemtClient::RUNTIME);
      return;
    }
    SendOk();
  }
#endif
}

taProject* TemtClient::GetCurrentProject() {
  if (!cur_proj)
    cur_proj = tabMisc::root->projects.SafeEl(0);
  return cur_proj.ptr();
}

void TemtClient::ParseCommand(const String& cl) {
  name_params.Reset(); // used by native and json parsers to store params
  cmd_line = trim(cl); // remove leading/trailing ws, including eol's
  
  if (cmd_line[0] == '{') {
#if (QT_VERSION >= 0x050000)
    msg_format = TemtClient::JSON;
    ParseCommandJSON(cmd_line);
#else
    taMisc::Warning("JSON requires Qt version >= 5.0");
#endif
    return;
  }
  else {
    msg_format = TemtClient::NATIVE;
    ParseCommandNATIVE(cmd_line);
  }
}

// (jtr 2/22/14) it would be nice to be able to store the position params
// as name params during the parse but the positional information
// isn't available until the cmd is called and
// I didn't want to put that logic into this method
void TemtClient::ParseCommandNATIVE(const String& cmd_string) {
  cmd = _nilString;
  pos_params.Reset(); // used by native parser only
  
  // we use the pos_params as an intermediate guy during processing...
  //TEMP
  // TODO: need to properly parse, manually, each guy, to make sure, ex. that
  // strings are pulled intact, that a quoted string with a = in it doesn't get
  // pulled as a name= etc. etc.
  
  // note: following from Qt help file
  //TODO: following not correct, need to manually deal with "" and remove ""
  String str = cmd_string;
  int p = 0;
  bool err = false;
  // we process cmd in two steps:
  // 1) we just put each token in the pos list
  // 2) then we process stuff
  String tok = NextToken(str, p, err);;
  while (tok.nonempty() && !err) {
    pos_params.Add(tok);
    tok = NextToken(str, p, err);;
  }
  
  // TODO: check err
  
  // pull 1st guy as the command
  if (pos_params.size > 0) {
    cmd = pos_params.FastEl(0);
    pos_params.RemoveIdx(0);
  }
  
  // pull guys w/ = as name=value, and they must all be after starting =
  int i = 0;
  int got_name_val = false;
  String item;
  String next_item;
  while (i < pos_params.size) {
    item = pos_params.FastEl(i);
    next_item = pos_params.SafeEl(i + 1);
    if (next_item == "=") {
      got_name_val = true; // they must all be from here on
      String val = pos_params.SafeEl(i + 2);
      name_params.SetVal(item, val); // note: could conceivably be a duplicate
      // remove the 3 items -- first 2 are certainly present
      pos_params.RemoveIdx(i); // name
      pos_params.RemoveIdx(i); // =
      if (i < pos_params.size)
        pos_params.RemoveIdx(i); // value
    } else {
      if (got_name_val) { // were expecting all the rest to be n=v guys
        SendError("all remaining params were expected to be name=value but found: " + item);
        return;
      }
      ++i;
    }
  }
  
  if (cmd.length() == 0) {
    // empty lines just echoed
    WriteLine(_nilString);
    return;
  }
  
  RunCommand(cmd);
}

#if (QT_VERSION >= 0x050000)
void TemtClient::ParseCommandJSON(const String& cmd_string) {
  QJsonParseError json_error;
  QString q_string = cmd_string.toQString();
  QJsonDocument json_doc = QJsonDocument::fromJson(q_string.toUtf8(), &json_error);  // converts to QByteArray
  
  if (json_error.error != QJsonParseError::NoError) {
    SendErrorJSON("JSON format error", TemtClient::INVALID_FORMAT);
    return;
  }
  else
  {
    QJsonObject json_obj = json_doc.object();
    QJsonObject::const_iterator obj_iter = json_obj.constBegin();
    
    while (obj_iter != json_obj.constEnd()) {
      QString node_name = obj_iter.key();
      if (node_name == "command") {
        name_params.SetVal("command", obj_iter.value().toString());
      }
      else if (node_name == "program") {
        name_params.SetVal("program", obj_iter.value().toString()); // program name
      }
      else if (node_name == "var_name") {
        name_params.SetVal("var_name", obj_iter.value().toString()); // variable name
      }
      else if (node_name == "var_value") {
        name_params.SetVal("var_value", obj_iter.value().toVariant()); // variable name
      }
      else if (node_name == "table") {
        name_params.SetVal("table", obj_iter.value().toString());  // table name
      }
      else if (node_name == "data") {
        tableData = obj_iter.value().toObject();  // json string of data table data
      }
      else if (node_name == "column") {
        name_params.SetVal("column", obj_iter.value().toString());  // a single column name - for get only - set controlled through "data"
      }
      else if (node_name == "row_from") {
        name_params.SetVal("row_from", obj_iter.value().toInt());  // first row to get/set
      }
      else if (node_name == "row_to") {
        name_params.SetVal("row_to",obj_iter.value().toInt());  // last row to get/set
      }
      else if (node_name == "rows") {
        name_params.SetVal("rows", obj_iter.value().toInt());  // count of rows to operate on (get, remove) - for set count is number of values sent
      }
      else if (node_name == "cell") {
        name_params.SetVal("cell", obj_iter.value().toInt());  // first cell to get/set - based on flat indexing
      }
      else if (node_name == "create") {
        name_params.SetVal("create", obj_iter.value().toBool());  // ok to create new columns - default is no
      }
      else if (node_name == "image_data") {
        name_params.SetVal("image_data", obj_iter.value().toString());  // ok to create new columns - default is no
      }
      else if (node_name == "enable") {
        name_params.SetVal("enable", obj_iter.value().toBool());  // enable or disable
      }
      else if (node_name == "json_format") {
        name_params.SetVal("json_format", obj_iter.value().toString());  // enable or disable
      }
      else if (node_name == "project") {
        name_params.SetVal("project", obj_iter.value().toString());  // enable or disable
      }
      else if (node_name == "path") {
          name_params.SetVal("path", obj_iter.value().toString());  // path to a taBase object
      }
      else if (node_name == "member") {
          name_params.SetVal("member", obj_iter.value().toString());  // member name
      }

      else {
        String err_msg = "Unknown parameter: " + node_name;
        SendErrorJSON(err_msg, TemtClient::UNKNOWN_PARAM);
        return;  // abort - force client to fix before we get into trouble
      }
      ++obj_iter;
    }
    
    String cmd = name_params.GetVal("command").toString();
    RunCommand(cmd);
  }
}
#endif

void TemtClient::RunCommand(const String& cmd) {
  if (cmd == "AppendData") {
    cmdAppendData();
  }
  else if (cmd == "Echo") {
    cmdEcho();
  }
  else if (cmd == "GetData") {
    cmdGetData();
  }
  else if (cmd == "GetDataCell") {
    cmdGetDataCell();
  }
  else if (cmd == "GetDataMatrixCell") {
    cmdGetDataMatrixCell();
  }
  else if (cmd == "GetVar") {
    cmdGetVar();
  }
  else if (cmd == "GetRunState") {
    cmdGetRunState();
  }
  else if (cmd == "RemoveData") {
    cmdRemoveData();
  }
  else if (cmd == "RunProgram") {
    cmdRunProgram(true);
  }
  else if (cmd == "RunProgramAsync") {
    cmdRunProgram(false);
  }
  else if (cmd == "SetData") {
    cmdSetData();
  }
  else if (cmd == "SetDataCell") {
    cmdSetDataCell();
  }
  else if (cmd == "SetDataMatrixCell") {
    cmdSetDataMatrixCell();
  }
  else if (cmd == "SetVar") {
    cmdSetVar();
  }
  else if (cmd == "SetImage") {
    cmdSetImage();
  }
  else if (cmd == "GetConsoleOutput") {
    cmdGetConsoleOutput();
  }
  else if (cmd == "CollectConsoleOutput") {
    cmdCollectConsoleOutput();
  }
  else if (cmd == "ClearConsoleOutput") {
    cmdClearConsoleOutput();
  }
  else if (cmd == "OpenProject") {
    cmdOpenProject();
  }
#if (QT_VERSION >= 0x050000)
  else if (cmd == "SetJsonFormat") {
    cmdSetJsonFormat();
  }
  else if (cmd == "GetMember") {
      cmdGetMember();
  }
#endif
  else
  {
    String err_msg = "Unknown command: " + cmd + "-- remember everything is case sensitive";
    if (msg_format == TemtClient::NATIVE) {
      SendError(err_msg);
      
    }
#if (QT_VERSION >= 0x050000)
    else {
      SendErrorJSON(err_msg, TemtClient::UNKNOWN_COMMAND);
    }
#endif
  }
  
  //TODO: we can (should!) look up and dispatch via name!
  /*  if (cmd == "CloseProject") {
   cmdCloseProject();
   } */
}

void TemtClient::SetSocket(QTcpSocket* sock_) {
  sock = sock_;
  QObject::connect(sock_, SIGNAL(disconnected()),
                   adapter(), SLOT(sock_disconnected()));
  QObject::connect(sock_, SIGNAL(readyRead()),
                   adapter(), SLOT(sock_readyRead()));
  QObject::connect(sock_, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                   adapter(), SLOT(sock_stateChanged(QAbstractSocket::SocketState)));
}

void TemtClient::setState(ClientState cs) {
  if (state == cs) return;
  //ADD ANY TRANSITION STUFF HERE
  state = cs;
}

void TemtClient::sock_disconnected() {
  state = CS_DISCONNECTED;
  sock = NULL; // goodbye...
  cmdCloseProject();
  if (server) server->ClientDisconnected(this);
  //NO MORE CODE: we will be deleted!
}

void TemtClient::sock_readyRead() {
  if (!sock) return;
  // we only do lines -- will call us again when a line is ready
  if (!sock->canReadLine()) return;
  
  QByteArray ba;
  String line;
  // need to keep fetching lines, since we only get notified once
  // note: typical case is we get one full line at a time
  bool ok = true; // in case reading goes daft
  while (ok && sock->canReadLine()) {
    ba = sock->readLine();
    qint64 line_len = ba.size();
    if (line_len <= 0) {
      ok = false;
      break;
    }
    line.set(ba.data(), (int)line_len); // note: includes \n or platform separator
    // strip cr/lf chars for client platform neutrality
    line.gsub('\n', "");
    line.gsub('\r', "");
    lines.Add(line);
  }
  HandleLines();
}

void TemtClient::HandleLines() {
  // so, we've grabbed as many lines as we can now, so what do we do???
  // cases:
  // DATA_IN: if all received, then dispatch handler, else return
  // READY: we can pull first guy, and do a command
  while (lines.size > 0) {
    switch (state) {
      case CS_READY: {
        String line = lines.FastEl(0);
        lines.RemoveIdx(0);
        ParseCommand(line);
      } break;
      case CS_DATA_IN: {
        // we would have reached here re-entrantly, within a cmd, so just exit
        return;
      } break;
      case CS_DISCONNECTED: {
        //shouldn't happen!!!
      } break;
        //default:
        // handle all cases explicitly!
    };
  }
}

void TemtClient::sock_stateChanged(QAbstractSocket::SocketState socketState) {
  //nothing yet
}

void TemtClient::SendError(const String& err_msg, TemtClient::ServerError err) {
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON)
    SendErrorJSON(err_msg, err);
  else
#endif
    SendErrorNATIVE(err_msg);
}

void TemtClient::SendErrorNATIVE(const String& err_msg, TemtClient::ServerError err) {
  String ln = "ERROR";
  if (err_msg.length() > 0)
    ln.cat(" ").cat(err_msg);
  WriteLine(ln);
}

#if (QT_VERSION >= 0x050000)
void TemtClient::SendErrorJSON(const String& err_msg, TemtClient::ServerError err) {
  QJsonObject root_object = QJsonObject();
  root_object.insert("status", QString("ERROR"));
  root_object.insert("message", err_msg.toQString());
  root_object.insert("error", err);
  
  QJsonDocument json_doc(root_object);
  QByteArray theString = json_doc.toJson(json_format);  // status always indented format
  QString reply(theString);
  WriteLine(reply);
}
#endif

void TemtClient::SendOk(const String& msg) {
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON)
    SendOkJSON(msg);
  else
#endif
    SendOkNATIVE(msg);
}

void TemtClient::SendOkNATIVE(const String& msg) {
  String ln = "OK";
  if (msg.nonempty()) {
    ln.cat(" ").cat(msg);
  }
  WriteLine(ln);
}

#if (QT_VERSION >= 0x050000)
void TemtClient::SendOkJSON(const String& msg) {
  QJsonObject root_object = QJsonObject();
  root_object.insert("status", QString("OK"));
  if (!msg.empty()) {
    root_object.insert("result", msg.toQString());
  }
  
  QJsonDocument json_doc(root_object);
  QByteArray theString = json_doc.toJson(json_format);  // status always indented format
  QString reply(theString);
  WriteLine(reply);
}
#endif

void TemtClient::SendReply(const String& r) {
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON)
    SendErrorJSON(r);
  else
#endif
    SendErrorNATIVE(r);
}

void TemtClient::SendReplyNATIVE(const String& r) {
  WriteLine(r);
}

#if (QT_VERSION >= 0x050000)
void TemtClient::SendReplyJSON(const String& r) {
  WriteLine(r);
}
#endif

void TemtClient::Write(const String& txt) {
  if (!isConnected()) return;
  sock->write(txt.toQByteArray());
}

void TemtClient::WriteLine(const String& ln) {
  if (!isConnected()) return;
  String lnt = ln + "\n";
  sock->write(lnt.toQByteArray());
}

// used by json calls
bool TemtClient::CalcRowParams(String operation, DataTable* table, int& row_from, int& rows, int row_to) {
  bool row_from_set = false;  // did user pass the parameter
  bool row_to_set = false;    // did user pass the parameter
  
  row_from = 0;  // default to first row
  if (!name_params.GetVal("row_from").isNull()) {
    row_from = name_params.GetVal("row_from").toInt();
    row_from_set = true;
    bool in_range = table->RowInRangeNormalize(row_from);
    if (!in_range) {
      SendError("the parameter 'row_from' is out of range", TemtClient::RUNTIME);
      return false;
    }
  }
  
  row_to = table->rows; // default to last row
  if (!name_params.GetVal("row_to").isNull()) {
    row_to = name_params.GetVal("row_to").toInt();
    row_to_set = true;
    bool in_range = table->RowInRangeNormalize(row_to);
    if (!in_range) {
      SendError("the parameter 'row to' is out of range", TemtClient::RUNTIME);
      return false;
    }
    rows = row_to - row_from + 1;
  }
  
  if (row_to <= row_from) {
    SendError("the parameter 'row_to' is less than or equal to 'row_from'", TemtClient::RUNTIME);
    return false;
  }

  if (row_to_set) {
    if (!name_params.GetVal("rows").isNull()) {
      SendError("the parameter 'row_to' is already set - can't also set the parameter 'rows' -- use one or the other", TemtClient::RUNTIME);
      return false;
    }
  }
  else {
    if (!name_params.GetVal("rows").isNull()) {
      rows = name_params.GetVal("rows").toInt();
      if (rows <0) {
        SendError("the parameter 'rows' must be a positive integer value", TemtClient::RUNTIME);
        return false;
      }
      if (rows > table->rows - row_from) {
        SendError("the number of rows requested is greater than total rows or the number of rows minus 'row_from' if specified. To get all rows or all rows beyond 'row_from' don't specify 'rows'", TemtClient::RUNTIME);
        return false;
      }
    }
    else {
      rows = table->rows - row_from;  // default is row_from to the end
    }
  }
  
  
  // this is the only row oriented call and does not go through json api
  // so it requires a bit of upfront checking - The json data table code
  // returns error messages but the old row based data table calls do not
  // (jtr 3/31/14)
  if (operation == "remove" && table->rows == 0) {
    SendOk();
    return false;  // false because we won't actually execute the remove code
  }
  return true;
}

#if (QT_VERSION >= 0x050000)
bool TemtClient::ValidateJSON_HasMember(const QJsonObject& n, const String& member_name) {
  bool rval = true;
  bool has_member = false;
  QJsonObject::const_iterator i = n.constBegin();
  while (i != n.constEnd()){
    // recursively call ourselves to dig deeper into the tree
    if (i.value().isArray() || i.value().isObject()) {
      if (i.key() == member_name) {
        has_member = true;
        break;
      }
      rval = ValidateJSON_HasMember(i.value().toObject(), member_name);
      if (rval == false) {
        return false;
      }
    }
    ++i;
  }
  
  if (!has_member)
    return false;
  return rval;
}

bool TemtClient::ValidateJSON_ColumnNames(DataTable* dt, const QJsonObject& n) { // check for unknown column names
  bool rval = true;
  bool has_column_node = false;
  QJsonObject::const_iterator i = n.constBegin();
  while (i != n.constEnd()){
    // recursively call ourselves to dig deeper into the tree
    if (i.value().isArray() || i.value().isObject()) {
      if (i.key() == "columns") {
        has_column_node = true;
        break;
      }
      rval = ValidateJSON_ColumnNames(dt, i.value().toObject());
      if (rval == false) {
        return rval;
      }
    }
    ++i;
  }
  
  if (has_column_node) {
    QJsonArray::const_iterator columns = i.value().toArray().constBegin();
    while (columns != i.value().toArray().constEnd() && rval == true) {
      QJsonValue value = *columns;
      const QJsonObject aCol = value.toObject();
      rval = ValidateJSON_ColumnName(dt, aCol);
      if (!rval)
        break;
      columns++;
    }
  }
  return rval;
}

bool TemtClient::ValidateJSON_ColumnName(DataTable* dt, const QJsonObject& aCol) {
  String columnName("");
  
  QJsonObject::const_iterator columnData = aCol.constBegin();
  while (columnData != aCol.constEnd()) {
    String node_name = columnData.key();
    if (node_name == "name") {
      columnName = columnData.value().toString();
      break;
    }
    columnData++;
  }
  
  if (columnName.empty()) {
    SendError("Column member 'name' not found", TemtClient::RUNTIME);
    return false;
  }
  
  if (dt) { // should have been checked by now
    DataCol* dc = dt->data.FindName(columnName);
    if (dc) {
      return true;
    }
    else {
      SendError("Column name '" + columnName + "' not found in data table '" + dt->name + "'", TemtClient::RUNTIME);
      return false;
    }
  }
  return true;
}
#endif

void TemtClient::cmdSetImage() {
  if (msg_format == TemtClient::NATIVE) {
    SendError("SetImage only implemented for JSON");
  }
#if (QT_VERSION >= 0x050000)
  if (msg_format == TemtClient::JSON) {
    String pnm = name_params.GetVal("program").toString();
    Program* prog = GetAssertProgram(pnm);
    if (!prog)
      return;
    
    String var_name = name_params.GetVal("var_name").toString();
    if (var_name.empty()) {
      SendError("var_name missing", TemtClient::NOT_FOUND);
      return;
    }
    if (!prog->HasVar(var_name)) {
      SendError("Var '" + var_name + "' not found", TemtClient::NOT_FOUND);
      return;
    }
    
    bool good_result = false;
    taImage* img = NULL;
    ProgVar* var = prog->vars.FindName(var_name);
    if (var && var->object_type->DerivesFrom(&TA_taImage)) {
      img = dynamic_cast<taImage*>(var->object_val.ptr());
      if (img) {
        QString image_data = name_params.GetVal("image_data").toString();
        QByteArray ba;
        ba.append(image_data);
        good_result = img->LoadImageFromBase64(ba);
      }
    }
    
    if (good_result) {
      SendOk();
    }
    else {
      SendError("SetImage: failed to create image", TemtClient::RUNTIME);
    }
  }
#endif
}

void TemtClient::cmdGetConsoleOutput() {
  SendOk(taMisc::GetConsoleHold());
}

void TemtClient::cmdCollectConsoleOutput() {
  bool enable = false;  // don't collect is the default
  if (!name_params.GetVal("enable").isNull()) {
    enable = name_params.GetVal("enable").toBool();
    taMisc::SetConsoleHoldState(enable);
    SendOk();
  }
#if (QT_VERSION >= 0x050000)
  else {
    SendErrorJSON("enable param not found", TemtClient::MISSING_PARAM);
  }
#endif
}

void TemtClient::cmdClearConsoleOutput() {
  taMisc::ClearConsoleHold();
  SendOk();
}

#if (QT_VERSION >= 0x050000)
void TemtClient::cmdSetJsonFormat() {
  if (!name_params.GetVal("json_format").isNull()) {
    if (name_params.GetVal("json_format") == "compact") {
      json_format = QJsonDocument::Compact;
      SendOk();
    }
    else if (name_params.GetVal("json_format") == "indented") {
      json_format = QJsonDocument::Indented;
      SendOk();
    }
    else {
      SendErrorJSON("unexpected value - must be 'compact' or 'indented' ", TemtClient::NOT_FOUND);
    }
  }
  else {
    SendErrorJSON("json_format param not found", TemtClient::MISSING_PARAM);
  }
}
#endif

#if (QT_VERSION >= 0x050000)
void TemtClient::cmdGetMember() {
  if (!name_params.GetVal("path").isNull()) {
    String pnm = name_params.GetVal("path").toString();
    taBase * obj = NULL;
    MemberDef* md = NULL;
    if (pnm.startsWith(".projects")) {
      obj = tabMisc::root->FindFromPath(pnm, md);
    } else {
      taProject* proj = GetCurrentProject();
      obj = proj->FindFromPath(pnm, md);
    }
    
    if(obj) {
      if (!name_params.GetVal("member").isNull()) {
        md = obj->GetTypeDef()->members.FindName(name_params.GetVal("member").toString());
        if (!md) {
          SendErrorJSON("No member " + name_params.GetVal("member").toString() + " was found in " + name_params.GetVal("path").toString(), TemtClient::NOT_FOUND);
          return;
        }
        SendOk(md->GetValVar(obj).toString());
        return;
      } else {
        SendOk(obj->PrintStr());
        return;
      }
    } else {
      SendError("Path '" + pnm + "' not found", TemtClient::NOT_FOUND);
      return;
    }
    SendErrorJSON("Working on the implementation", TemtClient::NOT_FOUND);
  }
  else {
    SendErrorJSON("path param not found", TemtClient::MISSING_PARAM);
  }
}
#endif

