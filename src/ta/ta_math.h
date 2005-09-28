// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


#ifndef TA_MATH_H
#define TA_MATH_H

#include <math.h>

extern "C" {
#ifdef LINUX
#define sqrtf(x) sqrt(x)
#define expf(x)  exp(x)
#define logf(x)  log(x)
#define fabsf(x) fabs(x)
#define powf(x,y) pow(x,y)
#endif // def LINUX

#ifdef AIXV4
#define sqrtf(x) sqrt(x)
#define expf(x)  exp(x)
#define logf(x)  log(x)
#define fabsf(x) fabs(x)
#define powf(x,y) pow(x,y)
#endif  // def AIXV4

#if defined (SUN4) || defined(SUN4solaris)
  extern unsigned sleep(unsigned);
#define sqrtf(x) sqrt(x)
#define expf(x)  exp(x)
#define logf(x)  log(x)
#define fabsf(x) fabs(x)
#define powf(x,y) pow(x,y)
#endif //  defined (SUN4) || defined(SUN4solaris)

  extern double acosh(double);
  extern double asinh(double);
  extern double atanh(double);
  extern double erf(double);
}

#endif // TA_MATH_H
