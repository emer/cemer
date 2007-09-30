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

#include <QByteArray>
#include <QDataStream>


//TEMP prob handle commands in another file
#include "ta_viewer.h"
#include "ta_project.h"
// /TEMP

//////////////////////////
//  TemtClientAdapter	//
//////////////////////////

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

void TemtClient::cmdCloseProject() {
//TEMP
SendError("CloseProject not implemented yet");
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
  QString str = cmd_line;
  QStringList list;
  list = str.split(QRegExp("\\s+"));
  for (int i = 0; i < list.count(); ++i) {
    pos_params.Add(list.at(i));
  }
  
  // pull 1st guy as the command
  if (pos_params.size > 0) {
    cmd = pos_params.FastEl(0);
    pos_params.RemoveIdx(0);
  }
  // pull guys w/ = as name=value, and they must all be after starting =
  int i = 0;
  int got_name_val = false;
  String item;
  while (i < pos_params.size) {
    item = pos_params.FastEl(i);
    if (item.contains("=")) {
      got_name_val = true; // they must all be from here on
      String name;
      String val;
      NameVar::Parse(item, name, val); // we already checked for =
      name_params.SetVal(name, val); // note: could conceivably be a duplicate
      pos_params.RemoveIdx(i); 
    } else {
      if (got_name_val) { // were expecting all the rest to be n=v guys
        SendError("all remaining params were expected to be name=value but found: " + item);
        return;
      }
    }
    ++i;
  }

// /TEMP  
  
  
  if (cmd.length() == 0) {
    // empty lines just echoed
    WriteLine(_nilString);
    return;
  }
  
  //TODO: we can (should!) look up and dispatch via name!
  if (cmd == "CloseProject") {
    cmdCloseProject();
  } else if (cmd == "OpenProject") {
    cmdOpenProject();
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
      //TODO: if not yet up to expected, then exit, else call dispatcher 
    } break;
    case CS_DISCONNECTED: {
    //shouldn't happen!!!
    } break;
    // handle all cases!
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

void TemtClient::SendOk(int lines) {
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
  if (notify) DataChanged(DCR_ITEM_UPDATED);
}

void TemtServer::InitServer_impl(bool& ok) {
  // nothing
}

bool TemtServer::OpenServer() {
  if (open) return true;
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
    
  String banner = "pdp++ server v" + taMisc::version + "\n";
  out << banner.chars(); 
  ts->write(block);
  DataChanged(DCR_ITEM_UPDATED);
}

