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

#ifndef TemtServer_QObj_h
#define TemtServer_QObj_h 1

// parent includes:
#include <taBase_QObj>

// member includes:

// declare all other types mentioned but not required to include:
class TemtServer; // 


class TA_API TemtServer_QObj: public taBase_QObj {
  // ##IGNORE QObject for attaching events/signals for its taBase owner
friend class TemtServer;
  Q_OBJECT
public:
  inline TemtServer*	owner() {return (TemtServer*)taBase_QObj::owner;}
  TemtServer_QObj(TemtServer* owner_): taBase_QObj((taOABase*)owner_) {}
  
#ifndef __MAKETA__ // maketa chokes on the net class types etc.
public slots:
  void 			server_newConnection();
#endif
};

#endif // TemtServer_QObj_h
