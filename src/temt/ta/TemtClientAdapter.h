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

#ifndef TemtClientAdapter_h
#define TemtClientAdapter_h 1

// parent includes:

// member includes:
#include <Program>

// declare all other types mentioned but not required to include:
class TemtClient; // 
class Program; // 


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

#endif // TemtClientAdapter_h
