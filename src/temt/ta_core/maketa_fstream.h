// Co2018ght 2006-2017, Regents of the University of Colorado,
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

/* This is part of libio/iostream, providing -*- C++ -*- input/output.
Copyright (C) 1993 Free Software Foundation

This file is part of the GNU IO Library.  This library is free
software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option)
any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

As a special exception, if you link this library with files
compiled with a GNU compiler to produce an executable, this does not cause
the resulting executable to be covered by the GNU General Public License.
This exception does not however invalidate any other reasons why
the executable file might be covered by the GNU General Public License. */

/* IMPORTANT NOTE: this is a "dummy" version of the standard libarary stream
   interface that exposes only the functions that maketa will scan, generating
   the exposed functionality that can be used in css etc */

#ifndef _FSTREAM_H
#define _FSTREAM_H
#include "maketa_iostream.h"

extern "C++" {

class ifstream : public istream {
public:
  void close();
  int is_open() const;
  void open(const char *name, openmode mode= _S_in);
};

class ofstream : public ostream {
public:
  void close();
  int is_open() const;
  void open(const char *name, openmode mode= _S_out);
};

class fstream : public iostream {
  public:
  void close();
  int is_open() const;
  void open(const char *name, openmode mode);
};
} // extern "C++"
#endif /*!_FSTREAM_H*/
