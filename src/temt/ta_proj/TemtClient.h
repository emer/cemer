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

#ifndef TemtClient_h
#define TemtClient_h 1

// parent includes:
#include <taOABase>

// member includes:
#ifndef __MAKETA__
#include <QPointer>
#include <QAbstractSocket>
  #if (QT_VERSION >= 0x050000)
  #include <QJsonObject>
  #include <QJsonDocument>
  #endif
#else
  #if (QT_VERSION >= 0x050000)
  class QJsonObject; // #IGNORE
  #endif
#endif

#include <NameVar_PArray>
#include <taProject>


// declare all other types mentioned but not required to include:
class TemtClient_QObj; // #IGNORE
class TemtServer; //
class DataTable; //
class Program; //
class QTcpSocket; //

taTypeDef_Of(TemtClient);

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
  
  enum MsgFormat {
      NATIVE, //
      JSON   //
    };

  enum ServerError {
    UNSPECIFIED,
    INVALID_FORMAT,
    UNKNOWN_COMMAND,
    MISSING_PARAM,  // program name, table name, etc not provided by client
    UNKNOWN_PARAM,
    NOT_FOUND,  // program name, table name, etc provided but not found
    NO_OPEN_PROJECT,
    RUNTIME
     };

  MsgFormat                     msg_format; // #READ_ONLY #NO_SAVE format of the message (command) from the client - native or json
  #if (QT_VERSION >= 0x050000)
    QJsonDocument::JsonFormat     json_format; // #READ_ONLY #NO_SAVE return to client document format - compact or indented
  #endif
  
  ClientState		state; // #READ_ONLY #SHOW #NO_SAVE comm state
  bool			isConnected() const {return (state != CS_DISCONNECTED);}
  
  TemtClient_QObj* adapter(); // #IGNORE
  TA_BASEFUNS(TemtClient);

  TemtServer*		server; // #READ_ONLY #NO_SAVE (will never change) set on create; NOT refcnted

  void			CloseClient();
  void			SetSocket(QTcpSocket* sock); // #IGNORE

  void      SendError(const String& err_msg, TemtClient::ServerError err = UNSPECIFIED); // relay to SendError in format of received message
  void      SendErrorNATIVE(const String& err_msg, TemtClient::ServerError err = UNSPECIFIED); // send error reply in ascii
  void      SendReply(const String& r); // relay to SendReply in format of received message
  void      SendReplyNATIVE(const String& r); // send reply ascii
  void      SendOk(const String& msg = _nilString); // relay to SendOk in format of received message
  void      SendOkNATIVE(const String& msg = _nilString); // send ok, w/ optional msg or data (should not have an eol)
#if (QT_VERSION >= 0x050000)
  void      SendErrorJSON(const String& err_msg, TemtClient::ServerError err = UNSPECIFIED); // send error reply in json format
  void      SendReplyJSON(const String& r); // send reply json
  void      SendOkJSON(const String& msg = _nilString); // send ok, w/ optional msg or data - json format
#endif
  
  void      WriteLine(const String& ln); // low level write, note: adds eol
  void			Write(const String& txt); // low level write
  
public: // commands, all are cmdXXX where XXX is exact command name
  virtual void    RunCommand(const String& cmd);  // dispatch specified command
  virtual void 		cmdAppendData();
  virtual void		cmdCloseProject();
  virtual void		cmdEcho(); // echos, for test
  virtual void 		cmdGetData();
  virtual void 		cmdGetDataCell();
  virtual void 		cmdGetDataMatrixCell();
  virtual void 		cmdGetVar();
  virtual void 		cmdGetRunState();
  virtual void		cmdOpenProject();
  virtual void 		cmdRemoveData();
  virtual void		cmdRunProgram(bool sync); 
  virtual void 		cmdSetData();
  virtual void 		cmdSetDataCell();
  virtual void 		cmdSetDataMatrixCell();
  virtual void		cmdSetVar();
  virtual void    cmdSetImage();
  virtual void    cmdCollectConsoleOutput();
  virtual void    cmdGetConsoleOutput();
  virtual void    cmdClearConsoleOutput();
#if (QT_VERSION >= 0x050000)
  virtual void    cmdSetJsonFormat();
    virtual void    cmdGetMember();
#endif
  
public: // slot forwardees
  void 			sock_readyRead();
  void 			sock_disconnected(); 
  void 			sock_stateChanged(QAbstractSocket::SocketState socketState);
  // #IGNORE

protected:
  static String 	ReadQuotedString(const String& str, int& p, bool& err);
  static String		NextToken(const String& str, int& p, bool& err);
    // skip ws, get the next token; removes quotes and processes quoted/escaped strings

  bool CalcRowParams(String operation, DataTable* table, int& row_from, int& rows, int row_to);
#ifndef __MAKETA__
  class TableParams {
  public:
    enum Cmd {
      Get,
      Append,
      Remove,
      Set,
      Cell // Get and Set, set mat param
    };
    
    TemtClient* tc;
    DataTable* tab;
    int rows; // Append/Set only
    union {
    int row_from; 
    int row;
    };
    int row_to;
    union {
    int col_from; 
    int col;
    };
    int col_to;
    union {
    bool header;
    bool mat;
    };
    union {
    int lines;
    int cell;
    };
    bool markers;
    
    bool	ValidateParams(Cmd cmd = Get, bool mat = false);
    TableParams(TemtClient* tc_, DataTable* tab_)
      {tc = tc_; tab = tab_;}
  };

  virtual void 		cmdGetDataCell_impl(TableParams& p);
  virtual void 		cmdSetDataCell_impl(TableParams& p);
  QPointer<QTcpSocket>	sock; // #IGNORE the socket for the connected client
#endif
  String_PArray		lines; // have to buffer between raw in and processing them -- this is a queue
  
// every command line is parsed into the following pieces before dispatching the command:
  String		      cmd_line; // the last cmd line
  String		      cmd; // this is the first item, the command
  String_PArray		pos_params; // positional (no "=") parameters, if any; str quoting/escaping already done (used by ascii parser)
  NameVar_PArray	name_params; // name params; str quoting/escaping already done
  taProjectRef		cur_proj; // set by OpenProject cmd, or to proj0
#if (QT_VERSION >= 0x050000)
  QJsonObject     tableData;  // #IGNORE this is the json data table data
#endif
  taProject*		GetCurrentProject(); // gets, and maybe asserts
  DataTable* 		GetAssertTable(const String& nm); // gets, and sends errs if not found; supports <GlobalTableName> or <ProgName>.<LocalTableName> formats
  Program* 		  GetAssertProgram(const String& pnm); // gets, and sends errs if not found
  
  void			    setState(ClientState cs);
  
  void			    HandleLines(); // line handling loop
  void          ParseCommand(const String& cl);
  void          ParseCommandNATIVE(const String& cl);
#if (QT_VERSION >= 0x050000)
  void          ParseCommandJSON(const String& cl);
  
  bool          ValidateJSON_HasMember(const QJsonObject& n, const String& member_name);
  // #IGNORE check for member name in json string
  bool          ValidateJSON_ColumnName(DataTable* dt, const QJsonObject& n);
  // #IGNORE validate name of a particular column
  bool          ValidateJSON_ColumnNames(DataTable* dt, const QJsonObject& n);
  // #IGNORE validate all column names
#endif
  
private:
  void	Copy_(const TemtClient& cp);
  void	Initialize();
  void 	Destroy();
};

#endif // TemtClient_h
