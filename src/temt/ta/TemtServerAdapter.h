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

#ifndef TemtServerAdapter_h
#define TemtServerAdapter_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:
class TemtServer; // 


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

#endif // TemtServerAdapter_h
