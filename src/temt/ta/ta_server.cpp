// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
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


#include "ta_server.h"

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QRegExp>
#include <QTimer>

#include <QByteArray>
#include <QDataStream>


//TEMP prob handle commands in another file
#include "ta_viewer.h"
#include "ta_project.h"
// /TEMP

//////////////////////////
//  TemtClientAdapter	//
//////////////////////////

void TemtClientAdapter::init() {
  prog_rval = Program::RV_OK;
  pds = PDS_NONE;
}

void TemtClientAdapter::prog_Run() {
  if (!prog) {
    prog_rval = Program::RV_NO_PROGRAM;
    return;
  }
  pds = PDS_RUNNING;
  prog->Run();
  prog_rval = (Program::ReturnVal)prog->ret_val;
  pds = PDS_DONE;
}

void TemtClientAdapter::SetProg(Program* prog_) {
  prog = prog_;
  pds = PDS_SET;
  prog_rval = Program::RV_OK;
}

void TemtClientAdapter::sock_disconnected() {
  owner()->sock_disconnected();
}

void TemtClientAdapter::sock_readyRead() {
  owner()->sock_readyRead();
}

void TemtClientAdapter::sock_stateChanged(QAbstractSocket::SocketState socketState) {
  owner()->sock_stateChanged(socketState);
}



//////////////////////////
//  TemtClient		//
//////////////////////////

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
  SetAdapter(new TemtClientAdapter(this));
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

void TemtClient::cmdRunProgram() {
  String pnm = pos_params.SafeEl(0);
  if (pnm.empty()) {
    SendError("RunProgram: program name expected");
    return;
  }
  
  // make sure project
  taProject* proj = GetCurrentProject();
  if (!proj) {
    SendError("RunProgram " + pnm + ": no project open");
    return;
  }
  
  // get program, make sure exists
  Program* prog = GetAssertProgram(pnm);
  if (!prog) return;
  
  // check if a prog already running
  Program::RunState grs = Program::GetGlobalRunState();
  if (!((grs == Program::DONE) || (grs == Program::NOT_INIT))) {
    SendError("RunProgram " + pnm + ": program already running");
    return;
  }
  
  // check that not already running
  Program::RunState rs = prog->run_state;
  if (!((rs == Program::DONE) || (rs == Program::NOT_INIT))) {
    SendError("RunProgram " + pnm + ": already running");
    return;
  }
  
  // check that it is initialized, otherwise call Init
  if (rs == Program::NOT_INIT) {
    //NOTE: Init is synchronous
    prog->Init();
    if (prog->ret_val != Program::RV_OK) {
      SendError("RunProgram " + pnm + "->Init() failed with ret_val: "
        + String(prog->ret_val));
      return;
    }
  }
  
  bool sync = name_params.GetValDef("sync", false).toBool();
  // run
  if (sync) {
    prog->Run();
    if (prog->ret_val != Program::RV_OK) {
      SendError("RunProgram " + pnm + "->Run() failed with ret_val: "
        + String(prog->ret_val));
      return;
    }
  } else { // async
    adapter()->SetProg(prog);
    QTimer::singleShot(0, adapter(), SLOT(prog_Run()));
  }
  SendOk();
}

void TemtClient::cmdSetData() {
//TEMP
SendError("SetData not implemented yet");
}

Program* TemtClient::GetAssertProgram(const String& pnm) {
// does many checks, to make sure the prog exists
  if (pnm.empty()) {
    SendError("program name expected");
    return NULL;
  }
  
  // make sure project
  taProject* proj = GetCurrentProject();
  if (!proj) {
    SendError("no project open");
    return NULL;
  }
  
  // get program, make sure exists
  Program* prog = proj->programs.FindLeafName(pnm);
  if (!prog) {
    SendError("Program '" + pnm + "' not found");
    return NULL;
  }
  return prog;
}

