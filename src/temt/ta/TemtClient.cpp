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

#include <taMisc>
#include <tabMisc>
#include <taRootBase>

#include <QTcpSocket>
#include <QTimer>

#include <sstream>

#include <libjson>
#include <JSONNode>
#include "../json/JSONDefs.h"

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
  //TEMP
  SendError("OpenProject not implemented yet");
  return;
  
  //TODO: checks:
  // * cannot be one open (OR: auto close it???)
  // * maybe check if already open, ignore??? say ok??? error???
  
  String proj_file = pos_params.SafeEl(0); // always arg 1
  taBase* proj_ = NULL;
  //TODO: pathing???
  if (tabMisc::root->projects.Load(proj_file, &proj_)) {
    taProject* proj = dynamic_cast<taProject*>(proj_);
    if (proj) {
      if (taMisc::gui_active) {
        MainWindowViewer::NewProjectBrowser(proj);
      }
    }
    SendOk();
    return;
  } else {
    SendError("file \"" + proj_file + "\" not found");
  }
}

void TemtClient::cmdRunProgram(bool sync) {
  // store position param to name/value params - backwards compatibility
  if (msgFormat == TemtClient::NATIVE) {
    String pnm = pos_params.SafeEl(0);
    name_params.SetVal("program", pnm);
  }
  
  // now we are native/json agnostic
  String pnm = name_params.GetVal("program").toString();
  if (pnm.empty()) {
    SendError("RunProgram: program name expected", TemtClient::NOT_FOUND);
    return;
  }
  
  // make sure project
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
  if (msgFormat == TemtClient::NATIVE) {
    SendError("SetData only implemented for JSON");
  }
  if (msgFormat == TemtClient::JSON) {
    String tnm = name_params.GetVal("table").toString();
    DataTable* tab = GetAssertTable(tnm);
    if (!tab) return;
    
    int row = 0;
    if (!name_params.GetVal("row_from").isNull()) {
      row = name_params.GetVal("row_from").toInt();
    }
    int cell = 0;
    if (!name_params.GetVal("cell").isNull()) {
      cell = name_params.GetVal("cell").toInt();
    }
    bool result = tab->SetDataFromJSON(tableData, row, cell);  // row -1 for append
    if (result) {
      SendOk();
    }
    else {
      SendError("SetData: " + tab->error_msg, TemtClient::RUNTIME);
    }
  }
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
  
  // get program, make sure exists
  Program* prog = proj->programs.FindLeafName(pnm);
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
  } else { // local table
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
  if (msgFormat == TemtClient::NATIVE) {
    String tnm = pos_params.SafeEl(0);
    name_params.SetVal("table", tnm);
  }
  
  String tnm = name_params.GetVal("table").toString();
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  if (msgFormat == TemtClient::JSON) {
    bool result = tab->SetDataFromJSON(tableData, -1);  // true for append
    
    if (result) {
      SendOk();
    }
    else {
      SendError("AppendData: " + tab->error_msg, TemtClient::RUNTIME);
    }
  }
  else {
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
  }
}

void TemtClient::cmdGetData() {
  if (msgFormat == TemtClient::NATIVE) {
    String tnm = pos_params.SafeEl(0);
    name_params.SetVal("table", tnm);
  }
  
  String tnm = name_params.GetVal("table").toString();
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  if (msgFormat == TemtClient::JSON) {
    ostringstream ostr;
    String data;
    String col_name = "";
    
    if (!name_params.GetVal("column").isNull()) {
      col_name = name_params.GetVal("column").toString();
    }
    
    int row_from, rows;
    int row_to = 0;
    bool valid = CalcRowParams("Get", tab, row_from, rows, row_to); // get start row and number of rows to get, remove, etc.
    if (!valid)
      return;
    
    bool result = tab->GetDataAsJSON(ostr, col_name, row_from, rows);
    if (result) {
      data = ostr.str().c_str();
      // doing the message wrap here because it isn't working in SendOkJSON
      String str = "{\"status\":\"OK\", \"result\": " + data + "}";
      Write(str);
    }
    else {
      SendError("GetData: " + tab->error_msg, TemtClient::RUNTIME);
    }
  }
  else {
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
  }
}

