// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.


#include "pdpserver.h"

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
//TEMP
#include <QByteArray>
#include <QDataStream>
// end TEMP

//////////////////////////
//  PdpServerAdapter	//
//////////////////////////

void PdpServerAdapter::server_newConnection() {
  owner()->server_newConnection();
}

void PdpServerAdapter::socket_disconnected() {
  owner()->socket_disconnected();
}

void PdpServerAdapter::socket_readyRead() {
  owner()->socket_readyRead();
}

void PdpServerAdapter::socket_stateChanged(QAbstractSocket::SocketState socketState) {
  owner()->socket_stateChanged(socketState);
}



//////////////////////////
//  PdpServer		//
//////////////////////////

void PdpServer::Initialize() {
  port = 5360;
  open = false;
  clients = 0;
  server = NULL;
  client = NULL;
  SetAdapter(new PdpServerAdapter(this));
  
}

void PdpServer::Destroy() {
  CloseServer(false);
}

void PdpServer::Copy_(const PdpServer& cp) {
  CloseServer();
  port = cp.port;
}

void PdpServer::CloseServer(bool notify) {
  if (!open) return;
  if (client) {
    client->disconnectFromHost();
    // probably called back immediately, so check again
    if (client) {
      client = NULL;
      --clients;
    }
  }
  if (server) {
    delete server;
    server = NULL;
  }
  open = false;
  if (notify) DataChanged(DCR_ITEM_UPDATED);
}

bool PdpServer::OpenServer() {
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

void  PdpServer::server_newConnection() {
  // setup data writing for hello or error block
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_0);
  
  // get the latest connection, and always set it to self-destruct on close
  QTcpSocket* ts = server->nextPendingConnection();
  QObject::connect(ts, SIGNAL(disconnected()),
    ts, SLOT(deleteLater()));
  // we only allow 1 client (in this version of pdp), so refuse others
  if (clients >= 1) {
    out << "pdp4.0\n**ERROR** -- too many connections already, closing...\n";
    ts->write(block);
    ts->disconnectFromHost();
    return;
  }
  client = ts;
  ++clients;
  QObject::connect(ts, SIGNAL(disconnected()),
    adapter(), SLOT(socket_disconnected()));
  QObject::connect(ts, SIGNAL(readyRead()),
    adapter(), SLOT(socket_readyRead()));
  QObject::connect(ts, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
    adapter(), SLOT(socket_stateChanged(QAbstractSocket::SocketState)));
            
    
  out << "pdp4.0 \n"; // TODO: also output version (after the space)
  ts->write(block);
  DataChanged(DCR_ITEM_UPDATED);
}

void  PdpServer::socket_disconnected() {
  //NOTE: only 1 allowed for now, so it must be our cached version
  if (client == NULL) return; // already removed
  client = NULL;
  --clients;
  DataChanged(DCR_ITEM_UPDATED);
}

void PdpServer::socket_readyRead() {
  if (!client) return; 
  qint64 ba = client->bytesAvailable(); // we use this to compare with readAll to check for error
  QByteArray block = client->readAll(); //note: no way to report an error
  if (block.count() != ba) {
    taMisc::Error("read error on socket");
  }

//TEMP
  // just echo for now
  client->write(block);
  
// /TEMP
}


void PdpServer::socket_stateChanged(QAbstractSocket::SocketState socketState) {
  //nothing yet
}