DataTable* TemtClient::GetAssertTable(const String& nm) {
// does many checks, to make sure the table and prog exists
  if (nm.empty()) {
    SendError("table name expected");
    return NULL;
  }
  
  // make sure project
  taProject* proj = GetCurrentProject();
  if (!proj) {
    SendError("no project open");
    return NULL;
  }
  
  DataTable* tab = NULL;
  // if a local table, then resolve
  String pnm = nm.before(".");
  if (pnm.empty()) {
    // global table
    tab = proj->data.FindLeafName(nm);
  } else { // local table
    String tnm = nm.after(".");
    Program* prog = GetAssertProgram(pnm);
    if (!prog) return NULL; //note: will already have sent error
    tab = dynamic_cast<DataTable*>(prog->objs.FindName(nm));
  }
  if (!tab) {
    SendError("Table '" + nm + "' not found");
    return NULL;
  }
  return tab;
}

bool TemtClient::TableParams::ValidateParams(TemtClient::TableParams::Cmd cmd, bool mat_) {
  // defaults for values -- some of the guys below alter these, before we read them
  rows = 1;
  header = false;
  
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
  String tnm = pos_params.SafeEl(0);
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  TableParams p(this, tab);
  bool cmd_ok = p.ValidateParams(TemtClient::TableParams::Append);
  //NOTE: p will have already sent an ERROR if cmd_ok is false
  // any subsequenct failure must send an ERROR
  
  
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
  while ((p.lines > 0) && (lines.size > 0)) {
    String ln;
    //note: parser needs the eol, so we have to add it back in
    ln = lines.FastEl(0) + '\n';
    lines.RemoveIdx(0);
    p.lines--;
    // note: making a new one each loop simplifies things
    istringstream istr(ln.chars());
  
    tab->LoadData_strm(istr, DataTable::TAB, 
      true // quote_str
      );
  }
  tab->StructUpdate(false);
  }
ok_exit:
  // ok, we did!
  SendOk();
}

void TemtClient::cmdGetData() {
  String tnm = pos_params.SafeEl(0);
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  TableParams p(this, tab);
  if (!p.ValidateParams()) return;
  
  // ok, we can do it!
  SendOk("line=" + String(p.lines));
  
  ostringstream ostr;
  String ln;
  
  // header row, if any
  if (p.header) {
    tab->SaveHeader_strm(ostr);
    ln = ostr.str().c_str();
    Write(ln);
  }
  
  // rows
  for (int row = p.row_from; row <= p.row_to; ++row) {
    ostr.str("");
    ostr.clear();
    tab->SaveDataRow_strm(ostr, row, DataTable::TAB, 
      true, // quote_str
      false, // row_mark
      p.col_from, p.col_to);
    ln = ostr.str().c_str();
    Write(ln);
  }
  
}

void TemtClient::cmdGetDataCell() {
  String tnm = pos_params.SafeEl(0);
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  TableParams p(this, tab);
  if (!p.ValidateParams(TableParams::Cell, false)) return;
  
  cmdGetDataCell_impl(p);
}

void TemtClient::cmdGetDataMatrixCell() {
  String tnm = pos_params.SafeEl(0);
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  TableParams p(this, tab);
  if (!p.ValidateParams(TableParams::Cell, true)) return;
  
  cmdGetDataCell_impl(p);
}

void TemtClient::cmdGetDataCell_impl(TableParams& p) {
  Variant v = p.tab->GetMatrixFlatVal(p.col, p.row, p.cell);
  String res;
  if (v.isStringType())
    res = String::StringToCppLiteral(v.toString());
  else res = v.toString();
  return SendOk(res);
}

void TemtClient::cmdSetDataCell() {
  String tnm = pos_params.SafeEl(0);
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  TableParams p(this, tab);
  if (!p.ValidateParams(TableParams::Cell, false)) return;
  
  cmdSetDataCell_impl(p);
}

