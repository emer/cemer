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

// ta_server.h -- tcp/ip server

#ifndef TA_SERVER_H
#define TA_SERVER_H

#include "ta_base.h"
#include "ta_project.h"

#ifndef __MAKETA__
# include <QtNetwork/QAbstractSocket> // for state defines
#endif


// forwards
class TemtClient;
class TemtClientAdapter;
class TemtServer;
class TemtServerAdapter;

class TA_API TemtClientAdapter: public taBaseAdapter {
  // ##IGNORE QObject for attaching events/signals for its taBase owner
friend class TemtClient;
  Q_OBJECT
public:
  enum ProgDispatchState {
    PDS_NONE,
    PDS_SET, // not dispatched yet
    PDS_RUNNING, // Run called
    PDS_DONE // Run finished -- note, could be an error
  };
  
  ProgramRef		prog; // program to run
  ProgDispatchState	pds; 
  Program::ReturnVal	prog_rval; // rval 
  
  inline TemtClient*	owner() {return (TemtClient*)taBaseAdapter::owner;}
  
  void			SetProg(Program* prog);
  
  TemtClientAdapter(TemtClient* owner_): taBaseAdapter((taOABase*)owner_) {init();}
  
#ifndef __MAKETA__ // maketa chokes on the net class types etc.
public slots:
  void			prog_Run(); // run the prog
  void 			sock_readyRead();
  void 			sock_disconnected(); //note: we only allow one for now, so monitor it here
  void 			sock_stateChanged(QAbstractSocket::SocketState socketState);
#endif
private:
  void init();
};

class TA_API TemtClient: public taOABase { 
  // #INSTANCE #TOKENS for tcp-based remote services -- represents one connected client 
INHERITED(taOABase)
friend class TableParams;
public:
  enum ClientState {
    CS_READY,	// expecting a command
    CS_DATA_IN,	// expecting another line of data in (from previous command)
    CS_DISCONNECTED // client has disconnected
  };
  
  ClientState		state; // #READ_ONLY #SHOW #NO_SAVE comm state 
  
  bool			isConnected() const {return (state != CS_DISCONNECTED);}
  
  inline TemtClientAdapter* adapter() {return (TemtClientAdapter*)taOABase::adapter;} // #IGNORE
  TA_BASEFUNS(TemtClient);

#ifndef __MAKETA__ // maketa chokes on the net class types etc.
  TemtServer*		server; // (will never change) set on create; NOT refcnted
  
  void			CloseClient();
  void			SetSocket(QTcpSocket* sock);

  void			SendError(const String& err_msg); // send error reply
  void			SendReply(const String& r); // send reply
  void			SendOk(const String& msg = _nilString); // send ok, w/ optional msg or data (should not have an eol)
//  void			SendOk(int lines = -1);
//  void			SendOk(int lines, const String& addtnl); //
  
  void			WriteLine(const String& ln); // low level write, note: adds eol
  void			Write(const String& txt); // low level write
  
public: // commands, all are cmdXXX where XXX is exact command name
  virtual void 		cmdAppendData();
  virtual void		cmdCloseProject();
  virtual void		cmdEcho(); // echos, for test
  virtual void 		cmdGetData();
  virtual void 		cmdGetVar();
  virtual void		cmdOpenProject();
  virtual void		cmdRunProgram(); 
  virtual void 		cmdSetData();
  virtual void		cmdSetVar();
  
public: // slot forwardees
  void 			sock_readyRead();
  void 			sock_disconnected(); 
  void 			sock_stateChanged(QAbstractSocket::SocketState socketState);

protected:
  static String 	ReadQuotedString(const String& str, int& p, bool& err);
  static String		NextToken(const String& str, int& p, bool& err);
    // skip ws, get the next token; removes quotes and processes quoted/escaped strings
#ifndef __MAKETA__
  class TableParams {
  public:
    enum Cmd {
      Get,
      Append,
      Set
    };
    
    TemtClient* tc;
    DataTable* tab;
    int rows; // Append/Set only
    int row_from;
    int row_to;
    int col_from;
    int col_to;
    bool header;
    int lines;
    
    bool	ValidateParams(Cmd cmd = Get);
    TableParams(TemtClient* tc_, DataTable* tab_)
      {tc = tc_; tab = tab_;}
      
  };
#endif
  QPointer<QTcpSocket>	sock; // #IGNORE the socket for the connected client
  String_PArray		lines; // have to buffer between raw in and processing them -- this is a queue
  
// every command line is parsed into the following pieces before dispatching the command:
  String		cmd_line; // the last cmd line
  String		cmd; // this is the first item, the command
  String_PArray		pos_params; // positional (no "=") parameters, if any; str quoting/escaping already done
  NameVar_PArray	name_params; // name params; str quoting/escaping already done
  taProjectRef		cur_proj; // set by OpenProject cmd, or to proj0
  
  taProject*		GetCurrentProject(); // gets, and maybe asserts
  DataTable* 		GetAssertTable(const String& nm); // gets, and sends errs if not found; supports <GlobalTableName> or <ProgName>.<LocalTableName> formats
  Program* 		GetAssertProgram(const String& pnm); // gets, and sends errs if not found
  
  void			setState(ClientState cs);
  
  void			HandleLines(); // line handling loop
  void			ParseCommand(const String& cl);
#endif
private:
  void	Copy_(const TemtClient& cp);
  void	Initialize();
  void 	Destroy();
};

class TA_API TemtClient_List: public taList<TemtClient> {
public:
  TA_BASEFUNS2_NOCOPY(TemtClient_List, taList<TemtClient>);

private:
  void	Initialize() {SetBaseType(&TA_TemtClient);}
  void 	Destroy() {}
};


class TA_API TemtServerAdapter: public taBaseAdapter {
  // ##IGNORE QObject for attaching events/signals for its taBase owner
friend class TemtServer;
  Q_OBJECT
public:
  inline TemtServer*	owner() {return (TemtServer*)taBaseAdapter::owner;}
  TemtServerAdapter(TemtServer* owner_): taBaseAdapter((taOABase*)owner_) {}
  
#ifndef __MAKETA__ // maketa chokes on the net class types etc.
public slots:
  void 			server_newConnection();
#endif
};

class TA_API TemtServer: public taOABase { 
  // #INSTANCE #TOKENS Temt Server, for tcp-based remote services 
INHERITED(taOABase)
public:
  unsigned short	port; // #DEF_5360 port number to use -- each instance must have unique port
  bool			open; // #NO_SAVE #SHOW #READ_ONLY set when server is open and accepting connections
  TemtClient_List	clients; // #SHOW #NO_SAVE #READ_ONLY how many clients are connected
  
  inline TemtServerAdapter* adapter() {return (TemtServerAdapter*)taOABase::adapter;} // #IGNORE
  
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

#endif
