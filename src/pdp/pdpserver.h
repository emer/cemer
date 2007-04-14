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

// pdpserver.h -- pdp server

#ifndef PDPSERVER_H
#define PDPSERVER_H

#include "pdp_def.h"

#include "ta_base.h"

#include "pdp_base.h"
#include "pdp_TA_type.h"

#ifndef __MAKETA__
# include <QtNetwork/QAbstractSocket> // for state defines
#endif

// forwards
class PdpServer;
class PdpServerAdapter;

class PDP_API PdpServerAdapter: public taBaseAdapter {
  // ##IGNORE QObject for attaching events/signals for its taBase owner
friend class PdpServer;
  Q_OBJECT
public:
  inline PdpServer*	owner() {return (PdpServer*)taBaseAdapter::owner;}
  PdpServerAdapter(PdpServer* owner_): taBaseAdapter((taOABase*)owner_) {}
  
#ifndef __MAKETA__ // maketa chokes on the net class types etc.
public slots:
  void 			server_newConnection();
  
  // socket functions -- currently on this obj, but if we allowed multi, have to create
  // separate QObject handler for each one, because Qt doesn't put the sender in the calls
  void 			socket_readyRead();
  void 			socket_disconnected(); //note: we only allow one for now, so monitor it here
  void 			socket_stateChanged(QAbstractSocket::SocketState socketState);
#endif
};

class PDP_API PdpServer: public taOABase { 
  // #INSTANCE #TOKENS Pdp Server, for tcp-based remote services 
INHERITED(taOABase)
public:
  unsigned short	port; // #DEF_5360 port number to use -- each instance must have unique port
  bool			open; // #NO_SAVE #SHOW #READ_ONLY set when server is open and accepting connections
  int			clients; // #SHOW #NO_SAVE #READ_ONLY how many clients are connected
  
  inline PdpServerAdapter* adapter() {return (PdpServerAdapter*)taOABase::adapter;} // #IGNORE

  bool			OpenServer(); // #BUTTON #GHOST_ON_open open the server and accept connections
  void			CloseServer(bool notify = true); // #BUTTON #GHOST_OFF_open #ARGC_0 stop the server and close open connections
  
  
  TA_BASEFUNS(PdpServer);

#ifndef __MAKETA__ // maketa chokes on the net class types etc.
public: // slot forwardees
  void 			server_newConnection(); //
  
  // socket functions -- currently on this obj, but if we allowed multi, have to create
  // separate QObject handler for each one, because Qt doesn't put the sender in the calls
  void 			socket_readyRead();
  void 			socket_disconnected(); //note: we only allow one for now, so monitor it here
  void 			socket_stateChanged(QAbstractSocket::SocketState socketState);
#endif

protected:
  QTcpServer*		server; // #IGNORE
  
  QTcpSocket*		client; // #IGNORE the one and only socket we allow to connect
private:
  void	Copy_(const PdpServer& cp);
  void	Initialize();
  void 	Destroy();
};

#endif