void TemtClient::cmdGetDataCell() {
  if (msgFormat == TemtClient::JSON) {
    SendError("For JSON use GetData, specify column and row", TemtClient::UNSPECIFIED);
  }
  
  String tnm = pos_params.SafeEl(0);
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  TableParams p(this, tab);
  if (!p.ValidateParams(TableParams::Cell, false)) return;
  
  cmdGetDataCell_impl(p);
}

void TemtClient::cmdGetDataMatrixCell() {
  if (msgFormat == TemtClient::NATIVE) {
    String tnm = pos_params.SafeEl(0);
    name_params.SetVal("table", tnm);
  }
  
  String tnm = name_params.GetVal("table").toString();
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  if (msgFormat == TemtClient::JSON) {
    ostringstream ostr;
    String data;
    String col_name = name_params.GetVal("column").toString();
    int row_from = name_params.GetVal("row_from").toInt();
    int cell = name_params.GetVal("cell").toInt();
    bool result = tab->GetDataMatrixCellAsJSON(ostr, col_name, row_from, cell);
    if (result) {
      data = ostr.str().c_str();
      // munge string before concatenating
      data.remove ("{", 0);
      data.remove("}", -1);
      data.prepend(',');
      // doing the message wrap here because it isn't working in SendOkJSON
      String str = "{\"status\":\"OK\"" + data + "}";
      Write(str);
    }
    else {
      SendError("GetDataMatrixCell: " + tab->error_msg, TemtClient::RUNTIME);
    }
  }
  else {
    TableParams p(this, tab);
    if (!p.ValidateParams(TableParams::Cell, true))
      return;
    cmdGetDataCell_impl(p);
  }
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
  if (msgFormat == TemtClient::JSON) {
    String tnm = name_params.GetVal("table").toString();
    DataTable* tab = GetAssertTable(tnm);
    if (!tab) return;
    
    int row = name_params.GetVal("row_from").toInt();
    tab->SetDataFromJSON(tableData, row, 1);  // 1 is total number of rows to set
    SendOk();
  }
  
  String tnm = pos_params.SafeEl(0);
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  TableParams p(this, tab);
  if (!p.ValidateParams(TableParams::Cell, false)) return;
  
  cmdSetDataCell_impl(p);
}

