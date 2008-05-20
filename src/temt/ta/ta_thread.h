// Copyright, 1995-2007, Regents of the University of Colorado,
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

//#ifdef TA_USE_THREADS

class TA_API taAtomic {
  // ##NO_INSTANCE static class for doing atomic (threadsafe) operations -- only exists on supported multi-threaded architectures (NOT PPC)
public:
  static inline int 	FetchAdd(volatile int *ptr, int value);
    // returns current value, then indivisibly adds value
    
#ifndef __MAKETA__
private:
  taAtomic(); // no instances allowed
  taAtomic(const taAtomic&); // no instances allowed
#endif
};

#ifndef __MAKETA__


#if defined(_MSC_VER) //note: assumes i386 arch, 32/64 doesn't matter (??)
int taAtomic::FetchAdd(volatile int *pointer, int value)
{
    __asm {
        mov EDX,pointer
        mov ECX,value
        lock xadd dword ptr[EDX],ECX
        mov value,ECX
    }
    return value;
}
#elif ((defined(__i386__) || defined(__x86_64__)))
#  if defined(__GNUC__)
int taAtomic::FetchAdd(volatile int *ptr, int value)
{
    asm volatile("lock\n"
                "xaddl %0,%1"
                : "=r" (value), "+m" (*ptr)
                : "0" (value)
                : "memory");
    return value;
}
#   else
#     error "Undefined compiler on i386 -- need to define q_atomic_fetch_and_add_int"
#   endif // compiler on Intel
#elif defined(_ARCH_PPC) && defined(Q_CC_GNU)
int taAtomic::FetchAdd(volatile int *ptr, int value)
{
    register int tmp;
    register int ret;
    asm volatile("lwarx  %0, 0, %3\n"
                 "add    %1, %4, %0\n"
                 "stwcx. %1, 0, %3\n"
                 "bne-   $-12\n"
                 : "=&r" (ret), "=&r" (tmp), "=m" (*ptr)
                 : "r" (ptr), "r" (value)
                 : "cc", "memory");
    return ret;
} 
#else // not MSVC or GNU so...
#   error "Undefined arch or compiler -- need to define taAtomic::FetchAdd"
#endif

#endif // maketa exclusion

#endif

