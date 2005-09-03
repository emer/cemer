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

/* Written by Per Bothner (bothner@cygnus.com). */

// strstream is not supported in g++ 3.x

// #ifndef __STRSTREAM_H
// #define __STRSTREAM_H

// #include <iostream.h>

// extern "C++" {

// class istrstream : public istream {
// public:
//   istrstream();
// };

// class ostrstream : public ostream {
//   char *str();
//   void freeze(int n = 1);
//   int frozen();
// };

// class strstream : public iostream {
//   public:
//   char *str();
//   void freeze(int n = 1);
// };
// } // extern "C++"

// #endif /*!__STRSTREAM_H*/
