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

#include "TemtServer.h"
#include <TemtServer_QObj>

#include <taMisc>

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>

TA_BASEFUNS_CTORS_DEFN(TemtServer);


void TemtServer::Initialize() {
  port = 5360;
  open = false;
  server = NULL;
  SetAdapter(new TemtServer_QObj(this));
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
  if (notify) SigEmitUpdated();
}

void TemtServer::InitServer_impl(bool& ok) {
  // nothing
}

bool TemtServer::OpenServer() {
  if (open) return true;
  if (taMisc::server_active) {
    taMisc::Error_nogui("A server is already open");
    return false;
  }
  server = new QTcpServer();
  if (!server->listen(QHostAddress::Any, port)) {
    taMisc::Error_nogui("Could not open the server: ",
      server->errorString().toLatin1());
    delete server;
    server = NULL;
    return false;
  }
  
  QObject::connect(server, SIGNAL(newConnection()), adapter(), SLOT(server_newConnection()));

  open = true;
  SigEmitUpdated();
  taMisc::server_active = true;
  return true;
}

void  TemtServer::server_newConnection() {
  // setup data writing for hello or error block
  QByteArray block;
  QDataStream out(&block, (QIODevice::OpenMode)QIODevice::WriteOnly);
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
  SigEmitUpdated();
}