void TemtClient::cmdSetDataMatrixCell() {
  if (msgFormat == TemtClient::JSON) {
    SendError("For JSON use SetData, specify row and cell and send one value");
    return;
  }
  
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
  if (msgFormat == TemtClient::NATIVE) {
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
  
  if (msgFormat == TemtClient::NATIVE) {
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
  
  // get the value, possibly converting
  String val;
  if (var->var_type == ProgVar::T_String) {
    val = String::StringToCppLiteral(var->string_val);
  }
  else {
    val = var->GetVar().toString();
  }
  
  // send message
  SendOk(val);
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
    if (msgFormat == TemtClient::NATIVE) {
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
  if (msgFormat == TemtClient::NATIVE) {
    String tnm = pos_params.SafeEl(0);
    name_params.SetVal("table", tnm);
  }
  
  String tnm = name_params.GetVal("table").toString();
  DataTable* tab = GetAssertTable(tnm);
  if (!tab)
    return;
  // note: ok if running
  
  if (msgFormat == TemtClient::NATIVE) {
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
  else if (msgFormat == TemtClient::JSON) {
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
  if (msgFormat == TemtClient::NATIVE) {
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
        SendError("Var '" + nm + "' is an Object--setting is not supported");
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
  
  if (msgFormat == TemtClient::JSON) {
    String pnm = name_params.GetVal("program").toString();
    Program* prog = GetAssertProgram(pnm);
    if (!prog)
      return;
    
    String var_name = name_params.GetVal("var_name").toString();
    if (var_name.empty()) {
      SendError("var_name missing");
      return;
    }
    if (!prog->HasVar(var_name)) {
      SendError("Var '" + var_name + "' not found");
      return;
    }
    
    // check if type ok to set -- assume it will be found since name is ok
    ProgVar* var = prog->FindVarName(var_name);
    if (var->var_type == ProgVar::T_Object) {
      SendError("Var '" + var_name + "' is an Object--setting is not supported");
      return;
    }
    
    // set
    String var_value = name_params.GetVal("var_value").toString();
    if (var_value.empty()) {
      SendError("var_value missing");
      return;
    }
    if (!prog->SetVar(var_name, var_value)) {
      SendError("An error occurred while seeting Var or Arg '" + var_name + "'");
      return;
    }
    SendOk();
  }
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
    msgFormat = TemtClient::JSON;
    ParseCommandJSON(cmd_line);
    return;
  }
  else {
    msgFormat = TemtClient::NATIVE;
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
    name_params.SetVal("command", cmd);
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

void TemtClient::ParseCommandJSON(const String& cmd_string) {
  json_string json_cmd_line = json_string(cmd_string.chars());
  
  if (!libjson::is_valid(json_cmd_line)) {
    SendErrorJSON("JSON format error", TemtClient::INVALID_FORMAT);
    return;
  }
  else
  {
    JSONNode n = libjson::parse(json_cmd_line);
    JSONNode::const_iterator i = n.begin();
    while (i != n.end()) {
      String node_name(i->name().c_str());
      
      if (node_name == "command") {
        name_params.SetVal("command", i->as_string().c_str());
      }
      else if (node_name == "program") {
        name_params.SetVal("program", i->as_string().c_str()); // program name
      }
      else if (node_name == "var_name") {
        name_params.SetVal("var_name", i->as_string().c_str()); // variable name
      }
      else if (node_name == "var_value") {
        name_params.SetVal("var_value", i->as_string().c_str()); // variable name
      }
      else if (node_name == "table") {
        name_params.SetVal("table", i->as_string().c_str());  // table name
      }
      else if (node_name == "data") {
        tableData = i->as_node();  // json string of data table data
      }
      else if (node_name == "column") {
        name_params.SetVal("column", i->as_string().c_str());  // a single column name - for get only - set controlled through "data"
      }
      else if (node_name == "row_from") {
        name_params.SetVal("row_from", i->as_int());  // first row to get/set
      }
      else if (node_name == "row_to") {
        name_params.SetVal("row_to", i->as_int());  // last row to get/set
      }
      else if (node_name == "rows") {
        name_params.SetVal("rows", i->as_int());  // count of rows to operate on (get, remove) - for set count is number of values sent
      }
      else if (node_name == "cell") {
        name_params.SetVal("cell", i->as_int());  // first cell to get/set - based on flat indexing
      }
      else {
        String err_msg = "Unknown parameter: " + node_name;
        SendErrorJSON(err_msg, TemtClient::UNKNOWN_PARAM);
        return;  // abort - force client to fix before we get into trouble
      }
      ++i;
    }
    
    String cmd = name_params.GetVal("command").toString();
    RunCommand(cmd);
  }
}

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
  else
  {
    String err_msg = "Unknown command: " + cmd;
    SendErrorJSON(err_msg, TemtClient::UNKNOWN_COMMAND);
  }
  
  //TODO: we can (should!) look up and dispatch via name!
  /*  if (cmd == "CloseProject") {
   cmdCloseProject();
   } else
   if (cmd == "OpenProject") {
   cmdOpenProject();
   } else { */
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
  if (msgFormat == TemtClient::JSON)
    SendErrorJSON(err_msg, err);
  else
    SendErrorNATIVE(err_msg);
}

void TemtClient::SendErrorNATIVE(const String& err_msg, TemtClient::ServerError err) {
  String ln = "ERROR";
  if (err_msg.length() > 0)
    ln.cat(" ").cat(err_msg);
  WriteLine(ln);
}

void TemtClient::SendErrorJSON(const String& err_msg, TemtClient::ServerError err) {
  JSONNode root(JSON_NODE);
  root.push_back(JSONNode("status", json_string("ERROR")));
  root.push_back(JSONNode("message", json_string(err_msg.chars())));
  root.push_back(JSONNode("error", err));
  
  String reply = root.write_formatted().c_str();
  WriteLine(reply);
}

void TemtClient::SendOk(const String& msg) {
  if (msgFormat == TemtClient::JSON)
    SendOkJSON(msg);
  else
    SendOkNATIVE(msg);
}

void TemtClient::SendOkNATIVE(const String& msg) {
  String ln = "OK";
  if (msg.nonempty()) {
    ln.cat(" ").cat(msg);
  }
  WriteLine(ln);
}

void TemtClient::SendOkJSON(const String& msg) {
  JSONNode root(JSON_NODE);
  root.push_back(JSONNode("status", json_string("OK")));
  if (!msg.empty()) {
    root.push_back(JSONNode("result", json_string(msg.chars())));
  }
  
  String reply = root.write_formatted().c_str();
  WriteLine(reply);
}

void TemtClient::SendReply(const String& r) {
  if (msgFormat == TemtClient::JSON)
    SendErrorJSON(r);
  else
    SendErrorNATIVE(r);
}

void TemtClient::SendReplyNATIVE(const String& r) {
  WriteLine(r);
}

void TemtClient::SendReplyJSON(const String& r) {
  WriteLine(r);
}

void TemtClient::Write(const String& txt) {
  if (!isConnected()) return;
  sock->write(QByteArray(txt.chars(), txt.length()));
}

void TemtClient::WriteLine(const String& ln) {
  if (!isConnected()) return;
  String lnt = ln + "\n";
  sock->write(QByteArray(lnt.chars(), lnt.length()));
}

// used by json calls
bool TemtClient::CalcRowParams(String operation, DataTable* table, int& row_from, int& rows, int row_to) {
  bool row_from_set = false;
  bool row_to_set = false;
  
  int row_count = table->rows;  // actual count of rows
  
  if (!name_params.GetVal("row_from").isNull()) {
    row_from = name_params.GetVal("row_from").toInt();
    row_from_set = true;
  }
  else {
    row_from = 0;
  }
  
  if (!name_params.GetVal("row_to").isNull()) {
    row_to = name_params.GetVal("row_to").toInt();
    row_to_set = true;
  }

  if (!name_params.GetVal("rows").isNull()) {
    rows = name_params.GetVal("rows").toInt();
  }
  else {
    rows = -1;
  }

  // this is the only row oriented call and does not go through json api
  // so it requires a bit of upfront checking - The json data table code
  // returns error messages but the old row based data table calls do not
  // (jtr 3/31/14)
  if (operation == "remove" && row_count == 0) {
    SendOk();
    return false;  // false because we won't actually execute the remove code
  }
  
  if (row_from_set && (row_from < 0 || row_from >= row_count)) {
    SendError("row out of range", TemtClient::RUNTIME);
    return false;
  }
  
  if (row_to_set && (row_to < 0 || row_to >= row_count)) {
    SendError("row out of range", TemtClient::RUNTIME);
    return false;
  }
  
  // if row_from and row_to are both set calc the rows
  if (row_from_set && row_to_set) {
    rows = row_to - row_from + 1;
  }
  else if (row_to_set) {  // not allowed
    SendError("Missing parameter - 'row_from' : 'row_to' not allowed without 'row_from'.", TemtClient::MISSING_PARAM);
    return false;
  }
  else if (!name_params.GetVal("rows").isNull()) {
    rows = name_params.GetVal("rows").toInt();
  }
  else {
    rows = -1;  // all rows= 0
  }
  return true;
}
