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

//TEMP prob handle commands in another file
#include "ta_viewer.h"
#include "ta_project.h"
// /TEMP

//////////////////////////
//  PdpClientAdapter	//
//////////////////////////

void PdpClientAdapter::sock_disconnected() {
  owner()->sock_disconnected();
}

void PdpClientAdapter::sock_readyRead() {
  owner()->sock_readyRead();
}

void PdpClientAdapter::sock_stateChanged(QAbstractSocket::SocketState socketState) {
  owner()->sock_stateChanged(socketState);
}



//////////////////////////
//  PdpClient		//
//////////////////////////

void PdpClient::Initialize() {
  connected = false;
  server = NULL;
  SetAdapter(new PdpClientAdapter(this));
}

void PdpClient::Destroy() {
  CloseClient();
}

void PdpClient::Copy_(const PdpClient& cp) {
//NOTE: not designed to be copied
  CloseClient();
  if (server != cp.server) server = NULL;
}

void PdpClient::CloseClient() {
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

void PdpClient::SetSocket(QTcpSocket* sock_) {
  sock = sock_;
  connected = true;
  QObject::connect(sock_, SIGNAL(disconnected()),
    adapter(), SLOT(sock_disconnected()));
  QObject::connect(sock_, SIGNAL(readyRead()),
    adapter(), SLOT(sock_readyRead()));
  QObject::connect(sock_, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
    adapter(), SLOT(sock_stateChanged(QAbstractSocket::SocketState)));
}            

void PdpClient::sock_disconnected() {
  connected = false;
  sock = NULL; // goodbye...
  if (server) server->ClientDisconnected(this);
//NO MORE CODE: we will be deleted!
}

void PdpClient::sock_readyRead() {
  if (!sock) return; 
  // we only do lines -- will call us again when a line is ready
  if (!sock->canReadLine()) return;
  
  QByteArray ba;
  // need to keep fetching lines, since we only get notified once
  // note: typical case is we get one full line at a time
  while (sock->canReadLine()) {
    // we will need to make a buffer large enough for max data
    qint64 av = sock->bytesAvailable(); // we also use this to compare with readAll to check for error
    if (av < 2) av = 2;
    //note: qt seems to choke if readLine maxsize is < 2, but av can be 1 for nl
    ba.resize(av);
    qint64 line_len = sock->readLine(ba.data(), av);
    String line(ba.data(), line_len); // note: includes \n
  
  //TEMP -- for demo -- doesn't have any state
    // strip nl
    if (line.endsWith('\n'));
      line = line.left(line.length() - 1);
    if (line.startsWith("open ")) {
      String fname = line.after("open ");
      taBase* proj_ = NULL;
      if (tabMisc::root->projects.Load(fname, &proj_)) {
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
}

void PdpClient::sock_stateChanged(QAbstractSocket::SocketState socketState) {
  //nothing yet
}


//////////////////////////
//  PdpServerAdapter	//
//////////////////////////

void PdpServerAdapter::server_newConnection() {
  owner()->server_newConnection();
}


//////////////////////////
//  PdpServer		//
//////////////////////////

void PdpServer::Initialize() {
  port = 5360;
  open = false;
  server = NULL;
  SetAdapter(new PdpServerAdapter(this));
}

void PdpServer::Destroy() {
  CloseServer(false);
}

void PdpServer::Copy_(const PdpServer& cp) {
  CloseServer();
  port = cp.port;
//NOTE: don't copy the clients -- always flushed
}

void PdpServer::ClientDisconnected(PdpClient* client) {
  // only called for asynchronous disconnects (not ones we force)
  clients.RemoveEl(client);
}

void PdpServer::CloseServer(bool notify) {
  if (!open) return;
  while (clients.size > 0) {
    PdpClient* cl = clients.FastEl(clients.size - 1);
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
  if (clients.size >= 1) {
    out << "ERROR 1 : too many connections already, closing...\n";
    ts->write(block);
    ts->disconnectFromHost();
    return;
  }
  PdpClient* cl = (PdpClient*)clients.New(1);
  cl->server = this;
  cl->SetSocket(ts);
    
  out << "pdp++ server v" << taMisc::version.chars() << "\n"; 
  ts->write(block);
  DataChanged(DCR_ITEM_UPDATED);
}

