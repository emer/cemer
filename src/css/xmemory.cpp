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


#include <stdlib.h>

/* because readline needs these, and they must be compiled with C linkage */

extern "C" {
  void execerror(char *s, char *t);
  char* readline(char *);
#ifndef DARWIN
  char* xmalloc(int);
  char* xrealloc(char*, int);
  void memory_error_and_abort ();
#endif
  char* rl_readline(char*);
}

#ifndef DARWIN
char* xmalloc (int bytes) {
  char *temp = (char *)malloc (bytes);

  if (!temp)
    memory_error_and_abort ();
  return (temp);
}

char* xrealloc (char* pointer, int bytes) {
  char *temp;

  if (!pointer)
    temp = (char *)malloc (bytes);
  else
    temp = (char *)realloc (pointer, bytes);

  if (!temp)
    memory_error_and_abort ();

  return (temp);
}

void memory_error_and_abort () {
/*  fprintf(stderr, "Out of virtual memory, aborting\n"); */
  abort ();
}
#endif
char* rl_readline(char* prmpt) {
  return readline(prmpt);
}

