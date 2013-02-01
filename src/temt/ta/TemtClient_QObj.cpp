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

#include "TemtClient_QObj.h"
#include <TemtClient>


void TemtClient_QObj::init() {
  prog_rval = Program::RV_OK;
  pds = PDS_NONE;
}

void TemtClient_QObj::prog_Run() {
  if (!prog) {
    prog_rval = Program::RV_NO_PROGRAM;
    return;
  }
  pds = PDS_RUNNING;
  prog->Run();
  prog_rval = (Program::ReturnVal)prog->ret_val;
  pds = PDS_DONE;
}

void TemtClient_QObj::SetProg(Program* prog_) {
  prog = prog_;
  pds = PDS_SET;
  prog_rval = Program::RV_OK;
}

void TemtClient_QObj::sock_disconnected() {
  owner()->sock_disconnected();
}

void TemtClient_QObj::sock_readyRead() {
  owner()->sock_readyRead();
}

void TemtClient_QObj::sock_stateChanged(QAbstractSocket::SocketState socketState) {
  owner()->sock_stateChanged(socketState);
}
