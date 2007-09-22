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
//TEMP
#include <QByteArray>
#include <QDataStream>
// end TEMP

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
  connected = false;
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
  if (!connected) return;
  if (sock) {
    sock->disconnectFromHost();
    // probably called back immediately, so check again
    if (sock) {
      sock = NULL;
    }
  }
  connected = false;
}

void TemtClient::SetSocket(QTcpSocket* sock_) {
  sock = sock_;
  connected = true;
  QObject::connect(sock_, SIGNAL(disconnected()),
    adapter(), SLOT(sock_disconnected()));
  QObject::connect(sock_, SIGNAL(readyRead()),
    adapter(), SLOT(sock_readyRead()));
  QObject::connect(sock_, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
    adapter(), SLOT(sock_stateChanged(QAbstractSocket::SocketState)));
}            

void TemtClient::sock_disconnected() {
  connected = false;
  sock = NULL; // goodbye...
  if (server) server->ClientDisconnected(this);
//NO MORE CODE: we will be deleted!
}

void TemtClient::sock_readyRead() {
  if (!sock) return; 
  // we only do lines -- will call us again when a line is ready
  if (!sock->canReadLine()) return;
  
  QByteArray ba;
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
    String line(ba.data(), line_len); // note: includes \n
  
  //TEMP -- for demo -- doesn't have any state
    // strip nl -- should be only one, because that's what a line is
    line = line.before('\n');
    // strip r, like from telnet
    line = line.before('\r');
    String cmd = line.before(" ");
    String args = line.after(" ");
    if (cmd == "open") {
      taBase* proj_ = NULL;
      if (tabMisc::root->projects.Load(args, &proj_)) {
        taProject* proj = dynamic_cast<taProject*>(proj_);
        if (proj) {
          if (taMisc::gui_active) {
            MainWindowViewer::NewProjectBrowser(proj);
          }
        }
        sock->write(QByteArray("OK open\n"));
      } else {
        sock->write(QByteArray("ERROR 3 : file not found\n"));
      }
    } 
    else {
      sock->write(QByteArray("ERROR 2 : unknown command\n"));
    }
  }
  if (!ok) {
    sock->write(QByteArray("ERROR 4 : an unknown read error occurred\n"));
  }
}

void TemtClient::sock_stateChanged(QAbstractSocket::SocketState socketState) {
  //nothing yet
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

