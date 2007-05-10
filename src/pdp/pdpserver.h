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
class PdpClient;
class PdpClientAdapter;
class PdpServer;
class PdpServerAdapter;

class PDP_API PdpClientAdapter: public taBaseAdapter {
  // ##IGNORE QObject for attaching events/signals for its taBase owner
friend class PdpClient;
  Q_OBJECT
public:
  inline PdpClient*	owner() {return (PdpClient*)taBaseAdapter::owner;}
  PdpClientAdapter(PdpClient* owner_): taBaseAdapter((taOABase*)owner_) {}
  
#ifndef __MAKETA__ // maketa chokes on the net class types etc.
public slots:
  void 			sock_readyRead();
  void 			sock_disconnected(); //note: we only allow one for now, so monitor it here
  void 			sock_stateChanged(QAbstractSocket::SocketState socketState);
#endif
};

class PDP_API PdpClient: public taOABase { 
  // #INSTANCE #TOKENS for tcp-based remote services -- represents one connected client 
INHERITED(taOABase)
public:
  bool			connected; // #READ_ONLY #SHOW #NO_SAVE true when the client is connected
  
  inline PdpClientAdapter* adapter() {return (PdpClientAdapter*)taOABase::adapter;} // #IGNORE
  TA_BASEFUNS(PdpClient);

#ifndef __MAKETA__ // maketa chokes on the net class types etc.
  PdpServer*		server; // (will never change) set on create; NOT refcnted
  
  void			CloseClient();
  void			SetSocket(QTcpSocket* sock);

public: // slot forwardees
  void 			sock_readyRead();
  void 			sock_disconnected(); 
  void 			sock_stateChanged(QAbstractSocket::SocketState socketState);

protected:
  QPointer<QTcpSocket>	sock; // #IGNORE the socket for the connected client
#endif
private:
  void	Copy_(const PdpClient& cp);
  void	Initialize();
  void 	Destroy();
};

class PDP_API PdpClient_List: public taList<PdpClient> {
public:
  TA_BASEFUNS2_NOCOPY(PdpClient_List, taList<PdpClient>);

private:
  void	Initialize() {SetBaseType(&TA_PdpClient);}
  void 	Destroy() {}
};


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
#endif
};

class PDP_API PdpServer: public taOABase { 
  // #INSTANCE #TOKENS Pdp Server, for tcp-based remote services 
INHERITED(taOABase)
public:
  unsigned short	port; // #DEF_5360 port number to use -- each instance must have unique port
  bool			open; // #NO_SAVE #SHOW #READ_ONLY set when server is open and accepting connections
  PdpClient_List	clients; // #SHOW #NO_SAVE #READ_ONLY how many clients are connected
  
  inline PdpServerAdapter* adapter() {return (PdpServerAdapter*)taOABase::adapter;} // #IGNORE

  bool			OpenServer(); // #BUTTON #GHOST_ON_open open the server and accept connections
  void			CloseServer(bool notify = true); // #BUTTON #GHOST_OFF_open #ARGC_0 stop the server and close open connections
  
// callbacks
  void			ClientDisconnected(PdpClient* client); // #IGNORE

  SIMPLE_LINKS(PdpServer);
  TA_BASEFUNS(PdpServer);

#ifndef __MAKETA__ // maketa chokes on the net class types etc.
public: // slot forwardees
  void 			server_newConnection(); //
#endif

protected:
  QTcpServer*		server; // #IGNORE
  
  PdpClient*		m_client; // #IGNORE unitary client
private:
  void	Copy_(const PdpServer& cp); // copying not really supported...
  void	Initialize();
  void 	Destroy();
};

#endif
