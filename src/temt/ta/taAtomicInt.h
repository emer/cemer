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

#ifndef taAtomicInt_h
#define taAtomicInt_h 1

#include "ta_def.h"

#if !defined(__MAKETA__) && defined(TA_USE_QT)
# if (QT_VERSION >= 0x040400)
#   include <QAtomicInt>
# else
#   include <qatomic.h>
# endif
#endif

#if defined(__MAKETA__)
class QAtomicInt;
#else

class TA_API QAtomicInt: public QBasicAtomicInt { // this copies the barest API of QAtomicInt
public:
  QAtomicInt& operator=(int value)
    {_q_value = value; return *this;}

  QAtomicInt& operator=(const QAtomicInt &other)
    {_q_value = other._q_value; return *this;}
  
  QAtomicInt(int value = 0) {_q_value = value;}
  QAtomicInt(const QAtomicInt & other) {_q_value = other._q_value;}
};

# ifdef TA_USE_QT
  inline bool QBasicAtomicInt::ref();
    {return q_atomic_increment(&_q_value) != 0;}
  
  inline bool QBasicAtomicInt::deref()
    {return q_atomic_decrement(&_q_value) != 0;}
  
  inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
    {return q_atomic_test_and_set_int(&_q_value, expectedValue, newValue) != 0;}
  
  inline int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
    {return q_atomic_set_int(&_q_value, newValue);}
  
  inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
    {return q_atomic_fetch_and_add_int(&_q_value, valueToAdd);}
# else // dummies for maketa
  inline bool QBasicAtomicInt::ref()
    {return ++_q_value != 0;}
  
  inline bool QBasicAtomicInt::deref()
    {return --_q_value != 0;}
  
  inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
    {if (_q_value == expectedValue) {
     _q_value = newValue;
     return true;
     }
     return false;
    }
  
  inline int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
    {int originalValue = _q_value;
     _q_value = newValue;
     return originalValue;
    }
  
  inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
    {int originalValue = _q_value;
     _q_value += valueToAdd;
     return originalValue;
    }
# endif

# endif

class taAtomicInt : public QAtomicInt {
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
#endif

#ifndef __MAKETA__
  inline taAtomicInt(int value = 0) : QAtomicInt(value) {};
  inline taAtomicInt(const taAtomicInt &other) : QAtomicInt(other) {};
  inline taAtomicInt& operator=(const taAtomicInt &other) {
    QAtomicInt::operator=(other);
    return *this;
  }
#endif

};

#endif // taAtomicInt_h