void TemtClient::cmdSetDataMatrixCell() {
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
  String pnm = pos_params.SafeEl(0);
  Program* prog = GetAssertProgram(pnm);
  if (!prog) return;
  // note: ok if running
  
  // 2nd param must be a var name
  String nm = pos_params.SafeEl(1);;
  // note: check name first, because GetVar raises error
  if (!prog->HasVar(nm)) {
    SendError("Var '" + nm + "' not found");
    return;
  }
  ProgVar* var = prog->FindVarName(nm);
  if (!var) { // shouldn't happen
    SendError("Var '" + nm + "' could unexpectedly not be retrieved");
    return;
  }
  
  // get the value, possibly converting
  String val;
  if (var->var_type == ProgVar::T_String) {
    val = String::StringToCppLiteral(var->string_val);
  } else {
    val = var->GetVar().toString();
  }
  SendOk(val);
}

void TemtClient::cmdGetRunState() {
  int run_state = 0; // temp
  if (pos_params.size == 0) {
    // global version
    run_state = Program::GetGlobalRunState();
  } else {
    // program version
    String pnm = pos_params.SafeEl(0);
    Program* prog = GetAssertProgram(pnm);
    if (!prog) return;
    run_state = prog->run_state;
  }
  SendOk(String(run_state));
}

void TemtClient::cmdRemoveData() {
  String tnm = pos_params.SafeEl(0);
  DataTable* tab = GetAssertTable(tnm);
  if (!tab) return;
  // note: ok if running
  
  TableParams p(this, tab);
  bool cmd_ok = p.ValidateParams(TemtClient::TableParams::Remove);
  if (!cmd_ok) return;
  
  // ok if none
  if (p.row_from >= 0) {
    if (!tab->RemoveRows(p.row_from, p.rows)) {
      SendError("RemoveRows command on table '" + tnm + "' did not succeed");
      return;
    }
  }
  SendOk();
}

void TemtClient::cmdSetVar() {
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
      SendError("Var '" + nm + "' not found");
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
      SendError("An error occurred while seeting Var or Arg '" + nm + "'");
      return;
    }
  }
  SendOk("vars set");
}

taProject* TemtClient::GetCurrentProject() {
  if (!cur_proj)
    cur_proj = tabMisc::root->projects.SafeEl(0);
  return cur_proj.ptr();
}

