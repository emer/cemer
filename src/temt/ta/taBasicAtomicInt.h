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

#include "ta_def.h"

#if !defined(__MAKETA__) && defined(TA_USE_QT)
# if (QT_VERSION >= 0x040400)
#   include <QAtomicInt>
# else
#   include <qatomic.h>
# endif
#endif

#if defined(__MAKETA__)
class QBasicAtomicInt;
#else

# if (QT_VERSION < 0x040400) || !defined(TA_USE_QT)
class TA_API QBasicAtomicInt {
  // ##SMART_INT this copies the barest API of QBasicAtomicInt, which is a Plain Old Data type
public:
  volatile int _q_value;
  
  bool ref();
  bool deref();
  bool testAndSetOrdered(int expectedValue, int newValue);
  int fetchAndStoreOrdered(int newValue);
  int fetchAndAddOrdered(int valueToAdd);

  // Non-atomic API
  inline bool operator==(int value) const
    {return _q_value == value;}

  inline bool operator!=(int value) const
    {return _q_value != value;}

  inline bool operator!() const
    {return _q_value == 0;}

  inline operator int() const
    {return _q_value;}
    
  QBasicAtomicInt& operator=(int value)
    {_q_value = value; return *this;}
};
#endif

#endif

TypeDef_Of(taBasicAtomicInt);

class TA_API taBasicAtomicInt : public QBasicAtomicInt {
  // ##SMART_INT a basic atomic integer for threadsafe processing
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
