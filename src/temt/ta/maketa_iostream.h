/*  This is part of libio/iostream, providing -*- C++ -*- input/output.
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

#ifndef _IOSTREAM_H
#define _IOSTREAM_H

#include "maketa_streambuf.h"

extern "C++" {
class istream; class ostream;

class ostream : virtual public ios {
public:
  ostream& flush();
  ostream& put(char c);
  ostream& write(const char *s, streamsize n);
  ostream& seekp(streamoff off, seekdir dir);
  streampos tellp();
};

class istream : virtual public ios {
public:
  istream& getline(char* ptr, int len, char delim = '\n');
  istream& read(char *ptr, streamsize n);

  int get();
  int peek();
  istream& ignore(int n=1, int delim = EOF);
  int sync ();
  istream& seekg(streamoff off, seekdir dir);
  streampos tellg();
  istream& putback(char ch);
  istream& unget();
};

class iostream : public istream, public ostream {
public:
  iostream() { };
};
} // extern "C++"
#endif /*!_IOSTREAM_H*/