void TemtClient::ParseCommand(const String& cl) {
  cmd_line = trim(cl); // remove leading/trailing ws, including eol's
  cmd = _nilString;
  pos_params.Reset();
  name_params.Reset();
  
  // we use the pos_params as an intermediate guy during processing...
//TEMP 
// TODO: need to properly parse, manually, each guy, to make sure, ex. that 
// strings are pulled intact, that a quoted string with a = in it doesn't get
// pulled as a name= etc. etc.

  // note: following from Qt help file
  //TODO: following not correct, need to manually deal with "" and remove "" 
  String str = cmd_line;
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
  
  //TODO: we can (should!) look up and dispatch via name!
/*  if (cmd == "CloseProject") {
    cmdCloseProject();
  } else 
  if (cmd == "OpenProject") {
    cmdOpenProject();
  } else { */
  if (cmd == "AppendData") {
    cmdAppendData();
  } else
  if (cmd == "Echo") {
    cmdEcho();
  } else
  if (cmd == "GetData") {
    cmdGetData();
  } else
  if (cmd == "GetDataCell") {
    cmdGetDataCell();
  } else
  if (cmd == "GetDataMatrixCell") {
    cmdGetDataMatrixCell();
  } else
  if (cmd == "GetVar") {
    cmdGetVar();
  } else
  if (cmd == "GetRunState") {
    cmdGetRunState();
  } else
  if (cmd == "RemoveData") {
    cmdRemoveData();
  } else
  if (cmd == "RunProgram") {
    cmdRunProgram();
  } else
  if (cmd == "SetDataCell") {
    cmdSetDataCell();
  } else
  if (cmd == "SetDataMatrixCell") {
    cmdSetDataMatrixCell();
  } else
  if (cmd == "SetVar") {
    cmdSetVar();
  } else {
    //TODO: need subclass hook, and/or change to symbolic, so subclass can do cmdXXX routine
    // unknown command
    String err = "\"" + cmd + "\" is an unknown command";
    SendError(err);
  }
  
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
    line.set(ba.data(), (int)line_len); // note: includes \n
    // strip nl -- should be only one, because that's what a line is
    line = line.before('\n');
    // strip r, like from telnet
    line = line.before('\r');
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

void TemtClient::SendError(const String& err_msg) {
  String ln = "ERROR";
  if (err_msg.length() > 0)
    ln.cat(" ").cat(err_msg);
  WriteLine(ln);
}

void TemtClient::SendReply(const String& r) {
  WriteLine(r);
}

void TemtClient::SendOk(const String& msg) {
  String ln = "OK";
  if (msg.nonempty()) {
    ln.cat(" ").cat(msg);
  }
  WriteLine(ln);
}

/*void TemtClient::SendOk(int lines) {
  SendOk(lines, _nilString);
}

void TemtClient::SendOk(int lines, const String& addtnl) {
  String ln = "OK";
  if (lines >= 0) {
    ln.cat(" lines=").cat(lines);
    if (addtnl.length() > 0)
      ln.cat(" ").cat(addtnl);
  }
  WriteLine(ln);
}*/

void TemtClient::Write(const String& txt) {
  if (!isConnected()) return;
  sock->write(QByteArray(txt.chars(), txt.length()));
}

void TemtClient::WriteLine(const String& ln) {
  if (!isConnected()) return;
  String lnt = ln + "\n";
  sock->write(QByteArray(lnt.chars(), lnt.length()));
}

//////////////////////////
//  TemtServerAdapter	//
//////////////////////////

void TemtServerAdapter::server_newConnection() {
  owner()->server_newConnection();
}


//////////////////////////
//  TemtServer		//
//////////////////////////

void TemtServer::Initialize() {
  port = 5360;
  open = false;
  server = NULL;
  SetAdapter(new TemtServerAdapter(this));
}

void TemtServer::Destroy() {
  CloseServer(false);
}

void TemtServer::Copy_(const TemtServer& cp) {
  CloseServer();
  port = cp.port;
//NOTE: don't copy the clients -- always flushed
}

void TemtServer::ClientDisconnected(TemtClient* client) {
  // only called for asynchronous disconnects (not ones we force)
  clients.RemoveEl(client);
}

void TemtServer::CloseServer(bool notify) {
  if (!open) return;
  while (clients.size > 0) {
    TemtClient* cl = clients.FastEl(clients.size - 1);
    cl->server = NULL; // prevents callback
    cl->CloseClient();
    clients.RemoveEl(cl);
  }
  if (server) {
    delete server;
    server = NULL;
  }
  open = false;
  taMisc::server_active = false;
  if (notify) DataChanged(DCR_ITEM_UPDATED);
}

void TemtServer::InitServer_impl(bool& ok) {
  // nothing
}

bool TemtServer::OpenServer() {
  if (open) return true;
  if (taMisc::server_active) {
    taMisc::Error("A server is already open");
    return false;
  }
  server = new QTcpServer();
  if (!server->listen(QHostAddress::Any, port)) {
    taMisc::Error("Could not open the server: ",
      server->errorString());
    delete server;
    server = NULL;
    return false;
  }
  
  QObject::connect(server, SIGNAL(newConnection()), adapter(), SLOT(server_newConnection()));

  open = true;
  DataChanged(DCR_ITEM_UPDATED);
  taMisc::server_active = true;
  return true;
}

void  TemtServer::server_newConnection() {
  // setup data writing for hello or error block
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_0);
  
  // get the latest connection, and always set it to self-destruct on close
  QTcpSocket* ts = server->nextPendingConnection();
  QObject::connect(ts, SIGNAL(disconnected()),
    ts, SLOT(deleteLater()));
  // we only allow 1 client (in this version of pdp), so refuse others
  if (clients.size >= 1) {
    out << "ERROR 1 : too many connections already, closing...\n";
    ts->write(block);
    ts->disconnectFromHost();
    return;
  }
  TemtClient* cl = (TemtClient*)clients.New(1);
  cl->server = this;
  cl->SetSocket(ts);
    
  String banner = "Emergent Server v" + taMisc::version + "\n";
  out << banner.chars(); 
  ts->write(block);
  DataChanged(DCR_ITEM_UPDATED);
}

