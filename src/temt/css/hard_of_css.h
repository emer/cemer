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


#ifndef hard_of_css_h
#define hard_of_css_h

// glue that makes it easy to do a hardcoded vesion of a css file
// basically establishes the same level of includes, libraries, etc.

#include <ta/ta_stdef.h>
#include "css_def.h"

#include <css/special_math.h>
#include <css/css_misc_funs.h>

#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

// these are for vt instantiation
#include <ta/ta_dump.h>
#include <ta/ta_group.h>
#include <ta/tdefaults.h>

// css file needs to have an s_main of this description..
void s_main(int ac, String* av);

// which gets called by this main, which is the real one.
int main(int argc, char** argv) {
  String* s_argv = new String[argc+4];
  
  int i;
  for(i=0; i<=argc; i++) {
    s_argv[i] = argv[i];
  }

  s_main(argc, s_argv);
  delete[] s_argv;
}

#endif // hard_of_css_h
