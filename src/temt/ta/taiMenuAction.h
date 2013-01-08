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

#ifndef taiMenuAction_h
#define taiMenuAction_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:
class QObject; // 


class TA_API taiMenuAction { // object that holds specs for receiver for Action(taiAction*) callback
public:
  QObject*      receiver;
  String        member;

  taiMenuAction() {receiver = NULL;}
  taiMenuAction(QObject* receiver_, const char* member_) {receiver = receiver_; member = member_;}
  taiMenuAction(const taiMenuAction& src) {receiver = src.receiver; member = src.member;} //

//  void                connect(QObject* sender, const char* signal) const; // #IGNORE
  taiMenuAction&        operator=(const taiMenuAction& rhs);
};

#endif // taiMenuAction_h
