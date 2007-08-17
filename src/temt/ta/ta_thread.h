// Copyright, 1995-2005, Regents of the University of Colorado,
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


// ta_thread.h -- multi-threading interface

#ifndef TA_THREAD_H
#define TA_THREAD_H

#include "ta_def.h"

class TA_API taAtomic {
  // ##NO_INSTANCE static class for doing atomic (threadsafe) operations
public:
  static int 		Increment(volatile int& i);
    // indivisibly increment i; returns 'true' if result is != 0
  static int 		Decrement(volatile int& i);
    // indivisibly decrement i; returns 'true' if result is != 0
  static int 		TestAndSet(volatile int& i, int expected, int newval);
    // if i==exp then return 'true' and set i=newval; else return 'false' (i unchanged)
  static int		GetNextValue(volatile int& i, int&result, int maxval, int inc_by=1);
    // get the next available value of i <= maxval, by adding inc_by -- useful for "greedy" multi-thread algorithms; 'true' if the result is valid
  static int		GetPrevValue(volatile int& i, int&result, int minval, int dec_by=1);
    // get the next available value of i >= minval, by subtracting dec_by -- useful for "greedy" multi-thread algorithms; 'true' if the result is valid
    
#ifndef __MAKETA__
private:
  taAtomic(); // no instances allowed
  taAtomic(const taAtomic&); // no instances allowed
#endif
};


#ifdef TA_USE_QT
#ifndef __MAKETA__
# include <QAtomic>
#endif

inline int taAtomic::Increment(volatile int& i)
  {return q_atomic_increment(&i);}
  
inline int taAtomic::Decrement(volatile int& i)
  {return q_atomic_decrement(&i);}

inline int taAtomic::TestAndSet(volatile int& i, int expected, int newval)
  {return q_atomic_test_and_set_int(&i, expected, newval);}

#else
//NOTE: shouldn't need these...
#endif

#endif

