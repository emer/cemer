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

#ifndef iMenuAction_h
#define iMenuAction_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class QObject; // 


class TA_API iMenuAction { // object that holds specs for receiver for Action(iAction*) callback
public:
  QObject*      receiver;
  String        member;

  iMenuAction() {receiver = NULL;}
  iMenuAction(QObject* receiver_, const char* member_) {receiver = receiver_; member = member_;}
  iMenuAction(const iMenuAction& src) {receiver = src.receiver; member = src.member;} //

//  void                connect(QObject* sender, const char* signal) const; // #IGNORE
  iMenuAction&        operator=(const iMenuAction& rhs);
};

#endif // iMenuAction_h
