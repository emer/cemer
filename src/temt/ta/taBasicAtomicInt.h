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

#ifndef taBasicAtomicInt_h
#define taBasicAtomicInt_h 1

// parent includes:
#include <QBasicAtomicInt>

// member includes:

// declare all other types mentioned but not required to include:


class taBasicAtomicInt : public QBasicAtomicInt {
public:
# if (QT_VERSION >= 0x050000)
  // Non-atomic API
  inline bool operator==(int value) const
    {return _q_value == value;}

  inline bool operator!=(int value) const
    {return _q_value != value;}

  inline bool operator!() const
    {return _q_value == 0;}

  inline operator int() const
    {return _q_value;}

  inline int operator=(int value)
  { _q_value = value; return value; }
#else
  void store(int newValue) { QBasicAtomicInt::operator=(newValue); }
#endif

};

#endif // taBasicAtomicInt_h
