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

#ifndef TemtServer_h
#define TemtServer_h 1

// parent includes:
#include <taOABase>

// member includes:
#include <TemtClient_List>

// declare all other types mentioned but not required to include:
class TemtServer_QObj; // #IGNORE
class QTcpServer;        // #IGNORE


TypeDef_Of(TemtServer);

class TA_API TemtServer: public taOABase { 
  // #INSTANCE #TOKENS Temt Server, for tcp-based remote services 
INHERITED(taOABase)
public:
  unsigned short	port; // #DEF_5360 port number to use -- each instance must have unique port
  bool			open; // #NO_SAVE #SHOW #READ_ONLY set when server is open and accepting connections
  TemtClient_List	clients; // #SHOW #NO_SAVE #READ_ONLY how many clients are connected
  
  inline TemtServer_QObj* adapter() {return (TemtServer_QObj*)taOABase::adapter;} // #IGNORE
  
  bool			isOpen() const {return open;}

  bool			InitServer() // initializes the server
    {bool ok = true; InitServer_impl(ok); return ok;}
  
  bool			OpenServer(); // #BUTTON #GHOST_ON_open open the server and accept connections
  void			CloseServer(bool notify = true); // #BUTTON #GHOST_OFF_open #ARGC_0 stop the server and close open connections
  
// callbacks
  void			ClientDisconnected(TemtClient* client); // #IGNORE

  SIMPLE_LINKS(TemtServer);
  TA_BASEFUNS(TemtServer);

#ifndef __MAKETA__ // maketa chokes on the net class types etc.
public: // slot forwardees
  void 			server_newConnection(); //
#endif

protected:
  QTcpServer*		server; // #IGNORE
  
  TemtClient*		m_client; // #IGNORE unitary client
  
  virtual void		InitServer_impl(bool& ok); // impl routine, set ok=f for failure
private:
  void	Copy_(const TemtServer& cp); // copying not really supported...
  void	Initialize();
  void 	Destroy();
};

#endif // TemtServer_h
